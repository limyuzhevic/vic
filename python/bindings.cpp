// ==============================================================================
// PYBIND11 PYTHON BINDINGS - IMPROVED STRUCTURE AND ERROR HANDLING
// ==============================================================================

// This file contains the complete pybind11 bindings for NLM Python bindings (pynlm).
// The bindings have been refactored for better organization, error handling, and Pythonic interfaces.

// ==============================================================================
// INCLUDES AND FORWARD DECLARATIONS
// ==============================================================================

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>

// NLM core headers
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

// ==============================================================================
// NAMESPACE DECLARATIONS
// ==============================================================================

using namespace pybind11;
namespace nlm = ::nlm;

// ==============================================================================
// HELPER FUNCTIONS AND UTILITIES
// ==============================================================================

namespace nlm {
namespace pybind11 {

// Helper functions for Python bindings
namespace pybind_utils {
    
    // Convert NeuronId to Python string representation
    std::string neuron_id_to_string(const NeuronId& id) {
        return "<NeuronId: " + std::to_string(id.value) + ">";
    }
    
    // Convert SynapseId to Python string representation
    std::string synapse_id_to_string(const SynapseId& id) {
        return "<SynapseId: " + std::to_string(id.value) + ">";
    }
    
    // Convert RegionId to Python string representation
    std::string region_id_to_string(const RegionId& id) {
        return "<RegionId: " + std::to_string(id.value) + ">";
    }
    
    // Convert PopulationId to Python string representation
    std::string population_id_to_string(const PopulationId& id) {
        return "<PopulationId: " + std::to_string(id.value) + ">";
    }
    
    // Convert ActionType to Python string
    std::string action_type_to_string(ActionType type) {
        switch (type) {
            case ActionType::MoveForward: return "MoveForward";
            case ActionType::MoveBackward: return "MoveBackward";
            case ActionType::MoveLeft: return "MoveLeft";
            case ActionType::MoveRight: return "MoveRight";
            case ActionType::TurnLeft: return "TurnLeft";
            case ActionType::TurnRight: return "TurnRight";
            case ActionType::Look: return "Look";
            case ActionType::LookUp: return "LookUp";
            case ActionType::LookDown: return "LookDown";
            case ActionType::Interact: return "Interact";
            case ActionType::Eat: return "Eat";
            case ActionType::Drink: return "Drink";
            case ActionType::Rest: return "Rest";
            case ActionType::Wait: return "Wait";
            case ActionType::Custom: return "Custom";
            default: return "Unknown";
        }
    }
    
    // Convert MotorCommand to Python string
    std::string motor_command_to_string(MotorCommand cmd) {
        switch (cmd) {
            case MotorCommand::MoveForward: return "MoveForward";
            case MotorCommand::MoveBackward: return "MoveBackward";
            case MotorCommand::TurnLeft: return "TurnLeft";
            case MotorCommand::TurnRight: return "TurnRight";
            case MotorCommand::LookLeft: return "LookLeft";
            case MotorCommand::LookRight: return "LookRight";
            case MotorCommand::Interact: return "Interact";
            case MotorCommand::Wait: return "Wait";
            default: return "Unknown";
        }
    }
    
    // Convert WorldObjectType to Python string
    std::string world_object_type_to_string(WorldObjectType type) {
        switch (type) {
            case WorldObjectType::Empty: return "Empty";
            case WorldObjectType::Resource: return "Resource";
            case WorldObjectType::Hazard: return "Hazard";
            case WorldObjectType::Wall: return "Wall";
            case WorldObjectType::Marker: return "Marker";
            default: return "Unknown";
        }
    }
    
    // Safe neuron count validation
    size_t safe_neuron_count(const std::shared_ptr<Brain>& brain, size_t max_neurons) {
        auto count = brain->getTotalNeuronCount();
        if (count > max_neurons) {
            throw py::value_error("Neuron count exceeds maximum allowed " + std::to_string(max_neurons));
        }
        return count;
    }
    
    // Validate simulation step range
    SimulationStep validate_step(const std::shared_ptr<Brain>& brain, SimulationStep step) {
        if (step < 0) {
            throw py::value_error("Step number cannot be negative");
        }
        if (step > 1000000000) {
            throw py::value_error("Step number too large (max 1,000,000,000)");
        }
        return step;
    }
    
    // Validate time range
    Timestamp validate_time(const std::shared_ptr<Brain>& brain, Timestamp time) {
        if (time < 0.0) {
            throw py::value_error("Time cannot be negative");
        }
        if (time > 1000000.0) {
            throw py::value_error("Time too large (max 1,000,000.0)");
        }
        return time;
    }
    
    // Format spike count for display
    std::string format_spike_count(size_t count) {
        if (count >= 1000000) {
            return std::to_string(count / 1000000) + "M";
        } else if (count >= 1000) {
            return std::to_string(count / 1000) + "K";
        } else {
            return std::to_string(count);
        }
    }
    
    // Format neuron count for display
    std::string format_neuron_count(size_t count) {
        if (count >= 1000000) {
            return std::to_string(count / 1000000) + "M neurons";
        } else if (count >= 1000) {
            return std::to_string(count / 1000) + "K neurons";
        } else {
            return std::to_string(count) + " neurons";
        }
    }
    
