// Example plugins for the NLM plugin architecture
// Demonstrates different plugin types and usage patterns

#include "RegionPlugin.hpp"
#include "PlasticityPlugin.hpp"
#include "NeuromodulatorPlugin.hpp"
#include "MemoryPlugin.hpp"
#include "EnvironmentPlugin.hpp"
#include <iostream>

// ============================================================================
// Example 1: Custom Brain Region Plugin
// ============================================================================

class CustomRegionPlugin : public nlm::RegionPlugin {
public:
    CustomRegionPlugin() = default;
    ~CustomRegionPlugin() override = default;
    
    const nlm::PluginMetadata& getMetadata() const override {
        static nlm::PluginMetadata metadata = {
            "CustomRegionPlugin", "1.0.0", "Custom brain region with specialized connectivity",
            "Developer Name", {}, 70, "BrainRegion", {
                {"regionSize", "200"}, {"neuronType", "Excitatory"}, {"connectionDensity", "0.3"}
            }
        };
        return metadata;
    }
    
    bool initialize(const std::unordered_map<std::string, std::string>& config) override {
        setState(nlm::PluginState::Initialized);
        return true;
    }
    
    void update(nlm::TimestepDuration dt) override {
        // Custom region update logic
    }
    
    void shutdown() override {
        setState(nlm::PluginState::Unloaded);
    }
    
    nlm::PluginState getState() const override {
        return state_;
    }
    
    const char* getName() const override { return "CustomRegionPlugin"; }
    const char* getVersion() const override { return "1.0.0"; }
    
    bool isValid() const override {
        return state_ == nlm::PluginState::Initialized || state_ == nlm::PluginState::Running;
    }
    
    bool configure(const std::unordered_map<std::string, std::string>& config) override {
        regionSize_ = getIntFromConfig(config, "regionSize", 200);
        neuronType_ = getStringFromConfig(config, "neuronType", "Excitatory");
        connectionDensity_ = getFloatFromConfig(config, "connectionDensity", 0.3f);
        return true;
    }
    
    const std::unordered_map<std::string, std::string>& getConfiguration() const override {
        return config_;
    }
    
    std::vector<std::string> getConfigParameters() const override {
        return {"regionSize", "neuronType", "connectionDensity"};
    }
    
    std::string getParameterType(const std::string& paramName) const override {
        if (paramName == "regionSize") return "int";
        if (paramName == "neuronType") return "string";
        if (paramName == "connectionDensity") return "float";
        return "string";
    }
    
    std::string getParameterDescription(const std::string& paramName) const override {
        if (paramName == "regionSize") return "Number of neurons in the region";
        if (paramName == "neuronType") return "Type of neurons in this region";
        if (paramName == "connectionDensity") return "Density of connections between neurons";
        return "";
    }
    
    std::unique_ptr<nlm::NeuralRegion> createRegion(nlm::RegionId id, const std::string& name = "") override {
        auto region = std::make_unique<nlm::NeuralRegion>(id, name);
        configureRegion(region.get(), config_);
        return region;
    }
    
    bool configureRegion(nlm::NeuralRegion* region, const std::unordered_map<std::string, std::string>& config) override {
        if (!region) return false;
        
        // Configure with custom neuron type
        // Note: This would require access to internal region configuration
        // In a real implementation, you would use the region's internal API
        
        // Set custom name
        region->setName(config.find("regionName") != config.end() ? config.at("regionName") : "Custom Region");
        
        return true;
    }
    
    void updateRegion(nlm::NeuralRegion* region, nlm::TimestepDuration dt) override {
        // Custom region dynamics based on configuration
        if (region && isValid()) {
            // Apply custom connectivity pattern
            // Implement specialized neural dynamics
        }
    }
    
    float getCpuUsage() const override { return connectionDensity_ * 0.2f; }
    size_t getMemoryUsage() const override { return sizeof(CustomRegionPlugin); }
    size_t getActiveObjects() const override { return 1; }
    float getPerformanceScore() const override { return 1.0f / (1.0f + regionSize_ / 1000.0f); }
    void resetPerformanceMetrics() override {}
    
