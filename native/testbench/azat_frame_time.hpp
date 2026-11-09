#pragma once

#include <array>
#include <cmath>
#include <cstddef>

// Average the last 60 raw frame intervals (about one second at 60 FPS).
class RollingFrameTime {
    static constexpr std::size_t sample_count = 60;
    std::array<double, sample_count> samples_{};
    std::size_t next_ = 0;
    std::size_t size_ = 0;
    double sum_ = 0;

public:
    void add(double seconds) {
        if (!std::isfinite(seconds) || seconds <= 0) return;
        if (size_ == sample_count) sum_ -= samples_[next_];
        else ++size_;
        samples_[next_] = seconds;
        sum_ += seconds;
        next_ = (next_ + 1) % sample_count;
    }

    double milliseconds() const { return size_ ? 1000.0 * sum_ / size_ : 0.0; }
};
