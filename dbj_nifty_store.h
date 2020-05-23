#pragma once

// (c) 2020 by dbj@dbj.org CC BY SA 4.0

// dbj nifty store -- aka "C++ Snake Oil" Inc.
//  clang++ prog.cc -Wall -Wextra -std=c++14
#include <iostream>
#include <utility>
#include <mutex>
#include <cstdlib>
#include "dbj_guid/dbj_guid.h"
#include "uuid4/uuid4.h"

namespace dbj {

	struct padlock final {

		using type = padlock;
		inline static std::mutex protector_{};  // protects last_
		using  guard = std::lock_guard<std::mutex>;

		padlock() {
			guard lock(type::protector_);
		}
	};

	struct nolock final {
		using type = nolock;
		nolock() = default;
	};

	using guid_source = dbj::GUID(*)();

	// this class has only one data member and no methods
	// ctros or dtors
	// thus its moveability and copyability depends entirely
	// on the moveability and copyability of that value type
	// all ireleveant since this is statics only class
	// menaing type contains all the functionality
	// not the instance
	template<
		typename T,
		// at compile time function pointer itself is 
		// giving unique ID not its call result
		// at runtime stor_id_ provides the actuall GUID
		guid_source store_id_,
		// by defauly we de not lock
		typename LOCK = nolock
	>
		struct data final
	{
		using type = data;
		using value_type = T;
		using lock_type = LOCK;


		// not before here we use the result of the 
		// guid_source function
		static dbj::GUID store_guid() noexcept {
			LOCK guard;
			return store_id_();
		}

		// store new value
		static value_type store(const T& new_val) noexcept
		{
			lock_type guard;
			type::last_ = new_val;
			return type::last_;
		};

		// disallow temporaries
		static value_type store(T&&) = delete;


		// just read the stored value
		static value_type read(void) noexcept {
			lock_type guard;
			return type::last_;
		}

	private:
		inline static value_type last_{};

	}; // data

} // dbj



namespace {

	// identification source is a function which returns dbj::GUID
	// footprint is: dbj::GUID (*)();

	// using the UDL and compile time 
	constexpr inline dbj::GUID guid_a() {
		using namespace dbj::literals;
		return "{FE297330-BAA5-407F-BB47-F78752D2C209}"_guid;
	}

	// using the WIN or not WIN generator 
	inline dbj::GUID guid_b() {
		static auto dbj_legacy_guid = dbj_runtime_guid();
		return dbj_legacy_guid;
	}

	// here we show we are dealing with the type 
	// not instances
	template< typename STORE >
	inline void store_user(
		typename STORE::value_type const& new_val_
	)
	{
		STORE::store(new_val_);
	}

	// we could pass store instance as argument
	template< typename STORE >
	inline void store_silly_user(
		STORE store_instance_, typename STORE::value_type const& new_val_
	)
	{
		store_instance_.store(new_val_);
	}

	// dissalow dangling ref's
	template< typename STORE >
	inline void store_user(typename STORE::value_type&&) = delete;

	// NOTE: above we do not car or deal with thread resilience
	// store's are thread resilient or not
	// like store_b is and store_a is not

	inline void test_dbj_data_store()
	{
		using dbj::data;

		// this is why we have second template argument
		// to have two different stores for the same data type
		using store_a = data<int, guid_a >;

		// second store protects the data in presence of multipla threads
		using store_b = data<int, guid_b, dbj::padlock >;

		int fty2 = 42;
		int th_teen = 13;
		store_a::store(fty2);
		store_user<store_a>(fty2);

		// we could do it this way
		// but that is slower since we copy the instance in
		// and the result is the same but slower
		// you just do not need instances
		store_silly_user(store_b{}, th_teen);

		std::cout << "\nA Stored: " << store_a::read();
		std::cout << "\nB Stored: " << store_b::read();

		assert(store_a::store_guid() != store_b::store_guid());
	}

}