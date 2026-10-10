#pragma once

#include <string>
#include <chrono>
#include <unordered_map>
#include <vector>
#include <memory>
#include <sstream>
#include <mutex>
#include "../Logger/Logger.hpp"

namespace nlm {

// Performance measurement units
enum class PerformanceUnit {
    Seconds,
    Milliseconds,
    Microseconds,
    Nanoseconds,
    Count,
    Bytes,
    Percentage,
    Hertz,
    Float
};

// Performance counter for tracking metrics
struct PerformanceCounter {
    std::string name;
    double value;
    double previousValue;
    PerformanceUnit unit;
    std::chrono::system_clock::time_point lastUpdate;
    bool isRate;
    
    PerformanceCounter(const std::string& n, double v, PerformanceUnit u = PerformanceUnit::Count, bool rate = false)
        : name(n), value(v), previousValue(v), unit(u), isRate(rate) {
        lastUpdate = std::chrono::system_clock::now();
    }
    
    // Calculate rate if this is a rate counter
    double getRate() const {
        if (!isRate) return 0.0;
        
        auto now = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdate).count();
        if (duration <= 0) return 0.0;
        
        double delta = value - previousValue;
        previousValue = value;
        lastUpdate = now;
        
        return delta / (duration / 1000.0);  // Convert to per second
    }
    
    // Update value
    void update(double newValue) {
        value = newValue;
        lastUpdate = std::chrono::system_clock::now();
    }
};

// Simple timer for measuring operation duration
class Timer {
public:
    Timer(const std::string& name, PerformanceUnit unit = PerformanceUnit::Milliseconds)
        : name(name), unit(unit), startTime(std::chrono::high_resolution_clock::now()),
          isRunning(true), accumulatedTime(0.0) {}
    
    ~Timer() {
        if (isRunning) {
            stop();
        }
    }
    
    // Start the timer
    void start() {
        startTime = std::chrono::high_resolution_clock::now();
        isRunning = true;
    }
    
    // Stop the timer and return elapsed time
    double stop() {
        if (!isRunning) return accumulatedTime;
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);
        
        accumulatedTime += duration.count();
        isRunning = false;
        
        return accumulatedTime;
    }
    
    // Get current elapsed time without stopping
    double elapsed() const {
        if (!isRunning) return accumulatedTime;
        
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - startTime);
        
        return accumulatedTime + duration.count();
    }
    
    // Reset timer
    void reset() {
        accumulatedTime = 0.0;
        isRunning = false;
    }
    
    // Get timer name
    const std::string& getName() const { return name; }
    
    // Get unit string
    static std::string unitToString(PerformanceUnit unit) {
        switch (unit) {
            case PerformanceUnit::Seconds: return "s";
            case PerformanceUnit::Milliseconds: return "ms";
            case PerformanceUnit::Microseconds: return "\u03BCs";
            case PerformanceUnit::Nanoseconds: return "ns";
            case PerformanceUnit::Count: return "count";
            case PerformanceUnit::Bytes: return "bytes";
            case PerformanceUnit::Percentage: return "%";
            case PerformanceUnit::Hertz: return "Hz";
            case PerformanceUnit::Float: return "float";
            default: return "";
        }
    }

private:
    std::string name;
    PerformanceUnit unit;
    std::chrono::high_resolution_clock::time_point startTime;
    bool isRunning;
    double accumulatedTime;
};

// Global performance monitor for collecting and managing performance metrics
class PerformanceMonitor {
public:
    static PerformanceMonitor& getInstance() {
        static PerformanceMonitor instance;
        return instance;
    }
    
    // Start timing an operation
    Timer* startTimer(const std::string& name, PerformanceUnit unit = PerformanceUnit::Milliseconds) {
        std::lock_guard<std::mutex> lock(mutex);
        
        timers.push_back(std::make_unique<Timer>(name, unit));
        Timer* timer = timers.back().get();
        timer->start();
        
        NLM_LOG_DEBUG("PerformanceMonitor: Started timer " + name);
        return timer;
    }
    
    // Stop a specific timer and get elapsed time
    double stopTimer(Timer* timer) {
        std::lock_guard<std::mutex> lock(mutex);
        
        for (auto it = timers.begin(); it != timers.end(); ++it) {
            if (it->get() == timer) {
                double elapsed = timer->stop();
                std::string unitStr = Timer::unitToString(timer->getUnit());
                NLM_LOG_INFO("PerformanceMonitor: Timer " + timer->getName() + " completed in " + 
                           std::to_string(elapsed) + " " + unitStr);
                
                timers.erase(it);
                return elapsed;
            }
        }
        
        NLM_LOG_WARNING("PerformanceMonitor: Timer not found");
        return 0.0;
    }
    
