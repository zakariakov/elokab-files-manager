#ifndef TRASH_H
#define TRASH_H

#include <QObject>

class Trash : public QObject
{
    Q_OBJECT
public:
    explicit Trash(QObject *parent = nullptr);

    //!
    void moveFilesToTrash(const QStringList &list);
    //!
    void deleteFiles(const QStringList &list);
    //!
    void restorFiles(const QStringList &list);


signals:

public slots:

    //!
    void cleanTrash();

private:

    //!
    void moveFileToTrash(const QString &file);

    //!
  //  QMap<QString,QString> readDesktopFile(const QString &fileName);


    QString TrashPathInfo;

    QString TrashPathFiles;
};

#endif // TRASH_H
