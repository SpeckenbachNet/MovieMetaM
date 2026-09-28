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

class ImageLabelM : public QLabel
{
    Q_OBJECT
public:
    explicit ImageLabelM(QWidget *parent = nullptr);
    QPixmap originalPixmap() const;
    void setOriginalPixmap(const QPixmap &pix);
    void clear();

private:
    QPixmap m_fullPixmap;
    QPixmap m_placeholderPixmap;
    QPoint m_dragStartPos; // To avoid accidental dragging when clicking.

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

signals:
    void imageChanged(const QString &pathOrUrl);
};

