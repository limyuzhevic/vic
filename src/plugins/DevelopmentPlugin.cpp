#pragma once

#include "IPlugin.hpp"
#include "IDevelopmentPlugin.hpp"
#include "../development/DevelopmentSystem.hpp"
#include <vector>
#include <memory>
#include <mutex>
#include <cmath>

namespace nlm {
namespace plugins {

// DevelopmentPlugin implementation
class DevelopmentPlugin : public IDevelopmentPlugin {
public:
    DevelopmentPlugin();
    ~DevelopmentPlugin() override;
    
    // IPlugin interface
    const char* getName() const override { return "DevelopmentPlugin"; }
    const char* getVersion() const override { return "1.0.0"; }
    const char* getDescription() const override { return "Development Plugin - neural development and maturation stages"; }
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
    std::map<std::string, std::string> getAllConfig() override;
    
    // IDevelopmentPlugin interface
    DevelopmentalStage getStage() const override { return currentStage_; }
    const char* getStageName() const override;
    void setStage(DevelopmentalStage stage) override;
    
    double getAge() const override { return currentAge_; }
    void setAge(double age) override { currentAge_ = std::max(0.0, age); }
    void advanceAge(double delta) override;
    
    // Developmental parameters
    float getPlasticityRate() const override { return plasticityRate_; }
    float getCriticalPeriodProgress() const override { return criticalPeriodProgress_; }
    bool isCriticalPeriodActive() const override { return isCriticalPeriodActive_; }
    
    // Developmental triggers
    void triggerEvent(const std::string& event) override;
    std::vector<std::string> getActiveTriggers() const override;
    
    // Development system properties
    float getMaturationRate() const { return maturationRate_; }
    void setMaturationRate(float rate) { maturationRate_ = std::max(0.0f, std::min(10.0f, rate)); }
    
    float getGrowthRate() const { return growthRate_; }
    void setGrowthRate(float rate) { growthRate_ = std::max(0.0f, std::min(10.0f, rate)); }
    
    float getPruningRate() const { return pruningRate_; }
    void setPruningRate(float rate) { pruningRate_ = std::max(0.0f, std::min(1.0f, rate)); }
    
    // Synaptogenesis
    bool getSynaptogenesisActive() const { return synaptogenesisActive_; }
    void setSynaptogenesisActive(bool active) { synaptogenesisActive_ = active; }
    
    float getSynaptogenesisRate() const { return synaptogenesisRate_; }
    void setSynaptogenesisRate(float rate) { synaptogenesisRate_ = std::max(0.0f, std::min(1.0f, rate)); }
    
    // Critical period dynamics
    double getCriticalPeriodStart() const { return criticalPeriodStart_; }
    double getCriticalPeriodEnd() const { return criticalPeriodEnd_; }
    void setCriticalPeriod(double start, double end);
    
    // Developmental milestones
    bool hasReachedMilestone(const std::string& milestone) const;
    void setMilestone(const std::string& milestone, bool achieved = true);
    std::vector<std::string> getCompletedMilestones() const;
    
    // Dependencies
    std::vector<std::string> getDependencies() const override;
    bool hasDependency(const std::string& pluginName) const override;
    
    // Metadata
    std::string getCategory() const override { return "Development"; }
    std::vector<std::string> getTags() const override { return {"development", "maturation", "growth", "critical-period", "synaptogenesis"}; }
    
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
    DevelopmentalStage currentStage_;
    double currentAge_;
    float plasticityRate_;
    float maturationRate_;
    float growthRate_;
    float pruningRate_;
    float synaptogenesisRate_;
    float criticalPeriodProgress_;
    bool isCriticalPeriodActive_;
    double criticalPeriodStart_;
    double criticalPeriodEnd_;
    mutable std::mutex mutex_;
    double lastUpdateTime_;
    double averageUpdateTime_;
    uint64_t updateCount_;
    std::vector<PluginError> errors_;
    PluginState state_;
    std::map<std::string, std::string> config_;
    
    // Development tracking
    std::vector<std::string> activeTriggers_;
    std::vector<std::string> completedMilestones_;
    double lastStageTransitionTime_;
    double developmentalTimeScale_;
};

} // namespace plugins
} // namespace nlm
