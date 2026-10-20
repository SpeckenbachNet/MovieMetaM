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

#include "themecolors.h"

#include <QApplication>
#include <QGuiApplication>
#include <QScreen>

namespace {

constexpr int kIconSize = 16;
constexpr int kBubbleMaxWidth = 300;
constexpr int kBubblePadding = 12;
constexpr int kBubbleGap = 6;   // to the icon

// The note as a small window of its own: rounded, in the colors of the input fields (a normal
// tooltip is square and cannot be styled the same on every system). Mouse events go through it.
class Bubble : public QWidget
{
public:
    Bubble() : QWidget(nullptr, Qt::ToolTip | Qt::FramelessWindowHint)
    {
        setAttribute(Qt::WA_TranslucentBackground);
        setAttribute(Qt::WA_ShowWithoutActivating);
        setAttribute(Qt::WA_TransparentForMouseEvents);
    }

    // Above the icon (the mouse stands on it and would cover a bubble below); below only if
    // there is no room above on the screen
    void showAt(const QString &text, QWidget *icon)
    {
        m_text = text;
        const QFontMetrics metrics(font());
        const QRect textRect = metrics.boundingRect(QRect(0, 0, kBubbleMaxWidth - 2 * kBubblePadding, 10000),
                                                    Qt::AlignLeft | Qt::TextWordWrap, text);
        resize(textRect.width() + 2 * kBubblePadding, textRect.height() + 2 * kBubblePadding);

        const QRect iconRect(icon->mapToGlobal(QPoint(0, 0)), icon->size());
        const QScreen *screen = icon->screen() ? icon->screen() : QGuiApplication::primaryScreen();
        const QRect available = screen->availableGeometry();
        int x = iconRect.center().x() - width() / 2;
        int y = iconRect.top() - height() - kBubbleGap;
        if (y < available.top()) y = iconRect.bottom() + kBubbleGap;
        x = qBound(available.left(), x, qMax(available.left(), available.right() - width()));
        move(x, y);
        show();
        raise();
        update();
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        const FieldLook look;
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(QPen(look.border, 1));
        painter.setBrush(mixColor(look.fill, look.text, 0.12));
        painter.drawRoundedRect(QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5), FieldLook::radius, FieldLook::radius);
        painter.setPen(look.text);
        painter.drawText(rect().adjusted(kBubblePadding, kBubblePadding, -kBubblePadding, -kBubblePadding),
                         Qt::AlignLeft | Qt::TextWordWrap, m_text);
    }

private:
    QString m_text;
};

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
    if (!m_bubble) m_bubble = new Bubble;
    static_cast<Bubble *>(m_bubble)->showAt(m_text, this);
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
