#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <chrono>
#include <sstream>
#include <iomanip>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

// NLM Python Version
#define NLM_PYTHON_VERSION "0.1.0"

namespace py = pybind11;
namespace nlm {

// Exception classes for better error handling
class NLMError : public std::runtime_error {
public:
    NLMError(const std::string& message) : std::runtime_error(message) {}
};

class ConfigError : public NLMError {
public:
    ConfigError(const std::string& message) : NLMError("Config error: " + message) {}
};

class BrainError : public NLMError {
public:
    BrainError(const std::string& message) : NLMError("Brain error: " + message) {}
};

class WorldError : public NLMError {
public:
    WorldError(const std::string& message) : NLMError("World error: " + message) {}
};

// Performance monitoring
class PerformanceMonitor {
public:
    struct Timings {
        double stepTime = 0.0;
        double totalTime = 0.0;
        size_t stepCount = 0;
        double avgStepTime() const { return stepCount > 0 ? totalTime / stepCount : 0.0; }
    };

    PerformanceMonitor() : startTime_(std::chrono::high_resolution_clock::now()) {}

    void stepStart() {
        stepStart_ = std::chrono::high_resolution_clock::now();
    }

    void stepEnd() {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - stepStart_);
        timings_.stepTime = duration.count() / 1000000.0; // Convert to seconds
        timings_.totalTime += timings_.stepTime;
        timings_.stepCount++;
    }

    const Timings& getTimings() const { return timings_; }

    void reset() {
        timings_ = Timings();
        startTime_ = std::chrono::high_resolution_clock::now();
    }

    double getUptime() const {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - startTime_);
        return duration.count() / 1000000.0;
    }

    std::string report() const {
        std::stringstream ss;
        ss << "Performance Report:";
        ss << "\n  Uptime: " << std::fixed << std::setprecision(3) << getUptime() << "s";
        ss << "\n  Steps executed: " << timings_.stepCount;
        ss << "\n  Average step time: " << std::fixed << std::setprecision(6) << timings_.avgStepTime() << "s";
        ss << "\n  Steps per second: " << (timings_.avgStepTime() > 0 ? 1.0 / timings_.avgStepTime() : 0.0);
        return ss.str();
    }

private:
    std::chrono::high_resolution_clock::time_point startTime_;
    std::chrono::high_resolution_clock::time_point stepStart_;
    Timings timings_;
};

// Event callback system
class EventCallbackManager {
public:
    using SpikeCallback = std::function<void(const NeuronId&, NeuronType, double)>;
    using LearningCallback = std::function<void(const SynapseId&, float, const std::string&)>;
    using StepCompleteCallback = std::function<void(SimulationStep, double, const Action*)>;
    using DevelopmentCallback = std::function<void(DevelopmentalStage, double)>;

    void addSpikeCallback(const SpikeCallback& callback) {
        spikeCallbacks_.push_back(callback);
    }

    void addLearningCallback(const LearningCallback& callback) {
        learningCallbacks_.push_back(callback);
    }

    void addStepCompleteCallback(const StepCompleteCallback& callback) {
        stepCompleteCallbacks_.push_back(callback);
    }

    void addDevelopmentCallback(const DevelopmentCallback& callback) {
        developmentCallbacks_.push_back(callback);
    }

    void clearCallbacks() {
        spikeCallbacks_.clear();
        learningCallbacks_.clear();
        stepCompleteCallbacks_.clear();
        developmentCallbacks_.clear();
    }

    const std::vector<SpikeCallback>& getSpikeCallbacks() const { return spikeCallbacks_; }
    const std::vector<LearningCallback>& getLearningCallbacks() const { return learningCallbacks_; }
    const std::vector<StepCompleteCallback>& getStepCompleteCallbacks() const { return stepCompleteCallbacks_; }
    const std::vector<DevelopmentCallback>& getDevelopmentCallbacks() const { return developmentCallbacks_; }

