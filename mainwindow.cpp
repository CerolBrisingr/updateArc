#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init_interface();
    _updater = new UpdateTool(&_settings);

    connect(_updater, SIGNAL(write_log(QString)),
            this, SLOT(writeLog(QString)));

    _updater->verifyLocation();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::writeLog(QString logline)
{
    ui->eventlog->moveCursor(QTextCursor::End);
    ui->eventlog->insertPlainText(logline);
    if (!ui->eventlog->hasFocus()) {
        QScrollBar* sb = ui->eventlog->verticalScrollBar();
        sb->setValue(sb->maximum());
    }
}

void MainWindow::on_pushButton_arcdps_clicked()
{
    _updater->updateArc();
}

void MainWindow::init_interface()
{
    _check_box_settings.push_back(new CheckBoxSetting(ui->checkBox_arcdps, &_settings, "updaters/arcdps"));
    _check_box_settings.push_back(new CheckBoxSetting(ui->checkBox_taco, &_settings, "updaters/taco"));
    _check_box_settings.push_back(new CheckBoxSetting(ui->checkBox_tekkit, &_settings, "updaters/tekkit"));

    _check_box_settings.push_back(new CheckBoxSetting(ui->checkBox_run_gw2, &_settings, "starters/gw2_run"));
    _check_box_settings.push_back(new CheckBoxSetting(ui->checkBox_run_taco, &_settings, "starters/taco_run"));

    set_edit(ui->lineEdit_run_gw2,
             _settings.getValueWrite("starters/gw2_arguments", "-bmp -maploadinfo"));
    ui->lineEdit_run_gw2->setEnabled(false);
    set_edit(ui->lineEdit_run_taco,
             _settings.getValueWrite("starters/taco_delay", "60"));
    ui->lineEdit_run_taco->setEnabled(false);

    _check_box_settings.push_back(new CheckBoxSetting(ui->checkBox_autorun, &_settings, "general/autorun"));
    _check_box_settings.push_back(new CheckBoxSetting(ui->checkBox_autoclose, &_settings, "general/autoclose"));
}

void MainWindow::set_edit(QLineEdit *edit, QString text)
{
    edit->setText(text);
}

void MainWindow::on_pushButton_taco_clicked()
{
    _updater->updateTaco();
}

void MainWindow::on_pushButton_tekkit_clicked()
{
    _updater->updateTekkit();
}

void MainWindow::on_pushButton_run_gw2_clicked()
{
    _updater->startGW2();
}

void MainWindow::on_pushButton_run_taco_clicked()
{
    _updater->startTacO();
}
