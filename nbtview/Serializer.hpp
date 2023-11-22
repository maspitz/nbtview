/**
 * @file Serializer.hpp
 * @brief Methods for binary serialization of NBT data
 * @author Michael Spitznagel
 * @copyright Copyright 2023 Michael Spitznagel. Released under the Boost
 * Software License 1.0
 *
 * https://github.com/maspitz/nbtview
 */

#ifndef SERIALIZER_H_
#define SERIALIZER_H_

#include <ostream>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

#include "BinaryWriter.hpp"
#include "Tag.hpp"

namespace nbtview {

namespace detail {

    struct PayloadSerializer {
        std::ostream &output;

        void operator()(const None &t) {}
        void operator()(const End &t) { BinaryWriter::write(t, output); }
        void operator()(const Byte &t) { BinaryWriter::write(t, output); }
        void operator()(const Short &t) { BinaryWriter::write(t, output); }
        void operator()(const Int &t) { BinaryWriter::write(t, output); }
        void operator()(const Long &t) { BinaryWriter::write(t, output); }
        void operator()(const Float &t) { BinaryWriter::write(t, output); }
        void operator()(const Double &t) { BinaryWriter::write(t, output); }
        void operator()(const Byte_Array &t) {
            BinaryWriter::write_vector(t, output);
        }
        void operator()(const String &t) {
            BinaryWriter::write_string(t, output);
        }
        void operator()(const List &t) {
            BinaryWriter::write(static_cast<Byte>(list_type(t)), output);
            BinaryWriter::write(static_cast<Int>(t.size()), output);
            for (const Tag &elt : t) {
                std::visit(*this, elt.get_value());
            }
        }
        void operator()(const Compound &t) {
            for (auto &[tag_name, tag_data] : t) {
                BinaryWriter::write(std::visit(TagID(), tag_data.get_value()),
                                    output);
                BinaryWriter::write_string(tag_name, output);
                std::visit(*this, tag_data.get_value());
            }
            BinaryWriter::write(End(0), output);
        }
        void operator()(const Int_Array &t) {
            BinaryWriter::write_vector(t, output);
        }
        void operator()(const Long_Array &t) {
            BinaryWriter::write_vector(t, output);
        }
    };

} // namespace detail

} // namespace nbtview
#endif // SERIALIZER_H_
