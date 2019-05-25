#include "pvz.h"
#include "mainwindow.h"
#include <ctime>
#include <random>
#include <QDebug>

template<typename T, typename... Args>
inline T PvZ::ReadMemory(Args... address) {
    return memory.ReadMem<T>({static_cast<uintptr_t>(address)...});
}

template<typename T, size_t size, typename... Args>
inline std::array<T, size> PvZ::ReadMemory(Args... address) {
    return memory.ReadMem<T, size>({static_cast<uintptr_t>(address)...});
}

template<typename T, typename... Args>
inline void PvZ::WriteMemory(T value, Args... address) {
    memory.WriteMem<T>(value, {static_cast<uintptr_t>(address)...});
}

inline void PvZ::WriteMemory(std::initializer_list<byte> il, uintptr_t address) {
    memory.Write(address, il.size(), (void *) il.begin());
}

// template<size_t original_size, size_t size>
// void PvZ::CodeInject(bool on, uint32_t address, std::array<byte, size> &ar) {
//     uint32_t injected_code = 0;
//     const int code_size = size + original_size + 5;
//     if (on) {
//         injected_code = (uint32_t) memory.Allocate(code_size, VM_PROT_ALL);
//         if (injected_code) {
//             uint32_t offset = injected_code - address - 5, offset2 =
//                     address + original_size - injected_code - code_size;
//
//             std::array<byte, original_size> ar1{};
//             ar1.fill(0x90);
//             ar1[0] = 0xE9;
//             memcpy(&ar1[1], &offset, 4);
//
//             auto ar2 = ReadMemory<byte, original_size>(address);
//             std::array<byte, code_size> ar3{};
//             memcpy(&ar3[0], &ar[0], size);
//             memcpy(&ar3[size], &ar2[0], original_size);
//             ar3[code_size - 5] = 0xE9;
//             memcpy(&ar3[code_size - 4], &offset2, 4);
//
//             WriteMemory(ar3, injected_code);
//             WriteMemory(ar1, address);
//         }
//     } else if (ReadMemory<byte>(address) == 0xE9) {
//         auto offset = ReadMemory<uint32_t>(address + 1);
//         injected_code = offset + address + 5;
//         auto ar1 = ReadMemory<byte, original_size>(injected_code + size);
//         WriteMemory(ar1, address);
//         memory.Free(injected_code, code_size);
//     }
// }

template<size_t size>
void PvZ::CodeInject(bool on, uint32_t address, const std::array<byte, size> &ar, size_t original_size) {
    uint32_t injected_code = 0;
    const int code_size = size + original_size + 5;
    if (on) {
        injected_code = (uint32_t) memory.Allocate(code_size, VM_PROT_ALL);
        if (injected_code) {
            uint32_t offset = injected_code - address - 5,
                    offset2 = address + original_size - injected_code - code_size;
            //new code to replace the original one
            auto ar1 = new byte[original_size];
            memset(ar1, 0x90, original_size);
            ar1[0] = 0xE9;
            memcpy(&ar1[1], &offset, 4);
            //the original code
            auto ar2 = new byte[original_size];
            memory.Read(address, original_size, ar2);
            //code to inject
            auto ar3 = new byte[code_size];
            memcpy(ar3, &ar[0], size);
            memcpy(&ar3[size], ar2, original_size);
            ar3[code_size - 5] = 0xE9;
            memcpy(&ar3[code_size - 4], &offset2, 4);
            
            memory.Write(injected_code, code_size, ar3);
            memory.Write(address, original_size, ar1);
            delete[] ar1;
            delete[] ar2;
            delete[] ar3;
        }
    } else if (ReadMemory<byte>(address) == 0xE9) {
        injected_code = ReadMemory<uint32_t>(address + 1) + address + 5;
        auto ar1 = new byte[original_size];
        memory.Read(injected_code + size, original_size, ar1);
        memory.Write(address, original_size, ar1);
        memory.Free(injected_code, code_size);
        delete[] ar1;
    }
}

PvZ::PvZ(Ui::MainWindow *ui, MainWindow *MainWindow) : GameProc(new xnu_proc), ui(ui), window(MainWindow),
                                                       memory(GameProc->memory()), code(GameProc->code()) {
    connect(this, &PvZ::ResourceValue, window, &MainWindow::SetResourceValue);
    connect(this, &PvZ::GameFound, window, &MainWindow::GameFound);
    connect(this, &PvZ::GameStatus, window, &MainWindow::ShowGameStatus);
    connect(this, &PvZ::GameNotFound, window, &MainWindow::GameNotFound);
    
    connect(this, &PvZ::CardPlant, window, &MainWindow::EnableCost);
    connect(this, &PvZ::CardPlant, window, &MainWindow::EnableCooldownTime);
    connect(this, &PvZ::CardZombie, window, &MainWindow::EnableCost);
    connect(this, &PvZ::CardZombie, window, &MainWindow::DisableCooldownTime);
    connect(this, &PvZ::CardOthers, window, &MainWindow::DisableCost);
    connect(this, &PvZ::CardOthers, window, &MainWindow::DisableCooldownTime);
    connect(this, &PvZ::CardProperty, window, &MainWindow::ShowCardProperty);
    
    connect(this, &PvZ::PlantHP, window, &MainWindow::ShowPlantHP);
    connect(this, &PvZ::PlantAttackInterval, window, &MainWindow::ShowPlantAttackInterval);
    connect(this, &PvZ::ProjectileDamage, window, &MainWindow::ShowProjectileDamage);
    connect(this, &PvZ::ZombieHP, window, &MainWindow::ShowZombieHP);
    
    connect(this, &PvZ::SpawnList, window, &MainWindow::UpdateSpawnTable);
    connect(this, &PvZ::GigaWaves, window, &MainWindow::UpdateGigaWaves);
    connect(this, &PvZ::Seed, window, &MainWindow::ShowSeed);
    
    connect(this, &PvZ::UserdataFolder, window, &MainWindow::OpenUserdataFolder);
}

PvZ::~PvZ() {
    //GameProc->Detach();
    delete GameProc;
}

bool PvZ::isGameOn(bool alert) {
    int temp;
    if (pid == 0 || memory.Read(base, sizeof(int), &temp) != KERN_SUCCESS) {
        pid = xnu_proc::PidFromName(const_cast<char *>(ProcessName.c_str()));
        if (pid == 0) {
            emit GameNotFound(alert);
            return false;
        } else {
            GameProc->Attach(pid);
            emit GameFound(pid);
            emit GameStatus(CurGameMode(), CurGameUI(), CurGameTime(), CurrentWave(), PlantsCount(), ZombiesCount(),
                            RefreshCountdown(), HugeWaveCountdown());
        }
    }
    return true;
}

inline int PvZ::CurGameMode() {
    if (CurGameUI() == 2 || CurGameUI() == 3)
        return ReadMemory<int>(base, 0x7C0);
    else return 0;
}

inline int PvZ::CurGameUI() {
    return ReadMemory<int>(base, 0x7C4);
}

inline int PvZ::CurGameTime() {
    if (CurGameUI() == 2 || CurGameUI() == 3)
        return ReadMemory<int>(base, 0x780, 0x5560);
    else return 0;
}

inline int PvZ::CurrentWave() {
    if (CurGameUI() == 2 || CurGameUI() == 3)
        return ReadMemory<int>(base, 0x780, 0x5570);
    else return 0;
}

inline int PvZ::PlantsCount() {
    if (CurGameUI() == 2 || CurGameUI() == 3)
        return ReadMemory<int>(base, 0x780, 0xB0);
    else return 0;
}

inline int PvZ::ZombiesCount() {
    if (CurGameUI() == 2 || CurGameUI() == 3)
        return ReadMemory<int>(base, 0x780, 0x94);
    else return 0;
}

inline int PvZ::RefreshCountdown() {
    if (CurGameUI() == 2 || CurGameUI() == 3)
        return ReadMemory<int>(base, 0x780, 0x5590);
    else return 0;
}

inline int PvZ::HugeWaveCountdown() {
    if (CurGameUI() == 2 || CurGameUI() == 3)
        return ReadMemory<int>(base, 0x780, 0x5598);
    else return 0;
}

inline int PvZ::CurScene() {
    return ReadMemory<int>(base, 0x780, 0x5540);
}

inline int PvZ::CurRowCount() {
    auto scene = ReadMemory<int>(base, 0x780, 0x5540);
    return (scene == 2 || scene == 3) ? 6 : 5;
}

//Status

void PvZ::FindGameProc() {
    isGameOn();
}

void PvZ::GetGameStatus(bool alert) {
    if (isGameOn(alert)) {
        emit GameStatus(CurGameMode(), CurGameUI(), CurGameTime(), CurrentWave(), PlantsCount(), ZombiesCount(),
                        RefreshCountdown(), HugeWaveCountdown());
    }
}

//Resource

void PvZ::ModifySun(int sun) {
    if (isGameOn())
        WriteMemory<int>(sun, base, 0x780, 0x5554);
}

void PvZ::ModifyMoney(int money) {
    if (isGameOn())
        WriteMemory<int>(money, base, 0x7F4, 0x38);
}

void PvZ::LockSun(bool on) {
    if (isGameOn()) {
        if (on) {
            WriteMemory({0x90, 0x90, 0x90, 0x90, 0x90, 0x90}, 0x1C72F);
            WriteMemory({0x90, 0x90, 0x90, 0x90, 0x90, 0x90}, 0x18E8F);
        } else {
            WriteMemory({0x89, 0x90, 0x54, 0x55, 0x00, 0x00}, 0x1C72F);
            WriteMemory({0x89, 0x90, 0x54, 0x55, 0x00, 0x00}, 0x18E8F);
        }
    }
}

void PvZ::LockMoney(bool on) {
    if (isGameOn()) {
        if (on)
            WriteMemory({0x90, 0x90, 0x90}, 0x43148);
        else
            WriteMemory({0x89, 0x50, 0x38}, 0x43148);
    }
}

void PvZ::ModifySunLimit(int sun_limit) {
    if (isGameOn()) {
        WriteMemory<int>(sun_limit, 0x18E9F);
        WriteMemory<int>(sun_limit, 0x18EAE);
    }
}

void PvZ::ModifyMoneyLimit(int money_limit) {
    if (isGameOn()) {
        WriteMemory<int>(money_limit, 0x43152);
        WriteMemory<int>(money_limit, 0x4315E);
    }
}

void PvZ::GetSliverSunflower() {
    if (isGameOn()) {
        if (ReadMemory<int>(base, 0x7F4, 0x3C) == 0)
            WriteMemory<int>(1, base, 0x7F4, 0x3C);//Adventure
    }
}