    // Format time duration for display
    std::string format_time(double seconds) {
        if (seconds >= 3600) {
            size_t hours = static_cast<size_t>(seconds / 3600);
            size_t minutes = static_cast<size_t>((seconds - hours * 3600) / 60);
            return std::to_string(hours) + "h " + std::to_string(minutes) + "m";
        } else if (seconds >= 60) {
            size_t minutes = static_cast<size_t>(seconds / 60);
            double secs = seconds - minutes * 60;
            return std::to_string(minutes) + "m " + std::to_string(static_cast<int>(secs)) + "s";
        } else {
            return std::to_string(static_cast<int>(seconds)) + "s";
        }
    }
}

// Python exception classes
class NLMError : public std::runtime_error {
public:
    NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class InvalidConfigError : public NLMError {
public:
    InvalidConfigError(const std::string& msg) : NLMError("InvalidConfigError: " + msg) {}
};

class SimulationError : public NLMError {
public:
    SimulationError(const std::string& msg) : NLMError("SimulationError: " + msg) {}
};

} // namespace pybind11
} // namespace nlm

// ==============================================================================
// TYPE DEFINITIONS FOR PYTHON BINDINGS
// ==============================================================================

using ConfigPtr = std::shared_ptr<nlm::Config>;
using BrainPtr = std::shared_ptr<nlm::Brain>;
using AgentBrainPtr = std::shared_ptr<nlm::AgentBrain>;
using SimpleWorldPtr = std::shared_ptr<nlm::SimpleWorld>;

// ==============================================================================
// MAIN BINDINGS MODULE
// ==============================================================================

PYBIND11_MODULE(pynlm, m) {
    // ==============================================================================
    // MODULE DOCUMENTATION AND METADATA
    // ==============================================================================
    
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        
        A Python binding for the NLM C++ neural simulation framework.
        
        This module provides comprehensive access to an experimental artificial
        developmental brain system that learns from experience through neural
        dynamics, memory systems, and neuromodulation.
        
        Key Features:
        - LIF neuron dynamics with event-driven spike propagation
        - Multiple memory systems (working, episodic, associative)
        - Neuromodulation (dopamine, curiosity, novelty detection)
        - Developmental plasticity and learning
        - Prediction and planning systems
        - Interactive environment simulation
        
        For detailed documentation, see:
        - easy_usage.md - Beginner-friendly usage guide
        - HOW_TO_USE.md - Technical documentation
        - docs/ARCHITECTURE.md - System architecture
        
        Examples:
        ```python
        import pynlm
        
        # Create and initialize a brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for i in range(100):
            brain.step(i)
        
        print(f"Total spikes: {brain.getTotalSpikeCount()}")
        ```
        
        License: MIT
        Authors: NLM Authors
    )pbdoc";

    // ==============================================================================
    // EXCEPTION HANDLING
    // ==============================================================================
    
    // Register Python exception classes
    pybind11::register_exception<nlm::NLMError>(m, "NLMError");
    pybind11::register_exception<nlm::InvalidConfigError>(m, "InvalidConfigError");
    pybind11::register_exception<nlm::SimulationError>(m, "SimulationError");

    // Register exception for std::runtime_error
    pybind11::register_exception<std::runtime_error>(m, "RuntimeError");

    // ==============================================================================
    // UTILITY FUNCTIONS
    // ==============================================================================
    
    // Import helper functions into module namespace
    m.def("neuron_id_to_string", &nlm::pybind11::pybind_utils::neuron_id_to_string,
          "Convert NeuronId to readable string representation");
    
    m.def("synapse_id_to_string", &nlm::pybind11::pybind_utils::synapse_id_to_string,
          "Convert SynapseId to readable string representation");
    
    m.def("region_id_to_string", &nlm::pybind11::pybind_utils::region_id_to_string,
          "Convert RegionId to readable string representation");
    
    m.def("population_id_to_string", &nlm::pybind11::pybind_utils::population_id_to_string,
          "Convert PopulationId to readable string representation");
    
    // Format helpers
    m.def("format_spike_count", &nlm::pybind11::pybind_utils::format_spike_count,
          "Format spike count for display");
    
    m.def("format_neuron_count", &nlm::pybind11::pybind_utils::format_neuron_count,
          "Format neuron count for display");
    
    m.def("format_time", &nlm::pybind11::pybind_utils::format_time,
          "Format time duration for display");

    // ==============================================================================
    // CORE TYPES BINDINGS
    // ==============================================================================

    // NeuronId type
    pybind11::class_<nlm::NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(pybind11::init<>())
        .def(pybind11::init<uint64_t>(), pybind11::arg("value"))
        .def_readwrite("value", &nlm::NeuronId::value)
        .def("index", &nlm::NeuronId::index)
        .def("__eq__", &nlm::NeuronId::operator==)
        .def("__ne__", &nlm::NeuronId::operator!=)
        .def("__hash__", [](const nlm::NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const nlm::NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        });

    // SynapseId type
    pybind11::class_<nlm::SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(pybind11::init<>())
        .def(pybind11::init<uint64_t>(), pybind11::arg("value"))
        .def_readwrite("value", &nlm::SynapseId::value)
        .def("index", &nlm::SynapseId::index)
        .def("__eq__", &nlm::SynapseId::operator==)
        .def("__ne__", &nlm::SynapseId::operator!=)
        .def("__hash__", [](const nlm::SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const nlm::SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });

    // RegionId type
    pybind11::class_<nlm::RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(pybind11::init<>())
        .def(pybind11::init<uint64_t>(), pybind11::arg("value"))
        .def_readwrite("value", &nlm::RegionId::value)
        .def("index", &nlm::RegionId::index)
        .def("__eq__", &nlm::RegionId::operator==)
        .def("__ne__", &nlm::RegionId::operator!=)
        .def("__hash__", [](const nlm::RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const nlm::RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        });

