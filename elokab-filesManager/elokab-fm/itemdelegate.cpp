#include "itemdelegate.h"
#include <EMimIcon>
//#include "iconprovider.h"
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

//____________________________________________________________________

ItemDelegate::ItemDelegate()
{
    mMargins=QSize(3,3);
    mSymlinkIcon=QIcon::fromTheme("emblem-symbolic-link");

    imageCache=new QMap<QString ,QIcon>  ;
    iconCache =new QMap<QString ,QIcon>  ;
    folderCache =new QMap<QString ,QIcon>  ;
    deskCache =new QMap<QString ,QIcon>  ;
}

int lineNumber(QString txt,QFont font,int _w)
{
    QFontMetrics fm1(font);
    QTextLayout txtlayout(txt, font);
    int numLines = 0;
    txtlayout.beginLayout();
    int height=0;
    for(;;) {
        QTextLine txtline = txtlayout.createLine();
        if(!txtline.isValid()) {
            break;
        }
        if(numLines>=3) {
            break;
        }
        txtline.setLineWidth(_w);
        height +=fm1.leading();
        txtline.setPosition(QPointF(0, height));

        height += txtline.height();

        if(txtline.naturalTextWidth()>0) {
            ++ numLines;
        }

    }
    txtlayout.endLayout();

    return height;

}



//______________________________________________________________________________
QIcon ItemDelegate::decoration(QString filePath)const
{
    QFileInfo info(filePath);
    //return QIcon();
    if(!QFile::exists(filePath))
        return QIcon();

    if( info.fileName()=="." || info.fileName()==".." )
        return QIcon();


    // ---------------iconCach---------------------
    //    if(iconCache->contains(filePath))
    //        return   iconCache->value(filePath);

    if(info.isSymLink()){

        if(QFile::exists(info.symLinkTarget())){
            filePath=info.symLinkTarget();
            info.setFile(filePath);
        }else{
            return  EIcon::fromTheme("application-octet-stream","unknon");

        }

    }

    QIcon retIcon;

    if(info.isDir())
    {
        retIcon=  EMimIcon::iconFolder(filePath);
        //        if(!retIcon.isNull())
        //            iconCache->insert(filePath,retIcon);
        return retIcon;

    }
    //---------------------------------------
    QString mim= EMimIcon::mimeTyppe(info);
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

        QPixmap pix;
        pix.loadFromData(EMimIcon::iconThambnail(filePath));
        retIcon=QIcon(pix);
        if(!retIcon.isNull()){
            imageCache->insert(filePath,retIcon);
            return retIcon;

        }
    }

    if(iconCache->contains(mim))
        return iconCache->value(mim);

    retIcon=   EMimIcon::iconByMimType(mim,filePath);

    if(!retIcon.isNull()){
        iconCache->insert(mim,retIcon);
        return retIcon;
    }

    // ---------------iconCach---------------------

    return QIcon::fromTheme("unknon");

}

