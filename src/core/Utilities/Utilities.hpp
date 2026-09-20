#pragma once

namespace nlm {

// Utility functions for numerical operations
inline float clamp(float value, float min, float max) {
    return value < min ? min : (value > max ? max : value);
}

inline float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

inline float easeInOut(float t) {
    return t * t / (t * t + (2.0f - t) * (2.0f - t));
}

// Exponential moving average for smoothing
class ExponentialMovingAverage {
public:
    ExponentialMovingAverage(float alpha = 0.1f) : alpha_(alpha), value_(0.0f) {}
    
    void update(float newValue) {
        value_ = alpha_ * newValue + (1.0f - alpha_) * value_;
    }
    
    float getValue() const { return value_; }
    void reset() { value_ = 0.0f; }
    
private:
    float alpha_;
    float value_;
};

// Circular buffer for history tracking
class CircularBuffer {
public:
    CircularBuffer(size_t size) : data_(size), head_(0), count_(0) {}
    
    void add(float value) {
        data_[head_] = value;
        head_ = (head_ + 1) % data_.size();
        if (count_ < data_.size()) count_++;
    }
    
    float get(size_t index) const {
        if (index >= count_) return 0.0f;
        size_t idx = (head_ + data_.size() - 1 - index) % data_.size();
        return data_[idx];
    }
    
    size_t size() const { return count_; }
    void clear() {
        head_ = 0;
        count_ = 0;
    }
    
private:
    std::vector<float> data_;
    size_t head_;
    size_t count_;
};

// Normalization utilities
inline float normalize(float value, float min, float max) {
    if (max <= min) return 0.0f;
    return (value - min) / (max - min);
}

inline float denormalize(float normalized, float min, float max) {
    return min + normalized * (max - min);
}

// Distance and proximity calculations
inline float euclideanDistance(float x1, float y1, float x2, float y2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    return std::sqrt(dx * dx + dy * dy);
}

inline float squaredDistance(float x1, float y1, float x2, float y2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    return dx * dx + dy * dy;
}

// Angle calculations
inline float angleBetween(float x1, float y1, float x2, float y2) {
    return std::atan2(y2 - y1, x2 - x1);
}

inline float normalizeAngle(float angle) {
    while (angle > M_PI) angle -= 2.0f * M_PI;
    while (angle < -M_PI) angle += 2.0f * M_PI;
    return angle;
}

// Random utilities
inline float randomFloat(float min, float max) {
    return min + static_cast<float>(rand()) / RAND_MAX * (max - min);
}

// Clamping for different types
template<typename T>
inline T clamp(T value, T min, T max) {
    return value < min ? min : (value > max ? max : value);
}

} // namespace nlm
