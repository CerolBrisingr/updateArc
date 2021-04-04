#include "form.h"
#include "ui_form.h"
#include <iostream>

Form::Form(QString arguments, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);

    std::cout << arguments.toStdString() << std::endl;
}

void Form::closeEvent(QCloseEvent *event) {
    emit(closed());
    event->accept();
}

Form::~Form()
{
    delete ui;
}
