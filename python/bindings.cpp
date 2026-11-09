#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <typeinfo>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <mutex>
#include <optional>
#include <variant>
#include <algorithm>
#include <numeric>
#include <cmath>

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

// Forward declarations for helper classes
class PythonEventSystem;
class PythonVisualizationHelper;
class PythonSimulationLogger;
class PythonConfigPresets;

// Event types for the Python event system
enum class BrainEventType {
    Spike,
    Action,
    Reward,
    DevelopmentalStageChange,
    PlasticityUpdate,
    MemoryUpdate,
    CuriosityUpdate,
    NoveltyUpdate,
    PredictionUpdate,
    AttentionUpdate,
    ConceptFormation
};

// Event structure for Python bindings
struct BrainEvent {
    BrainEventType type;
    Timestamp timestamp;
    SimulationStep step;
    std::string description;
    
    template<typename T>
    T& getData() { return std::get<T>(data); }
    
    template<typename T>
    const T& getData() const { return std::get<T>(data); }
    
    std::variant<
        NeuronId,                           // Spike event
        Action,                            // Action event
        float,                             // Reward event
        DevelopmentalStage,                // Developmental stage change
        PlasticityFlags,                    // Plasticity update
        std::vector<float>,                // Memory update
        float,                             // Curiosity update
        float,                             // Novelty update
        float,                             // Prediction update
        float,                             // Attention update
        std::vector<float>                 // Concept formation
    > data;
    
    BrainEvent() : type(BrainEventType::Spike), timestamp(0.0), step(0), description(""), data() {}
    
    BrainEvent(BrainEventType t, Timestamp ts, SimulationStep s, std::string desc, auto&& d)
        : type(t), timestamp(ts), step(s), description(std::move(desc)), data(std::move(d)) {}
};

// Python wrapper for configuration presets
class PythonConfigPresets {
public:
    static std::shared_ptr<Config> developmental() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 500);
        config->set("brain.inhibitory_ratio", 0.2);
        config->set("brain.development_enabled", true);
        config->set("brain.stage_update_rate", 1000.0);
        config->set("brain.critical_period_duration", 1000.0);
        config->set("brain.maturation_threshold", 5000.0);
        config->set("world.max_energy", 100.0);
        config->set("world.energy_decay_rate", 0.01);
        config->set("world.resource_value", 10.0);
        config->set("world.hazard_damage", 5.0);
        return config;
    }
    
    static std::shared_ptr<Config> adult() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 1000);
        config->set("brain.inhibitory_ratio", 0.3);
        config->set("brain.development_enabled", false);
        config->set("brain.stdp.learning_rate", 0.01);
        config->set("brain.stdp.depression_strength", 0.5);
        config->set("brain.stdp.potentiation_strength", 1.0);
        config->set("brain.hebbian.learning_rate", 0.005);
        config->set("world.max_energy", 50.0);
        config->set("world.energy_decay_rate", 0.02);
        config->set("world.resource_value", 15.0);
        config->set("world.hazard_damage", 8.0);
        return config;
    }
    
    static std::shared_ptr<Config> learning() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 750);
        config->set("brain.inhibitory_ratio", 0.25);
        config->set("brain.development_enabled", true);
        config->set("brain.stage_update_rate", 500.0);
        config->set("brain.critical_period_duration", 800.0);
        config->set("brain.reward_modulation_enabled", true);
        config->set("brain.curiosity_enabled", true);
        config->set("brain.novelty_enabled", true);
        config->set("world.max_energy", 75.0);
        config->set("world.energy_decay_rate", 0.015);
        config->set("world.resource_value", 12.0);
        config->set("world.hazard_damage", 6.0);
        return config;
    }
    
    static std::shared_ptr<Config> custom(std::map<std::string, std::variant<int, double, bool, std::string>> settings) {
        auto config = std::make_shared<Config>();
        for (const auto& [key, value] : settings) {
            config->set(key, value);
        }
        return config;
    }
};

// Python wrapper for enhanced enum support with string representations
class PythonEnumSupport {
public:
    // Enhanced NeuronType with string conversion
    static std::string neuronTypeToString(NeuronType type) {
        switch (type) {
            case NeuronType::Excitatory: return "Excitatory";
            case NeuronType::Inhibitory: return "Inhibitory";
            case NeuronType::Modulatory: return "Modulatory";
            case NeuronType::Sensory: return "Sensory";
            case NeuronType::Motor: return "Motor";
            case NeuronType::Internal: return "Internal";
            default: return "Unknown";
        }
    }
    
    static NeuronType neuronTypeFromString(const std::string& str) {
        std::string lower = str;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        if (lower == "excitatory") return NeuronType::Excitatory;
        if (lower == "inhibitory") return NeuronType::Inhibitory;
        if (lower == "modulatory") return NeuronType::Modulatory;
        if (lower == "sensory") return NeuronType::Sensory;
        if (lower == "motor") return NeuronType::Motor;
        if (lower == "internal") return NeuronType::Internal;
        throw std::invalid_argument("Invalid NeuronType string: " + str);
    }
    
    // Enhanced SynapseType with string conversion
    static std::string synapseTypeToString(SynapseType type) {
        switch (type) {
            case SynapseType::Excitatory: return "Excitatory";
            case SynapseType::Inhibitory: return "Inhibitory";
            case SynapseType::Modulatory: return "Modulatory";
            case SynapseType::Electrical: return "Electrical";
            case SynapseType::GapJunction: return "GapJunction";
            default: return "Unknown";
        }
    }
    
