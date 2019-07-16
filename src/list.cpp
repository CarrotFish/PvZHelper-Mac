#include "list.h"

List::List()
{
    PlantsList
            << QStringLiteral("豌豆射手")           // 0
            << QStringLiteral("向日葵")            // 1
            << QStringLiteral("樱桃炸弹")           // 2
            << QStringLiteral("坚果墙")            // 3
            << QStringLiteral("土豆雷")            // 4
            << QStringLiteral("寒冰射手")           // 5
            << QStringLiteral("大嘴花")            // 6
            << QStringLiteral("双发射手")           // 7
            << QStringLiteral("小喷菇")            // 8
            << QStringLiteral("阳光菇")            // 9
            << QStringLiteral("大喷菇")            // 10
            << QStringLiteral("墓碑吞噬者")          // 11
            << QStringLiteral("魅惑菇")            // 12
            << QStringLiteral("胆小菇")            // 13
            << QStringLiteral("寒冰菇")            // 14
            << QStringLiteral("毁灭菇")            // 15
            << QStringLiteral("睡莲")             // 16
            << QStringLiteral("窝瓜")             // 17
            << QStringLiteral("三线射手")           // 18
            << QStringLiteral("缠绕海草")           // 19
            << QStringLiteral("火爆辣椒")           // 20
            << QStringLiteral("地刺")             // 21
            << QStringLiteral("火炬树桩")           // 22
            << QStringLiteral("高坚果")            // 23
            << QStringLiteral("海蘑菇")            // 24
            << QStringLiteral("路灯花")            // 25
            << QStringLiteral("仙人掌")            // 26
            << QStringLiteral("三叶草")            // 27
            << QStringLiteral("裂荚射手")           // 28
            << QStringLiteral("杨桃")             // 29
            << QStringLiteral("南瓜头")            // 30
            << QStringLiteral("磁力菇")            // 31
            << QStringLiteral("卷心菜投手")          // 32
            << QStringLiteral("花盆")             // 33
            << QStringLiteral("玉米投手")           // 34
            << QStringLiteral("咖啡豆")            // 35
            << QStringLiteral("大蒜")             // 36
            << QStringLiteral("叶子保护伞")          // 37
            << QStringLiteral("金盏花")            // 38
            << QStringLiteral("西瓜投手")           // 39
            << QStringLiteral("机枪射手")           // 40
            << QStringLiteral("双子向日葵")          // 41
            << QStringLiteral("忧郁菇")            // 42
            << QStringLiteral("香蒲")             // 43
            << QStringLiteral("冰瓜")             // 44
            << QStringLiteral("吸金磁")            // 45
            << QStringLiteral("地刺王")            // 46
            << QStringLiteral("玉米加农炮")          // 47
            << QStringLiteral("模仿者")           // 48
            << QStringLiteral("爆炸坚果")           // 49
            << QStringLiteral("巨型坚果")           // 50
            << QStringLiteral("花苗")             // 51
            << QStringLiteral("反向双发射手");        // 52
    PlantsShortList
            << QStringLiteral("豌豆")           // 0
            << QStringLiteral("向日葵")            // 1
            << QStringLiteral("樱桃")           // 2
            << QStringLiteral("坚果墙")            // 3
            << QStringLiteral("土豆雷")            // 4
            << QStringLiteral("寒冰射手")           // 5
            << QStringLiteral("大嘴花")            // 6
            << QStringLiteral("双发")           // 7
            << QStringLiteral("小喷菇")            // 8
            << QStringLiteral("阳光菇")            // 9
            << QStringLiteral("大喷菇")            // 10
            << QStringLiteral("墓碑吞噬者")          // 11
            << QStringLiteral("魅惑菇")            // 12
            << QStringLiteral("胆小菇")            // 13
            << QStringLiteral("寒冰菇")            // 14
            << QStringLiteral("毁灭菇")            // 15
            << QStringLiteral("睡莲")             // 16
            << QStringLiteral("窝瓜")             // 17
            << QStringLiteral("三线射手")           // 18
            << QStringLiteral("缠绕海草")           // 19
            << QStringLiteral("火爆辣椒")           // 20
            << QStringLiteral("地刺")             // 21
            << QStringLiteral("火炬树桩")           // 22
            << QStringLiteral("高坚果")            // 23
            << QStringLiteral("海蘑菇")            // 24
            << QStringLiteral("路灯花")            // 25
            << QStringLiteral("仙人掌")            // 26
            << QStringLiteral("三叶草")            // 27
            << QStringLiteral("裂荚射手")           // 28
            << QStringLiteral("杨桃")             // 29
            << QStringLiteral("南瓜头")            // 30
            << QStringLiteral("磁力菇")            // 31
            << QStringLiteral("卷心菜投手")          // 32
            << QStringLiteral("花盆")             // 33
            << QStringLiteral("玉米投手")           // 34
            << QStringLiteral("咖啡豆")            // 35
            << QStringLiteral("大蒜")             // 36
            << QStringLiteral("叶子保护伞")          // 37
            << QStringLiteral("金盏花")            // 38
            << QStringLiteral("西瓜投手")           // 39
            << QStringLiteral("机枪射手")           // 40
            << QStringLiteral("双子向日葵")          // 41
            << QStringLiteral("忧郁菇")            // 42
            << QStringLiteral("香蒲")             // 43
            << QStringLiteral("冰瓜")             // 44
            << QStringLiteral("吸金磁")            // 45
            << QStringLiteral("地刺王")            // 46
            << QStringLiteral("玉米加农炮")          // 47
            << QStringLiteral("模仿者")           // 48
            << QStringLiteral("爆炸坚果")           // 49
            << QStringLiteral("巨型坚果")           // 50
            << QStringLiteral("花苗")             // 51
            << QStringLiteral("反向双发射手");        // 52
    ZombiesList
            << QStringLiteral("普通僵尸")           // 0
            << QStringLiteral("旗帜僵尸")           // 1
            << QStringLiteral("路障僵尸")           // 2
            << QStringLiteral("撑杆僵尸")           // 3
            << QStringLiteral("铁桶僵尸")           // 4
            << QStringLiteral("读报僵尸")           // 5
            << QStringLiteral("铁栅门僵尸")         // 6
            << QStringLiteral("橄榄球僵尸")         // 7
            << QStringLiteral("舞王僵尸")           // 8
            << QStringLiteral("伴舞僵尸")           // 9
            << QStringLiteral("鸭子僵尸")           // 10
            << QStringLiteral("潜水僵尸")           // 11
            << QStringLiteral("冰车僵尸")           // 12
            << QStringLiteral("雪橇僵尸")           // 13
            << QStringLiteral("海豚僵尸")           // 14
            << QStringLiteral("玩偶匣僵尸")          // 15
            << QStringLiteral("气球僵尸")           // 16
            << QStringLiteral("矿工僵尸")           // 17
            << QStringLiteral("跳跳僵尸")           // 18
            << QStringLiteral("雪人僵尸")           // 19
            << QStringLiteral("蹦极僵尸")           // 20
            << QStringLiteral("梯子僵尸")           // 21
            << QStringLiteral("投石车僵尸")           // 22
            << QStringLiteral("巨人僵尸")           // 23
            << QStringLiteral("小鬼僵尸")           // 24
            << QStringLiteral("僵王博士")           // 25
            << QStringLiteral("豌豆僵尸")           // 26
            << QStringLiteral("坚果僵尸")           // 27
            << QStringLiteral("辣椒僵尸")           // 28
            << QStringLiteral("机枪僵尸")           // 29
            << QStringLiteral("窝瓜僵尸")           // 30
            << QStringLiteral("高坚果僵尸")          // 31
            << QStringLiteral("红眼巨人");          // 32
    ZombiesShortList
            << QStringLiteral("普僵")             // 0
            << QStringLiteral("旗帜")             // 1
            << QStringLiteral("路障")             // 2
            << QStringLiteral("撑杆")             // 3
            << QStringLiteral("铁桶")             // 4
            << QStringLiteral("读报")             // 5
            << QStringLiteral("铁门")             // 6
            << QStringLiteral("橄榄")             // 7
            << QStringLiteral("舞王")             // 8
            << QStringLiteral("伴舞")             // 9
            << QStringLiteral("鸭子")             // 10
            << QStringLiteral("潜水")             // 11
            << QStringLiteral("冰车")             // 12
            << QStringLiteral("雪橇")             // 13
            << QStringLiteral("海豚")             // 14
            << QStringLiteral("小丑")             // 15
            << QStringLiteral("气球")             // 16
            << QStringLiteral("矿工")             // 17
            << QStringLiteral("跳跳")             // 18
            << QStringLiteral("雪人")             // 19
            << QStringLiteral("蹦极")             // 20
            << QStringLiteral("梯子")             // 21
            << QStringLiteral("投篮")             // 22
            << QStringLiteral("白眼")             // 23
            << QStringLiteral("小鬼")             // 24
            << QStringLiteral("僵王")             // 25
            << QStringLiteral("豌豆")             // 26
            << QStringLiteral("坚果")             // 27
            << QStringLiteral("辣椒")             // 28
            << QStringLiteral("机枪")             // 29
            << QStringLiteral("窝瓜")             // 30
            << QStringLiteral("高坚果")            // 31
            << QStringLiteral("红眼");            // 32
    CardList
            << QStringLiteral("豌豆射手")           // 0
            << QStringLiteral("向日葵")            // 1
            << QStringLiteral("樱桃炸弹")           // 2
            << QStringLiteral("坚果墙")            // 3
            << QStringLiteral("土豆雷")            // 4
            << QStringLiteral("寒冰射手")           // 5
            << QStringLiteral("大嘴花")            // 6
            << QStringLiteral("双发射手")           // 7
            << QStringLiteral("小喷菇")            // 8
            << QStringLiteral("阳光菇")            // 9
            << QStringLiteral("大喷菇")            // 10
            << QStringLiteral("墓碑吞噬者")          // 11
            << QStringLiteral("魅惑菇")            // 12
            << QStringLiteral("胆小菇")            // 13
            << QStringLiteral("寒冰菇")            // 14
            << QStringLiteral("毁灭菇")            // 15
            << QStringLiteral("睡莲")             // 16
            << QStringLiteral("窝瓜")             // 17
            << QStringLiteral("三线射手")           // 18
            << QStringLiteral("缠绕海草")           // 19
            << QStringLiteral("火爆辣椒")           // 20
            << QStringLiteral("地刺")             // 21
            << QStringLiteral("火炬树桩")           // 22
            << QStringLiteral("高坚果")            // 23
            << QStringLiteral("海蘑菇")            // 24
            << QStringLiteral("路灯花")            // 25
            << QStringLiteral("仙人掌")            // 26
            << QStringLiteral("三叶草")            // 27
            << QStringLiteral("裂荚射手")           // 28
            << QStringLiteral("杨桃")             // 29
            << QStringLiteral("南瓜头")            // 30
            << QStringLiteral("磁力菇")            // 31
            << QStringLiteral("卷心菜投手")          // 32
            << QStringLiteral("花盆")             // 33
            << QStringLiteral("玉米投手")           // 34
            << QStringLiteral("咖啡豆")            // 35
            << QStringLiteral("大蒜")             // 36
            << QStringLiteral("叶子保护伞")          // 37
            << QStringLiteral("金盏花")            // 38
            << QStringLiteral("西瓜投手")           // 39
            << QStringLiteral("机枪射手")           // 40
            << QStringLiteral("双子向日葵")          // 41
            << QStringLiteral("忧郁菇")            // 42
            << QStringLiteral("香蒲")             // 43
            << QStringLiteral("冰瓜")             // 44
            << QStringLiteral("吸金磁")            // 45
            << QStringLiteral("地刺王")            // 46
            << QStringLiteral("玉米加农炮")          // 47
            << QStringLiteral("模仿者")            // 48
            << QStringLiteral("爆炸坚果")           // 49
            << QStringLiteral("巨型坚果")           // 50
            << QStringLiteral("花苗")             // 51
            << QStringLiteral("反向双发射手")         // 52
            << QStringLiteral("未知")             // 53
            << QStringLiteral("重置植物")           // 54
            << QStringLiteral("恢复弹坑")           // 55
            << QStringLiteral("阳光")             // 56
            << QStringLiteral("钻石")             // 57
            << QStringLiteral("购买潜水僵尸")         // 58
            << QStringLiteral("购买奖杯")           // 59
            << QStringLiteral("普通僵尸")           // 60
            << QStringLiteral("路障僵尸")           // 61
            << QStringLiteral("撑杆僵尸")           // 62
            << QStringLiteral("铁桶僵尸")           // 63
            << QStringLiteral("梯子僵尸")           // 64
            << QStringLiteral("矿工僵尸")           // 65
            << QStringLiteral("蹦极僵尸")           // 66
            << QStringLiteral("橄榄球僵尸")         // 67
            << QStringLiteral("气球僵尸")           // 68
            << QStringLiteral("铁栅门僵尸")         // 69
            << QStringLiteral("冰车僵尸")           // 70
            << QStringLiteral("跳跳僵尸")           // 71
            << QStringLiteral("舞王僵尸")           // 72
            << QStringLiteral("巨人僵尸")           // 73
            << QStringLiteral("小鬼僵尸")           // 74
            << QStringLiteral("未知");            // 75
    CoinList
            << QStringLiteral("银币")             // 1
            << QStringLiteral("金币")             // 2
            << QStringLiteral("钻石")             // 3
            << QStringLiteral("阳光")             // 4
            << QStringLiteral("小阳光")            // 5
            << QStringLiteral("大阳光")            // 6
            << QStringLiteral("奖杯")             // 8
            << QStringLiteral("花苗")             // 17
            << QStringLiteral("钱袋")             // 18
            << QStringLiteral("银向日葵")           // 21
            << QStringLiteral("金向日葵")           // 22
            << QStringLiteral("巧克力");           // 23
    ResourceValueList
            << QStringLiteral("小阳光")
            << QStringLiteral("普通阳光")
            << QStringLiteral("大阳光")
            << QStringLiteral("银币(/10)")
            << QStringLiteral("金币(/10)")
            << QStringLiteral("钻石(/10)");
    GridItemList
            << QStringLiteral("墓碑")             // 1
            << QStringLiteral("梯子")             // 2
            << QStringLiteral("钉耙");            // 3
    MapList
            << QStringLiteral("白天")              // 0
            << QStringLiteral("夜晚")              // 1
            << QStringLiteral("泳池")              // 2
            << QStringLiteral("浓雾")              // 3
            << QStringLiteral("屋顶")              // 4
            << QStringLiteral("月夜")              // 5
            << QStringLiteral("蘑菇园")            // 6
            << QStringLiteral("花园")              // 7
            << QStringLiteral("水族馆")             // 8
            << QStringLiteral("智慧树");            // 9
    PlantHPList
            << QStringLiteral("普通植物")
            << QStringLiteral("坚果墙")
            << QStringLiteral("高坚果")
            << QStringLiteral("南瓜头")
            << QStringLiteral("大蒜")
            << QStringLiteral("地刺王")
            << QStringLiteral("爆炸坚果")
            << QStringLiteral("巨型坚果");
    ProjectileList
            << QStringLiteral("豌豆")
            << QStringLiteral("冰豌豆")
            << QStringLiteral("卷心菜")
            << QStringLiteral("西瓜")
            << QStringLiteral("孢子")
            << QStringLiteral("冰瓜")
            << QStringLiteral("火豌豆")
            << QStringLiteral("杨桃")
            << QStringLiteral("地刺")
            << QStringLiteral("篮球")
            << QStringLiteral("玉米粒")
            << QStringLiteral("黄油")
            << QStringLiteral("豌豆僵尸")
            << QStringLiteral("寒冰菇")
            << QStringLiteral("土豆雷")
            << QStringLiteral("窝瓜")
            << QStringLiteral("灰烬");
    ZombieHPList
            << QStringLiteral("普通僵尸")
            << QStringLiteral("撑杆僵尸")
            << QStringLiteral("舞王僵尸")
            << QStringLiteral("冰车僵尸")
            << QStringLiteral("海豚僵尸")
            << QStringLiteral("玩偶匣僵尸")
            << QStringLiteral("跳跳僵尸")
            << QStringLiteral("雪人僵尸")
            << QStringLiteral("蹦极僵尸")
            << QStringLiteral("梯子僵尸")
            << QStringLiteral("投石车僵尸")
            << QStringLiteral("巨人僵尸")
            << QStringLiteral("红眼巨人")
            << QStringLiteral("小鬼僵尸(IZ)")
            << QStringLiteral("僵王博士(AD)")
            << QStringLiteral("僵王博士(MG)")
            << QStringLiteral("路障")
            << QStringLiteral("铁桶")
            << QStringLiteral("报纸")
            << QStringLiteral("铁栅门")
            << QStringLiteral("橄榄球帽")
            << QStringLiteral("气球")
            << QStringLiteral("矿工帽")
            << QStringLiteral("梯子")
            << QStringLiteral("辣椒僵尸")
            << QStringLiteral("坚果头")
            << QStringLiteral("高坚果头");
    NonMaintainCheckBox
            << QStringLiteral("AutoRefreshGameStatus")
            << QStringLiteral("SlotVisible")
            << QStringLiteral("ShowShovel")
            << QStringLiteral("SeedVisible")
            << QStringLiteral("ImitaterOnly")
            << QStringLiteral("LimitYeti")
            << QStringLiteral("LimitBungee")
            << QStringLiteral("LimitGiga");
}
