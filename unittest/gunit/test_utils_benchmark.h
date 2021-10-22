// #ifndef H_TEST_UTILS_BENCHMARK
// #define H_TEST_UTILS_BENCHMARK

// #include <chrono>
// #include <vector>
// #include <tuple>

// namespace benchmark {

//     using std::chrono::_V2::steady_clock;

//     typedef std::tuple<steady_clock::time_point, steady_clock::time_point> time_pair;

//     int perf_init();
//     void perf_start(int fd);
//     void perf_stop(int fd);
//     long long perf_read(int fd);

//     class Stopwatch {
//         private:
//             bool running = false;

//             steady_clock::time_point timer_start;
//             steady_clock::time_point timer_stop;

//             std::vector<time_pair> points;

//         public:
//             void start();
//             void stop();
//             void restart();
//             std::chrono::nanoseconds duration();
//     };

// }

// #endif