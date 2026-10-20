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

// Two languages (see CLAUDE.md): the language of the app (menus, help: German or English) and
// the language of the movie data from TMDB (genres, search: many languages). Both follow the
// system until the user chooses another one (setup dialog, settings).
namespace Languages {

struct Language {
    QString code;   // "de" (app) or "de-DE" (movie data, as TMDB expects it)
    QString name;   // in the language itself, e.g. "Deutsch (Schweiz)"
};

QList<Language> appLanguages();    // German and English
QList<Language> dataLanguages();   // TMDB languages sorted by name, the system language included

QString appLanguage();    // from the settings ("language"), else the one of the system
QString dataLanguage();   // from the settings ("General/DataLanguage"), else the one of the system

} // namespace Languages
