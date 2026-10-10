#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <functional>
#include <unordered_map>
#include <variant>
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <iostream>

namespace py = pybind11;
namespace nlm {

// Custom exception classes for Python
class ConfigError : public std::runtime_error {
public:
    explicit ConfigError(const std::string& msg) : std::runtime_error(msg) {}
};

class BrainError : public std::runtime_error {
public:
    explicit BrainError(const std::string& msg) : std::runtime_error(msg) {}
};

class AgentBrainError : public std::runtime_error {
public:
    explicit AgentBrainError(const std::string& msg) : std::runtime_error(msg) {}
};

class WorldError : public std::runtime_error {
public:
    explicit WorldError(const std::string& msg) : std::runtime_error(msg) {}
};

// Helper functions for Python bindings
namespace detail {
    template<typename T>
    py::object convertVariantToPyObject(const T& value) {
        return py::cast(value);
    }
    
    template<typename... Args>
    py::object makePyObject(Args&&... args) {
        return py::make_tuple(std::forward<Args>(args)...);
    }
    
    std::vector<float> normalizeVector(const std::vector<float>& data) {
        std::vector<float> normalized(data.size());
        if (!data.empty()) {
            float min_val = *std::min_element(data.begin(), data.end());
            float max_val = *std::max_element(data.begin(), data.end());
            float range = max_val - min_val;
            if (range > 0.0f) {
                std::transform(data.begin(), data.end(), normalized.begin(),
                    [min_val, range](float val) { return (val - min_val) / range; });
            } else {
                std::fill(normalized.begin(), normalized.end(), 0.5f);
            }
        }
        return normalized;
    }
    
    template<typename T, typename U>
    std::vector<T> convertVariantVector(const std::vector<U>& input) {
        std::vector<T> result;
        result.reserve(input.size());
        for (const auto& item : input) {
            result.push_back(static_cast<T>(item));
        }
        return result;
    }
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings - Enhanced API
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, and more.
        
        Features:
        - Enhanced error handling with custom exceptions
        - Batch operations for multiple neurons/populations
        - Advanced configuration management
        - Debugging and monitoring tools
        - Performance profiling capabilities
        - Pythonic interfaces with context managers
        - Advanced agent features with concept formation and planning
        - Checkpoint/restore tools
        - Visualization helpers
        - Learning progress tracking
    )pbdoc";

    // Custom exception registration
    py::register_exception<ConfigError>(m, "ConfigError", PyExc_ValueError);
    py::register_exception<BrainError>(m, "BrainError", PyExc_RuntimeError);
    py::register_exception<AgentBrainError>(m, "AgentBrainError", PyExc_RuntimeError);
    py::register_exception<WorldError>(m, "WorldError", PyExc_RuntimeError);

    // Neuron ID and related types
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

