#pragma once

#include "Plugin.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include <string>
#include <memory>
#include <unordered_map>

namespace nlm {

// Abstract base class for neuromodulator plugins
class NeuromodulatorPlugin : public virtual IPlugin, public virtual IConfigurable {
public:
    virtual ~NeuromodulatorPlugin() = default;
    
    // Plugin category identification
    const char* getCategory() const override { return "Neuromodulator"; }
    
    // Neuromodulator plugin specific operations
    virtual std::unique_ptr<Neuromodulator> createModulator(const std::string& modulatorName = "Default") = 0;
    virtual bool configureModulator(Neuromodulator* modulator, const std::unordered_map<std::string, std::string>& config) = 0;
    virtual void updateModulator(Neuromodulator* modulator, TimestepDuration dt) = 0;
    
    // Neuromodulator-specific configuration parameters
    virtual std::vector<std::string> getModulatorConfigParameters() const {
        return {"baselineLevel", "decayRate", "releaseRate", "peakLevel", "minLevel"};
    }
    
    virtual std::string getModulatorParameterType(const std::string& paramName) const {
        if (paramName == "baselineLevel") return "float";
        if (paramName == "decayRate") return "float";
        if (paramName == "releaseRate") return "float";
        if (paramName == "peakLevel") return "float";
        if (paramName == "minLevel") return "float";
        return "string";
    }
    
    virtual std::string getModulatorParameterDescription(const std::string& paramName) const {
        if (paramName == "baselineLevel") return "Baseline concentration level";
        if (paramName == "decayRate") return "Natural decay rate";
        if (paramName == "releaseRate") return "Recovery rate after perturbation";
        if (paramName == "peakLevel") return "Maximum achievable level";
        if (paramName == "minLevel") return "Minimum level to maintain";
        return "";
    }
};

// Example neuromodulator plugin: DopaminePlugin
class DopaminePlugin : public NeuromodulatorPlugin {
public:
    DopaminePlugin() = default;
    ~DopaminePlugin() override = default;
    
    const PluginMetadata& getMetadata() const override {
        static PluginMetadata metadata = {
            "DopaminePlugin", "1.0.0", "Dopamine neuromodulator implementation for reward signaling",
            "NLM Team", {}, 85, "Neuromodulator", {
                {"baselineLevel", "0.1"}, {"decayRate", "0.5"}, {"releaseRate", "0.2"},
                {"peakLevel", "2.0"}, {"minLevel", "0.01"}
            }
        };
        return metadata;
    }
    
    bool initialize(const std::unordered_map<std::string, std::string>& config) override {
        setState(PluginState::Initialized);
        return true;
    }
    
    void update(TimestepDuration dt) override {
        // Update dopamine levels based on reward signals
        // Apply decay, release, and production dynamics
    }
    
    void shutdown() override {
        setState(PluginState::Unloaded);
    }
    
    PluginState getState() const override {
        return state_;
    }
    
    const char* getName() const override { return "DopaminePlugin"; }
    const char* getVersion() const override { return "1.0.0"; }
    
    bool isValid() const override {
        return state_ == PluginState::Initialized || state_ == PluginState::Running;
    }
    
    bool configure(const std::unordered_map<std::string, std::string>& config) override {
        baselineLevel_ = getFloatFromConfig(config, "baselineLevel", 0.1f);
        decayRate_ = getFloatFromConfig(config, "decayRate", 0.5f);
        releaseRate_ = getFloatFromConfig(config, "releaseRate", 0.2f);
        peakLevel_ = getFloatFromConfig(config, "peakLevel", 2.0f);
        minLevel_ = getFloatFromConfig(config, "minLevel", 0.01f);
        return true;
    }
    
    const std::unordered_map<std::string, std::string>& getConfiguration() const override {
        return config_;
    }
    
    std::vector<std::string> getConfigParameters() const override {
        return getModulatorConfigParameters();
    }
    
    std::string getParameterType(const std::string& paramName) const override {
        return getModulatorParameterType(paramName);
    }
    
