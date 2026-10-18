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

#include <QAbstractScrollArea>
#include <QAbstractSpinBox>
#include <QApplication>
#include <QColor>
#include <QComboBox>
#include <QLineEdit>
#include <QPalette>
#include <QStyledItemDelegate>
#include <QTabWidget>

// Colors are computed once, so a theme change (light/dark) needs a restart.

// Mix of two colors: amount = share of b (0 = a, 1 = b)
inline QColor mixColor(const QColor &a, const QColor &b, qreal amount)
{
    return QColor::fromRgbF(a.redF()   + (b.redF()   - a.redF())   * amount,
                            a.greenF() + (b.greenF() - a.greenF()) * amount,
                            a.blueF()  + (b.blueF()  - a.blueF())  * amount);
}

// Grey between window background and text color, for headers and tags.
// amount = share of the text color (0.35 looks like the selected tab on macOS).
inline QColor accentGrey(const QPalette &pal, qreal amount = 0.35)
{
    return mixColor(pal.color(QPalette::Window), pal.color(QPalette::WindowText), amount);
}

// Which corners of a field are rounded: an input field on top of a list
// (TagWidget) forms one box with the list.
enum class FieldCorners { All, Top, Bottom };

// Colors and shape shared by input fields and boxes (colors as in SearchField)
struct FieldLook {
    static constexpr int radius = 6;

    QColor fill;
    QColor text;
    QColor placeholder;
    QColor border;
    QColor focus;

    FieldLook()
    {
        const QPalette pal = QApplication::palette();
        fill        = pal.color(QPalette::Button);
        text        = pal.color(QPalette::ButtonText);
        placeholder = mixColor(fill, text, 0.50);
        border      = mixColor(fill, text, 0.30);
        focus       = pal.color(QPalette::Highlight);
    }

    // Style sheet rules for the corners (Top: no bottom radius, Bottom: no top border)
    static QString cornerRules(FieldCorners corners)
    {
        switch (corners) {
        case FieldCorners::Top:
            return QStringLiteral(" border-top-left-radius: %1px; border-top-right-radius: %1px;"
                                  " border-bottom-left-radius: 0; border-bottom-right-radius: 0;").arg(radius);
        case FieldCorners::Bottom:
            return QStringLiteral(" border-top: none; border-top-left-radius: 0; border-top-right-radius: 0;"
                                  " border-bottom-left-radius: %1px; border-bottom-right-radius: %1px;").arg(radius);
        default:
            return QStringLiteral(" border-radius: %1px;").arg(radius);
        }
    }
};

// Rounded input field in the colors of SearchField, but lower and less round
inline void applyInputStyle(QLineEdit *edit, FieldCorners corners = FieldCorners::All)
{
    constexpr int height = 26;
    const FieldLook look;

    edit->setFixedHeight(height);
    edit->setAttribute(Qt::WA_MacShowFocusRect, false);   // the border shows the focus
    edit->setStyleSheet(QStringLiteral(
        "QLineEdit { background: %1; color: %2; placeholder-text-color: %3; border: 1px solid %4;"
        "%5 padding: 0 6px; selection-background-color: %6; }"
        "QLineEdit:focus { border: 1px solid %6; }")
        .arg(look.fill.name(), look.text.name(), look.placeholder.name(), look.border.name(),
             FieldLook::cornerRules(corners), look.focus.name()));
}

// Rounded spin box (e.g. date) like the input fields. Without visible arrow buttons: with a
// style sheet macOS draws none; the value changes by typing, arrow keys and mouse wheel.
// The buttons get width 0 instead of NoButtons: with NoButtons and a style sheet the text
// field inside is only 1 px wide on macOS.
inline void applySpinStyle(QAbstractSpinBox *spin)
{
    constexpr int height = 26;
    const FieldLook look;

    spin->setFixedHeight(height);
    spin->setAttribute(Qt::WA_MacShowFocusRect, false);   // the border shows the focus
    spin->setStyleSheet(QStringLiteral(
        "QAbstractSpinBox { background: %1; color: %2; border: 1px solid %3;%4"
        " selection-background-color: %5; }"
        "QAbstractSpinBox:focus { border: 1px solid %5; }"
        "QAbstractSpinBox::up-button, QAbstractSpinBox::down-button { width: 0px; border: none; }")
        .arg(look.fill.name(), look.text.name(), look.border.name(),
             FieldLook::cornerRules(FieldCorners::All), look.focus.name()));

    // Distance of the text to the border, like in the input fields
    // (a padding in the style sheet is ignored here)
    if (QLineEdit *edit = spin->findChild<QLineEdit *>())
        edit->setTextMargins(6, 0, 6, 0);
}

