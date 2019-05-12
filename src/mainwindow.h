#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QTableWidget>

namespace Ui {
    class MainWindow;
}

class PvZ;

class List;

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    
    ~MainWindow() override;

signals:
    
    void FindGameProc();
    
    void GetGameStatus(bool alert = true);
    
    void ModifySun(int sun);
    
    void ModifyMoney(int money);
    
    void LockSun(bool on);
    
    void LockMoney(bool on);
    
    void ModifySunLimit(int sun_limit);
    
    void ModifyMoneyLimit(int money_limit);
    
    void GetSliverSunflower();
    
    void GetGoldenSunflower();
    
    void GetAllItems();
    
    void FinishAchievements();
    
    void ModifyResourceValue(int type, int value);
    
    void GetResourceValue(int type);
    
    void AutoCollect(bool on);
    
    void NoMoneyDrops(bool on);
    
    void NoSunFalling(bool on);
    
    void KeepSunFalling(bool on);
    
    void ModifyMode(int mode);
    
    void ModifyEndlessLevel(int level);
    
    void ModifyAdventureLevel(int level);
    
    void ModifyAdventureCompletionTimes(int times);
    
    void ImmediatelyWin();
    
    void ShowLimboPage(bool on);
    
    void UnlockAllModes(bool on);
    
    void ItsRaining(bool on);
    
    void ZombieQuick(bool on);
    
    void BungeeBlitz(bool on);
    
    void HighGravity(bool on);
    
    void GraveDanger(bool on);
    
    void SunnyDay(bool on);
    
    void DarkStormyNight(bool on);
    
    void BigTime(bool on);
    
    void ColumnLike(bool on);
    
    void ModifySlotCount(int count);
    
    void SlotVisible(bool visible);
    
    void ShowShovel(bool visible);
    
    void ModifySlotContent(int SlotID, int type, bool isImitater, bool visible);
    
    void NoCoolDown(bool on);
    
    void FreelyPlant(bool on);
    
    void PurplePlantAvailable(bool on);
    
    void AlwaysShovel(bool on);
    
    void HideMenu(bool on);
    
    void ModifyCardProperty(int CardID, int cost, int cooldowntime);
    
    void GetCardProperty(int CardID);
    
    void ModifyGameScene(int scene);
    
    void ModifyGridType(int row, int column, int type);
    
    void ModifyRowType(int row, int type);
    
    void PutLadder(int row, int column);
    
    void PutGrave(int row, int column);
    
    void PutRake(int row, int column);
    
    void PumpkinLadder(bool imitater_only);
    
    void SetPlant(int row, int column, int type, bool isImitater);
    
    void SetZombie(int row, int column, int type);
    
    void SpawnZombie(int type, int count);
    
    void NoFog(bool on);
    
    void BigFog(bool on);
    
    void SeeVase(bool on);
    
    void LawnMowersStart();
    
    void LawnMowersReset();
    
    void LawnMowersDisappear();
    
    void ClearAllPlants();
    
    void ClearAllZombies();
    
    void ClearAllItems();
    
    void ClearAllGridItems(int type);
    
    void ModifyPlantHP(int type, int value);
    
    void GetPlantHP(int type);
    
    void ModifyPlantAttackInterval(int type, int value);
    
    void GetPlantAttackInterval(int type);
    
    void ModifyProjectileDamage(int type, int damage);
    
    void GetProjectileDamage(int type);
    
    void PlantInvincible(bool on);
    
    void PlantWeak(bool on);
    
    void ChomperNoCD(bool on);
    
    void PotatoMineNoCD(bool on);
    
    void BombNoExplode(bool on);
    
    void BombInstantlyExplode(bool on);
    
    void CobCannonNoCD(bool on);
    
    void MagnetShroomNoCD(bool on);
    
    void NoCrater(bool on);
    
    void PlantBoard(bool on);
    
    void MushroomAwake(bool on);
    
    void SunShroomGrow(bool on);
    
    void AlwaysKernal(bool on);
    
    void AlwaysButter(bool on);
    
    void StrongBlover(bool on);
    
    void BeghouledFreelyMove(bool on);
    
    void ModifyZombieHP(int type, int value);
    
    void GetZombieHP(int type);
    
    void ZombieInvincible(bool on);
    
    void ZombieWeak(bool on);
    
    void JackNoExplode(bool on);
    
    void JackImmediatelyExplode(bool on);
    
    void NoImpThrow(bool on);
    
    void InfiniteImpThrow(bool on);
    
    void NoBasketball(bool on);
    
    void StandingStill(bool on);
    
    void InfiniteBasketball(bool on);
    
    void NoZombieSpawn(bool on);
    
    void AllZombieSpawn(bool on);
    
    void NoThreesome(bool on);
    
    void InfinitePole(bool on);
    
    void ZombieInvisible(bool on);
    
    void ZombieVisible(bool on);
    
    void ButterImmune(bool on);
    
    void IceImmune(bool on);
    
    void SlowdownImmune(bool on);
    
    void NoIceTrail(bool on);
    
    void NoYetiEscape(bool on);
    
    void NoEnterHouse(bool on);
    
    void AllZombiesXXX(int status);
    
    void SpawnNextWave();
    
    void NaturalSpawn(std::array<bool, 33> &zombies);
    
    void SimulateSpawn(std::array<bool, 33> &zombies, bool limit_flag, bool limit_yeti, bool limit_bungee,
                       bool limit_giga, std::array<bool, 20> &giga_waves);
    
    void ExtremeSpawn(std::array<bool, 33> &zombies, bool limit_flag, bool limit_yeti, bool limit_bungee,
                      bool limit_giga, std::array<bool, 20> &giga_waves);
    
    void RestoreSpawn();
    
    void GetSpawnList();
    
    void ModifyFertilizer(int value);
    
    void ModifyBugSpray(int value);
    
    void ModifyChocolate(int value);
    
    void ModifyTreeFertilizer(int value);
    
    void ModifyWisdomTreeHeight(int value);
    
    void NoSnailSleep(bool on);
    
    void MarigoldRefresh();
    
    void GardenPlantRight();
    
    void GardenPlantLeft();
    
    void SetDebugMode(int mode);
    
    void OpenUserdata();
    
    void SetSeed(uint32_t seed);
    
    void GetSeed();
    
    void GetRandomSeed();
    
    void BackgroundRunning(bool on);
    
    void DisablePause(bool on);
    
    void NoDataDelete(bool on);
    
    void NoDataSave(bool on);
    
    void PAKFilePath(const QString &Path);
    
    void AutoPAKFilePath(const QString &Path);
    
    void PAKFolderPath(const QString &Path);
    
    void AutoPAKFolderPath(const QString &Path);
    
    void SetMusic(int type);

