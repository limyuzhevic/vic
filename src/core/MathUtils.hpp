#pragma once

#include <algorithm>
#include <cmath>
#include <limits>
#include <type_traits>
#include <string>
#include <vector>

namespace nlm {

// Core mathematical utilities for neural simulation
class MathUtils {
public:
    // Safely clamp value to [min, max] with nan checking
    template<typename T>
    static T clamp(T value, T min, T max) {
        if (std::is_floating_point_v<T>) {
            if (std::isnan(value)) return min;
            if (std::isnan(min) || std::isnan(max)) return value;
        }
        return std::max(min, std::min(value, max));
    }
    
    // Linear interpolation
    template<typename T>
    static T lerp(T a, T b, T t) {
        return a + (b - a) * t;
    }
    
    // Exponential moving average
    static float exponentialMovingAverage(float current, float previous, float alpha) {
        return previous * (1.0f - alpha) + current * alpha;
    }
    
    // Sigmoid function for activation
    static float sigmoid(float x) {
        return 1.0f / (1.0f + std::exp(-std::clamp(x, -50.0f, 50.0f)));
    }
    
    // ReLU function
    static float relu(float x) {
        return std::max(0.0f, x);
    }
    
    // Safe division preventing division by zero
    template<typename T>
    static T safeDivide(T numerator, T denominator, T defaultValue = T(0)) {
        return denominator != T(0) ? numerator / denominator : defaultValue;
    }
    
    // Mean of vector
    template<typename T>
    static T mean(const std::vector<T>& values) {
        if (values.empty()) return T(0);
        T sum = T(0);
        for (const auto& v : values) sum += v;
        return sum / values.size();
    }
    
    // Variance of vector
    template<typename T>
    static T variance(const std::vector<T>& values) {
        if (values.size() < 2) return T(0);
        T m = mean(values);
        T sum = T(0);
        for (const auto& v : values) sum += (v - m) * (v - m);
        return sum / (values.size() - 1);
    }
    
    // Standard deviation
    template<typename T>
    static T stddev(const std::vector<T>& values) {
        return std::sqrt(variance(values));
    }
    
    // Check if value is approximately equal to another
    template<typename T>
    static bool approxEqual(T a, T b, T epsilon = std::numeric_limits<T>::epsilon() * 100) {
        return std::abs(a - b) <= epsilon;
    }
    
    // Smoothstep interpolation (cubic hermite)
    static float smoothstep(float edge0, float edge1, float x) {
        float t = safeDivide(x - edge0, edge1 - edge0, 0.5f);
        t = clamp(t, 0.0f, 1.0f);
        return t * t * (3.0f - 2.0f * t);
    }
    
    // Check for NaN or infinity
    template<typename T>
    static bool isValid(T value) {
        if constexpr (std::is_floating_point_v<T>) {
            return !std::isnan(value) && !std::isinf(value);
        }
        return true;
    }
};

// Standard Pimpl (Pointer to Implementation) template for clean interface/implementation separation
class PimplBase {
protected:
    struct ImplBase {
        virtual ~ImplBase() = default;
        virtual void* get() { return nullptr; }
        virtual const void* get() const { return nullptr; }
    };
    
    template<typename T>
    struct ConcreteImpl : public ImplBase {
        T data;
        ConcreteImpl() : data() {}
        ConcreteImpl(const T& d) : data(d) {}
        void* get() override { return &data; }
        const void* get() const override { return &data; }
    };
    
    template<typename T>
    std::unique_ptr<ImplBase> createImpl() {
        return std::make_unique<ConcreteImpl<T>>();
    }
    
    template<typename T, typename... Args>
    std::unique_ptr<ImplBase> createImpl(Args&&... args) {
        return std::make_unique<ConcreteImpl<T>>(std::forward<Args>(args)...);
    }
    
    template<typename T>
    T* getImpl() {
        auto* impl = static_cast<ConcreteImpl<T>*>(pImpl.get());
        return impl ? &impl->data : nullptr;
    }
    
    template<typename T>
    const T* getImpl() const {
        auto* impl = static_cast<const ConcreteImpl<T>*>(pImpl.get());
        return impl ? &impl->data : nullptr;
    }
    
    template<typename T>
    T& getImplRef() {
        auto* impl = static_cast<ConcreteImpl<T>*>(pImpl.get());
        static T defaultObj;
        return impl ? impl->data : defaultObj;
    }
    
    template<typename T>
    const T& getImplRef() const {
        auto* impl = static_cast<const ConcreteImpl<T>*>(pImpl.get());
        static const T defaultObj;
        return impl ? impl->data : defaultObj;
    }
    
    std::unique_ptr<ImplBase> pImpl;
};

// Standardized logging utilities
class LoggerUtils {
public:
    // Thread-safe log level
    enum class LogLevel {
        Debug = 0,
        Info = 1,
        Warning = 2,
        Error = 3,
        Critical = 4
    };
    
    // Formatted log message
    struct LogEntry {
        LogLevel level;
        std::string message;
        std::string timestamp;
        std::string threadId;
    };
    
