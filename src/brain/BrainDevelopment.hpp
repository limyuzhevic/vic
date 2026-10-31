// Brain development header
// Declares functions extracted from Brain.cpp for better organization

#pragma once

#include "../core/Types/Types.hpp"
#include <cstdint>

namespace nlm {

class Brain;

// Forward declarations for types used in development processing
class DevelopmentSystem;
class StructuralPlasticity;
class DevelopmentalStage;
class NeuronId;
class SimulationStep;
class Timestamp;

// Function declarations for brain development processing
void brainDevelopmentUpdate(Brain* brain, SimulationStep currentStep, Timestamp currentTime);

} // namespace nlm