    // PopulationId type
    pybind11::class_<nlm::PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(pybind11::init<>())
        .def(pybind11::init<uint64_t>(), pybind11::arg("value"))
        .def_readwrite("value", &nlm::PopulationId::value)
        .def("index", &nlm::PopulationId::index)
        .def("__eq__", &nlm::PopulationId::operator==)
        .def("__ne__", &nlm::PopulationId::operator!=)
        .def("__hash__", [](const nlm::PopulationId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const nlm::PopulationId& id) {
            return "<PopulationId: " + std::to_string(id.value) + ">";
        });

    // NeuronType enumeration
    pybind11::enum_<nlm::NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", nlm::NeuronType::Excitatory)
        .value("Inhibitory", nlm::NeuronType::Inhibitory)
        .value("Modulatory", nlm::NeuronType::Modulatory)
        .value("Sensory", nlm::NeuronType::Sensory)
        .value("Motor", nlm::NeuronType::Motor)
        .value("Internal", nlm::NeuronType::Internal)
        .export_values();

    // SynapseType enumeration
    pybind11::enum_<nlm::SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", nlm::SynapseType::Excitatory)
        .value("Inhibitory", nlm::SynapseType::Inhibitory)
        .value("Modulatory", nlm::SynapseType::Modulatory)
        .value("Electrical", nlm::SynapseType::Electrical)
        .value("GapJunction", nlm::SynapseType::GapJunction)
        .export_values();

    // DevelopmentalStage enumeration
    pybind11::enum_<nlm::DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", nlm::DevelopmentalStage::Initial)
        .value("CriticalPeriod", nlm::DevelopmentalStage::CriticalPeriod)
        .value("Maturation", nlm::DevelopmentalStage::Maturation)
        .value("Adult", nlm::DevelopmentalStage::Adult)
        .value("Aging", nlm::DevelopmentalStage::Aging)
        .export_values();

    // FiringState enumeration
    pybind11::enum_<nlm::FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", nlm::FiringState::Resting)
        .value("Active", nlm::FiringState::Active)
        .value("Refractory", nlm::FiringState::Refractory)
        .value("Inhibited", nlm::FiringState::Inhibited)
        .export_values();

    // ActionType enumeration
    pybind11::enum_<nlm::ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
        .value("MoveForward", nlm::ActionType::MoveForward)
        .value("MoveBackward", nlm::ActionType::MoveBackward)
        .value("MoveLeft", nlm::ActionType::MoveLeft)
        .value("MoveRight", nlm::ActionType::MoveRight)
        .value("TurnLeft", nlm::ActionType::TurnLeft)
        .value("TurnRight", nlm::ActionType::TurnRight)
        .value("Look", nlm::ActionType::Look)
        .value("LookUp", nlm::ActionType::LookUp)
        .value("LookDown", nlm::ActionType::LookDown)
        .value("Interact", nlm::ActionType::Interact)
        .value("Eat", nlm::ActionType::Eat)
        .value("Drink", nlm::ActionType::Drink)
        .value("Rest", nlm::ActionType::Rest)
        .value("Wait", nlm::ActionType::Wait)
        .value("Custom", nlm::ActionType::Custom)
        .export_values();

    // MotorCommand enumeration
    pybind11::enum_<nlm::MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", nlm::MotorCommand::MoveForward)
        .value("MoveBackward", nlm::MotorCommand::MoveBackward)
        .value("TurnLeft", nlm::MotorCommand::TurnLeft)
        .value("TurnRight", nlm::MotorCommand::TurnRight)
        .value("LookLeft", nlm::MotorCommand::LookLeft)
        .value("LookRight", nlm::MotorCommand::LookRight)
        .value("Interact", nlm::MotorCommand::Interact)
        .value("Wait", nlm::MotorCommand::Wait)
        .export_values();

    // WorldObjectType enumeration
    pybind11::enum_<nlm::WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", nlm::WorldObjectType::Empty)
        .value("Resource", nlm::WorldObjectType::Resource)
        .value("Hazard", nlm::WorldObjectType::Hazard)
        .value("Wall", nlm::WorldObjectType::Wall)
        .value("Marker", nlm::WorldObjectType::Marker)
        .export_values();

    // ==============================================================================
    // CONFIGURATION SYSTEM BINDINGS
    // ==============================================================================

