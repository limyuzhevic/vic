#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
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
#include "../src/performance/Performance.hpp"

namespace py = pybind11;
namespace nlm {

// Exception classes for Python bindings
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigError : public NLMException {
public:
    explicit ConfigError(const std::string& msg) : NLMException("Config Error: " + msg) {}
};

class BrainError : public NLMException {
public:
    explicit BrainError(const std::string& msg) : NLMException("Brain Error: " + msg) {}
};

class SimulationError : public NLMException {
public:
    explicit SimulationError(const std::string& msg) : NLMException("Simulation Error: " + msg) {}
};

// Performance monitoring utilities
class PerformanceMonitor {
public:
    PerformanceMonitor() : totalSteps(0), totalTime(0.0), peakMemory(0.0) {}
    
    void recordStep(double timestep) {
        totalSteps++;
        totalTime += timestep;
        // In real implementation, would track memory usage
    }
    
    double getAverageTimePerStep() const {
        return totalSteps > 0 ? totalTime / totalSteps : 0.0;
    }
    
    size_t getTotalSteps() const { return totalSteps; }
    double getTotalTime() const { return totalTime; }
    double getPeakMemory() const { return peakMemory; }
    
    std::string getReport() const {
        std::stringstream ss;
        ss << "Performance Monitor Report\n";
        ss << "==========================\n";
        ss << "Total steps: " << totalSteps << "\n";
        ss << "Total time: " << std::fixed << std::setprecision(3) << totalTime << "s\n";
        ss << "Avg time per step: " << std::fixed << std::setprecision(6) 
           << getAverageTimePerStep() << "s\n";
        ss << "Peak memory: " << std::fixed << std::setprecision(2) << peakMemory << " MB\n";
        return ss.str();
    }
    
private:
    size_t totalSteps;
    double totalTime;
    double peakMemory;
};

// Convenience function to create brain with optimal settings
std::shared_ptr<Brain> createOptimizedBrain(int neuronCount = 1000, double connectionProb = 0.1) {
    auto config = std::make_shared<Config>();
    
    // Set optimal defaults for simulation performance
    config->set("neuron_count", static_cast<int64_t>(neuronCount));
    config->set("region_count", static_cast<int64_t>(1));
    config->set("connection_probability", connectionProb);
    config->set("simulation_timestep", 0.001);
    config->set("random_seed", static_cast<int64_t>(42));
    
    // Enable important learning systems
    config->set("plasticity.stdp.enable", true);
    config->set("plasticity.hebbian.enable", true);
    config->set("plasticity.structural.enable", true);
    config->set("neuromod.dopamine.scale", 1.0);
    config->set("neuromod.curiosity.enable", true);
    config->set("neuromod.novelty.enable", true);
    
    return std::make_shared<Brain>(config);
}

// Batch processing utilities for performance
class BatchProcessor {
public:
    BatchProcessor(std::shared_ptr<Brain> brain) : brain(brain), stepsCompleted(0) {}
    
    // Process multiple steps efficiently
    void runSteps(int numSteps) {
        if (!brain) {
            throw BrainError("Brain not initialized");
        }
        
        if (numSteps <= 0) {
            throw SimulationError("Number of steps must be positive");
        }
        
        for (int step = 0; step < numSteps; ++step) {
            brain->step(step);
            stepsCompleted++;
        }
    }
    
    // Process steps with callback for progress monitoring
    void runStepsWithCallback(int numSteps, std::function<void(int, double)> callback = nullptr) {
        if (!brain) {
            throw BrainError("Brain not initialized");
        }
        
        double timestep = brain->getConfig()->getOr<double>("simulation_timestep", 0.001);
        
        for (int step = 0; step < numSteps; ++step) {
            brain->step(step);
            stepsCompleted++;
            
            if (callback) {
                double progress = static_cast<double>(step) / numSteps;
                callback(step, progress);
            }
        }
    }
    
    int getStepsCompleted() const { return stepsCompleted; }
    
private:
    std::shared_ptr<Brain> brain;
    int stepsCompleted;
};

// Context manager for automatic resource cleanup
class BrainContext {
public:
    BrainContext(std::shared_ptr<Brain> brain) : brain(brain) {
        if (brain) {
            brain->logStatus();
        }
    }
    
