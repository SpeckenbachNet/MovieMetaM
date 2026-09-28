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
#pragma once

#include <QString>
#include <QPixmap>
#include <QStringList>
#include <QDate>

class MovieData {
public:
    MovieData();

    // Grunddaten
    QString filePath;
    QString title;
    QString cast;
    QString directors;
    QString screenwriters;
    QString producers;
    QString studio;
    QString description;
    QDate releaseDate;
    QString genre;
    QPixmap cover;

    // Age rating specific
    QString ratingText;   // e.g. "ab 12 Jahren"
    QString ratingSystem; // e.g. "de-movie"
    int ratingScore;      // e.g. 300

    // Validating and help functions
    QString status = "unchanged";
    QString getItunExtc() const; // Baut den "de-movie|ab 12|200|" String

    // Help method for sanitizing ???
    static QString sanitizeInput(QString input);
    static QString normalizeTagString(const QString &input);

    static MovieData fromFile(const QString &filePath);
};
