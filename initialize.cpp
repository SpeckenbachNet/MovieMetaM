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
#include "ui_mainwin.h"

// ***** INITIALIZE *****
void MainWin::initializeForm() {

    // Style headers
    QString cssHeadline = "QLabel { background-color: grey; "
                          "font-weight: bold; "
                          "padding: 6px; }";
    ui->leftLabel->setStyleSheet(cssHeadline);
    ui->middleLabel->setStyleSheet(cssHeadline);
    ui->rightLabel->setStyleSheet(cssHeadline);
    ui->settingsHeaderLabel->setStyleSheet(cssHeadline);

    // fixed size of TMDB Image
    QPixmap picTmdb(":/images/tmdb.svg");
    ui->tmdbImageLabel->setPixmap(picTmdb);
    ui->tmdbImageLabel->setScaledContents(true);
    ui->tmdbImageLabel->setFixedSize(200,17);

    // Prepare InfoDialog
    ui->infoTextBrowser->setContentsMargins(15, 15, 15, 15);
    ui->infoTextBrowser->insertHtml(getInfoHtmlTemplate());
    ui->infoHeaderLabel->setStyleSheet(cssHeadline);
    ui->infoHeaderLabel->setText(tr("INFOABOUT") + " " + QCoreApplication::applicationName());

    // Set noCover to CoverImage first time
    ui->coverLabel->setScaledContents(true);
    QPixmap noCoverPix(":/images/noimage.svg");
    ui->coverLabel->setPixmap(noCoverPix);

    // Style Toolbar
    ui->addToolButton->setAutoRaise(true);
    ui->addToolButton->setIconSize(QSize(32, 32));
    ui->removeToolButton->setAutoRaise(true);
    ui->removeToolButton->setIconSize(QSize(32, 32));
    ui->searchToolButton->setAutoRaise(true);
    ui->searchToolButton->setIconSize(QSize(32, 32));

    QIcon saveIcon;
    saveIcon.addFile(":/icons/save.svg", QSize(), QIcon::Normal, QIcon::Off);
    saveIcon.addFile(":/icons/save_disabled.svg", QSize(), QIcon::Disabled, QIcon::Off);
    ui->actionSave->setIcon(saveIcon);
    ui->actionSave->setEnabled(false);

    QIcon saveAllIcon;
    saveAllIcon.addFile(":/icons/save_all.svg", QSize(), QIcon::Normal, QIcon::Off);
    saveAllIcon.addFile(":/icons/save_all_disabled.svg", QSize(), QIcon::Disabled, QIcon::Off);
    ui->actionSaveAll->setIcon(saveAllIcon);
    ui->actionSaveAll->setEnabled(false);

    QIcon saveSettingsIcon;
    saveSettingsIcon.addFile(":/icons/save.svg", QSize(), QIcon::Normal, QIcon::Off);
    saveSettingsIcon.addFile(":/icons/save_disabled.svg", QSize(), QIcon::Disabled, QIcon::Off);
    ui->actionSaveSettings->setIcon(saveSettingsIcon);
    ui->actionSaveSettings->setEnabled(false);

    // Select the hole row, Only one row is allowed
    // Prevents losing selction by clickng empty space.
    // This ensures that one movie always remains "active".
    // ------ FileList View ------
    ui->fileListTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->fileListTableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->fileListTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->fileListTableView->verticalHeader()->hide();   // no row header
    ui->fileListTableView->horizontalHeader()->hide(); // no column header

    // ------ SearchList View ------
    ui->searchTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->searchTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->searchTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // ------ DateEdit ------
    // d = Tag, M = Monat, yyyy = vierstelliges Jahr
    ui->releaseDateEdit->setDisplayFormat("dd.MM.yyyy");

    // Labelabstand
    ui->descriptionLabel->setContentsMargins(0, 60, 0, 0);
}

