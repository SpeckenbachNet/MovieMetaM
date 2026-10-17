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

    // Settings as INI file on every system (Windows would use the registry otherwise)
    QSettings::setDefaultFormat(QSettings::IniFormat);

    // 1. Sprache aus den QSettings laden
    QSettings settings;
    // Standardmäßig leer, falls noch nie gesetzt
    QString langCode = settings.value("language", "").toString();

    // 2. Falls Config leer ist, Systemsprache als Basis nehmen
    if (langCode.isEmpty()) {
        // bcp47Name() liefert z.B. "de" oder "en".
        // Wir nehmen nur die ersten zwei Zeichen für das 2-Letter-Kürzel
        langCode = QLocale::system().name().left(2);
    }

    QTranslator translator;

    // 3. Versuchen, die Sprache zu laden (z.B. MovieMetaM_de.qm)
    if (translator.load(":/i18n/" APP_NAME "_" + langCode)) {
        a.installTranslator(&translator);
    }
    // 4. FALLBACK: Wenn die Sprache nicht geladen werden konnte und nicht schon Englisch ist
    else if (langCode != "en") {
        if (translator.load(":/i18n/" APP_NAME "_en")) {
            a.installTranslator(&translator);
        }
    }

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

        // Werte aus dem Dialog in die Settings übernehmen
        settings.setValue("General/AgeRatingSystem", setupDialog.selectedOptionFromAgeRating());

        // Optional: Explizit synchronisieren (meist nicht nötig, aber sicher)
        settings.sync();
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
