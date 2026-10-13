#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <algorithm>
#include <sstream>

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

// Python exception types
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& msg) : NLMError("Configuration error: " + msg) {}
};

class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& msg) : NLMError("Simulation error: " + msg) {}
};

// Configuration convenience functions
namespace config {
    // Create a configuration from a Python dictionary
    std::shared_ptr<Config> from_dict(const py::dict& dict) {
        auto config = std::make_shared<Config>();
        
        for (auto item : dict) {
            std::string key = py::str(item.first);
            
            if (item.second.is(py::int_())) {
                config->set(key, item.second.cast<int64_t>());
            } else if (item.second.is(py::float_())) {
                config->set(key, item.second.cast<float>());
            } else if (item.second.is(py::bool_())) {
                config->set(key, item.second.cast<bool>());
            } else if (item.second.is(py::str())) {
                config->set(key, item.second.cast<std::string>());
            } else if (item.second.is(py::list())) {
                // Handle list types
                py::list pyList = item.second.cast<py::list>();
                if (pyList.empty()) continue;
                
                // Determine list type from first element
                if (pyList[0].is(py::int_())) {
                    std::vector<int64_t> intList;
                    intList.reserve(pyList.size());
                    for (auto elem : pyList) {
                        intList.push_back(elem.cast<int64_t>());
                    }
                    config->set(key, intList);
                } else if (pyList[0].is(py::float_())) {
                    std::vector<float> floatList;
                    floatList.reserve(pyList.size());
                    for (auto elem : pyList) {
                        floatList.push_back(elem.cast<float>());
                    }
                    config->set(key, floatList);
                } else if (pyList[0].is(py::str())) {
                    std::vector<std::string> stringList;
                    stringList.reserve(pyList.size());
                    for (auto elem : pyList) {
                        stringList.push_back(elem.cast<std::string>());
                    }
                    config->set(key, stringList);
                }
            }
        }
        
        return config;
    }
    
    // Get configuration as Python dictionary
    py::dict to_dict(const Config& config) {
        py::dict dict;
        
        auto keys = config.getKeys();
        for (const auto& key : keys) {
            // Try to get as string first, then try other types
            auto strValue = config.get<std::string>(key);
            if (strValue) {
                dict[key] = *strValue;
                continue;
            }
            
            auto intValue = config.get<int64_t>(key);
            if (intValue) {
                dict[key] = *intValue;
                continue;
            }
            
            auto floatValue = config.get<float>(key);
            if (floatValue) {
                dict[key] = *floatValue;
                continue;
            }
            
            auto boolValue = config.get<bool>(key);
            if (boolValue) {
                dict[key] = *boolValue;
                continue;
            }
        }
        
        return dict;
    }
    
    // Preset configurations
    std::shared_ptr<Config> create_basic_config() {
        auto config = std::make_shared<Config>();
        
        // Basic parameters
        config->set("neuron_count", static_cast<int64_t>(500));
        config->set("region_count", static_cast<int64_t>(1));
        config->set("simulation_timestep", 0.001);
        config->set("random_seed", static_cast<int64_t>(42));
        
        // Connectivity
        config->set("connection_probability", 0.15f);
        
        // Plasticity parameters
        config->set("stdp_ltp_weight", 0.02f);
        config->set("stdp_ltd_weight", 0.015f);
        config->set("stdp_tau", 20.0f);
        
        // Structural plasticity
        config->set("synaptogenesis_rate", 0.0001f);
        config->set("pruning_rate", 0.00001f);
        
        return config;
    }
    
    std::shared_ptr<Config> create_learning_config() {
        auto config = create_basic_config();
        
        // Enhanced learning parameters
        config->set("neuron_count", static_cast<int64_t>(1000));
        config->set("stdp_ltp_weight", 0.03f);
        config->set("stdp_ltd_weight", 0.02f);
        config->set("stdp_tau", 25.0f);
        config->set("synaptogenesis_rate", 0.0002f);
        
        return config;
    }
    
