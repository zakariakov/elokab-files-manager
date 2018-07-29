/***************************************************************************
 *   elokab Copyright (C) 2014 AbouZakaria <yahiaui@gmail.com>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef UDISKDEVICE_H
#define UDISKDEVICE_H

#include <QObject>
#include <QProcess>
#include <QHash>

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
    QString mId;

     int mType;

    bool mIsValid;
    bool mIsExternal;
    bool mIsMounted;
    bool mIsEjectable;
};

class UdiskDevice : public QObject
{
    Q_OBJECT
public:
    explicit UdiskDevice(QObject *parent = nullptr);
    ~UdiskDevice();
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
