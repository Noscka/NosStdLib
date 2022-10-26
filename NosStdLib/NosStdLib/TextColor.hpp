﻿#ifndef _TEXTCOLOR_HPP_
#define _TEXTCOLOR_HPP_

#include "Global.hpp"
#include <codecvt>
#include <stdint.h>
#include <string>
#include <string_view>
#include <format>

namespace NosStdLib
{
	/// <summary>
	/// namespace contains items which are experimental (require more testing)
	/// </summary>
	namespace Experimental
	{
		/// <summary>
		/// Simpliefied function to 1 function instead of 2 overloads
		/// </summary>
		/// <typeparam name="CharT">- the type to get converted to</typeparam>
		/// <param name="globalWString">- the wstring that will get converted</param>
		/// <returns>converted string</returns>
		template <typename CharT>
		std::basic_string<CharT> ConvertGlobal(const std::wstring& globalWString)
		{
			if constexpr (std::is_same_v<CharT, char>)
				return NosStdLib::Global::String::ToString(globalWString);
			else if constexpr (std::is_same_v<CharT, wchar_t>)
				return std::wstring(globalWString);
		}
	}

	/// <summary>
	/// This namespace contains items which are related to text coloring (mostly using ANSI)
	/// </summary>
	namespace TextColor
	{
		/// <summary>
		/// class to represent RGB
		/// </summary>
		class NosRGB
		{
		public:
			uint8_t R; /* Red */
			uint8_t B; /* Blue */
			uint8_t G; /* Green */

			NosRGB(){}

			NosRGB(uint8_t r, uint8_t b, uint8_t g)
			{
				R = r;
				B = b;
				G = g;
			}
		};

		/// <summary>
		/// Create necesacry ANSI escape code to give wanted color
		/// </summary>
		/// <typeparam name="CharT">- string type</typeparam>
		/// <param name="values">- the RGB values wanted</param>
		/// <returns>the string containing the ANSI escape code</returns>
		template <typename CharT>
		std::basic_string<CharT> MakeANSICode(const NosStdLib::TextColor::NosRGB& values)
		{
			/* TODO: find or create terminoligy for a value that is constant in a function. */
			/* TODO: Create terminoligy table */
			return std::vformat(NosStdLib::Experimental::ConvertGlobal<CharT>(L"\033[38;2;{};{};{}m"), std::make_format_args<std::basic_format_context<std::back_insert_iterator<std::_Fmt_buffer<CharT>>, CharT>>(values.R, values.G, values.B));
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
			std::wstring ColorRow(const int& columnCount, const NosStdLib::TextColor::NosRGB& rgbValue = NosStdLib::TextColor::NosRGB(20, 180, 170))
			{
				return  NosStdLib::TextColor::MakeANSICode<wchar_t>(rgbValue) + std::wstring(max(columnCount, 0), L'█') + L"\033[0m";
			}

			/// <summary>
			/// function which iterates the ColorRow function to make the console output nice colors
			/// </summary>
			/// <param name="singleRow">(singleRow = false) - if it should only output on 1 row or file the whole console</param>
			/// <param name="sleepSpeed">(default = 1) - the speed to output at</param>
			void IterateRainbow(const bool& singleRow = false, const int& sleepSpeed = 1)
			{
				NosStdLib::Global::Console::ShowCaret(false);

				HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
				CONSOLE_SCREEN_BUFFER_INFO csbi;
				GetConsoleScreenBufferInfo(consoleHandle, &csbi);
				int columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
				int rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

				int R = 255, G = 0, B = 0;


				while (true)
				{
					if (R > 0 && B == 0)
					{
						R--;
						G++;
					}
					if (G > 0 && R == 0)
					{
						G--;
						B++;
					}
					if (B > 0 && G == 0)
					{
						R++;
						B--;
					}

					wprintf(ColorRow(columns, NosStdLib::TextColor::NosRGB(R, G, B)).c_str());
					if(singleRow)
						SetConsoleCursorPosition(consoleHandle, { 0, 0 });
					Sleep(sleepSpeed);
				}

				NosStdLib::Global::Console::ShowCaret(true);
			}
		}
	}
}
#endif