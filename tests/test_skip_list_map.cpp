//============================================================================
// test_skip_list_map.cpp
//============================================================================

#include "skip_list_map.h"

#define CATCH_CONFIG_NO_STREAM_REDIRECTION
#include "catch.hpp"
#include "test_types.h"

#include <map>
#include <string>

using goodliffe::skip_list_map;

TEST_CASE( "skip_list_map/smoketest", "" )
{
}

// JL: this is fairly bare-bones, I'm just testing the stuff that differs in
// from skip_list itself, as most of the code is shared.

//============================================================================
// inserting one item

TEST_CASE( "skip_list_map/inserting one item is not empty()", "" )
{
    skip_list_map<int, std::string> map;
    map.insert(std::make_pair(10, "ten"));
    REQUIRE_FALSE(map.empty());
}

TEST_CASE( "skip_list_map/inserting one item begin() is not end()", "" )
{
    skip_list_map<int, std::string> map;
    map.insert(std::make_pair(10, "ten"));
    REQUIRE(map.begin() != map.end());
    REQUIRE(map.cbegin() != map.cend());
    REQUIRE_FALSE(map.begin() == map.end());
    REQUIRE_FALSE(map.cbegin() == map.cend());

    const skip_list_map<int, std::string> &cmap(map);
    REQUIRE(cmap.cbegin() != cmap.cend());
    REQUIRE_FALSE(cmap.cbegin() == cmap.cend());
}

TEST_CASE( "skip_list_map/inserting one item returned from front()", "" )
{
    skip_list_map<int, std::string> map;
    map.insert(std::make_pair(10, "ten"));
    REQUIRE(map.front().first == 10);
	REQUIRE(map.front().second == "ten");
    
    const skip_list_map<int, std::string> &cmap(map);
    REQUIRE(cmap.front().first == 10);
	REQUIRE(cmap.front().second == "ten");
}

TEST_CASE( "skip_list_map/inserting one item returned from begin()", "" )
{
    skip_list_map<int, std::string> map;
    map.insert(std::make_pair(10, "ten"));
    REQUIRE(map.begin()->first == 10);
    REQUIRE(map.begin()->second == "ten");
    
    const skip_list_map<int, std::string> &cmap(map);
    REQUIRE(cmap.begin()->first == 10);
    REQUIRE(cmap.begin()->second == "ten");
}

TEST_CASE( "skip_list_map/inserting one item returns a count()", "" )
{
    skip_list_map<int, std::string> map;
    map.insert(std::make_pair(10, "ten"));
    REQUIRE(map.count(0) == 0);
    REQUIRE(map.count(1) == 0);
    REQUIRE(map.count(10) == 1);
    REQUIRE(map.count(11) == 0);
}

TEST_CASE( "skip_list_map/inserting one item returns correct contains()", "" )
{
    skip_list_map<int, std::string> map;
    map.insert(std::make_pair(10, "ten"));
    REQUIRE(map.contains(10));
    REQUIRE_FALSE(map.contains(1));
    REQUIRE_FALSE(map.contains(9));
    REQUIRE_FALSE(map.contains(11));
}

TEST_CASE( "skip_list_map/insert() returns iterator to inserted element", "" )
{
    skip_list_map<int, std::string> map;
    
    skip_list_map<int, std::string>::insert_by_value_result r = map.insert(std::make_pair(10, "ten"));
    REQUIRE(r.second);
    REQUIRE(r.first->first == 10);
    REQUIRE(r.first->second == "ten");
}

TEST_CASE( "skip_list_map/can't insert same item twice", "" )
{
    skip_list_map<int, std::string> map;
    
    map.insert(std::make_pair(10, "ten"));
    REQUIRE(map.size() == 1);

    skip_list_map<int, std::string>::insert_by_value_result r = map.insert(std::make_pair(10, "ten"));
    REQUIRE(!r.second);
    REQUIRE(r.first == map.end());
    REQUIRE(map.size() == 1);
    
    skip_list_map<int, std::string>::iterator i = map.begin();
    REQUIRE(i->first == 10);
    REQUIRE(i->second == "ten");
	i++;	
	REQUIRE(i == map.end());
}

TEST_CASE( "skip_list_map/impl/double insert middle value", "" )
{
    skip_list_map<int, std::string> map;
    
    map.insert(std::make_pair(10, "ten"));
    map.insert(std::make_pair(30, "thirty"));
    map.insert(std::make_pair(20, "twenty"));
    REQUIRE(map.size() == 3);

    skip_list_map<int, std::string>::insert_by_value_result r = map.insert(std::make_pair(20, "twenty"));
    REQUIRE(!r.second);
    REQUIRE(map.size() == 3);
}

