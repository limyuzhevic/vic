#pragma once

#include "../core/Types/Types.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"

namespace nlm {

class NeuralRegion;
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class PredictionSystem;
class NeuralPlanner;
class ConceptFormation;
class AttentionalSelection;
class DevelopmentSystem;
class Dopamine;
class Curiosity;
class Novelty;
class SpikeSystem;
class STDP;
class Hebbian;
class StructuralPlasticity;
class CheckpointManager;

struct Brain::Impl;

// Forward declarations for member function signatures
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

// Main simulation loop
void Brain::step(SimulationStep currentStep);
void Brain::step(SimulationStep currentStep, Timestamp currentTime);

} // namespace nlm