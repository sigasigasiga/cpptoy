#include <benchmark/benchmark.h>

namespace siga {

void first_benchmark(benchmark::State &state)
{
    for(auto _ : state) {
        std::string s;
    }
}

void second_benchmark(benchmark::State &state)
{
    for(auto _ : state) {
        std::string s;
    }
}

} // namespace siga

BENCHMARK(siga::first_benchmark);
BENCHMARK(siga::second_benchmark);
