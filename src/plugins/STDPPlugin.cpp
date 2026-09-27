#pragma once

#include "IPlugin.hpp"
#include "IPlasticityPlugin.hpp"
#include "../brain/Synapse.hpp"
#include "../plasticity/STDP.hpp"
#include <vector>
#include <memory>
#include <mutex>

namespace nlm {
namespace plugins {

// STDPPlugin implementation
class STDPPlugin : public IPlasticityPlugin {
public:
    STDPPlugin();
    ~STDPPlugin() override;
    
    // IPlugin interface
    const char* getName() const override { return "STDPPlugin"; }
    const char* getVersion() const override { return "1.0.0"; }
    const char* getDescription() const override { return "Spiking Temporal Difference Learning Plugin"; }
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
    
    // IPlasticityPlugin interface
    const char* getPlasticityType() const override { return "STDP"; }
    const char* getPlasticityMechanism() const override { return "Spike timing dependent plasticity"; }
    
    float computeWeightChange(const Synapse& synapse, float preTrace, float postTrace,
                             float neuromodulatorLevel = 1.0f, float timeFactor = 1.0f) override;
    
    float getLearningRate() const override;
    void setLearningRate(float rate) override;
    
    float getEligibilityTraceDecay() const override;
    void setEligibilityTraceDecay(float decay) override;
    
    float getModulatoryFactor() const override;
    void setModulatoryFactor(float factor) override;
    
    void updateEligibilityTrace(float preActivity, float postActivity, TimestepDuration dt) override;
    float getEligibilityTrace() const override;
    void resetEligibilityTrace() override;
    
    void applyWeightChange(Synapse& synapse, float delta, bool constrain = true) override;
    float constrainWeightChange(float currentWeight, float delta) const override;
    
    void prepareForSimulation() override;
    void resetForNewEpisode() override;
    bool isReady() const override;
    
    // STDP specific parameters
    float getSTDPLearningRate() const override { return learningRate_; }
    void setSTDPLearningRate(float rate) override { learningRate_ = rate; }
    float getSTDPTimeConstant() const override { return timeConstant_; }
    void setSTDPTimeConstant(float timeConstant) override { timeConstant_ = timeConstant; }
    
    // Dependencies
    std::vector<std::string> getDependencies() const override;
    bool hasDependency(const std::string& pluginName) const override;
    
    // Metadata
    std::string getCategory() const override { return "Plasticity"; }
    std::vector<std::string> getTags() const override { return {"stdp", "spiking", "temporal", "learning"}; }
    
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
    float learningRate_;
    float timeConstant_;
    float eligibilityTrace_;
    float modulatoryFactor_;
    mutable std::mutex mutex_;
    double lastUpdateTime_;
    double averageUpdateTime_;
    uint64_t updateCount_;
    std::vector<PluginError> errors_;
    PluginState state_;
    std::map<std::string, std::string> config_;
};

} // namespace plugins
} // namespace nlm
