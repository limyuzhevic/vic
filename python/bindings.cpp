#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/iostream.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
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
#include "../src/experiments/Phase6IntegratedExperiment.hpp"

namespace py = pybind11;
namespace nlm {

// Forward declarations for helper classes
class PythonBrainWrapper;
class PythonConfigWrapper;
class PythonExperimentWrapper;

// Exception classes for better error handling
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& msg) : NLMError("Configuration Error: " + msg) {}
};

class BrainInitializationError : public NLMError {
public:
    explicit BrainInitializationError(const std::string& msg) : NLMError("Brain Initialization Error: " + msg) {}
};

class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& msg) : NLMError("Simulation Error: " + msg) {}
};

// Python context manager for automatic resource cleanup
class BrainContextManager {
private:
    std::shared_ptr<Brain> brain;
    std::shared_ptr<AgentBrain> agent;
    std::shared_ptr<SimpleWorld> world;
    bool autoCleanup;
    
public:
    BrainContextManager(std::shared_ptr<Brain> b, std::shared_ptr<AgentBrain> a, std::shared_ptr<SimpleWorld> w, bool cleanup = true)
        : brain(b), agent(a), world(w), autoCleanup(cleanup) {}
    
    ~BrainContextManager() {
        if (autoCleanup) {
            cleanup();
        }
    }
    
    void cleanup() {
        if (brain) {
            brain->reset();
        }
        if (agent) {
            agent->reset();
        }
        // World cleanup is handled differently
    }
    
    bool cleanupOnDestruction() const { return autoCleanup; }
    void setCleanupOnDestruction(bool cleanup) { autoCleanup = cleanup; }
};

// Advanced PythonBrainWrapper with enhanced functionality
class PythonBrainWrapper {
private:
    std::shared_ptr<Brain> brain;
    std::shared_ptr<Config> config;
    std::shared_ptr<PythonBrainWrapper> parent;
    std::vector<std::shared_ptr<PythonBrainWrapper>> children;
    std::string name;
    bool autoSave;
    std::string savePath;
    
    // Performance monitoring
    struct PerformanceMetrics {
        std::vector<double> stepTimes;
        std::vector<size_t> neuronCounts;
        std::vector<size_t> spikeCounts;
        size_t totalSteps;
        std::chrono::steady_clock::time_point startTime;
        double totalWallClockTime;
        
        PerformanceMetrics() : totalSteps(0), totalWallClockTime(0.0) {
            startTime = std::chrono::steady_clock::now();
        }
        
        void recordStep(size_t neurons, size_t spikes, double stepTime) {
            stepTimes.push_back(stepTime);
            neuronCounts.push_back(neurons);
            spikeCounts.push_back(spikes);
            totalSteps++;
        }
        
        void finalize() {
            auto endTime = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);
            totalWallClockTime = duration.count();
        }
        
        double getAverageStepTime() const {
            if (stepTimes.empty()) return 0.0;
            double sum = std::accumulate(stepTimes.begin(), stepTimes.end(), 0.0);
            return sum / stepTimes.size();
        }
        
        size_t getMaxSpikeCount() const {
            if (spikeCounts.empty()) return 0;
            return *std::max_element(spikeCounts.begin(), spikeCounts.end());
        }
        
        py::dict toDict() const {
            py::dict result;
            result["totalSteps"] = totalSteps;
            result["totalWallClockTime"] = totalWallClockTime;
            result["averageStepTime"] = getAverageStepTime();
            result["maxSpikeCount"] = getMaxSpikeCount();
            result["averageNeurons"] = neuronCounts.empty() ? 0 : 
                std::accumulate(neuronCounts.begin(), neuronCounts.end(), 0.0) / neuronCounts.size();
            return result;
        }
    };
    
    PerformanceMetrics metrics;
    
