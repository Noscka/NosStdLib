﻿#include "NosLib/Console.hpp"
#include "NosLib/DynamicMenuSystem.hpp"

#include <Windows.h>
#include <conio.h>

/* TODO: Figure out if it is worth it to change calling convention from default (__cdelc) to __fastcall */

int main()
{
    NosLib::Console::InitializeModifiers::EnableUnicode();
    NosLib::Console::InitializeModifiers::EnableANSI();
    NosLib::Console::InitializeModifiers::BeatifyConsole<wchar_t>(L"Dynamic Array Updating");
    NosLib::Console::InitializeModifiers::InitializeEventHandler();

	int variable1 = 0;

	NosLib::Menu::DynamicMenu mainMenu(L"Basic Menu");

	NosLib::Menu::DynamicMenu secondMenu(L"Second Menu");

	mainMenu.AddMenuEntry(new NosLib::Menu::MenuEntry<NosLib::Menu::DynamicMenu>(L"Second Menu", &secondMenu));

	for (int i = 0; i <= 40; i++)
	{
		mainMenu.AddMenuEntry(new NosLib::Menu::MenuEntry<int>(std::format(L"Int {}", i), &variable1));
	}

	mainMenu.StartMenu();

    wprintf(L"Press any button to continue"); _getch();
    return 0;
}