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

#include <QStringList>
#include <QWidget>

class QListView;
class QStandardItemModel;
class QTextBrowser;
class ToolbarM;

// Help page: own ToolbarM on top ("Back", title), the topics in a list on the left, the text on
// the right (like PhotoBoothM's HelpPage, in the look of MovieMetaM). Works without internet:
// the texts are Markdown files in the resources (help/<language>/<topic>.md), German or English
// like the app. The topics follow the screen from left to right, the info about the app last.
class HelpPage : public QWidget {
    Q_OBJECT

public:
    enum Topic { MovieList, MovieInfo, MovieSearch, Saving, About };   // order in the list

    explicit HelpPage(QWidget *parent = nullptr);

    void showTopic(Topic topic);
    ToolbarM *toolbar() const { return m_toolbar; }   // the main window gives it the app's shape

signals:
    void backRequested();

protected:
    void changeEvent(QEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;   // line width follows the text area

private:
    static constexpr int kMaxLineWidth = 760;   // lines not too long to read
    static constexpr int kTopicsWidth = 240;
    static constexpr int kImageHeight = 20;     // pictures in the texts (TMDB logo)

    void addTopic(const QString &icon, const QString &title, const QString &subtitle, const QString &file);
    void loadText(int row);
    void spaceOut();      // headings, line spacing, paragraphs
    void scaleImages();   // pictures to kImageHeight, sharp on the screen
    void restyle();
    static QString language();   // "de" or "en"

    ToolbarM           *m_toolbar = nullptr;
    QListView          *m_topics = nullptr;
    QStandardItemModel *m_model = nullptr;
    QTextBrowser       *m_text = nullptr;
    QStringList         m_icons;   // icon of each topic (tinted again on a theme change)
    QStringList         m_files;   // file name of each topic
};
