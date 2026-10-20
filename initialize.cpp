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
#include "themecolors.h"
#include "twolinedelegate.h"
#include "posterpreview.h"

#include <QDesktopServices>
#include <QSvgRenderer>
#include <QUrl>

// ***** INITIALIZE *****
void MainWin::initializeForm() {

    // Style headers (same grey as the tags)
    QString cssHeadline = "QLabel { background-color: " + accentGrey(palette()).name() + "; "
                          "color: palette(window-text); "
                          "font-weight: bold; "
                          "padding: 6px; "
                          "border: none; "   // middleLabel has a StyledPanel frame in the .ui
                          "border-radius: " + QString::number(FieldLook::radius) + "px; }";
    ui->leftLabel->setStyleSheet(cssHeadline);
    ui->middleLabel->setStyleSheet(cssHeadline);
    ui->rightLabel->setStyleSheet(cssHeadline);

    // Settings as cards: rounded box in the field colors, heading like the column headers
    const FieldLook look;
    ui->settingsGeneralCard->setStyleSheet(QStringLiteral(
        "QFrame#settingsGeneralCard { background: %1; border: 1px solid %2; border-radius: %3px; }")
        .arg(look.fill.name(), look.border.name()).arg(FieldLook::radius));
    ui->settingsGeneralLabel->setStyleSheet(cssHeadline);
    ui->ratingSettingsComboBox->setMinimumWidth(240);

    // Rounded input fields (the ones in the tag widgets style themselves)
    applyInputStyle(ui->titleEdit);
    applyInputStyle(ui->studioEdit);
    applyBoxStyle(ui->descriptionText, true);
    applySpinStyle(ui->releaseDateEdit);
    applyComboStyle(ui->genreComboBox);
    // The genre may also be typed: the list only suggests the TMDB genres
    ui->genreComboBox->setEditable(true);
    ui->genreComboBox->setInsertPolicy(QComboBox::NoInsert);
    applyComboStyle(ui->ratingComboBox);
    applyComboStyle(ui->ratingSettingsComboBox);
    applyComboStyle(ui->languageSettingsComboBox);
    applyComboStyle(ui->dataLanguageSettingsComboBox);
    applyTabStyle(ui->tabWidget);
    initializeDetailsTab();

    // Columns: on the first start the middle (movie info) gets the extra width, the lists keep
    // their preferred width; afterwards window and columns as the user left them
    ui->splitter->setStretchFactor(1, 1);
    restoreGeometry(settings.value("Window/Geometry").toByteArray());
    ui->splitter->restoreState(settings.value("Window/Splitter").toByteArray());

    // TMDB logo: at most 200 px wide, shrinks with the search column (sharp, not distorted)
    ui->tmdbImageLabel->load(QStringLiteral(":/images/tmdb.svg"));
    ui->tmdbImageLabel->renderer()->setAspectRatioMode(Qt::KeepAspectRatio);
    ui->tmdbImageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
    ui->tmdbImageLabel->setFixedHeight(17);
    ui->tmdbImageLabel->setMaximumWidth(200);

    // No movie yet: the cover shows its placeholder
    ui->coverLabel->clear();


    // Select the hole row, Only one row is allowed
    // Prevents losing selction by clickng empty space.
    // This ensures that one movie always remains "active".
    // ------ FileList View ------
    ui->fileListTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->fileListTableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->fileListTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // ------ SearchList View ------
    // One column: poster as icon, title and year as text; lies in the rounded tab area
    ui->searchTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->searchTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->searchTableView->setIconSize(kPosterSize);
    ui->searchTableView->setStyleSheet("QListView { background: transparent; border: none; outline: 0; }");
    ui->searchTableView->setItemDelegate(new TwoLineDelegate(ui->searchTableView));   // title + year
    // One model for all searches (only cleared), so the selection model and the connection stay
    m_SearchModel = new QStandardItemModel(this);
    ui->searchTableView->setModel(m_SearchModel);
    // The separator lines depend on the neighbour's selection: repaint the whole list
    connect(ui->searchTableView->selectionModel(), &QItemSelectionModel::selectionChanged,
            ui->searchTableView->viewport(), qOverload<>(&QWidget::update));
    // Enlarged poster when the mouse rests on a result
    new PosterPreview(ui->searchTableView, kPosterRole);

    // ------ DateEdit ------
    // d = Tag, M = Monat, yyyy = vierstelliges Jahr
    ui->releaseDateEdit->setDisplayFormat("dd.MM.yyyy");

    // ------ Movie info: width ------
    // The cover has a fixed size (a bigger cover would take the room from the description);
    // description, date, genre and age rating fill the left column, which gets the extra width.
    // The tag column on the right gets at most kTagColumnWidth (longer tags make no sense).
    ui->coverLabel->setFixedSize(kCoverSize);
    ui->middleLeftContainer->setAlignment(ui->coverLabel, Qt::AlignLeft);
    for (QWidget *field : {static_cast<QWidget *>(ui->titleEdit), ui->castContainer, ui->directorsContainer,
                           ui->screenwritersContainer, ui->producersContainer,
                           static_cast<QWidget *>(ui->studioEdit)}) {
        field->setMaximumWidth(kTagColumnWidth);
    }
}

