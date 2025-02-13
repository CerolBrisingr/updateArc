#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , _ui(new Ui::MainWindow)
    , _settings("settings.ini")
{
    _ui->setupUi(this);
    connect(&Log::writer, &Log::Logger::sendText,
            this, &MainWindow::writeLog,
            Qt::ConnectionType::QueuedConnection);

    initInterface();
    _update_helper = new UpdateTool();
    if (!_update_helper->isValid()) {
        disableInterface();
    }

    auto gw_path = _update_helper->getGwPath();
    _updaters.emplace_back(new Updater::ArcUpdater(gw_path, _ui->pushButton_arcdps, _ui->toolButton_block_arc, _ui->checkBox_arcdps, "arcdps"));

    std::shared_ptr<Installer::Installer> blishhud_installer = std::make_shared<Installer::InstallBlishhud>(gw_path);
    _updaters.emplace_back(new Updater::GitHupdater(gw_path, _ui->pushButton_blish, _ui->toolButton_remove_blish, _ui->checkBox_blish, Updater::Config::getBlishConfig(),
                                                    blishhud_installer));

    std::shared_ptr<Installer::Installer> boontable_installer = std::make_shared<Installer::InstallBoontable>(gw_path);
    _updaters.emplace_back(new Updater::GitHupdater(gw_path, _ui->pushButton_boon, _ui->toolButton_remove_boon, _ui->checkBox_boon, Updater::Config::getBoontableConfig(),
                                                    boontable_installer));

    std::shared_ptr<Installer::Installer> kp_installer = std::make_shared<Installer::InstallKp>(gw_path);
    _updaters.emplace_back(new Updater::GitHupdater(gw_path, _ui->pushButton_kp, _ui->toolButton_remove_kp, _ui->checkBox_kp, Updater::Config::getKpConfig(),
                                                    kp_installer));

    connect(_ui->pushButton_run_manually, SIGNAL(clicked()),
            this, SLOT(runSelectedOptions()));

    connect(_ui->pushButton_run_gw2, SIGNAL(clicked()),
            this, SLOT(runGw2()));
    connect(_ui->pushButton_run_blishhud, SIGNAL(clicked()),
            this, SLOT(runBlish()));
    connect(_ui->toolButton_config_run_gw2, SIGNAL(clicked()),
            this, SLOT(configGw2Arguments()));

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
    delete _ui;
}

void MainWindow::evaluateAutorun() {
    if (!_update_helper->isValid()) {
        return;
    }
    if (_ui->checkBox_autorun->isChecked()) {
        Log::write("Autorun active. Starting selected options.\n");
        runSelectedOptions();
    } else {
        Log::write("Autorun disabled. Do something already!\n");
    }
}

void MainWindow::writeLog(QString logline)
{
    _ui->eventlog->moveCursor(QTextCursor::End);
    _ui->eventlog->insertPlainText(logline);
    if (!_ui->eventlog->hasFocus()) {
        QScrollBar* sb = _ui->eventlog->verticalScrollBar();
        sb->setValue(sb->maximum());
    }
}

void MainWindow::initInterface()
{
    _check_box_settings.emplace_back(new CheckBoxSettings(_ui->checkBox_run_gw2, "starters/gw2_run"));
    _check_box_settings.emplace_back(new CheckBoxSettings(_ui->checkBox_run_blishhud, "starters/blish_run"));

    _line_edit_settings.emplace_back(new LineEditSettings(_ui->lineEdit_run_gw2, "starters/gw2_arguments", "-maploadinfo"));

    _check_box_settings.emplace_back(new CheckBoxSettings(_ui->checkBox_autorun, "general/autorun"));
    _check_box_settings.emplace_back(new CheckBoxSettings(_ui->checkBox_autoclose, "general/autoclose"));

    _ui->toolButton_config_arc->setVisible(false);
}

void MainWindow::setEdit(QLineEdit *edit, QString text)
{
    edit->setText(text);
}

