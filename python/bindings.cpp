#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <iostream>

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

// Python exception for NLM errors
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& message) : std::runtime_error(message) {}
};

// Python version of Config class with enhanced API
class ConfigWrapper {
private:
    std::shared_ptr<Config> config;
public:
    ConfigWrapper() : config(std::make_shared<Config>()) {}
    
    explicit ConfigWrapper(const std::string& filepath) {
        config = std::make_shared<Config>();
        if (!config->loadFromFile(filepath)) {
            throw NLMError("Failed to load configuration from file: " + filepath);
        }
    }
    
    // Enhanced configuration methods
    void set(const std::string& key, int value) {
        config->set<int>(key, value);
    }
    
    void set(const std::string& key, float value) {
        config->set<float>(key, value);
    }
    
    void set(const std::string& key, double value) {
        config->set<double>(key, value);
    }
    
    void set(const std::string& key, const std::string& value) {
        config->set<std::string>(key, value);
    }
    
    void set(const std::string& key, bool value) {
        config->set<bool>(key, value);
    }
    
    // Type-safe getters
    int getInt(const std::string& key) const {
        return config->get<int>(key);
    }
    
    float getFloat(const std::string& key) const {
        return config->get<float>(key);
    }
    
    double getDouble(const std::string& key) const {
        return config->get<double>(key);
    }
    
    std::string getString(const std::string& key) const {
        return config->get<std::string>(key);
    }
    
    bool getBool(const std::string& key) const {
        return config->get<bool>(key);
    }
    
    // Convenience methods for common configuration settings
    void configureBrain(int neuronCount = 1000, float connectionProbability = 0.1) {
        set("brain.neuron_count", neuronCount);
        set("brain.connection_probability", connectionProbability);
    }
    
    void configureWorld(int width, int height, int visionWidth = 8, int visionHeight = 8) {
        set("environment.width", width);
        set("environment.height", height);
        set("environment.vision_width", visionWidth);
        set("environment.vision_height", visionHeight);
    }
    
    // Validate configuration
    bool validate() const {
        // Simple validation - can be extended
        if (config->has("brain.neuron_count")) {
            int neuronCount = getInt("brain.neuron_count");
            if (neuronCount <= 0) return false;
        }
        return true;
    }
    
    std::string summary() const {
        return config->summary();
    }
    
    // Access to underlying config
    std::shared_ptr<Config> getConfig() { return config; }
    const std::shared_ptr<Config> getConfig() const { return config; }
};

// Enhanced Brain wrapper class
class BrainWrapper {
private:
    std::shared_ptr<Brain> brain;
public:
    BrainWrapper(std::shared_ptr<Config> config) : brain(std::make_shared<Brain>(config)) {}
    
    // Simulation control with error handling
    bool initialize() {
        try {
            return brain->initialize();
        } catch (const std::exception& e) {
            throw NLMError("Brain initialization failed: " + std::string(e.what()));
        }
    }
    
    // Enhanced step methods with validation
    void step(int stepNumber) {
        brain->step(static_cast<SimulationStep>(stepNumber));
    }
    
    void step(int stepNumber, double time) {
        brain->step(static_cast<SimulationStep>(stepNumber), static_cast<Timestamp>(time));
    }
    
    // Convenience simulation methods
    void runSimulation(int numSteps, double timestep = 0.1) {
        for (int i = 0; i < numSteps; ++i) {
            brain->step(i, i * timestep);
        }
    }
    
    // Enhanced action production
    std::unique_ptr<Action> produceActionWithValidation() {
        auto action = brain->produceAction();
        if (!action) {
            throw NLMError("Failed to produce action - brain may not be initialized");
        }
        return action;
    }
    
    // Get statistics with convenience methods
    std::map<std::string, double> getStatistics() const {
        std::map<std::string, double> stats;
        stats["total_neurons"] = brain->getTotalNeuronCount();
        stats["total_synapses"] = brain->getTotalSynapseCount();
        stats["firing_neurons"] = brain->getFiringNeuronCount();
        stats["active_neurons"] = brain->getActiveNeuronCount();
        stats["total_spikes"] = brain->getTotalSpikeCount();
        stats["avg_firing_rate"] = brain->getAverageFiringRate();
        stats["e_i_ratio"] = brain->getExcitationInhibitionRatio();
        stats["pending_spikes"] = brain->getPendingSpikeEventCount();
        return stats;
    }
    
