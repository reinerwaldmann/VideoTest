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
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open File"), "", tr("Video Files (*.avi)"));

    if (!fileName.isEmpty())
    {
        ui->fileLine->setText(fileName);
    }

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
if (!vdp.loadVideo(filename))
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
    if (checked) play(); else pause();

}


void MainWindow::on_stopButton_clicked()
{
}

void MainWindow::play ()
{
    if (1)
        ui->playButton->setText(tr("Pause"));

}


void MainWindow::pause()

{

    if (1)
        ui->playButton->setText(tr("Play"));
}


void MainWindow::stop()
{
}


void MainWindow::on_actionHelp_triggered()
{

}

void MainWindow::showFrame (const QImage &img)
{
    ui->label->setPixmap(QPixmap::fromImage(img));
}



void MainWindow::error (QString msg)
{
    QErrorMessage errorMessage;
    errorMessage.showMessage(msg);
    errorMessage.exec();
}

