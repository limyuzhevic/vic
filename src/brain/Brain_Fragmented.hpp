#pragma once

#include "Brain.hpp"

namespace nlm {

// Forward declarations to reduce include dependencies
class SpikeSystem;
class STDP;
class Hebbian;
class StructuralPlasticity;

// Brain.cpp has grown to 1117 lines and needs to be broken down
// This header contains the public Brain interface and internal implementation details

// Forward declarations for integrated systems
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
class PredictionError;
class Novelty;
class CheckpointManager;

struct DevelopmentalStage;
typedef uint64_t SimulationStep;
typedef double Timestamp;

} // namespace nlm