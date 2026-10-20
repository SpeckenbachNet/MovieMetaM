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

    // Overrides the draw function to control the look
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    // Overrides the size to tell the view how big the cell must be
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    QString m_styleSheet;
};
#endif // TAGDELEGATE_H
