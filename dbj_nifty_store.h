#pragma once

// (c) 2020 by dbj@dbj.org CC BY SA 4.0

// dbj nifty store -- aka "C++ Snake Oil" Inc.
//  clang++ prog.cc -Wall -Wextra -std=c++14
#include <stdio.h>
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

	// this class has only one static data member and no methods
	// this is all statics type
	// menaing type contains all the functionality
	// not instances
	template<
		typename T,
		// at compile time function pointer itself is 
		// giving unique ID not its call result
		// at runtime store_id_ provides the actuall GUID
		guid_source store_id_,
		// by default we de not lock
		typename LOCK = nolock
	>
		struct data final
	{
		using type = data;
		using value_type = T;
		using lock_type = LOCK;


		// not before this point we use the result of the 
		// guid_source function
		static dbj::GUID store_guid() noexcept {
#pragma warning(suppress: 4101)
			LOCK guard;
			return store_id_();
		}

		// warning C4101
		// store new value
		static value_type store(const T& new_val) noexcept
		{
#pragma warning(suppress: 4101)
			lock_type guard;
			type::last_ = new_val;
			return type::last_;
		};

		// disallow temporaries
		static value_type store(T&&) = delete;

		// just read the stored value
		static value_type read(void) noexcept {
#pragma warning(suppress: 4101)
			lock_type guard;
			return type::last_;
		}

		// this is no instances type
		// thus we will stop that nonsense ;)

		data() = delete;
		~data() = delete;
		data(data const&) = delete;
		data& operator = (data const&) = delete;
		data(data&&) = delete;
		data& operator = (data&&) = delete;

	private:
		inline static value_type last_{};
	}; // data
} // dbj



namespace {

	// identification source is a function pointer which returns dbj::GUID
	// footprint is: dbj::GUID (*)();

	// _guid UDL is compile time UDL
	// dbj::GUID is also a literal type
	constexpr inline dbj::GUID guid_a() {
		using namespace dbj::literals;
		return "{FE297330-BAA5-407F-BB47-F78752D2C209}"_guid;
	}

	// option is
	// using the WIN or not WIN generator 
	inline dbj::GUID guid_b() {
		static auto dbj_legacy_guid = dbj_runtime_guid();
		return dbj_legacy_guid;
	}

	inline void test_dbj_data_store() noexcept
	{
		using dbj::data;

		// first store does not protect the data in presence of multipla threads
		using store_a = data<int, guid_a >;

		// second store protects the data in presence of multipla threads
		using store_b = data<int, guid_b, dbj::padlock >;

		// the type has the operations
		// temporary can not be used
		int fty2 = 42;
		store_b::store(fty2);

		// store_b::store(13); does not compile
		int one3 = 13;
		store_b::store(one3);

		printf("\nA has in store: %4d", store_a::read());
		printf("\nB has in store: %4d", store_b::read());

		assert(store_a::store_guid() != store_b::store_guid());
	}

}