#include <benchmark/benchmark.h>

namespace siga {

void first_benchmark(benchmark::State &state)
{
    for(auto _ : state) {
        benchmark::DoNotOptimize(std::string{});
    }
}

void second_benchmark(benchmark::State &state)
{
    for(auto _ : state) {
        benchmark::DoNotOptimize(std::string{});
    }
}

} // namespace siga

BENCHMARK(siga::first_benchmark);
BENCHMARK(siga::second_benchmark);
