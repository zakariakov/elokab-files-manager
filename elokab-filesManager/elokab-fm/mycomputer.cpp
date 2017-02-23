#include "mycomputer.h"
#include "messages.h"
#ifdef DEBUG_APP
#include <QDebug>
#endif
#include <QPainter>

/*****************************************************************************************************
 *
 *****************************************************************************************************/

MyComputer::MyComputer(QObject *parent) :
     QObject(parent)
{

#ifdef DEBUG_APP
     Messages::showMessage(Messages::TOP,"MyComputer::MyComputer()");
#endif

}
/**************************************************************************************
 *                                  DEVICE
 **************************************************************************************/
void MyComputer::chargeDevices()
{
#ifdef DEBUG_APP
     Messages::showMessage(Messages::BEGIN,"MyComputer::chargeDevices()");
#endif

     mManager=new RazorMountManager;
     connect(mManager, SIGNAL(deviceAdded(RazorMountDevice*)),
             this, SLOT(addDiskItem(RazorMountDevice*)));
     connect(mManager, SIGNAL(deviceRemoved(RazorMountDevice*)),
             this, SLOT(removeDiskItem(RazorMountDevice*)));
     connect(mManager, SIGNAL(deviceChanged(RazorMountDevice*)),
             this, SLOT(updatDiskItem(RazorMountDevice*)));


     foreach(RazorMountDevice *device, mManager->devices())
     {
          addDiskItem(device);
     }
     updateDevices();

#ifdef DEBUG_APP
     Messages::showMessage(Messages::END,"MyComputer::chargeDevices()");
#endif
}
/**************************************************************************************
 *                                  DEVICE
 **************************************************************************************/
void MyComputer::addDiskItem(RazorMountDevice *device)
{
#ifdef DEBUG_APP
     Messages::showMessage(Messages::BEGIN,"MyComputer::addDiskItem()");
#endif

     if (isUsableDevice(device))
     {

          QTreeWidgetItem *item=new QTreeWidgetItem;
          item->setText(0,device->label());
          item->setIcon(0,getIcon(device));
          item->setData(0,Qt::ToolTipRole,device->mountPath());

          connect(device, SIGNAL(changed()),  this, SLOT(updateDevices()));

          listItemes[device]=item;
          emit slotAddItemDisk(item);
     }

#ifdef DEBUG_APP
     Messages::showMessage(Messages::END,"MyComputer::addDiskItem()");
#endif

}
/**************************************************************************************
 *                                  DEVICE
 **************************************************************************************/
void MyComputer::updatDiskItem(RazorMountDevice *device)
{
    if (isUsableDevice(device))
    {
      QTreeWidgetItem *item=listItemes.value(device);
//         emit slotRemoveItemDisk(item);
if(device->isMounted())
    emit deviceMounted(item,device->mountPath());
//         listItemes.remove(device);
qDebug()<<"divice changed"<<device->isMounted()<<device->mountPath();
    }
}

void MyComputer::removeDiskItem(RazorMountDevice *device)
{
#ifdef DEBUG_APP
     Messages::showMessage(Messages::BEGIN,"MyComputer::removeDiskItem()");
#endif

     if (isUsableDevice(device))
     {
          QTreeWidgetItem *item=listItemes.value(device);
          emit slotRemoveItemDisk(item);

          listItemes.remove(device);

     }

#ifdef DEBUG_APP
     Messages::showMessage(Messages::END,"MyComputer::removeDiskItem()");
#endif

}

/**************************************************************************************
 *                                  DEVICE
 **************************************************************************************/
bool MyComputer::updateDevices()
{
#ifdef DEBUG_APP
     Messages::showMessage(Messages::BEGIN,"MyComputer::updateDevices()");
#endif

     foreach (QTreeWidgetItem *item, listItemes)
     {
          RazorMountDevice *device=listItemes.key(item);
          item->setText(0,device->label());
          item->setIcon(0,getIcon(device));
          item->setData(0,Qt::ToolTipRole,device->mountPath());

     }

#ifdef DEBUG_APP
     Messages::showMessage(Messages::END,"MyComputer::updateDevices()");
#endif
     return true;

}
/**************************************************************************************
 *                                  DEVICE
 **************************************************************************************/
