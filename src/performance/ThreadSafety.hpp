/*
 * Thread Safety Improvements for NLM
 * 
 * This file adds thread safety mechanisms to prevent race conditions
 * and ensure safe concurrent access to shared resources.
 */

#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <mutex>
#include <shared_mutex>
#include <condition_variable>

namespace nlm {

class BrainThreadSafety {
public:
    BrainThreadSafety() : stepInProgress_(false) {}
    
    // Thread-safe step execution
    bool acquireStepLock() {
        std::unique_lock<std::mutex> lock(stepMutex_);
        if (stepInProgress_) {
            return false; // Step already in progress
        }
        stepInProgress_ = true;
        return true;
    }
    
    void releaseStepLock() {
        std::lock_guard<std::mutex> lock(stepMutex_);
        stepInProgress_ = false;
        stepCondition_.notify_all();
    }
    
    // Wait for step completion
    bool waitForStepCompletion(std::chrono::milliseconds timeout = std::chrono::milliseconds(100)) {
        std::unique_lock<std::mutex> lock(stepMutex_);
        return stepCondition_.wait_for(lock, timeout, [this] { return !stepInProgress_; });
    }
    
private:
    std::mutex stepMutex_;
    std::condition_variable stepCondition_;
    bool stepInProgress_;
};

class ConfigThreadSafety {
public:
    ConfigThreadSafety() {}
    
    // Thread-safe configuration access
    template<typename T>
    std::optional<T> getThreadSafe(const Config& config, const std::string& key) const {
        std::shared_lock<std::shared_mutex> lock(configMutex_);
        return config.get<T>(key);
    }
    
    // Thread-safe configuration modification
    void setThreadSafe(Config& config, const std::string& key, 
                      const ConfigValue& value, ConfigSource source) {
        std::unique_lock<std::shared_mutex> lock(configMutex_);
        config.set(key, value, source);
    }
    
    // Thread-safe configuration check
    bool hasThreadSafe(const Config& config, const std::string& key) const {
        std::shared_lock<std::shared_mutex> lock(configMutex_);
        return config.has(key);
    }
    
    // Thread-safe configuration modification
    void removeThreadSafe(Config& config, const std::string& key) {
        std::unique_lock<std::shared_mutex> lock(configMutex_);
        config.remove(key);
    }
    
    // Thread-safe configuration clearing
    void clearThreadSafe(Config& config) {
        std::unique_lock<std::shared_mutex> lock(configMutex_);
        config.clear();
    }
    
private:
    mutable std::shared_mutex configMutex_;
};

class SpikeSystemThreadSafety {
public:
    SpikeSystemThreadSafety() : processingSpikes_(false) {}
    
    // Thread-safe spike processing
    bool acquireSpikeProcessingLock() {
        std::unique_lock<std::mutex> lock(processingMutex_);
        if (processingSpikes_) {
            return false; // Already processing spikes
        }
        processingSpikes_ = true;
        return true;
    }
    
    void releaseSpikeProcessingLock() {
        std::lock_guard<std::mutex> lock(processingMutex_);
        processingSpikes_ = false;
    }
    
    // Thread-safe spike queue operations
    void queueSpikeThreadSafe(SpikeSystem& spikeSystem, const SpikeEvent& event) {
        std::lock_guard<std::mutex> lock(spikeQueueMutex_);
        spikeSystem.queueSpike(event);
    }
    
    void queueDelayedSpikeThreadSafe(SpikeSystem& spikeSystem, const DelayedSpikeEvent& event) {
        std::lock_guard<std::mutex> lock(spikeQueueMutex_);
        spikeSystem.queueDelayedSpike(event);
    }
    
    // Thread-safe spike processing
    void processSpikesThreadSafe(SpikeSystem& spikeSystem, SimulationStep step) {
        std::lock_guard<std::mutex> lock(spikeQueueMutex_);
        spikeSystem.processSpikes(step);
    }
    
    void processDelayedSpikesThreadSafe(SpikeSystem& spikeSystem, SimulationStep step, Timestamp time) {
        std::lock_guard<std::mutex> lock(spikeQueueMutex_);
        spikeSystem.processDelayedSpikes(step, time);
    }
    
private:
    std::mutex processingMutex_;
    std::mutex spikeQueueMutex_;
    bool processingSpikes_;
};

} // namespace nlm