    std::string getParameterDescription(const std::string& paramName) const override {
        return getModulatorParameterDescription(paramName);
    }
    
    std::unique_ptr<Neuromodulator> createModulator(const std::string& modulatorName = "Default") override {
        auto modulator = std::make_unique<Dopamine>();
        configureModulator(modulator.get(), config_);
        return modulator;
    }
    
    bool configureModulator(Neuromodulator* modulator, const std::unordered_map<std::string, std::string>& config) override {
        if (!modulator) return false;
        
        if (auto* dopamine = dynamic_cast<Dopamine*>(modulator)) {
            // Configure dopamine-specific properties
            dopamine->setLevel(baselineLevel_);
        }
        return true;
    }
    
    void updateModulator(Neuromodulator* modulator, TimestepDuration dt) override {
        // Update neuromodulator dynamics
        if (modulator && isValid()) {
            float currentLevel = modulator->getLevel();
            
            // Apply natural decay
            float newLevel = currentLevel * (1.0f - decayRate_ * dt);
            
            // Ensure minimum level
            newLevel = std::max(newLevel, minLevel_);
            
            // Apply release dynamics if production occurs
            if (currentLevel < baselineLevel_) {
                newLevel += (baselineLevel_ - currentLevel) * releaseRate_ * dt;
                newLevel = std::min(newLevel, baselineLevel_);
            }
            
            modulator->setLevel(std::min(newLevel, peakLevel_));
        }
    }
    
    void signalReward(float reward) {
        // External reward signaling affects dopamine levels
        if (currentDopamine_) {
            currentDopamine_->setLevel(std::min(currentDopamine_->getLevel() + reward * 0.5f, peakLevel_));
        }
    }
    
    float getCpuUsage() const override { return decayRate_ * 0.4f; }
    size_t getMemoryUsage() const override { return sizeof(DopaminePlugin); }
    size_t getActiveObjects() const override { return 1; }
    float getPerformanceScore() const override { return 1.0f / (1.0f + decayRate_); }
    void resetPerformanceMetrics() override {}
    
    bool saveState(const std::string& filepath) const override { return false; }
    bool loadState(const std::string& filepath) override { return false; }
    bool hasState() const override { return false; }
    void clearState() override {}
    std::string getStateSignature() const override { return "DopaminePlugin_v1"; }
    
private:
    PluginState state_ = PluginState::Unloaded;
    float baselineLevel_ = 0.1f;
    float decayRate_ = 0.5f;
    float releaseRate_ = 0.2f;
    float peakLevel_ = 2.0f;
    float minLevel_ = 0.01f;
    Neuromodulator* currentDopamine_ = nullptr;
    std::unordered_map<std::string, std::string> config_;
    
    float getFloatFromConfig(const std::unordered_map<std::string, std::string>& config,
                            const std::string& key, float defaultValue) const {
        auto it = config.find(key);
        return it != config.end() ? std::stof(it->second) : defaultValue;
    }
    
    void setState(PluginState state) {
        state_ = state;
    }
};

// Example neuromodulator plugin: AcetylcholinePlugin
class AcetylcholinePlugin : public NeuromodulatorPlugin {
public:
    AcetylcholinePlugin() = default;
    ~AcetylcholinePlugin() override = default;
    
    const PluginMetadata& getMetadata() const override {
        static PluginMetadata metadata = {
            "AcetylcholinePlugin", "1.0.0", "Acetylcholine neuromodulator for attention and memory",
            "NLM Team", {}, 75, "Neuromodulator", {
                {"baselineLevel", "0.2"}, {"decayRate", "0.4"}, {"releaseRate", "0.3"},
                {"peakLevel", "1.5"}, {"minLevel", "0.01"}
            }
        };
        return metadata;
    }
    
    bool initialize(const std::unordered_map<std::string, std::string>& config) override {
        setState(PluginState::Initialized);
        return true;
    }
    
    void update(TimestepDuration dt) override {
        // Update acetylcholine levels based on attention signals
        // Modulates plasticity and memory consolidation
    }
    
