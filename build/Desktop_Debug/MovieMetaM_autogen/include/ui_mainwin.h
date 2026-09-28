/********************************************************************************
** Form generated from reading UI file 'mainwin.ui'
**
** Created by: Qt User Interface Compiler version 6.11.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWIN_H
#define UI_MAINWIN_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableView>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <imagelabelm.h>
#include "tableviewm.h"

QT_BEGIN_NAMESPACE

class Ui_MainWin
{
public:
    QAction *actionSave;
    QAction *actionExit;
    QAction *actionSearch;
    QAction *actionSaveAll;
    QAction *actionInfo;
    QAction *actionHelp;
    QAction *actionSettings;
    QAction *actionClose;
    QAction *actionSaveSettings;
    QWidget *centralwidget;
    QGridLayout *gridLayout_2;
    QStackedWidget *stackedWidget;
    QWidget *mainPage;
    QVBoxLayout *verticalLayout;
    QSplitter *splitter;
    QWidget *layoutLeft;
    QVBoxLayout *verticalLayout_6;
    QLabel *leftLabel;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_7;
    TableViewM *fileListTableView;
    QGridLayout *gridLayout_3;
    QToolButton *removeToolButton;
    QToolButton *addToolButton;
    QSpacerItem *horizontalSpacer;
    QWidget *layoutMiddle;
    QVBoxLayout *verticalLayout_2;
    QLabel *middleLabel;
    QFrame *middleGroupBox;
    QHBoxLayout *horizontalLayout_3;
    QVBoxLayout *middleLeftContainer;
    ImageLabelM *coverLabel;
    QLabel *descriptionLabel;
    QTextEdit *descriptionText;
    QLabel *releaseLabel;
    QDateEdit *releaseDateEdit;
    QLabel *genreLabel;
    QComboBox *genreComboBox;
    QLabel *ratingLabel;
    QComboBox *ratingComboBox;
    QGridLayout *middleRightContainer;
    QLineEdit *titleEdit;
    QLabel *producersLabel;
    QWidget *directorsContainer;
    QLineEdit *studioEdit;
    QLabel *directorsLabel;
    QWidget *castContainer;
    QWidget *producersContainer;
    QLabel *studioLabel;
    QWidget *screenwritersContainer;
    QLabel *screenplayLabel;
    QLabel *actorsLabel;
    QLabel *titleLabel;
    QWidget *layoutRight;
    QGridLayout *gridLayout_4;
    QLabel *tmdbImageLabel;
    QTabWidget *tabWidget;
    QWidget *tab_1;
    QVBoxLayout *verticalLayout_5;
    QTableView *searchTableView;
    QWidget *tab_2;
    QWidget *tab_3;
    QLabel *rightLabel;
    QLabel *poweredLabel;
    QLineEdit *searchEdit;
    QToolButton *searchToolButton;
    QWidget *settingsPage;
    QGridLayout *gridLayout;
    QWidget *settingsCentralLayout_2;
    QVBoxLayout *settingsCentralLayout;
    QLabel *settingsHeaderLabel;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QWidget *settingsContentBox;
    QGroupBox *settingsGeneralGroupBox;
    QLabel *settingsRatingLabel;
    QComboBox *ratingSettingsComboBox;
    QWidget *infoPage;
    QVBoxLayout *verticalLayout_3;
    QVBoxLayout *verticalLayout_4;
    QLabel *infoHeaderLabel;
    QTextBrowser *infoTextBrowser;
    QWidget *helpPage;
    QMenuBar *menubar;
    QStatusBar *statusBar;
    QToolBar *mainToolBar;
    QToolBar *infoToolBar;
    QToolBar *settingsToolBar;

    void setupUi(QMainWindow *MainWin)
    {
        if (MainWin->objectName().isEmpty())
            MainWin->setObjectName("MainWin");
        MainWin->resize(1200, 700);
        MainWin->setAcceptDrops(false);
        actionSave = new QAction(MainWin);
        actionSave->setObjectName("actionSave");
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/icons/save.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        actionSave->setIcon(icon);
        actionSave->setMenuRole(QAction::MenuRole::NoRole);
        actionExit = new QAction(MainWin);
        actionExit->setObjectName("actionExit");
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/icons/exit.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        actionExit->setIcon(icon1);
        actionExit->setMenuRole(QAction::MenuRole::NoRole);
        actionSearch = new QAction(MainWin);
        actionSearch->setObjectName("actionSearch");
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/icons/search.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        actionSearch->setIcon(icon2);
        actionSearch->setMenuRole(QAction::MenuRole::NoRole);
        actionSaveAll = new QAction(MainWin);
        actionSaveAll->setObjectName("actionSaveAll");
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/icons/save_all.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        actionSaveAll->setIcon(icon3);
        actionSaveAll->setMenuRole(QAction::MenuRole::NoRole);
        actionInfo = new QAction(MainWin);
        actionInfo->setObjectName("actionInfo");
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/icons/info.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        actionInfo->setIcon(icon4);
        actionInfo->setMenuRole(QAction::MenuRole::NoRole);
        actionHelp = new QAction(MainWin);
        actionHelp->setObjectName("actionHelp");
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/icons/help.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        actionHelp->setIcon(icon5);
        actionHelp->setMenuRole(QAction::MenuRole::NoRole);
        actionSettings = new QAction(MainWin);
        actionSettings->setObjectName("actionSettings");
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/icons/settings.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        actionSettings->setIcon(icon6);
        actionSettings->setMenuRole(QAction::MenuRole::NoRole);
        actionClose = new QAction(MainWin);
        actionClose->setObjectName("actionClose");
        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/icons/close.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        actionClose->setIcon(icon7);
        actionClose->setMenuRole(QAction::MenuRole::NoRole);
        actionSaveSettings = new QAction(MainWin);
        actionSaveSettings->setObjectName("actionSaveSettings");
        actionSaveSettings->setIcon(icon);
        actionSaveSettings->setMenuRole(QAction::MenuRole::NoRole);
        centralwidget = new QWidget(MainWin);
        centralwidget->setObjectName("centralwidget");
        gridLayout_2 = new QGridLayout(centralwidget);
        gridLayout_2->setObjectName("gridLayout_2");
        stackedWidget = new QStackedWidget(centralwidget);
        stackedWidget->setObjectName("stackedWidget");
        stackedWidget->setMinimumSize(QSize(400, 0));
        mainPage = new QWidget();
        mainPage->setObjectName("mainPage");
        verticalLayout = new QVBoxLayout(mainPage);
        verticalLayout->setObjectName("verticalLayout");
        splitter = new QSplitter(mainPage);
        splitter->setObjectName("splitter");
        splitter->setOrientation(Qt::Orientation::Horizontal);
        layoutLeft = new QWidget(splitter);
        layoutLeft->setObjectName("layoutLeft");
        verticalLayout_6 = new QVBoxLayout(layoutLeft);
        verticalLayout_6->setObjectName("verticalLayout_6");
        verticalLayout_6->setContentsMargins(-1, -1, 18, -1);
        leftLabel = new QLabel(layoutLeft);
        leftLabel->setObjectName("leftLabel");
        leftLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_6->addWidget(leftLabel);

        groupBox = new QGroupBox(layoutLeft);
        groupBox->setObjectName("groupBox");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(groupBox->sizePolicy().hasHeightForWidth());
        groupBox->setSizePolicy(sizePolicy);
        verticalLayout_7 = new QVBoxLayout(groupBox);
        verticalLayout_7->setObjectName("verticalLayout_7");
        fileListTableView = new TableViewM(groupBox);
        fileListTableView->setObjectName("fileListTableView");
        fileListTableView->setFrameShape(QFrame::Shape::NoFrame);
        fileListTableView->setSelectionMode(QAbstractItemView::SelectionMode::NoSelection);
        fileListTableView->setShowGrid(false);
        fileListTableView->setGridStyle(Qt::PenStyle::SolidLine);

        verticalLayout_7->addWidget(fileListTableView);

        gridLayout_3 = new QGridLayout();
        gridLayout_3->setObjectName("gridLayout_3");
        removeToolButton = new QToolButton(groupBox);
        removeToolButton->setObjectName("removeToolButton");
        QIcon icon8;
        icon8.addFile(QString::fromUtf8(":/icons/delete.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        removeToolButton->setIcon(icon8);

        gridLayout_3->addWidget(removeToolButton, 0, 2, 1, 1);

        addToolButton = new QToolButton(groupBox);
        addToolButton->setObjectName("addToolButton");
        QIcon icon9;
        icon9.addFile(QString::fromUtf8(":/icons/new.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        addToolButton->setIcon(icon9);

        gridLayout_3->addWidget(addToolButton, 0, 0, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout_3->addItem(horizontalSpacer, 0, 1, 1, 1);


        verticalLayout_7->addLayout(gridLayout_3);


        verticalLayout_6->addWidget(groupBox);

        splitter->addWidget(layoutLeft);
        layoutMiddle = new QWidget(splitter);
        layoutMiddle->setObjectName("layoutMiddle");
        verticalLayout_2 = new QVBoxLayout(layoutMiddle);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout_2->setContentsMargins(18, -1, 18, -1);
        middleLabel = new QLabel(layoutMiddle);
        middleLabel->setObjectName("middleLabel");
        middleLabel->setFrameShape(QFrame::Shape::StyledPanel);
        middleLabel->setFrameShadow(QFrame::Shadow::Plain);
        middleLabel->setLineWidth(4);
        middleLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_2->addWidget(middleLabel);

        middleGroupBox = new QFrame(layoutMiddle);
        middleGroupBox->setObjectName("middleGroupBox");
        horizontalLayout_3 = new QHBoxLayout(middleGroupBox);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        horizontalLayout_3->setContentsMargins(-1, -1, -1, 0);
        middleLeftContainer = new QVBoxLayout();
        middleLeftContainer->setObjectName("middleLeftContainer");
        coverLabel = new ImageLabelM(middleGroupBox);
        coverLabel->setObjectName("coverLabel");
        coverLabel->setLayoutDirection(Qt::LayoutDirection::LeftToRight);
        coverLabel->setFrameShape(QFrame::Shape::Box);

        middleLeftContainer->addWidget(coverLabel);

        descriptionLabel = new QLabel(middleGroupBox);
        descriptionLabel->setObjectName("descriptionLabel");

        middleLeftContainer->addWidget(descriptionLabel);

        descriptionText = new QTextEdit(middleGroupBox);
        descriptionText->setObjectName("descriptionText");

        middleLeftContainer->addWidget(descriptionText);

        releaseLabel = new QLabel(middleGroupBox);
        releaseLabel->setObjectName("releaseLabel");

        middleLeftContainer->addWidget(releaseLabel);

        releaseDateEdit = new QDateEdit(middleGroupBox);
        releaseDateEdit->setObjectName("releaseDateEdit");

        middleLeftContainer->addWidget(releaseDateEdit);

        genreLabel = new QLabel(middleGroupBox);
        genreLabel->setObjectName("genreLabel");
        genreLabel->setAlignment(Qt::AlignmentFlag::AlignLeading|Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignTop);

        middleLeftContainer->addWidget(genreLabel);

        genreComboBox = new QComboBox(middleGroupBox);
        genreComboBox->setObjectName("genreComboBox");

        middleLeftContainer->addWidget(genreComboBox);

        ratingLabel = new QLabel(middleGroupBox);
        ratingLabel->setObjectName("ratingLabel");
        ratingLabel->setLayoutDirection(Qt::LayoutDirection::LeftToRight);
        ratingLabel->setAlignment(Qt::AlignmentFlag::AlignLeading|Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignTop);

        middleLeftContainer->addWidget(ratingLabel);

        ratingComboBox = new QComboBox(middleGroupBox);
        ratingComboBox->setObjectName("ratingComboBox");

        middleLeftContainer->addWidget(ratingComboBox);


        horizontalLayout_3->addLayout(middleLeftContainer);

        middleRightContainer = new QGridLayout();
        middleRightContainer->setObjectName("middleRightContainer");
        middleRightContainer->setSizeConstraint(QLayout::SizeConstraint::SetDefaultConstraint);
        titleEdit = new QLineEdit(middleGroupBox);
        titleEdit->setObjectName("titleEdit");

        middleRightContainer->addWidget(titleEdit, 3, 1, 1, 1);

        producersLabel = new QLabel(middleGroupBox);
        producersLabel->setObjectName("producersLabel");
        producersLabel->setLayoutDirection(Qt::LayoutDirection::LeftToRight);
        producersLabel->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTop|Qt::AlignmentFlag::AlignTrailing);

        middleRightContainer->addWidget(producersLabel, 8, 0, 1, 1);

        directorsContainer = new QWidget(middleGroupBox);
        directorsContainer->setObjectName("directorsContainer");

        middleRightContainer->addWidget(directorsContainer, 6, 1, 1, 1);

        studioEdit = new QLineEdit(middleGroupBox);
        studioEdit->setObjectName("studioEdit");

        middleRightContainer->addWidget(studioEdit, 9, 1, 1, 1);

        directorsLabel = new QLabel(middleGroupBox);
        directorsLabel->setObjectName("directorsLabel");
        directorsLabel->setLayoutDirection(Qt::LayoutDirection::LeftToRight);
        directorsLabel->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTop|Qt::AlignmentFlag::AlignTrailing);

        middleRightContainer->addWidget(directorsLabel, 6, 0, 1, 1);

        castContainer = new QWidget(middleGroupBox);
        castContainer->setObjectName("castContainer");

        middleRightContainer->addWidget(castContainer, 5, 1, 1, 1);

        producersContainer = new QWidget(middleGroupBox);
        producersContainer->setObjectName("producersContainer");

        middleRightContainer->addWidget(producersContainer, 8, 1, 1, 1);

        studioLabel = new QLabel(middleGroupBox);
        studioLabel->setObjectName("studioLabel");
        studioLabel->setLayoutDirection(Qt::LayoutDirection::LeftToRight);
        studioLabel->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTop|Qt::AlignmentFlag::AlignTrailing);

        middleRightContainer->addWidget(studioLabel, 9, 0, 1, 1);

        screenwritersContainer = new QWidget(middleGroupBox);
        screenwritersContainer->setObjectName("screenwritersContainer");

        middleRightContainer->addWidget(screenwritersContainer, 7, 1, 1, 1);

        screenplayLabel = new QLabel(middleGroupBox);
        screenplayLabel->setObjectName("screenplayLabel");
        screenplayLabel->setLayoutDirection(Qt::LayoutDirection::LeftToRight);
        screenplayLabel->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTop|Qt::AlignmentFlag::AlignTrailing);

        middleRightContainer->addWidget(screenplayLabel, 7, 0, 1, 1);

        actorsLabel = new QLabel(middleGroupBox);
        actorsLabel->setObjectName("actorsLabel");
        actorsLabel->setLayoutDirection(Qt::LayoutDirection::LeftToRight);
        actorsLabel->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTop|Qt::AlignmentFlag::AlignTrailing);

        middleRightContainer->addWidget(actorsLabel, 5, 0, 1, 1);

        titleLabel = new QLabel(middleGroupBox);
        titleLabel->setObjectName("titleLabel");
        titleLabel->setLayoutDirection(Qt::LayoutDirection::LeftToRight);
        titleLabel->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTop|Qt::AlignmentFlag::AlignTrailing);

        middleRightContainer->addWidget(titleLabel, 3, 0, 1, 1);


        horizontalLayout_3->addLayout(middleRightContainer);

        horizontalLayout_3->setStretch(1, 2);

        verticalLayout_2->addWidget(middleGroupBox);

        splitter->addWidget(layoutMiddle);
        layoutRight = new QWidget(splitter);
        layoutRight->setObjectName("layoutRight");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(layoutRight->sizePolicy().hasHeightForWidth());
        layoutRight->setSizePolicy(sizePolicy1);
        layoutRight->setMaximumSize(QSize(16777215, 16777215));
        gridLayout_4 = new QGridLayout(layoutRight);
        gridLayout_4->setObjectName("gridLayout_4");
        gridLayout_4->setContentsMargins(18, -1, -1, 0);
        tmdbImageLabel = new QLabel(layoutRight);
        tmdbImageLabel->setObjectName("tmdbImageLabel");
        QSizePolicy sizePolicy2(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(tmdbImageLabel->sizePolicy().hasHeightForWidth());
        tmdbImageLabel->setSizePolicy(sizePolicy2);
        tmdbImageLabel->setPixmap(QPixmap(QString::fromUtf8(":/tmdb.svg")));

        gridLayout_4->addWidget(tmdbImageLabel, 3, 1, 1, 2);

        tabWidget = new QTabWidget(layoutRight);
        tabWidget->setObjectName("tabWidget");
        tabWidget->setTabShape(QTabWidget::TabShape::Rounded);
        tab_1 = new QWidget();
        tab_1->setObjectName("tab_1");
        verticalLayout_5 = new QVBoxLayout(tab_1);
        verticalLayout_5->setObjectName("verticalLayout_5");
        searchTableView = new QTableView(tab_1);
        searchTableView->setObjectName("searchTableView");
        searchTableView->setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
        searchTableView->setAlternatingRowColors(true);
        searchTableView->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
        searchTableView->verticalHeader()->setVisible(false);
        searchTableView->verticalHeader()->setHighlightSections(true);

        verticalLayout_5->addWidget(searchTableView);

        tabWidget->addTab(tab_1, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName("tab_2");
        tabWidget->addTab(tab_2, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName("tab_3");
        tabWidget->addTab(tab_3, QString());

        gridLayout_4->addWidget(tabWidget, 7, 0, 1, 3);

        rightLabel = new QLabel(layoutRight);
        rightLabel->setObjectName("rightLabel");
        rightLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);

        gridLayout_4->addWidget(rightLabel, 0, 0, 1, 3);

        poweredLabel = new QLabel(layoutRight);
        poweredLabel->setObjectName("poweredLabel");

        gridLayout_4->addWidget(poweredLabel, 3, 0, 1, 1);

        searchEdit = new QLineEdit(layoutRight);
        searchEdit->setObjectName("searchEdit");

        gridLayout_4->addWidget(searchEdit, 4, 0, 1, 2);

        searchToolButton = new QToolButton(layoutRight);
        searchToolButton->setObjectName("searchToolButton");
        searchToolButton->setIcon(icon2);

        gridLayout_4->addWidget(searchToolButton, 4, 2, 1, 1);

        splitter->addWidget(layoutRight);

        verticalLayout->addWidget(splitter);

        stackedWidget->addWidget(mainPage);
        settingsPage = new QWidget();
        settingsPage->setObjectName("settingsPage");
        gridLayout = new QGridLayout(settingsPage);
        gridLayout->setObjectName("gridLayout");
        settingsCentralLayout_2 = new QWidget(settingsPage);
        settingsCentralLayout_2->setObjectName("settingsCentralLayout_2");
        settingsCentralLayout = new QVBoxLayout(settingsCentralLayout_2);
        settingsCentralLayout->setObjectName("settingsCentralLayout");
        settingsHeaderLabel = new QLabel(settingsCentralLayout_2);
        settingsHeaderLabel->setObjectName("settingsHeaderLabel");
        settingsHeaderLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(170, 255, 255);"));

        settingsCentralLayout->addWidget(settingsHeaderLabel);

        widget = new QWidget(settingsCentralLayout_2);
        widget->setObjectName("widget");
        sizePolicy1.setHeightForWidth(widget->sizePolicy().hasHeightForWidth());
        widget->setSizePolicy(sizePolicy1);
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setObjectName("horizontalLayout");
        settingsContentBox = new QWidget(widget);
        settingsContentBox->setObjectName("settingsContentBox");
        settingsContentBox->setMaximumSize(QSize(800, 16777215));
        settingsGeneralGroupBox = new QGroupBox(settingsContentBox);
        settingsGeneralGroupBox->setObjectName("settingsGeneralGroupBox");
        settingsGeneralGroupBox->setGeometry(QRect(20, 10, 571, 311));
        settingsGeneralGroupBox->setStyleSheet(QString::fromUtf8("QGroupBox::title {  \n"
"background-color: grey;\n"
" subcontrol-position: top left; /* position at the top left*/ \n"
" padding: 2px 13px;\n"
"}"));
        settingsRatingLabel = new QLabel(settingsGeneralGroupBox);
        settingsRatingLabel->setObjectName("settingsRatingLabel");
        settingsRatingLabel->setGeometry(QRect(20, 40, 151, 31));
        ratingSettingsComboBox = new QComboBox(settingsGeneralGroupBox);
        ratingSettingsComboBox->setObjectName("ratingSettingsComboBox");
        ratingSettingsComboBox->setGeometry(QRect(180, 40, 201, 30));

        horizontalLayout->addWidget(settingsContentBox);


        settingsCentralLayout->addWidget(widget);


        gridLayout->addWidget(settingsCentralLayout_2, 5, 0, 1, 1);

        stackedWidget->addWidget(settingsPage);
        infoPage = new QWidget();
        infoPage->setObjectName("infoPage");
        verticalLayout_3 = new QVBoxLayout(infoPage);
        verticalLayout_3->setObjectName("verticalLayout_3");
        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setObjectName("verticalLayout_4");
        infoHeaderLabel = new QLabel(infoPage);
        infoHeaderLabel->setObjectName("infoHeaderLabel");

        verticalLayout_4->addWidget(infoHeaderLabel);

        infoTextBrowser = new QTextBrowser(infoPage);
        infoTextBrowser->setObjectName("infoTextBrowser");

        verticalLayout_4->addWidget(infoTextBrowser);


        verticalLayout_3->addLayout(verticalLayout_4);

        stackedWidget->addWidget(infoPage);
        helpPage = new QWidget();
        helpPage->setObjectName("helpPage");
        stackedWidget->addWidget(helpPage);

        gridLayout_2->addWidget(stackedWidget, 0, 0, 1, 1);

        MainWin->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWin);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1200, 27));
        MainWin->setMenuBar(menubar);
        statusBar = new QStatusBar(MainWin);
        statusBar->setObjectName("statusBar");
        MainWin->setStatusBar(statusBar);
        mainToolBar = new QToolBar(MainWin);
        mainToolBar->setObjectName("mainToolBar");
        MainWin->addToolBar(Qt::ToolBarArea::TopToolBarArea, mainToolBar);
        infoToolBar = new QToolBar(MainWin);
        infoToolBar->setObjectName("infoToolBar");
        MainWin->addToolBar(Qt::ToolBarArea::TopToolBarArea, infoToolBar);
        settingsToolBar = new QToolBar(MainWin);
        settingsToolBar->setObjectName("settingsToolBar");
        MainWin->addToolBar(Qt::ToolBarArea::TopToolBarArea, settingsToolBar);

        mainToolBar->addAction(actionSave);
        mainToolBar->addAction(actionSaveAll);
        settingsToolBar->addAction(actionSaveSettings);

        retranslateUi(MainWin);

        stackedWidget->setCurrentIndex(0);
        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWin);
    } // setupUi

    void retranslateUi(QMainWindow *MainWin)
    {
        MainWin->setWindowTitle(QCoreApplication::translate("MainWin", "MovieMetaM 0.1", nullptr));
        actionSave->setText(QCoreApplication::translate("MainWin", "SAVE", nullptr));
#if QT_CONFIG(tooltip)
        actionSave->setToolTip(QCoreApplication::translate("MainWin", "SAVE_METADATA_TO_MOVIE", nullptr));
#endif // QT_CONFIG(tooltip)
        actionExit->setText(QCoreApplication::translate("MainWin", "EXIT", nullptr));
#if QT_CONFIG(tooltip)
        actionExit->setToolTip(QCoreApplication::translate("MainWin", "EXIT_PROGRAM", nullptr));
#endif // QT_CONFIG(tooltip)
        actionSearch->setText(QCoreApplication::translate("MainWin", "SEARCH", nullptr));
        actionSaveAll->setText(QCoreApplication::translate("MainWin", "SAVE_ALL", nullptr));
        actionInfo->setText(QCoreApplication::translate("MainWin", "INFO", nullptr));
        actionHelp->setText(QCoreApplication::translate("MainWin", "HELP", nullptr));
        actionSettings->setText(QCoreApplication::translate("MainWin", "SETTINGS", nullptr));
#if QT_CONFIG(tooltip)
        actionSettings->setToolTip(QCoreApplication::translate("MainWin", "SETTINGS", nullptr));
#endif // QT_CONFIG(tooltip)
        actionClose->setText(QCoreApplication::translate("MainWin", "CLOSE", nullptr));
#if QT_CONFIG(tooltip)
        actionClose->setToolTip(QCoreApplication::translate("MainWin", "CLOSE", nullptr));
#endif // QT_CONFIG(tooltip)
        actionSaveSettings->setText(QCoreApplication::translate("MainWin", "SAVE_SETTINGS", nullptr));
        leftLabel->setText(QCoreApplication::translate("MainWin", "MOVIE_LIST", nullptr));
        groupBox->setTitle(QString());
        removeToolButton->setText(QCoreApplication::translate("MainWin", "...", nullptr));
        addToolButton->setText(QCoreApplication::translate("MainWin", "...", nullptr));
        middleLabel->setText(QCoreApplication::translate("MainWin", "MOVIE_INFO", nullptr));
        coverLabel->setText(QCoreApplication::translate("MainWin", "COVER", nullptr));
        descriptionLabel->setText(QCoreApplication::translate("MainWin", "DESCRIPTION", nullptr));
        releaseLabel->setText(QCoreApplication::translate("MainWin", "RELEASE_DATE", nullptr));
        genreLabel->setText(QCoreApplication::translate("MainWin", "GENRE", nullptr));
        ratingLabel->setText(QCoreApplication::translate("MainWin", "AGE_RATING", nullptr));
        producersLabel->setText(QCoreApplication::translate("MainWin", "PRODUCERS", nullptr));
        directorsLabel->setText(QCoreApplication::translate("MainWin", "DIRECTORS", nullptr));
        studioLabel->setText(QCoreApplication::translate("MainWin", "STUDIO", nullptr));
        screenplayLabel->setText(QCoreApplication::translate("MainWin", "SCREENPLAY", nullptr));
        actorsLabel->setText(QCoreApplication::translate("MainWin", "ACTORS", nullptr));
        titleLabel->setText(QCoreApplication::translate("MainWin", "TITLE", nullptr));
        tmdbImageLabel->setText(QString());
        tabWidget->setTabText(tabWidget->indexOf(tab_1), QCoreApplication::translate("MainWin", "RESULTS", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QCoreApplication::translate("MainWin", "DATA", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QCoreApplication::translate("MainWin", "COVER", nullptr));
        rightLabel->setText(QCoreApplication::translate("MainWin", "MOVIE_SEARCH", nullptr));
        poweredLabel->setText(QCoreApplication::translate("MainWin", "Powered by", nullptr));
        searchToolButton->setText(QCoreApplication::translate("MainWin", "...", nullptr));
        settingsHeaderLabel->setText(QCoreApplication::translate("MainWin", "SETTINGS", nullptr));
        settingsGeneralGroupBox->setTitle(QCoreApplication::translate("MainWin", "[GENERAL]", nullptr));
        settingsRatingLabel->setText(QCoreApplication::translate("MainWin", "RATING", nullptr));
        infoHeaderLabel->setText(QCoreApplication::translate("MainWin", "INFO", nullptr));
        mainToolBar->setWindowTitle(QCoreApplication::translate("MainWin", "toolBar", nullptr));
        infoToolBar->setWindowTitle(QCoreApplication::translate("MainWin", "toolBar", nullptr));
        settingsToolBar->setWindowTitle(QCoreApplication::translate("MainWin", "toolBar", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWin: public Ui_MainWin {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWIN_H
