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

#include <QStyledItemDelegate>

// Entry of the movie list and the search results: icon on the left (status symbol, poster),
// beside it the title and below a smaller, dimmed second line (file name, year). The model
// text is "title\nsecond line". Rounded selection in the accent color, some space around.
class TwoLineDelegate : public QStyledItemDelegate
{
public:
    explicit TwoLineDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    static constexpr int kMarginH = 2;   // to the border of the box
    static constexpr int kMarginV = 1;   // half of the gap between two entries
    static constexpr int kPaddingH = 6;  // inside the entry
    static constexpr int kPaddingV = 7;   // the line box of the font has more room on top than below
    static constexpr int kIconGap = 8;   // between icon and text
};
