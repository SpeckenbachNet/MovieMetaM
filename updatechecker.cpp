/*
 * Copyright (C) 2026 Michael Speckenbach
 * This file is part of MovieMetaM.
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
#include "updatechecker.h"

#include <QDate>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSettings>
#include <QVersionNumber>

namespace {

const char *kLatestReleaseApi = "https://api.github.com/repos/SpeckenbachNet/MovieMetaM/releases/latest";
const char *kDownloadBase = "https://github.com/SpeckenbachNet/MovieMetaM/releases/latest/download/";

} // namespace

UpdateChecker::UpdateChecker(QObject *parent)
    : QObject(parent)
{
}

bool UpdateChecker::isEnabled()
{
    return QSettings().value("General/CheckUpdates", true).toBool();
}

QUrl UpdateChecker::downloadUrl()
{
    // The fixed names of the packages (tools/build-*.sh, MovieMetaM.iss)
#if defined(Q_OS_MACOS)
    const QString file = QStringLiteral("MovieMetaM-mac.dmg");
#elif defined(Q_OS_WIN)
    const QString file = QStringLiteral("MovieMetaM-windows-Setup.exe");
#else
    const QString file = QStringLiteral("MovieMetaM-linux.AppImage");
#endif
    return QUrl(QString::fromLatin1(kDownloadBase) + file);
}

void UpdateChecker::check()
{
    if (!isEnabled()) {
        return;
    }
    QSettings settings;
    announceIfNewer(settings.value("General/LatestVersion").toString());
    if (settings.value("General/LastUpdateCheck").toDate() != QDate::currentDate()) {
        request();
    }
}

void UpdateChecker::request()
{
    if (!m_network) {
        m_network = new QNetworkAccessManager(this);
    }
    QNetworkRequest request{QUrl(QString::fromLatin1(kLatestReleaseApi))};
    // GitHub wants a User-Agent; JSON of the REST API
    request.setHeader(QNetworkRequest::UserAgentHeader, QStringLiteral(APP_NAME "/" APP_VERSION));
    request.setRawHeader("Accept", "application/vnd.github+json");
    QNetworkReply *reply = m_network->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply] {
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            return;   // no network, no release yet (404), rate limit: try again tomorrow or next start
        }
        const QString tag = QJsonDocument::fromJson(reply->readAll()).object().value("tag_name").toString();
        const QString version = tag.startsWith(QLatin1Char('v')) ? tag.mid(1) : tag;
        if (QVersionNumber::fromString(version).isNull()) {
            return;
        }
        QSettings settings;
        settings.setValue("General/LastUpdateCheck", QDate::currentDate());
        settings.setValue("General/LatestVersion", version);
        announceIfNewer(version);
    });
}

void UpdateChecker::announceIfNewer(const QString &version)
{
    const QVersionNumber latest = QVersionNumber::fromString(version);
    if (!latest.isNull() && latest > QVersionNumber::fromString(QStringLiteral(APP_VERSION))) {
        emit updateAvailable(version);
    }
}
