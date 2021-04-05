#include "form.h"
#include "ui_form.h"
#include <iostream>

Form::Form(QString arguments, UpdateTool *updater, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form),
    _updater(updater)
{
    ui->setupUi(this);
    _commands = new command_list(ui->lineEdit_custom);
    _commands->add("-bmp", ui->checkBox_bmp);
    _commands->add("-clientport", ui->checkBox_clientport, ui->comboBox_clientport);
    _commands->add("-maploadinfo", ui->checkBox_maploadinfo);
    _commands->add("-autologin", ui->checkBox_autologin);

    _commands->import_arguments(arguments);

    link_interactions();
    on_interaction();
}

void Form::closeEvent(QCloseEvent *event) {
    emit(closed());
    event->accept();
}

Form::~Form()
{
    delete ui;
}


command_set::command_set(QString& command, QCheckBox *check, QComboBox *combo)
    :_command(command)
    ,_check(check)
    ,_combo(combo)
{

}

command_set::~command_set()
{

}

QString command_set::print_command()
{
    QString output = _command;
    if (_combo != nullptr) {
        output += " " + _combo->currentText();
    }
    return output;
}

bool command_set::import(QString argument)
{
    if (argument.contains(_command)) {
        _check->setCheckState(Qt::CheckState::Checked);
        if (_combo != nullptr) {
            QString value = extract_value(argument);
            int fitting_index = _combo->findText(value);
            if (fitting_index == -1) {
                fitting_index = 0;
            }
            _combo->setCurrentIndex(fitting_index);
        }
        return true;
    } else {
        return false;
    }
}

bool command_set::is_in_use()
{
    return (_check->checkState() == Qt::CheckState::Checked);
}

QString command_set::extract_value(QString argument)
{
    QString test = argument.remove(0, _command.size() + 1).trimmed();
    return test;
}

command_list::command_list(QLineEdit *custom_commands)
    :_custom_commands(custom_commands)
{

}

command_list::~command_list()
{
    for (auto set: _sets) {
        delete(set);
    }
}

void command_list::add(QString command, QCheckBox *check, QComboBox *combo)
{
    _sets.push_back(new command_set(command, check, combo));
}

void command_list::import_arguments(QString arguments)
{

    QStringList arguments_split = arguments.split("-", QString::SplitBehavior::SkipEmptyParts);
    QStringList arguments_remain;
    for (QString argument: arguments_split) {
        argument = "-" + argument.trimmed();
        bool hit = false;
        for (auto set: _sets) {
            hit = set->import(argument);
            if (hit) {
                break;
            }
        }
        if (!hit) {
            arguments_remain.append(argument);
        }
    }
    QString custom_commands = "";
    for (auto command: arguments_remain) {
        custom_commands += command + " ";
    }
    _custom_commands->setText(custom_commands.trimmed());

    return;
}

QString command_list::create_command_string()
{
    QString commands;
    for (auto set: _sets) {
        if (set->is_in_use()) {
            commands += set->print_command() + " ";
        }
    }
    commands += _custom_commands->text();
    return commands.trimmed();
}

void Form::on_pushButton_image_clicked()
{
    _updater->startGW2(QStringList("-image"));
}

void Form::on_pushButton_repair_clicked()
{
    _updater->startGW2(QStringList("-repair"));
}

void Form::on_pushButton_diag_clicked()
{
    _updater->startGW2(QStringList("-diag"));
}

void Form::on_pushButton_verify_clicked()
{
    _updater->startGW2(QStringList("-verify"));
}

void Form::on_pushButton_cancel_clicked()
{
    this->close();
}

void Form::on_pushButton_apply_clicked()
{
    emit(updated(ui->lineEdit_full->text()));
}

void Form::on_pushButton_ok_clicked()
{
    emit(updated(ui->lineEdit_full->text()));
    this->close();
}

void Form::on_interaction()
{
    ui->lineEdit_full->setText(_commands->create_command_string());
}

void Form::link_interactions()
{
    // Check boxes
    connect(ui->checkBox_autologin, SIGNAL(stateChanged(int)),
            this, SLOT(on_interaction()));
    connect(ui->checkBox_bmp, SIGNAL(stateChanged(int)),
            this, SLOT(on_interaction()));
    connect(ui->checkBox_clientport, SIGNAL(stateChanged(int)),
            this, SLOT(on_interaction()));
    connect(ui->checkBox_maploadinfo, SIGNAL(stateChanged(int)),
            this, SLOT(on_interaction()));

    // Combo boxes
    connect(ui->comboBox_clientport, SIGNAL(currentTextChanged(QString)),
            this, SLOT(on_interaction()));

    // Line edits
    connect(ui->lineEdit_custom, SIGNAL(textChanged(QString)),
            this, SLOT(on_interaction()));
}
