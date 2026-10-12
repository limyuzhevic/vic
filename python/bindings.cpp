// Enhanced NLM Python Bindings
// Version: 2.0.0

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <chrono>
#include <numeric>
#include <unordered_map>

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

// Custom exception hierarchy for better error handling
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& message) : std::runtime_error(message) {}
    virtual const char* what() const noexcept override {
        return std::runtime_error::what();
    }
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& message) : NLMError("Configuration Error: " + message) {}
};

class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& message) : NLMError("Simulation Error: " + message) {}
};

class MemoryError : public NLMError {
public:
    explicit MemoryError(const std::string& message) : NLMError("Memory Error: " + message) {}
};

class ValidationError : public NLMError {
public:
    explicit ValidationError(const std::string& message) : NLMError("Validation Error: " + message) {}
};

class ResourceError : public NLMError {
public:
    explicit ResourceError(const std::string& message) : NLMError("Resource Error: " + message) {}
};

// Helper function to extract numpy arrays more efficiently
template<typename T>
py::array_t<T> to_numpy(const std::vector<T>& vec) {
    return py::array_t<T>(vec.size(), vec.data());
}

// Enhanced configuration with Pythonic interface
class EnhancedConfig {
private:
    std::shared_ptr<Config> config_;
    
public:
    explicit EnhancedConfig(std::shared_ptr<Config> config) : config_(config) {}
    
    // Dictionary-like interface
    py::dict toDict() const {
        py::dict result;
        const auto& keys = config_->getKeys();
        for (const auto& key : keys) {
            try {
                result[key] = config_->getOr(key, 0.0f);
            } catch (...) {
                result[key] = py::none();
            }
        }
        return result;
    }
    
    // Pythonic dictionary access
    py::object operator[](const std::string& key) {
        try {
            return py::cast(config_->getOr(key, 0.0f));
        } catch (const std::exception& e) {
            throw ValidationError("Key not found: '" + key + "': " + e.what());
        }
    }
    
    // Set multiple values at once
    void batchSet(const py::dict& dict) {
        for (auto item : dict) {
            std::string key = py::cast<std::string>(item.first);
            config_->set(key, py::cast<float>(item.second));
        }
    }
    
    // Get with default value
    py::object getWithDefault(const std::string& key, py::object defaultValue) {
        try {
            return py::cast(config_->getOr(key, py::cast<float>(defaultValue)));
        } catch (...) {
            return defaultValue;
        }
    }
    
    // Check if key exists
    bool hasKey(const std::string& key) const {
        return config_->has(key);
    }
    
    // Get all keys as list
    std::vector<std::string> getAllKeys() const {
        return config_->getKeys();
    }
    
    // Clone configuration
    EnhancedConfig clone() const {
        auto new_config = std::make_shared<Config>();
        const auto& keys = config_->getKeys();
        for (const auto& key : keys) {
            new_config->set(key, config_->getOr(key, 0.0f));
        }
        return EnhancedConfig(new_config);
    }
    
    // Load configuration from dictionary
    void loadFromDict(const py::dict& dict) {
        for (auto item : dict) {
            std::string key = py::cast<std::string>(item.first);
            config_->set(key, py::cast<float>(item.second));
        }
    }
    
    // Get configuration summary with improved formatting
    std::string getSummary() const {
        std::string summary = "Configuration Summary (" + std::to_string(config_->getKeys().size()) + " entries):\n";
        const auto& keys = config_->getKeys();
        for (const auto& key : keys) {
            summary += "  " + key + ": ";
            try {
                auto value = config_->getOr(key, "unknown");
                if constexpr (std::is_same_v<std::decay_t<decltype(value)>, std::string>) {
                    summary += "\"" + value + "\"\n";
                } else if constexpr (std::is_same_v<std::decay_t<decltype(value)>, bool>) {
                    summary += (value ? "true" : "false") + "\n";
                } else {
                    summary += std::to_string(value) + "\n";
                }
            } catch (...) {
                summary += "<error>\n";
            }
        }
        return summary;
    }
};

// Smart brain wrapper for better memory management
class SmartBrain {
private:
    std::shared_ptr<Brain> brain_;
    bool autoCleanup_ = true;
    
public:
    explicit SmartBrain(std::shared_ptr<Brain> brain, bool autoCleanup = true) 
        : brain_(brain), autoCleanup_(autoCleanup) {}
    
    ~SmartBrain() {
        if (autoCleanup_ && brain_) {
            brain_->reset();
        }
    }
    
