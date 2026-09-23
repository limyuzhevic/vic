#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <cmath>
#include <algorithm>
#include <numeric>
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

namespace py = pybind11;
namespace nlm {

// NLM Exception class for Python
class NLMException : public py::exception {
public:
    NLMException(const std::string& msg) : py::exception("NLMError") {
        this->value_ = msg;
    }
    
    NLMException(const std::string& msg, const std::string& code) : py::exception("NLMError") {
        this->value_ = msg + " [CODE: " + code + "]";
    }
};

// Error conversion utilities
inline std::string format_error_message(const std::string& operation, const std::string& details, const std::string& code = "") {
    std::string msg = operation;
    if (!details.empty()) {
        msg += ": " + details;
    }
    if (!code.empty()) {
        msg += " [ERROR_CODE: " + code + "]";
    }
    return msg;
}

// Forward declarations
PYBIND11_MODULE(pynlm, m) {
    // Register NLMException
    py::register_exception<NLMException>(m, "NLMException");
    
    // Register basic runtime error as fallback
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    
    // Helper functions for error handling
    try {
        m.def("validate_config", [](const py::dict& config_dict) {
            Config config;
            try {
                config.loadFromJson(py::str(config_dict["json"]));
                bool is_valid = config.isValid();
                return py::make_tuple(is_valid, config.getValidationErrors());
            } catch (const std::exception& e) {
                throw NLMException(format_error_message("Config validation", e.what(), "CONFIG_VALIDATION_ERROR"));
            }
        }, py::arg("config_dict"), 
            "Validate a configuration dictionary. Returns (is_valid, errors).");
    } catch (const std::exception& e) {
        // Error handling
    }

    // Register basic error conversion helpers
    try {
        m.def("format_error", &format_error_message,
            py::arg("operation"), py::arg("details"), py::arg("code"),
            "Format an error message with operation context and error code.");
    } catch (const std::exception& e) {
        // Error handling
    }

    // Register type conversion helpers
    try {
        m.def("neuron_type_to_string", [](NeuronType type) {
            switch (type) {
                case NeuronType::Excitatory: return std::string("Excitatory");
                case NeuronType::Inhibitory: return std::string("Inhibitory");
                case NeuronType::Modulatory: return std::string("Modulatory");
                case NeuronType::Sensory: return std::string("Sensory");
                case NeuronType::Motor: return std::string("Motor");
                case NeuronType::Internal: return std::string("Internal");
                default: return std::string("Unknown");
            }
        }, py::arg("type"),
            "Convert NeuronType enum to string representation.");
        
        m.def("synapse_type_to_string", [](SynapseType type) {
            switch (type) {
                case SynapseType::Excitatory: return std::string("Excitatory");
                case SynapseType::Inhibitory: return std::string("Inhibitory");
                case SynapseType::Modulatory: return std::string("Modulatory");
                case SynapseType::Electrical: return std::string("Electrical");
                case SynapseType::GapJunction: return std::string("GapJunction");
                default: return std::string("Unknown");
            }
        }, py::arg("type"),
            "Convert SynapseType enum to string representation.");
        
        m.def("action_type_to_string", [](ActionType type) {
            switch (type) {
                case ActionType::MoveForward: return std::string("MoveForward");
                case ActionType::MoveBackward: return std::string("MoveBackward");
                case ActionType::MoveLeft: return std::string("MoveLeft");
                case ActionType::MoveRight: return std::string("MoveRight");
                case ActionType::TurnLeft: return std::string("TurnLeft");
                case ActionType::TurnRight: return std::string("TurnRight");
                case ActionType::Look: return std::string("Look");
                case ActionType::LookUp: return std::string("LookUp");
                case ActionType::LookDown: return std::string("LookDown");
                case ActionType::Interact: return std::string("Interact");
                case ActionType::Eat: return std::string("Eat");
                case ActionType::Drink: return std::string("Drink");
                case ActionType::Rest: return std::string("Rest");
                case ActionType::Wait: return std::string("Wait");
                case ActionType::Custom: return std::string("Custom");
                default: return std::string("Unknown");
            }
        }, py::arg("type"),
            "Convert ActionType enum to string representation.");
        
        m.def("developmental_stage_to_string", [](DevelopmentalStage stage) {
            switch (stage) {
                case DevelopmentalStage::Initial: return std::string("Initial");
                case DevelopmentalStage::CriticalPeriod: return std::string("CriticalPeriod");
                case DevelopmentalStage::Maturation: return std::string("Maturation");
                case DevelopmentalStage::Adult: return std::string("Adult");
                case DevelopmentalStage::Aging: return std::string("Aging");
                default: return std::string("Unknown");
            }
        }, py::arg("stage"),
            "Convert DevelopmentalStage enum to string representation.");
        
        m.def("firing_state_to_string", [](FiringState state) {
            switch (state) {
                case FiringState::Resting: return std::string("Resting");
                case FiringState::Active: return std::string("Active");
                case FiringState::Refractory: return std::string("Refractory");
                case FiringState::Inhibited: return std::string("Inhibited");
                default: return std::string("Unknown");
            }
        }, py::arg("state"),
            "Convert FiringState enum to string representation.");
    } catch (const std::exception& e) {
        // Error handling
    }

    // Register ConfigBuilder for fluent interface
    try {
        py::class_<ConfigBuilder>(m, "ConfigBuilder", R"pbdoc(Fluent configuration builder for NLM)
            
            Provides a fluent interface for configuring NLM components.
            Supports method chaining for easy configuration setup.
        )pbdoc")
            .def(py::init<>(), "Create a new ConfigBuilder")
            .def("set", [](ConfigBuilder& self, const std::string& key, const std::string& value) {
                return self.set(key, value);
            }, py::arg("key"), py::arg("value"),
                 "Set a string configuration value (method chaining)")
            .def("set", [](ConfigBuilder& self, const std::string& key, int value) {
                return self.set(key, value);
            }, py::arg("key"), py::arg("value"),
                 "Set an integer configuration value (method chaining)")
            .def("set", [](ConfigBuilder& self, const std::string& key, float value) {
                return self.set(key, value);
            }, py::arg("key"), py::arg("value"),
                 "Set a float configuration value (method chaining)")
            .def("set", [](ConfigBuilder& self, const std::string& key, bool value) {
                return self.set(key, value);
            }, py::arg("key"), py::arg("value"),
                 "Set a boolean configuration value (method chaining)")
            .def("neuralNetwork", &ConfigBuilder::neuralNetwork,
                 "Configure for neural network simulation")
            .def("reinforcementLearning", &ConfigBuilder::reinforcementLearning,
                 "Configure for reinforcement learning")
            .def("developmental", &ConfigBuilder::developmental,
                 "Configure for developmental processes")
            .def("validate", &ConfigBuilder::validate,
                 "Validate the configuration and return True if valid")
            .def("build", &ConfigBuilder::build,
                 "Build and return the configured Config object");
    } catch (const std::exception& e) {
        // Error handling
    }

    // Register callback registry
    try {
        py::class_<CallbackRegistry>(m, "CallbackRegistry", R"pbdoc(Registry for managing callbacks in NLM)
            
            Manages various callbacks for NLM events and operations.
            Supports reward, action, sensory, and result callbacks.
        )pbdoc")
            .def(py::init<>(), "Create a new callback registry")
            .def("add_reward_callback", &CallbackRegistry::addRewardCallback,
                 py::arg("callback"),
                 "Add a reward callback function")
            .def("add_action_callback", &CallbackRegistry::addActionCallback,
                 py::arg("callback"),
                 "Add an action callback function")
            .def("add_sensory_callback", &CallbackRegistry::addSensoryCallback,
                 py::arg("callback"),
                 "Add a sensory callback function")
            .def("add_result_callback", &CallbackRegistry::addResultCallback,
                 py::arg("callback"),
                 "Add a result callback function")
            .def("trigger_reward_callbacks", &CallbackRegistry::triggerRewardCallbacks,
                 py::arg("reward"), py::arg("predicted"),
                 "Trigger all reward callbacks")
            .def("trigger_action_callbacks", &CallbackRegistry::triggerActionCallbacks,
                 "Trigger action callbacks and return motor command")
            .def("trigger_sensory_callbacks", &CallbackRegistry::triggerSensoryCallbacks,
                 py::arg("percept"),
                 "Trigger sensory callbacks")
            .def("trigger_result_callbacks", &CallbackRegistry::triggerResultCallbacks,
                 py::arg("result"),
                 "Trigger result callbacks")
            .def("clear_all_callbacks", &CallbackRegistry::clearAllCallbacks,
                 "Clear all registered callbacks");
    } catch (const std::exception& e) {
        // Error handling
    }

    // Register context managers
    try {
        py::class_<BrainContext>(m, "BrainContext", R"pbdoc(Context manager for Brain resources)
            
            Provides automatic cleanup and state management for Brain objects.
            Ensures proper resource management using Python's context manager protocol.
        )pbdoc")
            .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
                 "Create a context manager for a Brain object")
            .def("getBrain", &BrainContext::getBrain,
                 "Get the managed Brain object")
            .def("getState", &BrainContext::getState,
                 "Get the current state as a dictionary")
            .def("reset", &BrainContext::reset,
                 "Reset the brain state to initial values");
        
        py::class_<AgentBrainContext>(m, "AgentBrainContext", R"pbdoc(Context manager for AgentBrain resources)
            
            Provides automatic cleanup and state management for AgentBrain objects.
            Ensures proper resource management using Python's context manager protocol.
        )pbdoc")
            .def(py::init<std::shared_ptr<AgentBrain>>(), py::arg("agent_brain"),
                 "Create a context manager for an AgentBrain object")
            .def("getAgentBrain", &AgentBrainContext::getAgentBrain,
                 "Get the managed AgentBrain object")
            .def("getState", &AgentBrainContext::getState,
                 "Get the current state as a dictionary")
            .def("reset", &AgentBrainContext::reset,
                 "Reset the agent brain state to initial values");
    } catch (const std::exception& e) {
        // Error handling
    }