    // Simple timestamp string
    static std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S")
           << "." << std::setfill('0') << std::setw(3) << ms.count();
        return ss.str();
    }
    
    // Get thread ID as string
    static std::string getThreadId() {
        std::stringstream ss;
        ss << std::this_thread::get_id();
        return ss.str();
    }
    
    // Format log message with level
    static std::string formatMessage(LogLevel level, const std::string& message) {
        static const char* levelNames[] = {"DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL"};
        static const char* levelColors[] = {"\033[36m", "\033[32m", "\033[33m", "\033[31m", "\033[35m"};
        static const char* resetColor = "\033[0m";
        
        int idx = static_cast<int>(level);
        if (idx >= 0 && idx < 5) {
            return std::string(levelColors[idx]) + "[" + levelNames[idx] + "] " + message + resetColor;
        }
        return "[" + message + "]";
    }
};

// Performance profiling utilities
class ProfilerUtils {
public:
    // Simple timing wrapper
    class Timer {
    public:
        Timer(const std::string& name) : name(name), start(std::chrono::high_resolution_clock::now()) {}
        ~Timer() {
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            std::cout << "[" << name << "] Execution time: " << duration.count() << " microseconds" << std::endl;
        }
    
    private:
        std::string name;
        std::chrono::high_resolution_clock::time_point start;
    };
    
    // Simple profiler for tracking time
    class Profiler {
    public:
        struct ProfileData {
            std::string name;
            uint64_t calls{0};
            uint64_t totalTime{0}; // microseconds
            uint64_t minTime{std::numeric_limits<uint64_t>::max()};
            uint64_t maxTime{0};
        };
        
        static Profiler& getInstance() {
            static Profiler instance;
            return instance;
        }
        
        void startProfile(const std::string& name) {
            currentProfile = name;
            startTime = std::chrono::high_resolution_clock::now();
        }
        
        void endProfile() {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
            uint64_t micros = duration.count();
            
            auto it = std::find_if(profiles.begin(), profiles.end(),
                [&](const ProfileData& p) { return p.name == currentProfile; });
            
            if (it == profiles.end()) {
                profiles.push_back({currentProfile, 1, micros, micros, micros});
            } else {
                it->calls++;
                it->totalTime += micros;
                it->minTime = std::min(it->minTime, micros);
                it->maxTime = std::max(it->maxTime, micros);
            }
            currentProfile.clear();
        }
        
        void printReport() const {
            std::cout << "\n=== Performance Profile Report ===" << std::endl;
            std::cout << "Total profiled operations: " << profiles.size() << std::endl;
            std::cout << std::endl;
            std::cout << std::left << std::setw(30) << "Operation" 
                      << std::right << std::setw(10) << "Calls"
                      << std::right << std::setw(15) << "Total (µs)"
                      << std::right << std::setw(15) << "Avg (µs)"
                      << std::right << std::setw(15) << "Min (µs)"
                      << std::right << std::setw(15) << "Max (µs)" << std::endl;
            std::cout << std::string(90, '-') << std::endl;
            
            for (const auto& profile : profiles) {
                uint64_t avg = profile.calls > 0 ? profile.totalTime / profile.calls : 0;
                std::cout << std::left << std::setw(30) << profile.name
                          << std::right << std::setw(10) << profile.calls
                          << std::right << std::setw(15) << profile.totalTime
                          << std::right << std::setw(15) << avg
                          << std::right << std::setw(15) << profile.minTime
                          << std::right << std::setw(15) << profile.maxTime << std::endl;
            }
            std::cout << std::string(90, '-') << std::endl;
        }
        
        void clear() {
            profiles.clear();
        }
        
    private:
        Profiler() = default;
        std::string currentProfile;
        std::chrono::high_resolution_clock::time_point startTime;
        std::vector<ProfileData> profiles;
    };
};

// Memory tracking utilities
class MemoryUtils {
public:
    // Get current memory usage in bytes (platform-specific)
    static size_t getCurrentMemoryUsage();
    
    // Get peak memory usage in bytes (platform-specific)
    static size_t getPeakMemoryUsage();
    
    // Simple memory leak detector (basic implementation)
    class LeakDetector {
    public:
        LeakDetector(const std::string& name) : name(name) {
            allocations[name]++;
        }
        
        ~LeakDetector() {
            if (allocations[name] > 0) {
                allocations[name]--;
            }
        }
        
        static void reportLeaks() {
            std::cout << "\n=== Memory Leak Report ===" << std::endl;
            bool hasLeaks = false;
            for (const auto& pair : allocations) {
                if (pair.second > 0) {
                    std::cout << "LEAK: " << pair.first << " - " << pair.second << " objects" << std::endl;
                    hasLeaks = true;
                }
            }
            if (!hasLeaks) {
                std::cout << "No memory leaks detected." << std::endl;
            }
            std::cout << "======================" << std::endl;
        }
        
    private:
        std::string name;
        static std::unordered_map<std::string, size_t> allocations;
    };
    
private:
    static std::unordered_map<std::string, size_t> LeakDetector::allocations;
};

} // namespace nlm
