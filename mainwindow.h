#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "classes/settings.h"
#include "classes/updater.h"

#include <QMainWindow>
#include <iostream>
#include <QCheckBox>
#include <QScrollBar>
#include <QLineEdit>
#include <QTextStream>

#include <chrono>
#include <thread>
#include <QVariant>
#include <QTime>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void evaluate_autorun();

private slots:
    void writeLog(QString logline);

    void run_selected_options();

    void update_arc();
    void update_taco();
    void update_tekkit();

    void run_gw2();
    void run_taco();

private:
    Ui::MainWindow *ui;
    Settings _settings = Settings("settings.ini");
    UpdateTool* _updater;
    std::vector<CheckBoxSetting*> _check_box_settings;

    void init_interface();
    void set_edit(QLineEdit* edit, QString text);

    void delay(int secs);

};
#endif // MAINWINDOW_H
