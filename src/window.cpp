#include "window.h"
#include "ui_portalwindow.h"
#include "ui_targetmapwindow.h"
#include "mainwindow.h"
#include "list.h"
#include <array>

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

Portal::~Portal() {
    delete PortalUi;
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

TargetMap::TargetMap(MainWindow *mainWindow) : TargetMapUi(new Ui::TargetMapWindow), mainWindow(mainWindow) {
    setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint |
                   Qt::WindowMinimizeButtonHint);
    TargetMapUi->setupUi(this);
    auto targetPlants = this->findChildren<QComboBox *>(QRegularExpression("TargetPlant_.*"));
    List list;
    for (auto targetPlant:targetPlants) {
        targetPlant->addItem("");
        targetPlant->addItems(list.PlantsList);
    }
    
    connect(TargetMapUi->GetTargetMap, &QPushButton::clicked, this, [=]() {
        int level = 0;
        if (TargetMapUi->SeeingStars->isChecked())
            level = 22;
        else if (TargetMapUi->ArtWallnut->isChecked())
            level = 36;
        else if (TargetMapUi->ArtSunflower->isChecked())
            level = 40;
        emit GetTargetMap(level);
    });
    connect(TargetMapUi->ClearTargetMap, &QPushButton::clicked, this, [=]() {
        auto targetPlants = this->findChildren<QComboBox *>(QRegularExpression("TargetPlant_.*"));
        for (auto targetPlant:targetPlants)
            targetPlant->setCurrentIndex(0);
    });
    connect(TargetMapUi->SetTargetMap, &QPushButton::clicked, this, [=]() {
        std::array<int, 54> targetMap{};
        auto targetPlants = this->findChildren<QComboBox *>(QRegularExpression("TargetPlant_.*"));
        for (auto targetPlant:targetPlants) {
            int id = targetPlant->objectName().remove("TargetPlant_").toInt();
            targetMap[id] = targetPlant->currentIndex() - 1;
        }
        int level = 0;
        if (TargetMapUi->SeeingStars->isChecked())
            level = 22;
        else if (TargetMapUi->ArtWallnut->isChecked())
            level = 36;
        else if (TargetMapUi->ArtSunflower->isChecked())
            level = 40;
        emit SetTargetMap(level, targetMap);
    });
    connect(this, &TargetMap::GetTargetMap, mainWindow, &MainWindow::GetTargetMap);
    connect(this, &TargetMap::SetTargetMap, mainWindow, &MainWindow::SetTargetMap);
}

TargetMap::~TargetMap() {
    delete TargetMapUi;
}

void TargetMap::ShowTargetMap(const std::array<int, 54> &targetMap) {
    auto targetPlants = this->findChildren<QComboBox *>(QRegularExpression("TargetPlant_.*"));
    for (auto targetPlant:targetPlants) {
        int id = targetPlant->objectName().remove("TargetPlant_").toInt();
        targetPlant->setCurrentIndex(targetMap[id] + 1);
    }
}

void TargetMap::closeEvent(QCloseEvent *event) {
    hide();
    event->ignore();
}
