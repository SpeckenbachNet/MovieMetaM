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

#include <QLabel>

class Bubble;

// A small "i" icon (tinted in the text color) that shows a longer text in a bubble above it on
// hover and on a click. For notes that should not take room in a dialog.
class InfoIcon : public QLabel
{
    Q_OBJECT
public:
    explicit InfoIcon(const QString &text, QWidget *parent = nullptr);
    ~InfoIcon() override;

    // The note about saving directly into the movie files (setup dialog and settings)
    static QString savingNote();

protected:
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void hideEvent(QHideEvent *event) override;
    void changeEvent(QEvent *event) override;

private:
    void updatePixmap();   // again on a change of the theme
    void showBubble();
    void hideBubble();

    QString m_text;
    Bubble *m_bubble = nullptr;   // window of its own, created at the first use
};
