#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <ios>
#include <iterator>
#include <sstream>
#include <vector>

#include "BinaryWriter.hpp"

namespace nbt = nbtview;

namespace doctest {

template <> struct StringMaker<std::vector<unsigned char>> {
    static String convert(const std::vector<unsigned char> &in) {
        std::ostringstream oss;

        oss << "[";
        oss << std::hex << std::setfill('0');
        for (typename std::vector<unsigned char>::const_iterator it =
                 in.begin();
             it != in.end(); ++it)
            oss << "0x" << std::setw(2) << static_cast<unsigned int>(*it)
                << ", ";
        oss << "]";

        return oss.str().c_str();
    }
};
} // namespace doctest

TEST_CASE("nbtview::BinaryWriter functions") {
    std::vector<unsigned char> output;
    auto inserter = std::back_inserter(output);
    nbt::BinaryWriter bw(inserter);

    SUBCASE("nbtview::BinaryWriter::write()") {

        SUBCASE("Test write with int8_t") {
            int8_t value = 0x7a;
            bw.write(value);
            CHECK(output == std::vector<unsigned char>{0x7a});
            value = 0xff;
            bw.write(value);
            value = 0x00;
            bw.write(value);
            CHECK(output == std::vector<unsigned char>{0x7a, 0xff, 0x00});
        }

        SUBCASE("Test write with int16_t") {
            bw.write(int16_t(0xcafe));
            CHECK(output == std::vector<unsigned char>{0xca, 0xfe});
        }

        SUBCASE("Test write with int32_t") {
            bw.write(int32_t(0x12cafe89));
            CHECK(output == std::vector<unsigned char>{0x12, 0xca, 0xfe, 0x89});
        }

        SUBCASE("Test write with int64_t") {
            bw.write(int64_t(0x0123456789abcdef));
            CHECK(output == std::vector<unsigned char>{0x01, 0x23, 0x45, 0x67,
                                                       0x89, 0xab, 0xcd, 0xef});
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
            bw.write(float(-248.75));
            CHECK(output == std::vector<unsigned char>{0xc3, 0x78, 0xc0, 0x00});
        }

        SUBCASE("Test write with double") {
            bw.write(double(0.2));
            CHECK(output == std::vector<unsigned char>{0x3f, 0xc9, 0x99, 0x99,
                                                       0x99, 0x99, 0x99, 0x9a});
        }
    }

    SUBCASE("nbtview::BinaryWriter::write_string()") {

        SUBCASE("Test write_string(\"foo\"), write_string(\"bar\")") {
            bw.write_string("foo");
            CHECK(output ==
                  std::vector<unsigned char>{0x00, 0x03, 'f', 'o', 'o'});
            bw.write_string("bar");
            CHECK(output == std::vector<unsigned char>{0x00, 0x03, 'f', 'o',
                                                       'o', 0x00, 0x03, 'b',
                                                       'a', 'r'});
        }
    }

    SUBCASE("nbtview::BinaryWriter::write_vector()") {

        SUBCASE("Test write_vector with vector<int8_t>") {
            std::vector<int8_t> values = {0x7b, 0x2d, 0x00, 0x7f};
            bw.write_vector(values);
            CHECK(output == std::vector<unsigned char>{0x7b, 0x2d, 0x00, 0x7f});
        }

        SUBCASE("Test write_vector with vector<int16_t>") {
            std::vector<int16_t> values = {0x12, 0x789a, 0x00, 0x5555};
            bw.write_vector(values);
            CHECK(output == std::vector<unsigned char>{0x00, 0x12, 0x78, 0x9a,
                                                       0x00, 0x00, 0x55, 0x55});
        }

        SUBCASE("Test write_vector with vector<int32_t>") {
            std::vector<int32_t> values = {0x01, 0x789abcde};
            bw.write_vector(values);
            CHECK(output == std::vector<unsigned char>{0x00, 0x00, 0x00, 0x01,
                                                       0x78, 0x9a, 0xbc, 0xde});
        }

        SUBCASE("Test write_vector with vector<int64_t>") {
            std::vector<int64_t> values = {0x1234, 0x0123456789abcdef};
            bw.write_vector(values);
            CHECK(output == std::vector<unsigned char>{0x00, 0x00, 0x00, 0x00,
                                                       0x00, 0x00, 0x12, 0x34,
                                                       0x01, 0x23, 0x45, 0x67,
                                                       0x89, 0xab, 0xcd, 0xef});
        }
    }
}
