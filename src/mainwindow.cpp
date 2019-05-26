#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "list.h"
#include "pvz.h"
#include "portal.h"
#include <QMessageBox>
#include <QHeaderView>
#include <QTimer>
#include <QMenuBar>
#include <QDesktopServices>
#include <QFileDialog>
#include <QDateTime>
#include <QStandardPaths>
#include <QDebug>

#define APP_VER "1.3.1"

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow), SpawnTable(new QTableWidget(this)), PortalWindow(new Portal(this)),
        pvz(new PvZ(ui, this)),
        list(new List) {
    SpawnTable->hide();
    PortalWindow->hide();
    ui->setupUi(this);
    setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint |
                   Qt::WindowMinimizeButtonHint);
    setFixedSize(width(), height());
    
    ConnectWidgets();
    ConnectSlots();
    
    emit FindGameProc();
}

void MainWindow::ConnectWidgets() {
    auto MenuBar = menuBar();
    auto helpMenu = MenuBar->addMenu("帮助");
    auto showHelp = helpMenu->addAction("使用说明");
    auto CheckUpdate = helpMenu->addAction("检查更新");
    helpMenu->addSeparator();
    auto showAbout = helpMenu->addAction("关于");
    auto showAboutQt = helpMenu->addAction("关于Qt");
    connect(showHelp, &QAction::triggered, this, &MainWindow::ShowHelpWindow);
    connect(showAbout, &QAction::triggered, this, &MainWindow::ShowAboutWindow);
    connect(CheckUpdate, &QAction::triggered, this, [=]() {
        QString url = "https://github.com/CarrotFish/PvZHelper-Mac/releases";
        QDesktopServices::openUrl(QUrl(url));
    });
    connect(showAboutQt, &QAction::triggered, this, &MainWindow::ShowAboutQtWindow);
    ui->label_35->setText(QStringLiteral("PvZ Helper v") + APP_VER);
    
    ui->PlantType->addItems(list->PlantsList);
    ui->ZombieType->addItems(list->ZombiesList);
    ui->SlotContent->addItems(list->CardList);
    ui->CardID->addItems(list->CardList);
    ui->ResourceType->addItems(list->ResourceList);
    ui->ResourceType->setCurrentIndex(1);
    ui->GameScene->addItems(list->MapList);
    ui->PlantHPType->addItems(list->PlantHPList);
    ui->PlantAttackIntervalType->addItems(list->PlantsList);
    ui->ProjectileType->addItems(list->ProjectileList);
    ui->ZombieHPType->addItems(list->ZombieHPList);
    ui->SpawnType->addItems(list->ZombiesList);
    ui->tabWidget->removeTab(10);
    QRegExp regExp("[a-fA-F0-9]{8}");
    ui->SpawnSeed->setValidator(new QRegExpValidator(regExp, this));
    //Page 0
    connect(ui->FindGameProc, &QPushButton::clicked, this, &MainWindow::FindGameProc);
    connect(ui->AutoRefreshGameStatus, &QCheckBox::toggled, this, &MainWindow::AutoRefreshGameStatus);
    connect(ui->MaintainCheckedItem, &QPushButton::clicked, this, &MainWindow::MaintainCheckedItem);
    //Page 1
    connect(ui->ModifySun, &QPushButton::clicked, this, [=]() {
        int sun = ui->Sun->text().toInt();
        emit ModifySun(sun);
    });
    connect(ui->ModifyMoney, &QPushButton::clicked, this, [=]() {
        int money = ui->Money->text().toInt();
        emit ModifyMoney(money);
    });
    connect(ui->LockSun, &QCheckBox::toggled, this, [=](bool on) {
        ui->ModifySun->clicked();
        emit LockSun(on);
    });
    connect(ui->LockMoney, &QCheckBox::toggled, this, [=](bool on) {
        ui->ModifySun->clicked();
        emit LockMoney(on);
    });
    connect(ui->ModifySunLimit, &QPushButton::clicked, this, [=]() {
        int sun_limit = ui->SunLimit->text().toInt();
        emit ModifySunLimit(sun_limit);
    });
    connect(ui->ModifyMoneyLimit, &QPushButton::clicked, this, [=]() {
        int money_limit = ui->MoneyLimit->text().toInt();
        emit ModifyMoneyLimit(money_limit);
    });
    connect(ui->GetSliverSunflower, &QPushButton::clicked, this, &MainWindow::GetSliverSunflower);
    connect(ui->GetGoldenSunflower, &QPushButton::clicked, this, &MainWindow::GetGoldenSunflower);
    connect(ui->GetAllItems, &QPushButton::clicked, this, &MainWindow::GetAllItems);
    connect(ui->FinishAchievements, &QPushButton::clicked, this, &MainWindow::FinishAchievements);
    connect(ui->ModifyResourceValue, &QPushButton::clicked, this, [=]() {
        int type = ui->ResourceType->currentIndex(), value = ui->ResourceValue->text().toInt();
        emit ModifyResourceValue(type, value);
    });
    connect(ui->ResourceType, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
            &MainWindow::GetResourceValue);
    connect(ui->AutoCollect, &QCheckBox::toggled, this, &MainWindow::AutoCollect);
    connect(ui->NoMoneyDrops, &QCheckBox::toggled, this, &MainWindow::NoMoneyDrops);
    connect(ui->NoSunFalling, &QCheckBox::toggled, this, [=](bool on) {
        if (on && ui->KeepSunFalling->isChecked())
            ui->KeepSunFalling->toggle();
        emit NoSunFalling(on);
    });
    connect(ui->KeepSunFalling, &QCheckBox::toggled, this, [=](bool on) {
        if (on && ui->NoSunFalling->isChecked())
            ui->NoSunFalling->toggle();
        emit KeepSunFalling(on);
    });
    //Page 2
    connect(ui->ModifyMode, &QPushButton::clicked, this, [=]() {
        int mode = ui->Mode->text().toInt();
        emit ModifyMode(mode);
    });
    connect(ui->StartLevel, &QPushButton::clicked, this, [=]() {
        int mode = ui->Mode->text().toInt();
        emit StartLevel(mode);
    });
    connect(ui->ModifyEndlessLevel, &QPushButton::clicked, this, [=]() {
        int level = ui->EndlessLevel->text().toInt();
        emit ModifyEndlessLevel(level);
    });
    connect(ui->ModifyAdventureLevel, &QPushButton::clicked, this, [=]() {
        int level = ui->AdventureLevel->text().toInt();
        emit ModifyAdventureLevel(level);
    });
    connect(ui->ModifyAdventureCompletionTimes, &QPushButton::clicked, this, [=]() {
        int times = ui->ModifyAdventureCompletionTimes->text().toInt();
        emit ModifyAdventureCompletionTimes(times);
    });
    connect(ui->ShowLimboPage, &QCheckBox::toggled, this, &MainWindow::ShowLimboPage);
    connect(ui->UnlockAllModes, &QCheckBox::toggled, this, &MainWindow::UnlockAllModes);
    connect(ui->ImmediatelyWin, &QPushButton::clicked, this, &MainWindow::ImmediatelyWin);
    connect(ui->ItsRaining, &QCheckBox::toggled, this, &MainWindow::ItsRaining);
    connect(ui->ZombieQuick, &QCheckBox::toggled, this, &MainWindow::ZombieQuick);
    connect(ui->BungeeBlitz, &QCheckBox::toggled, this, &MainWindow::BungeeBlitz);
    connect(ui->HighGravity, &QCheckBox::toggled, this, &MainWindow::HighGravity);
    connect(ui->GraveDanger, &QCheckBox::toggled, this, &MainWindow::GraveDanger);
    connect(ui->SunnyDay, &QCheckBox::toggled, this, &MainWindow::SunnyDay);
    connect(ui->DarkStormyNight, &QCheckBox::toggled, this, &MainWindow::DarkStormyNight);
    connect(ui->BigTime, &QCheckBox::toggled, this, &MainWindow::BigTime);
    connect(ui->ColumnLike, &QCheckBox::toggled, this, &MainWindow::ColumnLike);
    //Page 3
    connect(ui->ModifySlotCount, &QPushButton::clicked, this, [=]() {
        int count = ui->SlotCount->text().toInt();
        emit ModifySlotCount(count);
    });
    connect(ui->SlotVisible, &QCheckBox::toggled, this, &MainWindow::SlotVisible);
    connect(ui->ShowShovel, &QCheckBox::toggled, this, &MainWindow::ShowShovel);
    connect(ui->ModifySlotContent, &QPushButton::clicked, this, [=]() {
        int SlotID = ui->SlotNumber->currentIndex(), type = ui->SlotContent->currentIndex();
        bool isImiater = ui->IsImitater->isChecked(), visible = ui->SeedVisible->isChecked();
        emit ModifySlotContent(SlotID, type, isImiater, visible);
    });
    connect(ui->SlotContent, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
            [=](int type) {
                if (type > 52) {
                    ui->IsImitater->setChecked(false);
                    ui->IsImitater->setDisabled(true);
                } else
                    ui->IsImitater->setDisabled(false);
            });
    connect(ui->NoCoolDown, &QCheckBox::toggled, this, &MainWindow::NoCoolDown);
    connect(ui->FreelyPlant, &QCheckBox::toggled, this, &MainWindow::FreelyPlant);
    connect(ui->PurplePlantAvailable, &QCheckBox::toggled, this, &MainWindow::PurplePlantAvailable);
    connect(ui->AlwaysShovel, &QCheckBox::toggled, this, &MainWindow::AlwaysShovel);
    connect(ui->HideMenu, &QCheckBox::toggled, this, &MainWindow::HideMenu);
    connect(ui->ModifyCardProperty, &QPushButton::clicked, this, [=]() {
        int CardID = ui->CardID->currentIndex();
        int cost = ui->CardCost->text().toInt(), cooldowntime = ui->CardCooldownTime->text().toInt();
        emit ModifyCardProperty(CardID, cost, cooldowntime);
    });
    connect(ui->CardID, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
            &MainWindow::GetCardProperty);
    //Page 4
    connect(ui->ModifyGameScene, &QPushButton::clicked, this, [=]() {
        int scene = ui->GameScene->currentIndex();
        emit ModifyGameScene(scene);
    });
    connect(ui->ModifyGridType, &QPushButton::clicked, this, [=]() {
        int row = ui->Row->text().toInt() - 1, column = ui->Column->text().toInt() - 1, type =
                ui->GridType->currentIndex() + 1;
        emit ModifyGridType(row, column, type);
    });
    connect(ui->ModifyRowType, &QPushButton::clicked, this, [=]() {
        int row = ui->Row->text().toInt() - 1, type = ui->RowType->currentIndex();
        emit ModifyRowType(row, type);
    });
    connect(ui->PutItem, &QPushButton::clicked, this, [=]() {
        int type = ui->ItemType->currentIndex(), row = ui->Row->text().toInt() - 1, column =
                ui->Column->text().toInt() - 1;
        switch (type) {
            case 0:
                emit PutLadder(row, column);
                break;
            case 1:
                emit PutGrave(row, column);
                break;
            case 2:
                emit PutRake(row, column);
                break;
            default:
                emit PutCoin(type - 2, row, column);
                break;
        }
    });
    connect(ui->PumpkinLadder, &QPushButton::clicked, this, [=]() {
        bool imitater_only = ui->ImitaterOnly->isChecked();
        emit PumpkinLadder(imitater_only);
    });
    connect(ui->SetPlant, &QPushButton::clicked, this, [=]() {
        int row = ui->Row->text().toInt() - 1, column = ui->Column->text().toInt() - 1,
                type = ui->PlantType->currentIndex();
        bool isImitater = ui->IsImitater_2->isChecked();
        emit SetPlant(row, column, type, isImitater);
    });
    connect(ui->SetZombie, &QPushButton::clicked, this, [=]() {
        int row = ui->Row->text().toInt() - 1, column = ui->Column->text().toInt() - 1,
                type = ui->ZombieType->currentIndex();
        emit SetZombie(row, column, type);
    });
    connect(ui->SpawnZombie, &QPushButton::clicked, this, [=]() {
        int type = ui->ZombieType->currentIndex(), count = ui->ZombieCount->text().toInt();
        emit SpawnZombie(type, count);
    });
    connect(ui->NoFog, &QCheckBox::toggled, this, [=](bool on) {
        if (on && ui->BigFog->isChecked())
            ui->BigFog->toggle();
        emit NoFog(on);
    });
    connect(ui->BigFog, &QCheckBox::toggled, this, [=](bool on) {
        if (on && ui->NoFog->isChecked())
            ui->NoFog->toggle();
        emit BigFog(on);
    });
    connect(ui->SeeVase, &QCheckBox::toggled, this, &MainWindow::SeeVase);
    connect(ui->LawnMowerStart, &QPushButton::clicked, this, &MainWindow::LawnMowersStart);
    connect(ui->LawnMowerReset, &QPushButton::clicked, this, &MainWindow::LawnMowersReset);
    connect(ui->LawnMowerDisappear, &QPushButton::clicked, this, &MainWindow::LawnMowersDisappear);
    connect(ui->ShowPortal, &QPushButton::clicked, this, [=]() {
        PortalWindow->show();
    });
    connect(ui->ClearAll, &QPushButton::clicked, this, [=]() {
        int type = ui->ObjectType->currentIndex();
        switch (type) {
            case 0:
                emit ClearAllPlants();
                break;
            case 1:
                emit ClearAllZombies();
                break;
            case 2:
                emit ClearAllItems();
                break;
            case 3:
                emit ClearAllGridItems(1);//Grave
                break;
            case 4:
                emit ClearAllGridItems(2);//Crater
                break;
            case 5:
                emit ClearAllGridItems(3);//Ladder
                break;
            case 6:
                emit ClearAllGridItems(4);//White Portal
                break;
            case 7:
                emit ClearAllGridItems(5);//Black Portal
                break;
            case 8:
                emit ClearAllGridItems(7);//Vase
                break;
            case 9:
                emit ClearAllGridItems(11);//Rake
                break;
            case 10:
                emit ClearAllGridItems(12);//Brain
                break;
            default:
                break;
        }
    });
    //Page 5
    connect(ui->ModifyPlantHP, &QPushButton::clicked, this, [=]() {
        int type = ui->PlantHPType->currentIndex(), value = ui->PlantHP->text().toInt();
        emit ModifyPlantHP(type, value);
    });
    connect(ui->PlantHPType, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
            &MainWindow::GetPlantHP);
    connect(ui->ModifyPlantAttackInterval, &QPushButton::clicked, this, [=]() {
        int type = ui->PlantAttackIntervalType->currentIndex(), value = ui->PlantAttackInterval->text().toInt();
        emit ModifyPlantAttackInterval(type, value);
    });
    connect(ui->PlantAttackIntervalType, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
            &MainWindow::GetPlantAttackInterval);
    connect(ui->ModifyProjectileDamage, &QPushButton::clicked, this, [=]() {
        int type = ui->ProjectileType->currentIndex(), damage = ui->ProjectileDamage->text().toInt();
        emit ModifyProjectileDamage(type, damage);
    });
    connect(ui->ProjectileType, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
            &MainWindow::GetProjectileDamage);
    connect(ui->PlantInvincible, &QCheckBox::toggled, this, [=](bool on) {
        if (on && ui->PlantWeak->isChecked())
            ui->PlantWeak->toggle();
        emit PlantInvincible(on);
    });
    connect(ui->PlantWeak, &QCheckBox::toggled, this, [=](bool on) {
        if (on && ui->PlantInvincible->isChecked())
            ui->PlantInvincible->toggle();
        emit PlantWeak(on);
    });
    connect(ui->ChomperNoCD, &QCheckBox::toggled, this, &MainWindow::ChomperNoCD);
    connect(ui->PotatoMineNoCD, &QCheckBox::toggled, this, &MainWindow::PotatoMineNoCD);
    connect(ui->BombNoExplode, &QCheckBox::toggled, this, &MainWindow::BombNoExplode);
    connect(ui->BombInstantlyExplode, &QCheckBox::toggled, this, &MainWindow::BombInstantlyExplode);
    connect(ui->CobCannonNoCD, &QCheckBox::toggled, this, &MainWindow::CobCannonNoCD);
    connect(ui->MagnetShroomNoCD, &QCheckBox::toggled, this, &MainWindow::MagnetShroomNoCD);
    connect(ui->NoCrater, &QCheckBox::toggled, this, &MainWindow::NoCrater);
    connect(ui->PlantBoard, &QCheckBox::toggled, this, &MainWindow::PlantBoard);
    connect(ui->MushroomAwake, &QCheckBox::toggled, this, &MainWindow::MushroomAwake);
    connect(ui->SunShroomGrow, &QCheckBox::toggled, this, &MainWindow::SunShroomGrow);
    connect(ui->AlwaysKernal, &QCheckBox::toggled, this, [=](bool on) {
        if (on && ui->AlwaysButter->isChecked())
            ui->AlwaysButter->toggle();
        emit AlwaysKernal(on);
    });
    connect(ui->AlwaysButter, &QCheckBox::toggled, this, [=](bool on) {
        if (on && ui->AlwaysKernal->isChecked())
            ui->AlwaysKernal->toggle();
        emit AlwaysButter(on);
    });
    connect(ui->StrongBlover, &QCheckBox::toggled, this, &MainWindow::StrongBlover);
    connect(ui->BeghouledFreelyMove, &QCheckBox::toggled, this, &MainWindow::BeghouledFreelyMove);
    //Page 6
    connect(ui->ModifyZombieHP, &QPushButton::clicked, this, [=]() {
        int type = ui->ZombieHPType->currentIndex(), value = ui->ZombieHP->text().toInt();
        emit ModifyZombieHP(type, value);
    });
    connect(ui->ZombieHPType, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
            &MainWindow::GetZombieHP);
    connect(ui->ZombieInvincible, &QCheckBox::toggled, this, [=](bool on) {
        if (on && ui->ZombieWeak->isChecked())
            ui->ZombieWeak->toggle();
        emit ZombieInvincible(on);
    });
    connect(ui->ZombieWeak, &QCheckBox::toggled, this, [=](bool on) {
        if (on && ui->ZombieInvincible->isChecked())
            ui->ZombieInvincible->toggle();
        emit ZombieWeak(on);
    });
    connect(ui->JackNoExplode, &QCheckBox::toggled, this, [=](bool on) {
        if (on && ui->JackImmediatelyExplode->isChecked())
            ui->JackImmediatelyExplode->toggle();
        emit JackNoExplode(on);
    });
    connect(ui->JackImmediatelyExplode, &QCheckBox::toggled, this, [=](bool on) {
        if (on && ui->JackNoExplode->isChecked())
            ui->JackNoExplode->toggle();
        emit JackImmediatelyExplode(on);
    });
    connect(ui->NoImpThrow, &QCheckBox::toggled, this, [=](bool on) {
        if (on && ui->InfiniteImpThrow->isChecked())
            ui->InfiniteImpThrow->toggle();
        emit NoImpThrow(on);
    });
    connect(ui->InfiniteImpThrow, &QCheckBox::toggled, this, [=](bool on) {
        if (on && ui->NoImpThrow->isChecked())
            ui->NoImpThrow->toggle();
        emit InfiniteImpThrow(on);
    });
    // connect(ui->NoBasketball, &QCheckBox::toggled, this, [=](bool on) {
    //     if (on && ui->InfiniteBasketball->isChecked())
    //         ui->InfiniteBasketball->toggle();
    //     emit NoBasketball(on);
    // });
    connect(ui->StandingStill, &QCheckBox::toggled, this, &MainWindow::StandingStill);
    // connect(ui->InfiniteBasketball, &QCheckBox::toggled, this, [=](bool on) {
    //     if (on && ui->NoBasketball->isChecked())
    //         ui->NoBasketball->toggle();
    //     emit InfiniteBasketball(on);
    // });
    connect(ui->InfiniteBasketball, &QCheckBox::toggled, this, &MainWindow::InfiniteBasketball);
    connect(ui->NoZombieSpawn, &QCheckBox::toggled, this, [=](bool on) {
        if (on && ui->AllZombieSpawn->isChecked())
            ui->AllZombieSpawn->toggle();
        emit NoZombieSpawn(on);
    });
    connect(ui->AllZombieSpawn, &QCheckBox::toggled, this, [=](bool on) {
        if (on && ui->NoZombieSpawn->isChecked())
            ui->NoZombieSpawn->toggle();
        emit AllZombieSpawn(on);
    });
    connect(ui->NoThreesome, &QCheckBox::toggled, this, &MainWindow::NoThreesome);
    connect(ui->InfinitePole, &QCheckBox::toggled, this, &MainWindow::InfinitePole);
    connect(ui->ZombieInvisible, &QCheckBox::toggled, this, [=](bool on) {
        if (on && ui->ZombieVisible->isChecked())
            ui->ZombieVisible->toggle();
        emit ZombieInvisible(on);
    });
    connect(ui->ZombieVisible, &QCheckBox::toggled, this, [=](bool on) {
        if (on && ui->ZombieInvisible->isChecked())
            ui->ZombieInvisible->toggle();
        emit ZombieVisible(on);
    });
    connect(ui->ButterImmune, &QCheckBox::toggled, this, &MainWindow::ButterImmune);
    connect(ui->IceImmune, &QCheckBox::toggled, this, &MainWindow::IceImmune);
    connect(ui->SlowdownImmune, &QCheckBox::toggled, this, &MainWindow::SlowdownImmune);
    connect(ui->NoIceTrail, &QCheckBox::toggled, this, &MainWindow::NoIceTrail);
    connect(ui->NoYetiEscape, &QCheckBox::toggled, this, &MainWindow::NoYetiEscape);
    connect(ui->NoEnterHouse, &QCheckBox::toggled, this, &MainWindow::NoEnterHouse);
    connect(ui->GatherZombies, &QCheckBox::toggled, this, [=](bool on) {
        float pos = ui->ZombiePosition->value();
        emit GatherZombies(on, pos);
    });
    connect(ui->ZombiePosition, &QScrollBar::valueChanged, this, [=](int value) {
        if (ui->GatherZombies->isChecked()) {
            emit GatherZombies(true, value);
        }
    });
    connect(ui->AllZombiesXXX, &QPushButton::clicked, this, [=]() {
        int status = ui->ZombieStatus->currentIndex();
        emit AllZombiesXXX(status);
    });
    connect(ui->SpawnNextWave, &QPushButton::clicked, this, &MainWindow::SpawnNextWave);
    //Page 7
    connect(ui->NaturalSpawn, &QPushButton::clicked, this, [=]() {
        std::array<bool, 33> zombies = GetZombies();
        emit NaturalSpawn(zombies);
    });
    connect(ui->ExtremeSpawn, &QPushButton::clicked, this, [=]() {
        std::array<bool, 33> zombies = GetZombies();
        std::array<bool, 20> giga_waves{};
        giga_waves.fill(true);
        bool limit_yeti = ui->LimitYeti->isChecked(), limit_bungee = ui->LimitBungee->isChecked(),
                limit_giga = ui->LimitGiga->isChecked();
        if (limit_giga) {
            for (int i = 10; i < 19; i++)
                giga_waves[i] = false;
        }
        emit ExtremeSpawn(zombies, true, limit_yeti, limit_bungee, limit_giga, giga_waves);
    });
    connect(ui->ShowCurrentSpawn, &QPushButton::clicked, this, &MainWindow::GetSpawnList);
    connect(ui->ShowCurrentSpawn, &QPushButton::clicked, SpawnTable, &QTableWidget::show);
    connect(ui->SwitchDetailedPage, &QPushButton::clicked, this, [=]() {
        std::array<bool, 33> zombies = GetZombies();
        ui->SpawnList->clear();
        for (int i = 0; i < 33; i++) {
            if (zombies[i])
                ui->SpawnList->addItem(list->ZombiesList.at(i));
        }
        ui->tabWidget->removeTab(7);
        ui->tabWidget->removeTab(8);
        ui->tabWidget->removeTab(9);
        ui->tabWidget->addTab(ui->tab_10, "出怪");
        ui->tabWidget->addTab(ui->tab_8, "花园");
        ui->tabWidget->addTab(ui->tab_9, "杂项");
        ui->tabWidget->setCurrentIndex(7);
    });
    //Page 7.5
    connect(ui->ShowSpawnHelp, &QPushButton::clicked, this, &MainWindow::ShowSpawnHelpWindow);
    connect(ui->AddSpawnType, &QPushButton::clicked, this, [=]() {
        std::array<bool, 33> zombies = {false};
        zombies[list->ZombiesList.indexOf(ui->SpawnType->currentText())] = true;
        for (int i = 0; i < ui->SpawnList->count(); i++)
            zombies[list->ZombiesList.indexOf(ui->SpawnList->item(i)->text())] = true;
        ui->SpawnList->clear();
        for (int i = 0; i < 33; i++) {
            if (zombies[i])
                ui->SpawnList->addItem(list->ZombiesList.at(i));
        }
    });
    connect(ui->RemoveSpawnType, &QPushButton::clicked, this, [=]() {
        delete ui->SpawnList->takeItem(ui->SpawnList->currentRow());
    });
    connect(ui->ClearSpawnList, &QPushButton::clicked, ui->SpawnList, &QListWidget::clear);
    connect(ui->NaturalSpawn_2, &QRadioButton::toggled, this, [=](bool on) {
        if (on) {
            ui->LimitFlag_2->setChecked(true);
            ui->LimitFlag_2->setDisabled(true);
            ui->LimitYeti_2->setChecked(true);
            ui->LimitYeti_2->setDisabled(true);
            ui->LimitBungee_2->setChecked(true);
            ui->LimitBungee_2->setDisabled(true);
            if (ui->LimitGiga_2->isChecked())
                ui->LimitGiga_2->toggle();
            ui->LimitGiga_2->setDisabled(true);
        }
    });
    connect(ui->SimulateSpawn_2, &QRadioButton::toggled, this, [=](bool on) {
        if (on) {
            ui->LimitFlag_2->setDisabled(false);
            ui->LimitYeti_2->setDisabled(false);
            ui->LimitBungee_2->setDisabled(false);
            ui->LimitGiga_2->setDisabled(false);
        }
    });
    connect(ui->ExtremeSpawn_2, &QRadioButton::toggled, this, [=](bool on) {
        if (on) {
            ui->LimitFlag_2->setDisabled(false);
            ui->LimitYeti_2->setDisabled(false);
            ui->LimitBungee_2->setDisabled(false);
            ui->LimitGiga_2->setDisabled(false);
        }
    });
    connect(ui->LimitGiga_2, &QRadioButton::toggled, this, [=](bool on) {
        if (on) {
            ui->GigaWave_0->setDisabled(false);
            ui->GigaWave_1->setDisabled(false);
            ui->GigaWave_2->setDisabled(false);
            ui->GigaWave_3->setDisabled(false);
            ui->GigaWave_4->setDisabled(false);
            ui->GigaWave_5->setDisabled(false);
            ui->GigaWave_6->setDisabled(false);
            ui->GigaWave_7->setDisabled(false);
            ui->GigaWave_8->setDisabled(false);
            ui->GigaWave_9->setDisabled(false);
            ui->GigaWave_10->setDisabled(false);
            ui->GigaWave_11->setDisabled(false);
            ui->GigaWave_12->setDisabled(false);
            ui->GigaWave_13->setDisabled(false);
            ui->GigaWave_14->setDisabled(false);
            ui->GigaWave_15->setDisabled(false);
            ui->GigaWave_16->setDisabled(false);
            ui->GigaWave_17->setDisabled(false);
            ui->GigaWave_18->setDisabled(false);
            ui->GigaWave_19->setDisabled(false);
        } else {
            ui->GigaWave_0->setDisabled(true);
            ui->GigaWave_1->setDisabled(true);
            ui->GigaWave_2->setDisabled(true);
            ui->GigaWave_3->setDisabled(true);
            ui->GigaWave_4->setDisabled(true);
            ui->GigaWave_5->setDisabled(true);
            ui->GigaWave_6->setDisabled(true);
            ui->GigaWave_7->setDisabled(true);
            ui->GigaWave_8->setDisabled(true);
            ui->GigaWave_9->setDisabled(true);
            ui->GigaWave_10->setDisabled(true);
            ui->GigaWave_11->setDisabled(true);
            ui->GigaWave_12->setDisabled(true);
            ui->GigaWave_13->setDisabled(true);
            ui->GigaWave_14->setDisabled(true);
            ui->GigaWave_15->setDisabled(true);
            ui->GigaWave_16->setDisabled(true);
            ui->GigaWave_17->setDisabled(true);
            ui->GigaWave_18->setDisabled(true);
            ui->GigaWave_19->setDisabled(true);
        }
    });
    connect(ui->SetSpawn, &QPushButton::clicked, this, [=]() {
        std::array<bool, 33> zombies = {false};
        std::array<bool, 20> giga_waves = GetGigaWaves();
        for (int i = 0; i < ui->SpawnList->count(); i++) {
            zombies[list->ZombiesList.indexOf(ui->SpawnList->item(i)->text())] = true;
        }
        if (ui->NaturalSpawn_2->isChecked()) {
            emit NaturalSpawn(zombies);
        } else {
            bool limit_flag = ui->LimitFlag_2->isChecked(), limit_yeti = ui->LimitYeti_2->isChecked(),
                    limit_bungee = ui->LimitBungee_2->isChecked(), limit_giga = ui->LimitGiga_2->isChecked();
            if (ui->SimulateSpawn_2->isChecked())
                    emit SimulateSpawn(zombies, limit_flag, limit_yeti, limit_bungee, limit_giga, giga_waves);
            else if (ui->ExtremeSpawn_2->isChecked())
                    emit ExtremeSpawn(zombies, limit_flag, limit_yeti, limit_bungee, limit_giga, giga_waves);
        }
    });
    connect(ui->RestoreSpawn, &QPushButton::clicked, this, &MainWindow::RestoreSpawn);
    connect(ui->ShowCurrentSpawn_2, &QPushButton::clicked, this, &MainWindow::GetSpawnList);
    connect(ui->ShowCurrentSpawn_2, &QPushButton::clicked, SpawnTable, &QTableWidget::show);
    connect(ui->SwitchBriefPage, &QPushButton::clicked, this, [=]() {
        std::array<bool, 33> zombies = {false};
        for (int i = 0; i < ui->SpawnList->count(); i++)
            zombies[list->ZombiesList.indexOf(ui->SpawnList->item(i)->text())] = true;
        ui->SpawnType_0->setChecked(zombies[2]);
        ui->SpawnType_1->setChecked(zombies[3]);
        ui->SpawnType_2->setChecked(zombies[4]);
        ui->SpawnType_3->setChecked(zombies[5]);
        ui->SpawnType_4->setChecked(zombies[6]);
        ui->SpawnType_5->setChecked(zombies[7]);
        ui->SpawnType_6->setChecked(zombies[8]);
        ui->SpawnType_7->setChecked(zombies[11]);
        ui->SpawnType_8->setChecked(zombies[12]);
        ui->SpawnType_9->setChecked(zombies[14]);
        ui->SpawnType_10->setChecked(zombies[15]);
        ui->SpawnType_11->setChecked(zombies[16]);
        ui->SpawnType_12->setChecked(zombies[17]);
        ui->SpawnType_13->setChecked(zombies[18]);
        ui->SpawnType_14->setChecked(zombies[19]);
        ui->SpawnType_15->setChecked(zombies[20]);
        ui->SpawnType_16->setChecked(zombies[21]);
        ui->SpawnType_17->setChecked(zombies[22]);
        ui->SpawnType_18->setChecked(zombies[23]);
        ui->SpawnType_19->setChecked(zombies[32]);
        ui->tabWidget->removeTab(7);
        ui->tabWidget->removeTab(8);
        ui->tabWidget->removeTab(9);
        ui->tabWidget->addTab(ui->tab_7, "出怪");
        ui->tabWidget->addTab(ui->tab_8, "花园");
        ui->tabWidget->addTab(ui->tab_9, "杂项");
        ui->tabWidget->setCurrentIndex(7);
    });
    //Page 8
    connect(ui->ModifyFertilizer, &QPushButton::clicked, this, [=]() {
        int value = ui->Fertilizer->text().toInt();
        emit ModifyFertilizer(value);
    });
    connect(ui->ModifyBugSpray, &QPushButton::clicked, this, [=]() {
        int value = ui->BugSpray->text().toInt();
        emit ModifyBugSpray(value);
    });
    connect(ui->ModifyChocolate, &QPushButton::clicked, this, [=]() {
        int value = ui->Chocolate->text().toInt();
        emit ModifyChocolate(value);
    });
    connect(ui->ModifyTreeFertilizer, &QPushButton::clicked, this, [=]() {
        int value = ui->TreeFertilizer->text().toInt();
        emit ModifyTreeFertilizer(value);
    });
    connect(ui->ModifyWisdomTreeHeight, &QPushButton::clicked, this, [=]() {
        int value = ui->WisdomTreeHeight->text().toInt();
        emit ModifyWisdomTreeHeight(value);
    });
    connect(ui->NoSnailSleep, &QCheckBox::toggled, this, &MainWindow::NoSnailSleep);
    connect(ui->MarigoldRefresh, &QPushButton::clicked, this, &MainWindow::MarigoldRefresh);
    connect(ui->GardenPlantRight, &QPushButton::clicked, this, &MainWindow::GardenPlantRight);
    connect(ui->GardenPlantLeft, &QPushButton::clicked, this, &MainWindow::GardenPlantLeft);
    //Page 9
    connect(ui->SetDebugMode, &QPushButton::clicked, this, [=]() {
        int mode = ui->DebugMode->currentIndex();
        emit SetDebugMode(mode);
    });
    connect(ui->OpenUserdata, &QPushButton::clicked, this, &MainWindow::OpenUserdata);
    connect(ui->SetSeed, &QPushButton::clicked, this, [=]() {
        uint32_t seed = ui->SpawnSeed->text().toUInt(nullptr, 16);
        emit SetSeed(seed);
    });
    connect(ui->GetSeed, &QPushButton::clicked, this, &MainWindow::GetSeed);
    connect(ui->GetRandomSeed, &QPushButton::clicked, this, &MainWindow::GetRandomSeed);
    connect(ui->BackgroundRunning, &QCheckBox::toggled, this, &MainWindow::BackgroundRunning);
    connect(ui->DisablePause, &QCheckBox::toggled, this, &MainWindow::DisablePause);
    connect(ui->NoDataDelete, &QPushButton::toggled, this, &MainWindow::NoDataDelete);
    connect(ui->NoDataSave, &QPushButton::toggled, this, &MainWindow::NoDataSave);
    connect(ui->SelectFile, &QPushButton::clicked, this, &MainWindow::SelectPAKFile);
    connect(ui->SelectFolder, &QPushButton::clicked, this, &MainWindow::SelectPAKFolder);
    connect(ui->ExtractPAK, &QPushButton::clicked, this, [=]() {
        QFileInfo PAKPath = ui->PakPath->text(), FolderPath = ui->FolderPath->text();
        if (!PAKPath.exists() || !PAKPath.isFile()) {
            QMessageBox Message(QMessageBox::Information, "", "文件不存在!", QMessageBox::Ok, nullptr,
                                Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
            // Message.button(QMessageBox::Ok)->setText("确定");
            // Message.button(QMessageBox::Ok)->resize(100,30);
            Message.exec();
            return;
        }
        if (FolderPath.isDir()) {
            QMessageBox Question(QMessageBox::Question, "", "目标文件夹已存在，是否继续？", QMessageBox::Yes | QMessageBox::No,
                                 nullptr, Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint |
                                          Qt::WindowCloseButtonHint);
            Question.setDefaultButton(QMessageBox::No);
            // Question.button(QMessageBox::Yes)->setText("是");
            // Question.button(QMessageBox::No)->setText("否");
            // Question.button(QMessageBox::Yes)->resize(100,30);
            // Question.button(QMessageBox::No)->resize(100,30);
            auto reply = Question.exec();
            if (reply == QMessageBox::No)
                return;
        }
        emit ExtractPAK(PAKPath.filePath(), FolderPath.filePath());
    });
    connect(ui->PackPAK, &QPushButton::clicked, this, [=]() {
        QFileInfo PAKPath = ui->PakPath->text(), FolderPath = ui->FolderPath->text();
        if (!FolderPath.exists() || !FolderPath.isDir()) {
            QMessageBox Message(QMessageBox::Information, "", "目录不存在!", QMessageBox::Ok, nullptr,
                                Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
            // Message.button(QMessageBox::Ok)->setText("确定");
            // Message.button(QMessageBox::Ok)->resize(100,30);
            Message.exec();
            return;
        }
        if (PAKPath.isFile()) {
            QMessageBox Question(QMessageBox::Question, "", "目标文件已存在，是否继续？", QMessageBox::Yes | QMessageBox::No,
                                 nullptr, Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint |
                                          Qt::WindowCloseButtonHint);
            Question.setDefaultButton(QMessageBox::No);
            // Question.button(QMessageBox::Yes)->setText("是");
            // Question.button(QMessageBox::No)->setText("否");
            // Question.button(QMessageBox::Yes)->resize(100,30);
            // Question.button(QMessageBox::No)->resize(100,30);
            auto reply = Question.exec();
            if (reply == QMessageBox::No)
                return;
        }
        emit PackPAK(FolderPath.filePath(), PAKPath.filePath());
    });
    connect(ui->SetMusic, &QPushButton::clicked, this, [=]() {
        int type = ui->MusicType->currentIndex() + 1;
        emit SetMusic(type);
    });
}

void MainWindow::ConnectSlots() const {
    connect(this, &MainWindow::FindGameProc, pvz, &PvZ::FindGameProc);
    connect(this, &MainWindow::GetGameStatus, pvz, &PvZ::GetGameStatus);
    
    connect(this, &MainWindow::ModifySun, pvz, &PvZ::ModifySun);
    connect(this, &MainWindow::ModifyMoney, pvz, &PvZ::ModifyMoney);
    connect(this, &MainWindow::LockSun, pvz, &PvZ::LockSun);
    connect(this, &MainWindow::LockMoney, pvz, &PvZ::LockMoney);
    connect(this, &MainWindow::ModifySunLimit, pvz, &PvZ::ModifySunLimit);
    connect(this, &MainWindow::ModifyMoneyLimit, pvz, &PvZ::ModifyMoneyLimit);
    connect(this, &MainWindow::GetSliverSunflower, pvz, &PvZ::GetSliverSunflower);
    connect(this, &MainWindow::GetGoldenSunflower, pvz, &PvZ::GetGoldenSunflower);
    connect(this, &MainWindow::GetAllItems, pvz, &PvZ::GetAllItems);
    connect(this, &MainWindow::FinishAchievements, pvz, &PvZ::FinishAchievements);
    connect(this, &MainWindow::ModifyResourceValue, pvz, &PvZ::ModifyResourceValue);
    connect(this, &MainWindow::GetResourceValue, pvz, &PvZ::GetResourceValue);
    connect(this, &MainWindow::AutoCollect, pvz, &PvZ::AutoCollect);
    connect(this, &MainWindow::NoMoneyDrops, pvz, &PvZ::NoMoneyDrops);
    connect(this, &MainWindow::NoSunFalling, pvz, &PvZ::NoSunFalling);
    connect(this, &MainWindow::KeepSunFalling, pvz, &PvZ::KeepSunFalling);
    
    connect(this, &MainWindow::ModifyMode, pvz, &PvZ::ModifyMode);
    connect(this, &MainWindow::StartLevel, pvz, &PvZ::StartLevel);
    connect(this, &MainWindow::ModifyEndlessLevel, pvz, &PvZ::ModifyEndlessLevel);
    connect(this, &MainWindow::ModifyAdventureLevel, pvz, &PvZ::ModifyAdventureLevel);
    connect(this, &MainWindow::ModifyAdventureCompletionTimes, pvz, &PvZ::ModifyAdventureCompletionTimes);
    connect(this, &MainWindow::ShowLimboPage, pvz, &PvZ::ShowLimboPage);
    connect(this, &MainWindow::UnlockAllModes, pvz, &PvZ::UnlockAllModes);
    connect(this, &MainWindow::ImmediatelyWin, pvz, &PvZ::ImmediatelyWin);
    connect(this, &MainWindow::ItsRaining, pvz, &PvZ::ItsRaining);
    connect(this, &MainWindow::ZombieQuick, pvz, &PvZ::ZombieQuick);
    connect(this, &MainWindow::BungeeBlitz, pvz, &PvZ::BungeeBlitz);
    connect(this, &MainWindow::HighGravity, pvz, &PvZ::HighGravity);
    connect(this, &MainWindow::GraveDanger, pvz, &PvZ::GraveDanger);
    connect(this, &MainWindow::SunnyDay, pvz, &PvZ::SunnyDay);
    connect(this, &MainWindow::DarkStormyNight, pvz, &PvZ::DarkStormyNight);
    connect(this, &MainWindow::BigTime, pvz, &PvZ::BigTime);
    connect(this, &MainWindow::ColumnLike, pvz, &PvZ::ColumnLike);
    
    connect(this, &MainWindow::ModifySlotCount, pvz, &PvZ::ModifySlotCount);
    connect(this, &MainWindow::SlotVisible, pvz, &PvZ::SlotVisible);
    connect(this, &MainWindow::ShowShovel, pvz, &PvZ::ShowShovel);
    connect(this, &MainWindow::ModifySlotContent, pvz, &PvZ::ModifySlotContent);
    connect(this, &MainWindow::NoCoolDown, pvz, &PvZ::NoCoolDown);
    connect(this, &MainWindow::FreelyPlant, pvz, &PvZ::FreelyPlant);
    connect(this, &MainWindow::PurplePlantAvailable, pvz, &PvZ::PurplePlantAvailable);
    connect(this, &MainWindow::AlwaysShovel, pvz, &PvZ::AlwaysShovel);
    connect(this, &MainWindow::HideMenu, pvz, &PvZ::HideMenu);
    connect(this, &MainWindow::ModifyCardProperty, pvz, &PvZ::ModifyCardProperty);
    connect(this, &MainWindow::GetCardProperty, pvz, &PvZ::GetCardProperty);
    connect(this, &MainWindow::ModifyGameScene, pvz, &PvZ::ModifyGameScene);
    connect(this, &MainWindow::ModifyGridType, pvz, &PvZ::ModifyGridType);
    connect(this, &MainWindow::ModifyRowType, pvz, &PvZ::ModifyRowType);
    connect(this, &MainWindow::PutLadder, pvz, &PvZ::PutLadder);
    connect(this, &MainWindow::PutGrave, pvz, &PvZ::PutGrave);
    connect(this, &MainWindow::PutRake, pvz, &PvZ::PutRake);
    connect(this, &MainWindow::PutCoin, pvz, &PvZ::PutCoin);
    connect(this, &MainWindow::PumpkinLadder, pvz, &PvZ::PumpkinLadder);
    connect(this, &MainWindow::SetPlant, pvz, &PvZ::SetPlant);
    connect(this, &MainWindow::SetZombie, pvz, &PvZ::SetZombie);
    connect(this, &MainWindow::SpawnZombie, pvz, &PvZ::SpawnZombie);
    connect(this, &MainWindow::NoFog, pvz, &PvZ::NoFog);
    connect(this, &MainWindow::BigFog, pvz, &PvZ::BigFog);
    connect(this, &MainWindow::SeeVase, pvz, &PvZ::SeeVase);
    connect(this, &MainWindow::LawnMowersStart, pvz, &PvZ::LawnMowersStart);
    connect(this, &MainWindow::LawnMowersReset, pvz, &PvZ::LawnMowersReset);
    connect(this, &MainWindow::LawnMowersDisappear, pvz, &PvZ::LawnMowersDisappear);
    connect(this, &MainWindow::SetBlackPortal, pvz, &PvZ::SetBlackPortal);
    connect(this, &MainWindow::SetWhitePortal, pvz, &PvZ::SetWhitePortal);
    connect(this, &MainWindow::ActivePortal, pvz, &PvZ::ActivePortal);
    connect(this, &MainWindow::LockPortal, pvz, &PvZ::LockPortal);
    connect(this, &MainWindow::ClearAllPlants, pvz, &PvZ::ClearAllPlants);
    connect(this, &MainWindow::ClearAllZombies, pvz, &PvZ::ClearAllZombies);
    connect(this, &MainWindow::ClearAllItems, pvz, &PvZ::ClearAllItems);
    connect(this, &MainWindow::ClearAllGridItems, pvz, &PvZ::ClearAllGridItems);
    
    connect(this, &MainWindow::ModifyPlantHP, pvz, &PvZ::ModifyPlantHP);
    connect(this, &MainWindow::GetPlantHP, pvz, &PvZ::GetPlantHP);
    connect(this, &MainWindow::ModifyPlantAttackInterval, pvz, &PvZ::ModifyPlantAttackInterval);
    connect(this, &MainWindow::GetPlantAttackInterval, pvz, &PvZ::GetPlantAttackInterval);
    connect(this, &MainWindow::ModifyProjectileDamage, pvz, &PvZ::ModifyProjectileDamage);
    connect(this, &MainWindow::GetProjectileDamage, pvz, &PvZ::GetProjectileDamage);
    connect(this, &MainWindow::PlantInvincible, pvz, &PvZ::PlantInvincible);
    connect(this, &MainWindow::PlantWeak, pvz, &PvZ::PlantWeak);
    connect(this, &MainWindow::ChomperNoCD, pvz, &PvZ::ChomperNoCD);
    connect(this, &MainWindow::PotatoMineNoCD, pvz, &PvZ::PotatoMineNoCD);
    connect(this, &MainWindow::BombNoExplode, pvz, &PvZ::BombNoExplode);
    connect(this, &MainWindow::BombInstantlyExplode, pvz, &PvZ::BombInstantlyExplode);
    connect(this, &MainWindow::CobCannonNoCD, pvz, &PvZ::CobCannonNoCD);
    connect(this, &MainWindow::MagnetShroomNoCD, pvz, &PvZ::MagnetShroomNoCD);
    connect(this, &MainWindow::NoCrater, pvz, &PvZ::NoCrater);
    connect(this, &MainWindow::PlantBoard, pvz, &PvZ::PlantBoard);
    connect(this, &MainWindow::MushroomAwake, pvz, &PvZ::MushroomAwake);
    connect(this, &MainWindow::SunShroomGrow, pvz, &PvZ::SunShroomGrow);
    connect(this, &MainWindow::AlwaysKernal, pvz, &PvZ::AlwaysKernal);
    connect(this, &MainWindow::AlwaysButter, pvz, &PvZ::AlwaysButter);
    connect(this, &MainWindow::StrongBlover, pvz, &PvZ::StrongBlover);
    connect(this, &MainWindow::BeghouledFreelyMove, pvz, &PvZ::BeghouledFreelyMove);
    
    connect(this, &MainWindow::ModifyZombieHP, pvz, &PvZ::ModifyZombieHP);
    connect(this, &MainWindow::GetZombieHP, pvz, &PvZ::GetZombieHP);
    connect(this, &MainWindow::ZombieInvincible, pvz, &PvZ::ZombieInvincible);
    connect(this, &MainWindow::ZombieWeak, pvz, &PvZ::ZombieWeak);
    connect(this, &MainWindow::JackNoExplode, pvz, &PvZ::JackNoExplode);
    connect(this, &MainWindow::JackImmediatelyExplode, pvz, &PvZ::JackImmediatelyExplode);
    connect(this, &MainWindow::NoImpThrow, pvz, &PvZ::NoImpThrow);
    connect(this, &MainWindow::InfiniteImpThrow, pvz, &PvZ::InfiniteImpThrow);
    connect(this, &MainWindow::NoBasketball, pvz, &PvZ::NoBasketball);
    connect(this, &MainWindow::StandingStill, pvz, &PvZ::StandingStill);
    connect(this, &MainWindow::InfiniteBasketball, pvz, &PvZ::InfiniteBasketball);
    connect(this, &MainWindow::NoZombieSpawn, pvz, &PvZ::NoZombieSpawn);
    connect(this, &MainWindow::AllZombieSpawn, pvz, &PvZ::AllZombieSpawn);
    connect(this, &MainWindow::NoThreesome, pvz, &PvZ::NoThreesome);
    connect(this, &MainWindow::InfinitePole, pvz, &PvZ::InfinitePole);
    connect(this, &MainWindow::ZombieInvisible, pvz, &PvZ::ZombieInvisible);
    connect(this, &MainWindow::ZombieVisible, pvz, &PvZ::ZombieVisible);
    connect(this, &MainWindow::ButterImmune, pvz, &PvZ::ButterImmune);
    connect(this, &MainWindow::IceImmune, pvz, &PvZ::IceImmune);
    connect(this, &MainWindow::SlowdownImmune, pvz, &PvZ::SlowdownImmune);
    connect(this, &MainWindow::NoIceTrail, pvz, &PvZ::NoIceTrail);
    connect(this, &MainWindow::NoYetiEscape, pvz, &PvZ::NoYetiEscape);
    connect(this, &MainWindow::NoEnterHouse, pvz, &PvZ::NoEnterHouse);
    connect(this, &MainWindow::GatherZombies, pvz, &PvZ::GatherZombies);
    connect(this, &MainWindow::AllZombiesXXX, pvz, &PvZ::AllZombiesXXX);
    connect(this, &MainWindow::SpawnNextWave, pvz, &PvZ::SpawnNextWave);
    
    connect(this, &MainWindow::NaturalSpawn, pvz, &PvZ::NaturalSpawn);
    connect(this, &MainWindow::SimulateSpawn, pvz, &PvZ::SimulateSpawn);
    connect(this, &MainWindow::ExtremeSpawn, pvz, &PvZ::ExtremeSpawn);
    connect(this, &MainWindow::RestoreSpawn, pvz, &PvZ::RestoreSpawn);
    connect(this, &MainWindow::GetSpawnList, pvz, &PvZ::GetSpawnList);
    
    connect(this, &MainWindow::ModifyFertilizer, pvz, &PvZ::ModifyFertilizer);
    connect(this, &MainWindow::ModifyBugSpray, pvz, &PvZ::ModifyBugSpray);
    connect(this, &MainWindow::ModifyChocolate, pvz, &PvZ::ModifyChocolate);
    connect(this, &MainWindow::ModifyTreeFertilizer, pvz, &PvZ::ModifyTreeFertilizer);
    connect(this, &MainWindow::ModifyWisdomTreeHeight, pvz, &PvZ::ModifyWisdomTreeHeight);
    connect(this, &MainWindow::NoSnailSleep, pvz, &PvZ::NoSnailSleep);
    connect(this, &MainWindow::MarigoldRefresh, pvz, &PvZ::MarigoldRefresh);
    connect(this, &MainWindow::GardenPlantRight, pvz, &PvZ::GardenPlantRight);
    connect(this, &MainWindow::GardenPlantLeft, pvz, &PvZ::GardenPlantLeft);
    
    connect(this, &MainWindow::SetDebugMode, pvz, &PvZ::SetDebugMode);
    connect(this, &MainWindow::OpenUserdata, pvz, &PvZ::GetUserdataFolder);
    connect(this, &MainWindow::SetSeed, pvz, &PvZ::SetSeed);
    connect(this, &MainWindow::GetSeed, pvz, &PvZ::GetSeed);
    connect(this, &MainWindow::GetRandomSeed, pvz, &PvZ::GetRandomSeed);
    connect(this, &MainWindow::BackgroundRunning, pvz, &PvZ::BackgroundRunning);
    connect(this, &MainWindow::DisablePause, pvz, &PvZ::DisablePause);
    connect(this, &MainWindow::NoDataDelete, pvz, &PvZ::NoDataDelete);
    connect(this, &MainWindow::NoDataSave, pvz, &PvZ::NoDataSave);
    connect(this, &MainWindow::PAKFilePath, this, &MainWindow::UpdatePAKPath);
    connect(this, &MainWindow::AutoPAKFilePath, this, [=](QString Path) {
        if (ui->PakPath->text().isEmpty())
            ui->PakPath->setText(Path);
    });
    connect(this, &MainWindow::PAKFolderPath, this, &MainWindow::UpdateFolderPath);
    connect(this, &MainWindow::AutoPAKFolderPath, this, [=](QString Path) {
        if (ui->FolderPath->text().isEmpty())
            ui->FolderPath->setText(Path);
    });
    connect(this, &MainWindow::SetMusic, pvz, &PvZ::SetMusic);
}

MainWindow::~MainWindow() {
    delete ui;
    delete pvz;
    delete list;
    delete SpawnTable;
}

void MainWindow::ShowHelpWindow() {
    auto HelpWindow = new QMessageBox(this);
    HelpWindow->setWindowTitle("使用说明");
    HelpWindow->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    QString Message = "<h3>使用说明</h3>"
                      "<p>此修改器仅适用于1.0.40版PvZ</p>"
                      "<p>打开修改器后会自动寻找游戏进程，也可点击\"状态->寻找游戏进程\"来手动寻找</p>"
                      "<p>修改器仅修改游戏内存，不会改变游戏程序，重启游戏即可恢复更改</p>"
                      "<p>修改器部分功能不太稳定，请谨慎使用，并注意备份存档</p>"
                      "<p>感谢您的使用！</font></p>";
    HelpWindow->setText(Message);
    HelpWindow->setTextFormat(Qt::TextFormat::RichText);
    HelpWindow->setTextInteractionFlags(Qt::TextInteractionFlag::NoTextInteraction);
    HelpWindow->addButton(QMessageBox::Ok);
    // HelpWindow->button(QMessageBox::Ok)->setText("确定");
    // HelpWindow->button(QMessageBox::Ok)->resize(100,30);
    HelpWindow->show();
}

void MainWindow::ShowAboutWindow() {
    auto AboutWindow = new QMessageBox(this);
    AboutWindow->setWindowTitle("关于");
    AboutWindow->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    QString Message = QStringLiteral("<h3>关于</h3><p>PvZ Helper v") + APP_VER + "</p>"
                      + "<p>本程序由百度贴吧@46287153制作</p>"
                        "<p><a href=\"https://tieba.baidu.com/f?kw=植物大战僵尸\">植物大战僵尸吧</a></p>"
                        "<a href=\"https://tieba.baidu.com/home/main?un=46287153\">@46287153</a>"
                        "<p><a href=\"https://github.com/CarrotFish/PvZHelper-Mac\">源代码</a></p>";
    AboutWindow->setText(Message);
    AboutWindow->setTextFormat(Qt::TextFormat::RichText);
    AboutWindow->setTextInteractionFlags(Qt::TextInteractionFlag::LinksAccessibleByMouse);
    AboutWindow->addButton(QMessageBox::Ok);
    // AboutWindow->button(QMessageBox::Ok)->setText("确定");
    // AboutWindow->button(QMessageBox::Ok)->resize(100,30);
    AboutWindow->show();
}

void MainWindow::ShowAboutQtWindow() {
    //QMessageBox::aboutQt(this, "关于Qt");
    QApplication::aboutQt();
}

void MainWindow::ShowSpawnHelpWindow() {
    auto SpawnHelpWindow = new QMessageBox(this);
    SpawnHelpWindow->setWindowTitle("出怪修改说明");
    SpawnHelpWindow->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint |
                                    Qt::WindowCloseButtonHint);
    QString Message = "<h3>出怪修改说明</h3>"
                      "<p>请在选卡时或开始游戏后进行出怪修改，修改仅在本轮游戏有效<br>"
                      "\"自然出怪\"仅修改出怪种类，列表填充由游戏内部完成<br>"
                      "\"模拟自然出怪\"使用统计结果模拟无尽高F下的自然出怪比例<br>"
                      "\"极限出怪\"将出怪列表均匀填充<br>"
                      "\"恢复出怪\"使游戏重新由种子生成出怪列表<br>"
                      "勾选\"限制旗帜\"后旗帜僵尸仅在每大波出现一只<br>"
                      "勾选\"限制雪人\"后雪人僵尸每关仅出现一只<br>"
                      "勾选\"限制蹦极\"后蹦极僵尸仅在每大波出现<br>"
                      "勾选\"限制红眼\"后红眼巨人仅在所选波次出现<br>"
                      "点击\"查看出怪列表\"可查看每种僵尸的总数</p>";
    SpawnHelpWindow->setText(Message);
    SpawnHelpWindow->setTextFormat(Qt::TextFormat::RichText);
    SpawnHelpWindow->setTextInteractionFlags(Qt::TextInteractionFlag::NoTextInteraction);
    SpawnHelpWindow->addButton(QMessageBox::Ok);
    // SpawnHelpWindow->button(QMessageBox::Ok)->setText("确定");
    // SpawnHelpWindow->button(QMessageBox::Ok)->resize(100,30);
    SpawnHelpWindow->show();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (pvz->isGameOn(false) && !ui->KeepChangesOnExit->isChecked()) {
        for (int i = 0; i < ui->tabWidget->count(); i++) {
            auto Tab = ui->tabWidget->widget(i);
            auto WidgetList = Tab->findChildren<QCheckBox *>();
            for (auto CheckBox:WidgetList) {
                if (CheckBox->isChecked() && !list->NonMaintainCheckBox.contains(CheckBox->objectName())) {
                    CheckBox->toggle();
                }
            }
        }
        PortalWindow->RestoreChanges();
    }
    delete PortalWindow;
    event->accept();
}

void MainWindow::GameFound(pid_t pid) {
    ui->GamePID->setText(QString::number(pid));
    emit ui->ResourceType->currentIndexChanged(ui->ResourceType->currentIndex());
    emit ui->CardID->currentIndexChanged(ui->CardID->currentIndex());
    emit ui->PlantHPType->currentIndexChanged(ui->PlantHPType->currentIndex());
    emit ui->PlantAttackIntervalType->currentIndexChanged(ui->PlantAttackIntervalType->currentIndex());
    emit ui->ProjectileType->currentIndexChanged(ui->ProjectileType->currentIndex());
    emit ui->ZombieHPType->currentIndexChanged(ui->ZombieHPType->currentIndex());
    if (!ui->AutoRefreshGameStatus->isChecked())
        ui->AutoRefreshGameStatus->toggle();
}

void MainWindow::ShowGameStatus(int CurGameMode, int CurGameUI, int CurGameTime, int CurrentWave, int PlantsCount,
                                int ZombiesCount, int RefreshCountdown, int HugeWaveCountdown) {
    ui->CurrentMode->setText(QString::number(CurGameMode));
    ui->CurrentUI->setText(QString::number(CurGameUI));
    ui->GameTime->setText(QString::number(CurGameTime));
    ui->CurrentWave->setText(QString::number(CurrentWave));
    ui->PlantsCount->setText(QString::number(PlantsCount));
    ui->ZombiesCount->setText(QString::number(ZombiesCount));
    ui->RefreshCountdown->setText(QString::number(RefreshCountdown));
    ui->HugeWaveCountdown->setText(QString::number(HugeWaveCountdown));
}

void MainWindow::GameNotFound(bool alert) {
    ui->CurrentMode->clear();
    ui->CurrentUI->clear();
    ui->GameTime->clear();
    ui->ResourceValue->clear();
    ui->CardCost->clear();
    ui->CardCooldownTime->clear();
    ui->PlantHP->clear();
    ui->PlantAttackInterval->clear();
    ui->ProjectileDamage->clear();
    ui->ZombieHP->clear();
    ClearSpawnTable();
    if (alert) {
        QMessageBox Message(QMessageBox::Information, "", "找不到游戏!", QMessageBox::Ok, nullptr,
                            Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
        // Message.button(QMessageBox::Ok)->setText("确定");
        // Message.button(QMessageBox::Ok)->resize(100,30);
        Message.exec();
    }
}

void MainWindow::MaintainCheckedItem() {
    if (pvz->isGameOn()) {
        for (int i = 0; i < ui->tabWidget->count(); i++) {
            auto Tab = ui->tabWidget->widget(i);
            auto WidgetList = Tab->findChildren<QCheckBox *>();
            for (auto CheckBox:WidgetList) {
                if (!list->NonMaintainCheckBox.contains(CheckBox->objectName()))
                        emit CheckBox->toggled(CheckBox->isChecked());
            }
        }
        return;
    }
}

void MainWindow::AutoRefreshGameStatus(bool on) {
    static auto *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [=]() {
        emit GetGameStatus(false);
    });
    if (on) {
        timer->start(100);
    } else {
        timer->stop();
    }
}

void MainWindow::SetResourceValue(int value) {
    ui->ResourceValue->setText(QString::number(value));
}

void MainWindow::EnableCost() {
    ui->CardCost->setDisabled(false);
    ui->label_11->setDisabled(false);
}

void MainWindow::DisableCost() {
    ui->CardCost->setDisabled(true);
    ui->label_11->setDisabled(true);
}

void MainWindow::EnableCooldownTime() {
    ui->CardCooldownTime->setDisabled(false);
    ui->label_12->setDisabled(false);
}

void MainWindow::DisableCooldownTime() {
    ui->CardCooldownTime->setDisabled(true);
    ui->label_12->setDisabled(true);
}

void MainWindow::ShowCardProperty(int cost, int cooldowntime) {
    ui->CardCost->setText(QString::number(cost));
    ui->CardCooldownTime->setText(QString::number(cooldowntime));
}

void MainWindow::ShowPlantHP(int value) {
    ui->PlantHP->setText(QString::number(value));
}

void MainWindow::ShowPlantAttackInterval(int value) {
    ui->PlantAttackInterval->setText(QString::number(value));
}

void MainWindow::ShowProjectileDamage(int damage) {
    ui->ProjectileDamage->setText(QString::number(damage));
}

void MainWindow::ShowZombieHP(int value) {
    ui->ZombieHP->setText(QString::number(value));
}

std::array<bool, 33> MainWindow::GetZombies() const {
    std::array<bool, 33> zombies = {false};
    zombies[0] = true;
    zombies[1] = true;
    zombies[2] = ui->SpawnType_0->isChecked();
    zombies[3] = ui->SpawnType_1->isChecked();
    zombies[4] = ui->SpawnType_2->isChecked();
    zombies[5] = ui->SpawnType_3->isChecked();
    zombies[6] = ui->SpawnType_4->isChecked();
    zombies[7] = ui->SpawnType_5->isChecked();
    zombies[8] = ui->SpawnType_6->isChecked();
    zombies[11] = ui->SpawnType_7->isChecked();
    zombies[12] = ui->SpawnType_8->isChecked();
    zombies[14] = ui->SpawnType_9->isChecked();
    zombies[15] = ui->SpawnType_10->isChecked();
    zombies[16] = ui->SpawnType_11->isChecked();
    zombies[17] = ui->SpawnType_12->isChecked();
    zombies[18] = ui->SpawnType_13->isChecked();
    zombies[19] = ui->SpawnType_14->isChecked();
    zombies[20] = ui->SpawnType_15->isChecked();
    zombies[21] = ui->SpawnType_16->isChecked();
    zombies[22] = ui->SpawnType_17->isChecked();
    zombies[23] = ui->SpawnType_18->isChecked();
    zombies[32] = ui->SpawnType_19->isChecked();
    return zombies;
}

std::array<bool, 20> MainWindow::GetGigaWaves() const {
    std::array<bool, 20> giga_waves{};
    giga_waves[0] = ui->GigaWave_0->isChecked();
    giga_waves[1] = ui->GigaWave_1->isChecked();
    giga_waves[2] = ui->GigaWave_2->isChecked();
    giga_waves[3] = ui->GigaWave_3->isChecked();
    giga_waves[4] = ui->GigaWave_4->isChecked();
    giga_waves[5] = ui->GigaWave_5->isChecked();
    giga_waves[6] = ui->GigaWave_6->isChecked();
    giga_waves[7] = ui->GigaWave_7->isChecked();
    giga_waves[8] = ui->GigaWave_8->isChecked();
    giga_waves[9] = ui->GigaWave_9->isChecked();
    giga_waves[10] = ui->GigaWave_10->isChecked();
    giga_waves[11] = ui->GigaWave_11->isChecked();
    giga_waves[12] = ui->GigaWave_12->isChecked();
    giga_waves[13] = ui->GigaWave_13->isChecked();
    giga_waves[14] = ui->GigaWave_14->isChecked();
    giga_waves[15] = ui->GigaWave_15->isChecked();
    giga_waves[16] = ui->GigaWave_16->isChecked();
    giga_waves[17] = ui->GigaWave_17->isChecked();
    giga_waves[18] = ui->GigaWave_18->isChecked();
    giga_waves[19] = ui->GigaWave_19->isChecked();
    return giga_waves;
}

void MainWindow::UpdateSpawnTable(std::array<int, 33> &zombies_count) {
    int zombies_type_count = 0, total_count = std::accumulate(zombies_count.begin(), zombies_count.end(), 0);
    for (size_t i = 0; i < 33; i++)
        if (zombies_count[i] > 0)
            zombies_type_count++;
    bool visible = SpawnTable->isVisible();
    SpawnTable->clear();
    SpawnTable->setRowCount(total_count > 0 ? zombies_type_count + 1 : zombies_type_count);
    SpawnTable->setColumnCount(2);
    SpawnTable->setWindowTitle("出怪列表");
    int width = zombies_type_count >= 9 ? 236 : 216, height = 294;
    SpawnTable->setFixedSize(width, height);
    QStringList header;
    header << "类型" << "数目";
    SpawnTable->setHorizontalHeaderLabels(header);
    SpawnTable->setCornerButtonEnabled(false);
    SpawnTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    SpawnTable->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    SpawnTable->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    //tableWidget->setWindowFlags(Qt::Dialog);
    int row = 0;
    for (size_t i = 0; i < 33; i++) {
        if (zombies_count[i] > 0) {
            SpawnTable->setItem(row, 0, new QTableWidgetItem(list->ZombiesList.at(i)));
            SpawnTable->setItem(row, 1, new QTableWidgetItem(QString::number(zombies_count[i])));
            SpawnTable->item(row, 0)->setFlags(Qt::ItemFlags(~Qt::ItemIsSelectable & ~Qt::ItemIsEditable));
            SpawnTable->item(row, 1)->setFlags(Qt::ItemFlags(~Qt::ItemIsSelectable & ~Qt::ItemIsEditable));
            row++;
        }
    }
    if (total_count > 0) {
        SpawnTable->setItem(row, 0, new QTableWidgetItem("总数"));
        SpawnTable->setItem(row, 1, new QTableWidgetItem(QString::number(total_count)));
        SpawnTable->item(row, 0)->setFlags(Qt::ItemFlags(~Qt::ItemIsSelectable & ~Qt::ItemIsEditable));
        SpawnTable->item(row, 1)->setFlags(Qt::ItemFlags(~Qt::ItemIsSelectable & ~Qt::ItemIsEditable));
    }
    SpawnTable->update();
    if (visible)
        SpawnTable->show();
}

void MainWindow::ClearSpawnTable() {
    SpawnTable->clear();
    SpawnTable->hide();
}

void MainWindow::UpdateGigaWaves(std::array<bool, 20> &giga_waves) {
    ui->GigaWave_0->setChecked(giga_waves[0]);
    ui->GigaWave_1->setChecked(giga_waves[1]);
    ui->GigaWave_2->setChecked(giga_waves[2]);
    ui->GigaWave_3->setChecked(giga_waves[3]);
    ui->GigaWave_4->setChecked(giga_waves[4]);
    ui->GigaWave_5->setChecked(giga_waves[5]);
    ui->GigaWave_6->setChecked(giga_waves[6]);
    ui->GigaWave_7->setChecked(giga_waves[7]);
    ui->GigaWave_8->setChecked(giga_waves[8]);
    ui->GigaWave_9->setChecked(giga_waves[9]);
    ui->GigaWave_10->setChecked(giga_waves[10]);
    ui->GigaWave_11->setChecked(giga_waves[11]);
    ui->GigaWave_12->setChecked(giga_waves[12]);
    ui->GigaWave_13->setChecked(giga_waves[13]);
    ui->GigaWave_14->setChecked(giga_waves[14]);
    ui->GigaWave_15->setChecked(giga_waves[15]);
    ui->GigaWave_16->setChecked(giga_waves[16]);
    ui->GigaWave_17->setChecked(giga_waves[17]);
    ui->GigaWave_18->setChecked(giga_waves[18]);
    ui->GigaWave_19->setChecked(giga_waves[19]);
}

void MainWindow::ShowSeed(uint32_t seed) {
    ui->SpawnSeed->setText(QString("%1").arg(seed, 8, 16, QLatin1Char('0')).toUpper());
}

void MainWindow::OpenUserdataFolder(QString DataDir) {
    DataDir = "file:" + DataDir;
    QDesktopServices::openUrl(QUrl(DataDir, QUrl::TolerantMode));
}

void MainWindow::SelectPAKFile() {
    QFileInfo PAKPath = QFileDialog::getOpenFileName(this, "选择PAK文件", "~", "PAK文件(*.pak);;所有文件(*.*)");
    if (!PAKPath.filePath().isEmpty()) {
        emit PAKFilePath(PAKPath.filePath());
        emit AutoPAKFolderPath(PAKPath.absolutePath() + "/pak");
    }
}

void MainWindow::SelectPAKFolder() {
    QFileInfo FolderPath = QFileDialog::getExistingDirectory(this, "选择文件夹", "~");
    if (!FolderPath.filePath().isEmpty()) {
        emit PAKFolderPath(FolderPath.filePath());
        emit AutoPAKFilePath(FolderPath.absolutePath() + "/main.pak");
    }
}

void MainWindow::UpdatePAKPath(const QString &Path) {
    ui->PakPath->setText(Path);
}

void MainWindow::UpdateFolderPath(const QString &Path) {
    ui->FolderPath->setText(Path);
}

void MainWindow::ExtractPAK(const QString &PAKPath, const QString &FolderPath) {
    bool extract_failed = false;
    // open file
    QFile PAKFile(PAKPath);
    if (!PAKFile.open(QIODevice::ReadOnly | QIODevice::ExistingOnly)) {
        extract_failed = true;
        QMessageBox Message(QMessageBox::Warning, "", "无法读取文件：" + PAKPath, QMessageBox::Yes, nullptr,
                            Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
        // Message.button(QMessageBox::Yes)->setText("确定");
        // Message.button(QMessageBox::Ok)->resize(100,30);
        Message.exec();
        return;
    }
    
    // loat entire file to memory
    auto buffer = PAKFile.readAll();
    PAKFile.close();
    
    // XOR entire file with byte 0xf7
    for (auto &i : buffer) {
        i ^= 0xF7;
    }
    
    // check file header
    QByteArray file_header(9, 0x00);
    file_header[0] = 0xC0;
    file_header[1] = 0x4A;
    file_header[2] = 0xC0;
    file_header[3] = 0xBA;
    if (buffer.left(9) != file_header)
        return;
    
    unsigned int index = 9;
    unsigned int files_count = 0;
    std::vector<QString> files_name;
    std::vector<unsigned int> files_size;
    
    char name_size;
    QString file_name(256, 0);
    int file_size;
    char eof_flag;
    
    // get index info
    do {
        name_size = buffer[index];
        index++;
        
        file_name = buffer.mid(index, name_size);
        file_name.replace('\\', '/');
        index += int(name_size);
        
        file_size = *(int *) buffer.mid(index, sizeof(int)).data();
        index += 4;
        
        index += 8;
        
        eof_flag = buffer[index];
        index++;
        
        files_name.push_back(file_name);
        files_size.push_back(file_size);
        files_count++;
    } while (eof_flag == 0x00);
    
    // extract all files
    for (uint32_t i = 0; i < files_count; i++) {
        QString output_file_path = FolderPath + '/' + files_name.at(i);
        unsigned int output_file_size = files_size.at(i);
        QString output_file_folder = output_file_path.left(output_file_path.lastIndexOf('/'));
        bool path_created = QDir().mkpath(output_file_folder);
        
        if (path_created) {
            QFile file(output_file_path);
            if (file.open(QIODevice::WriteOnly)) {
                file.write(buffer.mid(index, output_file_size));
                file.close();
            } else {
                QMessageBox Message(QMessageBox::Warning, "", "无法创建文件：" + output_file_path,
                                    QMessageBox::Ignore | QMessageBox::Cancel, nullptr,
                                    Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint |
                                    Qt::WindowCloseButtonHint);
                Message.setDefaultButton(QMessageBox::Cancel);
                // Message.button(QMessageBox::Ignore)->setText("忽略");
                // Message.button(QMessageBox::Cancel)->setText("取消");
                // Message.button(QMessageBox::Ignore)->resize(100,30);
                // Message.button(QMessageBox::Cancel)->resize(100,30);
                if (Message.exec() == QMessageBox::Cancel) {
                    extract_failed = true;
                    break;
                }
            }
        } else {
            QMessageBox Message(QMessageBox::Warning, "", "无法创建目录：" + output_file_folder,
                                QMessageBox::Ignore | QMessageBox::Cancel, nullptr,
                                Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint |
                                Qt::WindowCloseButtonHint);
            Message.setDefaultButton(QMessageBox::Cancel);
            // Message.button(QMessageBox::Ignore)->setText("忽略");
            // Message.button(QMessageBox::Cancel)->setText("取消");
            // Message.button(QMessageBox::Ignore)->resize(100,30);
            // Message.button(QMessageBox::Cancel)->resize(100,30);
            if (Message.exec() == QMessageBox::Cancel) {
                extract_failed = true;
                break;
            }
        }
        index += output_file_size;
    }
    
    //change owner
    {
        QString userPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
        QString userName = userPath.section("/", -1, -1);
        //QString userName = FolderPath.section('/', 2, 2);
        QString cmd = FolderPath;
        cmd = "chown -R " + userName + ' ' + cmd.replace(' ', "\\ ");
        system(cmd.toStdString().data());
    }
    
    if (!extract_failed) {
        QMessageBox Message(QMessageBox::Information, "", "解包完成!", QMessageBox::Ok, nullptr,
                            Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
        // Message.button(QMessageBox::Ok)->setText("确定");
        // Message.button(QMessageBox::Ok)->resize(100,30);
        Message.exec();
    }
}

void MainWindow::PackPAK(const QString &FolderPath, const QString &PAKPath) {
    QFile PAKFile(PAKPath);
    if (!PAKFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QMessageBox Message(QMessageBox::Warning, "", "无法创建文件：" + PAKPath, QMessageBox::Ok, nullptr,
                            Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
        // Message.button(QMessageBox::Ok)->setText("确定");
        // Message.button(QMessageBox::Ok)->resize(100,30);
        Message.exec();
        return;
    }
    std::function<QFileInfoList(const QString &)> GetFileInfoList;
    
    GetFileInfoList = [&GetFileInfoList](const QString &path) -> QFileInfoList {
        QDir dir(path);
        QFileInfoList file_list = dir.entryInfoList(QDir::Files | QDir::NoSymLinks);
        QFileInfoList folder_list = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
        
        for (auto &child_folder : folder_list) {
            QString child_folder_name = child_folder.absoluteFilePath();
            QFileInfoList child_file_list = GetFileInfoList(child_folder_name);
            file_list.append(child_file_list);
        }
        
        return file_list;
    };
    
    QFileInfoList files_list = GetFileInfoList(FolderPath);
    unsigned int files_count = files_list.size();
    
    QByteArray file_header(9, 0x00);
    file_header[0] = 0xC0;
    file_header[1] = 0x4A;
    file_header[2] = 0xC0;
    file_header[3] = 0xBA;
    for (auto &i : file_header) {
        i ^= 0xF7;
    }
    
    char name_size;
    QString file_name(256, 0);
    int file_size;
    qint64 file_time;
    char eof_flag;
    
    // write header
    PAKFile.write(file_header);
    
    QDateTime origin(QDate(1601, 1, 1), QTime(0, 0, 0, 0), Qt::UTC);
    // write index section
    for (uint32_t i = 0; i < files_count; i++) {
        file_name = files_list.at(i).filePath();
        file_name = file_name.right(file_name.size() - FolderPath.size() - 1);
        name_size = file_name.size();
        file_size = files_list.at(i).size();
        auto fileDateTime = files_list.at(i).birthTime();
        file_time = 10000 * origin.msecsTo(fileDateTime);
        eof_flag = (i == (files_count - 1) ? 0x80 : 0x00);
        
        QByteArray file_struct;
        file_struct.append((const char *) &name_size, sizeof(char));
        file_struct.append(file_name.toStdString().data(), file_name.size());
        file_struct.append((const char *) &file_size, sizeof(int));
        file_struct.append((const char *) &file_time, sizeof(qint64));
        file_struct.append((const char *) &eof_flag, sizeof(char));
        
        for (auto &j : file_struct)
            j ^= 0xF7;
        PAKFile.write(file_struct);
    }
    
    // write data section
    for (uint32_t i = 0; i < files_count; i++) {
        QString file_path = files_list.at(i).filePath();
        
        QFile file(file_path);
        file.open(QIODevice::ReadOnly);
        auto buffer = file.readAll();
        
        for (auto &j : buffer)
            j ^= 0xF7;
        PAKFile.write(buffer);
    }
    PAKFile.close();
    
    //change owner
    {
        QString userPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
        QString userName = userPath.section("/", -1, -1);
        //QString userName = PAKPath.section('/', 2, 2);
        QString cmd = PAKPath;
        cmd = "chown " + userName + ' ' + cmd.replace(' ', "\\ ");
        system(cmd.toStdString().data());
    }
    
    QMessageBox Message(QMessageBox::Information, "", "打包完成!", QMessageBox::Ok, nullptr,
                        Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    // Message.button(QMessageBox::Ok)->setText("确定");
    // Message.button(QMessageBox::Ok)->resize(100,30);
    Message.exec();
    
}
