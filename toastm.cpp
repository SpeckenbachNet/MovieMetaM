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
#include "toastm.h"

#include "themecolors.h"

#include <QFontMetrics>
#include <QGraphicsOpacityEffect>
#include <QPainter>
#include <QPropertyAnimation>

namespace {
constexpr int kPaddingX = 16;
constexpr int kPaddingY = 10;
constexpr int kFadeInMsec = 150;
constexpr int kFadeOutMsec = 250;
constexpr qreal kMaxWidthShare = 0.8;   // of the target widget
}

ToastM::ToastM(QWidget *window)
    : QWidget(window)
{
    setObjectName(QStringLiteral("toastM"));
    setAttribute(Qt::WA_TransparentForMouseEvents);
    hide();

    m_effect = new QGraphicsOpacityEffect(this);
    setGraphicsEffect(m_effect);
    m_fade = new QPropertyAnimation(m_effect, "opacity", this);

    m_timer.setSingleShot(true);
    connect(&m_timer, &QTimer::timeout, this, [this] {
        m_fade->stop();
        m_fade->setDuration(kFadeOutMsec);
        m_fade->setStartValue(m_effect->opacity());
        m_fade->setEndValue(0.0);
        m_fade->start();
    });
    connect(m_fade, &QPropertyAnimation::finished, this, [this] {
        if (m_effect->opacity() <= 0.01) hide();
    });
}

void ToastM::show(QWidget *target, const QString &text, Level level, int msec, Position position)
{
    if (!target || !target->window()) return;
    QWidget *window = target->window();
    ToastM *toast = window->findChild<ToastM *>(QStringLiteral("toastM"), Qt::FindDirectChildrenOnly);
    if (!toast) toast = new ToastM(window);
    toast->present(target, text, level, msec, position);
}

void ToastM::present(QWidget *target, const QString &text, Level level, int msec, Position position)
{
    m_text = text;
    m_level = level;
    if (msec <= 0) {
        msec = (level == Level::Error)
                   ? kErrorMsec
                   : qBound(kInfoMinMsec, kInfoBaseMsec + kInfoPerCharMsec * int(text.size()), kInfoMaxMsec);
    }

    // Size: the text wrapped to at most 80 % of the target
    const QFontMetrics fm(font());
    const int maxTextWidth = qMax(120, int(target->width() * kMaxWidthShare) - 2 * kPaddingX);
    const QRect textRect = fm.boundingRect(QRect(0, 0, maxTextWidth, 10000),
                                           Qt::AlignCenter | Qt::TextWordWrap, text);
    resize(textRect.width() + 2 * kPaddingX, textRect.height() + 2 * kPaddingY);

    // Place: in the target, kept inside the window
    const QPoint center = target->mapTo(target->window(), target->rect().center());
    const QRect area(target->mapTo(target->window(), QPoint(0, 0)), target->size());
    int x = center.x() - width() / 2;
    int y = center.y() - height() / 2;
    const int margin = 24;
    if (position == Position::Top) y = area.top() + margin;
    if (position == Position::Bottom) y = area.bottom() - height() - margin;
    x = qBound(0, x, qMax(0, target->window()->width() - width()));
    y = qBound(0, y, qMax(0, target->window()->height() - height()));
    move(x, y);

    // Show: fade in, fade out after the time
    m_timer.stop();
    m_fade->stop();
    if (!isVisible()) m_effect->setOpacity(0.0);
    QWidget::show();
    raise();
    update();
    m_fade->setDuration(kFadeInMsec);
    m_fade->setStartValue(m_effect->opacity());
    m_fade->setEndValue(1.0);
    m_fade->start();
    m_timer.start(msec);
}

void ToastM::paintEvent(QPaintEvent *)
{
    const FieldLook look;
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // A little away from the field color, so it stands out in light and dark themes
    const QColor fill = mixColor(look.fill, look.text, 0.12);
    const QColor border = (m_level == Level::Error) ? QColor(0xD9, 0x53, 0x4F) : look.border;
    p.setBrush(fill);
    p.setPen(QPen(border, m_level == Level::Error ? 2 : 1));
    p.drawRoundedRect(QRectF(rect()).adjusted(1, 1, -1, -1), FieldLook::radius, FieldLook::radius);

    p.setPen(look.text);
    p.drawText(rect().adjusted(kPaddingX, kPaddingY, -kPaddingX, -kPaddingY),
               Qt::AlignCenter | Qt::TextWordWrap, m_text);
}
