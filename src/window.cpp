#include "window.h"
#include "ui_portalwindow.h"
#include "mainwindow.h"

Portal::Portal(MainWindow *mainWindow) : PortalUi(new Ui::PortalWindow), mainWindow(mainWindow) {
    setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint |
                   Qt::WindowMinimizeButtonHint);
    PortalUi->setupUi(this);
    connect(PortalUi->SetBlackPortal, &QPushButton::clicked, this, [=]() {
        int row_1 = PortalUi->BlackRow_1->value() - 1, column_1 = PortalUi->BlackColumn_1->value() - 1,
                row_2 = PortalUi->BlackRow_2->value() - 1, column_2 = PortalUi->BlackColumn_2->value() - 1;
        emit SetBlackPortal(row_1, column_1, row_2, column_2);
    });
    connect(PortalUi->SetWhitePortal, &QPushButton::clicked, this, [=]() {
        int row_1 = PortalUi->WhiteRow_1->value() - 1, column_1 = PortalUi->WhiteColumn_1->value() - 1,
                row_2 = PortalUi->WhiteRow_2->value() - 1, column_2 = PortalUi->WhiteColumn_2->value() - 1;
        emit SetWhitePortal(row_1, column_1, row_2, column_2);
    });
    connect(PortalUi->ActivePortal, &QCheckBox::toggled, this, &Portal::ActivePortal);
    connect(PortalUi->LockPortal, &QCheckBox::toggled, this, &Portal::LockPortal);
    connect(this, &Portal::SetBlackPortal, mainWindow, &MainWindow::SetBlackPortal);
    connect(this, &Portal::SetWhitePortal, mainWindow, &MainWindow::SetWhitePortal);
    connect(this, &Portal::ActivePortal, mainWindow, &MainWindow::ActivePortal);
    connect(this, &Portal::LockPortal, mainWindow, &MainWindow::LockPortal);
}

void Portal::closeEvent(QCloseEvent *event) {
    hide();
    event->ignore();
}

void Portal::RestoreChanges() {
    if (PortalUi->ActivePortal->isChecked())
        PortalUi->ActivePortal->toggle();
    if (PortalUi->LockPortal->isChecked())
        PortalUi->LockPortal->toggle();
}