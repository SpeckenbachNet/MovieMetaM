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
#include "agerating.h"
#include "themecolors.h"

#include <algorithm>

SetupDialog::SetupDialog(QWidget *parent) : QDialog(parent) {

    // initialize members
    infoLabel = new QLabel(this);
    infoLabel->setText(tr("<h2>Welcome to MovieMetaM!</h2>"
                          "<p>Since this is your first time using the program, we need to configure a few basic settings.</p>"
                          "<p>In this step, we define how age ratings in your collection are weighted to ensure a consistent metadata structure.</p>"
                          "<p>Note: if needed, you can choose a language other than your system language here (optional).</p>"));
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

    languageLabel = new QLabel(this);
    languageLabel->setText(tr("Choose another language (optional)"));
    languageLabel->setStyleSheet("font-weight: bold; margin-top: 10px;");

    languageComboBox = new QComboBox(this);
    languageComboBox->clear();
    languageComboBox->addItem("--- " + tr("Choose an entry") + " ---");
    languageComboBox->addItem(QStringLiteral("Deutsch (de - german)"));
    languageComboBox->addItem(QStringLiteral("English (en)"));
    languageComboBox->addItem(QStringLiteral("Italiano (it - italian)"));
    languageComboBox->addItem(QStringLiteral("Français (fr - french)"));
    languageComboBox->addItem(QStringLiteral("Nederlands (nl - dutch)"));
    languageComboBox->addItem(QStringLiteral("Dansk (da - danish)"));
    languageComboBox->addItem(QStringLiteral("Polski (pl - polish)"));
    languageComboBox->addItem(QStringLiteral("日本語 (ja - japanese)"));
    languageComboBox->addItem(QStringLiteral("한국어 (ko - korean)"));
    languageComboBox->addItem(QStringLiteral("繁體中文 (zh - chinese)"));
    languageComboBox->model()->sort(0);

    // Rounded like the combo boxes of the main window
    applyComboStyle(ageRatingComboBox);
    applyComboStyle(languageComboBox);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Done"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    // Jetzt korrekt: Ok ist ein StandardButton
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
    layout->addWidget(ageRatingLabel);
    layout->addWidget(ageRatingComboBox);
    layout->addSpacing(10);
    layout->addWidget(languageLabel);
    layout->addWidget(languageComboBox);
    layout->addStretch();
    layout->addWidget(buttonBox);

    setMinimumSize(420, 320);
}

QString SetupDialog::selectedOptionFromAgeRating() const
{
    return ageRatingComboBox->currentData().toString();
}
