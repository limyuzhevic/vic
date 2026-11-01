#pragma once

#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
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
#include <memory>
#include <vector>
#include <string>

namespace nlm {

class BrainInitializer {
public:
    BrainInitializer() = default;
    ~BrainInitializer() = default;

    // Initialize all brain components
    bool initializeBrain(Brain* brain, std::shared_ptr<Config> config);

    // Create and setup regions with neurons and populations
    void setupRegions(Brain* brain);

    // Initialize all integrated systems
    void initializeIntegratedSystems(Brain* brain);

    // Configure plasticity systems
    void configurePlasticity(Brain* brain);

    // Setup spike system handlers
    void setupSpikeSystem(Brain* brain);

    // Configure checkpoint system
    void configureCheckpointSystem(Brain* brain);

    // Get initialization statistics
    struct InitializationStats {
        size_t neuronCount = 0;
        size_t synapseCount = 0;
        size_t sensoryNeurons = 0;
        size_t motorNeurons = 0;
        double initializationTime = 0.0;
    };

    InitializationStats getStats() const { return stats; }

private:
    // Helper: create a single region with all populations
    void createRegion(Brain* brain, size_t regionIndex, size_t neuronsPerRegion);

    // Helper: populate sensory and motor neuron collections
    void collectIONeurons(Brain* brain, size_t regionIndex);

    // Helper: setup memory system capacities
    void setupMemorySystem(Brain* brain);

    // Helper: setup cognition system parameters
    void setupCognitionSystem(Brain* brain);

    // Helper: setup neuromodulation systems
    void setupNeuromodulationSystems(Brain* brain);

    InitializationStats stats;
};
