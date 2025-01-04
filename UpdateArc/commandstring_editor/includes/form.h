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
class CommandSet;
class CommandList;


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
    void onInteraction();

    void on_pushButton_cancel_clicked();
    void on_pushButton_apply_clicked();
    void on_pushButton_ok_clicked();
    void on_pushButton_image_clicked();
    void on_pushButton_repair_clicked();
    void on_pushButton_diag_clicked();
    void on_pushButton_verify_clicked();

private:
    Ui::Form *_ui;
    CommandList *_commands;
    UpdateTool *_updater;

    void linkInteractions();
    void closeEvent(QCloseEvent *event) override;
};


class CommandSet
{
public:
    CommandSet(QString& command, QCheckBox* check, QComboBox* combo = nullptr);
    ~CommandSet();

    QString printCommand();
    bool import(QString argument);
    bool isInUse();
    QString extractValue(QString argument);

private:
    QString _command;
    QCheckBox* _check;
    QComboBox* _combo;
};


class CommandList
{
public:
    CommandList(QLineEdit *custom_commands);
    ~CommandList();

    void add(QString command, QCheckBox* check, QComboBox* combo = nullptr);
    void importArguments(QString arguments);
    QString createCommandString();
    QLineEdit *_custom_commands;

private:
    std::vector<CommandSet*> _sets;
};


#endif // FORM_H
