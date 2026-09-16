// Brain.h - Forward declarations for Brain.cpp
// This file is needed to include declarations for methods used in Brain.cpp
// that are defined in Brain.hpp

#ifndef BRAIN_FORWARD_DECLARATIONS_H
#define BRAIN_FORWARD_DECLARATIONS_H

namespace nlm {

// Forward declarations for all Brain methods used in Brain.cpp
class Config;
class RandomGenerator;
class SimulationClock;
class Logger;
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;
class SemanticMemory;
class ProceduralMemory;
class PredictionSystem;
class NeuralPlanner;
class ConceptFormation;
class AttentionalSelection;
class DevelopmentSystem;
class Dopamine;
class Curiosity;
class Novelty;
class PredictionError;
class SpikeSystem;
class STDP;
class Hebbian;
class StructuralPlasticity;
class CheckpointManager;

} // namespace nlm

#endif // BRAIN_FORWARD_DECLARATIONS_H
