#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>

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

// Custom exception classes for Python bindings
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigurationError : public NLMException {
public:
    explicit ConfigurationError(const std::string& msg) : NLMException("Configuration Error: " + msg) {}
};

class InitializationError : public NLMException {
public:
    explicit InitializationError(const std::string& msg) : NLMException("Initialization Error: " + msg) {}
};

// Context manager wrapper for Brain to ensure proper resource management
class BrainContext {
private:
    std::shared_ptr<Brain> brain;
    bool initialized;

public:
    explicit BrainContext(std::shared_ptr<Brain> b) : brain(b), initialized(false) {
        if (!brain) {
            throw InitializationError("Cannot create context with null brain pointer");
        }
    }

    BrainContext(const BrainContext&) = delete;
    BrainContext& operator=(const BrainContext&) = delete;

    BrainContext(BrainContext&& other) noexcept : brain(std::move(other.brain)), initialized(other.initialized) {
        other.initialized = false;
    }

    ~BrainContext() {
        if (initialized) {
            try {
                brain->reset();
            } catch (const std::exception& e) {
                // Log error but don't propagate in destructor
                std::cerr << "Warning: Exception during brain cleanup: " << e.what() << std::endl;
            }
        }
    }

    BrainContext& operator=(BrainContext&& other) noexcept {
        if (this != &other) {
            if (initialized) {
                brain->reset();
            }
            brain = std::move(other.brain);
            initialized = other.initialized;
            other.initialized = false;
        }
        return *this;
    }

    void __enter__() {
        if (!brain->initialize()) {
            throw InitializationError("Failed to initialize brain");
        }
        initialized = true;
    }

    py::object __exit__(py::args args, py::kwargs kwargs) {
        if (initialized) {
            try {
                brain->reset();
                initialized = false;
            } catch (const std::exception& e) {
                // Re-raise as Python exception
                py::raise<NLMException>(e.what());
            }
        }
        return py::none();
    }

    std::shared_ptr<Brain> getBrain() const {
        return brain;
    }

    bool isInitialized() const {
        return initialized;
    }

    // Forward to brain methods for convenience
    void step(SimulationStep step) {
        if (!initialized) {
            throw InitializationError("Brain is not initialized. Use 'with' statement or call initialize().");
        }
        brain->step(step);
    }

    void step(SimulationStep step, Timestamp time) {
        if (!initialized) {
            throw InitializationError("Brain is not initialized. Use 'with' statement or call initialize().");
        }
        brain->step(step, time);
    }

    void reset() {
        if (initialized) {
            brain->reset();
            initialized = false;
        }
    }

    size_t getTotalNeuronCount() const {
        return brain->getTotalNeuronCount();
    }

    size_t getTotalSynapseCount() const {
        return brain->getTotalSynapseCount();
    }

    size_t getFiringNeuronCount() const {
        return brain->getFiringNeuronCount();
    }

    float getAverageFiringRate() const {
        return brain->getAverageFiringRate();
    }

    double getDevelopmentProgress() const {
        // Convert developmental stage to progress percentage
        auto stage = brain->getDevelopmentalStage();
        switch (stage) {
            case DevelopmentalStage::Initial:
                return 0.0;
            case DevelopmentalStage::CriticalPeriod:
                return 0.25;
            case DevelopmentalStage::Maturation:
                return 0.5;
            case DevelopmentalStage::Adult:
                return 0.75;
            case DevelopmentalStage::Aging:
                return 1.0;
            default:
                return 0.5;
        }
    }

    std::string getDevelopmentalStageName() const {
        auto stage = brain->getDevelopmentalStage();
        switch (stage) {
            case DevelopmentalStage::Initial:
                return "Initial";
            case DevelopmentalStage::CriticalPeriod:
                return "Critical Period";
            case DevelopmentalStage::Maturation:
                return "Maturation";
            case DevelopmentalStage::Adult:
                return "Adult";
            case DevelopmentalStage::Aging:
                return "Aging";
            default:
                return "Unknown";
        }
    }

    py::dict getStats() const {
        py::dict stats;
        stats["neuron_count"] = static_cast<size_t>(brain->getTotalNeuronCount());
        stats["synapse_count"] = static_cast<size_t>(brain->getTotalSynapseCount());
        stats["firing_count"] = static_cast<size_t>(brain->getFiringNeuronCount());
        stats["average_firing_rate"] = brain->getAverageFiringRate();
        stats["total_spikes"] = static_cast<size_t>(brain->getTotalSpikeCount());
        stats["development_stage"] = getDevelopmentalStageName();
        stats["development_progress"] = getDevelopmentProgress();
        return stats;
    }
};

// Configuration manager with enhanced Pythonic interface
class ConfigurationManager {
private:
    std::shared_ptr<Config> config;

public:
    ConfigurationManager() : config(std::make_shared<Config>()) {}

    ConfigurationManager(const std::string& filepath) : config(std::make_shared<Config>()) {
        if (!config->loadFromFile(filepath)) {
            throw ConfigurationError("Failed to load configuration from: " + filepath);
        }
    }

