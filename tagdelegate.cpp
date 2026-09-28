// tagdelegate.cpp

#include "tagdelegate.h"
#include <QPainter>
#include <QStyleOptionViewItem>

// **1. Konstruktor-Implementierung**
TagDelegate::TagDelegate(const QString &styleSheet, QObject *parent)
    : QStyledItemDelegate(parent),
    m_styleSheet(styleSheet)
{
    // Hier können Sie das Styling für die spätere Verwendung speichern
}

// **2. paint() Implementierung**
void TagDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // *** Wichtig: Momentan wird nur der Standard-Text gezeichnet ***
    // Hier müsste die eigentliche Logik zum Zeichnen des Hintergrunds und der abgerundeten Ecken
    // mit QPainter hinzukommen.

    // Fürs Erste rufen wir die Basisklasse auf, um den Text zu zeichnen.
    QStyledItemDelegate::paint(painter, option, index);
}

// **3. sizeHint() Implementierung**
QSize TagDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // *** Wichtig: Hier muss die korrekte Größe des Tags berechnet werden ***

    // Wir verwenden die Standardgröße der Basisklasse, die meistens zu klein ist,
    // aber den Linker-Fehler behebt. Wir müssen das später anpassen.
    QSize baseSize = QStyledItemDelegate::sizeHint(option, index);

    // Fügen Sie etwas Polsterung hinzu, um den Text nicht abzuschneiden
    return QSize(baseSize.width() + 10, baseSize.height() + 4);
}
