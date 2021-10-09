#include <chrono>
#include <vector>
#include <tuple>
#include <algorithm>
#include <numeric>

using std::chrono::steady_clock;
using std::chrono::nanoseconds;

typedef std::tuple<steady_clock::time_point, steady_clock::time_point> time_pair;

class Stopwatch {

private:
    bool running = false;

    steady_clock::time_point timer_start;
    steady_clock::time_point timer_stop;

    std::vector<time_pair> points;

public:
    void start() {
        timer_start = steady_clock::now();
        running = true;
    }

    void stop() {
        if(!running) {
            return;
        }
        timer_stop = steady_clock::now();
        running = false;
        points.push_back({timer_start, timer_stop});
    }

    void restart() {
        points.clear();
        start();
    }

    nanoseconds duration() {
        std::vector<std::chrono::nanoseconds> nanos(points.size(), nanoseconds::zero());
        // nanos.resize(points.size());
        std::transform(points.begin(), points.end(), nanos.begin(), [](time_pair p) {
            return (std::get<1>(p) - std::get<0>(p));
        });
        nanoseconds result = std::accumulate(nanos.begin(), nanos.end(), nanoseconds::zero(), [](nanoseconds a, nanoseconds b) {
            return a + b;
        });
        return result;
    }
};