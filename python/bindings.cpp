#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <utility>
#include <fstream>

// Core headers
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/core/Logger/Logger.hpp"
#include "../src/core/SimulationClock/SimulationClock.hpp"
#include "../src/core/Random/Random.hpp"

// Neural system headers
#include "../src/brain/Brain.hpp"
#include "../src/brain/Neuron.hpp"
#include "../src/brain/Synapse.hpp"
#include "../src/brain/NeuralPopulation.hpp"
#include "../src/brain/NeuralRegion.hpp"

// Dynamics and plasticity
#include "../src/dynamics/SpikeSystem.hpp"
#include "../src/plasticity/PlasticityRule.hpp"
#include "../src/plasticity/Hebbian.hpp"
#include "../src/plasticity/STDP.hpp"
#include "../src/plasticity/StructuralPlasticity.hpp"

// Memory systems
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/memory/NeuralAssociativeMemory.hpp"

// Neuromodulation
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/Curiosity.hpp"

// Cognition systems
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/cognition/AttentionalSelection.hpp"

// Development system
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/development/Synaptogenesis.hpp"
#include "../src/development/Pruning.hpp"
#include "../src/development/Maturation.hpp"

// Prediction system
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/prediction/NeuralPrediction.hpp"

// Environment and agent systems
#include "../src/environment/Environment.hpp"
#include "../src/environment/Observation.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

// World system
#include "../src/world/SimpleWorld.hpp"

// Sensory and motor systems
#include "../src/sensory/SensoryInput.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"
#include "../src/motor/Action.hpp"
#include "../src/motor/MotorSystem.hpp"

// Experiment framework
#include "../src/experiments/Experiment.hpp"
#include "../src/experiments/Metrics.hpp"

