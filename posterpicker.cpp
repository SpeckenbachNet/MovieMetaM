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
#include "posterpicker.h"

#include "themecolors.h"
#include "twolinedelegate.h"

#include <QFrame>
#include <QGuiApplication>
#include <QScreen>
#include <QListView>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QVBoxLayout>

PosterPicker::PosterPicker(QWidget *parent)
    : QWidget(parent)
{
    setFixedSize(kPosterSize);

    m_popup = new QFrame(this, Qt::Popup);
    auto *layout = new QVBoxLayout(m_popup);
    layout->setContentsMargins(0, 0, 0, 0);
    m_list = new QListView(m_popup);
    m_list->setModel(&m_model);
    m_list->setIconSize(kThumbSize);
    m_list->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_list->setSelectionMode(QAbstractItemView::SingleSelection);
    m_list->setItemDelegate(new TwoLineDelegate(m_list));
    applyBoxStyle(m_list, false, FieldCorners::All, "QListView { outline: 0; }");
    layout->addWidget(m_list);
    // The separator lines depend on the neighbour's selection: repaint the whole list
    connect(m_list->selectionModel(), &QItemSelectionModel::selectionChanged,
            m_list->viewport(), qOverload<>(&QWidget::update));

    connect(m_list, &QListView::clicked, this, [this](const QModelIndex &index) {
        m_current = index.row();
        m_popup->hide();
        update();
        emit posterChosen(m_current);
    });
}

void PosterPicker::setEntries(const QStringList &texts, int current)
{
    m_model.clear();
    for (const QString &text : texts) {
        auto *item = new QStandardItem(text);
        item->setEditable(false);
        m_model.appendRow(item);
    }
    m_current = (current >= 0 && current < texts.size()) ? current : -1;
    if (m_current >= 0) m_list->setCurrentIndex(m_model.index(m_current, 0));
    setCursor(texts.size() > 1 ? Qt::PointingHandCursor : Qt::ArrowCursor);
    update();
}

void PosterPicker::setThumbnail(int row, const QPixmap &pixmap)
{
    QStandardItem *item = m_model.item(row);
    if (!item) return;
    // Scaled in the resolution of the screen, same picture when selected (Qt would tint it)
    const qreal dpr = devicePixelRatioF();
    QPixmap thumb = pixmap.scaled(kThumbSize * dpr, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    thumb.setDevicePixelRatio(dpr);
    QIcon icon;
    icon.addPixmap(thumb, QIcon::Normal);
    icon.addPixmap(thumb, QIcon::Selected);
    item->setIcon(icon);
}

void PosterPicker::setPoster(const QPixmap &pixmap)
{
    m_poster = pixmap;
    update();
}

void PosterPicker::clear()
{
    m_model.clear();
    m_poster = QPixmap();
    m_current = -1;
    m_popup->hide();
    setCursor(Qt::ArrowCursor);
    update();
}

void PosterPicker::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && count() > 1) openList();
}

void PosterPicker::openList()
{
    // About six entries high, wider than the poster
    const int rowHeight = m_list->sizeHintForRow(0) > 0 ? m_list->sizeHintForRow(0) : kThumbSize.height() + 30;
    const int extra = 2 * m_list->frameWidth() + 6;
    const int wantedHeight = qMin(count(), 6) * rowHeight + extra;
    const int popupWidth = qMax(width(), 260);

    // Below the poster; if that does not fit on the screen: above it, or smaller (scrolls)
    const QRect screen = (window()->screen() ? window()->screen() : QGuiApplication::primaryScreen())->availableGeometry();
    const QPoint below = mapToGlobal(QPoint(0, height() + 4));
    const QPoint above = mapToGlobal(QPoint(0, -4));
    const int spaceBelow = screen.bottom() - below.y();
    const int spaceAbove = above.y() - screen.top();

    int height = wantedHeight;
    int y = below.y();
    if (wantedHeight > spaceBelow) {
        if (spaceAbove > spaceBelow) {
            height = qMin(wantedHeight, spaceAbove);
            y = above.y() - height;
        } else {
            height = qMax(spaceBelow, qMin(wantedHeight, 2 * rowHeight + extra));
        }
    }
    const int x = qBound(screen.left(), below.x(), qMax(screen.left(), screen.right() - popupWidth));

    m_popup->resize(popupWidth, height);
    m_popup->move(x, y);
    if (m_current >= 0) m_list->scrollTo(m_model.index(m_current, 0));
    m_popup->show();
    m_list->setFocus();
}

void PosterPicker::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    const QRectF area = QRectF(rect()).adjusted(1, 1, -1, -1);   // room for the border line
    QPainterPath rounded;
    rounded.addRoundedRect(area, FieldLook::radius, FieldLook::radius);

    if (!m_poster.isNull()) {
        // Fill the area, cut off what sticks out, drawn from the full resolution (sharp)
        const QSizeF filled = QSizeF(m_poster.size()).scaled(area.size(), Qt::KeepAspectRatioByExpanding);
        const QRectF target(area.center().x() - filled.width() / 2, area.center().y() - filled.height() / 2,
                            filled.width(), filled.height());
        painter.save();
        painter.setClipPath(rounded);
        painter.drawPixmap(target, m_poster, QRectF(m_poster.rect()));
        painter.restore();
    } else {
        const FieldLook look;
        painter.setPen(QPen(look.border, 1));
        painter.setBrush(Qt::NoBrush);
        painter.drawPath(rounded);
    }

    // Badge "2 / 24": more posters to choose from (on the picture: fixed dark with white text)
    if (count() > 1 && m_current >= 0) {
        const QString text = QStringLiteral("%1 / %2  ▾").arg(m_current + 1).arg(count());
        const QFontMetrics metrics(font());
        const QRectF badge(0, 0, metrics.horizontalAdvance(text) + 14, metrics.height() + 6);
        const QRectF placed = badge.translated(area.right() - badge.width() - 6, area.bottom() - badge.height() - 6);
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(0, 0, 0, 165));
        painter.drawRoundedRect(placed, FieldLook::radius, FieldLook::radius);
        painter.setPen(Qt::white);
        painter.drawText(placed, Qt::AlignCenter, text);
    }
}
