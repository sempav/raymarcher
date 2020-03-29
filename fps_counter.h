#include <chrono>
#include <numeric>
#include <vector>

class FpsCounter {
public:
    using Duration = std::chrono::duration<float>;

    FpsCounter(int window_size = 30) : times(window_size) {}

    float GetAvgFps() const { return 1.0 / GetAvgTime().count(); }

    Duration GetAvgTime() const {
        if (times_changed_since_last_query) {
            cached_avg_time = std::accumulate(begin(times), end(times), Duration(0)) / times.size();
            times_changed_since_last_query = false;
        }
        return cached_avg_time;
    }

    void RegisterFrame(std::chrono::duration<float> time) {
        times_changed_since_last_query = true;
        times[current_index] = time;
        current_index = (current_index + 1) % times.size();
    }

private:
    std::vector<std::chrono::duration<float>> times;
    int current_index = 0;

    mutable bool times_changed_since_last_query = true;
    mutable Duration cached_avg_time;
};
