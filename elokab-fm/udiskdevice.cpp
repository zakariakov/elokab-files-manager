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

#include "udiskdevice.h"
#include <QDebug>
#define UDISK_MOUNT "udisksctl mount -b %1"
#define UDISK_UNMOUNT "udisksctl unmount -b %1"
#define UDISK_INFO "udisksctl info -b %1"
#define UDISK_DUMP "udisksctl dump"
#define UDISK_MONITOR "udisksctl monitor"
#define UDISK_BLOCK "/org/freedesktop/UDisks2/block_devices/"
//______________________________________EMIMICON________________________________________________
QString formatSize(qint64 num)
{
     QString total;
     const qint64 kb = 1024;
     const qint64 mb = 1024 * kb;
     const qint64 gb = 1024 * mb;
     const qint64 tb = 1024 * gb;

     if (num >= tb) total = QString(QObject::tr("%1 TB")).arg(QString::number(qreal(num) / tb, 'f', 2));
     else if(num >= gb) total = QString(QObject::tr("%1 GB")).arg(QString::number(qreal(num) / gb, 'f', 2));
     else if(num >= mb) total = QString(QObject::tr("%1 MB")).arg(QString::number(qreal(num) / mb, 'f', 1));
     else if(num >= kb) total = QString(QObject::tr("%1 KB")).arg(QString::number(qreal(num) / kb,'f', 1));
     else total = QString(QObject::tr("%1 bytes")).arg(num);

     return total;
}

UdiskDevice::UdiskDevice(QObject *parent) : QObject(parent)
{
    mProcess=new QProcess;
    connect(mProcess,SIGNAL(readyReadStandardOutput()),this,SLOT(deviceStatuChanged()));
    mProcess->start(UDISK_MONITOR);
}
UdiskDevice::~UdiskDevice()
{
delete mProcess;
}

void UdiskDevice::deviceStatuChanged()
{

    qDebug()<<"deviceStatuChanged";
    QString result=mProcess->readAll();

    if(result.contains("Properties Changed")){
         UpdateDevicesProperties(result);

    }else{
        UpdateDevices();
    }

}
void UdiskDevice::UpdateDevicesProperties(const QString &result)
{

    QStringList list=result.split("\n");
    QString MountPoints;
    foreach (QString s, list) {
       if(s.trimmed().startsWith("MountPoints"))
           MountPoints=s.trimmed().section(":",-1).trimmed();
    }
    QHashIterator<QString, Device*> i(mDevicesHash);
    while (i.hasNext()) {
        i.next();
       Device *device=i.value();

       if(result.contains(device->id())){

            device->setMountPath(MountPoints.trimmed());
           qDebug()<<i.key()<<"=============MountPoints==========="<<MountPoints;

       }
    }
}

