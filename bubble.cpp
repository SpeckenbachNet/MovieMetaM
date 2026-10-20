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
#include "bubble.h"
#include "themecolors.h"

#include <QAbstractItemView>
#include <QApplication>
#include <QEvent>
#include <QGuiApplication>
#include <QHelpEvent>
#include <QPainter>
#include <QScreen>
#include <QTextDocument>

namespace {
constexpr int kMaxWidth = 300;
constexpr int kPadding = 12;
constexpr int kGap = 6;           // to the anchor
constexpr int kMoveToHide = 24;   // the mouse moves this far from the place of the tooltip: it goes
constexpr int kVisibleMsec = 12000;
}

Bubble::Bubble()
    : QWidget(nullptr, Qt::ToolTip | Qt::FramelessWindowHint)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowWithoutActivating);
    setAttribute(Qt::WA_TransparentForMouseEvents);
}

void Bubble::showAt(const QString &text, const QRect &anchor, bool preferAbove)
{
    m_text = text;
    const QFontMetrics metrics(font());
    const QRect textRect = metrics.boundingRect(QRect(0, 0, kMaxWidth - 2 * kPadding, 10000),
                                                Qt::AlignLeft | Qt::TextWordWrap, text);
    resize(textRect.width() + 2 * kPadding, textRect.height() + 2 * kPadding);

    const QScreen *screen = QGuiApplication::screenAt(anchor.center());
    if (!screen) screen = QGuiApplication::primaryScreen();
    const QRect available = screen->availableGeometry();

    const int above = anchor.top() - height() - kGap;
    const int below = anchor.bottom() + kGap;
    int y = preferAbove ? above : below;
    if (preferAbove && y < available.top()) y = below;
    if (!preferAbove && y + height() > available.bottom()) y = above;
    int x = anchor.center().x() - width() / 2;
    x = qBound(available.left(), x, qMax(available.left(), available.right() - width()));
    move(x, y);
    show();
    raise();
    update();
}

void Bubble::paintEvent(QPaintEvent *)
{
    const FieldLook look;
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(look.border, 1));
    painter.setBrush(mixColor(look.fill, look.text, 0.12));
    painter.drawRoundedRect(QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5), FieldLook::radius, FieldLook::radius);
    painter.setPen(look.text);
    painter.drawText(rect().adjusted(kPadding, kPadding, -kPadding, -kPadding),
                     Qt::AlignLeft | Qt::TextWordWrap, m_text);
}

TooltipFilter::TooltipFilter(QObject *parent)
    : QObject(parent)
{
    m_timer.setSingleShot(true);
    m_timer.setInterval(kVisibleMsec);
    connect(&m_timer, &QTimer::timeout, &m_bubble, &QWidget::hide);
}

void TooltipFilter::install()
{
    qApp->installEventFilter(new TooltipFilter(qApp));
}

bool TooltipFilter::eventFilter(QObject *watched, QEvent *event)
{
    switch (event->type()) {
    case QEvent::ToolTip: {
        auto *widget = qobject_cast<QWidget *>(watched);
        if (!widget || widget == &m_bubble) return false;
        const auto *help = static_cast<QHelpEvent *>(event);

        // The item views ask the entry under the mouse, the other widgets their own tooltip
        QString text;
        const auto *view = qobject_cast<QAbstractItemView *>(widget->parent());
        if (view && view->viewport() == widget) {
            text = view->indexAt(help->pos()).data(Qt::ToolTipRole).toString();
        } else {
            text = widget->toolTip();
        }
        if (text.isEmpty()) return false;   // the parent may have one
        if (Qt::mightBeRichText(text)) {
            QTextDocument document;
            document.setHtml(text);
            text = document.toPlainText();
        }

        m_origin = help->globalPos();
        m_bubble.showAt(text, QRect(m_origin, QSize(1, 22)), false);   // below the pointer
        m_timer.start();
        return true;
    }
    case QEvent::MouseMove:
        // The tooltip goes when the mouse has moved away from it
        if (m_bubble.isVisible()
            && (static_cast<QMouseEvent *>(event)->globalPosition().toPoint() - m_origin).manhattanLength() > kMoveToHide) {
            m_bubble.hide();
        }
        return false;
    case QEvent::Leave:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::KeyPress:
    case QEvent::Wheel:
    case QEvent::WindowDeactivate:
    case QEvent::FocusOut:
        if (m_bubble.isVisible()) m_bubble.hide();
        return false;
    default:
        return false;
    }
}
