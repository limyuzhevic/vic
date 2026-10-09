// Utility functions for NLM (Neural Learning Machine)
// Common helper functions used throughout the codebase

#pragma once

#include <cmath>
#include <algorithm>
#include <string>
#include <vector>
#include <random>
#include <chrono>
#include <thread>

namespace nlm {

// Mathematical utility functions
inline constexpr float PI = 3.141592653589793f;
inline constexpr float E = 2.718281828459045f;

// Clamp a value between min and max
inline float clamp(float value, float min, float max) {
    return std::max(min, std::min(value, max));
}

// Linear interpolation between two values
inline float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

// Exponential moving average
inline float ema(float current, float previous, float alpha) {
    return current * alpha + previous * (1.0f - alpha);
}

// Smoothstep function (Hermite interpolation)
inline float smoothstep(float edge0, float edge1, float x) {
    float t = std::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

// Normalizes a value to range [0, 1]
inline float normalize(float value, float min, float max) {
    if (max == min) return 0.0f;
    return (value - min) / (max - min);
}

// Denormalizes a value from range [0, 1] to [min, max]
inline float denormalize(float normalized, float min, float max) {
    return min + normalized * (max - min);
}

// Distance calculations
inline float distance(float x1, float y1, float x2, float y2) {
    return std::sqrt(std::pow(x2 - x1, 2) + std::pow(y2 - y1, 2));
}

// Time utilities
inline Timestamp getCurrentTimeStamp() {
    using clock = std::chrono::system_clock;
    auto now = clock::now();
    auto duration = now.time_since_epoch();
    return static_cast<Timestamp>(duration.count());
}

// String utilities
inline std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), 
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

inline std::string toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), 
                   [](unsigned char c) { return std::toupper(c); });
    return result;
}

inline bool startsWith(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}

inline bool endsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

inline std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

inline std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

// Random number utilities
inline float randomFloat(std::mt19937& rng) {
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    return dist(rng);
}

inline float randomFloat(std::mt19937& rng, float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(rng);
}

inline int randomInt(std::mt19937& rng, int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}

inline bool randomBernoulli(std::mt19937& rng, float probability) {
    std::bernoulli_distribution dist(probability);
    return dist(rng);
}

inline float randomNormal(std::mt19937& rng, float mean, float stddev) {
    std::normal_distribution<float> dist(mean, stddev);
    return dist(rng);
}

// Vector utilities
inline float vectorMagnitude(const std::vector<float>& vec) {
    float sum = 0.0f;
    for (float v : vec) {
        sum += v * v;
    }
    return std::sqrt(sum);
}

inline float vectorDot(const std::vector<float>& a, const std::vector<float>& b) {
    float result = 0.0f;
    size_t size = std::min(a.size(), b.size());
    for (size_t i = 0; i < size; ++i) {
        result += a[i] * b[i];
    }
    return result;
}

inline std::vector<float> vectorNormalize(const std::vector<float>& vec) {
    float mag = vectorMagnitude(vec);
    if (mag == 0.0f) return vec;
    std::vector<float> result = vec;
    for (float& v : result) {
        v /= mag;
    }
    return result;
}

// Array utilities
inline float arraySum(const float* array, size_t size) {
    float sum = 0.0f;
    for (size_t i = 0; i < size; ++i) {
        sum += array[i];
    }
    return sum;
}

inline float arrayMean(const float* array, size_t size) {
    if (size == 0) return 0.0f;
    return arraySum(array, size) / size;
}

inline float arrayVariance(const float* array, size_t size) {
    if (size < 2) return 0.0f;
    
    float mean = arrayMean(array, size);
    float sumSq = 0.0f;
    for (size_t i = 0; i < size; ++i) {
        float diff = array[i] - mean;
        sumSq += diff * diff;
    }
    return sumSq / (size - 1);
}

// Simulation timing utilities
inline bool shouldExecuteEveryNSteps(size_t currentStep, size_t interval) {
    return currentStep % interval == 0;
}

inline bool shouldExecuteEveryNMilliseconds(size_t currentStep, float timestep, size_t msInterval) {
    float intervalInSteps = msInterval / (timestep * 1000.0f);
    return shouldExecuteEveryNSteps(currentStep, static_cast<size_t>(intervalInSteps));
}

// Debugging utilities
inline std::string debugFormat(const std::string& format, ...) {
    va_list args;
    va_start(args, format);
    
    // Simple implementation - in production, use snprintf or similar
    std::string result;
    char buffer[256];
    vsnprintf(buffer, sizeof(buffer), format.c_str(), args);
    va_end(args);
    
    result = std::string(buffer);
    return result;
}

// Performance utilities
inline double getCurrentTime() {
    using clock = std::chrono::high_resolution_clock;
    auto now = clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration<double>(duration).count();
}

inline double elapsedTime(double startTime) {
    return getCurrentTime() - startTime;
}

// Type conversion utilities
inline NeuronId sizeToNeuronId(size_t value) {
    return static_cast<NeuronId>(value + 1);
}

inline PopulationId sizeToPopulationId(size_t value) {
    return static_cast<PopulationId>(value + 1);
}

inline RegionId sizeToRegionId(size_t value) {
    return static_cast<RegionId>(value + 1);
}

// Validation utilities
inline bool isValidNeuronId(NeuronId id) {
    return id != INVALID_NEURON_ID;
}

inline bool isValidPopulationId(PopulationId id) {
    return id != INVALID_POPULATION_ID;
}

inline bool isValidRegionId(RegionId id) {
    return id != INVALID_REGION_ID;
}

// Memory utilities
inline size_t calculateMemoryUsage(size_t neurons, size_t synapses) {
    // Rough estimates:
    // - Neuron: ~100 bytes
    // - Synapse: ~80 bytes
    return neurons * 100 + synapses * 80;
}

// Thread utilities
inline void sleepFor(double seconds) {
    std::this_thread::sleep_for(std::chrono::duration<double>(seconds));
}

} // namespace nlm