    // Add or update a performance counter
    void updateCounter(const std::string& name, double value, PerformanceUnit unit = PerformanceUnit::Count, bool isRate = false) {
        std::lock_guard<std::mutex> lock(mutex);
        
        // Check if counter already exists
        for (auto& counter : counters) {
            if (counter.name == name) {
                counter.update(value);
                NLM_LOG_DEBUG("PerformanceMonitor: Updated counter " + name + " to " + std::to_string(value));
                return;
            }
        }
        
        // Add new counter
        counters.emplace_back(name, value, unit, isRate);
        NLM_LOG_DEBUG("PerformanceMonitor: Added counter " + name + " = " + std::to_string(value));
    }
    
    // Get counter value
    double getCounterValue(const std::string& name) const {
        std::lock_guard<std::mutex> lock(mutex);
        
        for (const auto& counter : counters) {
            if (counter.name == name) {
                return counter.value;
            }
        }
        
        return 0.0;
    }
    
    // Get counter rate (if it's a rate counter)
    double getCounterRate(const std::string& name) const {
        std::lock_guard<std::mutex> lock(mutex);
        
        for (const auto& counter : counters) {
            if (counter.name == name && counter.isRate) {
                return counter.getRate();
            }
        }
        
        return 0.0;
    }
    
    // Generate performance report
    std::string generateReport() const {
        std::ostringstream oss;
        oss << "=== NLM Performance Monitor Report ===\n\n";
        
        oss << "Counters:\n";
        for (const auto& counter : counters) {
            oss << "  " << counter.name << " = " << counter.value;
            
            std::string unitStr = Timer::unitToString(counter.unit);
            if (!unitStr.empty()) {
                oss << " " << unitStr;
            }
            
            if (counter.isRate) {
                double rate = counter.getRate();
                oss << " (rate: " << rate << "/s)";
            }
            oss << "\n";
        }
        
        if (!timers.empty()) {
            oss << "\nActive Timers:\n";
            for (const auto& timer : timers) {
                double elapsed = timer->elapsed();
                std::string unitStr = Timer::unitToString(timer->getUnit());
                oss << "  " << timer->getName() << ": " << elapsed << " " << unitStr << " (running)\n";
            }
        }
        
        oss << "\n=== End Report ===\n";
        return oss.str();
    }
    
    // Log performance report
    void logPerformanceReport(LogLevel level = LogLevel::Info) {
        std::lock_guard<std::mutex> lock(mutex);
        std::string report = generateReport();
        
        switch (level) {
            case LogLevel::Debug:
                NLM_LOG_DEBUG(report);
                break;
            case LogLevel::Info:
                NLM_LOG_INFO(report);
                break;
            case LogLevel::Warning:
                NLM_LOG_WARNING(report);
                break;
            case LogLevel::Error:
                NLM_LOG_ERROR(report);
                break;
            case LogLevel::Critical:
                NLM_LOG_CRITICAL(report);
                break;
        }
    }
    
    // Clear all performance data
    void clear() {
        std::lock_guard<std::mutex> lock(mutex);
        counters.clear();
        timers.clear();
        NLM_LOG_INFO("PerformanceMonitor: Cleared all performance data");
    }

private:
    PerformanceMonitor() = default;
    ~PerformanceMonitor() = default;
    PerformanceMonitor(const PerformanceMonitor&) = delete;
    PerformanceMonitor& operator=(const PerformanceMonitor&) = delete;
    
    std::vector<PerformanceCounter> counters;
    std::vector<std::unique_ptr<Timer>> timers;
    mutable std::mutex mutex;
};

// Convenience functions for performance monitoring
inline Timer* startPerfTimer(const std::string& name, PerformanceUnit unit = PerformanceUnit::Milliseconds) {
    return PerformanceMonitor::getInstance().startTimer(name, unit);
}

inline double stopPerfTimer(Timer* timer) {
    return PerformanceMonitor::getInstance().stopTimer(timer);
}

inline void updatePerfCounter(const std::string& name, double value, PerformanceUnit unit = PerformanceUnit::Count, bool isRate = false) {
    PerformanceMonitor::getInstance().updateCounter(name, value, unit, isRate);
}

inline double getPerfCounterValue(const std::string& name) {
    return PerformanceMonitor::getInstance().getCounterValue(name);
}

inline double getPerfCounterRate(const std::string& name) {
    return PerformanceMonitor::getInstance().getCounterRate(name);
}

inline void logPerfReport(LogLevel level = LogLevel::Info) {
    PerformanceMonitor::getInstance().logPerformanceReport(level);
}

} // namespace nlm