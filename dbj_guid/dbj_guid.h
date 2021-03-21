//-------------------------------------------------------------------------------------------------------
// constexpr GUID parsing
// Written by Alexander Bessonov
// Written by Tobias Loew
// Written by DBJ -- note: this is not GUID generator. 
//                    Also vs WIN32 this is largely compile time excersize
//                    This file does not depend on std lib
//
// Licensed under the MIT license.
//-------------------------------------------------------------------------------------------------------
#ifndef DJB_GUID_INC
#define DJB_GUID_INC

#include <assert.h>
#include <stdint.h>

// https://gcc.gnu.org/onlinedocs/cpp/Pragmas.html
#define DO_PRAGMA_(x) _Pragma (#x)
#define DO_PRAGMA(x) DO_PRAGMA_(x)

#ifdef __clang__
#define NUSED_BEGIN \
	DO_PRAGMA(clang diagnostic push) \
	DO_PRAGMA(clang diagnostic ignored "-Wunused-variable")
#else
#define NUSED_BEGIN \
DO_PRAGMA( warning( push ) ) \
DO_PRAGMA( warning( disable : 4101) )
#endif  // __clang__

#ifdef __clang__
#define NUSED_END DO_PRAGMA(clang diagnostic pop)
#else  // ! __clang__
#define NUSED_END DO_PRAGMA( warning( pop ) ) 
#endif  // ! __clang__

// thing to be supressed must be on the next line
#ifdef _MSC_VER
#define DBJ_SUPRESS DO_PRAGMA(warning(suppress: 4101))
#else
#define DBJ_SUPRESS
#endif


namespace dbj {

	/*
	we will bravely try and stick to the 'GUID' name
	this is brave because of WIN32 includes that contain all
	sorts of GUID related macros
	basicaly if you need UUID + on win you include rpc.h
	in that scenario you do not need any kind of "portable" solution
	like this bellow is
	*/
	struct GUID final {
		uint32_t Data1{};
		uint16_t Data2{};
		uint16_t Data3{};
		uint8_t Data4[8]{};
	};

	// we do not use exceptions, on error we return null guid
	constexpr inline GUID null_guid{ 0,0,0, {0,0,0,0,0,0,0,0} };

	constexpr inline bool is_null(const GUID& guid_)
	{
		return (&guid_ == &null_guid);
	}

	// equality operators can be used at compile time too
	constexpr inline bool equal(const GUID& left_, const GUID& right_)
	{
		return
			(left_.Data1 == right_.Data1) &&
			(left_.Data2 == right_.Data2) &&
			(left_.Data3 == right_.Data3) &&
			(left_.Data4[0] == right_.Data4[0]) &&
			(left_.Data4[1] == right_.Data4[1]) &&
			(left_.Data4[2] == right_.Data4[2]) &&
			(left_.Data4[3] == right_.Data4[3]) &&
			(left_.Data4[4] == right_.Data4[4]) &&
			(left_.Data4[5] == right_.Data4[5]) &&
			(left_.Data4[6] == right_.Data4[6]) &&
			(left_.Data4[7] == right_.Data4[7]);
	}

	constexpr inline bool operator == (const GUID& left_, const GUID& right_) { return equal(left_, right_); }
	constexpr inline bool operator != (const GUID& left_, const GUID& right_) { return !equal(left_, right_); }

	namespace details
	{
		constexpr const size_t short_guid_form_length = 36;	// XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX
		constexpr const size_t long_guid_form_length = 38;	// {XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX}

		//
		constexpr uint8_t parse_hex_digit(const char c)
		{
			return
				('0' <= c && c <= '9')
				? c - '0'
				: ('a' <= c && c <= 'f')
				? 10 + c - 'a'
				: ('A' <= c && c <= 'F')
				? 10 + c - 'A'
				:
				(perror(__FILE__ " -- invalid character in GUID"), exit(0), '?');
		}

		constexpr uint8_t parse_hex_uint8_t(const char* ptr)
		{
			return (parse_hex_digit(ptr[0]) << 4) + parse_hex_digit(ptr[1]);
		}

		constexpr uint16_t parse_hex_uint16_t(const char* ptr)
		{
			return (parse_hex_uint8_t(ptr) << 8) + parse_hex_uint8_t(ptr + 2);
		}

		constexpr uint32_t parse_hex_uint32_t(const char* ptr)
		{
			return (parse_hex_uint16_t(ptr) << 16) + parse_hex_uint16_t(ptr + 4);
		}

		constexpr GUID parse_guid(const char* begin)
		{
			return GUID{
				parse_hex_uint32_t(begin),
				parse_hex_uint16_t(begin + 8 + 1),
				parse_hex_uint16_t(begin + 8 + 1 + 4 + 1),
				{
					parse_hex_uint8_t(begin + 8 + 1 + 4 + 1 + 4 + 1),
					parse_hex_uint8_t(begin + 8 + 1 + 4 + 1 + 4 + 1 + 2),
					parse_hex_uint8_t(begin + 8 + 1 + 4 + 1 + 4 + 1 + 2 + 2 + 1),
					parse_hex_uint8_t(begin + 8 + 1 + 4 + 1 + 4 + 1 + 2 + 2 + 1 + 2),
					parse_hex_uint8_t(begin + 8 + 1 + 4 + 1 + 4 + 1 + 2 + 2 + 1 + 2 + 2),
					parse_hex_uint8_t(begin + 8 + 1 + 4 + 1 + 4 + 1 + 2 + 2 + 1 + 2 + 2 + 2),
					parse_hex_uint8_t(begin + 8 + 1 + 4 + 1 + 4 + 1 + 2 + 2 + 1 + 2 + 2 + 2 + 2),
					parse_hex_uint8_t(begin + 8 + 1 + 4 + 1 + 4 + 1 + 2 + 2 + 1 + 2 + 2 + 2 + 2 + 2)
				}

			};
		}

