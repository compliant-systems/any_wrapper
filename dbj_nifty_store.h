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
    // thus store_id 
    template<typename T , guid_source store_id_ >
    struct data final
    {
        using type = data;

        static dbj::GUID store_guid() noexcept {
            return store_id_;
        }

        inline static T last_{ };

        static auto store(const T* new_val)
        {
            if (new_val) type::last_ = *new_val;
            return type::last_;
        };

        static auto store(void) { return type::last_; }

    }; // data

    inline dbj::GUID guid_a() { 
        using namespace dbj::literals;
        return "{FE297330-BAA5-407F-BB47-F78752D2C209}"_guid; 
    }

    inline dbj::GUID guid_b() { 
        using namespace dbj::literals;
        return "{AB297330-BAA5-407F-BB47-F78752D2C209}"_guid; 
    }

    inline void test_dbj_data_store()
    {
        using store_a = data<int, guid_a >;
        using store_b = data<int, guid_b >;

        int fty2 = 42;
        std::cout << "\nA Stored: " << store_a::store(&fty2);
        std::cout << "\nB Stored: " << store_b::store();
    }

} // dbj