void MainWin::initializeAgeRating() {

    // --- Germany ---
    ratingMap["DE"] = {tr("GERMANY"), "de-movie", "FSK", {
       {"0", 100, "ab 0 Jahren", RatingColor::White, "square"},
       {"6", 200, "ab 6 Jahren", RatingColor::Yellow, "square"},
       {"12", 300, "ab 12 Jahren", RatingColor::Green, "square"},
       {"16", 400, "ab 16 Jahren", RatingColor::Blue, "square"},
       {"18", 500, "ab 18 Jahren", RatingColor::Red, "square"}
   }};

    // --- United Kingdom ---
    ratingMap["UK"] = {tr("UNITEDKINGDOM"), "uk-movie", "BBFC", {
        {"U", 100, "Universal – Suitable for all", RatingColor::LightGreen, "diamond"},
        {"PG", 200, "Parental Guidance", RatingColor::Yellow, "diamond"},
        {"12A", 300, "Suitable for 12 years and over", RatingColor::Orange, "circle"},
        {"12", 325, "Suitable for 12 years and over", RatingColor::White, "circle"},
        {"15", 400, "Suitable only for 15 years and older", RatingColor::Magenta, "circle"},
        {"18", 500, "Suitable only for adults", RatingColor::Red, "circle"},
        {"R18", 600, "To be shown only in specially licensed cinemas, or supplied only in licensed sex shops, and to adults only", RatingColor::Blue, "square"},
    }};

    // --- United States ---
    ratingMap["US"] = {tr("UNITEDSTATES"), "us-movie", "MPA", {
        {"G", 100, "General Audiences", RatingColor::Green, "square"},
        {"PG", 200, "Parental Guidance Suggested", RatingColor::Orange, "square"},
        {"PG-13", 300, "Parents Strongly Cautioned", RatingColor::USPink, "square"},
        {"R", 400, "Restricted", RatingColor::Red, "square"},
        {"NC-17", 500, "Adults Only", RatingColor::USBlueNC17, "square"},
    }};

   //  // --- Austria ---
   //  masterMap["AT"] = {"at-movie", {
   //     {"Restricted", 100, "Ohne Beschränkung"},
   //     {"6+", 200, "ab 6 Jahren"},
   //     {"8+", 250, "ab 8 Jahren"},
   //     {"10+", 275, "ab 10 Jahren"},
   //     {"12+", 300, "ab 12 Jahren"},
   //     {"14+", 350, "ab 14 Jahren"},
   //     {"16+", 400, "ab 16 Jahren"}
   // }};

   //  // --- Espana ---
   //  masterMap["ES"] = {"es-movie", {
   //     {"A(i)", 100, "Apta para todos los públicos"},
   //     {"7(i)", 200, "No recomendada menores de 7"},
   //     {"12", 300, "No recomendada menores de 12"},
   //     {"16", 400, "No recomendada menores de 16"},
   //     {"18", 500, "No recomendada menores de 18"},
   //     {"X", 600, "Película X"}
   // }};

   //  // --- France ---
   //  masterMap["FR"] = {"fr-movie", {
   //     {"TP", 100, "TP (Tous Publics)"},
   //     {"-10", 200, "Interdit aux -10 ans"},
   //     {"-18", 300, "Interdit aux -12 ans"},
   //     {"-16", 400, "Interdit aux -16 ans"},
   //     {"-18", 500, "Interdit aux -18 ans"}
   // }};


   //  // --- Italy ---
   //  masterMap["IT"] = {"it-movie", {
   //     {"T", 100, "Film per tutti"},
   //     {"6+", 200, "Non adatto ai minori di 6 anni"},
   //     {"10+", 275, "Non adatto ai minori di 10 anni"},
   //     {"14+", 350, "Vietato ai minori di 14 anni"},
   //     {"18+", 500, "Vietato ai minori di 18 anni"}
   // }};

   //  // --- Switzerland ---
   //  masterMap["CH"] = {"ch-movie", {
   //      {"FSK0", 100, "Ohne Altersbeschränkung"},
   //      {"FSK6", 200, "ab 6 Jahren"},
   //      {"FSK12", 300, "ab 12 Jahren"},
   //      {"FSK16", 400, "ab 16 Jahren"},
   //      {"FSK18", 500, "Keine Jugendfreigabe"}
   // }};



}