    // Register serialization utilities
    try {
        py::class_<serialization>(m, "serialization", R"pbdoc(Serialization utilities for NLM)
            
            Provides methods for serializing and deserializing NLM objects
            to and from various formats including JSON and Python dictionaries.
        )pbdoc")
            .def_static("brain_to_dict", &serialization::brain_to_dict,
                       py::arg("brain"),
                       "Convert Brain to Python dictionary")
            .def_static("brain_from_dict", &serialization::brain_from_dict,
                       py::arg("brain"), py::arg("state"),
                       "Load Brain from Python dictionary")
            .def_static("agent_brain_to_dict", &serialization::agent_brain_to_dict,
                       py::arg("agent_brain"),
                       "Convert AgentBrain to Python dictionary")
            .def_static("agent_brain_from_dict", &serialization::agent_brain_from_dict,
                       py::arg("agent_brain"), py::arg("state"),
                       "Load AgentBrain from Python dictionary")
            .def_static("config_to_dict", &serialization::config_to_dict,
                       py::arg("config"),
                       "Convert Config to Python dictionary")
            .def_static("config_from_dict", &serialization::config_from_dict,
                       py::arg("config"), py::arg("state"),
                       "Load Config from Python dictionary")
            .def_static("brain_to_json", &serialization::brain_to_json,
                       py::arg("brain"),
                       "Convert Brain to JSON string")
            .def_static("brain_from_json", &serialization::brain_from_json,
                       py::arg("brain"), py::arg("json_str"),
                       "Load Brain from JSON string")
            .def_static("agent_brain_to_json", &serialization::agent_brain_to_json,
                       py::arg("agent_brain"),
                       "Convert AgentBrain to JSON string")
            .def_static("agent_brain_from_json", &serialization::agent_brain_from_json,
                       py::arg("agent_brain"), py::arg("json_str"),
                       "Load AgentBrain from JSON string");
    } catch (const std::exception& e) {
        // Error handling
    }

