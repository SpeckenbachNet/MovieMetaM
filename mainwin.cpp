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
#include "mediainfom.h"

#include <QtMath>

MainWin::MainWin(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWin)
{
    ui->setupUi(this);

    // Get countryCode from INI-File for AgeRating System
    countryCode = settings.value("General/AgeRatingSystem").toString();
    qDebug() << "Country Code at beginning: " << countryCode;
    dataLanguage = Languages::dataLanguage();

    // Initialize
    initializeForm();
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

    const auto &systems = AgeRatings::systems();
    auto it = systems.constFind(countryCode);
    if (it == systems.constEnd()) {
        qDebug() << "Country does not exist";
        return;
    }

    ui->ratingComboBox->addItem("");

    const CountryRatingSystem &system = it.value();
    for (const RatingEntry &entry : std::as_const(system.ratings)) {
        QIcon icon = createAgeRatingIcon(entry.key, entry.color, entry.shape, entry.style);

        ui->ratingComboBox->addItem(icon, entry.label, entry.score);
        int lastIndex = ui->ratingComboBox->count() - 1;
        ui->ratingComboBox->setItemData(lastIndex, entry.description.isEmpty() ? entry.label : entry.description,
                                        Qt::ToolTipRole);
        ui->ratingComboBox->setItemData(lastIndex, entry.code, kRatingCodeRole);
        ui->ratingComboBox->setItemData(lastIndex, system.technicalID, kRatingSystemRole);
    }

    ui->ratingComboBox->setIconSize(QSize(kRatingIconSize, kRatingIconSize));
}

// The system in the file decides which steps the list shows; the settings only apply to movies
// without an age rating. An entry the app doesn't know stays as it is, on top of the list.
void MainWin::showMovieRating(const MovieData &movie)
{
    const QString fileCountry = AgeRatings::countryOfSystem(movie.ratingSystem);
    fillRatingComboBox(fileCountry.isEmpty() ? countryCode : fileCountry);

    if (movie.ratingSystem.isEmpty()) {
        ui->ratingComboBox->setCurrentIndex(0);
        return;
    }

    // By code; older texts (e.g. Apple's "ab 12 Jahren") by the score of the same system
    int index = -1;
    if (!fileCountry.isEmpty()) {
        index = ui->ratingComboBox->findData(movie.ratingText, kRatingCodeRole);
        if (index < 0) {
            index = ui->ratingComboBox->findData(movie.ratingScore, Qt::UserRole);
        }
    }

    if (index < 0) {
        index = qMin(1, ui->ratingComboBox->count());   // below the empty entry
        ui->ratingComboBox->insertItem(index, movie.ratingSystem + ": " + movie.ratingText, movie.ratingScore);
        ui->ratingComboBox->setItemData(index, movie.ratingText, kRatingCodeRole);
        ui->ratingComboBox->setItemData(index, movie.ratingSystem, kRatingSystemRole);
    }
    ui->ratingComboBox->setCurrentIndex(index);
}

void MainWin::fillRatingSettingsComboBox()
{
    ui->ratingSettingsComboBox->clear();
    const auto &systems = AgeRatings::systems();
    for (auto it = systems.cbegin(); it != systems.cend(); ++it) {
        ui->ratingSettingsComboBox->addItem(it.value().displayName(), it.key());
    }
    ui->ratingSettingsComboBox->model()->sort(0);
    setComboEntryByString(ui->ratingSettingsComboBox, countryCode, true);
    fillRatingPreview(countryCode);
}

void MainWin::fillRatingPreview(const QString &countryCode)
{
    // Remove the badges of the system shown before
    QLayout *layout = ui->ratingPreviewContainer->layout();
    while (QLayoutItem *item = layout->takeAt(0)) {
        delete item->widget();
        delete item;
    }

    const auto &systems = AgeRatings::systems();
    auto it = systems.constFind(countryCode);
    if (it != systems.constEnd()) {
        // Badges in the resolution of the screen, the text of the step as tooltip
        const qreal dpr = devicePixelRatioF();
        for (const RatingEntry &entry : std::as_const(it.value().ratings)) {
            auto *badge = new QLabel(ui->ratingPreviewContainer);
            badge->setPixmap(createAgeRatingIcon(entry.key, entry.color, entry.shape, entry.style)
                                 .pixmap(QSize(kRatingPreviewSize, kRatingPreviewSize), dpr));
            badge->setToolTip(entry.description.isEmpty() ? entry.label : entry.description);
            layout->addWidget(badge);
        }
    }
    static_cast<QHBoxLayout *>(layout)->addStretch();
}

