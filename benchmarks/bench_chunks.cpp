#include <benchmark/benchmark.h>

#include <fstream>
#include <vector>

#include "Region.hpp"
#include "nbtview.hpp"
#include "zlib_utils.hpp"

static void BM_chunk_file_reads(benchmark::State &state) {
    const auto filename = "test_data/r.0.0.mca";
    int region_x = 0;
    int region_z = 0;
    nbtview::Region_File reg(filename);

    // timing loop
    for (auto _ : state) {
        for (int i = 0; i < nbtview::Region::chunk_count; ++i) {
            auto chunk_offset = reg.chunk_offset(i);
            auto chunk_length = reg.chunk_length(i);
            if (chunk_length == 0) {
                continue;
            }
            auto chunk_data = reg.get_chunk_data(i);
            auto [root_name, root_tag] =
                nbtview::read_binary(chunk_data.data(), chunk_data.size());

            if (!std::holds_alternative<nbtview::Compound>(root_tag)) {
                continue;
            }
            nbtview::Compound &root = std::get<nbtview::Compound>(root_tag);

            if (!root.contains<nbtview::Compound>("Level")) {
                continue;
            }
            nbtview::Compound &level = root.get<nbtview::Compound>("Level");

            if (!level.contains<nbtview::Int>("xPos") ||
                !level.contains<nbtview::Int>("zPos")) {
                continue;
            }
            nbtview::Int xPos = level.get<nbtview::Int>("xPos");
            nbtview::Int zPos = level.get<nbtview::Int>("zPos");

            // let local_x, local_z be chunk coordinates local to this region.
            nbtview::Int local_x = xPos & 0x1f;
            nbtview::Int local_z = zPos & 0x1f;
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

    nbtview::Region_File reg(filename);

    std::vector<std::vector<unsigned char>> chunk_data;
    for (int i = 0; i < nbtview::Region::chunk_count; ++i) {
        chunk_data.push_back(reg.get_chunk_data(i));
        while (nbtview::has_compression_header(chunk_data[i].data(),
                                               chunk_data[i].size())) {
            chunk_data[i] = nbtview::decompress_data(chunk_data[i].data(),
                                                     chunk_data[i].size());
        }
    }

    // timing loop: deserialize chunk data, etc.
    for (auto _ : state) {
        for (int i = 0; i < nbtview::Region::chunk_count; ++i) {
            auto chunk_offset = reg.chunk_offset(i);
            auto chunk_length = reg.chunk_length(i);
            if (chunk_length == 0) {
                continue;
            }
            auto [root_name, root_tag] = nbtview::read_binary(
                chunk_data[i].data(), chunk_data[i].size());

            if (!std::holds_alternative<nbtview::Compound>(root_tag)) {
                continue;
            }
            nbtview::Compound &root = std::get<nbtview::Compound>(root_tag);

            if (!root.contains<nbtview::Compound>("Level")) {
                continue;
            }
            nbtview::Compound &level = root.get<nbtview::Compound>("Level");

            if (!level.contains<nbtview::Int>("xPos") ||
                !level.contains<nbtview::Int>("zPos")) {
                continue;
            }
            nbtview::Int xPos = level.get<nbtview::Int>("xPos");
            nbtview::Int zPos = level.get<nbtview::Int>("zPos");

            // let local_x, local_z be chunk coordinates local to this region.
            nbtview::Int local_x = xPos & 0x1f;
            nbtview::Int local_z = zPos & 0x1f;
            benchmark::DoNotOptimize(xPos); // Prevent optimization
            benchmark::DoNotOptimize(zPos);
            benchmark::DoNotOptimize(local_x);
            benchmark::DoNotOptimize(local_z);
        }
    }
}

BENCHMARK(BM_chunk_decoding);

BENCHMARK_MAIN();
