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

Form::~Form()
{
    delete ui;
}
