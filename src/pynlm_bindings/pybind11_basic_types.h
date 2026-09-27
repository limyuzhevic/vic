// pybind11_basic_types.h - Basic types and enums

#pragma once

#include "pybind11/pybind11.h"
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <memory>
#include <string>
#include <vector>

namespace py = pybind11;
namespace nlm {

// Basic type definitions
struct NeuronId;
struct SynapseId;
struct RegionId;
struct PopulationId;

class NeuronType;
class SynapseType;
class DevelopmentalStage;
class FiringState;
class ActionType;
class MotorCommand;
class WorldObjectType;

// Basic type bindings
void bind_basic_types(py::module& m);

} // namespace nlm
