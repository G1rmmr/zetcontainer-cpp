#include "doctest.h"
#include "String.hpp"
#include <string_view>

TEST_CASE("String basic operations") {
    SUBCASE("Construction") {
        mir::String<32> emptyStr;
        CHECK(emptyStr.Empty());
        CHECK(emptyStr.Size() == 0);
        
        mir::String<32> hello("Hello");
        CHECK_FALSE(hello.Empty());
        CHECK(hello.Size() == 5);
        CHECK(hello == "Hello");
        CHECK(hello.View() == "Hello");
        
        mir::String<32> svStr(std::string_view("World"));
        CHECK(svStr == "World");
        
        mir::String<32> filled(5, 'a');
        CHECK(filled == "aaaaa");
    }
    
    SUBCASE("Concatenation") {
        mir::String<32> str("Hello");
        str += " ";
        str += std::string_view("World");
        CHECK(str == "Hello World");
        
        mir::String<32> str2 = mir::String<32>("Foo") + "Bar";
        CHECK(str2 == "FooBar");
    }
    
    SUBCASE("Indexing") {
        mir::String<32> str("Test");
        CHECK(str[0] == 'T');
        CHECK(str[3] == 't');
        
        str[1] = 'e';
        CHECK(str == "Test");
    }
}
