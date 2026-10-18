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
#include <QToolButton>
#include <qcombobox.h>

#include "moviedata.h"
#include "tagwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWin;
}
QT_END_NAMESPACE

namespace RatingColor {
    const QColor USBlueNC17  = QColor(0, 71, 145);
    const QColor LightBlue   = QColor(0, 174, 239);  // Spanien "7" / Sky Blue
    const QColor Blue        = QColor(0, 102, 204);  // FSK 16
    const QColor Cyan        = QColor(0, 159, 221);  // Strong cyan/light blue for Taiwan 6+
    const QColor LightGreen  = QColor(141, 198, 63);
    const QColor Green       = QColor(0, 153, 51);   // Classical FSK/EIRIN green
    const QColor Magenta     = QColor(255,0,255);
    const QColor Orange      = QColor(255, 102, 0);  // UK 12A / IT 12+
    const QColor Petrol      = QColor(0, 102, 128);  // Italy 10+
    const QColor ESPink      = QColor(255, 0, 127);  // Specific Pink Spain-X
    const QColor USPink      = QColor(255, 0, 102);
    const QColor Red         = QColor(204, 0, 0);    // FSK 18 / Japanese R15+
    const QColor White       = QColor(255, 255, 255);
    const QColor Yellow      = QColor(255, 204, 0);  // Rich yellow
}

class MainWin : public QMainWindow
{
    Q_OBJECT

struct RatingEntry {
    QString key;
    int score;
    QString label;
    QColor color;
    QString shape;
};

struct CountryRatingSystem {
    QString countryName;
    QString technicalID;
    QString systemName;
    QList<RatingEntry> ratings;
};


public:
    MainWin(QWidget *parent = nullptr);
    ~MainWin();
    QSettings settings;
    QMap<QString, QString> readMetadataTags(const QString &filePath, const QStringList &tags);
    void startApiRequest(const QUrl &url);
    void startImageDownload(const QUrl &url); // Starts download an return immediately
    QIcon scaled;

private:
    Ui::MainWin *ui;
    QNetworkAccessManager *m_ApiNetworkManager;
    QNetworkAccessManager *m_ImageNetworkManager;
    QHash<QNetworkReply*, QUrl> m_urlMapping;
    QTemporaryFile *m_tempFile = nullptr;
    QStandardItemModel *m_SearchModel = nullptr;
    int m_searchGeneration = 0;   // counts the searches: posters of an older search are dropped
    static constexpr QSize kPosterSize{40, 60};   // poster in the search results (2:3)
    QStandardItemModel *m_FilelistModel = nullptr;
    QPixmap extractMovieCover(const QString& filePath);
    QIcon m_statusGreen;    // status symbols in the movie list (same look when selected)
    QIcon m_statusYellow;
    QString getInfoHtmlTemplate();
    QString countryCode;
    QMap<QString, MovieData> m_loadedMovies;
    QMap<QString, MovieData> m_originalMovies;
    TagWidget *m_castWidget = nullptr;
    TagWidget *m_screenwritersWidget = nullptr;
    TagWidget *m_producersWidget = nullptr;
    TagWidget *m_directorsWidget = nullptr;

    // Toolbar buttons (ToolbarM)
    QToolButton *m_saveBtn = nullptr;
    QToolButton *m_saveAllBtn = nullptr;
    QToolButton *m_settingsBtn = nullptr;
    QToolButton *m_helpBtn = nullptr;
    QToolButton *m_settingsBackBtn = nullptr;
    QToolButton *m_saveSettingsBtn = nullptr;
    QToolButton *m_infoBackBtn = nullptr;
    QToolButton *m_addBtn = nullptr;       // below the movie list
    QToolButton *m_removeBtn = nullptr;

    void showPage(QWidget *page);
    void setSignalsBlocked(bool blocked);
    void displayMovie(const MovieData &movie);
    void fillSearchTableView(const QJsonDocument &jsonDoc);
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
    void initializeAgeRating();
    void initializeStatusIcons();
    void onDataChanged();
    void onSettingsChanged();
    void updateStatus(int row, QString status);
    void formatTextByLength(int limit);
    void clearData();
    void selectRowInTable(int row);
    void fillRatingComboBox(QString countryCode);
    void fillRatingSettingsComboBox();
    void setComboEntryByString(QComboBox *combo, QString findString, bool useData);
    QIcon createAgeRatingIcon(QString age, QColor color, QString shape);
    // Age rating icon in the combo box: 18 is the largest size that fits centered on macOS.
    // (macOS draws it about 2 px too high in the closed box – open point, see CLAUDE.md)
    static constexpr int kRatingIconSize = 18;
    QMap<QString, CountryRatingSystem> ratingMap;

protected:
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;

private slots:
    void onResizeMainWin();
    void onSearchTableViewDoubleClicked(const QModelIndex &index);
    void onFilelistTableViewClicked(const QModelIndex &index);
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