    static EventCallbackManager& getInstance() {
        static EventCallbackManager instance;
        return instance;
    }

private:
    EventCallbackManager() = default;
    std::vector<SpikeCallback> spikeCallbacks_;
    std::vector<LearningCallback> learningCallbacks_;
    std::vector<StepCompleteCallback> stepCompleteCallbacks_;
    std::vector<DevelopmentCallback> developmentCallbacks_;
};

// Debug and profiling utilities
class Debugger {
public:
    static std::string getMemoryUsage() {
        // Simplified memory reporting
        return "Memory monitoring not yet implemented (requires platform-specific code)";
    }

    static std::string getThreadInfo() {
        return "Thread monitoring not yet implemented";
    }

    static std::string getSystemInfo() {
        std::stringstream ss;
        ss << "NLM Python Bindings Version: " << NLM_PYTHON_VERSION;
        ss << "\nPlatform: " << (sizeof(void*) * 8) << "-bit";
        return ss.str();
    }
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A comprehensive Python interface for the NLM C++ neural simulation framework.
        Version: 0.1.0

        Features:
        - Brain simulation with spiking neurons
        - World interaction (SimpleWorld)
        - Agent perception and action
        - Configuration management
        - Performance monitoring
        - Event callbacks for custom simulation control
        - Debugging and profiling tools
        - JSON serialization support
        - Enhanced error handling with structured exceptions

        Classes:
        - Brain: Main neural simulation engine
        - Config: Configuration management
        - AgentBrain: Brain-to-world interface
        - SimpleWorld: 2D simulation environment
        - SensoryInput: Base class for sensory data
        - Vision: Visual sensory input
        - Audio: Audio sensory input
        - InternalSignals: Internal state signals
        - Action: Motor actions
        - WorldObject: Objects in the world
        - AgentBody: Agent physical state
        - SensoryPercept: Combined sensory perception

