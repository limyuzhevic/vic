// Enhanced AgentBrain.hpp - Improved encapsulation and API design
// Phase 6 Integration - AgentBrain v2.0 with better API

#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <string>
#include <functional>

namespace nlm {

// Forward declarations
class Neuron;

// AgentBrain Configuration
struct AgentBrainConfig {
    // Neuromodulation settings
    bool enableRewardModulation = true;
    bool enableStructuralPlasticity = true;
    bool enableDevelopment = true;
    bool enableCuriosity = true;
    bool enableNoveltyDetection = true;
    
    // Curiosity parameters
    float curiosityThreshold = 0.3f;
    float explorationProbability = 0.2f;
    
    // Novelty parameters
    float noveltyDecayRate = 0.99f;
    
    // Development parameters
    float developmentalDuration = 900.0; // steps
    
    // Performance optimizations
    bool enableCaching = true;
    bool enableBatchProcessing = false;
    
    // Debugging and monitoring
    bool enableDebugLogging = false;
    bool enablePerformanceMetrics = false;
    
    // Advanced features for expert users
    bool enableCustomPlasticityRules = false;
    bool enableAdvancedPrediction = false;
    
    // Custom callbacks for expert users
    std::function<void(const std::vector<float>&)> onSensoryProcessed;
    std::function<void(MotorCommand)> onActionSelected;
    std::function<void(float, float)> onRewardReceived;
};

// Enhanced AgentBrain: Connects NLM brain to the world with improved architecture
// Features:
// - Better encapsulation with private implementation (PIMPL)
// - Configurable behavior through AgentBrainConfig
// - Enhanced neuromodulation and curiosity systems
// - Performance optimizations
// - Advanced features for expert users
class AgentBrain {
public:
    // Constructor with configuration
    explicit AgentBrain(std::shared_ptr<Brain> brain, const AgentBrainConfig& config = AgentBrainConfig());
    ~AgentBrain();
    
    // Disable copying, enable moving
    AgentBrain(const AgentBrain&) = delete;
    AgentBrain& operator=(const AgentBrain&) = delete;
    AgentBrain(AgentBrain&&) noexcept;
    AgentBrain& operator=(AgentBrain&&) noexcept;

    // Configuration methods (fluent API for easy setup)
    AgentBrain& withRewardModulation(bool enable) { config_.enableRewardModulation = enable; return *this; }
    AgentBrain& withStructuralPlasticity(bool enable) { config_.enableStructuralPlasticity = enable; return *this; }
    AgentBrain& withDevelopment(bool enable) { config_.enableDevelopment = enable; return *this; }
    AgentBrain& withCuriosity(bool enable) { config_.enableCuriosity = enable; return *this; }
    AgentBrain& withCuriosityThreshold(float threshold) { config_.curiosityThreshold = threshold; return *this; }
    
    // Initialization
    void initialize(const SimpleWorld& world);
    
    // Core processing methods
    size_t getSensoryInputSize() const;
    size_t getMotorOutputSize() const;
    
    // Main processing pipeline
    void process(const SensoryPercept& percept, double timestep = 0.001);
    MotorCommand getAction() const;
    
    // Neuromodulation methods
    void applyReward(float reward, float predictedReward = 0.0f);
    void updateDevelopment(double timestep);
    
    // State access methods
    float getNeuromodulationLevel() const;
    float getCuriosityLevel() const;
    float getNoveltyLevel() const;
    float getPredictionError() const;
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Performance metrics
    struct PerformanceMetrics {
        size_t sensoryProcessingTime = 0;
        size_t actionSelectionTime = 0;
        size_t neuromodulationTime = 0;
        float memoryUsage = 0.0f;
        size_t neuronGroupsCached = 0;
    };
    
    const PerformanceMetrics& getPerformanceMetrics() const { return metrics_; }
    
    // Brain access
    Brain* getBrain() { return brain_.get(); }
    const Brain* getBrain() const { return brain_.get(); }
    
    // Configuration access
    const AgentBrainConfig& getConfig() const { return config_; }
    
