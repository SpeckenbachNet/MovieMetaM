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
#include "fileopener.h"

#include <QDataStream>
#include <QDir>
#include <QEvent>
#include <QFileInfo>
#include <QFileOpenEvent>
#include <QLocalServer>
#include <QLocalSocket>

FileOpener::FileOpener(QObject *parent)
    : QObject(parent)
{
}

QStringList FileOpener::pathsFromArguments(const QStringList &arguments)
{
    QStringList paths;
    for (int i = 1; i < arguments.size(); ++i) {   // [0] is the program
        const QFileInfo info(arguments.at(i));
        if (info.exists()) {
            paths << info.absoluteFilePath();   // the other instance has another working folder
        }
    }
    return paths;
}

// One server per user (the socket lives in the temp folder, which all users share on Linux)
QString FileOpener::serverName()
{
    const QString user = qEnvironmentVariable("USER", qEnvironmentVariable("USERNAME"));
    return QStringLiteral("MovieMetaM-") + user;
}

bool FileOpener::handOverToRunningInstance(const QStringList &paths)
{
#ifdef Q_OS_MACOS
    Q_UNUSED(paths)
    return false;   // macOS starts the app only once
#else
    QLocalSocket socket;
    socket.connectToServer(serverName());
    if (!socket.waitForConnected(500)) {
        return false;   // nobody listens (or a stale socket of a crashed instance)
    }
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out << paths;
    socket.write(data);
    socket.waitForBytesWritten(1000);
    socket.disconnectFromServer();
    if (socket.state() != QLocalSocket::UnconnectedState) {
        socket.waitForDisconnected(1000);
    }
    return true;
#endif
}

void FileOpener::listen()
{
#ifndef Q_OS_MACOS
    m_server = new QLocalServer(this);
    m_server->setSocketOptions(QLocalServer::UserAccessOption);
    QLocalServer::removeServer(serverName());   // left over from a crash: nobody answered above
    if (!m_server->listen(serverName())) {
        return;   // then a second instance simply starts on its own
    }
    connect(m_server, &QLocalServer::newConnection, this, [this] {
        while (QLocalSocket *socket = m_server->nextPendingConnection()) {
            auto *data = new QByteArray;
            connect(socket, &QLocalSocket::readyRead, this, [socket, data] {
                data->append(socket->readAll());
            });
            connect(socket, &QLocalSocket::disconnected, this, [this, socket, data] {
                QStringList paths;
                QDataStream in(*data);
                in >> paths;
                delete data;
                socket->deleteLater();
                deliver(paths);
            });
        }
    });
#endif
}

void FileOpener::start()
{
    m_started = true;
    if (m_hasPending) {
        const QStringList paths = m_pending;
        m_pending.clear();
        m_hasPending = false;
        emit filesReceived(paths);
    }
}

void FileOpener::deliver(const QStringList &paths)
{
    if (m_started) {
        emit filesReceived(paths);
    } else {
        m_pending << paths;
        m_hasPending = true;
    }
}

// macOS: "Open with", a file or folder on the Dock icon, double click
bool FileOpener::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::FileOpen) {
        const QString path = static_cast<QFileOpenEvent *>(event)->url().toLocalFile();
        deliver(path.isEmpty() ? QStringList() : QStringList{path});
        return true;
    }
    return QObject::eventFilter(watched, event);
}
