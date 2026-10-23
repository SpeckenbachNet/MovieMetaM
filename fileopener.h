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
#include <QStringList>
#include <QTimer>

class QLocalServer;

// Movie files that reach the app from outside: "Open with", dropped on the program icon, or
// given on the command line.
//  - Linux, Windows: the system starts the program with the files as arguments. A second instance
//    hands them over to the running one (QLocalServer) and quits.
//  - macOS: the running (or just started) app gets a QFileOpenEvent; there is only one instance.
// Files that come before the main window exists wait until start() is called. Files that come one
// after the other (macOS: one event per file; Windows: "Open with" starts one process per file)
// are collected for a moment and delivered as one list, so the list shows them as one drop.
class FileOpener : public QObject
{
    Q_OBJECT

public:
    explicit FileOpener(QObject *parent = nullptr);

    // The paths of the command line that exist, as absolute paths
    static QStringList pathsFromArguments(const QStringList &arguments);

    // True when another instance is running and got the files (this one should quit)
    bool handOverToRunningInstance(const QStringList &paths);
    // Waits for the files of later instances (no-op on macOS)
    void listen();
    // From now on the files are delivered at once
    void start();

signals:
    void filesReceived(const QStringList &paths);   // an empty list: just bring the window forward

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void deliver(const QStringList &paths);
    static QString serverName();

    QLocalServer *m_server = nullptr;
    QTimer m_batchTimer;   // collects what comes in quick succession
    bool m_started = false;
    bool m_hasPending = false;
    QStringList m_pending;
};