public slots:
    
    void GameFound(pid_t pid);
    
    void ShowGameStatus(int CurGameMode, int CurGameUI, int CurGameTime, int CurrentWave, int PlantsCount,
                        int ZombiesCount, int RefreshCountdown, int HugeWaveCountdown);
    
    void GameNotFound(bool alert);
    
    void SetResourceValue(int value);
    
    void EnableCost();
    
    void DisableCost();
    
    void EnableCooldownTime();
    
    void DisableCooldownTime();
    
    void ShowCardProperty(int cost, int cooldowntime);
    
    void ShowPlantHP(int value);
    
    void ShowPlantAttackInterval(int value);
    
    void ShowProjectileDamage(int damage);
    
    void ShowZombieHP(int value);
    
    void UpdateSpawnTable(std::array<int, 33> &zombies_count);
    
    void ClearSpawnTable();
    
    void UpdateGigaWaves(std::array<bool, 20> &giga_waves);
    
    void ShowSeed(uint32_t seed);
    
    void SelectPAKFile();
    
    void SelectPAKFolder();

private slots:
    
    void ShowHelpWindow();
    
    void ShowAboutWindow();
    
    static void ShowAboutQtWindow();
    
    void MaintainCheckedItem();
    
    void AutoRefreshGameStatus(bool on);
    
    void ShowSpawnHelpWindow();
    
    void UpdatePAKPath(const QString &Path);
    
    void UpdateFolderPath(const QString &Path);
    
    static void ExtractPAK(const QString &PAKPath, const QString &FolderPath);
    
    static void PackPAK(const QString &FolderPath, const QString &PAKPath);

private:
    Ui::MainWindow *ui;
    QTableWidget *SpawnTable;
    PvZ *pvz;
    List *list;
protected:
    void closeEvent(QCloseEvent *event) override;
    
    void ConnectSlots() const;
    
    void ConnectWidgets();
    
    std::array<bool, 33> GetZombies() const;
    
    std::array<bool, 20> GetGigaWaves() const;
};

#endif // MAINWINDOW_H
