#ifndef UDISKDEVICE_H
#define UDISKDEVICE_H

#include <QObject>
#include <QProcess>
#include <QHash>
#include "udiskdevice.h"

class Device : public QObject
{
    Q_OBJECT
public:
    enum MediaType {
              MediaTypeUnknown,
              MediaTypeDRIVE,
              MediaTypeUSB,
              MediaTypeCDROM
    };

     int mediaType() const { return mType; }
    QString mountPath() const { return mMountPath; }
    QString iconName() const { return mIconName; }
    QString id() const { return mId; }
    QString label() const { return mLabel; }
     QString devPath() const { return mDevPath; }

void setId(const QString &id){mId=id;}
    void setDevPath(const QString &devicePath){mDevPath=devicePath;}
    void setLabel(const QString &label){mLabel=label;}
    void setFileSystem(const QString &fsystem){mFileSystem=fsystem;}
    void setMountPath(const QString &mountPath){mMountPath=mountPath;}
    void setIconName(const QString &iconName){mIconName=iconName;}
     void setMediaType(int type){mType=type;}

    bool isValid() const { return mIsValid; }
    bool isExternal() const { return mIsExternal; }
    bool isMounted() const { return !mMountPath.isEmpty(); }
    bool isEjectable() const { return mIsEjectable; }

private:
    QString mDevPath;
    QString mLabel;
    QString mFileSystem;
    QString mMountPath;
    QString mIconName;
     int mType;
QString mId;

    bool mIsValid;
    bool mIsExternal;
    bool mIsMounted;
    bool mIsEjectable;
};

class UdiskDevice : public QObject
{
    Q_OBJECT
public:
    explicit UdiskDevice(QObject *parent = 0);

signals:
void deviceAdd(Device *D);

    void devicesChanged();
    void propertiesChanged();
    void devceChanged(const QString &device);
    void deviceMounted(const QString &device);
    void deviceUnMounted(const QString &device);
    void deviceAdded(Device *D);
    void deviceRemoved(const QString &device);

public slots:
    bool UpdateDevices();
    bool isMounted(const QString &device);
    QString mountPoint(const QString &device);
    QString getMountPoint(const QString &device);
    bool unmount(const QString &device);
    bool mount(const QString &device);
    //QList<Device*> listDevice(){return mListDevice;}

private slots:
   void deviceStatuChanged();
   int mediaType(const QString &drive);
   void UpdateDevicesProperties(const QString &result);
      QString iconName(int type);
private:
 //  QList<Device*> mListDevice;
    QHash <QString,Device*> mDevicesHash;
//QStringList mListdev;
    QProcess *mProcess;
    //     QHash<QString, QString> hash;
};

#endif // UDISKDEVICE_H
