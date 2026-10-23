#pragma once

#include <chrono>
#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <thread>
#include <atomic>
#include <memory>

namespace nlm {

class PerformanceProfiler {
public:
    struct ProfileData {
        std::string name;
        std::chrono::high_resolution_clock::time_point startTime;
        std::chrono::duration<double> duration;
        uint64_t calls;
        double totalTime;
        double minTime;
        double maxTime;
        std::string description;
    };
    
    PerformanceProfiler() : running(false), totalThreads(0) {
        initializeProfiledFunctions();
    }
    
    ~PerformanceProfiler() {
        stopAllProfiling();
    }
    
    // Start profiling a specific function
    void startProfile(const std::string& name, const std::string& description = "") {
        std::lock_guard<std::mutex> lock(profileMutex);
        auto it = profileData.find(name);
        if (it != profileData.end()) {
            it->second.startTime = std::chrono::high_resolution_clock::now();
            it->second.calls++;
        }
    }
    
    // Stop profiling and record the duration
    void stopProfile(const std::string& name) {
        std::lock_guard<std::mutex> lock(profileMutex);
        auto it = profileData.find(name);
        if (it != profileData.end()) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - it->second.startTime);
            
            double timeMs = duration.count() / 1000.0;
            it->second.duration += duration;
            it->second.totalTime += timeMs;
            
            if (timeMs < it->second.minTime || it->second.calls == 1) {
                it->second.minTime = timeMs;
            }
            if (timeMs > it->second.maxTime) {
                it->second.maxTime = timeMs;
            }
        }
    }
    
    // RAII-style profiling
    class ScopedProfiler {
    public:
        ScopedProfiler(PerformanceProfiler& profiler, const std::string& name, 
                      const std::string& description = "")
            : profiler(profiler), name(name) {
            profiler.startProfile(name, description);
        }
        
        ~ScopedProfiler() {
            profiler.stopProfile(name);
        }
    
    private:
        PerformanceProfiler& profiler;
        std::string name;
    };
    
    // Get profiling data
    const ProfileData& getProfileData(const std::string& name) const {
        static const ProfileData emptyData = {"", {}, {}, 0, 0.0, 0.0, 0.0, ""};
        auto it = profileData.find(name);
        return it != profileData.end() ? it->second : emptyData;
    }
    
    // Get all profiling data
    const std::unordered_map<std::string, ProfileData>& getAllProfileData() const {
        return profileData;
    }
    
    // Reset profiling data
    void reset() {
        std::lock_guard<std::mutex> lock(profileMutex);
        for (auto& pair : profileData) {
            pair.second.calls = 0;
            pair.second.totalTime = 0.0;
            pair.second.minTime = 0.0;
            pair.second.maxTime = 0.0;
        }
    }
    
    // Generate report
    std::string generateReport() const {
        std::stringstream ss;
        ss << "=== NLM Performance Profile Report ===\n\n";
        
        if (profileData.empty()) {
            ss << "No profiling data available.\n";
            return ss.str();
        }
        
        ss << std::left << std::setw(30) << "Function" 
           << std::setw(10) << "Calls" 
           << std::setw(15) << "Total Time (ms)" 
           << std::setw(15) << "Avg Time (ms)" 
           << std::setw(15) << "Min Time (ms)" 
           << std::setw(15) << "Max Time (ms)" 
           << std::setw(40) << "Description" << std::endl;
        
        ss << std::string(100, '-') << std::endl;
        
        for (const auto& pair : profileData) {
            const auto& data = pair.second;
            double avgTime = data.calls > 0 ? data.totalTime / data.calls : 0.0;
            
            ss << std::left << std::setw(30) << pair.first
               << std::setw(10) << data.calls
               << std::setw(15) << std::fixed << std::setprecision(3) << data.totalTime
               << std::setw(15) << std::fixed << std::setprecision(3) << avgTime
               << std::setw(15) << std::fixed << std::setprecision(3) << data.minTime
               << std::setw(15) << std::fixed << std::setprecision(3) << data.maxTime
               << std::setw(40) << data.description << std::endl;
        }
        
        return ss.str();
    }
    
    // Export to file
    bool exportToFile(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file) {
            return false;
        }
        
        file << generateReport();
        return true;
    }
    
    // Start/stop global profiling
    void startGlobalProfiling() {
        if (!running) {
            running = true;
            profileThread = std::thread(&PerformanceProfiler::profileWorker, this);
        }
    }
    
    void stopGlobalProfiling() {
        if (running) {
            running = false;
            if (profileThread.joinable()) {
                profileThread.join();
            }
        }
    }
    
    // Thread-safe profiling wrapper
    class ThreadSafeProfiler {
    public:
        ThreadSafeProfiler(PerformanceProfiler& profiler, const std::string& name,
                          const std::string& description = "")
            : profiler(profiler), name(name) {
            profiler.startProfile(name, description);
        }
        
        ~ThreadSafeProfiler() {
            profiler.stopProfile(name);
        }
    
    private:
        PerformanceProfiler& profiler;
        std::string name;
    };
    
