#include "form.h"
#include "ui_form.h"
#include <iostream>

Form::Form(QString arguments, UpdateTool *updater, QWidget *parent) :
    QWidget(parent),
    _ui(new Ui::Form),
    _updater(updater)
{
    _ui->setupUi(this);
    _commands = new CommandList(_ui->lineEdit_custom);
    _commands->add("-bmp", _ui->checkBox_bmp);
    _commands->add("-clientport", _ui->checkBox_clientport, _ui->comboBox_clientport);
    _commands->add("-maploadinfo", _ui->checkBox_maploadinfo);
    _commands->add("-autologin", _ui->checkBox_autologin);

    _commands->importArguments(arguments);

    linkInteractions();
    onInteraction();
}

void Form::closeEvent(QCloseEvent *event) {
    emit closed();
    event->accept();
}

Form::~Form()
{
    delete _ui;
}


CommandSet::CommandSet(QString& command, QCheckBox *check, QComboBox *combo)
    :_command(command)
    ,_check(check)
    ,_combo(combo)
{

}

CommandSet::~CommandSet()
{}

QString CommandSet::printCommand()
{
    QString output = _command;
    if (_combo != nullptr) {
        output += " " + _combo->currentText();
    }
    return output;
}

bool CommandSet::import(QString argument)
{
    if (argument.contains(_command)) {
        _check->setCheckState(Qt::CheckState::Checked);
        if (_combo != nullptr) {
            QString value = extractValue(argument);
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

bool CommandSet::isInUse()
{
    return (_check->checkState() == Qt::CheckState::Checked);
}

QString CommandSet::extractValue(QString argument)
{
    QString test = argument.remove(0, _command.size() + 1).trimmed();
    return test;
}

CommandList::CommandList(QLineEdit *custom_commands)
    :_custom_commands(custom_commands)
{

}

CommandList::~CommandList()
{
    for (auto set: _sets) {
        delete(set);
    }
}

void CommandList::add(QString command, QCheckBox *check, QComboBox *combo)
{
    _sets.push_back(new CommandSet(command, check, combo));
}

void CommandList::importArguments(QString arguments)
{

    QStringList arguments_split = arguments.split("-", Qt::SkipEmptyParts);
    QStringList arguments_remain;
    for (QString& argument: arguments_split) {
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
    for (auto& command: arguments_remain) {
        custom_commands += command + " ";
    }
    _custom_commands->setText(custom_commands.trimmed());

    return;
}

QString CommandList::createCommandString()
{
    QString commands;
    for (auto set: _sets) {
        if (set->isInUse()) {
            commands += set->printCommand() + " ";
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
    emit updated(_ui->lineEdit_full->text());
}

void Form::on_pushButton_ok_clicked()
{
    emit updated(_ui->lineEdit_full->text());
    this->close();
}

void Form::onInteraction()
{
    _ui->lineEdit_full->setText(_commands->createCommandString());
}

void Form::linkInteractions()
{
    // Check boxes
    connect(_ui->checkBox_autologin, SIGNAL(stateChanged(int)),
            this, SLOT(onInteraction()));
    connect(_ui->checkBox_bmp, SIGNAL(stateChanged(int)),
            this, SLOT(onInteraction()));
    connect(_ui->checkBox_clientport, SIGNAL(stateChanged(int)),
            this, SLOT(onInteraction()));
    connect(_ui->checkBox_maploadinfo, SIGNAL(stateChanged(int)),
            this, SLOT(onInteraction()));

    // Combo boxes
    connect(_ui->comboBox_clientport, SIGNAL(currentTextChanged(QString)),
            this, SLOT(onInteraction()));

    // Line edits
    connect(_ui->lineEdit_custom, SIGNAL(textChanged(QString)),
            this, SLOT(onInteraction()));
}