    std::shared_ptr<Config> create_fast_config() {
        auto config = create_basic_config();
        
        // Faster, smaller simulation
        config->set("neuron_count", static_cast<int64_t>(100));
        config->set("stdp_tau", 10.0f);
        config->set("simulation_timestep", 0.002);
        
        return config;
    }
    
    std::shared_ptr<Config> load_config(const std::string& filename) {
        auto config = std::make_shared<Config>();
        if (config->loadFromFile(filename)) {
            return config;
        }
        return create_basic_config();
    }
}

// Brain factory functions
namespace brain {
    // Create brain with default configuration
    std::shared_ptr<Brain> create() {
        return std::make_shared<Brain>(config::create_basic_config());
    }
    
    // Create brain with custom configuration
    std::shared_ptr<Brain> create_with_config(const std::shared_ptr<Config>& config) {
        return std::make_shared<Brain>(config);
    }
    
    // Create brain with dictionary configuration
    std::shared_ptr<Brain> create_from_dict(const py::dict& dict) {
        return std::make_shared<Brain>(config::from_dict(dict));
    }
    
    // Create brain with preset name
    std::shared_ptr<Brain> create_preset(const std::string& preset) {
        if (preset == "basic") {
            return config::create_basic_config();
        } else if (preset == "learning") {
            return config::create_learning_config();
        } else if (preset == "fast") {
            return config::create_fast_config();
        } else {
            throw ConfigurationError("Unknown preset: " + preset);
        }
    }
}

// Simulation utilities
namespace simulation {
    // Create a simple world with default configuration
    std::shared_ptr<SimpleWorld> create_world(int width = 100, int height = 100,
                                             int visionWidth = 7, int visionHeight = 7) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        return world;
    }
    
    // Create an agent brain with world
    std::shared_ptr<AgentBrain> create_agent_brain(std::shared_ptr<Brain> brain, 
                                                    std::shared_ptr<SimpleWorld> world) {
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        agentBrain->initialize(world);
        return agentBrain;
    }
}

// Learning experiment utilities
namespace experiments {
    // Simple learning experiment
    struct LearningExperiment {
        std::shared_ptr<Brain> brain;
        uint64_t seed;
        size_t initialSynapseCount;
        std::vector<float> initialWeights;
        std::vector<float> finalWeights;
        std::vector<NeuronId> mostActiveNeurons;
        
        LearningExperiment(std::shared_ptr<Brain> b, uint64_t s)
            : brain(b), seed(s), initialSynapseCount(0) {}
        
        void recordInitialState() {
            initialSynapseCount = brain->getTotalSynapseCount();
            initialWeights.clear();
            
            if (auto* region = brain->getRegion(RegionId(1))) {
                for (const auto& syn : region->getSynapses()) {
                    initialWeights.push_back(syn->getWeight());
                }
            }
        }
        
        void recordFinalState() {
            finalWeights.clear();
            
            if (auto* region = brain->getRegion(RegionId(1))) {
                for (const auto& syn : region->getSynapses()) {
                    finalWeights.push_back(syn->getWeight());
                }
            }
            
            mostActiveNeurons = brain->getSpikeSystem()->getMostActiveNeurons(10);
        }
        
        py::dict getMetrics() const {
            py::dict metrics;
            
            metrics["initial_synapse_count"] = initialSynapseCount;
            metrics["final_synapse_count"] = brain->getTotalSynapseCount();
            metrics["total_spikes"] = brain->getTotalSpikeCount();
            metrics["initial_mean_weight"] = initialWeights.empty() ? 0.0f : 
                std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f) / initialWeights.size();
            metrics["final_mean_weight"] = finalWeights.empty() ? 0.0f : 
                std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f) / finalWeights.size();
            metrics["weight_change"] = metrics["final_mean_weight"].cast<float>() - 
                                      metrics["initial_mean_weight"].cast<float>();
            
