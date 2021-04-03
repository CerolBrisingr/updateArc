#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "classes/settings.h"
#include "classes/updater.h"

#include <QMainWindow>
#include <iostream>
#include <QCheckBox>
#include <QScrollBar>
#include <QLineEdit>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void on_pushButton_arcdps_clicked();
    void writeLog(QString logline);

    void on_pushButton_taco_clicked();

    void on_pushButton_tekkit_clicked();

    void on_pushButton_run_gw2_clicked();

    void on_pushButton_run_taco_clicked();

private:
    Ui::MainWindow *ui;
    Settings _settings = Settings("settings.ini");
    UpdateTool* _updater;
    std::vector<CheckBoxSetting*> _check_box_settings;

    void init_interface();
    void set_edit(QLineEdit* edit, QString text);

};
#endif // MAINWINDOW_H
