#ifndef _DYNAMICARRAY_NosLib_HPP_
#define _DYNAMICARRAY_NosLib_HPP_

#include "TypeTraits.hpp"
#include "Pointers.hpp"
#include "DynamicArray/ArrayPositionTrack.hpp"
#include "TypeTraits.hpp"

#include <iostream>

namespace NosLib
{
	/* TODO: Add modes (such as double mode were the array doubles in size instead of increasing by stepsize) */
	/* TODO: (Aly's Idea), add continues array. instead of moving full area to a bigger area, just continue the area in a empty place */

	/// <summary>
	/// Class which allows for making arrays that can self increase on data overflow
	/// </summary>
	/// <typeparam name="ArrayDataType">- datatype for the array</typeparam>
	template<class ArrayDataType>
	class DynamicArray
	{
	private:
		int ArraySize;						/* Array starting size and the size after it is resized */
		int ArrayDefaultSize;				/* Array starting size which doesn't change */
		ArrayDataType* MainArray;			/* Pointer to Array */
		int CurrentArrayIndex;				/* keeps track amount of objects in array */
		int ArrayStepSize;					/* how much the array will get increased by when it reaches the limit */
		bool DeleteObjectsOnDestruction;	/* If the array should destroy all the objects (if possible) when getting destroyed */

		typedef ArrayDataType* iterator;
		typedef const ArrayDataType* const_iterator;
	public:
	#pragma region Constructors
		/// <summary>
		/// Constructor with starting size and step size params for custom objects
		/// </summary>
		/// <param name="StartSize"> - Starting size of the array</param>
		/// <param name="StepSize"> - how much the array will increase each time it reaches the limit</param>
		/// <param name="deleteObjectsOnDestruction">(default = true) - If the array should destroy all the objects (if possible) when getting destroyed</param>
		DynamicArray(const int& startSize, const int& stepSize, const bool& deleteObjectsOnDestruction = true)
		{
			ArrayDefaultSize = ArraySize = startSize;
			ArrayStepSize = stepSize;
			DeleteObjectsOnDestruction = deleteObjectsOnDestruction;

			// ! DO NOT CHANGE !
			CurrentArrayIndex = 0;
			MainArray = new ArrayDataType[ArraySize]();
		}

		/// <summary>
		/// Constructor to set all the variables
		/// </summary>
		DynamicArray()
		{
			new (this) DynamicArray<ArrayDataType>(10, 10);
		}

		/// Destroy array contained in object
		~DynamicArray()
		{
			/* if a pointer and not a function, go through all entries and delete */
			if constexpr (std::is_pointer<ArrayDataType>::value && !std::is_function< NosLib::TypeTraits::remove_all_pointers_t<ArrayDataType> >::value)
			{
				if (DeleteObjectsOnDestruction)
				{
					for (int i = 0; i < CurrentArrayIndex; i++)
					{
						delete MainArray[i];
					}
				}
			}
			delete[] MainArray;
		}
	#pragma endregion

	#pragma region MainArray Modification
		/// <summary>
		/// Append single Object
		/// </summary>
		/// <param name="ObjectToAdd"> - Object to add</param>
		void Append(const ArrayDataType& objectToAdd)
		{
			if (CurrentArrayIndex >= ArraySize) // if Current Index pointer is more then the array size (trying to add to OutOfRange space)
			{
				ArrayDataType* TempArray = new ArrayDataType[ArraySize](); // Create new array which will store the original values

				for (int i = 0; i < ArraySize; i++) // assign/copy all values from Array to Temp
				{
					TempArray[i] = MainArray[i];
				}

				delete[] MainArray;

				ArraySize += ArrayStepSize; // expand the Array size
				MainArray = new ArrayDataType[ArraySize](); // over ride MainArray with new, bigger, array

				/*
				ArraySize-ArrayStepSize calculates TempArray size
				Copy all values from Temp array to "old" expanded array
				*/
				for (int i = 0; i < ArraySize - ArrayStepSize; i++)
				{
					MainArray[i] = TempArray[i];

				}

				delete[] TempArray;
			}

			if constexpr (std::is_base_of_v<NosLib::ArrayPositionTrack::PositionTrack, NosLib::TypeTraits::remove_all_pointers_t<ArrayDataType>>) /* if a child of PositionTracking, give it a position */
			{
				NosLib::Pointers::OneOffRootPointer<ArrayDataType>(objectToAdd)->ModifyArrayPosition(CurrentArrayIndex);
			}

			MainArray[CurrentArrayIndex] = objectToAdd;
			CurrentArrayIndex++;
		}

