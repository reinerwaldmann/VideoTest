#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "opencv/cv.h"
#include <opencv/highgui.h>
#include <stdlib.h>
#include <stdio.h>
#include <QFileDialog>
#include <QErrorMessage>



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->settingsBox->hide();

    loadSettings();

    vdp = new VideoProcessor(this);
    connect(vdp, SIGNAL(showFrame(QImage)), this, SLOT(showFrame(QImage)));
    connect(vdp, SIGNAL(emitCurrentPosition(int)), this, SLOT(getCurrentPosition(int)));
    connect(vdp, SIGNAL(videoEnded()), this, SLOT(getVideoStopped()));




}

MainWindow::~MainWindow()
{
    delete vdp;
    delete ui;
}

void MainWindow::on_actionOpen_triggered()
{

    if (vdp->isPlaying()) return;
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open File"), "", tr("Video Files (*.avi)"));

    if (fileName.isEmpty()) return;

    ui->fileLine->setText(fileName);
    openFile(fileName);



}

void MainWindow::on_actionPreferences_triggered()
{
    if (ui->settingsBox->isHidden()) ui->settingsBox->show(); else ui->settingsBox->hide();
}


void MainWindow::on_actionExit_triggered()
{
this->close();
}


void MainWindow::openFile(QString filename)
{


    //ui->label->setText(filename);
ui->videoSlider->setValue(0);


if (!vdp->loadVideo(filename))
{
 error(tr("Error in OpenCV while opening file"));
}

}


void MainWindow::on_pushButton_clicked()
{
    on_actionOpen_triggered();
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    //if (confirmClose()) {
    if (1) {
           saveSettings();
           event->accept();
       } else {
           event->ignore();
       }

}

bool MainWindow::confirmClose()
{
QMessageBox::StandardButton ret = QMessageBox::warning(this, tr("Really?"),
             tr("Really exit?"),
             QMessageBox::Close | QMessageBox::Cancel);

        if (ret == QMessageBox::Close)
            return true;
        else if (ret == QMessageBox::Cancel)
            return false;
        else
            return false;

}

void MainWindow::loadSettings()
{

        QSettings settings(settingsFile, QSettings::NativeFormat);
        restoreGeometry(settings.value("geometry").toByteArray());
        ui->speedSlider->setValue(settings.value("preferences/speed",50).toInt());
        ui->threshSlider->setValue(settings.value("preferences/thresh",50).toInt());

}

void MainWindow::saveSettings()
{
    QSettings settings(settingsFile, QSettings::NativeFormat);

    settings.setValue("preferences/speed",ui->speedSlider->value()  );
    settings.setValue("preferences/thresh", ui->threshSlider->value());

    settings.setValue("geometry", saveGeometry());




}

void MainWindow::on_playButton_clicked(bool checked)
{
    if (vdp->isPlaying())
    {
        vdp->pause();
        ui->playButton->setText(tr("Play"));
        ui->playButton->setShortcut(QKeySequence(Qt::Key_P));

    }
    else
    {
        if (vdp->play())  ui->playButton->setText(tr("Pause"));
        ui->playButton->setShortcut(QKeySequence(Qt::Key_P));

    }



}


void MainWindow::on_stopButton_clicked()
{
}

void MainWindow::play ()
{

}


void MainWindow::pause()

{


}


void MainWindow::stop()
{
}


void MainWindow::on_actionHelp_triggered()
{

}

void MainWindow::showFrame (QImage img)
{

    if (!img.isNull())
        {
            ui->label->setAlignment(Qt::AlignCenter);
            ui->label->setPixmap(QPixmap::fromImage(img).scaled(ui->label->size(),
                                               Qt::KeepAspectRatio, Qt::FastTransformation));
    }

}



void MainWindow::error (QString msg)
{
    QErrorMessage errorMessage;
    errorMessage.showMessage(msg);
    errorMessage.exec();
}

void MainWindow::getCurrentPosition(int pos)
{
ui->videoSlider->setValue(pos);
}


void MainWindow::getVideoStopped()
{
    ui->playButton->setText(tr("Play"));
    ui->playButton->setShortcut(QKeySequence(Qt::Key_P));
}
