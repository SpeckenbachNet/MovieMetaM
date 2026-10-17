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
    ageRatingComboBox->addItem("--- " + tr("Choose an entry") + " ---");
    ageRatingComboBox->addItem(tr("Germany (FSK)"),"DE");
    ageRatingComboBox->addItem(tr("Netherlands (Kijkwijzer)"),"NL");
    ageRatingComboBox->addItem(tr("Austria (JMÜ)"),"AT");
    ageRatingComboBox->addItem(tr("United States (MPA)"),"US");
    ageRatingComboBox->addItem(tr("France (CNC)"),"FR");
    ageRatingComboBox->addItem(tr("Hong Kong (TELA)","HK"));
    ageRatingComboBox->addItem(tr("Italy (MiC)"),"IT");
    ageRatingComboBox->addItem(tr("Japan (Eirin)"),"JP");
    ageRatingComboBox->addItem(tr("Switzerland (FSK)"),"CH");
    ageRatingComboBox->addItem(tr("South Korea (KMRB)","KR"));
    ageRatingComboBox->addItem(tr("Taiwan"),"TW");
    ageRatingComboBox->addItem(tr("United Kingdom (BBFC)","UK"));
    ageRatingComboBox->model()->sort(0);

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
