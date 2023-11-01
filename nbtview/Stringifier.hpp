/**
 * @file stringifier.hpp
 * @brief Convert tag data to human-readable form
 * @author Michael Spitznagel
 * @copyright Copyright 2023 Michael Spitznagel. Released under the Boost
 * Software License 1.0
 *
 * https://github.com/maspitz/nbtview
 */

#ifndef STRINGIFIER_H
#define STRINGIFIER_H

#include <string>
#include <vector>

#include "Tag.hpp"

class List;
class Compound;

namespace nbtview {

std::string tag_to_string(const TagData &tag);
std::string tag_to_string(const List &tag);
std::string tag_to_string(const Compound &tag);

std::string tag_type_to_string(const TagData &tag);
std::string tag_type_to_string(const List &tag);
std::string tag_type_to_string(const Compound &tag);

std::string tag_code_to_string(TypeCode type);
} // namespace nbtview

#endif // STRINGIFIER_H