// Tab "Data": the details TMDB has for a search result and the button that takes them over
void MainWin::initializeDetailsTab()
{
    m_detailsTitle = new QLabel(ui->tab_2);
    QFont titleFont = m_detailsTitle->font();
    titleFont.setBold(true);
    m_detailsTitle->setFont(titleFont);
    m_detailsTitle->setWordWrap(true);
    m_detailsTitle->setAlignment(Qt::AlignCenter);

    m_posterPicker = new PosterPicker(ui->tab_2);

    m_detailsView = new QTextBrowser(ui->tab_2);
    m_detailsView->setOpenLinks(false);
    m_detailsView->setStyleSheet("QTextBrowser { background: transparent; border: none; }");

    m_applyBtn = new QPushButton(tr("Apply"), ui->tab_2);
    m_applyBtn->setToolTip(tr("Take the data into the selected movie"));
    m_applyBtn->setEnabled(false);   // until the details of a result are loaded

    // Title, poster, the other data, button
    auto *layout = new QVBoxLayout(ui->tab_2);
    layout->addWidget(m_detailsTitle);
    layout->addWidget(m_posterPicker, 0, Qt::AlignHCenter);
    layout->addWidget(m_detailsView, 1);
    layout->addWidget(m_applyBtn);
    connect(m_applyBtn, &QPushButton::clicked, this, &MainWin::applyTmdbMovie);
    connect(m_posterPicker, &PosterPicker::posterChosen, this, &MainWin::showTmdbPoster);

    showTmdbDetails();
}

void MainWin::initializeComboBoxes() {
    // Fill genre combobox (in the language of the movie data)
    requestGenres();

    //fill age rating combobox
    fillRatingComboBox(countryCode);

    //fill settings rating combobox
    fillRatingSettingsComboBox();

    // Settings: language of the app and of the movie data
    for (const Languages::Language &language : Languages::appLanguages()) {
        ui->languageSettingsComboBox->addItem(language.name, language.code);
    }
    setComboEntryByString(ui->languageSettingsComboBox, Languages::appLanguage(), true);
    for (const Languages::Language &language : Languages::dataLanguages()) {
        ui->dataLanguageSettingsComboBox->addItem(language.name, language.code);
    }
    setComboEntryByString(ui->dataLanguageSettingsComboBox, dataLanguage, true);

    // Settings: write into a copy first (slow, but safe)
    m_safeSave = settings.value("General/SafeSave", false).toBool();
    ui->safeSaveCheckBox->setChecked(m_safeSave);
}