    // Enhanced state management
    bool saveState(const std::string& filepath) const {
        try {
            return brain->save(filepath);
        } catch (const std::exception& e) {
            throw NLMError("Failed to save brain state: " + std::string(e.what()));
        }
    }
    
    bool loadState(const std::string& filepath) {
        try {
            return brain->load(filepath);
        } catch (const std::exception& e) {
            throw NLMError("Failed to load brain state: " + std::string(e.what()));
        }
    }
    
    // Development control
    void setDevelopmentStage(int stage) {
        DevelopmentalStage devStage = static_cast<DevelopmentalStage>(stage);
        brain->setDevelopmentalStage(devStage);
    }
    
    int getDevelopmentStage() const {
        return static_cast<int>(brain->getDevelopmentalStageStage());
    }
    
    // Access to subsystems
    std::shared_ptr<Brain> getBrain() { return brain; }
    const std::shared_ptr<Brain> getBrain() const { return brain; }
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------
        Enhanced Python bindings for the NLM C++ neural simulation framework.
        
        This module provides:
        - Brain simulation with spiking neural networks
        - Agent-based learning and behavior
        - World simulation for environment interaction
        - Memory and learning systems
        - Development and neuromodulation
        
        Key features:
        - Enhanced error handling and validation
        - Convenience methods for common operations
        - Type-safe configuration API
        - Statistics and monitoring utilities
        - Advanced simulation control
        
