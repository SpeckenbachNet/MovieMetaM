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
#include "languages.h"

#include <QFile>
#include <QLocale>
#include <QSettings>

#include <algorithm>

namespace {

// The translations TMDB has for most movies (its "primary translations"), language-COUNTRY
const char *const kDataLanguageCodes[] = {
    "ar-AE", "ar-SA", "bg-BG", "bn-BD", "ca-ES", "cs-CZ", "da-DK", "de-AT", "de-CH", "de-DE",
    "el-GR", "en-AU", "en-CA", "en-GB", "en-IE", "en-NZ", "en-US", "es-ES", "es-MX", "et-EE",
    "eu-ES", "fa-IR", "fi-FI", "fr-CA", "fr-FR", "gl-ES", "he-IL", "hi-IN", "hr-HR", "hu-HU",
    "id-ID", "it-IT", "ja-JP", "ka-GE", "kk-KZ", "ko-KR", "lt-LT", "lv-LV", "ms-MY", "ms-SG",
    "nl-BE", "nl-NL", "no-NO", "pl-PL", "pt-BR", "pt-PT", "ro-RO", "ru-RU", "sk-SK", "sl-SI",
    "sq-AL", "sr-RS", "sv-SE", "ta-IN", "te-IN", "th-TH", "tl-PH", "tr-TR", "uk-UA", "vi-VN",
    "zh-CN", "zh-HK", "zh-SG", "zh-TW",
};

// The languages the app can have (a translation is only used if its .qm is compiled in), with
// their names in the language itself (Qt would say "American English" or "Español de España")
struct AppLanguage {
    const char *code;
    const char *name;
};
const AppLanguage kAppLanguages[] = {
    {"de", "Deutsch"},
    {"en", "English"},
    {"fr", "Français"},
    {"es", "Español"},
    {"it", "Italiano"},
    {"pt_BR", "Português (Brasil)"},
    {"zh_CN", "简体中文"},
    {"ja", "日本語"},
    {"ko", "한국어"},
};

bool hasTranslation(const QString &code)
{
    return code == QLatin1String("en")
           || QFile::exists(QStringLiteral(":/i18n/%1_%2.qm").arg(QStringLiteral(APP_NAME), code));
}

QStringList availableAppLanguageCodes()
{
    QStringList codes;
    for (const AppLanguage &language : kAppLanguages) {
        const QString code = QString::fromLatin1(language.code);
        if (hasTranslation(code)) {
            codes.append(code);
        }
    }
    return codes;
}

// "Deutsch (Schweiz)": names in the language itself, so nothing to translate
QString nativeName(const QString &code)
{
    const QLocale locale(code);
    QString name = locale.nativeLanguageName();
    if (!name.isEmpty()) {
        name[0] = name.at(0).toUpper();   // e.g. "français", "español"
    }
    return QStringLiteral("%1 (%2)").arg(name, locale.nativeTerritoryName());
}

// "de-DE" from the system (QLocale uses "de_DE")
QString systemDataLanguage()
{
    return QLocale::system().name().replace('_', '-');
}

} // namespace

QList<Languages::Language> Languages::appLanguages()
{
    QList<Language> languages;
    for (const AppLanguage &language : kAppLanguages) {
        const QString code = QString::fromLatin1(language.code);
        if (hasTranslation(code)) {
            languages.append({code, QString::fromUtf8(language.name)});
        }
    }
    return languages;
}

QString Languages::resolveAppLanguage(const QString &wanted, const QStringList &available)
{
    const QString code = QString(wanted).replace(QLatin1Char('-'), QLatin1Char('_'));
    if (code.isEmpty()) {
        return QStringLiteral("en");
    }
    if (available.contains(code)) {
        return code;
    }
    // The same language in another country or script: "pt_PT" -> "pt_BR"
    const QString family = code.section(QLatin1Char('_'), 0, 0);
    for (const QString &candidate : available) {
        if (candidate.section(QLatin1Char('_'), 0, 0) == family) {
            return candidate;
        }
    }
    return QStringLiteral("en");
}

QList<Languages::Language> Languages::dataLanguages()
{
    QStringList codes;
    for (const char *code : kDataLanguageCodes) {
        codes.append(QString::fromLatin1(code));
    }
    // The system language is always there, even if TMDB doesn't list it (e.g. "fr-CH")
    const QString system = systemDataLanguage();
    if (!codes.contains(system)) {
        codes.append(system);
    }

    QList<Language> languages;
    for (const QString &code : std::as_const(codes)) {
        languages.append({code, nativeName(code)});
    }
    std::sort(languages.begin(), languages.end(), [](const Language &a, const Language &b) {
        return a.name.localeAwareCompare(b.name) < 0;
    });
    return languages;
}

QString Languages::appLanguage()
{
    QString code = QSettings().value("language").toString();
    if (code.isEmpty()) {
        code = QLocale::system().name();   // "de_DE"
    }
    return resolveAppLanguage(code, availableAppLanguageCodes());
}

QString Languages::dataLanguage()
{
    const QString code = QSettings().value("General/DataLanguage").toString();
    return code.isEmpty() ? systemDataLanguage() : code;
}
