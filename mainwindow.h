#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Canvas.h"
#include <QMenuBar>
#include <QAction>
#include <QColorDialog>
#include <QVBoxLayout>
#include <QInputDialog>
#include <QToolBar>
#include "ui_MainWindow.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Canvas *canvas;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
