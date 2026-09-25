#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

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

class NeuromodulationError : public NLMError {
public:
    explicit NeuromodulationError(const std::string& msg) : NLMError("Neuromodulation error: " + msg) {}
};

class MemoryError : public NLMError {
public:
    explicit MemoryError(const std::string& msg) : NLMError("Memory error: " + msg) {}
};

class DevelopmentError : public NLMError {
public:
    explicit DevelopmentError(const std::string& msg) : NLMError("Development error: " + msg) {}
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

inline void validateNeuronId(const NeuronId& id) {
    if (!id.isValid()) {
        throw NLMError("Invalid neuron ID");
    }
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Enhanced Python Bindings
        ---------------------------------------------
        A comprehensive Python binding for the NLM C++ neural simulation framework.
        
        This enhanced version provides extensive functionality including:
        - Custom exception hierarchy for better error handling
        - Enhanced configuration with parameter validation
        - Neuromodulation systems (dopamine, curiosity, novelty, prediction error)
        - Memory systems (working memory, episodic memory)
        - Performance optimization (memory pools, statistics)
        - Development systems
        - Convenience factory functions
        - Simulation helpers
        - Serialization utilities
        - Comprehensive statistical analysis tools
        
        Key features:
        - Robust error handling with descriptive Python exceptions
        - Parameter validation and safety checks
        - Pythonic API with convenience methods
        - Context manager support for resource management
        - Enhanced debugging and monitoring capabilities
        - Type conversion helpers for Python integration
    )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMError>(m, "NLMError", PyExc_RuntimeError.ptr());
    py::register_exception<ConfigError>(m, "ConfigError", PyExc_ValueError.ptr());
    py::register_exception<BrainError>(m, "BrainError", PyExc_RuntimeError.ptr());
    py::register_exception<WorldError>(m, "WorldError", PyExc_RuntimeError.ptr());
    py::register_exception<NeuromodulationError>(m, "NeuromodulationError", PyExc_RuntimeError.ptr());
    py::register_exception<MemoryError>(m, "MemoryError", PyExc_RuntimeError.ptr());
    py::register_exception<DevelopmentError>(m, "DevelopmentError", PyExc_RuntimeError.ptr());

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

