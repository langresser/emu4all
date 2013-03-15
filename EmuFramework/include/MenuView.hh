/*  This file is part of Imagine.

	Imagine is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Imagine is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Imagine.  If not, see <http://www.gnu.org/licenses/> */

#pragma once

#include <util/gui/BaseMenuView.hh>
#include "EmuSystem.hh"
#include <StateSlotView.hh>

#ifdef CONFIG_BLUETOOTH
	#include <bluetooth/sys.hh>
	#include <bluetooth/BluetoothInputDevScanner.hh>
#endif
#include <config/version.h>
#include <MultiChoiceView.hh>
#include <VController.hh>

#define kOldMenu 0

void startGameFromMenu();

class OptionCategoryView : public BaseMenuView
{
	TextMenuItem subConfig[3]
	{
		"视频设置",
		"音频设置",
		"输入设置"
#if kOldMenu
        ,
		"系统设置"

        ,
		"界面设置"
#endif
	};

	MenuItem *item[5] = {nullptr};
public:
	constexpr OptionCategoryView(): BaseMenuView("设置") { }

	void init(bool highlightFirst);
	void onSelectElement(const GuiTable1D *table, const Input::Event &e, uint i);
};

class RecentGameView : public BaseMenuView
{
private:
	TextMenuItem recentGame[10];
	TextMenuItem clear {"清除列表", TextMenuItem::SelectDelegate::create<&clearRecentMenuHandler>()};

	static void clearRecentMenuHandler(TextMenuItem &, const Input::Event &e);

	MenuItem *item[1 + 10 + 1] = {nullptr};
public:
	constexpr RecentGameView(): BaseMenuView("最近玩的游戏") { }

	void init(bool highlightFirst);
};

struct InputPlayerMapMenuItem : public MultiChoiceSelectMenuItem
{
	constexpr InputPlayerMapMenuItem() { }
	uint *player = nullptr;
	void init(const char *name, uint *player)
	{
		this->player = player;
		static const char *str[] = { "1", "2", "3", "4", "5" };
		MultiChoiceSelectMenuItem::init(name, str, *player, EmuSystem::maxPlayers);
	}

	void doSet(int val)
	{
		*player = val;
		vController.updateMapping(*player);
	}
};

class MenuView : public BaseMenuView
{
protected:
    TextMenuItem loadGameList {"返回游戏列表", TextMenuItem::SelectDelegate::create<&loadGameListHandler>()};
	static void loadGameListHandler(TextMenuItem &, const Input::Event &e);

	TextMenuItem loadGame {"加载游戏", TextMenuItem::SelectDelegate::create<&loadGameHandler>()};
	static void loadGameHandler(TextMenuItem &, const Input::Event &e);

	TextMenuItem reset {"重新开始游戏", TextMenuItem::SelectDelegate::create<&resetHandler>()};
	static void resetHandler(TextMenuItem &, const Input::Event &e);

	TextMenuItem loadState {"加载游戏进度", TextMenuItem::SelectDelegate::create<&loadStateHandler>()};
	static void loadStateHandler(TextMenuItem &item, const Input::Event &e);

	TextMenuItem recentGames {"最近玩的游戏", TextMenuItem::SelectDelegate::create<&recentGamesHandler>()};
	static void recentGamesHandler(TextMenuItem &, const Input::Event &e);

	TextMenuItem saveState {"保存游戏进度", TextMenuItem::SelectDelegate::create<&saveStateHandler>()};
	static void saveStateHandler(TextMenuItem &, const Input::Event &e);

	TextMenuItem stateSlot {TextMenuItem::SelectDelegate::create<&stateSlotHandler>()};
	static void stateSlotHandler(TextMenuItem &, const Input::Event &e);
	char stateSlotText[128] = {0};

	TextMenuItem options {"游戏设置", TextMenuItem::SelectDelegate::create<&optionsHandler>()};
	static void optionsHandler(TextMenuItem &, const Input::Event &e);

    TextMenuItem feedBack {"意见反馈", TextMenuItem::SelectDelegate::create<&feedBackHandler>()};
	static void feedBackHandler(TextMenuItem &, const Input::Event &e);
    
    TextMenuItem weibo {"联系我们", TextMenuItem::SelectDelegate::create<&weiboHandler>()};
	static void weiboHandler(TextMenuItem &, const Input::Event &e);
#if kOldMenu
	TextMenuItem inputManager {"外接输入设备设置", TextMenuItem::SelectDelegate::create<&inputManagerHandler>()};
	static void inputManagerHandler(TextMenuItem &, const Input::Event &e);
#endif
    
    TextMenuItem exitApp {"退出游戏", TextMenuItem::SelectDelegate::create<&exitAppHandler>()};
	static void exitAppHandler(TextMenuItem &, const Input::Event &e);
    
#if kOldMenu
	TextMenuItem benchmark {"游戏性能测试", TextMenuItem::SelectDelegate::create<&benchmarkHandler>()};
	static void benchmarkHandler(TextMenuItem &, const Input::Event &e);


	#ifdef CONFIG_BLUETOOTH
	TextMenuItem scanWiimotes {"Scan for Wiimotes/iCP/JS1", TextMenuItem::SelectDelegate::create<&bluetoothScanHandler>()};
	static void bluetoothScanHandler(TextMenuItem &, const Input::Event &e);

	TextMenuItem bluetoothDisconnect {"Disconnect Bluetooth", TextMenuItem::SelectDelegate::create<&bluetoothDisconnectHandler>()};
	static void bluetoothDisconnectHandler(TextMenuItem &item, const Input::Event &e);
	#endif

	TextMenuItem about {"关于我们", TextMenuItem::SelectDelegate::create<&aboutHandler>()};
	static void aboutHandler(TextMenuItem &, const Input::Event &e);

	TextMenuItem screenshot {"游戏截屏", TextMenuItem::SelectDelegate::create<&screenshotHandler>()};
	static void screenshotHandler(TextMenuItem &item, const Input::Event &e);
#endif

public:
	constexpr MenuView(): BaseMenuView("游戏设置") { }

	static const uint STANDARD_ITEMS = 15;
	static const uint MAX_SYSTEM_ITEMS = 2;

	void onShow();
	void loadFileBrowserItems(MenuItem *item[], uint &items);
	void loadStandardItems(MenuItem *item[], uint &items);

protected:
	MenuItem *item[STANDARD_ITEMS + MAX_SYSTEM_ITEMS] {nullptr};

public:
	virtual void init(bool highlightFirst)
	{
		uint items = 0;
		loadFileBrowserItems(item, items);
		loadStandardItems(item, items);
		assert(items <= sizeofArray(item));
		BaseMenuView::init(item, items, highlightFirst);
	}
};
