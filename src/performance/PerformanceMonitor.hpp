#ifndef NLM_PERFORMANCEMONITOR_HPP
#define NLM_PERFORMANCEMONITOR_HPP

#include <string>
#include <map>
#include <vector>
#include <chrono>
#include <sstream>
#include <fstream>
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct TimerData {
    bool isRunning = false;
    double elapsedTime = 0.0;
    std::chrono::steady_clock::time_point startTime;
};

class PerformanceMonitor {
public:
    PerformanceMonitor();
    ~PerformanceMonitor();
    
    // Monitoring control
    void startMonitoring();
    void stopMonitoring();
    void update(double timestep);
    
    // Performance reporting
    std::string getPerformanceReport() const;
    std::string getSystemInfo();
    void logPerformanceData();
    void logSystemInfo();
    void exportPerformanceData(const std::string& filepath);
    
    // Timer functionality
    void startTimer(const std::string& name);
    void stopTimer(const std::string& name);
    void resetTimer(const std::string& name);
    double getTimerTime(const std::string& name) const;
    bool isTimerRunning(const std::string& name) const;
    
    // Memory and system info
    static size_t getCurrentMemoryUsage();
    void setCPUUsage(double usage);
    void setMemoryUsage(size_t usage);
    
    // Checkpoint integration
    void saveCheckpoint(const std::string& filepath);
    void loadCheckpoint(const std::string& filepath);
    
    // Configuration
    void setUpdateInterval(double interval);
    
    // Data access
    std::vector<TimerData> getAllTimers() const;
    void clearTimers();
    
    // Statistics
    double getCurrentFPS() const { return currentFPS; }
    double getCurrentTPS() const { return currentTPS; }
    double getTotalTime() const { return timeSinceLastUpdate; }
    size_t getTotalSteps() const { return totalSteps; }
    size_t getMemoryUsage() const { return totalMemoryUsage; }
    size_t getPeakMemoryUsage() const { return peakMemoryUsage; }
    double getCPUUsage() const { return cpuUsage; }
    
    // Copy operations
    PerformanceMonitor(const PerformanceMonitor& other);
    PerformanceMonitor& operator=(const PerformanceMonitor& other);
    PerformanceMonitor(PerformanceMonitor&& other) noexcept;
    PerformanceMonitor& operator=(PerformanceMonitor&& other) noexcept;
    
private:
    // System information helpers
    static std::string getArchitectureString();
    static std::string getCompilerString();
    static std::string getBuildTypeString();
    static std::string getGitHash();
    static std::string getBuildDate();
    
    double timeSinceLastUpdate;
    size_t totalSteps;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point lastUpdateTime;
    double updateInterval;
    double currentFPS;
    double currentTPS;
    size_t totalMemoryUsage;
    size_t peakMemoryUsage;
    double cpuUsage;
    std::map<std::string, TimerData> activeTimers;
    bool isMonitoring;
    size_t framesSinceLastUpdate;
    std::chrono::steady_clock::time_point currentTime;
    }; // class PerformanceMonitor
    
    }; // namespace nlm
    
    #endif // NLM_PERFORMANCEMONITOR_HPP

} // namespace nlm

#endif // NLM_PERFORMANCEMONITOR_HPP