// Rounded combo box like the input fields. The arrow is a grey drawing (":icons/chevron_down",
// from PhotoBoothM): visible in light and dark themes without tinting. The open list: entries
// selected in the accent color like the movie list (its outer corners stay square: it is a
// window of its own).
inline void applyComboStyle(QComboBox *combo)
{
    constexpr int height = 26;
    const FieldLook look;
    const QColor hover = mixColor(look.fill, look.text, 0.09);

    combo->setFixedHeight(height);
    combo->setAttribute(Qt::WA_MacShowFocusRect, false);   // the border shows the focus
    // Plain delegate for the open list: the default (menu like) delegate is drawn by the
    // style itself and ignores the item rules of the style sheet
    combo->setItemDelegate(new QStyledItemDelegate(combo));
    combo->setStyleSheet(QStringLiteral(
        "QComboBox { background: %1; color: %2; border: 1px solid %3;%4 padding: 0 24px 0 6px; }"
        "QComboBox:hover { background: %6; }"
        "QComboBox:focus { border: 1px solid %5; }"
        "QComboBox::drop-down { border: none; width: 22px; }"
        "QComboBox::down-arrow { image: url(:/icons/chevron_down); width: 14px; height: 14px; }"
        "QComboBox QAbstractItemView { background: %1; color: %2; border: 1px solid %3; padding: 2px; outline: 0; }"
        "QComboBox QAbstractItemView::item { min-height: 24px; padding: 0 6px; border-radius: %7px; }"
        "QComboBox QAbstractItemView::item:selected, QComboBox QAbstractItemView::item:hover"
        " { background: palette(highlight); color: palette(highlighted-text); border: none; }")
        .arg(look.fill.name(), look.text.name(), look.border.name(),
             FieldLook::cornerRules(FieldCorners::All), look.focus.name(), hover.name())
        .arg(FieldLook::radius));
}

// Rounded tab widget: the area in the field color like the other boxes (square only top left,
// where the first tab starts), tabs rounded on top. The active tab has the field color and
// flows into the area; the others are set back (window color, dimmed text), lighter on hover.
inline void applyTabStyle(QTabWidget *tabs)
{
    const FieldLook look;
    const QPalette pal = QApplication::palette();
    const QColor window = pal.color(QPalette::Window);
    const QColor inactive = mixColor(window, look.text, 0.06);
    const QColor hover = mixColor(window, look.text, 0.12);
    const QColor dimmed = mixColor(look.fill, look.text, 0.65);

    tabs->setStyleSheet(QStringLiteral(
        "QTabWidget::pane { background: %1; border: 1px solid %2; border-radius: %7px;"
        " border-top-left-radius: 0; top: -1px; }"
        "QTabBar::tab { background: %4; color: %6; border: 1px solid %2; border-bottom: none;"
        " border-top-left-radius: %7px; border-top-right-radius: %7px; padding: 4px 12px; margin-right: 2px; }"
        "QTabBar::tab:hover { background: %5; }"
        "QTabBar::tab:selected { background: %1; color: %3; padding-bottom: 5px; margin-bottom: -1px; }"   // covers the area's top line
        "QTabBar::tab:!selected { margin-top: 2px; }")
        .arg(look.fill.name(), look.border.name(), look.text.name(),
             inactive.name(), hover.name(), dimmed.name())
        .arg(FieldLook::radius));
}

// Rounded box (text area, list) in the colors of the input fields.
// showFocus: border in the accent color while focused (only where the user types).
// extraStyle: further rules of the widget, e.g. for the list items.
inline void applyBoxStyle(QAbstractScrollArea *area, bool showFocus,
                          FieldCorners corners = FieldCorners::All,
                          const QString &extraStyle = QString())
{
    const FieldLook look;

    QString style = QStringLiteral(
        "QAbstractScrollArea { background: %1; color: %2; border: 1px solid %3;"
        "%4 padding: 2px; selection-background-color: %5; }")
        .arg(look.fill.name(), look.text.name(), look.border.name(),
             FieldLook::cornerRules(corners), look.focus.name());
    if (showFocus)
        style += QStringLiteral("QAbstractScrollArea:focus { border: 1px solid %1; }").arg(look.focus.name());

    area->setAttribute(Qt::WA_MacShowFocusRect, false);
    area->setStyleSheet(style + extraStyle);
}
