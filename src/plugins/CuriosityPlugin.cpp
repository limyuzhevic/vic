#pragma once

#include "IPlugin.hpp"
#include "INeuromodulationPlugin.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include <vector>
#include <memory>
#include <mutex>
#include <cmath>

namespace nlm {
namespace plugins {

// CuriosityPlugin implementation
class CuriosityPlugin : public INeuromodulationPlugin {
public:
    CuriosityPlugin();
    ~CuriosityPlugin() override;
    
    // IPlugin interface
    const char* getName() const override { return "CuriosityPlugin"; }
    const char* getVersion() const override { return "1.0.0"; }
    const char* getDescription() const override { return "Curiosity Neuromodulation Plugin - intrinsic motivation and exploration drive"; }
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
    std::vector<std::string> getTargetSystems() const override { return {"plasticity", "exploration"}; }
    bool affectsPlasticity() const override { return true; }
    bool affectsExcitability() const override { return false; }
    bool affectsInhibition() const override { return false; }
    
    // Curiosity calculation
    void updateCuriosity(const std::vector<float>& noveltySignals, float confidence = 1.0f);
    void resetCuriosity();
    
    // Curiosity components
    float getNoveltySensitivity() const { return noveltySensitivity_; }
    void setNoveltySensitivity(float sensitivity) { noveltySensitivity_ = std::max(0.0f, std::min(1.0f, sensitivity)); }
    
    float getExplorationDrive() const { return explorationDrive_; }
    void setExplorationDrive(float drive) { explorationDrive_ = std::max(0.0f, std::min(1.0f, drive)); }
    
    float getInformationGain() const { return informationGain_; }
    void setInformationGain(float gain) { informationGain_ = std::max(0.0f, std::min(1.0f, gain)); }
    
    // Exploration behavior
    float getExplorationRate() const { return explorationRate_; }
    void setExplorationRate(float rate) { explorationRate_ = std::max(0.0f, std::min(1.0f, rate)); }
    
    float getExploitationRate() const { return exploitationRate_; }
    void setExploitationRate(float rate) { exploitationRate_ = std::max(0.0f, std::min(1.0f, rate)); }
    
    // Intrinsic motivation
    float getIntrinsicMotivation() const { return intrinsicMotivation_; }
    void setIntrinsicMotivation(float motivation) { intrinsicMotivation_ = std::max(0.0f, std::min(1.0f, motivation)); }
    
    // Dependencies
    std::vector<std::string> getDependencies() const override;
    bool hasDependency(const std::string& pluginName) const override;
    
    // Metadata
    std::string getCategory() const override { return "Neuromodulation"; }
    std::vector<std::string> getTags() const override { return {"curiosity", "exploration", "intrinsic", "motivation", "novelty"}; }
    
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
    float noveltySensitivity_;
    float explorationDrive_;
    float informationGain_;
    float explorationRate_;
    float exploitationRate_;
    float intrinsicMotivation_;
    mutable std::mutex mutex_;
    double lastUpdateTime_;
    double averageUpdateTime_;
    uint64_t updateCount_;
    std::vector<PluginError> errors_;
    PluginState state_;
    std::map<std::string, std::string> config_;
    
    // Curiosity calculation components
    float currentCuriosity_;
    float maxCuriosity_;
    double lastUpdateTime_;
    std::vector<float> noveltyHistory_;
    std::vector<float> confidenceHistory_;
    static constexpr size_t HISTORY_SIZE = 100;
};

} // namespace plugins
} // namespace nlm
