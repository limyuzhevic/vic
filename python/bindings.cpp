#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <chrono>
#include <cmath>
#include <unordered_map>
#include <algorithm>
#include <numeric>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/performance/MemoryPool.hpp"
#include "../src/performance/CheckpointSystem.hpp"
#include "../src/experiments/Metrics.hpp"

namespace py = pybind11;
namespace nlm {

// Advanced Pythonic exception classes
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& msg) : std::runtime_error(msg) {}
};

class NetworkAnalysisError : public NLMException {
public:
    explicit NetworkAnalysisError(const std::string& msg) : NLMException("Network Analysis Error: " + msg) {}
};

class PerformanceError : public NLMException {
public:
    explicit PerformanceError(const std::string& msg) : NLMException("Performance Error: " + msg) {}
};

class SerializationError : public NLMException {
public:
    explicit SerializationError(const std::string& msg) : NLMException("Serialization Error: " + msg) {}
};

class DevelopmentError : public NLMException {
public:
    explicit DevelopmentError(const std::string& msg) : NLMException("Development Error: " + msg) {}
};

// Additional system headers for advanced utilities
#include <thread>
#include <mutex>
#include <condition_variable>
#include <sstream>
#include <iomanip>
#include <random>
#include <map>
#include <set>
#include <filesystem>
#include <ctime>
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    // Register custom exceptions with Python
    py::register_exception<ConfigError>(m, "ConfigError", PyExc_RuntimeError);
    py::register_exception<BrainError>(m, "BrainError", PyExc_RuntimeError);
    py::register_exception<WorldError>(m, "WorldError", PyExc_RuntimeError);
    py::register_exception<SensoryError>(m, "SensoryError", PyExc_RuntimeError);
    py::register_exception<ActionError>(m, "ActionError", PyExc_RuntimeError);
    py::register_exception<ValidationError>(m, "ValidationError", PyExc_ValueError);
    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Helper function for null pointer checking
    auto check_null = [](void* ptr, const std::string& obj_name) {
        if (!ptr) {
            throw std::runtime_error(obj_name + " is null");
        }
        return ptr;
    };

    // Helper function for parameter validation
    auto validate_param = [](bool condition, const std::string& param_name, 
                           const std::string& message) {
        if (!condition) {
            throw ValidationError("Invalid parameter '" + param_name + "': " + message);
        }
    };

    // NeuronId class with error handling
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

    // SynapseId class with error handling
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

    // RegionId class with error handling
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

    // PopulationId class with error handling
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Enum classes
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

    // Config class with enhanced error handling
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", [](Config& self, const std::string& filepath) {
            try {
                validate_param(!filepath.empty(), "filepath", "filepath cannot be empty");
                if (!self.loadFromFile(filepath)) {
                    throw ConfigError("Failed to load configuration from file: " + filepath);
                }
            } catch (const std::exception& e) {
                throw ConfigError(std::string("Error loading config from file: ") + e.what());
            }
        }, py::arg("filepath"),
           "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            try {
                validate_param(argc > 1, "argc", "argc must be greater than 1");
                if (!self.loadFromArgs(argc, argv)) {
                    throw ConfigError("Failed to load configuration from arguments");
                }
            } catch (const std::exception& e) {
                throw ConfigError(std::string("Error loading config from arguments: ") + e.what());
            }
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", [](Config& self, const std::string& filepath) {
            try {
                validate_param(!filepath.empty(), "filepath", "filepath cannot be empty");
                if (!self.saveToFile(filepath)) {
                    throw ConfigError("Failed to save configuration to file: " + filepath);
                }
            } catch (const std::exception& e) {
                throw ConfigError(std::string("Error saving config to file: ") + e.what());
            }
        }, py::arg("filepath"),
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
        });

    // SensoryInput class with error handling
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Vision class with error handling
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            try {
                validate_param(!data.empty(), "data", "data cannot be empty");
                self.setData(data);
            } catch (const std::exception& e) {
                throw SensoryError(std::string("Error setting vision data: ") + e.what());
            }
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    // Audio class with error handling
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            try {
                validate_param(!data.empty(), "data", "data cannot be empty");
                self.setData(data);
            } catch (const std::exception& e) {
                throw SensoryError(std::string("Error setting audio data: ") + e.what());
            }
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    // InternalSignals class with error handling
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    // Action class with error handling
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", [](Action& self, const std::vector<float>& params) {
            try {
                self.setParameters(params);
            } catch (const std::exception& e) {
                throw ActionError(std::string("Error setting action parameters: ") + e.what());
            }
        }, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", [](const Action& self) {
            try {
                auto result = self.clone();
                validate_param(result != nullptr, "clone", "Action clone returned null pointer");
                return result;
            } catch (const std::exception& e) {
                throw ActionError(std::string("Error cloning action: ") + e.what());
            }
        });

    // WorldObject class with error handling
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

    // AgentBody class with error handling
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

    // ActionResult class with error handling
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // SensoryPercept class with error handling
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("getVision", [](const SensoryPercept& self) {
            try {
                auto vision = self.getVision();
                validate_param(vision != nullptr, "vision", "Vision pointer is null");
                return vision;
            } catch (const std::exception& e) {
                throw SensoryError(std::string("Error getting vision from percept: ") + e.what());
            }
        })
        .def("setVision", [](SensoryPercept& self, std::shared_ptr<Vision> vision) {
            try {
                validate_param(vision != nullptr, "vision", "Vision cannot be null");
                self.setVision(vision);
            } catch (const std::exception& e) {
                throw SensoryError(std::string("Error setting vision in percept: ") + e.what());
            }
        }, py::arg("vision"))
        .def("getVisionWidth", [](const SensoryPercept& self) {
            try {
                auto vision = self.getVision();
                if (vision == nullptr) return 0u;
                return vision->getWidth();
            } catch (const std::exception& e) {
                throw SensoryError(std::string("Error getting vision width: ") + e.what());
            }
        })
        .def("getVisionHeight", [](const SensoryPercept& self) {
            try {
                auto vision = self.getVision();
                if (vision == nullptr) return 0u;
                return vision->getHeight();
            } catch (const std::exception& e) {
                throw SensoryError(std::string("Error getting vision height: ") + e.what());
            }
        })
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

    // SimpleWorld class with error handling
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"))
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &SimpleWorld::update, py::arg("timestep"))
        .def("applyMotorCommand", [](SimpleWorld& self, const MotorCommand& cmd, double currentTime) {
            try {
                validate_param(cmd != MotorCommand::Wait, "cmd", "Motor command cannot be Wait");
                self.applyMotorCommand(cmd, currentTime);
            } catch (const std::exception& e) {
                throw WorldError(std::string("Error applying motor command: ") + e.what());
            }
        }, py::arg("cmd"), py::arg("currentTime"))
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"))
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"))
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"))
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    // Brain class with error handling
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", [](Brain& self) {
            try {
                self.initialize();
            } catch (const std::exception& e) {
                throw BrainError(std::string("Error initializing brain: ") + e.what());
            }
        }, "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", [](Brain& self, std::shared_ptr<SensoryPercept> input) {
            try {
                validate_param(input != nullptr, "input", "SensoryPercept cannot be null");
                self.receiveSensoryInput(input);
            } catch (const std::exception& e) {
                throw BrainError(std::string("Error receiving sensory input: ") + e.what());
            }
        }, py::arg("input"),
           "Inject sensory input into the brain")
        .def("injectCurrent", [](Brain& self, const NeuronId& neuron, float current) {
            try {
                self.injectCurrent(neuron, current);
            } catch (const std::exception& e) {
                throw BrainError(std::string("Error injecting current: ") + e.what());
            }
        }, py::arg("neuron"), py::arg("current"),
           "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", [](Brain& self, NeuronType type, float current) {
            try {
                self.injectCurrentToNeurons(type, current);
            } catch (const std::exception& e) {
                throw BrainError(std::string("Error injecting current to neurons: ") + e.what());
            }
        }, py::arg("type"), py::arg("current"),
           "Inject current into all neurons of a specific type")
        .def("produceAction", [](Brain& self) {
            try {
                auto action = self.produceAction();
                validate_param(action != nullptr, "action", "Action produced is null");
                return action;
            } catch (const std::exception& e) {
                throw BrainError(std::string("Error producing action: ") + e.what());
            }
        }, "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", [](Brain& self, const std::string& filepath) {
            try {
                validate_param(!filepath.empty(), "filepath", "filepath cannot be empty");
                self.save(filepath);
            } catch (const std::exception& e) {
                throw BrainError(std::string("Error saving brain state: ") + e.what());
            }
        }, py::arg("filepath"),
           "Save brain state to file")
        .def("load", [](Brain& self, const std::string& filepath) {
            try {
                validate_param(!filepath.empty(), "filepath", "filepath cannot be empty");
                self.load(filepath);
            } catch (const std::exception& e) {
                throw BrainError(std::string("Error loading brain state: ") + e.what());
            }
        }, py::arg("filepath"),
           "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", [](Brain& self, const RegionId& id) {
            try {
                auto region = self.getRegion(id);
                validate_param(region != nullptr, "region", "Region not found");
                return region;
            } catch (const std::exception& e) {
                throw BrainError(std::string("Error getting region: ") + e.what());
            }
        }, py::arg("id"),
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
             "Log brain status");

    // AgentBrain class with error handling
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", [](AgentBrain& self, std::shared_ptr<SimpleWorld> world) {
            try {
                validate_param(world != nullptr, "world", "World cannot be null");
                self.initialize(world);
            } catch (const std::exception& e) {
                throw BrainError(std::string("Error initializing agent brain: ") + e.what());
            }
        }, py::arg("world"),
           "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", [](AgentBrain& self, std::shared_ptr<SensoryPercept> percept) {
            try {
                validate_param(percept != nullptr, "percept", "SensoryPercept cannot be null");
                self.processSensoryInput(percept);
            } catch (const std::exception& e) {
                throw BrainError(std::string("Error processing sensory input: ") + e.what());
            }
        }, py::arg("percept"),
           "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", [](AgentBrain& self, std::vector<float>& motorActivity) {
            try {
                return self.decodeMotorCommand(motorActivity);
            } catch (const std::exception& e) {
                throw BrainError(std::string("Error decoding motor command: ") + e.what());
            }
        }, py::arg("motorActivity"),
           "Decode brain motor activity into motor command")
        .def("applyRewardModulation", [](AgentBrain& self, float reward, float predictedReward) {
            try {
                self.applyRewardModulation(reward, predictedReward);
            } catch (const std::exception& e) {
                throw BrainError(std::string("Error applying reward modulation: ") + e.what());
            }
        }, py::arg("reward"), py::arg("predictedReward"),
           "Apply reward-based neuromodulation")
        .def("updateDevelopment", [](AgentBrain& self, double timestep) {
            try {
                self.updateDevelopment(timestep);
            } catch (const std::exception& e) {
                throw BrainError(std::string("Error updating development: ") + e.what());
            }
        }, py::arg("timestep"),
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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

    PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Advanced Python Bindings
        ------------------------------------------------------
        Enhanced Python binding for the NLM neural simulation framework with advanced expert features:
        
        1. Network Analysis Functions
        2. Configuration Utilities
        3. Simulation Analysis Tools
        4. Performance Monitoring Utilities
        5. Pythonic Convenience Methods
        6. Serialization Utilities
        7. Advanced Analysis Methods
    )pbdoc";

    // Register custom exceptions with Python
    py::register_exception<ConfigError>(m, "ConfigError", PyExc_RuntimeError);
    py::register_exception<BrainError>(m, "BrainError", PyExc_RuntimeError);
    py::register_exception<WorldError>(m, "WorldError", PyExc_RuntimeError);
    py::register_exception<SensoryError>(m, "SensoryError", PyExc_RuntimeError);
    py::register_exception<ActionError>(m, "ActionError", PyExc_RuntimeError);
    py::register_exception<ValidationError>(m, "ValidationError", PyExc_ValueError);
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    // Register advanced exception classes
    py::register_exception<NLMException>(m, "NLMException", PyExc_RuntimeError);
    py::register_exception<NetworkAnalysisError>(m, "NetworkAnalysisError", PyExc_RuntimeError);
    py::register_exception<PerformanceError>(m, "PerformanceError", PyExc_RuntimeError);
    py::register_exception<SerializationError>(m, "SerializationError", PyExc_RuntimeError);
    py::register_exception<DevelopmentError>(m, "DevelopmentError", PyExc_RuntimeError);

    // Helper function for null pointer checking
    auto check_null = [](void* ptr, const std::string& obj_name) {
        if (!ptr) {
            throw std::runtime_error(obj_name + " is null");
        }
        return ptr;
    };

    // Helper function for parameter validation
    auto validate_param = [](bool condition, const std::string& param_name, 
                           const std::string& message) {
        if (!condition) {
            throw ValidationError("Invalid parameter '" + param_name + "': " + message);
        }
    };

    // NeuronId class with error handling
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

    // SynapseId class with error handling
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

    // RegionId class with error handling
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

    // PopulationId class with error handling
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Enum classes
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

    // Config class with enhanced error handling
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", [](Config& self, const std::string& filepath) {
            try {
                validate_param(!filepath.empty(), "filepath", "filepath cannot be empty");
                if (!self.loadFromFile(filepath)) {
                    throw ConfigError("Failed to load configuration from file: " + filepath);
                }
            } catch (const std::exception& e) {
                throw ConfigError(std::string("Error loading config from file: ") + e.what());
            }
        }, py::arg("filepath"),
           "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            try {
                validate_param(argc > 1, "argc", "argc must be greater than 1");
                if (!self.loadFromArgs(argc, argv)) {
                    throw ConfigError("Failed to load configuration from arguments");
                }
            } catch (const std::exception& e) {
                throw ConfigError(std::string("Error loading config from arguments: ") + e.what());
            }
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", [](Config& self, const std::string& filepath) {
            try {
                validate_param(!filepath.empty(), "filepath", "filepath cannot be empty");
                if (!self.saveToFile(filepath)) {
                    throw ConfigError("Failed to save configuration to file: " + filepath);
                }
            } catch (const std::exception& e) {
                throw ConfigError(std::string("Error saving config to file: ") + e.what());
            }
        }, py::arg("filepath"),
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
        });

    // SensoryInput class with error handling
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Vision class with error handling
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            try {
                validate_param(!data.empty(), "data", "data cannot be empty");
                self.setData(data);
            } catch (const std::exception& e) {
                throw SensoryError(std::string("Error setting vision data: ") + e.what());
            }
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    // Audio class with error handling
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            try {
                validate_param(!data.empty(), "data", "data cannot be empty");
                self.setData(data);
            } catch (const std::exception& e) {
                throw SensoryError(std::string("Error setting audio data: ") + e.what());
            }
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    // InternalSignals class with error handling
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    // Action class with error handling
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", [](Action& self, const std::vector<float>& params) {
            try {
                self.setParameters(params);
            } catch (const std::exception& e) {
                throw ActionError(std::string("Error setting action parameters: ") + e.what());
            }
        }, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", [](const Action& self) {
            try {
                auto result = self.clone();
                validate_param(result != nullptr, "clone", "Action clone returned null pointer");
                return result;
            } catch (const std::exception& e) {
                throw ActionError(std::string("Error cloning action: ") + e.what());
            }
        });

    // WorldObject class with error handling
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

    // AgentBody class with error handling
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

    // ActionResult class with error handling
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // SensoryPercept class with error handling
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("getVision", [](const SensoryPercept& self) {
            try {
                auto vision = self.getVision();
                validate_param(vision != nullptr, "vision", "Vision pointer is null");
                return vision;
            } catch (const std::exception& e) {
                throw SensoryError(std::string("Error getting vision from percept: ") + e.what());
            }
        })
        .def("setVision", [](SensoryPercept& self, std::shared_ptr<Vision> vision) {
            try {
                validate_param(vision != nullptr, "vision", "Vision cannot be null");
                self.setVision(vision);
            } catch (const std::exception& e) {
                throw SensoryError(std::string("Error setting vision in percept: ") + e.what());
            }
        }, py::arg("vision"))
        .def("getVisionWidth", [](const SensoryPercept& self) {
            try {
                auto vision = self.getVision();
                if (vision == nullptr) return 0u;
                return vision->getWidth();
            } catch (const std::exception& e) {
                throw SensoryError(std::string("Error getting vision width: ") + e.what());
            }
        })
        .def("getVisionHeight", [](const SensoryPercept& self) {
            try {
                auto vision = self.getVision();
                if (vision == nullptr) return 0u;
                return vision->getHeight();
            } catch (const std::exception& e) {
                throw SensoryError(std::string("Error getting vision height: ") + e.what());
            }
        })
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

    // SimpleWorld class with error handling
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"))
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &SimpleWorld::update, py::arg("timestep"))
        .def("applyMotorCommand", [](SimpleWorld& self, const MotorCommand& cmd, double currentTime) {
            try {
                validate_param(cmd != MotorCommand::Wait, "cmd", "Motor command cannot be Wait");
                self.applyMotorCommand(cmd, currentTime);
            } catch (const std::exception& e) {
                throw WorldError(std::string("Error applying motor command: ") + e.what());
            }
        }, py::arg("cmd"), py::arg("currentTime"))
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"))
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"))
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"))
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    // Brain class with enhanced error handling
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", [](Brain& self) {
            try {
                self.initialize();
            } catch (const std::exception& e) {
                throw BrainError(std::string("Error initializing brain: ") + e.what());
            }
        }, "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", [](Brain& self, std::shared_ptr<SensoryPercept> input) {
            try {
                validate_param(input != nullptr, "input", "SensoryPercept cannot be null");
                self.receiveSensoryInput(input);
            } catch (const std::exception& e) {
                throw BrainError(std::string("Error receiving sensory input: ") + e.what());
            }
        }, py::arg("input"),
           "Inject sensory input into the brain")
        .def("injectCurrent", [](Brain& self, const NeuronId& neuron, float current) {
            try {
                self.injectCurrent(neuron, current);
            } catch (const std::exception& e) {
                throw BrainError(std::string("Error injecting current: ") + e.what());
            }
        }, py::arg("neuron"), py::arg("current"),
           "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", [](Brain& self, NeuronType type, float current) {
            try {
                self.injectCurrentToNeurons(type, current);
            } catch (const std::exception& e) {
                throw BrainError(std::string("Error injecting current to neurons: ") + e.what());
            }
        }, py::arg("type"), py::arg("current"),
           "Inject current into all neurons of a specific type")
        .def("produceAction", [](Brain& self) {
            try {
                auto action = self.produceAction();
                validate_param(action != nullptr, "action", "Action produced is null");
                return action;
            } catch (const std::exception& e) {
                throw BrainError(std::string("Error producing action: ") + e.what());
            }
        }, "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", [](Brain& self, const std::string& filepath) {
            try {
                validate_param(!filepath.empty(), "filepath", "filepath cannot be empty");
                self.save(filepath);
            } catch (const std::exception& e) {
                throw BrainError(std::string("Error saving brain state: ") + e.what());
            }
        }, py::arg("filepath"),
           "Save brain state to file")
        .def("load", [](Brain& self, const std::string& filepath) {
            try {
                validate_param(!filepath.empty(), "filepath", "filepath cannot be empty");
                self.load(filepath);
            } catch (const std::exception& e) {
                throw BrainError(std::string("Error loading brain state: ") + e.what());
            }
        }, py::arg("filepath"),
           "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", [](Brain& self, const RegionId& id) {
            try {
                auto region = self.getRegion(id);
                validate_param(region != nullptr, "region", "Region not found");
                return region;
            } catch (const std::exception& e) {
                throw BrainError(std::string("Error getting region: ") + e.what());
            }
        }, py::arg("id"),
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
             "Log brain status");

    // AgentBrain class with enhanced error handling
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", [](AgentBrain& self, std::shared_ptr<SimpleWorld> world) {
            try {
                validate_param(world != nullptr, "world", "World cannot be null");
                self.initialize(world);
            } catch (const std::exception& e) {
                throw BrainError(std::string("Error initializing agent brain: ") + e.what());
            }
        }, py::arg("world"),
           "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", [](AgentBrain& self, std::shared_ptr<SensoryPercept> percept) {
            try {
                validate_param(percept != nullptr, "percept", "SensoryPercept cannot be null");
                self.processSensoryInput( percept);
            } catch (const std::exception& e) {
                throw BrainError(std::string("Error processing sensory input: ") + e.what());
            }
        }, py::arg("percept"),
           "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", [](AgentBrain& self, std::vector<float>& motorActivity) {
            try {
                return self.decodeMotorCommand(motorActivity);
            } catch (const std::exception& e) {
                throw BrainError(std::string("Error decoding motor command: ") + e.what());
            }
        }, py::arg("motorActivity"),
           "Decode brain motor activity into motor command")
        .def("applyRewardModulation", [](AgentBrain& self, float reward, float predictedReward) {
            try {
                self.applyRewardModulation(reward, predictedReward);
            } catch (const std::exception& e) {
                throw BrainError(std::string("Error applying reward modulation: ") + e.what());
            }
        }, py::arg("reward"), py::arg("predictedReward"),
           "Apply reward-based neuromodulation")
        .def("updateDevelopment", [](AgentBrain& self, double timestep) {
            try {
                self.updateDevelopment(timestep);
            } catch (const std::exception& e) {
                throw BrainError(std::string("Error updating development: ") + e.what());
            }
        }, py::arg("timestep"),
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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

    // Factory functions with error handling
    m.def("createDefaultConfig", []() {
        try {
            auto config = std::make_shared<Config>();
            validate_param(config != nullptr, "config", "Failed to create default config");
            return config;
        } catch (const std::exception& e) {
            throw ConfigError(std::string("Error creating default config: ") + e.what());
        }
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) {
        try {
            validate_param(config != nullptr, "config", "Config cannot be null");
            auto brain = std::make_shared<Brain>(config);
            validate_param(brain != nullptr, "brain", "Failed to create brain");
            return brain;
        } catch (const std::exception& e) {
            throw BrainError(std::string("Error creating brain: ") + e.what());
        }
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() {
        try {
            auto world = std::make_shared<SimpleWorld>();
            validate_param(world != nullptr, "world", "Failed to create world");
            return world;
        } catch (const std::exception& e) {
            throw WorldError(std::string("Error creating world: ") + e.what());
        }
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) {
        try {
            validate_param(brain != nullptr, "brain", "Brain cannot be null");
            auto agentBrain = std::make_shared<AgentBrain>(brain);
            validate_param(agentBrain != nullptr, "agentBrain", "Failed to create agent brain");
            return agentBrain;
        } catch (const std::exception& e) {
            throw BrainError(std::string("Error creating agent brain: ") + e.what());
        }
    }, py::arg("brain"), "Create a new agent brain interface");

    // Constants with null checks
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
