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

#include <QLabel>
#include <QMimeData>
#include <QUrl>
#include <QDragEnterEvent>
#include <QPixmap>
#include <QEvent>
#include <QDrag>
#include <QDir>

// Cover of a movie: painted by itself (rounded like the input fields, sharp on scaled screens),
// takes images by drag and drop and can be dragged out as a file.
class ImageLabelM : public QLabel
{
    Q_OBJECT
public:
    explicit ImageLabelM(QWidget *parent = nullptr);

    void setCover(const QPixmap &cover);   // full resolution; empty: placeholder
    QPixmap cover() const { return m_cover; }
    void clear() { setCover(QPixmap()); }

private:
    QPixmap m_cover;               // in full resolution, also for dragging out
    bool m_dragActive = false;     // an image is dragged over the label
    // Data of the current drag, read when it enters: on Wayland a browser (Chromium/Brave)
    // delivers the data only while the drag runs – read first on the drop, it is empty
    QUrl m_dropUrl;
    QByteArray m_dropImage;
    void readDragData(const QMimeData *mime);
    void clearDragData();
    QPoint m_dragStartPos;         // drag out only after the start distance (no drag by a click)

protected:
    void paintEvent(QPaintEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

signals:
    void coverDropped(const QPixmap &cover);   // image is here (local file or image data)
    void coverUrlDropped(const QUrl &url);     // web link: to be downloaded
    void coverDropFailed();                    // nothing usable in the drop
};

