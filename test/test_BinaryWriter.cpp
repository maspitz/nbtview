#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <ios>
#include <sstream>
#include <string_view>
#include <vector>

#include "BinaryWriter.hpp"

namespace nbt = nbtview;

namespace doctest {

template <> struct StringMaker<std::vector<char>> {
    static String convert(const std::vector<char> &in) {
        std::ostringstream oss;

        oss << "[";
        oss << std::hex << std::setfill('0');
        for (typename std::vector<char>::const_iterator it = in.begin();
             it != in.end(); ++it)
            oss << "0x" << std::setw(2) << static_cast<unsigned int>(*it)
                << ", ";
        oss << "]";

        return oss.str().c_str();
    }
};
} // namespace doctest

std::vector<char> stream_chars(std::ostringstream &stream) {
    auto outview = stream.view();
    return std::vector<char>(outview.begin(), outview.end());
}

TEST_CASE("nbtview::BinaryWriter functions") {
    std::ostringstream output(std::ios::binary);

    SUBCASE("nbtview::BinaryWriter::write()") {

        SUBCASE("Test write with int8_t") {
            int8_t value = 0x7a;
            nbt::BinaryWriter::write(value, output);
            CHECK(stream_chars(output) == std::vector<char>{0x7a});
            value = 0xff;
            nbt::BinaryWriter::write(value, output);
            value = 0x00;
            nbt::BinaryWriter::write(value, output);
            CHECK(stream_chars(output) ==
                  std::vector<char>{0x7a, static_cast<char>(0xff), 0x00});
        }

        SUBCASE("Test write with int16_t") {
            nbt::BinaryWriter::write(int16_t(0xcafe), output);
            CHECK(stream_chars(output) ==
                  std::vector<char>{static_cast<char>(0xca),
                                    static_cast<char>(0xfe)});
        }

        SUBCASE("Test write with int32_t") {
            nbt::BinaryWriter::write(int32_t(0x12cafe89), output);
            CHECK(stream_chars(output) ==
                  std::vector<char>{0x12, static_cast<char>(0xca),
                                    static_cast<char>(0xfe),
                                    static_cast<char>(0x89)});
        }

        SUBCASE("Test write with int64_t") {
            nbt::BinaryWriter::write(int64_t(0x0123456789abcdef), output);
            CHECK(stream_chars(output) ==
                  std::vector<char>{
                      0x01, 0x23, 0x45, 0x67, static_cast<char>(0x89),
                      static_cast<char>(0xab), static_cast<char>(0xcd),
                      static_cast<char>(0xef)});
        }

        SUBCASE("Test floating point representation") {
            REQUIRE(sizeof(float) == 4);
            REQUIRE(sizeof(double) == 8);
            REQUIRE(std::numeric_limits<float>::is_iec559 == true);
            REQUIRE(std::numeric_limits<float>::digits == 24);
            REQUIRE(std::numeric_limits<double>::is_iec559 == true);
            REQUIRE(std::numeric_limits<double>::digits == 53);
        }

        SUBCASE("Test write with float") {
            nbt::BinaryWriter::write(-248.75f, output);
            CHECK(stream_chars(output) ==
                  std::vector<char>{static_cast<char>(0xc3), 0x78,
                                    static_cast<char>(0xc0), 0x00});
        }

        SUBCASE("Test write with double") {
            nbt::BinaryWriter::write(double(0.2), output);
            CHECK(stream_chars(output) ==
                  std::vector<char>{
                      0x3f, static_cast<char>(0xc9), static_cast<char>(0x99),
                      static_cast<char>(0x99), static_cast<char>(0x99),
                      static_cast<char>(0x99), static_cast<char>(0x99),
                      static_cast<char>(0x9a)});
        }
    }

    SUBCASE("nbtview::BinaryWriter::write_string()") {

        SUBCASE("Test write_string(\"foo\"), write_string(\"bar\")") {
            nbt::BinaryWriter::write_string("foo", output);
            CHECK(stream_chars(output) ==
                  std::vector<char>{0x00, 0x03, 'f', 'o', 'o'});
            nbt::BinaryWriter::write_string("bar", output);
            CHECK(stream_chars(output) ==
                  std::vector<char>{0x00, 0x03, 'f', 'o', 'o', 0x00, 0x03, 'b',
                                    'a', 'r'});
        }
    }

    SUBCASE("nbtview::BinaryWriter::write_vector()") {

        SUBCASE("Test write_vector with vector<int8_t>") {
            const std::vector<int8_t> values = {0x7b, 0x2d, 0x00, 0x7f};
            nbt::BinaryWriter::write_vector(values, output);
            CHECK(stream_chars(output) == std::vector<char>{0x00, 0x00, 0x00,
                                                            0x04, 0x7b, 0x2d,
                                                            0x00, 0x7f});
        }

        SUBCASE("Test write_vector with vector<int16_t>") {
            const std::vector<int16_t> values = {0x12, 0x789a, 0x00, 0x5555};
            nbt::BinaryWriter::write_vector(values, output);
            CHECK(stream_chars(output) ==
                  std::vector<char>{0x00, 0x00, 0x00, 0x04, 0x00, 0x12, 0x78,
                                    static_cast<char>(0x9a), 0x00, 0x00, 0x55,
                                    0x55});
        }

        SUBCASE("Test write_vector with vector<int32_t>") {
            const std::vector<int32_t> values = {0x01, 0x789abcde};
            nbt::BinaryWriter::write_vector(values, output);
            CHECK(stream_chars(output) ==
                  std::vector<char>{0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
                                    0x01, 0x78, static_cast<char>(0x9a),
                                    static_cast<char>(0xbc),
                                    static_cast<char>(0xde)});
        }

        SUBCASE("Test write_vector with vector<int64_t>") {
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
            CHECK(stream_chars(output) == correct_output);
        }
    }
}
