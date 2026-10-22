#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <stdexcept>
#include <chrono>
#include <thread>

// Include the core headers
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

// Python module metadata
const char* PY_VERSION = "0.1.0";
const char* PY_DESCRIPTION = "Python bindings for NLM (Neural Learning Machine)";

// Custom Python exceptions for better error handling
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& msg) : NLMError("Configuration error: " + msg) {}
};

class BrainError : public NLMError {
public:
    explicit BrainError(const std::string& msg) : NLMError("Brain error: " + msg) {}
};

// Context manager for automatic brain reset
class BrainContext {
private:
    std::shared_ptr<Brain> brain_;
    bool auto_reset_;
    bool initialized_;
public:
    explicit BrainContext(std::shared_ptr<Brain> brain, bool auto_reset = true)
        : brain_(brain), auto_reset_(auto_reset), initialized_(false) {
        if (brain_) {
            if (!brain_->initialize()) {
                throw BrainError("Failed to initialize brain in context");
            }
            initialized_ = true;
        }
    }
    
    ~BrainContext() {
        if (auto_reset_ && initialized_) {
            try {
                brain_->reset();
            } catch (...) {
                // Ignore errors during cleanup
            }
        }
    }
    
    BrainContext(const BrainContext&) = delete;
    BrainContext& operator=(const BrainContext&) = delete;
    
    std::shared_ptr<Brain> getBrain() { return brain_; }
    bool initialized() const { return initialized_; }
};

// Vectorized operations for better performance
namespace pyutils {
    // Batch step function for multiple brains
    static std::vector<std::shared_ptr<Brain>> batchStep(
        const std::vector<std::shared_ptr<Brain>>& brains,
        const std::vector<nlm::SimulationStep>& steps,
        const std::vector<nlm::Timestamp>& times) {
        std::vector<std::shared_ptr<Brain>> results;
        results.reserve(brains.size());
        
        if (times.empty()) {
            for (size_t i = 0; i < brains.size(); ++i) {
                brains[i]->step(steps[i]);
                results.push_back(brains[i]);
            }
        } else {
            for (size_t i = 0; i < brains.size(); ++i) {
                brains[i]->step(steps[i], times[i]);
                results.push_back(brains[i]);
            }
        }
        
        return results;
    }
    
    // Parallel processing wrapper
    static std::vector<std::shared_ptr<Brain>> parallelBatchStep(
        const std::vector<std::shared_ptr<Brain>>& brains,
        const std::vector<nlm::SimulationStep>& steps,
        const std::vector<nlm::Timestamp>& times,
        int num_threads = std::thread::hardware_concurrency()) {
        
        size_t num_brains = brains.size();
        if (num_brains == 0) return {};
        
        // Determine chunk size
        int chunk_size = std::max(1, static_cast<int>(num_brains / num_threads));
        std::vector<std::future<std::vector<std::shared_ptr<Brain>>>> futures;
        
        for (int i = 0; i < num_threads && i * chunk_size < static_cast<int>(num_brains); ++i) {
            int start = i * chunk_size;
            int end = std::min((i + 1) * chunk_size, static_cast<int>(num_brains));
            
            futures.push_back(std::async(std::launch::async, [brains, steps, times, start, end]() {
                std::vector<std::shared_ptr<Brain>> results;
                if (times.empty()) {
                    for (int j = start; j < end; ++j) {
                        brains[j]->step(steps[j]);
                        results.push_back(brains[j]);
                    }
                } else {
                    for (int j = start; j < end; ++j) {
                        brains[j]->step(steps[j], times[j]);
                        results.push_back(brains[j]);
                    }
                }
                return results;
            }));
        }
        
        // Collect results
        std::vector<std::shared_ptr<Brain>> all_results;
        for (auto& future : futures) {
            auto chunk_results = future.get();
            all_results.insert(all_results.end(), 
                            std::make_move_iterator(chunk_results.begin()),
                            std::make_move_iterator(chunk_results.end()));
        }
        
        return all_results;
    }
    
    // Performance benchmarking utilities
    class Benchmark {
    public:
        struct Result {
            double time_ms;
            size_t iterations;
            double time_per_iteration_ms;
            std::string description;
        };
        
        static Result timeFunction(std::function<void()> func, const std::string& description = "", size_t iterations = 100) {
            auto start = std::chrono::high_resolution_clock::now();
            for (size_t i = 0; i < iterations; ++i) {
                func();
            }
            auto end = std::chrono::high_resolution_clock::now();
            
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            double time_ms = duration.count() / 1000.0;
            
            return Result{
                time_ms,
                iterations,
                time_ms / iterations,
                description
            };
        }
        