void MainWin::initializeComboBoxes() {
    // Fill genre combobox
    QString url_genre = "https://api.themoviedb.org/3/genre/movie/list?language=en";
    startApiRequest(url_genre);

    //fill age rating combobox
    fillRatingComboBox(countryCode);

    //fill settings rating combobox
    fillRatingSettingsComboBox();
}

void MainWin::initializeToolBars()
{
    // Main Toolbar
    ui->mainToolBar->setMovable(false);
    addExpandingSpacer(ui->mainToolBar); // Spacer before right Icons
    ui->mainToolBar->addAction(ui->actionSettings);
    ui->mainToolBar->addAction(ui->actionHelp);
    ui->mainToolBar->addAction(ui->actionInfo);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(ui->actionExit);

    // Info Toolbar
    ui->infoToolBar->setMovable(false);
    addExpandingSpacer(ui->infoToolBar); // Spacer before right Icons
    ui->infoToolBar->addAction(ui->actionSettings);
    ui->infoToolBar->addAction(ui->actionHelp);
    ui->infoToolBar->addAction(ui->actionInfo);
    ui->infoToolBar->addSeparator();
    ui->infoToolBar->addAction(ui->actionClose);

    // Settings Toolbar
    ui->settingsToolBar->setMovable(false);
    addExpandingSpacer(ui->settingsToolBar); // Spacer before right Icons
    ui->settingsToolBar->addAction(ui->actionSettings);
    ui->settingsToolBar->addAction(ui->actionHelp);
    ui->settingsToolBar->addAction(ui->actionInfo);
    ui->settingsToolBar->addSeparator();
    ui->settingsToolBar->addAction(ui->actionClose);

    // set toolbars intial
    ui->mainToolBar->setVisible(true);
    ui->infoToolBar->setVisible(false);
    ui->settingsToolBar->setVisible(false);
}

void MainWin::initializeStatusIcons() {
    int canvasWidth = 32;
    int canvasHeight = 32;
    int size = 11;

    auto createDot = [&](QColor color) {
        QPixmap canvas(canvasWidth, canvasHeight);
        canvas.fill(Qt::transparent);

        QPainter painter(&canvas);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(Qt::NoPen);
        painter.setBrush(color);
        painter.drawEllipse(6, 6, size, size);

        return canvas;
    };

    auto createSquare = [&](QColor color) {
        QPixmap canvas(canvasWidth, canvasHeight);
        canvas.fill(Qt::transparent);

        QPainter painter(&canvas);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setBrush(color);
        painter.setPen(Qt::NoPen);
        painter.drawRect(6, 6, size, size);

        return canvas;
    };

    m_statusGreen = createSquare({0, 120, 0});
    m_statusRed = createDot(Qt::red);
    m_statusYellow = createDot(Qt::yellow);
}

