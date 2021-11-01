#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , _settings("settings.ini")
{
    ui->setupUi(this);
    connect(&Log::writer, &Log::Logger::sendText,
            this, &MainWindow::writeLog,
            Qt::ConnectionType::QueuedConnection);

    init_interface();
    _update_helper = new UpdateTool();
    if (!_update_helper->isValid()) {
        disable_interface();
    }

    auto gw_path = _update_helper->getGwPath();
    _updaters.emplace_back(new Updater::ArcUpdater(gw_path, ui->pushButton_arcdps, ui->toolButton_block_arc, ui->checkBox_arcdps, "arcdps"));
    _updaters.emplace_back(new Updater::GitHupdater(gw_path, ui->pushButton_taco, ui->toolButton_remove_taco, ui->checkBox_taco, "taco",
                                                    "BoyC", "GW2TacO"));
    _updaters.emplace_back(new Updater::TekkitUpdater(gw_path, ui->pushButton_tekkit, ui->toolButton_remove_tekkit, ui->checkBox_tekkit, "tekkit"));

    connect(ui->pushButton_run_manually, SIGNAL(clicked()),
            this, SLOT(run_selected_options()));

    connect(ui->pushButton_run_gw2, SIGNAL(clicked()),
            this, SLOT(run_gw2()));
    connect(ui->pushButton_run_taco, SIGNAL(clicked()),
            this, SLOT(run_taco()));
    connect(ui->toolButton_config_run_gw2, SIGNAL(clicked()),
            this, SLOT(config_gw2_arguments()));

}

MainWindow::~MainWindow()
{
    _is_cancelled = true;
    delete _update_helper;
    for (auto* updater: _updaters) {
        delete updater;
    }
    for (auto* line_edit_setting: _line_edit_settings) {
        delete line_edit_setting;
    }
    for (auto* check_box_setting: _check_box_settings) {
        delete check_box_setting;
    }
    delete ui;
}

