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

#include <QApplication>
#include <QFontMetrics>
#include <QListView>
#include <QMouseEvent>
#include <QPainter>
#include <QShortcut>
#include <QStyledItemDelegate>

namespace {

// Tags are drawn by the style sheet; this delegate adds a × at the right end of the tag under
// the mouse. Only a click on the × removes the tag (a click on the text selects it or starts
// dragging to reorder).
class TagDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    using QStyledItemDelegate::QStyledItemDelegate;

    // Same as the margins in the style sheet: the visible tag inside the item rect
    static QRect tagRect(const QRect &itemRect) { return itemRect.adjusted(2, 4, -2, 0); }

    static QRectF crossRect(const QRect &itemRect)
    {
        const QRect tag = tagRect(itemRect);
        const qreal size = 7.0;
        return QRectF(tag.right() - 7.0 - size, tag.center().y() - size / 2.0 + 0.5, size, size);
    }

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override
    {
        QStyledItemDelegate::paint(painter, option, index);
        if (!(option.state & QStyle::State_MouseOver)) return;

        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setPen(QPen(option.palette.color(QPalette::HighlightedText), 1.5, Qt::SolidLine, Qt::RoundCap));
        const QRectF cross = crossRect(option.rect);
        painter->drawLine(cross.topLeft(), cross.bottomRight());
        painter->drawLine(cross.topRight(), cross.bottomLeft());
        painter->restore();
    }

    // Only the height counts: the tag takes the width of the list. With its own width a long name
    // would make the line wider than the list: a horizontal scroll bar, and the × (at the right end
    // of the tag) out of sight. The text is elided instead (ElideRight of the view).
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        QSize size = QStyledItemDelegate::sizeHint(option, index);
        size.setWidth(1);
        return size;
    }

    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option,
                     const QModelIndex &index) override
    {
        if (event->type() == QEvent::MouseButtonRelease) {
            const auto *mouse = static_cast<QMouseEvent *>(event);
            // A bit more room than the drawn × to hit it easily
            const QRectF hit = crossRect(option.rect).adjusted(-4, -4, 4, 4);
            if (mouse->button() == Qt::LeftButton && hit.contains(mouse->position())) {
                emit removeRequested(index);
                return true;
            }
        }
        return QStyledItemDelegate::editorEvent(event, model, option, index);
    }

signals:
    void removeRequested(const QModelIndex &index);
};

// The tags as a string list; a name that does not fit into the list (elided with "…") gets its full
// text as the tooltip of the entry
class TagModel : public QStringListModel
{
public:
    using QStringListModel::QStringListModel;

    QListView *view = nullptr;

    QVariant data(const QModelIndex &index, int role) const override
    {
        if (role == Qt::ToolTipRole && view) {
            const QString text = QStringListModel::data(index, Qt::DisplayRole).toString();
            // Room for the padding of the tag (6 + 20 px) and the margins
            const int room = view->viewport()->width() - 40;
            return QFontMetrics(view->font()).horizontalAdvance(text) > room ? QVariant(text) : QVariant();
        }
        return QStringListModel::data(index, role);
    }
};

} // namespace

