#include <gtest/gtest.h>

#include <vector>

#include "BinaryReader.hpp"

TEST(BinaryReader, IntegerTypes) {
    const unsigned char v[] = {0x17, 0x23, 0x01, 0xff, 0x01, 0x23, 0x45, 0x67};
    auto s = nbtview::BinaryReader(v, sizeof(v));
    auto a = s.read<int8_t>();
    EXPECT_EQ(a, 0x17);
    auto b = s.read<int8_t>();
    EXPECT_EQ(b, 0x23);
    auto c = s.read<int16_t>();
    EXPECT_EQ(c, 0x01ff);
    auto d = s.read<int32_t>();
    EXPECT_EQ(d, 0x01234567);
    EXPECT_THROW(auto _ = s.read<int8_t>(), std::runtime_error);
}

TEST(BinaryReader, FloatingPointTypes) {
    const unsigned char v32[] = {0xc3, 0x78, 0xc0, 0x00};
    auto s32 = nbtview::BinaryReader(v32, sizeof(v32));
    auto f32 = s32.read<float>();

    EXPECT_EQ(f32, -248.75);

    auto v64 = std::vector<unsigned char>{0x3f, 0xc9, 0x99, 0x99,
                                          0x99, 0x99, 0x99, 0x9a};
    auto s64 = nbtview::BinaryReader(v64.data(), v64.size());
    auto f64 = s64.read<double>();

    EXPECT_EQ(f64, 0.2);
}

TEST(BinaryReader, String) {
    const unsigned char data1[] = {'H', 'e', 'l', 'l', 'o'};
    nbtview::BinaryReader scanner1(data1, sizeof(data1));
    std::string str1 = scanner1.read_string(sizeof(data1));
    EXPECT_EQ(str1, "Hello");

    // Test an empty string
    std::vector<unsigned char> data2 = {};
    nbtview::BinaryReader scanner2(data2.data(), data2.size());
    std::string str2 = scanner2.read_string(0);
    EXPECT_EQ(str2, "");

    // Test a string with unexpected end of input
    const std::vector<unsigned char> data3 = {'W', 'o', 'r'};
    nbtview::BinaryReader scanner3(data3.data(), data3.size());
    EXPECT_THROW(auto _ = scanner3.read_string(5),
                 nbtview::UnexpectedEndOfInputException);
}

TEST(BinaryReader, Vector) {
    // Test a valid vector of integers
    const unsigned char data1[] = {0x01, 0x02, 0x03};
    nbtview::BinaryReader scanner1(data1, sizeof(data1));
    auto vec1 =
        scanner1.read_array<int8_t>(sizeof(data1) / sizeof(unsigned char));
    EXPECT_EQ(vec1.size(), 3);
    EXPECT_EQ(vec1[0], 0x01);
    EXPECT_EQ(vec1[1], 0x02);
    EXPECT_EQ(vec1[2], 0x03);

    std::vector<unsigned char> data2 = {0x01, 0x02, 0x03, 0x04,
                                        0x05, 0x06, 0x07, 0x08};
    nbtview::BinaryReader scanner2(data2.data(), data2.size());
    auto vec2 = scanner2.read_array<int32_t>(data2.size() / sizeof(int32_t));
    EXPECT_EQ(vec2.size(), 2);
    EXPECT_EQ(vec2[0], 0x01020304);
    EXPECT_EQ(vec2[1], 0x05060708);

    // Test a vector with negative array length
    std::vector<unsigned char> data3 = {0x00, 0x00, 0x00, 0xFF};
    nbtview::BinaryReader scanner3(data3.data(), data3.size());
    EXPECT_THROW(auto _ = scanner3.read_array<int32_t>(-1), std::runtime_error);

    // Test a vector with unexpected end of input
    std::vector<unsigned char> data4 = {0x01, 0x02, 0x03};
    nbtview::BinaryReader scanner4(data4.data(), data4.size());
    EXPECT_THROW(auto _ = scanner4.read_array<int32_t>(2),
                 nbtview::UnexpectedEndOfInputException);
}
