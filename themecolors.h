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

#include <QColor>
#include <QPalette>

// Grey between window background and text color, for headers and tags.
// amount = share of the text color (0.35 looks like the selected tab on macOS).
// Works in light and dark themes; computed once, so a theme change needs a restart.
inline QColor accentGrey(const QPalette &pal, qreal amount = 0.35)
{
    const QColor bg = pal.color(QPalette::Window);
    const QColor fg = pal.color(QPalette::WindowText);
    return QColor::fromRgbF(bg.redF()   + (fg.redF()   - bg.redF())   * amount,
                            bg.greenF() + (fg.greenF() - bg.greenF()) * amount,
                            bg.blueF()  + (fg.blueF()  - bg.blueF())  * amount);
}