    void loadFromFile(const std::string& filepath) {
        if (!config->loadFromFile(filepath)) {
            throw ConfigurationError("Failed to load configuration from: " + filepath);
        }
    }

    void saveToFile(const std::string& filepath) {
        config->saveToFile(filepath);
    }

    void set(const std::string& key, const std::string& value) {
        config->set(key, value);
    }

    void set(const std::string& key, int value) {
        config->set(key, value);
    }

    void set(const std::string& key, float value) {
        config->set(key, value);
    }

    void set(const std::string& key, double value) {
        config->set(key, value);
    }

    bool has(const std::string& key) const {
        return config->has(key);
    }

    std::string get(const std::string& key) const {
        return config->getOr<std::string>(key, "");
    }

    int getInt(const std::string& key) const {
        return config->getOr<int>(key, 0);
    }

    float getFloat(const std::string& key) const {
        return config->getOr<float>(key, 0.0f);
    }

    double getDouble(const std::string& key) const {
        return config->getOr<double>(key, 0.0);
    }

    py::list getKeys() const {
        py::list keys;
        auto configKeys = config->getKeys();
        for (const auto& key : configKeys) {
            keys.append(key);
        }
        return keys;
    }

    std::string getSummary() const {
        return config->summary();
    }

    std::shared_ptr<Config> getConfig() const {
        return config;
    }

    // Load from environment variables
    void loadFromEnvironment(const std::string& prefix = "NLM_") {
        for (auto& [key, value] : config->getKeys()) {
            std::string envKey = prefix + key;
            const char* envValue = std::getenv(envKey.c_str());
            if (envValue) {
                // Convert value based on key type
                if (key.find("count") != std::string::npos || key.find("steps") != std::string::npos) {
                    try {
                        int intValue = std::stoi(envValue);
                        config->set(key, intValue);
                    } catch (...) {
                        // Ignore conversion errors
                    }
                } else if (key.find("rate") != std::string::npos || key.find("weight") != std::string::npos || 
                           key.find("probability") != std::string::npos) {
                    try {
                        float floatValue = std::stof(envValue);
                        config->set(key, floatValue);
                    } catch (...) {
                        // Ignore conversion errors
                    }
                } else {
                    config->set(key, std::string(envValue));
                }
            }
        }
    }

    // Create configuration from Python dictionary
    void updateFromDict(const py::dict& settings) {
        for (auto item : settings) {
            std::string key = py::cast<std::string>(item.first);
            py::object value = item.second;

            if (value.is(py::int_())) {
                config->set(key, py::cast<int>(value));
            } else if (value.is(py::float_())) {
                config->set(key, py::cast<float>(value));
            } else if (value.is(py::str())) {
                config->set(key, py::cast<std::string>(value));
            }
        }
    }

    py::dict toDict() const {
        py::dict result;
        for (auto& key : config->getKeys()) {
            result[key] = config->getOr<std::string>(key, "");
        }
        return result;
    }
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Enhanced Python Bindings
        -------------------------------------------------------
        An enhanced Python interface for the NLM C++ neural simulation framework.

        This module provides:
        - Context managers for safe brain resource management
        - Enhanced configuration system with file and environment support
        - Better error handling with specific Python exceptions
        - Convenience methods and statistics reporting
        - Pythonic interfaces for common operations

