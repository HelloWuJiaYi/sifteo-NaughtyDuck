/***
 * Copyright (C) Siftgame · 智能积木
 *
 * 游戏名称：com.sifteo.NaughtyDuck || 顽皮小黄鸭
 * 作者信息: 吴佳轶 || hellowujiayi@gmail.com || www.wujiayi.vip 
 * 
 * 版本说明：
 *    V0.1  2018-1-25  22:22:00  搭建游戏框架
 */

#include <sifteo.h>
#include <sifteo/menu.h>
#include "assets.gen.h"
#include "loader.h"

using namespace Sifteo;

static Metadata M = Metadata()
    .title("siftgame conduit")
    .package("com.siftgame.conduit", "V6.3")
    .icon(Icon)
    .cubeRange(3, 4);

static AssetSlot gMainSlot = AssetSlot::allocate()
    .bootstrap(BootstrapGroup);

static VideoBuffer vid[CUBE_ALLOCATION]; 

static TiltShakeRecognizer motion[CUBE_ALLOCATION];

// 加载菜单时需要的积木个数
unsigned NUM_CUBES = 3;

static CubeSet allCubes(0, NUM_CUBES);
static MyLoader loader(allCubes, gMainSlot, vid);

// 一级菜单图片资源
struct MenuItem items[] = { {&Icon1, &Label1}, {&Icon2, &Label2},
            {&Icon3, &Label3},  {&Icon4, &Label4},  {&Icon5, &Label5},  {&Icon6, &Label6},
            {&Icon7, &Label7},  {&Icon8, &Label8},  {&Icon9, &Label9}, {NULL, NULL} };

struct MenuAssets gAssets = {&BgTile, &Footer, &Label10, {&Tip1, &Tip2, NULL}};

// 二级菜单图片资源
struct MenuItem Menu1items[] = { {&Menu1Icon1, &Menu1Label1},
{&Menu1Icon2, &Menu1Label2},
            {&Menu1Icon3, &Menu1Label3},  {&Menu1Icon4, &Menu1Label4},
            {&Menu1Icon5, &Menu1Label5}, {NULL, NULL} };

struct MenuAssets Menu1gAssets = {&Menu1BgTile, &Menu1Footer,
&Menu1Label6, {&Menu1Tip1, &Menu1Tip2, NULL}};


// 延时函数
void time_delay(int msec) {
    SystemTime StartTime = SystemTime::now(); 
    TimeDelta elapsed = SystemTime::now() - StartTime; 
    while(elapsed.milliseconds() <= msec) {
        elapsed = SystemTime::now() - StartTime;
    } 
}

// 除了显示菜单的积木外，其他积木显示的背景
static void MenuBg() {
    for (CubeID cube : CubeSet::connected()) {
        vid[cube].initMode(BG0);
        vid[cube].attach(cube);
		vid[cube].bg0.image(vec(0,0), menubg);
    }
}

class SensorListener {
public:
    struct Counter {
        unsigned touch;
        unsigned neighborAdd;
        unsigned neighborRemove;
    } counters[CUBE_ALLOCATION];

    void install()
    {
        Events::neighborAdd.set(&SensorListener::onNeighborAdd, this);
        Events::neighborRemove.set(&SensorListener::onNeighborRemove, this);
        Events::cubeAccelChange.set(&SensorListener::onAccelChange, this);
        Events::cubeTouch.set(&SensorListener::onTouch, this);
        Events::cubeBatteryLevelChange.set(&SensorListener::onBatteryChange, this);
        Events::cubeConnect.set(&SensorListener::onConnect, this);

        // Handle already-connected cubes
        for (CubeID cube : CubeSet::connected())
            onConnect(cube);
    }

private:
    void onConnect(unsigned id)
    {
        CubeID cube(id);
        uint64_t hwid = cube.hwID();

        bzero(counters[id]);

        LOG("Cube %d connected\n", id);

        motion[id].attach(id);
        onAccelChange(cube);
        onBatteryChange(cube);
        onTouch(cube);
    }

    void onBatteryChange(unsigned id)
    {
        CubeID cube(id);
    }

    void onTouch(unsigned id)
    {
        CubeID cube(id);
        counters[id].touch++;
        LOG("Touch event on cube #%d, state=%d\n", id, cube.isTouching());
    }

