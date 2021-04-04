#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include <QCloseEvent>

namespace Ui {
class Form;
}

class Form : public QWidget
{
    Q_OBJECT

public:
    explicit Form(QString arguments, QWidget *parent = nullptr);
    ~Form();

signals:
    void closed();

private:
    Ui::Form *ui;

    void closeEvent(QCloseEvent *event);
};

#endif // FORM_H
