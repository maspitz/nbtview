// zlib_utils.cpp

#include <iostream>
#include <span>
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
        Inflater(std::span<unsigned char> input,
                 std::vector<unsigned char> &output)
            : output_(output) {
            stream_.zalloc = Z_NULL;
            stream_.zfree = Z_NULL;
            stream_.opaque = Z_NULL;
            stream_.avail_in = static_cast<uInt>(input.size());
            stream_.next_in = static_cast<Bytef *>(input.data());
            stream_.avail_out = buffer_size_;
            stream_.next_out = reinterpret_cast<Bytef *>(buffer_);

            const int AUTO_HEADER_DETECTION = 32;
            status_ = inflateInit2(&stream_, AUTO_HEADER_DETECTION | MAX_WBITS);

            output_.clear();

            if (status_ != Z_OK) {
                inflateEnd(&stream_);
                throw std::runtime_error(
                    "Failed to initialize nbtview::zlib::Inflater with error "
                    "code " +
                    std::to_string(status_));
            }
        }
        ~Inflater() { inflateEnd(&stream_); }

        void reset_input(std::span<unsigned char> input) {
            if (stream_.avail_in > 0) {
                std::cerr << "Warning: new input overrides existing input in "
                             "Inflater."
                          << std::endl;
            }
            stream_.avail_in = static_cast<uInt>(input.size());
            stream_.next_in = static_cast<Bytef *>(input.data());
        }

        int do_inflate() {

            do {
                stream_.avail_out = buffer_size_;
                stream_.next_out = reinterpret_cast<Bytef *>(buffer_);

                status_ = inflate(&stream_, Z_NO_FLUSH);

                if (status_ < 0) {
                    std::cerr << "Error while decompressing: " << stream_.msg
                              << std::endl;
                    return status_;
                }

                int bytesRead = buffer_size_ - stream_.avail_out;
                output_.insert(output_.end(), buffer_, buffer_ + bytesRead);
            } while (status_ != Z_STREAM_END && stream_.avail_in > 0);
            return status_;
        }

        int get_status() { return status_; }
        const char *err_msg() { return stream_.msg; }

      private:
        int status_;
        z_stream stream_;
        static const int buffer_size_ = 128;
        char buffer_[buffer_size_];
        std::vector<unsigned char> &output_;
    };

} // namespace zlib

std::vector<unsigned char>
decompress_data(std::vector<unsigned char> &compressed_data) {
    std::vector<unsigned char> decompressed_data;

    if (compressed_data.empty() == true) {
        return decompressed_data;
    }

    zlib::Inflater stream(compressed_data, decompressed_data);

    stream.do_inflate();

    return decompressed_data;
}

//! A return value less than zero indicates an error.
int inflate_sectors(
    const std::vector<std::span<unsigned char>> &compressed_sectors,
    std::vector<unsigned char> &decompressed_data) {

    if (compressed_sectors.empty() == true) {
        return 0;
    }

    int sector_idx = 0;
    zlib::Inflater stream(compressed_sectors[sector_idx], decompressed_data);

    // TODO:
    // return useful state information in the case of invalid input data.
    //  (sector_idx, bytes_read, error_code)

    int ret;
    ret = stream.do_inflate();
    while (ret != Z_STREAM_END) {
        sector_idx++;
        if (sector_idx >= compressed_sectors.size()) {
            throw std::runtime_error(
                "Unexpected end of input while inflating sectors.");
        }
        stream.reset_input(compressed_sectors[sector_idx]);
    }

    return ret;
}

} // namespace nbtview
