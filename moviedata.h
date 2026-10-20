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
#include <QSize>

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

    // Content advisory ("rtng"): 0 none, 2 clean, 1 and 4 explicit; other values stay as they are
    int advisory = 0;
    QSize videoSize;      // as the file shows it (read only); the HD flag follows it when saving

    // The HD flag of Apple ("hdvd") for a size: 0 SD, 1 720p, 2 1080p, 3 4K
    static int hdFlag(const QSize &size);
    static QString hdName(int flag);

    // Validating and help functions
    QString status = "unchanged";
    QString getItunExtc() const; // Baut den "de-movie|ab 12|200|" String

    // Help method for sanitizing ???
    static QString sanitizeInput(QString input);
    static QString normalizeTagString(const QString &input);

    static MovieData fromFile(const QString &filePath);
};