// The input field (upper part of the box) and the list (lower part); items in the accent color of
// the system, on hover mixed a bit towards the text color (lighter in dark themes, darker in light
// ones); the selected tag (target of the Delete key) the same. Room on the right for the ×
// (TagDelegate).
void TagWidget::restyle()
{
    const QPalette pal = QApplication::palette();
    const QColor accent = pal.color(QPalette::Highlight);
    const QString tagColor = accent.name();
    const QString textColor = pal.color(QPalette::HighlightedText).name();
    const QString hoverColor = mixColor(accent, pal.color(QPalette::WindowText), 0.20).name();
    applyBoxStyle(m_tagView, false, FieldCorners::Bottom,
        "QListView {font-size: 12px; outline: 0;}"
        "QListView::item { margin-top: 4px; margin-left: 2px; margin-right: 2px; padding: 2px 20px 2px 6px; background-color: " + tagColor + "; color: " + textColor + "; border-radius: 5px;}"
        "QListView::item:hover, QListView::item:selected { margin-top: 4px; margin-left: 2px; margin-right: 2px; padding: 2px 20px 2px 6px; background-color: " + hoverColor + "; color: " + textColor + "; border-radius: 5px;}"
    );
    applyInputStyle(m_lineEdit, FieldCorners::Top);
}

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
    auto *tagModel = new TagModel(this);
    tagModel->view = m_tagView;
    m_tagModel = tagModel;
    m_tagView->setModel(m_tagModel);
    m_tagView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);   // long names are elided, not scrolled
    m_tagView->setTextElideMode(Qt::ElideRight);

    auto *delegate = new TagDelegate(m_tagView);
    m_tagView->setItemDelegate(delegate);
    m_tagView->setMouseTracking(true);   // × follows the mouse

    // Create lineedit
    m_lineEdit = new QLineEdit(this);
    m_lineEdit->setPlaceholderText(placeholder);
    restyle();

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
    connect(delegate, &TagDelegate::removeRequested, this, &TagWidget::removeTag);
    // Keyboard: Delete or Backspace removes the selected tag (only a visibly selected one,
    // not Qt's current index, which may be set without a selection)
    for (const QKeySequence &key : {QKeySequence(Qt::Key_Delete), QKeySequence(Qt::Key_Backspace)}) {
        auto *shortcut = new QShortcut(key, m_tagView, nullptr, nullptr, Qt::WidgetShortcut);
        connect(shortcut, &QShortcut::activated, this, [this]() {
            const QModelIndexList selected = m_tagView->selectionModel()->selectedIndexes();
            if (!selected.isEmpty()) removeTag(selected.first());
        });
    }
    // The selection only serves the Delete key: Escape or leaving the list unselects
    auto *escape = new QShortcut(QKeySequence(Qt::Key_Escape), m_tagView, nullptr, nullptr, Qt::WidgetShortcut);
    connect(escape, &QShortcut::activated, m_tagView->selectionModel(), &QItemSelectionModel::clear);
    qApp->installEventFilter(this);   // focus of the list and clicks anywhere in the app

    // The safest connects for drag & drop:
    connect(m_tagModel, &QAbstractItemModel::rowsMoved, this, &TagWidget::onRowsMoved);
    connect(m_tagModel, &QAbstractItemModel::layoutChanged, this, &TagWidget::onRowsMoved);
    connect(m_tagModel, &QAbstractItemModel::dataChanged, this, &TagWidget::onRowsMoved);

    // After dragging no tag stays selected: depending on the direction Qt leaves the selection
    // on the moved tag or on its neighbour. At once, not delayed: a drag may insert a copy and
    // remove the original only afterwards.
    connect(m_tagModel, &QAbstractItemModel::rowsMoved, m_tagView->selectionModel(), &QItemSelectionModel::clear);
    connect(m_tagModel, &QAbstractItemModel::rowsRemoved, m_tagView->selectionModel(), &QItemSelectionModel::clear);
}

bool TagWidget::eventFilter(QObject *watched, QEvent *event)
{
    // Focus elsewhere (e.g. Tab): without the focus the Delete key does nothing, the selection
    // would only mislead (a popup such as a context menu keeps it)
    if (watched == m_tagView && event->type() == QEvent::FocusOut
        && static_cast<QFocusEvent *>(event)->reason() != Qt::PopupFocusReason) {
        m_tagView->selectionModel()->clear();
    }
    // A click outside the list, also on an empty area that takes no focus
    else if (event->type() == QEvent::MouseButtonPress && m_tagView->selectionModel()->hasSelection()) {
        auto *widget = qobject_cast<QWidget *>(watched);
        if (widget && widget != m_tagView && !m_tagView->isAncestorOf(widget)) {
            m_tagView->selectionModel()->clear();
        }
    }
    return QWidget::eventFilter(watched, event);
}

void TagWidget::onRowsMoved() {
    // Trigger onDataChange
    QTimer::singleShot(0, this, [this]() {
        QStringList list = getTags();

        // 2. Send the list (the type now matches the signal)
        emit tagsChanged(list);
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

void TagWidget::removeTag(const QModelIndex &index)
{
    if (!index.isValid()) return;

    // Model operation: removeRow() removes the item at the given index.
    m_tagModel->removeRow(index.row());
    // No tag selected afterwards: else Qt selects the next one and Delete would remove it too
    m_tagView->selectionModel()->clear();

    // Update the internal list and the signal
    m_tagList = m_tagModel->stringList();
    emit tagsChanged(m_tagList);
}

void TagWidget::clearTags()
{
    //
    if (m_lineEdit) {
        m_lineEdit->clear();
    }

    // Delete entries from ListView
    if (m_tagModel) {
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

#include "tagwidget.moc"
