﻿#ifndef _TEXTCOLOR_NOSSTDLIB_HPP_
#define _TEXTCOLOR_NOSSTDLIB_HPP_

#include "Console.hpp"
#include "RGB.hpp"
#include "String.hpp"

#include <codecvt>
#include <stdint.h>
#include <string>
#include <format>

namespace NosStdLib
{
	/// <summary>
	/// This namespace contains items which are related to text coloring (mostly using ANSI)
	/// </summary>
	namespace TextColor
	{
		/// <summary>
		/// Create necesacry ANSI escape code to give wanted color
		/// </summary>
		/// <typeparam name="CharT">- string type</typeparam>
		/// <param name="value">- the RGB values wanted</param>
		/// <param name="foreGroundBackGround">(default = true) - if the ANSI should output foreground (true) or background (false)</param>
		/// <returns>the string containing the ANSI escape code</returns>
		template <typename CharT>
		std::basic_string<CharT> MakeANSICode(const NosStdLib::RGB::NosRGB& value, const bool& foreGroundBackGround = true)
		{
			return value.MakeANSICode<CharT>(foreGroundBackGround);
		}

		/// <summary>
		/// namespace which contains functions which aren't useful but are entraiting or fun
		/// </summary>
		namespace Fun
		{
			/// <summary>
			/// Function which creates a line of characters with the specified color
			/// </summary>
			/// <param name="columnCount">- the amount of columns</param>
			/// <param name="rgbValue">(default = (20, 180, 170)) - the RGB value wanted</param>
			/// <returns>a row with a color</returns>
			std::wstring ColorRow(const int& columnCount,const NosStdLib::RGB::NosRGB& rgbValue = NosStdLib::RGB::NosRGB(20, 180, 170))
			{
				return NosStdLib::TextColor::MakeANSICode<wchar_t>(rgbValue, true) + std::wstring(max(columnCount, 0), L'█') + L"\033[0m";
			}

			/// <summary>
			/// function which iterates the ColorRow function to make the console output nice colors
			/// </summary>
			/// <param name="singleRow">(singleRow = false) - if it should only output on 1 row or file the whole console</param>
			/// <param name="sleepSpeed">(default = 1) - the speed to output at</param>
			void IterateRainbow(const bool& singleRow = false, const int& sleepSpeed = 1)
			{
				NosStdLib::Console::ShowCaret(false);

				HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
				CONSOLE_SCREEN_BUFFER_INFO csbi;
				GetConsoleScreenBufferInfo(consoleHandle, &csbi);

				NosStdLib::RGB::NosRGB currentColor(255, 0, 0, 1); /* make last one 85 for lego mode */

				while (true)
				{
					currentColor.Iterate();

					wprintf(ColorRow(csbi.srWindow.Right - csbi.srWindow.Left + 1, currentColor).c_str());
					if(singleRow)
						SetConsoleCursorPosition(consoleHandle, { 0, 0 });
					Sleep(sleepSpeed);
				}

				NosStdLib::Console::ShowCaret(true);
			}
		}
	}
}
#endif