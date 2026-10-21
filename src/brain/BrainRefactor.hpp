#pragma once

#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"

#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

class BrainFactory {
public:
    static std::unique_ptr<Brain> create(std::shared_ptr<Config> config);
};

class BrainInitializer {
public:
    static bool initializeBrain(Brain* brain);
    static void configureNeuralConnectivity(Brain* brain, size_t neuronCount, 
                                           size_t regionCount, float connectionProbability);
    static void initializeIntegratedSystems(Brain* brain, size_t neuronCount);
    static void setupSpikeHandlers(Brain* brain);
    static void configureCheckpointSystem(Brain* brain);
};

class BrainSimulator {
public:
    static void simulateStep(Brain* brain, SimulationStep currentStep);
    static void simulateStep(Brain* brain, SimulationStep currentStep, Timestamp currentTime);
};

class BrainIO {
public:
    static void handleSensoryInput(Brain* brain, const SensoryInput& input);
    static void injectCurrent(Brain* brain, NeuronId neuron, MembranePotential current);
    static void injectCurrentToNeurons(Brain* brain, NeuronType type, MembranePotential current);
    static std::unique_ptr<Action> produceMotorOutput(Brain* brain);
    static void applyNeuromodulation(Brain* brain, const Neuromodulator& signal);
};

class BrainManager {
public:
    static void updatePlasticity(Brain* brain);
    static void develop(Brain* brain);
    static void reset(Brain* brain);
    static bool saveBrainState(Brain* brain, const std::string& filepath) const;
    static bool loadBrainState(Brain* brain, const std::string& filepath);
    static size_t getRegionCount(const Brain* brain);
    static std::vector<RegionId> getRegionIds(const Brain* brain);
    static NeuralRegion* getRegion(Brain* brain, RegionId id);
    static const NeuralRegion* getRegion(const Brain* brain, RegionId id) const;
};

class BrainStatistics {
public:
    static float getExcitationInhibitionRatio(const Brain* brain);
    static size_t getTotalSpikeCount(const Brain* brain);
    static size_t getPendingSpikeEventCount(const Brain* brain);
    static size_t getTotalNeuronCount(const Brain* brain);
    static size_t getTotalSynapseCount(const Brain* brain);
    static size_t getActiveNeuronCount(const Brain* brain);
    static size_t getFiringNeuronCount(const Brain* brain);
    static float getAverageFiringRate(const Brain* brain);
    static void logStatus(const Brain* brain);
};

} // namespace nlm
