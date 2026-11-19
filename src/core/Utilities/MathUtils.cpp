#include "MathUtils.hpp"
#include <random>
#include <chrono>
#include <numeric>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>

namespace nlm {

// MathUtils implementation
float MathUtils::sigmoid(float x) {
    return 1.0f / (1.0f + std::exp(-x));
}

float MathUtils::sigmoidDerivative(float x) {
    float s = sigmoid(x);
    return s * (1.0f - s);
}

float MathUtils::relu(float x) {
    return std::max(0.0f, x);
}

float MathUtils::reluDerivative(float x) {
    return (x > 0.0f) ? 1.0f : 0.0f;
}

float MathUtils::tanh(float x) {
    return std::tanh(x);
}

float MathUtils::tanhDerivative(float x) {
    float t = tanh(x);
    return 1.0f - t * t;
}

float MathUtils::mean(const std::vector<float>& values) {
    if (values.empty()) return 0.0f;
    return sum(values) / static_cast<float>(values.size());
}

float MathUtils::variance(const std::vector<float>& values) {
    if (values.size() <= 1) return 0.0f;
    float m = mean(values);
    float sum = 0.0f;
    for (float val : values) {
        sum += (val - m) * (val - m);
    }
    return sum / static_cast<float>(values.size() - 1);
}

float MathUtils::standardDeviation(const std::vector<float>& values) {
    return std::sqrt(variance(values));
}

float MathUtils::median(const std::vector<float>& values) {
    if (values.empty()) return 0.0f;
    std::vector<float> sorted = values;
    std::sort(sorted.begin(), sorted.end());
    size_t n = sorted.size();
    if (n % 2 == 0) {
        return (sorted[n/2 - 1] + sorted[n/2]) / 2.0f;
    } else {
        return sorted[n/2];
    }
}

std::vector<float> MathUtils::add(const std::vector<float>& a, const std::vector<float>& b) {
    if (a.size() != b.size()) {
        throw std::invalid_argument("Vector sizes must match for addition");
    }
    std::vector<float> result(a.size());
    for (size_t i = 0; i < a.size(); ++i) {
        result[i] = a[i] + b[i];
    }
    return result;
}

std::vector<float> MathUtils::multiply(const std::vector<float>& a, const std::vector<float>& b) {
    if (a.size() != b.size()) {
        throw std::invalid_argument("Vector sizes must match for multiplication");
    }
    std::vector<float> result(a.size());
    for (size_t i = 0; i < a.size(); ++i) {
        result[i] = a[i] * b[i];
    }
    return result;
}

std::vector<float> MathUtils::scalarMultiply(float scalar, const std::vector<float>& vec) {
    std::vector<float> result(vec.size());
    for (size_t i = 0; i < vec.size(); ++i) {
        result[i] = scalar * vec[i];
    }
    return result;
}

float MathUtils::dotProduct(const std::vector<float>& a, const std::vector<float>& b) {
    if (a.size() != b.size()) {
        throw std::invalid_argument("Vector sizes must match for dot product");
    }
    float result = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        result += a[i] * b[i];
    }
    return result;
}

float MathUtils::vectorNorm(const std::vector<float>& vec) {
    return std::sqrt(dotProduct(vec, vec));
}

std::vector<float> MathUtils::applyThreshold(const std::vector<float>& values, float threshold) {
    std::vector<float> result(values.size());
    for (size_t i = 0; i < values.size(); ++i) {
        result[i] = (values[i] >= threshold) ? values[i] : 0.0f;
    }
    return result;
}

std::vector<float> MathUtils::clamp(const std::vector<float>& values, float min, float max) {
    std::vector<float> result(values.size());
    for (size_t i = 0; i < values.size(); ++i) {
        result[i] = std::max(min, std::min(max, values[i]));
    }
    return result;
}

std::vector<float> MathUtils::normalize(const std::vector<float>& values, float min, float max) {
    if (values.empty()) return {};
    float dataMin = *std::min_element(values.begin(), values.end());
    float dataMax = *std::max_element(values.begin(), values.end());
    
    if (dataMax == dataMin) return std::vector<float>(values.size(), 0.5f);
    
    std::vector<float> result(values.size());
    for (size_t i = 0; i < values.size(); ++i) {
        result[i] = (values[i] - dataMin) / (dataMax - dataMin);
    }
    return result;
}

void MathUtils::seedRandom(unsigned int seed) {
    static std::mt19937 rng(seed);
}

float MathUtils::randomFloat(float min, float max) {
    static std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<float> dist(min, max);
    return dist(rng);
}

int MathUtils::randomInt(int min, int max) {
    static std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}

uint64_t MathUtils::getCurrentTimestamp() {
    auto now = std::chrono::steady_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
}

double MathUtils::getElapsedSeconds(uint64_t startTime) {
    uint64_t endTime = getCurrentTimestamp();
    return (endTime - startTime) / 1000000.0;
}

bool MathUtils::isPowerOfTwo(int n) {
    return n > 0 && (n & (n - 1)) == 0;
}

int MathUtils::nextPowerOfTwo(int n) {
    int power = 1;
    while (power < n) {
        power <<= 1;
    }
    return power;
}

size_t MathUtils::getMemoryUsageMB() {
    // Platform-specific implementation would go here
    // For now, return 0 as a placeholder
    return 0;
}

} // namespace nlm
