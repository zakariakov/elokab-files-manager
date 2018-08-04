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
#include <QDir>
#include <QImageWriter>
#include <QImageReader>
#include <QtConcurrent>
#include <QMessageAuthenticationCode>

//____________________________________________________________________
ItemDelegate::ItemDelegate(bool modern):isTreeview(false),isModernMode(modern)
{

    mSymlinkIcon=EIcon::fromTheme("emblem-symbolic-link");
   // mSymlinkIcon=EIcon::fromTheme("inode-symlink");//application-x-zerosize

    imageCache  =new QMap<QString ,QIcon>  ;

    iconCache   =new QMap<QString ,QIcon>  ;

    deskCache   =new QMap<QString ,QIcon>  ;

    thumbnailCache=Edir::thumbnaileCachDir();

//isTreeview=false;
//isModernMode=true;
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
     int height=D_MARGINS;
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
    return height+D_MARGINS;

}


QIcon ItemDelegate::iconThumbnails(const QString &file, const QString &type) const
{
     QFileInfo fi(file);
    if(fi.path()==thumbnailCache)return QIcon();

    QMessageAuthenticationCode code(QCryptographicHash::Md5);
    code.addData(file.toUtf8());
    QString md5Name=code.result().toHex();
    QString fileThumbnail=thumbnailCache+"/"+md5Name;

    bool hasThumb=false;
    bool hasImage=false;
    QString fileIcon;

    // ---------------------- if Thumbnail file exist -----------------------
    if(QFile::exists(fileThumbnail)){
        QImageReader reader(fileThumbnail);
        if(reader.canRead()){
            QString fModified=reader.text(D_KEY_DATETIME);
            if(fModified== fi.lastModified().toString("dd MM yyyy hh:mm:ss")){
                hasImage=true;
                hasThumb=true;
                fileIcon=fileThumbnail;
            }else{ QFile::remove(fileThumbnail); }

        }else{ QFile::remove(fileThumbnail); }
    }

    // ----------------if no thumbnail file and size < 128--------------------
    if(!hasThumb){
        QImageReader reader(file);
        if(reader.canRead()){
            if(qMax(reader.size().width(),reader.size().height())<=128){
                // hasImage=   image.load((file));
                hasImage=true;
                fileIcon=file;
            }
        }
    }

    //-------------if no thumbnail and image size > 128-------------------
    if(!hasThumb && ! hasImage )  emit requireThumb(fi,type);


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
    QString filePath=index.data(D_MFPATH).toString();
    QFileInfo info(filePath);
    bool isSym=false;

    //---------------------------------------SYMLINK
    if(info.isSymLink()){
        //  qDebug()<<"isSymLink"<<filePath<<info.symLinkTarget();
        if(QFile::exists(info.symLinkTarget())){
            filePath=info.symLinkTarget();
            info.setFile(filePath);
            isSym=true;

        }else{
            return  EIcon::fromTheme("inode-symlink","application-x-zerosize");
        }
    }

    QIcon retIcon;

    //--------------------------------------- DIRECTORY
    if(info.isDir()) { return EMimIcon::iconFolder(filePath); }

    //---------------------------------------
    QString mim;
    if(isSym)  mim= EMimIcon::mimeTyppe(info);
    else  mim= index.data(D_MMIM).toString();

    //---------------------------------------X-DESKTOP
    if( mim=="application/x-desktop"){
        if(deskCache->contains(filePath)) return deskCache->value(filePath);

        retIcon=  EMimIcon::iconDesktopFile(filePath).pixmap(128).scaled(128,128);

        if(!retIcon.isNull())  deskCache->insert(filePath,retIcon);

        return retIcon;
    }

    //---------------------------------------THUMBNAILS
    if(mThumbnail)
    {
        //--------------------------------------- IMAGES TYPE
        if( mim.startsWith(D_IMAGE_TYPE) )
        {
            if(imageCache->contains(filePath)) return imageCache->value(filePath);
            if( QImageReader::supportedMimeTypes().contains(mim.toLatin1()))
               { retIcon=iconThumbnails(filePath,D_IMAGE_TYPE); }
        }// image

        //--------------------------------------- PDF TYPE
        else if( mPdfThumbnail && mim.endsWith(D_PDF_TYPE) )
        {
            if(imageCache->contains(filePath)) return imageCache->value(filePath);
            retIcon=iconThumbnails(filePath,D_PDF_TYPE);
        }// pdf

        //--------------------------------------- VIDEO TYPE
        else if(mVideoThumbnail && mim.startsWith(D_VIDEO_TYPE) )
        {
            if(imageCache->contains(filePath)) return imageCache->value(filePath);
            retIcon=iconThumbnails(filePath,D_VIDEO_TYPE);
        }// video

        //--------------------------------------- CACHE
        if(!retIcon.isNull()){
            imageCache->insert(filePath,retIcon);
            return retIcon;
        }
    }// mThumbnail

    //--------------------------------------- OTHER TYPE
    if(iconCache->contains(mim))
        return iconCache->value(mim);

    retIcon=EMimIcon::iconByMimType(mim,filePath);

    // ---------------iconCach---------------------
    if(!retIcon.isNull()){ iconCache->insert(mim,retIcon); return retIcon; }

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

       //منظور الايقونات (ICON view)-----------------------------------------------
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
    QString filePath=index.data(D_MFPATH).toString();
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
        QPoint iconPos(option.rect.x() + (option.rect.width() - option.decorationSize.width()) / 2, option.rect.y() + D_MARGINS);
        // in case the pixmap is smaller than the requested size
        QSize margin = ((option.decorationSize - pixmap.size()) / 2).expandedTo(QSize(0, 0));


         //----------------  ModernMode  --------------------------
        //draw RoundedRect if isModernMode in item area
        if(isModernMode){
            QRect rectborder=option.rect;
            rectborder.adjust(0,0,-1,-1);
            painter->setOpacity(0.3);
            painter->setPen(option.palette.highlight().color());
            painter->drawRoundedRect(rectborder,qreal(2.0),qreal(2.0));
            if(option.state & QStyle::State_Selected){
               rectborder.adjust(1,1,0,0);
               painter->fillRect(rectborder,option.palette.highlight());
            }
            painter->setOpacity(1.0);
        }
         //------------------------------------------------------------

        //رسم ايقونة مخفية
        if(fn.isHidden())
            painter->setOpacity(0.5);

        painter->drawPixmap(iconPos + QPoint(margin.width(), margin.height()), pixmap);

        //رسم اشارة الى ملف
        if(fn.isSymLink())
            painter->drawPixmap(iconPos, mSymlinkIcon.pixmap(option.decorationSize / 2, iconModeFromState(option.state)));

        //------------------------  رسم النص  --------------------------
        //مربع النص
        QRect textRect(option.rect.x() + (option.rect.width() - itemSize.width()) / 2,
                        option.rect.y() + D_MARGINS + option.decorationSize.height(),
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


        //qDebug()<<"draw"<<txt<<textRect.height();

        //----------------  ModernMode  --------------------------
         if(isModernMode){
             painter->setOpacity(0.1);
             painter->fillRect(textRect, option.palette.highlight());
              painter->setOpacity(1.0);
                textRect.adjust(1,D_MARGINS+fm1.leading(),-2,0);
             txt=fm1.elidedText(txt,Qt::ElideRight,textRect.width());

              painter->drawText(textRect,txt,textOption);
              return;
         }
          painter->setOpacity(1.0);
          //-------------- Classic style --------------------------
         textRect.adjust(0,D_MARGINS+fm1.leading(),0,0);

        int height=fm1.height()+fm1.leading();
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


}


//______________________________________________________________________________
void ItemDelegate::paintDetailView(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{

    // المتغيرات--------------------------------------
    QTextOption textOption;
    QString txt=index.data(Qt::EditRole).toString();

    QString filePath=index.data(D_MFPATH).toString();
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

            iconPos=QPoint(option.rect.x() + (option.rect.width() - option.decorationSize.width()-D_MARGINS),
                           option.rect.y() +(option.rect.height()-pixmap.height())/2);

            rect=QRect(option.rect.x(),
                       option.rect.y(),
                       option.rect.width()-option.decorationSize.width()-(D_MARGINS*2),
                       option.rect.height());

            textOption.setAlignment(Qt::AlignRight | Qt::AlignVCenter);

        }else{

            iconPos=QPoint(option.rect.x() + D_MARGINS ,
                           option.rect.y() +(option.rect.height()-pixmap.height())/2);

            rect=QRect(option.rect.x()+ option.decorationSize.width()+(D_MARGINS*2),
                       option.rect.y(),
                       option.rect.width()-option.decorationSize.width()-(D_MARGINS*2),
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

            QString type=index.data(D_MTYPE).toString();
            QString size=index.data(D_MSize).toString();
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
            // center this text verical
          int top;

          if((h*4)<option.rect.height())
                top= (option.rect.height()-(h*4))/2;
            else
             top=D_MARGINS+fm2.leading();

            rect.moveTop(rect.top()+top+fm2.leading());
            painter->drawText(rect,firstTxt,textOption);

            // رسم الخط الثاني العنوان titre2
            if(txt.length()>firstTxt.length() && (h*4)<option.rect.height()){

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
        else if(size<=128)defW=10;

        //طول الحاوي للنص
        int _w=option.decorationSize.width()+(defW);
        int line;
         if(isModernMode){
             line=option.fontMetrics.height()+option.fontMetrics.leading();
             line+=D_MARGINS*2;
         }else{
              line=lineNumber(txt,option,_w-4);
         }



        int _h=option.decorationSize.height()+(D_MARGINS*2)+line;

        return  QSize(_w,_h);

    }
    else // LIST AND DETAIL VIEW
    {
        if(isTreeview){

            return  QStyledItemDelegate::sizeHint(option,index);

        }else{

            int h=option.decorationSize.height()+(D_MARGINS*2);
            int minH=(option.fontMetrics.height()*3)+(option.fontMetrics.leading()*3)+(D_MARGINS*2);;
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


 }