        Usage examples:
        >>> import pynlm
        >>> config = pynlm.ConfigWrapper()
        >>> config.configureBrain(neuron_count=1000)
        >>> brain = pynlm.BrainWrapper(config)
        >>> brain.initialize()
        >>> brain.runSimulation(100)
    )pbdoc";

    // Register custom exception
    py::register_exception<NLMError>(m, "NLMError", PyExc_RuntimeError);

    // NeuronId with enhanced functionality
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

    // SynapseId with enhanced functionality
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

    // RegionId with enhanced functionality
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

    // PopulationId with enhanced functionality
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Enumerations
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

    // Enhanced Config class
    py::class_<ConfigWrapper>(m, "ConfigWrapper", R"pbdoc(Enhanced configuration wrapper for NLM system)pbdoc")
        .def(py::init<>())
        .def(py::init<std::string>(), py::arg("filepath"))
        .def("loadFromFile", &ConfigWrapper::getConfig->loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("saveToFile", &ConfigWrapper::getConfig->saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &ConfigWrapper::getConfig->has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &ConfigWrapper::getConfig->getKeys,
             "Get all configuration keys")
        .def("clear", &ConfigWrapper::getConfig->clear,
             "Clear all configuration entries")
        .def("summary", &ConfigWrapper::summary,
             "Get a summary string of the configuration")
        
        // Enhanced setter/getter methods
        .def("setInt", &ConfigWrapper::set, py::arg("key"), py::arg("value"), "Set integer configuration value")
        .def("setFloat", &ConfigWrapper::set, py::arg("key"), py::arg("value"), "Set float configuration value")
        .def("setDouble", &ConfigWrapper::set, py::arg("key"), py::arg("value"), "Set double configuration value")
        .def("setString", &ConfigWrapper::set, py::arg("key"), py::arg("value"), "Set string configuration value")
        .def("setBool", &ConfigWrapper::set, py::arg("key"), py::arg("value"), "Set boolean configuration value")
        
        .def("getInt", &ConfigWrapper::getInt, py::arg("key"), "Get integer configuration value")
        .def("getFloat", &ConfigWrapper::getFloat, py::arg("key"), "Get float configuration value")
        .def("getDouble", &ConfigWrapper::getDouble, py::arg("key"), "Get double configuration value")
        .def("getString", &ConfigWrapper::getString, py::arg("key"), "Get string configuration value")
        .def("getBool", &ConfigWrapper::getBool, py::arg("key"), "Get boolean configuration value")
        
        // Convenience configuration methods
        .def("configureBrain", &ConfigWrapper::configureBrain, py::arg("neuronCount") = 1000, py::arg("connectionProbability") = 0.1,
             "Configure brain parameters")
        .def("configureWorld", &ConfigWrapper::configureWorld, py::arg("width"), py::arg("height"), py::arg("visionWidth") = 8, py::arg("visionHeight") = 8,
             "Configure world parameters")
        .def("validate", &ConfigWrapper::validate,
             "Validate configuration parameters")
        .def("__repr__", [](const ConfigWrapper& cfg) {
            return "<ConfigWrapper: " + cfg.summary() + ">";
        });

    // Enhanced Brain class
    py::class_<BrainWrapper>(m, "BrainWrapper", R"pbdoc(Enhanced brain simulation wrapper)pbdoc")
        .def(py::init<std::shared_ptr<ConfigWrapper>>(), py::arg("config"))
        .def("initialize", &BrainWrapper::initialize,
             "Initialize the brain with configuration")
        .def("step", py::overload_cast<int>(&BrainWrapper::step), py::arg("stepNumber"),
             "Perform a simulation step with step number")
        .def("step", py::overload_cast<int, double>(&BrainWrapper::step), py::arg("stepNumber"), py::arg("time"),
             "Perform a simulation step with step number and timestamp")
        .def("runSimulation", &BrainWrapper::runSimulation, py::arg("numSteps"), py::arg("timestep") = 0.1,
             "Run a complete simulation with specified steps")
        .def("produceAction", &BrainWrapper::produceActionWithValidation,
             "Produce motor action with validation")
        .def("getStatistics", &BrainWrapper::getStatistics,
             "Get comprehensive brain statistics")
        .def("saveState", &BrainWrapper::saveState, py::arg("filepath"),
             "Save brain state to file")
        .def("loadState", &BrainWrapper::loadState, py::arg("filepath"),
             "Load brain state from file")
        .def("setDevelopmentStage", &BrainWrapper::setDevelopmentStage, py::arg("stage"),
             "Set developmental stage")
        .def("getDevelopmentStage", &BrainWrapper::getDevelopmentStage,
             "Get current developmental stage")
        .def("logStatus", &BrainWrapper::getBrain->logStatus,
             "Log brain status")
        .def("__repr__", [](const BrainWrapper& brain) {
            auto stats = brain.getStatistics();
            return "<BrainWrapper: " + std::to_string(stats["total_neurons"]) + 
                   " neurons, " + std::to_string(stats["total_spikes"]) + 
                   " total spikes>";
        });

    // Original classes (for backward compatibility and direct access)
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

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
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
        
        // Convenience methods for common agent operations
        .def("runEpisode", [](AgentBrain& self, std::shared_ptr<SimpleWorld> world, 
                              size_t numSteps) {
            // Default timestep
            double timestep = 0.1;
            
            for (size_t i = 0; i < numSteps; ++i) {
                // Update world
                world->update(timestep);
                
                // Get sensory input
                SensoryPercept percept = world->getSensoryPercept();
                
                // Process sensory input
                self.processSensoryInput(percept);
                
                // Note: Brain step is not called here - user manages brain separately
                
                if (i % 10 == 0) {
                    // Optional logging
                }
            }
            
            return 0;
        }, py::arg("world"), py::arg("numSteps"),
             "Run a complete episode simulation")
        
        // Environment interaction helper
        .def("simulateInteraction", [](AgentBrain& self, std::shared_ptr<SimpleWorld> world,
                                        size_t numSteps, bool enableLearning = true) {
            if (enableLearning) {
                self.enableRewardModulation(true);
                self.enableCuriosity(true);
                self.enableDevelopment(true);
            }
            
            for (size_t i = 0; i < numSteps; ++i) {
                world->update(0.1);
                
                SensoryPercept percept = world->getSensoryPercept();
                self.processSensoryInput(percept);
                
                // Note: Brain step is not called here - user manages brain separately
                
                if (i % 10 == 0) {
                    // Optional logging
                }
            }
            
            return 0;
        }, py::arg("world"), py::arg("numSteps"), py::arg("enableLearning") = true,
             "Run a simulation with automatic learning enabled");

    m.def("createDefaultConfig", []() -> std::shared_ptr<ConfigWrapper> {
        return std::make_shared<ConfigWrapper>();
    }, "Create a default configuration wrapper");

    m.def("createConfig", [](const std::string& filepath) -> std::shared_ptr<ConfigWrapper> {
        return std::make_shared<ConfigWrapper>(filepath);
    }, py::arg("filepath"), "Create configuration from file");

    m.def("createBrain", [](std::shared_ptr<ConfigWrapper> config) -> std::shared_ptr<BrainWrapper> {
        return std::make_shared<BrainWrapper>(config->getConfig());
    }, py::arg("config"), "Create a new brain wrapper with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<BrainWrapper> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain->getBrain());
    }, py::arg("brain"), "Create a new agent brain interface");

    m.def("INVALID_NEURON_ID", py::cast(INVALID_NEURON_ID));
    m.def("INVALID_SYNAPSE_ID", py::cast(INVALID_SYNAPSE_ID));
    m.def("INVALID_REGION_ID", py::cast(INVALID_REGION_ID));
    m.def("INVALID_POPULATION_ID", py::cast(INVALID_POPULATION_ID));

    // Enhanced factory functions
    m.def("createBrainWithConfig", [](const std::string& configFile) -> std::shared_ptr<BrainWrapper> {
        ConfigWrapper cfg(configFile);
        auto brain = std::make_shared<BrainWrapper>(std::make_shared<Config>(cfg.getConfig()));
        if (!brain->initialize()) {
            throw NLMError("Failed to initialize brain from config file: " + configFile);
        }
        return brain;
    }, py::arg("configFile"), "Create and initialize brain from config file");

    m.def("createReadyAgent", [](const std::string& configFile, size_t numSteps = 1000) {
        // Create and initialize complete agent system
        ConfigWrapper cfg(configFile);
        cfg.configureBrain(neuronCount = 1000);
        
        auto brain = std::make_shared<BrainWrapper>(std::make_shared<Config>(cfg.getConfig()));
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain->getBrain());
        agent->initialize(world);
        
        // Run simulation
        brain->runSimulation(numSteps);
        
        return std::make_tuple(brain, world, agent);
    }, py::arg("configFile"), py::arg("numSteps") = 1000, "Create and run a complete agent system");

    // Version and utility functions
    m.attr("__version__") = "0.1.0-enhanced";
    m.attr("__description__") = "Enhanced NLM Python bindings with improved error handling and convenience methods";

    // Module-level helper functions
    m.def("version", []() {
        return std::map<std::string, std::string>{
            {"version", "0.1.0-enhanced"},
            {"description", "Enhanced NLM Python bindings"},
            {"config", "Enhanced configuration with type safety"},
            {"brain", "Brain simulation with statistics"},
            {"agent", "Agent brain interface with convenience methods"}
        };
    }, "Get module version and feature information");

    m.def("validateConfiguration", [](const std::string& configFile) {
        ConfigWrapper cfg(configFile);
        bool isValid = cfg.validate();
        std::map<std::string, bool> result;
        result["valid"] = isValid;
        result["has_neuron_count"] = cfg.has("brain.neuron_count");
        result["has_connection_probability"] = cfg.has("brain.connection_probability");
        return result;
    }, py::arg("configFile"), "Validate configuration file");

    m.def("runQuickTest", []() {
        // Create a minimal working system for quick testing
        auto config = std::make_shared<ConfigWrapper>();
        config->configureBrain(neuronCount = 100);
        
        auto brain = std::make_shared<BrainWrapper>(std::make_shared<Config>(config->getConfig()));
        if (!brain->initialize()) {
            throw NLMError("Quick test failed: Brain initialization");
        }
        
        brain->runSimulation(10);
        
        return std::map<std::string, std::string>{
            {"status", "success"},
            {"neurons", std::to_string(brain->getStatistics()["total_neurons"])},
            {"spikes", std::to_string(brain->getStatistics()["total_spikes"])}
        };
    }, "Run a quick test of the NLM system");

} // namespace nlm