namespace py = pybind11;
namespace nlm {

// Custom exception types for better error handling
class InvalidNeuronIdError : public std::runtime_error {
public:
    explicit InvalidNeuronIdError(const std::string& msg) : std::runtime_error(msg) {}
};

class InvalidSynapseIdError : public std::runtime_error {
public:
    explicit InvalidSynapseIdError(const std::string& msg) : std::runtime_error(msg) {}
};

class FileNotFoundError : public std::runtime_error {
public:
    explicit FileNotFoundError(const std::string& msg) : std::runtime_error(msg) {}
};

class InvalidParameterError : public std::runtime_error {
public:
    explicit InvalidParameterError(const std::string& msg) : std::runtime_error(msg) {}
};

class MemoryAllocationError : public std::runtime_error {
public:
    explicit MemoryAllocationError(const std::string& msg) : std::runtime_error(msg) {}
};

class IntegrationError : public std::runtime_error {
public:
    explicit IntegrationError(const std::string& msg) : std::runtime_error(msg) {}
};

// Helper function for parameter validation
template<typename T>
T validateParameter(const T& value, const T& min, const T& max, const std::string& name) {
    if (value < min || value > max) {
        throw InvalidParameterError("Parameter " + name + " must be between " +
                                   std::to_string(min) + " and " + std::to_string(max) +
                                   ", got " + std::to_string(value));
    }
    return value;
}

// Helper function for file operations
bool safeSaveFile(const std::string& filepath, const std::vector<uint8_t>& data) {
    try {
        std::ofstream file(filepath, std::ios::binary);
        if (!file) {
            throw FileNotFoundError("Cannot open file for writing: " + filepath);
        }
        file.write(reinterpret_cast<const char*>(data.data()), data.size());
        if (!file) {
            throw FileNotFoundError("Error writing to file: " + filepath);
        }
        file.close();
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

// Helper function for file loading
std::vector<uint8_t> safeLoadFile(const std::string& filepath) {
    try {
        std::ifstream file(filepath, std::ios::binary);
        if (!file) {
            throw FileNotFoundError("Cannot open file for reading: " + filepath);
        }
        file.seekg(0, std::ios::end);
        size_t fileSize = file.tellg();
        file.seekg(0, std::ios::beg);
        
        std::vector<uint8_t> data(fileSize);
        if (!file.read(reinterpret_cast<char*>(data.data()), fileSize)) {
            throw FileNotFoundError("Error reading from file: " + filepath);
        }
        file.close();
        return data;
    } catch (const std::exception& e) {
        throw;
    }
}

// Vector helper functions for better Python integration
template<typename T>
std::vector<T> createVector(const std::vector<T>& vec) {
    return vec;
}

template<typename T>
std::vector<T> copyVector(const std::vector<T>& vec) {
    return vec;
}

// Type aliases for better readability
using FloatVector = std::vector<float>;
using IntVector = std::vector<uint64_t>;
using DoubleVector = std::vector<double>;
using StringVector = std::vector<std::string>;

// Helper functions for vector operations
template<typename T>
FloatVector floatVector(const std::vector<T>& input) {
    FloatVector result;
    result.reserve(input.size());
    for (const auto& item : input) {
        result.push_back(static_cast<float>(item));
    }
    return result;
}

template<typename T>
IntVector intVector(const std::vector<T>& input) {
    IntVector result;
    result.reserve(input.size());
    for (const auto& item : input) {
        result.push_back(static_cast<uint64_t>(item));
    }
    return result;
}

// Namespace aliases for backward compatibility
namespace py_types = pybind11::literals;

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides comprehensive access to neural brain systems, memory, cognition,
        and advanced simulation capabilities.
        
        Key features:
        - Complete brain simulation with real spiking neural computation
        - Memory systems (working, episodic, associative)
        - Neuromodulation (dopamine, curiosity, novelty)
        - Cognitive functions (planning, attention, concept formation)
        - Development and learning systems
        - Prediction and error computation
        - Advanced agent-world interaction
        - Checkpoint saving and loading
        - Extensive error handling and validation
    )pbdoc";

    // Register custom exceptions for better error handling
    py::register_exception<InvalidNeuronIdError>(m, "InvalidNeuronIdError");
    py::register_exception<InvalidSynapseIdError>(m, "InvalidSynapseIdError");
    py::register_exception<FileNotFoundError>(m, "FileNotFoundError");
    py::register_exception<InvalidParameterError>(m, "InvalidParameterError");
    py::register_exception<MemoryAllocationError>(m, "MemoryAllocationError");
    py::register_exception<IntegrationError>(m, "IntegrationError");

    // === ENUM DEFINITIONS ===

    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
        .value("MoveForward", ActionType::MoveForward)
        .value("MoveBackward", ActionType::MoveBackward)
        .value("MoveLeft", ActionType::MoveLeft)
        .value("MoveRight", ActionType::MoveRight)
        .value("TurnLeft", ActionType::TurnLeft)
        .value("TurnRight", ActionType::TurnRight)
        .value("Look", ActionType::Look)
        .value("LookUp", ActionType::LookUp)
        .value("LookDown", ActionType::LookDown)
        .value("Interact", ActionType::Interact)
        .value("Eat", ActionType::Eat)
        .value("Drink", ActionType::Drink)
        .value("Rest", ActionType::Rest)
        .value("Wait", ActionType::Wait)
        .value("Custom", ActionType::Custom)
        .export_values();

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // === ID TYPES ===

    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==, py::arg("other"))
        .def("__ne__", &NeuronId::operator!=, py::arg("other"))
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return "NeuronId(" + std::to_string(id.value) + ")";
        });

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==, py::arg("other"))
        .def("__ne__", &SynapseId::operator!=, py::arg("other"))
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const SynapseId& id) {
            return "SynapseId(" + std::to_string(id.value) + ")";
        });

    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==, py::arg("other"))
        .def("__ne__", &RegionId::operator!=, py::arg("other"))
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const RegionId& id) {
            return "RegionId(" + std::to_string(id.value) + ")";
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==, py::arg("other"))
        .def("__ne__", &PopulationId::operator!=, py::arg("other"))
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); });

    // === CONFIG CLASS ===

    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        // Enhanced getters with type safety and defaults
        .def("get", [](Config& self, const std::string& key, const std::string& defaultValue) {
            return self.getOr<std::string>(key, defaultValue);
        }, py::arg("key"), py::arg("default_value"), "Get a string configuration value")
        .def("get", [](Config& self, const std::string& key, int64_t defaultValue) {
            return self.getOr<int64_t>(key, defaultValue);
        }, py::arg("key"), py::arg("default_value"), "Get an integer configuration value")
        .def("get", [](Config& self, const std::string& key, double defaultValue) {
            return self.getOr<double>(key, defaultValue);
        }, py::arg("key"), py::arg("default_value"), "Get a double configuration value")
        .def("get", [](Config& self, const std::string& key, bool defaultValue) {
            return self.getOr<bool>(key, defaultValue);
        }, py::arg("key"), py::arg("default_value"), "Get a boolean configuration value")
        .def("set", &Config::set, py::arg("key"), py::arg("value"), "Set a configuration value")
        .def("remove", &Config::remove, py::arg("key"), "Remove a configuration key")
        .def("getAll", [](Config& self) {
            auto keys = self.getKeys();
            py::dict result;
            for (const auto& key : keys) {
                // Try different types
                try {
                    auto str = self.getOr<std::string>(key, "");
                    result[key] = str;
                } catch (...) {
                    try {
                        auto intVal = self.getOr<int64_t>(key, 0);
                        result[key] = intVal;
                    } catch (...) {
                        try {
                            auto doubleVal = self.getOr<double>(key, 0.0);
                            result[key] = doubleVal;
                        } catch (...) {
                            auto boolVal = self.getOr<bool>(key, false);
                            result[key] = boolVal;
                        }
                    }
                }
            }
            return result;
        }, "Get all configuration values as Python dict");

    // === SENSORY INPUT CLASSES ===

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("__repr__", [](const SensoryInput& input) {
            return "<SensoryInput: " + std::to_string(input.getTimestamp()) + ">";
        });

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("getDataPtr", [](Vision& self) {
            return py::memoryview(py::buffer(self.getData()));
        }, "Get pointer to vision data as memoryview");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    // === ACTION CLASS ===

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType, "Get the action type")
        .def("setType", &Action::setType, py::arg("type"), "Set the action type")
        .def("getParameters", &Action::getParameters, "Get the action parameters")
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set the action parameters")
        .def("getName", &Action::getName, "Get the action name")
        .def("setParameter", [](Action& self, size_t index, float value) {
            auto params = self.getParameters();
            if (index < params.size()) {
                params[index] = value;
                self.setParameters(params);
            }
        }, py::arg("index"), py::arg("value"), "Set a specific parameter by index")
        .def("getParameter", [](const Action& self, size_t index) -> float {
            const auto& params = self.getParameters();
            if (index < params.size()) {
                return params[index];
            }
            throw InvalidParameterError("Parameter index out of range: " + std::to_string(index));
        }, py::arg("index"), "Get a specific parameter by index")
        .def("getParameterCount", [](const Action& self) -> size_t {
            return self.getParameters().size();
        }, "Get the number of parameters")
        .def("hasParameter", [](const Action& self, const std::string& name) -> bool {
            return self.getName().find(name) != std::string::npos;
        }, py::arg("name"), "Check if action has parameter by name")
        .def("clone", &Action::clone, "Clone the action")
        .def("isReady", [](const Action& self) -> bool {
            return static_cast<bool>(self.getType());
        }, "Check if action is ready for execution")
        .def("__repr__", [](const Action& action) {
            std::string paramsStr = "[";
            const auto& params = action.getParameters();
            for (size_t i = 0; i < params.size(); ++i) {
                if (i > 0) paramsStr += ", ";
                paramsStr += std::to_string(params[i]);
            }
            paramsStr += "]";
            return "<Action: " + action.getName() + paramsStr + ">";
        })
        .def("__str__", [](const Action& action) {
            return action.getName();
        });

    // === WORLD OBJECT CLASS ===

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f, "Create a world object with position and type")
        .def(py::init<float, float, WorldObjectType, float, float, float, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f, py::arg("health") = 1.0f, py::arg("velocityX") = 0.0f, py::arg("velocityY") = 0.0f,
             "Create a world object with position, type, and velocity")
        .def_readwrite("x", &WorldObject::x, "Object X coordinate")
        .def_readwrite("y", &WorldObject::y, "Object Y coordinate")
        .def_readwrite("radius", &WorldObject::radius, "Object radius")
        .def_readwrite("type", &WorldObject::type, "Object type")
        .def_readwrite("value", &WorldObject::value, "Object value (e.g., resource amount)")
        .def_readwrite("active", &WorldObject::active, "Object active state")
        .def("getVelocity", &WorldObject::getVelocity, "Get object velocity")
        .def("setVelocity", &WorldObject::setVelocity, py::arg("vx"), py::arg("vy"), "Set object velocity")
        .def("getHealth", &WorldObject::getHealth, "Get object health")
        .def("setHealth", &WorldObject::setHealth, py::arg("health"), "Set object health")
        .def("isDestroyed", &WorldObject::isDestroyed, "Check if object is destroyed")
        .def("getDistance", [](const WorldObject& obj, float x, float y) {
            float dx = obj.x - x;
            float dy = obj.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Get distance to position (x, y)")
        .def("__repr__", [](const WorldObject& obj) {
            std::string typeName = "Unknown";
            switch (obj.type) {
                case WorldObjectType::Empty: typeName = "Empty"; break;
                case WorldObjectType::Resource: typeName = "Resource"; break;
                case WorldObjectType::Hazard: typeName = "Hazard"; break;
                case WorldObjectType::Wall: typeName = "Wall"; break;
                case WorldObjectType::Marker: typeName = "Marker"; break;
            }
            return "<WorldObject: " + typeName + " at (" + std::to_string(obj.x) + ", " + std::to_string(obj.y) +
                   ") radius=" + std::to_string(obj.radius) + ">";
        })
        .def("__str__", [](const WorldObject& obj) {
            std::string typeName = "Unknown";
            switch (obj.type) {
                case WorldObjectType::Empty: typeName = "Empty"; break;
                case WorldObjectType::Resource: typeName = "Resource"; break;
                case WorldObjectType::Hazard: typeName = "Hazard"; break;
                case WorldObjectType::Wall: typeName = "Wall"; break;
                case WorldObjectType::Marker: typeName = "Marker"; break;
            }
            return typeName + "@(" + std::to_string(obj.x) + ", " + std::to_string(obj.y) + ")";
        });

    // === SIMPLE WORLD CLASS ===

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("initialize", &SimpleWorld::initialize, py::arg("width"), py::arg("height"),
             py::arg("visionWidth") = 16, py::arg("visionHeight") = 16,
             "Initialize the world with dimensions")
        .def("reset", &SimpleWorld::reset, "Reset the world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set the agent's starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"), "Update the world state")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand, py::arg("cmd"), py::arg("currentTime"),
             "Apply a motor command to the world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal, "Get current sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal, "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"), "Add an object to the world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position (x, y)")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position (x, y) is valid")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"), "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"), "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get current simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"), "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get random seed");