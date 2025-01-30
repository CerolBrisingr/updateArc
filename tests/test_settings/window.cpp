#include "window.h"

MainWindow::MainWindow(QWidget *parent)
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

MainWindow::~MainWindow() {}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    _lastPosition = event->globalPosition();
    emit onMousePressed(event);
}

QPointF MainWindow::getLastPosition() const
{
    return _lastPosition;
}
