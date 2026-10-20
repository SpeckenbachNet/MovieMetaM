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

// setupdialog.cpp
#include "setupdialog.h"
#include "infoicon.h"
#include "agerating.h"
#include "languages.h"
#include "themecolors.h"

#include <algorithm>

SetupDialog::SetupDialog(QWidget *parent) : QDialog(parent) {

    // initialize members
    infoLabel = new QLabel(this);
    infoLabel->setText(tr("<h2>Welcome to MovieMetaM!</h2>"
                          "<p>Since this is your first time using the program, we need to configure a few basic settings.</p>"
                          "<p>In this step, we define how age ratings in your collection are weighted to ensure a consistent metadata structure.</p>"
                          "<p>Note: both languages follow your system. You can change them below (optional) and later in the settings. The language of the app takes effect after a restart.</p>"));
    infoLabel->setWordWrap(true);
    infoLabel->setTextFormat(Qt::RichText);
    infoLabel->setOpenExternalLinks(false);

    ageRatingLabel = new QLabel(this);
    ageRatingLabel->setText(tr("Please choose an age rating system"));
    ageRatingLabel->setStyleSheet("font-weight: bold; margin-top: 10px;");

    ageRatingComboBox = new QComboBox(this);
    ageRatingComboBox->clear();
    // The same systems as in the settings (one JSON file per country, see agerating.h);
    // sorted by name, the "choose" entry stays on top
    QStringList codes = AgeRatings::systems().keys();
    std::sort(codes.begin(), codes.end(), [](const QString &a, const QString &b) {
        return AgeRatings::systems().value(a).displayName().localeAwareCompare(
                   AgeRatings::systems().value(b).displayName()) < 0;
    });
    ageRatingComboBox->addItem("--- " + tr("Choose an entry") + " ---");
    for (const QString &code : std::as_const(codes)) {
        ageRatingComboBox->addItem(AgeRatings::systems().value(code).displayName(), code);
    }

    // Language of the app (German or English) and of the movie data (TMDB); both start with
    // the one of the system
    languageLabel = new QLabel(this);
    languageLabel->setText(tr("Language of the app"));
    languageLabel->setStyleSheet("font-weight: bold; margin-top: 10px;");

    languageComboBox = new QComboBox(this);
    for (const Languages::Language &language : Languages::appLanguages()) {
        languageComboBox->addItem(language.name, language.code);
    }
    languageComboBox->setCurrentIndex(qMax(0, languageComboBox->findData(Languages::appLanguage())));

    dataLanguageLabel = new QLabel(this);
    dataLanguageLabel->setText(tr("Language of the movie data (TMDB)"));
    dataLanguageLabel->setStyleSheet("font-weight: bold; margin-top: 10px;");

    dataLanguageComboBox = new QComboBox(this);
    for (const Languages::Language &language : Languages::dataLanguages()) {
        dataLanguageComboBox->addItem(language.name, language.code);
    }
    dataLanguageComboBox->setCurrentIndex(qMax(0, dataLanguageComboBox->findData(Languages::dataLanguage())));

    // Saving: the check box has the note about writing directly into the files as an info icon
    safeSaveCheckBox = new QCheckBox(tr("Safe saving (read the info)"), this);

    // Rounded like the combo boxes of the main window
    applyComboStyle(ageRatingComboBox);
    applyComboStyle(languageComboBox);
    applyComboStyle(dataLanguageComboBox);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Done"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    // Ok is a standard button
    QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
    okButton->setEnabled(false);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    connect(ageRatingComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, [okButton](int index) {
            okButton->setEnabled(index > 0);
        });

    // Create layout
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(infoLabel);
    layout->addSpacing(10);
    layout->addWidget(languageLabel);
    layout->addWidget(languageComboBox);
    layout->addWidget(dataLanguageLabel);
    layout->addWidget(dataLanguageComboBox);
    layout->addSpacing(10);
    layout->addWidget(ageRatingLabel);
    layout->addWidget(ageRatingComboBox);
    layout->addSpacing(10);
    auto *safeSaveRow = new QHBoxLayout();
    safeSaveRow->addWidget(safeSaveCheckBox);
    safeSaveRow->addWidget(new InfoIcon(InfoIcon::savingNote(), this));
    safeSaveRow->addStretch();
    layout->addLayout(safeSaveRow);
    layout->addStretch();
    layout->addWidget(buttonBox);

    setMinimumSize(420, 320);
}

QString SetupDialog::selectedOptionFromAgeRating() const
{
    return ageRatingComboBox->currentData().toString();
}

QString SetupDialog::selectedAppLanguage() const
{
    return languageComboBox->currentData().toString();
}

QString SetupDialog::selectedDataLanguage() const
{
    return dataLanguageComboBox->currentData().toString();
}

bool SetupDialog::selectedSafeSaving() const
{
    return safeSaveCheckBox->isChecked();
}