bool MainWindow::runUpdate()
{
    bool do_start_gw2 = _settings.readValue("starters/gw2_run").compare("on") == 0;
    bool do_start_blish = _settings.readValue("starters/blish_run").compare("on") == 0;

    // Try to run each updater (don't run if checkbox is not set)
    for (auto* updater: _updaters) {
        if (_is_cancelled) return false;
        updater->autoUpdate();
    }
    if (_is_cancelled) return false;

    if (do_start_gw2) {
        runGw2();
    }
    if (_is_cancelled) return false;

    if (do_start_blish) {
        runBlish();
    }

    if (_settings.readValue("General/autoclose").compare("on") == 0) {
        int wait_secs = 20;
        writeLog("Waiting for " + QString::number(wait_secs) + " seconds before we close the updater\n");
        for (int i = 0; i < wait_secs; i += 5) {
            writeLog("Timer: " + QString::number(i) + "\n");
            delay(5);
            if (_is_cancelled) return false;
        }
        writeLog("At least " + QString::number(wait_secs) + " seconds are over.\n");
    }

    // User should be able to change their mind about closing the window automatically
    if (_settings.readValue("General/autoclose").compare("on") == 0) {
        exit(0);
    }
    return true;
}

void MainWindow::disableInterface()
{
    _ui->checkBox_arcdps->setEnabled(false);
    _ui->checkBox_autoclose->setEnabled(false);
    _ui->checkBox_autorun->setEnabled(false);
    _ui->checkBox_run_gw2->setEnabled(false);
    _ui->checkBox_boon->setEnabled(false);
    _ui->checkBox_kp->setEnabled(false);
    _ui->checkBox_blish->setEnabled(false);
    _ui->checkBox_run_blishhud->setEnabled(false);

    _ui->pushButton_arcdps->setEnabled(false);
    _ui->pushButton_run_gw2->setEnabled(false);
    _ui->pushButton_run_manually->setEnabled(false);
    _ui->pushButton_blish->setEnabled(false);
    _ui->pushButton_boon->setEnabled(false);
    _ui->pushButton_kp->setEnabled(false);
    _ui->pushButton_run_blishhud->setEnabled(false);

    _ui->lineEdit_run_gw2->setEnabled(false);

    _ui->toolButton_block_arc->setEnabled(false);
    _ui->toolButton_cancel->setEnabled(false);
    _ui->toolButton_config_run_gw2->setEnabled(false);
    _ui->toolButton_remove_blish->setEnabled(false);
    _ui->toolButton_remove_boon->setEnabled(false);
    _ui->toolButton_remove_kp->setEnabled(false);
}

void MainWindow::delay(int secs)
{
    QTime dieTime = QTime::currentTime().addMSecs( 1000 * secs );
    while( QTime::currentTime() < dieTime )
    {
        QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
    }
}

void MainWindow::runSelectedOptions()
{
    _ui->pushButton_run_manually->setEnabled(false);
    _ui->toolButton_cancel->setEnabled(true);

    _is_cancelled = false;
    if (!runUpdate()) {
        writeLog("Execution cancelled\n");
    }

    _ui->toolButton_cancel->setEnabled(false);
    _ui->pushButton_run_manually->setEnabled(true);
}

void MainWindow::runGw2()
{
    _ui->pushButton_run_gw2->setEnabled(false);
    _update_helper->startGW2();
    _ui->pushButton_run_gw2->setEnabled(true);
}

void MainWindow::runBlish()
{
    _ui->pushButton_run_blishhud->setEnabled(false);
    _update_helper->startBlish();
    _ui->pushButton_run_blishhud->setEnabled(true);
}

void MainWindow::configGw2Arguments()
{
    if (!_has_config) {
        QString arguments = _ui->lineEdit_run_gw2->text();
        _set_args = new Form(arguments, _update_helper);
        _has_config = true;
        _set_args->show();
        connect(_set_args, SIGNAL(closed()),
                this, SLOT(configClosed()));
        connect(_set_args, SIGNAL(updated(QString)),
                this, SLOT(configGw2Updated(QString)));
    }
}

void MainWindow::configGw2Updated(QString config)
{
    _ui->lineEdit_run_gw2->setText(config);
}

void MainWindow::configClosed()
{
    _set_args->deleteLater();
    _has_config = false;
    _set_args = nullptr;
}

void MainWindow::on_toolButton_cancel_clicked()
{
    _is_cancelled = true;
}

