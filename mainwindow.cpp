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


    vdp = new VideoProcessor(this);
    connect(vdp, SIGNAL(showFrame(QImage)), this, SLOT(showFrame(QImage)));
    connect(vdp, SIGNAL(emitCurrentPosition(int)), this, SLOT(getCurrentPosition(int)));
    connect(vdp, SIGNAL(videoEnded()), this, SLOT(getVideoStopped()));
    connect(vdp, SIGNAL(playError(QString)), this, error(QString);

    loadSettings();
}

MainWindow::~MainWindow()
{
    delete vdp;
    delete ui;
}

/**
 * @brief MainWindow::on_actionOpen_triggered
 * slot@command: open file command
 */
void MainWindow::on_actionOpen_triggered()
{
    if (vdp->isPlaying()) return;
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open File"), "", tr("Video Files (*.avi)"));
    if (fileName.isEmpty()) return;
    ui->fileLine->setText(fileName);
    openFile(fileName);

}

/**
 * @brief MainWindow::on_actionPreferences_triggered
 * slot@command
 */
void MainWindow::on_actionPreferences_triggered()
{
    if (ui->settingsBox->isHidden()) ui->settingsBox->show(); else ui->settingsBox->hide();
}

/**
 * @brief MainWindow::on_actionExit_triggered
 * slot@command
 */
void MainWindow::on_actionExit_triggered()
{
this->close();
}



/**
 * @brief MainWindow::on_pushButton_clicked
 * slot@command
 */
void MainWindow::on_pushButton_clicked()
{
    on_actionOpen_triggered();
}

/**
 * @brief MainWindow::closeEvent
 * smart closer - saving settings and maybe prompting if we are really
 * willing to exit
 * @param event
 */
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

/**
 * @brief MainWindow::confirmClose
 * standart close conf
 * @return
 */
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

/**
 * @brief MainWindow::openFile
 * file loader, prepare UI &&load file into vdp
 * @param filename
 */
void MainWindow::openFile(QString filename)
{
if (!vdp->loadVideo(filename))
{
 error(tr("Error in OpenCV while opening file"));
}
}

/**
 * @brief MainWindow::loadSettings
 * standart settings loader
 */
void MainWindow::loadSettings()
{

        QSettings settings(settingsFile, QSettings::NativeFormat);
        restoreGeometry(settings.value("geometry").toByteArray());

        ui->speedSlider->setValue(settings.value("preferences/speed",ui->speedSlider->maximum()/2).toInt());
        ui->sizeSlider->setValue(settings.value("preferences/size",ui->sizeSlider->maximum()/2).toInt());
        ui->threshSlider->setValue(settings.value("preferences/thresh",ui->threshSlider->maximum()/2).toInt());

        on_speedSlider_valueChanged(ui->speedSlider->value());
        on_sizeSlider_valueChanged(ui->sizeSlider->value());
        on_threshSlider_valueChanged(ui->threshSlider->value());

}

/**
 * @brief MainWindow::saveSettings
 * standart settings saver
 */
void MainWindow::saveSettings()
{
    QSettings settings(settingsFile, QSettings::NativeFormat);

    settings.setValue("preferences/speed",ui->speedSlider->value()  );
    settings.setValue("preferences/thresh", ui->threshSlider->value());
    settings.setValue("preferences/size", ui->sizeSlider->value());

    settings.setValue("geometry", saveGeometry());
}

/**
 * @brief MainWindow::on_playButton_clicked
 * slot@command
 * here comes 'magic button' anti-pattern,
 * but acceptable as this is the only point of launch
 * @param checked
 */
void MainWindow::on_playButton_clicked(bool checked)
{
    if (vdp->isPlaying())
    {
        vdp->pause();
        ui->playButton->setText(tr("Play"));
    }
    else
    {
        if (vdp->play())  ui->playButton->setText(tr("Pause"));
    }
    ui->playButton->setShortcut(QKeySequence(Qt::Key_P));
}

void MainWindow::on_stopButton_clicked()
{
    vdp->seek0();
}


void MainWindow::on_actionHelp_triggered()
{
    const QString help_str=tr("Motion detection video player \n"
                        "Hotkeys: \n"
                        "Crtl+P - open preferences \n"
                        "Crtl+Q - exit\n"
                        "Crtl+O - open file\n"
                        "F1 - open help (this window)"
                        "Movement detection settings:\n"
                        "Speed - the more speed, the fastest objects  to be caught. "
                        "78 optimized for humans walking\n\n"
                        "Treshold - less  treshold - more details to be caught, "
                        "but more mistakes due to changing lightning conditions\n\n"
                        "Size - minimal size (%) of object rectangle frame to be "
                        "considered. % of overall window area. \n\n"
                        "Ilia S. Vassiliev for ELSI");

    QMessageBox::about(this, tr("About"), help_str);

}

/**
 * @brief MainWindow::showFrame
 * showing frame in video window
 * @param img
 */
void MainWindow::showFrame (QImage img)
{
    if (!img.isNull())
        {
            ui->label->setAlignment(Qt::AlignCenter);
            ui->label->setPixmap(QPixmap::fromImage(img).scaled(ui->label->size(),
                                               Qt::KeepAspectRatio, Qt::FastTransformation));
    }
}

/**
 * @brief MainWindow::error
 * simple error gateway: err stream goes to the screen only, no log etc.
 * @param msg
 */
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

void MainWindow::on_speedSlider_valueChanged(int value)
{
    vdp->set_frameStep(ui->speedSlider->maximum()-value);
    ui->lcdSpeed->display(value);
}

void MainWindow::on_sizeSlider_valueChanged(int value)
{
    vdp->set_minSizeInPercents(value);
    ui->lcdSize->display(value);
}

void MainWindow::on_threshSlider_valueChanged(int value)
{
    vdp->set_threshold(value);
    ui->lcdTresh->display(value);
}

void MainWindow::on_videoSlider_valueChanged(int value)
{

}

void MainWindow::on_videoSlider_sliderReleased()
{
vdp->seek(ui->videoSlider->value());
if (vdp->playFlagRemember) vdp->play(); //if we played before movin'slider, then play
}

void MainWindow::on_videoSlider_sliderPressed()
{
    vdp->playFlagRemember = vdp->isPlaying();
    vdp->pause();
}


void MainWindow::on_videoSlider_sliderMoved(int position)
{
}