    py::class_<Config>(m, "Config", R"pbdoc(Advanced configuration class for NLM system with enhanced Python integration)pbdoc")
        .def(py::init<>())
        .def(py::init<const Config&>(), py::arg("other"))
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
        .def("validate", &Config::validate,
             "Validate configuration for common issues")
        .def("setFromPython", [](Config& self, const py::dict& py_dict) {
            py::dict::iterator it;
            for (it = py_dict.begin(); it != py_dict.end(); ++it) {
                std::string key = py::cast<std::string>(it->first);
                py::object value = it->second;
                if (value.is_pyint()) {
                    self.setInt(key, py::cast<int>(value));
                } else if (value.is_pyfloat()) {
                    self.setFloat(key, py::cast<double>(value));
                } else if (value.is_pystr()) {
                    self.setString(key, py::cast<std::string>(value));
                } else if (value.is_pylist()) {
                    auto py_list = py::cast<std::vector<double>>(value);
                    self.setDoubleArray(key, py_list);
                }
            }
        }, py::arg("py_dict"),
            "Set configuration from Python dictionary")
        .def("toPythonDict", [](const Config& self) {
            py::dict result;
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                py::object py_value;
                if (self.hasInt(key)) {
                    py_value = py::cast(self.getInt(key));
                } else if (self.hasFloat(key)) {
                    py_value = py::cast(self.getFloat(key));
                } else if (self.hasString(key)) {
                    py_value = py::cast(self.getString(key));
                } else if (self.hasDoubleArray(key)) {
                    py_value = py::cast(self.getDoubleArray(key));
                }
                result[py::cast(key)] = py_value;
            }
            return result;
        }, "Convert configuration to Python dictionary")
        .def("setInt", &Config::setInt, py::arg("key"), py::arg("value"),
             "Set an integer configuration value")
        .def("getInt", [](const Config& self, const std::string& key, int default_value) {
            return self.getInt(key, default_value);
        }, py::arg("key"), py::arg("default_value") = 0,
            "Get an integer configuration value with default")
        .def("setFloat", &Config::setFloat, py::arg("key"), py::arg("value"),
             "Set a float configuration value")
        .def("getFloat", [](const Config& self, const std::string& key, double default_value) {
            return self.getFloat(key, default_value);
        }, py::arg("key"), py::arg("default_value") = 0.0,
            "Get a float configuration value with default")
        .def("setString", &Config::setString, py::arg("key"), py::arg("value"),
             "Set a string configuration value")
        .def("getString", [](const Config& self, const std::string& key, const std::string& default_value) {
            return self.getString(key, default_value);
        }, py::arg("key"), py::arg("default_value") = "",
            "Get a string configuration value with default")
        .def("setBool", &Config::setBool, py::arg("key"), py::arg("value"),
             "Set a boolean configuration value")
        .def("getBool", [](const Config& self, const std::string& key, bool default_value) {
            return self.getBool(key, default_value);
        }, py::arg("key"), py::arg("default_value") = false,
            "Get a boolean configuration value with default")
        .def("setDoubleArray", &Config::setDoubleArray, py::arg("key"), py::arg("values"),
             "Set a double array configuration value")
        .def("getDoubleArray", [](const Config& self, const std::string& key) {
            return self.getDoubleArray(key);
        }, py::arg("key"),
            "Get a double array configuration value")
        .def("getAllInts", &Config::getAllInts,
             "Get all integer configuration values")
        .def("getAllFloats", &Config::getAllFloats,
             "Get all float configuration values")
        .def("getAllStrings", &Config::getAllStrings,
             "Get all string configuration values")
        .def("getAllBools", &Config::getAllBools,
             "Get all boolean configuration values")
        .def("importConfig", &Config::importConfig, py::arg("other"),
             "Import configuration from another Config object")
        .def("mergeConfig", &Config::mergeConfig, py::arg("other"),
             "Merge configuration from another Config object")
        .def("diffConfig", [](const Config& self, const Config& other) {
            return self.diffConfig(other);
        }, py::arg("other"),
            "Get difference between this and another configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

// Memory system classes
    py::class_<WorkingMemory>(m, "WorkingMemory", R"pbdoc(Working memory system for short-term information storage)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t>(), py::arg("capacity"))
        .def("store", &WorkingMemory::store, py::arg("key"), py::arg("value"),
             "Store information in working memory")
        .def("retrieve", &WorkingMemory::retrieve, py::arg("key"),
             "Retrieve information from working memory")
        .def("hasKey", &WorkingMemory::hasKey, py::arg("key"),
             "Check if key exists in working memory")
        .def("remove", &WorkingMemory::remove, py::arg("key"),
             "Remove information from working memory")
        .def("clear", &WorkingMemory::clear,
             "Clear all working memory")
        .def("getSize", &WorkingMemory::getSize)
        .def("getCapacity", &WorkingMemory::getCapacity)
        .def("getAllKeys", &WorkingMemory::getAllKeys)
        .def("getMemoryUsage", &WorkingMemory::getMemoryUsage)
        .def("isFull", &WorkingMemory::isFull);

    py::class_<EpisodicMemory>(m, "EpisodicMemory", R"pbdoc(Episodic memory system for storing experience episodes)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t>(), py::arg("max_episodes"))
        .def("storeEpisode", &EpisodicMemory::storeEpisode, py::arg("episode_id"), py::arg("data"),
             "Store an episode with timestamp and context")
        .def("retrieveEpisode", &EpisodicMemory::retrieveEpisode, py::arg("episode_id"),
             "Retrieve a specific episode")
        .def("retrieveSimilarEpisodes", &EpisodicMemory::retrieveSimilarEpisodes, py::arg("context"),
             "Retrieve similar episodes based on context")
        .def("deleteEpisode", &EpisodicMemory::deleteEpisode, py::arg("episode_id"),
             "Delete an episode")
        .def("getEpisodeCount", &EpisodicMemory::getEpisodeCount)
        .def("getMaxEpisodes", &EpisodicMemory::getMaxEpisodes)
        .def("setMaxEpisodes", &EpisodicMemory::setMaxEpisodes, py::arg("max"))
        .def("getAllEpisodeIds", &EpisodicMemory::getAllEpisodeIds)
        .def("clear", &EpisodicMemory::clear,
             "Clear all episodic memories")
        .def("getMemoryStatistics", &EpisodicMemory::getMemoryStatistics)
        .def("cleanupOldEpisodes", &EpisodicMemory::cleanupOldEpisodes, py::arg("max_age"),
             "Remove episodes older than specified age");

// Performance classes
    py::class_<MemoryPool>(m, "MemoryPool", R"pbdoc(Memory pool for efficient memory management)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t>(), py::arg("initial_size"))
        .def("allocate", &MemoryPool::allocate, py::arg("size"),
             "Allocate memory from pool")
        .def("deallocate", &MemoryPool::deallocate, py::arg("ptr"), py::arg("size"),
             "Deallocate memory back to pool")
        .def("getAvailableMemory", &MemoryPool::getAvailableMemory)
        .def("getTotalMemory", &MemoryPool::getTotalMemory)
        .def("getAllocatedMemory", &MemoryPool::getAllocatedMemory)
        .def("getUsageRatio", &MemoryPool::getUsageRatio)
        .def("grow", &MemoryPool::grow, py::arg("additional_size"),
             "Grow the pool size")
        .def("shrink", &MemoryPool::shrink, py::arg("shrink_size"),
             "Shrink the pool size")
        .def("getFragmentationLevel", &MemoryPool::getFragmentationLevel)
        .def("optimize", &MemoryPool::optimize,
             "Optimize memory allocation")
        .def("clear", &MemoryPool::clear,
             "Clear all allocated memory");

    py::class_<PerformanceMonitor>(m, "PerformanceMonitor", R"pbdoc(Performance monitoring and statistics collection)pbdoc")
        .def(py::init<>())
        .def("recordMetric", &PerformanceMonitor::recordMetric, py::arg("name"), py::arg("value"), py::arg("unit"),
             "Record a performance metric")
        .def("getMetric", &PerformanceMonitor::getMetric, py::arg("name"),
             "Get a performance metric value")
        .def("getAllMetrics", &PerformanceMonitor::getAllMetrics,
             "Get all recorded metrics")
        .def("getMetricNames", &PerformanceMonitor::getMetricNames,
             "Get names of all recorded metrics")
        .def("getAverage", &PerformanceMonitor::getAverage, py::arg("name"),
             "Get average value of a metric")
        .def("getMin", &PerformanceMonitor::getMin, py::arg("name"),
             "Get minimum value of a metric")
        .def("getMax", &PerformanceMonitor::getMax, py::arg("name"),
             "Get maximum value of a metric")
        .def("getStandardDeviation", &PerformanceMonitor::getStandardDeviation, py::arg("name"),
             "Get standard deviation of a metric")
        .def("clear", &PerformanceMonitor::clear,
             "Clear all metrics")
        .def("exportMetrics", &PerformanceMonitor::exportMetrics,
             "Export metrics to a dictionary")
        .def("importMetrics", &PerformanceMonitor::importMetrics, py::arg("metrics"),
             "Import metrics from a dictionary")
        .def("startTiming", &PerformanceMonitor::startTiming, py::arg("operation_name"),
             "Start timing an operation")
        .def("stopTiming", &PerformanceMonitor::stopTiming, py::arg("operation_name"),
             "Stop timing an operation")
        .def("getTiming", &PerformanceMonitor::getTiming, py::arg("operation_name"),
             "Get timing information for an operation");

// Development system classes
    py::class_<DevelopmentalSystem>(m, "DevelopmentalSystem", R"pbdoc(Developmental system for brain maturation and learning)pbdoc")
        .def(py::init<>())
        .def(py::init<double, double>(), py::arg("growth_rate"), py::arg("maturation_rate"))
        .def("update", &DevelopmentalSystem::update, py::arg("timestep"),
             "Update developmental processes")
        .def("getCurrentStage", &DevelopmentalSystem::getCurrentStage)
        .def("setCurrentStage", &DevelopmentalSystem::setCurrentStage, py::arg("stage"),
             "Set developmental stage")
        .def("getGrowthRate", &DevelopmentalSystem::getGrowthRate)
        .def("setGrowthRate", &DevelopmentalSystem::setGrowthRate, py::arg("rate"),
             "Set growth rate")
        .def("getMaturationRate", &DevelopmentalSystem::getMaturationRate)
        .def("setMaturationRate", &DevelopmentalSystem::setMaturationRate, py::arg("rate"),
             "Set maturation rate")
        .def("getCriticalPeriodActive", &DevelopmentalSystem::getCriticalPeriodActive)
        .def("setCriticalPeriodActive", &DevelopmentalSystem::setCriticalPeriodActive, py::arg("active"),
             "Set critical period active state")
        .def("getMaturationProgress", &DevelopmentalSystem::getMaturationProgress)
        .def("getDevelopmentScore", &DevelopmentalSystem::getDevelopmentScore)
        .def("reset", &DevelopmentalSystem::reset,
             "Reset development system")
        .def("enableStage", &DevelopmentalSystem::enableStage, py::arg("stage"),
             "Enable a specific developmental stage")
        .def("isStageEnabled", &DevelopmentalSystem::isStageEnabled, py::arg("stage"),
             "Check if a developmental stage is enabled")
        .def("getEnabledStages", &DevelopmentalSystem::getEnabledStages,
             "Get all enabled developmental stages")
        .def("scheduleEvent", &DevelopmentalSystem::scheduleEvent, py::arg("event_name"), py::arg("delay"),
             "Schedule a developmental event")
        .def("getScheduledEvents", &DevelopmentalSystem::getScheduledEvents,
             "Get scheduled developmental events");

    py::class_<SynapticPlasticity>(m, "SynapticPlasticity", R"pbdoc(Synaptic plasticity mechanisms for learning)pbdoc")
        .def(py::init<>())
        .def(py::init<double, double>(), py::arg("hebbian_strength"), py::arg("homeostatic_strength"))
        .def("update", &SynapticPlasticity::update, py::arg("brain"),
             "Update synaptic plasticity based on brain state")
        .def("getHebbianStrength", &SynapticPlasticity::getHebbianStrength)
        .def("setHebbianStrength", &SynapticPlasticity::setHebbianStrength, py::arg("strength"),
             "Set Hebbian plasticity strength")
        .def("getHomeostaticStrength", &SynapticPlasticity::getHomeostaticStrength)
        .def("setHomeostaticStrength", &SynapticPlasticity::setHomeostaticStrength, py::arg("strength"),
             "Set homeostatic plasticity strength")
        .def("getLongTermPotentiation", &SynapticPlasticity::getLongTermPotentiation)
        .def("getLongTermDepression", &SynapticPlasticity::getLongTermDepression)
        .def("getPlasticityHomeostasis", &SynapticPlasticity::getPlasticityHomeostasis)
        .def("applyHebbianRule", &SynapticPlasticity::applyHebbianRule, py::arg("pre_neuron"), py::arg("post_neuron"),
             "Apply Hebbian learning rule")
        .def("applyHomeostaticRule", &SynapticPlasticity::applyHomeostaticRule, py::arg("neuron_id"), py::arg("target_activity"),
             "Apply homeostatic regulation")
        .def("reset", &SynapticPlasticity::reset,
             "Reset synaptic plasticity state")
        .def("getPlasticityStatistics", &SynapticPlasticity::getPlasticityStatistics,
             "Get plasticity statistics");

// Add missing neuromodulation classes
    py::class_<Novelty, Neuromodulator>(m, "Novelty", R"pbdoc(Novelty neuromodulator class)pbdoc")
        .def(py::init<>())
        .def(py::init<double>(), py::arg("detection_threshold"))
        .def("apply_modulation", &Novelty::applyModulation, py::arg("base_current"), py::arg("input_activity"),
             "Apply novelty modulation based on input activity patterns")
        .def("getCurrentLevel", &Novelty::getCurrentLevel)
        .def("getDetectionThreshold", &Novelty::getDetectionThreshold)
        .def("setDetectionThreshold", &Novelty::setDetectionThreshold, py::arg("threshold"))
        .def("getNoveltyScore", &Novelty::getNoveltyScore)
        .def("reset", &Novelty::reset);

    py::class_<PredictionError, Neuromodulator>(m, "PredictionError", R"pbdoc(Prediction error neuromodulator class)pbdoc")
        .def(py::init<>())
        .def(py::init<double>(), py::arg("learning_rate"))
        .def("apply_modulation", &PredictionError::applyModulation, py::arg("base_current"), py::arg("predicted_value"), py::arg("actual_value"),
             "Apply prediction error modulation based on prediction mismatch")
        .def("getCurrentLevel", &PredictionError::getCurrentLevel)
        .def("getLearningRate", &PredictionError::getLearningRate)
        .def("setLearningRate", &PredictionError::setLearningRate, py::arg("rate"))
        .def("getErrorMagnitude", &PredictionError::getErrorMagnitude)
        .def("getPredictionHistory", &PredictionError::getPredictionHistory)
        .def("reset", &PredictionError::reset);

    // Add missing memory system class
    py::class_<SemanticMemory>(m, "SemanticMemory", R"pbdoc(Semantic memory system for storing knowledge and facts)pbdoc")
        .def(py::init<>())
        .def("storeFact", &SemanticMemory::storeFact, py::arg("subject"), py::arg("predicate"), py::arg("object"),
             "Store a semantic fact")
        .def("retrieveFact", &SemanticMemory::retrieveFact, py::arg("subject"), py::arg("predicate"),
             "Retrieve a fact by subject and predicate")
        .def("searchFacts", &SemanticMemory::searchFacts, py::arg("pattern"),
             "Search facts matching a pattern")
        .def("removeFact", &SemanticMemory::removeFact, py::arg("subject"), py::arg("predicate"),
             "Remove a fact")
        .def("getFactCount", &SemanticMemory::getFactCount)
        .def("getAllSubjects", &SemanticMemory::getAllSubjects)
        .def("getAllPredicates", &SemanticMemory::getAllPredicates)
        .def("clear", &SemanticMemory::clear,
             "Clear all semantic memories")
        .def("getMemoryUsage", &SemanticMemory::getMemoryUsage)
        .def("exportFacts", &SemanticMemory::exportFacts,
             "Export all facts as a list")
        .def("importFacts", &SemanticMemory::importFacts, py::arg("facts"),
             "Import facts from a list");

    // Add missing performance class
    py::class_<Profiler>(m, "Profiler", R"pbdoc(CPU profiler for performance analysis)pbdoc")
        .def(py::init<>())
        .def("start", &Profiler::start, py::arg("name"),
             "Start profiling a function")
        .def("stop", &Profiler::stop, py::arg("name"),
             "Stop profiling a function")
        .def("getProfileData", &Profiler::getProfileData, py::arg("name"),
             "Get profile data for a function")
        .def("getAllProfiles", &Profiler::getAllProfiles,
             "Get all profile data")
        .def("clear", &Profiler::clear,
             "Clear all profile data")
        .def("getTotalTime", &Profiler::getTotalTime)
        .def("getAverageTime", &Profiler::getAverageTime)
        .def("getCallCount", &Profiler::getCallCount)
        .def("exportProfile", &Profiler::exportProfile,
             "Export profile data to a dictionary")
        .def("importProfile", &Profiler::importProfile, py::arg("profile_data"),
             "Import profile data from a dictionary");

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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

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

    // Enhanced convenience factory methods
    m.def("createStandardConfig", [](int neuron_count, double synapse_density, double learning_rate, double time_step) {
        auto config = std::make_shared<Config>();
        config->setNumNeurons(neuron_count);
        config->setSynapseDensity(synapse_density);
        config->setLearningRate(learning_rate);
        config->setTimeStep(time_step);
        return config;
    }, py::arg("neuron_count") = 1000, py::arg("synapse_density") = 0.1, py::arg("learning_rate") = 0.01, py::arg("time_step") = 0.1,
        "Create a standard NLM configuration with specified parameters");

    m.def("runCompleteSimulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, std::shared_ptr<AgentBrain> agent, int steps) {
        for (int step = 0; step < steps; ++step) {
            brain->step();
            if (agent) {
                agent->applyRewardModulation(0.1, 0.05);
            }
        }
        return true;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"),
        "Run a complete simulation for specified number of steps");

    m.def("benchmarkPerformance", [](std::shared_ptr<Brain> brain, int steps, int warmup_steps) {
        double start_time = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        
        // Warm-up
        for (int i = 0; i < warmup_steps; ++i) {
            brain->step();
        }
        
        // Benchmark
        for (int i = 0; i < steps; ++i) {
            brain->step();
        }
        
        double end_time = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        double elapsed_seconds = (end_time - start_time) / 1e9;
        
        return py::make_tuple(
            elapsed_seconds,
            steps / elapsed_seconds,
            brain->getTotalNeuronCount() * (steps / elapsed_seconds)
        );
    }, py::arg("brain"), py::arg("steps") = 1000, py::arg("warmup_steps") = 100,
        "Benchmark brain performance");

    // Enhanced simulation helpers
    m.def("runEpisode", [](std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world, int max_steps) {
        double start_time = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        int steps_completed = 0;
        
        for (int step = 0; step < max_steps; ++step) {
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            auto action = agent->decodeMotorCommand();
            auto result = world->applyMotorCommand(action, world->getSimulationTime());
            
            if (result.success) {
                agent->applyRewardModulation(result.reward, result.reward * 0.5);
            }
            
            if (checkCompletionConditions(agent->getBrain(), world, agent)) {
                break;
            }
            
            steps_completed++;
        }
        
        double end_time = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        double duration = (end_time - start_time) / 1e9;
        
        return py::make_tuple(steps_completed, duration, agent->getBrain()->getTotalSpikeCount());
    }, py::arg("agent"), py::arg("world"), py::arg("max_steps") = 100,
        "Run a complete agent episode");

    m.def("validateConfiguration", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, std::shared_ptr<AgentBrain> agent) {
        if (!validateSimulation(brain, world, agent)) {
            throw BrainError("Invalid simulation configuration");
        }
        return true;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
        "Validate simulation configuration");

    // Enhanced serialization helpers
    m.def("saveBrainStateWithValidation", [](std::shared_ptr<Brain> brain, const std::string& filepath, bool compress) {
        if (brain->getTotalNeuronCount() <= 0) {
            throw BrainError("Cannot save brain with zero neurons");
        }
        return brain->save(filepath);
    }, py::arg("brain"), py::arg("filepath"), py::arg("compress") = false,
        "Save brain state with validation");

    m.def("loadBrainStateWithValidation", [](const std::string& filepath) {
        auto config = std::make_shared<Config>();
        config->loadFromFile(filepath);
        auto brain = std::make_shared<Brain>(config);
        brain->load(filepath);
        return brain;
    }, py::arg("filepath"),
        "Load brain state with validation");

    // Enhanced type conversion helpers
    m.def("neuronIdToIndex", [](const NeuronId& id) {
        return id.index();
    }, py::arg("id"),
        "Convert NeuronId to index");

    m.def("indexToNeuronId", [](uint64_t index) {
        return NeuronId(index);
    }, py::arg("index"),
        "Convert index to NeuronId");

    m.def("collectBrainStatistics", [](std::shared_ptr<Brain> brain) {
        py::dict stats;
        stats["total_neurons"] = brain->getTotalNeuronCount();
        stats["active_neurons"] = brain->getActiveNeuronCount();
        stats["firing_neurons"] = brain->getFiringNeuronCount();
        stats["total_synapses"] = brain->getTotalSynapseCount();
        stats["average_firing_rate"] = brain->getAverageFiringRate();
        stats["excitation_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        stats["total_spike_count"] = brain->getTotalSpikeCount();
        stats["developmental_stage"] = brain->getDevelopmentalStage();
        return stats;
    }, py::arg("brain"),
        "Collect comprehensive brain statistics as dictionary");

    m.def("collectAgentStatistics", [](std::shared_ptr<AgentBrain> agent) {
        py::dict stats;
        stats["neuromodulation_level"] = agent->getNeuromodulationLevel();
        stats["curiosity_level"] = agent->getCuriosityLevel();
        stats["novelty_level"] = agent->getNoveltyLevel();
        stats["prediction_error"] = agent->getPredictionError();
        stats["developmental_stage"] = agent->getDevelopmentalStage();
        stats["reward_modulation_enabled"] = agent->isRewardModulationEnabled();
        stats["structural_plasticity_enabled"] = agent->isStructuralPlasticityEnabled();
        stats["development_enabled"] = agent->isDevelopmentEnabled();
        stats["curiosity_enabled"] = agent->isCuriosityEnabled();
        return stats;
    }, py::arg("agent"),
        "Collect comprehensive agent statistics as dictionary");

    // Debugging and monitoring helpers
    m.def("logBrainStatus", [](std::shared_ptr<Brain> brain) {
        brain->logStatus();
        return collectBrainStatistics(brain);
    }, py::arg("brain"),
        "Log brain status and return statistics");

    m.def("logAgentStatus", [](std::shared_ptr<AgentBrain> agent) {
        auto brain = agent->getBrain();
        brain->logStatus();
        auto agent_stats = collectAgentStatistics(agent);
        auto brain_stats = collectBrainStatistics(brain);
        
        py::dict combined_stats;
        for (const auto& item : py::cast<py::dict>(agent_stats)) {
            combined_stats[py::cast<std::string>(item.first)] = item.second;
        }
        for (const auto& item : py::cast<py::dict>(brain_stats)) {
            combined_stats[py::cast<std::string>(item.first)] = item.second;
        }
        return combined_stats;
    }, py::arg("agent"),
        "Log agent status and return combined statistics");

    // Type-safe configuration helpers
    m.def("configureBrainFromPython", [](std::shared_ptr<Brain> brain, const py::dict& config_dict) {
        auto nlm_config = brain->getConfig();
        nlm_config->setFromPython(config_dict);
    }, py::arg("brain"), py::arg("config_dict"),
        "Configure brain from Python dictionary");

    m.def("exportBrainConfig", [](std::shared_ptr<Brain> brain) {
        auto config = brain->getConfig();
        return config->toPythonDict();
    }, py::arg("brain"),
        "Export brain configuration as Python dictionary");

    m.def("importBrainConfig", [](std::shared_ptr<Brain> brain, const py::dict& config_dict) {
        auto config = brain->getConfig();
        config->setFromPython(config_dict);
    }, py::arg("brain"), py::arg("config_dict"),
        "Import configuration into brain");

    // Convenience methods for enhanced usability
    m.def("createBrainWithDefaults", []() {
        auto config = std::make_shared<Config>();
        return std::make_shared<Brain>(config);
    }, "Create a brain with default configuration");

    m.def("createWorldWithDefaults", [](int width, int height, int vision_width, int vision_height) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, vision_width, vision_height);
        return world;
    }, py::arg("width") = 100, py::arg("height") = 100, py::arg("vision_width") = 84, py::arg("vision_height") = 84,
        "Create a world with specified dimensions");

    m.def("createAgentWithDefaults", []() {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        return std::make_shared<AgentBrain>(brain);
    }, "Create an agent brain with default configuration");

} // namespace nlm
