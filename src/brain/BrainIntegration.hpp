// Brain Integration Module
// This module provides the main brain integration functionality
// Organized into logical components for better maintainability

#ifndef BRAIN_INTEGRATION_HPP
#define BRAIN_INTEGRATION_HPP

#include "Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Random/Random.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "development/DevelopmentSystem.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/AttentionalSelection.hpp"
#include "performance/CheckpointSystem.hpp"
#include <memory>
#include <vector>

namespace nlm {

class BrainIntegration {
public:
    // Constructor and Destructor
    BrainIntegration(std::shared_ptr<Config> config);
    ~BrainIntegration();
    
    // Disable copying, enable moving
    BrainIntegration(const BrainIntegration&) = delete;
    BrainIntegration& operator=(const BrainIntegration&) = delete;
    BrainIntegration(BrainIntegration&&) noexcept;
    BrainIntegration& operator=(BrainIntegration&&) noexcept;
    
    // Core initialization
    bool initialize(size_t neuronCount, size_t regionCount, float connectionProbability);
    
    // Main simulation step
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // I/O operations
    void receiveSensoryInput(const SensoryInput& input);
    std::unique_ptr<Action> produceAction();
    
    // System management
    void reset();
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    
    // Query systems
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    float getExcitationInhibitionRatio() const;
    
    // Get integrated system references
    NeuralWorkingMemory* getWorkingMemory();
    NeuralEpisodicMemory* getEpisodicMemory();
    NeuralAssociativeMemory* getAssociativeMemory();
    PredictionSystem* getPredictionSystem();
    NeuralPlanner* getPlanner();
    ConceptFormation* getConceptFormation();
    AttentionalSelection* getAttention();
    DevelopmentSystem* getDevelopmentSystem();
    Dopamine* getDopamine();
    Curiosity* getCuriosity();
    Novelty* getNovelty();
    PredictionError* getPredictionErrorSignal();
    
    // Statistics and logging
    void logStatus() const;
    
private:
    // Integration implementation
    struct IntegrationImpl;
    IntegrationImpl* pImpl;
};

} // namespace nlm

#endif // BRAIN_INTEGRATION_HPP