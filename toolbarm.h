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
#pragma once

#include <QColor>
#include <QFrame>
#include <QHBoxLayout>
#include <QIcon>
#include <QPixmap>
#include <QSize>
#include <QString>
#include <QToolButton>

class QLabel;

// ToolbarM 1.5.0 – master copy: ~/Development/QT/libraries/toolbarm (see CHANGELOG.md there).
// Projects keep a copy of toolbarm.h/.cpp: change the master copy first, then copy it over.
inline constexpr const char *ToolbarMVersion = "1.5.0";

// Toolbar with rounded buttons, painted with QPainter in the colors of the palette
// (light and dark themes); icons are black drawings tinted in the text color.
class ToolbarM : public QFrame {
    Q_OBJECT

public:
    // Blend factors towards the text color (0.0 = base color, 1.0 = text color).
    // Central place to tune contrast of toolbar and buttons.
    struct ShadeFactors {
        qreal toolbarBackground = 0.00;  // toolbar fill
        qreal toolbarDarkEdge   = 0.20;  // top/left edge
        qreal toolbarLightEdge  = 0.10;  // bottom/right edge
        qreal buttonHover       = 0.09;
        qreal buttonPressed     = 0.18;
        qreal buttonBorder      = 0.30;
        qreal buttonBorderOff   = 0.18;  // border of disabled buttons
        qreal separator         = 0.40;
    };

    explicit ToolbarM(QWidget *parent = nullptr);

    void setShadeFactors(const ShadeFactors &factors);
    ShadeFactors shadeFactors() const { return shade; }

    // Corner radius of the buttons and of the toolbar frame (default 15 / 18).
    // Limited to half the height, so a large value gives a pill shape.
    void setButtonRadius(qreal radius);
    void setToolbarRadius(qreal radius);

    // One button function for all variants (text only, icon only, text + icon).
    // NOTE: Parameter order matches the implementation (useSeparator before iconRight).
    QToolButton* addButton(const QString &objName,
                           const QString &text,
                           const QString &iconPath,
                           bool useSeparator = false,
                           bool iconRight = false);
    void addSpacer();

    // Own color for one button (e.g. an orange "Donate" button) instead of the palette color.
    // Without text color, text and icon get black or white, whichever is easier to read on fill.
    // Hover, pressed, border and separator are derived from these colors; disabled stays grey.
    // Invalid fill with a text color: only icon and text in that color, the button itself like
    // the others. Both invalid: the palette colors again.
    void setButtonColor(QToolButton *btn, const QColor &fill, const QColor &text = QColor());

    // Bold title in the middle of the toolbar (empty: no title). Lies over the toolbar,
    // so it stays exactly centered whatever buttons are left and right.
    void setTitle(const QString &title);

protected:
    void changeEvent(QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    friend class ToolbarButton;   // asks buttonSizeHint() for its size

    void updateColors();
    void paintButton(QToolButton *btn);
    QSize buttonSizeHint(const QToolButton *btn) const;   // same values as paintButton()
    QPixmap tintedPixmap(QToolButton *btn, const QColor &color) const;   // cached on the button
    void placeTitle();

    // Blends a towards b (t = 0.0 -> a, t = 1.0 -> b).
    // Used to derive borders/hover colors with guaranteed contrast in light AND dark themes.
    static QColor mix(const QColor &a, const QColor &b, qreal t);

    QHBoxLayout *mainLayout = nullptr;
    QLabel      *titleLabel = nullptr;

    // Palette colors (refreshed on palette/style change)
    QColor windowColor;
    QColor textColor;
    QColor buttonColor;
    QColor buttonTextColor;
    QColor disabledColor;
    QColor disabledTextColor;

    ShadeFactors shade;

    // Geometry
    int   buttonHeight      = 32;
    QSize iconSize{22, 22};
    int   iconSpacing       = 6;   // gap between icon and text
    int   separatorSpace    = 14;  // room for the separator line
    int   horizontalPadding = 14;  // left/right padding inside a button
    qreal buttonRadius      = 15.0;
    qreal toolbarRadius     = 18.0;
};