void MainWin::initializeTagWidgets() {

    // ********** Cast ******************************************
    // Get layout from container widget (manager for postion, size)
    QVBoxLayout *castContainerLayout = qobject_cast<QVBoxLayout*>(ui->castContainer->layout());
    if (!castContainerLayout) { castContainerLayout = new QVBoxLayout(ui->castContainer);};
    // Create TagWidget and assign the actorContainer as the parent.
    m_castWidget = new TagWidget(
        tr("ENTER_CAST"),
        ",",
        ui->castContainer // IMPORTANT: container is now parent
        );
    castContainerLayout->addWidget(m_castWidget); //Add TagWidget to layout
    castContainerLayout->setContentsMargins(0, 0, 0, 0);
    castContainerLayout->setSpacing(0);
    // END of declaration Cast **********************************

    // ********** Production **************************************
    // Get layout from container widget (manager for postion, size)
    QVBoxLayout *producerContainerLayout = qobject_cast<QVBoxLayout*>(ui->producersContainer->layout());
    if (!producerContainerLayout) { producerContainerLayout = new QVBoxLayout(ui->producersContainer);};
    // Create TagWidget and assign the productionContainer as the parent.
    m_producersWidget = new TagWidget(
        tr("ENTER_PRODUCERS"),
        ",",
        ui->producersContainer // IMPORTANT: container is now parent
    );
    producerContainerLayout->addWidget(m_producersWidget); //Add TagWidget to layout
    producerContainerLayout->setContentsMargins(0, 0, 0, 0);
    producerContainerLayout->setSpacing(0);
    // END of declaration production ******************************

    // ********** Screenplay***************************************
    // Get layout from container widget (manager for postion, size)
    QVBoxLayout *screenwritersContainerLayout = qobject_cast<QVBoxLayout*>(ui->screenwritersContainer->layout());
    if (!screenwritersContainerLayout) { screenwritersContainerLayout = new QVBoxLayout(ui->screenwritersContainer);};
    // Create TagWidget and assign the scriptContainer as the parent.
    m_screenwritersWidget = new TagWidget(
        tr("ENTER_SCREENWRITERS"),
        ",",
        ui->screenwritersContainer // IMPORTANT: container is now parent
        );
    screenwritersContainerLayout->addWidget(m_screenwritersWidget); //Add TagWidget to layout
    screenwritersContainerLayout->setContentsMargins(0, 0, 0, 0);
    screenwritersContainerLayout->setSpacing(0);
    // END of declaration script **********************************

    // ********** Directors ***************************************
    // Get layout from container widget (manager for postion, size)
    QVBoxLayout *directorsContainerLayout = qobject_cast<QVBoxLayout*>(ui->directorsContainer->layout());
    if (!directorsContainerLayout) { directorsContainerLayout = new QVBoxLayout(ui->directorsContainer);};
    // Create TagWidget and assign the scriptContainer as the parent.
    m_directorsWidget = new TagWidget(
        tr("ENTER_DIRECTORS"),
        ",",
        ui->directorsContainer // IMPORTANT: container is now parent
        );
    directorsContainerLayout->addWidget(m_directorsWidget); //Add TagWidget to layout
    directorsContainerLayout->setContentsMargins(0, 0, 0, 0);
    directorsContainerLayout->setSpacing(0);
    // END of declaration director **********************************

    // Set Size Policy to expanding.
    //m_actorWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // 5. **Stretch hinzufügen.**
    // Stellt sicher, dass das TagWidget am oberen Rand des Containers verankert bleibt
    // und nicht den gesamten vertikalen Platz unnötig füllt.
    //containerLayout->addStretch();

}

void MainWin::initializeDragAndDrop() {
    // ---- Drag and drop for filelist ----
    // Create model
    if (!m_FilelistModel) { m_FilelistModel = new QStandardItemModel(this); }
    // Set header
    m_FilelistModel->setColumnCount(2);
    m_FilelistModel->setHorizontalHeaderLabels(QStringList() << tr("") << tr("FILENAME"));
    // Assign model to view (This is important for DnD so that the view knows the columns.)
    ui->fileListTableView->setModel(m_FilelistModel);
    // Allow wordwrap
    ui->fileListTableView->setWordWrap(false);
    // Other Settings
    ui->fileListTableView->setShowGrid(false);
    ui->fileListTableView->horizontalHeader()->setMinimumSectionSize(0);
    ui->fileListTableView->setColumnWidth(0, 32);
    ui->fileListTableView->setStyleSheet("QTableView { show-decoration-selected: 1; }"
                                         "QTableView::item { alignment: center; }");
    ui->fileListTableView->setStyleSheet("QTableView::item { padding: 0px; margin: 0px; }");
    ui->fileListTableView->setTextElideMode(Qt::ElideRight); // Verhindert das "..." Kürzen
    ui->fileListTableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->fileListTableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed); // Titel-Spalte füllt Rest
    ui->fileListTableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);   // Jahr-Spalte feste Breite
    ui->fileListTableView->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignTop);
}

