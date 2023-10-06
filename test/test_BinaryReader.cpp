#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "BinaryReader.hpp"
#include "doctest.h"
#include <vector>

TEST_CASE("nbtview::detail::BinaryReader Integer Types") {
    const unsigned char v[] = {0x17, 0x23, 0x01, 0xff, 0x01, 0x23, 0x45, 0x67};
    auto s = nbtview::BinaryReader(v, sizeof(v));
    auto a = s.read<int8_t>();
    CHECK(a == 0x17);
    auto b = s.read<int8_t>();
    CHECK(b == 0x23);
    auto c = s.read<int16_t>();
    CHECK(c == 0x01ff);
    auto d = s.read<int32_t>();
    CHECK(d == 0x01234567);
    CHECK_THROWS_AS(auto _ = s.read<int8_t>(), std::runtime_error);
}

TEST_CASE("nbtview::detail::BinaryReader Floating Point Types") {
    const unsigned char v32[] = {0xc3, 0x78, 0xc0, 0x00};
    auto s32 = nbtview::BinaryReader(v32, sizeof(v32));
    auto f32 = s32.read<float>();

    REQUIRE(f32 == -248.75);

    auto v64 = std::vector<unsigned char>{0x3f, 0xc9, 0x99, 0x99,
                                          0x99, 0x99, 0x99, 0x9a};
    auto s64 = nbtview::BinaryReader(v64.data(), v64.size());
    auto f64 = s64.read<double>();

    REQUIRE(f64 == 0.2);
}

TEST_CASE("nbtview::BinaryReader String") {
    // Test a valid string
    const unsigned char data1[] = {'H', 'e', 'l', 'l', 'o'};
    nbtview::BinaryReader scanner1(data1, sizeof(data1));
    std::string str1 = scanner1.read_string(sizeof(data1));
    CHECK(str1 == "Hello");

    // Test an empty string
    std::vector<unsigned char> data2 = {};
    nbtview::BinaryReader scanner2(data2.data(), data2.size());
    std::string str2 = scanner2.read_string(0);
    CHECK(str2 == "");

    // Test a string with unexpected end of input
    const std::vector<unsigned char> data3 = {'W', 'o', 'r'};
    nbtview::BinaryReader scanner3(data3.data(), data3.size());
    CHECK_THROWS_AS(auto _ = scanner3.read_string(5),
                    nbtview::UnexpectedEndOfInputException);
}

TEST_CASE("nbtview::BinaryReader Vector") {
    // Test a valid vector of integers
    const unsigned char data1[] = {0x01, 0x02, 0x03};
    nbtview::BinaryReader scanner1(data1, sizeof(data1));
    auto vec1 =
        scanner1.read_array<int8_t>(sizeof(data1) / sizeof(unsigned char));
    CHECK(vec1.size() == 3);
    CHECK(vec1[0] == 0x01);
    CHECK(vec1[1] == 0x02);
    CHECK(vec1[2] == 0x03);

    std::vector<unsigned char> data2 = {0x01, 0x02, 0x03, 0x04,
                                        0x05, 0x06, 0x07, 0x08};
    nbtview::BinaryReader scanner2(data2.data(), data2.size());
    auto vec2 = scanner2.read_array<int32_t>(data2.size() / sizeof(int32_t));
    CHECK(vec2.size() == 2);
    CHECK(vec2[0] == 0x01020304);
    CHECK(vec2[1] == 0x05060708);

    // Test a vector with negative array length
    std::vector<unsigned char> data3 = {0x00, 0x00, 0x00, 0xFF};
    nbtview::BinaryReader scanner3(data3.data(), data3.size());
    CHECK_THROWS_AS(auto _ = scanner3.read_array<int32_t>(-1),
                    std::runtime_error);

    // Test a vector with unexpected end of input
    std::vector<unsigned char> data4 = {0x01, 0x02, 0x03};
    nbtview::BinaryReader scanner4(data4.data(), data4.size());
    CHECK_THROWS_AS(auto _ = scanner4.read_array<int32_t>(2),
                    nbtview::UnexpectedEndOfInputException);
}
