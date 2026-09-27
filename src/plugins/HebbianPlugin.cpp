#pragma once

#include "IPlugin.hpp"
#include "IPlasticityPlugin.hpp"
#include "../brain/Synapse.hpp"
#include "../plasticity/Hebbian.hpp"
#include <vector>
#include <memory>
#include <mutex>

namespace nlm {
namespace plugins {

// HebbianPlugin implementation
class HebbianPlugin : public IPlasticityPlugin {
public:
    HebbianPlugin();
    ~HebbianPlugin() override;
    
    // IPlugin interface
    const char* getName() const override { return "HebbianPlugin"; }
    const char* getVersion() const override { return "1.0.0"; }
    const char* getDescription() const override { return "Hebbian Learning Plugin - 'neurons that fire together, wire together'"; }
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
    const char* getPlasticityType() const override { return "Hebbian"; }
    const char* getPlasticityMechanism() const override { return "Hebbian learning - coincident spike detection"; }
    
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
    
    // Parameter constraints
    bool validateParameters() const override;
    std::vector<std::string> getParameterConstraints() const override;
    
    // Hebbian specific parameters
    float getHebbianLearningRate() const override { return learningRate_; }
    void setHebbianLearningRate(float rate) override { learningRate_ = rate; }
    float getHebbianCoherenceThreshold() const override { return coherenceThreshold_; }
    void setHebbianCoherenceThreshold(float threshold) override { coherenceThreshold_ = threshold; }
    
    // Coincidence detection
    float getCoincidenceWindow() const { return coincidenceWindow_; }
    void setCoincidenceWindow(float window) { coincidenceWindow_ = window; }
    
    // Dependencies
    std::vector<std::string> getDependencies() const override;
    bool hasDependency(const std::string& pluginName) const override;
    
    // Metadata
    std::string getCategory() const override { return "Plasticity"; }
    std::vector<std::string> getTags() const override { return {"hebbian", "coincidence", "learning", "associative"}; }
    
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
    float coherenceThreshold_;
    float coincidenceWindow_;
    float eligibilityTrace_;
    float modulatoryFactor_;
    mutable std::mutex mutex_;
    double lastUpdateTime_;
    double averageUpdateTime_;
    uint64_t updateCount_;
    std::vector<PluginError> errors_;
    PluginState state_;
    std::map<std::string, std::string> config_;
    
    // Coincidence detection statistics
    float lastPreActivity_;
    float lastPostActivity_;
    bool activityCoincidence_;
};

} // namespace plugins
} // namespace nlm
