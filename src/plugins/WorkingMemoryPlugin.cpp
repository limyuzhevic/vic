#pragma once

#include "IPlugin.hpp"
#include "IMemoryPlugin.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include <vector>
#include <memory>
#include <mutex>
#include <algorithm>

namespace nlm {
namespace plugins {

// WorkingMemoryPlugin implementation
class WorkingMemoryPlugin : public IMemoryPlugin {
public:
    WorkingMemoryPlugin();
    ~WorkingMemoryPlugin() override;
    
    // IPlugin interface
    const char* getName() const override { return "WorkingMemoryPlugin"; }
    const char* getVersion() const override { return "1.0.0"; }
    const char* getDescription() const override { return "Working Memory Plugin - transient information storage and active maintenance"; }
    const char* getAuthor() const override { return "NLM Team"; }
    
    bool initialize(const Config& config) override;
    void update(TimestepDuration dt) override;
    void shutdown() override;
    void cleanup() override;
    
    PluginState getState() const override;
    bool isEnabled() const override;
    void setEnabled(bool enabled) override;
    
    bool configure(const std::string& key, const std::string& value) override;
    std::string getConfig(const std::string& key) const override;
    std::map<std::string, std::string> getAllConfig() const override;
    
    // IMemoryPlugin interface
    std::string getMemoryType() const override { return "WorkingMemory"; }
    size_t getCapacity() const override { return capacity_; }
    float getAccessLatency() const override { return accessLatency_; }
    
    bool store(const std::string& key, const std::vector<float>& value, double timestamp) override;
    bool retrieve(const std::string& key, std::vector<float>& value, double& timestamp) const override;
    bool erase(const std::string& key) override;
    bool contains(const std::string& key) const override;
    
    // Working memory properties
    size_t getMaxSlots() const { return maxSlots_; }
    void setMaxSlots(size_t slots) { maxSlots_ = std::max(static_cast<size_t>(1), slots); }
    
    float getDecayRate() const { return decayRate_; }
    void setDecayRate(float rate) { decayRate_ = std::max(0.0f, std::min(1.0f, rate)); }
    
    float getUpdateFrequency() const { return updateFrequency_; }
    void setUpdateFrequency(float frequency) { updateFrequency_ = std::max(0.0f, std::min(1000.0f, frequency)); }
    
    // Memory management
    void clear() override;
    size_t size() const override;
    float getMemoryUtilization() const override;
    
    // Memory competition and prioritization
    void setSlotPriority(const std::string& key, float priority);
    float getSlotPriority(const std::string& key) const;
    std::vector<std::string> getHighPrioritySlots(float threshold = 0.7f) const;
    void evictLowPrioritySlots(float threshold = 0.3f);
    
    // Active maintenance
    bool isSlotActive(const std::string& key) const;
    void activateSlot(const std::string& key, float activationStrength = 1.0f);
    void deactivateSlot(const std::string& key);
    std::vector<std::string> getActiveSlots() const;
    
    // Memory consolidation
    void consolidateMemory();
    void consolidateSlot(const std::string& key, float consolidationStrength = 1.0f);
    
    // Dependencies
    std::vector<std::string> getDependencies() const override;
    bool hasDependency(const std::string& pluginName) const override;
    
    // Metadata
    std::string getCategory() const override { return "Memory"; }
    std::vector<std::string> getTags() const override { return {"working-memory", "transient", "active", "maintenance", "cognitive"}; }
    
    // Event system
    void subscribeToEvent(const std::string& eventName, const EventCallback& callback) override;
    void unsubscribeFromEvent(const std::string& eventName, const EventCallback& callback) override;
    
    // Error handling
    std::vector<PluginError> getErrors() const override;
    bool hasErrors() const override;
    void clearErrors() override;
    
    // Performance monitoring
    double getLastUpdateTime() const override { return lastUpdateTime_; }
    double getUpdateTimeAverage() const override { return averageUpdateTime_; }
    uint64_t getUpdateCount() const override { return updateCount_; }
    
    // Thread safety
    void lock() const override;
    void unlock() const override;
    bool tryLock() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal state
    size_t capacity_;
    size_t maxSlots_;
    float accessLatency_;
    float decayRate_;
    float updateFrequency_;
    mutable std::mutex mutex_;
    double lastUpdateTime_;
    double averageUpdateTime_;
    uint64_t updateCount_;
    std::vector<PluginError> errors_;
    PluginState state_;
    std::map<std::string, std::string> config_;
    
    // Working memory storage
    std::map<std::string, std::pair<std::vector<float>, double>> memorySlots_;
    std::map<std::string, float> slotPriorities_;
    std::map<std::string, float> slotActivations_;
    std::map<std::string, float> slotDecayTimers_;
    
    // Memory statistics
    size_t totalAccesses_;
    size_t totalStores_;
    size_t totalEvictions_;
    double totalDecayApplied_;
};

} // namespace plugins
} // namespace nlm
