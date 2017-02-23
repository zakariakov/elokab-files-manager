#ifndef EICONENGIN_H
#define EICONENGIN_H
#include <QtGui/QIconEngine>

class QIconLoaderEngineEntry
 {
public:
    virtual ~QIconLoaderEngineEntry() {}
    virtual QPixmap pixmap(const QSize &size,
                           QIcon::Mode mode,
                           QIcon::State state) = 0;
    QString filename;
//    QIconDirInfo dir;
    static int count;
};

class EIconEngin : public QIconEngineV2
{
public:
    EIconEngin(const QString& iconName );

    void paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state);
    QPixmap pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state);
public slots:

private:
    QString m_iconName;

    QIcon m_icon;
    QIcon m_iconFallback;
    QString themName;
    bool isfallback;
    void virtual_hook(int id, void *data);
    void ensureLoaded();
    QString themBackPath();
    void restoreIconThemeBack();
};

#endif // EICONENGIN_H
