#include <benchmark/benchmark.h>

#include <cstdint>
#include <numeric>
#include <span>
#include <vector>

#include <iostream>

#include "utils.hpp"

static void BM_swap_endian_uint32(benchmark::State &state) {
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

BENCHMARK(BM_swap_endian_uint32);

static void BM_load_big_endian_uint32(benchmark::State &state) {
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

BENCHMARK(BM_load_big_endian_uint32);

static void BM_bytes_to_number_uint32(benchmark::State &state) {
    std::vector<uint32_t> numbers(10000);
    std::ranges::iota(numbers, 1u);
    // timing loop
    for (auto _ : state) {
        for (auto &n : numbers) {
            uint32_t *intptr = &n;
            auto cptr = reinterpret_cast<const unsigned char *>(intptr);
            auto bytes_span =
                std::span<const unsigned char>(cptr, cptr + sizeof(uint32_t));
            auto val = nbtview::bytes_to_number<uint32_t>(bytes_span);
            benchmark::DoNotOptimize(val); // Prevent optimization
        }
    }
}

BENCHMARK(BM_bytes_to_number_uint32);

static void BM_bytes_to_uint32(benchmark::State &state) {
    std::vector<uint32_t> numbers(10000);
    std::ranges::iota(numbers, 1u);
    // timing loop
    for (auto _ : state) {
        for (auto &n : numbers) {
            uint32_t *intptr = &n;
            auto cptr = reinterpret_cast<const unsigned char *>(intptr);
            auto bytes_span =
                std::span<const unsigned char>(cptr, cptr + sizeof(uint32_t));
            auto val = nbtview::bytes_to_uint32(bytes_span);
            benchmark::DoNotOptimize(val); // Prevent optimization
        }
    }
}

BENCHMARK(BM_bytes_to_uint32);

static void BM_swap_endian_float(benchmark::State &state) {
    std::vector<float> numbers(10000, 23);
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

BENCHMARK(BM_swap_endian_float);

static void BM_load_big_endian_float(benchmark::State &state) {
    std::vector<float> numbers(10000);
    std::ranges::iota(numbers, 1u);
    // timing loop
    for (auto _ : state) {
        for (auto &n : numbers) {
            float *intptr = &n;
            auto cptr = reinterpret_cast<const unsigned char *>(intptr);
            auto val = nbtview::load_big_endian<float>(cptr);
            benchmark::DoNotOptimize(val); // Prevent optimization
        }
    }
}

BENCHMARK(BM_load_big_endian_float);

static void BM_bytes_to_number_float(benchmark::State &state) {
    std::vector<float> numbers(10000);
    std::ranges::iota(numbers, 1u);
    // timing loop
    for (auto _ : state) {
        for (auto &n : numbers) {
            float *intptr = &n;
            auto cptr = reinterpret_cast<const unsigned char *>(intptr);
            auto bytes_span =
                std::span<const unsigned char>(cptr, cptr + sizeof(float));
            auto val = nbtview::bytes_to_number<float>(bytes_span);
            benchmark::DoNotOptimize(val); // Prevent optimization
        }
    }
}

BENCHMARK(BM_bytes_to_number_float);

BENCHMARK_MAIN();