void PvZ::GetGoldenSunflower() {
    if (isGameOn()) {
        auto ptr1 = ReadMemory<uint32_t>(base) + 0x7F4;
        auto ptr2 = ReadMemory<uint32_t>(ptr1) + 0x3C;
        if (ReadMemory<int>(ptr2) < 2)
            WriteMemory<int>(2, ptr2);//Adventure
        for (int i = 0; i < 5; i++)//Survival
        {
            ptr2 += 0x4;
            WriteMemory<int>(5, ptr2);
        }
        for (int i = 0; i < 5; i++)//Survival Hard
        {
            ptr2 += 0x4;
            WriteMemory<int>(10, ptr2);
        }
        ptr2 += 0x14;
        for (int i = 0; i < 20; i++)//Mini-game
        {
            ptr2 += 0x4;
            if (ReadMemory<int>(ptr2) == 0)
                WriteMemory<int>(1, ptr2);
        }
        ptr2 = ReadMemory<uint32_t>(ptr1) + 0x104;
        for (int i = 0; i < 9; i++)//VaseBreaker
        {
            ptr2 += 0x4;
            if (ReadMemory<int>(ptr2) == 0)
                WriteMemory<int>(1, ptr2);
        }
        ptr2 += 0x4;
        for (int i = 0; i < 9; i++)//I,Zombie
        {
            ptr2 += 0x4;
            if (ReadMemory<int>(ptr2) == 0)
                WriteMemory<int>(1, ptr2);
        }
    }
}

void PvZ::GetAllItems() {
    if (isGameOn()) {
        auto ptr1 = ReadMemory<uint32_t>(base) + 0x7F4;
        auto ptr2 = ReadMemory<uint32_t>(ptr1) + 0x1CC;
        for (int i = 0; i < 9; i++)//Plants
        {
            ptr2 += 0x4;
            WriteMemory<int>(1, ptr2);
        }
        ptr2 = ReadMemory<uint32_t>(ptr1) + 0x204;
        WriteMemory<int>(1, ptr2);//Golden Watering Can
        ptr2 += 0x4;
        WriteMemory<int>(1020, ptr2);//Plant Fertilizer
        ptr2 += 0x4;
        WriteMemory<int>(1020, ptr2);//Bug Spray
        ptr2 += 0x4;
        WriteMemory<int>(1, ptr2);//Phonograph
        ptr2 += 0x4;
        WriteMemory<int>(1, ptr2);//Gardening Glove
        ptr2 += 0x4;
        WriteMemory<int>(1, ptr2);//Mushroom Garden
        ptr2 += 0x4;
        WriteMemory<int>(1, ptr2);//Wheel Barrow
        ptr2 += 0x4;
        if (ReadMemory<int>(ptr2) == 0) {
            time_t t;
            time(&t);
            WriteMemory<int>(int(t), ptr2);//Stinky the Snail
        }
        ptr2 += 0x4;
        WriteMemory<int>(4, ptr2);//Slots
        ptr2 += 0x4;
        WriteMemory<int>(1, ptr2);//Pool Cleaner
        ptr2 += 0x4;
        WriteMemory<int>(1, ptr2);//Roof Cleaner
        ptr2 += 0x8;
        WriteMemory<int>(1, ptr2);//Aquarium Garden
        ptr2 += 0x4;
        WriteMemory<int>(1010, ptr2);//Chocolate
        ptr2 += 0x4;
        WriteMemory<int>(1, ptr2);//Wisdom Tree
        ptr2 += 0x4;
        WriteMemory<int>(1010, ptr2);//Wisdom Tree Fertilizer
        ptr2 += 0x4;
        WriteMemory<int>(1, ptr2);//Wall-nut First Aid
    }
}

void PvZ::FinishAchievements() {
    if (isGameOn()) {
        auto ptr1 = ReadMemory<uint32_t>(base) + 0x7F4;
        auto ptr2 = ReadMemory<uint32_t>(ptr1) + 0x0B;
        for (int i = 0; i < 20; i++) {
            ptr2++;
            WriteMemory<bool>(true, ptr2);
        }
    }
}

void PvZ::ModifyResourceValue(int type, int value) {
    if (isGameOn()) {
        uint32_t Address[6] = {0x9B512, 0x9B4FE, 0x9B526, 0x9B547, 0x9B556, 0x9B565};
        WriteMemory<int>(value, Address[type]);
    }
}

void PvZ::GetResourceValue(int type) {
    if (isGameOn(false)) {
        uint32_t Address[6] = {0x9B512, 0x9B4FE, 0x9B526, 0x9B547, 0x9B556, 0x9B565};
        auto value = ReadMemory<int>(Address[type]);
        emit ResourceValue(value);
    }
}

void PvZ::AutoCollect(bool on) {
    if (isGameOn()) {
        std::array<byte, 19> ar = {0x8B, 0x45, 0x08, 0x89, 0x04, 0x24, 0xE8, 0x02, 0x00, 0x00, 0x00,
                                   0xEB, 0x06, 0x68, 0x32, 0xBC, 0x09, 0x00, 0xC3};
        CodeInject(on, 0x9FF82, ar, 6);
    }
}

void PvZ::NoMoneyDrops(bool on) {
    if (isGameOn()) {
        std::array<byte, 22> ar = {0x8B, 0x40, 0x58, 0x83, 0xF8, 0x03, 0x7F, 0x09, 0x8B, 0x45, 0x0C,
                                   0x8B, 0x00, 0xC6, 0x40, 0x38, 0x01, 0x8B, 0x45, 0x0C, 0x8B, 0x00};
        CodeInject(on, 0x1C5A5, ar, 7);
    }
}

void PvZ::NoSunFalling(bool on) {
    if (isGameOn()) {
        if (on)
            WriteMemory<byte>(0x81, 0x29769);
        else
            WriteMemory<byte>(0x85, 0x29769);
    }
}

void PvZ::KeepSunFalling(bool on) {
    if (isGameOn()) {
        if (on)
            WriteMemory({0x90, 0x90, 0x90, 0x90, 0x90, 0x90}, 0x29768);
        else
            WriteMemory({0x0F, 0x85, 0xBA, 0x00, 0x00, 0x00}, 0x29768);
    }
}

//Level

void PvZ::ModifyMode(int mode) {
    if (isGameOn() && (CurGameUI() == 2 || CurGameUI() == 3))
        WriteMemory<int>(mode, base, 0x7C0);
}

void PvZ::StartLevel(int mode) {
    if (isGameOn()) {
        if (CurGameUI() == 1) {
            // code.asm_init();
            // code.asm_mov_exx_dword_ptr(Reg::EAX, 0x35EE64);
            // code.asm_mov_ptr_esp_add_exx(0x0, Reg::EAX);
            // code.asm_call(0xB6C5C);
            // code.asm_mov_dword_ptr_esp_add(0x8, 1);
            // code.asm_mov_dword_ptr_esp_add(0x4, mode);
            // code.asm_mov_exx_dword_ptr(Reg::EAX, 0x35EE64);
            // code.asm_mov_ptr_esp_add_exx(0x0, Reg::EAX);
            // code.asm_call(0xB79F4);
            // code.asm_ret();
            // code.asm_code_inject();
            WriteMemory<int>(mode, 0xC8F08);
            WriteMemory<byte>(0x80, 0xC8D4D);
            WriteMemory<bool>(true, base, 0x26C, 0x7C, 0xC0);
            while (CurGameUI() == 1) {
                usleep(10000);
            }
            WriteMemory<byte>(0x8E, 0xC8D4D);
            WriteMemory<int>(0, 0xC8F08);
        } else if (CurGameUI() == 7) {
            code.asm_init();
            code.asm_mov_dword_ptr_esp_add(0x4, mode + 199);
            code.asm_mov_exx_dword_ptr(Reg::EAX, 0x35EE64);
            code.asm_mov_exx_dword_ptr_exx_add(Reg::EAX, 0x26C);
            code.asm_mov_exx_dword_ptr_exx_add(Reg::EAX, 0x7C);
            code.asm_mov_ptr_esp_add_exx(0x0, Reg::EAX);
            WriteMemory({0x90, 0x90}, 0x9A2F3);
            code.asm_call(0x9A2B6);
            code.asm_ret();
            code.asm_code_inject();
            WriteMemory({0x78, 0x3E}, 0x9A2F3);
        }
    }
}

void PvZ::ModifyEndlessLevel(int level) {
    if (isGameOn() && (CurGameMode() == 60 || CurGameMode() == 70 || (CurGameMode() >= 11 && CurGameMode() <= 15)))
        WriteMemory<int>(level, base, 0x780, 0x154, 0x6C);
}

void PvZ::ModifyAdventureLevel(int level) {
    if (isGameOn()) {
        WriteMemory<int>(level, base, 0x7F4, 0x34);
        if ((CurGameUI() == 2) || (CurGameUI() == 3))
            WriteMemory<int>(level, base, 0x780, 0x5544);
    }
}

void PvZ::ModifyAdventureCompletionTimes(int times) {
    if (isGameOn())
        WriteMemory<int>(times, base, 0x7F4, 0x3C);
}

void PvZ::ImmediatelyWin() {
    if (isGameOn() && CurGameUI() == 3) {
        code.asm_init();
        code.asm_mov_exx_dword_ptr(Reg::EAX, 0x35EE64);
        code.asm_mov_exx_dword_ptr_exx_add(Reg::EAX, 0x780);
        code.asm_mov_ptr_esp_add_exx(0x0, Reg::EAX);
        code.asm_call(0x1D63A);
        code.asm_ret();
        code.asm_code_inject();
    }
}

void PvZ::ShowLimboPage(bool on) {
    if (isGameOn()) {
        if (on)
            WriteMemory<byte>(0x00, 0x9AAD5);
        else
            WriteMemory<byte>(0x01, 0x9AAD5);
    }
}

void PvZ::UnlockAllModes(bool on) {
    if (isGameOn()) {
        if (on) {
            WriteMemory<byte>(0x00, 0xC4CA2);//Mini-game Mode
            WriteMemory<byte>(0x00, 0xC4CAC);//Puzzle Mode
            WriteMemory<byte>(0x00, 0xC4CB6);//Survival Mode
            WriteMemory({0x90, 0x90}, 0xBC1F9);//Almanac
            WriteMemory({0x90, 0x90}, 0xBC26B);//Shop
            WriteMemory({0x90, 0x90}, 0xBC2F9);//Zen Garden
            WriteMemory({0x90, 0x90}, 0x92A0B);//Credits
            WriteMemory({0x31, 0xC0, 0x90}, 0x97FFB);//Levels
        } else {
            WriteMemory<byte>(0x01, 0xC4CA2);//Mini-game Mode
            WriteMemory<byte>(0x01, 0xC4CAC);//Puzzle Mode
            WriteMemory<byte>(0x01, 0xC4CB6);//Survival Mode
            WriteMemory({0x74, 0x09}, 0xBC1F9);//Almanac
            WriteMemory({0x74, 0x09}, 0xBC26B);//Shop
            WriteMemory({0x74, 0x09}, 0xBC2F9);//Zen Garden
            WriteMemory({0x75, 0x24}, 0x92A0B);//Credits
            WriteMemory({0x8B, 0x45, 0xAC}, 0x97FFB);//Levels
        }
    }
}

void PvZ::ItsRaining(bool on) {
    if (isGameOn()) {
        if (on) {
            WriteMemory({0x90, 0x90}, 0x3160D);
            WriteMemory({0x90, 0x90}, 0xAA635);
            WriteMemory<byte>(0xEB, 0xB4697);
        } else {
            WriteMemory({0x75, 0x09}, 0x3160D);
            WriteMemory({0x75, 0x06}, 0xAA635);
            WriteMemory<byte>(0x74, 0xB4697);
        }
    }
}

void PvZ::ZombieQuick(bool on) {
    if (isGameOn()) {
        if (on)
            WriteMemory({0x90, 0x90}, 0xB48A3);
        else
            WriteMemory({0x75, 0x0E}, 0xB48A3);
    }
}