void MainWindow::evaluate_autorun() {
    if (!_update_helper->isValid()) {
        return;
    }
    if (ui->checkBox_autorun->isChecked()) {
        Log::write("Autorun active. Starting selected options.\n");
        run_selected_options();
    } else {
        Log::write("Autorun disabled. Do something already!\n");
    }
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

void MainWindow::init_interface()
{
    _check_box_settings.emplace_back(new CheckBoxSetting(ui->checkBox_run_gw2, "starters/gw2_run"));
    _check_box_settings.emplace_back(new CheckBoxSetting(ui->checkBox_run_taco, "starters/taco_run"));

    _line_edit_settings.emplace_back(new LineEditSettings(ui->lineEdit_run_gw2, "starters/gw2_arguments", "-maploadinfo"));
    _line_edit_settings.emplace_back(new LineEditSettings(ui->lineEdit_run_taco, "starters/taco_delay", "60"));

    _check_box_settings.emplace_back(new CheckBoxSetting(ui->checkBox_autorun, "general/autorun"));
    _check_box_settings.emplace_back(new CheckBoxSetting(ui->checkBox_autoclose, "general/autoclose"));
}

void MainWindow::set_edit(QLineEdit *edit, QString text)
{
    edit->setText(text);
}

bool MainWindow::run_update()
{
    bool do_start_gw2 = _settings.getValue("starters/gw2_run").compare("on") == 0;
    bool do_start_taco = _settings.getValue("starters/taco_run").compare("on") == 0;
    do_start_taco &= do_start_gw2;

    int wait_secs = QVariant(_settings.getValue("starters/taco_delay")).toInt();
    if (wait_secs < 30) {
        wait_secs = 30;
    }
    if (_is_cancelled) return false;

    // Try to run each updater (don't run if checkbox is not set)
    for (auto* updater: _updaters) {
        if (_is_cancelled) return false;
        updater->autoUpdate();
    }

    if (_is_cancelled) return false;

    if (do_start_gw2) {
        run_gw2();
    }
    if (_is_cancelled) return false;

    if (do_start_taco) {
        ui->pushButton_run_taco->setEnabled(false);
        writeLog("Waiting for " + QString(wait_secs) + "seconds before we start TacO\n");
        for (int i = 0; i < wait_secs; i += 5) {
            writeLog("Timer: " + QString(i) + "\n");
            delay(5);
            if (_is_cancelled) {
                ui->pushButton_run_taco->setEnabled(true);
                return false;
            }
        }
        writeLog("At least " + QString(wait_secs) + " seconds are over.\n");
        run_taco();
    } else if (_settings.getValue("General/autoclose").compare("on") == 0) {
        wait_secs = 20;
        writeLog("Waiting for " + QString(wait_secs) + " seconds before we close the updater\n");
        for (int i = 0; i < wait_secs; i += 5) {
            writeLog("Timer: " + QString(i) + "\n");
            delay(5);
            if (_is_cancelled) return false;
        }
        writeLog("At least " + QString(wait_secs) + " seconds are over.\n");
    }

    // User should be able to change their mind about closing the window automatically
    if (_settings.getValue("General/autoclose").compare("on") == 0) {
        this->close();
    }
    return true;
}

void MainWindow::disable_interface()
{
    ui->checkBox_arcdps->setEnabled(false);
    ui->checkBox_autoclose->setEnabled(false);
    ui->checkBox_autorun->setEnabled(false);
    ui->checkBox_run_gw2->setEnabled(false);
    ui->checkBox_run_taco->setEnabled(false);
    ui->checkBox_taco->setEnabled(false);
    ui->checkBox_tekkit->setEnabled(false);

    ui->pushButton_arcdps->setEnabled(false);
    ui->pushButton_run_gw2->setEnabled(false);
    ui->pushButton_run_manually->setEnabled(false);
    ui->pushButton_run_taco->setEnabled(false);
    ui->pushButton_tekkit->setEnabled(false);
    ui->pushButton_taco->setEnabled(false);

    ui->lineEdit_run_gw2->setEnabled(false);
    ui->lineEdit_run_taco->setEnabled(false);

    ui->toolButton_block_arc->setEnabled(false);
    ui->toolButton_remove_taco->setEnabled(false);
    ui->toolButton_remove_tekkit->setEnabled(false);
    ui->toolButton_cancel->setEnabled(false);
    ui->toolButton_config_run_gw2->setEnabled(false);
}

void MainWindow::delay(int secs)
{
    QTime dieTime = QTime::currentTime().addMSecs( 1000 * secs );
    while( QTime::currentTime() < dieTime )
    {
        QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
    }
}

void MainWindow::run_selected_options()
{
    ui->pushButton_run_manually->setEnabled(false);
    ui->toolButton_cancel->setEnabled(true);

    _is_cancelled = false;
    if (!run_update()) {
        writeLog("Execution cancelled\n");
    }

    ui->toolButton_cancel->setEnabled(false);
    ui->pushButton_run_manually->setEnabled(true);
}

void MainWindow::run_gw2()
{
    ui->pushButton_run_gw2->setEnabled(false);
    _update_helper->startGW2();
    ui->pushButton_run_gw2->setEnabled(true);
}

void MainWindow::run_taco()
{
    ui->pushButton_run_taco->setEnabled(false);
    _update_helper->startTacO();
    ui->pushButton_run_taco->setEnabled(true);
}

void MainWindow::config_gw2_arguments()
{
    if (!_has_config) {
        QString arguments = ui->lineEdit_run_gw2->text();
        _set_args = new Form(arguments, _update_helper);
        _has_config = true;
        _set_args->show();
        connect(_set_args, SIGNAL(closed()),
                this, SLOT(config_closed()));
        connect(_set_args, SIGNAL(updated(QString)),
                this, SLOT(config_gw2_updated(QString)));
    }
}

void MainWindow::config_gw2_updated(QString config)
{
    ui->lineEdit_run_gw2->setText(config);
}

void MainWindow::config_closed()
{
    _set_args->deleteLater();
    _has_config = false;
    _set_args = nullptr;
}

void MainWindow::on_toolButton_cancel_clicked()
{
    _is_cancelled = true;
}
