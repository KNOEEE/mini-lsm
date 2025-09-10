#include <string.h>
#include <random> // The essential header!

#include <benchmark/benchmark.h>

static void StringCopy(benchmark::State& state) {
  std::string s = "abcabcabc";
  for (auto _ : state) {
    std::string copy(s);
  }
}

BENCHMARK(StringCopy)->Iterations(1e4);

static void BM_memcpy(benchmark::State& state) {
  char* src = new char[state.range(0)];
  char* dst = new char[state.range(0)];
  memset(src, 'x', state.range(0));
  for (auto _ : state)
    memcpy(dst, src, state.range(0));
  state.SetBytesProcessed(int64_t(state.iterations()) *
                          int64_t(state.range(0)));
  delete[] src;
  delete[] dst;
}
// BENCHMARK(BM_memcpy)->Arg(8)->Arg(64)->Arg(512)->Arg(4<<10)->Arg(8<<10);
BENCHMARK(BM_memcpy)->Range(8, 8<<10);

static void BM_DenseRange(benchmark::State& state) {
  for(auto _ : state) {
    std::vector<int> v(state.range(0), state.range(0));
    auto data = v.data();
    benchmark::DoNotOptimize(data);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(BM_DenseRange)->DenseRange(0, 1024, 128);

int RandomNumber() {
  std::random_device rd;
  std::mt19937 engine(rd());
  std::uniform_int_distribution<int> distribution(1, 1e5);
  return distribution(engine);
}

std::set<int> ConstructRandomSet(int count) {
  std::set<int> s;
  for (int i = 0; i < count; i++) {
    s.insert(RandomNumber());
  }
  return s;
}

static void BM_SetInsert(benchmark::State& state) {
  std::set<int> data;
  for (auto _ : state) {
    state.PauseTiming();
    data = ConstructRandomSet(state.range(0));
    state.ResumeTiming();
    for (int j = 0; j < state.range(1); ++j)
      data.insert(RandomNumber());
  }
}
// BENCHMARK(BM_SetInsert)
//     ->Args({1<<10, 128})
//     ->Args({8<<10, 512});

static void CustomArguments(benchmark::internal::Benchmark* b) {
  for (int i = 0; i <= 2; ++i)
    for (int j = 32; j <= 1024*16; j *= 8)
      b->Args({i, j});
}
BENCHMARK(BM_SetInsert)->Apply(CustomArguments);

// https://github.com/google/benchmark/blob/main/docs/user_guide.md#passing-arguments