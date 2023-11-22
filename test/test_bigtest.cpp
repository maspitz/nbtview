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
    CHECK(root_tag.is<nbt::Compound>());
    CHECK(root_name == "Level");

    SUBCASE("byteTest") {
        CHECK(root_tag.contains("byteTest"));
        CHECK(root_tag["byteTest"].get<nbt::Byte>() == 127);
    }
    SUBCASE("longTest") {
        CHECK(root_tag.contains("longTest"));
        CHECK(root_tag["longTest"].get<nbt::Long>() == 9223372036854775807L);
    }
    SUBCASE("shortTest") {
        CHECK(root_tag.contains("shortTest"));
        CHECK(root_tag["shortTest"].get<nbt::Short>() == 32767);
    }
    SUBCASE("stringTest") {
        CHECK(root_tag.contains("stringTest"));
        CHECK(root_tag["stringTest"].get<nbt::String>() ==
              "HELLO WORLD THIS IS A TEST STRING ÅÄÖ!");
    }
    SUBCASE("floatTest") {
        CHECK(root_tag.contains("floatTest"));
        CHECK(root_tag["floatTest"].get<nbt::Float>() ==
              doctest::Approx(0.49823147));
    }
    SUBCASE("intTest") {
        CHECK(root_tag.contains("intTest"));
        CHECK(root_tag["intTest"].get<nbt::Int>() == 2147483647);
    }
    SUBCASE("doubleTest") {
        CHECK(root_tag.contains("doubleTest"));
        CHECK(root_tag["doubleTest"].get<nbt::Double>() ==
              doctest::Approx(0.4931287132182315));
    }
    SUBCASE("nested compound test") {
        CHECK(root_tag.contains("nested compound test"));
        auto &nested = root_tag["nested compound test"];
        SUBCASE("egg") {
            auto &egg = nested["egg"];
            CHECK(egg["name"].get<nbt::String>() == "Eggbert");
            CHECK(egg["value"].get<nbt::Float>() == doctest::Approx(0.5));
        }
        SUBCASE("ham") {
            auto &ham = nested["ham"];
            CHECK(ham["name"].get<nbt::String>() == "Hampus");
            CHECK(ham["value"].get<nbt::Float>() == doctest::Approx(0.75));
        }
    }
    SUBCASE("list test (long)") {
        auto &list_long = root_tag["listTest (long)"].get<nbt::List>();
        CHECK(nbt::list_type(list_long) == nbt::TypeCode::Long);
        REQUIRE(list_long.size() == 5);
        auto longvals = std::vector<nbt::Long>{11, 12, 13, 14, 15};
        for (std::size_t i = 0; i < list_long.size(); ++i) {
            CHECK(list_long[i].get<nbt::Long>() == longvals[i]);
        }
    }
    SUBCASE("list test (compound)") {
        auto &list_cmpd = root_tag["listTest (compound)"].get<nbt::List>();
        CHECK(nbt::list_type(list_cmpd) == nbt::TypeCode::Compound);
        REQUIRE(list_cmpd.size() == 2);
        auto &cmpd0 = list_cmpd[0].get<nbt::Compound>();
        CHECK(cmpd0["created-on"].get<nbt::Long>() == 1264099775885L);
        CHECK(cmpd0["name"].get<nbt::String>() == "Compound tag #0");
        auto &cmpd1 = list_cmpd[1].get<nbt::Compound>();
        CHECK(cmpd1["created-on"].get<nbt::Long>() == 1264099775885L);
        CHECK(cmpd1["name"].get<nbt::String>() == "Compound tag #1");
    }
    SUBCASE("byteArrayTest") {
        auto byte_array_name = "byteArrayTest "
                               "(the first 1000 values of (n*n*255+n*7)%100, "
                               "starting with n=0 (0, 62, 34, 16, 8, ...))";

        CHECK(root_tag.contains(byte_array_name));
        auto &byte_array = root_tag[byte_array_name].get<nbt::Byte_Array>();
        REQUIRE(byte_array.size() == 1000);
        for (int n = 0; n < 1000; ++n) {
            CHECK(byte_array[n] == (n * n * 255 + n * 7) % 100);
        }
    }
}
