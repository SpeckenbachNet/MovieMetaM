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
#include "posterpicker.h"
#include "helppage.h"
#include "tagwidget.h"

#include <QAction>
#include <QDesktopServices>
#include <QLineEdit>
#include <QPainter>
#include <QSvgRenderer>
#include <QUrl>

// ***** INITIALIZE *****
void MainWin::initializeForm() {

    // Colors and shapes of headers, fields and boxes: applyThemeStyles() (at the end of the start)
    ui->ratingSettingsComboBox->setMinimumWidth(240);
    // The genre may also be typed: the list only suggests the TMDB genres
    ui->genreComboBox->setEditable(true);
    ui->genreComboBox->setInsertPolicy(QComboBox::NoInsert);
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
    // The format row is centered under the cover and keeps its room when hidden (no jumping)
    ui->formatRow->setFixedWidth(kCoverSize.width());
    ui->middleLeftContainer->setAlignment(ui->formatRow, Qt::AlignLeft);
    QSizePolicy formatPolicy = ui->formatRow->sizePolicy();
    formatPolicy.setRetainSizeWhenHidden(true);
    ui->formatRow->setSizePolicy(formatPolicy);
    ui->formatRow->hide();
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

    // Content advisory (Apple's "rtng"): none, clean, explicit
    ui->advisoryComboBox->addItem(tr("None"), 0);
    ui->advisoryComboBox->addItem(tr("Clean"), 2);
    ui->advisoryComboBox->addItem(tr("Explicit"), 1);

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

    // Own genres (listed first in the genre box, e.g. "Series", "Concert") and the wish to see only those
    m_customGenres = settings.value("General/CustomGenres").toStringList();
    m_onlyCustomGenres = settings.value("General/OnlyCustomGenres", false).toBool();
    m_customGenresWidget = new TagWidget(tr("Enter genres"), ",", this);
    m_customGenresWidget->setMinimumHeight(130);
    m_customGenresWidget->setTags(m_customGenres.join(QLatin1Char(',')));
    m_onlyCustomGenresCheck = new QCheckBox(tr("Show only own genres"), this);
    m_onlyCustomGenresCheck->setChecked(m_onlyCustomGenres);

    // "Apply" of TMDB data: everything (as before) or only the fields that are still empty
    m_fillEmptyOnly = settings.value("General/FillEmptyOnly", false).toBool();
    m_fillEmptyOnlyCheck = new QCheckBox(tr("Only fill empty fields"), this);
    m_fillEmptyOnlyCheck->setChecked(m_fillEmptyOnly);

    // Own TMDB token (password field with an eye): replaces the one built into the app
    m_tmdbToken = settings.value("TMDB/Token").toString().trimmed();
    m_tmdbTokenEdit = new QLineEdit(this);
    m_tmdbTokenEdit->setEchoMode(QLineEdit::Password);
    m_tmdbTokenEdit->setPlaceholderText(tr("Token of the built-in account"));
    m_tmdbTokenEdit->setText(m_tmdbToken);
    // The eye at the end of the field shows or hides the token
    m_tmdbTokenEye = m_tmdbTokenEdit->addAction(eyeIcon(false), QLineEdit::TrailingPosition);
    m_tmdbTokenEye->setCheckable(true);
    m_tmdbTokenEye->setToolTip(tr("Show the token"));
    connect(m_tmdbTokenEye, &QAction::toggled, this, [this](bool shown) {
        m_tmdbTokenEdit->setEchoMode(shown ? QLineEdit::Normal : QLineEdit::Password);
        m_tmdbTokenEye->setIcon(eyeIcon(shown));
        m_tmdbTokenEye->setToolTip(shown ? tr("Hide the token") : tr("Show the token"));
    });

    initializeSettingsPage();
    updateTmdbTokenStatus();
    fillGenreList();   // available at once, also without TMDB (the genres of TMDB follow)
}

