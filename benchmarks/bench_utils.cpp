#include <benchmark/benchmark.h>

#include <cstdint>

#include "BinaryReader.hpp"

static void BR_bytes_to_number_uint32(benchmark::State &state) {
    std::vector<uint32_t> numbers(10000);
    std::ranges::iota(numbers, 1u);
    auto buffer = reinterpret_cast<const unsigned char *>(numbers.data());
    auto buffer_len = numbers.size() * sizeof(uint32_t);
    // timing loop
    for (auto _ : state) {
        nbtview::BinaryReader br(buffer, buffer_len);
        for (auto &n : numbers) {
            auto val = br.read<uint32_t>();
            benchmark::DoNotOptimize(val); // Prevent optimization
        }
    }
}

BENCHMARK(BR_bytes_to_number_uint32);

static void BR_bytes_to_number_float(benchmark::State &state) {
    std::vector<float> numbers(10000);
    std::ranges::iota(numbers, 1u);
    auto buffer = reinterpret_cast<const unsigned char *>(numbers.data());
    auto buffer_len = numbers.size() * sizeof(float);
    // timing loop
    for (auto _ : state) {
        nbtview::BinaryReader br(buffer, buffer_len);
        for (auto &n : numbers) {
            auto val = br.read<float>();
            benchmark::DoNotOptimize(val); // Prevent optimization
        }
    }
}

BENCHMARK(BR_bytes_to_number_float);

BENCHMARK_MAIN();
