#include "myfilesystemmodel.h"
#include "defines.h"
#include <EMimIcon>
#include <QMimeData>
#include <QUrl>
#include <QDebug>
#include <QApplication>
//#include <QTextCodec>
//#include <QSettings>
/*****************************************************************************************************
 *
 *
 *****************************************************************************************************/


MyFileSystemModel::MyFileSystemModel(QObject *parent):
     QFileSystemModel(parent)
{

    mimcach=new QHash<QString,QString>;
    setRootPath("/");
    setNameFilterDisables(false);
    setReadOnly(false);
    setResolveSymlinks(true);

   // Messages::debugMe(0,__LINE__,"MyFileSystemModel",__FUNCTION__,"End");
}

//--------------------------------------------------------------
QVariant MyFileSystemModel::data(const QModelIndex &index, int role) const
{



    if (!index.isValid())  return QVariant();


    if(index.column()==D_COL_NAME && role == Qt::ToolTipRole){
        return fileName(index);
    }// column 0

    //تحميل نوع الملف بالغة النظام
    if(index.column()==D_COL_TYPE && role == Qt::DisplayRole){
       return localeType(fileInfo(index));

    }// column 2

    if( /*index.column()==0 &&*/role == D_MMIM){

        if(mimcach->contains(filePath(index))){

           return mimcach->value(filePath(index));
        }else{
            QString mim=EMimIcon::mimeTyppe(fileInfo(index));
            mimcach->insert(filePath(index),mim);
           // qDebug()<<"mim from EMimIcon"<<mim;
            return mim;
        }

    }

    if(/*index.column()==0 && */role == D_MTYPE){

        return localeType(fileInfo(index));

    }

    if(/*index.column()==0 && */role == D_MSize){

        if(fileInfo(index).isDir())return QVariant();
        return EMimIcon::formatSize(size(index));
    }

    if(index.column()==D_COL_TRASHED && role == Qt::DisplayRole){
       return EMimIcon::trachInfo(filePath(index)).value("date");
    }

    if(index.column()==D_COL_ORIGPATH && role == Qt::DisplayRole){
        QString fp=EMimIcon::trachInfo(filePath(index)).value("path");
       return QFileInfo(fp).path();

    }

    return QFileSystemModel::data(index,role);


}// MyFileSystemModel::data

QString MyFileSystemModel::localeType(const QFileInfo &info) const
{
            QString mim;
            if(mimcach->contains(info.filePath()))
                mim=mimcach->value(info.filePath());
            else
                mim=EMimIcon::mimeTyppe(info);


           QString mimLang=EMimIcon::mimLang(mim);
         //  qDebug()<<"localeType"<<info.filePath()<<mim<<mimLang;
             if(mimLang.isEmpty())
                return type(index(info.filePath()));

               return mimLang;
}

//--------------------------------------------------------------
QVariant MyFileSystemModel::headerData(int section,
                                       Qt::Orientation orientation,
                                       int role) const
{

    if(orientation==Qt::Horizontal)
        if(role == Qt::DisplayRole)
            switch(section)
            {
            case D_COL_NAME:     return tr("Name");
            case D_COL_SIZE:     return tr("Size");
            case D_COL_TYPE:     return tr("Type");
            case D_COL_DATE:     return tr("Date Modified");
            case D_COL_TRASHED:  return tr("Trashed On");
            case D_COL_ORIGPATH: return tr("Original Location");
            default: return QVariant();
            }

    return QVariant();

}

int MyFileSystemModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED ( parent )

        return 6;


}


//--------------------------------------------------------------
bool MyFileSystemModel::dropMimeData(const QMimeData * data,
                                     Qt::DropAction action,
                                     int row,
                                     int column,
                                     const QModelIndex & parent )
{
    Q_UNUSED ( action )
    Q_UNUSED ( row )
    Q_UNUSED ( column )
//Messages::debugMe(0,__LINE__,"MyFileSystemModel",__FUNCTION__);

    if(isDir(parent))
    {
        QList<QUrl> files = data->urls();
        QStringList filesList;

        if(QFileInfo(files.at(0).path()).canonicalPath() == filePath(parent))
        {
            qDebug()<<"return canonicalPath()"<<QFileInfo(files.at(0).path()).canonicalPath()  ;
            return false;

        }

        foreach(QUrl item, files) filesList.append(item.path());

        Qt::KeyboardModifiers mods = QApplication::keyboardModifiers();

        if(mods == Qt::ControlModifier)
            emit dragDropFiles(true, filePath(parent), filesList);
        else
            emit dragDropFiles(false, filePath(parent), filesList);

        //  qDebug()<<data<< filePath(parent)<<filesList;

    }

//    Messages::debugMe(0,__LINE__,"MyFileSystemModel",__FUNCTION__,"End");

    return false;
}

void MyFileSystemModel::clearCache(const QString &path)
{
    QDirIterator it(path ,QDir::Files
                    |QDir::NoDotAndDotDot
                    |QDir::Hidden, QDirIterator::NoIteratorFlags);

    while (it.hasNext()) {
        QString file=it.next();

        if(mimcach->contains(path)) mimcach->remove(file);
    }
}


