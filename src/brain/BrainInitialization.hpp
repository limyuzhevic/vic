#pragma once

#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"

namespace nlm {

class BrainInitialization {
public:
    static bool initialize(Brain& brain);
    static void setupSensoryMotorNeurons(Brain& brain);
    static void initializeMemorySystems(Brain& brain);
    static void initializeCognitionSystems(Brain& brain);
    static void initializeNeuromodulationSystems(Brain& brain);
    static void configurePlasticitySystems(Brain& brain);
    static void configureCheckpointManager(Brain& brain);
    static void registerSpikeHandlers(Brain& brain);
};

} // namespace nlm