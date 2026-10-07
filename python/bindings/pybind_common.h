#ifndef PYNL_BINDINGS_H
#define PYNL_BINDINGS_H

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <memory>

namespace nlm {
    class Config;
    class Brain;
    class AgentBrain;
    class SimpleWorld;
    class SensoryInput;
    class Vision;
    class Audio;
    class InternalSignals;
    class SensoryPercept;
    class Action;
    class WorldObject;
    class AgentBody;
    class Observation;
    
    enum class NeuronType;
    enum class SynapseType;
    enum class DevelopmentalStage;
    enum class FiringState;
    enum class ActionType;
    enum class MotorCommand;
    enum class WorldObjectType;
    
    using NeuronId = uint64_t;
    using SynapseId = uint64_t;
    using RegionId = uint64_t;
    using PopulationId = uint64_t;
    
    extern const NeuronId INVALID_NEURON_ID;
    extern const SynapseId INVALID_SYNAPSE_ID;
    extern const RegionId INVALID_REGION_ID;
    extern const PopulationId INVALID_POPULATION_ID;
}

// Forward declare binding functions
void bindCoreTypes(py::module& m);
void bindConfigClass(py::module& m);
void bindBrainClass(py::module& m);
void bindAgentBrainClass(py::module& m);
void bindSimpleWorldClass(py::module& m);
void bindSensoryInputClasses(py::module& m);
void bindActionClasses(py::module& m);
void bindGetAgentStats(py::module& m);

#endif // PYNL_BINDINGS_H
