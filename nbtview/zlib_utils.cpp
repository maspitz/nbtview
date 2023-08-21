// zlib_utils.cpp

#include <iostream>
#include <span>
#include <utility>
#include <vector>

#include <zlib.h>

#include "zlib_utils.hpp"

namespace nbtview {

bool has_compression_header(const std::span<unsigned char> data) {
    if (data.size() < 4) {
        return false;
    }

    // all possible ZLIB headers, given DEFLATE method and a 32KB window
    // [RFC1950]
    if (data[0] == 0x78 && (data[1] == 0x01 || data[1] == 0x5e ||
                            data[1] == 0x9c || data[1] == 0xda)) {
        return true;
    }

    // GZIP header bytes, given DEFLATE method [RFC1952]
    if (data[0] == 0x1f && data[1] == 0x8b && data[2] == 0x08) {
        return true;
    }

    return false;
}

namespace zlib {

    //! Inflater wraps z_stream to ensure RAII semantics.
    class Inflater {
      public:
        Inflater() {
            input_bytes_read_ = 0;
            stream_.zalloc = Z_NULL;
            stream_.zfree = Z_NULL;
            stream_.opaque = Z_NULL;
            stream_.avail_in = 0;
            stream_.next_in = Z_NULL;

            const int AUTO_HEADER_DETECTION = 32;
            int status =
                inflateInit2(&stream_, AUTO_HEADER_DETECTION | MAX_WBITS);

            if (status != Z_OK) {
                inflateEnd(&stream_);
                throw std::runtime_error(
                    "Failed to initialize nbtview::zlib::Inflater with error "
                    "code " +
                    std::to_string(status));
            }
        }

        ~Inflater() { inflateEnd(&stream_); }

        /**
         * do_inflate() inflates the compressed data input and copies
         * uncompressed data to the output.
         *
         * Returns Z_DATA_ERROR: if the input data is corrupt.
         *
         * Returns Z_OK: if it has inflated all of the data input without
         * reaching the end of the compressed data stream.
         *
         * Returns Z_STREAM_END: if it has reached the end of the compressed
         * data stream.
         *
         * Returns Z_NEED_DICT, Z_STREAM_ERROR, Z_MEM_ERROR, Z_BUF_ERROR:
         * for various other conditions under which inflation cannot continue.
         * */
        int do_inflate(std::span<unsigned char> input,
                       std::vector<unsigned char> &output) {
            if (stream_.avail_in > 0) {
                std::cerr << "Warning: new input overrides existing unconsumed "
                             "input in Inflater."
                          << std::endl;
            }
            stream_.avail_in = static_cast<uInt>(input.size());
            stream_.next_in = static_cast<Bytef *>(input.data());

            int status = Z_OK;
            while ((stream_.avail_in > 0) && (status == Z_OK)) {
                stream_.avail_out = buffer_size_;
                stream_.next_out = reinterpret_cast<Bytef *>(buffer_);

                status = inflate(&stream_, Z_NO_FLUSH);

                int output_byte_count = buffer_size_ - stream_.avail_out;
                output.insert(output.end(), buffer_,
                              buffer_ + output_byte_count);
            }

            input_bytes_read_ = input.size() - stream_.avail_in;
            return status;
        }

        int input_bytes_read() { return input_bytes_read_; }
        const char *err_msg() { return stream_.msg; }

      private:
        int input_bytes_read_;
        z_stream stream_;
        static const int buffer_size_ = 128;
        char buffer_[buffer_size_];
    };

} // namespace zlib

std::vector<unsigned char>
decompress_data(std::vector<unsigned char> &input_data) {
    zlib::Inflater stream;
    std::vector<unsigned char> output_data;
    int status = stream.do_inflate(input_data, output_data);
    if (status != Z_STREAM_END) {
        throw std::runtime_error(
            "Could not decompress data (likely corrupt or incomplete)");
    }
    return output_data;
}

//! A return value less than zero indicates an error.
std::pair<std::vector<unsigned char>, Inflation_Status>
inflate_sectors(const std::vector<std::span<unsigned char>> &sectors) {
    zlib::Inflater stream;
    std::vector<unsigned char> output;
    Inflation_Status stat{
        .complete = false, .corrupt = false, .corrupt_sector = -1};

    int idx = 0;
    for (auto s : sectors) {
        int status = stream.do_inflate(sectors[idx], output);
        if (status == Z_STREAM_END) {
            stat.complete = true;
            break;
        } else if (status != Z_OK) {
            stat.corrupt = true;
            stat.corrupt_sector = idx;
            break;
        }
        idx++;
    }
    return {output, stat};
}

} // namespace nbtview
