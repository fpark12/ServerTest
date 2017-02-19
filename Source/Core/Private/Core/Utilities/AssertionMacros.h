/*
* Copyright (C) 2008-2016 TrinityCore <http://www.trinitycore.org/>
* Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the
* Free Software Foundation; either version 2 of the License, or (at your
* option) any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License along
* with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once

DECLSPEC_NORETURN Core_API void Assert(char const* file, int line, char const* function, char const* message) ATTR_NORETURN;
DECLSPEC_NORETURN Core_API void Assert(char const* file, int line, char const* function, char const* message, char const* format, ...) ATTR_NORETURN ATTR_PRINTF(5, 6);

DECLSPEC_NORETURN Core_API void Fatal(char const* file, int line, char const* function, char const* message, ...) ATTR_NORETURN ATTR_PRINTF(4, 5);

DECLSPEC_NORETURN Core_API void Error(char const* file, int line, char const* function, char const* message) ATTR_NORETURN;

DECLSPEC_NORETURN Core_API void Abort(char const* file, int line, char const* function) ATTR_NORETURN;

DECLSPEC_NORETURN Core_API void Warning(char const* file, int line, char const* function, char const* message) ATTR_NORETURN;

DECLSPEC_NORETURN Core_API void AbortHandler(int sigval) ATTR_NORETURN;


#if PLATFORM_WINDOWS
#define ASSERT_BEGIN __pragma(warning(push)) __pragma(warning(disable: 4127))
#define ASSERT_END __pragma(warning(pop))
#else
#define ASSERT_BEGIN
#define ASSERT_END
#endif

#define WPAssert(cond, ...) ASSERT_BEGIN do { if (!(cond)) Assert(__FILE__, __LINE__, __FUNCTION__, #cond, ##__VA_ARGS__); } while(0) ASSERT_END
#define WPFatal(cond, ...) ASSERT_BEGIN do { if (!(cond)) Fatal(__FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__); } while(0) ASSERT_END
#define WPError(cond, msg) ASSERT_BEGIN do { if (!(cond)) Error(__FILE__, __LINE__, __FUNCTION__, (msg)); } while(0) ASSERT_END
#define WPWarning(cond, msg) ASSERT_BEGIN do { if (!(cond)) Warning(__FILE__, __LINE__, __FUNCTION__, (msg)); } while(0) ASSERT_END
#define WPAbort() ASSERT_BEGIN do { Abort(__FILE__, __LINE__, __FUNCTION__); } while(0) ASSERT_END

#define ASSERT WPAssert
#define ABORT WPAbort

template <typename T> inline T* ASSERT_NOTNULL(T* pointer)
{
	ASSERT(pointer);
	return pointer;
}

class Status
{
private:
	enum class EStatusCode {
		OK = 0,
		FAILED = 1,
		UNKNOWN = 2
	};

	EStatusCode StatusCode;
	std::string ErrorMessage;

	inline std::string StatusCodeEnumToString(EStatusCode code) const;

public:


	Status();
	Status(EStatusCode statusCode, std::string errorMessage);
	Status(const Status& other);
	
	std::string ToString() const;

	Status& operator=(const Status& other);
	bool operator==(const Status& x) const;
	bool operator!=(const Status& x) const {
		return !operator==(x);
	}

	static const Status OK;             // Identical to 0-arg constructor
	static const Status FAILED;
	static const Status UNKNOWN;

	Status& operator<<(const Status& other);
};
typedef Status SC;