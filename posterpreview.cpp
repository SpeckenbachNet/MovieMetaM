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
#include "posterpreview.h"
#include "themecolors.h"

#include <QAbstractItemView>
#include <QHelpEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QScreen>

PosterPreview::PosterPreview(QAbstractItemView *view, int posterRole)
    : QWidget(view, Qt::ToolTip | Qt::FramelessWindowHint),
      m_view(view),
      m_posterRole(posterRole)
{
    setAttribute(Qt::WA_TranslucentBackground);   // rounded corners
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setFixedSize(kSize);
    view->viewport()->installEventFilter(this);
    view->viewport()->setMouseTracking(true);    // to notice moving to another entry
}

bool PosterPreview::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_view->viewport()) {
        switch (event->type()) {
        case QEvent::ToolTip: {
            // The mouse rests on the view (tooltip delay): show the poster of the entry
            auto *help = static_cast<QHelpEvent *>(event);
            showFor(m_view->indexAt(help->pos()), help->globalPos());
            return true;   // no normal tooltip
        }
        case QEvent::MouseMove:
            // Moved to another entry: hide, the next tooltip event shows the new poster
            if (isVisible() && m_view->indexAt(static_cast<QMouseEvent *>(event)->position().toPoint()) != m_index) {
                hide();
            }
            break;
        case QEvent::Leave:
        case QEvent::MouseButtonPress:
        case QEvent::Wheel:
            hide();
            break;
        default:
            break;
        }
    }
    return QWidget::eventFilter(watched, event);
}

void PosterPreview::showFor(const QModelIndex &index, const QPoint &globalPos)
{
    const QPixmap poster = index.isValid() ? index.data(m_posterRole).value<QPixmap>() : QPixmap();
    if (poster.isNull()) {
        hide();
        return;
    }
    m_poster = poster;
    m_index = index;

    // Right of the mouse, on the left if there is no room; kept inside the screen
    QPoint pos = globalPos + QPoint(kDistance, kDistance);
    if (const QScreen *screen = m_view->screen()) {
        const QRect area = screen->availableGeometry();
        if (pos.x() + width() > area.right()) {
            pos.setX(globalPos.x() - kDistance - width());
        }
        pos.setY(qBound(area.top(), pos.y(), area.bottom() - height()));
    }
    move(pos);
    show();
    update();
}

void PosterPreview::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    // Poster fitted into the window (2:3), rounded like the input fields, drawn from the full
    // resolution (sharp on scaled screens), thin border in the field colors
    const QSizeF fitted = QSizeF(m_poster.size()).scaled(QSizeF(rect().size()) - QSizeF(2, 2), Qt::KeepAspectRatio);
    const QRectF target((width() - fitted.width()) / 2, (height() - fitted.height()) / 2,
                        fitted.width(), fitted.height());
    QPainterPath rounded;
    rounded.addRoundedRect(target, FieldLook::radius, FieldLook::radius);

    painter.save();
    painter.setClipPath(rounded);
    painter.drawPixmap(target, m_poster, QRectF(m_poster.rect()));
    painter.restore();

    painter.setPen(QPen(FieldLook().border, 1));
    painter.setBrush(Qt::NoBrush);
    painter.drawPath(rounded);
}
