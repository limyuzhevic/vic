#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <stdexcept>
#include <sstream>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Acetylcholine.hpp"
#include "../src/neuromodulation/Norepinephrine.hpp"
#include "../src/neuromodulation/Serotonin.hpp"

namespace py = pybind11;
namespace nlm {

// Custom NLM exceptions
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& msg) : NLMError("Configuration error: " + msg) {}
};

class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& msg) : NLMError("Simulation error: " + msg) {}
};

class BrainInitializationError : public NLMError {
public:
    explicit BrainInitializationError(const std::string& msg) : NLMError("Brain initialization error: " + msg) {}
};

// Context manager for automatic brain cleanup
class BrainScope {
public:
    BrainScope(std::shared_ptr<Brain> brain) : brain_(brain) {}
    ~BrainScope() {
        if (brain_) {
            brain_->reset();
        }
    }
    
    // Prevent copying
    BrainScope(const BrainScope&) = delete;
    BrainScope& operator=(const BrainScope&) = delete;
    
    // Allow moving
    BrainScope(BrainScope&& other) noexcept : brain_(std::move(other.brain_)) {}
    BrainScope& operator=(BrainScope&& other) noexcept {
        if (this != &other) {
            brain_ = std::move(other.brain_);
        }
        return *this;
    }
    
    std::shared_ptr<Brain> getBrain() const { return brain_; }
    
private:
    std::shared_ptr<Brain> brain_;
};

// Context manager for world simulation
class WorldScope {
public:
    WorldScope(std::shared_ptr<SimpleWorld> world) : world_(world) {}
    ~WorldScope() {
        if (world_) {
            world_->reset();
        }
    }
    
    // Prevent copying
    WorldScope(const WorldScope&) = delete;
    WorldScope& operator=(const WorldScope&) = delete;
    
    // Allow moving
    WorldScope(WorldScope&& other) noexcept : world_(std::move(other.world_)) {}
    WorldScope& operator=(WorldScope&& other) noexcept {
        if (this != &other) {
            world_ = std::move(other.world_);
        }
        return *this;
    }
    
    std::shared_ptr<SimpleWorld> getWorld() const { return world_; }
    
private:
    std::shared_ptr<SimpleWorld> world_;
};

// Convenience functions for configuration
namespace config {
    std::shared_ptr<Config> createBrainConfig(int neuronCount = 1000, double synapseDensity = 0.1) {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", neuronCount);
        config->set("brain.synapse_density", synapseDensity);
        return config;
    }
    
    std::shared_ptr<Config> createAdvancedConfig(int neuronCount = 1000, double synapseDensity = 0.1, 
                                                 double v_thresh = -50.0, double v_rest = -70.0) {
        auto config = createBrainConfig(neuronCount, synapseDensity);
        config->set("brain.v_thresh", v_thresh);
        config->set("brain.v_rest", v_rest);
        return config;
    }
    
    std::shared_ptr<Config> createSimulationConfig(int steps = 1000, double timestep = 0.1) {
        auto config = std::make_shared<Config>();
        config->set("simulation.steps", steps);
        config->set("simulation.timestep", timestep);
        return config;
    }
}

// Convenience functions for brain creation
namespace brain {
    std::shared_ptr<Brain> createDefaultBrain(int neuronCount = 1000) {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", neuronCount);
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw BrainInitializationError("Failed to initialize default brain");
        }
        return brain;
    }
    
    std::shared_ptr<Brain> createTestBrain() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 100);
        config->set("brain.synapse_density", 0.05);
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw BrainInitializationError("Failed to initialize test brain");
        }
        return brain;
    }
}

// Convenience functions for world creation
namespace world {
    std::shared_ptr<SimpleWorld> createDefaultWorld(int width = 20, int height = 20) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, 8, 8);
        world->setAgentStart(width/2.0f, height/2.0f);
        return world;
    }
    
    std::shared_ptr<SimpleWorld> createEmptyWorld(int width = 20, int height = 20) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, 0, 0); // No vision sensors
        return world;
    }
}

// Performance monitoring utilities
namespace monitor {
    struct SimulationStats {
        double elapsedTime = 0.0;
        double stepsPerSecond = 0.0;
        size_t totalSpikes = 0;
        size_t activeNeurons = 0;
        double averageFiringRate = 0.0;
        float eiratio = 0.0f;
        
        std::string toString() const {
            std::stringstream ss;
            ss << "SimulationStats{" << std::endl;
            ss << "  elapsedTime: " << elapsedTime << std::endl;
            ss << "  stepsPerSecond: " << stepsPerSecond << std::endl;
            ss << "  totalSpikes: " << totalSpikes << std::endl;
            ss << "  activeNeurons: " << activeNeurons << std::endl;
            ss << "  averageFiringRate: " << averageFiringRate << std::endl;
            ss << "  eiratio: " << eiratio << std::endl;
            ss << "}" << std::endl;
            return ss.str();
        }
    };
    
