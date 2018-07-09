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
#include "defines.h"
#include "itemdelegate.h"
#include <EMimIcon>
#include <QAbstractItemDelegate>
#include <QStyledItemDelegate>
#include <QDebug>
#include <QPainter>
#include <QTextLayout>
#include <QApplication>
#include <QFileInfo>
#include <QDir>
#include <QStandardPaths>
#include <QBuffer>
#include <QImageWriter>
#include <QImageReader>
#include <QtConcurrent>
#include <QMessageAuthenticationCode>

//____________________________________________________________________
ItemDelegate::ItemDelegate()
{

    mSymlinkIcon=EIcon::fromTheme("emblem-symbolic-link");
   // mSymlinkIcon=EIcon::fromTheme("inode-symlink");//application-x-zerosize

    imageCache  =new QMap<QString ,QIcon>  ;

    iconCache   =new QMap<QString ,QIcon>  ;
    folderCache =new QMap<QString ,QIcon>  ;
    deskCache   =new QMap<QString ,QIcon>  ;
    listThumb =new QStringList;
    thumbnail=Edir::thumbnaileCachDir();

isTreeview=false;

}


int lineNumber(QString txt,const QStyleOptionViewItem &option,int _w)
{
    /*
    //! عدد الاسطر في نص الايقونة
  //  QFontMetrics fm1(font);
    QTextLayout txtlayout(txt, option.font);
    int numLines = 0;
    int height=0;

    txtlayout.beginLayout();

    for(;;) {
        QTextLine txtline = txtlayout.createLine();
        if(!txtline.isValid()) {
            break;
        }
        if(numLines>=3) {
            break;
        }
        txtline.setLineWidth(_w);

        txtline.setPosition(QPointF(0, height));
        height +=option.fontMetrics.leading();
        height += option.fontMetrics.height();//txtline.height();

        //if(txtline.naturalTextWidth()>0) {
            ++ numLines;
        //}

    }
    txtlayout.endLayout();
    */
//qDebug()<<txt<<numLines;
     int height=_MARGINS;
    QString firstTxt=txt;
    for (int i = 0; i < 3; ++i) {
        if(firstTxt.isEmpty())break;

        QString curentTxt=firstTxt;
        curentTxt=option.fontMetrics.elidedText(curentTxt,Qt::ElideRight,_w);

       height+=option.fontMetrics.height()+option.fontMetrics.leading();;
        //rrect.setTop(rrect.top()+height);;
        firstTxt=firstTxt.mid(curentTxt.length());

    }
   //  qDebug()<<txt<<height;
    return height+_MARGINS;

}

//TODO RMOVE THIS
void ItemDelegate::saveImageThumb(const QFileInfo &fi,const QString &fileThumbnail) const
{
//qDebug()<<"creatthumb"<<fi.filePath();

    QImage image;
   if( image.load(fi.filePath()))
   {

    image.setText("DATETIME",fi.lastModified().toString("dd MM yyyy hh:mm:ss"));
    image= image.scaled(QSize(128,128),Qt::KeepAspectRatio,Qt::SmoothTransformation);
    image.setText("DATETIME",fi.lastModified().toString("dd MM yyyy hh:mm:ss"));
    QByteArray format="jpg";
    if(image.hasAlphaChannel())
        format="png";

    if(image.save(fileThumbnail,format,50))   {
//        QSettings setting("elokab","thumbnails");
//        setting.setValue(fi.filePath(),QFileInfo(fileThumbnail).fileName());
        qApp->processEvents();
        //TODO RMOVE THIS imageHasThumb
        emit imageHasThumb(fi.filePath());
        listThumb->removeAll(fi.filePath());
    }

   }
}