bool UdiskDevice::UpdateDevices()
{

    QProcess p;
    p.start(UDISK_DUMP);
    if (!p.waitForStarted())
        return false;
    if (!p.waitForFinished())
        return false;

    QString result = p.readAll();

    QStringList list=result.split("\n");

    //Variable info
    QString IdDrive,IdLabel,IdType,DevicePath,MountPoints,Size;
    bool Mountable=false;


    QStringList templistDev;

    foreach (QString s, list) {


        if(s.trimmed().startsWith(UDISK_BLOCK))IdDrive=s.trimmed().remove(":");

        if(s.trimmed().startsWith("IdLabel:")) IdLabel=s.section(":",-1).trimmed();

        if(s.trimmed().startsWith("IdType:")) IdType=s.section(":",-1).trimmed();

        if(s.trimmed().startsWith("Device:"))DevicePath=s.section(":",-1).trimmed();

        if(s.trimmed().startsWith("MountPoints:")){ Mountable=true; MountPoints=s.section(":",-1).trimmed();}

        if(s.trimmed().startsWith("Size:")) Size=s.section(":",-1).trimmed();




        if(s.trimmed().isEmpty()){


            if(Mountable && MountPoints!="/" && !mDevicesHash.contains(DevicePath)){
              // qDebug()<<DevicePath<<"added"<<IdDrive;

                if(IdLabel.isEmpty()){
                    qint64 n=Size.toLong();
                    IdLabel=formatSize(n);
                }

                Device  *mDevice =new Device;
                mDevice->setId(IdDrive);
                int mtype=mediaType(IdDrive.toLower());
                mDevice->setMediaType(mtype);
                mDevice->setIconName(iconName(mtype));

                mDevice->setDevPath(DevicePath);
                mDevice->setLabel(IdLabel);
                mDevice->setMountPath(MountPoints);
                mDevice->setFileSystem(IdType);
              //  mListDevice.append(mDevice);
                mDevicesHash[DevicePath]=mDevice;
           //     mListdev<<DevicePath;
                templistDev<<DevicePath;
                emit deviceAdded(mDevice);

            }else if(Mountable && MountPoints!="/"){
                 templistDev<<DevicePath;
            }

            //clear variable
            IdLabel.clear();IdType.clear();DevicePath.clear();
            Size.clear();MountPoints.clear();
            Mountable=false;

        }

    }


    QHashIterator<QString, Device*> i(mDevicesHash);
    while (i.hasNext()) {
        i.next();

       QString s=i.key();
        if (!templistDev.contains(i.key())){
             qDebug()<<s<<"removed";
         //   mListdev.removeOne(s);
            Device *D= mDevicesHash.value(s);
            mDevicesHash.take(s);
//            mListDevice.removeOne(D);
            emit deviceRemoved(s);
            delete D;

        }
    }

    return true;
}



bool UdiskDevice::unmount(const QString &device)
{
    QString arg=QString(UDISK_UNMOUNT).arg(device);
    QProcess p;

    p.start(arg);
    if (!p.waitForStarted())
        return false;
    if (!p.waitForFinished())
        return false;

    QString result = p.readAll();
   qDebug()<<result;


    Device *D=mDevicesHash.value(device);
    D->setMountPath(QString());

    return true;
}
bool UdiskDevice::mount(const QString &device)
{

    QString arg=QString(UDISK_MOUNT).arg(device);
    QProcess p;
    p.start(arg);
    if (!p.waitForStarted())
        return false;
    if (!p.waitForFinished())
        return false;

    QString result = p.readAll();
    qDebug()<<result;

      Device *D=mDevicesHash.value(device);
      D->setMountPath(getMountPoint(device));
    return true;

}

bool UdiskDevice::isMounted(const QString &device)
{

   Device *D=mDevicesHash.value(device);
   return D->isMounted();
}

QString UdiskDevice::mountPoint(const QString &device)
{
    Device *D=mDevicesHash.value(device);
     return D->mountPath();
}

QString UdiskDevice::getMountPoint(const QString &device)
{

    QString arg=QString(UDISK_INFO).arg(device);
    QString path;
    QProcess p;

    p.start(arg);
    if (!p.waitForStarted())
        return path;
    if (!p.waitForFinished())
        return path;

    QString result = p.readAll();

    QStringList list=result.split("\n");

    foreach (QString s, list) {
        if(s.trimmed().startsWith("MountPoints:")){
            QString  MountPoints=s.section(":",-1).trimmed();
            path=MountPoints;
            return path;
        }
    }
    return path;

}

int UdiskDevice::mediaType(const QString &drive)
{
    if(drive.contains("usb"))
        return Device::MediaTypeUSB;
    if(drive.contains("dvd")
            ||drive.contains("cdrom")
            ||drive.contains("optical"))
        return Device::MediaTypeCDROM;
    else
        return  Device::MediaTypeDRIVE;

 //   return  Device::MediaTypeUnknown;
}

QString UdiskDevice::iconName(int type)
{
    switch (type) {
    case Device::MediaTypeUSB:
         return "drive-removable-media-usb";
       // break;
    case Device::MediaTypeCDROM:
        return "media-optical-dvd";
      //  break;
    case Device::MediaTypeDRIVE:
        return "drive-harddisk";
      //  break;
    case  Device::MediaTypeUnknown:
         return "drive-harddisk";
      //  break;
    default:
        break;
    }
    return "drive-harddisk";
}

