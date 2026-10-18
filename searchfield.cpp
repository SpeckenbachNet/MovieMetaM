/*
 * Copyright (C) 2026 Michael Speckenbach
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
// SearchField 1.2.0 – master copy: ~/Development/QT/libraries/searchfield (see CHANGELOG.md there)
#include "searchfield.h"

#include <QAction>
#include <QApplication>
#include <QEvent>
#include <QPainter>

namespace {

// Same geometry and blend factors as the ToolbarM buttons
constexpr int   kHeight = 32;
constexpr qreal kBorder = 0.30;
constexpr qreal kHover  = 0.09;

QColor mix(const QColor &a, const QColor &b, qreal t)
{
    return QColor::fromRgbF(a.redF()   * (1.0 - t) + b.redF()   * t,
                            a.greenF() * (1.0 - t) + b.greenF() * t,
                            a.blueF()  * (1.0 - t) + b.blueF()  * t);
}

QIcon tinted(const QString &path, const QColor &color)
{
    QPixmap pm = QIcon(path).pixmap(48, 48);
    QPainter p(&pm);
    p.setCompositionMode(QPainter::CompositionMode_SourceIn);
    p.fillRect(pm.rect(), color);
    return QIcon(pm);
}

// Clear button: filled circle with a cut-out x (the field color shows through)
QIcon clearIcon(const QColor &color)
{
    constexpr int size = 48;
    QPixmap pm(size, size);
    pm.fill(Qt::transparent);
    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    p.setBrush(color);
    p.drawEllipse(QRectF(4, 4, size - 8, size - 8));

    p.setCompositionMode(QPainter::CompositionMode_Clear);
    p.setPen(QPen(Qt::black, 4.5, Qt::SolidLine, Qt::RoundCap));
    const qreal a = size * 0.36, b = size * 0.64;
    p.drawLine(QPointF(a, a), QPointF(b, b));
    p.drawLine(QPointF(a, b), QPointF(b, a));
    return QIcon(pm);
}

} // namespace

SearchField::SearchField(QWidget *parent)
    : QLineEdit(parent)
{
    setPlaceholderText(tr("Search..."));
    setClearButtonEnabled(true);
    setFixedHeight(kHeight);
    setAttribute(Qt::WA_MacShowFocusRect, false);   // the border shows the focus
    m_searchAction = addAction(QIcon(), QLineEdit::LeadingPosition);
    m_searchAction->setToolTip(tr("Search"));
    updateLook();

    // The magnifier is a button: Enter and a click on it ask for a search
    connect(m_searchAction, &QAction::triggered, this, [this] { emit searchRequested(text()); });
    connect(this, &QLineEdit::returnPressed, this, [this] { emit searchRequested(text()); });
}

void SearchField::changeEvent(QEvent *event)
{
    QLineEdit::changeEvent(event);
    if (!m_updating && (event->type() == QEvent::ApplicationPaletteChange
                        || event->type() == QEvent::PaletteChange)) {
        updateLook();
    }
}

void SearchField::updateLook()
{
    m_updating = true;
    // Application palette: our own style sheet changes the widget palette
    const QPalette pal = QApplication::palette();
    const QColor fill   = pal.color(QPalette::Button);
    const QColor text   = pal.color(QPalette::ButtonText);
    const QColor border = mix(fill, text, kBorder);
    const QColor hover  = mix(fill, text, kHover);
    const QColor focus  = pal.color(QPalette::Highlight);

    setStyleSheet(QStringLiteral(
        "QLineEdit { background: %1; color: %2; border: 1px solid %3; border-radius: %4px;"
        " padding: 0 10px 0 4px; selection-background-color: %5; }"
        "QLineEdit:hover { background: %6; }"
        "QLineEdit:focus { border: 1px solid %5; }")
        .arg(fill.name(), text.name(), border.name())
        .arg(m_cornerRadius < 0 ? kHeight / 2 : qMin(m_cornerRadius, kHeight / 2))
        .arg(focus.name(), hover.name()));

    const QColor iconColor = mix(fill, text, 0.6);
    m_searchAction->setIcon(tinted(QStringLiteral(":icons/search"), iconColor));
    // The clear button of QLineEdit is an internal action with this object name
    if (QAction *clear = findChild<QAction *>(QStringLiteral("_q_qlineeditclearaction")))
        clear->setIcon(clearIcon(iconColor));
    m_updating = false;
}

void SearchField::setCornerRadius(int radius)
{
    m_cornerRadius = radius;
    updateLook();
}
