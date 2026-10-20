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
#include "tagwidget.h"
#include "themecolors.h"

TagWidget::TagWidget(
    const QString &placeholder, const QString &separator,
    QWidget *parent
    ) : QWidget(parent),
        m_separator(separator)
{
    setupUi(placeholder);

    // // --- CONFIGURATION ---
    // // Access the list box created in setupUi
    // m_tagView->setDragEnabled(true);
    // m_tagView->setAcceptDrops(true);
    // m_tagView->setDropIndicatorShown(true);
    // m_tagView->setDragDropMode(QAbstractItemView::InternalMove);
    // m_tagView->setDefaultDropAction(Qt::MoveAction);

    // m_tagModel = new QStringListModel(this);
    // m_tagView->setModel(m_tagModel);

    // // 1. When rows are physically moved (InternalMove)
    // connect(m_tagModel, &QAbstractItemModel::rowsMoved, this, &TagWidget::onRowsMoved);

    // // 2. When the layout is updated
    // connect(m_tagModel, &QAbstractItemModel::layoutChanged, this, &TagWidget::onRowsMoved);

    // // 3. When data changes (if an item is overwritten by a drop)
    // connect(m_tagModel, &QAbstractItemModel::dataChanged, this, &TagWidget::onRowsMoved);
}

void TagWidget::setupUi(const QString &placeholder)
{
    // Vertical layout for the whole widget
    QVBoxLayout *mainVLayout = new QVBoxLayout(this);
    mainVLayout->setContentsMargins(0, 0, 0, 0);
    mainVLayout->setSpacing(0);   // input field and list form one box

    // Create ListView
    m_tagView = new QListView(this);

    // Create Model
    m_tagModel = new QStringListModel(this);
    m_tagView->setModel(m_tagModel);

    // Lower part of the box; items in the accent color of the system, on hover mixed a bit
    // towards the text color (lighter in dark themes, darker in light ones)
    const QColor accent = palette().color(QPalette::Highlight);
    const QString tagColor = accent.name();
    const QString hoverColor = mixColor(accent, palette().color(QPalette::WindowText), 0.20).name();
    applyBoxStyle(m_tagView, false, FieldCorners::Bottom,
        "QListView {font-size: 12px; outline: 0;}"
        "QListView::item { margin-top: 4px; margin-left: 2px; margin-right: 2px; padding: 2px 6px; background-color: " + tagColor + "; color: palette(highlighted-text); border-radius: 5px;}"
        "QListView::item:hover { margin-top: 4px; margin-left: 2px; margin-right: 2px; padding: 2px 6px; background-color: " + hoverColor + "; color: palette(highlighted-text); border-radius: 5px;}"
    );

    // Create lineedit
    m_lineEdit = new QLineEdit(this);
    m_lineEdit->setPlaceholderText(placeholder);
    applyInputStyle(m_lineEdit, FieldCorners::Top);   // upper part of the box

    // add elements to Widget
    mainVLayout->addWidget(m_lineEdit);
    mainVLayout->addWidget(m_tagView);

    m_tagView->setDragEnabled(true);
    m_tagView->setAcceptDrops(true);
    m_tagView->setDropIndicatorShown(true);

    // Important: InternalMove allows reordering within the list
    m_tagView->setDragDropMode(QAbstractItemView::InternalMove);

    // Forces 'Move' instead of 'Copy' (avoids most duplicates)
    m_tagView->setDefaultDropAction(Qt::MoveAction);

    // Connections
    connect(m_lineEdit, &QLineEdit::returnPressed, this, &TagWidget::handleInputFinished);
    connect(m_tagView, &QListView::clicked, this, &TagWidget::removeTagByClick);

    // The safest connects for drag & drop:
    connect(m_tagModel, &QAbstractItemModel::rowsMoved, this, &TagWidget::onRowsMoved);
    connect(m_tagModel, &QAbstractItemModel::layoutChanged, this, &TagWidget::onRowsMoved);
    connect(m_tagModel, &QAbstractItemModel::dataChanged, this, &TagWidget::onRowsMoved);
}

void TagWidget::onRowsMoved() {
    // Trigger onDataChange
    QTimer::singleShot(0, this, [this]() {
        QStringList list = getTags();

        // 2. Send the list (the type now matches the signal)
        emit tagsChanged(list);
        qDebug() << "Order after moving (onRowsMoved)";
    });
}

void TagWidget::setTags(const QString &tags)
{
    // 1. Prepare the data
    QStringList newTags;
    if (!tags.isEmpty()) {
        newTags = tags.split(',', Qt::SkipEmptyParts);
        for (QString &tag : newTags) {
            tag = tag.trimmed();
        }
        newTags.removeAll(QString()); // drop empty entries after trimming
        newTags.removeDuplicates();   // Optional: Doppelte vermeiden
    }

    m_tagList = newTags;
    m_tagModel->setStringList(m_tagList);
    emit tagsChanged(m_tagList);
}

QStringList TagWidget::getTags() const
{
    QStringList currentList;
    int rows = m_tagModel->rowCount();

    for (int i = 0; i < rows; ++i) {
        // Get the value at every index (0 to end)
        QModelIndex index = m_tagModel->index(i, 0);
        QString tag = m_tagModel->data(index, Qt::DisplayRole).toString().trimmed();

        if (!tag.isEmpty()) {
            currentList << tag;
        }
    }

    return currentList;
}

QString TagWidget::getTagsCommaSeperated() const {
    QStringList list = getTags(); // Nutzt deine vorhandene Funktion
    return list.join(", ");       // Klebt alles mit Komma und Leerzeichen zusammen
}

void TagWidget::removeTagByClick(const QModelIndex &index)
{
    if (!index.isValid()) return;

    // Model operation: removeRow() removes the item at the given index.
    m_tagModel->removeRow(index.row());

    // Update the internal list and the signal
    m_tagList = m_tagModel->stringList();
    qDebug() << "The tagsChanged signal will be sent shortly";
    emit tagsChanged(m_tagList);
}

void TagWidget::clearTags()
{
    //
    if (m_lineEdit) {
        //qDebug() << "Delete entries from lineEdit";
        m_lineEdit->clear();
    }

    // Delete entries from ListView
    if (m_tagModel) {
        //qDebug() << "Delete entries from Model (ListView)";
        int rowCount = m_tagModel->rowCount();

        if (rowCount > 0) {
            m_tagModel->removeRows(0,rowCount);
        }
    }
}

void TagWidget::handleInputFinished()
{
    QString input = m_lineEdit->text().trimmed();

    if (input.isEmpty()) {
        return;
    }

    // Split the input (if several tags were entered with a separator)
    QStringList newTags = input.split(m_separator, Qt::SkipEmptyParts);

    // Get the model data (always the latest list)
    QStringList currentTags = m_tagModel->stringList();

    for (const QString &tag : std::as_const(newTags)) {
        QString trimmedTag = tag.trimmed();
        if (!trimmedTag.isEmpty() && !currentTags.contains(trimmedTag, Qt::CaseInsensitive)) {
            currentTags.append(trimmedTag);
        }
    }

    // Update the model, the UI is redrawn automatically
    m_tagModel->setStringList(currentTags);
    m_lineEdit->clear();

    // Update the internal list and the signal
    m_tagList = currentTags;
    emit tagsChanged(m_tagList);
}
