#include "Utilities.hpp"
#include <cmath>
#include <cstdlib>

namespace nlm {

// Implementations of utility functions

// Clamp function implementation
template<typename T>
T clamp(T value, T min, T max) {
    return value < min ? min : (value > max ? max : value);
}

// Define needed clamp for float
float clamp(float value, float min, float max) {
    return value < min ? min : (value > max ? max : value);
}

// Lerp implementation
float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

// Ease in-out implementation
float easeInOut(float t) {
    return t * t / (t * t + (2.0f - t) * (2.0f - t));
}

// ExponentialMovingAverage implementation
void ExponentialMovingAverage::update(float newValue) {
    value_ = alpha_ * newValue + (1.0f - alpha_) * value_;
}

// Normalization functions
float normalize(float value, float min, float max) {
    if (max <= min) return 0.0f;
    return (value - min) / (max - min);
}

float denormalize(float normalized, float min, float max) {
    return min + normalized * (max - min);
}

// Distance calculations
float euclideanDistance(float x1, float y1, float x2, float y2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    return std::sqrt(dx * dx + dy * dy);
}

float squaredDistance(float x1, float y1, float x2, float y2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    return dx * dx + dy * dy;
}

// Angle calculations
float angleBetween(float x1, float y1, float x2, float y2) {
    return std::atan2(y2 - y1, x2 - x1);
}

float normalizeAngle(float angle) {
    while (angle > M_PI) angle -= 2.0f * M_PI;
    while (angle < -M_PI) angle += 2.0f * M_PI;
    return angle;
}

// Random function
float randomFloat(float min, float max) {
    return min + static_cast<float>(rand()) / RAND_MAX * (max - min);
}

} // namespace nlm
