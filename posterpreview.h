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

#include <QPersistentModelIndex>
#include <QPixmap>
#include <QWidget>

class QAbstractItemView;

// Enlarged poster beside the mouse: shown when the mouse rests on an entry of the view (like a
// tooltip), hidden on leaving, scrolling or clicking. The poster in full resolution comes from
// the given role of the entry; entries without one show nothing.
class PosterPreview : public QWidget
{
public:
    PosterPreview(QAbstractItemView *view, int posterRole);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    void showFor(const QModelIndex &index, const QPoint &globalPos);

    static constexpr QSize kSize{200, 300};   // logical pixels, poster 2:3
    static constexpr int kDistance = 16;      // from the mouse

    QAbstractItemView *m_view;
    int m_posterRole;
    QPixmap m_poster;
    QPersistentModelIndex m_index;   // entry whose poster is shown
};
