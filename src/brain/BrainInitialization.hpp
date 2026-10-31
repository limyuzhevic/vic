// Brain initialization header
// Declares functions extracted from Brain.cpp for better organization

#pragma once

#include "../core/Config/Config.hpp"

namespace nlm {

class Brain;

// Forward declarations for types used in initialization
class Config;
class RandomGenerator;
class SimulationClock;
class Logger;
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
class SpikeSystem;
class STDP;
class Hebbian;
class StructuralPlasticity;
class CheckpointManager;
class DetailedSpikeEvent;
class DelayedSpikeEvent;
class Neuron;
class NeuralRegion;
class NeuralPopulation;
class NeuronId;
class MembranePotential;
class SimulationStep;
class Timestamp;
class RegionId;
class NeuronType;

// Function declarations for brain initialization
bool brainInitialize(Brain* brain);

} // namespace nlm
