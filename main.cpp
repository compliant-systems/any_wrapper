
#include "dbj_nifty_store.h"
#include "dbj_any_wrapper.h"

// makes everything inside it static
// as long as you are aware ...
namespace {

	auto print_dbj_any = [](FILE * fp_ , const char* prefix_, auto dbj_any) {
		// depends on to_string() correct implementation
		std::fprintf( fp_, "%s %s", prefix_, dbj_any.to_string().c_str());
	};

	void test_dbj_any_wrapper_range () {

		using namespace dbj;
		try {
			// transforming native array T[N] into std::array of 
			// any wrapped T's
			int int_arr[]{ 42 };

			std::array arr_of_wraps = any::wrapper_range(int_arr);

			// value type of any_0 is any wrapper
			using wrapper_type = typename decltype(arr_of_wraps)::value_type;
			using wrapper_data_type = wrapper_type::data_type;

			std::printf(
				"\n\nTransformed %s into %s ", DBJ_TYPENAME(int_arr), DBJ_TYPENAME(arr_of_wraps)
			);

			std::printf(" [");
			for (auto & elem : arr_of_wraps) {
				printf(" %s", elem.to_string().c_str());
			}
			std::printf(" ]");

			{
				// yes can do
				// string literal to std::array kept 
				// inside dbj::any::wrapper inside std::any
				char word_[] = "Hello dbj any!";
				std::array arr_of_wraps = dbj::any::wrapper_range(word_);
				using v2_data_t = typename decltype(arr_of_wraps)::value_type;

				std::printf(
					"\n\nTransformed %s into %s ", 
					DBJ_TYPENAME(word_), DBJ_TYPENAME(arr_of_wraps)
				);
			}
		}
		catch (...) {
			std::printf(
				"  Unknown exception caught in %s ",
				__func__
			);
		}
	}
}

int main() {

	dbj::test_dbj_data_store();
	// test_dbj_any_wrapper();
	printf("\n\n%s\n\n", std::string(dbj::any::wrapper<int>(42)).c_str());
	printf("\n\n%s\n\n", std::string(dbj::any::wrapper<bool>(42)).c_str());
}


