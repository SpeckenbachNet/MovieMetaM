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

    // // --- HIER DIE KONFIGURATION ---
    // // Wir greifen auf die ListBox zu, die in setupUi erstellt wurde
    // m_tagView->setDragEnabled(true);
    // m_tagView->setAcceptDrops(true);
    // m_tagView->setDropIndicatorShown(true);
    // m_tagView->setDragDropMode(QAbstractItemView::InternalMove);
    // m_tagView->setDefaultDropAction(Qt::MoveAction);

    // m_tagModel = new QStringListModel(this);
    // m_tagView->setModel(m_tagModel);

    // // 1. Wenn Zeilen physisch verschoben werden (InternalMove)
    // connect(m_tagModel, &QAbstractItemModel::rowsMoved, this, &TagWidget::onRowsMoved);

    // // 2. Wenn das Layout aktualisiert wird
    // connect(m_tagModel, &QAbstractItemModel::layoutChanged, this, &TagWidget::onRowsMoved);

    // // 3. Wenn Daten geändert werden (falls ein Item beim Drop überschrieben wird)
    // connect(m_tagModel, &QAbstractItemModel::dataChanged, this, &TagWidget::onRowsMoved);
}

void TagWidget::setupUi(const QString &placeholder)
{
    // Vertical layout for the whole widget
    QVBoxLayout *mainVLayout = new QVBoxLayout(this);
    mainVLayout->setContentsMargins(0, 0, 0, 0);
    //mainVLayout->setSpacing(0);

    // Create ListView
    m_tagView = new QListView(this);

    // Create Model
    m_tagModel = new QStringListModel(this);
    m_tagView->setModel(m_tagModel);

    // Style ListView items: same grey as the headers, a bit stronger on hover
    const QString tagColor = accentGrey(palette()).name();
    const QString hoverColor = accentGrey(palette(), 0.5).name();
    m_tagView->setStyleSheet(
        "QListView {font-size: 12px;}"
        "QListView::item { margin-top: 4px; margin-left: 2px; margin-right: 2px; padding: 2px 6px; background-color: " + tagColor + "; color: palette(window-text); border-radius: 5px;}"
        "QListView::item:hover { margin-top: 4px; margin-left: 2px; margin-right: 2px; padding: 2px 6px; background-color: " + hoverColor + "; color: palette(window-text); border-radius: 5px;}"
    );

    // Create lineedit
    m_lineEdit = new QLineEdit(this);
    m_lineEdit->setPlaceholderText(placeholder);

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

    // Die sichersten Connects für Drag & Drop:
    connect(m_tagModel, &QAbstractItemModel::rowsMoved, this, &TagWidget::onRowsMoved);
    connect(m_tagModel, &QAbstractItemModel::layoutChanged, this, &TagWidget::onRowsMoved);
    connect(m_tagModel, &QAbstractItemModel::dataChanged, this, &TagWidget::onRowsMoved);
}

void TagWidget::onRowsMoved() {
    // Trigger onDataChange
    QTimer::singleShot(0, this, [this]() {
        QStringList list = getTags();

        // 2. Sende die Liste (jetzt passt der Typ zum Signal)
        emit tagsChanged(list);
        qDebug() << "Order after moving (onRowsMoved)";
    });
}

void TagWidget::setTags(const QString &tags)
{
    // 1. Vorbereitung der Daten
    QStringList newTags;
    if (!tags.isEmpty()) {
        newTags = tags.split(',', Qt::SkipEmptyParts);
        for (QString &tag : newTags) {
            tag = tag.trimmed();
        }
        newTags.removeAll(QString()); // Leere Einträge nach dem Trimmen raus
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
        // Wir holen uns den Wert an jedem Index (0 bis Ende)
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

    // Model-Operation: removeRow() entfernt das Element am gegebenen Index.
    m_tagModel->removeRow(index.row());

    // Interne Liste und Signal aktualisieren
    m_tagList = m_tagModel->stringList();
    qDebug() << "Signal tagsChanged wird gleich gesendet";
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

    // Aufteilen der Eingabe (falls mehrere Tags mit Separator eingegeben wurden)
    QStringList newTags = input.split(m_separator, Qt::SkipEmptyParts);

    // Model-Daten abrufen (ist immer die aktuellste Liste)
    QStringList currentTags = m_tagModel->stringList();

    for (const QString &tag : std::as_const(newTags)) {
        QString trimmedTag = tag.trimmed();
        if (!trimmedTag.isEmpty() && !currentTags.contains(trimmedTag, Qt::CaseInsensitive)) {
            currentTags.append(trimmedTag);
        }
    }

    // Model aktualisieren und UI wird automatisch neu gezeichnet
    m_tagModel->setStringList(currentTags);
    m_lineEdit->clear();

    // Interne Liste und Signal aktualisieren
    m_tagList = currentTags;
    emit tagsChanged(m_tagList);
}