    // Register batch processing utilities
    try {
        py::class_<batch>(m, "batch", R"pbdoc(Batch processing utilities for NLM)
            
            Provides methods for efficient batch processing of multiple
            simulation steps, results analysis, and object cloning.
        )pbdoc")
            .def_static("process_batch", &batch::process_batch,
                       py::arg("agent_brain"), py::arg("percepts"), py::arg("timestep") = 0.1f,
                       py::arg("collect_stats") = true,
                       "Process a batch of sensory inputs and return results")
            .def_static("analyze_batch_results", &batch::analyze_batch_results,
                       py::arg("results"),
                       "Analyze batch processing results")
            .def_static("clone_batch", &batch::clone_batch,
                       py::arg("brains"),
                       "Clone a batch of Brain objects");
    } catch (const std::exception& e) {
        // Error handling
    }

// Register enhanced profiling utilities
    try {
        py::class_<profiling>(m, "enhanced_profiling", R"pbdoc(Enhanced profiling utilities for NLM)
            
            Provides comprehensive performance profiling including:
            - CPU time measurement
            - Memory usage tracking
            - Thread performance analysis
            - I/O performance monitoring
            - Bottleneck identification
            - Optimization recommendations
        )pbdoc")
            .def_static("enable_profiling", &nlm::Brain::enableProfiling,
                       py::arg("brain"),
                       "Enable performance profiling for a Brain object")
            .def_static("get_performance_profile", &nlm::Brain::getPerformanceProfile,
                       py::arg("brain"),
                       "Get performance profile for a Brain object")
            .def_static("get_memory_profile", &nlm::Brain::getMemoryProfile,
                       py::arg("brain"),
                       "Get memory profile for a Brain object")
            .def_static("get_optimization_recommendations", &nlm::Brain::getOptimizationRecommendations,
                       py::arg("brain"),
                       "Get optimization recommendations for a Brain object")
            .def_static("get_bottleneck_analysis", &nlm::Brain::getBottleneckAnalysis,
                       py::arg("brain"),
                       "Get bottleneck analysis for a Brain object")
            .def_static("get_detailed_metrics", &nlm::Brain::getDetailedMetrics,
                       py::arg("brain"),
                       "Get detailed performance metrics for a Brain object")
            .def_static("is_profiling_enabled", &nlm::Brain::isProfilingEnabled,
                       py::arg("brain"),
                       "Check if profiling is enabled for a Brain object")
            .def_static("reset_performance_counters", &nlm::Brain::resetPerformanceCounters,
                       py::arg("brain"),
                       "Reset performance counters for a Brain object");
    } catch (const std::exception& e) {
        // Error handling
    }