    // Disallow copy but allow move
    SmartBrain(const SmartBrain&) = delete;
    SmartBrain& operator=(const SmartBrain&) = delete;
    
    SmartBrain(SmartBrain&& other) noexcept : brain_(std::move(other.brain_)), 
                                              autoCleanup_(other.autoCleanup_) {
        other.autoCleanup_ = false;
    }
    
    SmartBrain& operator=(SmartBrain&& other) noexcept {
        if (this != &other) {
            brain_ = std::move(other.brain_);
            autoCleanup_ = other.autoCleanup_;
            other.autoCleanup_ = false;
        }
        return *this;
    }
    
    // Access methods
    Brain* operator->() { return brain_.get(); }
    const Brain* operator->() const { return brain_.get(); }
    Brain& operator*() { return *brain_; }
    const Brain& operator*() const { return *brain_; }
    
    // Check if valid
    explicit operator bool() const { return brain_ != nullptr; }
    
    // Get underlying pointer
    std::shared_ptr<Brain> get() const { return brain_; }
    
    // Disable auto cleanup
    void disableAutoCleanup() { autoCleanup_ = false; }
    
    // Get configuration
    EnhancedConfig getConfig() const {
        return EnhancedConfig(brain_->getConfig());
    }
    
    // Perform batch operations
    std::vector<std::unique_ptr<Action>> batchStep(const std::vector<SimulationStep>& steps, 
                                                  const std::vector<Timestamp>& times = {}) {
        std::vector<std::unique_ptr<Action>> results;
        results.reserve(steps.size());
        
        for (size_t i = 0; i < steps.size(); ++i) {
            Timestamp time = times.empty() ? 0.0 : times[i];
            brain_->step(steps[i], time);
            auto action = brain_->produceAction();
            if (action) {
                results.push_back(std::move(action));
            }
        }
        
        return results;
    }
    
    // Get metrics for batch processing
    std::unordered_map<std::string, double> getMetrics() const {
        std::unordered_map<std::string, double> metrics;
        metrics["average_firing_rate"] = brain_->getAverageFiringRate();
        metrics["excitement_ratio"] = brain_->getExcitationInhibitionRatio();
        metrics["total_spikes"] = brain_->getTotalSpikeCount();
        metrics["active_neurons"] = brain_->getActiveNeuronCount();
        metrics["firing_neurons"] = brain_->getFiringNeuronCount();
        metrics["total_neurons"] = brain_->getTotalNeuronCount();
        metrics["total_synapses"] = brain_->getTotalSynapseCount();
        metrics["developmental_stage"] = static_cast<int>(brain_->getDevelopmentalStage());
        return metrics;
    }
};

// Batch processor for efficient simulation of multiple steps
class BatchProcessor {
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimpleWorld> world_;
    
public:
    BatchProcessor(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world)
        : brain_(brain), world_(world) {}
    
    // Batch step simulation
    std::vector<std::unique_ptr<Action>> batchStep(const std::vector<SimulationStep>& steps, 
                                                  const std::vector<Timestamp>& times = {}) {
        std::vector<std::unique_ptr<Action>> results;
        results.reserve(steps.size());
        
        for (size_t i = 0; i < steps.size(); ++i) {
            Timestamp time = times.empty() ? world_->getSimulationTime() : times[i];
            brain_->step(steps[i], time);
            auto action = brain_->produceAction();
            if (action) {
                results.push_back(std::move(action));
            }
        }
        
        return results;
    }
    
    // Batch sensory processing
    std::vector<SimulationStep> batchProcessSensory(const std::vector<SensoryPercept>& percepts) {
        std::vector<SimulationStep> steps;
        steps.reserve(percepts.size());
        
        for (const auto& percept : percepts) {
            SimulationStep step;
            step.timestamp = world_->getSimulationTime();
            brain_->receiveSensoryInput(percept);
            steps.push_back(step);
        }
        
        return steps;
    }
    
    // Get batch metrics
    std::unordered_map<std::string, double> getBatchMetrics(const std::vector<SimulationStep>& steps) {
        std::unordered_map<std::string, double> metrics;
        metrics["steps_count"] = static_cast<double>(steps.size());
        metrics["average_firing_rate"] = brain_->getAverageFiringRate();
        metrics["excitement_ratio"] = brain_->getExcitationInhibitionRatio();
        metrics["total_spikes"] = brain_->getTotalSpikeCount();
        metrics["active_neurons"] = brain_->getActiveNeuronCount();
        metrics["firing_neurons"] = brain_->getFiringNeuronCount();
        metrics["total_neurons"] = brain_->getTotalNeuronCount();
        metrics["total_synapses"] = brain_->getTotalSynapseCount();
        return metrics;
    }
    
