#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QMessageBox>
#include <QSettings>
#include "videoprocessor.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event);



private slots:
    void on_actionOpen_triggered();

    void on_actionExit_triggered();

    void on_actionPreferences_triggered();

    void on_pushButton_clicked();

    void on_playButton_clicked(bool checked);

    void on_stopButton_clicked();

    void on_actionHelp_triggered();

    void showFrame (QImage img);

    void getCurrentPosition(int pos);

    void getVideoStopped();



    void on_speedSlider_valueChanged(int value);

    void on_sizeSlider_valueChanged(int value);

    void on_threshSlider_valueChanged(int value);

    void on_videoSlider_valueChanged(int value);

    void on_videoSlider_sliderReleased();

    void on_videoSlider_sliderPressed();


    void on_videoSlider_sliderMoved(int position);

    void error (QString msg);

private:
    Ui::MainWindow *ui;
    void openFile(QString filename);
    bool confirmClose();


    //settings
    const QString settingsFile="settings.ini";
    void loadSettings();
    void saveSettings();

    //video stuff
    VideoProcessor  * vdp;





};

#endif // MAINWINDOW_H
