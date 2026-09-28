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

#include "mainwin.h"
#include "./ui_mainwin.h"
#include "htmltemplates.h"

MainWin::MainWin(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWin),
    settings(QSettings::IniFormat, QSettings::UserScope, "Speckenbach.Net", "MovieMetaM")
{
    ui->setupUi(this);

    // Get countryCode from INI-File for AgeRating System
    countryCode = settings.value("General/AgeRatingSystem").toString();
    qDebug() << "Country Code at beginnign: " << countryCode;

    // Initialize
    initializeForm();
    initializeAgeRating();
    initializeToolBars();
    initializeTagWidgets();
    initializeDragAndDrop();
    initializeNetwork();
    initializeComboBoxes();
    initializeConnects();
    initializeStatusIcons();

    ui->fileListTableView->setAcceptDrops(true);
    ui->middleGroupBox->setEnabled(false);
}

void MainWin::fillRatingComboBox(QString countryCode)
{
    ui->ratingComboBox->clear();

    auto it = ratingMap.constFind(countryCode);
    if (it == ratingMap.constEnd()) {
        qDebug() << "Country does not exist";
        return;
    }

    ui->ratingComboBox->addItem("");

    const CountryRatingSystem &system = it.value();
    for (const RatingEntry &entry : std::as_const(system.ratings)) {
        QIcon icon = createAgeRatingIcon(entry.key,
                                         entry.color,
                                         entry.shape);

        ui->ratingComboBox->addItem(icon, entry.label, entry.score);
        int lastIndex = ui->ratingComboBox->count() - 1;
        ui->ratingComboBox->setItemData(lastIndex, entry.label, Qt::ToolTipRole);
    }

    ui->ratingComboBox->setIconSize(QSize(24, 24));
}

void MainWin::fillRatingSettingsComboBox()
{
    ui->ratingSettingsComboBox->clear();
    for (auto it = std::as_const(ratingMap).cbegin(); it != std::as_const(ratingMap).cend(); ++it) {
        const auto &entry = it.value();
        QString displayLabel = QString("%1 (%2)").arg(entry.countryName, entry.systemName);
        ui->ratingSettingsComboBox->addItem(displayLabel, it.key());
    }
    ui->ratingSettingsComboBox->model()->sort(0);
    setComboEntryByString(ui->ratingSettingsComboBox, countryCode, true);
}

QString MainWin::getInfoHtmlTemplate()
{
    // 1. Das Template laden
    QString htmlContent = AppTemplates::INFO_HTML;

    // 2. Ersetzung der Platzhalter
    htmlContent.replace("$APP_NAME$", QCoreApplication::applicationName());
    htmlContent.replace("$APP_VERSION$", QCoreApplication::applicationVersion());
    htmlContent.replace("$DEVELOPER_NAME$", "Michael Speckenbach");
    htmlContent.replace("$CURRENT_YEAR$", QString::number(QDate::currentDate().year()));
    htmlContent.replace("$LICENSE$", "GNU General Public License (GPLv3)");

    return htmlContent;
}

QIcon MainWin::createAgeRatingIcon(QString age, QColor color, QString shape) {
    // 1. Setup: Wir erstellen eine quadratische Leinwand (z.B. 64x64)
    int size = 32;
    QPixmap pixmap(size, size);
    pixmap.fill(Qt::transparent); // Make the background transparent

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    // Das Zeichen-Rechteck (mit 2 Pixel Padding für den Rahmen)
    QRect rect(2, 2, size - 4, size - 4);

    // 2. Form zeichnen (Hintergrund füllen)
    painter.setBrush(color);
    painter.setPen(QPen(Qt::black, 2)); // Schwarzer Rand, Stärke 2

    if (shape.toLower() == "square") {
        painter.drawRoundedRect(rect, 5, 5);
    }
    else if (shape.toLower() == "circle") {
        painter.drawEllipse(rect);
    }
    else if (shape.toLower() == "diamond") {
        // Die Raute über 4 Punkte (Mitte oben, rechts, unten, links)
        QPointF points[4] = {
            QPointF(size / 2.0, rect.top()),
            QPointF(rect.right(), size / 2.0),
            QPointF(size / 2.0, rect.bottom()),
            QPointF(rect.left(), size / 2.0)
        };
        painter.drawPolygon(points, 4);
    }
    else if (shape.toLower() == "triangle") {
        // Das Dreieck über 3 Punkte
        QPointF points[3] = {
            QPointF(size / 2.0, rect.top()),
            QPointF(rect.right(), rect.bottom()),
            QPointF(rect.left(), rect.bottom())
        };
        painter.drawPolygon(points, 3);
    }

    // If the background color is light, the text is black; otherwise, it is white.
    qreal luminance = (0.299 * color.red() + 0.587 * color.green() + 0.114 * color.blue());
    painter.setPen(luminance > 160 ? Qt::black : Qt::white);

    QFont font = painter.font();
    font.setBold(true);
    // Let's start with a large font.
    font.setPixelSize(size * 0.45);
    painter.setFont(font);

    // If the text is too wide (e.g., "PG-13"), reduce the font size.
    if (painter.fontMetrics().horizontalAdvance(age) > rect.width() - 10) {
        font.setPixelSize(size * 0.35);
        painter.setFont(font);
    }

    // In triangles, the text often appears too high; adjust it slightly downwards here.
    QRect textRect = rect;
    if (shape.toLower() == "triangle") textRect.setTop(rect.top() + (size * 0.15));

    painter.drawText(textRect, Qt::AlignCenter, age);
    painter.end();

    return QIcon(pixmap);
}

