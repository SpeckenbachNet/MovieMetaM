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

SetupDialog::SetupDialog(QWidget *parent) : QDialog(parent) {

    // initialize members
    infoLabel = new QLabel(this);
    infoLabel->setText(tr("SETUP_INFO_TEXT"));
    infoLabel->setWordWrap(true);
    infoLabel->setTextFormat(Qt::RichText);
    infoLabel->setOpenExternalLinks(false);

    ageRatingLabel = new QLabel(this);
    ageRatingLabel->setText(tr("CHOOSE_AGE_RATING_SYSTEM"));
    ageRatingLabel->setStyleSheet("font-weight: bold; margin-top: 10px;");

    ageRatingComboBox = new QComboBox(this);
    ageRatingComboBox->clear();
    ageRatingComboBox->addItem("--- " + tr("CHOOSE_ENTRY") + " ---");
    ageRatingComboBox->addItem(tr("GERMANY_AR"),"DE");
    ageRatingComboBox->addItem(tr("NETHERLANDS_AR"),"NL");
    ageRatingComboBox->addItem(tr("AUSTRIA_AR"),"AT");
    ageRatingComboBox->addItem(tr("UNITEDSTATES_AR"),"US");
    ageRatingComboBox->addItem(tr("FRANCE_AR"),"FR");
    ageRatingComboBox->addItem(tr("HONKONG_AR","HK"));
    ageRatingComboBox->addItem(tr("ITALY_AR"),"IT");
    ageRatingComboBox->addItem(tr("JAPAN_AR"),"JP");
    ageRatingComboBox->addItem(tr("SWITZERLAND_AR"),"CH");
    ageRatingComboBox->addItem(tr("SOUTHKOREA_AR","KR"));
    ageRatingComboBox->addItem(tr("REPUBLICOFCHINATAIWAN_AR"),"TW");
    ageRatingComboBox->addItem(tr("UNITEDKINGDOM_AR","UK"));
    ageRatingComboBox->model()->sort(0);

    languageLabel = new QLabel(this);
    languageLabel->setText(tr("CHOOSE_ALTERNATE_LANGUAGE_OPTIONAL"));
    languageLabel->setStyleSheet("font-weight: bold; margin-top: 10px;");

    languageComboBox = new QComboBox(this);
    languageComboBox->clear();
    languageComboBox->addItem("--- " + tr("CHOOSE_ENTRY") + " ---");
    languageComboBox->addItem(tr("LANG_DE"));
    languageComboBox->addItem(tr("LANG_EN"));
    languageComboBox->addItem(tr("LANG_IT"));
    languageComboBox->addItem(tr("LANG_FR"));
    languageComboBox->addItem(tr("LANG_NL"));
    languageComboBox->addItem(tr("LANG_DK"));
    languageComboBox->addItem(tr("LANG_PL"));
    languageComboBox->addItem(tr("LANG_JA"));
    languageComboBox->addItem(tr("LANG_KO"));
    languageComboBox->addItem(tr("LANG_ZH"));
    languageComboBox->model()->sort(0);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("DONE"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("CANCEL"));

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
