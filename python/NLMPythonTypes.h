// Type definitions and forward declarations for Python bindings
#ifndef NLM_PYTHON_TYPES_H
#define NLM_PYTHON_TYPES_H

#include <cstdint>
#include <string>

namespace py = pybind11;

// Core type definitions for Python bindings
struct NeuronId {
    uint64_t value;
    NeuronId() : value(0) {}
    NeuronId(uint64_t val) : value(val) {}
    uint64_t index() const { return value; }
    bool operator==(const NeuronId& other) const { return value == other.value; }
    bool operator!=(const NeuronId& other) const { return value != other.value; }
};

struct SynapseId {
    uint64_t value;
    SynapseId() : value(0) {}
    SynapseId(uint64_t val) : value(val) {}
    uint64_t index() const { return value; }
    bool operator==(const SynapseId& other) const { return value == other.value; }
    bool operator!=(const SynapseId& other) const { return value != other.value; }
};

struct RegionId {
    uint64_t value;
    RegionId() : value(0) {}
    RegionId(uint64_t val) : value(val) {}
    uint64_t index() const { return value; }
    bool operator==(const RegionId& other) const { return value == other.value; }
    bool operator!=(const RegionId& other) const { return value != other.value; }
};

struct PopulationId {
    uint64_t value;
    PopulationId() : value(0) {}
    PopulationId(uint64_t val) : value(val) {}
    uint64_t index() const { return value; }
    bool operator==(const PopulationId& other) const { return value == other.value; }
    bool operator!=(const PopulationId& other) const { return value != other.value; }
};

// Hash functions for use in Python collections
namespace std {
    template<> struct hash<NeuronId> {
        size_t operator()(const NeuronId& id) const { return hash<uint64_t>()(id.value); }
    };
    
    template<> struct hash<SynapseId> {
        size_t operator()(const SynapseId& id) const { return hash<uint64_t>()(id.value); }
    };
    
    template<> struct hash<RegionId> {
        size_t operator()(const RegionId& id) const { return hash<uint64_t>()(id.value); }
    };
    
    template<> struct hash<PopulationId> {
        size_t operator()(const PopulationId& id) const { return hash<uint64_t>()(id.value); }
    };
}

// Invalid ID constants
const NeuronId INVALID_NEURON_ID = NeuronId(0);
const SynapseId INVALID_SYNAPSE_ID = SynapseId(0);
const RegionId INVALID_REGION_ID = RegionId(0);
const PopulationId INVALID_POPULATION_ID = PopulationId(0);

#endif // NLM_PYTHON_TYPES_H