    SimulationStats collectStats(const std::shared_ptr<Brain>& brain, double elapsedTime) {
        SimulationStats stats;
        stats.elapsedTime = elapsedTime;
        if (elapsedTime > 0.0) {
            stats.stepsPerSecond = static_cast<double>(brain->getTotalSpikeCount()) / elapsedTime;
        }
        stats.totalSpikes = brain->getTotalSpikeCount();
        stats.activeNeurons = brain->getActiveNeuronCount();
        stats.averageFiringRate = brain->getAverageFiringRate();
        stats.eiratio = brain->getExcitationInhibitionRatio();
        return stats;
    }
}

// Data export/import utilities
namespace export_utils {
    bool exportBrainState(const std::shared_ptr<Brain>& brain, const std::string& filepath) {
        return brain->save(filepath);
    }
    
    bool importBrainState(const std::shared_ptr<Brain>& brain, const std::string& filepath) {
        return brain->load(filepath);
    }
    
    std::vector<float> getAllNeuronsData(const std::shared_ptr<Brain>& brain) {
        // This would require additional C++ support to collect neuron data
        // For now, return empty vector as placeholder
        return std::vector<float>();
    }
    
    std::vector<float> getAllSynapsesData(const std::shared_ptr<Brain>& brain) {
        // This would require additional C++ support to collect synapse data
        // For now, return empty vector as placeholder
        return std::vector<float>();
    }
}

