// Brain memory header
// Declares functions extracted from Brain.cpp for better organization

#pragma once

#include "../core/Types/Types.hpp"
#include <cstdint>

namespace nlm {

class Brain;

// Forward declarations for types used in memory processing
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;
class PredictionSystem;
class NeuralPlanner;
class ConceptFormation;
class AttentionalSelection;
class DevelopmentSystem;
class Dopamine;
class Curiosity;
class Novelty;
class PredictionError;
class NeuronId;
class SimulationStep;
class Timestamp;

// Function declarations for brain memory processing
void brainMemoryUpdate(Brain* brain, SimulationStep currentStep, Timestamp currentTime);

} // namespace nlm
