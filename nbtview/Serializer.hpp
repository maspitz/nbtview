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
        void operator()(const Byte_Array_Ptr &t) {
            BinaryWriter::write_vector(*t, output);
        }
        void operator()(const String_Ptr &t) {
            BinaryWriter::write_string(*t, output);
        }
        void operator()(const List_Ptr &t) {
            BinaryWriter::write(t->tag_type, output);
            BinaryWriter::write(static_cast<Int>(t->data.size()), output);
            for (Tag &elt : t->data) {
                const Tag &td = elt.tag_data();
                std::visit(*this, td);
                std::visit(*this, elt.tag_data());
            }
        }
        void operator()(const Compound_Ptr &t) {
            for (auto &kv : *t) {
                BinaryWriter::write(std::visit(TagID(), kv.second.tag_data()),
                                    output);
                BinaryWriter::write_string(kv.first, output);
                std::visit(*this, kv.second.tag_data());
            }
            BinaryWriter::write(End(0), output);
        }
        void operator()(const Int_Array_Ptr &t) {
            BinaryWriter::write_vector(*t, output);
        }
        void operator()(const Long_Array_Ptr &t) {
            BinaryWriter::write_vector(*t, output);
        }
    };

} // namespace detail

} // namespace nbtview
#endif // SERIALIZER_H_
