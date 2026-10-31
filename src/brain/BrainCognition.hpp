// Brain cognition header
// Declares functions extracted from Brain.cpp for better organization

#pragma once

#include "../core/Types/Types.hpp"
#include <cstdint>

namespace nlm {

class Brain;

// Forward declarations for types used in cognition processing
class NeuralPlanner;
class ConceptFormation;
class AttentionalSelection;
class NeuralWorkingMemory;
class NeuronId;
class SimulationStep;
class Timestamp;

// Function declarations for brain cognition processing
void brainCognitionUpdate(Brain* brain, SimulationStep currentStep, Timestamp currentTime);

} // namespace nlm