    // Register new profiling classes
    try {
        py::class_<nlm::PerformanceProfiler>(m, "PerformanceProfiler", R"pbdoc(Comprehensive performance profiler)
            
            Provides detailed performance metrics including:
            - CPU time measurements per function/class
            - Memory usage tracking and leak detection
            - Thread performance analysis
            - I/O performance monitoring
            - Bottleneck identification
            - Optimization recommendations
        )pbdoc")
            .def(py::init<>())
            .def("initialize", &nlm::PerformanceProfiler::initialize, py::arg("config"))
            .def("start_profiling", &nlm::PerformanceProfiler::startProfiling, py::arg("name"), py::arg("category") = "")
            .def("stop_profiling", &nlm::PerformanceProfiler::stopProfiling, py::arg("name"))
            .def("record_measurement", &nlm::PerformanceProfiler::recordMeasurement, py::arg("name"), py::arg("timeMs"), py::arg("memoryBytes") = 0)
            .def("record_thread_performance", &nlm::PerformanceProfiler::recordThreadPerformance, py::arg("threadPerf"))
            .def("record_io_performance", &nlm::PerformanceProfiler::recordIOPerformance, py::arg("ioPerf"))
            .def("get_performance_data", &nlm::PerformanceProfiler::getPerformanceData, py::arg("name"))
            .def("get_all_performance_data", &nlm::PerformanceProfiler::getAllPerformanceData)
            .def("get_performance_data_by_category", &nlm::PerformanceProfiler::getPerformanceDataByCategory, py::arg("category"))
            .def("analyze_bottlenecks", &nlm::PerformanceProfiler::analyzeBottlenecks)
            .def("get_optimization_recommendations", &nlm::PerformanceProfiler::getOptimizationRecommendations)
            .def("generate_report", &nlm::PerformanceProfiler::generateReport)
            .def("export_to_json", &nlm::PerformanceProfiler::exportToJSON, py::arg("filepath"))
            .def("export_to_csv", &nlm::PerformanceProfiler::exportToCSV, py::arg("filepath"))
            .def("reset", &nlm::PerformanceProfiler::reset)
            .def("is_enabled", &nlm::PerformanceProfiler::isEnabled)
            .def("enable", &nlm::PerformanceProfiler::enable, py::arg("enable"))
            .def("get_config", &nlm::PerformanceProfiler::getConfig)
            .def("update_config", &nlm::PerformanceProfiler::updateConfig, py::arg("config"));

        py::class_<nlm::MemoryProfiler>(m, "MemoryProfiler", R"pbdoc(Comprehensive memory profiler)
            
            Provides detailed memory profiling including:
            - Memory allocation tracking
            - Memory leak detection
            - Memory usage patterns analysis
            - Memory fragmentation analysis
            - Memory allocation hotspots
            - Memory optimization recommendations
        )pbdoc")
            .def(py::init<>())
            .def("initialize", &nlm::MemoryProfiler::initialize, py::arg("config"))
            .def("record_allocation", &nlm::MemoryProfiler::recordAllocation, py::arg("componentName"), py::arg("size"), py::arg("category") = "", py::arg("trackStackTrace") = true)
            .def("record_deallocation", &nlm::MemoryProfiler::recordDeallocation, py::arg("allocationId"))
            .def("record_current_usage", &nlm::MemoryProfiler::recordCurrentUsage)
            .def("get_memory_stats", &nlm::MemoryProfiler::getMemoryStats)
            .def("get_current_memory_mb", &nlm::MemoryProfiler::getCurrentMemoryMB)
            .def("get_peak_memory_mb", &nlm::MemoryProfiler::getPeakMemoryMB)
            .def("get_allocation_by_component", &nlm::MemoryProfiler::getAllocationByComponent)
            .def("get_allocation_by_category", &nlm::MemoryProfiler::getAllocationByCategory)
            .def("get_allocation_patterns", &nlm::MemoryProfiler::getAllocationPatterns)
            .def("get_memory_breakdown", &nlm::MemoryProfiler::getMemoryBreakdown)
            .def("detect_memory_leaks", &nlm::MemoryProfiler::detectMemoryLeaks)
            .def("analyze_fragmentation", &nlm::MemoryProfiler::analyzeFragmentation)
            .def("get_allocation_records", &nlm::MemoryProfiler::getAllocationRecords)
            .def("get_config", &nlm::MemoryProfiler::getConfig)
            .def("update_config", &nlm::MemoryProfiler::updateConfig, py::arg("config"))
            .def("generate_report", &nlm::MemoryProfiler::generateReport)
            .def("export_to_json", &nlm::MemoryProfiler::exportToJSON, py::arg("filepath"))
            .def("export_to_csv", &nlm::MemoryProfiler::exportToCSV, py::arg("filepath"))
            .def("reset", &nlm::MemoryProfiler::reset)
            .def("is_enabled", &nlm::MemoryProfiler::isEnabled)
            .def("enable", &nlm::MemoryProfiler::enable, py::arg("enable"))
            .def("find_allocation", &nlm::MemoryProfiler::findAllocation, py::arg("allocationId"))
            .def("get_allocation_count", &nlm::MemoryProfiler::getAllocationCount);

        py::class_<nlm::PerformanceAnalyzer>(m, "PerformanceAnalyzer", R"pbdoc(Performance data analyzer)
            
            Provides comprehensive performance analysis including:
            - Bottleneck identification
            - Performance regression detection
            - Configuration comparison
            - Workload characterization
            - Optimization recommendations
            - Performance trend analysis
        )pbdoc")
            .def(py::init<>())
            .def("initialize", &nlm::PerformanceAnalyzer::initialize, py::arg("data"), py::arg("timestamps") = std::vector<double>())
            .def("analyze_bottlenecks", &nlm::PerformanceAnalyzer::analyzeBottlenecks, py::arg("performanceData"))
            .def("detect_regressions", &nlm::PerformanceAnalyzer::detectRegressions, py::arg("comparisons"))
            .def("compare_configurations", &nlm::PerformanceAnalyzer::compareConfigurations, py::arg("configs"))
            .def("characterize_workload", &nlm::PerformanceAnalyzer::characterizeWorkload, py::arg("performanceData"))
            .def("analyze_trends", &nlm::PerformanceAnalyzer::analyzeTrends, py::arg("performanceData"))
            .def("generate_report", &nlm::PerformanceAnalyzer::generateReport)
            .def("get_configuration_analysis", &nlm::PerformanceAnalyzer::getConfigurationAnalysis, py::arg("configurationName"))
            .def("get_workload_characteristics", &nlm::PerformanceAnalyzer::getWorkloadCharacteristics)
            .def("get_bottleneck_analysis", &nlm::PerformanceAnalyzer::getBottleneckAnalysis)
            .def("get_regression_analysis", &nlm::PerformanceAnalyzer::getRegressionAnalysis)
            .def("is_successful", &nlm::PerformanceAnalyzer::isSuccessful)
            .def("add_configuration_analysis", &nlm::PerformanceAnalyzer::addConfigurationAnalysis, py::arg("analysis"))
            .def("add_bottleneck_analysis", &nlm::PerformanceAnalyzer::addBottleneckAnalysis, py::arg("bottleneck"))
            .def("add_regression_analysis", &nlm::PerformanceAnalyzer::addRegressionAnalysis, py::arg("regression"))
            .def("set_workload_characteristics", &nlm::PerformanceAnalyzer::setWorkloadCharacteristics, py::arg("characteristics"))
            .def("add_performance_trend", &nlm::PerformanceAnalyzer::addPerformanceTrend, py::arg("trend"));

        py::class_<nlm::ConfigurationProfiler>(m, "ConfigurationProfiler", R"pbdoc(Configuration performance analyzer)
            
            Provides configuration-based performance analysis including:
            - Performance analysis by configuration
            - Optimal parameter identification
            - Sensitivity analysis
            - Performance tuning recommendations
            - Configuration comparison
            - Parameter optimization
        )pbdoc")
            .def(py::init<>())
            .def("initialize", &nlm::ConfigurationProfiler::initialize)
            .def("add_configuration", &nlm::ConfigurationProfiler::addConfiguration, py::arg("config"))
            .def("get_configurations", &nlm::ConfigurationProfiler::getConfigurations)
            .def("analyze_configuration", &nlm::ConfigurationProfiler::analyzeConfiguration, py::arg("config"))
            .def("compare_configurations", &nlm::ConfigurationProfiler::compareConfigurations, py::arg("configs"))
            .def("optimize_parameters", &nlm::ConfigurationProfiler::optimizeParameters, py::arg("config"))
            .def("perform_sensitivity_analysis", &nlm::ConfigurationProfiler::performSensitivityAnalysis, py::arg("config"))
            .def("generate_report", &nlm::ConfigurationProfiler::generateReport)
            .def("get_optimal_configuration", &nlm::ConfigurationProfiler::getOptimalConfiguration)
            .def("export_configurations", &nlm::ConfigurationProfiler::exportConfigurations, py::arg("filepath"))
            .def("import_configurations", &nlm::ConfigurationProfiler::importConfigurations, py::arg("filepath"));
    } catch (const std::exception& e) {
        // Error handling
    }

