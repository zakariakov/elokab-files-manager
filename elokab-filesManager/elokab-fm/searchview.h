#ifndef SEARCHVIEW_H
#define SEARCHVIEW_H

#include <QWidget>
#include <QDir>
#include <QTimer>
#include <QTreeWidget>
#include <QFileInfo>
#include <QFileSystemWatcher>
#include <QCompleter>

namespace Ui {
class SearchView;
}
/**
 * @brief The SearchView class
 */
class SearchView : public QWidget
{
          Q_OBJECT
          
     public:

          //!
          explicit SearchView(QWidget *parent = 0);

          //!
          ~SearchView();

          //!
          void closeEvent(QCloseEvent *);

          //!
          QTreeWidget *searchTreeView();
     public slots:

          //! تلقي اسم الدليل الذي سستتم فيه عملية البحث
          void setPath(const QString &path);

          //! توقيف لبحث
          void stopSearch();

          //! ارجاع العنصر المحدد
          QString  selectedFile();

          //! ارجاع العناصر المحددة
          QStringList selectedFiles();

          //! عدد العناصر المحددة
          int selectedFilesCount();

          //! تحرير العنصر المحدد
          void editCurentItem();
     signals:

          //!
          void searchingCanceled(const QString &);

          //!
          void fileSelected(const QString &);

          //!
          void  setUrl(const QString &);

          //!
          void  showOpenwithDlg(const QString &);

     private slots:

          //!
          void cancelSearch();

          //!
          void updateAimation();

          //!
          void goSearch(const QString &str);

          //!
          void searchAllSubdir(const QString &path,const QString &text);

          //!
          void searchCurdir(const QString &path, const QString &text);

          //!
          QStringList findFiles(const QStringList &files, const QString &text);

          //!
          void  addFiles(const QStringList &files);

          //!
          void fileChanged(QString file);

          //!
          void lanchApp(QTreeWidgetItem* item,int);

          //!
          void on_treeWidget_currentItemChanged(QTreeWidgetItem */*current*/, QTreeWidgetItem *previous);

          //!
          void on_treeWidget_itemSelectionChanged();


     private:

          //! معرفة هل العنصر في حالة تغيير الاسم
          bool hasEdetor;

          //!
          Ui::SearchView *ui;

          //!  المجلد الحالي الذي سيتم البحث فيه
          QString   m_path;

          //! المجلد الرئيسي للبحث
          QDir  mCurrentDir;

          //! توقيف البحث
          bool mStop;

          //! مؤقت لعملية الحركة في صورةالتقدم
          QTimer *mTimer;

          //! عدد الملفات الناتجة
          int mNfiles;

          //! موضع صورة التقدم الحالي
          int mCurY;

          //! نص العنصر قبل تغيير اسمه
          QString mCurrentEditable;

          //! مراقب الملفات
          QFileSystemWatcher *mSysWatcher;

};

#endif // SEARCHVIEW_H
