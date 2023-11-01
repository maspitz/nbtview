/**
 * @file nbtview.hpp
 * @brief Interface to operations on NBT structured data
 * @author Michael Spitznagel
 * @copyright Copyright 2023 Michael Spitznagel. Released under the Boost
 * Software License 1.0
 *
 * https://github.com/maspitz/nbtview
 */

#ifndef NBTVIEW_H_
#define NBTVIEW_H_

#include <iosfwd>
#include <string>
#include <utility>
#include <vector>

#include "Tag.hpp"

class List;
class Compound;

namespace nbtview {

// Attempts to find a named tag in a range of NBT data by searching
// for its initial byte sequence.
//
// If the tag is found, an iterator to the beginning of its payload
// is returned. Otherwise, nbt_stop is returned.
//
// Caveats:
// (1) This method can be fooled if the tag's name appears in the
// payload of some other tag.  This is unlikely to happen by chance.
// (2) It is the calling code's responsibility to check that the
// remaining NBT data is large enough to contain the tag's expected
// payload.

std::vector<unsigned char>::const_iterator
fast_find_named_tag(std::vector<unsigned char>::const_iterator nbt_start,
                    std::vector<unsigned char>::const_iterator nbt_stop,
                    TypeCode tag_type, const std::string &tag_name);

/** @name Input interface
 * @{
 * @brief Deserialize from binary encoded input
 * */
/**
 * @brief Deserializes from a stream.
 * @param input An istream opened with ios::binary.
 * @return A pair consisting of the decoded root tag's name and payload.
 *
 * @throw std::runtime_error if the input could not be decoded successfully.
 * */
std::pair<std::string, TagData> read_binary(std::istream &input);
/**
 * @brief Deserializes from a vector of bytes.
 * @param bytes A vector of unsigned char.
 * @return A pair consisting of the decoded root tag's name and payload.
 *
 * @throw std::runtime_error if the input could not be decoded successfully.
 * */
std::pair<std::string, TagData> read_binary(std::vector<unsigned char> bytes);

std::pair<std::string, TagData> read_binary(const unsigned char *data,
                                            size_t data_length);
/**
 * @}
 * */

/** @name Output interface
 * @{
 * */
/**
 * @brief Serializes a tag to a stream.
 * @param tag The tag to be serialized.
 * @param name The name specified for the tag.
 * @param output An ostream opened with ios::binary.
 *
 * @note The binary encoding of the tag is in the NBT format.
 *
 * @note Overloads of the write_binary() function for Compound and List
 * parameters are provided because Compound and List are move-only classes,
 * which can prevent their implicit conversion to const Tag.
 * */
void write_binary(const TagData &tag, std::string_view name,
                  std::ostream &output);
void write_binary(const Compound &tag, std::string_view name,
                  std::ostream &output);
void write_binary(const List &tag, std::string_view name, std::ostream &output);

/**
 * @brief Outputs a string representation of a tag to a stream.
 * @param tag The tag to be written as a string.
 * @param name The name specified for the tag.
 * @param output An ostream opened with ios::binary.
 *
 * @note The string representation of the tag structure is in the SNBT format.
 *
 * @note Overloads of the stream insertion operator for Compound and List
 * parameters are provided because Compound and List are move-only classes,
 * which can prevent their implicit conversion to const Tag.
 * */
std::ostream &operator<<(std::ostream &os, const TagData &tag);
std::ostream &operator<<(std::ostream &os, const Compound &tag);
std::ostream &operator<<(std::ostream &os, const List &tag);
/**
 * @}
 * */

} // namespace nbtview

#endif // NBTVIEW_H_
