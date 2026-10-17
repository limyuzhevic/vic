#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <memory>
#include <functional>
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

// Logger Helper - Common logging patterns
class LoggerHelper {
public:
    // Standardized log levels
    enum class LogLevel {
        DEBUG = 0,
        INFO = 1,
        WARNING = 2,
        ERROR = 3,
        CRITICAL = 4
    };

    // Standardized log format
    struct LogEntry {
        LogLevel level;
        std::string message;
        std::string file;
        int line;
        std::string function;
        std::chrono::system_clock::time_point timestamp;
        
        LogEntry(LogLevel lvl, const std::string& msg, 
                 const std::string& file = "", int line = 0,
                 const std::string& function = "")
            : level(lvl), message(msg), file(file), line(line), 
              function(function), timestamp(std::chrono::system_clock::now()) {}
    };

    // Helper for standardized logging
    static void logMessage(LogLevel level, const std::string& message,
                          const std::string& file = "", int line = 0,
                          const std::string& function = "");

    // Convenience methods
    static void debug(const std::string& message, const std::string& file = "", 
                     int line = 0, const std::string& function = "") {
        logMessage(LogLevel::DEBUG, message, file, line, function);
    }

    static void info(const std::string& message, const std::string& file = "", 
                    int line = 0, const std::string& function = "") {
        logMessage(LogLevel::INFO, message, file, line, function);
    }

    static void warning(const std::string& message, const std::string& file = "", 
                       int line = 0, const std::string& function = "") {
        logMessage(LogLevel::WARNING, message, file, line, function);
    }

    static void error(const std::string& message, const std::string& file = "", 
                     int line = 0, const std::string& function = "") {
        logMessage(LogLevel::ERROR, message, file, line, function);
    }

    static void critical(const std::string& message, const std::string& file = "", 
                        int line = 0, const std::string& function = "") {
        logMessage(LogLevel::CRITICAL, message, file, line, function);
    }

    // Log level checking
    static bool isEnabled(LogLevel level);

    // Format log entry for output
    static std::string formatLogEntry(const LogEntry& entry);
    
    // Format duration for output
    static std::string formatDuration(const std::chrono::steady_clock::duration& duration);
    
    // Get current timestamp string
    static std::string getCurrentTimestamp();

private:
    static std::mutex logMutex_;
};

// Memory Utils - Memory management and pool utilities
class MemoryUtils {
public:
    // Memory statistics
    struct MemoryStats {
        size_t totalAllocated;
        size_t currentUsage;
        size_t peakUsage;
        std::vector<size_t> allocationsBySize;
        double utilizationRatio;
    };

    // Memory allocation tracking
    template<typename T>
    static T* trackedAllocate(const char* file, int line, size_t count = 1);

    template<typename T>
    static void trackedDeallocate(T* ptr, const char* file, int line);

    // Memory pool implementation
    class MemoryPool {
    public:
        explicit MemoryPool(size_t blockSize, size_t initialCapacity = 0);
        ~MemoryPool();
        
        void* allocate();
        void deallocate(void* ptr);
        bool isEmpty() const;
        size_t size() const;
        void clear();
        
        // Disable copying
        MemoryPool(const MemoryPool&) = delete;
        MemoryPool& operator=(const MemoryPool&) = delete;

    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };

    // Utility functions
    static size_t alignSize(size_t size, size_t alignment);
    static double calculateMemoryUtilization(size_t allocated, size_t total);
    static std::string formatMemorySize(size_t bytes);
    static MemoryStats getCurrentMemoryStats();
    
    // Smart pointer utilities
    template<typename T>
    static void trackSmartPointer(const std::shared_ptr<T>& ptr);

private:
    static std::mutex statsMutex_;
};

// Timer Utils - Timing and benchmarking utilities
class TimerUtils {
public:
    // High-resolution timer
    class Timer {
    public:
        Timer();
        ~Timer();
        
        void start();
        void stop();
        void reset();
        
        // Get elapsed time in various units
        double elapsedSeconds() const;
        double elapsedMilliseconds() const;
        double elapsedMicroseconds() const;
        std::chrono::milliseconds elapsedMs() const;
        std::chrono::microseconds elapsedUs() const;
        
        // Check if timer is running
        bool isRunning() const;
        
        // Start-stop wrapper for single-shot timing
        double measure(std::function<void()> func);

    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };

    // Benchmark utilities
    template<typename Func, typename... Args>
    static double benchmark(Func&& func, Args&&... args);

    static std::string formatTime(double seconds);
    static std::string formatTime(const std::chrono::steady_clock::duration& duration);
    
    // Scope timing for automatic logging
    class ScopedTimer {
    public:
        ScopedTimer(const std::string& label, LogLevel level = LogLevel::INFO);
        ~ScopedTimer();
        
        double elapsed() const;

    private:
        std::string label_;
        LogLevel level_;
        Timer timer_;
    };

private:
    enum class LogLevel {
        DEBUG, INFO, WARNING, ERROR, CRITICAL
    };
};

// Math Utils - Common mathematical operations
class MathUtils {
public:
    // Statistical operations
    template<typename T>
    static T mean(const std::vector<T>& values);
    
    template<typename T>
    static T median(std::vector<T> values);
    
    template<typename T>
    static T standardDeviation(const std::vector<T>& values, T mean);
    
    template<typename T>
    static T variance(const std::vector<T>& values, T mean);
    
    // Array operations
    template<typename T>
    static T sum(const std::vector<T>& values);
    
    template<typename T>
    static T product(const std::vector<T>& values);
    
    template<typename T>
    static T min(const std::vector<T>& values);
    
    template<typename T>
    static T max(const std::vector<T>& values);
    
    template<typename T>
    static std::vector<T> clamp(const std::vector<T>& values, T min, T max);
    
    // Linear algebra
    template<typename T>
    static T dotProduct(const std::vector<T>& a, const std::vector<T>& b);
    
    template<typename T>
    static std::vector<T> normalize(const std::vector<T>& vector, T length = 1.0);
    
    static float sigmoid(float x);
    static float relu(float x);
    static float tanh(float x);
    
    // Probability and randomness
    template<typename T>
    static T interpolate(T start, T end, float alpha);
    
    static float randomFloat(float min = 0.0f, float max = 1.0f);
    static int randomInt(int min, int max);
    
    // Rounding and precision
    static int roundToInt(float value);
    static float roundToFloat(float value, int decimalPlaces);
    
    // Distance calculations
    static float euclideanDistance(const std::vector<float>& a, const std::vector<float>& b);
    static float manhattanDistance(const std::vector<float>& a, const std::vector<float>& b);
    static float cosineSimilarity(const std::vector<float>& a, const std::vector<float>& b);
    
    // Threshold and binning
    static std::vector<int> binData(const std::vector<float>& data, int numBins);
    static std::vector<float> createThresholds(float min, float max, int numThresholds);

private:
    static std::mt19937 randomGenerator_;
};

} // namespace nlm
