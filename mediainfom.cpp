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
#include "mediainfom.h"
#include <QDebug>

MediaInfoM::MediaInfoM(QObject *parent) : QObject(parent) {}

QStringList MediaInfoM::supportedSuffixes()
{
    // For now MP4 only (m4v is the same format with Apple's suffix)
    return {"mp4", "m4v"};
}

bool MediaInfoM::isSupported(const QString &filePath)
{
    return supportedSuffixes().contains(QFileInfo(filePath).suffix().toLower());
}

quint32 MediaInfoM::boxTypeToUint32(const char boxType[4]) const
{
    // Wandelt die 4 Big-Endian-Bytes in einen quint32 um
    return (quint32(boxType[0]) << 24) |
           (quint32(boxType[1]) << 16) |
           (quint32(boxType[2]) << 8)  |
           (quint32(boxType[3]));
}

// box handling
bool MediaInfoM::findCoverBox(QFile &file, qint64 endPos, QByteArray &coverData)
{
    while (file.pos() < endPos) {
        Mp4Box box;
        if (!readBoxHeader(file, box))
            return false;

        qint64 boxEnd = file.pos() + box.size - box.headerSize;
        QString boxType = QString::fromUtf8(box.type);

        if (boxType == "covr") {
            // Within covr there is often a 'data' box containing image data.
            while (file.pos() < boxEnd) {
                Mp4Box innerBox;
                if (!readBoxHeader(file, innerBox))
                    break;

                QString innerType = QString::fromUtf8(innerBox.type);
                qint64 innerEnd = file.pos() + innerBox.size - innerBox.headerSize;

                if (innerType == "data") {
                    // Skip the first 8 byes (4 Byte Flags + 4 Byte Reserved)
                    file.seek(file.pos() + 8);
                    qint64 dataSize = innerEnd - file.pos();
                    coverData = file.read(dataSize);
                    return true;
                } else {
                    file.seek(innerEnd);
                }
            }
            return false;
        }
        else if (boxType == "ilst" || boxType == "meta" || boxType == "moov" || boxType == "udta") {
            if (boxType == "meta") {
                file.seek(file.pos() + 4); // Sikp version/flags
            }
            if (findCoverBox(file, boxEnd, coverData))
                return true;
            file.seek(boxEnd);
        } else {
            file.seek(boxEnd);
        }
    }
    return false;
}

bool MediaInfoM::readBoxHeader(QFile &file, Mp4Box &box)
{
    if (file.bytesAvailable() < 8)
        return false;

    QByteArray sizeData = file.read(4); // read 4 bytes size
    QByteArray typeData = file.read(4); // read 4 bytes type
    quint32 size32;
    // 1. Verwenden Sie die manuelle Bit-Shift-Logik, die in Qt-Dateien üblich ist:
    size32 = (quint8(sizeData[0]) << 24) |
             (quint8(sizeData[1]) << 16) |
             (quint8(sizeData[2]) << 8)  |
             quint8(sizeData[3]);

    quint64 size = size32;

    memcpy(box.type, typeData.data(), 4); // Copy 4 Bytes
    box.headerSize = 8;

    if (size == 0) {
        size = file.size() - file.pos() + 8; // box extends to EOF
    } else if (size == 1) {
        QByteArray size64Data = file.read(8);
        if (size64Data.size() < 8)
            return false;
        size = 0;
        for (int i = 0; i < 8; ++i)
            size = (size << 8) | quint8(size64Data[i]);
        box.headerSize += 8;
    }

    if (size < box.headerSize) {
        emit errorOccurred(QString("Invalid box size detected: %1 < %2").arg(size).arg(box.headerSize));
        return false;
    }

    box.size = size;
    box.dataPos = file.pos();
    //qDebug() << "===============================";
    //qDebug() << "Box type:" << QString::fromUtf8(box.type) << "Size:" << size << "HeaderSize:" << box.headerSize << "DataPos:" << box.dataPos;

    return true;
}