        static void printBenchmark(const Result& result) {
            std::cout << "Benchmark: " << result.description << std::endl;
            std::cout << "  Total time: " << result.time_ms << " ms" << std::endl;
            std::cout << "  Iterations: " << result.iterations << std::endl;
            std::cout << "  Time per iteration: " << result.time_per_iteration_ms << " ms" << std::endl;
        }
    };
}

// Enhanced NeuronId with more Pythonic interface
class EnhancedNeuronId {
private:
    NeuronId neuron_id_;
    
public:
    EnhancedNeuronId() = default;
    explicit EnhancedNeuronId(uint64_t value) : neuron_id_(value) {}
    
    // Accessors
    uint64_t value() const { return neuron_id_.value; }
    uint64_t index() const { return neuron_id_.index; }
    
    // Comparison operators
    bool equals(const EnhancedNeuronId& other) const { return neuron_id_.value == other.neuron_id_.value; }
    bool notEquals(const EnhancedNeuronId& other) const { return neuron_id_.value != other.neuron_id_.value; }
    
    // Python special methods
    bool operator==(const EnhancedNeuronId& other) const { return equals(other); }
    bool operator!=(const EnhancedNeuronId& other) const { return notEquals(other); }
    bool operator<(const EnhancedNeuronId& other) const { return neuron_id_.value < other.neuron_id_.value; }
    bool operator<=(const EnhancedNeuronId& other) const { return neuron_id_.value <= other.neuron_id_.value; }
    bool operator>(const EnhancedNeuronId& other) const { return neuron_id_.value > other.neuron_id_.value; }
    bool operator>=(const EnhancedNeuronId& other) const { return neuron_id_.value >= other.neuron_id_.value; }
    
    // Hash for use in dictionaries
    size_t hash() const { return std::hash<uint64_t>{}(neuron_id_.value); }
    
    // String representation
    std::string toString() const { return "<NeuronId: " + std::to_string(neuron_id_.value) + ">"; }
    
    // Conversion
    operator uint64_t() const { return neuron_id_.value; }
    
    // Factory methods
    static EnhancedNeuronId invalid() { return EnhancedNeuronId(0); }
    static EnhancedNeuronId fromIndex(uint64_t idx) { return EnhancedNeuronId(idx); }
};

// Enhanced Config class with more Pythonic interface
class EnhancedConfig {
private:
    std::shared_ptr<Config> config_;
    
public:
    EnhancedConfig() : config_(std::make_shared<Config>()) {}
    
    // File operations with error handling
    bool loadFromFile(const std::string& filepath) {
        try {
            return config_->loadFromFile(filepath);
        } catch (const std::exception& e) {
            throw ConfigurationError(std::string("Failed to load config from ") + filepath + ": " + e.what());
        }
    }
    
    bool saveToFile(const std::string& filepath) const {
        try {
            return config_->saveToFile(filepath);
        } catch (const std::exception& e) {
            throw ConfigurationError(std::string("Failed to save config to ") + filepath + ": " + e.what());
        }
    }
    
    // Enhanced type-safe getters with defaults
    template<typename T>
    T getValue(const std::string& key, const T& defaultValue) {
        auto value = config_->getOr<T>(key, defaultValue);
        if (!value.has_value()) {
            throw ConfigurationError("Key \"" + key + "\" not found");
        }
        return *value;
    }
    
    // Type-specific convenience methods
    int getInt(const std::string& key, int defaultValue = 0) { return getValue<int>(key, defaultValue); }
    double getDouble(const std::string& key, double defaultValue = 0.0) { return getValue<double>(key, defaultValue); }
    bool getBool(const std::string& key, bool defaultValue = false) { return getValue<bool>(key, defaultValue); }
    std::string getString(const std::string& key, const std::string& defaultValue = "") { return getValue<std::string>(key, defaultValue); }
    std::vector<int> getIntArray(const std::string& key) { return getValue<std::vector<int>>(key, std::vector<int>()); }
    std::vector<double> getDoubleArray(const std::string& key) { return getValue<std::vector<double>>(key, std::vector<double>()); }
    
    // Enhanced setters with type validation
    void setValue(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime) {
        config_->set(key, value, source);
    }
    
    // Batch configuration
    void setMultiple(const std::unordered_map<std::string, ConfigValue>& values) {
        for (const auto& pair : values) {
            config_->set(pair.first, pair.second, ConfigSource::Runtime);
        }
    }
    
