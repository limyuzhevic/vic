// Enum definitions for Python bindings
#ifndef NLM_PYTHON_ENUMS_H
#define NLM_PYTHON_ENUMS_H

#include <pybind11/pybind11.h>

namespace py = pybind11;

// Forward declarations for C++ enums
enum class NeuronType;
enum class SynapseType;
enum class DevelopmentalStage;
enum class FiringState;
enum class ActionType;
enum class MotorCommand;
enum class WorldObjectType;

void registerEnumDefinitions(py::module_& m);

#endif // NLM_PYTHON_ENUMS_H