            size_t strengthened = 0, weakened = 0, unchanged = 0;
            size_t minSize = std::min(initialWeights.size(), finalWeights.size());
            
            for (size_t i = 0; i < minSize; ++i) {
                float delta = finalWeights[i] - initialWeights[i];
                if (delta > 0.01f) strengthened++;
                else if (delta < -0.01f) weakened++;
                else unchanged++;
            }
            
            metrics["strengthened"] = strengthened;
            metrics["weakened"] = weakened;
            metrics["unchanged"] = unchanged;
            metrics["learning_detected"] = (std::abs(metrics["weight_change"].cast<float>()) > 0.001f) ||
                                         (strengthened > 0 || weakened > 0);
            
            return metrics;
        }
    };
    
    // Run a simple learning experiment
    LearningExperiment run_learning_experiment(std::shared_ptr<Brain> brain, uint64_t seed = 42, 
                                               int steps = 1000) {
        auto experiment = LearningExperiment(brain, seed);
        
        experiment.recordInitialState();
        
        // Enable plasticity
        if (auto region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);
            }
        }
        
        // Apply learning stimulus
        for (SimulationStep step = 0; step < steps; ++step) {
            for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            brain->step(step, step * 0.001);
        }
        
        experiment.recordFinalState();
        return experiment;
    }
}

// Context manager for experiment sessions
class ExperimentSession {
public:
    ExperimentSession(std::shared_ptr<Brain> brain, int steps = 1000)
        : brain_(brain), steps_(steps), started_(false) {}
    
    void __enter__() {
        started_ = true;
        if (auto region = brain_->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);
            }
        }
    }
    
    py::dict __exit__(int type, py::object value, py::object traceback) {
        // Run experiment
        for (SimulationStep step = 0; step < steps_; ++step) {
            brain_->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            brain_->step(step, step * 0.001);
        }
        
        // Get metrics
        py::dict metrics;
        metrics["total_spikes"] = brain_->getTotalSpikeCount();
        metrics["final_weights_count"] = brain_->getRegion(RegionId(1)) ? 
            brain_->getRegion(RegionId(1))->getSynapses().size() : 0;
        
        started_ = false;
        return metrics;
    }
    
    bool is_started() const { return started_; }
    
private:
    std::shared_ptr<Brain> brain_;
    int steps_;
    bool started_;
};

// Helper function to convert Python list to std::vector
template<typename T>
std::vector<T> pyListToStdVector(const py::list& pyList) {
    std::vector<T> result;
    result.reserve(pyList.size());
    for (auto item : pyList) {
        result.push_back(item.cast<T>());
    }
    return result;
}

