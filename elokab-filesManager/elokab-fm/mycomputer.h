#ifndef MYCOMPUTER_H
#define MYCOMPUTER_H
#include <RazorMount>
#include <QObject>
#include <QHash>
#include <QTreeWidgetItem>

/**
 * @brief The MyComputer class
 */
class MyComputer : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief MyComputer
     * @param parent
     */
    explicit MyComputer(QObject *parent = 0);
    /**
     * @brief listItemes
     */
    QHash<RazorMountDevice *,QTreeWidgetItem*> listItemes;
    /**
     * @brief mManager
     */
    RazorMountManager *mManager;
signals:
    /**
     * @brief slotAddItemDisk
     */
    void slotAddItemDisk(QTreeWidgetItem*);
    /**
     * @brief slotRemoveItemDisk
     */
    void slotRemoveItemDisk(QTreeWidgetItem*);

     //!
     void updateDeviceItem(QTreeWidgetItem*);

     void  deviceMounted(QTreeWidgetItem*,const QString &);
public slots:
    /**
     * @brief isMountedDevice
     * @param item
     * @return
     */
    bool isMountedDevice(QTreeWidgetItem *item);
    /**
     * @brief chargeDevices
     */
    void chargeDevices();
    /**
     * @brief mountDevices
     * @param item
     * @return
     */
    bool mountDevices(QTreeWidgetItem *item);
    /**
     * @brief unMountDevice
     * @param item
     * @return
     */
    bool unMountDevice(QTreeWidgetItem *item);
    /**
     * @brief getDevicePath
     * @param item
     * @return
     */
    QString getDevicePath(QTreeWidgetItem *item);
    /**
     * @brief deviceIcon
     * @param item
     * @return
    */
    QIcon deviceIcon( QTreeWidgetItem *item);
    /**
     * @brief updateDevices
     * @return
     */
    bool updateDevices();
private slots:
    /**
     * @brief addDiskItem
     * @param device
     */
    void  addDiskItem(RazorMountDevice *device);
    /**
     * @brief removeDiskItem
     * @param device
     */
    void removeDiskItem(RazorMountDevice *device);
    /**
     * @brief isUsableDevice
     * @param device
     * @return
     */

    bool isUsableDevice(const RazorMountDevice *device);

    void updatDiskItem(RazorMountDevice *device);

    /**
     * @brief updateDevice
     * @param device
     * @return
     */
    bool updateDevice( RazorMountDevice *device);
    /**
     * @brief getIcon
     * @param device
     * @return
     */
    QIcon getIcon(RazorMountDevice *device);
};

#endif // MYCOMPUTER_H
