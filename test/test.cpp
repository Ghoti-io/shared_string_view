/**
 * @file
 *
 * Test the shared_string_view behavior.
 */

#include <sstream>
#include <string>
#include <gtest/gtest.h>
#include "shared_string_view.hpp"

using namespace std;
using namespace Ghoti;

TEST(Constructor, Length) {
  // SSV created with a C-string.
  {
    shared_string_view ssv{""};
    ASSERT_EQ(ssv.length(), 0);
  }
  {
    shared_string_view ssv{"a"};
    ASSERT_EQ(ssv.length(), 1);
  }
  {
    shared_string_view ssv{"abc"};
    ASSERT_EQ(ssv.length(), 3);
  }
  // SSV created with a C-string and explicit length.
  {
    shared_string_view ssv{"abc", 3};
    ASSERT_EQ(ssv.length(), 3);
  }
  {
    shared_string_view ssv{"abc", 1};
    ASSERT_EQ(ssv.length(), 1);
  }
  // SSV created with a string object.
  {
    shared_string_view ssv{string("abc")};
    ASSERT_EQ(ssv.length(), 3);
  }
  // Copy constructor
  {
    shared_string_view ssv1{"abc"};
    shared_string_view ssv2{ssv1};
    ASSERT_EQ(string_view{ssv1}, string_view{ssv2});
  }
  // Move constructor
  {
    shared_string_view ssv1{"abc"};
    shared_string_view ssv2{move(ssv1)};
    ASSERT_NE(string_view{ssv1}, string_view{ssv2});
  }
}

TEST(Operator, string_stream) {
  {
    shared_string_view ssv{"abc 123"};
    ASSERT_EQ(string_view{ssv}, "abc 123"sv);
  }
  {
    shared_string_view ssv{"abc 123"};
    ASSERT_NE(string_view{ssv}, "abc 1234"sv);
  }
  {
    shared_string_view ssv{"abc 123"};
    ASSERT_NE(string_view{ssv}, ""sv);
  }
}

TEST(Operator, Extraction) {
  {
    shared_string_view ssv{"abc 123"};
    stringstream ss;
    ss << ssv;
    ASSERT_EQ(ss.str(), "abc 123");
  }
}

TEST(Operator, ThreeWayComparison) {
  shared_string_view ssv1{"abc 123"};
  shared_string_view ssv2{"abc 123"};
  shared_string_view ssv3{"abd 123"};

  // Test various comparison operators.
  ASSERT_EQ(ssv1, ssv2);
  ASSERT_NE(ssv1, ssv3);
  ASSERT_FALSE(ssv1 < ssv2);
  ASSERT_TRUE(ssv1 <= ssv2);
  ASSERT_FALSE(ssv1 > ssv2);
  ASSERT_TRUE(ssv1 >= ssv2);
  ASSERT_FALSE(ssv2 < ssv1);
  ASSERT_TRUE(ssv1 != ssv3);
  ASSERT_FALSE(ssv1 != ssv2);
  ASSERT_TRUE(ssv3 > ssv2);
  ASSERT_NE(ssv2, ssv3);

  // Force ssv2 and ssv3 to lose their target to make sure that this won't
  // cause a segfault.
  shared_string_view ssv4{move(ssv2)};
  shared_string_view ssv5{move(ssv3)};
  ASSERT_NE(ssv4, ssv5);
  ASSERT_NE(ssv1, ssv2);
  ASSERT_FALSE(ssv1 < ssv2);
  ASSERT_TRUE(ssv1 > ssv2);
  ASSERT_EQ(ssv2, ssv3);
}

TEST(Method, substr) {
  shared_string_view ssv{"abcdefghijklmnopqrstuvwxyz"};

  // Standard, in bounds substrings.
  ASSERT_EQ(ssv.substr(0,1), "a"sv);
  ASSERT_EQ(ssv.substr(1,1), "b"sv);
  ASSERT_EQ(ssv.substr(0,3), "abc"sv);
  ASSERT_EQ(ssv.substr(1,3), "bcd"sv);
  ASSERT_EQ(ssv.substr(23,3), "xyz"sv);

  // Substring requested is too long, return what is available.
  ASSERT_EQ(ssv.substr(25,3), "z"sv);
  ASSERT_EQ(ssv.substr(25,3).length(), 1);

  // Substring is out of bounds.
  ASSERT_EQ(ssv.substr(26,3), ""sv);
  ASSERT_EQ(ssv.substr(26,3).length(), 0);
  ASSERT_EQ(ssv.substr(42,3), ""sv);
  ASSERT_EQ(ssv.substr(42,3).length(), 0);

  // Substring of a substring.
  ASSERT_EQ(ssv.substr(10, 10), "klmnopqrst"sv);
  ASSERT_EQ(ssv.substr(10, 10).substr(3,3), "nop"sv);

  // Verify that substring view lengths are respected.
  ASSERT_EQ(ssv.substr(10, 10).substr(3,30), "nopqrst"sv);
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

