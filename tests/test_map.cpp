#include "doctest.h"
#include "Map.hpp"
#include "String.hpp"

TEST_CASE("Map basic operations") {
    mir::Map<int, mir::String<32>, 10> myMap;
    
    SUBCASE("Insertion and Find") {
        CHECK(myMap.Size() == 0);
        
        myMap.Insert(1, "One");
        myMap.Insert(2, "Two");
        
        CHECK(myMap.Size() == 2);
        
        auto val1 = myMap.Find(1);
        REQUIRE(val1 != nullptr);
        CHECK(*val1 == "One");
        
        auto val2 = myMap.Find(2);
        REQUIRE(val2 != nullptr);
        CHECK(*val2 == "Two");
        
        auto val3 = myMap.Find(3);
        CHECK(val3 == nullptr);
    }
    
    SUBCASE("Update existing key") {
        myMap.Insert(1, "Original");
        myMap.Insert(1, "Updated");
        
        auto val = myMap.Find(1);
        REQUIRE(val != nullptr);
        CHECK(*val == "Updated");
        CHECK(myMap.Size() == 1);
    }
    
    SUBCASE("Clear") {
        myMap.Insert(1, "One");
        myMap.Insert(2, "Two");
        myMap.Clear();
        
        CHECK(myMap.Size() == 0);
        CHECK(myMap.Find(1) == nullptr);
    }
    
    SUBCASE("Iterator iteration") {
        myMap.Insert(10, "Ten");
        myMap.Insert(20, "Twenty");
        
        int keySum = 0;
        for (auto pair : myMap) {
            keySum += pair.key;
        }
        CHECK(keySum == 30);
    }
}