    static SynapseType synapseTypeFromString(const std::string& str) {
        std::string lower = str;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        if (lower == "excitatory") return SynapseType::Excitatory;
        if (lower == "inhibitory") return SynapseType::Inhibitory;
        if (lower == "modulatory") return SynapseType::Modulatory;
        if (lower == "electrical") return SynapseType::Electrical;
        if (lower == "gapjunction") return SynapseType::GapJunction;
        throw std::invalid_argument("Invalid SynapseType string: " + str);
    }
    
    // Enhanced DevelopmentalStage with string conversion
    static std::string developmentalStageToString(DevelopmentalStage stage) {
        switch (stage) {
            case DevelopmentalStage::Initial: return "Initial";
            case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
            case DevelopmentalStage::Maturation: return "Maturation";
            case DevelopmentalStage::Adult: return "Adult";
            case DevelopmentalStage::Aging: return "Aging";
            default: return "Unknown";
        }
    }
    
    static DevelopmentalStage developmentalStageFromString(const std::string& str) {
        std::string lower = str;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        if (lower == "initial") return DevelopmentalStage::Initial;
        if (lower == "criticalperiod" || lower == "critical_period" || lower == "critical period") return DevelopmentalStage::CriticalPeriod;
        if (lower == "maturation") return DevelopmentalStage::Maturation;
        if (lower == "adult") return DevelopmentalStage::Adult;
        if (lower == "aging") return DevelopmentalStage::Aging;
        throw std::invalid_argument("Invalid DevelopmentalStage string: " + str);
    }
    
    // Enhanced ActionType with string conversion
    static std::string actionTypeToString(ActionType type) {
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
    
    static ActionType actionTypeFromString(const std::string& str) {
        std::string lower = str;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        if (lower == "moveforward") return ActionType::MoveForward;
        if (lower == "movebackward") return ActionType::MoveBackward;
        if (lower == "moveleft") return ActionType::MoveLeft;
        if (lower == "move right") return ActionType::MoveRight;
        if (lower == "turnleft") return ActionType::TurnLeft;
        if (lower == "turnright") return ActionType::TurnRight;
        if (lower == "look") return ActionType::Look;
        if (lower == "lookup") return ActionType::LookUp;
        if (lower == "lookdown") return ActionType::LookDown;
        if (lower == "interact") return ActionType::Interact;
        if (lower == "eat") return ActionType::Eat;
        if (lower == "drink") return ActionType::Drink;
        if (lower == "rest") return ActionType::Rest;
        if (lower == "wait") return ActionType::Wait;
        if (lower == "custom") return ActionType::Custom;
        throw std::invalid_argument("Invalid ActionType string: " + str);
    }
    
    // Enhanced MotorCommand with string conversion
    static std::string motorCommandToString(MotorCommand cmd) {
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
    
    static MotorCommand motorCommandFromString(const std::string& str) {
        std::string lower = str;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        if (lower == "moveforward") return MotorCommand::MoveForward;
        if (lower == "movebackward") return MotorCommand::MoveBackward;
        if (lower == "turnleft") return MotorCommand::TurnLeft;
        if (lower == "turnright") return MotorCommand::TurnRight;
        if (lower == "lookleft") return MotorCommand::LookLeft;
        if (lower == "lookright") return MotorCommand::LookRight;
        if (lower == "interact") return MotorCommand::Interact;
        if (lower == "wait") return MotorCommand::Wait;
        throw std::invalid_argument("Invalid MotorCommand string: " + str);
    }
};

// Event system with callbacks
class PythonEventSystem {
public:
    using EventCallback = std::function<void(const BrainEvent&)>;
    
    void registerCallback(BrainEventType type, EventCallback callback) {
        callbacks_[type].push_back(callback);
    }
    
    void unregisterCallback(BrainEventType type, EventCallback callback) {
        auto it = std::find(callbacks_[type].begin(), callbacks_[type].end(), callback);
        if (it != callbacks_[type].end()) {
            callbacks_[type].erase(it);
        }
    }
    
    void triggerEvent(const BrainEvent& event) {
        for (const auto& callback : callbacks_[event.type]) {
            try {
                callback(event);
            } catch (const std::exception& e) {
                std::cerr << "Error in event callback: " << e.what() << std::endl;
            }
        }
    }
    
    void filterEvents(std::vector<BrainEvent>& events, std::function<bool(const BrainEvent&)> predicate) {
        events.erase(std::remove_if(events.begin(), events.end(), 
                                  [&predicate](const BrainEvent& e) { return !predicate(e); }),
                    events.end());
    }
    
    void clearEvents() {
        events_.clear();
    }
    
    const std::vector<BrainEvent>& getEvents() const { return events_; }
    
    void addEvent(const BrainEvent& event) {
        events_.push_back(event);
    }
    
private:
    std::map<BrainEventType, std::vector<EventCallback>> callbacks_;
    std::vector<BrainEvent> events_;
    std::mutex mutex_;
};

// Python wrapper for visualization and simulation utilities
class PythonVisualizationHelper {
public:
    static std::string generateSimulationStats(const Brain& brain, const SimpleWorld& world) {
        std::stringstream ss;
        ss << "=== Simulation Statistics ===\n";
        ss << "Brain Statistics:\n";
        ss << "  Total neurons: " << brain.getTotalNeuronCount() << "\n";
        ss << "  Total synapses: " << brain.getTotalSynapseCount() << "\n";
        ss << "  Active neurons: " << brain.getActiveNeuronCount() << "\n";
        ss << "  Firing neurons: " << brain.getFiringNeuronCount() << "\n";
        ss << "  Average firing rate: " << brain.getAverageFiringRate() << " Hz\n";
        ss << "  Excitation/Inhibition ratio: " << brain.getExcitationInhibitionRatio() << "\n";
        ss << "  Total spike count: " << brain.getTotalSpikeCount() << "\n";
        ss << "  Developmental stage: " << PythonEnumSupport::developmentalStageToString(brain.getDevelopmentalStage()) << "\n";
        ss << "  Learned preference: " << brain.getLearnedPreference() << "\n";
        ss << "\nWorld Statistics:\n";
        ss << "  World size: " << world.getWidth() << " x " << world.getHeight() << "\n";
        ss << "  Simulation time: " << world.getSimulationTime() << " s\n";
        ss << "  Max energy: " << world.getMaxEnergy() << "\n";
        ss << "  Energy decay rate: " << world.getEnergyDecayRate() << "\n";
        ss << "  Random seed: " << world.getRandomSeed() << "\n";
        return ss.str();
    }
    
    static std::string generateBrainActivitySummary(const Brain& brain, size_t steps = 100) {
        std::stringstream ss;
        ss << "=== Brain Activity Summary (last " << steps << " steps) ===\n";
        ss << "Neural statistics:\n";
        ss << "  Average firing rate: " << brain.getAverageFiringRate() << " Hz\n";
        ss << "  Activity variance: N/A (requires historical data)\n";
        ss << "  Excitatory/Inhibitory balance: " << brain.getExcitationInhibitionRatio() << "\n";
        ss << "  Total spikes in history: " << brain.getTotalSpikeCount() << "\n";
        ss << "\nDevelopment:\n";
        ss << "  Current stage: " << PythonEnumSupport::developmentalStageToString(brain.getDevelopmentalStage()) << "\n";
        ss << "  Learned preference: " << brain.getLearnedPreference() << "\n";
        return ss.str();
    }
    
    static std::string generateComparisonReport(const std::vector<std::shared_ptr<Brain>>& brains,
                                               const std::vector<std::string>& labels) {
        if (brains.empty() || brains.size() != labels.size()) {
            throw std::invalid_argument("Brains and labels must have same non-zero length");
        }
        
        std::stringstream ss;
        ss << "=== Brain Comparison Report ===\n\n";
        for (size_t i = 0; i < brains.size(); ++i) {
            ss << "Brain " << labels[i] << ":\n";
            ss << "  Total neurons: " << brains[i]->getTotalNeuronCount() << "\n";
            ss << "  Total synapses: " << brains[i]->getTotalSynapseCount() << "\n";
            ss << "  Active neurons: " << brains[i]->getActiveNeuronCount() << "\n";
            ss << "  Firing rate: " << brains[i]->getAverageFiringRate() << " Hz\n";
            ss << "  Spike count: " << brains[i]->getTotalSpikeCount() << "\n";
            ss << "  Stage: " << PythonEnumSupport::developmentalStageToString(brains[i]->getDevelopmentalStage()) << "\n";
            ss << "  Preference: " << brains[i]->getLearnedPreference() << "\n\n";
        }
        return ss.str();
    }
};

// Python wrapper for logging and progress tracking
class PythonSimulationLogger {
public:
    enum class LogLevel { DEBUG, INFO, WARNING, ERROR };
    
    struct LogEntry {
        LogLevel level;
        Timestamp timestamp;
        std::string message;
        SimulationStep step;
        
        std::string toString() const {
            std::string levelStr;
            switch (level) {
                case LogLevel::DEBUG: levelStr = "DEBUG"; break;
                case LogLevel::INFO: levelStr = "INFO"; break;
                case LogLevel::WARNING: levelStr = "WARNING"; break;
                case LogLevel::ERROR: levelStr = "ERROR"; break;
            }
            std::stringstream ss;
            ss << "[" << levelStr << "] " << timestamp << " [step " << step << "]: " << message;
            return ss.str();
        }
    };
    
    void setLogLevel(LogLevel level) { logLevel_ = level; }
    
    void log(LogLevel level, const std::string& message, SimulationStep step = 0) {
        if (level < logLevel_) return;
        
        std::lock_guard<std::mutex> lock(logMutex_);
        LogEntry entry;
        entry.level = level;
        entry.timestamp = currentTime_;
        entry.message = message;
        entry.step = step;
        
        logs_.push_back(entry);
        
        // Also print to stdout for immediate visibility
        std::cout << entry.toString() << std::endl;
    }
    
    void debug(const std::string& message, SimulationStep step = 0) {
        log(LogLevel::DEBUG, message, step);
    }
    
    void info(const std::string& message, SimulationStep step = 0) {
        log(LogLevel::INFO, message, step);
    }
    
    void warning(const std::string& message, SimulationStep step = 0) {
        log(LogLevel::WARNING, message, step);
    }
    
    void error(const std::string& message, SimulationStep step = 0) {
        log(LogLevel::ERROR, message, step);
    }
    
    std::vector<LogEntry> getLogs() const { 
        std::lock_guard<std::mutex> lock(logMutex_);
        return logs_; 
    }
    
    void clearLogs() {
        std::lock_guard<std::mutex> lock(logMutex_);
        logs_.clear();
    }
    
    std::string getLogSummary() const {
        std::stringstream ss;
        ss << "=== Log Summary ===\n";
        ss << "Total entries: " << logs_.size() << "\n";
        size_t debugCount = 0, infoCount = 0, warningCount = 0, errorCount = 0;
        for (const auto& entry : logs_) {
            switch (entry.level) {
                case LogLevel::DEBUG: ++debugCount; break;
                case LogLevel::INFO: ++infoCount; break;
                case LogLevel::WARNING: ++warningCount; break;
                case LogLevel::ERROR: ++errorCount; break;
            }
        }
        ss << "DEBUG: " << debugCount << "\n";
        ss << "INFO: " << infoCount << "\n";
        ss << "WARNING: " << warningCount << "\n";
        ss << "ERROR: " << errorCount << "\n";
        return ss.str();
    }
    
    void updateTime(Timestamp time) { currentTime_ = time; }
    
private:
    std::vector<LogEntry> logs_;
    LogLevel logLevel_ = LogLevel::INFO;
    Timestamp currentTime_ = 0.0;
    mutable std::mutex logMutex_;
};

// Context manager wrapper for automatic resource cleanup
class BrainContextManager {
public:
    BrainContextManager(std::shared_ptr<Brain> brain) : brain_(brain) {}
    
    void __enter__() {
        return;
    }
    
    void __exit__(int /*exc_type*/, const std::string& /*exc_value*/, int /*exc_traceback*/) {
        cleanup();
    }
    
    void cleanup() {
        if (brain_) {
            brain_->reset();
            brain_.reset();
        }
    }
    
    std::shared_ptr<Brain> getBrain() const { return brain_; }
    
    bool isCleanedUp() const { return !brain_; }
    
private:
    std::shared_ptr<Brain> brain_;
};

class AgentBrainContextManager {
public:
    AgentBrainContextManager(std::shared_ptr<AgentBrain> agentBrain) : agentBrain_(agentBrain) {}
    
    void __enter__() {
        return;
    }
    
    void __exit__(int /*exc_type*/, const std::string& /*exc_value*/, int /*exc_traceback*/) {
        cleanup();
    }
    
    void cleanup() {
        if (agentBrain_) {
            agentBrain_->reset();
            agentBrain_.reset();
        }
    }
    
    std::shared_ptr<AgentBrain> getAgentBrain() const { return agentBrain_; }
    
    bool isCleanedUp() const { return !agentBrain_; }
    
private:
    std::shared_ptr<AgentBrain> agentBrain_;
};

class SimpleWorldContextManager {
public:
    SimpleWorldContextManager(std::shared_ptr<SimpleWorld> world) : world_(world) {}
    
    void __enter__() {
        return;
    }
    
    void __exit__(int /*exc_type*/, const std::string& /*exc_value*/, int /*exc_traceback*/) {
        cleanup();
    }
    
    void cleanup() {
        if (world_) {
            world_->reset();
            world_.reset();
        }
    }
    
    std::shared_ptr<SimpleWorld> getWorld() const { return world_; }
    
    bool isCleanedUp() const { return !world_; }
    
private:
    std::shared_ptr<SimpleWorld> world_;
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Enhanced Python Bindings
        ---------------------------------------------
        An enhanced Python binding for the NLM C++ neural simulation framework.
        Provides comprehensive high-level factory methods, context managers,
        Pythonic APIs, event systems, visualization helpers, and more.
    )pbdoc";

    // Register exception
    py::register_exception<std::runtime_error>(m, "RuntimeError");

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

    // Enhanced NeuronType enum with string conversion
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    // NeuronType helper functions
    m.def("neuron_type_to_string", &PythonEnumSupport::neuronTypeToString, py::arg("type"),
          "Convert NeuronType to human-readable string");
    m.def("neuron_type_from_string", &PythonEnumSupport::neuronTypeFromString, py::arg("str"),
          "Convert string to NeuronType");

    // Enhanced SynapseType enum with string conversion
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    // SynapseType helper functions
    m.def("synapse_type_to_string", &PythonEnumSupport::synapseTypeToString, py::arg("type"),
          "Convert SynapseType to human-readable string");
    m.def("synapse_type_from_string", &PythonEnumSupport::synapseTypeFromString, py::arg("str"),
          "Convert string to SynapseType");

    // Enhanced DevelopmentalStage enum with string conversion
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    // DevelopmentalStage helper functions
    m.def("developmental_stage_to_string", &PythonEnumSupport::developmentalStageToString, py::arg("stage"),
          "Convert DevelopmentalStage to human-readable string");
    m.def("developmental_stage_from_string", &PythonEnumSupport::developmentalStageFromString, py::arg("str"),
          "Convert string to DevelopmentalStage");

    // Enhanced FiringState enum with string conversion
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    // Enhanced ActionType enum with string conversion
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

    // ActionType helper functions
    m.def("action_type_to_string", &PythonEnumSupport::actionTypeToString, py::arg("type"),
          "Convert ActionType to human-readable string");
    m.def("action_type_from_string", &PythonEnumSupport::actionTypeFromString, py::arg("str"),
          "Convert string to ActionType");

    // Enhanced MotorCommand enum with string conversion
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

    // MotorCommand helper functions
    m.def("motor_command_to_string", &PythonEnumSupport::motorCommandToString, py::arg("cmd"),
          "Convert MotorCommand to human-readable string");
    m.def("motor_command_from_string", &PythonEnumSupport::motorCommandFromString, py::arg("str"),
          "Convert string to MotorCommand");

    // Enhanced WorldObjectType enum with string conversion
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // WorldObjectType helper functions
    m.def("world_object_type_to_string", [](WorldObjectType type) {
        switch (type) {
            case WorldObjectType::Empty: return std::string("Empty");
            case WorldObjectType::Resource: return std::string("Resource");
            case WorldObjectType::Hazard: return std::string("Hazard");
            case WorldObjectType::Wall: return std::string("Wall");
            case WorldObjectType::Marker: return std::string("Marker");
            default: return std::string("Unknown");
        }
    }, py::arg("type"), "Convert WorldObjectType to human-readable string");

    // Enhanced Config class with Pythonic API
    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Pythonic configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("load_from_file", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("load_from_args", [](Config& self, const std::vector<std::string>& args) {
            std::vector<char*> argv;
            argv.reserve(args.size());
            for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
            return self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
        }, py::arg("args"),
             "Load configuration from command line arguments")
        .def("save_to_file", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get_keys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        // Pythonic property access
        .def("get", [](Config& self, const std::string& key) {
            return self.getOr<void>(key, {});
        }, py::arg("key"), "Get configuration value (returns None if not found)")
        .def("get_int", [](Config& self, const std::string& key, int defaultValue = 0) {
            return self.getOr<int>(key, defaultValue);
        }, py::arg("key"), py::arg("default_value") = 0, "Get integer configuration value")
        .def("get_float", [](Config& self, const std::string& key, double defaultValue = 0.0) {
            return self.getOr<double>(key, defaultValue);
        }, py::arg("key"), py::arg("default_value") = 0.0, "Get float configuration value")
        .def("get_string", [](Config& self, const std::string& key, const std::string& defaultValue = "") {
            return self.getOr<std::string>(key, defaultValue);
        }, py::arg("key"), py::arg("default_value") = "", "Get string configuration value")
        .def("get_bool", [](Config& self, const std::string& key, bool defaultValue = false) {
            return self.getOr<bool>(key, defaultValue);
        }, py::arg("key"), py::arg("default_value") = false, "Get boolean configuration value")
        .def("set_int", [](Config& self, const std::string& key, int value, ConfigSource source = ConfigSource::Runtime) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime, "Set integer configuration value")
        .def("set_float", [](Config& self, const std::string& key, double value, ConfigSource source = ConfigSource::Runtime) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime, "Set float configuration value")
        .def("set_string", [](Config& self, const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime, "Set string configuration value")
        .def("set_bool", [](Config& self, const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime, "Set boolean configuration value");

    // Config presets
    m.def("create_developmental_config", []() -> std::shared_ptr<Config> {
        return PythonConfigPresets::developmental();
    }, "Create developmental configuration preset");
    
    m.def("create_adult_config", []() -> std::shared_ptr<Config> {
        return PythonConfigPresets::adult();
    }, "Create adult configuration preset");
    
    m.def("create_learning_config", []() -> std::shared_ptr<Config> {
        return PythonConfigPresets::learning();
    }, "Create learning configuration preset");
    
    m.def("create_custom_config", [](std::map<std::string, std::variant<int, double, bool, std::string>> settings) {
        return PythonConfigPresets::custom(settings);
    }, py::arg("settings"), "Create custom configuration with provided settings");

    // SensoryInput base class with Pythonic API
    py::class_<SensoryInput, std::shared_ptr<SensoryInput>>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("get_type", &SensoryInput::getType, "Get the type of sensory input")
        .def("get_data", &SensoryInput::getData, "Get the raw data as a vector")
        .def("get_dimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("get_timestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("set_timestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("__repr__", [](const SensoryInput& input) {
            return "<" + std::string(input.getType()) + " data at " + std::to_string(input.getTimestamp()) + ">";
        });

    // Vision sensory input with Pythonic API
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("set_data", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("get_width", &Vision::getWidth)
        .def("get_height", &Vision::getHeight)
        .def("get_channels", &Vision::getChannels)
        .def("to_array", [](Vision& self) {
            return self.getData();
        }, "Get vision data as Python list");

    // Audio sensory input with Pythonic API
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("set_data", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("set_sample_rate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("get_sample_rate", &Audio::getSampleRate)
        .def("get_num_samples", &Audio::getNumSamples);

    // InternalSignals sensory input with Pythonic API
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("add_signal", &InternalSignals::addSignal, py::arg("value"))
        .def("clear_signals", &InternalSignals::clearSignals)
        .def("get_signals", [](InternalSignals& self) {
            return self.getData();
        }, "Get all signals as Python list");

    // Action with enhanced Pythonic API
    py::class_<Action>(m, "Action", R"pbdoc(Pythonic action representation)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("get_type", &Action::getType)
        .def("set_type", &Action::setType, py::arg("type"))
        .def("get_parameters", &Action::getParameters)
        .def("set_parameters", &Action::setParameters, py::arg("params"))
        .def("get_name", &Action::getName)
        .def("clone", &Action::clone)
        .def("__repr__", [](const Action& action) {
            std::stringstream ss;
            ss << "<Action: " << PythonEnumSupport::actionTypeToString(action.getType())
               << " params: [";
            const auto& params = action.getParameters();
            for (size_t i = 0; i < params.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << params[i];
            }
            ss << "]>";
            return ss.str();
        });

    // WorldObject with enhanced Pythonic API
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
        .def("distance_to", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to position")
        .def("is_near", [](const WorldObject& self, float x, float y, float threshold) {
            float dx = self.x - x;
            float dy = self.y - y;
            float distance = std::sqrt(dx * dx + dy * dy);
            return distance <= threshold;
        }, py::arg("x"), py::arg("y"), py::arg("threshold"), "Check if object is within threshold distance");

    // AgentBody with enhanced Pythonic API
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Enhanced agent body state)pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x)
        .def_readwrite("y", &AgentBody::y)
        .def_readwrite("orientation", &AgentBody::orientation)
        .def_readwrite("velocity_x", &AgentBody::velocityX)
        .def_readwrite("velocity_y", &AgentBody::velocityY)
        .def_readwrite("angular_velocity", &AgentBody::angularVelocity)
        .def_readwrite("energy", &AgentBody::energy)
        .def_readwrite("health", &AgentBody::health)
        .def_readwrite("age", &AgentBody::age)
        .def_readwrite("is_moving", &AgentBody::isMoving)
        .def_readwrite("is_turning", &AgentBody::isTurning)
        .def_readwrite("last_action_time", &AgentBody::lastActionTime)
        .def("reset", &AgentBody::reset)
        .def("distance_to", [](const AgentBody& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to position")
        .def("position", [](const AgentBody& self) {
            return std::make_pair(self.x, self.y);
        }, "Get position as tuple")
        .def("set_position", [](AgentBody& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"), "Set position");

    // ActionResult with enhanced Pythonic API
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("__repr__", [](const ActionResult& result) {
            std::stringstream ss;
            ss << "<ActionResult: reward=" << result.reward << ", success=" << (result.success ? "true" : "false")
               << ", message=\"" << result.message << "\">";
            return ss.str();
        });

    // SensoryPercept with enhanced Pythonic API
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("get_vision", &SensoryPercept::getVision)
        .def("set_vision", &SensoryPercept::setVision, py::arg("vision"))
        .def("get_vision_width", &SensoryPercept::getVisionWidth)
        .def("get_vision_height", &SensoryPercept::getVisionHeight)
        .def("get_touch", &SensoryPercept::getTouch)
        .def("set_touch", &SensoryPercept::setTouch, py::arg("touch"))
        .def("get_internal", &SensoryPercept::getInternal)
        .def("set_internal", &SensoryPercept::setInternal, py::arg("internal"))
        .def("get_proprioception", &SensoryPercept::getProprioception)
        .def("set_proprioception", &SensoryPercept::setProprioception, py::arg("proprioception"))
        .def("get_audio", &SensoryPercept::getAudio)
        .def("set_audio", &SensoryPercept::setAudio, py::arg("audio"))
        .def("get_all_signals", &SensoryPercept::getAllSignals)
        .def("get_timestamp", &SensoryPercept::getTimestamp)
        .def("set_timestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"))
        .def("to_dict", [](const SensoryPercept& percept) {
            py::dict result;
            result["vision"] = percept.getVision();
            result["touch"] = percept.getTouch();
            result["internal"] = percept.getInternal();
            result["proprioception"] = percept.getProprioception();
            result["audio"] = percept.getAudio();
            result["timestamp"] = percept.getTimestamp();
            return result;
        }, "Convert percept to Python dictionary");

    // SimpleWorld with enhanced Pythonic API and context manager support
    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(Enhanced simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"))
        .def("reset", &SimpleWorld::reset)
        .def("set_agent_start", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &SimpleWorld::update, py::arg("timestep"))
        .def("apply_motor_command", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"))
        .def("get_sensory_percept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("get_agent_body", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        .def("add_object", &SimpleWorld::addObject, py::arg("obj"))
        .def("remove_object", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"))
        .def("is_valid_position", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"))
        .def("get_object_at", &SimpleWorld::getObjectAt, py::return_value_policy::reference_internal,
             py::arg("x"), py::arg("y"))
        .def("get_width", &SimpleWorld::getWidth)
        .def("get_height", &SimpleWorld::getHeight)
        .def("get_max_energy", &SimpleWorld::getMaxEnergy)
        .def("set_max_energy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("get_energy_decay_rate", &SimpleWorld::getEnergyDecayRate)
        .def("set_energy_decay_rate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("get_simulation_time", &SimpleWorld::getSimulationTime)
        .def("set_random_seed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("get_random_seed", &SimpleWorld::getRandomSeed)
        .def("__repr__", [](const SimpleWorld& world) {
            return "<SimpleWorld: " + std::to_string(world.getWidth()) + "x" + std::to_string(world.getHeight()) +
                   " at time " + std::to_string(world.getSimulationTime()) + ">";
        })
        // Context manager support
        .def("__enter__", [](std::shared_ptr<SimpleWorld> self) {
            return self;
        })
        .def("__exit__", [](std::shared_ptr<SimpleWorld> self, int /*exc_type*/, const std::string& /*exc_value*/, int /*exc_traceback*/) {
            self->reset();
        });

    // Brain with enhanced Pythonic API and context manager support
    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(Enhanced central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receive_sensory_input", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("inject_current", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("inject_current_to_neurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produce_action", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("add_region", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        // getRegion is const/non-const overloaded; bind the mutable one.
        .def("get_region", static_cast<NeuralRegion* (Brain::*)(RegionId)>(&Brain::getRegion),
             py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("get_region_count", &Brain::getRegionCount,
             "Get the number of regions")
        .def("get_region_ids", &Brain::getRegionIds,
             "Get all region IDs")
        .def("get_regions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
        .def("get_total_neuron_count", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("get_total_synapse_count", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("get_active_neuron_count", &Brain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("get_firing_neuron_count", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("get_average_firing_rate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("get_excitation_inhibition_ratio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("get_total_spike_count", &Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("get_developmental_stage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("set_developmental_stage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage (accepts string or enum)")
        .def("get_config", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("log_status", &Brain::logStatus,
             "Log brain status")
        .def("__repr__", [](const Brain& brain) {
            std::stringstream ss;
            ss << "<Brain: " << brain.getTotalNeuronCount() << " neurons, " << brain.getTotalSynapseCount() << " synapses, "
               << PythonEnumSupport::developmentalStageToString(brain.getDevelopmentalStage()) << " stage>";
            return ss.str();
        })
        // Context manager support
        .def("__enter__", [](std::shared_ptr<Brain> self) {
            return self;
        })
        .def("__exit__", [](std::shared_ptr<Brain> self, int /*exc_type*/, const std::string& /*exc_value*/, int /*exc_traceback*/) {
            self->reset();
        });

    // AgentBrain with enhanced Pythonic API and context manager support
    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(Enhanced agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("get_sensory_input_size", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("get_motor_output_size", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("process_sensory_input", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decode_motor_command", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("apply_reward_modulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("update_development", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        .def("get_developmental_stage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("get_neuromodulation_level", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("get_curiosity_level", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("get_novelty_level", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("get_prediction_error", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("get_brain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enable_reward_modulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"))
        .def("enable_structural_plasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enable_development", &AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enable_curiosity", &AgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("is_reward_modulation_enabled", &AgentBrain::isRewardModulationEnabled)
        .def("is_structural_plasticity_enabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("is_development_enabled", &AgentBrain::isDevelopmentEnabled)
        .def("is_curiosity_enabled", &AgentBrain::isCuriosityEnabled)
        // Context manager support
        .def("__enter__", [](std::shared_ptr<AgentBrain> self) {
            return self;
        })
        .def("__exit__", [](std::shared_ptr<AgentBrain> self, int /*exc_type*/, const std::string& /*exc_value*/, int /*exc_traceback*/) {
            self->reset();
        });

    // High-level factory methods and convenience functions
    m.def("create_default_config", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");
    
    m.def("create_brain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration options");
    
    m.def("create_agent_system", [](std::shared_ptr<Config> config, std::shared_ptr<SimpleWorld> world) {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        agentBrain->initialize(*world);
        return std::make_tuple(brain, agentBrain, world);
    }, py::arg("config"), py::arg("world"),
       "Create complete agent system (brain + agent brain + world)");
    
    m.def("create_simple_simulation", []() {
        auto config = PythonConfigPresets::learning();
        auto world = std::make_shared<SimpleWorld>();
        world->configure(100.0, 100.0, 16, 16);
        world->setAgentStart(50.0, 50.0);
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        agentBrain->initialize(*world);
        return std::make_tuple(config, brain, agentBrain, world);
    }, "Create a complete working simulation environment");
    
    m.def("create_visualization_helper", []() {
        return std::make_shared<PythonVisualizationHelper>();
    }, "Create visualization helper for plotting brain data");
    
    m.def("create_event_system", []() {
        return std::make_shared<PythonEventSystem>();
    }, "Create event system for brain callbacks");
    
    m.def("create_simple_world", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");
    
    m.def("create_agent_brain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Event system integration
    py::class_<PythonEventSystem>(m, "EventSystem", R"pbdoc(Python event system for brain callback management)pbdoc")
        .def("register_callback", &PythonEventSystem::registerCallback,
             py::arg("event_type"), py::arg("callback"),
             "Register a callback for specific event type")
        .def("unregister_callback", &PythonEventSystem::unregisterCallback,
             py::arg("event_type"), py::arg("callback"),
             "Unregister a specific callback")
        .def("trigger_event", &PythonEventSystem::triggerEvent,
             py::arg("event"),
             "Trigger an event (calls all registered callbacks)")
        .def("filter_events", &PythonEventSystem::filterEvents,
             py::arg("events"), py::arg("predicate"),
             "Filter events based on predicate function")
        .def("clear_events", &PythonEventSystem::clearEvents,
             "Clear all events")
        .def("get_events", &PythonEventSystem::getEvents,
             "Get all events")
        .def("add_event", &PythonEventSystem::addEvent,
             py::arg("event"),
             "Add an event to the system");

    // Visualization helper integration
    py::class_<PythonVisualizationHelper>(m, "VisualizationHelper", R"pbdoc(Helper class for brain visualization and plotting)pbdoc")
        .def("generate_simulation_stats", &PythonVisualizationHelper::generateSimulationStats,
             py::arg("brain"), py::arg("world"),
             "Generate comprehensive simulation statistics")
        .def("generate_brain_activity_summary", &PythonVisualizationHelper::generateBrainActivitySummary,
             py::arg("brain"), py::arg("steps") = 100,
             "Generate brain activity summary")
        .def("generate_comparison_report", &PythonVisualizationHelper::generateComparisonReport,
             py::arg("brains"), py::arg("labels"),
             "Generate comparison report for multiple brains");

    // Simulation logger integration
    py::class_<PythonSimulationLogger>(m, "SimulationLogger", R"pbdoc(Simulation logger with different log levels)pbdoc")
        .def(py::init<>())
        .def("set_log_level", &PythonSimulationLogger::setLogLevel,
             py::arg("level"),
             "Set the minimum log level")
        .def("debug", &PythonSimulationLogger::debug,
             py::arg("message"), py::arg("step") = 0,
             "Log debug message")
        .def("info", &PythonSimulationLogger::info,
             py::arg("message"), py::arg("step") = 0,
             "Log info message")
        .def("warning", &PythonSimulationLogger::warning,
             py::arg("message"), py::arg("step") = 0,
             "Log warning message")
        .def("error", &PythonSimulationLogger::error,
             py::arg("message"), py::arg("step") = 0,
             "Log error message")
        .def("get_logs", &PythonSimulationLogger::getLogs,
             "Get all log entries")
        .def("clear_logs", &PythonSimulationLogger::clearLogs,
             "Clear all logs")
        .def("get_log_summary", &PythonSimulationLogger::getLogSummary,
             "Get log summary statistics")
        .def("update_time", &PythonSimulationLogger::updateTime,
             py::arg("time"),
             "Update current simulation time");

    // BrainEvent type for events
    py::class_<BrainEvent>(m, "BrainEvent", R"pbdoc(Event for brain simulation callbacks)pbdoc")
        .def(py::init<>())
        .def_readwrite("type", &BrainEvent::type)
        .def_readwrite("timestamp", &BrainEvent::timestamp)
        .def_readwrite("step", &BrainEvent::step)
        .def_readwrite("description", &BrainEvent::description)
        .def("get_data", &BrainEvent::getData<void>, py::arg("data_type"),
             "Get event data (type-dependent)")
        .def("set_data", [](BrainEvent& self, auto&& data) {
            self.data = std::move(data);
        }, py::arg("data"),
             "Set event data (type-dependent)");

    // DevelopmentStageType for string conversion
    m.def("development_stage_to_string", &PythonEnumSupport::developmentalStageToString, py::arg("stage"),
          "Convert DevelopmentalStage to human-readable string");
    m.def("development_stage_from_string", &PythonEnumSupport::developmentalStageFromString, py::arg("str"),
          "Convert string to DevelopmentalStage");

    // Config validation helpers
    m.def("validate_config", [](const Config& config) {
        std::vector<std::string> errors;
        
        // Check for essential keys
        if (!config.has("brain.neuron_count")) {
            errors.push_back("Missing brain.neuron_count");
        }
        if (!config.has("brain.inhibitory_ratio")) {
            errors.push_back("Missing brain.inhibitory_ratio");
        }
        if (!config.has("world.max_energy")) {
            errors.push_back("Missing world.max_energy");
        }
        
        return errors;
    }, py::arg("config"),
       "Validate configuration and return list of errors");

    // Simulation utility functions
    m.def("run_simulation_step", [](Brain& brain, AgentBrain& agentBrain, SimpleWorld& world, 
                                 const SensoryPercept& percept, SimulationStep step) {
        agentBrain.processSensoryInput(percept);
        brain.step(step);
        agentBrain.updateDevelopment(0.1); // Fixed timestep for now
        world.update(0.1); // Fixed timestep for now
        return brain.produceAction();
    }, py::arg("brain"), py::arg("agent_brain"), py::arg("world"), 
       py::arg("percept"), py::arg("step"),
       "Run a complete simulation step");

    m.def("compare_simulations", [](const std::vector<std::tuple<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>>>& simulations,
                                   const std::vector<std::string>& labels) {
        std::vector<std::shared_ptr<Brain>> brains;
        std::vector<std::shared_ptr<SimpleWorld>> worlds;
        for (const auto& sim : simulations) {
            brains.push_back(std::get<0>(sim));
            worlds.push_back(std::get<1>(sim));
        }
        return PythonVisualizationHelper::generateComparisonReport(brains, labels);
    }, py::arg("simulations"), py::arg("labels"),
       "Compare multiple simulations and generate report");

    // Global event system instance (singleton pattern)
    m.def("get_global_event_system", []() {
        static std::shared_ptr<PythonEventSystem> eventSystem = std::make_shared<PythonEventSystem>();
        return eventSystem;
    }, "Get the global event system instance");

    // Global logger instance
    m.def("get_simulation_logger", []() {
        static std::shared_ptr<PythonSimulationLogger> logger = std::make_shared<PythonSimulationLogger>();
        return logger;
    }, "Get the global simulation logger instance");

    // Utility functions
    m.def("generate_simulation_report", [](Brain& brain, SimpleWorld& world) {
        return PythonVisualizationHelper::generateSimulationStats(brain, world);
    }, py::arg("brain"), py::arg("world"),
       "Generate comprehensive simulation report");

    // Invalid ID constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Register exception for Brain events
    py::register_exception<BrainEvent>(m, "BrainEvent", PyExc_RuntimeError);

    // Module docstring
    m.def("version", []() { return "2.0.0"; }, "Get the version of the enhanced NLM Python bindings");
}

} // namespace nlm
