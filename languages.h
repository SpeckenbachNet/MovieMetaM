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
#pragma once

#include <QList>
#include <QString>
#include <QStringList>

// Two languages (see CLAUDE.md): the language of the app (texts, help, names of the countries) and
// the language of the movie data from TMDB (genres, search: many languages). Both follow the
// system until the user chooses another one (setup dialog, settings).
//
// The languages of the app are those with a compiled translation (":/i18n/<app>_<code>.qm", the
// code like "de", "pt_BR", "zh_CN"); English is always there (the language of the source texts).
// A new language: its .ts file in CMakeLists.txt (APP_LANGUAGES), the help texts in
// help/<code>/ and the code in the table kAppLanguages of languages.cpp.
namespace Languages {

struct Language {
    QString code;   // "de" (app) or "de-DE" (movie data, as TMDB expects it)
    QString name;   // in the language itself, e.g. "Deutsch (Schweiz)"
};

QList<Language> appLanguages();    // the languages of the app that are there, English included;
                                   // a draft has the word for it behind its name
bool hasDraftLanguages();          // is one of the languages of the app a draft (then a note is shown)

QList<Language> dataLanguages();   // TMDB languages sorted by name, the system language included

// The language to use for a wanted code ("de", "pt_PT", "zh_TW", empty = system): the code itself,
// else a language of the same family ("pt" -> "pt_BR"), else "en". Without a list of
// available codes the languages of the app are used.
QString resolveAppLanguage(const QString &wanted, const QStringList &available);

QString appLanguage();    // from the settings ("language"), else the one of the system, resolved
QString dataLanguage();   // from the settings ("General/DataLanguage"), else the one of the system

} // namespace Languages