void PvZ::BungeeBlitz(bool on) {
    if (isGameOn()) {
        if (on)
            WriteMemory<byte>(0x80, 0x2CFE6);
        else
            WriteMemory<byte>(0x84, 0x2CFE6);
    }
}

void PvZ::HighGravity(bool on) {
    if (isGameOn()) {
        if (on) {
            WriteMemory({0x90, 0x90}, 0x105AC5);
            WriteMemory({0x90, 0x90}, 0x10686F);
            WriteMemory({0x90, 0x90}, 0xE589C);
        } else {
            WriteMemory({0x75, 0x1C}, 0x105AC5);
            WriteMemory({0x75, 0x61}, 0x10686F);
            WriteMemory({0x75, 0x6C}, 0xE589C);
        }
    }
}

void PvZ::GraveDanger(bool on) {
    if (isGameOn()) {
        if (on)
            WriteMemory({0x90, 0x90}, 0xA9D02);
        else
            WriteMemory({0x75, 0x4F}, 0xA9D02);
    }
}

void PvZ::SunnyDay(bool on) {
    if (isGameOn()) {
        if (on)
            WriteMemory({0x90, 0x90}, 0x297F6);
        else
            WriteMemory({0x75, 0x07}, 0x297F6);
    }
}

void PvZ::DarkStormyNight(bool on) {
    if (isGameOn()) {
        if (on) {
            WriteMemory<byte>(0xEB, 0x315F9);
            WriteMemory<byte>(0xEB, 0xAA625);
            WriteMemory<byte>(0xEB, 0x10845);
            WriteMemory({0x90, 0x90}, 0xB4638);
            WriteMemory({0x90, 0x90}, 0xB46A8);
            WriteMemory({0x90, 0x90}, 0xAA66A);
        } else {
            WriteMemory<byte>(0x75, 0x315F9);
            WriteMemory<byte>(0x75, 0xAA625);
            WriteMemory<byte>(0x75, 0x10845);
            WriteMemory({0x74, 0x0B}, 0xB4638);
            WriteMemory({0x74, 0x06}, 0xB46A8);
            WriteMemory({0x74, 0x12}, 0xAA66A);
        }
    }
}

void PvZ::BigTime(bool on) {
    if (isGameOn()) {
        if (on) {
            WriteMemory({0x90, 0x90}, 0x39917);
            WriteMemory({0x90, 0x90}, 0x3AF7A);
            WriteMemory({0x90, 0x90}, 0x35354);
            WriteMemory({0x90, 0x90}, 0x3BD16);
            WriteMemory({0x90, 0x90}, 0xF7946);
        } else {
            WriteMemory({0x75, 0x69}, 0x39917);
            WriteMemory({0x75, 0x30}, 0x3AF7A);
            WriteMemory({0x75, 0x7C}, 0x35354);
            WriteMemory({0x75, 0x54}, 0x3BD16);
            WriteMemory({0x75, 0x22}, 0xF7946);
        }
    }
}

void PvZ::ColumnLike(bool on) {
    if (isGameOn()) {
        if (on) {
            WriteMemory<byte>(0x80, 0x2BFCB);
            WriteMemory<byte>(0x80, 0x103572);
        } else {
            WriteMemory<byte>(0x85, 0x2BFCB);
            WriteMemory<byte>(0x85, 0x103572);
        }
    }
}

//Slot

void PvZ::ModifySlotCount(int count) {
    if (isGameOn() && (CurGameUI() == 2 || CurGameUI() == 3)) {
        WriteMemory<int>(count, base, 0x780, 0x138, 0x24);
        
        //Refresh card position
        //in game
        code.asm_init();
        code.asm_mov_exx_dword_ptr(Reg::EAX, 0x35EE64);
        code.asm_mov_exx_dword_ptr_exx_add(Reg::EAX, 0x780);
        code.asm_mov_exx_dword_ptr_exx_add(Reg::EAX, 0x138);
        code.asm_mov_ptr_esp_add_exx(0x0, Reg::EAX);
        code.asm_call(0xFA4D4);
        code.asm_ret();
        WriteMemory({0x90, 0x90, 0x90}, 0xFA4EE);
        code.asm_code_inject();
        WriteMemory({0x89, 0x50, 0x24}, 0xFA4EE);
        if (CurGameUI() == 2) {//choose seed
            code.asm_init();
            code.asm_mov_exx_dword_ptr(Reg::EAX, 0x35EE64);
            code.asm_mov_exx_dword_ptr_exx_add(Reg::EAX, 0x78C);
            code.asm_mov_ptr_esp_add_exx(0x0, Reg::EAX);
            code.asm_call(0x91EEA);
            code.asm_ret();
            code.asm_code_inject();
        }
    }
}

void PvZ::SlotVisible(bool visible) {
    if (isGameOn() && (CurGameUI() == 2 || CurGameUI() == 3))
        WriteMemory<bool>(visible, base, 0x780, 0x138, 0x18);
}

void PvZ::ShowShovel(bool visible) {
    if (isGameOn() && (CurGameUI() == 2 || CurGameUI() == 3))
        WriteMemory<bool>(visible, base, 0x780, 0x55E5);
}

void PvZ::ModifySlotContent(int SlotID, int type, bool isImitater, bool visible) {
    if (isGameOn() && CurGameUI() == 3) {
        if (isImitater) {
            WriteMemory<int>(type, base, 0x780, 0x138, 0x60 + SlotID * 0x50);
            type = 48;
        }
        WriteMemory<int>(type, base, 0x780, 0x138, 0x5C + SlotID * 0x50);
        WriteMemory<bool>(visible, base, 0x780, 0x138, 0x40 + SlotID * 0x50);
    }
}

void PvZ::NoCoolDown(bool on) {
    if (isGameOn()) {
        if (on) {
            WriteMemory<byte>(0x70, 0xF8334);
            //WriteMemory<byte>(0xEB,0xF848C);
            WriteMemory({0x31, 0xC0}, 0xF84A8);
        } else {
            WriteMemory<byte>(0x7E, 0xF8334);
            //WriteMemory<byte>(0x75,0xF848C);
            WriteMemory({0x89, 0xC8}, 0xF84A8);
        }
    }
}

void PvZ::FreelyPlant(bool on) {
    if (isGameOn()) {
        if (on) {
            WriteMemory<byte>(0x8D, 0x2A38C);
            WriteMemory({0x90, 0x90}, 0x103227);
            WriteMemory<byte>(0x81, 0xAC869);
        } else {
            WriteMemory<byte>(0x84, 0x2A38C);
            WriteMemory({0x74, 0x5D}, 0x103227);
            WriteMemory<byte>(0x84, 0xAC869);
        }
    }
}

void PvZ::PurplePlantAvailable(bool on) {
    if (isGameOn()) {
        if (on) {
            WriteMemory({0xB8, 0x01, 0x00, 0x00, 0x00, 0xC3}, 0x1F3E0);
            WriteMemory<byte>(0xEB, 0x1F12B);
            WriteMemory<byte>(0xEB, 0x1F232);
        } else {
            WriteMemory({0x55, 0x89, 0xE5, 0x83, 0xEC, 0x28}, 0x1F3E0);
            WriteMemory<byte>(0x74, 0x1F12B);
            WriteMemory<byte>(0x74, 0x1F232);
        }
    }
}

void PvZ::AlwaysShovel(bool on) {
    if (isGameOn() && CurGameUI() == 3) {
        if (on) {
            WriteMemory({0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90}, 0x12FA0);
            WriteMemory<int>(6, base, 0x780, 0x12C, 0x30);
        } else {
            WriteMemory({0xC7, 0x40, 0x30, 0x00, 0x00, 0x00, 0x00}, 0x12FA0);
            WriteMemory<int>(0, base, 0x780, 0x12C, 0x30);
        }
    }
}

void PvZ::HideMenu(bool on) {
    if (isGameOn() && (CurGameUI() == 2 || CurGameUI() == 3)) {
        if (on)
            WriteMemory<int>(900, base, 0x780, 0x13C, 0xC);
        else
            WriteMemory<int>(681, base, 0x780, 0x13C, 0xC);
    }
}

void PvZ::ModifyCardProperty(int CardID, int cost, int cooldowntime) {
    if (isGameOn()) {
        uint32_t CostAddress = 0;
        if (CardID <= 52) {
            if (CurGameMode() == 20 || CurGameMode() == 24) {
                if (CardID == 7)
                    CostAddress = 0x3DA39;
                else if (CardID == 10)
                    CostAddress = 0x3DA4B;
                else if (CardID == 23)
                    CostAddress = 0x3DA5D;
            } else
                CostAddress = 0x35CFC0 + 0x10 + CardID * 0x24;
            WriteMemory<int>(cooldowntime, 0x35CFC0 + 0x14 + CardID * 0x24);
        } else if (CardID >= 56 && CardID <= 74) {
            CostAddress = ReadMemory<uint32_t>(0x2E8D30 + (CardID - 56) * 0x4) + 0x3;
        } else if (CardID == 54)
            CostAddress = 0x3DA6F;
        else if (CardID == 55)
            CostAddress = 0x3DA81;
        if (CostAddress)
            WriteMemory<int>(cost, CostAddress);
    }
}

void PvZ::GetCardProperty(int CardID) {
    if (isGameOn(false)) {
        int cost = 0, cooldowntime = 0;
        if (CardID <= 52) {
            emit CardPlant();
            cost = ReadMemory<int>(0x35CFD0 + CardID * 0x24);
            cooldowntime = ReadMemory<int>(0x35CFD4 + CardID * 0x24);
            if (CurGameMode() == 20 || CurGameMode() == 24) {
                if (CardID == 7)
                    cost = ReadMemory<int>(0x3DA39);
                else if (CardID == 10)
                    cost = ReadMemory<int>(0x3DA4B);
                else if (CardID == 23)
                    cost = ReadMemory<int>(0x3DA5D);
            }
        } else {
            if (CardID >= 54) {
                emit CardZombie();
                if (CardID == 54)
                    cost = ReadMemory<int>(0x3DA6F);
                else if (CardID == 55)
                    cost = ReadMemory<int>(0x3DA81);
                else {
                    auto address = ReadMemory<uint32_t>(0x2E8D30 + (CardID - 56) * 0x4) + 0x3;
                    cost = ReadMemory<int>(address);
                }
            } else {
                emit CardOthers();
            }
        }
        emit CardProperty(cost, cooldowntime);
    }
}

//Scene

