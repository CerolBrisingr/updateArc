#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "classes/settings.h"
#include "classes/updater.h"
#include "classes/logger.h"
#include <updater/baseupdater.h>
#include <updater/arcupdater.h>
#include <updater/githupdater.h>
#include "form.h"

#include <installer/install_blishhud.h>
#include <installer/install_boontable.h>
#include <installer/install_kp.h>

#include <QMainWindow>
#include <iostream>
#include <QCheckBox>
#include <QScrollBar>
#include <QLineEdit>
#include <QTextStream>
#include <QVariant>
#include <QTime>

#include <vector>
#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    void evaluateAutorun();

public slots:
    void writeLog(QString logline);

private slots:
    void runSelectedOptions();

    void runGw2();
    void runBlish();

    void configGw2Arguments();
    void configGw2Updated(QString config);
    void configClosed();

    void onToolButtonCancelClicked();

private:
    Ui::MainWindow *_ui;
    Form *_set_args = nullptr;
    bool _has_config = false;
    bool _is_cancelled = false;
    Settings _settings; // settings.ini

    UpdateTool* _update_helper;
    std::vector<Updater::BaseUpdater*> _updaters;

    std::vector<CheckBoxSetting*> _check_box_settings;
    std::vector<LineEditSettings*> _line_edit_settings;

    void initInterface();
    void setEdit(QLineEdit* edit, QString text);
    bool runUpdate();
    void disableInterface();

    void delay(int secs);

};
#endif // MAINWINDOW_H
