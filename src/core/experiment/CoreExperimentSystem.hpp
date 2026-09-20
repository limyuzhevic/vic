#pragma once

#include "../brain/NeuralPrediction.hpp"
#include "../brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/Attention.hpp"
#include "../neuromodulation/Reward.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../core/Logger/Logger.hpp"
#include <string>
#include <vector>
#include <memory>

namespace nlm {

// Core experiment framework for integrating all systems
class CoreExperimentSystem {
public:
    CoreExperimentSystem();
    ~CoreExperimentSystem();
    
    // Run complete system integration
    void runIntegrationExperiment(const std::string& experimentName, size_t numEpisodes);
    
    // Run specific system tests
    void runPredictionSystemTest();
    void runMemorySystemTest();
    void runCognitionSystemTest();
    void runNeuromodulationSystemTest();
    
    // Get system status
    bool isPredictionIntegrated() const { return predictionSystem_ != nullptr; }
    bool isMemoryIntegrated() const { return workingMemory_ != nullptr && episodicMemory_ != nullptr; }
    bool isCognitionIntegrated() const { return planner_ != nullptr && concepts_ != nullptr && attention_ != nullptr; }
    bool isNeuromodulationIntegrated() const { return reward_ != nullptr && curiosity_ != nullptr; }
    
    // Get metrics
    float getPredictionAccuracy() const { return predictionAccuracy_; }
    float getMemoryRetention() const { return memoryRetention_; }
    float getCognitionScore() const { return cognitionScore_; }
    float getNeuromodulationLevel() const { return neuromodulationLevel_; }
    
private:
    void initializeSystems(std::shared_ptr<Brain> brain);
    void verifySystemIntegration(std::shared_ptr<Brain> brain);
    void collectSystemMetrics(std::shared_ptr<Brain> brain);
    
    // System pointers
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<NeuralPrediction> predictionSystem_;
    std::shared_ptr<NeuralWorkingMemory> workingMemory_;
    std::shared_ptr<NeuralEpisodicMemory> episodicMemory_;
    std::shared_ptr<NeuralPlanner> planner_;
    std::shared_ptr<ConceptFormation> concepts_;
    std::shared_ptr<Attention> attention_;
    std::shared_ptr<Reward> reward_;
    std::shared_ptr<Curiosity> curiosity_;
    std::shared_ptr<PredictionError> predictionError_;
    std::shared_ptr<Novelty> novelty_;
    
    // Metrics
    float predictionAccuracy_ = 0.0f;
    float memoryRetention_ = 0.0f;
    float cognitionScore_ = 0.0f;
    float neuromodulationLevel_ = 0.0f;
    size_t totalSteps_ = 0;
};

} // namespace nlm
