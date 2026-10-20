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
#include <QTextList>
#include <QToolButton>
#include <QVBoxLayout>

namespace {

constexpr int kIconSize = 22;

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
    addTopic(":icons/add", tr("Movie List"), tr("Add, select, remove"), "movielist");
    addTopic(":icons/app_icon", tr("Movie Info"), tr("Edit, cover, age rating"), "movieinfo");
    addTopic(":icons/search", tr("Movie Search"), tr("Find and apply TMDB data"), "moviesearch");
    addTopic(":icons/save", tr("Saving"), tr("Save, safe saving"), "saving");
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
    m_text->setMarkdown(text);
    spaceOut();
    scaleImages();
    m_text->verticalScrollBar()->setValue(0);
}

// More air than Markdown gives by default: room above the headings, a bit more line spacing,
// a small gap between paragraphs and list items
void HelpPage::spaceOut()
{
    QTextDocument *doc = m_text->document();
    QTextCursor cursor(doc);
    for (QTextBlock block = doc->begin(); block.isValid(); block = block.next()) {
        QTextBlockFormat format = block.blockFormat();
        const bool first = block == doc->begin();
        if (format.headingLevel() > 0) {
            format.setTopMargin(first ? 0 : 22);
            format.setBottomMargin(6);
        } else {
            format.setBottomMargin(block.textList() ? 4 : 8);
        }
        format.setLineHeight(130, QTextBlockFormat::ProportionalHeight);
        cursor.setPosition(block.position());
        cursor.setBlockFormat(format);
    }
}

// SVG pictures (TMDB logo) drawn in the resolution of the screen and shown kImageHeight high;
// the document would load them in their own size, too big and blurred
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
    }
    QWidget::changeEvent(event);
}