//______________________________________________________________________________
void ItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(!index.isValid())
        return;

   // QStyleOptionViewItem opt = option;

    QSize itemSize=QSize(option.rect.width(),option.rect.height());

    //اذا لم لم يكن العنصر يحمل ايقونة
    //سيتم حمل الاعدادات الافتراضية
    if(index.column()!=0){

        QStyledItemDelegate::paint(painter,option,index);
        return;

        /*
                if((opt.state & QStyle::State_Selected) ) {
                    painter->fillRect(option.rect, opt.palette.highlight());
                }
        QTextOption textOption;
        if(qApp->isRightToLeft()) {
            textOption.setTextDirection(Qt::RightToLeft);
        }
        else {
            textOption.setTextDirection(Qt::LeftToRight);
        }
        painter->setPen(opt.palette.color( QPalette::Text));
        painter->drawText(option.rect,index.data(Qt::EditRole).toString(),textOption);

        return;
        */
    }

    // المتغيرات--------------------------------------
    QTextOption textOption;
    QString txt=index.data(Qt::EditRole).toString();
    QFontMetrics fm1( option.font);
    QString filePath=index.data(Qt::UserRole+1).toString();
    QFileInfo fn(filePath);
    QIcon ico;
    ico=decoration(filePath);
    if(ico.isNull())
        ico=EIcon::fromTheme("application-octet-stream","unknon");

    QPixmap pixmap = ico.pixmap(option.decorationSize,iconModeFromState(option.state));

    //منظور الايقونات (ICON list view)---------------------
    if(option.decorationPosition == QStyleOptionViewItem::Top ||
            option.decorationPosition == QStyleOptionViewItem::Bottom) {

        //  painter->save();
        painter->setClipRect(option.rect);

//        option.decorationAlignment = Qt::AlignHCenter | Qt::AlignTop;
//        option.displayAlignment = Qt::AlignTop | Qt::AlignHCenter;

        // draw the icon
        QPoint iconPos(option.rect.x() + (option.rect.width() - option.decorationSize.width()) / 2, option.rect.y() + mMargins.height());
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
                        option.rect.y() + mMargins.height() + option.decorationSize.height(),
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

        textRect.adjust(2,2,-4,-4);

        //انشاء تقطيع النص عندما يكون اكبر من مربع النص
        QTextLayout txtLayout(txt, option.font);
        txtLayout.setTextOption(textOption);
        int numLines=0;
        txtLayout.beginLayout();
        //QString elidedText;
        int height=0;
        for(;;) {
            QTextLine txtLine = txtLayout.createLine();
            if(!txtLine.isValid()) {
                break;
            }

            if(numLines>=3) {
                break;
            }

            txtLine.setLineWidth(textRect.width()-2);
            height +=fm1.leading();
            txtLine.setPosition(QPointF(0, height));


            if(txtLine.naturalTextWidth()>textRect.width()-2){
                // QTextLine lastLine = txtLayout.lineAt(numLines );
                textOption.setAlignment(Qt::AlignJustify);
                QString  lastText;//=txt.mid(lastLine.textStart()); ;
                lastText=fm1.elidedText(txt,Qt::ElideRight,textRect.width());
                //    QPointF pos(textRect.x(),textRect.top()+height);
                //QRect rrect=QRect(textRect.x(),textRect.top()+height,textRect.width(),textRect.height());
                QRectF rrect=textRect;
                rrect.adjust(2,height,0,0);
               // qDebug()<<rrect<<textRect<<txt;
                painter->drawText(rrect,lastText,textOption);
                //break;
            }else{
                txtLine.draw(painter, textRect.topLeft());
            }

            height += txtLine.height();
            numLines++;
        }
        txtLayout.endLayout();
        //**********************************************
        //  painter->drawText(textRect,txt,textOption);

    }
    // منظر تفصيلي (tree view)-----------------------------------------------
    else {

        //رسم التحديد على كامل المربع
        if((option.state & QStyle::State_Selected) ) {
            painter->fillRect(option.rect, option.palette.highlight());
        }

        QPoint iconPos;
        QRect rect;

        if(qApp->isRightToLeft()){
            iconPos=QPoint(option.rect.x() + (option.rect.width() - option.decorationSize.width()-mMargins.width()),
                           option.rect.y() +(option.rect.height()-option.decorationSize.height())/2);

            rect=QRect(option.rect.x(),
                       option.rect.y(),
                       option.rect.width()-option.decorationSize.width()-(mMargins.width()*2),
                       option.rect.height());

            //      textOption.setTextDirection(Qt::RightToLeft);
            textOption.setAlignment(Qt::AlignRight | Qt::AlignVCenter);

        }else{
            iconPos=QPoint(option.rect.x() + mMargins.width() ,
                           option.rect.y() +(option.rect.height()-option.decorationSize.height())/2);

            rect=QRect(option.rect.x()+ option.decorationSize.width()+(mMargins.width()*2),
                       option.rect.y(),
                       option.rect.width()-option.decorationSize.width()-(mMargins.width()*2),
                       option.rect.height());
            //textOption.setTextDirection(Qt::LeftToRight);
            textOption.setAlignment(Qt::AlignLeft| Qt::AlignVCenter);
        }

        //    rect.adjust(2, 2, -2, -2);

        //رسم النقاط الثلاث اذا كان النص اكبر من المربع
        if(fm1.width(txt)>=rect.width())
            txt=fm1.elidedText(txt,Qt::ElideRight,rect.width());

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

        //
        painter->drawText(rect,txt,textOption);



    }
}

//______________________________________________________________________________
QSize ItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{

    // QSize s= QAbstractItemDelegate::sizeHint(option,index);
    if(!index.isValid())
        return  QSize();

    if(option.decorationPosition == QStyleOptionViewItem::Top ||
            option.decorationPosition == QStyleOptionViewItem::Bottom) {

        //  qDebug()<< option.decorationSize.width()<<option.decorationSize.height();
        // QFontMetrics fm1( option.font);

        QString txt=index.data(Qt::EditRole).toString();

        //طول الحاوي للنص
        int _w=option.decorationSize.width()+(60);


        int line=lineNumber(txt,option.font,_w-4);
        //       qDebug()<<"line" <<line<<_w<<ln<<txt;

        int _h=option.decorationSize.height()+(mMargins.height()*3)+line/*(fh*line)*/;
        return  QSize(_w,_h);


    }else{

        return  QStyledItemDelegate::sizeHint(option,index);
    }

}

//______________________________________________________________________________
QIcon::Mode ItemDelegate::iconModeFromState(const QStyle::State state) {

//    if(state & QStyle::State_Enabled) {
//        return (state & QStyle::State_Selected) ? QIcon::Selected : QIcon::Normal;
//    }
    if(state & QStyle::State_Enabled) {
        if((state & QStyle::State_Selected) )
            return  QIcon::Active;


        if((state & QStyle::State_MouseOver) )
            return  QIcon::Normal;

        return QIcon::Selected;
    }





    return QIcon::Disabled;
}