// Initialize Python module with enhanced API
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------------
        
        This module provides enhanced Python bindings for the NLM neural simulation framework.
        It includes convenience functions, type conversions, and context managers for
        easier use of the NLM library.
        
        Key features:
        - Configuration helpers with dictionary support
        - Brain factory functions
        - Experiment utilities with built-in metrics
        - Context managers for experiment sessions
        - Comprehensive error handling
        - Enhanced documentation strings
    )pbdoc";
    
    // Register custom exception types
    py::register_exception<NLMError>(m, "NLMError");
    py::register_exception<ConfigurationError>(m, "ConfigurationError");
    py::register_exception<SimulationError>(m, "SimulationError");
    
    // Configuration class
    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration class for NLM system with enhanced Python interface
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
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        // Enhanced getter methods
        .def("get", [](const Config& self, const std::string& key) {
            auto value = self.get<std::string>(key);
            if (!value) {
                throw py::key_error("Key not found: " + key);
            }
            return *value;
        }, py::arg("key"), "Get configuration value as string")
        .def("get", [](const Config& self, const std::string& key, const py::object& defaultValue) {
            auto value = self.get<std::string>(key);
            if (!value) {
                if (defaultValue.is(py::none())) {
                    throw py::key_error("Key not found: " + key);
                }
                return defaultValue;
            }
            return py::str(*value);
        }, py::arg("key"), py::arg("default"), "Get configuration value with default")
        .def("getInt", [](const Config& self, const std::string& key, int64_t defaultValue = 0) {
            return self.getOr<int64_t>(key, defaultValue);
        }, py::arg("key"), py::arg("default") = 0, "Get integer configuration value")
        .def("getFloat", [](const Config& self, const std::string& key, float defaultValue = 0.0f) {
            return self.getOr<float>(key, defaultValue);
        }, py::arg("key"), py::arg("default") = 0.0f, "Get float configuration value")
        .def("getBool", [](const Config& self, const std::string& key, bool defaultValue = false) {
            return self.getOr<bool>(key, defaultValue);
        }, py::arg("key"), py::arg("default") = false, "Get boolean configuration value")
        .def("set", [](Config& self, const std::string& key, const py::object& value) {
            if (value.is(py::int_())) {
                self.set(key, value.cast<int64_t>());
            } else if (value.is(py::float_())) {
                self.set(key, value.cast<float>());
            } else if (value.is(py::bool_())) {
                self.set(key, value.cast<bool>());
            } else if (value.is(py::str())) {
                self.set(key, value.cast<std::string>());
            } else if (value.is(py::list())) {
                // Handle list types
                auto pyList = value.cast<py::list>();
                if (pyList.empty()) return;
                
                if (pyList[0].is(py::int_())) {
                    auto intList = pyListToStdVector<int64_t>(pyList);
                    self.set(key, intList);
                } else if (pyList[0].is(py::float_())) {
                    auto floatList = pyListToStdVector<float>(pyList);
                    self.set(key, floatList);
                } else if (pyList[0].is(py::str())) {
                    auto stringList = pyListToStdVector<std::string>(pyList);
                    self.set(key, stringList);
                }
            } else {
                throw ConfigurationError("Unsupported value type for configuration key: " + key);
            }
        }, py::arg("key"), py::arg("value"), "Set configuration value")
        .def("update", &Config::set, py::arg("key"), py::arg("value"),
             "Update configuration value")
        .def("merge", [](Config& self, const py::dict& dict) {
            // Merge dictionary into config
            for (auto item : dict) {
                std::string key = py::str(item.first);
                if (item.second.is(py::int_())) {
                    self.set(key, item.second.cast<int64_t>());
                } else if (item.second.is(py::float_())) {
                    self.set(key, item.second.cast<float>());
                } else if (item.second.is(py::bool_())) {
                    self.set(key, item.second.cast<bool>());
                } else if (item.second.is(py::str())) {
                    self.set(key, item.second.cast<std::string>());
                }
            }
        }, py::arg("dict"), "Merge dictionary into configuration");
    
    // NeuronType enum
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();
    
    // SynapseType enum
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();
    
    // ConfigSource enum
    py::enum_<ConfigSource>(m, "ConfigSource", R"pbdoc(Configuration source)pbdoc")
        .value("Default", ConfigSource::Default)
        .value("File", ConfigSource::File)
        .value("CommandLine", ConfigSource::CommandLine)
        .value("Runtime", ConfigSource::Runtime)
        .export_values();
    
    // Brain class
    py::class_<Brain>(m, "Brain", R"pbdoc(
        Brain class for NLM neural simulation
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration")
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
        .def("getOr", [](const Brain& self, const std::string& key, const py::object& defaultValue) {
            auto config = self.getConfig();
            if (!config) return defaultValue;
            
            if (key.find("int") != std::string::npos || key.find("count") != std::string::npos || 
                key.find("step") != std::string::npos) {
                auto value = config->getOr<int64_t>(key, defaultValue.cast<int64_t>());
                return py::int_(value);
            } else if (key.find("float") != std::string::npos || key.find("weight") != std::string::npos ||
                       key.find("rate") != std::string::npos || key.find("tau") != std::string::npos) {
                auto value = config->getOr<float>(key, defaultValue.cast<float>());
                return py::float_(value);
            } else if (key.find("bool") != std::string::npos) {
                auto value = config->getOr<bool>(key, defaultValue.cast<bool>());
                return py::bool_(value);
            }
            
            auto value = config->getOr<std::string>(key, defaultValue.cast<std::string>());
            return py::str(value);
        }, py::arg("key"), py::arg("default"), "Get configuration value with type conversion")
        .def("setConfig", [](Brain& self, const py::dict& dict) {
            auto config = self.getConfig();
            if (config) {
                // Merge dictionary into config
                for (auto item : dict) {
                    std::string key = py::str(item.first);
                    if (item.second.is(py::int_())) {
                        config->set(key, item.second.cast<int64_t>());
                    } else if (item.second.is(py::float_())) {
                        config->set(key, item.second.cast<float>());
                    } else if (item.second.is(py::bool_())) {
                        config->set(key, item.second.cast<bool>());
                    } else if (item.second.is(py::str())) {
                        config->set(key, item.second.cast<std::string>());
                    }
                }
            }
        }, py::arg("dict"), "Update brain configuration from dictionary");
    
    // AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to world
    )pbdoc")
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
    
    // Context manager registration
    py::class_<ExperimentSession>(m, "ExperimentSession", 
        R"pbdoc(Context manager for experiment sessions with automatic setup and cleanup)pbdoc")
        .def(py::init<std::shared_ptr<Brain>, int>(), py::arg("brain"), py::arg("steps") = 1000,
             "Create experiment session with brain and step count")
        .def("__enter__", &ExperimentSession::__enter__,
             "Enter experiment session (enables plasticity)")
        .def("__exit__", &ExperimentSession::__exit__,
             py::arg("type"), py::arg("value"), py::arg("traceback"),
             "Exit experiment session (collects metrics)")
        .def("is_started", &ExperimentSession::is_started,
             "Check if experiment session is active");
    
    // Factory function registrations
    m.def("createDefaultConfig", &config::create_basic_config,
          "Create default configuration");
    
    m.def("createLearningConfig", &config::create_learning_config,
          "Create learning-focused configuration");
    
    m.def("createFastConfig", &config::create_fast_config,
          "Create fast simulation configuration");
    
    m.def("loadConfig", &config::load_config, py::arg("filename"),
          "Load configuration from file");
    
    m.def("createBrain", &brain::create,
          "Create brain with default configuration");
    
    m.def("createBrainFromConfig", &brain::create_with_config, py::arg("config"),
          "Create brain with custom configuration");
    
    m.def("createBrainFromDict", &brain::create_from_dict, py::arg("dict"),
          "Create brain from dictionary configuration");
    
    m.def("createWorld", &simulation::create_world,
          py::arg("width") = 100, py::arg("height") = 100,
          py::arg("visionWidth") = 7, py::arg("visionHeight") = 7,
          "Create simple world with default configuration");
    
    m.def("createAgentBrain", &simulation::create_agent_brain,
          py::arg("brain"), py::arg("world"),
          "Create agent brain interface");
    
    m.def("runLearningExperiment", &experiments::run_learning_experiment,
          py::arg("brain"), py::arg("seed") = 42, py::arg("steps") = 1000,
          "Run learning experiment and return metrics");
    
    // Convenience constants
    m.attr("DEFAULT_NEURON_COUNT") = 500;
    m.attr("DEFAULT_REGION_COUNT") = 1;
    m.attr("DEFAULT_SIMULATION_STEPS") = 1000;
    
    // Module-level convenience functions
    m.def("dict_to_config", &config::from_dict, py::arg("dict"),
          "Convert dictionary to configuration object");
    
    m.def("config_to_dict", &config::to_dict, py::arg("config"),
          "Convert configuration object to dictionary");
    
    // Error translator
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const NLMError& e) {
            py::set_error(PyExc_RuntimeError, e.what());
        } catch (const std::exception& e) {
            py::set_error(PyExc_RuntimeError, e.what());
        }
    });
}

} // namespace nlm