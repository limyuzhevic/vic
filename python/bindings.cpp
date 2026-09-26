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
#include <chrono>
#include <thread>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <iomanip>

// Include existing NLM headers
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

// Enhanced exception classes for better error handling
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& msg) : NLMError("Configuration Error: " + msg) {}
};

class InitializationError : public NLMError {
public:
    explicit InitializationError(const std::string& msg) : NLMError("Initialization Error: " + msg) {}
};

class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& msg) : NLMError("Simulation Error: " + msg) {}
};

// Pythonic context manager for Brain
class BrainContextManager {
public:
    BrainContextManager(std::shared_ptr<Brain> brain) : brain_(brain) {}
    
    void __enter__() {
        // Auto-initialize brain if not already initialized
        if (!brain_->getConfig() || !brain_->getRegionCount()) {
            brain_->initialize();
        }
    }
    
    py::object __exit__(int exc_type, py::object exc_value, py::object traceback) {
        if (exc_type != 0) {
            // Log exception
            brain_->logStatus();
        }
        // Auto-cleanup
        brain_->reset();
        return py::bool_(false); // Don't suppress exceptions
    }
    
    std::shared_ptr<Brain> getBrain() { return brain_; }
    
private:
    std::shared_ptr<Brain> brain_;
};

// Performance monitoring utilities
class PerformanceMonitor {
public:
    struct Metrics {
        size_t stepsCompleted = 0;
        size_t totalSpikes = 0;
        double avgFiringRate = 0.0;
        double eiRatio = 0.0;
        double processingTimeMs = 0.0;
        size_t memoryUsageMB = 0;
        Timestamp currentTime = 0.0;
        SimulationStep currentStep = 0;
        
        std::string toString() const {
            std::stringstream ss;
            ss << std::fixed << std::setprecision(3);
            ss << "Metrics {";
            ss << "Steps: " << stepsCompleted << ", ";
            ss << "Total Spikes: " << totalSpikes << ", ";
            ss << "Firing Rate: " << avgFiringRate << ", ";
            ss << "E/I Ratio: " << eiRatio << ", ";
            ss << "Time: " << processingTimeMs << "ms, ";
            ss << "Mem: " << memoryUsageMB << "MB";
            ss << "}";
            return ss.str();
        }
    };
    
    PerformanceMonitor(std::shared_ptr<Brain> brain) : brain_(brain), startTime_(std::chrono::high_resolution_clock::now()) {
        brain->logStatus();
    }
    
    Metrics getMetrics() const {
        Metrics m;
        if (brain_) {
            m.stepsCompleted = brain_->getTotalSpikeCount(); // Approximation
            m.totalSpikes = brain_->getTotalSpikeCount();
            m.avgFiringRate = brain_->getAverageFiringRate();
            m.eiRatio = brain_->getExcitationInhibitionRatio();
            m.currentTime = brain_->getTotalSpikeCount() * 0.001; // Approximation
            m.currentStep = m.totalSpikes / 1000; // Approximation
            
            auto endTime = std::chrono::high_resolution_clock::now();
            m.processingTimeMs = std::chrono::duration<double, std::milli>(endTime - startTime_).count();
            
            // Estimate memory usage (rough approximation)
            m.memoryUsageMB = (brain_->getTotalNeuronCount() * 100 + brain_->getTotalSynapseCount() * 40) / (1024 * 1024);
        }
        return m;
    }
    
    void reset() {
        startTime_ = std::chrono::high_resolution_clock::now();
    }
    
    double getElapsedTimeMs() const {
        auto endTime = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::milli>(endTime - startTime_).count();
    }
    
private:
    std::shared_ptr<Brain> brain_;
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime_;
};

// Configuration management utilities
class BrainConfigurator {
public:
    static std::shared_ptr<Config> createDefaultConfig() {
        auto config = std::make_shared<Config>();
        
        // Set sensible defaults for Phase 6 integrated brain
        config->set("neuron_count", size_t(1000));
        config->set("region_count", size_t(1));
        config->set("connection_probability", float(0.1f));
        config->set("simulation_timestep", double(0.001));
        config->set("random_seed", uint64_t(42));
        config->set("stdp_ltp_weight", float(0.01f));
        config->set("stdp_ltd_weight", float(0.012f));
        config->set("stdp_tau", float(20.0f));
        config->set("synaptogenesis_rate", float(0.0001f));
        config->set("pruning_rate", float(0.00001f));
        config->set("replay_interval", size_t(100));
        config->set("consolidation_interval", size_t(1000));
        
        return config;
    }
    