private:
    void initializeProfiledFunctions() {
        // Initialize common profiling entries
        profileData["Brain::step"] = {"Brain::step", {}, std::chrono::duration<double>(0), 0, 0.0, 0.0, 0.0, "Main simulation loop"};
        profileData["Neuron::update"] = {"Neuron::update", {}, std::chrono::duration<double>(0), 0, 0.0, 0.0, 0.0, "Neuron state update"};
        profileData["Synapse::transmit"] = {"Synapse::transmit", {}, std::chrono::duration<double>(0), 0, 0.0, 0.0, 0.0, "Synaptic transmission"};
        profileData["STDP::update"] = {"STDP::update", {}, std::chrono::duration<double>(0), 0, 0.0, 0.0, 0.0, "Spike-Timing Dependent Plasticity"};
        profileData["Hebbian::update"] = {"Hebbian::update", {}, std::chrono::duration<double>(0), 0, 0.0, 0.0, 0.0, "Hebbian plasticity"};
        profileData["SensoryInput::process"] = {"SensoryInput::process", {}, std::chrono::duration<double>(0), 0, 0.0, 0.0, 0.0, "Sensory input processing"};
        profileData["MotorSystem::output"] = {"MotorSystem::output", {}, std::chrono::duration<double>(0), 0, 0.0, 0.0, 0.0, "Motor output generation"};
        profileData["MemorySystem::store"] = {"MemorySystem::store", {}, std::chrono::duration<double>(0), 0, 0.0, 0.0, 0.0, "Memory storage"};
        profileData["PredictionSystem::predict"] = {"PredictionSystem::predict", {}, std::chrono::duration<double>(0), 0, 0.0, 0.0, 0.0, "Prediction system"};
        profileData["Neuromodulator::affect"] = {"Neuromodulator::affect", {}, std::chrono::duration<double>(0), 0, 0.0, 0.0, 0.0, "Neuromodulation effects"};
    }
    
    void profileWorker() {
        while (running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            // Update system-wide metrics
            updateSystemMetrics();
        }
    }
    
    void updateSystemMetrics() {
        // Update system-wide performance metrics
        // This could include memory usage, thread utilization, etc.
    }
    
    mutable std::mutex profileMutex;
    std::unordered_map<std::string, ProfileData> profileData;
    std::atomic<bool> running;
    std::thread profileThread;
    std::atomic<int> totalThreads;
};

// Convenience functions for easy profiling
class Profiler {
public:
    static PerformanceProfiler& getInstance() {
        static PerformanceProfiler instance;
        return instance;
    }
    
    // Start/stop global profiling
    static void startGlobalProfiling() {
        getInstance().startGlobalProfiling();
    }
    
    static void stopGlobalProfiling() {
        getInstance().stopGlobalProfiling();
    }
    
    // Get report
    static std::string getReport() {
        return getInstance().generateReport();n    }
    
    // Export to file
    static bool exportReport(const std::string& filename) {
        return getInstance().exportToFile(filename);
    }
    
    // Reset profiler
    static void reset() {
        getInstance().reset();
    }
    
    // Helper function for easy profiling
    template<typename Func>
    static auto profile(const std::string& name, Func&& func, const std::string& description = "") {
        PerformanceProfiler& profiler = getInstance();
        profiler.startProfile(name, description);
        
        auto result = func();
        
        profiler.stopProfile(name);
        return result;
    }
};

} // namespace nlm