        Enumerations:
        - NeuronType: Various neuron types
        - SynapseType: Synapse connection types
        - DevelopmentalStage: Brain development stages
        - FiringState: Neuron firing states
        - ActionType: High-level action types
        - MotorCommand: Low-level motor commands
        - WorldObjectType: Types of world objects
    )pbdoc";

    m.attr("__version__") = NLM_PYTHON_VERSION;

    // Register exception classes
    py::register_exception<NLMError>(m, "NLMError");
    py::register_exception<ConfigError>(m, "ConfigError");
    py::register_exception<BrainError>(m, "BrainError");
    py::register_exception<WorldError>(m, "WorldError");

    // Neuron ID types
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

    // Enhanced Config class with set() method
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
        .def("get", &Config::get<int>, py::arg("key"),
             "Get integer configuration value")
        .def("get", &Config::get<double>, py::arg("key"),
             "Get float/double configuration value")
        .def("get", &Config::get<bool>, py::arg("key"),
             "Get boolean configuration value")
        .def("get", &Config::get<std::string>, py::arg("key"),
             "Get string configuration value")
        .def("getOr", &Config::getOr<int>, py::arg("key"), py::arg("default_value"),
             "Get integer value or default if key missing")
        .def("getOr", &Config::getOr<double>, py::arg("key"), py::arg("default_value"),
             "Get float value or default if key missing")
        .def("getOr", &Config::getOr<bool>, py::arg("key"), py::arg("default_value"),
             "Get boolean value or default if key missing")
        .def("getOr", &Config::getOr<std::string>, py::arg("key"), py::arg("default_value"),
             "Get string value or default if key missing")
        // NEW: set() method
        .def("set", (void (Config::*)(const std::string&, int, ConfigSource)) &Config::set,
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set integer configuration value")
        .def("set", (void (Config::*)(const std::string&, double, ConfigSource)) &Config::set,
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set float/double configuration value")
        .def("set", (void (Config::*)(const std::string&, bool, ConfigSource)) &Config::set,
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set boolean configuration value")
        .def("set", (void (Config::*)(const std::string&, const std::string&, ConfigSource)) &Config::set,
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set string configuration value")
        .def("remove", &Config::remove, py::arg("key"),
             "Remove a configuration key")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // JSON serialization support for Config
    m.def("configToJson", [](const Config& config) {
        // TODO: Implement proper JSON serialization
        return "{\"error\": \"JSON serialization not yet implemented\"}";
    }, py::arg("config"), "Convert configuration to JSON string");

    m.def("jsonToConfig", [](const std::string& json) {
        // TODO: Implement proper JSON deserialization
        auto config = std::make_shared<Config>();
        return config;
    }, py::arg("json"), "Create configuration from JSON string");

    // Enhanced performance monitoring
    py::class_<PerformanceMonitor>(m, "PerformanceMonitor", "Performance monitoring utility")
        .def(py::init<>())
        .def("stepStart", &PerformanceMonitor::stepStart,
             "Mark start of simulation step")
        .def("stepEnd", &PerformanceMonitor::stepEnd,
             "Mark end of simulation step")
        .def("getUptime", &PerformanceMonitor::getUptime,
             "Get monitor uptime in seconds")
        .def("getAverageStepTime", &PerformanceMonitor::Timings::avgStepTime,
             "Get average step time in seconds")
        .def("getStepCount", [](const PerformanceMonitor& monitor) { return monitor.getTimings().stepCount; },
             "Get number of steps executed")
        .def("reset", &PerformanceMonitor::reset,
             "Reset performance monitor")
        .def("report", &PerformanceMonitor::report,
             "Get performance report string");

    // Event callback system
    py::class_<EventCallbackManager>(m, "EventCallbackManager", "Event callback manager for simulation hooks")
        .def(py::init<>())
        .def("addSpikeCallback", &EventCallbackManager::addSpikeCallback,
             "Add spike event callback", py::arg("callback"))
        .def("addLearningCallback", &EventCallbackManager::addLearningCallback,
             "Add learning event callback", py::arg("callback"))
        .def("addStepCompleteCallback", &EventCallbackManager::addStepCompleteCallback,
             "Add step completion callback", py::arg("callback"))
        .def("addDevelopmentCallback", &EventCallbackManager::addDevelopmentCallback,
             "Add development stage callback", py::arg("callback"))
        .def("clearCallbacks", &EventCallbackManager::clearCallbacks,
             "Clear all event callbacks")
        .def_static("getInstance", &EventCallbackManager::getInstance,
                   "Get singleton instance of EventCallbackManager");

    // Debugging and profiling utilities
    py::class_<Debugger>(m, "Debugger", "Debugging and profiling utilities")
        .def_static("getSystemInfo", &Debugger::getSystemInfo,
                   "Get system information")
        .def_static("getMemoryUsage", &Debugger::getMemoryUsage,
                   "Get memory usage information")
        .def_static("getThreadInfo", &Debugger::getThreadInfo,
                   "Get thread information");

    // Enhanced SensoryInput classes
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

    // Enhanced Action class
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
        .def("__repr__", [](const Action& action) {
            std::stringstream ss;
            ss << "<Action type=" << static_cast<int>(action.getType()) << ">";
            return ss.str();
        });

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
            std::stringstream ss;
            ss << "<WorldObject type=" << static_cast<int>(obj.type) << " at (" << obj.x << "," << obj.y << ")>";
            return ss.str();
        });

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

    // Enhanced SimpleWorld class
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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("__repr__", [](const SimpleWorld& world) {
            return "<SimpleWorld size=" + std::to_string(world.getWidth()) + "x" + std::to_string(world.getHeight()) + ">";
        });

    // Enhanced Brain class
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
        .def("__repr__", [](const Brain& brain) {
            std::stringstream ss;
            ss << "<Brain neurons=" << brain.getTotalNeuronCount() 
               << " synapses=" << brain.getTotalSynapseCount() << ">";
            return ss.str();
        });

    // Enhanced AgentBrain class
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

    // Factory functions
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

    // Module constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Error handling helpers
    py::class_<py::error_already_set>(m, "ErrorAlreadySet")
        .def(py::init<>());
    }

} // namespace nlm