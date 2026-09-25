#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <limits>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

// Custom exceptions for better Python error handling
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigError : public NLMError {
public:
    explicit ConfigError(const std::string& msg) : NLMError("Configuration error: " + msg) {}
};

class BrainError : public NLMError {
public:
    explicit BrainError(const std::string& msg) : NLMError("Brain error: " + msg) {}
};

class WorldError : public NLMError {
public:
    explicit WorldError(const std::string& msg) : NLMError("World error: " + msg) {}
};

// Helper functions for parameter validation
inline void validatePositive(const std::string& name, double value, double max = std::numeric_limits<double>::max()) {
    if (value <= 0.0) {
        throw NLMError(name + " must be positive, got " + std::to_string(value));
    }
    if (max != std::numeric_limits<double>::max() && value > max) {
        throw NLMError(name + " must be <= " + std::to_string(max) + ", got " + std::to_string(value));
    }
}

inline void validateRange(const std::string& name, double value, double min, double max) {
    if (value < min || value > max) {
        throw NLMError(name + " must be between " + std::to_string(min) + " and " + std::to_string(max) + ", got " + std::to_string(value));
    }
}

inline void validateNonNegative(const std::string& name, size_t value) {
    if (value == 0) {
        throw NLMError(name + " cannot be zero");
    }
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Key features:
        - Robust error handling with descriptive Python exceptions
        - Parameter validation and safety checks
        - Pythonic API with convenience methods
        - Context manager support for resource management
        - Enhanced debugging and monitoring capabilities
    )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMError>(m, "NLMError", PyExc_RuntimeError.ptr());
    py::register_exception<ConfigError>(m, "ConfigError", PyExc_ValueError.ptr());
    py::register_exception<BrainError>(m, "BrainError", PyExc_RuntimeError.ptr());
    py::register_exception<WorldError>(m, "WorldError", PyExc_RuntimeError.ptr());

    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        });

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });

    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

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
        // Enhanced configuration methods with validation
        .def("set_safe", [](Config& self, const std::string& key, const std::string& value) {
            if (value.empty()) {
                throw ConfigError("Value cannot be empty for key: " + key);
            }
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value with validation")
        .def("set_int", [](Config& self, const std::string& key, int64_t value) {
            validateRange(key, static_cast<double>(value), -std::numeric_limits<int64_t>::max(),
                         std::numeric_limits<int64_t>::max());
            self.set(key, std::to_string(value));
        }, py::arg("key"), py::arg("value"), "Set integer configuration value")
        .def("set_float", [](Config& self, const std::string& key, double value) {
            validateRange(key, value, -std::numeric_limits<double>::max(),
                         std::numeric_limits<double>::max());
            self.set(key, std::to_string(value));
        }, py::arg("key"), py::arg("value"), "Set float configuration value")
        .def("get", [](const Config& self, const std::string& key, const std::string& defaultValue = "") {
            if (self.has(key)) {
                return self.get(key);
            }
            return defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = "", "Get configuration value with default")
        .def("get_int", [](const Config& self, const std::string& key, int64_t defaultValue = 0) {
            if (self.has(key)) {
                try {
                    return std::stoll(self.get(key));
                } catch (const std::exception& e) {
                    throw ConfigError("Invalid integer value for key '" + key + "': " + e.what());
                }
            }
            return defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = 0, "Get integer configuration value")
        .def("get_float", [](const Config& self, const std::string& key, double defaultValue = 0.0) {
            if (self.has(key)) {
                try {
                    return std::stod(self.get(key));
                } catch (const std::exception& e) {
                    throw ConfigError("Invalid float value for key '" + key + "': " + e.what());
                }
            }
            return defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = 0.0, "Get float configuration value")
        .def("validate", [](const Config& self) {
            // Basic validation - check for required keys and reasonable ranges
            std::vector<std::string> errors;
            
            if (self.has("brain.neuron_count")) {
                try {
                    int neuronCount = std::stoi(self.get("brain.neuron_count"));
                    validateNonNegative("brain.neuron_count", static_cast<size_t>(neuronCount));
                    if (neuronCount > 1000000) { // Reasonable upper limit
                        errors.push_back("brain.neuron_count is too large (> 1,000,000)");
                    }
                } catch (const std::exception& e) {
                    errors.push_back("Invalid brain.neuron_count: " + std::string(e.what()));
                }
            }
            
            if (!errors.empty()) {
                throw ConfigError("Configuration validation failed:\n" + 
                                 std::accumulate(std::next(errors.begin()), errors.end(), errors[0],
                                               std::string(), [](const std::string& a, const std::string& b) {
                                                   return a + "\n" + b;
                                               }));
            }
            
            return true;
        }, "Validate configuration for common issues");

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

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

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active);

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x)
        .def_readwrite("y", &AgentBody::y)
        .def_readwrite("orientation", &AgentBody::orientation)
        .def_readwrite("velocityX", &AgentBody::velocityX)
        .def_readwrite("velocityY", &AgentBody::velocityY)
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity)
        .def_readwrite("energy", &AgentBody::energy)
        .def_readwrite("health", &AgentBody::health)
        .def_readwrite("age", &AgentBody::age)
        .def_readwrite("isMoving", &AgentBody::isMoving)
        .def_readwrite("isTurning", &AgentBody::isTurning)
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime)
        .def("reset", &AgentBody::reset);

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"))
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"))
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"))
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"))
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"))
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"));

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions with validation")
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world by timestep seconds")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept from world")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add world object")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum agent energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducible simulations")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             "Get current random seed")
        // Enhanced world methods with validation
        .def("run_episode", [](SimpleWorld& self, size_t numSteps) {
            validateNonNegative("numSteps", numSteps);
            for (size_t step = 0; step < numSteps; ++step) {
                self.update(0.1);
            }
        }, py::arg("numSteps"),
           "Run world simulation for specified number of steps")
        .def("get_world_state", [](const SimpleWorld& self) {
            py::dict state;
            state["width"] = self.getWidth();
            state["height"] = self.getHeight();
            state["max_energy"] = self.getMaxEnergy();
            state["energy_decay_rate"] = self.getEnergyDecayRate();
            state["simulation_time"] = self.getSimulationTime();
            state["random_seed"] = self.getRandomSeed();
            
            auto body = self.getAgentBody();
            py::dict agentBody;
            agentBody["x"] = body.x;
            agentBody["y"] = body.y;
            agentBody["orientation"] = body.orientation;
            agentBody["velocity_x"] = body.velocityX;
            agentBody["velocity_y"] = body.velocityY;
            agentBody["energy"] = body.energy;
            agentBody["health"] = body.health;
            agentBody["age"] = body.age;
            state["agent_body"] = agentBody;
            
            return state;
        }, "Get comprehensive world state as dictionary")
        .def("validate_configuration", [](const SimpleWorld& self) {
            if (self.getWidth() <= 0 || self.getHeight() <= 0) {
                throw WorldError("World dimensions must be positive");
            }
            if (self.getEnergyDecayRate() < 0.0 || self.getEnergyDecayRate() > 1.0) {
                throw WorldError("Energy decay rate must be between 0.0 and 1.0");
            }
            if (self.getMaxEnergy() <= 0.0) {
                throw WorldError("Maximum energy must be positive");
            }
            return true;
        }, "Validate world configuration for consistency");

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", [](Brain& self, const std::string& type, double current) {
            NeuronType neuronType;
            if (type == "excitatory") neuronType = NeuronType::Excitatory;
            else if (type == "inhibitory") neuronType = NeuronType::Inhibitory;
            else if (type == "sensory") neuronType = NeuronType::Sensory;
            else if (type == "motor") neuronType = NeuronType::Motor;
            else if (type == "modulatory") neuronType = NeuronType::Modulatory;
            else if (type == "internal") neuronType = NeuronType::Internal;
            else throw BrainError("Unknown neuron type: " + type);
            
            validatePositive("current", current);
            self.injectCurrentToNeurons(neuronType, current);
        }, py::arg("type"), py::arg("current"),
           "Inject current into all neurons of a specific type (string version)")
        .def("injectCurrentToNeurons_enum", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type (enum version)")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        // Enhanced brain methods with validation and convenience features
        .def("run_for_steps", [](Brain& self, size_t numSteps) {
            validateNonNegative("numSteps", numSteps);
            for (size_t i = 0; i < numSteps; ++i) {
                self.step(i);
            }
        }, py::arg("numSteps"),
           "Run brain simulation for a specific number of steps")
        .def("get_statistics", [](const Brain& self) {
            py::dict stats;
            stats["total_neurons"] = self.getTotalNeuronCount();
            stats["total_synapses"] = self.getTotalSynapseCount();
            stats["firing_neurons"] = self.getFiringNeuronCount();
            stats["active_neurons"] = self.getActiveNeuronCount();
            stats["average_firing_rate"] = self.getAverageFiringRate();
            stats["excitation_inhibition_ratio"] = self.getExcitationInhibitionRatio();
            stats["total_spikes"] = self.getTotalSpikeCount();
            stats["developmental_stage"] = self.getDevelopmentalStage();
            return stats;
        }, "Get comprehensive brain statistics as dictionary")
        .def("validate", [](Brain& self) {
            // Validate brain state
            if (self.getTotalNeuronCount() == 0) {
                throw BrainError("Brain has no neurons - initialization may have failed");
            }
            if (self.getTotalSynapseCount() > self.getTotalNeuronCount() * 100) { // Reasonable upper limit
                throw BrainError("Brain has unusually high synapse-to-neuron ratio");
            }
            return true;
        }, "Validate brain state for consistency");

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"))
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        // Enhanced agent brain methods with validation
        .def("get_agent_status", [](const AgentBrain& self) {
            py::dict status;
            status["curiosity_level"] = self.getCuriosityLevel();
            status["novelty_level"] = self.getNoveltyLevel();
            status["neuromodulation_level"] = self.getNeuromodulationLevel();
            status["prediction_error"] = self.getPredictionError();
            status["developmental_stage"] = self.getDevelopmentalStage();
            status["reward_mod_enabled"] = self.isRewardModulationEnabled();
            status["structural_plasticity_enabled"] = self.isStructuralPlasticityEnabled();
            status["development_enabled"] = self.isDevelopmentEnabled();
            status["curiosity_enabled"] = self.isCuriosityEnabled();
            return status;
        }, "Get comprehensive agent brain status as dictionary")
        .def("run_episode", [](AgentBrain& self, SimpleWorld& world, size_t numSteps) {
            validateNonNegative("numSteps", numSteps);
            for (size_t step = 0; step < numSteps; ++step) {
                world.update(0.1);
                auto percept = world.getSensoryPercept();
                self.processSensoryInput(percept);
                self.getBrain()->step(step);
            }
        }, py::arg("world"), py::arg("numSteps"),
           "Run a complete episode with world updates")
        .def("learn_from_episode", [](AgentBrain& self, SimpleWorld& world, size_t numSteps,
                                      float reward, float predictedReward = 0.0f) {
            validateNonNegative("numSteps", numSteps);
            validatePositive("reward", reward);
            validatePositive("predictedReward", predictedReward);
            
            for (size_t step = 0; step < numSteps; ++step) {
                world.update(0.1);
                auto percept = world.getSensoryPercept();
                self.processSensoryInput(percept);
                self.getBrain()->step(step);
                self.applyRewardModulation(reward, predictedReward);
                self.updateDevelopment(0.1);
            }
        }, py::arg("world"), py::arg("numSteps"), py::arg("reward"), py::arg("predictedReward") = 0.0f,
           "Run an episode with learning from rewards");

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration")
    .def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration")
    .def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world")
    .def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface")
    // Enhanced factory functions and utilities
    .def("create_standard_config", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", "1000");
        config->set("brain.synapse_density", "0.1");
        config->set("plasticity.stdp.enable", "true");
        config->set("plasticity.hebbian.enable", "true");
        config->set("neuromod.dopamine.scale", "1.0");
        config->set("neuromod.curiosity.enable", "true");
        config->set("neuromod.novelty.enable", "true");
        return config;
    }, "Create a standard configuration with typical values")
    .def("run_complete_simulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                        std::shared_ptr<AgentBrain> agent, size_t numSteps) {
        validateNonNegative("numSteps", numSteps);
        
        for (size_t step = 0; step < numSteps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Optional reward modulation
            if (agent->isRewardModulationEnabled()) {
                float reward = 0.0f; // Could extract from world perception
                agent->applyRewardModulation(reward, 0.0f);
            }
            
            // Update development
            if (agent->isDevelopmentEnabled()) {
                agent->updateDevelopment(0.1);
            }
        }
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("numSteps"),
       "Run a complete simulation with brain, world, and agent integration")
    .def("benchmark_performance", [](std::shared_ptr<Brain> brain, size_t numSteps) {
        validateNonNegative("numSteps", numSteps);
        
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t step = 0; step < numSteps; ++step) {
            brain->step(step);
        }
        auto end = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        double stepsPerSecond = (numSteps / (duration.count() / 1000.0));
        
        py::dict results;
        results["steps_completed"] = static_cast<size_t>(numSteps);
        results["total_time_ms"] = static_cast<double>(duration.count());
        results["steps_per_second"] = stepsPerSecond;
        results["average_time_per_step_ms"] = (duration.count() / static_cast<double>(numSteps));
        
        return results;
    }, py::arg("brain"), py::arg("numSteps"),
       "Run performance benchmark and return timing statistics")
    .def("save_brain_state", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        brain->save(filepath);
        return true;
    }, py::arg("brain"), py::arg("filepath"),
       "Save brain state to file and return success status")
    .def("load_brain_state", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        brain->load(filepath);
        return true;
    }, py::arg("brain"), py::arg("filepath"),
       "Load brain state from file and return success status");

    // Module attributes and version info
    m.attr("__version__") = "0.1.0";
    m.attr("__author__") = "NLM Authors";
    m.attr("__description__") = "NLM (Neural Learning Machine) Python Bindings";
    
    // Invalid IDs for reference
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Error types for user convenience
    py::class_<NLMError>(m, "NLMError", py::base<std::runtime_error>(),
        "Base exception for all NLM errors")
        .def(py::init<const std::string&>());
    
    py::class_<ConfigError>(m, "ConfigError", py::base<NLMError>(),
        "Configuration-related errors")
        .def(py::init<const std::string&>());
    
    py::class_<BrainError>(m, "BrainError", py::base<NLMError>(),
        "Brain-related errors")
        .def(py::init<const std::string&>());
    
    py::class_<WorldError>(m, "WorldError", py::base<NLMError>(),
        "World-related errors")
        .def(py::init<const std::string&>());
}

} // namespace nlm
