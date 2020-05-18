#pragma once

// (c) 2020 by dbj@dbj.org CC BY SA 4.0

// dbj nifty store -- aka "C++ Snake Oil" Inc.
//  clang++ prog.cc -Wall -Wextra -std=c++14
#include <iostream>
#include <utility>
#include <cstdlib>
#include "dbj_guid.h"
#include "uuid4/uuid4.h"

namespace dbj {

    using guid_source = dbj::GUID (*)();

    // "store" "any" "data" -- nifty?
    // problem: one store per one type
    // thus store_id()
    // note: at compile time function pointer itself is 
    // giving unique ID not its call result
    // this class has only one data member and no methods
    // ctros or dtors
    // thus its moveability and copyability depends entirely
    // on the moveability and copyability of that value type
    template<typename T , guid_source store_id_ >
    struct data final
    {
        using type = data;
        using value_type = T;

        // not before here we use the result of the 
        // guid_source function
        static dbj::GUID store_guid() noexcept {
            return store_id_();
        }

        inline static value_type last_{ };

        // store new value
        // by mandating pointer arg
        // we disallow temporaries
        static value_type store(const T* new_val)
        {
            if (new_val) type::last_ = *new_val;
            return type::last_;
        };

        // just read the stored value
        static value_type read(void) { return type::last_; }

    }; // data

} // dbj



namespace {

    // identification source is a function which returns dbj::GUID
    // footprint is: dbj::GUID (*)();

    // using the UDL
    inline dbj::GUID guid_a() {
        using namespace dbj::literals;
        return "{FE297330-BAA5-407F-BB47-F78752D2C209}"_guid;
    }

    // using the WIN or not WIN generator 
    inline dbj::GUID guid_b() {
        static auto dbj_legacy_guid = dbj_runtime_guid();
        return dbj_legacy_guid;
    }

    inline void test_dbj_data_store()
    {

        using dbj::data;
        using namespace dbj::literals;
        constexpr dbj::GUID guid_dbj_a = "{FE297330-BAA5-407F-BB47-F78752D2C209}"_guid;
        constexpr dbj::GUID guid_dbj_b = "{AB297330-BAA5-407F-BB47-F78752D2C209}"_guid;

        // this is why we have second template argument
        // to have two different stores for the same data type
        using store_a = data<int, guid_a >;
        using store_b = data<int, guid_b >;

        int fty2 = 42;
        store_a::store(&fty2);

        std::cout << "\nA Stored: " << store_a::read();
        std::cout << "\nB Stored: " << store_b::read();

        auto guido = store_a::store_guid();
        auto guida = store_b::store_guid();
    }

}