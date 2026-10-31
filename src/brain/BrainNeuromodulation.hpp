// Brain neuromodulation header
// Declares functions extracted from Brain.cpp for better organization

#pragma once

#include "../core/Types/Types.hpp"
#include <cstdint>

namespace nlm {

class Brain;

// Forward declarations for types used in neuromodulation processing
class Dopamine;
class Curiosity;
class Novelty;
class PredictionError;
class NeuralWorkingMemory;
class NeuronId;
class SimulationStep;
class Timestamp;

// Function declarations for brain neuromodulation processing
void brainNeuromodulationUpdate(Brain* brain, SimulationStep currentStep, Timestamp currentTime);

} // namespace nlm
