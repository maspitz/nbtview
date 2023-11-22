#include <gtest/gtest.h>

#include <ios>
#include <sstream>
#include <vector>

#include "BinaryWriter.hpp"

namespace nbt = nbtview;

std::vector<char> stream_chars(std::ostringstream &stream) {
    auto outview = stream.view();
    return std::vector<char>(outview.begin(), outview.end());
}

TEST(BinaryWriter, Int8) {
    std::ostringstream output(std::ios::binary);
    int8_t value = 0x7a;
    nbt::BinaryWriter::write(value, output);
    EXPECT_EQ(stream_chars(output), std::vector<char>{0x7a});
    value = 0xff;
    nbt::BinaryWriter::write(value, output);
    value = 0x00;
    nbt::BinaryWriter::write(value, output);
    std::vector<char> expect_result{0x7a, static_cast<char>(0xff), 0x00};
    EXPECT_EQ(stream_chars(output), expect_result);
}

TEST(BinaryWriter, Int16) {
    std::ostringstream output(std::ios::binary);
    nbt::BinaryWriter::write(int16_t(0xcafe), output);
    std::vector<char> expect_result{static_cast<char>(0xca),
                                    static_cast<char>(0xfe)};
    EXPECT_EQ(stream_chars(output), expect_result);
}

TEST(BinaryWriter, Int32) {
    std::ostringstream output(std::ios::binary);
    nbt::BinaryWriter::write(int32_t(0x12cafe89), output);

    std::vector<char> expect_result{0x12, static_cast<char>(0xca),
                                    static_cast<char>(0xfe),
                                    static_cast<char>(0x89)};
    EXPECT_EQ(stream_chars(output), expect_result);
}

TEST(BinaryWriter, Int64) {
    std::ostringstream output(std::ios::binary);
    nbt::BinaryWriter::write(int64_t(0x0123456789abcdef), output);

    std::vector<char> expect_result{0x01,
                                    0x23,
                                    0x45,
                                    0x67,
                                    static_cast<char>(0x89),
                                    static_cast<char>(0xab),
                                    static_cast<char>(0xcd),
                                    static_cast<char>(0xef)};
    EXPECT_EQ(stream_chars(output), expect_result);
}

TEST(BinaryWriter, FloatingPointRepresentation) {
    std::ostringstream output(std::ios::binary);
    EXPECT_EQ(sizeof(float), 4);
    EXPECT_EQ(sizeof(double), 8);
    EXPECT_EQ(std::numeric_limits<float>::is_iec559, true);
    EXPECT_EQ(std::numeric_limits<float>::digits, 24);
    EXPECT_EQ(std::numeric_limits<double>::is_iec559, true);
    EXPECT_EQ(std::numeric_limits<double>::digits, 53);
}

TEST(BinaryWriter, Float) {
    std::ostringstream output(std::ios::binary);
    nbt::BinaryWriter::write(-248.75f, output);

    std::vector<char> expect_result{static_cast<char>(0xc3), 0x78,
                                    static_cast<char>(0xc0), 0x00};
    EXPECT_EQ(stream_chars(output), expect_result);
}

TEST(BinaryWriter, Double) {
    std::ostringstream output(std::ios::binary);
    nbt::BinaryWriter::write(double(0.2), output);

    std::vector<char> expect_result{0x3f,
                                    static_cast<char>(0xc9),
                                    static_cast<char>(0x99),
                                    static_cast<char>(0x99),
                                    static_cast<char>(0x99),
                                    static_cast<char>(0x99),
                                    static_cast<char>(0x99),
                                    static_cast<char>(0x9a)};
    EXPECT_EQ(stream_chars(output), expect_result);
}

TEST(BinaryWriter, String) {
    std::ostringstream output(std::ios::binary);
    nbt::BinaryWriter::write_string("foo", output);

    std::vector<char> expect_result1{0x00, 0x03, 'f', 'o', 'o'};
    EXPECT_EQ(stream_chars(output), expect_result1);
    nbt::BinaryWriter::write_string("bar", output);

    std::vector<char> expect_result2{0x00, 0x03, 'f', 'o', 'o',
                                     0x00, 0x03, 'b', 'a', 'r'};
    EXPECT_EQ(stream_chars(output), expect_result2);
}

TEST(BinaryWriter, VectorInt8) {
    std::ostringstream output(std::ios::binary);
    const std::vector<int8_t> values = {0x7b, 0x2d, 0x00, 0x7f};
    nbt::BinaryWriter::write_vector(values, output);
    std::vector<char> expect_result{0x00, 0x00, 0x00, 0x04,
                                    0x7b, 0x2d, 0x00, 0x7f};
    EXPECT_EQ(stream_chars(output), expect_result);
}

TEST(BinaryWriter, VectorInt16) {
    std::ostringstream output(std::ios::binary);
    const std::vector<int16_t> values = {0x12, 0x789a, 0x00, 0x5555};
    nbt::BinaryWriter::write_vector(values, output);

    std::vector<char> expect_result{0x00, 0x00, 0x00, 0x04,
                                    0x00, 0x12, 0x78, static_cast<char>(0x9a),
                                    0x00, 0x00, 0x55, 0x55};
    EXPECT_EQ(stream_chars(output), expect_result);
}

TEST(BinaryWriter, VectorInt32) {
    std::ostringstream output(std::ios::binary);
    const std::vector<int32_t> values = {0x01, 0x789abcde};
    nbt::BinaryWriter::write_vector(values, output);

    std::vector<char> expect_result{0x00,
                                    0x00,
                                    0x00,
                                    0x02,
                                    0x00,
                                    0x00,
                                    0x00,
                                    0x01,
                                    0x78,
                                    static_cast<char>(0x9a),
                                    static_cast<char>(0xbc),
                                    static_cast<char>(0xde)};
    EXPECT_EQ(stream_chars(output), expect_result);
}

TEST(BinaryWriter, VectorInt64) {
    std::ostringstream output(std::ios::binary);
    const std::vector<int64_t> values = {0x1234, 0x0123456789abcdef};
    nbt::BinaryWriter::write_vector(values, output);
    std::vector<char> correct_output{0x00,
                                     0x00,
                                     0x00,
                                     0x02,
                                     0x00,
                                     0x00,
                                     0x00,
                                     0x00,
                                     0x00,
                                     0x00,
                                     0x12,
                                     0x34,
                                     0x01,
                                     0x23,
                                     0x45,
                                     0x67,
                                     static_cast<char>(0x89),
                                     static_cast<char>(0xab),
                                     static_cast<char>(0xcd),
                                     static_cast<char>(0xef)};
    EXPECT_EQ(stream_chars(output), correct_output);
}
