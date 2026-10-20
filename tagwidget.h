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
#include <QWidget>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QLabel>
#include <QEvent>
#include <QListView>
#include <QStringListModel>
#include <QTimer>

class TagWidget : public QWidget
{
    Q_OBJECT

public:
    TagWidget(const QString &placeholder,
              const QString &separator,
              QWidget *parent = nullptr);

    // Schnittstelle
    void setTags(const QString &tags);
    void clearTags();
    QStringList getTags() const;
    QString getTagsCommaSeperated() const;

signals:
    void tagsChanged(const QStringList &newTags);

private slots:
    void handleInputFinished();
    void removeTagByClick(const QModelIndex &index); // slot for removing a tag
    void onRowsMoved(); // Konsistent zum Signal-Namen

private:
    void setupUi(const QString &placeholder); // initialization moved out

    // Member
    QStringList m_tagList; // internal list for easy handling
    QString m_separator;
    QString m_tokenStyle; // needed for styling the tags in the view (via delegate)

    QStringListModel *m_tagModel = nullptr;
    QListView *m_tagView = nullptr;
    QLineEdit *m_lineEdit = nullptr;
};