namespace {

// Outline of a regular polygon with n corners in rect, the first corner at startAngle (degrees,
// 0 = right, counted clockwise like the screen)
QPolygonF regularPolygon(const QRectF &rect, int corners, qreal startAngle)
{
    QPolygonF polygon;
    const QPointF c = rect.center();
    for (int i = 0; i < corners; ++i) {
        const qreal a = qDegreesToRadians(startAngle + i * 360.0 / corners);
        polygon << QPointF(c.x() + rect.width() / 2 * qCos(a), c.y() + rect.height() / 2 * qSin(a));
    }
    return polygon;
}

// Five-pointed star around center, the top point up; a thick star (inner corners at 62 % of
// the outer radius), so there is room for the code in the middle
QPolygonF star(const QPointF &center, qreal radius)
{
    QPolygonF polygon;
    for (int i = 0; i < 10; ++i) {
        const qreal r = (i % 2 == 0 ? 1.0 : 0.62) * radius;
        const qreal a = qDegreesToRadians(-90.0 + i * 36.0);
        polygon << QPointF(center.x() + r * qCos(a), center.y() + r * qSin(a));
    }
    return polygon;
}

} // namespace

QIcon MainWin::createAgeRatingIcon(QString age, QColor color, QString shape, QString style) {
    // Square canvas in a higher resolution than shown (kRatingIconSize), so it stays
    // sharp on Retina screens
    int size = 64;
    QPixmap pixmap(size, size);
    pixmap.fill(Qt::transparent); // Make the background transparent

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    // Drawing rect (padding for the border)
    int pad = size / 16;
    QRectF rect(pad, pad, size - 2 * pad, size - 2 * pad);
    shape = shape.toLower();

    // Outline of the badge; textRect: the room inside the shape where the code fits
    // (altRect: a second room to try, the larger font wins)
    QPainterPath path;
    QRectF textRect = rect.adjusted(size * 0.08, 0, -size * 0.08, 0);
    QRectF altRect;
    if (shape == "circle") {
        path.addEllipse(rect);
        textRect = rect.adjusted(size * 0.12, 0, -size * 0.12, 0);
    } else if (shape == "diamond") {
        path.addPolygon(regularPolygon(rect, 4, -90));
        // Widest in the middle: a flat line there may be almost as wide as the diamond (long
        // codes like "21+"); short codes like "F" get larger in the square in the middle
        textRect = QRectF(rect.center().x() - rect.width() * 0.34, rect.center().y() - rect.height() * 0.15,
                          rect.width() * 0.68, rect.height() * 0.30);
        altRect = rect.adjusted(size * 0.22, size * 0.22, -size * 0.22, -size * 0.22);
    } else if (shape == "triangle") {
        // Equilateral like the logos (height = 0.866 × width), centered in the height; the code
        // sits in the lower, wider part and may touch the border
        const qreal height = rect.width() * 0.866;
        const qreal top = rect.top() + (rect.height() - height) / 2;
        path.addPolygon(QPolygonF(QList<QPointF>{QPointF(rect.center().x(), top),
                                                 QPointF(rect.right(), top + height),
                                                 QPointF(rect.left(), top + height)}));
        textRect = QRectF(rect.center().x() - rect.width() * 0.31, top + height * 0.48,
                          rect.width() * 0.62, height * 0.45);
    } else if (shape == "octagon") {
        path.addPolygon(regularPolygon(rect, 8, 22.5));
    } else if (shape == "hexagon") {
        path.addPolygon(regularPolygon(rect, 6, 0));
        textRect = rect.adjusted(size * 0.16, 0, -size * 0.16, 0);
    } else if (shape == "star") {
        // A star is 1.90 radii wide and 1.81 high (the lower points are higher than the
        // bottom): as large as the rect allows, centered in the height
        const qreal radius = qMin(rect.width() / 1.902, rect.height() / 1.809);
        const QPointF center(rect.center().x(), rect.top() + (rect.height() - 1.809 * radius) / 2 + radius);
        path.addPolygon(star(center, radius));
        // The code may reach over the arms: it gets a contour in the star color (see below)
        textRect = QRectF(center.x() - 0.68 * radius, center.y() - 0.32 * radius, 1.36 * radius, 0.72 * radius);
    } else {   // square
        path.addRoundedRect(rect, size * 5 / 32, size * 5 / 32);
        // Long codes ("NC-17", "MA15+") may reach up to the border
        textRect = rect.adjusted(size * 0.03, 0, -size * 0.03, 0);
    }
    path.closeSubpath();

    // Style: filled = colored area, black border, text black or white (whichever is easier to
    // read); outline = white area, border and text in the color; light = white area, black
    // border, text in the color; dark = dark area, text in the color
    QColor fill = color;
    QColor border = Qt::black;
    QColor text;
    qreal borderWidth = size / 16.0;
    if (style == "outline") {
        fill = Qt::white;
        border = color;
        text = color;
        borderWidth = size / 11.0;
    } else if (style == "light") {
        fill = Qt::white;
        text = color;
    } else if (style == "dark") {
        fill = QColor(0x31, 0x30, 0x30);
        text = color;
    } else {
        const qreal luminance = 0.299 * color.red() + 0.587 * color.green() + 0.114 * color.blue();
        text = luminance > 160 ? Qt::black : Qt::white;
    }
    painter.setBrush(fill);
    painter.setPen(QPen(border, borderWidth));
    painter.drawPath(path);

    // Font as large as the room inside the shape allows (long codes like "PG-13" get smaller)
    QFont font = painter.font();
    font.setBold(true);
    auto fittingSize = [&](const QRectF &room) {
        int pixelSize = qRound(size * 0.45);
        for (; pixelSize > size / 6; --pixelSize) {
            font.setPixelSize(pixelSize);
            const QFontMetricsF metrics(font);
            if (metrics.horizontalAdvance(age) <= room.width() && metrics.capHeight() <= room.height()) {
                break;
            }
        }
        return pixelSize;
    };
    int pixelSize = fittingSize(textRect);
    if (!altRect.isNull() && fittingSize(altRect) > pixelSize) {
        textRect = altRect;
        pixelSize = fittingSize(altRect);
    }
    font.setPixelSize(pixelSize);
    painter.setFont(font);
    if (shape == "star") {
        // Contour in the star color: readable where the code reaches over the arms, on light
        // and dark backgrounds
        const QFontMetricsF metrics(font);
        QPainterPath textPath;
        textPath.addText(textRect.center().x() - metrics.horizontalAdvance(age) / 2,
                         textRect.center().y() + metrics.capHeight() / 2, font, age);
        // Bold glyphs consist of overlapping parts ("+", "P"): odd-even filling would leave
        // the overlaps empty
        textPath.setFillRule(Qt::WindingFill);
        painter.strokePath(textPath, QPen(fill, size / 10.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter.fillPath(textPath, text);
    } else {
        painter.setPen(text);
        painter.drawText(textRect, Qt::AlignCenter, age);
    }
    painter.end();

    // Same picture when selected: Qt would tint it with the accent color in the open list
    QIcon icon;
    icon.addPixmap(pixmap, QIcon::Normal);
    icon.addPixmap(pixmap, QIcon::Selected);
    return icon;
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

    if (countryCode != ui->ratingSettingsComboBox->currentData(Qt::UserRole)
        || Languages::appLanguage() != ui->languageSettingsComboBox->currentData().toString()
        || dataLanguage != ui->dataLanguageSettingsComboBox->currentData().toString()) {
        settingsChanged = true;
        m_saveSettingsBtn->setEnabled(true);
    } else {
        settingsChanged = false;
        m_saveSettingsBtn->setEnabled(false);
    }
}

void MainWin::updateStatus (int row, QString status) {
    QIcon m_status;
    if (status == "modified") m_status = m_statusYellow;
    else if (status == "unchanged") m_status = m_statusGreen;
    QStandardItem *movieItem = m_FilelistModel->item(row, 0);
    if (movieItem) {
        movieItem->setData(m_status, Qt::DecorationRole);
        const QString statusText = status == "modified" ? tr("Changed, saving needed") : tr("Unchanged");
        movieItem->setToolTip(movieItem->data(Qt::UserRole).toString() + "\n" + statusText);
    }
}

void MainWin::formatTextByLength(int limit)
{
    // 1. Vorbereitungen: Signale blockieren & Scroll-Position merken
    ui->descriptionText->blockSignals(true);
    int scrollPos = ui->descriptionText->verticalScrollBar()->value();

    // Get the widget's current cursor (holds the user's position)
    QTextCursor userCursor = ui->descriptionText->textCursor();
    int oldPos = userCursor.position();

    // 2. Daten laden
    QString text = ui->descriptionText->toPlainText();
    int totalLength = text.length();

    // Create a working cursor for the formatting
    QTextCursor editCursor(ui->descriptionText->document());

    // Gets the default text color of the current system theme
    QColor defaultColor = ui->descriptionText->palette().color(QPalette::Text);

    // --- Format-Definitionen ---
    QTextCharFormat defaultFormat;
    defaultFormat.setForeground(QBrush(defaultColor));

    QTextCharFormat warningFormat;
    warningFormat.setForeground(QBrush(Qt::red));

    // --- 3. Formatierung anwenden ---

    // A. Reset the whole text to the default color first
    editCursor.select(QTextCursor::Document);
    editCursor.setCharFormat(defaultFormat);

    // B. If the limit is exceeded, color the text red from the limit on
    if (totalLength > limit) {
        editCursor.setPosition(limit);
        editCursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
        editCursor.mergeCharFormat(warningFormat);
    }

    // --- 4. Restore cursor and UI ---

    // Restore the user's original cursor
    // qMin makes sure we do not end up outside the text
    userCursor.setPosition(qMin(oldPos, totalLength));
    ui->descriptionText->setTextCursor(userCursor);

    // Put the scroll bar back to its old position
    ui->descriptionText->verticalScrollBar()->setValue(scrollPos);

    ui->descriptionText->blockSignals(false);
}

void MainWin::saveMovieDetails()
{
    // Get the current list of actors
    //QStringList currentActors = m_actorWidget->getTags();

    // Get the current list of genres
    //QStringList currentGenres = m_genreWidget->getTags();

    //qDebug() << "Saved cast:" << currentActors;
    //qDebug() << "Saved genres:" << currentGenres;

    // The logic to store these lists follows here
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
    work.description = ui->descriptionText->toPlainText();

    // DateEdit
    // A movie without date shows the default of the field (clearData): not a change
    const bool noDateUnchanged = !original.releaseDate.isValid()
                                 && ui->releaseDateEdit->date() == QDate(1972, 1, 1);
    if (!noDateUnchanged && ui->releaseDateEdit->date() != original.releaseDate) {
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

    // Age rating: only system and step count (an older text of the same step is no change)
    int currentScore = ui->ratingComboBox->currentData(Qt::UserRole).toInt();
    QString currentSystem = ui->ratingComboBox->currentData(kRatingSystemRole).toString();
    if (currentScore != original.ratingScore || currentSystem != original.ratingSystem) {
        qDebug() << "AgeRating has changed.";
        hasChanged = true;
    }
    work.ratingScore = currentScore;
    work.ratingSystem = currentSystem;
    work.ratingText = ui->ratingComboBox->currentData(kRatingCodeRole).toString();

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
        qDebug() << "The dataset is the same again!";
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

void MainWin::fillSearchTableView(const QJsonDocument &jsonDoc)
{
    if (jsonDoc.isNull() || !jsonDoc.isObject()) {
        qWarning() << "Error: Invalid or empty JSON document for movie details.";
        return;
    }

    QJsonObject rootObj = jsonDoc.object();

    if (!rootObj.contains("results") || !rootObj.value("results").isArray()) {
        qWarning() << "'Results' is not an array or is missing";
        return;
    }

    QJsonArray jsonArray = rootObj.value("results").toArray();

    if (jsonArray.isEmpty()) return;

    m_SearchModel->clear();   // created once in initializeForm()
    // A new search: posters still loading for the old one are dropped (see the callback)
    const int generation = ++m_searchGeneration;

    for (const QJsonValue &val : std::as_const(jsonArray)) {
        QJsonObject obj = val.toObject();

        // One entry: title and year as text, TMDB id in UserRole, poster as icon (loaded below)
        QString year = obj.value("release_date").toString().left(4);
        QString text = obj.value("title").toString();
        if (!year.isEmpty()) {
            text += "\n" + year;
        }
        QStandardItem *item = new QStandardItem(text);
        item->setData(obj.value("id").toInt(), Qt::UserRole);
        m_SearchModel->appendRow(item);

        // Poster: loaded asynchronously; the persistent index follows the entry and becomes
        // invalid when the model is cleared by a new search
        QString posterImage = obj.value("poster_path").toString();
        if (posterImage.isEmpty()) {
            continue;
        }
        QUrl imageUrl("https://image.tmdb.org/t/p/w500" + posterImage);
        QPersistentModelIndex itemIndex(item->index());
        startImageDownload(imageUrl, [this, itemIndex, generation](const QPixmap &pixmap) {
            if (generation != m_searchGeneration || !itemIndex.isValid()) {
                return;   // result of an older search
            }
            // Scaled in the resolution of the screen, so it stays sharp; same picture when
            // selected (Qt would tint it with the accent color)
            const qreal dpr = ui->searchTableView->devicePixelRatioF();
            QPixmap poster = pixmap.scaled(kPosterSize * dpr, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            poster.setDevicePixelRatio(dpr);
            QIcon icon;
            icon.addPixmap(poster, QIcon::Normal);
            icon.addPixmap(poster, QIcon::Selected);
            m_SearchModel->setData(itemIndex, icon, Qt::DecorationRole);
            m_SearchModel->setData(itemIndex, pixmap, kPosterRole);   // full resolution for the preview
        });
    }
}

void MainWin::fillGenreComboBox(const QJsonDocument &jsonDoc)
{
    if (jsonDoc.isNull() || !jsonDoc.isObject()) {
        qWarning() << "Error: Invalid or empty JSON document for genres.";
        return;
    }

    QJsonObject rootObj = jsonDoc.object();

    if (!rootObj.contains("genres") || !rootObj.value("genres").isArray()) {
        qWarning() << "'Genres' is not an array or is missing";
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

    // Filled again after a change of the language: keep the genre of the current movie
    // selected, without a "changed"
    const QSignalBlocker blocker(ui->genreComboBox);
    const QString currentGenre = ui->genreComboBox->currentText();
    ui->genreComboBox->clear();
    ui->genreComboBox->addItem("");
    ui->genreComboBox->addItems(genreNames);
    setComboEntryByString(ui->genreComboBox, currentGenre, false);
}

void MainWin::requestGenres()
{
    startApiRequest(QUrl("https://api.themoviedb.org/3/genre/movie/list?language=" + dataLanguage));
}

void MainWin::addFilesToModel(const QStringList &filePaths)
{
    if (filePaths.isEmpty()) return;

    int firstNewRowIndex = -1;
    int skippedCount = 0;

    for (const QString &path : filePaths) {
        // Only supported file types (MP4 for now)
        if (!MediaInfoM::isSupported(path)) {
            qDebug() << "Unsupported file type, skip:" << path;
            ++skippedCount;
            continue;
        }

        // Check if path is already in map
        if (m_loadedMovies.contains(path)) {
            qDebug() << "File already exists, skip:" << path;
            continue; // Skip to the next file in the list
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

    if (skippedCount > 0) {
        ui->statusBar->showMessage(tr("%n file(s) skipped: only MP4 is supported.", "", skippedCount), 10000);
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

    // One entry per movie: title + file name as text, status as icon, file path in UserRole
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
    QStandardItem *movieItem = new QStandardItem(combinedText);
    movieItem->setData(filePath, Qt::UserRole);
    movieItem->setData(m_statusGreen, Qt::DecorationRole);
    movieItem->setToolTip(filePath + "\n" + tr("Unchanged"));
    movieItem->setEditable(false);


    // Add row to model
    m_FilelistModel->appendRow(movieItem);
}

void MainWin::setSignalsBlocked(bool blocked) {
    // List of all widgets that could send signals
    //ui->middleGroupBox->blockSignals(blocked);

    // Or go through all children of a certain layout/widget directly:
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

    // Insert the cover; without one the label shows its placeholder
    ui->coverLabel->setCover(movie.cover);

    // Fill the age rating combobox with the system of the file and select its entry
    showMovieRating(movie);

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
    if (index == -1 && combo->isEditable() && !findString.isEmpty()) {
        combo->setCurrentText(findString);   // free text, not in the list
        return;
    }
    combo->setCurrentIndex(index != -1 ? index : 0);
}

void MainWin::selectRowInTable(int row)
{
    // Exists row?
    if (row < 0 || row >= m_FilelistModel->rowCount())
        return;

    // The only column holds the file path in UserRole
    QModelIndex targetIndex = m_FilelistModel->index(row, 0);

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
void MainWin::onSaveAllButtonClicked() {
    // Go through all loaded movies in the map
    QMap<QString, MovieData>::iterator i;
    for (i = m_loadedMovies.begin(); i != m_loadedMovies.end(); ++i) {

        MovieData &movie = i.value(); // reference to the movie
        QString path = i.key();      // the file path

        //if (movie.status) {
            // 1. Write to the file
            // if (movie.saveToFile(path)) {
            //     // 2. Reset the status
            //     movie.isDirty = false;

            //     // 3. Update the UI: find the row in the table
            //     updateRowIconToGreen(path);
            // }
        //}
    }
}

void MainWin::onSearchTableViewDoubleClicked(const QModelIndex &index){
    if (!index.isValid())
        return;

    // Index of the first column in the same row
    QModelIndex idIndex = index.sibling(index.row(), 0);
    QVariant idData = idIndex.data(Qt::UserRole);

    if (idData.isValid()) {
        int id = idData.toInt();
        qDebug() << "ID of selected Row:" << id;
    } else {
        qDebug() << "No ID found!";
    }
}

void MainWin::onCurrentMovieChanged(const QModelIndex &index){
    if (!index.isValid()) return;

    // Get path from UserRole
    QString filePath = index.model()->data(index.model()->index(index.row(), 0), Qt::UserRole).toString();

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
    // Filter from the supported file types, e.g. "*.mp4 *.m4v"
    QStringList patterns;
    for (const QString &suffix : MediaInfoM::supportedSuffixes()) {
        patterns << "*." + suffix;
    }

    // Open filedialog
    QStringList files = QFileDialog::getOpenFileNames(
        this,                              // Parent-Widget
        tr("Choose movies"),                // Title of dialog
        QString(),                         // Start-Directory (empty = Standard)
        tr("Movie files") + " (" + patterns.join(' ') + ")" // Filter
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
    auto answer = QMessageBox::question(this, tr("Remove"),
        tr("Really remove %n movie(s)?", "", selectedRows.size())
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
        QModelIndex idx0 = model->index(currentRow, 0);        // The only column holds the file path
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
        onCurrentMovieChanged(firstIndex);   // currentChanged does not fire if row 0 stays current
    } else {
        // If the list is empty: clear and lock the UI
        clearData();
        ui->middleGroupBox->setEnabled(false);
    }
}

void MainWin::onSearchButtonClicked()
{
    // Logic for "Search":
    qDebug() << "Action SEARCH triggered...";
    QString searchText = ui->searchEdit->text();
    qDebug() << "Search Text: " << searchText;
    if(searchText == "") {
        ui->statusBar->showMessage(tr("Please enter a search term"), 10000);
    }
    else
    {
        searchText.replace(" ", "+");
        QString url_search = "https://api.themoviedb.org/3/search/movie?query="+ searchText+"&language=" + dataLanguage;
        startApiRequest(url_search);
    }
}

// ***** NETWORK ****
void MainWin::startImageDownload(const QUrl &imageUrl, std::function<void(const QPixmap &pixmap)> callback,
                                 std::function<void()> onError)
{
    qDebug() << "Enter Download function:" << imageUrl;
    QNetworkRequest request(imageUrl);
    QNetworkReply *reply = m_ImageNetworkManager->get(request);

    // Connect the reply signal directly to the processing lambda
    QObject::connect(reply, &QNetworkReply::finished,
        this, [this, callback, onError, reply]() { // 'reply' is captured for deleteLater

             QNetworkReply *finishedReply = qobject_cast<QNetworkReply*>(sender());

             if (!finishedReply) {
                 return;
             }

             if (finishedReply->error() != QNetworkReply::NoError) {
                 qWarning() << "Download Error for " << finishedReply->url().toString()
                            << ":" << finishedReply->errorString();
                 finishedReply->deleteLater();
                 // On error no run of callback
                 if (onError) onError();
                 return;
             }
             QByteArray imageData = finishedReply->readAll();
             QPixmap pixmap;

             if (pixmap.loadFromData(imageData)) {
                 // if SUCCESS: run callback and deliver Pixmap
                 callback(pixmap);
             } else {
                 qWarning() << "ERROR: Image data could not be loaded in the generic handler!";
                 if (onError) onError();
             }

             // Ressourcen freigeben
             finishedReply->deleteLater();
    });
}

void MainWin::onImageDownloadFinished(QNetworkReply *reply)
{
    qDebug() << "Download finsh function";
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
    // Token comes from tmdb_token.txt at build time (see CMakeLists.txt)
    const QByteArray authToken = QByteArrayLiteral(TMDB_TOKEN);
    if (authToken.isEmpty()) {
        ui->statusBar->showMessage(tr("No TMDB token: the movie search is not available."), 10000);
        return;
    }
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
        // Calls the function that fills the genre combo box
        fillGenreComboBox(jsonDoc);

    } else if (requestUrl.contains("/search/movie"))
    {
        // Calls the function that fills the movie list
        fillSearchTableView(jsonDoc);
    }

    // free request
    reply->deleteLater();
}

// ***** HANDLES *****
// Every page brings its own toolbar, so switching the page is all there is to do
void MainWin::showPage(QWidget *page)
{
    ui->stackedWidget->setCurrentWidget(page);
}

void MainWin::handleSaveSettingsButton_Clicked() {
    qDebug() << "Action Save Setting triggered.";
    const QString newAppLanguage = ui->languageSettingsComboBox->currentData().toString();
    const bool appLanguageChanged = newAppLanguage != Languages::appLanguage();
    const QString newDataLanguage = ui->dataLanguageSettingsComboBox->currentData().toString();

    settings.setValue("General/AgeRatingSystem", ui->ratingSettingsComboBox->currentData().toString());
    settings.setValue("language", newAppLanguage);
    settings.setValue("General/DataLanguage", newDataLanguage);
    settings.sync();
    if (settings.status() == QSettings::NoError) {
        // Erfolg; the language of the app only changes with the next start
        ui->statusBar->showMessage(appLanguageChanged
                                       ? tr("Settings saved. The language changes after a restart.")
                                       : tr("Settings saved."), 30000); // 30 s visible
    } else {
        // Error (e.g. file is read-only)
        ui->statusBar->showMessage(tr("Error: could not save the settings!"), 50000);
    }

    // The new system applies at once: show the current movie again (its list may change)
    countryCode = ui->ratingSettingsComboBox->currentData().toString();
    settingsChanged = false;
    m_saveSettingsBtn->setEnabled(false);
    const QString movieKey = ui->fileListTableView->movieKey();
    if (m_loadedMovies.contains(movieKey)) {
        displayMovie(m_loadedMovies.value(movieKey));
    } else {
        fillRatingComboBox(countryCode);
    }

    // Genres in the new language of the movie data
    if (newDataLanguage != dataLanguage) {
        dataLanguage = newDataLanguage;
        requestGenres();
    }
}

// Window size and column widths for the next start
void MainWin::closeEvent(QCloseEvent *event)
{
    settings.setValue("Window/Geometry", saveGeometry());
    settings.setValue("Window/Splitter", ui->splitter->saveState());
    QMainWindow::closeEvent(event);
}

MainWin::~MainWin()
{
    delete ui;
}
