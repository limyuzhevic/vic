// Real-Time Monitor Header
#pragma once

#include <string>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <thread>

namespace nlm {

/**
 * Real-time monitor for NLM
 */
class RealTimeMonitor {
public:
    struct MonitorData {
        // System resources
        double cpuUsage;
        double memoryUsage;
        double diskUsage;
        
        // NLM brain state
        size_t totalNeurons;
        size_t totalSynapses;
        float firingRate;
        float avgSynapticWeight;
        size_t totalSpikes;
        
        // Memory systems
        bool workingMemoryActive;
        size_t episodicMemoryEpisodes;
    };
    
    RealTimeMonitor();
    ~RealTimeMonitor();
    
    /**
     * Start the monitor
     */
    void start();
    
    /**
     * Stop the monitor
     */
    void stop();
    
    /**
     * Update monitor with new data
     */
    void update(const MonitorData& data);
    
    /**
     * Set check interval
     */
    void setCheckInterval(double interval);
    
private:
    void monitorLoop();
    void checkSystemHealth();
    void checkBrainHealth();
    void checkResourceUsage();
    void logWarning(const std::string& message);
    void logCritical(const std::string& message);
    static std::string getCurrentTimestamp();
    std::string getTimestampString(const std::chrono::high_resolution_clock::time_point& timestamp);
    
    bool _isRunning;
    double _checkInterval;
    
    mutable std::mutex _mutex;
    std::condition_variable _monitorUpdateCondition;
    
    MonitorData _currentData;
    std::thread _monitorThread;
};

} // namespace nlm