void PvZ::ModifyGameScene(int scene) {
    if (isGameOn() && (CurGameUI() == 2 || CurGameUI() == 3)) {
        WriteMemory<int>(scene, base, 0x780, 0x5540);
        
        //Load resources
        code.asm_init();
        code.asm_mov_exx_dword_ptr(Reg::EAX, 0x35EE64);
        code.asm_mov_exx_dword_ptr_exx_add(Reg::EAX, 0x780);
        code.asm_mov_ptr_esp_add_exx(0x0, Reg::EAX);
        code.asm_call(0x10062);
        code.asm_ret();
        code.asm_code_inject();
        
        //write grid type && row type
        // 0.none 1.land 2.water
        int row_0[6] = {1, 1, 1, 1, 1, 0};
        int row_1[6] = {1, 1, 2, 2, 1, 1};
        switch (scene) {
            case 0:
            case 1:
            case 4:
            case 5:
            case 6:
            case 7:
            case 9:
                for (size_t r = 0; r < 6; r++)
                    ModifyRowType(r, row_0[r]);
                break;
            case 2:
            case 3:
            case 8:
                for (size_t r = 0; r < 6; r++)
                    ModifyRowType(r, row_1[r]);
                break;
            default:
                break;
        }
        
        // 1.lawn 2.bare 3.pool
        int block_0[6] = {1, 1, 1, 1, 1, 2};
        int block_1[6] = {1, 1, 3, 3, 1, 1};
        switch (scene) {
            case 0:
            case 1:
            case 4:
            case 5:
            case 6:
            case 7:
            case 9:
                for (size_t r = 0; r < 6; r++)
                    for (size_t c = 0; c < 9; c++)
                        ModifyGridType(r, c, block_0[r]);
                break;
            case 2:
            case 3:
            case 8:
                for (size_t r = 0; r < 6; r++)
                    for (size_t c = 0; c < 9; c++)
                        ModifyGridType(r, c, block_1[r]);
                break;
            default:
                break;
        }
    }
}

void PvZ::ModifyGridType(int row, int column, int type) {
    if (isGameOn() && (CurGameUI() == 2 || CurGameUI() == 3)) {
        uint32_t row_count = CurRowCount();
        uint32_t col_count = 9;
        if (row == -1 && column == -1)
            for (size_t r = 0; r < row_count; r++)
                for (size_t c = 0; c < col_count; c++)
                    WriteMemory<int>(type, base, 0x780, 0x15C + r * 0x04 + c * 0x18);
        else if (row != -1 && column == -1)
            for (size_t c = 0; c < col_count; c++)
                WriteMemory<int>(type, base, 0x780, 0x15C + row * 0x04 + c * 0x18);
        else if (row == -1 && column != -1)
            for (size_t r = 0; r < row_count; r++)
                WriteMemory<int>(type, base, 0x780, 0x15C + r * 0x04 + column * 0x18);
        else
            WriteMemory<int>(type, base, 0x780, 0x15C + column * 0x18 + row * 0x04);
    }
}

void PvZ::ModifyRowType(int row, int type) {
    if (isGameOn() && (CurGameUI() == 2 || CurGameUI() == 3)) {
        if (row == -1)
            for (size_t r = 0; r < 6; r++)
                WriteMemory<int>(type, base, 0x780, 0x5CC + r * 0x04);
        else
            WriteMemory<int>(type, base, 0x780, 0x5CC + row * 0x04);
    }
}

void PvZ::PutLadder(int row, int column) {
    if (isGameOn() && (CurGameUI() == 2 || CurGameUI() == 3)) {
        int row_count = CurRowCount();
        int col_count = 9;
        code.asm_init();
        if (row == -1 && column == -1)
            for (int r = 0; r < row_count; r++)
                for (int c = 0; c < col_count; c++)
                    code.asm_put_ladder(r, c);
        else if (row != -1 && column == -1)
            for (int c = 0; c < col_count; c++)
                code.asm_put_ladder(row, c);
        else if (row == -1 && column != -1)
            for (int r = 0; r < row_count; r++)
                code.asm_put_ladder(r, column);
        else
            code.asm_put_ladder(row, column);
        code.asm_ret();
        code.asm_code_inject();
    }
}

void PvZ::PutGrave(int row, int column) {
    if (isGameOn() && (CurGameUI() == 2 || CurGameUI() == 3)) {
        int row_count = CurRowCount();
        int col_count = 9;
        code.asm_init();
        if (row == -1 && column == -1)
            for (int r = 0; r < row_count; r++)
                for (int c = 0; c < col_count; c++)
                    code.asm_put_grave(r, c);
        else if (row != -1 && column == -1)
            for (int c = 0; c < col_count; c++)
                code.asm_put_grave(row, c);
        else if (row == -1 && column != -1)
            for (int r = 0; r < row_count; r++)
                code.asm_put_grave(r, column);
        else
            code.asm_put_grave(row, column);
        code.asm_ret();
        code.asm_code_inject();
    }
}

void PvZ::PutRake(int row, int column) {
    if (isGameOn() && (CurGameUI() == 2 || CurGameUI() == 3)) {
        int row_count = CurRowCount();
        int col_count = 9;
        code.asm_init();
        if (row == -1 && column == -1)
            for (int r = 0; r < row_count; r++)
                for (int c = 0; c < col_count; c++)
                    code.asm_put_rake(r, c);
        else if (row != -1 && column == -1)
            for (int c = 0; c < col_count; c++)
                code.asm_put_rake(row, c);
        else if (row == -1 && column != -1)
            for (int r = 0; r < row_count; r++)
                code.asm_put_rake(r, column);
        else
            code.asm_put_rake(row, column);
        code.asm_ret();
        
        WriteMemory({0x81, 0x5A, 0x01}, 0x26DC7);
        WriteMemory<byte>(0x0C, 0x26F48);//Get row
        WriteMemory<byte>(0x10, 0x26F51);//Get column
        code.asm_code_inject();
        WriteMemory({0x84, 0x93, 0x02}, 0x26DC7);
        WriteMemory<byte>(0xE0, 0x26F48);
        WriteMemory<byte>(0xE8, 0x26F51);
    }
}

void PvZ::PutCoin(int type, int row, int column) {
    if (isGameOn() && (CurGameUI() == 2 || CurGameUI() == 3)) {
        int scene = CurScene();
        int row_count = CurRowCount();
        int col_count = 9;
        code.asm_init();
        if (row == -1 && column == -1)
            for (int r = 0; r < row_count; r++)
                for (int c = 0; c < col_count; c++)
                    code.asm_put_coin(r, c, type, scene);
        else if (row != -1 && column == -1)
            for (int c = 0; c < col_count; c++)
                code.asm_put_coin(row, c, type, scene);
        else if (row == -1 && column != -1)
            for (int r = 0; r < row_count; r++)
                code.asm_put_coin(r, column, type, scene);
        else
            code.asm_put_coin(row, column, type, scene);
        code.asm_ret();
        code.asm_code_inject();
    }
}

void PvZ::PumpkinLadder(bool imitater_only) {
    if (isGameOn() && (CurGameUI() == 2 || CurGameUI() == 3)) {
        auto grid_item_count_max = ReadMemory<uint32_t>(base, 0x780, 0x114);
        auto grid_item_offset = ReadMemory<uint32_t>(base, 0x780, 0x110);
        bool has_ladder[6][9] = {{false}};
        
        for (unsigned int i = 0; i < grid_item_count_max; i++) {
            auto grid_item_disappeared = ReadMemory<bool>(grid_item_offset + 0x20 + i * 0xEC);
            auto grid_item_type = ReadMemory<int>(grid_item_offset + 0x08 + i * 0xEC); // 3 for Ladder
            auto grid_item_row = ReadMemory<int>(grid_item_offset + 0x14 + i * 0xEC);
            auto grid_item_col = ReadMemory<int>(grid_item_offset + 0x10 + i * 0xEC);
            if (!grid_item_disappeared && grid_item_type == 3)
                has_ladder[grid_item_row][grid_item_col] = true;
        }
        
        auto plant_count_max = ReadMemory<uint32_t>(base, 0x780, 0xA4);
        auto plant_offset = ReadMemory<uint32_t>(base, 0x780, 0xA0);
        code.asm_init();
        for (unsigned int i = 0; i < plant_count_max; i++) {
            auto plant_existing = ReadMemory<uint16_t>(plant_offset + 0x14A + 0x14C * i);
            auto plant_disappeared = ReadMemory<bool>(plant_offset + 0x141 + 0x14C * i);
            auto plant_crushed = ReadMemory<bool>(plant_offset + 0x142 + 0x14C * i);
            auto plant_type = ReadMemory<int>(plant_offset + 0x24 + 0x14C * i);
            if (plant_existing && !plant_disappeared && !plant_crushed && plant_type == 30) // 30 for Pumpkin
            {
                auto plant_row = ReadMemory<int>(plant_offset + 0x1C + 0x14C * i);
                auto plant_col = ReadMemory<int>(plant_offset + 0x28 + 0x14C * i);
                auto is_imitater = ReadMemory<int>(plant_offset + 0x138 + 0x14C * i) == 48;
                auto block_type = ReadMemory<int>(base, 0x780,
                                                  0x15C + 0x04 * plant_row + 0x18 * plant_col); // 1 for lawn
                if (plant_col != 0 && block_type == 1 && !has_ladder[plant_row][plant_col])
                    if (!imitater_only || is_imitater)
                        code.asm_put_ladder(plant_row, plant_col);
            }
        }
        code.asm_ret();
        code.asm_code_inject();
    }
}

void PvZ::SetPlant(int row, int column, int type, bool imitater) {
    if (isGameOn() && (CurGameUI() == 2 || CurGameUI() == 3)) {
        int row_count = CurRowCount();        // 6 or 5
        int col_count = (type == 47 ? 8 : 9); // Cob Cannon cannot be planted at col 9
        int width = (type == 47 ? 2 : 1);     // Cob Cannon's width is 2
        int mode = CurGameMode();
        bool iz_style = (mode >= 61 && mode <= 70);
        code.asm_init();
        if (row == -1 && column == -1)
            for (int r = 0; r < row_count; r++)
                for (int c = 0; c < col_count; c += width)
                    code.asm_set_plant(r, c, type, imitater, iz_style);
        else if (row != -1 && column == -1)
            for (int c = 0; c < col_count; c += width)
                code.asm_set_plant(row, c, type, imitater, iz_style);
        else if (row == -1 && column != -1)
            for (int r = 0; r < row_count; r++)
                code.asm_set_plant(r, column, type, imitater, iz_style);
        else
            code.asm_set_plant(row, column, type, imitater, iz_style);
        code.asm_ret();
        code.asm_code_inject();
    }
}

void PvZ::SetZombie(int row, int column, int type) {
    if (isGameOn() && (CurGameUI() == 2 || CurGameUI() == 3)) {
        if (type == 25) {//Dr.Zomboss
            SpawnZombie(25, 1);
            return;
        }
        int row_count = CurRowCount();
        int col_count = 9;
        code.asm_init();
        if (row == -1 && column == -1)
            for (int r = 0; r < row_count; r++)
                for (int c = 0; c < col_count; c++)
                    code.asm_set_zombie(r, c, type);
        else if (row != -1 && column == -1)
            for (int c = 0; c < col_count; c++)
                code.asm_set_zombie(row, c, type);
        else if (row == -1 && column != -1)
            for (int r = 0; r < row_count; r++)
                code.asm_set_zombie(r, column, type);
        else
            code.asm_set_zombie(row, column, type);
        code.asm_ret();
        code.asm_code_inject();
    }
}

void PvZ::SpawnZombie(int type, int count) {
    if (isGameOn() && (CurGameUI() == 2 || CurGameUI() == 3)) {
        int zombie_count_limit = ReadMemory<int>(base, 0x780, 0x8C);
        if (count > zombie_count_limit)
            count = zombie_count_limit;
        code.asm_init();
        for (size_t i = 0; i < count; i++) {
            code.asm_spawn_zombie(type);
        }
        code.asm_ret();
        code.asm_code_inject();
    }
}

