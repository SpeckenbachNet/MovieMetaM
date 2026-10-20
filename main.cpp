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
#include <QLocale>
#include <QTranslator>
#include <QStringLiteral>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName(QStringLiteral(APP_NAME));
    a.setApplicationVersion(QLatin1String(APP_VERSION));
    a.setOrganizationName(QStringLiteral(COMPANY_NAME));
    a.setWindowIcon(QIcon(QStringLiteral(":/icons/app_icon")));   // title bar, task bar, dialogs
    fixHighlightContrast();   // readable selection also with the accent color "Graphite" (macOS)

    // Settings as INI file on every system (Windows would use the registry otherwise)
    QSettings::setDefaultFormat(QSettings::IniFormat);

    QSettings settings;

    // Language of the app: from the settings, else the one of the system (German or English)
    QTranslator translator;
    auto loadTranslation = [&a, &translator](const QString &langCode) {
        a.removeTranslator(&translator);
        if (translator.load(":/i18n/" APP_NAME "_" + langCode)) {
            a.installTranslator(&translator);
        }
    };
    QString langCode = Languages::appLanguage();
    loadTranslation(langCode);

    // === Set up the application as needed. ===
    // Check if the initial setup has already been completed.
    bool hasRequiredSettings = settings.contains("General/AgeRatingSystem");
    qDebug() << hasRequiredSettings;

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
    w.show();
    return a.exec();
}
