// Performance Monitoring and Profiling
// Adds profiling capabilities to measure performance of neural network operations

#pragma once

#include <chrono>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <iomanip>
#include <sstream>

namespace nlm {

class Profiler {
public:
    struct ProfileData {
        std::string name;
        std::chrono::high_resolution_clock::time_point startTime;
        std::chrono::high_resolution_clock::time_point endTime;
        double durationMs;
        size_t callCount;
        double totalDurationMs;
        double averageDurationMs;
        
        ProfileData(const std::string& n) 
            : name(n), startTime(), endTime(), durationMs(0.0), 
              callCount(0), totalDurationMs(0.0), averageDurationMs(0.0) {}
        
        void start() {
            startTime = std::chrono::high_resolution_clock::now();
        }
        
        void stop() {
            endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
            durationMs = duration.count() / 1000.0; // Convert to milliseconds
            totalDurationMs += durationMs;
            callCount++;
            averageDurationMs = totalDurationMs / callCount;
        }
        
        std::string getReport() const {
            std::stringstream ss;
            ss << "  " << name << ": " 
               << std::fixed << std::setprecision(3) << durationMs << "ms "
               << "(avg: " << averageDurationMs << "ms, "
               << callCount << " calls)";
            return ss.str();
        }
    };
    
private:
    std::unordered_map<std::string, ProfileData> profiles;
    bool enabled;
    
public:
    Profiler() : enabled(true) {}
    
    void enable(bool enableFlag) {
        enabled = enableFlag;
    }
    
    void start(const std::string& name) {
        if (!enabled) return;
        if (profiles.find(name) == profiles.end()) {
            profiles[name] = ProfileData(name);
        }
        profiles[name].start();
    }
    
    void stop(const std::string& name) {
        if (!enabled) return;
        auto it = profiles.find(name);
        if (it != profiles.end()) {
            it->second.stop();
        }
    }
    
    void reset(const std::string& name) {
        auto it = profiles.find(name);
        if (it != profiles.end()) {
            it->second = ProfileData(name);
        }
    }
    
    std::string getReport() const {
        std::stringstream ss;
        ss << "=== Performance Profile Report ===\n";
        ss << "Enabled: " << (enabled ? "Yes" : "No") << "\n";
        ss << "Total Profiled Operations: " << profiles.size() << "\n\n";
        
        for (const auto& pair : profiles) {
            ss << pair.second.getReport() << "\n";
        }
        
        ss << "\n=== End Report ===";
        return ss.str();
    }
    
    double getTotalDuration() const {
        double total = 0.0;
        for (const auto& pair : profiles) {
            total += pair.second.totalDurationMs;
        }
        return total;
    }
    
    size_t getTotalCalls() const {
        size_t total = 0;
        for (const auto& pair : profiles) {
            total += pair.second.callCount;
        }
        return total;
    }
};

// Global profiler instance
inline Profiler& getGlobalProfiler() {
    static Profiler profiler;
    return profiler;
}

// Convenience macros for timing
#ifdef PROFILE_ENABLED
#define PROFILE_START(name) nlm::getGlobalProfiler().start(name)
#define PROFILE_STOP(name) nlm::getGlobalProfiler().stop(name)
#define PROFILE_SCOPE(name) \
    class ScopedProfile { \
    public: ScopedProfile(const std::string& n) : name(n) { PROFILE_START(name); } \
    ~ScopedProfile() { PROFILE_STOP(name); } \
    private: std::string name; \
    } scopedProfile_##__LINE__(name)
#else
#define PROFILE_START(name)
#define PROFILE_STOP(name)
#define PROFILE_SCOPE(name)
#endif

// Performance monitoring utilities
namespace perf {
    
    // RAII profiler for scoped timing
    class ScopedProfiler {
    public:
        ScopedProfiler(const std::string& name, Profiler& profiler = getGlobalProfiler())
            : name(name), profiler(profiler) {
            profiler.start(name);
        }
        
        ~ScopedProfiler() {
            profiler.stop(name);
        }
        
    private:
        std::string name;
        Profiler& profiler;
    };
    
    // Get performance report
    inline std::string getReport() {
        return getGlobalProfiler().getReport();
    }
    
    // Enable/disable profiling
    inline void enable(bool enableFlag) {
        getGlobalProfiler().enable(enableFlag);
    }
    
    // Reset all profiles
    inline void reset() {
        getGlobalProfiler().enable(true);
        // Clear all profiles by creating new ones
    }
    
    // Print performance report to console
    inline void printReport() {
        std::cout << getReport() << std::endl;
    }
}

// Template-based timing utility
class Timer {
public:
    Timer(const std::string& name, Profiler& profiler = getGlobalProfiler())
        : name(name), profiler(profiler), started(false) {}
    
    ~Timer() {
        if (started) {
            profiler.stop(name);
        }
    }
    
    void start() {
        if (!started) {
            profiler.start(name);
            started = true;
        }
    }
    
    void stop() {
        if (started) {
            profiler.stop(name);
            started = false;
        }
    }
    
    bool isRunning() const {
        return started;
    }
    
    double getDuration() const {
        auto it = profiler.getProfiles().find(name);
        if (it != profiler.getProfiles().end()) {
            return it->second.durationMs;
        }
        return 0.0;
    }
    
private:
    std::string name;
    Profiler& profiler;
    bool started;
    
    // Access to profiles for inspection
    friend Profiler;
    const auto& getProfiles() const { return profiler.profiles; }
};

} // namespace nlm

// Example usage for performance monitoring:

// Basic profiling
void exampleBasicProfiling() {
    nlm::Profiler profiler;
    
    profiler.start("neuron_update");
    // Do some work...
    for (int i = 0; i < 1000; ++i) {
        // Simulate neuron processing
    }
    profiler.stop("neuron_update");
    
    profiler.start("synapse_update");
    // Do some work...
    for (int i = 0; i < 5000; ++i) {
        // Simulate synapse processing
    }
    profiler.stop("synapse_update");
    
    std::cout << profiler.getReport() << std::endl;
}

// Scoped profiling
void exampleScopedProfiling() {
    std::cout << "=== Scoped Profiling Example ===" << std::endl;
    
    nlm::perf::enable(true);
    
    // This will automatically profile the scope
    {
        nlm::perf::ScopedProfiler profiler("brain_step");
        // Brain simulation work
        for (int step = 0; step < 100; ++step) {
            // Simulate neural computation
        }
    }
    
    nlm::perf::printReport();
}

// Global profiling for integrated brain
void exampleGlobalProfiling() {
    std::cout << "=== Global Profiling Example ===" << std::endl;
    
    nlm::perf::enable(true);
    
    // Simulate brain operations with global profiler
    {
        nlm::perf::ScopedProfiler stepProfiler("brain_step");
        
        // Multiple internal operations
        {
            nlm::perf::ScopedProfiler neuronUpdate("neuron_lif_update");
            // LIF neuron updates
        }
        
        {
            nlm::perf::ScopedProfiler spikeHandler("spike_processing");
            // Spike detection and propagation
        }
        
        {
            nlm::perf::ScopedProfiler plasticityUpdate("plasticity_application");
            // STDP and Hebbian learning
        }
        
        {
            nlm::perf::ScopedProfiler memoryUpdate("memory_operations");
            // Memory system updates
        }
    }
    
    nlm::perf::printReport();
}