    bool saveState(const std::string& filepath) const override { return false; }
    bool loadState(const std::string& filepath) override { return false; }
    bool hasState() const override { return false; }
    void clearState() override {}
    std::string getStateSignature() const override { return "CustomRegionPlugin_v1"; }
    
private:
    nlm::PluginState state_ = nlm::PluginState::Unloaded;
    int regionSize_ = 200;
    std::string neuronType_ = "Excitatory";
    float connectionDensity_ = 0.3f;
    std::unordered_map<std::string, std::string> config_;
    
    int getIntFromConfig(const std::unordered_map<std::string, std::string>& config, 
                        const std::string& key, int defaultValue) const {
        auto it = config.find(key);
        return it != config.end() ? std::stoi(it->second) : defaultValue;
    }
    
    float getFloatFromConfig(const std::unordered_map<std::string, std::string>& config,
                            const std::string& key, float defaultValue) const {
        auto it = config.find(key);
        return it != config.end() ? std::stof(it->second) : defaultValue;
    }
    
    std::string getStringFromConfig(const std::unordered_map<std::string, std::string>& config,
                                   const std::string& key, const std::string& defaultValue) const {
        auto it = config.find(key);
        return it != config.end() ? it->second : defaultValue;
    }
    
    void setState(nlm::PluginState state) {
        state_ = state;
    }
};

// ============================================================================
// Example 2: Custom Plasticity Rule Plugin
// ============================================================================

class AdaptivePlasticityPlugin : public nlm::PlasticityPlugin {
public:
    AdaptivePlasticityPlugin() = default;
    ~AdaptivePlasticityPlugin() override = default;
    
    const nlm::PluginMetadata& getMetadata() const override {
        static nlm::PluginMetadata metadata = {
            "AdaptivePlasticityPlugin", "1.0.0", "Adaptive plasticity with dynamic learning rates",
            "Developer Name", {"BasicSTDP"}, 75, "PlasticityRule", {
                {"baseLearningRate", "0.01"}, {"adaptationFactor", "0.1"}, {"maxLearningRate", "0.5"}
            }
        };
        return metadata;
    }
    
    bool initialize(const std::unordered_map<std::string, std::string>& config) override {
        setState(nlm::PluginState::Initialized);
        return true;
    }
    
    void update(nlm::TimestepDuration dt) override {
        // Update adaptive learning rates based on performance
        adaptLearningRates(dt);
    }
    
    void shutdown() override {
        setState(nlm::PluginState::Unloaded);
    }
    
    nlm::PluginState getState() const override {
        return state_;
    }
    
    const char* getName() const override { return "AdaptivePlasticityPlugin"; }
    const char* getVersion() const override { return "1.0.0"; }
    
    bool isValid() const override {
        return state_ == nlm::PluginState::Initialized || state_ == nlm::PluginState::Running;
    }
    
    bool configure(const std::unordered_map<std::string, std::string>& config) override {
        baseLearningRate_ = getFloatFromConfig(config, "baseLearningRate", 0.01f);
        adaptationFactor_ = getFloatFromConfig(config, "adaptationFactor", 0.1f);
        maxLearningRate_ = getFloatFromConfig(config, "maxLearningRate", 0.5f);
        minLearningRate_ = getFloatFromConfig(config, "minLearningRate", 0.001f);
        return true;
    }
    
    const std::unordered_map<std::string, std::string>& getConfiguration() const override {
        return config_;
    }
    
    std::vector<std::string> getConfigParameters() const override {
        return {"baseLearningRate", "adaptationFactor", "maxLearningRate", "minLearningRate"};
    }
    
    std::string getParameterType(const std::string& paramName) const override {
        return "float";
    }
    
    std::string getParameterDescription(const std::string& paramName) const override {
        if (paramName == "baseLearningRate") return "Base learning rate for plasticity";
        if (paramName == "adaptationFactor") return "Factor controlling learning rate adaptation";
        if (paramName == "maxLearningRate") return "Maximum allowed learning rate";
        if (paramName == "minLearningRate") return "Minimum allowed learning rate";
        return "";
    }
    
    std::unique_ptr<nlm::PlasticityRule> createRule(const std::string& ruleName = "Default") override {
        auto rule = std::make_unique<nlm::STDPPlasticityRule>();
        configureRule(rule.get(), config_);
        return rule;
    }
    