// The eye of the token field (open: show, crossed out: hide), tinted in the text color. QLineEdit
// shows it at 16 px: only the area of the eye is drawn (without the empty margins of the 24 grid),
// in the resolution of the screen
QIcon MainWin::eyeIcon(bool shown) const
{
    constexpr int size = 16;
    const qreal dpr = devicePixelRatioF();
    QPixmap pm(QSize(size, size) * dpr);
    pm.fill(Qt::transparent);
    pm.setDevicePixelRatio(dpr);
    QSvgRenderer renderer(shown ? QStringLiteral(":icons/eye_off") : QStringLiteral(":icons/eye"));
    renderer.setViewBox(QRectF(2.5, 2.5, 19, 19));
    QPainter p(&pm);
    renderer.render(&p, QRectF(0, 0, size, size));
    p.setCompositionMode(QPainter::CompositionMode_SourceIn);
    p.fillRect(pm.rect(), QApplication::palette().color(QPalette::Text));
    return QIcon(pm);
}

// Style sheets with the colors of the theme. They hold fixed colors, so they are set again when
// light / dark mode switches (onColorSchemeChanged). Here are all of the main window; the tag
// widgets, the poster list and the help page do it themselves.
void MainWin::applyThemeStyles()
{
    const FieldLook look;

    // Headers of the columns and of the settings cards: a light grey band, the bold text
    // carries the heading
    const QString headlineStyle = "QLabel { background-color: " + accentGrey(QApplication::palette(), 0.18).name() + "; "
                                  "color: palette(window-text); font-weight: bold; padding: 6px; "
                                  "border: none; "   // middleLabel has a StyledPanel frame in the .ui
                                  "border-radius: " + QString::number(FieldLook::radius) + "px; }";
    ui->leftLabel->setStyleSheet(headlineStyle);
    ui->middleLabel->setStyleSheet(headlineStyle);
    ui->rightLabel->setStyleSheet(headlineStyle);
    for (QLabel *headline : findChildren<QLabel *>(QStringLiteral("settingsHeadline"))) {
        headline->setStyleSheet(headlineStyle);
    }
    // Settings as cards: rounded box in the field colors
    const QString cardStyle = QStringLiteral(
        "QFrame#settingsCard { background: %1; border: 1px solid %2; border-radius: %3px; }")
        .arg(look.fill.name(), look.border.name()).arg(FieldLook::radius);
    for (QFrame *card : findChildren<QFrame *>(QStringLiteral("settingsCard"))) {
        card->setStyleSheet(cardStyle);
    }
    if (QLabel *tmdbHint = findChild<QLabel *>(QStringLiteral("tmdbHint"))) {
        tmdbHint->setStyleSheet(QStringLiteral("QLabel { color: %1; }").arg(look.placeholder.name()));
    }

    // Rounded input fields (the ones in the tag widgets style themselves)
    applyInputStyle(ui->titleEdit);
    applyInputStyle(ui->studioEdit);
    applyInputStyle(m_tmdbTokenEdit);
    applyBoxStyle(ui->descriptionText, true);
    applySpinStyle(ui->releaseDateEdit);
    applyComboStyle(ui->genreComboBox);
    applyComboStyle(ui->ratingComboBox);
    applyComboStyle(ui->advisoryComboBox);
    applyComboStyle(ui->ratingSettingsComboBox);
    applyComboStyle(ui->languageSettingsComboBox);
    applyComboStyle(ui->dataLanguageSettingsComboBox);
    applyTabStyle(ui->tabWidget);
    // Movie list: rounded box like the other fields; the entries (title + file name, rounded
    // selection in the accent color) are drawn by TwoLineDelegate (outline: 0 – no focus frame)
    applyBoxStyle(ui->fileListTableView, false, FieldCorners::All, "QListView { outline: 0; }");
    // Format of the video: chip in the look of the tags ("1080p") and the size beside it
    ui->hdChipLabel->setStyleSheet(QStringLiteral(
        "QLabel { font-size: 12px; padding: 2px 6px; border-radius: 5px;"
        " background-color: %1; color: %2; }")
        .arg(QApplication::palette().color(QPalette::Highlight).name(),
             QApplication::palette().color(QPalette::HighlightedText).name()));
    if (m_tmdbTokenEye) {
        m_tmdbTokenEye->setIcon(eyeIcon(m_tmdbTokenEye->isChecked()));
    }
}

