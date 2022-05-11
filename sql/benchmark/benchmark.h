#ifndef H_TEST_UTILS_BENCHMARK
#define H_TEST_UTILS_BENCHMARK

namespace benchmark {

int perf_init();
void perf_start(int fd);
void perf_stop(int fd);
long long perf_read(int fd);
}  // namespace benchmark

#endif