public:
    PythonBrainWrapper(std::shared_ptr<Brain> b, std::shared_ptr<Config> c) : brain(b), config(c), autoSave(false) {}
    
    // Get the underlying brain
    std::shared_ptr<Brain> getBrain() const { return brain; }
    
    // Set name for this wrapper
    void setName(const std::string& n) { name = n; }
    
    // Enable/disable auto-save
    void enableAutoSave(const std::string& path) { autoSave = true; savePath = path; }
    
    // Get performance metrics
    py::dict getPerformanceMetrics() const {
        return metrics.toDict();
    }
    
    // Record performance data
    void recordStep(size_t neurons, size_t spikes, double stepTime) {
        metrics.recordStep(neurons, spikes, stepTime);
    }
    
    // Get metrics summary
    py::dict getMetricsSummary() const {
        py::dict result;
        if (metrics.stepTimes.empty()) {
            return result;
        }
        
        std::vector<double> sortedTimes = metrics.stepTimes;
        std::sort(sortedTimes.begin(), sortedTimes.end());
        
        result["min_step_time"] = sortedTimes.front();
        result["max_step_time"] = sortedTimes.back();
        result["median_step_time"] = sortedTimes[sortedTimes.size() / 2];
        
        size_t maxSpikes = metrics.getMaxSpikeCount();
        result["max_spikes_per_step"] = maxSpikes;
        
        double avgNeurons = metrics.getAverageStepTime();
        result["avg_neurons_per_step"] = avgNeurons;
        
        return result;
    }
    
    // Get statistics
    py::dict getStats() const {
        py::dict result;
        result["total_neurons"] = brain->getTotalNeuronCount();
        result["total_synapses"] = brain->getTotalSynapseCount();
        result["active_neurons"] = brain->getActiveNeuronCount();
        result["firing_neurons"] = brain->getFiringNeuronCount();
        result["average_firing_rate"] = brain->getAverageFiringRate();
        result["excitation_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        result["total_spikes"] = brain->getTotalSpikeCount();
        result["developmental_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        return result;
    }
    
    // Get child regions (Python Brain hierarchy)
    std::vector<std::shared_ptr<PythonBrainWrapper>> getChildren() const {
        return children;
    }
    
    // Add child wrapper
    void addChild(std::shared_ptr<PythonBrainWrapper> child) {
        children.push_back(child);
        child->parent = shared_from_this();
    }
    
    // Save brain state
    bool save(const std::string& filepath) {
        if (autoSave && savePath.empty()) {
            // Try to save to the specified path
        }
        return brain->save(filepath);
    }
    
    // Load brain state
    bool load(const std::string& filepath) {
        return brain->load(filepath);
    }
    
    // Get brain info as string
    std::string info() const {
        std::stringstream ss;
        ss << "Brain Wrapper Info:" << std::endl;
        ss << "  Name: " << (name.empty() ? "unnamed" : name) << std::endl;
        ss << "  Total Neurons: " << brain->getTotalNeuronCount() << std::endl;
        ss << "  Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
        ss << "  Developmental Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
        ss << "  Total Steps Recorded: " << metrics.totalSteps << std::endl;
        return ss.str();
    }
    
    // String representation
    std::string toString() const {
        return info();
    }
};

// Enhanced PythonConfigWrapper with validation and type safety
class PythonConfigWrapper {
private:
    std::shared_ptr<Config> config;
    bool strictValidation;
    std::unordered_map<std::string, std::function<bool(const Config&)>> validators;
    
public:
    PythonConfigWrapper(std::shared_ptr<Config> c) : config(c), strictValidation(true) {}
    
    // Set strict validation mode
    void setStrictValidation(bool strict) { strictValidation = strict; }
    
    // Add custom validator
    void addValidator(const std::string& name, std::function<bool(const Config&)> validator) {
        validators[name] = validator;
    }
    
    // Validate configuration
    py::dict validate() const {
        py::dict result;
        bool isValid = true;
        
        // Basic validation
        if (!config) {
            result["valid"] = false;
            result["errors"].push_back("Config is null");
            return result;
        }
        
        // Run custom validators
        for (const auto& pair : validators) {
            try {
                if (!pair.second(*config)) {
                    isValid = false;
                    result["errors"].push_back("Validator '" + pair.first + "' failed");
                }
            } catch (const std::exception& e) {
                isValid = false;
                result["errors"].push_back("Validator '" + pair.first + "' exception: " + e.what());
            }
        }
        
        result["valid"] = isValid;
        
        // Get config keys
        auto keys = config->getKeys();
        result["keys"] = keys;
        
        return result;
    }
    
    // Get the underlying config
    std::shared_ptr<Config> getConfig() const { return config; }
    
    // Get config info as string
    std::string info() const {
        std::stringstream ss;
        ss << "Config Wrapper Info:" << std::endl;
        ss << "  Strict Validation: " << (strictValidation ? "enabled" : "disabled") << std::endl;
        
        auto keys = config->getKeys();
        ss << "  Config Keys (" << keys.size() << "): " << std::endl;
        for (const auto& key : keys) {
            ss << "    " << key << std::endl;
        }
        
        ss << "  Config Summary:" << std::endl;
        ss << config->summary() << std::endl;
        
        return ss.str();
    }
    
    // String representation
    std::string toString() const {
        return info();n    }
};

// Enhanced experiment wrapper
class PythonExperimentWrapper {
private:
    std::unique_ptr<Phase6IntegratedExperiment> experiment;
    bool verbose;
    
public:
    PythonExperimentWrapper() : experiment(std::make_unique<Phase6IntegratedExperiment>()), verbose(true) {}
    
    // Set verbose mode
    void setVerbose(bool v) { verbose = v; }
    
    // Run integration verification
    py::dict runIntegrationVerification() {
        py::dict result;
        
        bool integrationOK = experiment->verifyIntegration();
        result["success"] = integrationOK;
        
        if (verbose) {
            std::cout << (integrationOK ? "Integration verification PASSED" : "Integration verification FAILED") << std::endl;
        }
        
        return result;
    }
    
    // Run integration tests
    py::dict runIntegrationTests() {
        py::dict result;
        
        try {
            if (verbose) {
                std::cout << "=== Memory Integration Test ===" << std::endl;
            }
            experiment->testMemoryIntegration();
            
            if (verbose) {
                std::cout << "=== Neuromodulation Integration Test ===" << std::endl;
            }
            experiment->testNeuromodulationIntegration();
            
            if (verbose) {
                std::cout << "=== Checkpoint Test ===" << std::endl;
            }
            experiment->testCheckpointing();
            
            if (verbose) {
                std::cout << "=== Replay Test ===" << std::endl;
            }
            experiment->testReplay();
            
            result["success"] = true;
            result["message"] = "All integration tests passed";
            
        } catch (const std::exception& e) {
            result["success"] = false;
            result["error"] = std::string(e.what());
        }
        
        return result;
    }
    
    // Run full experiment
    py::dict runExperiment(const py::dict& config) {
        Phase6Config expConfig;
        
        // Parse Python dict to Phase6Config
        if (config.contains("neuronCount")) {
            expConfig.neuronCount = config["neuronCount"].cast<int>();
        }
        if (config.contains("maxSteps")) {
            expConfig.maxSteps = config["maxSteps"].cast<int>();
        }
        if (config.contains("enableCheckpointing")) {
            expConfig.enableCheckpointing = config["enableCheckpointing"].cast<bool>();
        }
        if (config.contains("enableReplay")) {
            expConfig.enableReplay = config["enableReplay"].cast<bool>();
        }
        if (config.contains("enableDevelopment")) {
            expConfig.enableDevelopment = config["enableDevelopment"].cast<bool>();
        }
        
        try {
            auto result = experiment->run(expConfig);
            
            py::dict pyResult;
            pyResult["totalReward"] = result.totalReward;
            pyResult["avgFiringRate"] = result.avgFiringRate;
            pyResult["memoryEpisodesStored"] = result.memoryEpisodesStored;
            pyResult["dopamineLevel"] = result.dopamineLevel;
            pyResult["totalWallClockTime"] = result.totalWallClockTime;
            pyResult["memoryWorkingMemoryIntegrated"] = result.memoryWorkingMemoryIntegrated;
            pyResult["memoryEpisodicMemoryIntegrated"] = result.memoryEpisodicMemoryIntegrated;
            pyResult["neuromodulationIntegrated"] = result.neuromodulationIntegrated;
            pyResult["predictionIntegrated"] = result.predictionIntegrated;
            pyResult["developmentIntegrated"] = result.developmentIntegrated;
            pyResult["checkpointingWorks"] = result.checkpointingWorks;
            
            return pyResult;
            
        } catch (const std::exception& e) {
            py::dict errorResult;
            errorResult["success"] = false;
            errorResult["error"] = std::string(e.what());
            return errorResult;
        }
    }
    
    // Get experiment info
    std::string info() const {
        return "Phase 6 Integrated Experiment Wrapper (verbose=" + std::string(verbose ? "true" : "false") + ")";
    }
    
    // String representation
    std::string toString() const {
        return info();
    }
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Enhanced Python Bindings
        ---------------------------------------------------------
        An enhanced Python binding for the NLM C++ neural simulation framework.
        Provides enhanced classes with context managers, performance monitoring,
        and advanced validation for production use.
        
        Key features:
        - Context managers for automatic resource cleanup
        - Performance monitoring and metrics collection
        - Advanced configuration validation
        - Enhanced error handling with specific exception types
        - Brain hierarchy and wrapper support
        - Integration with Phase 6 experimental framework
    )pbdoc";

    // Register exception classes
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::class_<NLMError>(m, "NLMError")
        .def(py::init<const std::string&>());
    
    py::class_<ConfigurationError, NLMError>(m, "ConfigurationError")
        .def(py::init<const std::string&>());
    
    py::class_<BrainInitializationError, NLMError>(m, "BrainInitializationError")
        .def(py::init<const std::string&>());
    
    py::class_<SimulationError, NLMError>(m, "SimulationError")
        .def(py::init<const std::string&>());

    // Enhanced Config class with wrapper
    py::class_<Config>(m, "Config")
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
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value, ConfigSource::User);
        }, py::arg("key"), py::arg("value"), "Set configuration value")
        .def("set", [](Config& self, const std::string& key, int64_t value) {
            self.set(key, value, ConfigSource::User);
        }, py::arg("key"), py::arg("value"), "Set configuration value")
        .def("set", [](Config& self, const std::string& key, double value) {
            self.set(key, value, ConfigSource::User);
        }, py::arg("key"), py::arg("value"), "Set configuration value")
        .def("set", [](Config& self, const std::string& key, float value) {
            self.set(key, value, ConfigSource::User);
        }, py::arg("key"), py::arg("value"), "Set configuration value")
        .def("get", [](Config& self, const std::string& key) {
            auto result = self.get<std::string>(key);
            return result ? *result : std::string("");
        }, py::arg("key"), "Get configuration value as string")
        .def("get", [](Config& self, const std::string& key, int64_t defaultValue) {
            return self.getOr<int64_t>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get configuration value as int64_t")
        .def("get", [](Config& self, const std::string& key, double defaultValue) {
            return self.getOr<double>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get configuration value as double")
        .def("get", [](Config& self, const std::string& key, float defaultValue) {
            return self.getOr<float>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get configuration value as float");

    // Enhanced Brain class with context manager and wrapper
    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain")
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
        .def("__repr__", [](const Brain& b) {
            return "<Brain with " + std::to_string(b.getTotalNeuronCount()) + " neurons>";
        });

    // Context manager for Brain
    py::class_<BrainContextManager>(m, "BrainContextManager")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<AgentBrain>, std::shared_ptr<SimpleWorld>, bool>())
        .def("cleanup", &BrainContextManager::cleanup)
        .def("cleanupOnDestruction", &BrainContextManager::cleanupOnDestruction)
        .def("setCleanupOnDestruction", &BrainContextManager::setCleanupOnDestruction);

    // Enhanced AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain")
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

    // SimpleWorld class
    py::class_<SimpleWorld>(m, "SimpleWorld")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions")
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add world object")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove world object at position")
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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    // Action class
    py::class_<Action>(m, "Action")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);

    // SensoryInput and its subclasses
    py::class_<SensoryInput>(m, "SensoryInput")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    py::class_<Audio, SensoryInput>(m, "Audio")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    // WorldObject class
    py::class_<WorldObject>(m, "WorldObject")
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

    // AgentBody class
    py::class_<AgentBody>(m, "AgentBody")
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

    // SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept")
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

    // Convenience factory for context manager
    m.def("createAgentSystem", [](std::shared_ptr<Config> config, std::shared_ptr<SimpleWorld> world) {
        auto brain = std::make_shared<Brain>(config);
        auto agent = std::make_shared<AgentBrain>(brain);
        auto context = std::make_shared<BrainContextManager>(brain, agent, world);
        return std::make_tuple(brain, agent, context);
    }, py::arg("config"), py::arg("world"),
       "Create a complete agent system with context manager");

    // Invalid ID constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Phase 6 experiment support
    py::class_<Phase6IntegratedExperiment>(m, "Phase6IntegratedExperiment")
        .def(py::init<>())
        .def("verifyIntegration", &Phase6IntegratedExperiment::verifyIntegration,
             "Verify that all systems are properly integrated")
        .def("testMemoryIntegration", &Phase6IntegratedExperiment::testMemoryIntegration,
             "Test memory system integration")
        .def("testNeuromodulationIntegration", &Phase6IntegratedExperiment::testNeuromodulationIntegration,
             "Test neuromodulation system integration")
        .def("testCheckpointing", &Phase6IntegratedExperiment::testCheckpointing,
             "Test checkpoint system")
        .def("testReplay", &Phase6IntegratedExperiment::testReplay,
             "Test memory replay system")
        .def("run", &Phase6IntegratedExperiment::run,
             py::arg("config"), "Run the full Phase 6 experiment");

    // Experimental Python-specific extensions
    m.def("benchmarkBrain", [](std::shared_ptr<Brain> brain, int steps) {
        auto config = brain->getConfig();
        auto startTime = std::chrono::steady_clock::now();
        
        for (int step = 0; step < steps; ++step) {
            brain->step(step);
        }
        
        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);
        
        py::dict result;
        result["steps_performed"] = steps;
        result["total_wall_time"] = duration.count();
        result["average_step_time"] = duration.count() / steps;
        result["total_neurons"] = brain->getTotalNeuronCount();
        result["total_synapses"] = brain->getTotalSynapseCount();
        result["total_spikes"] = brain->getTotalSpikeCount();
        
        return result;
    }, py::arg("brain"), py::arg("steps"),
       "Benchmark brain performance over a number of steps");

    // Python-specific utility functions
    m.def("validateBrainState", [](std::shared_ptr<Brain> brain) {
        py::dict result;
        result["valid"] = brain != nullptr;
        
        if (brain) {
            result["has_config"] = brain->getConfig() != nullptr;
            result["is_initialized"] = brain->getTotalNeuronCount() > 0;
            result["neuron_count"] = brain->getTotalNeuronCount();
            result["synapse_count"] = brain->getTotalSynapseCount();
        }
        
        return result;
    }, py::arg("brain"), "Validate brain state and return status information");

    m.def("createSimulationContext", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                       std::shared_ptr<AgentBrain> agent, bool autoCleanup) {
        return std::make_shared<BrainContextManager>(brain, agent, world, autoCleanup);
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("autoCleanup") = true,
       "Create a simulation context for automatic resource management");

    // Advanced configuration validation utilities
    m.def("validateConfiguration", [](const py::dict& configDict) {
        auto config = std::make_shared<Config>();
        
        // Parse dictionary into config
        for (auto& item : configDict) {
            std::string key = item.first.cast<std::string>();
            if (item.second.is_number_integer()) {
                config->set(key, static_cast<int64_t>(item.second.cast<int64_t>()), ConfigSource::User);
            } else if (item.second.is_number_float()) {
                float value = item.second.cast<float>();
                config->set(key, value, ConfigSource::User);
            } else if (item.second.is_string()) {
                config->set(key, item.second.cast<std::string>(), ConfigSource::User);
            }
        }
        
        py::dict result;
        result["config"] = config;
        result["validation"] = PythonConfigWrapper(config).validate();
        
        return result;
    }, py::arg("configDict"), "Validate configuration from Python dictionary");

    // Enhanced simulation utilities
    m.def("runEpisode", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                        std::shared_ptr<AgentBrain> agent, int steps,
                        bool recordMetrics = false) {
        py::dict result;
        
        if (recordMetrics) {
            // Record initial state
            py::dict metrics;
            metrics["initial_neurons"] = brain->getTotalNeuronCount();
            metrics["initial_synapses"] = brain->getTotalSynapseCount();
            metrics["initial_spikes"] = brain->getTotalSpikeCount();
            result["initial_metrics"] = metrics;
        }
        
        // Run simulation
        for (int step = 0; step < steps; ++step) {
            world->update(0.1);
            agent->processSensoryInput(world->getSensoryPercept());
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            if (recordMetrics && step % 10 == 0) {
                py::dict stepMetrics;
                stepMetrics["step"] = step;
                stepMetrics["neurons"] = brain->getTotalNeuronCount();
                stepMetrics["spikes"] = brain->getTotalSpikeCount();
                stepMetrics["firing"] = brain->getFiringNeuronCount();
                result["metrics_" + std::to_string(step)] = stepMetrics;
            }
        }
        
        if (recordMetrics) {
            // Record final state
            py::dict finalMetrics;
            finalMetrics["final_neurons"] = brain->getTotalNeuronCount();
            finalMetrics["final_synapses"] = brain->getTotalSynapseCount();
            finalMetrics["final_spikes"] = brain->getTotalSpikeCount();
            finalMetrics["final_firing"] = brain->getFiringNeuronCount();
            result["final_metrics"] = finalMetrics;
        }
        
        return result;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"), py::arg("recordMetrics") = false,
       "Run a complete simulation episode with optional metrics recording");

    // Learning utilities
    m.def("extractLearningData", [](std::shared_ptr<Brain> brain) {
        py::dict result;
        
        // Extract learning-related information
        result["total_spikes"] = brain->getTotalSpikeCount();
        result["firing_rate"] = brain->getAverageFiringRate();
        result["development_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        
        // Get region information
        auto regionIds = brain->getRegionIds();
        py::list regionInfo;
        
        for (auto rid : regionIds) {
            auto* region = brain->getRegion(rid);
            if (region) {
                py::dict regionData;
                regionData["id"] = static_cast<uint64_t>(rid.value);
                regionData["neuron_count"] = region->getAllNeurons().size();
                regionData["synapse_count"] = region->getSynapses().size();
                regionInfo.append(regionData);
            }
        }
        
        result["regions"] = regionInfo;
        
        return result;
    }, py::arg("brain"), "Extract learning and development data from brain state");

    // Advanced error handling utilities
    m.def("safeBrainOperation", [](std::shared_ptr<Brain> brain, std::function<py::dict()> operation) {
        py::dict result;
        result["success"] = false;
        result["error"] = "";
        
        try {
            if (!brain || !brain->getConfig()) {
                result["error"] = "Invalid brain or configuration";
                return result;
            }
            
            py::dict operationResult = operation();
            result["success"] = true;
            result["result"] = operationResult;
            
        } catch (const NLMError& e) {
            result["error"] = std::string(e.what());
        } catch (const std::exception& e) {
            result["error"] = "Unexpected error: " + std::string(e.what());
        } catch (...) {
            result["error"] = "Unknown error occurred";
        }
        
        return result;
    }, py::arg("brain"), py::arg("operation"),
       "Safely execute a brain operation with comprehensive error handling");

    // Configuration presets
    m.def("getPresetConfigs", []() {
        py::dict presets;
        
        // Simple preset
        auto simpleConfig = std::make_shared<Config>();
        simpleConfig->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        simpleConfig->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        presets["simple"] = simpleConfig;
        
        // Complex preset
        auto complexConfig = std::make_shared<Config>();
        complexConfig->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        complexConfig->set("region_count", static_cast<int64_t>(5), ConfigSource::Default);
        complexConfig->set("connection_probability", 0.15f, ConfigSource::Default);
        complexConfig->set("plasticity.stdp.enable", true, ConfigSource::Default);
        complexConfig->set("plasticity.hebbian.enable", true, ConfigSource::Default);
        complexConfig->set("neuromod.dopamine.scale", 1.5f, ConfigSource::Default);
        complexConfig->set("neuromod.curiosity.enable", true, ConfigSource::Default);
        presets["complex"] = complexConfig;
        
        return presets;
    }, "Get preset configuration templates");

    // Advanced exploration tools
    m.def("exploreBrainTopology", [](std::shared_ptr<Brain> brain) {
        py::dict topology;
        
        auto regionIds = brain->getRegionIds();
        topology["region_count"] = regionIds.size();
        
        py::list regions;
        for (auto rid : regionIds) {
            auto* region = brain->getRegion(rid);
            if (region) {
                py::dict regionInfo;
                regionInfo["id"] = static_cast<uint64_t>(rid.value);
                
                auto neurons = region->getAllNeurons();
                regionInfo["neuron_count"] = neurons.size();
                
                auto synapses = region->getSynapses();
                regionInfo["synapse_count"] = synapses.size();
                
                // Analyze neuron types
                py::dict neuronTypes;
                neuronTypes["excitatory"] = 0;
                neuronTypes["inhibitory"] = 0;
                neuronTypes["modulatory"] = 0;
                
                for (auto* neuron : neurons) {
                    neuronTypes[static_cast<int>(neuron->getType())]++;
                }
                
                regionInfo["neuron_types"] = neuronTypes;
                regions.append(regionInfo);
            }
        }
        
        topology["regions"] = regions;
        
        return topology;
    }, py::arg("brain"), "Explore and analyze brain topology");

    // Resource monitoring
    m.def("monitorResources", [](std::shared_ptr<Brain> brain) {
        py::dict resources;
        
        resources["brain_info"] = PythonBrainWrapper(brain, brain->getConfig()).info();
        
        // System resource simulation
        resources["simulated_memory_usage_mb"] = 50.0 + (brain->getTotalNeuronCount() * 0.1);
        resources["simulated_cpu_usage_percent"] = 10.0 + (brain->getTotalSynapseCount() * 0.01);
        
        return resources;
    }, py::arg("brain"), "Monitor resource usage and system status");

    // Export and import utilities
    m.def("exportBrainState", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        bool success = brain->save(filepath);
        if (!success) {
            throw SimulationError("Failed to export brain state to " + filepath);
        }
        return success;
    }, py::arg("brain"), py::arg("filepath"),
       "Export brain state to file");

    m.def("importBrainState", [](const std::string& filepath) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->load(filepath)) {
            throw SimulationError("Failed to import brain state from " + filepath);
        }
        
        return brain;
    }, py::arg("filepath"), "Import brain state from file");

    // Version and info
    m.attr("__version__") = "2.0.0";  // Enhanced version
    m.attr("__description__") = "Enhanced NLM Python bindings with context managers, performance monitoring, and advanced validation";
    m.attr("__enhanced__") = true;

    // Example demonstrations (only when in debug mode)
    m.def("example_simple_brain", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        for (int step = 0; step < 50; ++step) {
            brain->step(step);
        }
        
        py::dict result;
        result["neurons"] = brain->getTotalNeuronCount();
        result["spikes"] = brain->getTotalSpikeCount();
        result["success"] = true;
        
        return result;
    }, "Simple brain example");

    m.def("example_agent_world", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(200), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        auto world = std::make_shared<SimpleWorld>();
        auto agent = std::make_shared<AgentBrain>(brain);
        
        world->configure(10, 10, 6, 6);
        world->reset();
        world->setAgentStart(5.0, 5.0);
        
        agent->initialize(world);
        
        for (int step = 0; step < 100; ++step) {
            world->update(0.1);
            agent->processSensoryInput(world->getSensoryPercept());
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
        
        py::dict result;
        result["steps"] = 100;
        result["neurons"] = brain->getTotalNeuronCount();
        result["spikes"] = brain->getTotalSpikeCount();
        result["firing"] = brain->getFiringNeuronCount();
        result["success"] = true;
        
        return result;
    }, "Agent-world interaction example");

    m.def("example_phase6_experiment", []() {
        auto experiment = std::make_unique<Phase6IntegratedExperiment>();
        
        Phase6Config config;
        config.neuronCount = 300;
        config.maxSteps = 500;
        config.enableCheckpointing = true;
        config.enableReplay = true;
        config.enableDevelopment = true;
        
        auto result = experiment->run(config);
        
        py::dict pyResult;
        pyResult["success"] = true;
        pyResult["total_reward"] = result.totalReward;
        pyResult["avg_firing_rate"] = result.avgFiringRate;
        pyResult["memory_episodes_stored"] = result.memoryEpisodesStored;
        pyResult["dopamine_level"] = result.dopamineLevel;
        
        return pyResult;
    }, "Phase 6 experiment example");
}

} // namespace nlm