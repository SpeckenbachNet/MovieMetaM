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

#include <QPixmap>
#include <QStandardItemModel>
#include <QWidget>

class QFrame;
class QListView;

// The poster of a search result, large; with several posters a click opens a list below it
// (thumbnail, language, size, rating) to choose another one. The class only shows and reports
// the choice; the images come from outside (setThumbnail, setPoster).
class PosterPicker : public QWidget
{
    Q_OBJECT
public:
    static constexpr QSize kPosterSize{150, 225};   // 2:3
    static constexpr QSize kThumbSize{40, 60};

    explicit PosterPicker(QWidget *parent = nullptr);

    // One text per poster: "title\nsecond line" like the other lists; current: the shown one
    void setEntries(const QStringList &texts, int current);
    void setThumbnail(int row, const QPixmap &pixmap);   // full resolution
    void setPoster(const QPixmap &pixmap);               // the large one, full resolution
    int current() const { return m_current; }
    int count() const { return m_model.rowCount(); }
    void clear();

signals:
    void posterChosen(int row);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    void openList();

    QStandardItemModel m_model;
    QFrame *m_popup = nullptr;      // window of its own (closes on a click beside it)
    QListView *m_list = nullptr;
    QPixmap m_poster;
    int m_current = -1;
};
