#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "opencv/cv.h"
#include <opencv/highgui.h>
#include <stdlib.h>
#include <stdio.h>



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}
