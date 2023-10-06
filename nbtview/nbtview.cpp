#include <algorithm>
#include <istream>
#include <span>
#include <string>
#include <utility>
#include <vector>

#include "zlib_utils.hpp"

#include "BinaryDeserializer.hpp"
#include "Serializer.hpp"
#include "Stringifier.hpp"
#include "Tag.hpp"
#include "nbtview.hpp"

namespace nbtview {

std::vector<unsigned char>::const_iterator
fast_find_named_tag(std::vector<unsigned char>::const_iterator nbt_start,
                    std::vector<unsigned char>::const_iterator nbt_stop,
                    TypeCode tag_type, const std::string &tag_name) {

    auto loc = nbt_start;
    const char byte_1 = static_cast<char>(tag_type);
    const char byte_2 = tag_name.length() / 16;
    const char byte_3 = tag_name.length() % 16;
    while (1) {
        loc = std::search(loc, nbt_stop, tag_name.begin(), tag_name.end());
        if (loc == nbt_stop) {
            return nbt_stop;
        }
        if (loc - nbt_start >= 3 && *(loc - 3) == byte_1 &&
            *(loc - 2) == byte_2 && *(loc - 1) == byte_3 &&
            tag_name.length() < nbt_stop - loc) {
            return loc + tag_name.length();
        }
        loc++;
    }
}

std::pair<std::string, Tag> read_binary(std::istream &input) {
    // Get stream size
    input.seekg(0, input.end);
    std::streampos n_bytes = input.tellg();
    input.seekg(0, input.beg);

    // Read stream into vector of bytes
    std::vector<unsigned char> bytes(n_bytes);
    input.read(reinterpret_cast<char *>(bytes.data()), n_bytes);
    return read_binary(bytes);
}

std::pair<std::string, Tag> read_binary(std::vector<unsigned char> bytes) {
    return read_binary(std::span<unsigned char>(bytes));
}

std::pair<std::string, Tag> read_binary(std::span<unsigned char> bytes) {
    std::vector<unsigned char> data_holder;
    if (has_compression_header(bytes)) {
        data_holder = decompress_data(bytes);
        bytes = std::span(data_holder);
    }
    BinaryDeserializer reader(bytes.data(), bytes.size());
    auto root_data = reader.deserialize();
    auto &root_name = root_data.first;
    return {root_name, std::move(root_data.second)};
}

void write_binary(const Tag &tag, std::string_view name, std::ostream &output) {
    BinaryWriter::write(std::visit(TagID(), tag), output);
    BinaryWriter::write_string(name, output);
    std::visit(detail::PayloadSerializer{output}, tag);
}
void write_binary(const Compound &tag, std::string_view name,
                  std::ostream &output) {
    BinaryWriter::write(TagID()(tag), output);
    BinaryWriter::write_string(name, output);
    detail::PayloadSerializer{output}(tag);
}
void write_binary(const List &tag, std::string_view name,
                  std::ostream &output) {
    BinaryWriter::write(TagID()(tag), output);
    BinaryWriter::write_string(name, output);
    detail::PayloadSerializer{output}(tag);
}

std::ostream &operator<<(std::ostream &os, const Tag &tag) {
    os << tag_to_string(tag);
    return os;
}

std::ostream &operator<<(std::ostream &os, const Compound &tag) {
    os << tag_to_string(tag);
    return os;
}

std::ostream &operator<<(std::ostream &os, const List &tag) {
    os << tag_to_string(tag);
    return os;
}

} // namespace nbtview
