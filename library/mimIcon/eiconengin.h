#ifndef EICONENGIN_H
#define EICONENGIN_H
#include <QIconEngine>
#include <QIconEngineV2>
//#include <QList>
class EIconEngin : public QIconEngine
{
public:

    EIconEngin(const QString& iconName,const QString& themName =QString());
private:

    QString m_iconName;
    QIcon m_icon;
    bool isfallback;
    QString m_themName;
  //  QList<QSize>m_availableSizes;

    void paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state);
    QPixmap pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state);
    void virtual_hook(int id, void *data);

    void iconFromThemePath(const QString &theme);
    void addIconFile(const QString &curPath);
    void ensureLoaded();


};

#endif // EICONENGIN_H