// Light / dark mode switched: everything in a fixed order, the selection colors first (the tags
// use them)
void MainWin::onColorSchemeChanged()
{
    fixHighlightContrast();   // the selection colors of the new mode (Graphite on macOS)
    applyThemeStyles();
    for (TagWidget *tags : findChildren<TagWidget *>()) {
        tags->restyle();
    }
    m_posterPicker->restyle();
    ui->helpPage->refreshTheme();
    ui->searchEdit->setCornerRadius(FieldLook::radius);   // also sets its colors again
    ui->coverLabel->update();
}

// Says which TMDB token the next request uses
void MainWin::updateTmdbTokenStatus()
{
    if (!m_tmdbTokenEdit->text().trimmed().isEmpty()) {
        m_tmdbTokenStatus->setText(tr("Your own token is used."));
    } else if (!QByteArrayLiteral(TMDB_TOKEN).isEmpty()) {
        m_tmdbTokenStatus->setText(tr("The token built into the app is used."));
    } else {
        m_tmdbTokenStatus->setText(tr("No token: the movie search is not available."));
    }
}

// The settings page: cards by topic in two columns, each setting a block (a heading, the control,
// what belongs to it below). The widgets are made in the .ui file or in initializeComboBoxes(); they
// move out of the old card "General", which is deleted afterwards.
void MainWin::initializeSettingsPage()
{
    // Colors of cards and headlines: applyThemeStyles() (by the object names)
    auto *container = new QWidget(ui->settingsPage);
    container->setMinimumWidth(820);
    container->setMaximumWidth(1180);
    auto *columns = new QHBoxLayout(container);
    columns->setContentsMargins(0, 0, 0, 0);
    columns->setSpacing(18);
    auto *left = new QVBoxLayout;
    auto *right = new QVBoxLayout;
    left->setSpacing(18);
    right->setSpacing(18);
    columns->addLayout(left, 1);
    columns->addLayout(right, 1);

    // A card with its headline; returns the layout for the blocks
    const auto addCard = [&](QVBoxLayout *column, const QString &title) {
        auto *card = new QFrame(container);
        card->setObjectName(QStringLiteral("settingsCard"));
        auto *layout = new QVBoxLayout(card);
        layout->setContentsMargins(12, 12, 12, 16);
        layout->setSpacing(8);
        auto *headline = new QLabel(title, card);
        headline->setObjectName(QStringLiteral("settingsHeadline"));
        layout->addWidget(headline);
        column->addWidget(card);
        return layout;
    };
    // A heading above a control; a note (info icon) sits at the end of the heading line
    const auto addBlock = [&](QVBoxLayout *card, const QString &heading, QWidget *control, QWidget *info = nullptr) {
        auto *row = new QHBoxLayout;
        row->setContentsMargins(0, 6, 0, 0);
        row->addWidget(new QLabel(heading, card->parentWidget()));
        if (info) {
            row->addWidget(info);
        }
        row->addStretch();
        card->addLayout(row);
        ui->settingsGeneralForm->removeWidget(control);
        card->addWidget(control);
    };

    // Left: language, age rating, saving, apply
    QVBoxLayout *languageCard = addCard(left, tr("Languages"));
    addBlock(languageCard, tr("Language"), ui->languageSettingsComboBox,
             Languages::hasDraftLanguages() ? new InfoIcon(InfoIcon::draftLanguagesNote(), this) : nullptr);
    addBlock(languageCard, tr("Movie data language"), ui->dataLanguageSettingsComboBox);

    QVBoxLayout *ratingCard = addCard(left, tr("Age Rating"));
    ui->settingsGeneralForm->removeWidget(ui->ratingSettingsComboBox);
    ratingCard->addWidget(ui->ratingSettingsComboBox);
    ui->settingsGeneralForm->removeWidget(ui->ratingPreviewContainer);
    ratingCard->addWidget(ui->ratingPreviewContainer);   // the badges of the system, right below the choice

    QVBoxLayout *savingCard = addCard(left, tr("Saving"));
    auto *savingRow = new QHBoxLayout;
    ui->settingsGeneralForm->removeWidget(ui->safeSaveCheckBox);
    savingRow->addWidget(ui->safeSaveCheckBox);
    savingRow->addWidget(new InfoIcon(InfoIcon::savingNote(), this));
    savingRow->addStretch();
    savingCard->addLayout(savingRow);

    QVBoxLayout *applyCard = addCard(left, tr("Apply"));
    applyCard->addWidget(m_fillEmptyOnlyCheck);
    left->addStretch();

    // Right: own genres, TMDB
    QVBoxLayout *genresCard = addCard(right, tr("Own genres"));
    genresCard->addWidget(m_customGenresWidget);
    genresCard->addWidget(m_onlyCustomGenresCheck);   // belongs to the genres: right below them

    QVBoxLayout *tmdbCard = addCard(right, tr("TMDB"));
    auto *tmdbHint = new QLabel(tr("With an own token (API Read Access Token) the search uses your TMDB account. "
                                   "You get it free in your <a href=\"https://www.themoviedb.org/settings/api\">TMDB account</a>. "
                                   "Leave the field empty to use the token built into the app."), this);
    tmdbHint->setWordWrap(true);
    tmdbHint->setOpenExternalLinks(true);
    tmdbHint->setObjectName(QStringLiteral("tmdbHint"));
    m_tmdbTokenStatus = new QLabel(this);
    tmdbCard->addWidget(m_tmdbTokenEdit);
    tmdbCard->addWidget(m_tmdbTokenStatus);
    tmdbCard->addWidget(tmdbHint);
    right->addStretch();

    // The new cards take the place of the old one
    delete ui->settingsCardsRow->replaceWidget(ui->settingsGeneralCard, container);
    delete ui->settingsGeneralCard;   // with the labels of the old form
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
    // The rounded box: applyThemeStyles()
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
    // × or Delete: the cover is removed from the shown movie (the file changes when saving)
    connect(ui->coverLabel, &ImageLabelM::coverRemoveRequested, this, [applyCover] { applyCover(QPixmap()); });

    // ----- Drag and drop Files
    connect(ui->fileListTableView, &TableViewM::filesDropped, this, &MainWin::addFilesToModel);

    // ----- Status
    // Changes from UI saved in Dataset.
    // LineEdits
    connect(ui->titleEdit, &QLineEdit::textEdited, this, &MainWin::onDataChanged);
    connect(ui->titleEdit, &QLineEdit::textEdited, this, &MainWin::showTitleInSearch);   // after onDataChanged: the new title
    connect(ui->studioEdit, &QLineEdit::textEdited, this, &MainWin::onDataChanged);
    // TextEdit
    connect(ui->descriptionText, &QTextEdit::textChanged, this, &MainWin::onDataChanged);
    // ComboBoxes
    connect(ui->ratingComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWin::onDataChanged);
    connect(ui->advisoryComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWin::onDataChanged);
    connect(ui->genreComboBox, &QComboBox::editTextChanged, this, &MainWin::onDataChanged);
    connect(ui->ratingSettingsComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWin::onSettingsChanged);
    connect(ui->languageSettingsComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWin::onSettingsChanged);
    connect(ui->dataLanguageSettingsComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWin::onSettingsChanged);
    connect(ui->safeSaveCheckBox, &QCheckBox::toggled, this, &MainWin::onSettingsChanged);
    connect(m_customGenresWidget, &TagWidget::tagsChanged, this, &MainWin::onSettingsChanged);
    connect(m_onlyCustomGenresCheck, &QCheckBox::toggled, this, &MainWin::onSettingsChanged);
    connect(m_fillEmptyOnlyCheck, &QCheckBox::toggled, this, &MainWin::onSettingsChanged);
    connect(m_tmdbTokenEdit, &QLineEdit::textChanged, this, &MainWin::onSettingsChanged);
    connect(m_tmdbTokenEdit, &QLineEdit::textChanged, this, &MainWin::updateTmdbTokenStatus);
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
    connect(ui->fileListTableView, &TableViewM::removeRequested, this, &MainWin::onRemoveFilesButtonClicked);

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