void MainWin::initializeToolBars() {
    // --- Main toolbar: saving left, settings and help right
    m_saveBtn = ui->mainToolbar->addButton("saveBtn", tr("Save"), ":icons/save", true);
    m_saveBtn->setToolTip(tr("Save metadata to the movie"));
    m_saveBtn->setEnabled(false);
    m_saveAllBtn = ui->mainToolbar->addButton("saveAllBtn", tr("Save all"), ":icons/save_all", true);
    m_saveAllBtn->setToolTip(tr("Save the metadata of all changed movies"));
    m_saveAllBtn->setEnabled(false);
    ui->mainToolbar->addSpacer();
    // Donation: orange icon on a normal button, so it stands out a bit
    m_coffeeBtn = ui->mainToolbar->addButton("coffeeBtn", "", ":icons/coffee");
    m_coffeeBtn->setToolTip(tr("Buy me a coffee – support MovieMetaM via PayPal"));
    ui->mainToolbar->setButtonColor(m_coffeeBtn, QColor(), QColor("#F5A623"));
    m_settingsBtn = ui->mainToolbar->addButton("settingsBtn", "", ":icons/settings");
    m_settingsBtn->setToolTip(tr("Settings"));
    m_helpBtn = ui->mainToolbar->addButton("helpBtn", "", ":icons/help");
    m_helpBtn->setToolTip(tr("Help"));

    // --- Settings toolbar
    m_settingsBackBtn = ui->settingsToolbar->addButton("settingsBackBtn", tr("Back"), ":icons/back", true);
    ui->settingsToolbar->setTitle(tr("Settings"));
    ui->settingsToolbar->addSpacer();
    m_saveSettingsBtn = ui->settingsToolbar->addButton("saveSettingsBtn", tr("Save"), ":icons/save", true);
    m_saveSettingsBtn->setToolTip(tr("Save settings"));
    m_saveSettingsBtn->setEnabled(false);

    // --- Toolbar below the movie list (narrow: icons only)
    m_addBtn = ui->fileListToolbar->addButton("addBtn", "", ":icons/add");
    m_addBtn->setToolTip(tr("Add movies"));
    ui->fileListToolbar->addSpacer();
    m_removeBtn = ui->fileListToolbar->addButton("removeBtn", "", ":icons/remove");
    m_removeBtn->setToolTip(tr("Remove the selected movies"));
    m_removeBtn->setEnabled(false);   // until a movie is selected

    // Same corner radius as the fields instead of the pill shape,
    // frame in one color (no darker top/left edge) like the rest of the app
    // (the help page brings its toolbar with it)
    for (ToolbarM *toolbar : {ui->mainToolbar, ui->settingsToolbar, ui->fileListToolbar, ui->helpPage->toolbar()}) {
        toolbar->setButtonRadius(FieldLook::radius);
        toolbar->setToolbarRadius(FieldLook::radius);
        ToolbarM::ShadeFactors shade = toolbar->shadeFactors();
        shade.toolbarDarkEdge  = 0.20;
        shade.toolbarLightEdge = 0.20;
        toolbar->setShadeFactors(shade);
    }
    ui->searchEdit->setCornerRadius(FieldLook::radius);
}

void MainWin::initializeStatusIcons() {
    // Drawn in the resolution of the screen (e.g. 1.25 or 2), on whole device pixels:
    // a 32 px picture enlarged by the scaling would blur all edges
    const qreal dpr = devicePixelRatioF();
    const int canvasSize = qRound(32 * dpr);    // icon size of the list: 32 logical px
    const int side = qRound(11 * dpr);          // symbol: 11 logical px
    const int origin = (canvasSize - side) / 2; // centered
    const int line = qMax(1, qRound(dpr));      // outline: 1 logical px
    const QRect symbolRect(origin, origin, side, side);
    // A light, half transparent outline keeps the symbol visible on the field color,
    // on the accent color of a selected entry and in light themes
    const QColor outline(255, 255, 255, 170);

    // Qt tints icons of selected entries with the accent color: give the selected mode the
    // same picture, so the status colors stay true on the accent color
    auto selectedLikeNormal = [](const QPixmap &pixmap) {
        QIcon icon;
        icon.addPixmap(pixmap, QIcon::Normal);
        icon.addPixmap(pixmap, QIcon::Selected);
        return icon;
    };

    // Outline = the symbol in the outline color, the colored symbol one line smaller on top
    auto createDot = [&](QColor color) {
        QPixmap canvas(canvasSize, canvasSize);
        canvas.fill(Qt::transparent);

        QPainter painter(&canvas);
        painter.setRenderHint(QPainter::Antialiasing);   // a circle needs smoothing
        painter.setPen(Qt::NoPen);
        painter.setBrush(outline);
        painter.drawEllipse(symbolRect);
        painter.setBrush(color);
        painter.drawEllipse(symbolRect.adjusted(line, line, -line, -line));
        painter.end();

        canvas.setDevicePixelRatio(dpr);
        return selectedLikeNormal(canvas);
    };

    auto createSquare = [&](QColor color) {
        QPixmap canvas(canvasSize, canvasSize);
        canvas.fill(Qt::transparent);

        QPainter painter(&canvas);                       // no smoothing: crisp edges
        painter.fillRect(symbolRect, outline);
        painter.fillRect(symbolRect.adjusted(line, line, -line, -line), color);
        painter.end();

        canvas.setDevicePixelRatio(dpr);
        return selectedLikeNormal(canvas);
    };

    m_statusGreen = createSquare({40, 170, 70});   // lighter green than before, visible on grey and blue
    m_statusYellow = createDot({255, 200, 0});   // amber: the white outline stands out (pure yellow is as light as it)
}