QIcon ItemDelegate::iconThumbnails(const QString &file) const
{

    QFileInfo fi(file);



        QMessageAuthenticationCode code(QCryptographicHash::Md5);
        code.addData(file.toLatin1());

  //  qDebug()<<code.result().toHex();
    QString fileThumbnail=thumbnail+"/"+code.result().toHex();
   // QString fileThumbnail=thumbnail+"/"+file.toHex();
   //    if(fileThumbnail.length()>255)
  //      fileThumbnail=  fileThumbnail.left(255);


    QImage image;
    bool hasThumb=false;
    bool hasImage=false;
    QString fileIcon;
    // if this Thumbnail file exist and canread
    if(QFile::exists(fileThumbnail)){

        QImageReader reader(fileThumbnail);
        if(reader.canRead()){
            QString fModified=reader.text("DATETIME");
            if(fModified== fi.lastModified().toString("dd MM yyyy hh:mm:ss")){
                // hasImage=  image.load(fileThumbnail);
                hasImage=true;
                hasThumb=true;
                fileIcon=fileThumbnail;
            }
        }
    }

    // write this Thumbnail file if canread this orig file
    if(!hasThumb){
        QImageReader reader(file);
        if(reader.canRead()){
            if(qMax(reader.size().width(),reader.size().height())<=128){
               // hasImage=   image.load((file));
                hasImage=true;
                fileIcon=file;
            }else{
               // if(fi.absolutePath()!=thumbnail){
                    if(!listThumb->contains(fi.filePath())) {
                       // listThumb->append(fi.filePath());

//                        QFuture<void> f1 =QtConcurrent::run(this, &ItemDelegate::saveImageThumb,fi,fileThumbnail);
//                              f1.waitForFinished();

                        //QtConcurrent::run(this, &ItemDelegate::saveImageThumb,fi,fileThumbnail);
                    }
               // }
            }
        }
    }


    if(hasImage && !fileIcon.isEmpty()){
        QIcon icon;
        icon.addFile(fileIcon,QSize(128,128));
        return icon;
    }

    return QIcon();
}

//______________________________________________________________________________
QIcon ItemDelegate::decoration(const QModelIndex &index)const
{
    QString filePath=index.data(_MFPATH).toString();
    QFileInfo info(filePath);
    bool isSym=false;
//    if(!QFile::exists(filePath))
//        return QIcon();

//    if( info.fileName()=="." || info.fileName()==".." )
//        return QIcon();

    if(info.isSymLink()){
     //  qDebug()<<"isSymLink"<<filePath<<info.symLinkTarget();
        if(QFile::exists(info.symLinkTarget())){
            filePath=info.symLinkTarget();
            info.setFile(filePath);
            isSym=true;

        }else{
          //  qDebug()<<"symLinkTarget"<<filePath;
            return  EIcon::fromTheme("inode-symlink","application-x-zerosize");

        }

    }

    QIcon retIcon;

    if(info.isDir())
    {
        return EMimIcon::iconFolder(filePath);

    }
    //---------------------------------------
    QString mim;
    if(isSym)
        mim= EMimIcon::mimeTyppe(info);
    else
        mim= index.data(_MMIM).toString();

    //---------------------------------------

    QString suf=info.suffix().toLower();
    if(suf=="desktop" || mim=="application-x-desktop"){
        if(deskCache->contains(filePath))
            return deskCache->value(filePath);

        retIcon=  EMimIcon::iconDesktopFile(filePath).pixmap(128).scaled(128,128);
        if(!retIcon.isNull())
            deskCache->insert(filePath,retIcon);

        return retIcon;
    }


    if(mThumbnail && mim.startsWith("image"))
    {

        if(imageCache->contains(filePath))
            return imageCache->value(filePath);

        if( QImageReader::supportedMimeTypes().contains(mim.toLatin1())){
            retIcon=iconThumbnails(filePath);
            if(!retIcon.isNull()){
                imageCache->insert(filePath,retIcon);
                return retIcon;
            }
        }
    }

    if(iconCache->contains(mim))
        return iconCache->value(mim);

    retIcon=EMimIcon::iconByMimType(mim,filePath);

    // ---------------iconCach---------------------
    if(!retIcon.isNull()){
        iconCache->insert(mim,retIcon);
        return retIcon;
    }

    return QIcon::fromTheme("unknon");

}

