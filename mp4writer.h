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

#include "moviedata.h"

#include <QCoreApplication>
#include <QImage>
#include <QString>

// Writes the data of a movie into its MP4 file with TagLib (the reading is MediaInfoM).
// Two ways (the setting "Safe saving"):
//  - direct (default): TagLib writes into the file itself; fast, also for big files and on
//    network drives, but a write that is interrupted can damage the file.
//  - safe: the tags are written into a copy, the copy is read back and checked, and only then
//    it replaces the original. Needs the room of a second file and time to copy.
// Both ways read the file back and check it. The class works in any thread (no QPixmap).
class Mp4Writer
{
    Q_DECLARE_TR_FUNCTIONS(Mp4Writer)
public:
    // movie: what is to be in the file (its cover pixmap is not used: no QPixmap outside the GUI
    // thread), cover / originalCover: the cover now and as it was read (an unchanged cover is not
    // written again). false: error holds the reason; with safe == true the file is as it was.
    static bool save(const MovieData &movie, const QImage &cover, const QImage &originalCover,
                     bool safe, QString *error);

    // The iTunMOVI entry (cast, directors, ...) as Apple writes it; existing: the entry so far
    // (parts the app does not edit, e.g. codirectors, are kept)
    static QString buildItunMovi(const MovieData &movie, const QString &existing = QString());

private:
    static bool writeTags(const QString &path, const MovieData &movie, const QImage &cover,
                          const QImage &originalCover, QString *error);
    static bool verify(const QString &path, const MovieData &movie, const QImage &cover);
};
