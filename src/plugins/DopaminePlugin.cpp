#pragma once

#include "IPlugin.hpp"
#include "INeuromodulationPlugin.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include <vector>
#include <memory>
#include <mutex>
#include <algorithm>

namespace nlm {
namespace plugins {

// DopaminePlugin implementation
class DopaminePlugin : public INeuromodulationPlugin {
public:
    DopaminePlugin();
    ~DopaminePlugin() override;
    
    // IPlugin interface
    const char* getName() const override { return "DopaminePlugin"; }
    const char* getVersion() const override { return "1.0.0"; }
    const char* getDescription() const override { return "Dopamine Neuromodulation Plugin - reward prediction and reinforcement learning"; }
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
    
    // INeuromodulationPlugin interface
    float getModulationStrength() const override { return modulationStrength_; }
    void setModulationStrength(float strength) override { modulationStrength_ = std::max(0.0f, std::min(1.0f, strength)); }
    
    float getModulationRange() const override { return modulationRange_; }
    void setModulationRange(float range) override { modulationRange_ = std::max(0.0f, range); }
    
    float computeModulation(float input, float timeFactor = 1.0f, float contextFactor = 1.0f) override;
    
    // Neuromodulator effects
    std::vector<std::string> getTargetSystems() const override { return {"plasticity", "excitability", "inhibition"}; }
    bool affectsPlasticity() const override { return true; }
    bool affectsExcitability() const override { return true; }
    bool affectsInhibition() const override { return false; }
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Dopamine system properties
    float getBaselineLevel() const { return baselineLevel_; }
    void setBaselineLevel(float level) { baselineLevel_ = std::max(0.0f, level); }
    
    float getTimeConstant() const { return timeConstant_; }
    void setTimeConstant(float timeConstant) { timeConstant_ = std::max(0.01f, timeConstant); }
    
    float getLearningRate() const { return learningRate_; }
    void setLearningRate(float rate) { learningRate_ = std::max(0.0f, std::min(1.0f, rate)); }
    
    // Dopamine signal properties
    float getPhasicResponse() const { return phasicResponse_; }
    float getTonicResponse() const { return tonicResponse_; }
    double getLastPhasicTime() const { return lastPhasicTime_; }
    
    // Prediction error integration
    void setPredictedReward(float reward) { predictedReward_ = reward; }
    float getPredictionError() const { return predictionError_; }
    void updatePredictionError(float actualReward, float predictedReward);
    
    // Dependencies
    std::vector<std::string> getDependencies() const override;
    bool hasDependency(const std::string& pluginName) const override;
    
    // Metadata
    std::string getCategory() const override { return "Neuromodulation"; }
    std::vector<std::string> getTags() const override { return {"dopamine", "reward", "reinforcement", "prediction-error"}; }
    
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
    float modulationStrength_;
    float modulationRange_;
    float baselineLevel_;
    float timeConstant_;
    float learningRate_;
    float predictionError_;
    float predictedReward_;
    float phasicResponse_;
    float tonicResponse_;
    double lastPhasicTime_;
    mutable std::mutex mutex_;
    double lastUpdateTime_;
    double averageUpdateTime_;
    uint64_t updateCount_;
    std::vector<PluginError> errors_;
    PluginState state_;
    std::map<std::string, std::string> config_;
    
    // Reward history
    std::vector<float> rewardHistory_;
    std::vector<float> predictionErrorHistory_;
    static constexpr size_t HISTORY_SIZE = 100;
};

} // namespace plugins
} // namespace nlm