    bool configureRule(nlm::PlasticityRule* rule, const std::unordered_map<std::string, std::string>& config) override {
        if (!rule) return false;
        
        if (auto* stdpRule = dynamic_cast<nlm::STDPPlasticityRule*>(rule)) {
            // Configure STDP rule with adaptive parameters
            float currentRate = getCurrentLearningRate();
            // Apply adaptive learning rate to the rule
        }
        return true;
    }
    
    void updateRule(nlm::PlasticityRule* rule, nlm::TimestepDuration dt) override {
        // Update rule with adaptive learning rates
        adaptLearningRates(dt);
        
        if (rule && isValid()) {
            // Apply adaptive plasticity dynamics
        }
    }
    
    void adaptLearningRates(nlm::TimestepDuration dt) {
        // Adaptive learning rate based on recent performance
        float performance = getRecentPerformance();
        float targetRate = baseLearningRate_;
        
        if (performance > 0.8f) {
            targetRate = std::min(maxLearningRate_, baseLearningRate_ * (1.0f + adaptationFactor_));
        } else if (performance < 0.5f) {
            targetRate = std::max(minLearningRate_, baseLearningRate_ * (1.0f - adaptationFactor_));
        }
        
        currentLearningRate_ = targetRate;
    }
    
    float getRecentPerformance() const {
        // In a real implementation, this would compute performance based on simulation results
        // For example: error reduction, learning progress, stability metrics
        return 0.7f; // Placeholder
    }
    
    float getCurrentLearningRate() const {
        return currentLearningRate_;
    }
    
    float getCpuUsage() const override { return adaptationFactor_ * 0.3f; }
    size_t getMemoryUsage() const override { return sizeof(AdaptivePlasticityPlugin); }
    size_t getActiveObjects() const override { return 1; }
    float getPerformanceScore() const override { return 1.0f / (1.0f + adaptationFactor_); }
    void resetPerformanceMetrics() override {}
    
    bool saveState(const std::string& filepath) const override { return false; }
    bool loadState(const std::string& filepath) override { return false; }
    bool hasState() const override { return false; }
    void clearState() override {}
    std::string getStateSignature() const override { return "AdaptivePlasticityPlugin_v1"; }
    
private:
    nlm::PluginState state_ = nlm::PluginState::Unloaded;
    float baseLearningRate_ = 0.01f;
    float adaptationFactor_ = 0.1f;
    float maxLearningRate_ = 0.5f;
    float minLearningRate_ = 0.001f;
    float currentLearningRate_ = 0.01f;
    std::unordered_map<std::string, std::string> config_;
    
    float getFloatFromConfig(const std::unordered_map<std::string, std::string>& config,
                            const std::string& key, float defaultValue) const {
        auto it = config.find(key);
        return it != config.end() ? std::stof(it->second) : defaultValue;
    }
    
    void setState(nlm::PluginState state) {
        state_ = state;
    }
};

// ============================================================================
// Example 3: Custom Neuromodulator Plugin
// ============================================================================

class StressNeuromodulatorPlugin : public nlm::NeuromodulatorPlugin {
public:
    StressNeuromodulatorPlugin() = default;
    ~StressNeuromodulatorPlugin() override = default;
    
    const nlm::PluginMetadata& getMetadata() const override {
        static nlm::PluginMetadata metadata = {
            "StressNeuromodulatorPlugin", "1.0.0", "Stress response neuromodulator for behavioral adaptation",
            "Developer Name", {}, 80, "Neuromodulator", {
                {"baselineStress", "0.3"}, {"stressSensitivity", "0.5"}, {"recoveryRate", "0.2"}
            }
        };
        return metadata;
    }
    
    bool initialize(const std::unordered_map<std::string, std::string>& config) override {
        setState(nlm::PluginState::Initialized);
        return true;
    }
    
    void update(nlm::TimestepDuration dt) override {
        // Update stress levels based on environmental demands
        updateStressDynamics(dt);
        applyStressEffects();
    }
    
    void shutdown() override {
        setState(nlm::PluginState::Unloaded);
    }
    
    nlm::PluginState getState() const override {
        return state_;
    }
    