void MainWin::initializeTagWidgets() {

    // ********** Cast ******************************************
    // Get layout from container widget (manager for postion, size)
    QVBoxLayout *castContainerLayout = qobject_cast<QVBoxLayout*>(ui->castContainer->layout());
    if (!castContainerLayout) { castContainerLayout = new QVBoxLayout(ui->castContainer);};
    // Create TagWidget and assign the actorContainer as the parent.
    m_castWidget = new TagWidget(
        tr("Enter actors"),
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
        tr("Enter producers"),
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
        tr("Enter screenwriters"),
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
        tr("Enter directors"),
        ",",
        ui->directorsContainer // IMPORTANT: container is now parent
        );
    directorsContainerLayout->addWidget(m_directorsWidget); //Add TagWidget to layout
    directorsContainerLayout->setContentsMargins(0, 0, 0, 0);
    directorsContainerLayout->setSpacing(0);
    // END of declaration director **********************************

    // Set Size Policy to expanding.
    //m_actorWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // 5. Add a stretch: keeps the TagWidget at the top of the container
    // so it does not fill all the vertical space.
    //containerLayout->addStretch();

}

void MainWin::initializeDragAndDrop() {
    // ---- Drag and drop for filelist ----
    // Create model
    if (!m_FilelistModel) { m_FilelistModel = new QStandardItemModel(this); }
    // One column: title + file name as text, status as icon, file path in UserRole
    m_FilelistModel->setColumnCount(1);
    // Assign model to view (This is important for DnD so that the view knows the columns.)
    ui->fileListTableView->setModel(m_FilelistModel);
    // Allow wordwrap
    ui->fileListTableView->setWordWrap(false);
    // Other Settings
    ui->fileListTableView->setIconSize(QSize(32, 32));   // status dot (drawn on a 32 px canvas)
    // Rounded box like the other fields; the entries (title + file name, rounded selection in
    // the accent color) are drawn by TwoLineDelegate (outline: 0 – no focus frame)
    applyBoxStyle(ui->fileListTableView, false, FieldCorners::All, "QListView { outline: 0; }");
    ui->fileListTableView->setItemDelegate(new TwoLineDelegate(ui->fileListTableView));
    // The separator lines depend on the neighbour's selection: repaint the whole list
    connect(ui->fileListTableView->selectionModel(), &QItemSelectionModel::selectionChanged,
            ui->fileListTableView->viewport(), qOverload<>(&QWidget::update));
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
    // Takes a dropped cover into the shown movie: label, dataset, status
    auto applyCover = [this](const QPixmap &cover) {
        int rowIndex = ui->fileListTableView->currentIndex().row();
        if (rowIndex < 0) return;
        QString movieKey = ui->fileListTableView->model()->data(ui->fileListTableView->model()->index(rowIndex, 0), Qt::UserRole).toString();

        ui->coverLabel->setCover(cover);
        m_loadedMovies[movieKey].cover = cover;
        updateStatus(rowIndex, "modified");
        onDataChanged();
    };
    auto coverFailed = [this]() {
        showColumnStatus(StatusColumn::Info, tr("The image could not be loaded."), ToastM::Level::Error);
    };

    // Local file or image data: the label already has the picture
    connect(ui->coverLabel, &ImageLabelM::coverDropped, this, applyCover);
    // Web link: download first
    connect(ui->coverLabel, &ImageLabelM::coverUrlDropped, this, [this, applyCover, coverFailed](const QUrl &url) {
        if (ui->fileListTableView->currentIndex().row() < 0) return;
        startImageDownload(url, applyCover, coverFailed);
    });
    connect(ui->coverLabel, &ImageLabelM::coverDropFailed, this, coverFailed);

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
    connect(ui->genreComboBox, &QComboBox::editTextChanged, this, &MainWin::onDataChanged);
    connect(ui->ratingSettingsComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWin::onSettingsChanged);
    connect(ui->languageSettingsComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWin::onSettingsChanged);
    connect(ui->dataLanguageSettingsComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWin::onSettingsChanged);
    connect(ui->safeSaveCheckBox, &QCheckBox::toggled, this, &MainWin::onSettingsChanged);
    // Preview of the chosen system at once, before saving
    connect(ui->ratingSettingsComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this] {
        fillRatingPreview(ui->ratingSettingsComboBox->currentData().toString());
    });
    // DateEdit
    connect(ui->releaseDateEdit, &QDateEdit::dateChanged, this, &MainWin::onDataChanged);

    // TagWidgets
    connect(m_castWidget, &TagWidget::tagsChanged, this, &MainWin::onDataChanged);
    connect(m_directorsWidget, &TagWidget::tagsChanged, this, &MainWin::onDataChanged);
    connect(m_screenwritersWidget, &TagWidget::tagsChanged, this, &MainWin::onDataChanged);
    connect(m_producersWidget, &TagWidget::tagsChanged, this, &MainWin::onDataChanged);

    // Toolbar buttons: page switching
    connect(m_settingsBtn, &QToolButton::clicked, this, [this] { showPage(ui->settingsPage); });
    connect(m_helpBtn, &QToolButton::clicked, this, [this] { showPage(ui->helpPage); });
    connect(m_coffeeBtn, &QToolButton::clicked, this, [] {
        QDesktopServices::openUrl(QUrl(QStringLiteral("https://paypal.me/michaelspeckenbach")));
    });
    connect(m_settingsBackBtn, &QToolButton::clicked, this, [this] { showPage(ui->mainPage); });
    connect(ui->helpPage, &HelpPage::backRequested, this, [this] { showPage(ui->mainPage); });
    connect(m_saveSettingsBtn, &QToolButton::clicked, this, &MainWin::handleSaveSettingsButton_Clicked);

    // ----- Other connects
    connect(ui->searchTableView, &QAbstractItemView::doubleClicked, this, &MainWin::onSearchTableViewDoubleClicked);
    connect(ui->fileListTableView->selectionModel(), &QItemSelectionModel::currentChanged, this, &MainWin::onCurrentMovieChanged);
    connect(ui->searchEdit, &SearchField::searchRequested, this, &MainWin::onSearchButtonClicked);
    connect(m_addBtn, &QToolButton::clicked, this, &MainWin::onAddFilesButtonClicked);
    // Saving runs in a thread
    m_saveWatcher = new QFutureWatcher<QList<SaveResult>>(this);
    connect(m_saveWatcher, &QFutureWatcher<QList<SaveResult>>::finished, this, &MainWin::savingFinished);
    connect(m_saveBtn, &QToolButton::clicked, this, &MainWin::saveMovieDetails);
    connect(m_saveAllBtn, &QToolButton::clicked, this, &MainWin::onSaveAllButtonClicked);
    connect(m_removeBtn, &QToolButton::clicked, this, &MainWin::onRemoveFilesButtonClicked);

    // "Remove" only while at least one movie is selected
    connect(ui->fileListTableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, [this] {
        m_removeBtn->setEnabled(ui->fileListTableView->selectionModel()->hasSelection());
    });
}

void MainWin::initializeNetwork() {
    // Network Access
    m_ApiNetworkManager = new QNetworkAccessManager(this);
    m_ImageNetworkManager = new QNetworkAccessManager(this);
}


