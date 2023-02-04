#ifndef _MOUSETRACKING_NOSSTDLIB_HPP_
#define _MOUSETRACKING_NOSSTDLIB_HPP_

#include "../Console.hpp"
#include "../RGB.hpp"

#include <Windows.h>
#include <stdio.h>
#include <iostream>

namespace NosStdLib
{
	/// <summary>
	/// namespace that contains mouse tracking functions which are "fun"
	/// </summary>
	namespace MouseTrackingFun
	{
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
		/// <param name="consoleContext">- consoleContext (has to be the same one that was used in the ConsoleMSPaintInit function)</param>
		/// <param name="brushThickness">- how thick the brush is</param>
		/// <param name="brushType">(default = BrushType::square) - the brush type</param>
		void ConsoleMSPaint(const PMSLLHOOKSTRUCT& mouseHookStruct, const HDC& consoleContext, const int& brushThickness, const BrushType& brushType = BrushType::square)
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
							SetPixel(consoleContext, consoleDisplayX + i, consoleDisplayY + j, paintColor);
						}
					}
					break;

				case BrushType::star:
					for (int i = (-1 * brushThickness)/2; i <= brushThickness /2; i++)
					{
						SetPixel(consoleContext, consoleDisplayX +i, consoleDisplayY, paintColor);
					}
					for (int i = (-1 * brushThickness)/2; i <= brushThickness /2; i++)
					{
						SetPixel(consoleContext, consoleDisplayX, consoleDisplayY + i, paintColor);
					}
					break;
				}
			}
		}

		/// <summary>
		/// Function needed to make ConsoleMSPaint actually run
		/// </summary>
		/// <param name="consoleHandle">- pointer to consoleHandle (can be uninitialize as in unassigned)</param>
		/// <param name="consoleContext">- pointer to consoleContext (can be uninitialize as in unassigned)</param>
		void ConsoleMSPaintInit(HWND* consoleHandle, HDC* consoleContext)
		{
			wprintf(L"Press any button to enter NosPaint"); _getch();

			*consoleHandle = GetConsoleWindow();
			*consoleContext = GetDC(*consoleHandle);
		}
	}

	/// <summary>
	/// Namespace which tracks mouse position
	/// </summary>
	namespace MouseTracking
	{
		HHOOK MouseHook;

		HWND ConsoleHandle;
		HDC ConsoleContext;

		LRESULT CALLBACK mouseHookProc(int nCode, WPARAM wParam, LPARAM lParam)
		{
			if (nCode <= 0)
			{
				PMSLLHOOKSTRUCT mouseHookStruct = (PMSLLHOOKSTRUCT)lParam;

				NosStdLib::Console::ShowCaret(false);

				NosStdLib::MouseTrackingFun::ConsoleMSPaint(mouseHookStruct, ConsoleContext, 10, NosStdLib::MouseTrackingFun::BrushType::square);
			}

			return CallNextHookEx(MouseHook, nCode, wParam, lParam);
		}

		/// <summary>
		/// Initialize and create lowlevel Mouse callback
		/// </summary>
		bool InitializeMouseTracking()
		{
			NosStdLib::MouseTrackingFun::ConsoleMSPaintInit(&ConsoleHandle, &ConsoleContext);

			DWORD prev_mode;
			return ((GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &prev_mode) &&
					SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), ENABLE_EXTENDED_FLAGS | (prev_mode & ~ENABLE_QUICK_EDIT_MODE))) &&
					(MouseHook = SetWindowsHookEx(WH_MOUSE_LL, NosStdLib::MouseTracking::mouseHookProc, NULL, NULL)));
		}
	}
}

#endif