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
#include "infoicon.h"

#include <QEvent>
#include <QIcon>
#include <QMouseEvent>
#include <QPainter>

#include "bubble.h"

namespace {

constexpr int kIconSize = 16;

} // namespace

InfoIcon::InfoIcon(const QString &text, QWidget *parent)
    : QLabel(parent)
{
    m_text = text;
    setCursor(Qt::PointingHandCursor);
    setFixedSize(kIconSize + 4, kIconSize + 4);
    setAlignment(Qt::AlignCenter);
    updatePixmap();
}

QString InfoIcon::savingNote()
{
    return tr("The data is written directly into the movie file. If saving is interrupted (crash, "
              "power failure, lost network connection), the file can be damaged. \"Safe saving\" "
              "writes into a copy first, needs more space and is slow for big files and on network drives.");
}

void InfoIcon::updatePixmap()
{
    // Black drawing, in the resolution of the screen, tinted in the text color
    const qreal dpr = devicePixelRatioF();
    QPixmap pixmap = QIcon(QStringLiteral(":/icons/info")).pixmap(QSize(kIconSize, kIconSize), dpr);
    QPainter painter(&pixmap);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.fillRect(pixmap.rect(), palette().color(QPalette::Text));
    painter.end();
    setPixmap(pixmap);
}

void InfoIcon::showBubble()
{
    // Above the icon: the mouse stands on it and would cover a bubble below
    if (!m_bubble) m_bubble = new Bubble;
    m_bubble->showAt(m_text, QRect(mapToGlobal(QPoint(0, 0)), size()), true);
}

void InfoIcon::hideBubble()
{
    if (m_bubble) m_bubble->hide();
}

void InfoIcon::enterEvent(QEnterEvent *event)
{
    showBubble();
    QLabel::enterEvent(event);
}

void InfoIcon::leaveEvent(QEvent *event)
{
    hideBubble();
    QLabel::leaveEvent(event);
}

void InfoIcon::mousePressEvent(QMouseEvent *)
{
    showBubble();   // also by click: not only to find with the mouse resting on it
}

void InfoIcon::hideEvent(QHideEvent *event)
{
    hideBubble();
    QLabel::hideEvent(event);
}

void InfoIcon::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::PaletteChange) updatePixmap();
    QLabel::changeEvent(event);
}

InfoIcon::~InfoIcon()
{
    delete m_bubble;
}
