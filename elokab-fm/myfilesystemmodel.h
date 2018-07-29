#ifndef MYFILESYSTEMMODEL_H
#define MYFILESYSTEMMODEL_H

#include <QFileSystemModel>
#include <QHash>

class MyFileSystemModel : public QFileSystemModel
{
    Q_OBJECT
public:

    explicit MyFileSystemModel(QObject *parent = nullptr);

     //! مسح مسار الحالي في المخبأ
    void clearCache(const QString &path);


signals:
     //! اشارة بان الملف تم سحبه وافلاته
    void dragDropFiles(bool copy,QString path, QStringList list);

protected:

     //! بيانات العناصر في الاعمدة
    QVariant data(const QModelIndex &index, int role) const ;

     //! ارجاع اسماء الاعمدة
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

     //! عند عملية السحب والافلات
    bool dropMimeData(const QMimeData * data,
                      Qt::DropAction action,
                      int row,
                      int column,
                      const QModelIndex & parent );

    //! ارجاع عدد الاعمدة
    int columnCount(const QModelIndex &parent = QModelIndex()) const ;

public slots:

private:

      //! مخبأة انواع الملفات
    QHash<QString, QString> *mimcach;

      //! جلب نوع الملف بلغة المظام
    QString localeType(const QFileInfo &info)const;



};


#endif // MYFILESYSTEMMODEL_H