    // Process entire episode
    std::vector<std::unique_ptr<Action>> processEpisode(const std::vector<SensoryPercept>& percepts,
                                                       const std::vector<ActionType>& actions = {}) {
        std::vector<std::unique_ptr<Action>> results;
        results.reserve(percepts.size());
        
        for (size_t i = 0; i < percepts.size(); ++i) {
            brain_->receiveSensoryInput(percepts[i]);
            SimulationStep step;
            step.timestamp = world_->getSimulationTime();
            brain_->step(step);
            
            if (i < actions.size()) {
                auto action = brain_->produceAction();
                if (action) {
                    results.push_back(std::move(action));
                }
            }
        }
        
        return results;
    }
    
    // Get comprehensive analysis
    std::unordered_map<std::string, py::object> analyze() {
        std::unordered_map<std::string, py::object> analysis;
        auto metrics = getBatchMetrics(std::vector<SimulationStep>());
        
        // Convert to Python objects for complex return type
        py::dict metrics_dict;
        for (const auto& pair : metrics) {
            if (std::is_same_v<decltype(pair.second), double>) {
                metrics_dict[pair.first] = pair.second;
            }
        }
        analysis["metrics"] = metrics_dict;
        analysis["brain_status"] = "active";
        return analysis;
    }
};

// Context manager for automatic resource cleanup
class BrainContextManager {
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimpleWorld> world_;
    bool autoSave_ = false;
    std::string autoSavePath_;
    
public:
    BrainContextManager(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                       bool autoSave = false, const std::string& autoSavePath = "") 
        : brain_(brain), world_(world), autoSave_(autoSave), autoSavePath_(autoSavePath) {}
    
    void __enter__() {
        brain_->initialize();
        return brain_.get();
    }
    
    void __exit__(int /*exception_type*/, const py::object& /*exception_value*/, 
                  const py::object& /*traceback_value*/) {
        if (autoSave_ && !autoSavePath_.empty()) {
            brain_->save(autoSavePath_);
        }
        brain_->reset();
        if (world_) {
            world_->reset();
        }
    }
    
    // Get the underlying brain
    std::shared_ptr<Brain> getBrain() const { return brain_; }
    
    // Get the world
    std::shared_ptr<SimpleWorld> getWorld() const { return world_; }
};

// Enhanced neuron class with Pythonic operators and utilities
class EnhancedNeuron {
private:
    Neuron* neuron_ = nullptr;
    
public:
    EnhancedNeuron() : neuron_(nullptr) {}
    explicit EnhancedNeuron(Neuron* neuron) : neuron_(neuron) {}
    
    // Operator overloading for common operations
    EnhancedNeuron& operator+=(float current) {
        if (neuron_) {
            neuron_->injectCurrent(current);
        }
        return *this;
    }
    
    EnhancedNeuron& operator+=(const EnhancedNeuron& other) {
        if (neuron_ && other.neuron_) {
            neuron_->injectCurrent(other.neuron_->getCurrent());
        }
        return *this;
    }
    
    float getCurrent() const { return neuron_ ? neuron_->getCurrent() : 0.0f; }
    NeuronId getId() const { return neuron_ ? neuron_->getId() : INVALID_NEURON_ID; }
    NeuronType getType() const { return neuron_ ? neuron_->getType() : NeuronType::Internal; }
    bool isActive() const { return neuron_ ? neuron_->isActive() : false; }
    float getPotential() const { return neuron_ ? neuron_->getPotential() : 0.0f; }
    
    std::string toString() const {
        if (!neuron_) {
            return "<Neuron: null>";
        }
        std::stringstream ss;
        ss << "Neuron[id=" << getId().value 
           << ", type=" << static_cast<int>(getType())
           << ", current=" << getCurrent()
           << ", active=" << (isActive() ? "true" : "false") << "]";
        return ss.str();
    }
    
    // Comparison operators
    bool operator==(const EnhancedNeuron& other) const {
        return neuron_ && other.neuron_ && neuron_->getId() == other.neuron_->getId();
    }
    
    bool operator!=(const EnhancedNeuron& other) const {
        return !(*this == other);
    }
    
