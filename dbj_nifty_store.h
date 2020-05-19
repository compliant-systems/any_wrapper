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
    struct protected_data final
    {
        using type = protected_data;
        using value_type = T;
        using  padlock = std::lock_guard<std::mutex> ;

        // not before here we use the result of the 
        // guid_source function
        static dbj::GUID store_guid() noexcept {
            type::padlock lock(type::protector_);
            return store_id_();
        }

        // store new value
        static value_type store(const T & new_val)
        {
            type::padlock lock(type::protector_);
            type::last_ = new_val;
            return type::last_;
        };

        // disallow temporaries
        static value_type store( T && ) = delete;


        // just read the stored value
        static value_type read(void) { 
            type::padlock lock( type::protector_ );
            return type::last_; 
        }

    private:
        inline static value_type last_{};
        inline static std::mutex protector_{};  // protects last_
    }; // protected_data

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

    inline void test_dbj_data_store()
    {
        using dbj::protected_data;

        // this is why we have second template argument
        // to have two different stores for the same data type
        using store_a = protected_data<int, guid_a >;
        using store_b = protected_data<int, guid_b >;

        int fty2 = 42;
        store_a::store(fty2);

        std::cout << "\nA Stored: " << store_a::read();
        std::cout << "\nB Stored: " << store_b::read();

       assert( store_a::store_guid() != store_b::store_guid() );
    }

}