    void shutdown() override {
        setState(PluginState::Unloaded);
    }
    
    PluginState getState() const override {
        return state_;
    }
    
    const char* getName() const override { return "AcetylcholinePlugin"; }
    const char* getVersion() const override { return "1.0.0"; }
    
    bool isValid() const override {
        return state_ == PluginState::Initialized || state_ == PluginState::Running;
    }
    
    bool configure(const std::unordered_map<std::string, std::string>& config) override {
        baselineLevel_ = getFloatFromConfig(config, "baselineLevel", 0.2f);
        decayRate_ = getFloatFromConfig(config, "decayRate", 0.4f);
        releaseRate_ = getFloatFromConfig(config, "releaseRate", 0.3f);
        peakLevel_ = getFloatFromConfig(config, "peakLevel", 1.5f);
        minLevel_ = getFloatFromConfig(config, "minLevel", 0.01f);
        return true;
    }
    
    const std::unordered_map<std::string, std::string>& getConfiguration() const override {
        return config_;
    }
    
    std::vector<std::string> getConfigParameters() const override {
        return getModulatorConfigParameters();
    }
    
    std::string getParameterType(const std::string& paramName) const override {
        return getModulatorParameterType(paramName);
    }
    
    std::string getParameterDescription(const std::string& paramName) const override {
        return getModulatorParameterDescription(paramName);
    }
    
    std::unique_ptr<Neuromodulator> createModulator(const std::string& modulatorName = "Default") override {
        auto modulator = std::make_unique<Acetylcholine>();
        configureModulator(modulator.get(), config_);
        return modulator;
    }
    
    bool configureModulator(Neuromodulator* modulator, const std::unordered_map<std::string, std::string>& config) override {
        if (!modulator) return false;
        
        if (auto* ach = dynamic_cast<Acetylcholine*>(modulator)) {
            // Configure acetylcholine-specific properties
            modulator->setLevel(baselineLevel_);
        }
        return true;
    }
    
    void updateModulator(Neuromodulator* modulator, TimestepDuration dt) override {
        // Update neuromodulator dynamics
        if (modulator && isValid()) {
            float currentLevel = modulator->getLevel();
            
            // Apply attention-based modulation
            float newLevel = currentLevel * (1.0f - decayRate_ * dt);
            newLevel = std::max(newLevel, minLevel_);
            
            if (currentLevel < baselineLevel_) {
                newLevel += (baselineLevel_ - currentLevel) * releaseRate_ * dt;
                newLevel = std::min(newLevel, baselineLevel_);
            }
            
            modulator->setLevel(std::min(newLevel, peakLevel_));
        }
    }
    
    float getCpuUsage() const override { return decayRate_ * 0.3f; }
    size_t getMemoryUsage() const override { return sizeof(AcetylcholinePlugin); }
    size_t getActiveObjects() const override { return 1; }
    float getPerformanceScore() const override { return 1.0f / (1.0f + decayRate_); }
    void resetPerformanceMetrics() override {}
    
    bool saveState(const std::string& filepath) const override { return false; }
    bool loadState(const std::string& filepath) override { return false; }
    bool hasState() const override { return false; }
    void clearState() override {}
    std::string getStateSignature() const override { return "AcetylcholinePlugin_v1"; }
    
private:
    PluginState state_ = PluginState::Unloaded;
    float baselineLevel_ = 0.2f;
    float decayRate_ = 0.4f;
    float releaseRate_ = 0.3f;
    float peakLevel_ = 1.5f;
    float minLevel_ = 0.01f;
    std::unordered_map<std::string, std::string> config_;
    
    float getFloatFromConfig(const std::unordered_map<std::string, std::string>& config,
                            const std::string& key, float defaultValue) const {
        auto it = config.find(key);
        return it != config.end() ? std::stof(it->second) : defaultValue;
    }
    
    void setState(PluginState state) {
        state_ = state;
    }
};

// Note: Acetylcholine class would need to be included from neuromodulation module
class Acetylcholine {
public:
    void setLevel(float level) {}
};

} // namespace nlm