    void onAccelChange(unsigned id)
    {
        CubeID cube(id);
        auto accel = cube.accel();
        unsigned changeFlags = motion[id].update();
        if (changeFlags) {
            LOG("Tilt/shake changed, flags=%08x\n", changeFlags);
            auto tilt = motion[id].tilt;
        }
    }

    void onNeighborRemove(unsigned firstID, unsigned firstSide, unsigned secondID, unsigned secondSide)
    {
        LOG("Neighbor Remove: %02x:%d - %02x:%d\n", firstID, firstSide, secondID, secondSide);
    }

    void onNeighborAdd(unsigned firstID, unsigned firstSide, unsigned secondID, unsigned secondSide)
    {
        LOG("Neighbor Add: %02x:%d - %d:%d\n", firstID, firstSide, secondID, secondSide);
    }
};


void ChooseThePlot(unsigned int MenuIndex, unsigned int MenuNumber) {

}

void TwoLevelMenu(unsigned index) { 

    loader.load(Menu1Group, gMainSlot, 0);  
    MenuBg();

    Menu m(vid[0], &Menu1gAssets, Menu1items);
    m.anchor(0);

    struct MenuEvent e;
    uint8_t item;

    while (m.pollEvent(&e)) {
        switch (e.type) {
            case MENU_ITEM_PRESS:
                if (e.item >= 7) {
                    continue;
                } else {
                    m.anchor(e.item);
                }
                if (e.item == 7) {
                    static unsigned randomIcon = 0;
                    randomIcon = (randomIcon + 1) % e.item;
                    m.replaceIcon(e.item, items[randomIcon].icon, items[randomIcon].label);
                }
                break;
            case MENU_EXIT: ASSERT(false); break;
            case MENU_NEIGHBOR_ADD: break;
            case MENU_NEIGHBOR_REMOVE: break;
            case MENU_ITEM_ARRIVE: item = e.item; break;
            case MENU_ITEM_DEPART: break;
            case MENU_PREPAINT: break;
            case MENU_UNEVENTFUL: ASSERT(false); break;
        }
        m.performDefault();
    }   
    switch (e.item) {
        case 0: ChooseThePlot(index, 1); break;
        case 1: ChooseThePlot(index, 2); break;
        case 2: ChooseThePlot(index, 3); break;
        case 3: ChooseThePlot(index, 4); break;
        case 4: ChooseThePlot(index, 5); break;
    }
}

void FirstLevelMenu() {
    loader.load(MenuGroup, gMainSlot, 0);  
    MenuBg();

    Menu m(vid[0], &gAssets, items);
    m.anchor(0);

    struct MenuEvent e;
    uint8_t item;

    while (m.pollEvent(&e)) {
        switch (e.type) {
            case MENU_ITEM_PRESS:
                if (e.item >= 7) {
                    continue;
                } else {
                    m.anchor(e.item);
                }
                if (e.item == 7) {
                    static unsigned randomIcon = 0;
                    randomIcon = (randomIcon + 1) % e.item;
                    m.replaceIcon(e.item, items[randomIcon].icon, items[randomIcon].label);
                }
                break;
            case MENU_EXIT: ASSERT(false); break;
            case MENU_NEIGHBOR_ADD: break;
            case MENU_NEIGHBOR_REMOVE: break;
            case MENU_ITEM_ARRIVE: item = e.item; break;
            case MENU_ITEM_DEPART: break;
            case MENU_PREPAINT: break;
            case MENU_UNEVENTFUL: ASSERT(false); break;
        }
        m.performDefault();
    }   
    switch (e.item) {
        case 0: TwoLevelMenu(1); break;
        case 1: TwoLevelMenu(2); break;
        case 2: TwoLevelMenu(3); break;
        case 3: TwoLevelMenu(4); break;
        case 4: TwoLevelMenu(5); break;
        case 5: TwoLevelMenu(6); break;
        case 6: TwoLevelMenu(7); break;
    }
}

void main()
{
    for (CubeID cube : CubeSet::connected())
		NUM_CUBES = cube;

	static CubeSet allCubes(0, NUM_CUBES + 1);
	static MyLoader loader(allCubes, gMainSlot, vid);

    static SensorListener sensors;
    sensors.install();

	while(1) {
		FirstLevelMenu(); 
	}
}
