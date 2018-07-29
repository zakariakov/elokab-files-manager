#include "settingsdlg.h"
#include "ui_settingsdlg.h"
#include <QCheckBox>
#include <EDir>
#include <QLineEdit>
#include <QDebug>
#include <QImageReader>
#include <QMessageBox>
#include <QTextCodec>
SettingsDlg::SettingsDlg(Settings *settings, QWidget *parent) :
    QDialog(parent),mSettings(settings),
    ui(new Ui::SettingsDlg)
{
    ui->setupUi(this);

    ui->checkBoxSingleClick->setChecked(mSettings.isSingleclick());
    ui->checkBoxConfirmDrag->setChecked(mSettings.isConfirmDragDrop());
    ui->checkBoxExpandable->setChecked(mSettings.isRootDecorated());
    ui->checkBoxClassicIcon->setChecked(mSettings.isClassicIcons());
    ui->lineEditTerminal->setText(mSettings.terminal());
    ui->checkBoxPdfThumb->setChecked(mSettings.pdfThumbnails());
    ui->checkBoxVideoThumb->setChecked(mSettings.videoThumbnails());
    ui->checkBoxRename->setChecked(mSettings.doubleClickEdit());

    //ui->checkBoxPdfThumb->setEnabled(EMimIcon::findProgram("convert"));
    ////canReadPdf=EMimIcon::findProgram("pdfimages");
    //ui->checkBoxVideoThumb->setEnabled(EMimIcon::findProgram("ffmpeg"));

}

SettingsDlg::~SettingsDlg()
{
    delete ui;
}

bool    SettingsDlg::isSingleclick()     {return ui->checkBoxSingleClick->isChecked();}
bool    SettingsDlg::isConfirmDragDrop() {return ui->checkBoxConfirmDrag->isChecked();}
bool    SettingsDlg::isRootDecorated()   {return ui->checkBoxExpandable->isChecked();}
bool    SettingsDlg::isClassicIcons()    {return ui->checkBoxClassicIcon->isChecked();}
bool    SettingsDlg::pdfThumbNails()     {return ui->checkBoxPdfThumb->isChecked();}
bool    SettingsDlg::videoThumbNails()   {return ui->checkBoxVideoThumb->isChecked();}
bool    SettingsDlg::doubleClickRemame() {return ui->checkBoxRename->isChecked();}
QString SettingsDlg::terminal()          {return ui->lineEditTerminal->text();}

void SettingsDlg::on_buttonBox_accepted()
{
    //    mSettings.setSingleclick(ui->checkBoxSingleClick->isChecked());
    //    mSettings.setConfirmDragDrop(ui->checkBoxConfirmDrag->isChecked());
    //    mSettings.setRootIsDecorated(ui->checkBoxExpandable->isChecked());
    //    mSettings.setClassicIcons(ui->checkBoxClassicIcon->isChecked());
    //    mSettings.setTerminal(ui->lineEditTerminal->text());

}

void SettingsDlg::on_pushButtonClean_clicked()
{
    QMessageBox msgBox;
    msgBox.setText(tr("Do you want to Clean thumbnails in cache?"));
    msgBox.setInformativeText(tr("This operation remove thumbnail file if original path no exist."));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Yes);
    int ret = msgBox.exec();
    if(ret==QMessageBox::Cancel)return;

    QDir dirCache(Edir::thumbnaileCachDir());
    QStringList list=dirCache.entryList();
    foreach (QString s, list) {
        QString fileThumb=dirCache.absoluteFilePath(s);
        QImageReader reader;
        reader.setFileName(fileThumb);
        QTextCodec *codec = QTextCodec::codecForName("UTF-8");
        QByteArray encodedString =QByteArray::fromHex(reader.text("FILEPATH").toUtf8());
        QString fileName = codec->toUnicode(encodedString);
        // qDebug()<<reader.text("FILEPATH")<<"hex";
        //        QString fileName=reader.text("FILEPATH");
        if(!QFile::exists(fileName)){
            QFile::remove(fileThumb);
            qDebug()<<fileName<<"Exist";
            qDebug()<<fileThumb<<"cleaned";

        }
    }
}

void SettingsDlg::on_pushButtonRmove_clicked()
{
    QMessageBox msgBox;
     msgBox.setText(tr("Do you want to Delete all thumbnails in cache?"));
     msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
     msgBox.setDefaultButton(QMessageBox::Yes);
     int ret = msgBox.exec();
     if(ret==QMessageBox::Cancel)return;

    QDir dirCache(Edir::thumbnaileCachDir());
    QStringList list=dirCache.entryList();
    foreach (QString s, list) {
        QString file=dirCache.absoluteFilePath(s);
        QFile::remove(file);
        qDebug()<<file<<"removed";
    }

}
