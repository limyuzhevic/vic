// BrainPluginRegistry.hpp - Plugin/registry system for extending brain updaters
#pragma once

#include "BrainSystemUpdater.hpp"
#include <memory>
#include <vector>
#include <string>
#include <map>
#include <functional>
#include <iostream>

namespace nlm {

// Plugin registry for brain system updaters
class BrainPluginRegistry {
public:
    using UpdaterFactory = std::function<std::unique_ptr<BrainSystemUpdater>()>;
    using UpdaterPluginFunc = std::function<std::unique_ptr<BrainSystemUpdater>()>;
    
    static BrainPluginRegistry& getInstance();
    
    // Register a new updater plugin
    void registerUpdater(const std::string& name, UpdaterPluginFunc factory);
    
    // Get a factory for a specific updater
    UpdaterFactory getUpdaterFactory(const std::string& name) const;
    
    // List all available updaters
    std::vector<std::string> getAvailableUpdaters() const;
    
    // Create updater from name
    std::unique_ptr<BrainSystemUpdater> createUpdater(const std::string& name);
    
    // Initialize with default updaters
    void initializeDefaultUpdaters();
    
private:
    BrainPluginRegistry();
    ~BrainPluginRegistry() = default;
    
    // Disable copying
    BrainPluginRegistry(const BrainPluginRegistry&) = delete;
    BrainPluginRegistry& operator=(const BrainPluginRegistry&) = delete;
    
    std::map<std::string, UpdaterPluginFunc> updaterFactories_;
};

// User-extensible updater plugin system
class BrainPluginManager {
public:
    BrainPluginManager() = default;
    
    // Load plugins from configuration
    void loadPlugins(const std::string& configPath);
    
    // Add custom updater with lambda/function
    template<typename T, typename... Args>
    void addCustomUpdater(const std::string& name, Args&&... args) {
        auto factory = [args...]() -> std::unique_ptr<BrainSystemUpdater> {
            return std::make_unique<T>(std::forward<Args>(args)...);
        };
        BrainPluginRegistry::getInstance().registerUpdater(name, factory);
    }
    
    // Create updaters from configuration
    void configureFromFile(const std::string& configFile);
    
    // Get all registered updaters
    std::vector<std::string> getRegisteredUpdaters() const {
        return BrainPluginRegistry::getInstance().getAvailableUpdaters();
    }
    
    // Create orchestrator with loaded plugins
    static std::unique_ptr<BrainUpdaterOrchestrator> createOrchestrator();
    
private:
    struct PluginConfig {
        std::string name;
        std::string type;  // "neuron", "memory", "neuromodulation", "custom"
        bool enabled;
        std::map<std::string, std::string> parameters;
    };
};

// Example user-extensible updater plugins

// Custom analysis updater - runs analysis on brain state
class AnalysisUpdater : public BrainSystemUpdater {
public:
    AnalysisUpdater(float analysisInterval = 100.0f) 
        : analysisInterval_(analysisInterval), stepCount_(0) {}
    
    void update(Brain& brain, SimulationStep currentStep,
               Timestamp currentTime, TimestepDuration timestep) override {
        stepCount_++;
        if (stepCount_ >= analysisInterval_) {
            performAnalysis(brain);
            stepCount_ = 0;
        }
    }
    
    std::string getName() const override { return "AnalysisUpdater"; }
    
