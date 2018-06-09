#include "pathwidget.h"
#include "ui_pathwidget.h"
//#include <EIcon>
#include <EMimIcon>
#include <QDebug>
#include <QDir>
#include <QSettings>

/*********************************************************************************
 *
 ********************************************************************************/
PathWidget::PathWidget(QWidget *parent) :
     QWidget(parent),
     ui(new Ui::PathWidget)
{
     ui->setupUi(this);
     QColor color=this->palette().dark().color();
     QColor color2=this->palette().light().color();
     QColor color3=this->palette().shadow().color();
     QString style=QString("QWidget #widgetPath {"
                           "\n      border:1px inset rgba(%7, %8, %9, 150);"
                           " border-radius: 5px;"
                           "background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1,"
                           "stop:0 rgba(%4, %5, %6, 80),"
                           "stop:1 rgba(%1, %2, %3, 80));"
                           "}"
                           )
               .arg(color.red()).arg(color.green()).arg(color.blue())
               .arg(color2.red()).arg(color2.green()).arg(color2.blue())
               .arg(color3.red()).arg(color3.green()).arg(color3.blue());

     setStyleSheet(style);
     // setLayoutDirection(Qt::LeftToRight);
     ui->toolButtonEdit->setIcon(EIcon::fromTheme("document-edit","edit-rename"));
     connect( ui->toolButtonEdit,SIGNAL(toggled(bool)),this,SLOT(toolbarVisible(bool)));

     // ui->lineEdit->setText(url);
     ui->lineEdit->setVisible(false);
     mMenu=new QMenu;
     //    btnRoot->setMenu(mMenu);
     mactRoot=new   QAction(tr("Root"),this);
     connect(mactRoot,SIGNAL(triggered()),this,SLOT(goActionPath()));

     mToolBar=new QToolBar(this);
     ui->horizontalLayout->addWidget(mToolBar);
     mToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
     mToolBar->setIconSize(QSize(16,16));
     //wButtons=new WidgetButtons(ui->widget);

     //wButtons->move(0,1);

     completer = new QCompleter(this);
     completer->setMaxVisibleItems(7);
     completer->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
     fsModel = new QFileSystemModel(completer);
     fsModel->setFilter(QDir::AllDirs|QDir::NoDotAndDotDot);
     fsModel->setRootPath("/");

     //wButtons->show();
     completer->setModel(fsModel);

     ui->lineEdit->setCompleter(completer);
     QSettings settings("elokab", "elokabFm");
     settings.beginGroup("Main");
     ui->toolButtonEdit->setChecked(settings.value("ShowEditLocation",false).toBool());
     settings.endGroup();

}
/*********************************************************************************
 *
 ********************************************************************************/
PathWidget::~PathWidget()
{
    qDebug()<<"PathWidget deleted";
     QSettings settings("elokab", "elokabFm");
     settings.beginGroup("Main");
     settings.setValue("ShowEditLocation",ui->toolButtonEdit->isChecked());
     //   ui->toolButtonEdit->setChecked(settings.value("ShowEditLocation",false).toBool());
     settings.endGroup();
     delete ui;
}

/*********************************************************************************
 *
 ********************************************************************************/
void  PathWidget::resizeEvent(QResizeEvent */*event*/)
{
     // ui->widget->setMinimumHeight(wButtons->height());
     // wButtons->resizeEvent(event);
}

/*********************************************************************************
 *
 ********************************************************************************/
void PathWidget::setUrl(const QString &url)
{

     if(url==":/search")
       return;


     ui->lineEdit->setText(url);

     setupToolUrl(url);
    }

/*********************************************************************************
 *
 ********************************************************************************/
void PathWidget::on_lineEdit_returnPressed()
{
     QDir dir(ui->lineEdit->text());
     if(!dir.exists()){

          qDebug()<<"PathWidget::on_lineEdit_returnPressed()===directory no exist :"+ui->lineEdit->text();
          return;
     }
     emit urlChanged(ui->lineEdit->text());
     setupToolUrl(ui->lineEdit->text());
}

void PathWidget::toolbarVisible(bool checked)
{
     if(!checked)
          setupToolUrl(ui->lineEdit->text());
}

/*********************************************************************************
 *
 ********************************************************************************/
void PathWidget::setupToolUrl(const QString &url)
{
     if(ui->toolButtonEdit->isChecked())
          return;

     //     if(!QFile::exists(url))
     //          return;
     QString urlBtn=url;


     foreach (QAction *act, listActions) {
          act->blockSignals(true);
          listActions.removeOne(act);
          delete act;
     }
     listActions.clear();
     foreach (QMenu *menu, listmenus) {
          menu->deleteLater();
     }

     listmenus.clear();
     mToolBar->clear();

     if(urlBtn==":/trash"){
          QAction *act=mToolBar->addAction
                    (EIcon::fromTheme("user-trash","emptytrash"),tr("Trash"));
          QFont font;font.setBold(true);
          act->setFont(font);
          listActions.append(act);
          return;
     }

     QDir dir(urlBtn);
     while (dir.cdUp()) {

          QString text=dir.dirName();
          if(dir==QDir::rootPath())
               text=tr("Root");

          if(text==":"||text.isEmpty())
               continue;


          QAction *act=new QAction(text,this);
          connect(act,SIGNAL(triggered()),this,SLOT(goActionPath()));

          act->setData(dir.absolutePath());

          act->setMenu(menuDirs(dir));
          mToolBar->insertAction(mToolBar->actions().first(),act);


          listActions.append(act);
     }

     dir.setPath(urlBtn);
     QString text=dir.dirName();
     if(dir==QDir::rootPath())
          text=tr("Root");

     QAction *act=mToolBar->addAction(EMimIcon::iconFolder(urlBtn).pixmap(22),text);
     QFont font;font.setBold(true);
     act->setFont(font);
     listActions.append(act);


     //     if(wButtons->width()>ui->widget->width())
     //          wButtons->move(ui->widget->width()-wButtons->width(),2);
     //     else
     //          wButtons->move(0,2);

}

/**************************************************************************************
 *
 **************************************************************************************/
QMenu *PathWidget::menuDirs( QDir dir)
{
     QMenu *menu=new QMenu;
     //QList<QAction *> list;
     foreach (QString subDir, dir.entryList(QDir::AllDirs|  QDir::NoDotAndDotDot))
     {
          QFileInfo fi(dir.absolutePath()+"/"+subDir);

          QAction *act=new QAction(fi.fileName(),this);
          connect(act,SIGNAL(triggered()),this,SLOT(goActionPath()));

          act->setData(fi.filePath());
          menu->addAction(act);

          listActions.append(act);
     }
     listmenus.append(menu);
     //return list;

     return menu;
}

/**************************************************************************************
 *
 **************************************************************************************/
void PathWidget::goActionPath()
{
     QAction *act=qobject_cast<QAction *>(sender());
     if(act){
          QString path=act->data().toString();
          if(path.isEmpty()||path.isNull())
               path=QDir::rootPath();
          emit urlChanged(path);
    //    setUrl(path);
     }

}
