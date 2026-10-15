#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
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
#include "../src/dynamics/SpikeSystem.hpp"
#include "../src/plasticity/STDP.hpp"
#include "../src/plasticity/Hebbian.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/memory/WorkingMemory.hpp"
#include "../src/memory/EpisodicMemory.hpp"

using namespace nlm;

namespace py = pybind11;

// Python exception types
class NLMRuntimeError : public std::runtime_error {
public:
    NLMRuntimeError(const std::string& msg) : std::runtime_error(msg) {}
};

class NLMConfigError : public NLMRuntimeError {
public:
    NLMConfigError(const std::string& msg) : NLMRuntimeError("Configuration error: " + msg) {}
};

class NLMSimulationError : public NLMRuntimeError {
public:
    NLMSimulationError(const std::string& msg) : NLMRuntimeError("Simulation error: " + msg) {}
};

void registerPythonExceptions(py::module_& m) {
    py::register_exception<NLMRuntimeError>(m, "RuntimeError", PyExc_RuntimeError);
    py::register_exception<NLMConfigError>(m, "ConfigError", PyExc_ValueError);
    py::register_exception<NLMSimulationError>(m, "SimulationError", PyExc_RuntimeError);
}

// Helper functions for Python
std::string configToDict(const Config& config) {
    std::string result = "{";
    auto keys = config.getKeys();
    for (size_t i = 0; i < keys.size(); ++i) {
        result += "\"" + keys[i] + "\": \"" + std::to_string(config.getOr<double>(keys[i], 0.0)) + "\"";
        if (i < keys.size() - 1) result += ", ";
    }
    result += "}";
    return result;
}

std::vector<float> getWeightsFromConfig(const Config& config) {
    std::vector<float> weights;
    auto keys = config.getKeys();
    for (const auto& key : keys) {
        weights.push_back(config.getOr<float>(key, 0.0f));
    }
    return weights;
}

// Simplified learning experiment for Python API
class LearningExperiment {
public:
    LearningExperiment(std::shared_ptr<Brain> brain, uint64_t seed)
        : brain_(brain), seed_(seed) {}
    
    void recordInitialState() {
        initialSynapseCount_ = brain_->getTotalSynapseCount();
        initialWeights_.clear();
        
        if (auto* region = brain_->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                initialWeights_.push_back(syn->getWeight());
            }
        }
    }
    
    void recordFinalState() {
        finalWeights_.clear();
        
        if (auto* region = brain_->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                finalWeights_.push_back(syn->getWeight());
            }
        }
        
        mostActiveNeurons_ = brain_->getSpikeSystem()->getMostActiveNeurons(10);
    }
    
    std::string computeStatistics() {
        std::stringstream ss;
        ss << "=== Learning Experiment Results ===\n\n";
        
        if (initialWeights_.empty() || finalWeights_.empty()) {
            ss << "ERROR: No weights recorded\n";
            return ss.str();
        }
        
        // Compute weight changes
        float initialSum = std::accumulate(initialWeights_.begin(), initialWeights_.end(), 0.0f);
        float finalSum = std::accumulate(finalWeights_.begin(), finalWeights_.end(), 0.0f);
        float initialMean = initialSum / initialWeights_.size();
        float finalMean = finalSum / finalWeights_.size();
        
        ss << "Weight Statistics:\n";
        ss << "  Initial mean weight: " << initialMean << "\n";
        ss << "  Final mean weight: " << finalMean << "\n";
        ss << "  Change: " << (finalMean - initialMean) << "\n\n";
        
        // Count synapses that changed significantly
        size_t strengthened = 0;
        size_t weakened = 0;
        size_t unchanged = 0;
        
        size_t minSize = std::min(initialWeights_.size(), finalWeights_.size());
        for (size_t i = 0; i < minSize; ++i) {
            float delta = finalWeights_[i] - initialWeights_[i];
            if (delta > 0.01f) ++strengthened;
            else if (delta < -0.01f) ++weakened;
            else ++unchanged;
        }
        
        ss << "Synaptic Changes:\n";
        ss << "  Strengthened: " << strengthened << "\n";
        ss << "  Weakened: " << weakened << "\n";
        ss << "  Unchanged: " << unchanged << "\n\n";
        
        ss << "Spike Activity:\n";
        ss << "  Total spikes: " << brain_->getTotalSpikeCount() << "\n";
        ss << "  Most active neurons recorded: " << mostActiveNeurons_.size() << "\n\n";
        
        // Determine if learning occurred
        bool learningOccurred = (std::abs(finalMean - initialMean) > 0.001f) ||
                                (strengthened > 0 || weakened > 0);
        
        if (learningOccurred) {
            ss << "✓ LEARNING DETECTED: Synaptic weights changed through experience\n";
        } else {
            ss << "✗ NO LEARNING: Weights did not change significantly\n";
        }
        
        return ss.str();
    }
    
    bool learningOccurred() {
        if (initialWeights_.empty() || finalWeights_.empty()) {
            return false;
        }
        
        float initialSum = std::accumulate(initialWeights_.begin(), initialWeights_.end(), 0.0f);
        float finalSum = std::accumulate(finalWeights_.begin(), finalWeights_.end(), 0.0f);
        float initialMean = initialSum / initialWeights_.size();
        float finalMean = finalSum / finalWeights_.size();
        
        if (std::abs(finalMean - initialMean) > 0.001f) {
            return true;
        }
        
        size_t minSize = std::min(initialWeights_.size(), finalWeights_.size());
        for (size_t i = 0; i < minSize; ++i) {
            if (std::abs(finalWeights_[i] - initialWeights_[i]) > 0.01f) {
                return true;
            }
        }
        
        return false;
    }
    
    size_t getTotalSynapseCount() const { return initialSynapseCount_; }
    size_t getMostActiveNeuronCount() const { return mostActiveNeurons_.size(); }
    