    const char* getName() const override { return "StressNeuromodulatorPlugin"; }
    const char* getVersion() const override { return "1.0.0"; }
    
    bool isValid() const override {
        return state_ == nlm::PluginState::Initialized || state_ == nlm::PluginState::Running;
    }
    
    bool configure(const std::unordered_map<std::string, std::string>& config) override {
        baselineStress_ = getFloatFromConfig(config, "baselineStress", 0.3f);
        stressSensitivity_ = getFloatFromConfig(config, "stressSensitivity", 0.5f);
        recoveryRate_ = getFloatFromConfig(config, "recoveryRate", 0.2f);
        peakStress_ = getFloatFromConfig(config, "peakStress", 2.0f);
        stressThreshold_ = getFloatFromConfig(config, "stressThreshold", 1.0f);
        return true;
    }
    
    const std::unordered_map<std::string, std::string>& getConfiguration() const override {
        return config_;
    }
    
    std::vector<std::string> getConfigParameters() const override {
        return {"baselineStress", "stressSensitivity", "recoveryRate", "peakStress", "stressThreshold"};
    }
    
    std::string getParameterType(const std::string& paramName) const override {
        return "float";
    }
    
    std::string getParameterDescription(const std::string& paramName) const override {
        if (paramName == "baselineStress") return "Baseline stress level";
        if (paramName == "stressSensitivity") return "Sensitivity to stress-inducing stimuli";
        if (paramName == "recoveryRate") return "Rate of stress recovery";
        if (paramName == "peakStress") return "Maximum achievable stress level";
        if (paramName == "stressThreshold") return "Threshold for stress response activation";
        return "";
    }
    
    std::unique_ptr<nlm::Neuromodulator> createModulator(const std::string& modulatorName = "Default") override {
        auto modulator = std::make_unique<nlm::Dopamine>();
        configureModulator(modulator.get(), config_);
        return modulator;
    }
    
    bool configureModulator(nlm::Neuromodulator* modulator, const std::unordered_map<std::string, std::string>& config) override {
        if (!modulator) return false;
        
        // Configure dopamine with stress-specific properties
        float initialLevel = baselineStress_ * stressSensitivity_;
        modulator->setLevel(initialLevel);
        
        // Store reference for stress effects
        currentModulator_ = modulator;
        
        return true;
    }
    
    void updateModulator(nlm::Neuromodulator* modulator, nlm::TimestepDuration dt) override {
        // Update stress dynamics
        updateStressDynamics(dt);
        
        // Apply stress effects to neuromodulator
        if (modulator && isValid()) {
            applyStressEffects();
        }
    }
    
    void updateStressDynamics(nlm::TimestepDuration dt) {
        // Stress dynamics based on environmental demands and recovery
        float currentStress = stressLevel_;
        
        // Natural decay towards baseline
        float decay = (currentStress - baselineStress_) * recoveryRate_ * dt;
        stressLevel_ = currentStress - decay;
        
        // Apply stress response if threshold exceeded
        if (currentStress > stressThreshold_) {
            stressLevel_ = std::min(stressLevel_ + stressSensitivity_ * dt * 0.5f, peakStress_);
        }
        
        // Ensure minimum stress level
        stressLevel_ = std::max(stressLevel_, baselineStress_);
    }
    
    void applyStressEffects() {
        if (!currentModulator_) return;
        
        float stressMultiplier = 1.0f + (stressLevel_ / stressThreshold_) * 0.5f;
        
        // Stress affects plasticity
        if (auto* dopamine = dynamic_cast<nlm::Dopamine*>(currentModulator_)) {
            float plasticityEffect = stressMultiplier * 0.5f; // Stress enhances plasticity
            // In real implementation: dopamine->applyPlasticityEffect(plasticityEffect);
        }
        
        // Stress affects motivation and learning
        if (stressLevel_ > stressThreshold_) {
            // High stress: focused but reduced exploration
            // In real implementation: apply cognitive effects
        }
    }
    
    void increaseStress(float amount) {
        stressLevel_ = std::min(stressLevel_ + amount, peakStress_);
    }
    
    void decreaseStress(float amount) {
        stressLevel_ = std::max(stressLevel_ - amount, baselineStress_);
    }
    
    float getCurrentStress() const {
        return stressLevel_;
    }
    