//============================================================================
// erasing by key

TEST_CASE( "skip_list_map/erase/not contained item doesn't break things", "" )
{
    skip_list_map<int, std::string> map;
    map.insert(std::make_pair(10, "ten"));
    REQUIRE(map.erase(11) == 0);

    REQUIRE(map.count(10) == 1);
    REQUIRE(map.begin() != map.end());
}

TEST_CASE( "skip_list_map/erase/only item", "" )
{
    skip_list_map<int, std::string> map;
    map.insert(std::make_pair(10, "ten"));

    REQUIRE(map.erase(10) == 1);
    REQUIRE(map.count(10) == 0);
    REQUIRE(map.begin() == map.end());
    
    REQUIRE(map.erase(10) == 0);
}

TEST_CASE( "skip_list_map/erase/first item", "" )
{
    skip_list_map<int, std::string> map;
    map.insert(std::make_pair(10, "ten"));
    map.insert(std::make_pair(20, "twenty"));
    
    REQUIRE(map.size() == 2);
    REQUIRE(map.erase(10) == 1);
    REQUIRE(map.size() == 1);
    REQUIRE(map.front().first == 20);
    REQUIRE(map.front().second == "twenty");

    REQUIRE(map.count(10) == 0);
    REQUIRE(map.size() == 1);
}

TEST_CASE( "skip_list_map/erase/last item", "" )
{
    skip_list_map<int, std::string> map;
    map.insert(std::make_pair(10, "ten"));
    map.insert(std::make_pair(20, "twenty"));
    
    REQUIRE(map.size() == 2);
    REQUIRE(map.erase(20) == 1);
    REQUIRE(map.size() == 1);
    REQUIRE(map.front().first == 10);
    REQUIRE(map.front().second == "ten");
    
    REQUIRE(map.count(20) == 0);
}

//============================================================================
// find

TEST_CASE( "skip_list_map/find/empty map", "" )
{
    skip_list_map<int, std::string> map;
    const skip_list_map<int, std::string> &cmap = map;

    REQUIRE(map.find(12)  == map.end());
    REQUIRE(cmap.find(12) == map.end());
    REQUIRE(cmap.find(12) == cmap.end());
}

TEST_CASE( "skip_list_map/find/in a populated map", "" )
{
    skip_list_map<int, std::string> map;
    const skip_list_map<int, std::string> &cmap = map;

    map.insert(std::make_pair(30, "thirty"));
    map.insert(std::make_pair(10, "ten"));
    map.insert(std::make_pair(40, "forty"));
    map.insert(std::make_pair(20, "twenty"));
    map.insert(std::make_pair(0, "nought"));
    
    REQUIRE(map.find(12) == map.end());
    REQUIRE(cmap.find(12) == map.end());
    REQUIRE(cmap.find(12) == cmap.end());

    REQUIRE(map.find(10) != map.end());
    REQUIRE(map.find(10)->first == 10);
    REQUIRE(map.find(20)->first == 20);
    REQUIRE(map.find(30)->first == 30);
    REQUIRE(map.find(40)->first == 40);
    REQUIRE(map.find(0)->first == 0);
    REQUIRE(map.find(10)->second == "ten");
    REQUIRE(map.find(20)->second == "twenty");
    REQUIRE(map.find(30)->second == "thirty");
    REQUIRE(map.find(40)->second == "forty");
    REQUIRE(map.find(0)->second == "nought");
    
    REQUIRE(cmap.find(10) != cmap.end());
    REQUIRE(cmap.find(10)->first == 10);
    REQUIRE(cmap.find(20)->first == 20);
    REQUIRE(cmap.find(30)->first == 30);
    REQUIRE(cmap.find(40)->first == 40);
    REQUIRE(cmap.find(0)->first == 0);
    REQUIRE(cmap.find(10)->second == "ten");
    REQUIRE(cmap.find(20)->second == "twenty");
    REQUIRE(cmap.find(30)->second == "thirty");
    REQUIRE(cmap.find(40)->second == "forty");
    REQUIRE(cmap.find(0)->second == "nought");
}