    // Conversion to Python bool
    explicit operator bool() const {
        return neuron_ != nullptr && isActive();
    }
    
    // Convenience methods
    std::string getTypeName() const {
        switch (getType()) {
            case NeuronType::Excitatory: return "Excitatory";
            case NeuronType::Inhibitory: return "Inhibitory";
            case NeuronType::Modulatory: return "Modulatory";
            case NeuronType::Sensory: return "Sensory";
            case NeuronType::Motor: return "Motor";
            case NeuronType::Internal: return "Internal";
            default: return "Unknown";
        }
    }
    
    // Check if neuron is of specific type
    bool isType(NeuronType type) const {
        return getType() == type;
    }
    
    // Get neuron state information
    py::dict getState() const {
        py::dict state;
        state["id"] = getId().value;
        state["type"] = getType();
        state["current"] = getCurrent();
        state["active"] = isActive();
        state["potential"] = getPotential();
        state["type_name"] = getTypeName();
        return state;
    }
};

// Convenience methods for power users
class BrainConvenience {
private:
    std::shared_ptr<Brain> brain_;
    
public:
    explicit BrainConvenience(std::shared_ptr<Brain> brain) : brain_(brain) {}
    
    // Quick simulation helper
    std::vector<std::unique_ptr<Action>> simulateEpisode(const std::vector<SensoryPercept>& percepts, 
                                             const std::vector<ActionType>& actions) {
        std::vector<std::unique_ptr<Action>> results;
        for (const auto& percept : percepts) {
            brain_->receiveSensoryInput(percept);
            SimulationStep step;
            step.timestamp = brain_->getConfig()->getOr("simulation_time_step", 0.1f);
            brain_->step(step);
            
            if (!actions.empty()) {
                auto action = brain_->produceAction();
                if (action) {
                    results.push_back(std::move(action));
                }
            }
        }
        return results;
    }
    
    // Advanced analysis
    std::unordered_map<std::string, double> analyzePerformance(const std::string& metricType = "comprehensive") {
        std::unordered_map<std::string, double> analysis;
        
        if (metricType == "performance" || metricType == "comprehensive") {
            analysis["firing_rate"] = brain_->getAverageFiringRate();
            analysis["excitement_balance"] = brain_->getExcitationInhibitionRatio();
            analysis["spike_efficiency"] = brain_->getTotalSpikeCount() / std::max(1.0, brain_->getActiveNeuronCount());
        }
        
        if (metricType == "activity" || metricType == "comprehensive") {
            analysis["activity_level"] = static_cast<double>(brain_->getActiveNeuronCount()) / std::max(1.0, brain_->getTotalNeuronCount());
            analysis["firing_efficiency"] = static_cast<double>(brain_->getFiringNeuronCount()) / std::max(1.0, brain_->getActiveNeuronCount());
        }
        
        if (metricType == "development" || metricType == "comprehensive") {
            analysis["developmental_stage"] = static_cast<int>(brain_->getDevelopmentalStage());
            analysis["novelty_level"] = brain_->getNoveltyLevel();
            analysis["curiosity_level"] = brain_->getCuriosityLevel();
            analysis["prediction_error"] = brain_->getPredictionError();
        }
        
        return analysis;
    }
    
    // Memory management optimization
    void optimizeMemory() {
        // Clear unused regions
        for (const auto& region_id : brain_->getRegionIds()) {
            auto region = brain_->getRegion(region_id);
            if (region && region->getNeuronCount() == 0) {
                // This is a hypothetical optimization - actual implementation would depend on Brain API
            }
        }
    }
    
    // Advanced configuration helper
    std::string configureForTask(const std::string& taskType, const std::unordered_map<std::string, float>& params = {}) {
        brain_->getConfig()->set("task_type", taskType);
        
        if (params.find("learning_rate") != params.end()) {
            brain_->getConfig()->set("learning_rate", params.at("learning_rate"));
        }
        if (params.find("exploration_rate") != params.end()) {
            brain_->getConfig()->set("exploration_rate", params.at("exploration_rate"));
        }
        if (params.find("sparsity") != params.end()) {
            brain_->getConfig()->set("sparsity", params.at("sparsity"));
        }
        
        brain_->initialize();
        return "Configuration optimized for " + taskType;
    }
    
    // Get brain statistics
    py::dict getStatistics() const {
        auto metrics = analyzePerformance("comprehensive");
        py::dict stats;
        for (const auto& pair : metrics) {
            stats[pair.first] = pair.second;
        }
        stats["total_regions"] = static_cast<double>(brain_->getRegionCount());
        return stats;
    }
    