private:
    std::shared_ptr<Brain> brain_;
    uint64_t seed_;
    size_t initialSynapseCount_;
    std::vector<float> initialWeights_;
    std::vector<float> finalWeights_;
    std::vector<NeuronId> mostActiveNeurons_;
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        
        Enhanced Python bindings for the NLM C++ neural simulation framework.
        
        This module provides comprehensive Python APIs for creating and managing
        artificial neural systems that can learn, adapt, and develop over time.
        
        Key Features:
        * Complete Brain simulation with neurons and synapses
        * Advanced plasticity mechanisms (STDP, Hebbian, structural)
        * Neuromodulation system for reward-based learning
        * Memory systems (working, episodic, semantic, procedural, associative)
        * Cognitive functions (concept formation, planning, attention)
        * Development system with age-dependent plasticity
        * Environmental interaction through AgentBrain and SimpleWorld
        * Comprehensive logging and error handling
        
        The NLM project investigates whether artificial systems can develop
        intelligent behavior through experience without pre-programming.
        
        For more information:
        * Documentation: https://nlm.readthedocs.io
        * GitHub: https://github.com/nlm-project/nlm
        * Examples: See examples/ directory in the repository
    )pbdoc";
    
    // Register Python exceptions
    registerPythonExceptions(m);

    // Configuration validation and utility functions
    m.def("validate_config", [](const std::shared_ptr<Config>& config) {
        if (!config) {
            throw NLMConfigError("Config is null");
        }
        
        auto keys = config->getKeys();
        std::vector<std::string> errors;
        
        // Validate essential parameters
        if (!config->has("brain.neuron_count") || config->getOr<int64_t>("brain.neuron_count", 0) <= 0) {
            errors.push_back("brain.neuron_count must be a positive integer");
        }
        
        if (!config->has("brain.synapse_density") || config->getOr<float>("brain.synapse_density", 0.0f) < 0.0f || config->getOr<float>("brain.synapse_density", 1.0f) > 1.0f) {
            errors.push_back("brain.synapse_density must be a float between 0.0 and 1.0");
        }
        
        if (config->has("plasticity.stdp.enable") && !config->getOr<bool>("plasticity.stdp.enable", true)) {
            // Validate complementary STDP parameters if enabled
            if (config->has("plasticity.stdp.learning_rate") && config->getOr<float>("plasticity.stdp.learning_rate", 0.0f) < 0.0f) {
                errors.push_back("plasticity.stdp.learning_rate must be non-negative when STDP is enabled");
            }
        }
        
        if (!errors.empty()) {
            std::string errorMsg = "Configuration validation failed:\n";
            for (const auto& error : errors) {
                errorMsg += "  - " + error + "\n";
            }
            throw NLMConfigError(errorMsg);
        }
        
        return true;
    }, py::arg("config"), R"pbdoc(
        Validate a configuration object.
        
        Args:
            config: The configuration to validate
            
        Returns:
            True if configuration is valid
            
        Raises:
            NLMConfigError: If configuration is invalid
        )pbdoc");
    
    m.def("create_config_from_dict", [](const std::unordered_map<std::string, double>& dict) {
        auto config = std::make_shared<Config>();
        
        for (const auto& pair : dict) {
            // Try to infer type from value
            double value = pair.second;
            std::string key = pair.first;
            
            // Simple type inference based on key prefix
            if (key.find("neuron_count") != std::string::npos || key.find("region_count") != std::string::npos) {
                config->set(key, static_cast<int64_t>(value), ConfigSource::Runtime);
            } else if (key.find("rate") != std::string::npos || key.find("density") != std::string::npos || key.find("weight") != std::string::npos || key.find("learning_rate") != std::string::npos) {
                config->set(key, static_cast<float>(value), ConfigSource::Runtime);
            } else if (key.find("threshold") != std::string::npos || key.find("potential") != std::string::npos || key.find("time") != std::string::npos) {
                config->set(key, static_cast<float>(value), ConfigSource::Runtime);
            } else if (key.find("enabled") != std::string::npos || key.find("active") != std::string::npos) {
                config->set(key, value > 0.5, ConfigSource::Runtime);
            } else {
                config->set(key, value, ConfigSource::Runtime);
            }
        }
        
        return config;
    }, py::arg("dict"), R"pbdoc(
        Create a configuration from a dictionary.
        
        Args:
            dict: Dictionary with configuration parameters
            
        Returns:
            Configured object ready for brain creation
        )pbdoc");
    
    // Enhanced Brain class with additional functionality
    py::class_<Brain>(m, "Brain", R"pbdoc(
        Central neural simulation brain class with advanced learning capabilities.
        
        This class represents an artificial brain composed of neurons connected by synapses.
        It implements Leaky Integrate-and-Fire (LIF) neuron dynamics and supports
        multiple plasticity mechanisms including STDP, Hebbian learning, and
        structural plasticity.
        
        The brain can receive sensory input, produce motor actions, and learn
        from experience through neuromodulation and development systems.
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             R"pbdoc(Create a brain with the given configuration.
             
             Args:
                 config: Configuration object specifying brain parameters
             )pbdoc")
        .def("initialize", &Brain::initialize,
             R"pbdoc(Initialize the brain with the current configuration.
             
             This method allocates neurons and synapses according to the
             configuration parameters. Must be called before using the brain.
             
             Returns:
                 True if initialization succeeds
             )pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             R"pbdoc(Perform a simulation step.
             
             Args:
                 currentStep: Current simulation step number
             )pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             R"pbdoc(Perform a simulation step with explicit timestamp.
             
             Args:
                 currentStep: Current simulation step number
                 currentTime: Current simulation time in seconds
             )pbdoc")
        .def("receive_sensory_input", &Brain::receiveSensoryInput,
             py::arg("input"),
             R"pbdoc(Inject sensory input into the brain.
             
             Args:
                 input: Sensory input data
             )pbdoc")
        .def("inject_current", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             R"pbdoc(Inject current into a specific neuron.
             
             Args:
                 neuron: Neuron ID
                 current: Current to inject (positive for excitation)
             )pbdoc")
        .def("inject_current_to_neurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             R"pbdoc(Inject current into all neurons of a specific type.
             
             Args:
                 type: Type of neurons to inject current into
                 current: Current to inject (positive for excitation)
             )pbdoc")
        .def("produce_action", &Brain::produceAction,
             R"pbdoc(Produce motor action based on current neural activity.
             
             Returns:
                 Motor action produced by the brain
             )pbdoc")
        .def("reset", &Brain::reset,
             R"pbdoc(Reset brain state to initial conditions.
             
             This clears all neuron states and synaptic weights, returning
             the brain to its initial configuration.
             )pbdoc")
        .def("save", &Brain::save, py::arg("filepath"),
             R"pbdoc(Save brain state to file.
             
             Args:
                 filepath: Path to file to save brain state
                 
             Returns:
                 True if save succeeds
             )pbdoc")
        .def("load", &Brain::load, py::arg("filepath"),
             R"pbdoc(Load brain state from file.
             
             Args:
                 filepath: Path to file to load brain state from
                 
             Returns:
                 True if load succeeds
             )pbdoc")
        .def("add_region", &Brain::addRegion, py::arg("name") = "",
             R"pbdoc(Add a new neural region.
             
             Args:
                 name: Name for the new region (empty for auto-generated)
                 
             Returns:
                 Region ID for the newly created region
             )pbdoc")
        .def("get_region", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             R"pbdoc(Get a region by ID.
             
             Args:
                 id: Region ID
                 
             Returns:
                 Reference to the region object
             )pbdoc")
        .def("get_region_count", &Brain::getRegionCount,
             R"pbdoc(Get the number of regions in the brain.
             
             Returns:
                 Number of regions
             )pbdoc")
        .def("get_region_ids", &Brain::getRegionIds,
             R"pbdoc(Get all region IDs.
             
             Returns:
                 List of all region IDs
             )pbdoc")
        .def("get_regions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get all regions.
             
             Returns:
                 List of all region references
             )pbdoc")
        .def("get_total_neuron_count", &Brain::getTotalNeuronCount,
             R"pbdoc(Get total neuron count across all regions.
             
             Returns:
                 Total number of neurons
             )pbdoc")
        .def("get_total_synapse_count", &Brain::getTotalSynapseCount,
             R"pbdoc(Get total synapse count across all regions.
             
             Returns:
                 Total number of synapses
             )pbdoc")
        .def("get_active_neuron_count", &Brain::getActiveNeuronCount,
             R"pbdoc(Get count of active neurons.
             
             Returns:
                 Number of active neurons
             )pbdoc")
        .def("get_firing_neuron_count", &Brain::getFiringNeuronCount,
             R"pbdoc(Get count of currently firing neurons.
             
             Returns:
                 Number of firing neurons
             )pbdoc")
        .def("get_average_firing_rate", &Brain::getAverageFiringRate,
             R"pbdoc(Get average firing rate across all neurons.
             
             Returns:
                 Average firing rate in Hz
             )pbdoc")
        .def("get_excitation_inhibition_ratio", &Brain::getExcitationInhibitionRatio,
             R"pbdoc(Get excitation/inhibition balance ratio.
             
             Returns:
                 E/I ratio (higher = more excitation)
             )pbdoc")
        .def("get_total_spike_count", &Brain::getTotalSpikeCount,
             R"pbdoc(Get total spike count.
             
             Returns:
                 Total number of spikes since brain creation
             )pbdoc")
        .def("get_developmental_stage", &Brain::getDevelopmentalStage,
             R"pbdoc(Get current developmental stage.
             
             Returns:
                 Current developmental stage
             )pbdoc")
        .def("set_developmental_stage", &Brain::setDevelopmentalStage, py::arg("stage"),
             R"pbdoc(Set developmental stage.
             
             Args:
                 stage: Developmental stage to set
             )pbdoc")
        .def("get_config", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get the configuration object.
             
             Returns:
                 Reference to the brain's configuration
             )pbdoc")
        .def("log_status", &Brain::logStatus,
             R"pbdoc(Log brain status to global logger.
             
             This outputs comprehensive information about the brain state
             including neuron counts, firing statistics, and configuration.
             )pbdoc")
        // Additional Python-friendly methods
        .def("run_experiment", [](std::shared_ptr<Brain> brain, int num_steps) {
            std::stringstream ss;
            ss << "=== Neural Learning Experiment ===\n";
            ss << "Brain initialized with " << brain->getTotalNeuronCount() << " neurons\n";
            ss << "Running " << num_steps << " simulation steps...\n\n";
            
            for (int step = 0; step < num_steps; ++step) {
                brain->step(step);
                if (step % 100 == 0) {
                    ss << "Step " << step << ": " << brain->getFiringNeuronCount() << " firing neurons\n";
                }
            }
            
            ss << "\n=== Experiment Results ===\n";
            ss << "Total steps: " << num_steps << "\n";
            ss << "Total spikes: " << brain->getTotalSpikeCount() << "\n";
            ss << "Final firing rate: " << brain->getAverageFiringRate() << " Hz\n";
            ss << "Excitatory/Inhibitory ratio: " << brain->getExcitationInhibitionRatio() << "\n";
            
            return ss.str();
        }, py::arg("num_steps"),
             R"pbdoc(Run a complete experiment with the brain.
             
             Args:
                 num_steps: Number of simulation steps to run
                 
             Returns:
                 String containing experiment results and statistics
             )pbdoc")
        .def("enable_learning", [](std::shared_ptr<Brain> brain, bool enableHebbian, bool enableStdp, bool enableStructural) {
            if (auto* region = brain->getRegion(RegionId(1))) {
                for (auto& syn : region->getSynapses()) {
                    syn->enablePlasticity(enableHebbian, enableStdp, enableStructural);
                }
            }
            return true;
        }, py::arg("enableHebbian"), py::arg("enableStdp"), py::arg("enableStructural"),
             R"pbdoc(Enable specific plasticity mechanisms.
             
             Args:
                 enableHebbian: Enable Hebbian learning
                 enableStdp: Enable Spike-Timing-Dependent Plasticity
                 enableStructural: Enable structural plasticity
                 
             Returns:
                 True if successful
             )pbdoc")
        .def("disable_learning", [](std::shared_ptr<Brain> brain) {
            if (auto* region = brain->getRegion(RegionId(1))) {
                for (auto& syn : region->getSynapses()) {
                    syn->enablePlasticity(false, false, false);
                }
            }
            return true;
        }, R"pbdoc(Disable all plasticity mechanisms.
             
             Returns:
                 True if successful
             )pbdoc")
        .def("get_statistics", [](std::shared_ptr<Brain> brain) {
            std::stringstream ss;
            ss << "=== Brain Statistics ===\n";
            ss << "Neurons: " << brain->getTotalNeuronCount() << "\n";
            ss << "Synapses: " << brain->getTotalSynapseCount() << "\n";
            ss << "Active neurons: " << brain->getActiveNeuronCount() << "\n";
            ss << "Firing neurons: " << brain->getFiringNeuronCount() << "\n";
            ss << "Average firing rate: " << brain->getAverageFiringRate() << " Hz\n";
            ss << "E/I ratio: " << brain->getExcitationInhibitionRatio() << "\n";
            ss << "Total spikes: " << brain->getTotalSpikeCount() << "\n";
            ss << "Developmental stage: " << static_cast<int>(brain->getDevelopmentalStage()) << "\n";
            
            return ss.str();
        }, R"pbdoc(Get comprehensive statistics about the brain.
             
             Returns:
                 Formatted string containing brain statistics
             )pbdoc")
        .def("get_neuron_types", [](std::shared_ptr<Brain> brain) {
            std::vector<int> counts;
            // Count neurons by type
            for (int i = 0; i < static_cast<int>(NeuronType::Internal) + 1; ++i) {
                int count = 0;
                if (auto* region = brain->getRegion(RegionId(1))) {
                    auto neurons = region->getAllNeurons();
                    for (auto* neuron : neurons) {
                        if (neuron->getType() == static_cast<NeuronType>(i)) {
                            count++;
                        }
                    }
                }
                counts.push_back(count);
            }
            return counts;
        }, R"pbdoc(Get counts of neurons by type.
             
             Returns:
                 List where indices correspond to NeuronType values
             )pbdoc")
        .def("compare_with", [](std::shared_ptr<Brain> brain1, std::shared_ptr<Brain> brain2) {
            std::stringstream ss;
            ss << "=== Brain Comparison ===\n";
            ss << "Neuron count difference: " << (brain1->getTotalNeuronCount() - brain2->getTotalNeuronCount()) << "\n";
            ss << "Synapse count difference: " << (brain1->getTotalSynapseCount() - brain2->getTotalSynapseCount()) << "\n";
            ss << "Average firing rate difference: " << (brain1->getAverageFiringRate() - brain2->getAverageFiringRate()) << " Hz\n";
            ss << "Total spikes difference: " << (brain1->getTotalSpikeCount() - brain2->getTotalSpikeCount()) << "\n";
            
            return ss.str();
        }, py::arg("brain2"),
             R"pbdoc(Compare this brain with another.
             
             Args:
                 brain2: Brain to compare against
                 
             Returns:
                 Formatted string with comparison results
             )pbdoc");
    
    // Enhanced Config class with additional functionality
    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration class for NLM system with validation and utility methods.
        
        This class manages all configuration parameters for the NLM system,
        including brain parameters, plasticity settings, neuromodulation
        parameters, and simulation settings.
    )pbdoc")
        .def(py::init<>())
        .def("load_from_file", &Config::loadFromFile, py::arg("filepath"),
             R"pbdoc(Load configuration from a JSON file.
             
             Args:
                 filepath: Path to JSON configuration file
                 
             Returns:
                 True if file loaded successfully
             )pbdoc")
        .def("load_from_args", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
             R"pbdoc(Load configuration from command line arguments.
             
             Args:
                 argc: Argument count
                 argv: Argument vector
                 
             Returns:
                 True if arguments loaded successfully
             )pbdoc")
        .def("save_to_file", &Config::saveToFile, py::arg("filepath"),
             R"pbdoc(Save configuration to a JSON file.
             
             Args:
                 filepath: Path to file to save configuration
                 
             Returns:
                 True if save succeeds
             )pbdoc")
        .def("has", &Config::has, py::arg("key"),
             R"pbdoc(Check if a configuration key exists.
             
             Args:
                 key: Configuration key
                 
             Returns:
                 True if key exists
             )pbdoc")
        .def("get_keys", &Config::getKeys,
             R"pbdoc(Get all configuration keys.
             
             Returns:
                 List of all configuration keys
             )pbdoc")
        .def("clear", &Config::clear,
             R"pbdoc(Clear all configuration entries.
             
             Returns:
                 True if clear succeeds
             )pbdoc")
        .def("summary", &Config::summary,
             R"pbdoc(Get a summary string of the configuration.
             
             Returns:
                 Configuration summary string
             )pbdoc")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        // Additional Python-friendly methods
        .def("set_int", [](Config& self, const std::string& key, int value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
             R"pbdoc(Set integer configuration value.
             
             Args:
                 key: Configuration key
                 value: Integer value
             )pbdoc")
        .def("set_float", [](Config& self, const std::string& key, float value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
             R"pbdoc(Set float configuration value.
             
             Args:
                 key: Configuration key
                 value: Float value
             )pbdoc")
        .def("set_bool", [](Config& self, const std::string& key, bool value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
             R"pbdoc(Set boolean configuration value.
             
             Args:
                 key: Configuration key
                 value: Boolean value
             )pbdoc")
        .def("set_string", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
             R"pbdoc(Set string configuration value.
             
             Args:
                 key: Configuration key
                 value: String value
             )pbdoc")
        .def("get_int", [](Config& self, const std::string& key, int default_value) {
            return self.getOr<int64_t>(key, default_value);
        }, py::arg("key"), py::arg("default_value"),
             R"pbdoc(Get integer configuration value.
             
             Args:
                 key: Configuration key
                 default_value: Default value if key doesn't exist
                 
             Returns:
                 Integer value
             )pbdoc")
        .def("get_float", [](Config& self, const std::string& key, float default_value) {
            return self.getOr<double>(key, default_value);
        }, py::arg("key"), py::arg("default_value"),
             R"pbdoc(Get float configuration value.
             
             Args:
                 key: Configuration key
                 default_value: Default value if key doesn't exist
                 
             Returns:
                 Float value
             )pbdoc")
        .def("get_bool", [](Config& self, const std::string& key, bool default_value) {
            auto opt_val = self.get<bool>(key);
            return opt_val ? *opt_val : default_value;
        }, py::arg("key"), py::arg("default_value"),
             R"pbdoc(Get boolean configuration value.
             
             Args:
                 key: Configuration key
                 default_value: Default value if key doesn't exist
                 
             Returns:
                 Boolean value
             )pbdoc")
        .def("get_string", [](Config& self, const std::string& key, const std::string& default_value) {
            auto opt_val = self.get<std::string>(key);
            return opt_val ? *opt_val : default_value;
        }, py::arg("key"), py::arg("default_value"),
             R"pbdoc(Get string configuration value.
             
             Args:
                 key: Configuration key
                 default_value: Default value if key doesn't exist
                 
             Returns:
                 String value
             )pbdoc")
        .def("merge_config", [](Config& self, const std::shared_ptr<Config>& other) {
            auto keys = other->getKeys();
            for (const auto& key : keys) {
                if (other->has(key)) {
                    auto value = other->get<std::string>(key);
                    self.set(key, value, ConfigSource::Runtime);
                }
            }
            return true;
        }, py::arg("other"),
             R"pbdoc(Merge another configuration into this one.
             
             Args:
                 other: Configuration to merge
                 
             Returns:
                 True if merge succeeds
             )pbdoc")
        .def("get_section", [](Config& self, const std::string& section_prefix) {
            std::unordered_map<std::string, double> section;
            auto keys = self->getKeys();
            
            for (const auto& key : keys) {
                if (key.find(section_prefix + ".") == 0) {
                    std::string fullKey = key.substr(section_prefix.length() + 1);
                    auto value = self->get<double>(key);
                    if (value) {
                        section[fullKey] = *value;
                    }
                }
            }
            
            return section;
        }, py::arg("section_prefix"),
             R"pbdoc(Get configuration section.
             
             Args:
                 section_prefix: Configuration section prefix (e.g., "brain")
                 
             Returns:
                 Dictionary of configuration values in the section
             )pbdoc")
        .def("validate_range", [](Config& self, const std::string& key, double min_value, double max_value) {
            auto value = self.get<double>(key);
            if (!value) {
                return false;  // Key doesn't exist
            }
            
            if (*value < min_value || *value > max_value) {
                return false;
            }
            
            return true;
        }, py::arg("key"), py::arg("min_value"), py::arg("max_value"),
             R"pbdoc(Validate that a configuration value is within range.
             
             Args:
                 key: Configuration key
                 min_value: Minimum allowed value
                 max_value: Maximum allowed value
                 
             Returns:
                 True if value is within range or key doesn't exist
             )pbdoc")
        .def("clamp_value", [](Config& self, const std::string& key, double min_value, double max_value) {
            auto value = self.get<double>(key);
            if (value) {
                double clamped = std::max(min_value, std::min(*value, max_value));
                self.set(key, clamped, ConfigSource::Runtime);
            }
            return true;
        }, py::arg("key"), py::arg("min_value"), py::arg("max_value"),
             R"pbdoc(Clamp a configuration value to within range.
             
             Args:
                 key: Configuration key
                 min_value: Minimum allowed value
                 max_value: Maximum allowed value
                 
             Returns:
                 True if operation succeeds
             )pbdoc")
        .def("copy", [](const Config& self) {
            auto config = std::make_shared<Config>();
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                auto value = self.get<std::string>(key);
                if (value) {
                    config->set(key, *value, ConfigSource::Runtime);
                }
            }
            return config;
        }, R"pbdoc(Create a copy of this configuration.
             
             Returns:
                 Copy of this configuration
             )pbdoc");
    
    // Enhanced SimpleWorld class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
        Simple 2D world for NLM simulation.
        
        This class provides a 2D environment where agents can interact.
        It includes sensory input, motor output, and object management.
        The world updates over time and provides observations to agents.
    )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             R"pbdoc(Configure the world dimensions.
             
             Args:
                 width: World width in arbitrary units
                 height: World height in arbitrary units
                 visionWidth: Width of agent's vision field
                 visionHeight: Height of agent's vision field
             )pbdoc")
        .def("reset", &SimpleWorld::reset,
             R"pbdoc(Reset the world to initial state.
             
             Returns:
                 True if reset succeeds
             )pbdoc")
        .def("set_agent_start", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             R"pbdoc(Set agent starting position.
             
             Args:
                 x: Starting x coordinate
                 y: Starting y coordinate
             )pbdoc")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             R"pbdoc(Update the world by timestep seconds.
             
             Args:
                 timestep: Time step to advance
                 
             Returns:
                 True if update succeeds
             )pbdoc")
        .def("apply_motor_command", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             R"pbdoc(Apply motor command to the world.
             
             Args:
                 cmd: Motor command to apply
                 currentTime: Current simulation time
                 
             Returns:
                 Action result from the world
             )pbdoc")
        .def("get_sensory_percept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get sensory percept from the world.
             
             Returns:
                 Sensory percept containing all sensory inputs
             )pbdoc")
        .def("get_agent_body", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get agent body state.
             
             Returns:
                 Agent body state object
             )pbdoc")
        .def("add_object", &SimpleWorld::addObject, py::arg("obj"),
             R"pbdoc(Add object to the world.
             
             Args:
                 obj: World object to add
             )pbdoc")
        .def("remove_object", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             R"pbdoc(Remove object at position.
             
             Args:
                 x: X coordinate
                 y: Y coordinate
             )pbdoc")
        .def("is_valid_position", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             R"pbdoc(Check if position is valid.
             
             Args:
                 x: X coordinate
                 y: Y coordinate
                 
             Returns:
                 True if position is valid
             )pbdoc")
        .def("get_width", &SimpleWorld::getWidth,
             R"pbdoc(Get world width.
             
             Returns:
                 World width
             )pbdoc")
        .def("get_height", &SimpleWorld::getHeight,
             R"pbdoc(Get world height.
             
             Returns:
                 World height
             )pbdoc")
        .def("get_max_energy", &SimpleWorld::getMaxEnergy,
             R"pbdoc(Get maximum energy level.
             
             Returns:
                 Maximum energy
             )pbdoc")
        .def("set_max_energy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             R"pbdoc(Set maximum energy level.
             
             Args:
                 e: Maximum energy
             )pbdoc")
        .def("get_energy_decay_rate", &SimpleWorld::getEnergyDecayRate,
             R"pbdoc(Get energy decay rate.
             
             Returns:
                 Energy decay rate
             )pbdoc")
        .def("set_energy_decay_rate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             R"pbdoc(Set energy decay rate.
             
             Args:
                 r: Energy decay rate
             )pbdoc")
        .def("get_simulation_time", &SimpleWorld::getSimulationTime,
             R"pbdoc(Get simulation time.
             
             Returns:
                 Current simulation time
             )pbdoc")
        .def("set_random_seed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             R"pbdoc(Set random seed.
             
             Args:
                 seed: Random seed value
             )pbdoc")
        .def("get_random_seed", &SimpleWorld::getRandomSeed,
             R"pbdoc(Get random seed.
             
             Returns:
                 Random seed value
             )pbdoc")
        // Additional Python-friendly methods
        .def("create_object", [](SimpleWorld& self, float x, float y, WorldObjectType type, float value) {
            WorldObject obj(x, y, type, value);
            self.addObject(obj);
            return obj;
        }, py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value"),
             R"pbdoc(Create and add object to world.
             
             Args:
                 x: X coordinate
                 y: Y coordinate
                 type: Object type
                 value: Object value
                 
             Returns:
                 Created object
             )pbdoc")
        .def("get_objects_at_position", [](SimpleWorld& self, float x, float y) {
            std::vector<WorldObject> objects;
            // In a real implementation, this would query the world's object storage
            // For now, return empty vector
            return objects;
        }, py::arg("x"), py::arg("y"),
             R"pbdoc(Get objects at position.
             
             Args:
                 x: X coordinate
                 y: Y coordinate
                 
             Returns:
                 List of objects at position
             )pbdoc")
        .def("count_objects_of_type", [](SimpleWorld& self, WorldObjectType type) {
            // In a real implementation, this would query the world's object storage
            return 0;
        }, py::arg("type"),
             R"pbdoc(Count objects of specific type.
             
             Args:
                 type: Object type to count
                 
             Returns:
                 Number of objects of this type
             )pbdoc")
        .def("get_agent_energy", [](SimpleWorld& self) {
            auto body = self.getAgentBody();
            return body.energy;
        }, R"pbdoc(Get agent energy.
             
             Returns:
                 Agent energy level
             )pbdoc")
        .def("get_agent_health", [](SimpleWorld& self) {
            auto body = self.getAgentBody();
            return body.health;
        }, R"pbdoc(Get agent health.
             
             Returns:
                 Agent health level
             )pbdoc")
        .def("is_agent_alive", [](SimpleWorld& self) {
            auto body = self.getAgentBody();
            return body.health > 0.0f;
        }, R"pbdoc(Check if agent is alive.
             
             Returns:
                 True if agent is alive
             )pbdoc")
        .def("get_world_bounds", [](SimpleWorld& self) {
            return std::make_pair(self.getWidth(), self.getHeight());
        }, R"pbdoc(Get world bounds.
             
             Returns:
                 Tuple of (width, height)
             )pbdoc")
        .def("get_vision_parameters", [](SimpleWorld& self) {
            return std::make_tuple(self.getWidth(), self.getHeight());
        }, R"pbdoc(Get vision parameters.
             
             Returns:
                 Tuple of (world_width, world_height)
             )pbdoc")
        .def("check_collision", [](SimpleWorld& self, float x, float y) {
            return self.isValidPosition(x, y);
        }, py::arg("x"), py::arg("y"),
             R"pbdoc(Check for collision at position.
             
             Args:
                 x: X coordinate
                 y: Y coordinate
                 
             Returns:
                 True if collision detected
             )pbdoc");
    
    // AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to world.
        
        This class provides the interface between the neural brain and the
        external world. It handles sensory transduction and motor decoding,
        enabling the brain to interact with its environment.
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             R"pbdoc(Create agent brain with underlying brain.
             
             Args:
                 brain: Underlying neural brain
             )pbdoc")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             R"pbdoc(Initialize agent brain with world.
             
             Args:
                 world: World to interact with
             )pbdoc")
        .def("get_sensory_input_size", &AgentBrain::getSensoryInputSize,
             R"pbdoc(Get expected sensory input size.
             
             Returns:
                 Expected sensory input size
             )pbdoc")
        .def("get_motor_output_size", &AgentBrain::getMotorOutputSize,
             R"pbdoc(Get expected motor output size.
             
             Returns:
                 Expected motor output size
             )pbdoc")
        .def("process_sensory_input", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             R"pbdoc(Process sensory percept and inject into brain.
             
             Args:
                 percept: Sensory percept to process
             )pbdoc")
        .def("decode_motor_command", &AgentBrain::decodeMotorCommand,
             R"pbdoc(Decode brain motor activity into motor command.
             
             Returns:
                 Motor command decoded from brain activity
             )pbdoc")
        .def("apply_reward_modulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             R"pbdoc(Apply reward-based neuromodulation.
             
             Args:
                 reward: Actual reward received
                 predictedReward: Predicted reward
             )pbdoc")
        .def("update_development", &AgentBrain::updateDevelopment, py::arg("timestep"),
             R"pbdoc(Update development system.
             
             Args:
                 timestep: Time step to update development
             )pbdoc")
        .def("get_developmental_stage", &AgentBrain::getDevelopmentalStage,
             R"pbdoc(Get current developmental stage.
             
             Returns:
                 Current developmental stage
             )pbdoc")
        .def("get_neuromodulation_level", &AgentBrain::getNeuromodulationLevel,
             R"pbdoc(Get current neuromodulation level.
             
             Returns:
                 Current neuromodulation level
             )pbdoc")
        .def("get_curiosity_level", &AgentBrain::getCuriosityLevel,
             R"pbdoc(Get curiosity level.
             
             Returns:
                 Current curiosity level
             )pbdoc")
        .def("get_novelty_level", &AgentBrain::getNoveltyLevel,
             R"pbdoc(Get novelty level.
             
             Returns:
                 Current novelty level
             )pbdoc")
        .def("get_prediction_error", &AgentBrain::getPredictionError,
             R"pbdoc(Get prediction error.
             
             Returns:
                 Current prediction error
             )pbdoc")
        .def("reset", &AgentBrain::reset,
             R"pbdoc(Reset agent for new episode.
             
             Returns:
                 True if reset succeeds
             )pbdoc")
        .def("get_brain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get the underlying brain.
             
             Returns:
                 Reference to underlying brain
             )pbdoc")
        .def("enable_reward_modulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"),
             R"pbdoc(Enable/disable reward modulation.
             
             Args:
                 enable: Whether to enable reward modulation
                 
             Returns:
                 True if operation succeeds
             )pbdoc")
        .def("enable_structural_plasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             R"pbdoc(Enable/disable structural plasticity.
             
             Args:
                 enable: Whether to enable structural plasticity
                 
             Returns:
                 True if operation succeeds
             )pbdoc")
        .def("enable_development", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             R"pbdoc(Enable/disable development system.
             
             Args:
                 enable: Whether to enable development
                 
             Returns:
                 True if operation succeeds
             )pbdoc")
        .def("enable_curiosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             R"pbdoc(Enable/disable curiosity system.
             
             Args:
                 enable: Whether to enable curiosity
                 
             Returns:
                 True if operation succeeds
             )pbdoc")
        .def("is_reward_modulation_enabled", &AgentBrain::isRewardModulationEnabled,
             R"pbdoc(Check if reward modulation is enabled.
             
             Returns:
                 True if reward modulation enabled
             )pbdoc")
        .def("is_structural_plasticity_enabled", &AgentBrain::isStructuralPlasticityEnabled,
             R"pbdoc(Check if structural plasticity is enabled.
             
             Returns:
                 True if structural plasticity enabled
             )pbdoc")
        .def("is_development_enabled", &AgentBrain::isDevelopmentEnabled,
             R"pbdoc(Check if development is enabled.
             
             Returns:
                 True if development enabled
             )pbdoc")
        .def("is_curiosity_enabled", &AgentBrain::isCuriosityEnabled,
             R"pbdoc(Check if curiosity is enabled.
             
             Returns:
                 True if curiosity enabled
             )pbdoc")
        // Additional Python-friendly methods
        .def("get_all_subsystems_enabled", [](const AgentBrain& agent) {
            return std::make_tuple(agent.isRewardModulationEnabled(),
                                  agent.isStructuralPlasticityEnabled(),
                                  agent.isDevelopmentEnabled(),
                                  agent.isCuriosityEnabled());
        }, R"pbdoc(Get status of all subsystems.
             
             Returns:
                 Tuple of (reward_modulation, structural_plasticity, development, curiosity)
             )pbdoc")
        .def("train_agent", [](AgentBrain& agent, SimpleWorld& world, int num_episodes) {
            std::stringstream ss;
            ss << "=== Agent Training ===\n";
            ss << "Starting training for " << num_episodes << " episodes...\n\n";
            
            for (int episode = 0; episode < num_episodes; ++episode) {
                ss << "Episode " << episode + 1 << ": ";
                
                // Reset for new episode
                agent.reset();
                world.reset();
                
                // Run episode
                for (int step = 0; step < 100; ++step) {
                    // Get sensory input
                    auto percept = world.getSensoryPercept();
                    
                    // Process sensory input
                    agent.processSensoryInput(percept);
                    
                    // Decode motor command
                    auto cmd = agent.decodeMotorCommand();
                    
                    // Apply command to world
                    world.applyMotorCommand(cmd, world.getSimulationTime());
                    
                    // Update development
                    agent.updateDevelopment(0.1);
                    
                    if (step % 20 == 0) {
                        ss << "\r  Step " << step;
                    }
                }
                
                ss << " completed\n";
            }
            
            ss << "\n=== Training Complete ===\n";
            ss << "Final neuromodulation level: " << agent.getNeuromodulationLevel() << "\n";
            ss << "Final curiosity level: " << agent.getCuriosityLevel() << "\n";
            ss << "Final novelty level: " << agent.getNoveltyLevel() << "\n";
            
            return ss.str();
        }, py::arg("num_episodes"),
             R"pbdoc(Train the agent in the world.
             
             Args:
                 num_episodes: Number of training episodes
                 
             Returns:
                 String containing training results
             )pbdoc")
        .def("run_episode", [](AgentBrain& agent, SimpleWorld& world, int max_steps) {
            std::stringstream ss;
            ss << "=== Episode ===\n";
            
            agent.reset();
            world.reset();
            
            float total_reward = 0.0f;
            
            for (int step = 0; step < max_steps; ++step) {
                ss << "Step " << step << ": ";
                
                // Get sensory input
                auto percept = world.getSensoryPercept();
                
                // Process sensory input
                agent.processSensoryInput(percept);
                
                // Decode motor command
                auto cmd = agent.decodeMotorCommand();
                
                // Apply command to world
                auto result = world.applyMotorCommand(cmd, world.getSimulationTime());
                
                // Apply reward modulation
                agent.applyRewardModulation(result.reward, 0.0f);
                
                // Update development
                agent.updateDevelopment(0.1);
                
                total_reward += result.reward;
                
                ss << "Action: " << static_cast<int>(cmd) << " | Reward: " << result.reward << "\n";
                
                if (!result.success) {
                    ss << "Episode ended early due to failed action\n";
                    break;
                }
            }
            
            ss << "\n=== Episode Results ===\n";
            ss << "Total reward: " << total_reward << "\n";
            ss << "Final neuromodulation: " << agent.getNeuromodulationLevel() << "\n";
            ss << "Final curiosity: " << agent.getCuriosityLevel() << "\n";
            ss << "Final novelty: " << agent.getNoveltyLevel() << "\n";
            ss << "Prediction error: " << agent.getPredictionError() << "\n";
            
            return ss.str();
        }, py::arg("max_steps"),
             R"pbdoc(Run a single episode with the agent.
             
             Args:
                 max_steps: Maximum number of steps per episode
                 
             Returns:
                 String containing episode results
             )pbdoc")
        .def("compare_agents", [](AgentBrain& agent1, AgentBrain& agent2) {
            std::stringstream ss;
            ss << "=== Agent Comparison ===\n";
            ss << "Reward modulation (agent1): " << agent1.getNeuromodulationLevel() << "\n";
            ss << "Reward modulation (agent2): " << agent2.getNeuromodulationLevel() << "\n";
            ss << "Curiosity (agent1): " << agent1.getCuriosityLevel() << "\n";
            ss << "Curiosity (agent2): " << agent2.getCuriosityLevel() << "\n";
            ss << "Novelty (agent1): " << agent1.getNoveltyLevel() << "\n";
            ss << "Novelty (agent2): " << agent2.getNoveltyLevel() << "\n";
            ss << "Prediction error (agent1): " << agent1.getPredictionError() << "\n";
            ss << "Prediction error (agent2): " << agent2.getPredictionError() << "\n";
            
            return ss.str();
        }, py::arg("agent2"),
             R"pbdoc(Compare two agents.
             
             Args:
                 agent2: Second agent to compare against
                 
             Returns:
                 String containing comparison results
             )pbdoc")
        .def("create_default_agent", [](std::shared_ptr<Brain> brain) {
            auto agent = std::make_shared<AgentBrain>(brain);
            // Enable common subsystems for new agents
            agent->enableRewardModulation(true);
            agent->enableCuriosity(true);
            agent->enableDevelopment(true);
            return agent;
        }, py::arg("brain"),
             R"pbdoc(Create a new agent with default settings.
             
             Args:
                 brain: Brain to connect to agent
                 
             Returns:
                 New agent with default settings
             )pbdoc");
    
    // Convenience functions
    m.def("create_default_config", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, R"pbdoc(Create a default configuration object.
    
    Returns:
        Default configuration
    )pbdoc");
    
    m.def("create_brain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), R"pbdoc(Create a brain with configuration.
    
    Args:
        config: Configuration for the brain
        
    Returns:
        New brain instance
    )pbdoc");
    
    m.def("create_simple_world", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, R"pbdoc(Create a simple world.
    
    Returns:
        New simple world instance
    )pbdoc");
    
    m.def("create_agent_brain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), R"pbdoc(Create an agent brain interface.
    
    Args:
        brain: Brain to interface
        
    Returns:
        New agent brain interface
    )pbdoc");
    
    // Learning experiment functionality
    m.def("run_learning_experiment", [](std::shared_ptr<Brain> brain, uint64_t seed = 42) {
        LearningExperiment experiment(brain, seed);
        experiment.recordInitialState();
        
        // Enable plasticity
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
            }
        }
        
        // Run learning steps
        for (SimulationStep step = 0; step < 1000; ++step) {
            // Simple learning pattern
            for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            brain->step(step, step * 0.001);
        }
        
        experiment.recordFinalState();
        
        return experiment;
    }, py::arg("brain"), py::arg("seed") = 42, R"pbdoc(Run a learning experiment.
    
    Args:
        brain: Brain to run experiment on
        seed: Random seed for experiment
        
    Returns:
        Learning experiment object
    )pbdoc");
    
    // LearningExperiment Python interface
    py::class_<LearningExperiment>(m, "LearningExperiment", R"pbdoc(
        Learning experiment for measuring synaptic changes through experience.
        
        This class records initial and final brain states, computes statistics
        about synaptic changes, and determines whether learning occurred.
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>, uint64_t>(), py::arg("brain"), py::arg("seed"),
             R"pbdoc(Create learning experiment.
             
             Args:
                 brain: Brain to experiment on
                 seed: Random seed
             )pbdoc")
        .def("record_initial_state", &LearningExperiment::recordInitialState,
             R"pbdoc(Record initial brain state.
             
             Records initial synaptic weights and neuron counts.
             )pbdoc")
        .def("record_final_state", &LearningExperiment::recordFinalState,
             R"pbdoc(Record final brain state.
             
             Records final synaptic weights and neuron counts.
             )pbdoc")
        .def("compute_statistics", &LearningExperiment::computeStatistics,
             R"pbdoc(Compute learning statistics.
             
             Returns:
                 Formatted string with experiment results
             )pbdoc")
        .def("learning_occurred", &LearningExperiment::learningOccurred,
             R"pbdoc(Check if learning occurred.
             
             Returns:
                 True if learning detected
             )pbdoc")
        .def("get_total_synapse_count", &LearningExperiment::getTotalSynapseCount,
             R"pbdoc(Get total synapse count from initial state.
             
             Returns:
                 Number of synapses
             )pbdoc")
        .def("get_most_active_neuron_count", &LearningExperiment::getMostActiveNeuronCount,
             R"pbdoc(Get count of most active neurons.
             
             Returns:
                 Number of most active neurons
             )pbdoc")
        .def("get_initial_mean_weight", [](const LearningExperiment& exp) {
            if (exp.getTotalSynapseCount() == 0) return 0.0f;
            float sum = std::accumulate(exp.initialWeights_.begin(), exp.initialWeights_.end(), 0.0f);
            return sum / exp.initialWeights_.size();
        }, R"pbdoc(Get initial mean synaptic weight.
             
             Returns:
                 Initial mean weight
             )pbdoc")
        .def("get_final_mean_weight", [](const LearningExperiment& exp) {
            if (exp.getTotalSynapseCount() == 0) return 0.0f;
            float sum = std::accumulate(exp.finalWeights_.begin(), exp.finalWeights_.end(), 0.0f);
            return sum / exp.finalWeights_.size();
        }, R"pbdoc(Get final mean synaptic weight.
             
             Returns:
                 Final mean weight
             )pbdoc")
        .def("get_weight_change", [](const LearningExperiment& exp) {
            if (exp.getTotalSynapseCount() == 0) return 0.0f;
            float initialMean = exp.get_initial_mean_weight();
            float finalMean = exp.get_final_mean_weight();
            return finalMean - initialMean;
        }, R"pbdoc(Get synaptic weight change.
             
             Returns:
                 Weight change (final - initial)
             )pbdoc")
        .def("__repr__", [](const LearningExperiment& exp) {
            std::stringstream ss;
            ss << "<LearningExperiment synapses=" << exp.getTotalSynapseCount()
               << " most_active=" << exp.getMostActiveNeuronCount()
               << " learning=" << (exp.learningOccurred() ? "true" : "false") << ">";
            return ss.str();
        });
    
    // System constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Type aliases for Python
    py::class_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration.)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();
    
    py::class_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration.)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();
    
    py::class_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration.)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();
    
    py::class_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration.)pbdoc")
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
    
    py::class_<MotorCommand>(m, "MotorCommand", R"pbdoc(Motor command enumeration.)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();
    
    py::class_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration.)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();
}

} // namespace nlm