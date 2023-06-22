#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <variant>
#include <vector>

#include "Compound.hpp"
#include "List.hpp"
#include "NbtReader.hpp"
#include "nbtview.hpp"

namespace nbt = nbtview;

TEST_CASE("nbtview: bigtest.nbt values") {
    const auto filename = "test_data/bigtest.nbt";
    auto root_tag = nbt::NbtReader::read_from_file(filename);

    SUBCASE("byteTest") {
        CHECK(root_tag.contains<nbt::Byte>("byteTest"));
        CHECK(root_tag.at<nbt::Byte>("byteTest") == 127);
    }
    SUBCASE("longTest") {
        CHECK(root_tag.contains<nbt::Long>("longTest"));
        CHECK(root_tag.at<nbt::Long>("longTest") == 9223372036854775807L);
    }
    SUBCASE("shortTest") {
        CHECK(root_tag.contains<nbt::Short>("shortTest"));
        CHECK(root_tag.at<nbt::Short>("shortTest") == 32767);
    }
    SUBCASE("stringTest") {
        CHECK(root_tag.contains<nbt::String>("stringTest"));
        CHECK(root_tag.at<nbt::String>("stringTest") ==
              "HELLO WORLD THIS IS A TEST STRING ÅÄÖ!");
    }
    SUBCASE("floatTest") {
        CHECK(root_tag.contains<nbt::Float>("floatTest"));
        CHECK(root_tag.at<nbt::Float>("floatTest") ==
              doctest::Approx(0.49823147));
    }
    SUBCASE("intTest") {
        CHECK(root_tag.contains<nbt::Int>("intTest"));
        CHECK(root_tag.at<nbt::Int>("intTest") == 2147483647);
    }
    SUBCASE("doubleTest") {
        CHECK(root_tag.contains<nbt::Double>("doubleTest"));
        CHECK(root_tag.at<nbt::Double>("doubleTest") ==
              doctest::Approx(0.4931287132182315));
    }
    SUBCASE("nested compound test") {
        CHECK(root_tag.contains<nbt::Compound>("nested compound test"));
        auto nested = root_tag.at<nbt::Compound>("nested compound test");
        SUBCASE("egg") {
            auto egg = nested.at<nbt::Compound>("egg");
            CHECK(egg.at<nbt::String>("name") == "Eggbert");
            CHECK(egg.at<nbt::Float>("value") == doctest::Approx(0.5));
        }
        SUBCASE("ham") {
            auto ham = nested.at<nbt::Compound>("ham");
            CHECK(ham.at<nbt::String>("name") == "Hampus");
            CHECK(ham.at<nbt::Float>("value") == doctest::Approx(0.75));
        }
    }
    SUBCASE("list test (long)") {
        auto list_long = root_tag.at<nbt::List>("listTest (long)");
        CHECK(list_long.list_type() == nbt::TypeCode::Long);
        REQUIRE(list_long.size() == 5);
        auto longvals = std::vector<nbt::Long>{11, 12, 13, 14, 15};
        for (std::size_t i = 0; i < list_long.size(); ++i) {
            CHECK(list_long.at<nbt::Long>(i) == longvals[i]);
        }
    }
    SUBCASE("list test (compound)") {
        auto list_cmpd = root_tag.at<nbt::List>("listTest (compound)");
        CHECK(list_cmpd.list_type() == nbt::TypeCode::Compound);
        REQUIRE(list_cmpd.size() == 2);
        auto cmpd0 = list_cmpd.at<nbt::Compound>(0);
        CHECK(cmpd0.at<nbt::Long>("created-on") == 1264099775885L);
        CHECK(cmpd0.at<nbt::String>("name") == "Compound tag #0");
        auto cmpd1 = list_cmpd.at<nbt::Compound>(1);
        CHECK(cmpd1.at<nbt::Long>("created-on") == 1264099775885L);
        CHECK(cmpd1.at<nbt::String>("name") == "Compound tag #1");
    }
    SUBCASE("byteArrayTest") {
        auto byte_array_name = "byteArrayTest "
                               "(the first 1000 values of (n*n*255+n*7)%100, "
                               "starting with n=0 (0, 62, 34, 16, 8, ...))";

        CHECK(root_tag.contains<nbt::Byte_Array>(byte_array_name));
        auto byte_array = root_tag.at<nbt::Byte_Array>(byte_array_name);
        REQUIRE(byte_array.size() == 1000);
        for (int n = 0; n < 1000; ++n) {
            CHECK(byte_array[n] == (n * n * 255 + n * 7) % 100);
        }
    }
}
