#include "MathUtils.hpp"
#include <chrono>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <random>

namespace nlm {

// MathUtils implementation

std::mt19937 MathUtils::randomGenerator_;

template<typename T>
T MathUtils::mean(const std::vector<T>& values) {
    if (values.empty()) {
        throw std::invalid_argument("Cannot compute mean of empty vector");
    }
    
    T sum = std::accumulate(values.begin(), values.end(), T{});
    return sum / values.size();
}

template<typename T>
T MathUtils::median(std::vector<T> values) {
    if (values.empty()) {
        throw std::invalid_argument("Cannot compute median of empty vector");
    }
    
    std::sort(values.begin(), values.end());
    size_t size = values.size();
    
    if (size % 2 == 0) {
        return (values[size / 2 - 1] + values[size / 2]) / 2;
    } else {
        return values[size / 2];
    }
}

template<typename T>
T MathUtils::standardDeviation(const std::vector<T>& values, T mean) {
    if (values.empty()) {
        throw std::invalid_argument("Cannot compute standard deviation of empty vector");
    }
    
    T sumOfSquares = std::accumulate(values.begin(), values.end(), T{},
        [mean](T acc, T val) { return acc + (val - mean) * (val - mean); });
    
    return std::sqrt(sumOfSquares / values.size());
}

template<typename T>
T MathUtils::variance(const std::vector<T>& values, T mean) {
    if (values.empty()) {
        throw std::invalid_argument("Cannot compute variance of empty vector");
    }
    
    T sumOfSquares = std::accumulate(values.begin(), values.end(), T{},
        [mean](T acc, T val) { return acc + (val - mean) * (val - mean); });
    
    return sumOfSquares / values.size();
}

template<typename T>
T MathUtils::sum(const std::vector<T>& values) {
    return std::accumulate(values.begin(), values.end(), T{});
}

template<typename T>
T MathUtils::product(const std::vector<T>& values) {
    return std::accumulate(values.begin(), values.end(), T{},
        std::multiplies<T>());
}

template<typename T>
T MathUtils::min(const std::vector<T>& values) {
    if (values.empty()) {
        throw std::invalid_argument("Cannot find minimum of empty vector");
    }
    
    return *std::min_element(values.begin(), values.end());
}

template<typename T>
T MathUtils::max(const std::vector<T>& values) {
    if (values.empty()) {
        throw std::invalid_argument("Cannot find maximum of empty vector");
    }
    
    return *std::max_element(values.begin(), values.end());
}

template<typename T>
std::vector<T> MathUtils::clamp(const std::vector<T>& values, T min, T max) {
    std::vector<T> result;
    result.reserve(values.size());
    
    for (const T& value : values) {
        result.push_back(std::clamp(value, min, max));
    }
    
    return result;
}

template<typename T>
T MathUtils::dotProduct(const std::vector<T>& a, const std::vector<T>& b) {
    if (a.size() != b.size()) {
        throw std::invalid_argument("Vectors must have same size for dot product");
    }
    
    T result = std::inner_product(a.begin(), a.end(), b.begin(), T{});
    return result;
}

template<typename T>
std::vector<T> MathUtils::normalize(const std::vector<T>& vector, T length) {
    T currentLength = std::sqrt(dotProduct(vector, vector));
    
    if (currentLength == 0 || currentLength == length) {
        return vector;
    }
    
    std::vector<T> result;
    result.reserve(vector.size());
    
    for (const T& value : vector) {
        result.push_back(value * length / currentLength);
    }
    
    return result;
}

float MathUtils::sigmoid(float x) {
    return 1.0f / (1.0f + std::exp(-x));
}

float MathUtils::relu(float x) {
    return x < 0.0f ? 0.0f : x;
}

float MathUtils::tanh(float x) {
    return std::tanh(x);
}

template<typename T>
T MathUtils::interpolate(T start, T end, float alpha) {
    return start + (end - start) * alpha;
}

float MathUtils::randomFloat(float min, float max) {
    std::uniform_real_distribution<float> distribution(min, max);
    return distribution(randomGenerator_);
}

int MathUtils::randomInt(int min, int max) {
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(randomGenerator_);
}

int MathUtils::roundToInt(float value) {
    return std::round(value);
}

float MathUtils::roundToFloat(float value, int decimalPlaces) {
    float multiplier = std::pow(10.0f, static_cast<float>(decimalPlaces));
    return std::round(value * multiplier) / multiplier;
}

float MathUtils::euclideanDistance(const std::vector<float>& a, const std::vector<float>& b) {
    if (a.size() != b.size()) {
        throw std::invalid_argument("Vectors must have same size for distance calculation");
    }
    
    float sum = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        float diff = a[i] - b[i];
        sum += diff * diff;
    }
    
    return std::sqrt(sum);
}

float MathUtils::manhattanDistance(const std::vector<float>& a, const std::vector<float>& b) {
    if (a.size() != b.size()) {
        throw std::invalid_argument("Vectors must have same size for distance calculation");
    }
    
    float sum = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        sum += std::abs(a[i] - b[i]);
    }
    
    return sum;
}

float MathUtils::cosineSimilarity(const std::vector<float>& a, const std::vector<float>& b) {
    if (a.size() != b.size()) {
        throw std::invalid_argument("Vectors must have same size for similarity calculation");
    }
    
    float dot = dotProduct(a, b);
    float normA = std::sqrt(dotProduct(a, a));
    float normB = std::sqrt(dotProduct(b, b));
    
    if (normA == 0.0f || normB == 0.0f) {
        return 0.0f;
    }
    
    return dot / (normA * normB);
}

std::vector<int> MathUtils::binData(const std::vector<float>& data, int numBins) {
    if (data.empty() || numBins <= 0) {
        return std::vector<int>();
    }
    
    auto minmax = std::minmax_element(data.begin(), data.end());
    float min = *minmax.first;
    float max = *minmax.second;
    
    if (max == min) {
        return std::vector<int>(data.size(), 0);
    }
    
    std::vector<int> bins(data.size());
    float binWidth = (max - min) / numBins;
    
    for (size_t i = 0; i < data.size(); ++i) {
        if (data[i] == max) {
            bins[i] = numBins - 1;
        } else {
            bins[i] = static_cast<int>((data[i] - min) / binWidth);
        }
    }
    
    return bins;
}

std::vector<float> MathUtils::createThresholds(float min, float max, int numThresholds) {
    if (numThresholds <= 1) {
        return std::vector<float>{min, max};
    }
    
    std::vector<float> thresholds;
    float step = (max - min) / (numThresholds - 1);
    
    for (int i = 0; i < numThresholds; ++i) {
        thresholds.push_back(min + i * step);
    }
    
    return thresholds;
}

// Explicit template instantiations for common types

} // namespace nlm
