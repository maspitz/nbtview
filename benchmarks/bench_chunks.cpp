#include <benchmark/benchmark.h>

#include <fstream>
#include <vector>

#include "Region.hpp"
#include "nbtview.hpp"
#include "zlib_utils.hpp"

namespace nbt = nbtview;

static void BM_chunk_file_reads(benchmark::State &state) {
    const auto filename = "test_data/r.0.0.mca";
    int region_x = 0;
    int region_z = 0;
    nbt::Region_File reg(filename);

    // timing loop
    for (auto _ : state) {
        for (int i = 0; i < nbt::Region::chunk_count; ++i) {
            auto chunk_offset = reg.chunk_offset(i);
            auto chunk_length = reg.chunk_length(i);
            if (chunk_length == 0) {
                continue;
            }
            auto chunk_data = reg.get_chunk_data(i);
            auto [root_name, root_tag] =
                nbt::read_binary(chunk_data.data(), chunk_data.size());

            if (root_tag.is<nbt::Compound>() == false) {
                continue;
            }
            if (root_tag.contains("Level") == false) {
                continue;
            }
            nbt::Tag &level = root_tag["Level"];

            if (!level.is<nbt::Compound>() || !level.contains("xPos") ||
                !level.contains("yPos")) {
                continue;
            }

            nbt::Int xPos = level["xPos"].get<nbt::Int>();
            nbt::Int zPos = level["zPos"].get<nbt::Int>();

            // let local_x, local_z be chunk coordinates local to this region.
            nbt::Int local_x = xPos & 0x1f;
            nbt::Int local_z = zPos & 0x1f;
            benchmark::DoNotOptimize(xPos); // Prevent optimization
            benchmark::DoNotOptimize(zPos);
            benchmark::DoNotOptimize(local_x);
            benchmark::DoNotOptimize(local_z);
        }
    }
}

BENCHMARK(BM_chunk_file_reads);

static void BM_chunk_decoding(benchmark::State &state) {
    const auto filename = "test_data/r.0.0.mca";
    int region_x = 0;
    int region_z = 0;

    // read and decompress chunk data

    nbt::Region_File reg(filename);

    std::vector<std::vector<unsigned char>> chunk_data;
    for (int i = 0; i < nbt::Region::chunk_count; ++i) {
        chunk_data.push_back(reg.get_chunk_data(i));
        while (nbt::has_compression_header(chunk_data[i].data(),
                                           chunk_data[i].size())) {
            chunk_data[i] = nbt::decompress_data(chunk_data[i].data(),
                                                 chunk_data[i].size());
        }
    }

    // timing loop: deserialize chunk data, etc.
    for (auto _ : state) {
        for (int i = 0; i < nbt::Region::chunk_count; ++i) {
            auto chunk_offset = reg.chunk_offset(i);
            auto chunk_length = reg.chunk_length(i);
            if (chunk_length == 0) {
                continue;
            }
            auto [root_name, root_tag] =
                nbt::read_binary(chunk_data[i].data(), chunk_data[i].size());

            if (!root_tag.is<nbt::Compound>() || !root_tag.contains("Level")) {
                continue;
            }

            nbt::Tag &level = root_tag["Level"];

            if (!level.contains("xPos") || !level.contains("zPos")) {
                continue;
            }
            nbt::Int xPos = level["xPos"].get<nbt::Int>();
            nbt::Int zPos = level["zPos"].get<nbt::Int>();

            // let local_x, local_z be chunk coordinates local to this region.
            nbt::Int local_x = xPos & 0x1f;
            nbt::Int local_z = zPos & 0x1f;
            benchmark::DoNotOptimize(xPos); // Prevent optimization
            benchmark::DoNotOptimize(zPos);
            benchmark::DoNotOptimize(local_x);
            benchmark::DoNotOptimize(local_z);
        }
    }
}

BENCHMARK(BM_chunk_decoding);

BENCHMARK_MAIN();
