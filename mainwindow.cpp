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

    connect(ui->pushButton_run_manually, SIGNAL(clicked()),
            this, SLOT(run_selected_options()));

    connect(ui->pushButton_arcdps, SIGNAL(clicked()),
            this, SLOT(update_arc()));
    connect(ui->pushButton_taco, SIGNAL(clicked()),
            this, SLOT(update_taco()));
    connect(ui->pushButton_tekkit, SIGNAL(clicked()),
            this, SLOT(update_tekkit()));

    connect(ui->pushButton_run_gw2, SIGNAL(clicked()),
            this, SLOT(run_gw2()));
    connect(ui->pushButton_run_taco, SIGNAL(clicked()),
            this, SLOT(run_taco()));

    _updater->verifyLocation();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::evaluate_autorun() {
    if (ui->checkBox_autorun->isChecked()) {
        writeLog("Autorun active. Starting selected options.\n");
        run_selected_options();
    } else {
        writeLog("Autorun disabled. Do something already!\n");
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
    // Currently necessary for streamed text output
    QTextStream stream;
    QString stream_string;
    stream.setString(&stream_string);

    ui->pushButton_run_manually->setEnabled(false);

    bool do_update_arc = _settings.getValue("updaters/arcdps").compare("on") == 0;
    bool do_update_taco = _settings.getValue("updaters/taco").compare("on") == 0;
    bool do_update_tekkit = _settings.getValue("updaters/tekkit").compare("on") == 0;

    bool do_start_gw2 = _settings.getValue("starters/gw2_run").compare("on") == 0;
    bool do_start_taco = _settings.getValue("starters/taco_run").compare("on") == 0;
    do_start_taco &= do_start_gw2;

    int wait_secs = QVariant(_settings.getValue("starters/taco_delay")).toInt();
    if (wait_secs < 30) {
        wait_secs = 30;
    }

    if (do_update_arc) {
        update_arc();
    }
    if (do_update_taco) {
        update_taco();
    }
    if (do_update_tekkit) {
        update_tekkit();
    }

    if (do_start_gw2) {
        run_gw2();
    }

    if (do_start_taco) {
        ui->checkBox_run_taco->setEnabled(false);
        stream << "Waiting for " << wait_secs << "seconds before we start TacO"; writeLog(stream.readAll() + "\n");
        for (int i = 0; i < wait_secs; i += 5) {
            stream << "Timer: " << i; writeLog(stream.readAll() + "\n");
            delay(5);
        }
        stream << "At least " << wait_secs  << " are over."; writeLog(stream.readAll() + "\n");
        run_taco();
    } else if (_settings.getValue("General/autoclose").compare("on") == 0) {
        wait_secs = 20;
        stream << "Waiting for " << wait_secs << " seconds before we close the updater"; writeLog(stream.readAll() + "\n");
        for (int i = 0; i < wait_secs; i += 5) {
            stream << "Timer: " << i; writeLog(stream.readAll() + "\n");
            delay(5);
        }
        stream << "At least " << wait_secs  << " are over."; writeLog(stream.readAll() + "\n");
    }

    if (_settings.getValue("General/autoclose").compare("on") == 0) {
        this->close();
    }

    ui->pushButton_run_manually->setEnabled(true);
}

void MainWindow::update_arc()
{
    ui->pushButton_arcdps->setEnabled(false);
    _updater->updateArc();
    ui->pushButton_arcdps->setEnabled(true);
}

void MainWindow::update_taco()
{
    ui->pushButton_taco->setEnabled(false);
    _updater->updateTaco();
    ui->pushButton_taco->setEnabled(true);
}

void MainWindow::update_tekkit()
{
    ui->pushButton_tekkit->setEnabled(false);
    _updater->updateTekkit();
    ui->pushButton_tekkit->setEnabled(true);
}

void MainWindow::run_gw2()
{
    ui->pushButton_run_gw2->setEnabled(false);
    _updater->startGW2();
    ui->pushButton_run_gw2->setEnabled(true);
}

void MainWindow::run_taco()
{
    ui->pushButton_run_taco->setEnabled(false);
    _updater->startTacO();
    ui->pushButton_run_taco->setEnabled(true);
}