void MainWin::clearData()
{
    // editFileds
    ui->titleEdit->clear();
    ui->studioEdit->clear();

    // textFields
    ui->descriptionText->setPlainText("");

    // Release Date
    ui->releaseDateEdit->setDate(QDate(1972, 1, 1));

    // comboBoxes
    ui->genreComboBox->setCurrentIndex(0);
    ui->ratingComboBox->setCurrentIndex(0);

    // tagWidgets
    m_castWidget->clearTags();
    m_directorsWidget->clearTags();
    m_screenwritersWidget->clearTags();
    m_producersWidget->clearTags();

    // Cover
    ui->coverLabel->clear();

}

void MainWin::onDataChanged() {
    int row = ui->fileListTableView->selectedRow();
    // Save Dataset and change Status.
    if (this->validateAndRefreshChanges()) {
        qDebug() << "Dataset Changed";
        this->updateStatus(row, "modified");
        // Placeholder save buttons
    }
    else {

        this->updateStatus(row, "unchanged");
        qDebug() << "Dataset unchanged!";
    }
}

void MainWin::onSettingsChanged() {

    if(countryCode != ui->ratingSettingsComboBox->currentData(Qt::UserRole)){
        settingsChanged = true;
        ui->actionSaveSettings->setEnabled(true);
    } else {
        settingsChanged = false;
        ui->actionSaveSettings->setEnabled(false);
    }
}

void MainWin::updateStatus (int row, QString status) {
    QPixmap m_status;
    if (status == "modified") m_status = m_statusYellow;
    else if (status == "unchanged") m_status = m_statusGreen;
    QStandardItem *statusItem = m_FilelistModel->item(row, 0);
    if (statusItem) {
        statusItem->setData(m_status, Qt::DecorationRole);
        statusItem->setToolTip(tr("DATACHANGEDSAVENEEDED"));
    }
}

void MainWin::formatTextByLength(int limit)
{
    // 1. Vorbereitungen: Signale blockieren & Scroll-Position merken
    ui->descriptionText->blockSignals(true);
    int scrollPos = ui->descriptionText->verticalScrollBar()->value();

    // Aktuellen Cursor des Widgets holen (enthält die Position des Users)
    QTextCursor userCursor = ui->descriptionText->textCursor();
    int oldPos = userCursor.position();

    // 2. Daten laden
    QString text = ui->descriptionText->toPlainText();
    int totalLength = text.length();

    // Wir erstellen einen Arbeits-Cursor für die Formatierung
    QTextCursor editCursor(ui->descriptionText->document());

    // Holt die Standard-Textfarbe des aktuellen System-Themes
    QColor defaultColor = ui->descriptionText->palette().color(QPalette::Text);

    // --- Format-Definitionen ---
    QTextCharFormat defaultFormat;
    defaultFormat.setForeground(QBrush(defaultColor));

    QTextCharFormat warningFormat;
    warningFormat.setForeground(QBrush(Qt::red));

    // --- 3. Formatierung anwenden ---

    // A. Gesamten Text zuerst auf Standard (Schwarz) zurücksetzen
    editCursor.select(QTextCursor::Document);
    editCursor.setCharFormat(defaultFormat);

    // B. Wenn Limit überschritten, ab dem Limit rot einfärben
    if (totalLength > limit) {
        editCursor.setPosition(limit);
        editCursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
        editCursor.mergeCharFormat(warningFormat);
    }

    // --- 4. Cursor und UI wiederherstellen ---

    // Den ursprünglichen Cursor des Users wiederherstellen
    // qMin stellt sicher, dass wir nicht außerhalb des Textes landen
    userCursor.setPosition(qMin(oldPos, totalLength));
    ui->descriptionText->setTextCursor(userCursor);

    // Scrollbar wieder an die alte Stelle setzen
    ui->descriptionText->verticalScrollBar()->setValue(scrollPos);

    ui->descriptionText->blockSignals(false);
}

