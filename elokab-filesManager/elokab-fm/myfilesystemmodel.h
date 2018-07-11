#ifndef MYFILESYSTEMMODEL_H
#define MYFILESYSTEMMODEL_H
#include "iconprovider.h"
#include <QFileSystemModel>
#include <QHash>

class MyFileSystemModel : public QFileSystemModel
{
    Q_OBJECT
public:

    explicit MyFileSystemModel(IconProvider *iconProvider,QObject *parent = 0);

    void setPreview(bool preview){mPreview=preview;}
    void clearCache(const QString &path);


    signals:
     //void imageLoaded(QFileInfo minfo);
     void dragDropFiles(bool copy,QString path, QStringList list);
     //void iconUpdate(const QModelIndex index)const;

    protected:

       QVariant data(const QModelIndex &index, int role) const ;

        QVariant headerData(int section, Qt::Orientation orientation, int role) const;

     //   int columnCount(const QModelIndex &parent) const;
        bool dropMimeData(const QMimeData * data,
                          Qt::DropAction action,
                          int row,
                          int column,
                          const QModelIndex & parent );

    public slots:

    //    void updateIcons(const QString &file){
    //        if(index(file).isValid())
    //          iconUpdate(index(file));
    //    }
    private:

        IconProvider *mIconProvider;
        bool mPreview;
    //  QHash<QString,QModelIndex> *hashIndex;
       //QHash< QModelIndex,Thumbnail* > *mThumbnails;
         QHash<QString,QString>*mimcach;
       //   QHash<QString,QIcon>*iconmimCach;
       //      QHash<QString,QIcon>*iconDesktopCach;
       //   QHash<QString,QByteArray> *hashImages;


};


#endif // MYFILESYSTEMMODEL_H