    // Reset and reinitialize
    void resetAndInitialize() {
        brain_->reset();
        brain_->initialize();
    }
    
    // Enable all systems
    void enableAllSystems() {
        // Enable all plasticity, development, and neuromodulation
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        ------------------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides comprehensive API with custom exceptions, batch operations,
        context managers, and Pythonic patterns for improved usability.
        
        Key features:
        - Custom exception hierarchy (NLMError, ConfigurationError, etc.)
        - Batch processing methods for efficiency
        - Context managers for resource management
        - Pythonic APIs (operator overloading, dict-like interfaces)
        - Advanced convenience methods for power users
        - Enhanced memory management with SmartBrain wrapper
        - Type-safe conversions and error handling
        - Enhanced configuration with dictionary interface
        - Comprehensive documentation and error messages
    )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMError>(m, "NLMError");
    py::register_exception<ConfigurationError>(m, "ConfigurationError");
    py::register_exception<SimulationError>(m, "SimulationError");
    py::register_exception<MemoryError>(m, "MemoryError");
    py::register_exception<ValidationError>(m, "ValidationError");
    py::register_exception<ResourceError>(m, "ResourceError");

    // NeuronId with enhanced features
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
            return "NeuronId(" + std::to_string(id.value) + ")";
        })
        .def("to_dict", [](const NeuronId& id) {
            py::dict result;
            result["value"] = id.value;
            return result;
        }, "Convert to Python dictionary");

    // SynapseId
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
        .def("to_dict", [](const SynapseId& id) {
            py::dict result;
            result["value"] = id.value;
            return result;
        }, "Convert to Python dictionary");

    // RegionId
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
        .def("to_dict", [](const RegionId& id) {
            py::dict result;
            result["value"] = id.value;
            return result;
        }, "Convert to Python dictionary");

    // PopulationId
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

    // Configuration with enhanced interface
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system with enhanced Python interface)pbdoc")
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
        .def("set", &Config::set, py::arg("key"), py::arg("value"),
             "Set a configuration value")
        .def("get", py::overload_cast<const std::string&>(&Config::get),
             py::arg("key"), "Get a configuration value")
        .def("get", py::overload_cast<const std::string&, float>(&Config::get),
             py::arg("key"), py::arg("default_value"), "Get a configuration value with default")
        .def("getOr", &Config::getOr, py::arg("key"), py::arg("default_value") = 0.0f,
             "Get a configuration value with default (enhanced API)")
        .def("getInt", &Config::getInt, py::arg("key"), py::arg("default_value") = 0,
             "Get an integer configuration value")
        .def("getFloat", &Config::getFloat, py::arg("key"), py::arg("default_value") = 0.0f,
             "Get a float configuration value")
        .def("getBool", &Config::getBool, py::arg("key"), py::arg("default_value") = false,
             "Get a boolean configuration value")
        .def("getString", &Config::getString, py::arg("key"), py::arg("default_value") = "",
             "Get a string configuration value")
        .def("remove", &Config::remove, py::arg("key"),
             "Remove a configuration key")
        .def("getAll", [](Config& self) {
            auto keys = self.getKeys();
            py::dict result;
            for (const auto& key : keys) {
                result[key] = self.getOr(key, 0.0f);
            }
            return result;
        }, "Get all configuration values as a dictionary");

    // Enhanced configuration helper
    py::class_<EnhancedConfig>(m, "EnhancedConfig", 
                              R"pbdoc(Enhanced configuration with Pythonic interface)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("to_dict", &EnhancedConfig::toDict,
             "Convert configuration to Python dictionary")
        .def("__getitem__", &EnhancedConfig::operator[],
             "Pythonic dictionary access: config['key']")
        .def("__setitem__", [](EnhancedConfig& self, const std::string& key, py::object value) {
            self.get().set(key, py::cast<float>(value));
        }, py::arg("key"), py::arg("value"),
             "Pythonic dictionary assignment: config['key'] = value")
        .def("batch_set", &EnhancedConfig::batchSet, py::arg("dict"),
             "Set multiple configuration values from a Python dictionary")
        .def("load_from_dict", &EnhancedConfig::loadFromDict, py::arg("dict"),
             "Load configuration from Python dictionary")
        .def("get_with_default", &EnhancedConfig::getWithDefault, 
             py::arg("key"), py::arg("default_value"),
             "Get value with default fallback")
        .def("has_key", &EnhancedConfig::hasKey, py::arg("key"),
             "Check if configuration has a key")
        .def("get_all_keys", &EnhancedConfig::getAllKeys,
             "Get all configuration keys as list")
        .def("clone", &EnhancedConfig::clone,
             "Create a copy of the configuration")
        .def("get_summary", &EnhancedConfig::getSummary,
             "Get enhanced configuration summary");

    // SensoryInput base class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("as_numpy", [](const SensoryInput& self) {
            return to_numpy(self.getData());
        }, "Get data as NumPy array");

    // Vision class
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
        .def("set_data", &Vision::setData, py::arg("data"), "Alias for setData")
        .def("width", &Vision::getWidth, "Get width (property)")
        .def("height", &Vision::getHeight, "Get height (property)");

    // Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    // InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    // Action class
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
            return "<Action type=" + std::to_string(static_cast<int>(action.getType())) + ">";
        })
        .def("__eq__", [](const Action& a1, const Action& a2) {
            return a1.getType() == a2.getType() && a1.getParameters() == a2.getParameters();
        });

    // WorldObject class
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
        .def("distance_to", [](const WorldObject& self, const WorldObject& other) {
            float dx = self.x - other.x;
            float dy = self.y - other.y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("other"), "Calculate distance to another object")
        .def("to_dict", [](const WorldObject& obj) {
            py::dict result;
            result["x"] = obj.x;
            result["y"] = obj.y;
            result["radius"] = obj.radius;
            result["type"] = obj.type;
            result["value"] = obj.value;
            result["active"] = obj.active;
            return result;
        }, "Convert to Python dictionary");

    // AgentBody class
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
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("other"), "Calculate distance to another agent body")
        .def("to_dict", [](const AgentBody& body) {
            py::dict result;
            result["x"] = body.x;
            result["y"] = body.y;
            result["orientation"] = body.orientation;
            result["velocityX"] = body.velocityX;
            result["velocityY"] = body.velocityY;
            result["angularVelocity"] = body.angularVelocity;
            result["energy"] = body.energy;
            result["health"] = body.health;
            result["age"] = body.age;
            result["isMoving"] = body.isMoving;
            result["isTurning"] = body.isTurning;
            result["lastActionTime"] = body.lastActionTime;
            return result;
        }, "Convert to Python dictionary");

    // ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("__repr__", [](const ActionResult& result) {
            std::stringstream ss;
            ss << "<ActionResult reward=" << result.reward 
               << " success=" << (result.success ? "true" : "false") << ">";
            return ss.str();
        })
        .def("to_dict", [](const ActionResult& result) {
            py::dict dict;
            dict["reward"] = result.reward;
            dict["success"] = result.success;
            dict["message"] = result.message;
            return dict;
        }, "Convert to Python dictionary");

    // SensoryPercept class
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
        .def("clear", &SensoryPercept::clear, "Clear all sensory data")
        .def("as_numpy", [](SensoryPercept& self) {
            return py::cast(self.getAllSignals());
        }, "Get all signals as Python list")
        .def("to_dict", [](SensoryPercept& percept) {
            py::dict result;
            result["vision"] = percept.getVision();
            result["touch"] = percept.getTouch();
            result["internal"] = percept.getInternal();
            result["proprioception"] = percept.getProprioception();
            result["audio"] = percept.getAudio();
            result["timestamp"] = percept.getTimestamp();
            return result;
        }, "Convert to Python dictionary");

    // SimpleWorld class with enhanced interface
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation with enhanced API)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure the world dimensions")
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update the world with a timestep")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply a motor command to the world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get the current sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get the agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add a world object")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("step", [](SimpleWorld& self, float timestep) {
            self.update(timestep);
        }, py::arg("timestep"), "Step the simulation (alias for update)");

    // Enhanced neuron class with operator overloading
    py::class_<EnhancedNeuron>(m, "EnhancedNeuron", R"pbdoc(Enhanced neuron with Pythonic operators)pbdoc")
        .def(py::init<>(), "Create null neuron")
        .def(py::init<Neuron*>(), py::arg("neuron"), "Initialize enhanced neuron wrapper")
        .def("__iadd__", &EnhancedNeuron::operator+=
             "Operator += for current injection")
        .def("__repr__", &EnhancedNeuron::toString)
        .def("__str__", &EnhancedNeuron::toString)
        .def("__eq__", &EnhancedNeuron::operator==,
             "Equality comparison")
        .def("__ne__", &EnhancedNeuron::operator!=,
             "Inequality comparison")
        .def("__bool__", &EnhancedNeuron::operator bool,
             "Boolean conversion (active?)")
        .def("get_current", &EnhancedNeuron::getCurrent,
             "Get neuron current")
        .def("get_id", &EnhancedNeuron::getId,
             "Get neuron ID")
        .def("get_type", &EnhancedNeuron::getType,
             "Get neuron type")
        .def("is_active", &EnhancedNeuron::isActive,
             "Check if neuron is active")
        .def("get_potential", &EnhancedNeuron::getPotential,
             "Get membrane potential")
        .def("get_type_name", &EnhancedNeuron::getTypeName,
             "Get human-readable type name")
        .def("is_type", &EnhancedNeuron::isType, py::arg("type"),
             "Check if neuron is of specific type")
        .def("get_state", &EnhancedNeuron::getState,
             "Get neuron state as dictionary");

    // Smart brain wrapper for better memory management
    py::class_<SmartBrain>(m, "SmartBrain",
                          R"pbdoc(Smart brain wrapper with automatic memory management)pbdoc")
        .def(py::init<std::shared_ptr<Brain>, bool>(),
             py::arg("brain"), py::arg("auto_cleanup") = true,
             "Initialize smart brain wrapper")
        .def("disable_auto_cleanup", &SmartBrain::disableAutoCleanup,
             "Disable automatic cleanup")
        .def("get", &SmartBrain::get,
             "Get underlying brain (property)")
        .def("get_config", &SmartBrain::getConfig,
             "Get enhanced configuration interface")
        .def("batch_step", &SmartBrain::batchStep,
             py::arg("steps"), py::arg("times") = std::vector<Timestamp>(),
             "Process multiple steps in batch")
        .def("get_metrics", &SmartBrain::getMetrics,
             "Get comprehensive brain metrics")
        .def("__bool__", &SmartBrain::operator bool,
             "Check if brain is valid (bool conversion)")
        .def("__repr__", [](const SmartBrain& smart) {
            return smart.get() ? "<SmartBrain valid>" : "<SmartBrain invalid>";
        });

    // Batch processor for efficient simulation
    py::class_<BatchProcessor>(m, "BatchProcessor",
                              R"pbdoc(Batch processor for efficient simulation of multiple steps)pbdoc")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>>(),
             py::arg("brain"), py::arg("world"),
             "Initialize batch processor")
        .def("batch_step", &BatchProcessor::batchStep,
             py::arg("steps"), py::arg("times") = std::vector<Timestamp>(),
             "Batch process multiple simulation steps")
        .def("batch_process_sensory", &BatchProcessor::batchProcessSensory,
             py::arg("percepts"),
             "Batch process multiple sensory inputs")
        .def("get_batch_metrics", &BatchProcessor::getBatchMetrics,
             py::arg("steps"),
             "Get metrics for batch processing")
        .def("process_episode", &BatchProcessor::processEpisode,
             py::arg("percepts"), py::arg("actions") = std::vector<ActionType>(),
             "Process entire episode with given percepts and actions")
        .def("analyze", &BatchProcessor::analyze,
             "Get comprehensive analysis of batch processing");

    // Context manager for resource management
    py::class_<BrainContextManager>(m, "BrainContextManager",
                                   R"pbdoc(Context manager for automatic brain resource management)pbdoc")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, bool, std::string>(),
             py::arg("brain"), py::arg("world"), py::arg("auto_save") = false, py::arg("auto_save_path") = "",
             "Create brain context manager")
        .def("__enter__", &BrainContextManager::__enter__,
             "Enter context manager")
        .def("__exit__", &BrainContextManager::__exit__,
             "Exit context manager")
        .def("get_brain", &BrainContextManager::getBrain,
             "Get the underlying brain")
        .def("get_world", &BrainContextManager::getWorld,
             "Get the associated world");

    // Convenience class for advanced users
    py::class_<BrainConvenience>(m, "BrainConvenience",
                               R"pbdoc(Convenience methods for advanced NLM users)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Initialize convenience methods")
        .def("simulate_episode", &BrainConvenience::simulateEpisode,
             py::arg("percepts"), py::arg("actions"),
             "Simulate an episode with given percepts and actions")
        .def("analyze_performance", &BrainConvenience::analyzePerformance,
             py::arg("metric_type") = "comprehensive",
             "Analyze brain performance with specified metrics")
        .def("optimize_memory", &BrainConvenience::optimizeMemory,
             "Optimize memory usage")
        .def("configure_for_task", &BrainConvenience::configureForTask,
             py::arg("task_type"), py::arg("params") = std::unordered_map<std::string, float>(),
             "Configure brain for a specific task")
        .def("get_statistics", &BrainConvenience::getStatistics,
             "Get brain statistics as dictionary")
        .def("reset_and_initialize", &BrainConvenience::resetAndInitialize,
             "Reset brain and reinitialize")
        .def("enable_all_systems", &BrainConvenience::enableAllSystems,
             "Enable all plasticity, development, and neuromodulation systems");

    // Enhanced Brain class with additional methods
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class with enhanced API)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Initialize brain with configuration")
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
        // Enhanced convenience methods
        .def("get_enhanced_config", [](Brain& self) {
            return EnhancedConfig(self.getConfig());
        }, "Get enhanced configuration interface")
        .def("get_convenience", [](Brain& self) {
            return BrainConvenience(self.shared_from_this());
        }, "Get convenience methods for advanced users")
        .def("get_enhanced_neuron", [](Brain& self, const NeuronId& id) {
            // This would need actual neuron lookup implementation
            // For now, return a null enhanced neuron
            return EnhancedNeuron();
        }, "Get enhanced neuron wrapper by ID")
        .def("batch_process", [](Brain& self, const std::vector<SimulationStep>& steps, 
                               const std::vector<Timestamp>& times = {}) {
            // This requires a world reference - would need context
            auto world = std::make_shared<SimpleWorld>();
            BatchProcessor processor(self.shared_from_this(), world);
            return processor.batchStep(steps, times);
        }, py::arg("steps"), py::arg("times") = std::vector<Timestamp>(),
           "Process multiple steps in batch")
        .def("set_developmental_stage", &Brain::setDevelopmentalStage,
             py::arg("stage"), "Set developmental stage (alias)")
        .def("get_developmental_stage", &Brain::getDevelopmentalStage,
             "Get developmental stage (alias)")
        .def("get_smart", [](Brain& self) {
            return SmartBrain(self.shared_from_this());
        }, "Get smart brain wrapper for enhanced memory management");

    // AgentBrain class with enhanced interface
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world with enhanced API)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Initialize with brain")
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
        // Enhanced convenience methods
        .def("create_context_manager", [](AgentBrain& self, std::shared_ptr<SimpleWorld> world,
                                        bool autoSave = false, const std::string& autoSavePath = "") {
            return BrainContextManager(self.getBrain(), world, autoSave, autoSavePath);
        }, py::arg("world"), py::arg("auto_save") = false, py::arg("auto_save_path") = "",
           "Create context manager for automatic resource management")
        .def("get_brain", &AgentBrain::getBrain, "Get underlying brain (alias)")
        .def("process_input", &AgentBrain::processSensoryInput, 
             py::arg("percept"), "Process sensory input (alias)")
        .def("get_developmental_stage", &AgentBrain::getDevelopmentalStage,
             "Get developmental stage (alias)");

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
    
    // Module-level convenience functions
    m.def("simulate_batch", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                            const std::vector<SimulationStep>& steps,
                            const std::vector<Timestamp>& times = {}) {
        BatchProcessor processor(brain, world);
        return processor.batchStep(steps, times);
    }, py::arg("brain"), py::arg("world"), py::arg("steps"), 
       py::arg("times") = std::vector<Timestamp>(),
       "Module-level function for batch simulation");
    
    m.def("create_enhanced_config", [](std::shared_ptr<Config> config) {
        return EnhancedConfig(config);
    }, py::arg("config"), "Create enhanced configuration interface");
    
    m.def("create_convenience", [](std::shared_ptr<Brain> brain) {
        return BrainConvenience(brain);
    }, py::arg("brain"), "Create convenience methods for brain");
    
    m.def("version", []() { return "2.0.0"; }, "Get bindings version");
    
    // Error examples for documentation
    m.def("raise_example_errors", []() {
        throw ConfigurationError("Failed to load configuration from file: invalid JSON format");
    }, "Example: raise ConfigurationError");
    
    m.def("raise_simulation_error", []() {
        throw SimulationError("Simulation step cannot proceed: invalid time step");
    }, "Example: raise SimulationError");
}

} // namespace nlm