void MainWin::saveMovieDetails()
{
    // Die aktuelle Liste der Schauspieler abrufen
    //QStringList currentActors = m_actorWidget->getTags();

    // Die aktuelle Liste der Genres abrufen
    //QStringList currentGenres = m_genreWidget->getTags();

    //qDebug() << "Gespeicherte Schauspieler:" << currentActors;
    //qDebug() << "Gespeicherte Genres:" << currentGenres;

    // Hier folgt die Logik, um diese Listen in Ihrem QStandardItemModel oder Ihrer Datenbank zu speichern
    // ...
}

void MainWin::removeMovie(const QString &moviePath) {
    // Remove from working state (work)
    if (m_loadedMovies.contains(moviePath)) {
        m_loadedMovies.remove(moviePath);
    }
    // Remove from original state (original)
    if (m_originalMovies.contains(moviePath)) {
        m_originalMovies.remove(moviePath);
    }
    qDebug() << "Movie completely removed from all storage:" << moviePath;
}

bool MainWin::validateAndRefreshChanges() {

    // Get movie key
    bool hasChanged = false;
    QString movieKey = ui->fileListTableView->movieKey();
    if (movieKey.isEmpty()) return false;

    // Get orgiginal map for comparing and working map for saving
    const MovieData &original = m_originalMovies[movieKey];
    MovieData &work = m_loadedMovies[movieKey];

    // Text/Editfields
    if (ui->titleEdit->text() != original.title) {
        qDebug() << "Title has changed.";
        qDebug() << "UI Text: " << ui-> titleEdit->text();
        qDebug() << "OR Text: " << original.title;
        hasChanged = true;
    }
    work.title = ui->titleEdit->text();

    if (ui->studioEdit->text() != original.studio) {
        qDebug() << "Studio has changed";
        hasChanged = true;
    }
    work.studio = ui->studioEdit->text();

    if (ui->descriptionText->toPlainText() != original.description) {
        qDebug() << "Description has changed";
        hasChanged = true;
    }
    work.studio = ui->studioEdit->text();

    // DateEdit
    if (ui->releaseDateEdit->date() != original.releaseDate) {
        qDebug() << "Release Date has changed.";
        hasChanged = true;
    }
    work.releaseDate = ui->releaseDateEdit->date();

    // Comboboxes
    if (ui->genreComboBox->currentText() != original.genre) {
        qDebug() << "Genre has changed.";
        hasChanged = true;
    }
    work.genre = ui->genreComboBox->currentText();

    int currentScore = ui->ratingComboBox->currentData(Qt::UserRole).toInt();
    if (currentScore != original.ratingScore) {
        qDebug() << "AgeRating has changed.";
        hasChanged = true;
    }
    work.ratingScore = currentScore;

    // ----- Cover Label -----
    QPixmap currentCover = work.cover;

    // Comparison against the ORIGINAL
    // We first check whether either is null and then check the image data
    bool coverActuallyChanged = false;

    if (currentCover.isNull() != original.cover.isNull()) {
        // One state is empty, the other is not -> Changed!
        coverActuallyChanged = true;
    }
    else if (!currentCover.isNull()) {
        // Both have an image -> We compare the image data (pixels)
        // toImage() and a comparison is the safest way in Qt
        if (currentCover.toImage() != original.cover.toImage()) {
            coverActuallyChanged = true;
        }
    }
    qDebug() << "Current Size:" << currentCover.size() << "Original Size:" << original.cover.size() << "Work Cover:" << work.cover.size();
    if (coverActuallyChanged) {
        qDebug() << "Cover has changed.";
        hasChanged = true;
    }

    work.cover = currentCover;

    // ----- TagWidgets -----
    // Get current Cast from Widget
    QString currentCastList = m_castWidget->getTagsCommaSeperated(); // get Tags from Widget
    // Compare current Cast with original map
    if (currentCastList != original.cast) {
        qDebug() << "Cast has changed.";
        qDebug() << "Current: " << currentCastList;
        qDebug() << "Original " << original.cast;
        hasChanged = true; // Only if a name is actually missing or spelled differently
    }else {
        qDebug() << "Current: " << currentCastList;
        qDebug() << "Original " << original.cast;
        qDebug() << "Datensatz ist wieder gleich!";
    }
    // Set current Cast to working map

    work.cast = currentCastList;

    QString currentDirectorList = m_directorsWidget->getTagsCommaSeperated();
    if (currentDirectorList != original.directors) {
        qDebug() << "Directors have changed.";
        hasChanged = true;
    }
    work.directors = currentDirectorList;

    QString currentScreenwriterList = m_screenwritersWidget->getTagsCommaSeperated();
    if (currentScreenwriterList != original.screenwriters) {
        qDebug() << "Screenwriters have changed.";
        hasChanged = true;
    }
    work.screenwriters = currentScreenwriterList;

    QString currentProducerList = m_producersWidget->getTagsCommaSeperated();
    if (currentProducerList != original.producers) {
        qDebug() << "Producers have changed.";
        hasChanged = true;
    }
    work.producers = currentProducerList;

    // Only change the status of movie if something has actually changed.
    work.status = hasChanged ? "modified" : "unchanged";
    qDebug() << "hasChanged: " << hasChanged;
    return hasChanged;
}