    virtual void performAnalysis(Brain& brain) {
        // Default analysis - can be overridden by users
        NLM_LOG_INFO("Performing brain state analysis...")
        
        // Calculate statistics
        float avgFiringRate = brain.getAverageFiringRate();
        float eiratio = brain.getExcitationInhibitionRatio();
        size_t totalSpikes = brain.getTotalSpikeCount();
        
        NLM_LOG_INFO("  Average firing rate: " + std::to_string(avgFiringRate) + " Hz");
        NLM_LOG_INFO("  E/I ratio: " + std::to_string(eiratio));
        NLM_LOG_INFO("  Total spikes: " + std::to_string(totalSpikes));
        
        // Check for patterns
        checkForPatterns(brain);
    }
    
protected:
    virtual void checkForPatterns(Brain& brain) {
        // Default pattern checking - override in user code
        if (brain.getAverageFiringRate() > 50.0f) {
            NLM_LOG_INFO("  Pattern detected: High firing rate (>50Hz)");
        }
    }
    
private:
    float analysisInterval_;
    SimulationStep stepCount_;
};

// Custom learning updater - applies reinforcement learning
class LearningUpdater : public BrainSystemUpdater {
public:
    LearningUpdater(float learningRate = 0.01f) : learningRate_(learningRate) {}
    
    void update(Brain& brain, SimulationStep currentStep,
               Timestamp currentTime, TimestepDuration timestep) override {
        // Apply reinforcement learning based on prediction error
        applyReinforcementLearning(brain);
    }
    
    std::string getName() const override { return "LearningUpdater"; }
    
    virtual void applyReinforcementLearning(Brain& brain) {
        // Default RL - override for custom implementations
        if (brain.getPredictionErrorSignal()) {
            float predictionError = brain.getPredictionErrorSignal()->getLevel();
            
            // Adjust synaptic weights based on prediction error
            for (auto& region : brain.getRegions()) {
                for (auto& syn : region->getSynapses()) {
                    if (syn->getPlasticityFlags().reward_modulated) {
                        // Apply reward-modulated weight change
                        float delta = predictionError * learningRate_ * syn->getEligibilityTrace();
                        syn->addToWeight(delta);
                    }
                }
            }
        }
    }
    
private:
    float learningRate_;
};

// Custom debugging updater - provides debug output
class DebugUpdater : public BrainSystemUpdater {
public:
    DebugUpdater(bool detailedOutput = true) : detailedOutput_(detailedOutput) {}
    
    void update(Brain& brain, SimulationStep currentStep,
               Timestamp currentTime, TimestepDuration timestep) override {
        if (shouldDebug(currentStep)) {
            logDebugInfo(brain, currentStep, currentTime);
        }
    }
    
    std::string getName() const override { return "DebugUpdater"; }
    
    virtual bool shouldDebug(SimulationStep step) const {
        // Debug every N steps or at specific milestones
        return (step % 100 == 0) || (step <= 10) || (step >= 9990);
    }
    
    virtual void logDebugInfo(Brain& brain, SimulationStep currentStep, Timestamp currentTime) {
        NLM_LOG_INFO(std::string("=== DEBUG INFO (Step " + std::to_string(currentStep) + ") ==="));
        
        if (detailedOutput_) {
            // Detailed debugging output
            NLM_LOG_INFO("  Regions: " + std::to_string(brain.getRegionCount()));
            NLM_LOG_INFO("  Neurons: " + std::to_string(brain.getTotalNeuronCount()));
            NLM_LOG_INFO("  Synapses: " + std::to_string(brain.getTotalSynapseCount()));
            NLM_LOG_INFO("  Active neurons: " + std::to_string(brain.getActiveNeuronCount()));
            NLM_LOG_INFO("  Firing neurons: " + std::to_string(brain.getFiringNeuronCount()));
            NLM_LOG_INFO("  Total spikes: " + std::to_string(brain.getTotalSpikeCount()));
            
            // Memory system status
            if (brain.getWorkingMemory()) {
                NLM_LOG_INFO("  Working memory traces: " + std::to_string(brain.getWorkingMemory()->getActiveTraces()));
            }
            if (brain.getEpisodicMemory()) {
                NLM_LOG_INFO("  Episodic memory episodes: " + std::to_string(brain.getEpisodicMemory()->getEpisodeCount()));
            }
        }
    }
    
private:
    bool detailedOutput_;
};

} // namespace nlm
