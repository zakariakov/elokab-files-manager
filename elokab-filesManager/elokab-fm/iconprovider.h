#ifndef ICONPROVIDER_H
#define ICONPROVIDER_H

#include <QFileInfo>
#include  <QFileIconProvider>
#include  <QHash>

/**
 * @brief The IconProvider class
 */
class IconProvider:public QFileIconProvider
{
public:
    /**
     * @brief IconProvider
     */
    IconProvider();
    /**
     * @brief The IconType enum
     */
    enum IconType {Exec,Computer, Desktop, Trashcan, Network, Drive, Folder, File,Image,Music,Text ,Video};
    /**
     * @brief icon
     * @param type
     * @return
     */
    virtual  QIcon icon(IconType type) const;
    /**
     * @brief icon
     * @param info
     * @return
     */
   virtual  QIcon icon(const QFileInfo &info) const ;
    /**
     * @brief type
     * @param info
     * @return
     */
    virtual  QString type(const QFileInfo &info)const ;

   //   QIcon iconF(const QFileInfo &info) const ;

public slots:
    void setShowThambnails(bool arg){mPreview=arg;}
    void setStandardIcon(bool arg){mStandardIcon=arg;}
    void setlocale(const QString &lc);

    void clearCache();
    void saveCacheIcons();
    QIcon iconStandard(const QFileInfo &info) const;

  //  void updateIcons();
  //  void updatePath(const QString &path);

private  slots:
    /**
     * @brief loadMimeTypes
     */
    void loadMimeTypes() const ;
    QIcon iconPixmmap(const QString &file) const;

private:
    /*!< cmment */
    QHash<QString,QString> *mimeSufix;
    /*!< cmment */
    QHash<QString,QString> *mimeLanguage;
    bool mPreview;
    bool mStandardIcon;

    QString mLc;
    QHash<QString,QIcon> *iconCach;
};

#endif // ICONPROVIDER_H
