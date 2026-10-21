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
#include "imagelabelm.h"
#include "themecolors.h"

#include <QApplication>
#include <QIcon>
#include <QPainter>
#include <QPainterPath>

ImageLabelM::ImageLabelM(QWidget *parent) : QLabel(parent) {
    // Important, so that the label reacts to drops at all
    setAcceptDrops(true);
    // Selected = focused (accent frame); a click focuses, Tab reaches it too
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);   // the × shows on hover
    // A click outside, also on an empty area that takes no focus, unselects (like the tags)
    qApp->installEventFilter(this);
}

QRectF ImageLabelM::crossRect() const {
    const qreal size = 22.0;
    return QRectF(width() - size - 8.0, 8.0, size, size);
}

bool ImageLabelM::eventFilter(QObject *watched, QEvent *event) {
    if (event->type() == QEvent::MouseButtonPress && hasFocus() && watched != this) {
        auto *widget = qobject_cast<QWidget *>(watched);
        if (widget && !isAncestorOf(widget)) clearFocus();
    }
    return QLabel::eventFilter(watched, event);
}

void ImageLabelM::keyPressEvent(QKeyEvent *event) {
    if (!m_cover.isNull() && (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace)) {
        emit coverRemoveRequested();
    } else if (event->key() == Qt::Key_Escape) {
        clearFocus();
    } else {
        QLabel::keyPressEvent(event);
    }
}

void ImageLabelM::focusInEvent(QFocusEvent *event) {
    QLabel::focusInEvent(event);
    update();
}

void ImageLabelM::focusOutEvent(QFocusEvent *event) {
    QLabel::focusOutEvent(event);
    update();
}

void ImageLabelM::enterEvent(QEnterEvent *event) {
    QLabel::enterEvent(event);
    m_hover = true;
    update();
}

void ImageLabelM::leaveEvent(QEvent *event) {
    QLabel::leaveEvent(event);
    m_hover = false;
    m_overCross = false;
    setToolTip(QString());
    update();
}

void ImageLabelM::setCover(const QPixmap &cover) {
    m_cover = cover;
    update();
}

void ImageLabelM::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    const QRectF area = QRectF(rect()).adjusted(1, 1, -1, -1);   // room for the border line
    const qreal radius = FieldLook::radius;
    QPainterPath rounded;
    rounded.addRoundedRect(area, radius, radius);

    if (!m_cover.isNull()) {
        // Fill the area and cut off what sticks out (no distortion, no empty stripes);
        // drawn from the full resolution, so it stays sharp on scaled screens
        const QSizeF filled = QSizeF(m_cover.size()).scaled(area.size(), Qt::KeepAspectRatioByExpanding);
        const QRectF target(area.center().x() - filled.width() / 2, area.center().y() - filled.height() / 2,
                            filled.width(), filled.height());
        painter.save();
        painter.setClipPath(rounded);
        painter.drawPixmap(target, m_cover, QRectF(m_cover.rect()));
        painter.restore();
    } else {
        // Placeholder, rendered in the resolution of the screen, fitted into the area
        const QSize placeholder = QSize(area.width(), area.height()) - QSize(16, 16);
        const QPixmap pix = QIcon(QStringLiteral(":/images/noimage.svg")).pixmap(placeholder, devicePixelRatioF());
        const QSizeF shown = QSizeF(pix.size()) / pix.devicePixelRatio();
        painter.drawPixmap(QPointF(area.center().x() - shown.width() / 2, area.center().y() - shown.height() / 2), pix);

        // Dashed border: here an image can be dropped
        QPen dashed(palette().color(QPalette::Mid), 2, Qt::DashLine);
        painter.setPen(dashed);
        painter.setBrush(Qt::NoBrush);
        painter.drawPath(rounded);
    }

    if (hasFocus() && !m_cover.isNull() && !m_dragActive) {
        // Selected (Delete removes it): frame in the accent color
        painter.setPen(QPen(palette().color(QPalette::Highlight), 2));
        painter.setBrush(Qt::NoBrush);
        painter.drawPath(rounded);
    }

    if (m_hover && !m_cover.isNull() && !m_dragActive) {
        // Round button over the picture (any colors underneath): dark disc, white ×
        const QRectF disc = crossRect();
        painter.setPen(Qt::NoPen);
        painter.setBrush(m_overCross ? palette().color(QPalette::Highlight) : QColor(0, 0, 0, 150));
        painter.drawEllipse(disc);
        painter.setPen(QPen(m_overCross ? palette().color(QPalette::HighlightedText) : QColor(Qt::white),
                            1.5, Qt::SolidLine, Qt::RoundCap));
        const QRectF cross = QRectF(0, 0, 7, 7).translated(disc.center() - QPointF(3.5, 3.5));
        painter.drawLine(cross.topLeft(), cross.bottomRight());
        painter.drawLine(cross.topRight(), cross.bottomLeft());
    }

    if (m_dragActive) {
        // An image is dragged over the label: accent color, background slightly tinted
        QColor accent = palette().color(QPalette::Highlight);
        QColor tint = accent;
        tint.setAlphaF(0.1);
        painter.setPen(QPen(accent, 2));
        painter.setBrush(tint);
        painter.drawPath(rounded);
    }
}