void MainWin::addExpandingSpacer(QToolBar *toolbar)
{
    if (!toolbar) return;

    // Toolbar as parent
    QWidget *spacer = new QWidget(toolbar);

    // set Spacer to expanding
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // add Spacer to toolbar
    toolbar->addWidget(spacer);
}

void MainWin::fillSearchTableView(const QJsonDocument &jsonDoc)
{
    if (jsonDoc.isNull() || !jsonDoc.isObject()) {
        qWarning() << "Error: Invalid or empty JSON document for movie details.";
        return;
    }

    QJsonObject rootObj = jsonDoc.object();

    if (!rootObj.contains("results") || !rootObj.value("results").isArray()) {
        qWarning() << "'results' ist kein Array oder fehlt";
        return;
    }

    QJsonArray jsonArray = rootObj.value("results").toArray();

    if (jsonArray.isEmpty()) return;

    if (!m_SearchModel) {
        m_SearchModel = new QStandardItemModel(this);
    } else {
        m_SearchModel->clear(); // Altes Model leeren, wenn es existiert
    }

    // Nur zwei Spalten: Titel und Datum
    QStringList headers = {"title", "release_date"};
    m_SearchModel->setColumnCount(headers.size());
    m_SearchModel->setHorizontalHeaderLabels(QStringList() << tr("TITLE") << tr("YEAR"));

    for (const QJsonValue &val : std::as_const(jsonArray)) {
        QJsonObject obj = val.toObject();
        QList<QStandardItem *> rowItems;

        // URL for Cover
        QString posterImage = obj.value("poster_path").toString();
        qDebug() << posterImage;
        QUrl imageUrl("https://image.tmdb.org/t/p/w500" + posterImage);

        // title
        QStandardItem *titleItem = new QStandardItem(obj.value("title").toString());
        titleItem->setData(obj.value("id").toInt(), Qt::UserRole);
        rowItems.append(titleItem);

        // Prepare Index varaibles
        int rowIndex = m_SearchModel->rowCount(); // Next free row number
        int columnIndex = 0;

        // Calls the new, generic download logic.
        // The code within this lambda is executed asynchronously,
        // once the image has been loaded and the pixmap is complete.
        startImageDownload(imageUrl,
           [this, titleItem, rowIndex, columnIndex](const QPixmap &pixmap) {

               // Callback part (Model-specific)

               if (titleItem && m_SearchModel) {
                // Scale cover for tableview (64x45)
                   QIcon scaledCover(pixmap.scaled(64, 45, Qt::KeepAspectRatio, Qt::SmoothTransformation));

                   const QModelIndex itemIndex = m_SearchModel->index(rowIndex, columnIndex);

                   if (itemIndex.isValid()) {
                       // Update model with icon
                       m_SearchModel->setData(itemIndex, scaledCover, Qt::DecorationRole);
                       qDebug() << "Icon for" << titleItem->text() << "set over generic handler.";
                   }
               }
           }
           );

        // Second column
        QString dateStr = obj.value("release_date").toString();
        QString year = dateStr.left(4);
        rowItems.append(new QStandardItem(year));

        // add row to model (nachdem der rowIndex für das Lambda berechnet wurde)
        m_SearchModel->appendRow(rowItems);
    }

    // Width of TableView
    int totalWidth = ui->layoutRight->width();

    // Column Year 1/4 from Width, Title Column the rest
    ui->searchTableView->setModel(m_SearchModel);

    ui->searchTableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch); // Titel-Spalte füllt Rest
    ui->searchTableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);   // Jahr-Spalte feste Breite
    ui->searchTableView->setColumnWidth(1, totalWidth / 4);

    ui->searchTableView->verticalHeader()->setDefaultSectionSize(64);
    QSize iconSize(64, 45);
    ui->searchTableView->setIconSize(iconSize);
}

