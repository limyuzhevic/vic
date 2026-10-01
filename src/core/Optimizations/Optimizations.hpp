// NLM String Utilities and Performance Optimizations
// Provides optimized string operations and logging utilities for performance

#ifndef NLM_OPTIMIZATIONS_HPP
#define NLM_OPTIMIZATIONS_HPP

#include <string>
#include <sstream>
#include <vector>
#include <array>
#include <numeric>
#include <format>
#include <chrono>

namespace nlm {
namespace optimizations {

// Optimized string building utilities
class StringBuilder {
private:
    std::string result;
    std::vector<std::string> parts;
    
public:
    StringBuilder& append(const std::string& str) {
        parts.push_back(str);
        return *this;
    }
    
    StringBuilder& append(const char* str) {
        parts.push_back(str);
        return *this;
    }
    
    StringBuilder& append(int value) {
        parts.push_back(std::to_string(value));
        return *this;
    }
    
    StringBuilder& append(float value) {
        std::ostringstream oss;
        oss << value;
        parts.push_back(oss.str());
        return *this;
    }
    
    StringBuilder& append(double value) {
        std::ostringstream oss;
        oss << value;
        parts.push_back(oss.str());
        return *this;
    }
    
    std::string build() {
        size_t totalSize = 0;
        for (const auto& part : parts) {
            totalSize += part.size();
        }
        
        result.clear();
        result.reserve(totalSize);
        
        for (const auto& part : parts) {
            result += part;
        }
        
        parts.clear();
        return result;
    }
    
    void clear() {
        result.clear();
        parts.clear();
    }
};

// Optimized statistical computations
class StatsCalculator {
public:
    static float calculateMean(const std::vector<float>& values) {
        if (values.empty()) return 0.0f;
        float sum = std::accumulate(values.begin(), values.end(), 0.0f);
        return sum / values.size();
    }
    
    static float calculateVariance(const std::vector<float>& values, float mean) {
        if (values.size() < 2) return 0.0f;
        
        float sum = 0.0f;
        for (float val : values) {
            float diff = val - mean;
            sum += diff * diff;
        }
        return sum / (values.size() - 1);
    }
    
    static float calculateStdDev(const std::vector<float>& values) {
        if (values.empty()) return 0.0f;
        float mean = calculateMean(values);
        float variance = calculateVariance(values, mean);
        return std::sqrt(variance);
    }
    
    // High-performance one-pass variance calculation
    static float calculateVarianceOnePass(const std::vector<float>& values) {
        if (values.size() < 2) return 0.0f;
        
        float mean = 0.0f, M2 = 0.0f, delta;
        for (size_t i = 0; i < values.size(); ++i) {
            delta = values[i] - mean;
            mean += delta / (i + 1);
            M2 += delta * (values[i] - mean);
        }
        return M2 / (values.size() - 1);
    }
};

// Optimized logging with conditional compilation
class LoggingOptimizer {
private:
    static bool shouldLog(LogLevel level) {
        return static_cast<int>(level) >= static_cast<int>(LogLevel::Info);
    }
    
public:
    static void logWithOptimization(const std::string& message, LogLevel level = LogLevel::Info) {
        if (shouldLog(level)) {
            NLM_LOG_INFO(message);
        }
    }
    
    static void logFormatted(const std::string& format, LogLevel level = LogLevel::Info) {
        if (shouldLog(level)) {
            // Use std::format if available (C++20), otherwise fallback
#ifdef __cpp_lib_format
            NLM_LOG_INFO(std::format(format));
#else
            NLM_LOG_INFO(format);
#endif
        }
    }
};

// Memory pool for small object allocation
class ObjectPool {
private:
    std::vector<float> floatPool;
    std::vector<int> intPool;
    size_t floatPos;
    size_t intPos;
    
public:
    ObjectPool(size_t floatCapacity = 1024, size_t intCapacity = 1024) {
        floatPool.resize(floatCapacity, 0.0f);
        intPool.resize(intCapacity, 0);
        floatPos = 0;
        intPos = 0;
    }
    
    float* allocateFloat() {
        if (floatPos < floatPool.size()) {
            return &floatPool[floatPos++];
        }
        return nullptr;
    }
    
    int* allocateInt() {
        if (intPos < intPool.size()) {
            return &intPool[intPos++];
        }
        return nullptr;
    }
    
    void reset() {
        floatPos = 0;
        intPos = 0;
    }
};

// Timer for performance measurement
class PerformanceTimer {
private:
    std::chrono::high_resolution_clock::time_point startTime;
    
public:
    void start() {
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    double elapsedMs() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        return static_cast<double>(duration.count());
    }
    
    double elapsedSeconds() {
        return elapsedMs() / 1000.0;
    }
};

// Batch processing utilities
class BatchProcessor {
public:
    template<typename T, typename Func>
    static void processInBatches(const std::vector<T>& data, Func processor, size_t batchSize = 1000) {
        for (size_t i = 0; i < data.size(); i += batchSize) {
            size_t end = std::min(i + batchSize, data.size());
            for (size_t j = i; j < end; ++j) {
                processor(data[j]);
            }
        }
    }
    
    template<typename T, typename Result, typename Func>
    static std::vector<Result> transformInBatches(const std::vector<T>& input, Func transformer, size_t batchSize = 1000) {
        std::vector<Result> results;
        results.reserve(input.size());
        
        for (size_t i = 0; i < input.size(); i += batchSize) {
            size_t end = std::min(i + batchSize, input.size());
            for (size_t j = i; j < end; ++j) {
                results.push_back(transformer(input[j]));
            }
        }
        
        return results;
    }
};

} // namespace optimizations

} // namespace nlm

#endif // NLM_OPTIMIZATIONS_HPP