    static std::shared_ptr<Config> createQuickStartConfig(std::string profile = "balanced") {
        auto config = createDefaultConfig();
        
        if (profile == "fast") {
            config->set("neuron_count", size_t(100));
            config->set("simulation_timestep", double(0.01));
        } else if (profile == "detailed") {
            config->set("neuron_count", size_t(5000));
            config->set("connection_probability", float(0.2f));
        } else if (profile == "research") {
            config->set("neuron_count", size_t(10000));
            config->set("connection_probability", float(0.3f));
            config->set("stdp_ltp_weight", float(0.02f));
            config->set("stdp_ltd_weight", float(0.025f));
        }
        
        return config;
    }
    
    static std::shared_ptr<Config> loadConfigFromFile(const std::string& filepath) {
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(filepath)) {
            throw ConfigurationError("Failed to load config from " + filepath);
        }
        return config;
    }
    
    static std::shared_ptr<Config> loadConfigFromArgs(int argc, char** argv) {
        auto config = std::make_shared<Config>();
        if (!config->loadFromArgs(argc, argv)) {
            throw ConfigurationError("Failed to load config from command line arguments");
        }
        return config;
    }
    
    static void validateConfig(const std::shared_ptr<Config>& config) {
        if (!config) {
            throw ConfigurationError("Config is null");
        }
        
        // Check required parameters
        auto neuronCount = config->getOr<size_t>("neuron_count", 1);
        if (neuronCount == 0) {
            throw ConfigurationError("neuron_count must be greater than 0");
        }
        
        auto timestep = config->getOr<double>("simulation_timestep", 0.001);
        if (timestep <= 0.0) {
            throw ConfigurationError("simulation_timestep must be positive");
        }
        
        auto connProb = config->getOr<float>("connection_probability", 0.1f);
        if (connProb < 0.0f || connProb > 1.0f) {
            throw ConfigurationError("connection_probability must be between 0.0 and 1.0");
        }
    }
};

// Simulation helper utilities
class SimulationHelpers {
public:
    static std::vector<std::vector<float>> runQuickSimulation(
        const std::shared_ptr<Brain>& brain,
        size_t steps = 100,
        const std::shared_ptr<Config>& config = nullptr) {
        
        if (!brain) {
            throw InitializationError("Brain is null");
        }
        
        if (!brain->getConfig() && !config) {
            auto defaultConfig = BrainConfigurator::createDefaultConfig();
            brain->Brain::operator=(*(std::make_shared<Brain>(defaultConfig)));
        } else if (config) {
            brain->Brain::operator=(*(std::make_shared<Brain>(config)));
        }
        
        // Initialize brain
        brain->initialize();
        
        // Collect metrics
        std::vector<std::vector<float>> results;
        results.reserve(steps);
        
        for (size_t i = 0; i < steps; ++i) {
            brain->step(static_cast<SimulationStep>(i));
            
            std::vector<float> stepMetrics;
            stepMetrics.push_back(static_cast<float>(brain->getTotalSpikeCount()));
            stepMetrics.push_back(static_cast<float>(brain->getFiringNeuronCount()));
            stepMetrics.push_back(brain->getAverageFiringRate());
            stepMetrics.push_back(brain->getExcitationInhibitionRatio());
            
            results.push_back(stepMetrics);
        }
        
        return results;
    }
    
    static std::map<std::string, double> analyzeSimulationResults(
        const std::vector<std::vector<float>>& results) {
        
        if (results.empty()) {
            throw SimulationError("No simulation results to analyze");
        }
        
        std::map<std::string, double> analysis;
        
        // Calculate statistics for each metric
        size_t numSteps = results.size();
        
        // Spike count statistics
        std::vector<float> spikeCounts;
        spikeCounts.reserve(numSteps);
        for (const auto& row : results) {
            if (!row.empty()) spikeCounts.push_back(row[0]);
        }
        
        if (!spikeCounts.empty()) {
            double sum = std::accumulate(spikeCounts.begin(), spikeCounts.end(), 0.0);
            analysis["avg_spike_count"] = sum / spikeCounts.size();
            analysis["min_spike_count"] = *std::min_element(spikeCounts.begin(), spikeCounts.end());
            analysis["max_spike_count"] = *std::max_element(spikeCounts.begin(), spikeCounts.end());
        }
        
        // Firing rate statistics
        std::vector<float> firingRates;
        firingRates.reserve(numSteps);
        for (const auto& row : results) {
            if (row.size() > 2) firingRates.push_back(row[2]);
        }
        
        if (!firingRates.empty()) {
            double sum = std::accumulate(firingRates.begin(), firingRates.end(), 0.0);
            analysis["avg_firing_rate"] = sum / firingRates.size();
            analysis["min_firing_rate"] = *std::min_element(firingRates.begin(), firingRates.end());
            analysis["max_firing_rate"] = *std::max_element(firingRates.begin(), firingRates.end());
        }
        
        // E/I ratio statistics
        std::vector<float> eiRatios;
        eiRatios.reserve(numSteps);
        for (const auto& row : results) {
            if (row.size() > 3) eiRatios.push_back(row[3]);
        }
        
        if (!eiRatios.empty()) {
            double sum = std::accumulate(eiRatios.begin(), eiRatios.end(), 0.0);
            analysis["avg_ei_ratio"] = sum / eiRatios.size();
            analysis["min_ei_ratio"] = *std::min_element(eiRatios.begin(), eiRatios.end());
            analysis["max_ei_ratio"] = *std::max_element(eiRatios.begin(), eiRatios.end());
        }
        
        return analysis;
    }
    
