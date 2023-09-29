#include <benchmark/benchmark.h>

#include <cstdint>
#include <numeric>
#include <vector>

#include <iostream>

#include "utils.hpp"

static void BM_swap_endian(benchmark::State &state) {
    std::vector<uint32_t> numbers(10000, 23);
    //    std::ranges::iota(numbers, 1u);
    // timing loop
    for (auto _ : state) {
        for (auto it = numbers.begin(); it != numbers.end(); ++it) {
            nbtview::swap_endian(*it);
            auto val = *it;
            benchmark::DoNotOptimize(val); // Prevent optimization
        }
    }
}

BENCHMARK(BM_swap_endian);

static void BM_load_big_endian(benchmark::State &state) {
    std::vector<uint32_t> numbers(10000);
    std::ranges::iota(numbers, 1u);
    // timing loop
    for (auto _ : state) {
        for (auto &n : numbers) {
            uint32_t *intptr = &n;
            auto cptr = reinterpret_cast<const unsigned char *>(intptr);
            auto val = nbtview::load_big_endian<uint32_t>(cptr);
            benchmark::DoNotOptimize(val); // Prevent optimization
        }
    }
}

BENCHMARK(BM_load_big_endian);

BENCHMARK_MAIN();