    float getCpuUsage() const override { return stressSensitivity_ * 0.4f; }
    size_t getMemoryUsage() const override { return sizeof(StressNeuromodulatorPlugin); }
    size_t getActiveObjects() const override { return 1; }
    float getPerformanceScore() const override { return 1.0f / (1.0f + stressLevel_); }
    void resetPerformanceMetrics() override {}
    
    bool saveState(const std::string& filepath) const override { return false; }
    bool loadState(const std::string& filepath) override { return false; }
    bool hasState() const override { return false; }
    void clearState() override {}
    std::string getStateSignature() const override { return "StressNeuromodulatorPlugin_v1"; }
    
private:
    nlm::PluginState state_ = nlm::PluginState::Unloaded;
    float baselineStress_ = 0.3f;
    float stressSensitivity_ = 0.5f;
    float recoveryRate_ = 0.2f;
    float peakStress_ = 2.0f;
    float stressThreshold_ = 1.0f;
    float stressLevel_ = 0.3f;
    nlm::Neuromodulator* currentModulator_ = nullptr;
    std::unordered_map<std::string, std::string> config_;
    
    float getFloatFromConfig(const std::unordered_map<std::string, std::string>& config,
                            const std::string& key, float defaultValue) const {
        auto it = config.find(key);
        return it != config.end() ? std::stof(it->second) : defaultValue;
    }
    
    void setState(nlm::PluginState state) {
        state_ = state;
    }
};

// ============================================================================
// Example 4: Custom Memory System Plugin
// ============================================================================

class EpisodicMemoryPlugin : public nlm::MemoryPlugin {
public:
    EpisodicMemoryPlugin() = default;
    ~EpisodicMemoryPlugin() override = default;
    
    const nlm::PluginMetadata& getMetadata() const override {
        static nlm::PluginMetadata metadata = {
            "EpisodicMemoryPlugin", "1.0.0", "Episodic memory for experience recording and replay",
            "Developer Name", {}, 70, "Memory", {
                {"memoryCapacity", "1000"}, {"replayFrequency", "10.0"}, {"decayRate", "0.05"}
            }
        };
        return metadata;
    }
    
    bool initialize(const std::unordered_map<std::string, std::string>& config) override {
        setState(nlm::PluginState::Initialized);
        return true;
    }
    
    void update(nlm::TimestepDuration dt) override {
        // Update memory consolidation and replay
        updateMemoryConsolidation(dt);
        updateMemoryReplay(dt);
    }
    
    void shutdown() override {
        setState(nlm::PluginState::Unloaded);
    }
    
    nlm::PluginState getState() const override {
        return state_;
    }
    
    const char* getName() const override { return "EpisodicMemoryPlugin"; }
    const char* getVersion() const override { return "1.0.0"; }
    
    bool isValid() const override {
        return state_ == nlm::PluginState::Initialized || state_ == nlm::PluginState::Running;
    }
    
    bool configure(const std::unordered_map<std::string, std::string>& config) override {
        capacity_ = getIntFromConfig(config, "memoryCapacity", 1000);
        replayFrequency_ = getFloatFromConfig(config, "replayFrequency", 10.0f);
        decayRate_ = getFloatFromConfig(config, "decayRate", 0.05f);
        consolidationRate_ = getFloatFromConfig(config, "consolidationRate", 0.3f);
        return true;
    }
    
    const std::unordered_map<std::string, std::string>& getConfiguration() const override {
        return config_;
    }
    
    std::vector<std::string> getConfigParameters() const override {
        return {"memoryCapacity", "replayFrequency", "decayRate", "consolidationRate"};
    }
    
    std::string getParameterType(const std::string& paramName) const override {
        if (paramName == "memoryCapacity") return "int";
        return "float";
    }
    
    std::string getParameterDescription(const std::string& paramName) const override {
        if (paramName == "memoryCapacity") return "Maximum number of episodic memories";
        if (paramName == "replayFrequency") return "Frequency of memory replay events";
        if (paramName == "decayRate") return "Decay rate for episodic memories";
        if (paramName == "consolidationRate") return "Rate of memory consolidation";
        return "";
    }
    
