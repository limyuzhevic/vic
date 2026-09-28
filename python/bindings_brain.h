// bindings_brain.h - Brain module bindings
#pragma once
#include <pybind11/pybind11.h>
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"

namespace py = pybind11;

void register_bindings_brain(py::module& m);

// Forward declarations for Brain types
namespace nlm {
    class Brain;
    class Config;
    class NeuronId;
    class SynapseId;
    class RegionId;
    class PopulationId;
    class NeuronType;
    class SynapseType;
    class DevelopmentalStage;
    class FiringState;
    class ActionType;
    class MotorCommand;
    class WorldObjectType;
}