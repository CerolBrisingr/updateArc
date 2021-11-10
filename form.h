#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include <QCloseEvent>
#include <QCheckBox>
#include <QComboBox>
#include "classes/updater.h"

namespace Ui {
class Form;
}
class command_set;
class command_list;


class Form : public QWidget
{
    Q_OBJECT

public:
    explicit Form(QString arguments, UpdateTool *updater, QWidget *parent = nullptr);
    ~Form() override;

signals:
    void closed();
    void updated(QString);

private slots:
    void on_pushButton_image_clicked();
    void on_pushButton_repair_clicked();
    void on_pushButton_diag_clicked();
    void on_pushButton_verify_clicked();
    void on_pushButton_cancel_clicked();
    void on_pushButton_apply_clicked();
    void on_pushButton_ok_clicked();
    void on_interaction();

private:
    Ui::Form *ui;
    command_list *_commands;
    UpdateTool *_updater;

    void link_interactions();
    void closeEvent(QCloseEvent *event) override;
};


class command_set
{
public:
    command_set(QString& command, QCheckBox* check, QComboBox* combo = nullptr);
    ~command_set();

    QString print_command();
    bool import(QString argument);
    bool is_in_use();
    QString extract_value(QString argument);

private:
    QString _command;
    QCheckBox* _check;
    QComboBox* _combo;
};


class command_list
{
public:
    command_list(QLineEdit *custom_commands);
    ~command_list();

    void add(QString command, QCheckBox* check, QComboBox* combo = nullptr);
    void import_arguments(QString arguments);
    QString create_command_string();
    QLineEdit *_custom_commands;

private:
    std::vector<command_set*> _sets;
};


#endif // FORM_H
