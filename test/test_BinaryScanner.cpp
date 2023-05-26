#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "BinaryScanner.hpp"
#include "doctest.h"

TEST_CASE("nbtview::detail::BinaryScanner Integer Types") {
    auto v =
        std::vector<uint8_t>{0x17, 0x23, 0x01, 0xff, 0x01, 0x23, 0x45, 0x67};
    auto s = nbtview::BinaryScanner(v);
    auto a = s.get_value<int8_t>();
    auto b = s.get_value<int8_t>();
    auto c = s.get_value<int16_t>();
    auto d = s.get_value<int32_t>();
    CHECK_THROWS_AS(s.get_value<int8_t>(), std::runtime_error);

    REQUIRE(a == 0x17);
    REQUIRE(b == 0x23);
    REQUIRE(c == 0x01ff);
    REQUIRE(d == 0x01234567);
}

TEST_CASE("nbtview::detail::BinaryScanner Floating Point Types") {
    auto v32 = std::vector<uint8_t>{0xc3, 0x78, 0xc0, 0x00};
    auto s32 = nbtview::BinaryScanner(v32);
    auto f32 = s32.get_value<float>();

    REQUIRE(f32 == -248.75);

    auto v64 =
        std::vector<uint8_t>{0x3f, 0xc9, 0x99, 0x99, 0x99, 0x99, 0x99, 0x9a};
    auto s64 = nbtview::BinaryScanner(v64);
    auto f64 = s64.get_value<double>();

    REQUIRE(f64 == 0.2);
}

TEST_CASE("nbtview::BinaryScanner String") {
    // Test a valid string
    std::vector<uint8_t> data1 = {0x00, 0x05, 'H', 'e', 'l', 'l', 'o'};
    nbtview::BinaryScanner scanner1(data1);
    std::string str1 = scanner1.get_string();
    CHECK(str1 == "Hello");

    // Test an empty string
    std::vector<uint8_t> data2 = {0x00, 0x00};
    nbtview::BinaryScanner scanner2(data2);
    std::string str2 = scanner2.get_string();
    CHECK(str2 == "");

    // Test a string with unexpected end of input
    std::vector<uint8_t> data3 = {0x00, 0x05, 'W', 'o', 'r'};
    nbtview::BinaryScanner scanner3(data3);
    CHECK_THROWS_AS(scanner3.get_string(),
                    nbtview::UnexpectedEndOfInputException);
}