void PvZ::NoFog(bool on) {
    if (isGameOn()) {
        if (on)
            //WriteMemory<byte>(0x81,0x1896C);
            WriteMemory({0x90, 0x90}, 0x1086B);
        else
            //WriteMemory<byte>(0x84,0x1896C);
            WriteMemory({0x74, 0x09}, 0x1086B);
    }
}

void PvZ::BigFog(bool on) {
    if (isGameOn()) {
        if (on)
            WriteMemory({0xB8, 0xFF, 0x00, 0x00, 0x00, 0x90, 0x90}, 0x1895D);
        else
            WriteMemory({0x8B, 0x84, 0x83, 0xBC, 0x04, 0x00, 0x00}, 0x1895D);
    }
}

void PvZ::SeeVase(bool on) {
    if (isGameOn()) {
        if (on)
            WriteMemory(std::array<byte, 11>{0xC7, 0x40, 0x4C, 0x64, 0x00, 0x00, 0x00, 0x90, 0x90, 0x90, 0x90},
                        0x20812E);
        else
            WriteMemory(std::array<byte, 11>{0x8B, 0x40, 0x4C, 0x85, 0xC0, 0x0F, 0x8E, 0xC6, 0x04, 0x00, 0x00},
                        0x20812E);
    }
}

void PvZ::LawnMowersStart() {
    if (isGameOn() && (CurGameUI() == 2 || CurGameUI() == 3)) {
        auto lawn_mower_count_max = ReadMemory<uint32_t>(base, 0x780, 0xF8);
        auto lawn_mower_offset = ReadMemory<uint32_t>(base, 0x780, 0xF4);
        code.asm_init();
        for (size_t i = 0; i < lawn_mower_count_max; i++) {
            auto lawn_mower_disappeared = ReadMemory<bool>(lawn_mower_offset + 0x30 + 0x48 * i);
            if (!lawn_mower_disappeared) {
                uint32_t addr = lawn_mower_offset + 0x48 * i;
                code.asm_mov_exx(Reg::EAX, addr);
                code.asm_mov_ptr_esp_add_exx(0x0, Reg::EAX);
                code.asm_call(0x8C00);
            }
        }
        code.asm_ret();
        code.asm_code_inject();
    }
}

void PvZ::LawnMowersReset() {
    if (isGameOn() && (CurGameUI() == 2 || CurGameUI() == 3)) {
        LawnMowersDisappear();
        code.asm_init();
        code.asm_mov_exx_dword_ptr(Reg::EAX, 0x35EE64);
        code.asm_mov_exx_dword_ptr_exx_add(Reg::EAX, 0x780);
        code.asm_mov_ptr_esp_add_exx(0x0, Reg::EAX);
        code.asm_call(0x10ED2);
        code.asm_ret();
        WriteMemory<byte>(0xEB, 0x10F3D);
        WriteMemory<byte>(0xEB, 0x11017);
        code.asm_code_inject();
        WriteMemory<byte>(0x74, 0x10F3D);
        WriteMemory<byte>(0x74, 0x11017);
        
        auto lawn_mower_count_max = ReadMemory<uint32_t>(base, 0x780, 0xF8);
        auto lawn_mower_offset = ReadMemory<uint32_t>(base, 0x780, 0xF4);
        for (size_t i = 0; i < lawn_mower_count_max; i++) {
            auto lawn_mower_disappeared = ReadMemory<bool>(lawn_mower_offset + 0x30 + 0x48 * i);
            if (!lawn_mower_disappeared) {
                WriteMemory<bool>(true, lawn_mower_offset + 0x31 + 0x48 * i);//visible
                WriteMemory<float>(-21.0, lawn_mower_offset + 0x8 + 0x48 * i);//position
            }
        }
    }
}

void PvZ::LawnMowersDisappear() {
    if (isGameOn() && (CurGameUI() == 2 || CurGameUI() == 3)) {
        auto lawn_mower_count_max = ReadMemory<uint32_t>(base, 0x780, 0xF8);
        auto lawn_mower_offset = ReadMemory<uint32_t>(base, 0x780, 0xF4);
        code.asm_init();
        for (size_t i = 0; i < lawn_mower_count_max; i++) {
            auto lawn_mower_disappeared = ReadMemory<bool>(lawn_mower_offset + 0x30 + 0x48 * i);
            if (!lawn_mower_disappeared) {
                uint32_t addr = lawn_mower_offset + 0x48 * i;
                code.asm_mov_exx(Reg::EAX, addr);
                code.asm_mov_ptr_esp_add_exx(0x0, Reg::EAX);
                code.asm_call(0x925C);
            }
        }
        code.asm_ret();
        code.asm_code_inject();
    }
}

void PvZ::ClearAllPlants() {
    if (isGameOn() && (CurGameUI() == 2 || CurGameUI() == 3)) {
        auto plant_count_max = ReadMemory<uint32_t>(base, 0x780, 0xA4);
        auto plant_offset = ReadMemory<uint32_t>(base, 0x780, 0xA0);
        
        code.asm_init();
        for (size_t i = 0; i < plant_count_max; i++) {
            auto plant_existing = ReadMemory<uint16_t>(plant_offset + 0x14A + 0x14C * i);
            auto plant_disappeared = ReadMemory<bool>(plant_offset + 0x141 + 0x14C * i);
            auto plant_crushed = ReadMemory<bool>(plant_offset + 0x142 + 0x14C * i);
            if (plant_existing && !plant_disappeared && !plant_crushed) {
                uint32_t addr = plant_offset + 0x14C * i;
                code.asm_mov_dword_ptr_esp_add(0x0, addr);
                code.asm_call(0x37522);
            }
        }
        code.asm_ret();
        code.asm_code_inject();
    }
}

void PvZ::ClearAllZombies() {
    if (isGameOn() && (CurGameUI() == 2 || CurGameUI() == 3)) {
        auto zombie_count_max = ReadMemory<uint32_t>(base, 0x780, 0x88);
        auto zombie_offset = ReadMemory<uint32_t>(base, 0x780, 0x84);
        
        code.asm_init();
        for (size_t i = 0; i < zombie_count_max; i++) {
            auto zombie_existing = ReadMemory<uint16_t>(zombie_offset + 0x166 + 0x168 * i);
            auto zombie_disappeared = ReadMemory<bool>(zombie_offset + 0xEC + 0x168 * i);
            if (zombie_existing && !zombie_disappeared) {
                uint32_t addr = zombie_offset + 0x168 * i;
                code.asm_mov_dword_ptr_esp_add(0x0, addr);
                code.asm_call(0xE3AC2);
            }
        }
        code.asm_ret();
        code.asm_code_inject();
    }
}

void PvZ::ClearAllItems() {
    if (isGameOn() && (CurGameUI() == 2 || CurGameUI() == 3)) {
        WriteMemory({0x90, 0x90}, 0x2F3D2);//Can't find a good way...
        usleep(10000);
        WriteMemory({0x75, 0x18}, 0x2F3D2);
    }
}

void PvZ::ClearAllGridItems(int type) {
    if (isGameOn() && (CurGameUI() == 2 || CurGameUI() == 3)) {
        auto griditem_count_max = ReadMemory<uint32_t>(base, 0x780, 0x114);
        auto griditem_offset = ReadMemory<uint32_t>(base, 0x780, 0x110);
        
        code.asm_init();
        for (size_t i = 0; i < griditem_count_max; i++) {
            auto griditem_existing = ReadMemory<uint16_t>(griditem_offset + 0xEA + 0xEC * i);
            auto griditem_disappeared = ReadMemory<bool>(griditem_offset + 0x20 + 0xEC * i);
            auto griditem_type = ReadMemory<int>(griditem_offset + 0x8 + 0xEC * i);
            if (griditem_existing && !griditem_disappeared && griditem_type == type) {
                uint32_t addr = griditem_offset + 0xEC * i;
                code.asm_mov_dword_ptr_esp_add(0x0, addr);
                code.asm_call(0x207090);
            }
        }
        code.asm_ret();
        code.asm_code_inject();
    }
}

//Plants

void PvZ::ModifyPlantHP(int type, int value) {
    if (isGameOn()) {
        int address[8] = {0x39F5E, 0x3A8B4, 0x3A982, 0x3ACD4, 0x3A9BC, 0x3AEE9, 0x3A8E4, 0x3A952};
        WriteMemory<int>(value, address[type]);
    }
}

void PvZ::GetPlantHP(int type) {
    if (isGameOn(false)) {
        int address[8] = {0x39F5E, 0x3A8B4, 0x3A982, 0x3ACD4, 0x3A9BC, 0x3AEE9, 0x3A8E4, 0x3A952};
        int value = ReadMemory<int>(address[type]);
        emit PlantHP(value);
    }
}

void PvZ::ModifyPlantAttackInterval(int type, int value) {
    if (isGameOn()) {
        WriteMemory<int>(value, 0x35CFDC + type * 0x24);
    }
}

void PvZ::GetPlantAttackInterval(int type) {
    if (isGameOn(false)) {
        int value = ReadMemory<int>(0x35CFDC + type * 0x24);
        emit PlantAttackInterval(value);
    }
}

void PvZ::ModifyProjectileDamage(int type, int damage) {
    if (isGameOn()) {
        uint32_t address = 0;
        if (type <= 12) {
            address = 0x361408 + type * 0xC;
            if (type > 10)
                address += 0xC;
        } else if (type == 13)
            address = 0xE6CC4;
        else if (type == 14)
            address = 0x22C07;
        else if (type == 15)
            address = 0x40311;
        else if (type == 16)
            address = 0xE63FA;
        WriteMemory<int>(damage, address);
    }
}

void PvZ::GetProjectileDamage(int type) {
    if (isGameOn(false)) {
        uint32_t address = 0;
        if (type <= 12) {
            address = 0x361408 + type * 0xC;
            if (type > 10)
                address += 0xC;
        } else if (type == 13)
            address = 0xE6CC4;
        else if (type == 14)
            address = 0x22C07;
        else if (type == 15)
            address = 0x40311;
        else if (type == 16)
            address = 0xE63FA;
        int damage = ReadMemory<int>(address);
        emit ProjectileDamage(damage);
    }
}

void PvZ::PlantInvincible(bool on) {
    if (isGameOn()) {
        if (on) {
            WriteMemory({0x90, 0x90, 0x90}, 0xD82CA);//Chew
            WriteMemory({0xE9, 0xB7, 0x00, 0x00, 0x00, 0x90}, 0x3D5B4);//Crush
            WriteMemory({0x90, 0x90, 0x90}, 0x1061A2);//Zombie Pea
            WriteMemory({0x90, 0x90, 0x90}, 0x105DEF);//Basketball
        } else {
            WriteMemory({0x89, 0x50, 0x40}, 0xD82CA);
            WriteMemory({0x8B, 0x45, 0x08, 0x8B, 0x40, 0x1C}, 0x3D5B4);
            WriteMemory({0x89, 0x50, 0x40}, 0x1061A2);
            WriteMemory({0x89, 0x50, 0x40}, 0x105DEF);
        }
    }
}

void PvZ::PlantWeak(bool on) {
    if (isGameOn()) {
        if (on)
            WriteMemory({0x90, 0x90, 0x90, 0x90, 0x90, 0x90}, 0xD8345);
        else
            WriteMemory({0x0F, 0x8F, 0x6F, 0x01, 0x00, 0x00}, 0xD8345);
    }
}