    // Advanced expert methods
    void setCustomPlasticityRule(std::function<float(SynapticWeight)> rule) {
        customPlasticityRule_ = rule;
        config_.enableCustomPlasticityRules = true;
    }
    
    void enableBatchProcessing(bool enable) {
        config_.enableBatchProcessing = enable;
        if (enable && batchBuffer_.empty()) {
            batchBuffer_.reserve(1000); // Pre-allocate buffer
        }
    }
    
    // Callback management for expert users
    void setSensoryCallback(std::function<void(const std::vector<float>&)> callback) {
        config_.onSensoryProcessed = callback;
    }
    
    void setActionCallback(std::function<void(MotorCommand)> callback) {
        config_.onActionSelected = callback;
    }
    
    void setRewardCallback(std::function<void(float, float)> callback) {
        config_.onRewardReceived = callback;
    }
    
    // Debug and monitoring methods
    std::string getDebugInfo() const;
    void enableDebugLogging(bool enable) { config_.enableDebugLogging = enable; }
    void enablePerformanceMetrics(bool enable) { config_.enablePerformanceMetrics = enable; }
    
    // Batch processing for high-performance scenarios
    void startBatchProcessing();
    void addToBatch(const SensoryPercept& percept);
    std::vector<MotorCommand> executeBatch(double timestep = 0.001);
    void endBatchProcessing();
    
    // Memory optimization
    void clearCaches();
    size_t getCacheSize() const { return cachedSensory_.size() + cachedAction_.size(); }
    
    // Reset and lifecycle management
    void reset();
    void cleanup();

private:
    // Pimpl pattern for better encapsulation
    struct Impl;
    Impl* pImpl_;
    
    // Configuration
    AgentBrainConfig config_;
    
    // Core components
    std::shared_ptr<Brain> brain_;
    
    // Performance metrics
    mutable PerformanceMetrics metrics_;
    
    // Custom rules for expert users
    std::function<float(SynapticWeight)> customPlasticityRule_;
    
    // Batch processing support
    std::vector<SensoryPercept> batchBuffer_;
    
    // Caches for performance
    std::vector<float> cachedSensory_;
    MotorCommand cachedAction_;
    
    // Callback functions (stored on config for lifetime management)
    
    // Helper methods
    void validateState() const;
    void updateMetrics(size_t operationTime) const;
    MotorCommand processWithAdvancedFeatures() const;
    
    // Advanced processing methods
    void processSensoryWithCaching(const SensoryPercept& percept);
    void neuromodulateWithCustomRules(float reward, float predictedReward);
    MotorCommand selectActionWithExploration() const;
    
    // Expert features
    float applyCustomPlasticity(SynapticWeight currentWeight) const;
    void executeCustomCallbacks(const std::vector<float>& sensoryData, 
                               MotorCommand action, float reward, float predictedReward);
    
    friend class AgentBrainBuilder; // For fluent API building
};

// Fluent API builder for AgentBrain (expert user convenience)
class AgentBrainBuilder {
public:
    AgentBrainBuilder() = default;
    
    static AgentBrainBuilder create(std::shared_ptr<Brain> brain) {
        AgentBrainBuilder builder;
        builder.config_.brain = brain;
        return builder;
    }
    
    AgentBrainBuilder& withConfig(const AgentBrainConfig& config) {
        config_.brain = brain_;
        config_ = config;
        return *this;
    }
    
    AgentBrainBuilder& enableAllFeatures() {
        config_.enableRewardModulation = true;
        config_.enableStructuralPlasticity = true;
        config_.enableDevelopment = true;
        config_.enableCuriosity = true;
        config_.enableNoveltyDetection = true;
        config_.enableAdvancedPrediction = true;
        config_.enableCustomPlasticityRules = true;
        return *this;
    }
    
    AgentBrainBuilder& optimizeForPerformance() {
        config_.enableCaching = true;
        config_.enableBatchProcessing = true;
        config_.enablePerformanceMetrics = true;
        return *this;
    }
    
    AgentBrain build() {
        return AgentBrain(config_.brain, config_);
    }
    
private:
    AgentBrainConfig config_;
    std::shared_ptr<Brain> brain_;
};

} // namespace nlm
