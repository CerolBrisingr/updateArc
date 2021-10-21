#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(&Log::writer, &Log::Logger::sendText,
            this, &MainWindow::writeLog,
            Qt::ConnectionType::QueuedConnection);

    init_interface();
    _update_helper = new UpdateTool(&_settings);
    if (!_update_helper->verifyLocation()) {
        disable_interface();
    } else {
        _location_ok = true;
    }

    connect(_update_helper, SIGNAL(write_log(QString)),
            this, SLOT(writeLog(QString)), Qt::QueuedConnection);

    auto gw_path = _update_helper->_gw_path;
    _updaters.emplace_back(new Updater::ArcUpdater(gw_path, ui->pushButton_arcdps, ui->toolButton_block_arc, ui->checkBox_arcdps));
    _updaters.emplace_back(new Updater::TekkitUpdater(gw_path, ui->pushButton_tekkit, ui->toolButton_remove_tekkit, ui->checkBox_tekkit));



    connect(ui->pushButton_run_manually, SIGNAL(clicked()),
            this, SLOT(run_selected_options()));

    connect(ui->pushButton_taco, SIGNAL(clicked()),
            this, SLOT(update_taco()));

    connect(ui->pushButton_run_gw2, SIGNAL(clicked()),
            this, SLOT(run_gw2()));
    connect(ui->pushButton_run_taco, SIGNAL(clicked()),
            this, SLOT(run_taco()));
    connect(ui->toolButton_config_run_gw2, SIGNAL(clicked()),
            this, SLOT(config_gw2_arguments()));

}

MainWindow::~MainWindow()
{
    delete _update_helper;
    for (auto* updater: _updaters) {
        delete updater;
    }
    delete ui;
}

void MainWindow::evaluate_autorun() {
    if (!_location_ok) {
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
    _check_box_settings.push_back(new CheckBoxSetting(ui->checkBox_arcdps, &_settings, "updaters/arcdps"));
    _check_box_settings.push_back(new CheckBoxSetting(ui->checkBox_taco, &_settings, "updaters/taco"));
    _check_box_settings.push_back(new CheckBoxSetting(ui->checkBox_tekkit, &_settings, "updaters/tekkit"));

    _check_box_settings.push_back(new CheckBoxSetting(ui->checkBox_run_gw2, &_settings, "starters/gw2_run"));
    _check_box_settings.push_back(new CheckBoxSetting(ui->checkBox_run_taco, &_settings, "starters/taco_run"));

    _line_edit_settings.push_back(new LineEditSettings(ui->lineEdit_run_gw2, &_settings,
                                                       "starters/gw2_arguments", "-maploadinfo"));
    _line_edit_settings.push_back(new LineEditSettings(ui->lineEdit_run_taco, &_settings,
                                                       "starters/taco_delay", "60"));

    _check_box_settings.push_back(new CheckBoxSetting(ui->checkBox_autorun, &_settings, "general/autorun"));
    _check_box_settings.push_back(new CheckBoxSetting(ui->checkBox_autoclose, &_settings, "general/autoclose"));
}

void MainWindow::set_edit(QLineEdit *edit, QString text)
{
    edit->setText(text);
}

bool MainWindow::run_update()
{
    // Currently necessary for streamed text output
    QTextStream stream;
    QString stream_string;
    stream.setString(&stream_string);

    bool do_update_taco = _settings.getValue("updaters/taco").compare("on") == 0;

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
    if (do_update_taco) {
        update_taco();
    }
    if (_is_cancelled) return false;

    if (do_start_gw2) {
        run_gw2();
    }
    if (_is_cancelled) return false;

    if (do_start_taco) {
        ui->pushButton_run_taco->setEnabled(false);
        stream << "Waiting for " << wait_secs << "seconds before we start TacO"; writeLog(stream.readAll() + "\n");
        for (int i = 0; i < wait_secs; i += 5) {
            stream << "Timer: " << i; writeLog(stream.readAll() + "\n");
            delay(5);
            if (_is_cancelled) {
                ui->pushButton_run_taco->setEnabled(true);
                return false;
            }
        }
        stream << "At least " << wait_secs  << " seconds are over."; writeLog(stream.readAll() + "\n");
        run_taco();
    } else if (_settings.getValue("General/autoclose").compare("on") == 0) {
        wait_secs = 20;
        stream << "Waiting for " << wait_secs << " seconds before we close the updater"; writeLog(stream.readAll() + "\n");
        for (int i = 0; i < wait_secs; i += 5) {
            stream << "Timer: " << i; writeLog(stream.readAll() + "\n");
            delay(5);
            if (_is_cancelled) return false;
        }
        stream << "At least " << wait_secs  << " seconds are over."; writeLog(stream.readAll() + "\n");
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

void MainWindow::update_taco()
{
    ui->pushButton_taco->setEnabled(false);
    _update_helper->updateTaco();
    ui->pushButton_taco->setEnabled(true);
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
