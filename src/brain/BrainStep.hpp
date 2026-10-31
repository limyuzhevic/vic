// Brain step implementation header
// Declares functions extracted from Brain.cpp for better organization

#pragma once

#include "../core/Types/Types.hpp"
#include <cstdint>

namespace nlm {

class Brain;

// Forward declarations for types used in step processing
class SpikeSystem;
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
class FiringState;
class Synapse;
class SynapseType;
class SynapseCheckpointData;
class CheckpointReader;
class CheckpointWriter;

// Function declarations for brain step processing
void brainStep(Brain* brain, SimulationStep currentStep);
void brainStepWithTime(Brain* brain, SimulationStep currentStep, Timestamp currentTime);

} // namespace nlm

