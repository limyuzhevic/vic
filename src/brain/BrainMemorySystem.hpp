#pragma once

#include "Brain.hpp"

namespace nlm {

class BrainMemorySystem;
class BrainCognitionSystem;
class BrainNeuromodulationSystem;
class BrainDevelopmentSystem;

class BrainMemorySystem {
public:
    static void initializeIntegratedSystems(Brain* brain);
    static void updateEpisodicMemory(Brain* brain, Timestamp currentTime);
};

class BrainCognitionSystem {
public:
    static void initializeCognitionSystems(Brain* brain);
};

class BrainNeuromodulationSystem {
public:
    static void initializeNeuromodulationSystems(Brain* brain);
    static void updateNeuromodulation(Brain* brain);
    static void applyPlasticityRules(Brain* brain, Timestamp currentTime);
};

class BrainDevelopmentSystem {
public:
    static void initializeDevelopmentSystem(Brain* brain);
};

} // namespace nlm
