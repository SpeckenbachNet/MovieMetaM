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

#ifndef MAINWIN_H
#define MAINWIN_H

#include <QMainWindow>

#include <QCloseEvent>
#include <QColor>
#include <QDebug>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QMap>
#include <QMessageBox>
#include <QMimeData>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPainter>
#include <QPainterPath>
#include <QStandardItemModel>
#include <QScrollBar>
#include <QString>
#include <QSet>
#include <QSettings>
#include <QTableWidgetItem>
#include <QTemporaryFile>
#include <QTimer>
#include <QUrlQuery>
#include <QToolButton>
#include <qcombobox.h>

#include "agerating.h"
#include "languages.h"
#include "moviedata.h"
#include "tagwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWin;
}
QT_END_NAMESPACE

class MainWin : public QMainWindow
{
    Q_OBJECT

public:
    MainWin(QWidget *parent = nullptr);
    ~MainWin();
    QSettings settings;
    QMap<QString, QString> readMetadataTags(const QString &filePath, const QStringList &tags);
    void startApiRequest(const QUrl &url);
    void startImageDownload(const QUrl &url); // Starts download an return immediately
    QIcon scaled;

    // The columns of the main window; each shows its messages in the status bar below it
    enum class StatusColumn { Movies, Info, Search };
    void showColumnStatus(StatusColumn column, const QString &text, int msec = 10000);

protected:
    void closeEvent(QCloseEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;   // status labels follow the columns

private:
    Ui::MainWin *ui;
    QNetworkAccessManager *m_ApiNetworkManager;
    QNetworkAccessManager *m_ImageNetworkManager;
    QHash<QNetworkReply*, QUrl> m_urlMapping;
    QTemporaryFile *m_tempFile = nullptr;
    QStandardItemModel *m_SearchModel = nullptr;
    int m_searchGeneration = 0;   // counts the searches: posters of an older search are dropped
    static constexpr QSize kPosterSize{40, 60};   // poster in the search results (2:3)
    static constexpr int kPosterRole = Qt::UserRole + 1;   // poster in full resolution (preview)
    QStandardItemModel *m_FilelistModel = nullptr;
    QPixmap extractMovieCover(const QString& filePath);
    QIcon m_statusGreen;    // status symbols in the movie list (same look when selected)
    QIcon m_statusYellow;
    QString countryCode;
    QString dataLanguage;   // language of the movie data for TMDB, e.g. "de-DE"
    QMap<QString, MovieData> m_loadedMovies;
    QMap<QString, MovieData> m_originalMovies;
    TagWidget *m_castWidget = nullptr;
    TagWidget *m_screenwritersWidget = nullptr;
    TagWidget *m_producersWidget = nullptr;
    TagWidget *m_directorsWidget = nullptr;

    // Toolbar buttons (ToolbarM)
    QToolButton *m_saveBtn = nullptr;
    QToolButton *m_saveAllBtn = nullptr;
    QToolButton *m_coffeeBtn = nullptr;    // donation (PayPal)
    QToolButton *m_settingsBtn = nullptr;
    QToolButton *m_helpBtn = nullptr;
    QToolButton *m_settingsBackBtn = nullptr;
    QToolButton *m_saveSettingsBtn = nullptr;
    QToolButton *m_addBtn = nullptr;       // below the movie list
    QToolButton *m_removeBtn = nullptr;

    // Status bar: one label below each column (a message over the whole bar hides them)
    QWidget *m_statusContainer = nullptr;
    QLabel *m_statusLabels[3] = {};
    QString m_statusTexts[3];
    QTimer *m_statusTimers[3] = {};
    bool m_statusUpdatePending = false;
    void initializeStatusBar();
    void updateStatusLabels();

    void showPage(QWidget *page);
    void setSignalsBlocked(bool blocked);
    void displayMovie(const MovieData &movie);
    void fillSearchTableView(const QJsonDocument &jsonDoc);
    void requestGenres();   // genre list from TMDB in the language of the movie data
    void fillGenreComboBox(const QJsonDocument &jsonDoc);
    void fillRatingComboBox(const QJsonDocument &jsonDoc);
    void addFilesToModel(const QStringList &filePaths);
    void addFilePathToModel(const QString &filePath);
    void saveMovieDetails();
    void removeMovie(const QString &moviePath);
    bool validateAndRefreshChanges();
    bool settingsChanged;
    void readTagsFromFile(const QString &filePath);
    // onError (optional): download failed or the data is no image
    void startImageDownload(const QUrl &imageUrl, std::function<void(const QPixmap &pixmap)> callback,
                            std::function<void()> onError = nullptr);
    void initializeToolBars();
    void initializeTagWidgets();
    void initializeDragAndDrop();
    void initializeConnects();
    void initializeNetwork();
    void initializeForm();
    void initializeComboBoxes();
    void initializeStatusIcons();
    void onDataChanged();
    void onSettingsChanged();
    void updateStatus(int row, QString status);
    void formatTextByLength(int limit);
    void clearData();
    void selectRowInTable(int row);
    void fillRatingComboBox(QString countryCode);
    void showMovieRating(const MovieData &movie);   // fills the list with the system of the file
    // Entries of the age rating combo box: score in Qt::UserRole, code and system ID of the file
    static constexpr int kRatingCodeRole = Qt::UserRole + 1;
    static constexpr int kRatingSystemRole = Qt::UserRole + 2;
    void fillRatingSettingsComboBox();
    void fillRatingPreview(const QString &countryCode);   // badges of the system chosen in the settings
    void setComboEntryByString(QComboBox *combo, QString findString, bool useData);
    // shape: square, circle, diamond, triangle, octagon, hexagon, star; style: filled, outline, light, dark
    QIcon createAgeRatingIcon(QString age, QColor color, QString shape, QString style = "filled");
    // Age rating icon in the combo box: 18 is the largest size that fits centered on macOS.
    // (macOS draws it about 2 px too high in the closed box – open point, see CLAUDE.md)
    static constexpr int kRatingIconSize = 18;
    static constexpr int kRatingPreviewSize = 28;   // badges in the preview of the settings

    // Movie info: fixed cover size (poster ratio 183 : 129.5) and the widest tag fields
    static constexpr QSize kCoverSize{150, 212};
    static constexpr int kTagColumnWidth = 240;

private slots:
    void onSearchTableViewDoubleClicked(const QModelIndex &index);
    void onCurrentMovieChanged(const QModelIndex &index);
    void onSaveAllButtonClicked();
    void onApiRequestFinished(QNetworkReply *reply);
    void onImageDownloadFinished(QNetworkReply *reply);
    void onSearchButtonClicked();
    void onAddFilesButtonClicked();
    void onRemoveFilesButtonClicked();

    // Toolbar action-handler
    void handleSaveSettingsButton_Clicked();

signals:
    // New Signal: Triggered when immage is loaded
    void imageDownloadFinished(const QPixmap &pixmap, const QUrl &originalUrl, QNetworkReply *reply);
    // Optional: signal in case of error
    void imageDownloadFailed(const QUrl &originalUrl, const QString &errorString);
};


#endif // MAINWIN_H
