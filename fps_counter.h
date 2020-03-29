#include <vector>

class FpsCounter {
    int buflen;
    std::vector<float> times;
    int cur;

    static constexpr int DEFAULT_LEN = 10;
    static constexpr float DEFAULT_TIME = 1.0 / 30;

public:
    FpsCounter(int buflen = DEFAULT_LEN) : buflen(buflen), times(buflen, DEFAULT_TIME) {}
    float GetAvgFps() { return 1.0 / GetAvgTime(); }
    float GetAvgTime() {
        float res = 0.0f;
        for (float f : times) {
            res += f;
        }
        return res / buflen;
    }
    void AddFrame(float time) {
        times[cur] = time;
        cur = (cur + 1) % buflen;
    }
};
