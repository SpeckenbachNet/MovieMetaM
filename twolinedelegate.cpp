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
#include "twolinedelegate.h"
#include "themecolors.h"

#include <QAbstractItemView>
#include <QPainter>

namespace {

// Second line: a bit smaller than the title
QFont secondLineFont(const QFont &titleFont)
{
    QFont font = titleFont;
    if (font.pointSizeF() > 0) {
        font.setPointSizeF(font.pointSizeF() * 0.88);
    } else {
        font.setPixelSize(qRound(font.pixelSize() * 0.88));
    }
    return font;
}

} // namespace

TwoLineDelegate::TwoLineDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

void TwoLineDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    // Text from the model: initStyleOption() turns "\n" into a line separator (U+2028)
    const QString text = index.data(Qt::DisplayRole).toString();
    const QString title = text.section('\n', 0, 0);
    const QString second = text.section('\n', 1);
    const bool selected = opt.state & QStyle::State_Selected;

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    // Entry with a little distance to the border and to the neighbours
    const QRect entry = opt.rect.adjusted(kMarginH, kMarginV, -kMarginH, -kMarginV);
    QColor titleColor = opt.palette.color(QPalette::Text);
    if (selected) {
        // Background from the group of the view (Qt drops State_Active when the list loses the
        // focus), the text always from the active group: Windows has black as inactive
        // HighlightedText, black on the accent color of the style sheet. If they still do not
        // contrast (seen on macOS with "Graphite": light grey with white text), black or white
        const QPalette::ColorGroup group = (opt.state & QStyle::State_Active) ? QPalette::Active
                                                                                : QPalette::Inactive;
        const QColor background = opt.palette.color(group, QPalette::Highlight);
        titleColor = opt.palette.color(QPalette::Active, QPalette::HighlightedText);
        if (contrastRatio(background, titleColor) < 3.0) {
            titleColor = contrastRatio(background, Qt::black) >= contrastRatio(background, Qt::white)
                             ? QColor(Qt::black) : QColor(Qt::white);
        }
        painter->setPen(Qt::NoPen);
        painter->setBrush(background);
        painter->drawRoundedRect(entry, FieldLook::radius, FieldLook::radius);
    }
    // Second line dimmed: half transparent, so it fits on any background (the same when selected,
    // else it looks like the title on a light selection)
    QColor secondColor = titleColor;
    secondColor.setAlphaF(0.6);

    // Icon on the left, centered; its room is kept even without icon (texts stay in line)
    const QSize iconSize = opt.decorationSize;
    const QRect iconRect(entry.left() + kPaddingH, entry.top() + (entry.height() - iconSize.height()) / 2,
                         iconSize.width(), iconSize.height());
    if (!opt.icon.isNull()) {
        opt.icon.paint(painter, iconRect, Qt::AlignCenter, selected ? QIcon::Selected : QIcon::Normal);
    }

    // Title and second line, together centered beside the icon; too long: "..."
    const QFont titleFont = opt.font;
    const QFont smallFont = secondLineFont(titleFont);
    const QFontMetrics titleMetrics(titleFont);
    const QFontMetrics smallMetrics(smallFont);
    const int textLeft = iconRect.right() + 1 + kIconGap;
    const int textWidth = qMax(0, entry.right() - kPaddingH - textLeft);
    const int textHeight = titleMetrics.height() + (second.isEmpty() ? 0 : smallMetrics.height());
    int y = entry.top() + (entry.height() - textHeight) / 2;

    painter->setFont(titleFont);
    painter->setPen(titleColor);
    painter->drawText(QRect(textLeft, y, textWidth, titleMetrics.height()), Qt::AlignLeft | Qt::AlignVCenter,
                      titleMetrics.elidedText(title, Qt::ElideRight, textWidth));
    if (!second.isEmpty()) {
        y += titleMetrics.height();
        painter->setFont(smallFont);
        painter->setPen(secondColor);
        painter->drawText(QRect(textLeft, y, textWidth, smallMetrics.height()), Qt::AlignLeft | Qt::AlignVCenter,
                          smallMetrics.elidedText(second, Qt::ElideRight, textWidth));
    }

    // Thin line between the entries, from the text start (no table look); none after the last
    // entry and none next to the selection, so the selected entry stands free
    const bool last = index.row() + 1 >= index.model()->rowCount(index.parent());
    bool nextSelected = false;
    if (!last) {
        if (const auto *view = qobject_cast<const QAbstractItemView *>(opt.widget)) {
            nextSelected = view->selectionModel()
                           && view->selectionModel()->isSelected(index.siblingAtRow(index.row() + 1));
        }
    }
    if (!last && !selected && !nextSelected) {
        QColor lineColor = opt.palette.color(QPalette::Text);
        lineColor.setAlphaF(0.14);   // fits any background, light and dark
        painter->setRenderHint(QPainter::Antialiasing, false);   // crisp on one pixel
        painter->setPen(QPen(lineColor, 0));
        painter->drawLine(textLeft, opt.rect.bottom(), entry.right() - kPaddingH, opt.rect.bottom());
    }

    painter->restore();
}

QSize TwoLineDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    const QString second = index.data(Qt::DisplayRole).toString().section('\n', 1);
    const int textHeight = QFontMetrics(opt.font).height()
                           + (second.isEmpty() ? 0 : QFontMetrics(secondLineFont(opt.font)).height());
    const int height = qMax(opt.decorationSize.height(), textHeight) + 2 * (kPaddingV + kMarginV);
    // The width does not matter: the list gives every entry its full width
    return QSize(opt.decorationSize.width() + kIconGap + 2 * (kPaddingH + kMarginH), height);
}
