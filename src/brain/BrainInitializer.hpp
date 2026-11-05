// BrainInitializer.hpp - Configuration loading and neural initialization
#pragma once

#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../performance/CheckpointSystem.hpp"

namespace nlm {

class Brain;

class BrainInitializer {
public:
    static void initialize(Brain& brain);
    static void loadConfiguration(Brain& brain);
    static void setupRegions(Brain& brain);
    static void setupConnectivity(Brain& brain);
    static void initializeIntegratedSystems(Brain& brain);
    static void registerSpikeHandlers(Brain& brain);
    static void configurePlasticitySystems(Brain& brain);
    static void configureCheckpointManager(Brain& brain);
    static void setupMemorySystems(Brain& brain);
    static void setupPredictionSystem(Brain& brain);
    static void setupCognitionSystems(Brain& brain);
    static void setupNeuromodulationSystems(Brain& brain);
    static void setupDevelopmentSystem(Brain& brain);

private:
    static void setupRandomGenerator(Brain& brain);
    static void setupPlasticitySystems(Brain& brain);
    static void setupInterRegionConnections(Brain& brain);
};
