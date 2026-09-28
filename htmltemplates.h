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

#ifndef HTMLTEMPLATES_H
#define HTMLTEMPLATES_H
#include <QString>

namespace AppTemplates {

// --- 1. Template für den Info-Dialog (About) ---
const QString INFO_HTML =
    R"(
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0//EN" "http://www.w3.org/TR/REC-html40/strict.dtd">
<html>
<head>
<meta charset="utf-8" />
</head>
<body>
<p class="version">Version: $APP_VERSION$</p>
<p>Copyright © $CURRENT_YEAR$ $DEVELOPER_NAME$</p>
<h3>$DEPENDENCIES$</h3>
<p>This product uses:</p>
<p><a href="https://www.qt.io/"><strong>Qt</strong></a><br />License: GNU Lesser General Public License, version 3 (<a href="https://www.gnu.org/licenses/lgpl-3.0.txt">LGPLv3</a>)<br />Source Code: The source code for Qt can be obtained from The Qt Company website.</p>
<div>
<p><a href="https://mediaarea.net/en/MediaInfo/" target="_blank"><strong>MediaInfoLib</strong></a><br />Copyright: Copyright (c) 2002-2023 MediaArea.net SARL.<br />License: <a href="https://www.freebsd.org/copyright/freebsd-license/">BSD 2-Clause License</a></p>
</div>
<div class="tmdb-credit" style="display: flex; align-items: center; margin-top: 15px;">
    <p>
        $APP_NAME$ retrieves movie and TV metadata using the service listed below:
    <br />
    <a href="https://www.themoviedb.org" target="_blank" style="margin-right: 15px;">
        <img src=":/images/tmdb.svg" alt="The Movie Database" height="20">
    </a>
    </p>
    <p style="margin: 0;">
        This product uses the TMDb API but is not endorsed or certified by TMDb.
    </p>
</div>
<h3>$LICENSE$</h3>
<p>Copyright (c) $CURRENT_YEAR$ $DEVELOPER_NAME$</p>
<p>This software, <strong>$APP_NAME$</strong>, is free software; you can redistribute it and/or modify it under the terms of the<br /><strong>GNU General Public License (GPL) version 3</strong> as published by the Free Software Foundation.</p>
<p>This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY.</p>
<p>For the full text and further details, please refer to the included file <strong>README</strong> (or <strong>LICENSE</strong>).<br />Or visit the official website: <a href="https://www.gnu.org/licenses/gpl-3.0.html" target="_blank">https://www.gnu.org/licenses/gpl-3.0.html</a></p>
</body>
</html>
)";

// --- 3. Template für eine Fehlermeldung (falls benötigt) ---
const QString ERROR_MESSAGE_HTML =
    R"(
    <h3 style="color:red;">Fehler ($ERROR_CODE$)</h3>
    <p>$ERROR_MESSAGE$</p>
    )";

} // namespace AppTemplates

#endif // HTMLTEMPLATES_H