void MainWin::fillGenreComboBox(const QJsonDocument &jsonDoc)
{
    if (jsonDoc.isNull() || !jsonDoc.isObject()) {
        qWarning() << "Error: Invalid or empty JSON document for genres.";
        return;
    }

    QJsonObject rootObj = jsonDoc.object();

    if (!rootObj.contains("genres") || !rootObj.value("genres").isArray()) {
        qWarning() << "'genres' ist kein Array oder fehlt";
        return;
    }

    QJsonArray jsonArray = rootObj.value("genres").toArray();
    QStringList genreNames;

    for (const QJsonValue &value : std::as_const(jsonArray)) {
        if (value.isObject()) {
            QJsonObject genreObject = value.toObject();

            // Extrahieren des Namens
            QString name = genreObject["name"].toString();

            if (!name.isEmpty()) {
                genreNames.append(name);
            }
        }
    }

    ui->genreComboBox->clear();
    ui->genreComboBox->addItem("");
    ui->genreComboBox->addItems(genreNames);
}

void MainWin::addFilesToModel(const QStringList &filePaths)
{
    if (filePaths.isEmpty()) return;

    int firstNewRowIndex = -1;

    for (const QString &path : filePaths) {
        // Check if path is already in map
        if (m_loadedMovies.contains(path)) {
            qDebug() << "File already exists, skip:" << path;
            continue; // Springe zur nächsten Datei in der Liste
        }

        // If new: Note where the first NEW film lands.
        if (firstNewRowIndex == -1) {
            firstNewRowIndex = m_FilelistModel->rowCount();
        }

        // Add now to Model
        addFilePathToModel(path);

        if (m_loadedMovies.contains(path)) {
            m_originalMovies[path] = m_loadedMovies[path];
        }
    }

    // Only select if we have actually added something NEW.
    if (firstNewRowIndex != -1) {
        selectRowInTable(firstNewRowIndex);
    } else {
        qDebug() << "No new files found to add.";
    }
}

void MainWin::addFilePathToModel(const QString &filePath)
{
    // Read data from file to model and save in map
    MovieData movie = MovieData::fromFile(filePath);
    //qDebug() << "Cast readed: " << movie.cast;
    m_loadedMovies.insert(filePath, movie);  // put movie to model
    m_originalMovies.insert(filePath, movie); // put movie also to memory

    // Save status in first Column and filePath in UserRole
    QStandardItem *statusItem = new QStandardItem("");
    statusItem->setData(m_statusGreen, Qt::DecorationRole);
    statusItem->setToolTip(tr("UNCHANGED"));
    statusItem->setEditable(false);

    // Save filename in ListBox
    QString combinedText;
    QString fileName = QFileInfo(filePath).fileName();

    if (!movie.title.isEmpty()) {
        combinedText = movie.title + "\n" + fileName;
    }
    else {
        // Split Filename before . and replace "_" with " "
        QStringList splitedTitle = fileName.split(".");
        QString onlyTitle = splitedTitle.first();
        QString noTitle = onlyTitle.replace("_"," ");
        combinedText = noTitle + "\n" + fileName;
    }
    QStandardItem *filePathItem = new QStandardItem(combinedText);
    filePathItem->setData(filePath, Qt::UserRole);
    filePathItem->setToolTip(filePath);

    // Set small font size
    QFont font = filePathItem->font();
    font.setPointSize(9);
    filePathItem->setFont(font);
    filePathItem->setTextAlignment(Qt::AlignLeft | Qt::AlignTop);

    // Add row to model
    QList<QStandardItem*> row;
    row << statusItem << filePathItem;
    m_FilelistModel->appendRow(row);
}

void MainWin::setSignalsBlocked(bool blocked) {
    // Liste aller Widgets, die Signale senden könnten
    //ui->middleGroupBox->blockSignals(blocked);

    // Oder direkt alle Kinder eines bestimmten Layouts/Widgets durchlaufen:
    const auto children = ui->middleGroupBox->findChildren<QWidget*>();
    for (QWidget* child : children) {
        child->blockSignals(blocked);
    }
}

