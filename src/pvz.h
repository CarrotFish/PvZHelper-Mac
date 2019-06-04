#ifndef PVZ_H
#define PVZ_H

#include <QObject>
#include <array>
#include <initializer_list>
#include "memory.h"
#include "code.h"

typedef uint8_t byte;

namespace Ui {
    class MainWindow;
}

class MainWindow;

class PvZ : public QObject {
Q_OBJECT

signals:
    
    void GameFound(pid_t pid);
    
    void
    GameStatus(int CurGameMode, int CurGameUI, int CurGameTime, int CurrentWave, int PlantsCount, int ZombiesCount,
               int RefreshCountdown, int HugeWaveCountdown);
    
    void GameNotFound(bool alert);
    
    void ResourceValue(int Value);
    
    void CardPlant();
    
    void CardZombie();
    
    void CardOthers();
    
    void CardProperty(int cost, int cooldowntime);
    
    void PlantHP(int value);
    
    void PlantAttackInterval(int value);
    
    void ProjectileDamage(int value);
    
    void ZombieHP(int value);
    
    void SpawnList(std::array<int, 33> &zombies_count);
    
    void GigaWaves(std::array<bool, 20> &giga_waves);
    
    void Seed(uint32_t seed);
    
    void UserdataFolder(QString DataDir);

public:
    PvZ(Ui::MainWindow *ui, MainWindow *MainWindow);
    
    ~PvZ() override;

public slots:
    
    bool isGameOn(bool alert = true);
    
    void FindGameProc();
    
    void GetGameStatus(bool alert = true);

//Resources
    
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

//Level
    
    void ModifyMode(int mode);
    
    void StartLevel(int mode);
    
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

//Slot
    
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

//Scene
    
    void ModifyGameScene(int scene);
    
    void ModifyGridType(int row, int column, int type);
    
    void ModifyRowType(int row, int type);
    
    void PutLadder(int row, int column);
    
    void PutGrave(int row, int column);
    
    void PutRake(int row, int column);
    
    void PutCoin(int type, int row, int column);
    
    void PumpkinLadder(bool imitater_only);
    
    void SetPlant(int row, int column, int type, bool imitater);
    
    void SetZombie(int row, int column, int type);
    
    void SpawnZombie(int type, int count);
    
    void NoFog(bool on);
    
    void BigFog(bool on);
    
    void SeeVase(bool on);
    
    void LawnMowersStart();
    
    void LawnMowersReset();
    
    void LawnMowersDisappear();
    
    void SetBlackPortal(int row_1, int column_1, int row_2, int column_2);
    
    void SetWhitePortal(int row_1, int column_1, int row_2, int column_2);
    
    void ActivePortal(bool on);
    
    void LockPortal(bool on);
    
    void ClearAllPlants();
    
    void ClearAllZombies();
    
    void ClearAllItems();
    
    void ClearAllGridItems(int type);

//Plants
    
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

//Zombies
    
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
    
    void GatherZombies(bool on, float pos);
    
    void AllZombiesXXX(int status);
    
    void SpawnNextWave();

//Spawn
    
    void NaturalSpawn(std::array<bool, 33> &zombies);
    
    void SimulateSpawn(std::array<bool, 33> &zombies, bool limit_flag, bool limit_yeti, bool limit_bungee,
                       bool limit_giga, std::array<bool, 20> &giga_waves);
    
    void ExtremeSpawn(std::array<bool, 33> &zombies, bool limit_flag, bool limit_yeti, bool limit_bungee,
                      bool limit_giga, std::array<bool, 20> &giga_waves);
    
    void RestoreSpawn();
    
    void GetSpawnList();

//Garden
    
    void ModifyFertilizer(int value);
    
    void ModifyBugSpray(int value);
    
    void ModifyChocolate(int value);
    
    void ModifyTreeFertilizer(int value);
    
    void ModifyWisdomTreeHeight(int height);
    
    void NoSnailSleep(bool on);
    
    void MarigoldRefresh();
    
    void GardenPlantRight();
    
    void GardenPlantLeft();

//Others
    
    void SetDebugMode(int mode);
    
    void GetUserdataFolder();
    
    void SetSeed(uint32_t seed);
    
    void GetSeed();
    
    void GetRandomSeed();
    
    void BackgroundRunning(bool on);
    
    void DisablePause(bool on);
    
    void NoDataDelete(bool on);
    
    void NoDataSave(bool on);
    
    void SetMusic(int type);

private:
    template<typename T, typename... Args>
    T ReadMemory(Args... address);
    
    template<typename T, size_t size, typename... Args>
    std::array<T, size> ReadMemory(Args... address);
    
    template<typename T, typename... Args>
    void WriteMemory(T value, Args... address);
    
    void WriteMemory(std::initializer_list<byte> il, uintptr_t address);
    
    int CurGameMode();
    
    int CurGameUI();
    
    int CurGameTime();
    
    int CurrentWave();
    
    int PlantsCount();
    
    int ZombiesCount();
    
    int RefreshCountdown();
    
    int HugeWaveCountdown();
    
    int CurScene();
    
    int CurRowCount();
    
    void UpdateZombiesType();
    
    void UpdateZombiesList();
    
    void UpdateZombiesPreview();
    
    void InternalSpawn(std::array<bool, 33> &zombies);
    
    void CustomizeSpawn(std::array<bool, 33> &zombies, bool simulate, bool limit_flag, bool limit_yeti,
                        bool limit_bungee, bool limit_giga, std::array<bool, 20> &giga_waves);
    
    Memory memory;
    Code code;
    pid_t pid = 0;
    const uint32_t base = 0x35EE64;
    const std::string ProcessName = "Plants vs. Zombi";
    
    Ui::MainWindow *ui;
    MainWindow *window;
};


#endif //PVZ_H
