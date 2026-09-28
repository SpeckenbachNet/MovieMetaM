#ifndef TAGDELEGATE_H
#define TAGDELEGATE_H
// tagdelegate.h

#include <QString>
#include <QPainter>
#include <QStyledItemDelegate>

class TagDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    TagDelegate(const QString &styleSheet, QObject *parent = nullptr);

    // Überschreibt die Draw-Funktion, um das Aussehen zu steuern
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    // Überschreibt die Größe, um dem View mitzuteilen, wie groß die Zelle sein muss
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    QString m_styleSheet;
};
#endif // TAGDELEGATE_H
