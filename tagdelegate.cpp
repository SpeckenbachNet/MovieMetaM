// tagdelegate.cpp

#include "tagdelegate.h"
#include <QPainter>
#include <QStyleOptionViewItem>

// 1. Constructor
TagDelegate::TagDelegate(const QString &styleSheet, QObject *parent)
    : QStyledItemDelegate(parent),
    m_styleSheet(styleSheet)
{
    // The styling is kept for later use
}

// 2. paint()
void TagDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // For now only the default text is drawn; drawing the background and
    // the rounded corners with QPainter is still missing.

    // For now call the base class to draw the text.
    QStyledItemDelegate::paint(painter, option, index);
}

// 3. sizeHint()
QSize TagDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // The correct size of the tag still has to be calculated: for now the
    // base class size, which is usually too small, is used.
    QSize baseSize = QStyledItemDelegate::sizeHint(option, index);

    // Add some padding so the text is not cut off
    return QSize(baseSize.width() + 10, baseSize.height() + 4);
}
