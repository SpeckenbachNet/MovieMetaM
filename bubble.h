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

#include <QObject>
#include <QPoint>
#include <QRect>
#include <QString>
#include <QTimer>
#include <QWidget>

// A note as a small window of its own: rounded, in the colors of the input fields. The tooltips
// of the system are square and look different on every system. Mouse events go through it.
class Bubble : public QWidget
{
public:
    Bubble();

    // At the anchor (a rectangle in screen coordinates), centered; above it or below it, the
    // other side if there is no room on the screen. The text is wrapped (about 300 px wide).
    void showAt(const QString &text, const QRect &anchor, bool preferAbove);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QString m_text;
};

// All tooltips of the application as a Bubble: takes the tooltip request of every widget (and of
// the entries of the item views) instead of the tooltip of the system. Install once.
class TooltipFilter : public QObject
{
public:
    static void install();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    explicit TooltipFilter(QObject *parent);

    Bubble m_bubble;
    QTimer m_timer;     // a tooltip does not stay for ever
    QPoint m_origin;    // where the mouse was when it appeared
};
