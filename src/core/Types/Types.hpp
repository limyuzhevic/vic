#pragma once

#include <cstdint>
#include <string>

namespace nlm {

// Type aliases for better readability
using NeuronId = uint64_t;
using SynapseId = uint64_t;
using PopulationId = uint64_t;
using RegionId = uint64_t;
using SimulationStep = uint64_t;
using Timestamp = double;
using TimestepDuration = double;
using MembranePotential = float;
using SynapticWeight = float;
using Delay = uint32_t;
using FiringRate = float;
using AdaptationVariable = float;

// Define invalid values
const NeuronId INVALID_NEURON_ID = static_cast<NeuronId>(-1);
const SynapseId INVALID_SYNAPSE_ID = static_cast<SynapseId>(-1);
const PopulationId INVALID_POPULATION_ID = static_cast<PopulationId>(-1);
const RegionId INVALID_REGION_ID = static_cast<RegionId>(-1);

// Enumeration for neuron types
enum class NeuronType {
    Excitatory,
    Inhibitory,
    Modulatory,
    Sensory,
    Motor,
    Internal
};

// Enumeration for synapse types
enum class SynapseType {
    Excitatory,
    Inhibitory,
    Modulatory,
    Electrical,
    GapJunction
};

// Enumeration for development stages
enum class DevelopmentalStage {
    Initial,
    CriticalPeriod,
    Maturation,
    Adult,
    Aging
};

// Enumeration for firing states
enum class FiringState {
    Resting,
    Active,
    Refractory,
    Inhibited
};

// Enumeration for action types
enum class ActionType {
    MoveForward,
    MoveBackward,
    MoveLeft,
    MoveRight,
    TurnLeft,
    TurnRight,
    Look,
    LookUp,
    LookDown,
    Interact,
    Eat,
    Drink,
    Rest,
    Wait,
    Custom
};

// Enumeration for motor commands
enum class MotorCommand {
    MoveForward,
    MoveBackward,
    TurnLeft,
    TurnRight,
    LookLeft,
    LookRight,
    Interact,
    Wait
};

// Enumeration for world object types
enum class WorldObjectType {
    Empty,
    Resource,
    Hazard,
    Wall,
    Marker
};

// Enumeration for plasticity flags
struct PlasticityFlags {
    bool hebbian = false;
    bool stdp = false;
    bool reward_modulated = false;
};

} // namespace nlm
