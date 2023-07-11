// Serializer.hpp

#ifndef SERIALIZER_H_
#define SERIALIZER_H_

#include <ostream>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

#include "BinaryWriter.hpp"
#include "Compound.hpp"
#include "List.hpp"
#include "Tag.hpp"

namespace nbtview {

namespace detail {

    struct PayloadSerializer {
        std::ostream &output;

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
            BinaryWriter::write(t.list_type(), output);
            BinaryWriter::write(static_cast<Int>(t.size()), output);
            for (const Tag &elt : t) {
                std::visit(*this, elt);
            }
        }
        void operator()(const Compound &t) {
            for (const auto &kv : t) {
                BinaryWriter::write(std::visit(TagID(), kv.second), output);
                BinaryWriter::write_string(kv.first, output);
                std::visit(*this, kv.second);
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

class Serializer {
  public:
    static void serialize(const Tag &t, std::string_view name,
                          std::ostream &output) {
        BinaryWriter::write(std::visit(TagID(), t), output);
        BinaryWriter::write_string(name, output);
        std::visit(detail::PayloadSerializer{output}, t);
    }
    static void serialize(const Compound &t, std::string_view name,
                          std::ostream &output) {
        BinaryWriter::write(TagID()(t), output);
        BinaryWriter::write_string(name, output);
        detail::PayloadSerializer{output}(t);
    }
    static void serialize(const List &t, std::string_view name,
                          std::ostream &output) {
        BinaryWriter::write(TagID()(t), output);
        BinaryWriter::write_string(name, output);
        detail::PayloadSerializer{output}(t);
    }
};

} // namespace nbtview
#endif // SERIALIZER_H_
