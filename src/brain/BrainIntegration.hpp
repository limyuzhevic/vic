#pragma once

#include "Brain.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/NeuralPopulation.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../brain/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nlm {

// Brain integration system: manages all integrated systems and their interactions
class BrainIntegration {
public:
    BrainIntegration();
    ~BrainIntegration();
    
    // Disable copying
    BrainIntegration(const BrainIntegration&) = delete;
    BrainIntegration& operator=(const BrainIntegration&) = delete;
    
    // Enable moving
    BrainIntegration(BrainIntegration&&) noexcept;
    BrainIntegration& operator=(BrainIntegration&&) noexcept;
    
    // Integration setup
    bool initialize(std::shared_ptr<Brain> brain);
    
    // System access
    std::shared_ptr<NeuralWorkingMemory> getWorkingMemory() { return workingMemory; }
    std::shared_ptr<NeuralEpisodicMemory> getEpisodicMemory() { return episodicMemory; }
    std::shared_ptr<NeuralAssociativeMemory> getAssociativeMemory() { return associativeMemory; }
    
    std::shared_ptr<PredictionSystem> getPredictionSystem() { return predictionSystem; }
    std::shared_ptr<NeuralPlanner> getPlanner() { return planner; }
    std::shared_ptr<ConceptFormation> getConceptFormation() { return conceptFormation; }
    
    std::shared_ptr<Dopamine> getDopamine() { return dopamine; }
    std::shared_ptr<Curiosity> getCuriosity() { return curiosity; }
    std::shared_ptr<PredictionError> getPredictionError() { return predictionError; }
    std::shared_ptr<Novelty> getNovelty() { return novelty; }
    
    std::shared_ptr<STDP> getSTDP() { return stdp; }
    std::shared_ptr<Hebbian> getHebbian() { return hebbian; }
    std::shared_ptr<StructuralPlasticity> getStructuralPlasticity() { return structuralPlasticity; }
    
    std::shared_ptr<CheckpointManager> getCheckpointManager() { return checkpointManager; }
    
    // Simulation control
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Statistics and access
    size_t getTotalNeuronCount() const { return totalNeuronCount; }
    size_t getTotalSynapseCount() const { return totalSynapseCount; }
    size_t getTotalSpikeCount() const { return totalSpikesTotal; }
    
    // Memory access helpers
    std::vector<NeuronId> getMostActiveNeurons(size_t count) const;
    size_t getPendingSpikeEventCount() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm