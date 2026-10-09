# pragma once

#include "core/Types/Types.hpp"
#include "../core/Logger/Logger.hpp"
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>

namespace nlm {

/**
 * @brief Core types and constants for NLM neural simulation framework
 * 
 * This file contains fundamental type definitions and constants used throughout
 * the NLM system. All other components should include this file to ensure
 * consistent type definitions.
 */

// Basic identifier types
struct NeuronId {
    uint64_t value;
    
    NeuronId() : value(0) {}
    explicit NeuronId(uint64_t v) : value(v) {}
    
    uint64_t index() const { return value; }
    
    bool operator==(const NeuronId& other) const { return value == other.value; }
    bool operator!=(const NeuronId& other) const { return value != other.value; }
    bool operator<(const NeuronId& other) const { return value < other.value; }
    bool operator>(const NeuronId& other) const { return value > other.value; }
};

struct SynapseId {
    uint64_t value;
    
    SynapseId() : value(0) {}
    explicit SynapseId(uint64_t v) : value(v) {}
    
    uint64_t index() const { return value; }
    
    bool operator==(const SynapseId& other) const { return value == other.value; }
    bool operator!=(const SynapseId& other) const { return value != other.value; }
};

struct RegionId {
    uint64_t value;
    
    RegionId() : value(0) {}
    explicit RegionId(uint64_t v) : value(v) {}
    
    uint64_t index() const { return value; }
    
    bool operator==(const RegionId& other) const { return value == other.value; }
    bool operator!=(const RegionId& other) const { return value != other.value; }
};

struct PopulationId {
    uint64_t value;
    
    PopulationId() : value(0) {}
    explicit PopulationId(uint64_t v) : value(v) {}
    
    uint64_t index() const { return value; }
    
    bool operator==(const PopulationId& other) const { return value == other.value; }
    bool operator!=(const PopulationId& other) const { return value != other.value; }
};

struct InterRegionConnectionId {
    uint64_t value;
    
    InterRegionConnectionId() : value(0) {}
    explicit InterRegionConnectionId(uint64_t v) : value(v) {}
    
    uint64_t index() const { return value; }
    
    bool operator==(const InterRegionConnectionId& other) const { return value == other.value; }
    bool operator!=(const InterRegionConnectionId& other) const { return value != other.value; }
};

struct SimulationStep {
    uint64_t value;
    
    SimulationStep() : value(0) {}
    explicit SimulationStep(uint64_t v) : value(v) {}
    
    uint64_t index() const { return value; }
};

struct Timestamp {
    double value;
    
    Timestamp() : value(0.0) {}
    explicit Timestamp(double v) : value(v) {}
    
    double getMilliseconds() const { return value * 1000.0; }
};

struct Delay {
    uint32_t steps;
    
    Delay() : steps(1) {}
    explicit Delay(uint32_t s) : steps(s) {}
};

// Basic value types
using MembranePotential = float;    // Membrane voltage (mV)
using SynapticWeight = float;       // Synaptic strength (-1.0 to +1.0)
using FiringRate = float;           // Neurons firing rate (Hz)
using PlasticityFlags = struct {
    bool hebbian = false;
    bool stdp = false;
    bool reward_modulated = false;
    bool reward_modulated = false;
};

// Enumeration types
enum class NeuronType {
    Excitatory,
    Inhibitory,
    Modulatory,
    Sensory,
    Motor,
    Internal
};

enum class SynapseType {
    Excitatory,
    Inhibitory,
    Modulatory,
    Electrical,
    GapJunction
};

enum class DevelopmentalStage {
    Initial,
    CriticalPeriod,
    Maturation,
    Adult,
    Aging
};

enum class FiringState {
    Resting,
    Active,
    Refractory,
    Inhibited
};

enum class ActionType {
    MoveForward,
    MoveBackward,
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

enum class WorldObjectType {
    Empty,
    Resource,
    Hazard,
    Wall,
    Marker
};

enum class EventType {
    Spike,
    DelayedSpike,
    PlasticityUpdate,
    Reward,
    Novelty,
    Prediction
};

// Constants
namespace Constants {
    constexpr float PI = 3.141592653589793f;
    constexpr float E = 2.718281828459045f;
    constexpr float DEFAULT_RESTING_POTENTIAL = -70.0f;
    constexpr float DEFAULT_THRESHOLD = -55.0f;
    constexpr float DEFAULT_RESET_POTENTIAL = -70.0f;
    constexpr float DEFAULT_LEAK_CONDUCTANCE = 10.0f;
    constexpr uint32_t DEFAULT_REFECTORY_PERIOD = 5;
    constexpr float DEFAULT_LTP_WEIGHT = 0.01f;
    constexpr float DEFAULT_LTD_WEIGHT = 0.012f;
    constexpr float DEFAULT_STDP_TIME_CONSTANT = 20.0f;
    constexpr float MAX_WEIGHT = 1.0f;
    constexpr float MIN_WEIGHT = -1.0f;
}

} // namespace nlm
