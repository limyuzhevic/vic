# Common utility functions for NLM
// Reusable utility functions and patterns used throughout NLM

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <memory>
#include <algorithm>
#include <cmath>
#include <optional>

namespace nlm {
namespace utils {

// String utilities
class StringUtils {
public:
    // Trim whitespace from both ends
    static std::string trim(const std::string& str);
    
    // Convert string to lowercase
    static std::string toLower(const std::string& str);
    
    // Convert string to uppercase
    static std::string toUpper(const std::string& str);
    
    // Check if string contains only alphanumeric characters and underscores
    static bool isAlphanumeric(const std::string& str);
    
    // Split string by delimiter
    static std::vector<std::string> split(const std::string& str, char delimiter);
    
    // Join vector of strings with delimiter
    static std::string join(const std::vector<std::string>& strings, char delimiter);
    
    // Check if string starts with prefix
    static bool startsWith(const std::string& str, const std::string& prefix);
    
    // Check if string ends with suffix
    static bool endsWith(const std::string& str, const std::string& suffix);
    
    // Replace all occurrences of substring
    static std::string replaceAll(const std::string& str, const std::string& from, const std::string& to);
};

// Configuration utilities
class ConfigUtils {
public:
    // Validate configuration key format
    static bool isValidKey(const std::string& key);
    
    // Parse key=value string
    static std::pair<std::string, std::string> parseKeyValue(const std::string& line);
    
    // Format configuration value for output
    template<typename T>
    static std::string formatValue(const T& value);
    
    // Parse configuration value from string
    template<typename T>
    static std::optional<T> parseValue(const std::string& str);
    
    // Validate numeric range
    static bool inRange(double value, double min, double max);
    
    // Validate string pattern
    static bool matchesPattern(const std::string& value, const std::string& pattern);
};

// Performance timing utilities
class TimingUtils {
public:
    // Benchmark timer utility
    class BenchmarkTimer {
    public:
        BenchmarkTimer();
        double elapsed_ms() const;
        void reset();
        
    private:
        std::chrono::high_resolution_clock::time_point start_time;
    };
    
    // Average timing over multiple runs
    template<typename Func, typename... Args>
    static double average_time(Func&& func, Args&&... args, size_t iterations = 100);
    
    // Measure execution time of function
    template<typename Func, typename... Args>
    static double measure_time(Func&& func, Args&&... args);
    
    // Format time duration for display
    static std::string formatDuration(double milliseconds);
};

// Memory management utilities
class MemoryUtils {
public:
    // Safe string assignment
    static void safeAssign(std::string& target, const std::string& source, size_t max_length = 1024);
    
    // Clear vector safely
    template<typename T>
    static void safeClear(std::vector<T>& vec);
    
    // Get safe index (clamp to valid range)
    static size_t safeIndex(size_t index, size_t size);
    
    // Memory-efficient string joining
    static std::string efficientJoin(const std::vector<std::string>& strings, const std::string& delimiter);
};

// Error handling utilities
class ErrorUtils {
public:
    // Create error message with context
    static std::string createError(const std::string& operation, const std::string& message);
    
    // Check if error should be fatal
    static bool isFatalError(const std::string& error);
    
    // Log error with stack trace (simplified)
    static void logError(const std::string& error);
    
    // Get user-friendly error message
    static std::string getUserFriendlyError(const std::string& error);
};

// Mathematical utilities
class MathUtils {
public:
    // Clamp value to range
    static double clamp(double value, double min, double max);
    
    // Linear interpolation
    static double lerp(double a, double b, double t);
    
    // Map value from one range to another
    static double mapRange(double value, double in_min, double in_max, double out_min, double out_max);
    
    // Normalize value to [0, 1] range
    static double normalize(double value, double min, double max);
    
    // Calculate mean of vector
    static double mean(const std::vector<double>& values);
    
    // Calculate standard deviation
    static double standardDeviation(const std::vector<double>& values, double mean);
    
    // Check if value is approximately equal to another
    static bool approximatelyEqual(double a, double b, double epsilon = 1e-6);
    
    // Round to nearest multiple
    static double roundToNearest(double value, double multiple);
};

// Template implementations
namespace {

// StringUtils implementations
template<typename T>
std::string ConfigUtils::formatValue(const T& value) {
    return std::to_string(value);
}

std::string ConfigUtils::formatValue(const std::string& value) {
    return "\"" + value + "\"";
}

std::string ConfigUtils::formatValue(const bool& value) {
    return value ? "true" : "false";
}

// TimingUtils implementations
template<typename Func, typename... Args>
double TimingUtils::average_time(Func&& func, Args&&... args, size_t iterations) {
    std::vector<double> times;
    times.reserve(iterations);
    
    for (size_t i = 0; i < iterations; ++i) {
        times.push_back(measure_time(func, std::forward<Args>(args)...));
    }
    
    return mean(times);
}

template<typename Func, typename... Args>
double TimingUtils::measure_time(Func&& func, Args&&... args) {
    BenchmarkTimer timer;
    func(std::forward<Args>(args)...);
    return timer.elapsed_ms();
}

// MemoryUtils implementations
template<typename T>
void MemoryUtils::safeClear(std::vector<T>& vec) {
    std::vector<T>().swap(vec);
}

} // namespace anonymous

} // namespace utils
} // namespace nlm
