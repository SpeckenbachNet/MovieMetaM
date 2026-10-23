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
#include "setupdialog.h"          // My setup dialog
#include "languages.h"
#include "themecolors.h"

#include <QApplication>
#include <QLibraryInfo>
#include <QLocale>
#include <QTranslator>
#include <QStringLiteral>
#include "bubble.h"
#include "fileopener.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName(QStringLiteral(APP_NAME));
    a.setApplicationVersion(QLatin1String(APP_VERSION));
    a.setOrganizationName(QStringLiteral(COMPANY_NAME));
    a.setWindowIcon(QIcon(QStringLiteral(":/icons/app_icon")));   // title bar, task bar, dialogs
    // Linux: Wayland app id, ties the window to net.speckenbach.MovieMetaM.desktop (icon in the task bar)
    a.setDesktopFileName(QStringLiteral("net.speckenbach.MovieMetaM"));

    // Movie files from outside (Open with, dropped on the program icon, command line): a second
    // instance hands them to the running one and quits; macOS delivers them as an event
    FileOpener opener;
    const QStringList startFiles = FileOpener::pathsFromArguments(a.arguments());
    if (opener.handOverToRunningInstance(startFiles)) {
        return 0;
    }
    opener.listen();
    a.installEventFilter(&opener);

    TooltipFilter::install();   // tooltips as rounded bubbles, like the rest of the app
    fixHighlightContrast();   // readable selection also with the accent color "Graphite" (macOS)

    // Settings as INI file on every system (Windows would use the registry otherwise)
    QSettings::setDefaultFormat(QSettings::IniFormat);

    QSettings settings;

    // Language of the app: from the settings, else the one of the system (German or English)
    // and Qt's own texts (standard buttons "Yes" / "No" / "Cancel", file dialog) in the same language
    QTranslator translator;
    QTranslator qtTranslator;
    auto loadTranslation = [&a, &translator, &qtTranslator](const QString &langCode) {
        a.removeTranslator(&translator);
        if (translator.load(":/i18n/" APP_NAME "_" + langCode)) {
            a.installTranslator(&translator);
        }
        a.removeTranslator(&qtTranslator);
        // Embedded in the app (see CMakeLists.txt), else from the Qt installation
        if (qtTranslator.load(QStringLiteral(":/i18n/qtbase_") + langCode)
            || qtTranslator.load(QLocale(langCode), QStringLiteral("qtbase"), QStringLiteral("_"),
                                 QLibraryInfo::path(QLibraryInfo::TranslationsPath))) {
            a.installTranslator(&qtTranslator);
        }
    };
    QString langCode = Languages::appLanguage();
    loadTranslation(langCode);

    // === Set up the application as needed. ===
    // Check if the initial setup has already been completed.
    bool hasRequiredSettings = settings.contains("General/AgeRatingSystem");

    if (!hasRequiredSettings)
    {
        SetupDialog setupDialog;

        if (setupDialog.exec() == QDialog::Rejected)
        {
            return 0; // Cancel -> Exit Application
        }

        // Take the values from the dialog into the settings
        settings.setValue("General/AgeRatingSystem", setupDialog.selectedOptionFromAgeRating());
        settings.setValue("language", setupDialog.selectedAppLanguage());
        settings.setValue("General/DataLanguage", setupDialog.selectedDataLanguage());
        settings.setValue("General/SafeSave", setupDialog.selectedSafeSaving());

        // Optional: sync explicitly (usually not needed, but safe)
        settings.sync();

        // Another language of the app chosen: the main window starts in it
        if (setupDialog.selectedAppLanguage() != langCode) {
            loadTranslation(setupDialog.selectedAppLanguage());
        }
    }

    // === Start main window ===
    MainWin w;
    w.setWindowTitle(
        QStringLiteral(APP_NAME) +
        " " +
        QStringLiteral(APP_VERSION)
    );
    QObject::connect(&opener, &FileOpener::filesReceived, &w, &MainWin::openFiles);
    w.show();
    if (!startFiles.isEmpty()) {
        w.openFiles(startFiles);
    }
    opener.start();   // files that came meanwhile (macOS: the event after the start)
    return a.exec();
}