void PvZ::ChomperNoCD(bool on) {
    if (isGameOn()) {
        if (on)
            WriteMemory({0x90, 0x90, 0x90, 0x90, 0x90, 0x90}, 0x3F4BD);
        else
            WriteMemory({0x0F, 0x85, 0x94, 0x00, 0x00, 0x00}, 0x3F4BD);
    }
}

void PvZ::PotatoMineNoCD(bool on) {
    if (isGameOn()) {
        if (on)
            WriteMemory({0x90, 0x90, 0x90, 0x90, 0x90, 0x90}, 0x3FA2C);
        else
            WriteMemory({0x0F, 0x85, 0xF0, 0x02, 0x00, 0x00}, 0x3FA2C);
    }
}

void PvZ::BombNoExplode(bool on) {
    if (isGameOn()) {
        if (on)
            WriteMemory<byte>(0xEB, 0x42C5D);
        else
            WriteMemory<byte>(0x75, 0x42C5D);
    }
}

void PvZ::BombInstantlyExplode(bool on) {
    if (isGameOn()) {
        if (on)
            WriteMemory({0x90, 0x90}, 0x42C5D);
        else
            WriteMemory({0x75, 0x0B}, 0x42C5D);
    }
}

void PvZ::CobCannonNoCD(bool on) {
    if (isGameOn()) {
        if (on)
            WriteMemory({0x90, 0x90, 0x90, 0x90, 0x90, 0x90}, 0x35A38);
        else
            WriteMemory({0x0F, 0x85, 0x99, 0x01, 0x00, 0x00}, 0x35A38);
    }
}

void PvZ::MagnetShroomNoCD(bool on) {
    if (isGameOn()) {
        if (on)
            WriteMemory({0x90, 0x90, 0x90, 0x90, 0x90, 0x90}, 0x3680D);
        else
            WriteMemory({0x0F, 0x85, 0xEE, 0x05, 0x00, 0x00}, 0x3680D);
    }
}

void PvZ::NoCrater(bool on) {
    if (isGameOn()) {
        if (on)
            WriteMemory({0x90, 0x90}, 0x22B0B);
        else
            WriteMemory({0x75, 0x0B}, 0x22B0B);
    }
}

void PvZ::PlantBoard(bool on) {
    if (isGameOn()) {
        if (on) {
            WriteMemory({0x90, 0x90}, 0x3C518);
        } else {
            WriteMemory({0x75, 0x09}, 0x3C518);
        }
    }
}

void PvZ::MushroomAwake(bool on) {
    if (isGameOn()) {
        if (on) {
            WriteMemory<byte>(0xEB, 0x3A2DD);
            WriteMemory<byte>(0xEB, 0x22562);
            WriteMemory({0x90, 0x90}, 0x225A7);
            //wake up existing mushrooms
            if (CurGameUI() == 2 || CurGameUI() == 3) {
                auto plant_count_max = ReadMemory<uint32_t>(base, 0x780, 0xA4);
                auto plant_offset = ReadMemory<uint32_t>(base, 0x780, 0xA0);
                code.asm_init();
                for (size_t i = 0; i < plant_count_max; i++) {
                    auto plant_disappeared = ReadMemory<bool>(plant_offset + 0x141 + 0x14C * i);
                    auto plant_crushed = ReadMemory<bool>(plant_offset + 0x142 + 0x14C * i);
                    auto plant_sleeping = ReadMemory<bool>(plant_offset + 0x143 + 0x14C * i);
                    if (!plant_disappeared && !plant_crushed && plant_sleeping) {
                        uint32_t addr = plant_offset + 0x14C * i;
                        code.asm_mov_dword_ptr_esp_add(0x4, 0);
                        code.asm_mov_exx(Reg::EAX, addr);
                        code.asm_mov_ptr_esp_add_exx(0x0, Reg::EAX);
                        code.asm_call(0x3442A);
                    }
                }
                code.asm_ret();
                code.asm_code_inject();
            }
        } else {
            WriteMemory<byte>(0x75, 0x3A2DD);
            WriteMemory<byte>(0x75, 0x22562);
            WriteMemory({0x75, 0x06}, 0x225A7);
        }
    }
}

void PvZ::SunShroomGrow(bool on) {
    if (isGameOn()) {
        if (on)
            WriteMemory({0x90, 0x90}, 0x3540E);
        else
            WriteMemory({0x75, 0x4C}, 0x3540E);
    }
}

void PvZ::AlwaysKernal(bool on) {
    if (isGameOn()) {
        if (on)
            WriteMemory<byte>(0xEB, 0x4243F);
        else
            WriteMemory<byte>(0x74, 0x4243F);
    }
}

void PvZ::AlwaysButter(bool on) {
    if (isGameOn()) {
        if (on)
            WriteMemory({0x90, 0x90}, 0x4243F);
        else
            WriteMemory({0x74, 0x5F}, 0x4243F);
    }
}

void PvZ::StrongBlover(bool on) {
    if (isGameOn()) {
        if (on)
            WriteMemory({0x90, 0x90}, 0x3CDF1);
        else
            WriteMemory({0x75, 0x15}, 0x3CDF1);
    }
}

void PvZ::BeghouledFreelyMove(bool on) {
    if (isGameOn()) {
        if (on) {
            WriteMemory<byte>(0x81, 0xA38A5);
            WriteMemory<byte>(0xEB, 0xB3F73);
            WriteMemory<byte>(0x81, 0xA3C36);
            WriteMemory<byte>(0xEB, 0xB3FC9);
        } else {
            WriteMemory<byte>(0x84, 0xA38A5);
            WriteMemory<byte>(0x74, 0xB3F73);
            WriteMemory<byte>(0x84, 0xA3C36);
            WriteMemory<byte>(0x74, 0xB3FC9);
        }
    }
}

//Zombies

void PvZ::ModifyZombieHP(int type, int value) {
    if (isGameOn()) {
        int address[27] = {0xEAB8A, 0xEB34E, 0xEC8D8, 0xEB819, 0xEB4D0, 0xEBBF9, 0xEC4A8, 0xEAD26, 0xEAE9C, 0xEADC9,
                           0xEB96B, 0xEB586, 0xEB7F8, 0xEC9CD, 0xECAC1, 0xECAD0, 0xEAC67, 0xEACCF, 0xEC5F2, 0xEACF4,
                           0xEB1AE, 0xEC74A, 0xEB1ED, 0xEADE3, 0xED242, 0xECEC7, 0xED077};
        WriteMemory<int>(value, address[type]);
    }
}

void PvZ::GetZombieHP(int type) {
    if (isGameOn(false)) {
        int address[27] = {0xEAB8A, 0xEB34E, 0xEC8D8, 0xEB819, 0xEB4D0, 0xEBBF9, 0xEC4A8, 0xEAD26, 0xEAE9C, 0xEADC9,
                           0xEB96B, 0xEB586, 0xEB7F8, 0xEC9CD, 0xECAC1, 0xECAD0, 0xEAC67, 0xEACCF, 0xEC5F2, 0xEACF4,
                           0xEB1AE, 0xEC74A, 0xEB1ED, 0xEADE3, 0xED242, 0xECEC7, 0xED077};
        int value = ReadMemory<int>(address[type]);
        emit ZombieHP(value);
    }
}

void PvZ::ZombieInvincible(bool on) {
    if (isGameOn()) {
        if (on) {
            WriteMemory({0x90, 0x90, 0x90, 0x90, 0x90, 0x90}, 0xE5B72);
            WriteMemory({0x90, 0x90, 0x90, 0x90, 0x90, 0x90}, 0xDE091);
            WriteMemory({0x90, 0x90, 0x90, 0x90, 0x90, 0x90}, 0xDE85E);
            WriteMemory<byte>(0xEB, 0xE63E1);
        } else {
            WriteMemory({0x89, 0x90, 0xC8, 0x00, 0x00, 0x00}, 0xE5B72);
            WriteMemory({0x89, 0x90, 0xD0, 0x00, 0x00, 0x00}, 0xDE091);
            WriteMemory({0x89, 0x90, 0xDC, 0x00, 0x00, 0x00}, 0xDE85E);
            WriteMemory<byte>(0x7F, 0xE63E1);
        }
    }
}

void PvZ::ZombieWeak(bool on) {
    if (isGameOn()) {
        if (on) {
            WriteMemory({0x90, 0x90}, 0xE6255);
            WriteMemory({0x90, 0x90}, 0xDE0CC);
            WriteMemory({0x90, 0x90}, 0xDE86F);
            WriteMemory<byte>(0x70, 0xE63E1);
        } else {
            WriteMemory({0x7F, 0x2A}, 0xE6255);
            WriteMemory({0x75, 0x1D}, 0xDE0CC);
            WriteMemory({0x75, 0x1D}, 0xDE86F);
            WriteMemory<byte>(0x7F, 0xE63E1);
        }
    }
}

void PvZ::JackNoExplode(bool on) {
    if (isGameOn()) {
        if (on) {
            WriteMemory<byte>(0x81, 0xE8D4A);
            WriteMemory<byte>(0x81, 0xE8C46);
        } else {
            WriteMemory<byte>(0x8F, 0xE8D4A);
            WriteMemory<byte>(0x8F, 0xE8C46);
        }
    }
}

void PvZ::JackImmediatelyExplode(bool on) {
    if (isGameOn()) {
        if (on) {
            WriteMemory<byte>(0x80, 0xE8D4A);
            WriteMemory<byte>(0x80, 0xE8C46);
        } else {
            WriteMemory<byte>(0x8F, 0xE8D4A);
            WriteMemory<byte>(0x8F, 0xE8C46);
        }
    }
}

void PvZ::NoImpThrow(bool on) {
    if (isGameOn()) {
        if (on)
            WriteMemory<byte>(0xEB, 0xE8525);
        else
            WriteMemory<byte>(0x74, 0xE8525);
    }
}

void PvZ::InfiniteImpThrow(bool on) {
    if (isGameOn()) {
        if (on)
            WriteMemory<byte>(0x01, 0xE827E);
        else
            WriteMemory<byte>(0x00, 0xE827E);
    }
}

void PvZ::NoBasketball(bool on) {
    if (isGameOn()) {
        if (on)
            WriteMemory<byte>(0x81, 0xD6851);
        else
            WriteMemory<byte>(0x85, 0xD6851);
    }
}

void PvZ::StandingStill(bool on) {
    if (isGameOn()) {
        if (on)
            WriteMemory({0xEB, 0x2C}, 0xD3565);
        else
            WriteMemory({0x74, 0x17}, 0xD3565);
    }
}

void PvZ::InfiniteBasketball(bool on) {
    if (isGameOn()) {
        if (on)
            WriteMemory({0x90, 0x90, 0x90, 0x90, 0x90, 0x90}, 0xD68C8);
        else
            WriteMemory({0x89, 0x90, 0x14, 0x01, 0x00, 0x00}, 0xD68C8);
    }
}

void PvZ::NoZombieSpawn(bool on) {
    if (isGameOn()) {
        if (on) {
            WriteMemory({0x90, 0x90, 0x90, 0x90, 0x90, 0x90}, 0x2D830);
            WriteMemory({0x90, 0x90, 0x90, 0x90, 0x90, 0x90}, 0x2D63C);
        } else {
            WriteMemory({0x89, 0x90, 0x90, 0x55, 0x00, 0x00}, 0x2D830);
            WriteMemory({0x89, 0x90, 0x98, 0x55, 0x00, 0x00}, 0x2D63C);
        }
    }
}