void MediaInfoM::parseBoxes(QFile &file, qint64 endPos, const std::function<void(const Mp4Box &, QFile &)> &visitor)
{
    //qDebug() << "--- Entering parseBoxes. StartPos:" << file.pos() << "EndPos:" << endPos;

    while (file.pos() < endPos) {
        Mp4Box box;
        qint64 startPos = file.pos();

        if (!readBoxHeader(file, box))
            break;

        qint64 boxEnd = startPos + box.size;

        // Sanity check, wie zuvor
        if (boxEnd > endPos)
            boxEnd = endPos;
        if (boxEnd > file.size())
            boxEnd = file.size();

        if (box.size < box.headerSize || boxEnd <= startPos) {
            emit errorOccurred("Invalid box size or position, aborting parsing");
            break;
        }

        qint64 posBeforeVisit = file.pos();

        // --- HIER WIRD DIE BOX VERARBEITET ---
        visitor(box, file);

        // --- SICHERHEITS-FALLBACK ---
        // Stellt sicher, dass der Dateiposition am Ende der Box landet.
        if (file.pos() < boxEnd) {
            qDebug() << "WARNING: Visitor did not consume box " << QString::fromUtf8(box.type)
            << ". Forcing seek from" << file.pos() << "to" << boxEnd;
            if (!file.seek(boxEnd)) {
                emit errorOccurred("Failed to seek to box end after visit.");
                break;
            }
        } else if (file.pos() > boxEnd) {
            qDebug() << "ERROR: Visitor exceeded box end. Expected" << boxEnd << ", actual" << file.pos();
            break;
        }
    }
    //qDebug() << "--- Exiting parseBoxes. FinalPos:" << file.pos();
}

// extract funtions for several movie types
QImage MediaInfoM::extractCoverImageMp4(const QString &filePath)
{
    // Extracts the cover image from an MP4 file by searching specifically for the 'moov' box,
    // then recursively finds the 'covr' box and finally the 'data' box containing the image bytes.
    // Returns a QImage of the cover or an empty QImage if none found or on error.
    // Emits errorOccurred signal on failures.
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        emit errorOccurred(QString("Cannot open file: %1").arg(filePath));
        return QImage();
    }

    QByteArray coverData;           // Buffer for raw cover image data
    qint64 fileSize = file.size();

    // Iterate over MP4 boxes until end of file
    while (file.pos() < fileSize) {
        Mp4Box box;
        if (!readBoxHeader(file, box))
            break;  // Stop on read error or EOF

        if (box.size < box.headerSize) {
            emit errorOccurred("Invalid box size detected");
            return QImage();
        }

        qint64 boxEnd = file.pos() + box.size - box.headerSize;
        if (boxEnd > fileSize)
            boxEnd = fileSize;

        QString boxType = QString::fromUtf8(box.type);

        // The 'moov' box contains metadata including the cover
        if (boxType == "moov") {
            // Recursively search for cover box inside 'moov'
            if (findCoverBox(file, boxEnd, coverData)) {
                QImage img;
                bool ok = img.loadFromData(coverData);
                if (!ok) {
                    emit errorOccurred("Error: Cover image data cannot be loaded.");
                    return QImage();
                }
                return img;  // Successfully loaded cover image
            }
        } else {
            file.seek(boxEnd);  // Skip non-'moov' boxes
        }
    }

    emit errorOccurred(QString("No cover found in file: %1").arg(filePath));
    return QImage();
}

