#pragma once

#include "../core/Types/Types.hpp"
#include "Brain.hpp"
#include "SpikeSystem.hpp"
#include "STDP.hpp"
#include "Hebbian.hpp"
#include "StructuralPlasticity.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/Memory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Reward.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <vector>
#include <string>
#include <memory>

namespace nlm {

class BrainSimulator {
public:
    BrainSimulator() = default;
    ~BrainSimulator() = default;

    // Main simulation step implementation
    void simulate(Brain* brain, SimulationStep currentStep, Timestamp currentTime);

    // Individual simulation phases
    void processDelayedSpikes(Brain* brain, SimulationStep currentStep, Timestamp currentTime);
    void updateNeurons(Brain* brain, Timestamp currentTime);
    void processSpikes(Brain* brain, SimulationStep currentStep);
    void updateWorkingMemory(Brain* brain);
    void updateNeuromodulation(Brain* brain);
    void applyPlasticity(Brain* brain, SimulationStep currentStep, Timestamp currentTime, float plasticityMod);
    void updateEpisodicMemory(Brain* brain, SimulationStep currentStep);
    void updatePredictionSystem(Brain* brain);
    void updateAttentionSystem(Brain* brain);
    void updateConceptFormation(Brain* brain);
    void applyStructuralPlasticity(Brain* brain, SimulationStep currentStep);
    void replayMemories(Brain* brain, SimulationStep currentStep);
    void applyDevelopment(Brain* brain, SimulationStep currentStep);
    void consolidateMemories(Brain* brain, SimulationStep currentStep);
    void updateCheckpoint(Brain* brain, SimulationStep currentStep, Timestamp currentTime);

    // Statistics and logging
    struct SimulationStats {
        size_t totalSpikesProcessed = 0;
        size_t neuronsUpdated = 0;
        size_t spikesDetected = 0;
        size_t workingMemoryUpdates = 0;
        size_t neuromodulationUpdates = 0;
        size_t plasticityUpdates = 0;
        size_t memoryUpdates = 0;
        double simulationTime = 0.0;
    };

    SimulationStats getStats() const { return stats; }
    void resetStats() { stats = SimulationStats(); }

private:
    // Helper: detect and queue spikes
    void detectSpikes(Brain* brain, SimulationStep currentStep, Timestamp currentTime);

    // Helper: apply neuromodulation effects on neural excitability
    void applyNeuromodulationToNeurons(Brain* brain);

    // Helper: apply homeostatic synaptic scaling
    void applySynapticScaling(Brain* brain, SimulationStep currentStep);

    // Helper: calculate neuromodulation factor for plasticity
    float calculatePlasticityModulation(Brain* brain) const;

    // Helper: store brain state as episodic memory
    void storeEpisodicMemory(Brain* brain, SimulationStep currentStep);

    SimulationStats stats;
};