    // Config class
    pybind11::class_<nlm::Config, ConfigPtr>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(pybind11::init<>())
        .def("loadFromFile", &nlm::Config::loadFromFile, pybind11::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](nlm::Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, pybind11::arg("argc"), pybind11::arg("argv"),
            "Load configuration from command line arguments")
        .def("saveToFile", &nlm::Config::saveToFile, pybind11::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &nlm::Config::has, pybind11::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &nlm::Config::getKeys,
             "Get all configuration keys")
        .def("clear", &nlm::Config::clear,
             "Clear all configuration entries")
        .def("summary", &nlm::Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const nlm::Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        // Convenience methods for common configuration operations
        .def("set_int", [](nlm::Config& self, const std::string& key, int value) {
            self.set(key, value);
        }, pybind11::arg("key"), pybind11::arg("value"), "Set integer value")
        .def("set_float", [](nlm::Config& self, const std::string& key, double value) {
            self.set(key, value);
        }, pybind11::arg("key"), pybind11::arg("value"), "Set float value")
        .def("set_string", [](nlm::Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, pybind11::arg("key"), pybind11::arg("value"), "Set string value")
        .def("set_bool", [](nlm::Config& self, const std::string& key, bool value) {
            self.set(key, value);
        }, pybind11::arg("key"), pybind11::arg("value"), "Set boolean value")
        .def("get_int", [](nlm::Config& self, const std::string& key, int defaultValue) {
            auto result = self.get<int>(key);
            return result ? *result : defaultValue;
        }, pybind11::arg("key"), pybind11::arg("defaultValue") = 0, "Get integer value with default")
        .def("get_float", [](nlm::Config& self, const std::string& key, double defaultValue) {
            auto result = self.get<double>(key);
            return result ? *result : defaultValue;
        }, pybind11::arg("key"), pybind11::arg("defaultValue") = 0.0, "Get float value with default")
        .def("get_string", [](nlm::Config& self, const std::string& key, const std::string& defaultValue) {
            auto result = self.get<std::string>(key);
            return result ? *result : defaultValue;
        }, pybind11::arg("key"), pybind11::arg("defaultValue") = "", "Get string value with default")
        .def("get_bool", [](nlm::Config& self, const std::string& key, bool defaultValue) {
            auto result = self.get<bool>(key);
            return result ? *result : defaultValue;
        }, pybind11::arg("key"), pybind11::arg("defaultValue") = false, "Get boolean value with default");

    // Factory functions for configuration
    m.def("createDefaultConfig", []() -> ConfigPtr {
        return std::make_shared<nlm::Config>();
    }, "Create a default configuration");

    // ==============================================================================
    // SENSORY SYSTEMS BINDINGS
    // ==============================================================================

    // SensoryInput base class
    pybind11::class_<nlm::SensoryInput, std::shared_ptr<nlm::SensoryInput>>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &nlm::SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &nlm::SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &nlm::SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &nlm::SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &nlm::SensoryInput::setTimestamp, pybind11::arg("timestamp"),
             "Set the timestamp");

    // Vision class
    pybind11::class_<nlm::Vision, nlm::SensoryInput, std::shared_ptr<nlm::Vision>>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(pybind11::init<>())
        .def(pybind11::init<size_t, size_t, size_t>(), pybind11::arg("width"), pybind11::arg("height"),
             pybind11::arg("channels") = 3)
        .def("setData", [](nlm::Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, pybind11::arg("data"))
        .def("getWidth", &nlm::Vision::getWidth)
        .def("getHeight", &nlm::Vision::getHeight)
        .def("getChannels", &nlm::Vision::getChannels);

    // Audio class
    pybind11::class_<nlm::Audio, nlm::SensoryInput, std::shared_ptr<nlm::Audio>>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(pybind11::init<>())
        .def(pybind11::init<size_t, size_t>(), pybind11::arg("sampleRate"), pybind11::arg("numSamples"))
        .def("setData", [](nlm::Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, pybind11::arg("data"))
        .def("setSampleRate", &nlm::Audio::setSampleRate, pybind11::arg("sampleRate"))
        .def("getSampleRate", &nlm::Audio::getSampleRate)
        .def("getNumSamples", &nlm::Audio::getNumSamples);

    // InternalSignals class
    pybind11::class_<nlm::InternalSignals, nlm::SensoryInput, std::shared_ptr<nlm::InternalSignals>>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(pybind11::init<>())
        .def("addSignal", &nlm::InternalSignals::addSignal, pybind11::arg("value"))
        .def("clearSignals", &nlm::InternalSignals::clearSignals);

    // ==============================================================================
    // MOTOR SYSTEM BINDINGS
    // ==============================================================================

    // Action class
    pybind11::class_<nlm::Action, std::shared_ptr<nlm::Action>>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(pybind11::init<>())
        .def(pybind11::init<nlm::ActionType>(), pybind11::arg("type"))
        .def(pybind11::init<nlm::ActionType, std::vector<float>>(), pybind11::arg("type"), pybind11::arg("parameters"))
        .def("getType", &nlm::Action::getType)
        .def("setType", &nlm::Action::setType, pybind11::arg("type"))
        .def("getParameters", &nlm::Action::getParameters)
        .def("setParameters", &nlm::Action::setParameters, pybind11::arg("params"))
        .def("getName", &nlm::Action::getName)
        .def("clone", &nlm::Action::clone)
        .def("__repr__", [](const nlm::Action& action) {
            std::string typeStr = nlm::pybind11::pybind_utils::action_type_to_string(action.getType());
            return "<Action: " + typeStr + ">";
        });

    // WorldObject class
    pybind11::class_<nlm::WorldObject, std::shared_ptr<nlm::WorldObject>>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(pybind11::init<>())
        .def(pybind11::init<float, float, nlm::WorldObjectType, float, float>(),
             pybind11::arg("x"), pybind11::arg("y"), pybind11::arg("type"), pybind11::arg("value") = 0.0f,
             pybind11::arg("radius") = 0.5f)
        .def_readwrite("x", &nlm::WorldObject::x)
        .def_readwrite("y", &nlm::WorldObject::y)
        .def_readwrite("radius", &nlm::WorldObject::radius)
        .def_readwrite("type", &nlm::WorldObject::type)
        .def_readwrite("value", &nlm::WorldObject::value)
        .def_readwrite("active", &nlm::WorldObject::active)
        .def("__repr__", [](const nlm::WorldObject& obj) {
            std::string typeStr = nlm::pybind11::pybind_utils::world_object_type_to_string(obj.type);
            return "<WorldObject: " + typeStr + " at (" + std::to_string(obj.x) + "," + std::to_string(obj.y) + ")>";
        });

    // ==============================================================================
    // AGENT AND BODY SYSTEMS BINDINGS
    // ==============================================================================

    // AgentBody class
    pybind11::class_<nlm::AgentBody, std::shared_ptr<nlm::AgentBody>>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(pybind11::init<>())
        .def_readwrite("x", &nlm::AgentBody::x)
        .def_readwrite("y", &nlm::AgentBody::y)
        .def_readwrite("orientation", &nlm::AgentBody::orientation)
        .def_readwrite("velocityX", &nlm::AgentBody::velocityX)
        .def_readwrite("velocityY", &nlm::AgentBody::velocityY)
        .def_readwrite("angularVelocity", &nlm::AgentBody::angularVelocity)
        .def_readwrite("energy", &nlm::AgentBody::energy)
        .def_readwrite("health", &nlm::AgentBody::health)
        .def_readwrite("age", &nlm::AgentBody::age)
        .def_readwrite("isMoving", &nlm::AgentBody::isMoving)
        .def_readwrite("isTurning", &nlm::AgentBody::isTurning)
        .def_readwrite("lastActionTime", &nlm::AgentBody::lastActionTime)
        .def("reset", &nlm::AgentBody::reset);

    // ActionResult class
    pybind11::class_<nlm::ActionResult, std::shared_ptr<nlm::ActionResult>>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(pybind11::init<>())
        .def(pybind11::init<float, bool, std::string>(),
             pybind11::arg("reward"), pybind11::arg("success"), pybind11::arg("message") = "")
        .def_readwrite("reward", &nlm::ActionResult::reward)
        .def_readwrite("success", &nlm::ActionResult::success)
        .def_readwrite("message", &nlm::ActionResult::message);

    // SensoryPercept class
    pybind11::class_<nlm::SensoryPercept, std::shared_ptr<nlm::SensoryPercept>>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(pybind11::init<>())
        .def("getVision", &nlm::SensoryPercept::getVision)
        .def("setVision", &nlm::SensoryPercept::setVision, pybind11::arg("vision"))
        .def("getVisionWidth", &nlm::SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &nlm::SensoryPercept::getVisionHeight)
        .def("getTouch", &nlm::SensoryPercept::getTouch)
        .def("setTouch", &nlm::SensoryPercept::setTouch, pybind11::arg("touch"))
        .def("getInternal", &nlm::SensoryPercept::getInternal)
        .def("setInternal", &nlm::SensoryPercept::setInternal, pybind11::arg("internal"))
        .def("getProprioception", &nlm::SensoryPercept::getProprioception)
        .def("setProprioception", &nlm::SensoryPercept::setProprioception, pybind11::arg("proprioception"))
        .def("getAudio", &nlm::SensoryPercept::getAudio)
        .def("setAudio", &nlm::SensoryPercept::setAudio, pybind11::arg("audio"))
        .def("getAllSignals", &nlm::SensoryPercept::getAllSignals)
        .def("getTimestamp", &nlm::SensoryPercept::getTimestamp)
        .def("setTimestamp", &nlm::SensoryPercept::setTimestamp, pybind11::arg("timestamp"));

    // ==============================================================================
    // WORLD SYSTEM BINDINGS
    // ==============================================================================

    // SimpleWorld class
    pybind11::class_<nlm::SimpleWorld, std::shared_ptr<nlm::SimpleWorld>>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(pybind11::init<>())
        .def("configure", &nlm::SimpleWorld::configure, pybind11::arg("width"), pybind11::arg("height"),
             pybind11::arg("visionWidth"), pybind11::arg("visionHeight"))
        .def("reset", &nlm::SimpleWorld::reset)
        .def("setAgentStart", &nlm::SimpleWorld::setAgentStart, pybind11::arg("x"), pybind11::arg("y"))
        .def("update", &nlm::SimpleWorld::update, pybind11::arg("timestep"))
        .def("applyMotorCommand", &nlm::SimpleWorld::applyMotorCommand,
             pybind11::arg("cmd"), pybind11::arg("currentTime"))
        .def("getSensoryPercept", &nlm::SimpleWorld::getSensoryPercept,
             pybind11::return_value_policy::reference_internal)
        .def("getAgentBody", &nlm::SimpleWorld::getAgentBody,
             pybind11::return_value_policy::reference_internal)
        .def("addObject", &nlm::SimpleWorld::addObject, pybind11::arg("obj"))
        .def("removeObject", &nlm::SimpleWorld::removeObject, pybind11::arg("x"), pybind11::arg("y"))
        .def("isValidPosition", &nlm::SimpleWorld::isValidPosition, pybind11::arg("x"), pybind11::arg("y"))
        .def("getWidth", &nlm::SimpleWorld::getWidth)
        .def("getHeight", &nlm::SimpleWorld::getHeight)
        .def("getMaxEnergy", &nlm::SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &nlm::SimpleWorld::setMaxEnergy, pybind11::arg("e"))
        .def("getEnergyDecayRate", &nlm::SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &nlm::SimpleWorld::setEnergyDecayRate, pybind11::arg("r"))
        .def("getSimulationTime", &nlm::SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &nlm::SimpleWorld::setRandomSeed, pybind11::arg("seed"))
        .def("getRandomSeed", &nlm::SimpleWorld::getRandomSeed);

    // ==============================================================================
    // BRAIN SYSTEM BINDINGS
    // ==============================================================================

    // Brain class
    pybind11::class_<nlm::Brain, BrainPtr>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(pybind11::init<std::shared_ptr<nlm::Config>>(), pybind11::arg("config"))
        .def("initialize", &nlm::Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (nlm::Brain::*)(nlm::SimulationStep)>(&nlm::Brain::step),
             pybind11::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (nlm::Brain::*)(nlm::SimulationStep, nlm::Timestamp)>(&nlm::Brain::step),
             pybind11::arg("currentStep"), pybind11::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &nlm::Brain::receiveSensoryInput,
             pybind11::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &nlm::Brain::injectCurrent,
             pybind11::arg("neuron"), pybind11::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &nlm::Brain::injectCurrentToNeurons,
             pybind11::arg("type"), pybind11::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &nlm::Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &nlm::Brain::reset,
             "Reset brain state")
        .def("save", &nlm::Brain::save, pybind11::arg("filepath"),
             "Save brain state to file")
        .def("load", &nlm::Brain::load, pybind11::arg("filepath"),
             "Load brain state from file")
        .def("addRegion", &nlm::Brain::addRegion, pybind11::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", &nlm::Brain::getRegion, pybind11::arg("id"),
             pybind11::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("getRegionCount", &nlm::Brain::getRegionCount,
             "Get the number of regions")
        .def("getRegionIds", &nlm::Brain::getRegionIds,
             "Get all region IDs")
        .def("getRegions", &nlm::Brain::getRegions,
             pybind11::return_value_policy::reference_internal,
             "Get all regions")
        .def("getTotalNeuronCount", &nlm::Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &nlm::Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &nlm::Brain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("getFiringNeuronCount", &nlm::Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("getAverageFiringRate", &nlm::Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &nlm::Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &nlm::Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("getDevelopmentalStage", &nlm::Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &nlm::Brain::setDevelopmentalStage,
             pybind11::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &nlm::Brain::getConfig,
             pybind11::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &nlm::Brain::logStatus,
             "Log brain status")
        // Convenience methods for batch operations
        .def("runSteps", [](BrainPtr& brain, size_t numSteps, float timestep = 0.001f) {
            for (size_t i = 0; i < numSteps; ++i) {
                brain->step(static_cast<nlm::SimulationStep>(i), static_cast<nlm::Timestamp>(i * timestep));
            }
        }, pybind11::arg("numSteps"), pybind11::arg("timestep") = 0.001f,
            "Run multiple simulation steps efficiently")
        .def("getNeuronCountByType", [](const BrainPtr& brain, nlm::NeuronType type) {
            size_t count = 0;
            // This would require access to internal brain structure
            // For now, return total count as placeholder
            return brain->getTotalNeuronCount();
        }, pybind11::arg("type"), "Get neuron count by type")
        .def("getAverageMembranePotential", [](const BrainPtr& brain) {
            // This would require access to neuron internal state
            // For now, return a placeholder
            return -70.0f;
        }, "Get average membrane potential (placeholder)")
        .def("getSynapseDensity", [](const BrainPtr& brain) {
            size_t totalNeurons = brain->getTotalNeuronCount();
            size_t totalSynapses = brain->getTotalSynapseCount();
            return totalNeurons > 0 ? static_cast<float>(totalSynapses) / totalNeurons : 0.0f;
        }, "Get synapse density (synapses per neuron)")
        
        // Enhanced convenience methods for batch operations
        .def("run_steps_efficient", &nlm::Brain::runSteps,
             pybind11::arg("numSteps"), pybind11::arg("timestep") = 0.001f,
             "Run multiple simulation steps efficiently")
        
        .def("inject_current_batch", &nlm::Brain::injectCurrentToNeurons,
             pybind11::arg("neurons"), pybind11::arg("current"),
             "Inject current to multiple neurons efficiently")
        
        .def("get_region_stats", &nlm::Brain::getRegionStats,
             pybind11::arg("regionId"),
             "Get detailed statistics for a specific region")
        
        .def("get_all_region_stats", &nlm::Brain::getAllRegionStats,
             "Get statistics for all regions")
        
        .def("get_neuron_count_by_type_map", &nlm::Brain::getNeuronCountByType,
             "Get neuron count by type (excitatory, inhibitory, etc.)")
        
        .def("get_firing_rate_by_type_map", &nlm::Brain::getFiringRateByType,
             "Get firing rates by neuron type")
        
        .def("update_working_memory_pattern", [](BrainPtr& brain, const std::vector<float>& pattern) {
            brain->updateWorkingMemory(pattern);
        }, pybind11::arg("pattern"),
             "Update working memory with input pattern")
        
        .def("get_working_memory_activation", &nlm::Brain::getWorkingMemoryActivation,
             "Get working memory activation levels")
        
        .def("store_episodic_memory", [](BrainPtr& brain, const std::vector<float>& pattern, float importance = 1.0f) {
            brain->storeEpisodicMemory(pattern, importance);
        }, pybind11::arg("pattern"), pybind11::arg("importance") = 1.0f,
             "Store pattern in episodic memory")
        
        .def("retrieve_episodic_memory", [](const BrainPtr& brain, size_t episodeIndex = 0) {
            return brain->retrieveEpisodicMemory(episodeIndex);
        }, pybind11::arg("episodeIndex") = 0,
             "Retrieve pattern from episodic memory")
        
        .def("compute_prediction", [](const BrainPtr& brain, const std::vector<float>& input) {
            return brain->computePrediction(input);
        }, pybind11::arg("input"),
             "Compute prediction for given input")
        
        .def("get_prediction_error", &nlm::Brain::getPredictionError,
             "Get current prediction error")
        
        .def("apply_reward_modulation_batch", [](BrainPtr& brain, float reward, float predictedReward = 0.0f) {
            brain->applyRewardModulation(reward, predictedReward);
        }, pybind11::arg("reward"), pybind11::arg("predictedReward") = 0.0f,
             "Apply reward-based neuromodulation")
        
        .def("apply_curiosity_modulation_batch", [](BrainPtr& brain, float novelty, float explorationDrive = 0.0f) {
            brain->applyCuriosityModulation(novelty, explorationDrive);
        }, pybind11::arg("novelty"), pybind11::arg("explorationDrive") = 0.0f,
             "Apply curiosity-driven exploration")
        
        .def("apply_dopamine_modulation_batch", [](BrainPtr& brain, float predictionError, float learningRate = 0.001f) {
            brain->applyDopamineModulation(predictionError, learningRate);
        }, pybind11::arg("predictionError"), pybind11::arg("learningRate") = 0.001f,
             "Apply dopamine-based modulation")
        
        .def("advance_development_stage", &nlm::Brain::advanceDevelopmentStage,
             "Advance developmental stage")
        
        .def("accelerate_development_batch", [](BrainPtr& brain, float factor = 1.0f) {
            brain->accelerateDevelopment(factor);
        }, pybind11::arg("factor") = 1.0f,
             "Accelerate development process")
        
        .def("plan_actions_batch", [](const BrainPtr& brain, size_t planningHorizon = 5) {
            return brain->planActions(planningHorizon);
        }, pybind11::arg("planningHorizon") = 5,
             "Plan actions for future steps")
        
        .def("refine_action_plan_batch", [](BrainPtr& brain, const std::vector<ActionType>& currentPlan,
                                             const std::vector<float>& feedback) {
            return brain->refineActionPlan(currentPlan, feedback);
        }, pybind11::arg("currentPlan"), pybind11::arg("feedback"),
             "Refine action plan based on feedback")
        
        .def("get_active_neurons_batch", &nlm::Brain::getActiveNeurons,
             "Get list of currently active neurons")
        
        .def("get_firing_neurons_batch", &nlm::Brain::getFiringNeurons,
             "Get list of currently firing neurons")
        
        .def("get_neuron_count_by_type_map_enhanced", &nlm::Brain::getNeuronCountByType,
             "Get neuron count by type as map")
        
        .def("apply_stdp_batch", [](BrainPtr& brain, float learningRate = 0.001f, float tauPlus = 20.0f, float tauMinus = 20.0f) {
            brain->applySTDP(learningRate, tauPlus, tauMinus);
        }, pybind11::arg("learningRate"), pybind11::arg("tauPlus"), pybind11::arg("tauMinus"),
             "Apply STDP with custom parameters")
        
        .def("strengthen_synapses_batch", [](BrainPtr& brain, const std::vector<std::pair<SynapseId, float>>& changes) {
            brain->strengthenSynapses(changes);
        }, pybind11::arg("changes"),
             "Strengthen multiple synapses")
        
        .def("get_strong_synapses_batch", &nlm::Brain::getStrongSynapses,
             pybind11::arg("threshold") = 0.5f,
             "Get synapses above strength threshold")
        
        .def("add_neurons_to_region_batch", [](BrainPtr& brain, const std::vector<NeuronId>& neurons, RegionId regionId) {
            brain->addNeuronsToRegion(neurons, regionId);
        }, pybind11::arg("neurons"), pybind11::arg("regionId"),
             "Add multiple neurons to region")
        
        .def("set_region_excitability_batch", [](BrainPtr& brain, RegionId regionId, float excitability) {
            brain->setRegionExcitability(regionId, excitability);
        }, pybind11::arg("regionId"), pybind11::arg("excitability"),
             "Set excitability for all neurons in region")
        
        .def("reset_brain_completely", &nlm::Brain::resetBrain,
             "Completely reset brain state")
        
        .def("save_to_checkpoint_batch", [](const BrainPtr& brain, const std::string& path) {
            return brain->saveToCheckpoint(path);
        }, pybind11::arg("path"),
             "Save brain state to checkpoint")
        
        .def("load_from_checkpoint_batch", [](BrainPtr& brain, const std::string& path) {
            brain->loadFromCheckpoint(path);
        }, pybind11::arg("path"),
             "Load brain state from checkpoint")
        
        .def("get_performance_metrics_batch", &nlm::Brain::getPerformanceMetrics,
             "Get brain performance metrics")
        
        .def("check_for_anomalies_batch", &nlm::Brain::checkForAnomalies,
             "Check for system anomalies")
        
        .def("get_active_alerts_batch", &nlm::Brain::getActiveAlerts,
             "Get active system alerts")
        
        .def("update_configuration_parameter_batch", [](BrainPtr& brain, const std::string& key, const pybind11::object& value) {
            return brain->updateConfigurationParameter(key, value);
        }, pybind11::arg("key"), pybind11::arg("value"),
             "Update configuration parameter with Python object")
        
        .def("batch_update_configuration", [](BrainPtr& brain, const std::map<std::string, pybind11::object>& updates) {
            return brain->batchUpdateConfiguration(updates);
        }, pybind11::arg("updates"),
             "Batch update multiple configuration parameters")
        
        .def("get_system_status_batch", &nlm::Brain::getSystemStatus,
             "Get comprehensive system status")
        
        .def("get_brain_diagnostics_batch", &nlm::Brain::getBrainDiagnostics,
             "Get comprehensive brain diagnostics")
        
        .def("detect_errors_batch", &nlm::Brain::detectErrors,
             "Detect system errors")
        
        .def("get_recent_errors_batch", &nlm::Brain::getRecentErrors,
             "Get recent error messages")
        
        .def("attempt_recovery_batch", &nlm::Brain::attemptRecovery,
             "Attempt system recovery")
        
        .def("validate_brain_integrity_batch", &nlm::Brain::validateBrainIntegrity,
             "Validate brain system integrity")
        
        .def("get_brain_diagnostics_enhanced", &nlm::Brain::getBrainDiagnostics,
             "Get comprehensive brain diagnostics")
        
        .def("get_resource_usage_batch", &nlm::Brain::getResourceUsage,
             "Get resource usage statistics")
        
        .def("start_profiling_batch", &nlm::Brain::startProfiling,
             "Start brain profiling")
        
        .def("stop_profiling_batch", &nlm::Brain::stopProfiling,
             "Stop brain profiling")
        
        .def("is_profiling_batch", &nlm::Brain::isProfiling,
             "Check if profiling is active")
        
        .def("get_profiling_results_batch", &nlm::Brain::getProfilingResults,
             "Get profiling results");

    // AgentBrain class
    pybind11::class_<nlm::AgentBrain, std::shared_ptr<nlm::AgentBrain>>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(pybind11::init<std::shared_ptr<nlm::Brain>>(), pybind11::arg("brain"))
        .def("initialize", &nlm::AgentBrain::initialize, pybind11::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &nlm::AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &nlm::AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &nlm::AgentBrain::processSensoryInput,
             pybind11::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &nlm::AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &nlm::AgentBrain::applyRewardModulation,
             pybind11::arg("reward"), pybind11::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &nlm::AgentBrain::updateDevelopment,
             pybind11::arg("timestep"),
             "Update development system")
        .def("getDevelopmentalStage", &nlm::AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("getNeuromodulationLevel", &nlm::AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("getCuriosityLevel", &nlm::AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("getNoveltyLevel", &nlm::AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("getPredictionError", &nlm::AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &nlm::AgentBrain::reset,
             "Reset agent for new episode")
        .def("getBrain", &nlm::AgentBrain::getBrain,
             pybind11::return_value_policy::reference_internal,
             "Get the underlying brain")
        // Subsystem enable/disable methods
        .def("enableRewardModulation", &nlm::AgentBrain::enableRewardModulation,
             pybind11::arg("enable"))
        .def("enableStructuralPlasticity", &nlm::AgentBrain::enableStructuralPlasticity,
             pybind11::arg("enable"))
        .def("enableDevelopment", &nlm::AgentBrain::enableDevelopment,
             pybind11::arg("enable"))
        .def("enableCuriosity", &nlm::AgentBrain::enableCuriosity,
             pybind11::arg("enable"))
        .def("isRewardModulationEnabled", &nlm::AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &nlm::AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &nlm::AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &nlm::AgentBrain::isCuriosityEnabled);

    // Factory functions for creating main objects
    m.def("createDefaultConfig", []() -> ConfigPtr {
        return std::make_shared<nlm::Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](ConfigPtr config) -> BrainPtr {
        return std::make_shared<nlm::Brain>(config);
    }, pybind11::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> SimpleWorldPtr {
        return std::make_shared<nlm::SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](BrainPtr brain) -> AgentBrainPtr {
        return std::make_shared<nlm::AgentBrain>(brain);
    }, pybind11::arg("brain"), "Create a new agent brain interface");

    // Invalid object constants
    m.attr("INVALID_NEURON_ID") = pybind11::cast<nlm::NeuronId>(nlm::INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = pybind11::cast<nlm::SynapseId>(nlm::INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = pybind11::cast<nlm::RegionId>(nlm::INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = pybind11::cast<nlm::PopulationId>(nlm::INVALID_POPULATION_ID);

    // Legacy compatibility functions (for backward compatibility)
    m.def("createAgent", [](BrainPtr brain, SimpleWorldPtr world) -> AgentBrainPtr {
        auto agent = std::make_shared<nlm::AgentBrain>(brain);
        agent->initialize(*world);
        return agent;
    }, pybind11::arg("brain"), pybind11::arg("world"), "Legacy agent creation function");

    // Statistics and monitoring helpers
    m.def("format_spike_count", &nlm::pybind11::pybind_utils::format_spike_count,
          "Format spike count for display");

    m.def("format_neuron_count", &nlm::pybind11::pybind_utils::format_neuron_count,
          "Format neuron count for display");

    m.def("format_time", &nlm::pybind11::pybind_utils::format_time,
          "Format time duration for display");

    // ==============================================================================
    // UTILITY FUNCTIONS
    // ==============================================================================

    // Error helper functions
    m.def("validate_neuron_count", &nlm::pybind11::pybind_utils::safe_neuron_count,
          "Validate neuron count against maximum limit");

    m.def("validate_step", &nlm::pybind11::pybind_utils::validate_step,
          "Validate simulation step number");

    m.def("validate_time", &nlm::pybind11::pybind_utils::validate_time,
          "Validate simulation time");

    // ==============================================================================
    // LEGACY COMPATIBILITY
    // ==============================================================================

    // Previous function names (for backward compatibility)
    m.def("createBrain", [](ConfigPtr config) -> BrainPtr {
        return std::make_shared<nlm::Brain>(config);
    }, "Legacy function name - use createBrain instead");

    m.def("createAgentBrain", [](BrainPtr brain) -> AgentBrainPtr {
        return std::make_shared<nlm::AgentBrain>(brain);
    }, "Legacy function name - use createAgentBrain instead");

    m.def("createSimpleWorld", []() -> SimpleWorldPtr {
        return std::make_shared<nlm::SimpleWorld>();
    }, "Legacy function name - use createSimpleWorld instead");

    m.def("createDefaultConfig", []() -> ConfigPtr {
        return std::make_shared<nlm::Config>();
    }, "Legacy function name - use createDefaultConfig instead");

    // ==============================================================================
    // VERSION AND METADATA
    // ==============================================================================

    m.attr("__version__") = "0.1.0";
    m.attr("__author__") = "NLM Authors";
    m.attr("__description__") = "Python bindings for Neural Learning Machine";

    m.def("getVersion", []() { return std::string("0.1.0"); },
          "Get NLM Python bindings version");

    m.def("getAuthor", []() { return std::string("NLM Authors"); },
          "Get NLM Python bindings author");

    m.def("getBuildInfo", []() {
        return std::string("Built with C++20, pybind11 " PYBIND11_VERSION);;
    }, "Get build information");

    std::cout << "Python bindings initialized successfully!" << std::endl;
    std::cout << "Available modules: Config, Brain, AgentBrain, SimpleWorld, Vision, Audio, etc." << std::endl;
    std::cout << "Legacy compatibility functions are available for backward compatibility." << std::endl;
}

} // namespace nlm