void PvZ::AllZombieSpawn(bool on) {
    if (isGameOn()) {
        if (on)
            WriteMemory({0x90, 0x90, 0x90, 0x90, 0x90, 0x90}, 0x2DA11);
        else
            WriteMemory({0x0F, 0x85, 0xE9, 0x01, 0x00, 0x00}, 0x2DA11);
    }
}

void PvZ::NoThreesome(bool on) {
    if (isGameOn()) {
        if (on)
            WriteMemory<byte>(0xEB, 0x2D613);
        else
            WriteMemory<byte>(0x75, 0x2D613);
    }
}

void PvZ::InfinitePole(bool on) {
    if (isGameOn()) {
        if (on)
            WriteMemory<byte>(0x0B, 0xDB7D1);
        else
            WriteMemory<byte>(0x0D, 0xDB7D1);
    }
}

void PvZ::ZombieInvisible(bool on) {
    if (isGameOn()) {
        if (on) {
            WriteMemory({0x90, 0x90}, 0xDD457);
            WriteMemory({0x90, 0x90}, 0xDFF38);
        } else {
            WriteMemory({0x75, 0x14}, 0xDD457);
            WriteMemory({0x75, 0x0B}, 0xDFF38);
        }
    }
}

void PvZ::ZombieVisible(bool on) {
    if (isGameOn()) {
        if (on) {
            WriteMemory<byte>(0xEB, 0xDD457);
            WriteMemory<byte>(0xEB, 0xDFF38);
        } else {
            WriteMemory<byte>(0x75, 0xDD457);
            WriteMemory<byte>(0x75, 0xDFF38);
        }
    }
}

void PvZ::ButterImmune(bool on) {
    if (isGameOn()) {
        if (on) {
            WriteMemory<byte>(0x81, 0xDCF99);
            WriteMemory({0x31, 0xD2, 0x90}, 0xEA22F);
        } else {
            WriteMemory<byte>(0x85, 0xDCF99);
            WriteMemory({0x8D, 0x50, 0xFF}, 0xEA22F);
        }
    }
}

void PvZ::IceImmune(bool on) {
    if (isGameOn()) {
        if (on) {
            WriteMemory<byte>(0x81, 0xE6C06);
            WriteMemory({0x31, 0xD2, 0x90}, 0xEA198);
        } else {
            WriteMemory<byte>(0x85, 0xE6C06);
            WriteMemory({0x8D, 0x50, 0xFF}, 0xEA198);
        }
    }
}

void PvZ::SlowdownImmune(bool on) {
    if (isGameOn()) {
        if (on) {
            WriteMemory<byte>(0xEB, 0xD8E06);
            WriteMemory({0x31, 0xD2, 0x90}, 0xEA1F5);
        } else {
            WriteMemory<byte>(0x75, 0xD8E06);
            WriteMemory({0x8D, 0x50, 0xFF}, 0xEA1F5);
        }
    }
}

void PvZ::NoIceTrail(bool on) {
    if (isGameOn()) {
        if (on) {
            WriteMemory({0x90, 0x90}, 0x21298);
            WriteMemory({0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90}, 0xD3260);
        } else {
            WriteMemory({0x75, 0x2B}, 0x21298);
            WriteMemory({0x89, 0x84, 0x91, 0x00, 0x06, 0x00, 0x00}, 0xD3260);
        }
    }
}

void PvZ::NoYetiEscape(bool on) {
    if (isGameOn()) {
        if (on)
            WriteMemory<byte>(0xEB, 0xD7E76);
        else
            WriteMemory<byte>(0x75, 0xD7E76);
    }
}

void PvZ::NoEnterHouse(bool on) {
    if (isGameOn()) {
        if (on)
            WriteMemory({0x90, 0x90}, 0xE7188);
        else
            WriteMemory({0x74, 0x0D}, 0xE7188);
    }
}

void PvZ::GatherZombies(bool on, float pos) {
    if (isGameOn()) {
        if (on) {
            std::array<byte, 8> ar = {0xC7, 0x40, 0x2C, 0x00, 0x00, 0x00, 0x00, 0x90};
            memcpy(&ar[3], &pos, 4);
            WriteMemory<byte>(0x81, 0xD33B4);
            WriteMemory(ar, 0xD33A9);
        } else {
            WriteMemory({0x89, 0x04, 0x24, 0xE8, 0xAF, 0xFA, 0xFF, 0xFF}, 0xD33A9);
            WriteMemory<byte>(0x85, 0xD33B4);
        }
    }
}

void PvZ::AllZombiesXXX(int status) {
    if (isGameOn()) {
        auto zombie_count_max = ReadMemory<uint32_t>(base, 0x780, 0x88);
        auto zombie_offset = ReadMemory<uint32_t>(base, 0x780, 0x84);
        for (size_t i = 0; i < zombie_count_max; i++) {
            auto zombie_existing = ReadMemory<uint16_t>(zombie_offset + 0x166 + i * 0x168);
            auto zombie_disappeared = ReadMemory<bool>(zombie_offset + 0xEC + i * 0x168);
            if (zombie_existing && !zombie_disappeared) {
                switch (status) {
                    case 0: // Kill
                        WriteMemory<int>(3, zombie_offset + 0x28 + i * 0x168);
                        break;
                    case 1: // Hypnotize
                        WriteMemory<bool>(true, zombie_offset + 0xB8 + i * 0x168);
                        break;
                    case 2: // Blow away
                        WriteMemory<bool>(true, zombie_offset + 0xB9 + i * 0x168);
                        break;
                    case 3: // Disappear
                        WriteMemory<int>(1, zombie_offset + 0x28 + i * 0x168);
                        break;
                    case 4: // Dying
                        WriteMemory<bool>(false, zombie_offset + 0xBA + i * 0x168);
                        break;
                    default:
                        break;
                }
            }
        }
    }
}

void PvZ::SpawnNextWave() {
    if (isGameOn() && CurGameUI() == 3)
        WriteMemory<int>(1, base, 0x780, 0x5590);
}

//Spawn

// generate type from seed
void PvZ::UpdateZombiesType() {
    code.asm_init();
    code.asm_mov_exx_dword_ptr(Reg::EAX, 0x35EE64);
    code.asm_mov_exx_dword_ptr_exx_add(Reg::EAX, 0x780);
    // code.asm_mov_exx_dword_ptr_exx_add(Reg::EAX, 0x154);
    code.asm_mov_ptr_esp_add_exx(0x0, Reg::EAX);
    // code.asm_call(0xA8552); Endless only
    // code.asm_call(0xA883C);
    code.asm_call(0x14BEA);
    code.asm_ret();
    code.asm_code_inject();
}

// generate list from type
void PvZ::UpdateZombiesList() {
    code.asm_init();
    code.asm_mov_exx_dword_ptr(Reg::EAX, 0x35EE64);
    code.asm_mov_exx_dword_ptr_exx_add(Reg::EAX, 0x780);
    code.asm_mov_ptr_esp_add_exx(0x0, Reg::EAX);
    code.asm_call(0x13F5A);
    code.asm_ret();
    code.asm_code_inject();
}

void PvZ::UpdateZombiesPreview() {
    WriteMemory<byte>(0x80, 0xFB004);
    code.asm_init();
    code.asm_mov_exx_dword_ptr(Reg::EAX, 0x35EE64);
    code.asm_mov_exx_dword_ptr_exx_add(Reg::EAX, 0x780);
    code.asm_mov_ptr_esp_add_exx(0x0, Reg::EAX);
    code.asm_call(0x1C362);//clear zombies
    code.asm_mov_exx_dword_ptr(Reg::EAX, 0x35EE64);
    code.asm_mov_exx_dword_ptr_exx_add(Reg::EAX, 0x780);
    code.asm_mov_exx_dword_ptr_exx_add(Reg::EAX, 0x150);
    code.asm_mov_ptr_esp_add_exx(0x0, Reg::EAX);
    code.asm_call(0xFAFF0);//generate new zombies
    code.asm_ret();
    code.asm_code_inject();
    WriteMemory<byte>(0x85, 0xFB004);
}

void PvZ::InternalSpawn(std::array<bool, 33> &zombies) {
    if (isGameOn() && (CurGameUI() == 2 || CurGameUI() == 3)) {
        auto zombies_type_offset = ReadMemory<uint32_t>(base, 0x780) + 0x54C8;
        auto zombies_list_offset = ReadMemory<uint32_t>(base, 0x780) + 0x6A8;
        
        // backup original zombie types
        auto zombies_type_original = ReadMemory<bool, 33>(zombies_type_offset);
        
        // zombies[0] for "Zombie", zombies[26] for "Peashooter Zombie"
        // game will crash if neither of them was set when using call 0x13F5A
        // so replace the zombie which has the minimal id with Peashooter Zombie
        bool has_normal_zombie = zombies[0] || zombies[26];
        uint32_t replace_zombie = 0;
        if (!has_normal_zombie) {
            for (size_t i = 0; i < 33; i++)
                if (zombies[i]) {
                    replace_zombie = i;
                    break;
                }
        }
        
        WriteMemory(zombies, zombies_type_offset);
        if (!has_normal_zombie) {
            WriteMemory<bool>(false, zombies_type_offset + replace_zombie * sizeof(bool));
            WriteMemory<bool>(true, zombies_type_offset + 26 * sizeof(bool));
        }
        
        UpdateZombiesList();
        
        // change replace zombie back after the zombies list was generated
        if (!has_normal_zombie) {
            auto zombies_list = ReadMemory<uint32_t, 2000>(zombies_list_offset);
            for (size_t i = 0; i < 2000; i++) {
                if (zombies_list[i] == 26)
                    WriteMemory<uint32_t>(replace_zombie, zombies_list_offset + i * sizeof(uint32_t));
            }
        }
        
        if (CurGameUI() == 2)
            UpdateZombiesPreview();
        
        // restore original zombie types
        WriteMemory(zombies_type_original, zombies_type_offset);
    }
    GetSpawnList();
}