QMap<QString, QString> MediaInfoM::extractMetadataMp4(const QString &filePath) {
    const quint32 DASH_BOX_NUM = 0x2D2D2D2D;
    const quint32 UUID_BOX_NUM = 0x75756964;

    const QMap<quint32, QString> standardTags = {
        {0xA96E616D, "title"}, {0xA9415254, "artist"}, {0x61415254, "albumArtist"},
        {0xA9616C62, "album"}, {0xA9777274, "composer"}, {0xA967656E, "genre"},
        {0xA9646179, "releaseDate"}, {0x64657363, "description"}, {0x6C646573, "longDescription"}
    };

    QFile file(filePath);
    QMap<QString, QString> metadata;
    if (!file.open(QIODevice::ReadOnly)) return metadata;

    // Kompatible Initialisierung für QSet
    QList<quint32> keys = standardTags.keys();
    QSet<quint32> relevantTagValues;
    for(quint32 k : std::as_const(keys)) relevantTagValues.insert(k);

    std::function<void(const Mp4Box &, QFile &)> metaVisitor = [&](const Mp4Box &box, QFile &file) {
        quint32 boxTypeNum = boxTypeToUint32(box.type);
        QString type = QString::fromUtf8(box.type);
        qint64 boxDataEnd = file.pos() + box.size - box.headerSize;

        if (type == "moov" || type == "trak" || type == "udta" || type == "meta" || type == "ilst") {
            if (type == "meta") file.seek(file.pos() + 4);
            parseBoxes(file, boxDataEnd, metaVisitor);
        } else if (boxTypeNum == UUID_BOX_NUM) {
            readXmpData(file, boxDataEnd, metadata);
        } else if (boxTypeNum == DASH_BOX_NUM) {
            qint64 startPos = file.pos();
            QString key = readProprietaryKey(file, boxDataEnd);
            file.seek(startPos);
            if (!key.isEmpty()) readStringDataBox(file, key, metadata, boxDataEnd);
            file.seek(boxDataEnd);
        } else if (relevantTagValues.contains(boxTypeNum)) {
            readStringDataBox(file, standardTags[boxTypeNum], metadata, boxDataEnd);
            file.seek(boxDataEnd);
        } else {
            file.seek(boxDataEnd);
        }
    };

    // parse file
    parseBoxes(file, file.size(), metaVisitor);

    // Post-processing: Dissecting iTunMOVI XML
    if (metadata.contains("iTunMOVI")) {
        QString xmlData = metadata.value("iTunMOVI");

        // Remove DOCTYPE
        int doctypeIndex = xmlData.indexOf("<!DOCTYPE");
        if (doctypeIndex != -1) {
            int doctypeEnd = xmlData.indexOf('>', doctypeIndex);
            if (doctypeEnd != -1) {
                xmlData.remove(doctypeIndex, doctypeEnd - doctypeIndex + 1);
            }
        }
        // Parse XML data
        parseXmlString(xmlData, metadata);

        // Reomve raw XML
        metadata.remove("iTunMOVI");
    }

    return metadata;
}

// Read/parse functions
QString MediaInfoM::readProprietaryKey(QFile &file, qint64 parentBoxEnd) {
    qint64 startPos = file.pos();
    QByteArray containerData = file.read(parentBoxEnd - startPos);

    // Deep Scan nach bekannten Keys
    if (containerData.contains("iTunMOVI")) return "iTunMOVI";
    if (containerData.contains("iTunEXTC")) return "iTunEXTC";

    // Fallback: Suche nach der 'name' Box Signatur
    int nameIdx = containerData.indexOf("name");
    if (nameIdx >= 4 && nameIdx + 8 < containerData.size()) {
        // Wir nehmen die nächsten 20 Bytes nach dem "name" Header als Text
        return QString::fromUtf8(containerData.mid(nameIdx + 4, 24)).trimmed();
    }
    return "";
}

void MediaInfoM::readStringDataBox(QFile &file, QString key, QMap<QString, QString> &metadata, qint64 parentBoxEnd) {
    qint64 currentPos = file.pos();
    if (parentBoxEnd <= currentPos) return;

    QByteArray buffer = file.read(parentBoxEnd - currentPos);
    QString result = "";

    // A: XML Suche (iTunMOVI)
    int xmlStart = buffer.indexOf("<?xml");
    if (xmlStart == -1) xmlStart = buffer.indexOf("<plist");

    if (xmlStart != -1) {
        int xmlEnd = buffer.lastIndexOf("</plist>");
        if (xmlEnd > xmlStart) {
            xmlEnd += 8;
            result = QString::fromUtf8(buffer.mid(xmlStart, xmlEnd - xmlStart));
        }
    }

    // B: Fallback (Suche nach "data" Header)
    if (result.isEmpty()) {
        int dataIdx = buffer.indexOf("data");
        if (dataIdx != -1 && dataIdx + 12 < buffer.size()) {
            result = QString::fromUtf8(buffer.mid(dataIdx + 12)).trimmed();
        }
    }

    if (!result.isEmpty()) {
        // --- Normalisierung Start ---
        if (key == "releaseDate") {
            // Gehen wir mal davon aus, dass das Datum in folgendem Format ist:
            // ISO date: 2000-12-14T12:00:00Z oder 2000-1-1T12:00:00Z
            // Dann Fallback auf anderer Formate oder nur das Jahr.
            QDate date;
            QDateTime rdate = QDateTime::fromString(result, Qt::ISODate);
            if (rdate.isValid()) {
                date = rdate.date();
            }

            // ------ Fallback ------
            // Attempt: Classic date (1950-12-18)
            if (!date.isValid()) {
                date = QDate::fromString(result, "yyyy-M-d");
            }

            // Attempt: Only the year
            if (!date.isValid()) {
                // Wir nehmen die ersten 4 Zeichen, falls dort "2004-..." steht
                date = QDate::fromString(result.left(4), "yyyy");
            }

            // Write back result
            if (date.isValid()) {
                result = date.toString("yyyy-MM-dd");
            } else {
                // Falls gar nichts geht: Debug-Ausgabe, um das Format zu finden
                qDebug() << "Unknown date format:" << result;
            }
        }
        // else if (key == "ageRating") {
        //     // Falls es der Apple-String ist "de-de|fsk|12|", extrahieren wir die Zahl
        //     if (result.contains("|")) {
        //         QStringList parts = result.split("|", Qt::SkipEmptyParts);
        //         for (const QString &part : std::as_const(parts)) {
        //             bool ok;
        //             int val = part.toInt(&ok);
        //             if (ok) {
        //                 result = QString::number(val);
        //                 break;
        //             }
        //         }
        //     }
        // }
        // --- End of normalizing ---
        metadata.insert(key, result);
    }
    file.seek(parentBoxEnd);
}

