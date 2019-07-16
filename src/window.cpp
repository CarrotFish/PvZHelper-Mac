#include "window.h"
#include "ui_portalwindow.h"
#include "ui_targetmapwindow.h"
#include "mainwindow.h"
#include "list.h"
#include <array>

Portal::Portal(MainWindow *mainWindow) : QWidget(mainWindow), ui(new Ui::PortalWindow), mainWindow(mainWindow)
{
    setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
    ui->setupUi(this);
    
    connect(ui->SetBlackPortal, &QPushButton::clicked, this, [=]() {
        int row_1 = ui->BlackRow_1->value() - 1, column_1 = ui->BlackColumn_1->value() - 1,
                row_2 = ui->BlackRow_2->value() - 1, column_2 = ui->BlackColumn_2->value() - 1;
        emit SetBlackPortal(row_1, column_1, row_2, column_2);
    });
    connect(ui->SetWhitePortal, &QPushButton::clicked, this, [=]() {
        int row_1 = ui->WhiteRow_1->value() - 1, column_1 = ui->WhiteColumn_1->value() - 1,
                row_2 = ui->WhiteRow_2->value() - 1, column_2 = ui->WhiteColumn_2->value() - 1;
        emit SetWhitePortal(row_1, column_1, row_2, column_2);
    });
    connect(ui->ActivePortal, &QCheckBox::toggled, this, &Portal::ActivePortal);
    connect(ui->LockPortal, &QCheckBox::toggled, this, &Portal::LockPortal);
    connect(this, &Portal::SetBlackPortal, mainWindow, &MainWindow::SetBlackPortal);
    connect(this, &Portal::SetWhitePortal, mainWindow, &MainWindow::SetWhitePortal);
    connect(this, &Portal::ActivePortal, mainWindow, &MainWindow::ActivePortal);
    connect(this, &Portal::LockPortal, mainWindow, &MainWindow::LockPortal);
}

Portal::~Portal()
{
    delete ui;
}

void Portal::RestoreChanges()
{
    if (ui->ActivePortal->isChecked())
        ui->ActivePortal->toggle();
    if (ui->LockPortal->isChecked())
        ui->LockPortal->toggle();
}

TargetMap::TargetMap(MainWindow *mainWindow) : QWidget(mainWindow), ui(new Ui::TargetMapWindow), mainWindow(mainWindow)
{
    setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
    ui->setupUi(this);
    auto targetPlants = this->findChildren<QComboBox *>(QRegularExpression("TargetPlant_.*"));
    List list;
    for (auto targetPlant:targetPlants) {
        targetPlant->addItem("");
        targetPlant->addItems(list.PlantsList);
    }
    
    connect(ui->GetTargetMap, &QPushButton::clicked, this, [=]() {
        int level = 0;
        if (ui->SeeingStars->isChecked())
            level = 22;
        else if (ui->ArtWallnut->isChecked())
            level = 36;
        else if (ui->ArtSunflower->isChecked())
            level = 40;
        emit GetTargetMap(level);
    });
    connect(ui->ClearTargetMap, &QPushButton::clicked, this, [=]() {
        auto targetPlants = this->findChildren<QComboBox *>(QRegularExpression("TargetPlant_.*"));
        for (auto targetPlant:targetPlants)
            targetPlant->setCurrentIndex(0);
    });
    connect(ui->SetTargetMap, &QPushButton::clicked, this, [=]() {
        std::array<int, 54> targetMap{};
        auto targetPlants = this->findChildren<QComboBox *>(QRegularExpression("TargetPlant_.*"));
        for (auto targetPlant:targetPlants) {
            int id = targetPlant->objectName().remove("TargetPlant_").toInt();
            targetMap[id] = targetPlant->currentIndex() - 1;
        }
        int level = 0;
        if (ui->SeeingStars->isChecked())
            level = 22;
        else if (ui->ArtWallnut->isChecked())
            level = 36;
        else if (ui->ArtSunflower->isChecked())
            level = 40;
        emit SetTargetMap(level, targetMap);
    });
    connect(this, &TargetMap::GetTargetMap, mainWindow, &MainWindow::GetTargetMap);
    connect(this, &TargetMap::SetTargetMap, mainWindow, &MainWindow::SetTargetMap);
}

TargetMap::~TargetMap()
{
    delete ui;
}

void TargetMap::ShowTargetMap(const std::array<int, 54> &targetMap)
{
    auto targetPlants = this->findChildren<QComboBox *>(QRegularExpression("TargetPlant_.*"));
    for (auto targetPlant:targetPlants) {
        int id = targetPlant->objectName().remove("TargetPlant_").toInt();
        targetPlant->setCurrentIndex(targetMap[id] + 1);
    }
}
