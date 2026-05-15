#ifndef TESTWINDOW_H
#define TESTWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QLineEdit>
#include <QMouseEvent>
#include <QPointF>

class MockWindow : public QMainWindow
{
    Q_OBJECT

signals:
    void onMousePressed(QMouseEvent *event);

public:
    MockWindow(QWidget *parent = nullptr);
    ~MockWindow() override;

    QPointF getLastPosition() const;

public:
    QLineEdit *ptrLineEdit;
    QCheckBox *ptrCheckBox;


private:
    void mousePressEvent(QMouseEvent *event) override;

private:
    QPointF _lastPosition;
};

#endif // TESTWINDOW_H
