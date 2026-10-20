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

#include <QListView>
#include <QDragEnterEvent>
#include <QMouseEvent>
#include <QMimeData>

// Movie list: a list (not a table), so the style sheet can draw each entry as one
// rounded piece; accepts dropped files
class TableViewM : public QListView
{
    Q_OBJECT
public:
    explicit TableViewM(QWidget *parent = nullptr);
    int selectedRow() const;
    QString movieKey(int column = 0) const; // the file path (key) is in UserRole of column 0

signals:
    void filesDropped(const QStringList &paths);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

};

