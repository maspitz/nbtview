#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "Compound.hpp"
#include "List.hpp"
#include "NbtReader.hpp"
#include "nbtview.hpp"

namespace nbt = nbtview;

TEST_CASE("nbtview: bigtest.nbt values") {
    const auto filename = "test_data/bigtest.nbt";
    auto root_tag = nbt::NbtReader::read_from_file(filename);

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
}
