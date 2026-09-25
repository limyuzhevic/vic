// Common utilities for NLM
// This file contains common helper functions and utilities
// that can be used across different components

#pragma once

#include <vector>
#include <cmath>
#include <algorithm>
#include <random>
#include <numeric>
#include <string>
#include <memory>

namespace nlm {

// Math utilities
class MathUtils {
public:
    // Clamp value between min and max
    static float clamp(float value, float min, float max) {
        return std::max(min, std::min(value, max));
    }
    
    // Linear interpolation
    static float lerp(float a, float b, float t) {
        return a + (b - a) * t;
    }
    
    // Sigmoid function
    static float sigmoid(float x) {
        return 1.0f / (1.0f + std::exp(-x));
    }
    
    // Softmax function (for multi-class probabilities)
    static std::vector<float> softmax(const std::vector<float>& inputs) {
        std::vector<float> result;
        float max = *std::max_element(inputs.begin(), inputs.end());
        float sum = 0.0f;
        
        for (float x : inputs) {
            result.push_back(std::exp(x - max));
            sum += result.back();
        }
        
        for (float& val : result) {
            val /= sum;
        }
        
        return result;
    }
    
    // Normalize vector
    static std::vector<float> normalize(const std::vector<float>& values) {
        float sum = std::accumulate(values.begin(), values.end(), 0.0f);
        if (sum == 0.0f) {
            return std::vector<float>(values.size(), 0.0f);
        }
        
        std::vector<float> normalized;
        for (float val : values) {
            normalized.push_back(val / sum);
        }
        
        return normalized;
    }
};

// Statistics utilities
class StatsUtils {
public:
    // Calculate mean
    static float mean(const std::vector<float>& values) {
        return std::accumulate(values.begin(), values.end(), 0.0f) / values.size();
    }
    
    // Calculate variance
    static float variance(const std::vector<float>& values, float mean) {
        float sum = 0.0f;
        for (float val : values) {
            sum += (val - mean) * (val - mean);
        }
        return sum / values.size();
    }
    
    // Calculate standard deviation
    static float stddev(const std::vector<float>& values, float mean) {
        return std::sqrt(variance(values, mean));
    }
    
    // Calculate median
    static float median(std::vector<float> values) {
        std::sort(values.begin(), values.end());
        size_t n = values.size();
        if (n % 2 == 0) {
            return (values[n/2 - 1] + values[n/2]) / 2.0f;
        } else {
            return values[n/2];
        }
    }
    
    // Calculate percentile
    static float percentile(const std::vector<float>& values, float percentile) {
        std::vector<float> sorted = values;
        std::sort(sorted.begin(), sorted.end());
        size_t index = static_cast<size_t>(percentile * (sorted.size() - 1));
        return sorted[index];
    }
};

// String utilities
class StringUtils {
public:
    // Convert float to string with precision
    static std::string toString(float value, int precision = 2) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(precision) << value;
        return oss.str();
    }
    
    // Format string with arguments
    static std::string format(const std::string& format, ...);
    
    // Check if string starts with prefix
    static bool startsWith(const std::string& str, const std::string& prefix) {
        return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
    }
    
    // Check if string ends with suffix
    static bool endsWith(const std::string& str, const std::string& suffix) {
        return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
    }
    
    // Split string by delimiter
    static std::vector<std::string> split(const std::string& str, char delimiter) {
        std::vector<std::string> result;
        std::string token;
        std::istringstream iss(str);
        
        while (std::getline(iss, token, delimiter)) {
            result.push_back(token);
        }
        
        return result;
    }
};

// Memory utilities
class MemoryUtils {
public:
    // Check if memory allocation is safe
    static bool isMemorySafe(size_t numElements, size_t elementSize, size_t maxMemory) {
        return numElements * elementSize <= maxMemory;
    }
    
    // Calculate memory usage in human-readable format
    static std::string formatMemorySize(size_t bytes) {
        const char* units[] = {"B", "KB", "MB", "GB", "TB"};
        int unitIndex = 0;
        double size = static_cast<double>(bytes);
        
        while (size >= 1024.0 && unitIndex < 4) {
            size /= 1024.0;
            unitIndex++;
        }
        
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << size << " " << units[unitIndex];
        return oss.str();
    }
};

// Random number generation utilities
class RandomUtils {
public:
    // Get random float between min and max
    static float randomFloat(float min, float max) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dist(min, max);
        return dist(gen);
    }
    
    // Get random integer between min and max
    static int randomInt(int min, int max) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dist(min, max);
        return dist(gen);
    }
    
    // Shuffle vector using Fisher-Yates algorithm
    template<typename T>
    static void shuffle(std::vector<T>& vec) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        for (int i = vec.size() - 1; i > 0; --i) {
            int j = std::uniform_int_distribution<int>(0, i)(gen);
            std::swap(vec[i], vec[j]);
        }
    }
};

// Time utilities
class TimeUtils {
public:
    // Get current time in milliseconds since epoch
    static long long getCurrentTimeMs() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
    }
    
    // Get current time in seconds since epoch
    static double getCurrentTimeSec() {
        return std::chrono::duration_cast<std::chrono::duration<double>>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
    }
    
    // Convert seconds to human-readable format
    static std::string formatTime(double seconds) {
        int hours = static_cast<int>(seconds / 3600);
        int minutes = static_cast<int>((seconds / 60) % 60);
        int secs = static_cast<int>(seconds % 60);
        
        std::ostringstream oss;
        oss << std::setfill('0') << std::setw(2) << hours << ":"
            << std::setw(2) << minutes << ":"
            << std::setw(2) << secs;
        return oss.str();
    }
};

} // namespace nlm