		// on wrong input we do not throw exception
		// we set errno to EINVAL and return dbj::null_guid 
		constexpr GUID make_guid_helper(const char* str, size_t N)
		{
			using namespace details;

			if (!(N == long_guid_form_length || N == short_guid_form_length)) {
				perror(__FILE__ "\n\nString GUID of the form {XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX} or XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX is expected");
				errno = EINVAL;
				return null_guid;
			}

			if (N == long_guid_form_length && (str[0] != '{' || str[long_guid_form_length - 1] != '}'))
			{
				perror(__FILE__ "\n\nMissing opening or closing brace");
				errno = EINVAL;
				return null_guid;
			}

			return parse_guid(str + (N == long_guid_form_length ? 1 : 0));
		}

		// parse char array into GUID
		template<size_t N>
		constexpr GUID make_guid(const char(&str)[N])
		{
			return make_guid_helper(str, N - 1);
		}
	}
	using details::make_guid;

	/*
	Actually the only public interface is user defined literal.
	usage:
	using namespace dbj::literals;
	constexpr dbj::GUID guid_dbj = "{FE297330-BAA5-407F-BB47-F78752D2C209}"_guid;
	*/
	namespace literals
	{
		constexpr GUID operator "" _guid(const char* str, size_t N)
		{
			using namespace details;
			return make_guid_helper(str, N);
		}
	}
} // dbj

// mandatory portable helpers

// we have here portable UUID generator but I very much doubt
// "enterprise" developers will be allowed to use it

// note: rpc.h also includes windows.h so we shall not #include rpc.h
// Note: if this is used in the code which does include rpc.h 
// simply comment it out

#if defined(_WIN32)

#ifndef __RPC_H__

extern "C" {

#define RPC_S_OK 0

	typedef struct _GUID {
		unsigned long  Data1;
		unsigned short Data2;
		unsigned short Data3;
		unsigned char  Data4[8];
	} GUID;

	typedef _GUID UUID;
	typedef long RPC_STATUS;
	typedef unsigned const char* RPC_CSTR;

	__declspec(dllimport)
		long __stdcall UuidCreate(UUID*);

	__declspec(dllimport)
		long __stdcall UuidToStringA(
			const UUID*, unsigned const char**
		);

	__declspec(dllimport)
		long __stdcall RpcStringFreeA(unsigned const char**);

} // "C" 

#endif // __RPC_H__

// for RPC usage in WIN32 this lib manual inclusion is mandatory
#pragma comment(lib, "Rpcrt4.lib")
#include <crtdbg.h>

#else  // not _WIN32
#include "../uuid4/uuid4.h"
#endif // not _WIN32


// note: you know that anonymouys namespace leaves it to the linker what to do with 
// inlines inside it, which almost always in that scenario are turned into statics
namespace {
#if defined(_WIN32)
	/*
	windows uses DCE RPC
	*/
	inline dbj::GUID dbj_runtime_guid() {
		UUID legacy_uuid_;
		RPC_STATUS rpc_rezult_ = UuidCreate(&legacy_uuid_);
		_ASSERTE(rpc_rezult_ == RPC_S_OK);

		// Note: DCE strings are made of unsigned chars
		RPC_CSTR  string_uuid_;

		rpc_rezult_ = UuidToStringA(
			&legacy_uuid_,
			&string_uuid_
		);
		_ASSERTE(rpc_rezult_ == RPC_S_OK);

		dbj::GUID win_rpc_guid_ = dbj::details::make_guid_helper((const char*)string_uuid_, strlen((const char*)string_uuid_));

		rpc_rezult_ = RpcStringFreeA(
			&string_uuid_
		);
		_ASSERTE(rpc_rezult_ == RPC_S_OK);

		return win_rpc_guid_;
	}

#undef RPC_S_OK
#else // NOT _WIN32
	/*
	little non win portable uuid generator
	note: works for windows too
	*/
	inline dbj::GUID dbj_runtime_guid() {
		char string_uuid_[UUID4_LEN]{};
		uuid4_generate(string_uuid_);

		dbj::GUID win_rpc_guid_ =
			dbj::details::make_guid_helper(
				(const char*)string_uuid_,
				strlen((const char*)string_uuid_)
			);

		return win_rpc_guid_;
	}
#endif // NOT _WIN32


	inline void test_dbj_guid() {

		using namespace dbj::literals;

		// compile time 
		constexpr dbj::GUID guid_1 = "{FE297330-BAA5-407F-BB47-F78752D2C209}"_guid;
		constexpr dbj::GUID guid_2 = "{FE297330-BAA5-407F-BB47-F78752D2C209}"_guid;

		static_assert(guid_1 == guid_2);

		static_assert(!is_null(guid_1));

		// runtime 
		// this is portable function
		dbj::GUID  guid_3 = dbj_runtime_guid();

		assert(guid_1 != guid_3);
	}

} // nspace

#endif // !DJB_GUID_INC