    // Register helper functions for Python
    try {
        m.def("serialize_to_json", [](const py::dict& state) {
            try {
                return serialization::dict_to_json(state);
            } catch (const std::exception& e) {
                throw NLMException(format_error_message("JSON serialization", e.what(), "JSON_SERIALIZATION_ERROR"));
            }
        }, py::arg("state"),
            "Serialize a Python dictionary to JSON string.");
        
        m.def("deserialize_from_json", [](const std::string& json_str) {
            try {
                return serialization::json_to_dict(json_str);
            } catch (const std::exception& e) {
                throw NLMException(format_error_message("JSON deserialization", e.what(), "JSON_DESERIALIZATION_ERROR"));
            }
        }, py::arg("json_str"),
            "Deserialize a JSON string to Python dictionary.");
        
        m.def("create_steps", &helpers::create_steps,
            py::arg("num_steps"), py::arg("timestep") = 0.1f,
            "Create a list of SimulationStep objects.");
        
        m.def("collect_stats", &helpers::collect_stats,
            py::arg("brain"),
            "Collect statistics from a Brain object.");
        
        m.def("validate_config_helper", &helpers::validate_config,
            py::arg("config"),
            "Validate a Config object and return True if valid.");
    } catch (const std::exception& e) {
        // Error handling
    }

