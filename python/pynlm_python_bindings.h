#ifndef PYNLM_PYTHON_BINDINGS_H
#define PYNLM_PYTHON_BINDINGS_H

/**
 * @file pynlm_python_bindings.h
 * @brief Python bindings interface for NLM (Neural Learning Machine)
 * 
 * This header defines the C++ API that is exposed to Python via pybind11.
 * It includes all the Python-accessible classes, functions, and exceptions.
 * 
 * @author NLM Project
 * @date 2024
 * @version 0.1.0
 */

#include <memory>
#include <string>
#include <vector>

// Forward declarations for Python binding types
namespace pybind11 {
    class module_;
}

namespace nlm {
    // Forward declarations of NLM classes
    class Config;
    class Brain;
    class AgentBrain;
    class SimpleWorld;
    class SensoryInput;
    class Vision;
    class Audio;
    class InternalSignals;
    class Action;
    class WorldObject;
    class AgentBody;
    class ActionResult;
    class SensoryPercept;
    class NeuronId;
    class SynapseId;
    class RegionId;
    class PopulationId;
    class InterRegionConnection;
    
    // Enumerations exposed to Python
    enum class NeuronType;
    enum class SynapseType;
    enum class DevelopmentalStage;
    enum class FiringState;
    enum class ActionType;
    enum class MotorCommand;
    enum class WorldObjectType;
}

// Python module initialization function
void init_pynlm(pybind11::module_& m);

// Python module version information
constexpr const char* PYNLM_VERSION = "0.1.0";
constexpr const char* PYNLM_AUTHOR = "NLM Project";
constexpr const char* PYNLM_DESCRIPTION = "Neural Learning Machine - An experimental artificial developmental brain";

#endif // PYNLM_PYTHON_BINDINGS_H