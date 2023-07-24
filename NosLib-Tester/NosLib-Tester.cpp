﻿#include "NosLib/Console.hpp"
#include "NosLib/String.hpp"

#include "AllHeaders.hpp"

#include <Windows.h>
#include <conio.h>

/* TODO: Figure out if it is worth it to change calling convention from default (__cdelc) to __fastcall */

int main()
{
	NosLib::Console::InitializeModifiers::EnableUnicode();
	NosLib::Console::InitializeModifiers::EnableANSI();
	NosLib::Console::InitializeModifiers::BeatifyConsole<wchar_t>(L"Rewriting Center String");
	NosLib::Console::InitializeModifiers::InitializeEventHandler();

	std::wstring stringToCenter(158, L'█');
	
	wprintf(NosLib::String::CenterString(stringToCenter, true).c_str());

	wprintf(L"Press any button to continue"); _getch();
	return 0;
}