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
#include "imagelabelm.h"

ImageLabelM::ImageLabelM(QWidget *parent) : QLabel(parent) {
    // Das ist wichtig, damit das Label überhaupt auf Drops reagiert
    setAcceptDrops(true);

    // Ein bisschen modernes Styling direkt im Code
    setAlignment(Qt::AlignCenter);
    setText("Cover hierher ziehen");
    setStyleSheet("border: 2px dashed #666; color: #666;");
}

void ImageLabelM::clear() {
    // Load placeholder
    m_placeholderPixmap = QPixmap(":/images/noimage.svg");

    // Set placeholder
    if (!m_placeholderPixmap.isNull()) {
        this->setPixmap(m_placeholderPixmap.scaled(this->size(),
                                             Qt::KeepAspectRatio,
                                             Qt::SmoothTransformation));
    } else {
        QLabel::clear(); // Fallback, if placeholder could not be loaded
    }
    qDebug() << "ImageLabelM: Internal status corrected & placeholder set.";
}

QPixmap ImageLabelM::originalPixmap() const {
    return m_fullPixmap;
}

void ImageLabelM::setOriginalPixmap(const QPixmap &pix) {
    m_fullPixmap = pix;
    // Für die Anzeige im Label verkleinern wir es nur optisch
    if (!pix.isNull()) {
        // Anzeige-Version für das UI (skaliert)
        setPixmap(m_fullPixmap.scaled(this->size(),
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation)
        );
    } else {
        clear(); // Falls das Bild leer ist
    }
}

void ImageLabelM::dragEnterEvent(QDragEnterEvent *event) {
    // Wir prüfen, ob der User eine Datei (URL) hält
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
        // Optisches Feedback: Rahmen wird hell
        setStyleSheet("border: 2px solid #3498db; background-color: rgba(52, 152, 219, 0.1);");
    }
}

void ImageLabelM::dragLeaveEvent(QDragLeaveEvent *event) {
    // User moved the mouse away again -> Reset style
    setStyleSheet("border: 2px dashed #666; background-color: transparent;");
}

void ImageLabelM::dragMoveEvent(QDragMoveEvent *event) {
    //qDebug() << "Enter dragMoveEvent";
    // if (event->mimeData()->hasUrls()) {
    //     event->acceptProposedAction();
    // }
}

void ImageLabelM::dropEvent(QDropEvent *event) {
    //qDebug() << "LABEL: Drop detected!";
    setStyleSheet("border: 2px dashed #666;"); // Back to standard
    event->acceptProposedAction();
    const QMimeData* mime = event->mimeData();

    if (mime->hasUrls()) {

        QUrl url = mime->urls().value(0);
        if (url.isLocalFile()) {
            QString path = url.toLocalFile();
            setPixmap(QPixmap(path).scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
            emit imageChanged(path);
        }
        // Image URL from Browser
        else {
            qDebug() << "Browser-URL detected:" << url.toString();
            qDebug() << "LABEL: Send Signal now!";
            emit imageChanged(url.toString());

        }
    }
    // The image is sent directly as a data packet
    else if (mime->hasFormat("image/png") || mime->hasFormat("image/jpeg")) {
        QPixmap pix;
        pix.loadFromData(mime->data("image/png")); // Attempting to load the raw data
        if (!pix.isNull()) {
            setPixmap(pix.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
            emit imageChanged("raw_data_from_browser");
        }
    }
}

void ImageLabelM::mouseMoveEvent(QMouseEvent *event)
{
    qDebug() << "mouseMoveEvent entered";
    if (!(event->buttons() & Qt::LeftButton)) return;

    if (!this->pixmap()) {
        qDebug() << "Abort: Labe has no Pixmap!";
        return;
    }
    QPixmap pix = this->pixmap(Qt::ReturnByValue);

    if (pix.isNull()) {
        qDebug() << "Abort: Pixmap is empty!";
        return;
    }

    qDebug() << "Pixmap found! Size: " << pix.size();

    QString tempPath = QDir::tempPath() + "/exported_cover.jpg";
    if (!pix.save(tempPath, "JPG", 95)) {  // JPG quality 95%
        qDebug() << "Could not save file to " << tempPath;
        return;
    }

    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;
    mimeData->setUrls({QUrl::fromLocalFile(tempPath)});
    drag->setMimeData(mimeData);

    drag->setPixmap(pix.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    drag->setHotSpot(QPoint(50, 50));

    qDebug() << "Starte Drag-Vorgang...";
    drag->exec(Qt::CopyAction);
}