void MainWin::displayMovie(const MovieData &movie) {
    setSignalsBlocked(true); // Stop the signals so that no change is registered.
    ui->middleGroupBox->setEnabled(true);  // Free Editfields e.g.

    // Clear Data
    clearData();
    //qDebug() << "Alles cleared";

    // Add TagWidgets
    m_castWidget->setTags(movie.cast);                  // Cast
    m_producersWidget->setTags(movie.producers);        // Producers
    m_screenwritersWidget->setTags(movie.screenwriters);// Screenwriters
    m_directorsWidget->setTags(movie.directors);        // Directors

    // Fill other fields
    ui->titleEdit->setText(movie.title);
    ui->studioEdit->setText(movie.studio);

    // Set description and set cursor to first position
    ui->descriptionText->setText(movie.description);
    ui->descriptionText->moveCursor(QTextCursor::Start);

    // Set release Date
    ui->releaseDateEdit->setDate(movie.releaseDate);

    // Insert the cover or, if none exists, a placeholder.
    QPixmap noCoverPix(":/images/noimage.svg");
    if (!movie.cover.isNull()) {
        coverPix = movie.cover;
    } else {
        qDebug() << "No cover found. Set noCoverPix";
        coverPix = noCoverPix;
    }
    ui->coverLabel->setPixmap(coverPix);

    // Set entry in ageRating combobox
    setComboEntryByString(ui->ratingComboBox,QString::number(movie.ratingScore),true);

    // Set Entry in genre combobox
    if (!movie.genre.isEmpty()) {
        setComboEntryByString(ui->genreComboBox,movie.genre,false);
    } else {
        qDebug() << "No User-Content found";
    }
    setSignalsBlocked(false);
    formatTextByLength(256);
}

void MainWin::setComboEntryByString(QComboBox *combo, QString findString, bool useData) {
    if (!combo) return;

    int index = -1;

    if (useData) {
        index = combo->findData(findString, Qt::UserRole, Qt::MatchExactly);
    } else {
        qDebug() << "NotUseData: " << findString;
        index = combo->findText(findString, Qt::MatchExactly);
    }
    combo->setCurrentIndex(index != -1 ? index : 0);
}