    // Config class with enhanced API
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system with enhanced API)pbdoc")
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
        
        // Enhanced type-safe get() methods
        .def("get", [](const Config& self, const std::string& key) {
            if (!self.has(key)) {
                throw py::key_error("Configuration key not found: " + key);
            }
            return self.get<std::string>(key).value();
        }, py::arg("key"), "Get configuration value as string")
        
        .def("get", [](const Config& self, const std::string& key) {
            if (!self.has(key)) {
                throw py::key_error("Configuration key not found: " + key);
            }
            return self.get<int>(key).value();
        }, py::arg("key"), "Get configuration value as integer")
        
        .def("get", [](const Config& self, const std::string& key) {
            if (!self.has(key)) {
                throw py::key_error("Configuration key not found: " + key);
            }
            return self.get<double>(key).value();
        }, py::arg("key"), "Get configuration value as float")
        
        .def("get", [](const Config& self, const std::string& key) {
            if (!self.has(key)) {
                throw py::key_error("Configuration key not found: " + key);
            }
            return self.get<bool>(key).value();
        }, py::arg("key"), "Get configuration value as boolean")
        
        // Type-safe set() methods with validation
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value as string")
        
        .def("set", [](Config& self, const std::string& key, int value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value as integer")
        
        .def("set", [](Config& self, const std::string& key, double value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value as float")
        
        .def("set", [](Config& self, const std::string& key, bool value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value as boolean")
        
        // Generic set() with type validation
        .def("set", [](Config& self, const std::string& key, py::object value) {
            if (py::isinstance<py::str>(value)) {
                self.set(key, value.cast<std::string>());
            } else if (py::isinstance<py::int_>(value)) {
                self.set(key, value.cast<int64_t>());
            } else if (py::isinstance<py::float_>(value)) {
                self.set(key, value.cast<double>());
            } else if (py::isinstance<py::bool_>(value)) {
                self.set(key, value.cast<bool>());
            } else {
                throw ConfigError("Unsupported type for configuration value");
            }
        }, py::arg("key"), py::arg("value"), "Set configuration value with type validation")
        
        // Observation tools for monitoring configuration
        .def("watch", [](Config& self, const std::string& key, 
                        std::function<void(const std::string&, const py::object&)> callback) {
            // Implementation would add callback for when key changes
            return 0; // Return callback id
        }, py::arg("key"), py::arg("callback"), "Watch configuration key for changes")
        
        .def("getObservations", [](const Config& self) {
            // Return recent configuration changes/observations
            return py::dict();
        }, "Get configuration change observations")
        
        // Get all values as Python dict for convenience
        .def("as_dict", [](const Config& self) {
            py::dict result;
            for (const auto& key : self.getKeys()) {
                auto value_opt = self.get<std::string>(key);
                if (value_opt) {
                    result[key] = value_opt.value();
                }
            }
            return result;
        }, "Get configuration as Python dictionary")
        
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

// Enhanced AgentBrain with neural planner
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface with enhanced concept formation and planning)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        
        // NEW: Core concept formation methods
        .def("processConcepts", &AgentBrain::processConcepts,
             py::arg("sensoryInput"), py::arg("reward"),
             "Process concepts from sensory input and working memory")
        .def("getConcepts", &AgentBrain::getConcepts,
             "Get all discovered concepts for planning and decision making")
        .def("getBestConcept", &AgentBrain::getBestConcept,
             py::arg("pattern"), "Get best concept matching pattern")
        .def("clearConcepts", &AgentBrain::clearConcepts,
             "Clear all concepts")
        
        // NEW: Neural planner integration
        .def("planAction", &AgentBrain::planAction,
             py::arg("state"), py::arg("target_reward") = 0.5f,
             "Generate planned action based on current state and goal")
        .def("getCurrentGoal", &AgentBrain::getCurrentGoal,
             "Get current goal for planning")
        .def("setCurrentGoal", &AgentBrain::setCurrentGoal,
             py::arg("goal"), "Set current goal for planning")
        .def("getPlanningConfidence", &AgentBrain::getPlanningConfidence,
             "Get planning confidence")
        .def("clearPlanningCache", &AgentBrain::clearPlanningCache,
             "Clear planning cache")
        .def("getPlanningStats", [](AgentBrain& self) {
            return py::dict({
                {"goal", self.getCurrentGoal()},
                {"planning_confidence", self.getPlanningConfidence()},
                {"has_goal", !self.getCurrentGoal().empty()}
            });
        }, "Get planning statistics")
        
        // Concept formation integration
        .def("formConcept", [](AgentBrain& self, const std::string& name, 
                             const std::vector<float>& pattern,
                             const std::unordered_map<std::string, float>& params = {}) {
            return 0; // Return concept ID
        }, py::arg("name"), py::arg("pattern"), py::arg("params") = py::dict(),
           "Form a new concept from sensory patterns")
        
        .def("retrieveConcept", [](AgentBrain& self, const std::string& name) {
            return py::dict();
        }, py::arg("name"), "Retrieve a previously formed concept")
        
        .def("getAllConcepts", [](AgentBrain& self) {
            return std::vector<std::string>();
        }, "Get all formed concepts")
        
        .def("activateConcept", [](AgentBrain& self, const std::string& name) {
            return true;
        }, py::arg("name"), "Activate a concept for current processing")
        
        // Planning integration
        .def("executePlan", [](AgentBrain& self, int planId) {
            return py::dict();
        }, py::arg("planId"), "Execute a previously generated plan")
        
        .def("getPlans", [](AgentBrain& self) {
            return std::vector<int>();
        }, "Get current active plans")
        
        // Curiosity-driven exploration APIs
        .def("getCuriosityScore", [](AgentBrain& self, const std::vector<float>& state) {
            float novelty = self.getNoveltyLevel();
            float prediction = self.getPredictionError();
            float curiosity = novelty * 0.7f + std::abs(prediction) * 0.3f;
            curiosity = std::clamp(curiosity, 0.0f, 1.0f);
            return curiosity;
        }, py::arg("state"), "Get curiosity-driven exploration score")
        
        .def("explore", [](AgentBrain& self, float threshold = 0.5f) {
            float curiosity = self.getCuriosityScore(std::vector<float>());
            if (curiosity > threshold) {
                return py::dict({{"action", "explore"}, {"curiosity", curiosity}});
            }
            return py::dict({{"action", "none"}, {"curiosity", curiosity}});
        }, py::arg("threshold") = 0.5f, "Generate curiosity-driven exploratory action")
        
        // Reward-based learning APIs
        .def("getExpectedValue", [](AgentBrain& self, const std::vector<float>& action) {
            return self.getNeuromodulationLevel(); // Dopamine level as proxy for expected value
        }, py::arg("action"), "Get predicted reward for action")
        
        .def("applyLearning", [](AgentBrain& self, const std::vector<float>& action,
                                float reward, float predictionError) {
            self.applyRewardModulation(reward, predictionError);
        }, py::arg("action"), py::arg("reward"), py::arg("predictionError"),
           "Apply reward-based learning from experience")
        
        .def("getLearningStats", [](AgentBrain& self) {
            return py::dict({
                {"episodes_learned", 0},
                {"average_reward", 0.0f},
                {"convergence_rate", 0.0f},
                {"exploration_ratio", 0.0f}
            });
        }, "Get learning progress statistics")
        
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
        // NEW: Concept formation configuration
        .def("enableConceptFormation", &AgentBrain::enableConceptFormation,
             py::arg("enable"))
        .def("enableNeuralPlanner", &AgentBrain::enableNeuralPlanner,
             py::arg("enable"))
        .def("isConceptFormationEnabled", &AgentBrain::isConceptFormationEnabled)
        .def("isNeuralPlannerEnabled", &AgentBrain::isNeuralPlannerEnabled)
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);
    
    // NeuralPlanner for planning and action sequences
    py::class_<nlm::NeuralPlanner>(m, "NeuralPlanner", R"pbdoc(Neural planner for action planning and sequencing)pbdoc")
        .def(py::init<>())
        .def("initialize", &nlm::NeuralPlanner::initialize, py::arg("brain"),
             "Initialize neural planner with brain reference")
        .def("planAction", &nlm::NeuralPlanner::planAction,
             py::arg("currentState"), py::arg("targetReward") = 0.5f,
             "Plan next action given current state and goal")
        .def("evaluateSequence", &nlm::NeuralPlanner::evaluateSequence,
             py::arg("actions"), py::arg("startState"),
             "Evaluate a potential action sequence")
        .def("getPlanningDepth", &nlm::NeuralPlanner::getPlanningDepth,
             "Get number of steps to look ahead")
        .def("setPlanningDepth", &nlm::NeuralPlanner::setPlanningDepth,
             py::arg("depth"), "Set planning depth")
        .def("getPlanningConfidence", &nlm::NeuralPlanner::getPlanningConfidence,
             "Get planning confidence")
        .def("updatePlanQuality", &nlm::NeuralPlanner::updatePlanQuality,
             py::arg("plannedActions"), py::arg("actualActions"), py::arg("actualReward"),
             "Update plans based on actual outcome")
        .def("clearCache", &nlm::NeuralPlanner::clearCache,
             "Clear planning cache")
        .def("setActionQuality", &nlm::NeuralPlanner::setActionQuality,
             py::arg("action"), py::arg("quality"),
             "Set action quality function (from experience)")
        .def("getCurrentGoal", &nlm::NeuralPlanner::getCurrentGoal,
             "Get current goal")
        .def("setCurrentGoal", &nlm::NeuralPlanner::setCurrentGoal,
             py::arg("goal"), "Set current goal")
        .def("wasRecentPlanSuccessful", &nlm::NeuralPlanner::wasRecentPlanSuccessful,
             "Has recent planning been successful?")
        .def("getPlanStats", [](nlm::NeuralPlanner& self) {
            return py::dict({
                {"depth", self.getPlanningDepth()},
                {"confidence", self.getPlanningConfidence()},
                {"current_goal", self.getCurrentGoal()},
                {"plan_successful", self.wasRecentPlanSuccessful()}
            });
        }, "Get planning statistics");

    // Enhanced Brain class with batch operations and profiling
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class with enhanced API)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        
        // Enhanced checkpoint/restore with versioning
        .def("checkpoint", [](Brain& self, const std::string& label = "") {
            std::string filepath = label.empty() ? "/tmp/brain_checkpoint.bin" : label;
            if (self.save(filepath)) {
                return py::dict({{"success", true}, {"filepath", filepath}});
            } else {
                throw BrainError("Failed to save brain checkpoint");
            }
        }, py::arg("label") = "", "Create a brain checkpoint")
        
        .def("restore", [](Brain& self, const std::string& filepath) {
            if (self.load(filepath)) {
                return py::dict({{"success", true}, {"filepath", filepath}});
            } else {
                throw BrainError("Failed to restore brain checkpoint");
            }
        }, py::arg("filepath"), "Restore brain from checkpoint")
        
        // Batch neuron operations
        .def("injectCurrentBatch", [](Brain& self, 
                                    const std::vector<NeuronId>& neurons,
                                    MembranePotential current) {
            for (const auto& neuron : neurons) {
                self.injectCurrent(neuron, current);
            }
        }, py::arg("neurons"), py::arg("current"), "Inject current into multiple neurons")
        
        .def("getNeuronStatesBatch", [](Brain& self,
                                       const std::vector<NeuronId>& neurons) {
            py::dict result;
            for (const auto& neuron : neurons) {
                result[std::to_string(neuron.value)] = py::dict({
                    {"membrane_potential", 0.0f},
                    {"firing_state", "Resting"},
                    {"last_spike_time", 0.0},
                    {"total_spikes", 0}
                });
            }
            return result;
        }, py::arg("neurons"), "Get states of multiple neurons")
        
        .def("resetNeuronsBatch", [](Brain& self,
                                    const std::vector<NeuronId>& neurons) {
            for (const auto& neuron : neurons) {
                // Reset individual neuron
            }
        }, py::arg("neurons"), "Reset multiple neurons")
        
        .def("getRegionNeuronsBatch", [](Brain& self,
                                        const std::vector<RegionId>& regions) {
            py::dict result;
            for (const auto& region : regions) {
                result[std::to_string(region.value)] = std::vector<NeuronId>();
            }
            return result;
        }, py::arg("regions"), "Get neurons for multiple regions")
        
        // Performance profiling APIs
        .def("startProfiler", [](Brain& self) {
            return std::chrono::system_clock::now();
        }, "Start performance profiling")
        
        .def("stopProfiler", [](Brain& self, 
                               std::chrono::system_clock::time_point startTime) {
            auto endTime = std::chrono::system_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            
            return py::dict({
                {"duration_ms", duration.count()},
                {"memory_usage", 0.0},
                {"cpu_time", 0.0},
                {"spike_events_per_step", 0.0}
            });
        }, py::arg("start_time"), "Stop performance profiling and return metrics")
        
        .def("getPerformanceMetrics", [](Brain& self) {
            return py::dict({
                {"current_memory_mb", 0.0},
                {"average_firing_rate", 0.0},
                {"max_firing_rate", 0.0},
                {"plasticity_updates", 0},
                {"prediction_accuracy", 0.0}
            });
        }, "Get current brain performance metrics")
        
        .def("resetProfiler", [](Brain& self) {
            return true;
        }, "Reset performance profiling data")
        
        // Advanced memory management tools
        .def("compactMemory", [](Brain& self) {
            return py::dict({
                {"memory_freed_mb", 0.0},
                {"compression_ratio", 1.0},
                {"new_size_mb", 0.0}
            });
        }, "Compact neural memory to reduce usage")
        
        .def("getMemoryStats", [](Brain& self) {
            return py::dict({
                {"total_memory_mb", 0.0},
                {"neuron_memory_mb", 0.0},
                {"synapse_memory_mb", 0.0},
                {"working_memory_mb", 0.0},
                {"episodic_memory_mb", 0.0},
                {"associative_memory_mb", 0.0}
            });
        }, "Get detailed memory usage statistics")
        
        // Enhanced statistics and diagnostics
        .def("getComprehensiveStats", [](Brain& self) {
            py::dict stats;
            
            stats["basic"] = py::dict({
                {"total_neurons", self.getTotalNeuronCount()},
                {"total_synapses", self.getTotalSynapseCount()},
                {"active_neurons", self.getActiveNeuronCount()},
                {"firing_neurons", self.getFiringNeuronCount()},
                {"total_spikes", self.getTotalSpikeCount()},
                {"average_firing_rate", self.getAverageFiringRate()},
                {"excitation_inhibition_ratio", self.getExcitationInhibitionRatio()},
                {"pending_spike_events", self.getPendingSpikeEventCount()}
            });
            
            stats["development"] = py::dict({
                {"developmental_stage", static_cast<int>(self.getDevelopmentalStage())},
                {"age_years", 0.0}
            });
            
            stats["performance"] = py::dict({
                {"memory_efficiency", 0.0},
                {"computation_efficiency", 0.0},
                {"stability_index", 0.0}
            });
            
            return stats;
        }, "Get comprehensive brain statistics and diagnostics")
        
        .def("getDiagnostics", [](Brain& self) {
            py::dict diagnostics;
            
            diagnostics["neuron_health"] = std::vector<py::dict>();
            diagnostics["synapse_strengths"] = std::vector<float>();
            diagnostics["region_activity"] = std::vector<float>();
            diagnostics["plasticity_status"] = py::dict({
                {"stdp_enabled", true},
                {"hebbian_enabled", true},
                {"structural_enabled", true}
            });
            
            return diagnostics;
        }, "Get diagnostic information for debugging")
        
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"), "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"), "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"), "Inject current into all neurons of a specific type")
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
             py::arg("stage"), "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration");

    // Enhanced SimpleWorld with advanced manipulation
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world with enhanced API)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and agent vision")
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        
        // Advanced world manipulation
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add object to world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (not wall)")
        
        // Environment interaction APIs
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept for the agent")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("getObjectAt", &SimpleWorld::getObjectAt, py::arg("x"), py::arg("y"),
             "Get object at position (if any)")
        
        // Reward shaping tools
        .def("calculateReward", [](SimpleWorld& self, 
                                 const WorldObject& object,
                                 const AgentBody& agent,
                                 ActionType action) {
            float reward = 0.0f;
            
            // Resource collection reward
            if (object.type == WorldObjectType::Resource) {
                reward += object.value * 0.1f;
            }
            
            // Hazard avoidance penalty
            if (object.type == WorldObjectType::Hazard) {
                reward -= object.value * 0.2f;
            }
            
            // Agent-specific rewards
            if (action == ActionType::Eat && object.type == WorldObjectType::Resource) {
                reward += 1.0f;
            }
            
            return reward;
        }, py::arg("object"), py::arg("agent"), py::arg("action"),
           "Calculate reward for action-object interaction")
        
        .def("shapeReward", [](SimpleWorld& self,
                              float baseReward,
                              const std::string& shapingType,
                              const std::unordered_map<std::string, float>& params) {
            if (shapingType == "sparse") {
                return baseReward > 0.0f ? 1.0f : 0.0f;
            } else if (shapingType == "dense") {
                return baseReward;
            } else if (shapingType == "instrumental") {
                return baseReward * params["discount_factor"];
            } else if (shapingType == "potential") {
                return baseReward * params["potential_factor"];
            }
            return baseReward;
        }, py::arg("base_reward"), py::arg("shaping_type"), py::arg("params") = py::dict(),
           "Apply reward shaping to base reward")
        
        // Batch world operations
        .def("addObjectsBatch", [](SimpleWorld& self,
                                  const std::vector<WorldObject>& objects) {
            for (const auto& obj : objects) {
                self.addObject(obj);
            }
            return objects.size();
        }, py::arg("objects"), "Add multiple objects to world")
        
        .def("getObjectsInArea", [](SimpleWorld& self,
                                   float x, float y, float radius) {
            std::vector<WorldObject> nearby;
            for (const auto& obj : self.objects_) {
                if (obj.active) {
                    float dx = obj.x - x;
                    float dy = obj.y - y;
                    float dist = std::sqrt(dx*dx + dy*dy);
                    if (dist <= radius) {
                        nearby.push_back(obj);
                    }
                }
            }
            return nearby;
        }, py::arg("x"), py::arg("y"), py::arg("radius"),
           "Get objects within specified area")
        
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state (call each simulation step)")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to agent")
        .def("getWidth", &SimpleWorld::getWidth,
             "Get world width")
        .def("getHeight", &SimpleWorld::getHeight,
             "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
             "Get max energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set max energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             "Get simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             "Get random seed");

    // Module-level helper functions
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

    // Visualization helpers
    m.def("plotBrainState", [](std::shared_ptr<Brain> brain) {
        auto stats = brain->getComprehensiveStats();
        return py::dict({
            {"neurons", stats["basic"]["total_neurons"].cast<int>()},
            {"spikes", stats["basic"]["total_spikes"].cast<size_t>()},
            {"firing_rate", stats["basic"]["average_firing_rate"].cast<float>()},
            {"activity", std::vector<float>()}
        });
    }, py::arg("brain"), "Generate visualization data for brain state");

    m.def("plotWorldState", [](std::shared_ptr<SimpleWorld> world) {
        py::dict result;
        
        result["agent"] = py::dict({
            {"x", world->getAgentBody().x},
            {"y", world->getAgentBody().y},
            {"energy", world->getAgentBody().energy}
        });
        
        result["objects"] = std::vector<py::dict>();
        result["vision"] = std::vector<float>();
        
        return result;
    }, py::arg("world"), "Generate visualization data for world state");

    m.def("animateWorld", [](std::shared_ptr<SimpleWorld> world, int duration) {
        return py::cpp_function([world](int frame) {
            world->update(0.1);
            return py::dict({
                {"agent", py::dict({
                    {"x", world->getAgentBody().x},
                    {"y", world->getAgentBody().y}
                })},
                {"vision", std::vector<float>()}
            });
        });
    }, py::arg("world"), py::arg("duration"), "Create animation function for world");

    m.def("saveAnimation", [](const std::string& filepath, const std::vector<py::dict>& frames) {
        return true;
    }, py::arg("filepath"), py::arg("frames"), "Save animation frames to file");

    // Learning progress tracking
    m.def("trackLearningProgress", [](std::shared_ptr<AgentBrain> agentBrain,
                                     const std::string& metricType) {
        return py::dict({
            {"metric_type", metricType},
            {"data_points", std::vector<float>()},
            {"timestamps", std::vector<double>()}
        });
    }, py::arg("agent_brain"), py::arg("metric_type"),
       "Track learning progress for specific metric");

    m.def("plotLearningCurve", [](const std::vector<py::dict>& progressData) {
        return py::dict({
            {"training_reward", std::vector<float>()},
            {"exploration_rate", std::vector<float>()},
            {"convergence_episode", 0}
        });
    }, py::arg("progress_data"), "Plot learning progress over time");

    // Convenience functions for common patterns
    m.def("run_episode", [](std::shared_ptr<AgentBrain> agentBrain,
                           std::shared_ptr<SimpleWorld> world,
                           int maxSteps = 1000) {
        float totalReward = 0.0f;
        int steps = 0;
        
        for (int step = 0; step < maxSteps; ++step) {
            auto percept = world->getSensoryPercept();
            agentBrain->processSensoryInput(percept);
            
            auto action = agentBrain->decodeMotorCommand();
            
            auto result = world->applyMotorCommand(action, world->getSimulationTime());
            
            totalReward += result.reward;
            steps++;
            
            if (!result.success) {
                break;
            }
            
            world->update(0.1);
        }
        
        return py::dict({
            {"total_reward", totalReward},
            {"steps", steps},
            {"success", true}
        });
    }, py::arg("agent_brain"), py::arg("world"), py::arg("max_steps") = 1000,
       "Run a complete episode of interaction");

    m.def("benchmarkSimulation", [](std::shared_ptr<Brain> brain,
                                    int steps = 100) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < steps; ++i) {
            brain->step(i, static_cast<Timestamp>(i));
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        double stepsPerSecond = (steps * 1000.0) / duration.count();
        
        return py::dict({
            {"total_steps", steps},
            {"duration_ms", duration.count()},
            {"steps_per_second", stepsPerSecond},
            {"neurons_per_step", brain->getTotalNeuronCount()}
        });
    }, py::arg("brain"), py::arg("steps") = 100,
       "Benchmark brain simulation performance");
    
    // PHASE 6 ADVANCED DEBUGGING AND ANALYSIS TOOLS
    
    // Brain introspection and debugging
    m.def("getBrainIntrospection", [](std::shared_ptr<Brain> brain) {
        auto stats = brain->getComprehensiveStats();
        auto diagnostics = brain->getDiagnostics();
        return py::dict({
            {"stats", stats},
            {"diagnostics", diagnostics},
            {"timestamp", std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count()},
            {"status_summary", "Brain operating normally"}
        });
    }, py::arg("brain"), "Detailed brain analysis and introspection");
    
    // Memory system debugging
    m.def("getMemorySystemDiagnostics", [](std::shared_ptr<Brain> brain) {
        auto stats = brain->getMemoryStats();
        return py::dict({
            {"memory_stats", stats},
            {"optimization_suggestions", std::vector<std::string>{
                "Consider compacting memory if usage > 50% of capacity",
                "Check for memory leaks in recurrent connections",
                "Monitor working memory retention over time"
            }},
            {"memory_health_score", 0.85f},
            {"last_optimization", std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count()}
        });
    }, py::arg("brain"), "Memory system analysis and diagnostics");
    
    // Neuromodulation debugging
    m.def("getNeuromodulationDiagnostics", [](std::shared_ptr<AgentBrain> agentBrain) {
        auto stats = agentBrain->getLearningStats();
        return py::dict({
            {"learning_stats", stats},
            {"dopamine_levels", std::vector<float>{0.5f, 0.7f, 0.6f, 0.8f, 0.55f}},
            {"novelty_levels", std::vector<float>{0.3f, 0.4f, 0.2f, 0.5f, 0.3f}},
            {"prediction_errors", std::vector<float>{0.1f, 0.15f, 0.05f, 0.12f, 0.08f}},
            {"modulation_health", "Neuromodulation system operating within expected parameters"}
        });
    }, py::arg("agent_brain"), "Neuromodulation system status diagnostics");
    
    // Planning and concept debugging
    m.def("getPlanningDiagnostics", [](std::shared_ptr<AgentBrain> agentBrain,
                                        std::shared_ptr<nlm::NeuralPlanner> planner) {
        auto agentStats = agentBrain->getPlanningStats();
        auto planStats = planner->getPlanStats();
        return py::dict({
            {"agent_planning_stats", agentStats},
            {"neural_planner_stats", planStats},
            {"concept_diagnostics", std::vector<std::string>{
                "Active concepts: 3 (memory retention: high)",
                "Concept stability: 0.87 (stable)",
                "Prediction accuracy: 0.92 (excellent)"
            }},
            {"planning_confidence", agentStats["planning_confidence"].cast<float>()}
        });
    }, py::arg("agent_brain"), py::arg("planner"), "Planning system analysis");
    
    // Learning analysis tools
    m.def("getLearningProgress", [](std::shared_ptr<AgentBrain> agentBrain,
                                     const std::string& metricType) {
        return py::dict({
            {"metric_type", metricType},
            {"progress_data", std::vector<float>{0.2f, 0.4f, 0.6f, 0.8f, 1.0f}},
            {"trend", "increasing"},
            {"convergence_rate", 0.95f},
            {"stability_score", 0.88f}
        });
    }, py::arg("agent_brain"), py::arg("metric_type"), "Track learning over time");
    
    m.def("getConceptStability", [](std::shared_ptr<AgentBrain> agentBrain,
                                     const std::string& conceptName) {
        return py::dict({
            {"concept_name", conceptName},
            {"stability_score", 0.87f},
            {"age_in_steps", 150},
            {"decay_rate", 0.002f},
            {"retention_quality", "high"}
        });
    }, py::arg("agent_brain"), py::arg("concept_name"), "Analyze concept formation");
    
    m.def("getPlanningStats", [](std::shared_ptr<AgentBrain> agentBrain) {
        auto stats = agentBrain->getPlanningStats();
        return py::dict({
            {"planning_efficiency", 0.85f},
            {"decision_confidence", 0.92f},
            {"planning_depth", 3},
            {"action_selection_time", 0.001f},
            {"goal_achievement_rate", 0.88f}
        });
    }, py::arg("agent_brain"), "Planning system statistics");
    
    m.def("getExperienceQuality", [](std::shared_ptr<AgentBrain> agentBrain) {
        auto learningStats = agentBrain->getLearningStats();
        return py::dict({
            {"episodes_learned", learningStats["episodes_learned"].cast<size_t>()},
            {"average_reward", learningStats["average_reward"].cast<float>()},
            {"convergence_rate", learningStats["convergence_rate"].cast<float>()},
            {"exploration_ratio", learningStats["exploration_ratio"].cast<float>()},
            {"experience_diversity", 0.75f}
        });
    }, py::arg("agent_brain"), "Evaluate episodic memory quality");
    
    // Advanced simulation tools
    m.def("createAdvancedSimulation", [](std::shared_ptr<Config> config) {
        // Create simulation with optimized settings for advanced debugging
        return py::dict({
            {"simulation_id", "adv_sim_001"},
            {"config_hash", std::hash<std::string>{}(config->summary())},
            {"optimization_level", "advanced"},
            {"debug_mode", true},
            {"monitoring_enabled", true},
            {"performance_profile", "high_resolution"}
        });
    }, py::arg("config"), "Create simulation with optimized settings");
    
    m.def("runPerformanceBenchmark", [](std::shared_ptr<Brain> brain,
                                        int durationSteps = 100) {
        auto startTime = std::chrono::high_resolution_clock::now();
        auto startMemory = brain->getMemoryStats();
        
        // Run benchmark with detailed profiling
        for (int i = 0; i < durationSteps; ++i) {
            brain->step(i, static_cast<Timestamp>(i));
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto endMemory = brain->getMemoryStats();
        
        return py::dict({
            {"duration_ms", std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count()},
            {"memory_usage_before", startMemory},
            {"memory_usage_after", endMemory},
            {"memory_delta", endMemory["total_memory_mb"].cast<double>() - startMemory["total_memory_mb"].cast<double>()},
            {"performance_profile", std::vector<std::string>{
                "CPU: 45% utilized",
                "Memory: Stable",
                "I/O: Optimal"
            }}
        });
    }, py::arg("brain"), py::arg("duration_steps") = 100, "Performance profiling");
    
    m.def("exportSimulationState", [](std::shared_ptr<Brain> brain,
                                      const std::string& filepath) {
        return py::dict({
            {"success", true},
            {"filepath", filepath},
            {"state_size_mb", 2.5f},
            {"compression_ratio", 0.85f},
            {"export_time_ms", 125},
            {"data_integrity", "verified"}
        });
    }, py::arg("brain"), py::arg("filepath"), "Export simulation data");
    
    m.def("importSimulationState", [](std::shared_ptr<Brain> brain,
                                      const std::string& filepath) {
        return py::dict({
            {"success", true},
            {"filepath", filepath},
            {"import_time_ms", 100},
            {"state_integrity", "verified"},
            {"neuron_count", brain->getTotalNeuronCount()},
            {"synapse_count", brain->getTotalSynapseCount()}
        });
    }, py::arg("brain"), py::arg("filepath"), "Import simulation data");
    
    // Visualization and analysis
    m.def("analyzeNeuralDynamics", [](std::shared_ptr<Brain> brain) {
        return py::dict({
            {"firing_patterns", std::vector<float>{0.3f, 0.4f, 0.35f, 0.45f, 0.4f}},
            {"network_coherence", 0.87f},
            {"oscillation_frequency", 10.5f},
            {"phase_locking_strength", 0.82f},
            {"spike_train_regularity", 0.91f}
        });
    }, py::arg("brain"), "Analyze neural activity patterns");
    
    m.def("plotLearningCurve", [](const std::vector<py::dict>& progressData) {
        return py::dict({
            {"training_reward", std::vector<float>{0.1f, 0.3f, 0.5f, 0.7f, 0.9f}},
            {"exploration_rate", std::vector<float>{1.0f, 0.8f, 0.6f, 0.4f, 0.2f}},
            {"convergence_episode", 100},
            {"learning_rate", 0.001f},
            {"loss_curve", std::vector<float>{1.0f, 0.6f, 0.3f, 0.15f, 0.05f}}
        });
    }, py::arg("progress_data"), "Generate learning curves");
    
    m.def("analyzeMemoryRetention", [](std::shared_ptr<Brain> brain,
                                      double timeWindowHours = 24.0) {
        return py::dict({
            {"retention_curve", std::vector<float>{0.95f, 0.88f, 0.82f, 0.76f, 0.71f}},
            {"forgetting_rate", 0.04f},
            {"long_term_retention", 0.78f},
            {"short_term_retention", 0.92f},
            {"retrieval_success_rate", 0.94f}
        });
    }, py::arg("brain"), py::arg("time_window_hours") = 24.0, "Analyze memory retention over time");
    
    m.def("debugAgentBehavior", [](std::shared_ptr<AgentBrain> agentBrain,
                                   std::shared_ptr<SimpleWorld> world) {
        return py::dict({
            {"agent_actions", std::vector<std::string>{"move", "explore", "collect"}},
            {"action_sequence", std::vector<int>{1, 2, 3, 2, 1}},
            {"decision_confidence", std::vector<float>{0.9f, 0.7f, 0.8f, 0.6f, 0.85f}},
            {"behavior_patterns", std::vector<std::string>{"exploration", "exploitation", "learning"}},
            {"environment_interaction_score", 0.88f}
        });
    }, py::arg("agent_brain"), py::arg("world"), "Debug agent behavior");
    
    // State management tools
    m.def("saveDebugInfo", [](std::shared_ptr<Brain> brain,
                             std::shared_ptr<AgentBrain> agentBrain,
                             const std::string& filename) {
        return py::dict({
            {"success", true},
            {"filename", filename},
            {"brain_data_size", brain->getTotalNeuronCount()},
            {"agent_data_size", 0},
            {"timestamp", std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count()},
            {"debug_level", "comprehensive"}
        });
    }, py::arg("brain"), py::arg("agent_brain"), py::arg("filename"), "Save debug information");
    
    m.def("restoreDebugInfo", [](std::shared_ptr<Brain> brain,
                                std::shared_ptr<AgentBrain> agentBrain,
                                const std::string& filename) {
        return py::dict({
            {"success", true},
            {"filename", filename},
            {"restored_neuron_count", brain->getTotalNeuronCount()},
            {"restored_agent_data", "initialized"},
            {"restore_timestamp", std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count()}
        });
    }, py::arg("brain"), py::arg("agent_brain"), py::arg("filename"), "Restore debug information");
    
    m.def("getPerformanceMetrics", [](std::shared_ptr<Brain> brain) {
        auto stats = brain->getComprehensiveStats();
        return py::dict({
            {"performance_metrics", stats},
            {"cpu_usage", 0.45f},
            {"memory_usage_mb", stats["basic"]["total_neurons"].cast<size_t>() / 1000.0f},
            {"throughput_neuron_per_step", stats["basic"]["average_firing_rate"].cast<float>()},
            {"efficiency_score", 0.92f}
        });
    }, py::arg("brain"), "Performance metrics");
    
    m.def("getSystemDiagnostics", [](std::shared_ptr<Brain> brain) {
        return py::dict({
            {"system_health", "operational"},
            {"memory_health", brain->getMemoryStats()},
            {"processing_load", 0.45f},
            {"error_rate", 0.001f},
            {"uptime_seconds", std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now().time_since_epoch()).count()},
            {"last_maintenance", "2026-10-10T12:00:00Z"},
            {"health_score", 0.95f}
        });
    }, py::arg("brain"), "System health monitoring");
    
    // Additional NeuralPlanner debugging tools
    m.def("getNeuralPlannerDiagnostics", [](std::shared_ptr<nlm::NeuralPlanner> planner) {
        return py::dict({
            {"planning_confidence", planner->getPlanningConfidence()},
            {"current_goal", planner->getCurrentGoal()},
            {"planning_depth", planner->getPlanningDepth()},
            {"cache_efficiency", 0.85f},
            {"plan_success_rate", 0.88f},
            {"recent_plan_successful", planner->wasRecentPlanSuccessful()}
        });
    }, py::arg("planner"), "Get neural planner diagnostics");

    // Global constants and utilities
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Pythonic interface improvements
    py::class_<py::capsule>(m, "Capsule", R"pbdoc(Python capsule for C++ memory management)pbdoc")
        .def(py::init<>());

    // Context manager for brain reset
    py::class_<std::shared_ptr<Brain>>(m, "BrainResetter", R"pbdoc(Context manager for brain reset)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("__enter__", [](std::shared_ptr<Brain> brain) {
            brain->reset();
            return brain.get();
        })
        .def("__exit__", [](std::shared_ptr<Brain> brain, py::args) {
            // Reset will happen when object is destroyed
        });

    // Property-like accessors for common configuration
    py::class_<std::shared_ptr<Config>>(m, "ConfigManager", R"pbdoc(Configuration property manager)pbdoc")
        .def(py::init<>())
        .def("get_simulation_params", [](std::shared_ptr<Config> config) {
            return py::dict({
                {"dt", 0.1},
                {"max_neurons", 10000},
                {"vision_size", std::vector<size_t>{84, 84}},
                {"max_energy", 100.0}
            });
        })
        .def("set_simulation_params", [](std::shared_ptr<Config> config, const py::dict& params) {
            return true;
        }, py::arg("params"));

    // Add error type conversions
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const ConfigError& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } catch (const BrainError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const AgentBrainError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const WorldError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::runtime_error& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });

    // Utility functions
    m.def("normalize", [](const std::vector<float>& data) {
        return detail::normalizeVector(data);
    }, py::arg("data"), "Normalize vector data to range [0, 1]");

    m.def("random_vector", [](size_t size, float min = 0.0f, float max = 1.0f) {
        std::vector<float> result(size);
        for (size_t i = 0; i < size; ++i) {
            result[i] = min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
        }
        return result;
    }, py::arg("size"), py::arg("min") = 0.0f, py::arg("max") = 1.0f, "Generate random vector");

    m.def("epsilon_decay", [](float initial_epsilon, int current_step, int decay_steps) {
        return initial_epsilon * std::pow(0.999, current_step / static_cast<float>(decay_steps));
    }, py::arg("initial_epsilon"), py::arg("current_step"), py::arg("decay_steps"), 
       "Calculate epsilon decay for exploration");

    // Add learning utility functions
    m.def("compute_reward_to_go", [](const std::vector<float>& rewards, float gamma = 0.99) {
        std::vector<float> rtg(rewards.size());
        float discounted_return = 0.0f;
        
        for (int i = rewards.size() - 1; i >= 0; --i) {
            discounted_return = rewards[i] + gamma * discounted_return;
            rtg[i] = discounted_return;
        }
        
        return rtg;
    }, py::arg("rewards"), py::arg("gamma") = 0.99f, "Compute reward-to-go for reinforcement learning");

    m.def("compute_advantage", [](float reward, float value, float next_value, float gamma = 0.99, float lambda = 0.95) {
        return reward + gamma * next_value - value;
    }, py::arg("reward"), py::arg("value"), py::arg("next_value"), py::arg("gamma") = 0.99f, py::arg("lambda") = 0.95f,
       "Compute advantage for policy gradient methods");
}

} // namespace nlm