bool MyComputer::updateDevice( RazorMountDevice *device)
{
#ifdef DEBUG_APP
     Messages::showMessage(Messages::BEGIN,"MyComputer::updateDevice()");
#endif

     QTreeWidgetItem *item=listItemes.value(device);
     item->setText(0,device->label());
     item->setIcon(0,getIcon(device));
     item->setData(0,Qt::ToolTipRole,device->mountPath());

     emit updateDeviceItem(item);

#ifdef DEBUG_APP
     Messages::showMessage(Messages::NORMALE,"MyComputer::updateDevice()","device->label :"+device->label());
     Messages::showMessage(Messages::NORMALE,"MyComputer::updateDevice()","device->mountPath :"+device->mountPath());
     Messages::showMessage(Messages::END,"MyComputer::updateDevice()");
#endif

     return true;

}
/**************************************************************************************
  *                                  DEVICE
  **************************************************************************************/

QIcon MyComputer::getIcon( RazorMountDevice *device)
{
 //   qDebug()<<"getIcon( RazorMountDevice *device)"<<device->iconName();
     if(!device->isMounted())
          return QIcon::fromTheme( device->iconName(),
                                   QIcon( ":/icons/drive-harddisk" ) );

     QPixmap pix=QIcon::fromTheme( device->iconName(),
                                   QIcon( ":/icons/drive-harddisk" ) ).pixmap(22);
     QPixmap pixreject=QIcon::fromTheme("media-eject",
                                        QIcon( ":/icons/media-eject" ) ).pixmap(16);

     QPainter p;
     p.begin(&pix);
     p.drawPixmap(QRect(8,8,16,16),pixreject,pixreject.rect());
     p.end();

     return  QIcon(pix);
}
/**************************************************************************************
   *                                  DEVICE
   **************************************************************************************/

QIcon MyComputer::deviceIcon( QTreeWidgetItem *item)
{
     RazorMountDevice *device=  listItemes.key(item);
     if(!device)return QIcon();
     return getIcon( device);
}
bool MyComputer::isMountedDevice(QTreeWidgetItem *item)
{
     RazorMountDevice *device=  listItemes.key(item);
     if(!device)return false;

     if(device->isMounted())
          return true;
     else
          return false;
}

/**************************************************************************************
 *                                  DEVICE
 **************************************************************************************/
bool MyComputer::mountDevices(QTreeWidgetItem *item)
{
     RazorMountDevice *device=  listItemes.key(item);
     if(!device)return false;
     if(device->isMounted())
     {
          return true;
     }else{
          if( device->mount())
          {
               if(updateDevice(device))
                    return true;
          }
     }

     return false;
}
/**************************************************************************************
  *                                  DEVICE
  **************************************************************************************/

bool MyComputer::unMountDevice(QTreeWidgetItem *item)
{
     RazorMountDevice *device=  listItemes.key(item);
     if(!device)return false;
     if(device->unmount())return true;
     return false;

}

/**************************************************************************************
  *                                  DEVICE
  **************************************************************************************/
QString MyComputer::getDevicePath(QTreeWidgetItem *item)
{

     RazorMountDevice *device=  listItemes.key(item);
     if(!device)return "";
     if(updateDevice(device))
          return device->mountPath();

     return "";
}

/**************************************************************************************
 *                                  DEVICE
 **************************************************************************************/
bool MyComputer::isUsableDevice(const RazorMountDevice *device)
{

     switch (device->mediaType())
     {
          case RazorMountDevice::MediaTypeFdd:
               return  true;

          case RazorMountDevice::MediaTypeOptical:
               return true;
               break;

          case RazorMountDevice::MediaTypePartition:

               return true;

          case RazorMountDevice::MediaTypeDrive:
               return device->isExternal();
               break;

          default:
               return false;
     }

     return false;
}
