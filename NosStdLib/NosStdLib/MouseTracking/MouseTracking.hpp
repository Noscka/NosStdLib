﻿#ifndef _MOUSETRACKING_NOSSTDLIB_HPP_
#define _MOUSETRACKING_NOSSTDLIB_HPP_

#include "../Console.hpp"
#include "../RGB.hpp"
#include "../String.hpp"

#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <future>

namespace NosStdLib
{
	/// <summary>
	/// namespace that contains mouse tracking functions which are "fun"
	/// </summary>
	namespace MouseTrackingFun
	{
		HWND ConsoleHandle;
		HDC ConsoleContext;

		NosStdLib::RGB::NosRGB paintColor(255, 0, 0, 1);

		enum BrushType : uint8_t
		{
			square = 0,
			star = 1,
		};

		/// <summary>
		/// Function which actually makes the "MSPaint" work. gets put inside the mouse hook callback function.
		/// </summary>
		/// <param name="mouseHookStruct">- mouse hook struct (gotten from casting lParam)</param>
		/// <param name="brushThickness">- how thick the brush is</param>
		/// <param name="brushType">(default = BrushType::square) - the brush type</param>
		void ConsoleMSPaint(const PMSLLHOOKSTRUCT& mouseHookStruct, const int& brushThickness, const BrushType& brushType = BrushType::square)
		{
			if ((GetKeyState(VK_LBUTTON) & 0x8000) != 0) /* check if left mouse button is pressed */
			{
				int windowX, windowY;
				NosStdLib::Console::GetWindowPosition(&windowX, &windowY); /* get the coords of the window */

				/* position inside the display (black text area) */
				int consoleDisplayX = (mouseHookStruct->pt.x - windowX) - 6,  /* x coord */
					consoleDisplayY = (mouseHookStruct->pt.y - windowY) - 31; /* y coord */

				paintColor.Iterate();

				switch (brushType)
				{
				case BrushType::square:
					for (int i = (-1 * brushThickness) / 2; i <= brushThickness / 2; i++)
					{
						for (int j = (-1 * brushThickness) / 2; j <= brushThickness / 2; j++)
						{
							SetPixel(ConsoleContext, consoleDisplayX + i, consoleDisplayY + j, paintColor);
						}
					}
					break;

				case BrushType::star:
					for (int i = (-1 * brushThickness)/2; i <= brushThickness /2; i++)
					{
						SetPixel(ConsoleContext, consoleDisplayX +i, consoleDisplayY, paintColor);
					}
					for (int i = (-1 * brushThickness)/2; i <= brushThickness /2; i++)
					{
						SetPixel(ConsoleContext, consoleDisplayX, consoleDisplayY + i, paintColor);
					}
					break;
				}
			}
		}

		/// <summary>
		/// Function needed to make ConsoleMSPaint actually run
		/// </summary>
		void ConsoleMSPaintInit()
		{
			wprintf(L"Press any button to enter NosPaint"); _getch();

			ConsoleContext = GetDC(GetConsoleWindow());
		}
	}

	/// <summary>
	/// Namespace which tracks mouse position
	/// </summary>
	namespace MouseTracking
	{
		std::wstring CharCoordPrint(const int& x, const int& y)
		{
			NosStdLib::Console::ConsoleSizeStruct size = NosStdLib::Console::GetConsoleSize();

			if ((x <= 0 || y <= 0) || (x >= size.Columns || y >= size.Rows))
			{
				return std::wstring(size.Columns * size.Rows, L' ');
			}

			return (std::wstring(size.Columns * (y - 1), L' ') +
					std::wstring(x - 1, L' ') +
					std::wstring(1, L'█') +
					std::wstring(size.Columns - x, L' ') +
					std::wstring(((size.Columns * size.Rows) - (size.Columns * y)), L' '));
		}

		/// <summary>
		/// Calculates the position of the mouse in character terms
		/// </summary>
		/// <param name="mouseHookStruct">- for finding mouse position</param>
		void CalcCharPixel(const PMSLLHOOKSTRUCT& mouseHookStruct)
		{
			int windowX, windowY;
			NosStdLib::Console::GetWindowPosition(&windowX, &windowY); /* get the coords of the window */

			/* position inside the display (black text area) */
			int consoleDisplayX = (mouseHookStruct->pt.x - windowX) - 6,  /* x coord */
				consoleDisplayY = (mouseHookStruct->pt.y - windowY) - 31; /* y coord */

			CONSOLE_FONT_INFOEX consoleFontInfo;
			consoleFontInfo.cbSize = sizeof(CONSOLE_FONT_INFOEX);

			GetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), false, &consoleFontInfo);

			int charX = (consoleDisplayX / consoleFontInfo.dwFontSize.X)+1,
				charY = (consoleDisplayY / consoleFontInfo.dwFontSize.Y)+1;

			//wprintf(NosStdLib::String::CenterString<wchar_t>(std::format(L"{} | {}", charX, charY), true, true).c_str());

			wprintf(CharCoordPrint(charX, charY).c_str());
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {0, 0});
		}

		HHOOK MouseHook;

		LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam)
		{
			if (nCode <= 0)
			{
				PMSLLHOOKSTRUCT mouseHookStruct = (PMSLLHOOKSTRUCT)lParam;

				NosStdLib::Console::ShowCaret(false);

				std::async(std::launch::async, CalcCharPixel, mouseHookStruct);
			}

			return CallNextHookEx(MouseHook, nCode, wParam, lParam);
		}

		/// <summary>
		/// Initialize and create lowlevel Mouse callback
		/// </summary>
		bool InitializeMouseTracking()
		{
			DWORD prev_mode;
			return ((GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &prev_mode) &&
					SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), ENABLE_EXTENDED_FLAGS | (prev_mode & ~ENABLE_QUICK_EDIT_MODE))) &&
					(MouseHook = SetWindowsHookEx(WH_MOUSE_LL, NosStdLib::MouseTracking::MouseHookProc, NULL, NULL)));
		}
	}
}

#endif