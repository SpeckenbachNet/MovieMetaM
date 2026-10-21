/*
 * Copyright (C) 2026 Michael Speckenbach
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
#include "helppage.h"
#include "themecolors.h"
#include "toolbarm.h"
#include "twolinedelegate.h"

#include <QEvent>
#include <QFile>
#include <QHBoxLayout>
#include <QImage>
#include <QListView>
#include <QLocale>
#include <QPainter>
#include <QScrollBar>
#include <QSettings>
#include <QStandardItemModel>
#include <QSvgRenderer>
#include <QTextBlock>
#include <QTextBrowser>
#include <QTextCursor>
#include <QTextDocumentFragment>
#include <QTextList>
#include <QTextTable>
#include <QToolButton>
#include <QVBoxLayout>

namespace {

constexpr int kIconSize = 24;   // the size the icons are drawn in: a stroke of 2 stays 2 px

// Black drawing in the given color, in the resolution of the screen
QPixmap tintedPixmap(const QString &path, const QColor &color, qreal dpr)
{
    const int size = qRound(kIconSize * dpr);
    QPixmap pm(size, size);
    pm.fill(Qt::transparent);
    QPainter p(&pm);
    QSvgRenderer(path).render(&p);
    p.setCompositionMode(QPainter::CompositionMode_SourceIn);
    p.fillRect(pm.rect(), color);
    p.end();
    pm.setDevicePixelRatio(dpr);
    return pm;
}

// Quotes ("> ...") of the Markdown become notes: a box with room inside and a stripe in the accent
// color on the left (QTextDocument draws a quote as a bare background that touches the text, so each
// one becomes a table of two cells). From the end, the positions before stay valid.
void quotesToNotes(QTextDocument *doc, const QColor &fill, const QColor &stripe)
{
    QList<int> starts;
    for (QTextBlock block = doc->begin(); block.isValid(); block = block.next()) {
        if (block.blockFormat().hasProperty(QTextFormat::BlockQuoteLevel)) {
            starts.append(block.position());
        }
    }
    for (int i = starts.size() - 1; i >= 0; --i) {
        QTextCursor cursor(doc);
        cursor.setPosition(starts.at(i));
        cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        const QTextDocumentFragment text = cursor.selection();
        cursor.removeSelectedText();

        QTextBlockFormat empty;   // the block left behind: no quote any more, no room
        empty.setTopMargin(0);
        empty.setBottomMargin(0);
        cursor.setBlockFormat(empty);

        QTextTableFormat table;
        table.setBorder(0);
        table.setCellPadding(0);
        table.setCellSpacing(0);
        table.setWidth(QTextLength(QTextLength::PercentageLength, 100));
        table.setColumnWidthConstraints({QTextLength(QTextLength::FixedLength, 4),
                                         QTextLength(QTextLength::VariableLength, 0)});
        table.setTopMargin(6);
        table.setBottomMargin(6);
        QTextTable *note = cursor.insertTable(1, 2, table);

        QTextTableCellFormat stripeCell;
        stripeCell.setBackground(stripe);
        note->cellAt(0, 0).setFormat(stripeCell);
        QTextTableCellFormat textCell;
        textCell.setBackground(fill);
        textCell.setPadding(10);
        note->cellAt(0, 1).setFormat(textCell);
        note->cellAt(0, 1).firstCursorPosition().insertFragment(text);
    }
}

} // namespace

HelpPage::HelpPage(QWidget *parent)
    : QWidget(parent)
{
    // --- Own toolbar on top, like every page
    m_toolbar = new ToolbarM(this);
    m_toolbar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    QToolButton *backBtn = m_toolbar->addButton("helpBackBtn", tr("Back"), ":icons/back", true);
    m_toolbar->setTitle(tr("Help"));
    connect(backBtn, &QToolButton::clicked, this, &HelpPage::backRequested);

    // --- Topics on the left: a list like the movie list
    m_model = new QStandardItemModel(this);
    m_topics = new QListView(this);
    m_topics->setModel(m_model);
    m_topics->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_topics->setSelectionMode(QAbstractItemView::SingleSelection);
    m_topics->setIconSize(QSize(kIconSize, kIconSize));
    m_topics->setItemDelegate(new TwoLineDelegate(m_topics));
    m_topics->setFixedWidth(kTopicsWidth);
    applyBoxStyle(m_topics, false, FieldCorners::All, "QListView { outline: 0; }");

    // --- Text on the right in a box like the description
    m_text = new QTextBrowser(this);
    m_text->setOpenExternalLinks(true);
    applyBoxStyle(m_text, false);
    m_text->document()->setDocumentMargin(14);
    // Lines not too long to read (narrower windows: see eventFilter, otherwise the text runs
    // out on the right)
    m_text->setLineWrapMode(QTextEdit::FixedPixelWidth);
    m_text->setLineWrapColumnOrWidth(kMaxLineWidth);
    m_text->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_text->viewport()->installEventFilter(this);

    auto *content = new QHBoxLayout;
    content->setSpacing(18);
    content->addWidget(m_topics);
    content->addWidget(m_text, 1);

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(m_toolbar);
    layout->addLayout(content, 1);

    // Order as in enum Topic
    addTopic(":icons/list_movie", tr("Movie List"), tr("Add, select, remove"), "movielist");
    addTopic(":icons/info_movie", tr("Movie Info"), tr("Edit, cover, age rating"), "movieinfo");
    addTopic(":icons/search_movie", tr("Movie Search"), tr("Find and apply TMDB data"), "moviesearch");
    addTopic(":icons/save_movie", tr("Saving"), tr("Save, safe saving"), "saving");
    addTopic(":icons/settings", tr("Settings"), tr("Languages, age rating, saving"), "settings");
    addTopic(":icons/info", tr("About %1").arg(QStringLiteral(APP_NAME)),
             tr("Version, license, credits"), "about");
    // The separator lines depend on the neighbour's selection: repaint the whole list
    connect(m_topics->selectionModel(), &QItemSelectionModel::currentRowChanged, this,
            [this](const QModelIndex &current) {
                loadText(current.row());
                m_topics->viewport()->update();
            });

    restyle();
    showTopic(MovieList);
}

void HelpPage::addTopic(const QString &icon, const QString &title, const QString &subtitle, const QString &file)
{
    m_model->appendRow(new QStandardItem(title + '\n' + subtitle));
    m_icons.append(icon);
    m_files.append(file);
}

void HelpPage::showTopic(Topic topic)
{
    m_topics->setCurrentIndex(m_model->index(topic, 0));
    loadText(topic);   // also if it was the current one: from the top again
}

// Same choice as the translator (main.cpp): the language of the settings, else the one of the
// system; German or English (fallback)
QString HelpPage::language()
{
    QString code = QSettings().value("language").toString();
    if (code.isEmpty()) {
        code = QLocale::system().name().left(2);
    }
    return code == QLatin1String("de") ? QStringLiteral("de") : QStringLiteral("en");
}

void HelpPage::loadText(int row)
{
    if (row < 0 || row >= m_files.size()) {
        return;
    }
    QFile file(QStringLiteral(":/help/%1/%2.md").arg(language(), m_files.at(row)));
    QString text = file.open(QIODevice::ReadOnly) ? QString::fromUtf8(file.readAll()) : QString();
    // Placeholders of the info about the app
    text.replace(QLatin1String("{{version}}"), QStringLiteral(APP_VERSION));
    text.replace(QLatin1String("{{qt}}"), QString::fromLatin1(qVersion()));
    // A line under the title (Markdown: a rule)
    if (text.startsWith(QLatin1String("# "))) {
        const int end = text.indexOf(QLatin1Char('\n'));
        if (end > 0) text.insert(end, QStringLiteral("\n\n---"));
    }
    m_text->setMarkdown(text);
    spaceOut();
    scaleImages();
    m_text->verticalScrollBar()->setValue(0);
}

// More air than Markdown gives by default: room above the headings (in the grey of the headings
// of the app), a bit more line spacing, a small gap between paragraphs and list items; notes
// from the quotes
void HelpPage::spaceOut()
{
    QTextDocument *doc = m_text->document();
    const QPalette pal = palette();
    const QColor headingColor = accentGrey(pal, 0.6);   // more contrast than the default: better to read in the long text
    const FieldLook look;   // the text box has the color of the fields: the notes a shade away from it
    quotesToNotes(doc, mixColor(look.fill, look.text, 0.08), pal.color(QPalette::Highlight));

    QTextCursor cursor(doc);
    for (QTextBlock block = doc->begin(); block.isValid(); block = block.next()) {
        QTextBlockFormat format = block.blockFormat();
        const bool first = block == doc->begin();
        cursor.setPosition(block.position());
        if (format.headingLevel() > 0) {
            format.setTopMargin(first ? 0 : 22);
            format.setBottomMargin(6);
            QTextCharFormat color;
            color.setForeground(headingColor);
            cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
            cursor.mergeCharFormat(color);
        } else if (cursor.currentTable()) {
            format.setTopMargin(0);   // the room of a note is its cell padding
            format.setBottomMargin(0);
        } else {
            format.setBottomMargin(block.textList() ? 4 : 8);
        }
        // A picture in the line: no extra line height (130 % of a screenshot is a wide gap below it)
        bool hasImage = false;
        for (auto it = block.begin(); !it.atEnd(); ++it) {
            hasImage = hasImage || it.fragment().charFormat().isImageFormat();
        }
        format.setLineHeight(hasImage ? 100 : 130, QTextBlockFormat::ProportionalHeight);
        cursor.setPosition(block.position());
        cursor.setBlockFormat(format);
    }
}

// SVG pictures (TMDB logo) drawn in the resolution of the screen and shown kImageHeight high;
// the document would load them in their own size, too big and blurred. Screenshots keep their
// size (half the pixels).
void HelpPage::scaleImages()
{
    QTextDocument *doc = m_text->document();
    const qreal dpr = devicePixelRatioF();

    // First collect, then change (changing the format can merge the fragments)
    QList<QTextFragment> images;
    for (QTextBlock block = doc->begin(); block.isValid(); block = block.next()) {
        for (auto it = block.begin(); !it.atEnd(); ++it) {
            if (it.fragment().charFormat().isImageFormat()) {
                images.append(it.fragment());
            }
        }
    }

    for (const QTextFragment &fragment : images) {
        QTextImageFormat format = fragment.charFormat().toImageFormat();

        // Screenshots (PNG): Retina shots, shown in the size of half the pixels
        if (!format.name().endsWith(QLatin1String(".svg"), Qt::CaseInsensitive)) {
            QImage shot(format.name());
            if (shot.isNull()) {
                continue;
            }
            shot.setDevicePixelRatio(kScreenshotScale);
            doc->addResource(QTextDocument::ImageResource, QUrl(format.name()), shot);
            format.setWidth(shot.width() / qreal(kScreenshotScale));
            format.setHeight(shot.height() / qreal(kScreenshotScale));
            QTextCursor cursor(doc);
            cursor.setPosition(fragment.position());
            cursor.setPosition(fragment.position() + fragment.length(), QTextCursor::KeepAnchor);
            cursor.setCharFormat(format);
            continue;
        }

        QSvgRenderer renderer(format.name());
        if (!renderer.isValid() || renderer.defaultSize().isEmpty()) {
            continue;
        }
        const QSizeF size(kImageHeight * renderer.defaultSize().width()
                              / qreal(renderer.defaultSize().height()), kImageHeight);
        QImage image((size * dpr).toSize(), QImage::Format_ARGB32_Premultiplied);
        image.fill(Qt::transparent);
        QPainter painter(&image);
        renderer.render(&painter);
        painter.end();
        doc->addResource(QTextDocument::ImageResource, QUrl(format.name()), image);

        format.setWidth(size.width());
        format.setHeight(size.height());
        QTextCursor cursor(doc);
        cursor.setPosition(fragment.position());
        cursor.setPosition(fragment.position() + fragment.length(), QTextCursor::KeepAnchor);
        cursor.setCharFormat(format);
    }
}

// Icons in the color of the text; selected entries in the color of the selected text
void HelpPage::restyle()
{
    const qreal dpr = devicePixelRatioF();
    for (int row = 0; row < m_model->rowCount(); ++row) {
        QIcon icon;
        icon.addPixmap(tintedPixmap(m_icons.at(row), palette().color(QPalette::Text), dpr), QIcon::Normal);
        icon.addPixmap(tintedPixmap(m_icons.at(row), palette().color(QPalette::HighlightedText), dpr),
                       QIcon::Selected);
        m_model->item(row)->setIcon(icon);
    }
}

bool HelpPage::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_text->viewport() && event->type() == QEvent::Resize) {
        m_text->setLineWrapColumnOrWidth(qMin(kMaxLineWidth, m_text->viewport()->width()));
    }
    return QWidget::eventFilter(watched, event);
}

void HelpPage::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::PaletteChange) {
        restyle();   // light / dark mode switched
        if (m_text && !m_files.isEmpty()) {
            loadText(m_topics->currentIndex().row());   // colors of headings and notes
        }
    }
    QWidget::changeEvent(event);
}
