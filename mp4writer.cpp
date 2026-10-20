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
#include "mp4writer.h"
#include "mediainfom.h"

#include <mp4coverart.h>
#include <mp4file.h>
#include <mp4item.h>
#include <mp4tag.h>
#include <tstringlist.h>

#include <QBuffer>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QImage>
#include <QRegularExpression>
#include <QStorageInfo>

namespace {

// TagLib strings and keys are UTF-8 (the key "\xC2\xA9nam" is the atom (c)nam)
TagLib::String tl(const QString &text)
{
    return TagLib::String(text.toUtf8().constData(), TagLib::String::UTF8);
}

TagLib::String key(const char *utf8)
{
    return TagLib::String(utf8, TagLib::String::UTF8);
}

const char *const kFreeform = "----:com.apple.iTunes:";

void setText(TagLib::MP4::Tag *tag, const TagLib::String &name, const QString &text)
{
    if (text.isEmpty()) {
        tag->removeItem(name);
    } else {
        tag->setItem(name, TagLib::MP4::Item(TagLib::StringList(tl(text))));
    }
}

QStringList splitNames(const QString &names)
{
    QStringList list = names.split(',', Qt::SkipEmptyParts);
    for (QString &name : list) name = name.trimmed();
    list.removeAll(QString());
    return list;
}

QString nameArray(const QString &keyName, const QString &names)
{
    const QStringList list = splitNames(names);
    if (list.isEmpty()) return QString();
    QString xml = QStringLiteral("\t<key>%1</key>\n\t<array>\n").arg(keyName);
    for (const QString &name : list) {
        xml += QStringLiteral("\t\t<dict>\n\t\t\t<key>name</key>\n\t\t\t<string>%1</string>\n\t\t</dict>\n")
                   .arg(name.toHtmlEscaped());
    }
    return xml + QStringLiteral("\t</array>\n");
}

} // namespace

QString Mp4Writer::buildItunMovi(const MovieData &movie, const QString &existing)
{
    QString body;
    body += nameArray("cast", movie.cast);
    body += nameArray("directors", movie.directors);
    body += nameArray("producers", movie.producers);
    body += nameArray("screenwriters", movie.screenwriters);
    if (!movie.studio.isEmpty()) {
        body += QStringLiteral("\t<key>studio</key>\n\t<string>%1</string>\n").arg(movie.studio.toHtmlEscaped());
    }

    // What the app does not edit stays: the codirectors of the old entry
    static const QRegularExpression codirectors(
        QStringLiteral("<key>codirectors</key>\\s*<array>.*?</array>"), QRegularExpression::DotMatchesEverythingOption);
    const QRegularExpressionMatch kept = codirectors.match(existing);
    if (kept.hasMatch()) body += QStringLiteral("\t") + kept.captured(0) + QStringLiteral("\n");

    if (body.isEmpty()) return QString();
    return QStringLiteral(
               "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
               "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n"
               "<plist version=\"1.0\">\n<dict>\n")
           + body + QStringLiteral("</dict>\n</plist>\n");
}

bool Mp4Writer::writeTags(const QString &path, const MovieData &movie, const QImage &cover,
                          const QImage &originalCover, QString *error)
{
#ifdef Q_OS_WIN
    TagLib::MP4::File file(path.toStdWString().c_str());
#else
    TagLib::MP4::File file(QFile::encodeName(path).constData());
#endif
    if (!file.isValid() || !file.tag()) {
        *error = tr("The file could not be read as MP4.");
        return false;
    }
    TagLib::MP4::Tag *tag = file.tag();

    setText(tag, key("\xC2\xA9nam"), movie.title);
    setText(tag, key("ldes"), movie.description);
    setText(tag, key("desc"), movie.description.left(255));   // the short description has a limit
    setText(tag, key("\xC2\xA9gen"), movie.genre);
    setText(tag, key("\xC2\xA9""day"), movie.releaseDate.isValid() ? movie.releaseDate.toString(Qt::ISODate) : QString());

    // Age rating: "system|code|score|", like the media servers read it
    setText(tag, key((QByteArray(kFreeform) + "iTunEXTC").constData()), movie.getItunExtc());

    // Cast and crew: one plist
    const TagLib::String moviKey = key((QByteArray(kFreeform) + "iTunMOVI").constData());
    QString existing;
    if (tag->contains(moviKey)) {
        const TagLib::StringList old = tag->item(moviKey).toStringList();
        if (!old.isEmpty()) existing = QString::fromUtf8(old.front().toCString(true));
    }
    setText(tag, moviKey, buildItunMovi(movie, existing));

    // Cover: only if it changed (an unchanged one would be encoded again and lose quality)
    if (cover.isNull() != originalCover.isNull() || (!cover.isNull() && cover != originalCover)) {
        if (cover.isNull()) {
            tag->removeItem(key("covr"));
        } else {
            const QImage &image = cover;
            QByteArray bytes;
            QBuffer buffer(&bytes);
            buffer.open(QIODevice::WriteOnly);
            const bool png = image.hasAlphaChannel();
            image.save(&buffer, png ? "PNG" : "JPEG", 95);
            TagLib::MP4::CoverArtList covers;
            covers.append(TagLib::MP4::CoverArt(png ? TagLib::MP4::CoverArt::PNG : TagLib::MP4::CoverArt::JPEG,
                                                TagLib::ByteVector(bytes.constData(), bytes.size())));
            tag->setItem(key("covr"), TagLib::MP4::Item(covers));
        }
    }

    if (!file.save()) {
        *error = tr("The tags could not be written.");
        return false;
    }
    return true;
}