TEST_CASE( "skip_list_map/find/in a populated map with several keys having the same value", "" )
{
    skip_list_map<int, std::string> map;
    const skip_list_map<int, std::string> &cmap = map;

	std::string value("value");
    map.insert(std::make_pair(30, value));
    map.insert(std::make_pair(10, value));
    map.insert(std::make_pair(40, value));
    map.insert(std::make_pair(20, value));
    map.insert(std::make_pair(0, value));
    
    REQUIRE(map.find(12) == map.end());
    REQUIRE(cmap.find(12) == map.end());
    REQUIRE(cmap.find(12) == cmap.end());

    REQUIRE(map.find(10) != map.end());
    REQUIRE(map.find(10)->first == 10);
    REQUIRE(map.find(20)->first == 20);
    REQUIRE(map.find(30)->first == 30);
    REQUIRE(map.find(40)->first == 40);
    REQUIRE(map.find(0)->first == 0);
    REQUIRE(map.find(10)->second == value);
    REQUIRE(map.find(20)->second == value);
    REQUIRE(map.find(30)->second == value);
    REQUIRE(map.find(40)->second == value);
    REQUIRE(map.find(0)->second == value);
    
    REQUIRE(cmap.find(10) != cmap.end());
    REQUIRE(cmap.find(10)->first == 10);
    REQUIRE(cmap.find(20)->first == 20);
    REQUIRE(cmap.find(30)->first == 30);
    REQUIRE(cmap.find(40)->first == 40);
    REQUIRE(cmap.find(0)->first == 0);
    REQUIRE(cmap.find(10)->second == value);
    REQUIRE(cmap.find(20)->second == value);
    REQUIRE(cmap.find(30)->second == value);
    REQUIRE(cmap.find(40)->second == value);
    REQUIRE(cmap.find(0)->second == value);
}


//============================================================================
// lower_bound

TEST_CASE( "skip_list_map/lower_bound/with empty map", "" )
{
    skip_list_map<int, std::string> map;
    
    REQUIRE(map.lower_bound(0) == map.end());
    REQUIRE(map.lower_bound(1) == map.end());
    REQUIRE(map.lower_bound(100) == map.end());
}

TEST_CASE( "skip_list_map/lower_bound/comparison with map", "" )
{
    std::map<int, std::string> stdmap; // we use map as a comparison of behaviour
	std::string str1("str1");
	std::string str2("str2");
	stdmap.insert(std::make_pair(5, str1));
    stdmap.insert(std::make_pair(7, str2));
    stdmap.insert(std::make_pair(11, str1));
    stdmap.insert(std::make_pair(21, str2));

    skip_list_map<int, std::string> map(stdmap.begin(), stdmap.end());

    REQUIRE(LowerBoundTest(5,  stdmap, 0)); REQUIRE(LowerBoundTest(5,  map, 0));
    REQUIRE(LowerBoundTest(7,  stdmap, 1)); REQUIRE(LowerBoundTest(7,  map, 1));
    REQUIRE(LowerBoundTest(11, stdmap, 2)); REQUIRE(LowerBoundTest(11, map, 2));
    REQUIRE(LowerBoundTest(21, stdmap, 3)); REQUIRE(LowerBoundTest(21, map, 3));

    REQUIRE(LowerBoundTest(0,  stdmap, 0)); REQUIRE(LowerBoundTest(0,  map, 0));
    REQUIRE(LowerBoundTest(4,  stdmap, 0)); REQUIRE(LowerBoundTest(4,  map, 0));
    REQUIRE(LowerBoundTest(6,  stdmap, 1)); REQUIRE(LowerBoundTest(6,  map, 1));
    REQUIRE(LowerBoundTest(8,  stdmap, 2)); REQUIRE(LowerBoundTest(8,  map, 2));
    REQUIRE(LowerBoundTest(10, stdmap, 2)); REQUIRE(LowerBoundTest(10, map, 2));
    REQUIRE(LowerBoundTest(15, stdmap, 3)); REQUIRE(LowerBoundTest(15, map, 3));
    REQUIRE(LowerBoundTest(22, stdmap, 4)); REQUIRE(LowerBoundTest(22, map, 4));
    
    const skip_list_map<int, std::string> &cmap = map;
    REQUIRE(LowerBoundTest(5,  cmap, 0));
    REQUIRE(LowerBoundTest(7,  cmap, 1));
    REQUIRE(LowerBoundTest(11, cmap, 2));
    REQUIRE(LowerBoundTest(21, cmap, 3));
    
    REQUIRE(LowerBoundTest(0,  cmap, 0));
    REQUIRE(LowerBoundTest(4,  cmap, 0));
    REQUIRE(LowerBoundTest(6,  cmap, 1));
    REQUIRE(LowerBoundTest(8,  cmap, 2));
    REQUIRE(LowerBoundTest(10, cmap, 2));
    REQUIRE(LowerBoundTest(15, cmap, 3));
    REQUIRE(LowerBoundTest(22, cmap, 4));
}

