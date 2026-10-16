// src/performance/Profiler.hpp
#pragma once

#include <string>
#include <chrono>
#include <unordered_map>
#include <vector>
#include <memory>

namespace nlm {

// Performance profiler for timing operations
class Profiler {
public:
    Profiler();
    ~Profiler();
    
    // Start profiling a named operation
    void start(const std::string& name);
    
    // Stop profiling and record the duration
    double stop(const std::string& name);
    
    // Get total duration for a named operation
    double getTotalDuration(const std::string& name) const;
    
    // Get average duration for a named operation
    double getAverageDuration(const std::string& name) const;
    
    // Get number of measurements for a named operation
    size_t getMeasurementCount(const std::string& name) const;
    
    // Get all recorded operation names
    std::vector<std::string> getOperationNames() const;
    
    // Clear all recorded data
    void clear();
    
    // Get total profiling time
    double getTotalTime() const;
    
    // Get profiling statistics report
    std::string getReport() const;
    
    // Enable/disable detailed output
    void setVerbose(bool enable) { verbose_ = enable; }
    
    // Set minimum threshold for reporting (in seconds)
    void setThreshold(double threshold) { threshold_ = threshold; }
    
    // Profile a function (RAII helper)
    template<typename F, typename... Args>
    auto profile(const std::string& name, F&& func, Args&&... args) {
        start(name);
        auto result = func(std::forward<Args>(args)...);
        double duration = stop(name);
        return result;
    }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    bool verbose_;
    double threshold_;
};

} // namespace nlm