    static std::string generateQuickReport(
        const std::shared_ptr<Brain>& brain,
        const std::string& title = "Simulation Report") {
        
        if (!brain) {
            throw InitializationError("Brain is null");
        }
        
        std::stringstream ss;
        ss << std::fixed << std::setprecision(4);
        
        ss << "=== " << title << " ===" << std::endl;
        ss << "Brain Status:" << std::endl;
        ss << "  - Regions: " << brain->getRegionCount() << std::endl;
        ss << "  - Total Neurons: " << brain->getTotalNeuronCount() << std::endl;
        ss << "  - Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
        ss << "  - Active Neurons: " << brain->getActiveNeuronCount() << std::endl;
        ss << "  - Firing Neurons (this step): " << brain->getFiringNeuronCount() << std::endl;
        ss << "  - Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
        ss << "  - Average Firing Rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
        ss << "  - E/I Ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
        ss << "  - Developmental Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
        
        return ss.str();
    }
};

// Iterator support for convenient data access
class RegionIterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = NeuralRegion*;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type&;
    
    RegionIterator(Brain* brain, size_t index) : brain_(brain), index_(index) {}
    
    reference operator*() const {
        return brain_->getRegion(brain_->getRegionIds()[index_]);
    }
    
    pointer operator->() const {
        return brain_->getRegion(brain_->getRegionIds()[index_]);
    }
    
    RegionIterator& operator++() {
        ++index_;
        return *this;
    }
    
    RegionIterator operator++(int) {
        RegionIterator temp = *this;
        ++(*this);
        return temp;
    }
    
    bool operator==(const RegionIterator& other) const {
        return index_ == other.index_;
    }
    
    bool operator!=(const RegionIterator& other) const {
        return !(*this == other);
    }
    
private:
    Brain* brain_;
    size_t index_;
};

class RegionConstIterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = const NeuralRegion*;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type&;
    
    RegionConstIterator(const Brain* brain, size_t index) : brain_(brain), index_(index) {}
    
    reference operator*() const {
        return brain_->getRegion(brain_->getRegionIds()[index_]);
    }
    
    pointer operator->() const {
        return brain_->getRegion(brain_->getRegionIds()[index_]);
    }
    
    RegionConstIterator& operator++() {
        ++index_;
        return *this;
    }
    
    RegionConstIterator operator++(int) {
        RegionConstIterator temp = *this;
        ++(*this);
        return temp;
    }
    
    bool operator==(const RegionConstIterator& other) const {
        return index_ == other.index_;
    }
    
    bool operator!=(const RegionConstIterator& other) const {
        return !(*this == other);
    }
    
private:
    const Brain* brain_;
    size_t index_;
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------------
        A comprehensive Python API for the NLM C++ neural simulation framework.
        Provides enhanced classes with Pythonic interfaces, convenience methods,
        context managers, performance monitoring, and extensive error handling.
    )pbdoc";

    // Register enhanced exception classes
    py::register_exception<NLMError>(m, "NLMError");
    py::register_exception<ConfigurationError>(m, "ConfigurationError");
    py::register_exception<InitializationError>(m, "InitializationError");
    py::register_exception<SimulationError>(m, "SimulationError");

    // === BEGINNER-FRIENDLY CONVENIENCE METHODS ===

    // Quick start factory functions
    m.def("create_default_brain", []() -> std::shared_ptr<Brain> {
        auto config = BrainConfigurator::createDefaultConfig();
        BrainConfigurator::validateConfig(config);
        return std::make_shared<Brain>(config);
    }, "Create a brain with default configuration (quick start)", py::arg("quick_start") = false);

    m.def("create_brain", BrainConfigurator::createDefaultConfig, 
          "Create a default configuration object");

    m.def("create_brain_with_config", [](const std::shared_ptr<Config>& config) -> std::shared_ptr<Brain> {
        BrainConfigurator::validateConfig(config);
        return std::make_shared<Brain>(config);
    }, "Create a brain with specified configuration", py::arg("config"));

    m.def("create_brain_from_file", [](const std::string& filepath) -> std::shared_ptr<Brain> {
        auto config = BrainConfigurator::loadConfigFromFile(filepath);
        return std::make_shared<Brain>(config);
    }, "Create a brain from a configuration file", py::arg("filepath"));

    m.def("create_brain_from_args", [](int argc, char** argv) -> std::shared_ptr<Brain> {
        auto config = BrainConfigurator::loadConfigFromArgs(argc, argv);
        return std::make_shared<Brain>(config);
    }, "Create a brain from command line arguments", py::arg("argc"), py::arg("argv"));

    // === CONTEXT MANAGERS ===

    // Brain context manager
    py::class_<BrainContextManager>(m, "BrainContextManager")
        .def(py::init<std::shared_ptr<Brain>>())
        .def("__enter__", &BrainContextManager::__enter__)
        .def("__exit__", &BrainContextManager::__exit__)
        .def("get_brain", &BrainContextManager::getBrain, "Get the brain object");

    // SimpleWorld context manager
    py::class_<SimpleWorld>(m, "SimpleWorld")
        .def(py::init<>())
        .def("__enter__", &SimpleWorld::__enter__)
        .def("__exit__", &SimpleWorld::__exit__)
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal);

    // === ADVANCED CONFIGURATION MANAGEMENT ===

    // Enhanced Config class
    py::class_<Config>(m, "Config", R"pbdoc(Advanced configuration manager for NLM)pbdoc")
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
        .def("get", &Config::get, py::arg("key"), py::arg("default") = nullptr,
             "Get configuration value with optional default")
        .def("set", &Config::set, py::arg("key"), py::arg("value"),
             "Set configuration value")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("validate", [](const Config& self) {
            BrainConfigurator::validateConfig(std::const_pointer_cast<Config>(&self));
        }, "Validate configuration parameters")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Enhanced Brain class with Pythonic interfaces
    py::class_<Brain>(m, "Brain", R"pbdoc(Advanced neural simulation brain with Pythonic interfaces)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        
        // === CONVENIENCE INITIALIZATION ===
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("quick_start", [](Brain& self, const std::string& profile = "balanced") {
            auto config = BrainConfigurator::createQuickStartConfig(profile);
            return self.initialize();
        }, "Initialize brain with quick start profile", py::arg("profile") = "balanced")
        
        // === CONTEXT MANAGER SUPPORT ===
        .def("with_context", [](std::shared_ptr<Brain> brain) {
            return BrainContextManager(brain);
        }, "Create a context manager for this brain", py::return_value_policy::reference)
        
        // === PERFORMANCE MONITORING ===
        .def("monitor", [](std::shared_ptr<Brain> brain) {
            return PerformanceMonitor(brain);
        }, "Create a performance monitor for this brain", py::return_value_policy::reference)
        
        // === SIMULATION HELPERS ===
        .def("run_simulation", &SimulationHelpers::runQuickSimulation,
             "Run a quick simulation with predefined steps", py::arg("steps") = 100)
        .def("analyze_results", &SimulationHelpers::analyzeSimulationResults,
             "Analyze simulation results", py::arg("results"))
        .def("generate_report", &SimulationHelpers::generateQuickReport,
             "Generate a human-readable report", py::arg("title") = "Simulation Report")
        
        // === PYTHONIC INTERFACES ===
        
        // Iterator support
        .def("__iter__", [](std::shared_ptr<Brain> brain) {
            return std::vector<RegionIterator>{
                RegionIterator(brain.get(), 0)
            };
        })
        
        // Property-based access for commonly used attributes
        .def_property_readonly("region_count", &Brain::getRegionCount,
                              "Get the number of regions")
        .def_property_readonly("total_neurons", &Brain::getTotalNeuronCount,
                              "Get total neuron count across all regions")
        .def_property_readonly("total_synapses", &Brain::getTotalSynapseCount,
                              "Get total synapse count across all regions")
        .def_property_readonly("active_neurons", &Brain::getActiveNeuronCount,
                              "Get count of active neurons")
        .def_property_readonly("firing_neurons", &Brain::getFiringNeuronCount,
                              "Get count of currently firing neurons")
        .def_property_readonly("average_firing_rate", &Brain::getAverageFiringRate,
                              "Get average firing rate across all neurons")
        .def_property_readonly("ei_ratio", &Brain::getExcitationInhibitionRatio,
                              "Get excitation/inhibition balance ratio")
        .def_property_readonly("total_spikes", &Brain::getTotalSpikeCount,
                              "Get total spike count")
        .def_property_readonly("developmental_stage", &Brain::getDevelopmentalStage,
                              "Get current developmental stage")
        .def_property_readonly("working_memory", &Brain::getWorkingMemory,
                              "Get working memory system")
        .def_property_readonly("episodic_memory", &Brain::getEpisodicMemory,
                              "Get episodic memory system")
        .def_property_readonly("prediction_system", &Brain::getPredictionSystem,
                              "Get prediction system")
        .def_property_readonly("planner", &Brain::getPlanner,
                              "Get neural planner")
        
        // === FLUENT INTERFACE METHODS ===
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step_with_time", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        
        // Enhanced injection methods with validation
        .def("inject", &Brain::injectCurrent, py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron (Pythonic alias for injectCurrent)")
        .def("inject_type", &Brain::injectCurrentToNeurons, py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type (Pythonic alias for injectCurrentToNeurons)")
        
        // === ENHANCED ERROR HANDLING ===
        .def("safe_step", [](Brain& self, SimulationStep step) {
            try {
                self.step(step);
                return true;
            } catch (const std::exception& e) {
                throw SimulationError(std::string("Safe step failed: ") + e.what());
            }
        }, "Safely perform a simulation step with error handling", py::arg("step"))
        
        .def("safe_receive", [](Brain& self, const SensoryInput& input) {
            try {
                self.receiveSensoryInput(input);
                return true;
            } catch (const std::exception& e) {
                throw SimulationError(std::string("Safe receive failed: ") + e.what());
            }
        }, "Safely receive sensory input with error handling", py::arg("input"))
        
        // === EXPERIMENT MANAGEMENT TOOLS ===
        .def("experiment_run", [](std::shared_ptr<Brain> brain, size_t steps, const std::string& experimentName) {
            brain->initialize();
            for (size_t i = 0; i < steps; ++i) {
                brain->step(static_cast<SimulationStep>(i));
            }
            brain->logStatus();
            return SimulationHelpers::generateQuickReport(brain, experimentName);
        }, "Run a complete experiment with automatic initialization and reporting", 
           py::arg("steps"), py::arg("experimentName"))
        
        .def("save_experiment", &Brain::save, py::arg("filepath"),
             "Save experiment state to file")
        .def("load_experiment", &Brain::load, py::arg("filepath"),
             "Load experiment state from file")
        
        // === OTHER EXISTING METHODS ===
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
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status");

    // === AGENT BRAIN ENHANCEMENTS ===
    
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Advanced agent brain interface with enhanced features)pbdoc")
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
        .def("is_curiosity_enabled", &AgentBrain::isCuriosityEnabled);

    // === FACTORY FUNCTIONS ===

    m.def("createDefaultConfig", BrainConfigurator::createDefaultConfig,
          "Create a default configuration object");

    m.def("createQuickConfig", BrainConfigurator::createQuickStartConfig,
          "Create a quick start configuration", py::arg("profile") = "balanced");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        BrainConfigurator::validateConfig(config);
        return std::make_shared<Brain>(config);
    }, "Create a new brain with configuration (validated)", py::arg("config"));

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, "Create a new agent brain interface", py::arg("brain"));

    // === UTILITY FUNCTIONS ===

    m.def("run_quick_simulation", &SimulationHelpers::runQuickSimulation,
          "Run a quick simulation", py::arg("brain"), py::arg("steps") = 100);

    m.def("analyze_results", &SimulationHelpers::analyzeSimulationResults,
          "Analyze simulation results", py::arg("results"));

    m.def("generate_report", &SimulationHelpers::generateQuickReport,
          "Generate a human-readable report", py::arg("brain"), py::arg("title") = "Report");

    m.def("validate_config", BrainConfigurator::validateConfig,
          "Validate configuration parameters", py::arg("config"));

    // === CONSTANTS ===

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // === MODULE CONSTANTS ===

    m.attr("VERSION") = "6.0.0-enhanced";
    m.attr("BUILD_TYPE") = "Phase 6: Integrated Artificial Brain";
    m.attr("DESCRIPTION") = "Enhanced NLM Python API with beginner-friendly interfaces, "
                           "context managers, performance monitoring, and comprehensive error handling";
}

} // namespace nlm
