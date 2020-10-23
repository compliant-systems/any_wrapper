#pragma once

#include <memory>

#ifndef _WIN32
#include <cxxabi.h>
#endif

/*
OS agnostic typename
works with GCC and CLANG on Linux etc ...
*/
#define DBJ_TYPE_NAME(T) dbj::name<T>().c_str()
#define DBJ_TYPENAME(T) dbj::name<decltype(T)>().c_str()     


namespace dbj {
	template < typename T >
	const std::string name() noexcept
	{
#ifdef _WIN32
		return { typeid(T).name() };
#else // __linux__
		// delete malloc'd memory
		struct free_ {
			void operator()(void* p) const { std::free(p); }
		};
		// custom smart pointer for c-style strings allocated with std::malloc
		using ptr_type = std::unique_ptr<char, free_>;

		// special function to de-mangle names
		int error{};
		ptr_type name{ abi::__cxa_demangle(typeid(T).name(), 0, 0, &error) };

		if (!error)        return { name.get() };
		if (error == -1)   return { "memory allocation failed" };
		if (error == -2)   return { "not a valid mangled name" };
		// else if(error == -3)
		return { "bad argument" };
#endif // __linux__
	} // name()
} // dbj

#define DBJ_TYPE_NAME(T) dbj::name<T>().c_str()
#define DBJ_TYPENAME(T) dbj::name<decltype(T)>().c_str()     