// --------- XML Part --------
void MediaInfoM::readXmpData(QFile &file, qint64 boxDataEnd, QMap<QString, QString> &metadata)
{
    qint64 currentPos = file.pos();

    // Die XMP-Datenbox (uuid) besteht aus: 16 Bytes UUID + XML-Daten
    const qint64 uuidSize = 16;

    if (currentPos + uuidSize > boxDataEnd) {
        qDebug() << "readXmpData: Box size too small to contain UUID.";
        file.seek(boxDataEnd); // Korrektur, falls Pointer zu weit ist
        return;
    }

    // 1. UUID lesen (16 Bytes)
    QByteArray uuid = file.read(uuidSize);

    // Standard-XMP-UUID: 89744C68-8849-4002-A819-B7D8D85F8E56
    // Die Box enthält die Bytes ohne Bindestriche (Big-Endian)
    QByteArray xmpUuid = QByteArray::fromHex("89744C6888494002A819B7D8D85F8E56");

    if (uuid == xmpUuid) {
        qDebug() << "XMP: Found standard XMP UUID. Read XML data...";

        // 2. Restliche Daten lesen (XML-Struktur)
        qint64 xmlDataSize = boxDataEnd - file.pos();

        if (xmlDataSize > 0) {
            QByteArray xmlData = file.read(xmlDataSize);
            QString xmlString = QString::fromUtf8(xmlData);

            qDebug() << "XML Data: " << xmlString;
            // OPTION 1: XML-Daten als Roh-String speichern (empfohlen als erster Schritt)
            //metadata.insert("XMP_RAW_XML", xmlString);

            qDebug() << "XMP: XML data successfully read (" << xmlDataSize << " Bytes).";

            // HINWEIS: Hier müsste später die Logik zum Parsen der XML-Struktur (mit QXmlStreamReader) folgen.

        } else {
            qDebug() << "XMP: UUID found, but no XML data present.";
        }
    } else {
        qDebug() << "XMP: UUID box found, but no standard XMP UUID (" << uuid.toHex() << "). Skip.";
    }

    // 3. Unbedingte Zeigerkorrektur
    if (file.pos() != boxDataEnd) {
        if (!file.seek(boxDataEnd)) {
            qDebug() << "XMP: FATAL: Failed to seek to box end" << boxDataEnd << ". Parser might fail.";
        }
    }
}

void MediaInfoM::parseItunMoviXml(const QString &xmlData, QMap<QString, QString> &metadata) {
    if (xmlData.isEmpty()) return;

    // Hilfsfunktion, um einen Block zu isolieren und die Namen zu extrahieren
    auto extractList = [&](QString appleKey) -> QString {
        int keyPos = xmlData.indexOf("<key>" + appleKey + "</key>");
        if (keyPos == -1) return "";

        int arrayStart = xmlData.indexOf("<array>", keyPos);
        int arrayEnd = xmlData.indexOf("</array>", arrayStart);
        if (arrayStart == -1 || arrayEnd == -1) return "";

        QString block = xmlData.mid(arrayStart, arrayEnd - arrayStart);
        QStringList names;

        // Suche alle Inhalte zwischen <string>...</string>
        QRegularExpression re("<string>(.*?)</string>");
        QRegularExpressionMatchIterator i = re.globalMatch(block);
        while (i.hasNext()) {
            names.append(i.next().captured(1));
        }
        return names.join(", ");
    };

    // Jetzt füllen wir die Map mit DEINEN Wunschbegriffen
    QString cast = extractList("cast");
    if (!cast.isEmpty()) metadata.insert("Cast", cast);

    QString directors = extractList("directors");
    if (!directors.isEmpty()) metadata.insert("Directors", directors);

    QString producers = extractList("producers");
    if (!producers.isEmpty()) metadata.insert("Producers", producers);

    QString screenwriters = extractList("screenwriters");
    if (!screenwriters.isEmpty()) metadata.insert("Screenwriters", screenwriters);

    // Studio ist kein Array, sondern ein direkter String nach dem Key
    QRegularExpression reStudio("<key>studio</key>\\s*<string>(.*?)</string>");
    QRegularExpressionMatch m = reStudio.match(xmlData);
    if (m.hasMatch()) metadata.insert("Studio", m.captured(1));
}

