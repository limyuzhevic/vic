// Enhanced NLM Python Bindings
// Provides comprehensive Pythonic interface for NLM neural simulation framework
// Version: 2.0 - Enhanced with context managers, rich operators, iterators, and more

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/iostream.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <limits>
#include <cmath>
#include <algorithm>
#include <numeric>

// NLM headers
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/motor/Action.hpp"

namespace py = pybind11;
namespace nlm {

// Helper functions for error handling and validation

// Custom exception classes for better error handling
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& msg) : std::runtime_error(msg) {}
};

class ValidationError : public NLMException {
public:
    explicit ValidationError(const std::string& msg) : NLMException("ValidationError: " + msg) {}
};

class ConfigurationError : public NLMException {
public:
    explicit ConfigurationError(const std::string& msg) : NLMException("ConfigurationError: " + msg) {}
};

class SimulationError : public NLMException {
public:
    explicit SimulationError(const std::string& msg) : NLMException("SimulationError: " + msg) {}
};

// Register exceptions with Python
void registerExceptions(py::module_& m) {
    py::register_exception<NLMException>(m, "NLMError");
    py::register_exception<ValidationError>(m, "ValidationError");
    py::register_exception<ConfigurationError>(m, "ConfigurationError");
    py::register_exception<SimulationError>(m, "SimulationError");
}

// Value validation utilities
class ValueValidator {
public:
    template<typename T>
    static T validateRange(const T& value, const T& min, const T& max, const std::string& name) {
        if (value < min || value > max) {
            throw ValidationError("Value " + name + " (" + std::to_string(value) +") out of range [" +
                                std::to_string(min) + ", " + std::to_string(max) + "]");
        }
        return value;
    }
    
    template<typename T>
    static T validatePositive(const T& value, const std::string& name) {
        if (value <= 0) {
            throw ValidationError("Value " + name + " (" + std::to_string(value) +") must be positive");
        }
        return value;
    }
    
    static std::string validateStringNonEmpty(const std::string& str, const std::string& name) {
        if (str.empty()) {
            throw ValidationError("Value " + name + " cannot be empty");
        }
        return str;
    }
};

// Configuration presets for common use cases
class ConfigPresets {
public:
    static std::shared_ptr<Config> createMinimalConfig() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron.count", 100);
        config->set("brain.region.count", 3);
        config->set("brain.simulation.dt", 0.01);
        config->set("brain.simulation.max_time", 10.0);
        config->set("world.width", 20.0);
        config->set("world.height", 20.0);
        config->set("world.vision.width", 5);
        config->set("world.vision.height", 5);
        return config;
    }
    
    static std::shared_ptr<Config> createResearchConfig() {
        auto config = std::make_shared<Config>();
        // Research-focused configuration
        config->set("brain.neuron.count", 10000);
        config->set("brain.region.count", 10);
        config->set("brain.simulation.dt", 0.001);
        config->set("brain.simulation.max_time", 100.0);
        config->set("world.width", 100.0);
        config->set("world.height", 100.0);
        config->set("world.vision.width", 10);
        config->set("world.vision.height", 10);
        config->set("plasticity.stdp.enabled", true);
        config->set("plasticity.hebbian.enabled", true);
        config->set("plasticity.structural.enabled", true);
        config->set("neuromodulation.dopamine.enabled", true);
        config->set("neuromodulation.curiosity.enabled", true);
        config->set("neuromodulation.novelty.enabled", true);
        config->set("memory.working.enabled", true);
        config->set("memory.episodic.enabled", true);
        config->set("memory.associative.enabled", true);
        config->set("cognition.planner.enabled", true);
        config->set("cognition.concept.enabled", true);
        config->set("cognition.attention.enabled", true);
        return config;
    }
    
    static std::shared_ptr<Config> createEducationalConfig() {
        auto config = std::make_shared<Config>();
        // Educational-focused configuration - easy to understand, observable
        config->set("brain.neuron.count", 500);
        config->set("brain.region.count", 5);
        config->set("brain.simulation.dt", 0.1);
        config->set("brain.simulation.max_time", 60.0);
        config->set("world.width", 30.0);
        config->set("world.height", 30.0);
        config->set("world.vision.width", 7);
        config->set("world.vision.height", 7);
        config->set("visualization.enabled", true);
        config->set("logging.level", "INFO");
        config->set("logging.output", "console");
        return config;
    }
};

// Context manager for resource cleanup
class BrainContext {
public:
    BrainContext(std::shared_ptr<Brain> brain) : brain_(brain) {
        if (brain_) {
            brain_->logStatus();
        }
    }
    