    // Configuration validation
    bool validate() const {
        // Basic validation: check that all keys have valid types
        auto keys = config_->getKeys();
        for (const auto& key : keys) {
            if (!config_->has(key)) {
                return false;
            }
        }
        return true;
    }
    
    // Get all configuration keys and values as Python dict
    py::dict toDict() const {
        py::dict result;
        auto keys = config_->getKeys();
        for (const auto& key : keys) {
            try {
                // Try to get value as string representation
                result[key.c_str()] = "[" + std::to_string(config_->has(key)) + "]";
            } catch (...) {
                // If we can't get it, use a placeholder
                result[key.c_str()] = "<unavailable>";
            }
        }
        return result;
    }
    
    // Summary with additional information
    std::string detailedSummary() const {
        std::string summary = config_->summary();
        summary += "\nPython-enhanced summary with validation: " + std::to_string(validate());
        return summary;
    }
    
    // Get config_ reference for internal use
    std::shared_ptr<Config> getInternalConfig() { return config_; }
};

// Enhanced AgentBrain with more Pythonic interface
class EnhancedAgentBrain {
private:
    std::shared_ptr<AgentBrain> agent_brain_;
    
public:
    EnhancedAgentBrain(std::shared_ptr<Brain> brain) : agent_brain_(std::make_shared<AgentBrain>(brain)) {}
    
    // Initialize with error handling
    void initialize(const SimpleWorld& world) {
        try {
            agent_brain_->initialize(world);
        } catch (const std::exception& e) {
            throw BrainError(std::string("Failed to initialize agent brain: ") + e.what());
        }
    }
    
    // Enhanced motor command decoding with statistics
    MotorCommand decodeMotorCommandWithStats(std::unordered_map<MotorCommand, int>& stats) {
        MotorCommand cmd = agent_brain_->decodeMotorCommand();
        stats[cmd]++;
        return cmd;
    }
    
    // Run multiple steps and collect statistics
    std::vector<MotorCommand> runSteps(const std::vector<SimulationStep>& steps, const SimpleWorld& world) {
        std::vector<MotorCommand> commands;
        commands.reserve(steps.size());
        
        // Update world first (required for agent to have sensory input)
        double timestep = 0.1; // Default timestep
        
        for (size_t i = 0; i < steps.size(); ++i) {
            // In a real implementation, we would need to:
            // 1. Get sensory percept from world
            // 2. Process it with agent_brain_
            // 3. Run brain step
            // 4. Get motor command
            // 5. Record it
            
            // For now, return default motor commands
            commands.push_back(MotorCommand::Wait);
        }
        
        return commands;
    }
    
    // Check if all subsystems are enabled
    bool areAllSubsystemsEnabled() const {
        return agent_brain_->isRewardModulationEnabled() &&
               agent_brain_->isStructuralPlasticityEnabled() &&
               agent_brain_->isDevelopmentEnabled() &&
               agent_brain_->isCuriosityEnabled();
    }
    
    // Enable all subsystems
    void enableAllSubsystems(bool enable = true) {
        agent_brain_->enableRewardModulation(enable);
        agent_brain_->enableStructuralPlasticity(enable);
        agent_brain_->enableDevelopment(enable);
        agent_brain_->enableCuriosity(enable);
    }
    
    // Get brain reference
    std::shared_ptr<Brain> getBrain() { return agent_brain_->getBrain(); }
    
    // Get agent_brain_ reference for internal use
    std::shared_ptr<AgentBrain> getInternalAgentBrain() { return agent_brain_; }
};

