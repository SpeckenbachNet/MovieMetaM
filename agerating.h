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

#include <QColor>
#include <QList>
#include <QMap>
#include <QString>

// One step of an age rating system, e.g. FSK 12
struct RatingEntry {
    QString key;     // shown in the badge, e.g. "12"
    QString code;    // written into the file (iTunEXTC prefix|code|score|), e.g. "FSK 12" – the form
                     // media servers know (see CLAUDE.md, age ratings)
    int     score;   // order and value in the file, e.g. 200
    QString label;   // text in the combo box, e.g. "ab 12 Jahren" (official text, not translated)
    QString description;   // tooltip (may be empty)
    QColor  color;   // badge color
    QString shape;   // badge shape: "square", "circle", "diamond", "triangle", "octagon", "hexagon", "star"
    QString style;   // "filled" (colored area, default), "outline" (white area, border and code in the
                     // color), "light" (white area, black border, code in the color),
                     // "dark" (dark area, code in the color)
};

// Age rating system of a country
struct CountryRatingSystem {
    QString countryName;   // in the language of the app (German or English)
    QString technicalID;   // e.g. "de-movie"
    QString systemName;    // e.g. "FSK"
    QList<RatingEntry> ratings;

    QString displayName() const { return QStringLiteral("%1 (%2)").arg(countryName, systemName); }
};

// The age rating systems are data, not code: one JSON file per country in the resources
// (:/ratings/<country code>.json, see Ratings.qrc). A new country is a new file.
namespace AgeRatings {

// All systems by country code ("DE", "UK", ...); read once on the first call
const QMap<QString, CountryRatingSystem> &systems();

} // namespace AgeRatings