        Key Features:
        * BrainContext class for automatic brain lifecycle management
        * ConfigurationManager for advanced configuration handling
        * Enhanced error messages and exception handling
        * Python-style statistics and progress reporting
        * Support for dictionary-based configuration
    )pbdoc";

    // Register custom exceptions
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const NLMException& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::exception& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });

    // Register custom exceptions
    py::register_exception<NLMException>(m, "NLMError");
    py::register_exception<ConfigurationError>(m, "ConfigurationError");
    py::register_exception<InitializationError>(m, "InitializationError");

    // NeuronId with enhanced Python interface
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
        })
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        });

    // SynapseId with enhanced Python interface
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
        })
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
        });

    // RegionId with enhanced Python interface
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
        })
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
        });

    // PopulationId with enhanced Python interface
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // NeuronType enumeration
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    // SynapseType enumeration
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    // DevelopmentalStage enumeration
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    // FiringState enumeration
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    // ActionType enumeration
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

    // MotorCommand enumeration
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

    // WorldObjectType enumeration
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Enhanced Config class with better Python interface
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
        });

    // Enhanced SensoryInput class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Vision class with enhanced Python interface
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

    // Audio class with enhanced Python interface
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    // InternalSignals class with enhanced Python interface
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    // Action class with enhanced Python interface
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

    // WorldObject class with enhanced Python interface
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

    // AgentBody class with enhanced Python interface
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

    // ActionResult class with enhanced Python interface
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // SensoryPercept class with enhanced Python interface
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

    // SimpleWorld class with enhanced Python interface
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"))
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &SimpleWorld::update, py::arg("timestep"))
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"))
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

    // Enhanced Brain class with context manager and better Python interface
    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
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
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
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
             "Log brain status");

    // Brain context manager class
    py::class_<BrainContext>(m, "BrainContext", R"pbdoc(Context manager for safe brain resource management)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>())
        .def("__enter__", &BrainContext::__enter__)
        .def("__exit__", &BrainContext::__exit__, py::arg("args"), py::arg("kwargs"))
        .def("getBrain", &BrainContext::getBrain,
             "Get the underlying brain object")
        .def("isInitialized", &BrainContext::isInitialized,
             "Check if the brain context is initialized")
        .def("step", &BrainContext::step, py::arg("step"),
             "Perform a simulation step")
        .def("stepWithTime", [](BrainContext& self, SimulationStep step, Timestamp time) {
            self.step(step, time);
        }, py::arg("step"), py::arg("time"),
           "Perform a simulation step with timestamp")
        .def("reset", &BrainContext::reset,
             "Reset the brain context")
        .def("getStats", &BrainContext::getStats,
             "Get brain statistics as a dictionary")
        .def("getTotalNeuronCount", &BrainContext::getTotalNeuronCount,
             "Get total neuron count")
        .def("getTotalSynapseCount", &BrainContext::getTotalSynapseCount,
             "Get total synapse count")
        .def("getFiringNeuronCount", &BrainContext::getFiringNeuronCount,
             "Get firing neuron count")
        .def("getAverageFiringRate", &BrainContext::getAverageFiringRate,
             "Get average firing rate")
        .def("getDevelopmentProgress", &BrainContext::getDevelopmentProgress,
             "Get development progress (0-1)")
        .def("getDevelopmentStageName", &BrainContext::getDevelopmentalStageName,
             "Get human-readable developmental stage name");

    // AgentBrain class with enhanced Python interface
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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

    // Configuration manager class
    py::class_<ConfigurationManager>(m, "ConfigurationManager", R"pbdoc(Advanced configuration manager for NLM)pbdoc")
        .def(py::init<>())
        .def(py::init<std::string>(), py::arg("filepath"))
        .def("loadFromFile", &ConfigurationManager::loadFromFile, py::arg("filepath"),
             "Load configuration from a file")
        .def("saveToFile", &ConfigurationManager::saveToFile, py::arg("filepath"),
             "Save configuration to a file")
        .def("set", static_cast<void (ConfigurationManager::*)(const std::string&, const std::string&)>(&ConfigurationManager::set),
             py::arg("key"), py::arg("value"),
             "Set string configuration value")
        .def("set", static_cast<void (ConfigurationManager::*)(const std::string&, int)>(&ConfigurationManager::set),
             py::arg("key"), py::arg("value"),
             "Set integer configuration value")
        .def("set", static_cast<void (ConfigurationManager::*)(const std::string&, float)>(&ConfigurationManager::set),
             py::arg("key"), py::arg("value"),
             "Set float configuration value")
        .def("set", static_cast<void (ConfigurationManager::*)(const std::string&, double)>(&ConfigurationManager::set),
             py::arg("key"), py::arg("value"),
             "Set double configuration value")
        .def("has", &ConfigurationManager::has, py::arg("key"),
             "Check if configuration key exists")
        .def("get", static_cast<std::string (ConfigurationManager::*)(const std::string&) const>(&ConfigurationManager::get),
             py::arg("key"),
             "Get string configuration value")
        .def("getInt", &ConfigurationManager::getInt, py::arg("key"),
             "Get integer configuration value")
        .def("getFloat", &ConfigurationManager::getFloat, py::arg("key"),
             "Get float configuration value")
        .def("getDouble", &ConfigurationManager::getDouble, py::arg("key"),
             "Get double configuration value")
        .def("getKeys", &ConfigurationManager::getKeys,
             "Get all configuration keys")
        .def("getSummary", &ConfigurationManager::getSummary,
             "Get configuration summary")
        .def("loadFromEnvironment", &ConfigurationManager::loadFromEnvironment, py::arg("prefix") = "NLM_",
             "Load configuration from environment variables")
        .def("updateFromDict", &ConfigurationManager::updateFromDict, py::arg("settings"),
             "Update configuration from Python dictionary")
        .def("toDict", &ConfigurationManager::toDict,
             "Convert configuration to Python dictionary")
        .def("getConfig", &ConfigurationManager::getConfig,
             "Get the underlying Config object");

    // Factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createBrainContext", [](std::shared_ptr<Brain> brain) -> BrainContext {
        return BrainContext(brain);
    }, py::arg("brain"), "Create a brain context manager");

    m.def("createConfigurationManager", []() -> ConfigurationManager {
        return ConfigurationManager();
    }, "Create a new configuration manager");

    m.def("createConfigurationManagerFromFile", [](const std::string& filepath) -> ConfigurationManager {
        return ConfigurationManager(filepath);
    }, py::arg("filepath"), "Create a configuration manager from file");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Global constants and attributes
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Add Python package metadata
    m.attr("__version__") = "2.0.0-enhanced";
    m.attr("__description__") = "Enhanced Python bindings for NLM (Neural Learning Machine)";
    m.attr("__author__") = "NLM Project Team";
}

} // namespace nlm

#endif // NLM_PYTHON_BINDINGS_HPP"