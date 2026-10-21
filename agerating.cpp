/*
 * Copyright (C) 2026 Michael Speckenbach
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#include "agerating.h"
#include "languages.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLocale>
#include <QSettings>

namespace {

QMap<QString, CountryRatingSystem> load()
{
    QMap<QString, CountryRatingSystem> systems;
    const QString lang = Languages::appLanguage();

    const QDir dir(QStringLiteral(":/ratings"));
    const QStringList files = dir.entryList({QStringLiteral("*.json")}, QDir::Files);
    for (const QString &fileName : files) {
        QFile file(dir.filePath(fileName));
        if (!file.open(QIODevice::ReadOnly)) {
            continue;
        }
        QJsonParseError error;
        const QJsonObject root = QJsonDocument::fromJson(file.readAll(), &error).object();
        if (error.error != QJsonParseError::NoError) {
            qWarning() << "Age rating file" << fileName << ":" << error.errorString();
            continue;
        }

        CountryRatingSystem system;
        const QJsonObject names = root.value("country").toObject();
        // The name in the language of the app: the English name is translated (context "Country",
        // texts in countrynames.cpp); without a translation the German name of the file, else English
        const QString english = names.value("en").toString();
        system.countryName = QCoreApplication::translate("Country", english.toUtf8().constData());
        if (system.countryName == english && names.contains(lang)) {
            system.countryName = names.value(lang).toString(english);
        }
        system.technicalID = root.value("id").toString();
        system.systemName  = root.value("system").toString();
        const QJsonArray ratings = root.value("ratings").toArray();
        for (const QJsonValue &value : ratings) {
            const QJsonObject r = value.toObject();
            RatingEntry entry;
            entry.key         = r.value("key").toString();
            entry.code        = r.value("code").toString(entry.key);
            entry.score       = r.value("score").toInt();
            entry.label       = r.value("label").toString();
            entry.description = r.value("description").toString();
            entry.color       = QColor(r.value("color").toString());
            entry.shape       = r.value("shape").toString();
            entry.style       = r.value("style").toString(QStringLiteral("filled"));
            system.ratings.append(entry);
        }
        systems.insert(QFileInfo(fileName).baseName(), system);   // country code = file name
    }
    return systems;
}

} // namespace

const QMap<QString, CountryRatingSystem> &AgeRatings::systems()
{
    static const QMap<QString, CountryRatingSystem> systems = load();
    return systems;
}

QString AgeRatings::countryOfSystem(const QString &technicalID)
{
    if (technicalID.isEmpty()) {
        return QString();
    }
    const auto &all = systems();
    for (auto it = all.cbegin(); it != all.cend(); ++it) {
        if (it.value().technicalID == technicalID) {
            return it.key();
        }
    }
    return QString();
}

const RatingEntry *AgeRatings::findEntry(const QString &countryCode, const QString &certification)
{
    const auto normalized = [](QString text) { return text.remove(' ').toLower(); };
    const QString wanted = normalized(certification);
    const auto &all = systems();
    const auto system = all.constFind(countryCode);
    if (wanted.isEmpty() || system == all.constEnd()) {
        return nullptr;
    }
    for (const RatingEntry &entry : system.value().ratings) {
        if (normalized(entry.key) == wanted) return &entry;
    }
    for (const RatingEntry &entry : system.value().ratings) {
        if (normalized(entry.code) == wanted) return &entry;
    }
    return nullptr;
}
