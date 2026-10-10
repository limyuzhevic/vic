// Core types and configuration system
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace nlm {

using NeuronId = uint64_t;
using RegionId = uint64_t;
using PopulationId = uint64_t;
using SynapseId = uint64_t;
using Timestamp = double;
using TimestepDuration = double;
using MembranePotential = float;
using SynapticWeight = float;
using Delay = uint32_t;

enum class NeuronType {
    Internal,
    Sensory,
    Motor,
    Excitatory,
    Inhibitory
};

enum class NeuronState {
    Resting,
    Active,
    Refractory,
    Silent
};

enum class SynapseType {
    Excitatory,
    Inhibitory,
    Modulatory
};

struct PlasticityFlags {
    bool hebbian = false;
    bool stdp = false;
    bool reward_modulated = false;
};

struct FiringState {
    FiringState() : membranePotential(-70.0f), threshold(-55.0f), 
                   refractoryRemaining(0), refractoryPeriod(0), 
                   firingRate(0.0f), leakConductance(0.1f),
                   restingPotential(-70.0f), resetPotential(-65.0f),
                   adaptationVariable(0.0f), lastSpikeTime(-1.0f) {}
    
    MembranePotential membranePotential;
    MembranePotential threshold;
    uint32_t refractoryRemaining;
    uint32_t refractoryPeriod;
    float firingRate;
    MembranePotential leakConductance;
    MembranePotential restingPotential;
    MembranePotential resetPotential;
    float adaptationVariable;
    float lastSpikeTime;
};

const NeuronId INVALID_NEURON_ID = 0;
const RegionId INVALID_REGION_ID = 0;
const PopulationId INVALID_POPULATION_ID = 0;

class Config;
class RandomGenerator;
class Logger;
class Brain;
class Neuron;
class Synapse;
class STDP;
class Hebbian;
class PlasticityRule;

} // namespace nlm