#pragma once

#include "../core/Types/Types.hpp"
#include "Dopamine.hpp"
#include "Curiosity.hpp"
#include "Novelty.hpp"
#include "PredictionError.hpp"
#include "../brain/Brain.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/NeuralSelfModel.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include <memory>
#include <vector>

namespace nlm {

// NeuromodulationSystem: Coordinates all neuromodulatory signals and integrates them
// into cognitive and neural processes
class NeuromodulationSystem {
public:
    NeuromodulationSystem();
    ~NeuromodulationSystem();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Update all neuromodulatory signals
    void update(TimestepDuration dt);
    
    // Get integrated neuromodulation level
    float getIntegratedLevel() const;
    
    // Get individual neuromodulator levels
    float getDopamineLevel() const;
    float getCuriosityLevel() const;
    float getNoveltyLevel() const;
    float getPredictionErrorLevel() const;
    
    // Get neuromodulation effects on processes
    float getPlasticityModulation() const;
    float getLearningRateModulation() const;
    float getConfidenceModulation() const;
    float getPredictionModulation() const;
    
    // Connect to cognitive systems
    void updateConceptFormation(ConceptFormation* conceptFormation,
                                const std::vector<float>& neuralPattern,
                                float reward,
                                SimulationStep step);
    
    void updatePlanner(NeuralPlanner* planner,
                      const std::vector<float>& currentState,
                      const std::vector<float>& goal,
                      float predictionError);
    
    void updateSelfModel(NeuralSelfModel* selfModel,
                        const std::vector<float>& neuralPattern,
                        const std::vector<float>& planningContext,
                        float noveltyLevel,
                        float curiosityLevel,
                        float predictionError,
                        float rewardLevel);
    
    void updatePredictionSystem(PredictionSystem* predictionSystem,
                                const std::vector<float>& currentState,
                                const std::vector<float>& targetState);
    
    // Connect to neural plasticity
    void updatePlasticity(STDP* stdp, float baseLearningRate) const;
    void updateHebbian(Hebbian* hebbian, float baseLearningRate) const;
    
    // Reset system
    void reset();
    
    // Log system status
    void logStatus() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