TEST_CASE( "skip_list_map/lower_bound/no uninitialised comparisons", "" )
{
    DistinctivelyInitialisedType zero(0);
    DistinctivelyInitialisedType one(1);
    DistinctivelyInitialisedType three(3);
    DistinctivelyInitialisedType five(5);
        
    skip_list_map<DistinctivelyInitialisedType, DistinctivelyInitialisedType> map;
    map.insert(std::make_pair(one, zero));
    map.insert(std::make_pair(three, five));

    REQUIRE(LowerBoundTest(zero,  map, 0));
    REQUIRE(LowerBoundTest(one,   map, 0));
    REQUIRE(LowerBoundTest(three, map, 1));
    REQUIRE(LowerBoundTest(five,  map, 2));
}

//============================================================================
// upper_bound

TEST_CASE( "skip_list_map/upper_bound/with empty map", "" )
{
    skip_list_map<int, std::string> map;
    REQUIRE(map.upper_bound(0) == map.end());
    REQUIRE(map.upper_bound(1) == map.end());
    REQUIRE(map.upper_bound(100) == map.end());
}

TEST_CASE( "skip_list_map/upper_bound/comparison with map", "" )
{
    std::map<int, std::string> stdmap; // we use stdmap as a comparison of behaviour
	std::string str1("str1");
	std::string str2("str2");
	stdmap.insert(std::make_pair(5, str1));
    stdmap.insert(std::make_pair(7, str2));
    stdmap.insert(std::make_pair(11, str1));
    stdmap.insert(std::make_pair(21, str2));
    
    skip_list_map<int, std::string> map(stdmap.begin(), stdmap.end());
    REQUIRE(UpperBoundTest(5,  stdmap, 1)); REQUIRE(UpperBoundTest(5,  map, 1));
    REQUIRE(UpperBoundTest(7,  stdmap, 2)); REQUIRE(UpperBoundTest(7,  map, 2));
    REQUIRE(UpperBoundTest(11, stdmap, 3)); REQUIRE(UpperBoundTest(11, map, 3));
    REQUIRE(UpperBoundTest(21, stdmap, 4)); REQUIRE(UpperBoundTest(21, map, 4));

    REQUIRE(UpperBoundTest(0,  stdmap, 0)); REQUIRE(UpperBoundTest(0,  map, 0));
    REQUIRE(UpperBoundTest(4,  stdmap, 0)); REQUIRE(UpperBoundTest(4,  map, 0));
    REQUIRE(UpperBoundTest(6,  stdmap, 1)); REQUIRE(UpperBoundTest(6,  map, 1));
    REQUIRE(UpperBoundTest(8,  stdmap, 2)); REQUIRE(UpperBoundTest(8,  map, 2));
    REQUIRE(UpperBoundTest(10, stdmap, 2)); REQUIRE(UpperBoundTest(10, map, 2));
    REQUIRE(UpperBoundTest(15, stdmap, 3)); REQUIRE(UpperBoundTest(15, map, 3));
    REQUIRE(UpperBoundTest(22, stdmap, 4)); REQUIRE(UpperBoundTest(22, map, 4));
    
    const skip_list_map<int, std::string> &cmap = map;
    REQUIRE(UpperBoundTest(5,  cmap, 1));
    REQUIRE(UpperBoundTest(7,  cmap, 2));
    REQUIRE(UpperBoundTest(11, cmap, 3));
    REQUIRE(UpperBoundTest(21, cmap, 4));
    
    REQUIRE(UpperBoundTest(0,  cmap, 0));
    REQUIRE(UpperBoundTest(4,  cmap, 0));
    REQUIRE(UpperBoundTest(6,  cmap, 1));
    REQUIRE(UpperBoundTest(8,  cmap, 2));
    REQUIRE(UpperBoundTest(10, cmap, 2));
    REQUIRE(UpperBoundTest(15, cmap, 3));
    REQUIRE(UpperBoundTest(22, cmap, 4));
}

TEST_CASE( "skip_list_map/upper_bound/no uninitialised comparisons", "" )
{
    DistinctivelyInitialisedType zero(0);
    DistinctivelyInitialisedType one(1);
    DistinctivelyInitialisedType three(3);
    DistinctivelyInitialisedType five(5);
        
    skip_list_map<DistinctivelyInitialisedType, DistinctivelyInitialisedType> map;
    map.insert(std::make_pair(one, five));
    map.insert(std::make_pair(three, zero));

    REQUIRE(UpperBoundTest(zero,  map, 0));
    REQUIRE(UpperBoundTest(one,   map, 1));
    REQUIRE(UpperBoundTest(three, map, 2));
    REQUIRE(UpperBoundTest(five,  map, 2));
}
