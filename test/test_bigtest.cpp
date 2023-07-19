#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <fstream>
#include <variant>
#include <vector>

#include "Tag.hpp"
#include "nbtview.hpp"

namespace nbt = nbtview;

TEST_CASE("nbtview: bigtest.nbt values") {
    const auto filename = "test_data/bigtest.nbt";
    std::ifstream bigtest_stream(filename);
    auto [root_name, root_tag] = nbt::read_binary(bigtest_stream);
    auto &root_compound = std::get<nbt::Compound>(root_tag);
    CHECK(root_name == "Level");

    SUBCASE("byteTest") {
        CHECK(root_compound.contains<nbt::Byte>("byteTest"));
        CHECK(root_compound.get<nbt::Byte>("byteTest") == 127);
    }
    SUBCASE("longTest") {
        CHECK(root_compound.contains<nbt::Long>("longTest"));
        CHECK(root_compound.get<nbt::Long>("longTest") == 9223372036854775807L);
    }
    SUBCASE("shortTest") {
        CHECK(root_compound.contains<nbt::Short>("shortTest"));
        CHECK(root_compound.get<nbt::Short>("shortTest") == 32767);
    }
    SUBCASE("stringTest") {
        CHECK(root_compound.contains<nbt::String>("stringTest"));
        CHECK(root_compound.get<nbt::String>("stringTest") ==
              "HELLO WORLD THIS IS A TEST STRING ÅÄÖ!");
    }
    SUBCASE("floatTest") {
        CHECK(root_compound.contains<nbt::Float>("floatTest"));
        CHECK(root_compound.get<nbt::Float>("floatTest") ==
              doctest::Approx(0.49823147));
    }
    SUBCASE("intTest") {
        CHECK(root_compound.contains<nbt::Int>("intTest"));
        CHECK(root_compound.get<nbt::Int>("intTest") == 2147483647);
    }
    SUBCASE("doubleTest") {
        CHECK(root_compound.contains<nbt::Double>("doubleTest"));
        CHECK(root_compound.get<nbt::Double>("doubleTest") ==
              doctest::Approx(0.4931287132182315));
    }
    SUBCASE("nested compound test") {
        CHECK(root_compound.contains<nbt::Compound>("nested compound test"));
        auto &nested = root_compound.get<nbt::Compound>("nested compound test");
        SUBCASE("egg") {
            auto &egg = nested.get<nbt::Compound>("egg");
            CHECK(egg.get<nbt::String>("name") == "Eggbert");
            CHECK(egg.get<nbt::Float>("value") == doctest::Approx(0.5));
        }
        SUBCASE("ham") {
            auto &ham = nested.get<nbt::Compound>("ham");
            CHECK(ham.get<nbt::String>("name") == "Hampus");
            CHECK(ham.get<nbt::Float>("value") == doctest::Approx(0.75));
        }
    }
    SUBCASE("list test (long)") {
        auto &list_long = root_compound.get<nbt::List>("listTest (long)");
        CHECK(list_long.list_type() == nbt::TypeCode::Long);
        REQUIRE(list_long.size() == 5);
        auto longvals = std::vector<nbt::Long>{11, 12, 13, 14, 15};
        for (std::size_t i = 0; i < list_long.size(); ++i) {
            CHECK(list_long.get<nbt::Long>(i) == longvals[i]);
        }
    }
    SUBCASE("list test (compound)") {
        auto &list_cmpd = root_compound.get<nbt::List>("listTest (compound)");
        CHECK(list_cmpd.list_type() == nbt::TypeCode::Compound);
        REQUIRE(list_cmpd.size() == 2);
        auto &cmpd0 = list_cmpd.get<nbt::Compound>(0);
        CHECK(cmpd0.get<nbt::Long>("created-on") == 1264099775885L);
        CHECK(cmpd0.get<nbt::String>("name") == "Compound tag #0");
        auto &cmpd1 = list_cmpd.get<nbt::Compound>(1);
        CHECK(cmpd1.get<nbt::Long>("created-on") == 1264099775885L);
        CHECK(cmpd1.get<nbt::String>("name") == "Compound tag #1");
    }
    SUBCASE("byteArrayTest") {
        auto byte_array_name = "byteArrayTest "
                               "(the first 1000 values of (n*n*255+n*7)%100, "
                               "starting with n=0 (0, 62, 34, 16, 8, ...))";

        CHECK(root_compound.contains<nbt::Byte_Array>(byte_array_name));
        auto &byte_array = root_compound.get<nbt::Byte_Array>(byte_array_name);
        REQUIRE(byte_array.size() == 1000);
        for (int n = 0; n < 1000; ++n) {
            CHECK(byte_array[n] == (n * n * 255 + n * 7) % 100);
        }
    }
}