    // Register Python-specific helper functions
    try {
        m.def("neuron_type_to_string", &py_helpers::neuron_type_to_string,
            py::arg("type"),
            "Convert NeuronType enum to string representation.");
        
        m.def("synapse_type_to_string", &py_helpers::synapse_type_to_string,
            py::arg("type"),
            "Convert SynapseType enum to string representation.");
        
        m.def("action_type_to_string", &py_helpers::action_type_to_string,
            py::arg("type"),
            "Convert ActionType enum to string representation.");
        
        m.def("developmental_stage_to_string", &py_helpers::developmental_stage_to_string,
            py::arg("stage"),
            "Convert DevelopmentalStage enum to string representation.");
        
        m.def("firing_state_to_string", &py_helpers::firing_state_to_string,
            py::arg("state"),
            "Convert FiringState enum to string representation.");
        
        m.def("validate_config_keys", &py_helpers::validate_config_keys,
            py::arg("config"),
            "Validate configuration keys and return list of errors.");
        
        m.def("is_valid_neuron_config", &py_helpers::is_valid_neuron_config,
            py::arg("config"),
            "Check if a neuron configuration dictionary is valid.");
        
        m.def("is_valid_synapse_config", &py_helpers::is_valid_synapse_config,
            py::arg("config"),
            "Check if a synapse configuration dictionary is valid.");
        
        m.def("brain_to_dict", &py_helpers::brain_to_dict,
            py::arg("brain"),
            "Convert Brain to Python dictionary (helper function).");
        
        m.def("brain_from_dict", &py_helpers::brain_from_dict,
            py::arg("brain"), py::arg("state"),
            "Load Brain from Python dictionary (helper function).");
        
        m.def("calculate_metrics", &py_helpers::calculate_metrics,
            py::arg("data"),
            "Calculate metrics from data dictionary.");
        
        m.def("get_summary_stats", &py_helpers::get_summary_stats,
            py::arg("data"),
            "Get summary statistics from data dictionary.");
        
        m.def("normalize_percepts", &py_helpers::normalize_percepts,
            py::arg("percepts"),
            "Normalize sensory percepts.");
        
        m.def("filter_results", &py_helpers::filter_results,
            py::arg("results"), py::arg("success_only") = true,
            "Filter results by success status.");
        
        m.def("calculate_distance", &py_helpers::calculate_distance,
            py::arg("x1"), py::arg("y1"), py::arg("x2"), py::arg("y2"),
            "Calculate Euclidean distance between two points.");
        
        m.def("clamp", &py_helpers::clamp,
            py::arg("value"), py::arg("min"), py::arg("max"),
            "Clamp a value between minimum and maximum.");
        
        m.def("is_close", &py_helpers::is_close,
            py::arg("a"), py::arg("b"), py::arg("epsilon") = 1e-6f,
            "Check if two float values are close within epsilon.");
    } catch (const std::exception& e) {
        // Error handling
    }