		/// <summary>
		/// Adds from the beginning address to the range to the end of current array
		/// </summary>
		/// <param name="beginning">- the beginning address</param>
		/// <param name="range">- the range of items wanted</param>
		void MultiAppend(ArrayDataType* beginning, const int& range)
		{
			for (int i = 0; i < range; i++){Append(beginning[i]);}
		}

		/// <summary>
		/// Adds from beginning address to end address to the end of current array
		/// </summary>
		/// <param name="beginning">- the beginning address</param>
		/// <param name="end">- the end address</param>
		void MultiAppend(ArrayDataType* beginning, ArrayDataType* end) /* TODO: Allow for custom starting point */
		{
			int distance = std::distance(beginning, end);
			MultiAppend(beginning, distance);
		}

		/// <summary>
		/// Replaces a a chosen position with the chosen Object
		/// </summary>
		/// <param name="ReplaceObject"> - Object to place in the position</param>
		/// <param name="position"> - position to put the Object in</param>
		void Replace(const ArrayDataType& replaceObject, const int& position)
		{
			if (position >= (CurrentArrayIndex - 1) || position < 0)// check if the position to remove is in array range
			{
				throw std::out_of_range("position was out of range of the array");
				return;
			}

			if constexpr (std::is_base_of_v<NosLib::ArrayPositionTrack::PositionTrack, NosLib::TypeTraits::remove_all_pointers_t<ArrayDataType>>) /* if a child of PositionTracking, give it a position */
			{
				NosLib::Pointers::OneOffRootPointer<ArrayDataType>(replaceObject)->ModifyArrayPosition(position);
			}

			MainArray[position] = replaceObject;
		}

		/// <summary>
		/// Remove object in position and move all Object in front, back 1 spot
		/// </summary>
		/// <param name="position">- Position to remove</param>
		/// <param name="deleteObject">(default = true) - if function should also delete the object</param>
		void Remove(const int& position, const bool& deleteObject = true)
		{
			if (position >= CurrentArrayIndex || position < 0)// check if the position to remove is in array range
			{
				throw std::out_of_range("position was out of range of the array");
				return;
			}

			for (int i = position; i < (CurrentArrayIndex - 1); i++) // moving all back
			{
				MainArray[i] = MainArray[i + 1];

				if constexpr (std::is_base_of_v<NosLib::ArrayPositionTrack::PositionTrack, NosLib::TypeTraits::remove_all_pointers_t<ArrayDataType>>) /* if a child of PositionTracking, give it a position */
				{
					NosLib::Pointers::OneOffRootPointer<ArrayDataType>(MainArray[i])->ModifyArrayPosition(i);
				}
			}

			if (deleteObject)
			{delete MainArray[CurrentArrayIndex - 1];}
			MainArray[CurrentArrayIndex - 1] = nullptr; // make last character blank
			CurrentArrayIndex--;
		}

		/// <summary>
		/// Remove object from array and move all objects in front back 1 spot (has to have a different name incase DataType is int)
		/// </summary>
		/// <param name="object">- object to find and remove</param>
		/// <param name="checkAll">(deleteObject = true) - if function should also delete the object</param>
		/// <param name="checkAll">(default = false) - if the for loop should check for all instances</param>
		void ObjectRemove(const ArrayDataType& object, const bool& deleteObject = true, const bool& checkAll = false)
		{
			for (int i = 0; i <= CurrentArrayIndex; i++)
			{
				if (object == MainArray[i]) { Remove(i, deleteObject); if (!checkAll) { return; } }
			}
		}