void ImageLabelM::readDragData(const QMimeData *mime) {
    clearDragData();

    // First link only: a browser adds a description line to the uri-list for links
    const QList<QUrl> urls = mime->urls();
    if (!urls.isEmpty()) {
        m_dropUrl = urls.first();
    }

    // Image data: image/png, image/jpeg ... or as file data, e.g. Brave:
    // application/octet-stream;name="picture.jpg"
    const QStringList formats = mime->formats();
    for (const QString &format : formats) {
        if (format.startsWith("image/") || (format.startsWith("application/octet-stream") && format.contains("name="))) {
            m_dropImage = mime->data(format);
            if (!m_dropImage.isEmpty()) {
                break;
            }
        }
    }
}

void ImageLabelM::clearDragData() {
    m_dropUrl = QUrl();
    m_dropImage.clear();
}

void ImageLabelM::dragEnterEvent(QDragEnterEvent *event) {
    // Read the data now (see m_dropUrl); accept files, links and image data
    readDragData(event->mimeData());
    if (m_dropUrl.isValid() || !m_dropImage.isEmpty()) {
        event->acceptProposedAction();
        m_dragActive = true;
        update();
    }
}

void ImageLabelM::dragLeaveEvent(QDragLeaveEvent *event) {
    Q_UNUSED(event);
    // User moved the mouse away again
    m_dragActive = false;
    clearDragData();
    update();
}

void ImageLabelM::dragMoveEvent(QDragMoveEvent *event) {
    // Keep telling the source that we take the data: on Wayland a browser (Chromium/Brave)
    // otherwise delivers empty data on the drop (file managers are less strict)
    if (m_dropUrl.isValid() || !m_dropImage.isEmpty()) {
        event->acceptProposedAction();
    }
}

void ImageLabelM::dropEvent(QDropEvent *event) {
    m_dragActive = false;
    update();

    // Image data from the drag first (a picture dragged out of a browser brings its file),
    // then a local file, then a web link to download. Uses the data read on entering.
    QPixmap pix;
    if (!m_dropImage.isEmpty()) {
        pix.loadFromData(m_dropImage);
    }
    if (pix.isNull() && m_dropUrl.isLocalFile()) {
        pix.load(m_dropUrl.toLocalFile());
    }

    if (!pix.isNull()) {
        emit coverDropped(pix);
    } else if (m_dropUrl.scheme() == "http" || m_dropUrl.scheme() == "https") {
        emit coverUrlDropped(m_dropUrl);
    } else {
        emit coverDropFailed();
    }

    clearDragData();
    event->acceptProposedAction();
}

void ImageLabelM::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragStartPos = event->position().toPoint();
        m_pressedOnCross = !m_cover.isNull() && crossRect().contains(event->position());
    }
    QLabel::mousePressEvent(event);
}

void ImageLabelM::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_pressedOnCross) {
        m_pressedOnCross = false;
        if (crossRect().contains(event->position())) emit coverRemoveRequested();
        return;
    }
    QLabel::mouseReleaseEvent(event);
}

void ImageLabelM::mouseMoveEvent(QMouseEvent *event)
{
    // Over the ×: highlight it and name it
    const bool overCross = !m_cover.isNull() && crossRect().contains(event->position());
    if (overCross != m_overCross) {
        m_overCross = overCross;
        setToolTip(overCross ? tr("Remove cover") : QString());
        update();
    }

    if (!(event->buttons() & Qt::LeftButton) || m_pressedOnCross) return;

    // Only after the usual distance: a click with a slightly moving hand starts no drag
    if ((event->position().toPoint() - m_dragStartPos).manhattanLength() < QApplication::startDragDistance()) {
        return;
    }

    // Placeholder: nothing to drag out
    if (m_cover.isNull()) {
        return;
    }

    // The cover in full resolution as a file
    QString tempPath = QDir::tempPath() + "/exported_cover.jpg";
    if (!m_cover.save(tempPath, "JPG", 95)) {  // JPG quality 95%
        qDebug() << "Could not save file to " << tempPath;
        return;
    }

    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;
    mimeData->setUrls({QUrl::fromLocalFile(tempPath)});
    drag->setMimeData(mimeData);

    drag->setPixmap(m_cover.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    drag->setHotSpot(QPoint(50, 50));

    drag->exec(Qt::CopyAction);
}
