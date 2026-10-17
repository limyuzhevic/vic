// Improved Agent Brain Architecture - Refactored for Better Modularity
#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../development/DevelopmentSystem.hpp"
#include <memory>
#include <vector>
#include <unordered_map>

namespace nlm {

// Agent Configuration Structure
struct AgentConfig {
    // Sensory configuration
    bool enableVision = true;
    bool enableTouch = true;
    bool enableInternal = true;
    bool enableProprioception = true;
    
    // Motor configuration
    bool enableCuriosityExploration = true;
    float curiosityThreshold = 0.3f;
    float explorationChance = 0.3f;
    
    // Neuromodulation configuration
    bool enableRewardModulation = true;
    bool enableNoveltyDetection = true;
    bool enablePredictionError = true;
    
    // Learning configuration
    bool enableStructuralPlasticity = true;
    bool enableDevelopment = true;
    double developmentTimeScale = 1.0;
    
    // Performance configuration
    bool enableVectorization = true;
    bool enableParallelProcessing = false;
    
    // Memory configuration
    bool enableWorkingMemory = true;
    bool enableEpisodicMemory = true;
    
    AgentConfig() = default;
};

// Agent Statistics Structure
struct AgentStats {
    size_t totalSteps = 0;
    size_t totalSpikes = 0;
    float totalReward = 0.0f;
    float avgFiringRate = 0.0f;
    float noveltyLevel = 0.0f;
    float curiosityLevel = 0.0f;
    float dopamineLevel = 0.0f;
    float predictionError = 0.0f;
    double developmentalAge = 0.0;
    DevelopmentalStage developmentalStage = DevelopmentalStage::Initial;
    size_t memoryEpisodesStored = 0;
    size_t workingMemoryTraces = 0;
    
    AgentStats() = default;
};

// Enhanced AgentBrain with better architecture
class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain);
    ~AgentBrain();
    
    // Core agent functions
    void initialize(const SimpleWorld& world);
    void update(double timestep);
    
    // Sensory processing
    void processSensoryInput(const SensoryPercept& percept);
    
    // Motor output
    MotorCommand getMotorCommand();
    
    // Neuromodulation
    void applyRewardModulation(float reward, float predictedReward = 0.0f);
    void updateDevelopment(double timestep);
    
    // Configuration
    void configure(const AgentConfig& config);
    const AgentConfig& getConfig() const { return config_; }
    
    // Statistics
    const AgentStats& getStats() const { return stats_; }
    void resetStats() { 
        stats_ = AgentStats();
    }
    
    // Brain access
    Brain* getBrain() { return brain_.get(); }
    const std::shared_ptr<Brain>& getBrainPtr() const { return brain_; }
    
    // Sub-system access (for advanced users)
    NeuralWorkingMemory* getWorkingMemory() { 
        return brain_ ? brain_->getWorkingMemory() : nullptr;
    }
    
    NeuralEpisodicMemory* getEpisodicMemory() { 
        return brain_ ? brain_->getEpisodicMemory() : nullptr;
    }
    
    Dopamine* getDopamine() { 
        return brain_ ? brain_->getDopamine() : nullptr;
    }
    
    Curiosity* getCuriosity() { 
        return brain_ ? brain_->getCuriosity() : nullptr;
    }
    
    Novelty* getNovelty() { 
        return brain_ ? brain_->getNovelty() : nullptr;
    }
    
    PredictionError* getPredictionError() { 
        return brain_ ? brain_->getPredictionErrorSignal() : nullptr;
    }
    
    ConceptFormation* getConceptFormation() { 
        return brain_ ? brain_->getConceptFormation() : nullptr;
    }
    
    NeuralPlanner* getPlanner() { 
        return brain_ ? brain_->getPlanner() : nullptr;
    }
    
    DevelopmentSystem* getDevelopmentSystem() { 
        return brain_ ? brain_->getDevelopmentSystem() : nullptr;
    }
    
    PredictionSystem* getPredictionSystem() { 
        return brain_ ? brain_->getPredictionSystem() : nullptr;
    }
    
    // High-level agent behaviors
    MotorCommand selectActionWithCuriosity(MotorCommand defaultCmd);
    MotorCommand selectActionWithNovelty(MotorCommand defaultCmd);
    MotorCommand selectActionWithPlanning();
    
    // Episode management
    void startEpisode(int episode);
    void endEpisode(float totalReward);
    
    // Checkpointing
    bool save(const std::string& filename) const;
    bool load(const std::string& filename);
    
    // Configuration access
    void enableFeature(const std::string& feature, bool enable);
    bool isFeatureEnabled(const std::string& feature) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper methods
    void initializeNeuronGroups();
    void updateSensorySystems(const SensoryPercept& percept);
    void updateMotorSystems();
    void updateMemorySystems();
    void updatePredictionSystems();
    void updateDevelopmentSystems();
    void updateNeuromodulationSystems();
    
    // Configuration and statistics
    AgentConfig config_;
    AgentStats stats_;
    
    // Performance monitoring
    double lastUpdateTime_;
    size_t updatesSinceLastStat_;
    
    // Episode tracking
    int currentEpisode_;
    float episodeStartTime_;
    float episodeTotalReward_;
    
    // State management
    enum class AgentState {
        Initializing,
        Running,
        Paused,
        Saving,
        Loading,
        ShuttingDown
    };
    
    AgentState state_;
    
    // Configuration feature map
    std::unordered_map<std::string, bool> featureFlags_;
    
    // Private constructor for factory pattern
    explicit AgentBrain(std::shared_ptr<Brain> brain, const AgentConfig& config = AgentConfig());
    
    friend class AgentBrainFactory;
};

// Factory pattern for agent creation
class AgentBrainFactory {
public:
    static std::unique_ptr<AgentBrain> createStandardAgent(std::shared_ptr<Brain> brain);
    static std::unique_ptr<AgentBrain> createMinimalAgent(std::shared_ptr<Brain> brain);
    static std::unique_ptr<AgentBrain> createAdvancedAgent(std::shared_ptr<Brain> brain);
    static std::unique_ptr<AgentBrain> createFromConfig(std::shared_ptr<Brain> brain, const AgentConfig& config);
    
    // Feature helpers
    static void enableAllFeatures(AgentConfig& config);
    static void enableCoreFeatures(AgentConfig& config);
    static void enableBasicFeatures(AgentConfig& config);
    static void disableAllFeatures(AgentConfig& config);
};

// Convenience functions for easy agent management
inline std::unique_ptr<AgentBrain> createAgentBrain(std::shared_ptr<Brain> brain) {
    return AgentBrainFactory::createStandardAgent(brain);
}

inline std::unique_ptr<AgentBrain> createAgentBrain(std::shared_ptr<Brain> brain, const AgentConfig& config) {
    return AgentBrainFactory::createFromConfig(brain, config);
}

} // namespace nlm