    std::unique_ptr<nlm::NeuralWorkingMemory> createWorkingMemory(const std::string& memoryName = "Default") override {
        auto memory = std::make_unique<nlm::NeuralWorkingMemory>(capacity_);
        memory->setDecayRate(decayRate_);
        memory->setLearningRate(consolidationRate_);
        memory->setName(memoryName + "_Episodic");
        
        // Configure memory-specific parameters
        // In real implementation: memory->setReplayFrequency(replayFrequency_);
        
        setWorkingMemory(std::move(memory));
        return getWorkingMemory();
    }
    
    bool configureMemory(nlm::NeuralWorkingMemory* memory, const std::unordered_map<std::string, std::string>& config) override {
        if (!memory) return false;
        
        memory->setCapacity(capacity_);
        memory->setDecayRate(decayRate_);
        memory->setLearningRate(consolidationRate_);
        
        if (config.find("memoryName") != config.end()) {
            memory->setName(config.at("memoryName") + "_Episodic");
        }
        
        return true;
    }
    
    void updateMemory(nlm::NeuralWorkingMemory* memory, nlm::TimestepDuration dt) override {
        // Update episodic memory dynamics
        if (memory) {
            updateMemoryConsolidation(memory, dt);
            updateMemoryReplay(dt);
        }
    }
    
    void updateMemoryConsolidation(nlm::NeuralWorkingMemory* memory, nlm::TimestepDuration dt) {
        if (!memory) return;
        
        // Consolidation: transfer recent memories to long-term storage
        // This is a simplified implementation
        // In real implementation: perform actual memory consolidation algorithms
    }
    
    void updateMemoryReplay(nlm::TimestepDuration dt) {
        // Replay mechanism for episodic memory strengthening
        replayTimer_ += dt;
        
        if (replayTimer_ >= replayFrequency_) {
            // Trigger memory replay event
            triggerMemoryReplay();
            replayTimer_ = 0.0f;
        }
    }
    
    void triggerMemoryReplay() {
        // Replay selected episodic memories for consolidation
        // In real implementation: select and replay memories
        // replayCount_++;
    }
    
    float getConsolidationProgress() const {
        return consolidationProgress_;
    }
    
    float getCpuUsage() const override { return decayRate_ * 0.3f + replayTimer_ / replayFrequency_ * 0.2f; }
    size_t getMemoryUsage() const override { 
        size_t total = sizeof(EpisodicMemoryPlugin);
        if (workingMemory_) {
            total += workingMemory_->getMemoryUsage();
        }
        return total;
    }
    size_t getActiveObjects() const override { return workingMemory_ ? workingMemory_->getItemCount() : 0; }
    float getPerformanceScore() const override { return 1.0f / (1.0f + decayRate_ + replayTimer_ / replayFrequency_ * 0.5f); }
    void resetPerformanceMetrics() override {}
    
    bool saveState(const std::string& filepath) const override {
        if (!workingMemory_) return false;
        // Serialize episodic memory state
        // In real implementation: save memory contents to file
        return false;
    }
    
    bool loadState(const std::string& filepath) override {
        if (!workingMemory_) return false;
        // Deserialize episodic memory state
        // In real implementation: load memory contents from file
        return false;
    }
    
    bool hasState() const override {
        return workingMemory_ && workingMemory_->getItemCount() > 0;
    }
    
    void clearState() override {
        if (workingMemory_) {
            workingMemory_->clear();
        }
        consolidationProgress_ = 0.0f;
        replayTimer_ = 0.0f;
    }
    
    std::string getStateSignature() const override {
        return "EpisodicMemoryPlugin_v1_" + std::to_string(workingMemory_ ? workingMemory_->getItemCount() : 0);
    }
    
    void setWorkingMemory(std::unique_ptr<nlm::NeuralWorkingMemory> memory) {
        MemoryPlugin::setWorkingMemory(std::move(memory));
    }
    
private:
    nlm::PluginState state_ = nlm::PluginState::Unloaded;
    int capacity_ = 1000;
    float replayFrequency_ = 10.0f;
    float decayRate_ = 0.05f;
    float consolidationRate_ = 0.3f;
    float consolidationProgress_ = 0.0f;
    float replayTimer_ = 0.0f;
    int replayCount_ = 0;
};