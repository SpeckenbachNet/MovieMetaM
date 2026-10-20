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
#include "moviedata.h"
#include <QRegularExpression>
#include "mediainfom.h"

MovieData::MovieData()
    : ratingScore(0)
{
}

/**
 * Builds the normalized Apple rating string.
 * Makes sure no pipes in the text break the format.
 */
QString MovieData::getItunExtc() const {
    if (ratingSystem.isEmpty() || ratingText.isEmpty()) {
        return "";
    }

    // Make sure there are no pipes in the text (sanitizing)
    QString cleanText = sanitizeInput(ratingText);

    return QString("%1|%2|%3|").arg(
        ratingSystem,
        cleanText,
        QString::number(ratingScore)
    );
}

/**
 * Checks whether the data is consistent.
 */
// bool MovieData::isValid() const {
//     if (title.trimmed().isEmpty()) return false;
//     if (year < 1895 || year > 2100) return false;
//     if (ratingScore < 0) return false;

//     return true;
// }

/**
 * Removes characters that could cause problems in MP4 tags.
 */
QString MovieData::sanitizeInput(QString input) {
    // Pipe-Zeichen entfernen, da diese als Trenner in iTunEXTC dienen
    return input.replace("|", "").trimmed();
}

QString MovieData::normalizeTagString(const QString &input) {
    QStringList list = input.split(',', Qt::SkipEmptyParts);
    for (QString &s : list) s = s.trimmed();
    return list.join(", ");
}

MovieData MovieData::fromFile(const QString &filePath) {
    MovieData movie;
    movie.filePath = filePath;

    if (QFile::exists(filePath)) {
        // Create Media extractor
        MediaInfoM extractorTags;
        QObject::connect(&extractorTags, &MediaInfoM::errorOccurred, [](const QString &msg){
            qWarning() << msg;
        });

        // Get Metatags from file and fill tag model
        QMap metaTags = extractorTags.extractMetadata(filePath);

        // Fill varaibles with values
        movie.title         = metaTags.value("title");
        movie.cast          = normalizeTagString(metaTags.value("cast"));
        movie.directors     = normalizeTagString(metaTags.value("directors"));
        movie.screenwriters = normalizeTagString(metaTags.value("screenwriters"));
        movie.producers     = normalizeTagString(metaTags.value("producers"));
        movie.studio        = metaTags.value("studio");
        movie.description   = metaTags.value("longDescription");
        movie.genre         = metaTags.value("genre");
        movie.releaseDate   = QDate::fromString(metaTags.value("releaseDate"),Qt::ISODate);

        // xxxxxx Rating sytem: possible adjustments when adding other film formats
        QString rawRating = metaTags.value("iTunEXTC");
        QStringList parts = rawRating.split('|');
        if(parts.size() >= 3) {
            movie.ratingSystem = parts[0];          // de-movie
            movie.ratingText   = parts[1];          // ab 12 Jahren
            movie.ratingScore  = parts[2].toInt();  // 300
        }

        // Cover Image
        movie.cover = QPixmap::fromImage(extractorTags.extractCoverImage(filePath));

    }
     return movie;
}