// Main bindings module
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A comprehensive Python binding for the NLM C++ neural simulation framework.
        Provides advanced brain simulation, agent-world interaction, and research capabilities.
        
        Key Features:
        - Complete C++ API access with Pythonic interface
        - Context managers for automatic resource cleanup
        - Extensive configuration and simulation utilities
        - Performance monitoring and statistics collection
        - Data export/import utilities
        - Custom exceptions for error handling
        - Advanced neuromodulation support
        - Social learning and observation capabilities
        - Developmental stage tracking
        - Rich statistical analysis tools
    )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMError>(m, "NLMError");
    py::register_exception<ConfigurationError>(m, "ConfigurationError");
    py::register_exception<SimulationError>(m, "SimulationError");
    py::register_exception<BrainInitializationError>(m, "BrainInitializationError");

    // =============================================================================
    // Core Type Definitions
    // =============================================================================

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

    // =============================================================================
    // Configuration System
    // =============================================================================

    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Configuration system for NLM brain)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
            std::vector<char*> argv;
            argv.reserve(args.size());
            for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
            return self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
        }, py::arg("args"),
             "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get", [](const Config& self, const std::string& key) {
            // Try to get as int, then double, then bool, then string
            auto intVal = self.get<int>(key);
            if (intVal.has_value()) return *intVal;
            
            auto doubleVal = self.get<double>(key);
            if (doubleVal.has_value()) return *doubleVal;
            
            auto boolVal = self.get<bool>(key);
            if (boolVal.has_value()) return *boolVal;
            
            auto stringVal = self.get<std::string>(key);
            if (stringVal.has_value()) return *stringVal;
            
            throw ConfigurationError("Key not found: " + key);
        }, py::arg("key"),
             "Get configuration value (automatically infer type)")
        .def("get", [](const Config& self, const std::string& key, const py::object& defaultValue) {
            auto intVal = self.get<int>(key);
            if (intVal.has_value()) return *intVal;
            
            auto doubleVal = self.get<double>(key);
            if (doubleVal.has_value()) return *doubleVal;
            
            auto boolVal = self.get<bool>(key);
            if (boolVal.has_value()) return *boolVal;
            
            auto stringVal = self.get<std::string>(key);
            if (stringVal.has_value()) return *stringVal;
            
            // Return default value if provided
            if (!defaultValue.is_none()) {
                return defaultValue.cast<typename Config::ConfigValue::ValueType>();
            }
            
            throw ConfigurationError("Key not found and no default provided: " + key);
        }, py::arg("key"), py::arg("defaultValue"),
             "Get configuration value with default fallback")
        .def("set", [](Config& self, const std::string& key, const py::object& value) {
            if (value.is_none()) {
                throw ConfigurationError("Cannot set configuration value to None");
            }
            if (value.isinstance(py::int_())) {
                self.set(key, value.cast<int>());
            } else if (value.isinstance(py::float_())) {
                self.set(key, value.cast<double>());
            } else if (value.isinstance(py::bool_())) {
                self.set(key, value.cast<bool>());
            } else if (value.isinstance(py::str())) {
                self.set(key, value.cast<std::string>());
            } else if (value.isinstance(py::list())) {
                auto vec = value.cast<std::vector<double>>();
                self.set(key, vec);
            } else {
                throw ConfigurationError("Unsupported type for configuration value: " + key);
            }
        }, py::arg("key"), py::arg("value"),
             "Set configuration value with automatic type conversion")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // =============================================================================
    // Sensory Systems
    // =============================================================================

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

    // =============================================================================
    // Neuromodulation System
    // =============================================================================

    py::class_<Neuromodulator>(m, "Neuromodulator", py::abstract(), R"pbdoc(Abstract base class for neuromodulatory signals)pbdoc")
        .def("getName", &Neuromodulator::getName, "Get the neuromodulator name")
        .def("getLevel", &Neuromodulator::getLevel, "Get the current level")
        .def("setLevel", &Neuromodulator::setLevel, py::arg("level"), "Set the level")
        .def("getPlasticityFactor", &Neuromodulator::getPlasticityFactor, "Get plasticity factor")
        .def("update", &Neuromodulator::update, py::arg("dt"), "Update neuromodulator state");

    py::class_<Dopamine, Neuromodulator>(m, "Dopamine", R"pbdoc(Dopamine neuromodulator - reward and reinforcement)pbdoc")
        .def(py::init<>())
        .def("signalReward", &Dopamine::signalReward, py::arg("reward"), "Signal a reward")
        .def("signalRewardPredictionError", &Dopamine::signalRewardPredictionError, py::arg("error"), "Signal reward prediction error");

    py::class_<Acetylcholine, Neuromodulator>(m, "Acetylcholine", R"pbdoc(Acetylcholine neuromodulator - attention and memory)pbdoc")
        .def(py::init<>());

    py::class_<Norepinephrine, Neuromodulator>(m, "Norepinephrine", R"pbdoc(Norepinephrine neuromodulator - arousal and vigilance)pbdoc")
        .def(py::init<>());

    py::class_<Serotonin, Neuromodulator>(m, "Serotonin", R"pbdoc(Serotonin neuromodulator - mood and impulsivity)pbdoc")
        .def(py::init<>());

    // =============================================================================
    // Motor System
    // =============================================================================

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

    // =============================================================================
    // Agent Systems
    // =============================================================================

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

    // =============================================================================
    // World System
    // =============================================================================

    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
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

    // =============================================================================
    // Brain System
    // =============================================================================

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
        .def("getRegion", static_cast<NeuralRegion* (Brain::*)(RegionId)>(&Brain::getRegion),
             py::arg("id"),
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
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             "Get count of pending spike events")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("getLearnedPreference", &Brain::getLearnedPreference,
             "Get learned preference trait")
        .def("setLearnedPreference", &Brain::setLearnedPreference,
             py::arg("preference"),
             "Set learned preference trait")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status");

    // =============================================================================
    // AgentBrain System
    // =============================================================================

    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
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
        .def("getThoughtTrace", &AgentBrain::getThoughtTrace,
             "Get thought trace (human-readable)")
        .def("getInternalFiringRates", &AgentBrain::getInternalFiringRates,
             "Get internal firing rates")
        .def("getLearnedPreference", &AgentBrain::getLearnedPreference,
             "Get learned preference trait")
        .def("observeOtherAgent", &AgentBrain::observeOtherAgent,
             py::arg("observed"), py::arg("beforeVision"), py::arg("afterVision"),
             "Observe another agent's behavior")
        .def("getMotorGroupActivities", &AgentBrain::getMotorGroupActivities,
             "Get detailed motor group activity metrics")
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

    // =============================================================================
    // Factory Functions
    // =============================================================================

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // =============================================================================
    // Convenience Functions
    // =============================================================================

    // Configuration utilities
    m.def("createBrainConfig", &config::createBrainConfig,
          py::arg("neuronCount") = 1000, py::arg("synapseDensity") = 0.1,
          "Create a simple brain configuration");

    m.def("createAdvancedConfig", &config::createAdvancedConfig,
          py::arg("neuronCount") = 1000, py::arg("synapseDensity") = 0.1,
          py::arg("v_thresh") = -50.0, py::arg("v_rest") = -70.0,
          "Create an advanced brain configuration");

    m.def("createSimulationConfig", &config::createSimulationConfig,
          py::arg("steps") = 1000, py::arg("timestep") = 0.1,
          "Create a simulation configuration");

    // Brain creation utilities
    m.def("createDefaultBrain", &brain::createDefaultBrain,
          py::arg("neuronCount") = 1000,
          "Create and initialize a default brain");

    m.def("createTestBrain", &brain::createTestBrain,
          "Create and initialize a test brain");

    // World creation utilities
    m.def("createDefaultWorld", &world::createDefaultWorld,
          py::arg("width") = 20, py::arg("height") = 20,
          "Create a default world");

    m.def("createEmptyWorld", &world::createEmptyWorld,
          py::arg("width") = 20, py::arg("height") = 20,
          "Create a world with no sensory inputs");

    // =============================================================================
    // Context Managers
    // =============================================================================

    py::class_<BrainScope>(m, "BrainScope", R"pbdoc(Context manager for automatic brain cleanup)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("getBrain", &BrainScope::getBrain, "Get the managed brain");

    py::class_<WorldScope>(m, "WorldScope", R"pbdoc(Context manager for automatic world cleanup)pbdoc")
        .def(py::init<std::shared_ptr<SimpleWorld>>(), py::arg("world"))
        .def("getWorld", &WorldScope::getWorld, "Get the managed world");

    // =============================================================================
    // Monitoring and Statistics
    // =============================================================================

    py::class_<monitor::SimulationStats>(m, "SimulationStats", R"pbdoc(Simulation statistics and monitoring data)pbdoc")
        .def_readonly("elapsedTime", &monitor::SimulationStats::elapsedTime)
        .def_readonly("stepsPerSecond", &monitor::SimulationStats::stepsPerSecond)
        .def_readonly("totalSpikes", &monitor::SimulationStats::totalSpikes)
        .def_readonly("activeNeurons", &monitor::SimulationStats::activeNeurons)
        .def_readonly("averageFiringRate", &monitor::SimulationStats::averageFiringRate)
        .def_readonly("eiratio", &monitor::SimulationStats::eiratio)
        .def("toString", &monitor::SimulationStats::toString, "Get statistics as string");

    m.def("collectStats", &monitor::collectStats,
          py::arg("brain"), py::arg("elapsedTime"),
          "Collect simulation statistics");

    // =============================================================================
    // Export/Import Utilities
    // =============================================================================

    m.def("exportBrainState", &export_utils::exportBrainState,
          py::arg("brain"), py::arg("filepath"),
          "Export brain state to file");

    m.def("importBrainState", &export_utils::importBrainState,
          py::arg("brain"), py::arg("filepath"),
          "Import brain state from file");

    // =============================================================================
    // Constants
    // =============================================================================

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // =============================================================================
    // Module-level Documentation
    // =============================================================================

    m.def("version", []() { return "0.2.0 (Phase 1.0 - Enhanced Python Bindings)"; },
          "Get the version of pynlm");

    m.def("examples", []() {
        return R"pbdoc(
            Basic Examples:
            ===============
            
            import pynlm
            
            # Create a simple brain
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Run simulation
            for i in range(100):
                brain.step(i)
            
            # Create a world
            world = pynlm.createSimpleWorld()
            world.configure(20, 20, 8, 8)
            world.setAgentStart(10.0, 10.0)
            
            # Create agent interface
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            # Enable learning subsystems
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            
            # Run complete simulation loop
            for step in range(1000):
                world.update(0.1)
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
        
            Advanced Examples:
            ==================
            
            # Create with custom configuration
            config = pynlm.createBrainConfig(2000, 0.2)
            brain = pynlm.createBrain(config)
            
            # Use context manager for automatic cleanup
            with pynlm.BrainScope(brain) as scope:
                for step in range(500):
                    brain.step(step)
                    if step % 100 == 0:
                        print(f"Step {step}: {brain.getTotalSpikeCount()} spikes")
            
            # Get detailed statistics
            stats = pynlm.collectStats(brain, 10.0)
            print(stats.toString())
        
            )pbdoc";
    }, "Get example usage and documentation");

    m.def("get_available_features", []() {
        return std::vector<std::string>{
            "Brain simulation and statistics",
            "Agent-world interface",
            "Sensory input/output systems",
            "Neuromodulation (Dopamine, Acetylcholine, NE, 5-HT)",
            "Motor control and action execution",
            "World simulation with spatial dynamics",
            "Developmental stage tracking",
            "Social learning and observation",
            "Configuration management",
            "Performance monitoring",
            "Context managers for resource cleanup",
            "Data export/import utilities",
            "Convenience factory functions",
            "Custom exception handling"
        };
    }, "Get list of available features");

    m.def("get_configuration_options", []() {
        return std::vector<std::string>{
            "brain.neuron_count",
            "brain.synapse_density",
            "brain.connection_probability",
            "brain.initial_weight_mean",
            "brain.initial_weight_std",
            "brain.v_thresh",
            "brain.v_rest",
            "brain.v_reset",
            "brain.tau_mem",
            "brain.tau_ref",
            "plasticity.stdp.enable",
            "plasticity.stdp.learning_rate",
            "plasticity.stdp.tau_plus",
            "plasticity.stdp.tau_minus",
            "plasticity.hebbian.enable",
            "plasticity.structural.enable",
            "neuromod.dopamine.scale",
            "neuromod.curiosity.enable",
            "neuromod.novelty.enable"
        };
    }, "Get available configuration options");
}

} // namespace nlm