//______________________________________________________________________________
void ItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(!index.isValid())
        return;

   // QStyleOptionViewItem opt = option;

       //اذا لم لم يكن العنصر يحمل ايقونة
    //سيتم حمل الاعدادات الافتراضية
    if(index.column()!=0){

        QStyledItemDelegate::paint(painter,option,index);
        return;

     }

       //منظور الايقونات (ICON view)---------------------
    if(option.decorationPosition == QStyleOptionViewItem::Top ||
            option.decorationPosition == QStyleOptionViewItem::Bottom) {

       paintIconView(painter,option,index);
    }
    //! منظر تفصيلي (Detail View)-----------------------------------------------
    else{
        paintDetailView(painter,option,index);
    }

}

//______________________________________________________________________________
void ItemDelegate::paintIconView(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{

    QSize itemSize=QSize(option.rect.width(),option.rect.height());


    // المتغيرات--------------------------------------
    QTextOption textOption;
    QString txt=index.data(Qt::EditRole).toString();
    QFontMetrics fm1=option.fontMetrics;
    QString filePath=index.data(_MFPATH).toString();
    QFileInfo fn(filePath);
    QIcon ico;
    ico=decoration(index);
    if(ico.isNull())
        ico=EIcon::fromTheme("application-octet-stream","unknon");

    QPixmap pixmap = ico.pixmap(option.decorationSize,iconModeFromState(option.state));


        //  painter->save();
        painter->setClipRect(option.rect);

        //رسم التحديد
//        if((option.state & QStyle::State_Selected) )
//            painter->fillRect(option.rect, option.palette.highlight());

        // draw the icon
        QPoint iconPos(option.rect.x() + (option.rect.width() - option.decorationSize.width()) / 2, option.rect.y() + _MARGINS);
        // in case the pixmap is smaller than the requested size
        QSize margin = ((option.decorationSize - pixmap.size()) / 2).expandedTo(QSize(0, 0));

        ///رسم الايقونة والنص كاملا عند التحدبد
        //draw selected item area
        //        if((opt.state & QStyle::State_Selected) ) {
        //            painter->fillRect(opt.rect, opt.palette.highlight());
        //        }

        //رسم ايقونة مخفية
        if(fn.isHidden())
            painter->setOpacity(0.5);

        painter->drawPixmap(iconPos + QPoint(margin.width(), margin.height()), pixmap);

        //رسم اشارة الى ملف
        if(fn.isSymLink())
            painter->drawPixmap(iconPos, mSymlinkIcon.pixmap(option.decorationSize / 2, iconModeFromState(option.state)));

        //------------------------  رسم النص  --------------------------
        //مربع النص
        QRectF textRect(option.rect.x() + (option.rect.width() - itemSize.width()) / 2,
                        option.rect.y() + _MARGINS + option.decorationSize.height(),
                        itemSize.width(),
                        itemSize.height() - option.decorationSize.height());



        textOption.setAlignment(Qt::AlignHCenter);

        //رسم التحديد على مربع النص
        if((option.state & QStyle::State_Selected) )
            painter->fillRect(textRect, option.palette.highlight());

        //لون النص عند التحديد او الوضع العادي
        if(option.state & QStyle::State_Selected)
            painter->setPen(option.palette.color( QPalette::HighlightedText));
        else
            painter->setPen(option.palette.color( QPalette::Text));

        painter->setOpacity(1.0);
        //qDebug()<<"draw"<<txt<<textRect.height();
        textRect.adjust(0,fm1.leading(),0,0);

        int height=fm1.height()+fm1.leading();
        /*  //انشاء تقطيع النص عندما يكون اكبر من مربع النص
        QTextLayout txtLayout(txt, option.font);
        txtLayout.setTextOption(textOption);
        //int numLines=0;
        txtLayout.beginLayout();
       int height=0;

        for(int i=0;i<3;++i) {
            QTextLine txtLine = txtLayout.createLine();
            if(!txtLine.isValid()) {
                break;
            }

//            if(numLines>=3) {
//                break;
//            }

            txtLine.setLineWidth(textRect.width()-2);
            height +=fm1.leading();
            txtLine.setPosition(QPointF(0, height));

            //اذا كان النص اكبر من الحاوي
            if(txtLine.naturalTextWidth()>textRect.width()-2){

                textOption.setAlignment(Qt::AlignJustify);
                QTextLine lastLine;
                if(i>0)
                 lastLine = txtLayout.lineAt(-i );
                else
                 lastLine = txtLayout.lineAt(0 );
qDebug()<<txt<<i<<lastLine.textStart();
                QString  lastText=txt.mid(lastLine.textStart()); ;

                lastText=fm1.elidedText(lastText,Qt::ElideRight,textRect.width());
                QRectF rrect=textRect;
                rrect.adjust(2,height,0,0);

                painter->drawText(rrect,lastText,textOption);
                //break;
            }else{
                txtLine.draw(painter, textRect.topLeft());
            }

            height += txtLine.height();
           // numLines++;
        }
         txtLayout.endLayout();
        */
        // QRectF rrect=textRect;
        QString firstTxt=txt;
        for (int i = 0; i < 3; ++i) {
            if(firstTxt.isEmpty())break;
            QString curentTxt=firstTxt;
            curentTxt=fm1.elidedText(curentTxt,Qt::ElideRight,textRect.width());

            if(curentTxt.endsWith("…") && i<2) curentTxt.remove("…");

            painter->drawText(textRect,curentTxt,textOption);
            textRect.adjust(0,height,0,0);
            //rrect.setTop(rrect.top()+height);;
            firstTxt=firstTxt.mid(curentTxt.length());

        }

        //**********************************************
        //  painter->drawText(textRect,txt,textOption);

}

//______________________________________________________________________________
void ItemDelegate::paintDetailView(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{

    // المتغيرات--------------------------------------
    QTextOption textOption;
    QString txt=index.data(Qt::EditRole).toString();

    QString filePath=index.data(_MFPATH).toString();
    QFileInfo fn(filePath);
    QIcon ico;
    ico=decoration(index);
    if(ico.isNull())
        ico=EIcon::fromTheme("application-octet-stream","unknon");

    QPixmap pixmap = ico.pixmap(option.decorationSize,iconModeFromState(option.state));

    // المتغيرات--------------------------------------

        //رسم التحديد على كامل المربع
        if((option.state & QStyle::State_Selected) ) {
            painter->fillRect(option.rect, option.palette.highlight());
        }

        QPoint iconPos; QRect rect;

        if(qApp->isRightToLeft()){

            iconPos=QPoint(option.rect.x() + (option.rect.width() - option.decorationSize.width()-_MARGINS),
                           option.rect.y() +(option.rect.height()-pixmap.height())/2);

            rect=QRect(option.rect.x(),
                       option.rect.y(),
                       option.rect.width()-option.decorationSize.width()-(_MARGINS*2),
                       option.rect.height());

            textOption.setAlignment(Qt::AlignRight | Qt::AlignVCenter);

        }else{

            iconPos=QPoint(option.rect.x() + _MARGINS ,
                           option.rect.y() +(option.rect.height()-pixmap.height())/2);

            rect=QRect(option.rect.x()+ option.decorationSize.width()+(_MARGINS*2),
                       option.rect.y(),
                       option.rect.width()-option.decorationSize.width()-(_MARGINS*2),
                       option.rect.height());

            textOption.setAlignment(Qt::AlignLeft| Qt::AlignVCenter);
        }


        //اذا كان الملف مخفي
        if(fn.isHidden())
            painter->setOpacity(0.3);

        //رسم الايقونة
        painter->drawPixmap(iconPos , pixmap);

        //رسم الايقونة المخفية
        if(fn.isSymLink())
            painter->drawPixmap(iconPos, mSymlinkIcon.pixmap(option.decorationSize / 2, iconModeFromState(option.state)));

        //
        painter->setOpacity(1.0);

        //لون الخط في حالة التحديد او عدمه
        if(option.state & QStyle::State_Selected)
            painter->setPen(option.palette.color( QPalette::HighlightedText));
        else
            painter->setPen(option.palette.color( QPalette::Text));


        if(isTreeview){
            //رسم النقاط الثلاث اذا كان النص اكبر من المربع
            QFontMetrics fm1=option.fontMetrics;
            txt=fm1.elidedText(txt,Qt::ElideRight,rect.width());
            painter->drawText(rect,txt,textOption);

        }else{

            QString type=index.data(_MTYPE).toString();
            QString size=index.data(_MSize).toString();
            painter->save();
            QFont font(option.font);
            font.setBold(true);
            painter->setFont(font);
            QFontMetrics fm2(font);
            int h=fm2.height()+fm2.leading();
            rect.setHeight(h);

            // رسم الخط الاول العنوان titre1
            QString firstTxt=fm2.elidedText(txt,Qt::ElideRight,rect.width()/*,Qt::TextWordWrap|Qt::TextWrapAnywhere*/);
            if(firstTxt.endsWith("…")) firstTxt.remove("…");
            rect.moveTop(rect.top()+_MARGINS+fm2.leading());
            painter->drawText(rect,firstTxt,textOption);

            // رسم الخط الثاني العنوان titre2
            if(txt.length()>firstTxt.length() && option.decorationSize.width() > 48 ){

                QString lastTxt=txt.mid(firstTxt.length());
                lastTxt=fm2.elidedText(lastTxt,Qt::ElideRight,rect.width());
                rect.moveTop(rect.top()+h);
                painter->drawText(rect,lastTxt,textOption);

            }

            // رسم الخط الثالث النوع type
            painter->restore();
            font.setBold(false);
            font.setItalic(true);
            painter->setFont(font);
            rect.moveTop(rect.top()+h);
            painter->drawText(rect,type,textOption);

            // رسم الخط الرابع الحجم size
            rect.moveTop(rect.top()+h);
            painter->drawText(rect,size,textOption);
        }


}

//______________________________________________________________________________
QSize ItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{

    // QSize s= QAbstractItemDelegate::sizeHint(option,index);
    if(!index.isValid()) return  QSize();

    //ICON VIEW MODE
    if(option.decorationPosition == QStyleOptionViewItem::Top ||
       option.decorationPosition == QStyleOptionViewItem::Bottom)
    {

        QString txt=index.data(Qt::EditRole).toString();

        int defW=70;
        int size=option.decorationSize.width();

             if(size<=24)defW=70;
        else if(size<=32)defW=60;
        else if(size<=48)defW=56;
        else if(size<=64)defW=40;
        else if(size<=72)defW=20;
        else if(size<=96)defW=10;
        else if(size<=128)defW=0;

        //طول الحاوي للنص
        int _w=option.decorationSize.width()+(defW);

        int line=lineNumber(txt,option,_w-4);

        int _h=option.decorationSize.height()+(_MARGINS*2)+line;

        return  QSize(_w,_h);

    }
    else // LIST AND DETAIL VIEW
    {
        if(isTreeview){

            return  QStyledItemDelegate::sizeHint(option,index);

        }else{

            int h=option.decorationSize.height()+(_MARGINS*2);
            int minH=(option.fontMetrics.height()*3)+(option.fontMetrics.leading()*3);
            if(h<minH) h=minH;
            // qDebug()<<__LINE__<<__FILE__<<__FUNCTION__;
            return  QSize(option.decorationSize.width()+200,h);

        }
    }

}

//______________________________________________________________________________
QIcon::Mode ItemDelegate::iconModeFromState(const QStyle::State state)
{

    if(state & QStyle::State_Enabled) {
        if((state & QStyle::State_Selected) )
            return  QIcon::Selected;

        if((state & QStyle::State_MouseOver) )
            return  QIcon::Selected;

        return QIcon::Normal;
    }

    return QIcon::Disabled;

}

//______________________________________________________________________________
 void ItemDelegate::clearCurentPath(const QString &path)
 {

     QDir dir(path);
     QDirIterator it(path ,QDir::Files|QDir::Hidden, QDirIterator::NoIteratorFlags);

     while (it.hasNext()) {
         QString file= it.next();
         clearItemCache(file);
     }

 }

 //______________________________________________________________________________
 void ItemDelegate::clearItemCache(const QString &file)
 {

     qDebug()<<"clearItemCache"<<file;
     imageCache->remove(file) ;
     iconCache ->remove(file)  ;
    // folderCache->remove(file)   ;
     deskCache->remove(file)  ;
     listThumb->removeAll(file);

 }