// The written file is read again with the reader of the app
bool Mp4Writer::verify(const QString &path, const MovieData &movie, const QImage &cover)
{
    MediaInfoM info;
    const QMap<QString, QString> meta = info.extractMetadata(path);
    const auto differs = [&](const char *name, const QString &read, const QString &wanted) {
        if (read == wanted) return false;
        qWarning() << "Verify:" << name << "differs. Read:" << read.left(80) << "Wanted:" << wanted.left(80);
        return true;
    };
    if (differs("title", meta.value("title"), movie.title)) return false;
    if (differs("description", meta.value("longDescription"), movie.description)) return false;
    if (differs("age rating", meta.value("iTunEXTC"), movie.getItunExtc())) return false;
    if (!cover.isNull() && info.extractCoverImage(path).isNull()) {
        qWarning() << "Verify: the cover cannot be read";
        return false;
    }
    return true;
}

bool Mp4Writer::save(const MovieData &movie, const QImage &cover, const QImage &originalCover,
                     bool safe, QString *error)
{
    QString dummy;
    if (!error) error = &dummy;

    const QString path = movie.filePath;
    const QFileInfo info(path);
    if (!info.exists()) {
        *error = tr("The file does not exist.");
        return false;
    }
    if (!info.isWritable()) {
        *error = tr("The file is not writable.");
        return false;
    }

    // Direct: TagLib writes into the file itself, then it is read back
    if (!safe) {
        if (!writeTags(path, movie, cover, originalCover, error)) return false;
        if (!verify(path, movie, cover)) {
            *error = tr("The file was written, but the check failed. Please look at the file.");
            return false;
        }
        return true;
    }

    // Safe: the copy needs as much room as the file
    if (QStorageInfo(info.absolutePath()).bytesAvailable() < info.size() + 16 * 1024 * 1024) {
        *error = tr("Not enough free space.");
        return false;
    }

    // The copy keeps the suffix (the reader knows the format by it): "movie.tmp.mp4"
    const QString stem = info.absolutePath() + '/' + info.completeBaseName();
    const QString temp = stem + QStringLiteral(".tmp.") + info.suffix();
    const QString backup = path + QStringLiteral(".bak");
    QFile::remove(temp);
    if (!QFile::copy(path, temp)) {
        *error = tr("The file could not be copied.");
        return false;
    }

    if (!writeTags(temp, movie, cover, originalCover, error)) {
        QFile::remove(temp);
        return false;
    }
    if (!verify(temp, movie, cover)) {
        QFile::remove(temp);
        *error = tr("The saved file could not be verified.");
        return false;
    }

    QFile::setPermissions(temp, QFile::permissions(path));   // as the original had them

    // Replace: the original goes aside first, so it comes back if something fails
    QFile::remove(backup);
    if (!QFile::rename(path, backup)) {
        QFile::remove(temp);
        *error = tr("The file could not be replaced.");
        return false;
    }
    if (!QFile::rename(temp, path)) {
        QFile::rename(backup, path);
        QFile::remove(temp);
        *error = tr("The file could not be replaced.");
        return false;
    }
    QFile::remove(backup);
    return true;
}