QStringList MediaInfoM::parseNameArray(QXmlStreamReader &xml) {
    QStringList names;
    while (!xml.atEnd() && !xml.hasError()) {
        xml.readNext();

        if (xml.isStartElement() && xml.name() == "dict") {
            //qDebug() << "Enter element dict";
            QString name;
            while (!(xml.isEndElement() && xml.name() == "dict")) {
                xml.readNext();
                if (xml.isStartElement() && xml.name() == "key") {
                    //qDebug() << "Enter element key";
                    QString key = xml.readElementText();
                    if (key == "name") {
                        xml.readNext();
                        if (xml.readNext() && xml.name() == "string") {
                            name = xml.readElementText();
                        }
                    }
                }
            }
            if (!name.isEmpty())
                names << name;
        } else if (xml.isEndElement() && xml.name() == "array") {
            break; // End of arrays
        }
    }
    return names;
}

void MediaInfoM::parseXmlString(const QString &xmlData, QMap<QString, QString> &metadata) {
    QXmlStreamReader xml(xmlData);

    QString currentKey;
    bool expectingValue = false;

    while (!xml.atEnd() && !xml.hasError()) {
        xml.readNext();

        if (xml.isStartElement()) {
            if (xml.name() == "key") {
                // Element key: remeber key
                currentKey = xml.readElementText();
                //qDebug() << "Tag key - Current Key: " << currentKey;
                expectingValue = true;
            } else if (expectingValue && xml.name() == "string") {
                metadata[currentKey] = xml.readElementText();
                expectingValue = false;
            } else if (expectingValue && xml.name() == "array") {
                // Elemnet array: fetch all names
                //qDebug() << "Current Key: " << currentKey;
                QStringList names = parseNameArray(xml);
                metadata[currentKey] = names.join(", ");
                expectingValue = false;
            }
        }
    }

    if (xml.hasError()) {
        qWarning() << "XML Fehler:" << xml.errorString();
    }
}

// extract function with filetype detect
QString MediaInfoM::detectFileType(const QString &filePath)
{
    QString suffix = QFileInfo(filePath).suffix().toLower();
    return suffix;
}

QMap<QString, QString> MediaInfoM::extractMetadata(const QString &filePath)
{
    QString fileType = detectFileType(filePath);

    if (fileType == "mp4" || fileType == "m4v" || fileType == "3gp") {
        return extractMetadataMp4(filePath);
    }
    else if (fileType == "mkv") {
        emit errorOccurred("MKV-Cover-Extraktion noch nicht implementiert");
        return {};   // empty map: no metadata
    }
    else if (fileType == "avi") {
        emit errorOccurred("AVI-Cover-Extraktion noch nicht implementiert");
        return {};
    }
    else {
        emit errorOccurred("Unbekanntes oder nicht unterstütztes Format: " + fileType);
        return {};
    }
}

QImage MediaInfoM::extractCoverImage(const QString &filePath)
{
    QString fileType = detectFileType(filePath);

    if (fileType == "mp4" || fileType == "m4v") {
        return extractCoverImageMp4(filePath);
    }
    else if (fileType == "mkv") {
        emit errorOccurred("MKV-Cover-Extraktion noch nicht implementiert");
        return QImage();
    }
    else if (fileType == "avi") {
        emit errorOccurred("AVI-Cover-Extraktion noch nicht implementiert");
        return QImage();
    }
    else {
        emit errorOccurred("Unbekanntes oder nicht unterstütztes Format: " + fileType);
        return QImage();
    }
}