void MainWin::selectRowInTable(int row)
{
    // Exists row?
    if (row < 0 || row >= m_FilelistModel->rowCount())
        return;

    // Get index for Column 2 (there is the UserRole!)
    QModelIndex targetIndex = m_FilelistModel->index(row, 1);

    // Select and focus row visually
    ui->fileListTableView->setCurrentIndex(targetIndex);
    ui->fileListTableView->selectionModel()->select(targetIndex,
        QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
    ui->fileListTableView->scrollTo(targetIndex);

    // Get UserRole and trigger displayMovie
    QString filePath = targetIndex.data(Qt::UserRole).toString();

    if (m_loadedMovies.contains(filePath)) {
        // Get MovieData-Package from Map
        MovieData movie = m_loadedMovies.value(filePath);
        displayMovie(movie);
    }
}

// ***** SLOTS *****
void MainWin::onResizeMainWin(){
    // ------ Cover Label ------
    // Calculate 1/3 of the width of the box.
    int boxWidth = ui->middleGroupBox->width();
    int targetWidth = boxWidth / 3;

    // Set wdith for cover
    ui->coverLabel->setFixedWidth(targetWidth);
    ui->descriptionText->setFixedWidth(targetWidth);

    // Calculate the height based on your factor (183 / 129.5)
    int targetHeight = targetWidth * 183 / 129.5;
    ui->coverLabel->setFixedHeight(targetHeight);

    // Distance to the next element
    ui->descriptionLabel->setContentsMargins(0, 0, 0, 0);
    ui->middleLeftContainer->activate();
}

void MainWin::onSaveAllButtonClicked() {
    // Wir gehen durch alle geladenen Filme in der Map
    QMap<QString, MovieData>::iterator i;
    for (i = m_loadedMovies.begin(); i != m_loadedMovies.end(); ++i) {

        MovieData &movie = i.value(); // Referenz auf den Film
        QString path = i.key();      // Der Dateipfad

        //if (movie.status) {
            // 1. In Datei schreiben (deine Logik)
            // if (movie.saveToFile(path)) {
            //     // 2. Status zurücksetzen
            //     movie.isDirty = false;

            //     // 3. UI aktualisieren: Wir müssen die Zeile in der Tabelle finden
            //     updateRowIconToGreen(path);
            // }
        //}
    }
}

void MainWin::onSearchTableViewDoubleClicked(const QModelIndex &index){
    if (!index.isValid())
        return;

    // Index der ersten Spalte in der gleichen Zeile
    QModelIndex idIndex = index.sibling(index.row(), 0);
    QVariant idData = idIndex.data(Qt::UserRole);

    if (idData.isValid()) {
        int id = idData.toInt();
        qDebug() << "ID der ausgewählten Zeile:" << id;
    } else {
        qDebug() << "Keine ID gefunden!";
    }
}

void MainWin::onFilelistTableViewClicked(const QModelIndex &index){
    if (!index.isValid()) return;

    // Get path from UserRole
    QString filePath = index.model()->data(index.model()->index(index.row(), 1), Qt::UserRole).toString();

    // Fill map with details
    if (m_loadedMovies.contains(filePath)) {
        // Get MovieData Object
        MovieData &movie = m_loadedMovies[filePath];

        // Display movie data
        displayMovie(movie);
    }
}

void MainWin::onAddFilesButtonClicked()
{
    // Open filedialog
    QStringList files = QFileDialog::getOpenFileNames(
        this,                              // Parent-Widget
        tr("CHOOSE_MOVIE"),                // Title of dialog
        QString(),                         // Start-Directory (empty = Standard)
        tr("MOVIE_FILES") + " (*.mp4 *.mkv *.avi);;" + tr("Alle Dateien") + "(*.*)" // Filter
    );

    // Check if user cancel operation
    if (!files.isEmpty()) {
        // Put filelist to Model
        addFilesToModel(files);

        qDebug() << files.size() << "Files over dialog added.";
    }
}

void MainWin::onRemoveFilesButtonClicked()
{
    // Get rows from TableView
    QModelIndexList selectedRows = ui->fileListTableView->selectionModel()->selectedRows();

    if (selectedRows.isEmpty()) return;

    // Security check (optional)
    auto answer = QMessageBox::question(this, tr("REMOVE"),
        QString("%1 Film(e) wirklich entfernen?").arg(selectedRows.size())
    );
    if (answer != QMessageBox::Yes) return;

    // To prevent the indexes from becoming invalid when deleted (because they shift down),
    // we need to sort them and delete them from BACK to the front.
    std::sort(selectedRows.begin(), selectedRows.end(), [](const QModelIndex &a, const QModelIndex &b) {
        return a.row() > b.row();
    });

    // Removing Movie
    for (const QModelIndex &index : std::as_const(selectedRows)) {
        int currentRow = index.row(); // Get the current row from selector

        const QAbstractItemModel *model = index.model();       // Get a pointer to the model (read-only)
        QModelIndex idx0 = model->index(currentRow, 1);        // Create an index for the row and second column
        QString movieKey = idx0.data(Qt::UserRole).toString(); // Retrieve the UserRole data as a QString

        // Remove from Map and clear UI
        removeMovie(movieKey);

        // Remove entry from TableView
        ui->fileListTableView->model()->removeRow(index.row());
    }

    // Set the selection to the first entry.
    if (ui->fileListTableView->model()->rowCount() > 0) {
        QModelIndex firstIndex = ui->fileListTableView->model()->index(0, 0);
        ui->fileListTableView->setCurrentIndex(firstIndex);
    } else {
        // If the list is empty: clear and lock the UI
        clearData();
        ui->middleGroupBox->setEnabled(false);
    }
}

void MainWin::onSearchButtonClicked()
{
    // Logik für "Search":
    qDebug() << "Action SEARCH triggered...";
    QString searchText = ui->searchEdit->text();
    qDebug() << "Search Text: " << searchText;
    if(searchText == "") {
        ui->statusBar->showMessage(tr("PLEASE_ENTER_SEARCH_TERM"), 10000);
    }
    else
    {
        searchText.replace(" ", "+");
        QString url_search = "https://api.themoviedb.org/3/search/movie?query="+ searchText+"&language=de-DE";
        startApiRequest(url_search);
    }
}

// ***** NETWORK ****
void MainWin::startImageDownload(const QUrl &imageUrl, std::function<void(const QPixmap &pixmap)> callback)
{
    qDebug() << "1. Download-Funktion betreten für:" << imageUrl;
    QNetworkRequest request(imageUrl);
    QNetworkReply *reply = m_ImageNetworkManager->get(request);

    // Verbinden des Reply-Signals direkt mit dem verarbeitenden Lambda
    QObject::connect(reply, &QNetworkReply::finished,
        this, [this, callback, reply]() { // 'reply' wird für deleteLater captured

             QNetworkReply *finishedReply = qobject_cast<QNetworkReply*>(sender());

             if (!finishedReply) {
                 return;
             }

             if (finishedReply->error() != QNetworkReply::NoError) {
                 qWarning() << "Download Fehler für" << finishedReply->url().toString()
                            << ":" << finishedReply->errorString();
                 finishedReply->deleteLater();
                 // On error no run of callback
                 return;
             }
             QByteArray imageData = finishedReply->readAll();
             QPixmap pixmap;

             if (pixmap.loadFromData(imageData)) {
                 // if SUCCESS: run callback and deliver Pixmap
                 callback(pixmap);
             } else {
                 qWarning() << "FEHLER: Bilddaten konnten nicht geladen werden im generischen Handler!";
             }

             // Ressourcen freigeben
             finishedReply->deleteLater();
    });
}

void MainWin::onImageDownloadFinished(QNetworkReply *reply)
{
    qDebug() << "Download Finsh Function";
    // Get URL from Mapping
    QUrl originalUrl = reply->property("OriginalUrl").toUrl();

    // Error handling
    if (reply->error() != QNetworkReply::NoError) {
        // Signal errors and abort
        emit imageDownloadFailed(originalUrl, reply->errorString());
        return;
    }

    // Read data
    QByteArray imageData = reply->readAll();

    // Create QPixmap from Raw data
    QPixmap pixmap;

    if (pixmap.loadFromData(imageData)) {
        // SUCCESS: Send QPixmap aund URL over signal
        qDebug() << "Download Finished";
        emit imageDownloadFinished(pixmap, originalUrl, reply);
    } else {
        // Error parsing Image data
        emit imageDownloadFailed(originalUrl, "Invalid image format or empty data.");
    }
}

void MainWin::startApiRequest(const QUrl &url)
{
    // Define Request
    QNetworkRequest request(url);

    // Set Authentication-Token as header
    const QByteArray authToken = "eyJhbGciOiJIUzI1NiJ9.eyJhdWQiOiIxODM3ZjY1Njg1YjNhOWYwM2MyN2Y5Mjg3NDdkZjMxNSIsIm5iZiI6MTc2NTAyNDc1Mi43ODksInN1YiI6IjY5MzQyM2YwMzdmYzMyNmM2ZjQ4ZTY5ZCIsInNjb3BlcyI6WyJhcGlfcmVhZCJdLCJ2ZXJzaW9uIjoxfQ.B0HZMFNnxC8BBGt_dziS39kbG23YWMcpOdBs8px3lt4";
    request.setRawHeader("Authorization", "Bearer " + authToken);

    // Send Get-Request
    m_ApiNetworkManager->get(request);

    // qDebug() << "API-Request startet. Saved in:" << m_tempFile->fileName();
}

void MainWin::onApiRequestFinished(QNetworkReply *reply)
{
    // Get URL for parsing
    QString requestUrl = reply->url().toString();

    // Error handling
    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "Network-Error: " << reply->errorString();
        reply->deleteLater();
        return;
    }

    // Read Data from request and save it in JsonDoc
    QByteArray responseData = reply->readAll(); 
    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);

    if (requestUrl.contains("/genre/movie/list"))
    {
        // Ruft die Funktion auf, die die Genre-ComboBox/Listbox füllt
        fillGenreComboBox(jsonDoc);

    } else if (requestUrl.contains("/search/movie"))
    {
        // Ruft die Funktion auf, die die Film-ListView füllt
        fillSearchTableView(jsonDoc);
    }

    // free request
    reply->deleteLater();
}