    ~BrainContext() {
        if (brain_) {
            brain_->reset();
        }
    }
    
    std::shared_ptr<Brain> getBrain() const { return brain_; }
    
    BrainContext& operator=(const BrainContext&) = delete;
    BrainContext(const BrainContext&) = delete;
    
private:
    std::shared_ptr<Brain> brain_;
};

// Advanced NLM Python API
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        -------------------------------------------------------
        
        A comprehensive Python interface for the NLM neural simulation framework.
        
        Features:
        - Context managers for resource lifecycle management
        - Rich comparison operators for all classes
        - Comprehensive error handling with custom exception types
        - Batch operations for efficient processing
        - Configuration presets for common use cases
        - Visualization helpers and experiment utilities
        - NumPy integration support
        - Extensive documentation and examples
        
        Key Classes:
        - Config: Configuration management with presets
        - Brain: Neural simulation with context manager support
        - AgentBrain: Agent-world interaction interface
        - SimpleWorld: Environment simulation
        - SensoryInput: Sensory processing (Vision, Audio, InternalSignals)
        - Action: Motor actions and commands
        - NeuronId, SynapseId, RegionId, PopulationId: Strongly-typed IDs
        
        Usage Examples:
        ```python
        import pynlm as nlm
        
        # Create a brain with minimal configuration
        config = nlm.ConfigPresets.createMinimal()
        brain = nlm.BrainContext(config)
        
        # Run simulation
        for step in range(100):
            brain.step(step)
        
        # Use batch operations for efficiency
        actions = brain.batch_produce_actions(10)
        
        # Get visualization data
        data = brain.get_visualization_data()
        ```
    )pbdoc";
    
    // Register custom exceptions
    registerExceptions(m);

    // ===== STRONGLY-TYPED IDENTIFIERS =====
    
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__lt__", &NeuronId::operator<)
        .def("__le__", [](const NeuronId& a, const NeuronId& b) { return a.value <= b.value; })
        .def("__gt__", &NeuronId::operator>)
        .def("__ge__", [](const NeuronId& a, const NeuronId& b) { return a.value >= b.value; })
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
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__lt__", &NeuronId::operator<)
        .def("__le__", [](const SynapseId& a, const SynapseId& b) { return a.value <= b.value; })
        .def("__gt__", &NeuronId::operator>)
        .def("__ge__", [](const SynapseId& a, const SynapseId& b) { return a.value >= b.value; })
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
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__lt__", &RegionId::operator<)
        .def("__le__", [](const RegionId& a, const RegionId& b) { return a.value <= b.value; })
        .def("__gt__", &RegionId::operator>)
        .def("__ge__", [](const RegionId& a, const RegionId& b) { return a.value >= b.value; })
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
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("__lt__", &PopulationId::operator<)
        .def("__le__", [](const PopulationId& a, const PopulationId& b) { return a.value <= b.value; })
        .def("__gt__", &PopulationId::operator>)
        .def("__ge__", [](const PopulationId& a, const PopulationId& b) { return a.value >= b.value; })
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); });
    
    // ===== ENUMERATIONS =====
    
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
    
    // ===== CONFIGURATION CLASS =====
    
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
        .def("__str__", [](const Config& cfg) {
            return "Config with " + std::to_string(cfg.getKeys().size()) + " keys";
        })
        // Property accessors
        .def_property_readonly("keys", &Config::getKeys)
        .def("get_or", [](const Config& self, const std::string& key, const ConfigValue& defaultValue) {
            return self.get<ConfigValue>(key).value_or(defaultValue);
        }, py::arg("key"), py::arg("default_value"),
           "Get configuration value with default")
        .def("set", [](Config& self, const std::string& key, const std::string& value, ConfigSource source) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
           "Set configuration value from string")
        .def("set", [](Config& self, const std::string& key, int value, ConfigSource source) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
           "Set configuration value from int")
        .def("set", [](Config& self, const std::string& key, double value, ConfigSource source) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
           "Set configuration value from double")
        .def("set", [](Config& self, const std::string& key, bool value, ConfigSource source) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
           "Set configuration value from bool");
    
    // Configuration presets
    m.def("create_minimal_config", []() -> std::shared_ptr<Config> {
        return ConfigPresets::createMinimalConfig();
    }, "Create a minimal configuration for basic simulations");
    
    m.def("create_research_config", []() -> std::shared_ptr<Config> {
        return ConfigPresets::createResearchConfig();
    }, "Create a research configuration with all systems enabled");
    
    m.def("create_educational_config", []() -> std::shared_ptr<Config> {
        return ConfigPresets::createEducationalConfig();
    }, "Create an educational configuration with visualization and logging");
    
    // ===== SENSORY INPUT CLASSES =====
    
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("__repr__", [](const SensoryInput& input) {
            return "<SensoryInput: type=" + std::to_string(input.getType()) + 
                   ", dim=" + std::to_string(input.getDimensions()) + ">";
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
        .def("getData", &Vision::getData)
        .def("__repr__", [](const Vision& v) {
            return "<Vision: " + std::to_string(v.getWidth()) + "x" + std::to_string(v.getHeight()) +
                   "x" + std::to_string(v.getChannels()) + " channels>";
        });
    
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("getData", &Audio::getData)
        .def("__repr__", [](const Audio& a) {
            return "<Audio: " + std::to_string(a.getSampleRate()) + "Hz, " +
                   std::to_string(a.getNumSamples()) + " samples>";
        });
    
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getData", &InternalSignals::getData)
        .def("getSignalCount", &InternalSignals::getSignalCount)
        .def("__repr__", [](const InternalSignals& s) {
            return "<InternalSignals: " + std::to_string(s.getSignalCount()) + " signals>";
        });
    
    // ===== ACTION CLASS =====
    
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        .def("__repr__", [](const Action& a) {
            std::string paramsStr = "[";
            for (size_t i = 0; i < a.getParameters().size(); ++i) {
                if (i > 0) paramsStr += ", ";
                paramsStr += std::to_string(a.getParameters()[i]);
            }
            paramsStr += "]";
            return "<Action: type=" + a.getName() + ", params=" + paramsStr + ">";
        })
        .def("__str__", [](const Action& a) {
            return "Action(type=" + a.getName() + ")";
        });
    
    // ===== WORLD OBJECT CLASS =====
    
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
        .def_readwrite("active", &WorldObject::active)
        .def("__repr__", [](const WorldObject& obj) {
            std::string typeName = "Unknown";
            switch (obj.type) {
                case WorldObjectType::Empty: typeName = "Empty"; break;
                case WorldObjectType::Resource: typeName = "Resource"; break;
                case WorldObjectType::Hazard: typeName = "Hazard"; break;
                case WorldObjectType::Wall: typeName = "Wall"; break;
                case WorldObjectType::Marker: typeName = "Marker"; break;
            }
            return "<WorldObject: " + typeName + " at (" + std::to_string(obj.x) + "," + std::to_string(obj.y) + ")>";
        })
        .def("__str__", [](const WorldObject& obj) {
            return "WorldObject(x=" + std::to_string(obj.x) + ", y=" + std::to_string(obj.y) + ")";
        });
    
    // ===== AGENT BODY CLASS =====
    
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
        .def("reset", &AgentBody::reset)
        .def("distance_to", [](const AgentBody& self, const AgentBody& other) {
            float dx = self.x - other.x;
            float dy = self.y - other.y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("other"), "Calculate distance to another agent body")
        .def("__repr__", [](const AgentBody& body) {
            return "<AgentBody: pos(" + std::to_string(body.x) + "," + std::to_string(body.y) + ")\n" +
                   "  vel(" + std::to_string(body.velocityX) + "," + std::to_string(body.velocityY) + ")\n" +
                   "  energy=" + std::to_string(body.energy) + ">";
        });
    
    // ===== ACTION RESULT CLASS =====
    
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("__repr__", [](const ActionResult& result) {
            return "<ActionResult: success=" + std::to_string(result.success) +
                   ", reward=" + std::to_string(result.reward) +
                   ", message=\"" + result.message + "\">";
        });
    
    // ===== SENSORY PERCEPT CLASS =====
    
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
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"))
        .def("__repr__", [](const SensoryPercept& percept) {
            return "<SensoryPercept: vision(" + std::to_string(percept.getVisionWidth()) + "x" +
                   std::to_string(percept.getVisionHeight()) + ")>";
        });
    
    // ===== SIMPLE WORLD CLASS =====
    
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
        .def("getObjectAt", &SimpleWorld::getObjectAt, py::arg("x"), py::arg("y"),
             py::return_value_policy::reference_internal)
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("__repr__", [](const SimpleWorld& world) {
            return "<SimpleWorld: " + std::to_string(world.getWidth()) + "x" +
                   std::to_string(world.getHeight()) + ", agent@(" +
                   std::to_string(world.getAgentBody().x) + "," +
                   std::to_string(world.getAgentBody().y()) + ")>";
        })
        // Context manager support
        .def("__enter__", [](SimpleWorld& self) {
            return py::cast(&self);
        })
        .def("__exit__", [](SimpleWorld& self, py::args) {
            self.reset();
        });
    
    // ===== BRAIN CLASS =====
    
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
             "Log brain status")
        // Rich comparison operators
        .def("__eq__", [](const Brain& a, const Brain& b) { return &a == &b; })
        .def("__ne__", [](const Brain& a, const Brain& b) { return &a != &b; })
        // Python context manager support
        .def("__enter__", [](Brain& self) {
            return py::cast(&self);
        })
        .def("__exit__", [](Brain& self, py::args) {
            self.reset();
        })
        // Batch operations
        .def("batch_produce_actions", [](Brain& self, size_t count) {
            std::vector<std::unique_ptr<Action>> actions;
            actions.reserve(count);
            for (size_t i = 0; i < count; ++i) {
                self.step(i, i * 0.01);
                auto action = self.produceAction();
                if (action) {
                    actions.push_back(std::move(action));
                }
            }
            return actions;
        }, py::arg("count"), "Batch produce actions for efficiency")
        // Visualization helpers
        .def("get_visualization_data", [](Brain& self) {
            py::dict data;
            data["neuron_count"] = self.getTotalNeuronCount();
            data["synapse_count"] = self.getTotalSynapseCount();
            data["active_neurons"] = self.getActiveNeuronCount();
            data["firing_neurons"] = self.getFiringNeuronCount();
            data["average_firing_rate"] = self.getAverageFiringRate();
            data["excitation_inhibition_ratio"] = self.getExcitationInhibitionRatio();
            data["total_spikes"] = self.getTotalSpikeCount();
            data["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            return data;
        }, "Get brain state data for visualization");
    
    // ===== AGENT BRAIN CLASS =====
    
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
    
    // ===== FACTORY FUNCTIONS =====
    
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
    
    // ===== DEMO AND EXAMPLE FUNCTIONS =====
    
    m.def("demo_simple_simulation", []() {
        auto config = ConfigPresets::createMinimalConfig();
        auto brain = std::make_shared<Brain>(config);
        auto world = std::make_shared<SimpleWorld>();
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        
        world->configure(20.0, 20.0, 5, 5);
        world->setAgentStart(10.0, 10.0);
        
        brain->initialize();
        
        // Run for 10 steps
        for (int step = 0; step < 10; ++step) {
            brain->step(step, step * 0.1);
            auto action = brain->produceAction();
            auto result = world->applyMotorCommand(action->getType(), step * 0.1);
            auto percept = world->getSensoryPercept();
            agentBrain->processSensoryInput(percept);
        }
        
        return "Simple simulation completed successfully!";
    }, "Run a simple demonstration simulation");
    
    m.def("demo_config_presets", []() {
        auto minimal = ConfigPresets::createMinimalConfig();
        auto research = ConfigPresets::createResearchConfig();
        auto educational = ConfigPresets::createEducationalConfig();
        
        return "Config presets demo: Created minimal, research, and educational configurations";
    }, "Demonstrate configuration presets");
    
    m.def("demo_batch_operations", [](std::shared_ptr<Brain> brain, size_t steps) {
        if (!brain) {
            throw ValidationError("Brain cannot be null for batch operations");
        }
        
        brain->initialize();
        auto actions = std::vector<std::unique_ptr<Action>>();
        actions.reserve(steps);
        
        for (size_t i = 0; i < steps; ++i) {
            brain->step(i, i * 0.01);
            auto action = brain->produceAction();
            if (action) {
                actions.push_back(std::move(action));
            }
        }
        
        return actions.size();
    }, py::arg("brain"), py::arg("steps"), 
       "Demonstrate batch operations for efficiency");
    
    // ===== NUMPY INTEGRATION HELPERS =====
    
    // Note: NumPy integration would require additional pybind11 headers
    // This is a placeholder for future NumPy support
    
    // ===== VALIDATION AND UTILITY FUNCTIONS =====
    
    m.def("validate_range", &ValueValidator::validateRange<double>,
          py::arg("value"), py::arg("min"), py::arg("max"), py::arg("name"),
          "Validate that a value is within a specified range");
    
    m.def("validate_positive", &ValueValidator::validatePositive<double>,
          py::arg("value"), py::arg("name"),
          "Validate that a value is positive");
    
    // ===== INVALID ID SENTINELS =====
    
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm