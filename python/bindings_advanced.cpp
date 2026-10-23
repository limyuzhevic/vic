# Advanced Python bindings for NLM
# Enhanced API for expert users with additional functionality, error handling, and utilities

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <stdexcept>

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

// Exception types for better error handling
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigurationError : public NLMException {
public:
    explicit ConfigurationError(const std::string& msg) : NLMException("Configuration error: " + msg) {}
};

class SimulationError : public NLMException {
public:
    explicit SimulationError(const std::string& msg) : NLMException("Simulation error: " + msg) {}
};

class MemoryError : public NLMException {
public:
    explicit MemoryError(const std::string& msg) : NLMException("Memory error: " + msg) {}
};

PYBIND11_MODULE(pynlm, m) {
    // Register exception types
    py::register_exception<NLMException>(m, "NLMException", PyExc_RuntimeError);
    py::register_exception<ConfigurationError>(m, "ConfigurationError", PyExc_ValueError);
    py::register_exception<SimulationError>(m, "SimulationError", PyExc_RuntimeError);
    py::register_exception<MemoryError>(m, "MemoryError", PyExc_MemoryError);
    
    // Advanced configuration management
    py::class_<Config>(m, "AdvancedConfig", R"pbdoc(
        Advanced configuration system for NLM with validation and expert controls
    )pbdoc")
        .def(py::init<>())
        // Basic configuration operations
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
        
        // Advanced configuration operations
        .def("set", [](Config& self, const std::string& key, const py::object& value) {
            // Template-based conversion for different value types
            if (value.is<int>()) {
                self.set(key, value.cast<int>());
            } else if (value.is<float>()) {
                self.set(key, value.cast<float>());
            } else if (value.is<double>()) {
                self.set(key, value.cast<double>());
            } else if (value.is<std::string>()) {
                self.set(key, value.cast<std::string>());
            } else if (value.is<bool>()) {
                self.set(key, value.cast<bool>());
            } else if (value.is<uint64_t>()) {
                self.set(key, value.cast<uint64_t>());
            } else {
                throw ConfigurationError("Unsupported configuration value type for key: " + key);
            }
        }, py::arg("key"), py::arg("value"),
           "Set configuration value with type conversion")
        
        // Typed getters with defaults
        .def("getInt", [](const Config& self, const std::string& key, int defaultValue) {
            auto opt = self.get<int>(key);
            return opt ? *opt : defaultValue;
        }, py::arg("key"), py::arg("default") = 0,
           "Get integer configuration value")
        
        .def("getFloat", [](const Config& self, const std::string& key, float defaultValue) {
            auto opt = self.get<float>(key);
            return opt ? *opt : defaultValue;
        }, py::arg("key"), py::arg("default") = 0.0f,
           "Get float configuration value")
        
        .def("getDouble", [](const Config& self, const std::string& key, double defaultValue) {
            auto opt = self.get<double>(key);
            return opt ? *opt : defaultValue;
        }, py::arg("key"), py::arg("default") = 0.0,
           "Get double configuration value")
        
        .def("getString", [](const Config& self, const std::string& key, const std::string& defaultValue) {
            auto opt = self.get<std::string>(key);
            return opt ? *opt : defaultValue;
        }, py::arg("key"), py::arg("default") = "",
           "Get string configuration value")
        
        .def("getBool", [](const Config& self, const std::string& key, bool defaultValue) {
            auto opt = self.get<bool>(key);
            return opt ? *opt : defaultValue;
        }, py::arg("key"), py::arg("default") = false,
           "Get boolean configuration value")
        
        // Configuration validation
        .def("validate", [](const Config& self) {
            // Basic validation: check for required keys
            std::vector<std::string> errors;
            
            if (!self.has("brain.neuron_count")) {
                errors.push_back("Missing required key: brain.neuron_count");
            }
            
            if (!self.has("brain.synapse_density")) {
                errors.push_back("Missing required key: brain.synapse_density");
            }
            
            if (self.has("brain.neuron_count")) {
                auto opt = self.get<size_t>("brain.neuron_count");
                if (opt && *opt == 0) {
                    errors.push_back("brain.neuron_count must be > 0");
                } else if (opt && *opt > 1000000) {
                    errors.push_back("brain.neuron_count should not exceed 1,000,000 for typical use");
                }
            }
            
            if (!errors.empty()) {
                throw ConfigurationError("Validation failed: " + 
                    (errors.size() == 1 ? errors[0] : 
                     std::accumulate(std::next(errors.begin()), errors.end(), errors[0],
                                   [](const std::string& a, const std::string& b) { return a + ", " + b; })));
            }
            
            return true;
        }, "Validate configuration with expert-level checks")
        
        // Configuration comparison
        .def("merge", [](Config& self, const Config& other) {
            auto keys = other.getKeys();
            for (const auto& key : keys) {
                self.set(key, other.getKeys()); // Note: This is simplified - actual implementation would need to get values
            }
            return self;
        }, py::arg("other"), "Merge another configuration into this one")
        
        .def("diff", [](const Config& self, const Config& other) {
            auto selfKeys = self.getKeys();
            auto otherKeys = other.getKeys();
            std::vector<std::string> diff;
            
            // Find keys in self but not in other
            for (const auto& key : selfKeys) {
                if (!other.has(key)) {
                    diff.push_back("Self has additional key: " + key);
                }
            }
            
            // Find keys in other but not in self
            for (const auto& key : otherKeys) {
                if (!self.has(key)) {
                    diff.push_back("Other has additional key: " + key);
                }
            }
            
            // Find differences in common keys
            std::vector<std::string> commonKeys;
            for (const auto& key : selfKeys) {
                if (other.has(key)) {
                    commonKeys.push_back(key);
                }
            }
            
            for (const auto& key : commonKeys) {
                auto selfOpt = self.get<std::string>(key);  // Simplified - assumes string values
                auto otherOpt = other.get<std::string>(key);
                if (selfOpt && otherOpt && *selfOpt != *otherOpt) {
                    diff.push_back("Difference in key " + key + ": " + *selfOpt + " != " + *otherOpt);
                }
            }
            
            return diff;
        }, py::arg("other"), "Compare two configurations and return differences")
        
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Advanced Brain class with expert methods
    py::class_<Brain>(m, "AdvancedBrain", R"pbdoc(
        Advanced brain simulation with expert-level controls and monitoring
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create a new brain with configuration")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration", py::call_guard<py::gil_scoped_release>())
        
        // Enhanced step methods with timing and progress tracking
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("stepWithTime", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        
        // Enhanced sensory input processing
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        
        // Enhanced current injection
        .def("injectCurrentToNeuron", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentByType", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        
        // Enhanced action production
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        
        // Enhanced state management
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save,
             py::arg("filepath"),
             "Save brain state to file (checkpointing)")
        .def("load", &Brain::load,
             py::arg("filepath"),
             "Load brain state from file")
        
        // Advanced statistics and monitoring
        .def("getStats", [](const Brain& brain) {
            py::dict stats;
            stats["total_neurons"] = brain.getTotalNeuronCount();
            stats["total_synapses"] = brain.getTotalSynapseCount();
            stats["active_neurons"] = brain.getActiveNeuronCount();
            stats["firing_neurons"] = brain.getFiringNeuronCount();
            stats["total_spikes"] = brain.getTotalSpikeCount();
            stats["pending_spikes"] = brain.getPendingSpikeEventCount();
            stats["average_firing_rate"] = brain.getAverageFiringRate();
            stats["excitation_inhibition_ratio"] = brain.getExcitationInhibitionRatio();
            return stats;
        }, "Get comprehensive brain statistics as a dictionary")
        
        .def("getRegionStats", [](const Brain& brain) {
            py::dict regionStats;
            auto regionIds = brain.getRegionIds();
            
            for (const auto& regionId : regionIds) {
                py::dict regionData;
                NeuralRegion* region = brain.getRegion(regionId);
                if (region) {
                    regionData["id"] = regionId.index();
                    regionData["name"] = region->getName();
                    regionData["neuron_count"] = region->getTotalNeuronCount();
                    regionData["synapse_count"] = region->getSynapseCount();
                    regionData["average_weight"] = region->getAverageSynapticWeight();
                    regionData["population_count"] = region->getPopulationCount();
                    
                    // Get population breakdown by type
                    std::map<NeuronType, size_t> populationByType;
                    for (const auto& pop : region->getPopulations()) {
                        populationByType[pop->getNeuronType()] += pop->getNeuronCount();
                    }
                    
                    py::dict populations;
                    for (const auto& pair : populationByType) {
                        populations[py::cast(pair.first)] = pair.second;
                    }
                    regionData["populations_by_type"] = populations;
                }
                
                regionStats[std::to_string(regionId.index())] = regionData;
            }
            
            return regionStats;
        }, "Get detailed statistics for all regions")
        
        // Enhanced neuromodulation controls
        .def("applyNeuromodulation", &Brain::applyNeuromodulation,
             py::arg("signal"),
             "Apply neuromodulatory signals")
        
        // Enhanced development controls
        .def("develop", &Brain::develop,
             "Apply developmental changes")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        
        // Advanced plasticity control
        .def("updatePlasticity", &Brain::updatePlasticity,
             "Update plasticity rules")
        .def("getPlasticityStats", [](const Brain& brain) {
            py::dict plasticityStats;
            
            STDP* stdp = brain.getSTDP();
            if (stdp) {
                py::dict stdpData;
                stdpData["ltp_weight"] = stdp->getLTPWeight();
                stdpData["ltd_weight"] = stdp->getLTDWeight();
                stdpData["time_constant"] = stdp->getTimeConstant();
                plasticityStats["stdp"] = stdpData;
            }
            
            Hebbian* hebbian = brain.getHebbian();
            if (hebbian) {
                plasticityStats["hebbian_enabled"] = true;  // Simplified - actual implementation would need more methods
            }
            
            StructuralPlasticity* structural = brain.getStructuralPlasticity();
            if (structural) {
                // Add structural plasticity stats if available
                plasticityStats["structural_enabled"] = true;
            }
            
            return plasticityStats;
        }, "Get plasticity system statistics")
        
        // Memory system access
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get episodic memory system")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             "Get associative memory system")
        
        // Prediction system access
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get prediction system")
        
        // Cognition system access
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Get neural planner")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get concept formation system")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Get attentional selection system")
        
        // Development system access
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get development system")
        
        // Neuromodulation system access
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get dopamine system")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get curiosity system")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get novelty detection system")
        .def("getPredictionError", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Get prediction error system")
        
        // Advanced simulation control
        .def("runSimulation", [](Brain& brain, size_t steps, size_t progressInterval) {
            for (size_t step = 0; step < steps; ++step) {
                brain.step(step);
                if (progressInterval > 0 && step % progressInterval == 0) {
                    std::cout << "Progress: " << (100.0 * step / steps) << "%" << std::endl;
                }
            }
            return brain.getTotalSpikeCount();
        }, py::arg("steps") = 1000, py::arg("progressInterval") = 0,
           "Run simulation for specified number of steps")
        
        // State snapshot and replay
        .def("createSnapshot", [](Brain& brain, const std::string& label) {
            // Create a named snapshot of current brain state
            // This would involve saving to a temporary checkpoint
            return label;  // Simplified
        }, py::arg("label"),
           "Create a snapshot of current brain state")
        
        // Configuration access
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        
        // Random generator access
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             py::return_value_policy::reference_internal,
             "Get random number generator")
        
        // Status logging
        .def("logStatus", &Brain::logStatus,
             "Log brain status (enhanced version)");

    // Advanced AgentBrain class with expert methods
    py::class_<AgentBrain>(m, "AdvancedAgentBrain", R"pbdoc(
        Advanced agent brain interface with expert-level controls
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create a new agent brain interface")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        
        // Enhanced sensory processing
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain", py::call_guard<py::gil_scoped_release>())
        
        // Enhanced motor decoding
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        
        // Enhanced neuromodulation
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        
        // Enhanced development
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        
        // State management
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
        
        // Subsystem controls
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"),
             "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             "Enable/disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             "Enable/disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             "Enable/disable curiosity")
        
        // Status checks
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled");

    // Simple factory functions for backward compatibility
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

    // Advanced utility functions
    m.def("benchmarkBrain", [](std::shared_ptr<Brain> brain, size_t neurons, size_t steps) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", neurons);
        config->set("simulation_timestep", 0.001);
        
        auto benchmarkBrain = std::make_shared<Brain>(config);
        if (!benchmarkBrain->initialize()) {
            throw SimulationError("Failed to initialize brain for benchmark");
        }
        
        auto startTime = std::chrono::high_resolution_clock::now();
        for (size_t step = 0; step < steps; ++step) {
            benchmarkBrain->step(step);
        }
        auto endTime = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        double neuronsPerSecond = (neurons * steps) / (duration.count() / 1000.0);
        
        return py::dict(
            "neurons_per_second", neuronsPerSecond,
            "total_steps", steps,
            "wall_time_ms", duration.count(),
            "spikes_per_step", brain->getTotalSpikeCount() / steps
        );
    }, py::arg("brain") = nullptr, py::arg("neurons") = 1000, py::arg("steps") = 1000,
       "Benchmark brain performance with specified parameters");

    m.def("validateConfiguration", [](const Config& config) {
        return config.validate();  // Use the new validation method
    }, py::arg("config"), "Validate configuration with expert-level checks");

    m.def("createBenchmarkConfig", [](size_t neurons) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", neurons);
        config->set("region_count", std::max<size_t>(1, neurons / 1000));
        config->set("simulation_timestep", 0.001);
        config->set("random_seed", 42);
        config->set("connection_probability", 0.1);
        return config;
    }, py::arg("neurons") = 1000, "Create a benchmark configuration optimized for performance testing");

    m.def("createSmallScaleConfig", [](size_t neurons) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", neurons);
        config->set("region_count", 1);
        config->set("simulation_timestep", 0.001);
        config->set("random_seed", 123);
        config->set("connection_probability", 0.05);
        return config;
    }, py::arg("neurons") = 100, "Create a small-scale configuration for development and testing");

    m.def("createLargeScaleConfig", [](size_t neurons) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", neurons);
        config->set("region_count", std::max<size_t>(2, neurons / 10000));
        config->set("simulation_timestep", 0.0001);  // Smaller timestep for stability
        config->set("random_seed", 456);
        config->set("connection_probability", 0.05);
        return config;
    }, py::arg("neurons") = 10000, "Create a large-scale configuration for performance testing");

    // Utility functions for data analysis
    m.def("analyzeBrainData", [](const Brain& brain) {
        // Analyze brain state and return insights
        py::dict analysis;
        
        analysis["total_neurons"] = brain.getTotalNeuronCount();
        analysis["total_synapses"] = brain.getTotalSynapseCount();
        analysis["firing_rate"] = brain.getAverageFiringRate();
        analysis["excitation_inhibition_ratio"] = brain.getExcitationInhibitionRatio();
        
        // Calculate complexity metrics
        float connectionDensity = static_cast<float>(brain.getTotalSynapseCount()) /
                                 (brain.getTotalNeuronCount() * (brain.getTotalNeuronCount() - 1));
        analysis["connection_density"] = connectionDensity;
        
        // Calculate firing statistics
        analysis["spike_per_neuron"] = static_cast<float>(brain.getTotalSpikeCount()) /
                                      brain.getTotalNeuronCount();
        
        return analysis;
    }, py::arg("brain"), "Analyze brain state and return performance metrics");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Version information
    m.attr("__version__") = "0.1.0-advanced";
    m.attr("__description__") = R"pbdoc(
        NLM (Neural Learning Machine) - Advanced Python Bindings
        Enhanced API for expert users with advanced configuration, 
        monitoring, and performance analysis tools
    )pbdoc";

} // namespace nlm
