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
#include "tableviewm.h"

TableViewM::TableViewM(QWidget *parent)
    : QListView(parent)
{
    setAcceptDrops(true);
    setDragDropMode(QAbstractItemView::DropOnly);
}

int TableViewM::selectedRow() const {
    return currentIndex().row();
}

QString TableViewM::movieKey(int column) const
{
    int row = selectedRow();
    if (row < 0 || !model()) return QString();

    // Get Key from UserRole
    return model()->index(row, column).data(Qt::UserRole).toString();
}

void TableViewM::dragEnterEvent(QDragEnterEvent *event)
{
    //qDebug() << "DRAG ENTER TABLE VIEW";

    if (event->mimeData()->hasUrls())
        event->acceptProposedAction();
    else
        event->ignore();
}

void TableViewM::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void TableViewM::dropEvent(QDropEvent *event)
{
    //qDebug() << "DROP EVENT TABLE VIEW";

    if (!event->mimeData()->hasUrls()) {
        event->ignore();
        return;
    }

    // Create empty list
    QStringList droppedFiles;
    const auto urls = event->mimeData()->urls();

    // Fill list
    for (const QUrl &url : urls) {
        const QString filePath = url.toLocalFile();
        if (!filePath.isEmpty()) {
            qDebug() << "Collected:" << filePath;
            droppedFiles.append(filePath); // Add path to list
        }
    }

    // Send signal once whith hole list.
    if (!droppedFiles.isEmpty()) {
        emit filesDropped(droppedFiles);
    }

    event->acceptProposedAction();
}

void TableViewM::keyPressEvent(QKeyEvent *event)
{
    // Like the button "−" below the list (it asks if a movie has unsaved changes)
    if ((event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) && selectionModel()->hasSelection()) {
        emit removeRequested();
        return;
    }
    QListView::keyPressEvent(event);
}

void TableViewM::mousePressEvent(QMouseEvent *event)
{
    // A click on the empty area would clear the selection while the form still shows a movie
    if (!indexAt(event->pos()).isValid()) {
        event->accept();
        return;
    }
    QListView::mousePressEvent(event);
}
