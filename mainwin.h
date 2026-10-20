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
#include <QFutureWatcher>
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
#include <QPushButton>
#include <QStandardItemModel>
#include <QScrollBar>
#include <QString>
#include <QSet>
#include <QSettings>
#include <QTextBrowser>
#include <QTableWidgetItem>
#include <QTemporaryFile>
#include <QUrlQuery>
#include <QToolButton>
#include <qcombobox.h>

#include "agerating.h"
#include "languages.h"
#include "moviedata.h"
#include "posterpicker.h"
#include "tagwidget.h"
#include "toastm.h"

// One movie to save: a copy of the data for the thread (without the pixmap: QPixmap belongs to
// the GUI thread, the covers go as QImage)
struct SaveJob {
    QString key;
    MovieData movie;
    QImage cover;
    QImage originalCover;
};

struct SaveResult {
    QString key;
    bool ok = false;
    QString error;
};

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

    // Where a message appears: over one of the columns or over the whole window
    enum class StatusColumn { Movies, Info, Search, Window };
    void showColumnStatus(StatusColumn column, const QString &text,
                          ToastM::Level level = ToastM::Level::Info, int msec = 0);

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::MainWin *ui;
    QNetworkAccessManager *m_ApiNetworkManager;
    QNetworkAccessManager *m_ImageNetworkManager;
    QHash<QNetworkReply*, QUrl> m_urlMapping;
    QTemporaryFile *m_tempFile = nullptr;
    QStandardItemModel *m_SearchModel = nullptr;
    // Tab "Data": the details of the search result that was double clicked
    QLabel *m_detailsTitle = nullptr;
    PosterPicker *m_posterPicker = nullptr;     // the posters TMDB has for it
    QTextBrowser *m_detailsView = nullptr;
    QJsonArray m_tmdbReleaseDates;              // certifications of the result, by country
    QString m_tmdbPosterPath;                   // poster of the search result (preselected)
    QStringList m_posterPaths;                  // TMDB file paths of the entries of m_posterPicker
    int m_detailsGeneration = 0;                // counts the double clicks: older answers are dropped
    QPushButton *m_applyBtn = nullptr;
    MovieData m_tmdbMovie;                      // what TMDB knows about it (see fillDetailsTab)
    static constexpr int kCastLimit = 20;       // TMDB lists far more names than make sense
    int m_searchGeneration = 0;   // counts the searches: posters of an older search are dropped
    static constexpr QSize kPosterSize{40, 60};   // poster in the search results (2:3)
    static constexpr int kPosterRole = Qt::UserRole + 1;   // poster in full resolution (preview)
    static constexpr int kPosterPathRole = Qt::UserRole + 2;   // TMDB path of the poster
    static constexpr int kPosterLimit = 30;     // posters offered for one movie
    QStandardItemModel *m_FilelistModel = nullptr;
    QPixmap extractMovieCover(const QString& filePath);
    QIcon m_statusGreen;    // status symbols in the movie list (same look when selected)
    QIcon m_statusYellow;
    QString countryCode;
    QFutureWatcher<QList<SaveResult>> *m_saveWatcher = nullptr;
    bool m_saving = false;
    bool m_safeSave = false;   // setting "General/SafeSave": write into a copy first
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

    void showPage(QWidget *page);
    void setSignalsBlocked(bool blocked);
    void displayMovie(const MovieData &movie);
    void fillSearchTableView(const QJsonDocument &jsonDoc);
    void requestGenres();   // genre list from TMDB in the language of the movie data
    void fillGenreComboBox(const QJsonDocument &jsonDoc);
    void fillDetailsTab(const QJsonDocument &jsonDoc);
    void fillPosters(const QJsonDocument &jsonDoc);
    void showTmdbPoster(int row);               // the large poster in the tab "Data"
    QString ratingCountry(const MovieData &movie) const;   // system the age rating of a movie follows
    bool findTmdbRating(const QString &country, QString &systemID, RatingEntry &entry) const;
    void showTmdbDetails();                     // text of the tab "Data"
    void applyTmdbMovie();                      // takes everything found into the current movie
    void fillRatingComboBox(const QJsonDocument &jsonDoc);
    void addFilesToModel(const QStringList &filePaths);
    void addFilePathToModel(const QString &filePath);
    void saveMovieDetails();                    // the selected movie into its file
    void startSaving(const QStringList &keys);  // writes the movies in a thread (the window is locked meanwhile)
    void savingFinished();                      // back in the GUI thread: status, list and messages
    void updateSaveButtons();                   // "Save" / "Save all" only when there is something to save
    void removeMovie(const QString &moviePath);
    bool validateAndRefreshChanges();
    bool settingsChanged;
    void readTagsFromFile(const QString &filePath);
    // onError (optional): download failed or the data is no image
    void startImageDownload(const QUrl &imageUrl, std::function<void(const QPixmap &pixmap)> callback,
                            std::function<void()> onError = nullptr);
    void initializeToolBars();
    void initializeTagWidgets();
    void initializeDetailsTab();
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