		/// <summary>
		/// Clear the dynamic array to the original size
		/// </summary>
		void Clear()
		{
			CurrentArrayIndex = 0;
			ArraySize = ArrayDefaultSize;
			MainArray = new ArrayDataType[ArrayDefaultSize]();
		}
	#pragma endregion

	#pragma region Variable Returns
		/// <summary>
		/// Return array contained in the object
		/// </summary>
		/// <returns>Object array</returns>
		ArrayDataType* GetArray()
		{
			return MainArray;
		}

		/// <summary>
		/// Returns the max array size (won't be the current index)
		/// </summary>
		/// <returns>int of current array size</returns>
		int GetArrayCurrentMaxSize()
		{
			return ArraySize;
		}

		/// <summary>
		/// Returns the starting size and the size it will return to when clearing
		/// </summary>
		/// <returns>int of starting size and the size it will return to when clearing</returns>
		int GetArrayStartMaxSize()
		{
			return ArrayDefaultSize;
		}

		/// <summary>
		/// Returns the 
		/// </summary>
		/// <returns></returns>
		int GetLastArrayIndex()
		{
			return CurrentArrayIndex-1;
		}

		/// <summary>
		/// Returns the steps size
		/// </summary>
		/// <returns>step size</returns>
		int GetArrayStepSize()
		{
			return ArrayStepSize;
		}
	#pragma endregion

	#pragma region For Loop Functions
		// For loop range-based function
		iterator begin() { return &MainArray[0]; }
		const_iterator begin() const { return &MainArray[0]; }
		iterator end() { return &MainArray[CurrentArrayIndex]; }
		const_iterator end() const { return &MainArray[CurrentArrayIndex]; }
	#pragma endregion

	#pragma region Operators

		/// <summary>
		/// Allows usage of << operator with all oStreams
		/// </summary>
		/// <typeparam name="CharT">- output stream character type</typeparam>
		/// <typeparam name="TraitsT">- output stream character traits</typeparam>
		/// <param name="oStreamReference">- output stream</param>
		/// <param name="MainArray">- the array object</param>
		/// <returns>Original stream refrence</returns>
		template <typename CharT, typename TraitsT>
		friend std::basic_ostream<CharT, TraitsT>& operator<<(std::basic_ostream<CharT, TraitsT>& oStreamReference, const DynamicArray& MainArray)
		{
			if constexpr (NosLib::TypeTraits::is_character<ArrayDataType>::value)
			{
				oStreamReference << MainArray.MainArray;
			}
			else
			{
				for (int i = 0; i < MainArray.CurrentArrayIndex; i++)
				{
					oStreamReference << MainArray.MainArray[i];

					if (!(i == MainArray.CurrentArrayIndex - 1))
					{
						oStreamReference << ", ";
					}
				}
			}
		
			return oStreamReference;
		}

		/// <summary>
		/// [] operator which acts the same as the Array [] operator
		/// </summary>
		/// <param name="position">- position of the value wanted</param>
		/// <returns>value in the position</returns>
		ArrayDataType& operator[](const int& position)
		{
			return MainArray[position];
		}

		/// <summary>
		/// Inserts object into self
		/// </summary>
		/// <param name="insersationObject">- the object to insert</param>
		/// <returns>combined objects</returns>
		DynamicArray<ArrayDataType>& operator<<(DynamicArray<ArrayDataType>& insersationObject)
		{
			this->MultiAppend(insersationObject.begin(), insersationObject.end());
			return *this;
		}

		/// <summary>
		/// Inserts object into self
		/// </summary>
		/// <param name="insersationObject">- the object to insert</param>
		/// <returns>combined objects</returns>
		DynamicArray<ArrayDataType>& operator+=(DynamicArray<ArrayDataType>& insersationObject)
		{
			this->MultiAppend(insersationObject.begin(), insersationObject.end());
			return *this;
		}
	#pragma endregion
	};
}
#endif