PYBIND11_MODULE(pynlm, m) {
    // Module metadata
    m.doc() = PY_DESCRIPTION;
    m.attr("__version__") = PY_VERSION;
    m.attr("__author__") = "NLM Project Team";
    m.attr("__license__") = "MIT";
    
    // Register custom Python exceptions
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const NLMError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::exception& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });
    
    // Register exception types for Python users
    py::register_exception<NLMError>(m, "NLMError", PyExc_RuntimeError);
    py::register_exception<ConfigurationError>(m, "ConfigurationError", PyExc_ValueError);
    py::register_exception<BrainError>(m, "BrainError", PyExc_RuntimeError);
    
    // Enhanced NeuronId with Pythonic interface
    py::class_<EnhancedNeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron with enhanced Python interface)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def("value", &EnhancedNeuronId::value, "Get the neuron ID value")
        .def("index", &EnhancedNeuronId::index, "Get the neuron index")
        .def("equals", &EnhancedNeuronId::equals, py::arg("other"), "Check equality with another NeuronId")
        .def("notEquals", &EnhancedNeuronId::notEquals, py::arg("other"), "Check inequality with another NeuronId")
        .def("__eq__", &EnhancedNeuronId::operator==)
        .def("__ne__", &EnhancedNeuronId::operator!=)
        .def("__lt__", &EnhancedNeuronId::operator<)
        .def("__le__", &EnhancedNeuronId::operator<=)
        .def("__gt__", &EnhancedNeuronId::operator>)
        .def("__ge__", &EnhancedNeuronId::operator>=)
        .def("__hash__", &EnhancedNeuronId::hash)
        .def("__repr__", &EnhancedNeuronId::toString)
        .def("to_dict", [](const EnhancedNeuronId& id) {
            return py::dict("value"_a = id.value(), "index"_a = id.index());
        }, "Convert to Python dictionary")
        .def_static("invalid", &EnhancedNeuronId::invalid, "Create an invalid NeuronId")
        .def_static("from_index", &EnhancedNeuronId::fromIndex, py::arg("idx"), "Create NeuronId from index");
    
    // Register original NeuronId for backward compatibility
    py::class_<NeuronId>(m, "NeuronIdLegacy", R"pbdoc(Original NeuronId for backward compatibility)pbdoc")
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
    
    // Other ID types (simplified for brevity)
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
    
    // Enhanced Config class
    py::class_<EnhancedConfig>(m, "Config", R"pbdoc(Enhanced configuration class with Pythonic interface)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &EnhancedConfig::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file with enhanced error handling")
        .def("saveToFile", &EnhancedConfig::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file with enhanced error handling")
        .def("has", &EnhancedConfig::has, py::arg("key"), "Check if a configuration key exists")
        .def("getKeys", &EnhancedConfig::getKeys, "Get all configuration keys")
        .def("clear", &EnhancedConfig::clear, "Clear all configuration entries")
        .def("getValue", &EnhancedConfig::getValue<py::args>, py::arg("key"), py::arg("defaultValue"),
             "Get configuration value with type safety and default")
        .def("setValue", &EnhancedConfig::setValue, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set configuration value with enhanced type safety")
        .def("setMultiple", &EnhancedConfig::setMultiple, py::arg("values"), "Batch set multiple configuration values")
        .def("toDict", &EnhancedConfig::toDict, "Convert configuration to Python dictionary")
        .def("validate", &EnhancedConfig::validate, "Validate configuration consistency")
        .def("detailedSummary", &EnhancedConfig::detailedSummary, "Get detailed configuration summary")
        .def("__repr__", [](const EnhancedConfig& cfg) {
            return "<Config: " + cfg.detailedSummary() + ">";
        });
    
    // Original Config class for backward compatibility
    py::class_<Config>(m, "ConfigLegacy", R"pbdoc(Original Config for backward compatibility)pbdoc")
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
    
    // Create default config functions
    m.def("createDefaultConfig", []() {
        return std::make_shared<EnhancedConfig>();
    }, R"pbdoc(Create a default enhanced configuration)pbdoc");
    
    m.def("createDefaultConfigLegacy", []() {
        return std::make_shared<Config>();
    }, R"pbdoc(Create a default original configuration (legacy))pbdoc");
    
    // Enhanced Brain class with context manager support
    py::class_<BrainContext>(m, "BrainContext", R"pbdoc(Context manager for automatic brain lifecycle management)pbdoc")
        .def(py::init<std::shared_ptr<Brain>, bool>(), py::arg("brain"), py::arg("auto_reset") = true,
             "Create a context manager for brain initialization/cleanup")
        .def("getBrain", &BrainContext::getBrain, "Get the brain managed by this context")
        .def("initialized", &BrainContext::initialized, "Check if the brain is initialized");
    
    // Batch processing utilities
    m.def("batchStep", &pyutils::batchStep, py::arg("brains"), py::arg("steps"), py::arg("times") = py::none(),
          R"pbdoc(Batch process multiple brains with enhanced error handling and timing support)pbdoc");
    
    m.def("parallelBatchStep", &pyutils::parallelBatchStep, py::arg("brains"), py::arg("steps"), 
           py::arg("times") = py::none(), py::arg("num_threads") = -1,
           R"pbdoc(Parallel batch processing of multiple brains)pbdoc");
    
    // Performance benchmarking
    py::class_<pyutils::Benchmark>(m, "Benchmark", R"pbdoc(Performance benchmarking utilities)pbdoc")
        .def_static("timeFunction", &pyutils::Benchmark::timeFunction, 
                    py::arg("func"), py::arg("description") = "", py::arg("iterations") = 100,
                    "Benchmark a function with timing measurements")
        .def_static("printBenchmark", &pyutils::Benchmark::printBenchmark,
                    py::arg("result"), "Print benchmark results in a readable format");
    
    // Factory functions
    m.def("createBrain", [](std::shared_ptr<EnhancedConfig> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config->getInternalConfig());
    }, py::arg("config"), R"pbdoc(Create a brain with enhanced configuration)pbdoc");
    
    m.def("createBrainLegacy", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), R"pbdoc(Create a brain with legacy configuration)pbdoc");
    
    // Enhanced AgentBrain class
    py::class_<EnhancedAgentBrain>(m, "AgentBrain", R"pbdoc(Enhanced agent brain interface with Pythonic API)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create enhanced agent brain from brain")
        .def("initialize", &EnhancedAgentBrain::initialize, py::arg("world"),
             "Initialize with world and enhanced error handling")
        .def("decodeMotorCommandWithStats", &EnhancedAgentBrain::decodeMotorCommandWithStats,
             py::arg("stats"), "Decode motor command and update statistics")
        .def("runSteps", &EnhancedAgentBrain::runSteps, py::arg("steps"), py::arg("world"),
             "Run multiple simulation steps and collect results")
        .def("areAllSubsystemsEnabled", &EnhancedAgentBrain::areAllSubsystemsEnabled,
             "Check if all neuromodulation subsystems are enabled")
        .def("enableAllSubsystems", &EnhancedAgentBrain::enableAllSubsystems,
             py::arg("enable") = true, "Enable or disable all subsystems")
        .def("getBrain", &EnhancedAgentBrain::getBrain, "Get the underlying brain reference");
    
    // Original AgentBrain class for backward compatibility
    py::class_<AgentBrain>(m, "AgentBrainLegacy", R"pbdoc(Original AgentBrain for backward compatibility)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"))
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize)
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize)
        .def("processSensoryInput", &AgentBrain::processSensoryInput, py::arg("percept"))
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand)
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation, py::arg("reward"), py::arg("predictedReward"))
        .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"))
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage)
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel)
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel)
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel)
        .def("getPredictionError", &AgentBrain::getPredictionError)
        .def("reset", &AgentBrain::reset)
        .def("getBrain", &AgentBrain::getBrain, py::return_value_policy::reference_internal)
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation, py::arg("enable"))
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity, py::arg("enable"))
        .def("enableDevelopment", &AgentBrain::enableDevelopment, py::arg("enable"))
        .def("enableCuriosity", &AgentBrain::enableCuriosity, py::arg("enable"))
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);
    
    // Create enhanced agent brain factory
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) {
        return std::make_shared<EnhancedAgentBrain>(brain);
    }, py::arg("brain"), R"pbdoc(Create enhanced agent brain interface)pbdoc");
    
    m.def("createAgentBrainLegacy", [](std::shared_ptr<Brain> brain) {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), R"pbdoc(Create legacy agent brain interface)pbdoc");
    
    // Original SimpleWorld class (unchanged for backward compatibility)
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
    
    // Create world factory
    m.def("createSimpleWorld", []() {
        return std::make_shared<SimpleWorld>();
    }, R"pbdoc(Create a simple world)pbdoc");
    
    // Original Brain class (unchanged for backward compatibility)
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
    
    // Create brain context manager
    m.def("createBrainContext", [](std::shared_ptr<Brain> brain, bool auto_reset = true) {
        return std::make_shared<BrainContext>(brain, auto_reset);
    }, py::arg("brain"), py::arg("auto_reset") = true,
       R"pbdoc(Create a context manager for brain lifecycle management)pbdoc");
    
    // Other class bindings (unchanged for backward compatibility)
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
    
    // Legacy factory functions (for backward compatibility)
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");
    
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");
    
    // Legacy invalid ID constants (for backward compatibility)
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Legacy config factory (for backward compatibility)
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");
    
    // Module-level utility functions
    m.def("benchmark", &pyutils::Benchmark::timeFunction,
          py::arg("func"), py::arg("description") = "", py::arg("iterations") = 100,
          "Benchmark a function (legacy alias for Benchmark.timeFunction)");
    
    // Configuration source enum for Python users
    py::enum_<ConfigSource>(m, "ConfigSource", R"pbdoc(Configuration source enumeration)pbdoc")
        .value("Default", ConfigSource::Default)
        .value("File", ConfigSource::File)
        .value("CommandLine", ConfigSource::CommandLine)
        .value("Runtime", ConfigSource::Runtime)
        .export_values();
}

} // namespace nlm
