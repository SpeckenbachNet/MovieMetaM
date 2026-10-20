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

#include <QObject>
#include <QImage>
#include <QString>
#include <QMap>
#include <QSet>
#include <QFile>
#include <QFileInfo>
#include <QXmlStreamReader>
#include <QtEndian>
#include <QRegularExpression>
#include <QStringList>

class MediaInfoM : public QObject
{
    Q_OBJECT
public:
    explicit MediaInfoM(QObject *parent = nullptr);

    // File types the app can handle (lower case, without dot)
    static QStringList supportedSuffixes();
    static bool isSupported(const QString &filePath);

    // Extracts the cover as a QImage from an MP4 file
    QImage extractCoverImage(const QString &filePath);
    QImage extractCoverImageMp4(const QString &filePath);

    // Extracts selected metadata from an MP4 file as a map
    QMap<QString, QString> extractMetadata(const QString &filePath);
    QMap<QString, QString> extractMetadataMp4(const QString &filePath);

signals:
    void errorOccurred(const QString &message);

private:
    struct Mp4Box {
        quint64 size;
        qint64 headerSize;
        qint64 dataPos;
        char type[4];
    };

    quint32 boxTypeToUint32(const char boxType[4]) const;
    bool readBoxHeader(QFile &file, Mp4Box &box);
    bool findCoverBox(QFile &file, qint64 endPos, QByteArray &coverData);
    void parseBoxes(QFile &file, qint64 endPos, const std::function<void(const Mp4Box &, QFile &)> &visitor);
    void readXmpData(QFile &file, qint64 boxDataEnd, QMap<QString, QString> &metadata);
    void readStringDataBox(QFile &file, QString key, QMap<QString, QString> &metadata, qint64 parentBoxEnd);
    void parseItunMoviXml(const QString &xmlData, QMap<QString, QString> &metadata);
    void parseXmlString(const QString &xmlData, QMap<QString, QString> &metadata);
    QStringList parseNameArray(QXmlStreamReader &xml);
    QString readProprietaryKey(QFile &file, qint64 parentBoxEnd);
    QString detectFileType(const QString &filePath);
};