void PvZ::CustomizeSpawn(std::array<bool, 33> &zombies, bool simulate, bool limit_flag, bool limit_yeti,
                         bool limit_bungee, bool limit_giga, std::array<bool, 20> &giga_waves) {
    if (isGameOn() && (CurGameUI() == 2 || CurGameUI() == 3)) {
        auto *zombies_list = new std::array<uint32_t, 2000>;
        std::fill((*zombies_list).begin(), (*zombies_list).end(), 0);
        
        bool has_flag = zombies[1];
        bool has_yeti = zombies[19];
        bool has_bungee = zombies[20];
        bool has_giga = zombies[32];
        
        int count = 0;
        for (size_t i = 0; i < 33; i++)
            if (zombies[i])
                count++;
        
        if (count > 0) {
            std::vector<double> weights = {20, 0, 37, 67, 95, 37, 108, 67, 37, 0, 0, 67, 67, 67, 52, 37, 67, 37, 37, 17,
                                           0, 37, 52, 52, 0, 0, 120, 94, 37, 67, 67, 67, 36};
            std::vector<double> weights_flag = {84, 10, 29, 50, 71, 29, 80, 50, 28, 0, 0, 50, 50, 49, 39, 29, 50, 29,
                                                29, 10, 28, 29, 39, 39, 0, 0, 89, 70, 28, 50, 50, 50, 122};
            std::discrete_distribution<unsigned int> dist(weights.begin(), weights.end());
            std::discrete_distribution<unsigned int> dist_flag(weights_flag.begin(), weights_flag.end());
            auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
            std::mt19937 gen(seed);
            
            uint32_t type = 0;
            for (size_t i = 0; i < 2000; i++) {
                do {
                    if (simulate) {
                        if (((i / 50) % 10) == 9) // flag wave
                            type = dist_flag(gen);
                        else
                            type = dist(gen);
                    } else {
                        (++type) %= 33;
                    }
                } while ((!zombies[type])                              //
                         || (has_flag && limit_flag && type == 1)      //
                         || (has_yeti && limit_yeti && type == 19)     //
                         || (has_bungee && limit_bungee && type == 20) //
                         || (has_giga && limit_giga && type == 32 && !giga_waves[(i / 50) % 20]));
                
                (*zombies_list)[i] = type;
            }
            
            std::vector<size_t> index_flag = {450, 950, 1450, 1950};
            std::vector<size_t> index_bungee = {451, 452, 453, 454, 951, 952, 953, 954, 1451, 1452, 1453, 1454, 1951,
                                                1952, 1953, 1954};
            
            if (has_flag && limit_flag) {
                for (auto i : index_flag)
                    (*zombies_list)[i] = 1;
            }
            if (has_bungee && limit_bungee) {
                for (auto i : index_bungee)
                    (*zombies_list)[i] = 20;
            }
            if (has_yeti && limit_yeti) {
                uint32_t i;
                do {
                    i = gen() % 1000;
                } while ((has_flag && limit_flag &&
                          std::find(index_flag.begin(), index_flag.end(), i) != index_flag.end()) //
                         || (has_bungee && limit_bungee &&
                             std::find(index_bungee.begin(), index_bungee.end(), i) != index_bungee.end()));
                (*zombies_list)[i] = 19;
            }
        }
        
        WriteMemory((*zombies_list), base, 0x780, 0x6A8);
        
        // giga count in some wave may be 0 in simulate mode in some extreme cases
        if (simulate && has_giga) {
            int test_giga_count[20] = {0};
            for (size_t w = 0; w < 20; w++)
                for (size_t j = 0; j < 50; j++)
                    if ((*zombies_list)[j + w * 50] == 32)
                        test_giga_count[w]++;
            bool ok = true;
            for (int w = 0; w < 20; w++) {
                if (!((!limit_giga && (test_giga_count[w] > 0)) ||
                      (limit_giga && ((test_giga_count[w] > 0) == giga_waves[w])))) {
                    ok = false;
                    break;
                }
            }
            if (!ok)
                CustomizeSpawn(zombies, simulate, limit_flag, limit_yeti, limit_bungee, limit_giga, giga_waves);
        }
        if (CurGameUI() == 2)
            UpdateZombiesPreview();
        
        delete zombies_list;
        GetSpawnList();
    }
}

void PvZ::NaturalSpawn(std::array<bool, 33> &zombies) {
    InternalSpawn(zombies);
}

void PvZ::SimulateSpawn(std::array<bool, 33> &zombies, bool limit_flag, bool limit_yeti, bool limit_bungee,
                        bool limit_giga, std::array<bool, 20> &giga_waves) {
    CustomizeSpawn(zombies, true, limit_flag, limit_yeti, limit_bungee, limit_giga, giga_waves);
}

void PvZ::ExtremeSpawn(std::array<bool, 33> &zombies, bool limit_flag, bool limit_yeti, bool limit_bungee,
                       bool limit_giga, std::array<bool, 20> &giga_waves) {
    CustomizeSpawn(zombies, false, limit_flag, limit_yeti, limit_bungee, limit_giga, giga_waves);
}

void PvZ::RestoreSpawn() {
    if (isGameOn() && (CurGameUI() == 2 || CurGameUI() == 3)) {
        UpdateZombiesType();
        if (CurGameUI() == 2)
            UpdateZombiesPreview();
        GetSpawnList();
    }
}

void PvZ::GetSpawnList() {
    std::array<int, 33> zombies_count = {0};
    if (isGameOn(false) && (CurGameUI() == 2 || CurGameUI() == 3)) {
        auto zombies_list = ReadMemory<uint32_t, 2000>(base, 0x780, 0x6A8);
        std::array<bool, 20> giga_waves = {false};
        auto wave_count = ReadMemory<uint32_t>(base, 0x780, 0x5558);
        for (size_t i = 0; i < wave_count; i++)
            for (size_t j = 0; j < 50; j++) {
                if (zombies_list[i * 50 + j] == -1)
                    break;
                if (!giga_waves[i] && zombies_list[i * 50 + j] == 32)
                    giga_waves[i] = true;
                zombies_count[zombies_list[i * 50 + j]]++;
            }
        emit GigaWaves(giga_waves);
    }
    emit SpawnList(zombies_count);
}

//Garden

void PvZ::ModifyFertilizer(int value) {
    if (isGameOn())
        WriteMemory<int>(value + 1000, base, 0x7F4, 0x204);
}

void PvZ::ModifyBugSpray(int value) {
    if (isGameOn())
        WriteMemory<int>(value + 1000, base, 0x7F4, 0x20C);
}

void PvZ::ModifyChocolate(int value) {
    if (isGameOn())
        WriteMemory<int>(value + 1000, base, 0x7F4, 0x238);
}

void PvZ::ModifyTreeFertilizer(int value) {
    if (isGameOn())
        WriteMemory<int>(value + 1000, base, 0x7F4, 0x240);
}

void PvZ::ModifyWisdomTreeHeight(int height) {
    if (isGameOn()) {
        if (CurGameMode() == 50) {
            WriteMemory<int>(height - 1, base, 0x7F4, 0x104);
            code.asm_init();
            // code.asm_mov_exx(Reg::EAX, 0x35EE64); Game crashes
            code.asm_mov_exx_dword_ptr(Reg::EAX, 0x35EE64);
            code.asm_mov_exx_dword_ptr_exx_add(Reg::EAX, 0x780);
            code.asm_mov_exx_dword_ptr_exx_add(Reg::EAX, 0x154);
            code.asm_mov_ptr_esp_add_exx(0x0, Reg::EAX);
            code.asm_call(0xAF280);
            code.asm_ret();
            code.asm_code_inject();
        } else
            WriteMemory<int>(height, base, 0x7F4, 0x104);
    }
}

void PvZ::NoSnailSleep(bool on) {
    if (isGameOn()) {
        if (on) {
            if (ReadMemory<int>(base, 0x7F4, 0x220)) {
                time_t t;
                time(&t);
                WriteMemory<int>(int(t), base, 0x7F4, 0x220);
            }
            WriteMemory<byte>(0xEB, 0x2186D5);
        } else
            WriteMemory<byte>(0x7E, 0x2186D5);
    }
}

void PvZ::MarigoldRefresh() {
    if (isGameOn()) {
        WriteMemory<int>(0x0, base, 0x7F4, 0x1F8);
        WriteMemory<int>(0x0, base, 0x7F4, 0x1FC);
        WriteMemory<int>(0x0, base, 0x7F4, 0x200);
    }
}

void PvZ::GardenPlantRight() {
    auto plant_count = ReadMemory<uint32_t>(base, 0x7F4, 0x360);
    auto plant_offset = ReadMemory<uint32_t>(base, 0x7F4) + 0x360;
    for (size_t i = 0; i < plant_count; i++) {
        WriteMemory<int>(0x0, plant_offset + 0x14 + 0x3C * i);
    }
}

void PvZ::GardenPlantLeft() {
    auto plant_count = ReadMemory<uint32_t>(base, 0x7F4, 0x360);
    auto plant_offset = ReadMemory<uint32_t>(base, 0x7F4) + 0x360;
    for (size_t i = 0; i < plant_count; i++) {
        WriteMemory<int>(0x1, plant_offset + 0x14 + 0x3C * i);
    }
}

//Others

void PvZ::SetDebugMode(int mode) {
    if (isGameOn() && CurGameUI() == 3) {
        WriteMemory<int>(mode, base, 0x780, 0x55EC);
    }
}

void PvZ::GetUserdataFolder() {
    if (isGameOn()) {
        QString DataDir = GameProc->memory().ReadString(ReadMemory<uint32_t>(0x3B61A4));
        // QString Cmd = "open " + DataDir.replace(" ", "\\ ") + "userdata";
        // system(Cmd.toStdString().data());
        DataDir += "userdata";
        emit UserdataFolder(DataDir);
    }
}

void PvZ::SetSeed(uint32_t seed) {
    if (isGameOn() && (CurGameUI() == 2 || CurGameUI() == 3)) {
        WriteMemory<uint32_t>(seed, base, 0x780, 0x5610);
    }
}

void PvZ::GetSeed() {
    if (isGameOn() && (CurGameUI() == 2 || CurGameUI() == 3)) {
        auto seed = ReadMemory<uint32_t>(base, 0x780, 0x5610);
        emit Seed(seed);
    }
}

void PvZ::GetRandomSeed() {
    std::mt19937 gen(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    auto seed = gen();
    if (isGameOn(false) && (CurGameUI() == 2 || CurGameUI() == 3)) {
        SetSeed(seed);
    }
    emit Seed(seed);
}

void PvZ::BackgroundRunning(bool on) {
    if (isGameOn()) {
        if (on)
            //WriteMemory({0x90, 0x90, 0x90, 0x90, 0x90, 0x90}, 0x2A33BC);
            WriteMemory({0x90, 0x90, 0x90, 0x90, 0x90}, 0x2A358F);
        else
            //WriteMemory({0xFF, 0x90, 0x50, 0x01, 0x00, 0x00}, 0x2A33BC);
            WriteMemory({0xE8, 0x7A, 0xFD, 0xFF, 0xFF}, 0x2A358F);
    }
}

void PvZ::DisablePause(bool on) {
    if (isGameOn()) {
        if (on)
            WriteMemory<byte>(0x81, 0x13358);
        else
            WriteMemory<byte>(0x84, 0x13358);
    }
}

void PvZ::NoDataDelete(bool on) {
    if (isGameOn()) {
        if (on)
            WriteMemory({0x31, 0xC9}, 0xD936);
        else
            WriteMemory({0x89, 0xC1}, 0xD936);
    }
}

void PvZ::NoDataSave(bool on) {
    if (isGameOn()) {
        if (on)
            WriteMemory({0x90, 0x90, 0x90, 0x90, 0x90}, 0xC1161);
        else
            WriteMemory({0xE8, 0xE6, 0x5A, 0xF6, 0xFF}, 0xC1161);
    }
}

void PvZ::SetMusic(int type) {
    if (isGameOn()) {
        code.asm_init();
        code.asm_mov_dword_ptr_esp_add(0x4, type);
        code.asm_mov_exx_dword_ptr(Reg::EAX, 0x35EE64);
        code.asm_mov_exx_dword_ptr_exx_add(Reg::EAX, 0x804);
        code.asm_mov_ptr_esp_add_exx(0x0, Reg::EAX);
        code.asm_call(0x21D6CC);
        code.asm_ret();
        code.asm_code_inject();
    }
}