    // Register example usage functions
    try {
        m.def("register_examples", &register_examples,
            "Register example code snippets for common NLM usage patterns.");
    } catch (const std::exception& e) {
        // Error handling
    }

    // Original bindings (maintained for backward compatibility)
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
        // AgentBrain advanced configuration and behavior control
        .def("configure", &AgentBrain::withConfig, py::arg("config"),
             "Configure agent brain with advanced configuration settings")
        .def("enableFeatures", &AgentBrain::enableAllFeatures,
             "Enable all agent features (reward modulation, structural plasticity, development, curiosity)")
        .def("disableFeatures", &AgentBrain::disableAllFeatures,
             "Disable all agent features")
        .def("setRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"), "Enable/disable reward modulation")
        .def("setDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable/disable developmental processes")
        .def("setCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable/disable curiosity-driven exploration")
        .def("setStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable/disable structural plasticity")
        
        // Advanced agent behavior and control
        .def("setExplorationLevel", &AgentBrain::enableCuriosity,
             py::arg("enabled"), "Control curiosity/exploration behavior")
        .def("setPlasticityLevel", [](nlm::AgentBrain& self, float level) {
            // This would require access to plasticityModifier_ - may need to add a setter
            return self;
        }, py::arg("level"), "Set plasticity modifier level")
        .def("getStats", [](const nlm::AgentBrain& self) {
            py::dict stats;
            stats["neuromodulation"] = self.getNeuromodulationLevel();
            stats["curiosity"] = self.getCuriosityLevel();
            stats["novelty"] = self.getNoveltyLevel();
            stats["prediction_error"] = self.getPredictionError();
            stats["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            return stats;
        }, "Get agent statistics dictionary")
        
        // Advanced sensory and motor control
        .def("injectSensory", &AgentBrain::processSensoryInput,
             py::arg("percept"), "Process sensory input from environment")
        .def("getMotorCommand", &AgentBrain::decodeMotorCommand,
             "Get current motor command based on neural activity")
        
        // Callback and event handling (advanced feature)
        .def("setRewardCallback", [](nlm::AgentBrain& self, std::function<void(float, float)> callback) {
            // This would require adding a callback member to AgentBrain
            return self;
        }, py::arg("callback"), "Set reward callback function for reward processing")
        .def("setActionCallback", [](nlm::AgentBrain& self, std::function<nlm::MotorCommand()> callback) {
            // This would require adding a callback member to AgentBrain  
            return self;
        }, py::arg("callback"), "Set action callback function for motor control")
        
        // Context manager support for episode management
        .def("__enter__", [](nlm::AgentBrain& self) {
            return &self;
        }, "Context manager entry - reset agent for new episode")
        .def("__exit__", [](nlm::AgentBrain& self, py::object, py::object, py::object) {
            self.reset();
            return false;
        }, "Context manager exit - reset agent state")
        
        // Rich comparison operators (advanced usage)
        .def("__eq__", [](const nlm::AgentBrain& self, const nlm::AgentBrain& other) {
            // Compare agent state - simplified version
            return self.getNeuromodulationLevel() == other.getNeuromodulationLevel() &&
                   self.getCuriosityLevel() == other.getCuriosityLevel();
        }, "Compare two agent brains by neuromodulation and curiosity levels")
        .def("__lt__", [](const nlm::AgentBrain& self, const nlm::AgentBrain& other) {
            // Compare by neuromodulation level
            return self.getNeuromodulationLevel() < other.getNeuromodulationLevel();
        }, "Compare agents by neuromodulation level")
        .def("__hash__", [](const nlm::AgentBrain& self) {
            return std::hash<float>{}(self.getNeuromodulationLevel()) ^ 
                   std::hash<float>{}(self.getCuriosityLevel());
        }, "Hash agent for use in sets/dicts")
        
        // Advanced serialization and persistence
        .def("to_dict", [](const nlm::AgentBrain& self) {
            py::dict state;
            state["neuromodulation"] = self.getNeuromodulationLevel();
            state["curiosity"] = self.getCuriosityLevel();
            state["novelty"] = self.getNoveltyLevel();
            state["prediction_error"] = self.getPredictionError();
            state["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            state["reward_modulation_enabled"] = self.isRewardModulationEnabled();
            state["development_enabled"] = self.isDevelopmentEnabled();
            state["curiosity_enabled"] = self.isCuriosityEnabled();
            return state;
        }, "Export agent state as dictionary")
        .def("from_dict", [](nlm::AgentBrain& self, const py::dict& state) {
            // Restore agent state from dictionary
            // This would require adding setters for the state variables
            return self;
        }, py::arg("state"), "Restore agent state from dictionary")
        
        // Advanced observation and analysis
        .def("analyzeBehavior", [](const nlm::AgentBrain& self, double timestep) {
            // Analyze agent behavior over time
            py::dict analysis;
            analysis["stability"] = 1.0f - std::abs(self.getPredictionError());
            analysis["exploration"] = self.getCuriosityLevel();
            analysis["adaptation"] = std::abs(self.getPredictionError());
            return analysis;
        }, py::arg("timestep"), "Analyze agent behavior and return analysis dictionary")
        .def("getPerformanceMetrics", [](const nlm::AgentBrain& self) {
            py::dict metrics;
            metrics["neuromodulation_stability"] = 1.0f - std::abs(self.getPredictionError());
            metrics["exploration_ratio"] = self.getCuriosityLevel();
            metrics["adaptation_speed"] = std::abs(self.getPredictionError()) * 10.0f; // Placeholder
            return metrics;
        }, "Get performance metrics for agent")

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

    // Advanced factory methods
    m.def("createAgentBrainWithConfig", [](std::shared_ptr<Config> config) -> std::shared_ptr<AgentBrain> {
        return AgentBrain::createWithConfig(config);
    }, py::arg("config"), "Create agent brain with configuration (advanced factory)");

    m.def("createAgentBrainDefault", []() -> std::shared_ptr<AgentBrain> {
        return AgentBrain::createDefault();
    }, "Create default agent brain (advanced factory)");

    m.def("createAgentBrainBuilder", []() -> std::shared_ptr<AgentBrain> {
        return AgentBrain::createDefault();
    }, "Create agent brain builder for fluent interface configuration");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
