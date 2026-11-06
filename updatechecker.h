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

#include <QObject>
#include <QUrl>

class QNetworkAccessManager;

// Is there a newer version? Asks GitHub for the latest release (at most once a day, setting
// "General/LastUpdateCheck") and compares its tag ("v1.0.1") with APP_VERSION. A newer version
// found is remembered ("General/LatestVersion"), so it is shown again on the next start of the
// day without a new request. No answer, no network: nothing happens.
// Setting "General/CheckUpdates" (on by default): off = no request, nothing shown.
class UpdateChecker : public QObject
{
    Q_OBJECT

public:
    explicit UpdateChecker(QObject *parent = nullptr);

    void check();                  // at the start: the remembered version, then GitHub if due
    static bool isEnabled();       // setting "General/CheckUpdates"
    static QUrl downloadUrl();     // the package of this system in the latest release

signals:
    void updateAvailable(const QString &version);   // e.g. "1.0.1"

private:
    void request();
    void announceIfNewer(const QString &version);

    QNetworkAccessManager *m_network = nullptr;
};
