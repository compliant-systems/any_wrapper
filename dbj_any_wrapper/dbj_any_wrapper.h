#pragma once

#include <any>
#include <array>
#include <string>
#include <cstdio>

#include "dbj_name.h"

// 2018 dbj@dbj created -- std any with identity
namespace dbj {

	namespace any {

		using namespace std;

		template <typename T> class wrapper;

		template <typename T>
		class wrapper final
		{

			static_assert(!std::is_reference<T>::value,
				"[dbj::wrapper] Can not use a reference type");

			std::any any_{};

		public:
			// types
			typedef wrapper type;
			typedef T data_type;

			// used by factory function
			wrapper() noexcept {};

			// give data
			//wrapper(const data_type& ref) noexcept
			//	: any_(ref) {
			//}

			explicit wrapper(data_type val_) noexcept
				: any_(val_) {
			}
			// copy
			wrapper(const wrapper& rhs) noexcept : any_(rhs.any_) { }
			wrapper& operator=(const wrapper& x) noexcept {
				if (this != &x) {
					this->any_ = x.any_;
				}
				return *this;
			}
			// move
			wrapper(wrapper&& rhs) noexcept : any_(move(rhs.any_)) {  }

			wrapper& operator=(wrapper&& x) noexcept {
				if (this != &x) {
					this->any_ = move(x.any_);
				}
				return *this;
			}
			// destruct
			~wrapper() { this->any_.reset(); }

			// using the stored value in std::any
			// access
			operator data_type& () const noexcept = delete;
			operator data_type const& () const noexcept { return move(this->get()); }
			operator data_type && () && noexcept { return move(this->get()); }

			// data_type && get()		const noexcept { return move(any_cast<data_type>(this->any_)); }

			// only if function is stored
			template< class... ArgTypes >
			invoke_result_t<T&, ArgTypes...>
				operator() (ArgTypes&&... args) const {
				if (!empty()) {
					return invoke(get(), forward<ArgTypes>(args)...);
				}

				perror("can not call on empty data wrapped ");
				exit(0);
			}

			data_type get() const noexcept {

				return any_cast<data_type>(this->any_);
				// return data_type{};
			}

			bool empty() const {
				return !(this->any_).has_value();
			}

			const std::string to_string() const noexcept
			{
				const auto val_ = this->get();
				// no need for template jockeying
				// STL will complain enough if dubious 
				// to_string is attempted
				return { std::to_string(val_) };
			}

			// not really necessary?
			operator const std::string() const {
				return
					std::string{ "dbj::any::wrapper<" }
					.append(typeid(T).name())
					.append(">")
					.append(" ==> ")
					.append(this->to_string());
			}

			// factory methods ----------------------------------------

			template <
				typename T,
				typename ANYW = typename type
			>
				static auto make(T val_)
				-> ANYW
			{
				static_assert(!std::is_same<const char*, T>(),
					"std::any::make() can not use 'char *' pointer argument");

				return ANYW{ val_ };
			};

		}; // any::wrapper

		// input is T[N] native array 
		// each element of an T[N] is any wrapped
		// and the result is kept inside std::aray
		// crazy, but true ...
		template <
			typename T,
			std::size_t N,
			typename ANYW = wrapper<T>,
			typename RETT = typename std::array< ANYW, N >
		>
			inline auto wrapper_range(const T(&arrf)[N])
			-> RETT
		{
			RETT rezult{};
			std::size_t j{ 0 };

			for (auto element : arrf) {
				rezult[j++] = ANYW::make(element);
			}
			return rezult;
		};
	} // any
} // dbj


// makes everything inside it static
// as long as you are aware ...
namespace {

	inline auto
		print_dbj_any = [](FILE* fp_, const char* prefix_, auto dbj_any)
		noexcept
	{
		// depends on to_string() correct implementation
		std::fprintf(fp_, "%s %s", prefix_, dbj_any.to_string().c_str());
	};

	inline  void
		test_dbj_any_wrapper_range()
		noexcept
	{
		auto types_show = [](auto int_arr, auto arr_of_wraps) noexcept {
			printf(
				"\n\nTransformed %s into %s ", DBJ_TYPENAME(int_arr), DBJ_TYPENAME(arr_of_wraps)
			);
		};
		auto arr_print = [](auto arr_of_wraps) noexcept {
			std::printf(" [");
			for (auto& elem : arr_of_wraps) {
				printf(" %s", elem.to_string().c_str());
			}
			std::printf(" ]");
		};

		using namespace dbj;

		// transforming native array T[N] into std::array of 
			// any wrapped T's
		int int_arr[]{ 42 };
		std::array arr_of_wraps = any::wrapper_range(int_arr);
		types_show(int_arr, arr_of_wraps);
		arr_print(arr_of_wraps);

		{
			char word_[] = "Hello dbj any!";
			std::array arr_of_wraps = dbj::any::wrapper_range(word_);
			types_show(word_, arr_of_wraps);
			arr_print(arr_of_wraps);
		}
	}
}


/* standard suffix */
#pragma comment( user, __FILE__ "(c) 2018 - 2021 by dbj@dbj.org | Version: " __DATE__ __TIME__ ) 
/*
Copyright 2017 - 2020 by dbj@dbj.org

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http ://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
