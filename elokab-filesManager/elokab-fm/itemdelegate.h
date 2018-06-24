#ifndef ITEMDELEGATE_H
#define ITEMDELEGATE_H
#include <QAbstractItemDelegate>
#include <QStyledItemDelegate>
//#include <QFutureWatcher>
#include <QFileInfo>


class ItemDelegate : public QStyledItemDelegate
{
  Q_OBJECT
public:
    explicit ItemDelegate();

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    //    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const ;

private:
    QSize mMargins;
    QIcon mSymlinkIcon;
    QSize mItemSize;
    QMap<QString ,QIcon>  *imageCache ;
    QMap<QString ,QIcon>  *iconCache ;
    QMap<QString ,QIcon>  *folderCache;
    QMap<QString ,QIcon>  *deskCache;

    bool mThumbnail;
    static QIcon::Mode iconModeFromState(QStyle::State state);
    QIcon decoration( QString filePath) const;
  //  QByteArray iconThambnail(const QString &file)const;

public slots:
      void setTumbnail(bool thumb){mThumbnail=thumb;}
};

#endif // ITEMDELEGATE_H
