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

#include <QTimer>
#include <QWidget>

class QGraphicsOpacityEffect;
class QPropertyAnimation;

// Short message that fades in over the content and fades out again (instead of a status bar).
// One toast per window; a new message replaces the current one. Mouse clicks go through it.
//   ToastM::show(column, tr("No movies found"));
//   ToastM::show(this, tr("Could not save!"), ToastM::Level::Error);
class ToastM : public QWidget
{
    Q_OBJECT
public:
    enum class Level { Info, Error };                 // Error: red border, stays longer
    enum class Position { Center, Top, Bottom };      // inside the target widget

    // Info: the time follows the text (a short "Saved" goes quickly): base + per character,
    // within the limits; Error: fixed
    static constexpr int kInfoBaseMsec = 1000;
    static constexpr int kInfoPerCharMsec = 35;
    static constexpr int kInfoMinMsec = 1500;
    static constexpr int kInfoMaxMsec = 4000;
    static constexpr int kErrorMsec = 5000;

    // target: the toast appears over this widget (a column, or the main window).
    // msec = 0: the default of the level (Info: by the length of the text).
    static void show(QWidget *target, const QString &text, Level level = Level::Info,
                     int msec = 0, Position position = Position::Center);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    explicit ToastM(QWidget *window);
    void present(QWidget *target, const QString &text, Level level, int msec, Position position);

    QString m_text;
    Level m_level = Level::Info;
    QTimer m_timer;
    QGraphicsOpacityEffect *m_effect = nullptr;
    QPropertyAnimation *m_fade = nullptr;
};