// ***** HANDLES *****
void MainWin::handleExitApplication()
{
    // Hier fügen Sie Ihre Logik ein (z.B. Speicherabfrage)

    // Quit application
    QApplication::quit();
}

void MainWin::handleInfoButton_Clicked()
{
    qDebug() << "Action INFO triggered...";
    ui->stackedWidget->setCurrentIndex(2);
    // set toolbar
    ui->mainToolBar->setVisible(false);
    ui->settingsToolBar->setVisible(false);
    ui->infoToolBar->setVisible(true);
}

void MainWin::handleSettingsButton_Clicked()
{
    qDebug() << "Action SETTINGS triggered...";
    ui->stackedWidget->setCurrentIndex(1);
    // set toolbar
    ui->mainToolBar->setVisible(false);
    ui->infoToolBar->setVisible(false);
    ui->settingsToolBar->setVisible(true);
}

void MainWin::handleCloseButton_Clicked()
{
    qDebug() << "Action INFO triggered...";
    ui->stackedWidget->setCurrentIndex(0);
    // set toolbar
    ui->mainToolBar->setVisible(true);
    ui->infoToolBar->setVisible(false);
}

void MainWin::handleSaveSettingsButton_Clicked() {
    qDebug() << "Action Save Setting triggered.";
    settings.setValue("General/AgeRatingSystem", ui->ratingSettingsComboBox->currentData().toString());
    settings.sync();
    if (settings.status() == QSettings::NoError) {
        // Erfolg
        ui->statusBar->showMessage(tr("Einstellungen erfolgreich gespeichert."), 30000); // 3 Sek. sichtbar
    } else {
        // Fehler (z.B. Datei schreibgeschützt)
        ui->statusBar->showMessage(tr("Fehler: Konnte Einstellungen nicht speichern!"), 50000);
    }
}
// ***** EVENTS *****
void MainWin::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    onResizeMainWin();
}

void MainWin::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    onResizeMainWin();
}

MainWin::~MainWin()
{
    delete ui;
}
