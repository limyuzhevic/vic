#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

// Neuromodulation integration functions declaration
class Neuromodulator {
public:
    // These are forward declarations - the actual implementations
    // are in NeuromodulatorIntegration.cpp
    static void applyAllNeuromodulators(Brain* brain);
    static void processNeuromodulation(Brain* brain, TimestepDuration dt);
    static void integrateNeuromodulationWithMemory(Brain* brain);
    static void integrateNeuromodulationWithAttention(Brain* brain);
    static void integrateNeuromodulationWithPlasticity(Brain* brain);
};

} // namespace nlm