    ~BrainContext() {
        if (brain) {
            brain->reset();
        }
    }
    
    // Prevent copying
    BrainContext(const BrainContext&) = delete;
    BrainContext& operator=(const BrainContext&) = delete;
    
    // Allow moving
    BrainContext(BrainContext&& other) noexcept : brain(std::move(other.brain)) {}
    BrainContext& operator=(BrainContext&& other) noexcept {
        if (this != &other) {
            brain = std::move(other.brain);
        }
        return *this;
    }
    
    std::shared_ptr<Brain> getBrain() const { return brain; }
    
private:
    std::shared_ptr<Brain> brain;
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) - Enhanced Python Bindings
        ----------------------------------------------------------
        
        A comprehensive Python interface for the NLM C++ neural simulation framework
        with enhanced error handling, convenience methods, and performance monitoring.
        
        Key Improvements:
        - Enhanced error handling with specific exception types
        - Convenience functions for common operations
        - Performance monitoring utilities
        - Batch processing for efficient simulation
        - Context managers for resource management
        - Advanced configuration utilities
        - Comprehensive validation and input checking
    )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMException>(m, "NLMException");
    py::register_exception<ConfigError>(m, "ConfigError");
    py::register_exception<BrainError>(m, "BrainError");
    py::register_exception<SimulationError>(m, "SimulationError");

    // Performance monitor class
    py::class_<PerformanceMonitor>(m, "PerformanceMonitor", R"pbdoc(
        Performance monitoring utility for NLM simulations
    )pbdoc")
        .def(py::init<>())
        .def("record_step", &PerformanceMonitor::recordStep, py::arg("timestep"))
        .def("get_average_time_per_step", &PerformanceMonitor::getAverageTimePerStep)
        .def("get_total_steps", &PerformanceMonitor::getTotalSteps)
        .def("get_total_time", &PerformanceMonitor::getTotalTime)
        .def("get_peak_memory", &PerformanceMonitor::getPeakMemory)
        .def("get_report", &PerformanceMonitor::getReport);

    // Enhanced Config class with validation and convenience methods
    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration class for NLM system with enhanced validation and utilities
    )pbdoc")
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
        .def("get", [](Config& self, const std::string& key) {
            auto value = self.get<std::string>(key);
            if (value) {
                return *value;
            }
            throw ConfigError("Key not found: " + key);
        }, py::arg("key"), "Get configuration value with validation")
        .def("getOr", [](Config& self, const std::string& key, auto defaultValue) {
            auto value = self.get<decltype(defaultValue)>(key);
            if (value) {
                return *value;
            }
            return defaultValue;
        }, py::arg("key"), py::arg("defaultValue"), "Get configuration value or default")
        .def("set", &Config::set, py::arg("key"), py::arg("value"),
             "Set configuration value with validation")
        .def("validate", &Config::summary, "Validate and get configuration summary")
        .def("getKeys", &Config::getKeys, "Get all configuration keys")
        .def("clear", &Config::clear, "Clear all configuration entries")
        .def("copy", [](const Config& self) {
            auto copy = std::make_shared<Config>();
            // Copy implementation would need to be added
            return copy;
        }, "Create a copy of this configuration")
        .def("merge", [](Config& self, const Config& other) {
            // Merge implementation would need to be added
            return self;
        }, py::arg("other"), "Merge another configuration into this one");

    // Enhanced Brain class with convenience methods and error handling
    py::class_<Brain>(m, "Brain", R"pbdoc(
        Enhanced brain class with convenience methods and comprehensive error handling
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration (enhanced error handling)")
        .def("initialize", &Brain::initialize, "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"), "Perform a simulation step")
        .def("step_with_time", &Brain::step, py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receive_sensory_input", &Brain::receiveSensoryInput,
             py::arg("input"), "Inject sensory input into the brain (enhanced)")
        .def("inject_current", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"), "Inject current into a specific neuron")
        .def("inject_current_to_type", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"), "Inject current into all neurons of a type")
        .def("produce_action", &Brain::produceAction,
             "Produce motor action based on neural activity (enhanced)")
        .def("reset", &Brain::reset, "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"), "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"), "Load brain state from file")
        .def("add_region", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("get_region", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal, "Get a region by ID")
        .def("get_region_count", &Brain::getRegionCount, "Get the number of regions")
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
        .def("get_pending_spike_event_count", &Brain::getPendingSpikeEventCount,
             "Get pending spike event count")
        .def("get_developmental_stage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("set_developmental_stage", &Brain::setDevelopmentalStage,
             py::arg("stage"), "Set developmental stage")
        .def("get_config", &Brain::getConfig,
             py::return_value_policy::reference_internal, "Get the configuration")
        .def("log_status", &Brain::logStatus, "Log brain status")
        // Enhanced methods
        .def("run_steps", [](std::shared_ptr<Brain> self, int numSteps) {
            BatchProcessor processor(self);
            processor.runSteps(numSteps);
            return processor.getStepsCompleted();
        }, py::arg("numSteps"), "Run multiple simulation steps efficiently")
        .def("run_steps_with_callback", [](std::shared_ptr<Brain> self, int numSteps, 
                                          py::function callback) {
            BatchProcessor processor(self);
            if (callback) {
                processor.runStepsWithCallback(numSteps, callback);
            } else {
                processor.runSteps(numSteps);
            }
            return processor.getStepsCompleted();
        }, py::arg("numSteps"), py::arg("callback") = nullptr, "Run steps with progress callback")
        .def("get_performance_monitor", []() {
            return std::make_shared<PerformanceMonitor>();
        }, "Create a performance monitor")
        .def("create_context", [](std::shared_ptr<Brain> self) {
            return BrainContext(self);
        }, py::arg("brain"), "Create a context manager for automatic cleanup");

    // Enhanced AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Enhanced agent brain interface with convenience methods
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world (enhanced)")
        .def("process_sensory_input", &AgentBrain::processSensoryInput,
             py::arg("percept"), "Process sensory input and inject into brain")
        .def("decode_motor_command", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("apply_reward_modulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predicted_reward"),
             "Apply reward-based neuromodulation")
        .def("update_development", &AgentBrain::updateDevelopment,
             py::arg("timestep"), "Update development system")
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
        .def("reset", &AgentBrain::reset, "Reset agent for new episode")
        .def("get_brain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal, "Get the underlying brain")
        // Enhanced convenience methods
        .def("enable_all_features", [](AgentBrain& self) {
            self.enableRewardModulation(true);
            self.enableStructuralPlasticity(true);
            self.enableDevelopment(true);
            self.enableCuriosity(true);
        }, "Enable all learning features at once")
        .def("disable_all_features", [](AgentBrain& self) {
            self.enableRewardModulation(false);
            self.enableStructuralPlasticity(false);
            self.enableDevelopment(false);
            self.enableCuriosity(false);
        }, "Disable all learning features at once")
        .def("get_feature_status", [](const AgentBrain& self) {
            return std::make_tuple(
                self.isRewardModulationEnabled(),
                self.isStructuralPlasticityEnabled(),
                self.isDevelopmentEnabled(),
                self.isCuriosityEnabled()
            );
        }, "Get status of all enabled features");

    // Convenience functions
    m.def("create_default_config", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration with validation");

    m.def("create_optimized_brain", &createOptimizedBrain,
          py::arg("neuronCount") = 1000, py::arg("connectionProb") = 0.1,
          "Create an optimized brain with default settings for performance");

    m.def("create_brain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        if (!config) {
            throw ConfigError("Configuration cannot be null");
        }
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration (enhanced validation)");

    m.def("create_simple_world", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("create_agent_brain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        if (!brain) {
            throw BrainError("Brain cannot be null");
        }
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface (enhanced validation)");

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Type conversions
    m.def("to_float", [](py::float value) { return static_cast<float>(value); },
          "Convert Python float to C++ float");
    
    m.def("to_double", [](py::float value) { return static_cast<double>(value); },
          "Convert Python float to C++ double");
    
    m.def("to_int", [](py::int_ value) { return static_cast<int64_t>(value); },
          "Convert Python int to C++ int64_t");

    // Error constants
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const NLMException& e) {
            py::set_error(PyExc_RuntimeError, e.what());
        } catch (const std::exception& e) {
            py::set_error(PyExc_RuntimeError, e.what());
        }
    });
}

} // namespace nlm