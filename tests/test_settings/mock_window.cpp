#include "mock_window.h"

MockWindow::MockWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *centralWidget = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout();

    ptrLineEdit = new QLineEdit();
    mainLayout->addWidget(ptrLineEdit);
    ptrCheckBox = new QCheckBox();
    mainLayout->addWidget(ptrCheckBox);

    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);
}

MockWindow::~MockWindow() {}

void MockWindow::mousePressEvent(QMouseEvent *event)
{
    _lastPosition = event->globalPosition();
    emit onMousePressed(event);
}

QPointF MockWindow::getLastPosition() const
{
    return _lastPosition;
}
