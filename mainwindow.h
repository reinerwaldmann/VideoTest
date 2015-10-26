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

    void play ();
    void pause();
    void stop() ;





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



private:
    Ui::MainWindow *ui;
    void openFile(QString filename);
    bool confirmClose();
    void error (QString msg);

    //settings
    const QString settingsFile="settings.ini";
    void loadSettings();
    void saveSettings();

    //video stuff
    VideoProcessor  * vdp;





};

#endif // MAINWINDOW_H
