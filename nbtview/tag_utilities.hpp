// tag_utilities.hpp

#ifndef TAG_UTILITIES_H
#define TAG_UTILITIES_H

#include <string>
#include <vector>

#include "Tag.hpp"

class List;

namespace nbtview {

std::string tag_to_string(const Tag &tag);
std::string tag_to_string(const List &tag);
std::string tag_to_string(const Compound &tag);

std::string tag_type_to_string(const Tag &tag);
std::string tag_type_to_string(const List &tag);
std::string tag_type_to_string(const Compound &tag);

std::string tag_code_to_string(TypeCode type);
} // namespace nbtview

#endif // TAG_UTILITIES_H
