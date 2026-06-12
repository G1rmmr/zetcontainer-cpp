#include "doctest.h"
#include "List.hpp"

TEST_CASE("List basic operations") {
    zet::List<int, 5> list;
    
    SUBCASE("Initialization") {
        CHECK(list.Size() == 0);
        CHECK(list.Capacity() == 5);
    }
    
    SUBCASE("Push and Pop") {
        list.Push(10);
        list.Push(20);
        CHECK(list.Size() == 2);
        CHECK(list[0] == 10);
        CHECK(list[1] == 20);
        
        list.Pop();
        CHECK(list.Size() == 1);
        CHECK(list[0] == 10);
    }
    
    SUBCASE("Clear") {
        list.Push(10);
        list.Push(20);
        list.Clear();
        CHECK(list.Size() == 0);
    }
    
    SUBCASE("Copy constructor and assignment") {
        list.Push(1);
        list.Push(2);
        
        zet::List<int, 5> list2(list);
        CHECK(list2.Size() == 2);
        CHECK(list2[0] == 1);
        CHECK(list2[1] == 2);
        
        zet::List<int, 5> list3;
        list3 = list;
        CHECK(list3.Size() == 2);
        CHECK(list3[0] == 1);
        CHECK(list3[1] == 2);
    }
    
    SUBCASE("Move constructor and assignment") {
        list.Push(1);
        list.Push(2);
        
        zet::List<int, 5> list2(std::move(list));
        CHECK(list2.Size() == 2);
        CHECK(list2[0] == 1);
        CHECK(list2[1] == 2);
        CHECK(list.Size() == 0); // list was cleared on move
    }
    
    SUBCASE("Iterators") {
        list.Push(10);
        list.Push(20);
        int sum = 0;
        for (int val : list) {
            sum += val;
        }
        CHECK(sum == 30);
    }
}
