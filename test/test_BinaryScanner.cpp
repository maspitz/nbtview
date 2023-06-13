#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "BinaryScanner.hpp"
#include "doctest.h"
#include <vector>

TEST_CASE("nbtview::detail::BinaryScanner Integer Types") {
    auto v = std::vector<unsigned char>{0x17, 0x23, 0x01, 0xff,
                                        0x01, 0x23, 0x45, 0x67};
    auto s = nbtview::BinaryScanner(v);
    auto a = s.get_value<int8_t>();
    CHECK(a == 0x17);
    auto b = s.get_value<int8_t>();
    CHECK(b == 0x23);
    auto c = s.get_value<int16_t>();
    CHECK(c == 0x01ff);
    auto d = s.get_value<int32_t>();
    CHECK(d == 0x01234567);
    CHECK_THROWS_AS(s.get_value<int8_t>(), std::runtime_error);
}

TEST_CASE("nbtview::detail::BinaryScanner Floating Point Types") {
    auto v32 = std::vector<unsigned char>{0xc3, 0x78, 0xc0, 0x00};
    auto s32 = nbtview::BinaryScanner(v32);
    auto f32 = s32.get_value<float>();

    REQUIRE(f32 == -248.75);

    auto v64 = std::vector<unsigned char>{0x3f, 0xc9, 0x99, 0x99,
                                          0x99, 0x99, 0x99, 0x9a};
    auto s64 = nbtview::BinaryScanner(v64);
    auto f64 = s64.get_value<double>();

    REQUIRE(f64 == 0.2);
}

TEST_CASE("nbtview::BinaryScanner String") {
    // Test a valid string
    std::vector<unsigned char> data1 = {0x00, 0x05, 'H', 'e', 'l', 'l', 'o'};
    nbtview::BinaryScanner scanner1(data1);
    std::string str1 = scanner1.get_string();
    CHECK(str1 == "Hello");

    // Test an empty string
    std::vector<unsigned char> data2 = {0x00, 0x00};
    nbtview::BinaryScanner scanner2(data2);
    std::string str2 = scanner2.get_string();
    CHECK(str2 == "");

    // Test a string with unexpected end of input
    std::vector<unsigned char> data3 = {0x00, 0x05, 'W', 'o', 'r'};
    nbtview::BinaryScanner scanner3(data3);
    CHECK_THROWS_AS(scanner3.get_string(),
                    nbtview::UnexpectedEndOfInputException);
}

TEST_CASE("nbtview::BinaryScanner Vector") {
    // Test a valid vector of integers
    std::vector<unsigned char> data1 = {0x00, 0x00, 0x00, 0x03,
                                        0x01, 0x02, 0x03};
    nbtview::BinaryScanner scanner1(data1);
    auto vec1 = scanner1.get_vector<int8_t>();
    REQUIRE(vec1 != nullptr);
    CHECK(vec1->size() == 3);
    CHECK((*vec1)[0] == 0x01);
    CHECK((*vec1)[1] == 0x02);
    CHECK((*vec1)[2] == 0x03);

    std::vector<unsigned char> data2 = {0x00, 0x00, 0x00, 0x02, 0x01, 0x02,
                                        0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    nbtview::BinaryScanner scanner2(data2);
    auto vec2 = scanner2.get_vector<int32_t>();
    REQUIRE(vec2 != nullptr);
    CHECK(vec2->size() == 2);
    CHECK((*vec2)[0] == 0x01020304);
    CHECK((*vec2)[1] == 0x05060708);

    // Test a vector with negative array length
    std::vector<unsigned char> data3 = {0x00, 0x00, 0x00, 0xFF};
    nbtview::BinaryScanner scanner3(data3);
    CHECK_THROWS_AS(scanner3.get_vector<int32_t>(), std::runtime_error);

    // Test a vector with unexpected end of input
    std::vector<unsigned char> data4 = {0x00, 0x00, 0x00, 0x02,
                                        0x01, 0x02, 0x03};
    nbtview::BinaryScanner scanner4(data4);
    CHECK_THROWS_AS(scanner4.get_vector<int32_t>(),
                    nbtview::UnexpectedEndOfInputException);
}