void MainWin::initializeConnects() {
    // ----- Connect finished-signal with our slot
    connect(m_ApiNetworkManager, &QNetworkAccessManager::finished,
        this, &MainWin::onApiRequestFinished
    );

    // ----- Color textedit
    connect(ui->descriptionText, &QTextEdit::textChanged,
        this, [this]() {
                this->formatTextByLength(256); // Limit 256 festlegen
    });

    // ----- Drag and drop Cover Image
    connect(ui->coverLabel, &ImageLabelM::imageChanged, this, [this](const QString &pathOrUrl) {
        // Get selected row
        int rowIndex = ui->fileListTableView->currentIndex().row();
        if (rowIndex < 0) return;

        QString movieKey = ui->fileListTableView->model()->data(ui->fileListTableView->model()->index(rowIndex, 1), Qt::UserRole).toString();

        if (pathOrUrl.startsWith("http")) {
            // Internet download
            this->startImageDownload(pathOrUrl, [this, movieKey, rowIndex](const QPixmap &coverPix) {
                qDebug() << "Download callback reached!";
                if (!coverPix.isNull()) {
                    // Set cover to label
                    ui->coverLabel->setOriginalPixmap(coverPix.scaled(ui->coverLabel->size(),
                         Qt::KeepAspectRatio,
                         Qt::SmoothTransformation)
                    );
                    // Save cover in dataset
                    this->m_loadedMovies[movieKey].cover = coverPix;
                    this->updateStatus(rowIndex, "modified");
                    this->onDataChanged();
                }
            });
        } else {
            // Load local File again and save
            QPixmap localPix(pathOrUrl);
            if (!localPix.isNull()) {
                this->m_loadedMovies[movieKey].cover = localPix;
                this->updateStatus(rowIndex, "modified");
                this->onDataChanged();
            }
        }
    });

    // ----- Drag and drop Files
    connect(ui->fileListTableView, &TableViewM::filesDropped, this, &MainWin::addFilesToModel);

    // ----- Status
    // Changes from UI saved in Dataset.
    // LineEdits
    connect(ui->titleEdit, &QLineEdit::textEdited, this, &MainWin::onDataChanged);
    connect(ui->studioEdit, &QLineEdit::textEdited, this, &MainWin::onDataChanged);
    // TextEdit
    connect(ui->descriptionText, &QTextEdit::textChanged, this, &MainWin::onDataChanged);
    // ComboBoxes
    connect(ui->ratingComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWin::onDataChanged);
    connect(ui->genreComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWin::onDataChanged);
    connect(ui->ratingSettingsComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWin::onSettingsChanged);
    // DateEdit
    connect(ui->releaseDateEdit, &QDateEdit::dateChanged, this, &MainWin::onDataChanged);

    // TagWidgets
    connect(m_castWidget, &TagWidget::tagsChanged, this, &MainWin::onDataChanged);
    connect(m_directorsWidget, &TagWidget::tagsChanged, this, &MainWin::onDataChanged);
    connect(m_screenwritersWidget, &TagWidget::tagsChanged, this, &MainWin::onDataChanged);
    connect(m_producersWidget, &TagWidget::tagsChanged, this, &MainWin::onDataChanged);

    // Actions
    connect(ui->actionExit, &QAction::triggered, qApp, &QApplication::quit);
    connect(ui->actionInfo, &QAction::triggered, this, &MainWin::handleInfoButton_Clicked);
    connect(ui->actionSettings, &QAction::triggered, this, &MainWin::handleSettingsButton_Clicked);
    connect(ui->actionClose, &QAction::triggered, this, &MainWin::handleCloseButton_Clicked);
    connect(ui->actionSaveSettings, &QAction::triggered, this, &MainWin::handleSaveSettingsButton_Clicked);

    // ----- Other connects
    connect(ui->searchTableView, &QTableView::doubleClicked, this, &MainWin::onSearchTableViewDoubleClicked);
    connect(ui->fileListTableView, &QTableView::clicked, this, &::MainWin::onFilelistTableViewClicked);
    connect(ui->searchToolButton, &QToolButton::clicked, this, &MainWin::onSearchButtonClicked);
    connect(ui->addToolButton, &QToolButton::clicked, this, &MainWin::onAddFilesButtonClicked);
    connect(ui->removeToolButton, &QToolButton::clicked, this, &::MainWin::onRemoveFilesButtonClicked);
    connect(ui->searchEdit, &QLineEdit::returnPressed, ui->searchToolButton, &QToolButton::click);
}

void MainWin::initializeNetwork() {
    // Network Access
    m_ApiNetworkManager = new QNetworkAccessManager(this);
    m_ImageNetworkManager = new QNetworkAccessManager(this);
}


