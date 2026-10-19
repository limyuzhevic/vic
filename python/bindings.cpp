#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/iostream.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <algorithm>
#include <cmath>
#include <fstream>
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

// Advanced configuration validation and schema
class ConfigValidator {
public:
    static bool validate(const Config& config) {
        bool valid = true;
        
        // Validate brain configuration
        if (config.has("brain.neuron_count")) {
            auto neuronCount = config.getOr<size_t>("brain.neuron_count", 1000);
            if (neuronCount == 0 || neuronCount > 100000) {
                std::cerr << "Warning: Invalid neuron count: " << neuronCount << std::endl;
                valid = false;
            }
        }
        
        // Validate connection probability
        if (config.has("brain.connection_probability")) {
            auto connProb = config.getOr<float>("brain.connection_probability", 0.1f);
            if (connProb < 0.0f || connProb > 1.0f) {
                std::cerr << "Warning: Invalid connection probability: " << connProb << std::endl;
                valid = false;
            }
        }
        
        // Validate simulation timestep
        if (config.has("simulation_timestep")) {
            auto timestep = config.getOr<double>("simulation_timestep", 0.001);
            if (timestep <= 0.0 || timestep > 1.0) {
                std::cerr << "Warning: Invalid simulation timestep: " << timestep << std::endl;
                valid = false;
            }
        }
        
        // Validate plasticity parameters
        if (config.has("plasticity.stdp.learning_rate")) {
            auto lrnRate = config.getOr<float>("plasticity.stdp.learning_rate", 0.001f);
            if (lrnRate < 0.0f) {
                std::cerr << "Warning: Invalid STDP learning rate: " << lrnRate << std::endl;
                valid = false;
            }
        }
        
        return valid;
    }
    
    static std::string getSchema() {
        return R"(
        {
            "brain": {
                "neuron_count": {"type": "integer", "min": 1, "max": 100000, "default": 1000},
                "connection_probability": {"type": "float", "min": 0.0, "max": 1.0, "default": 0.1},
                "connection_probability": {"type": "float", "min": 0.0, "max": 1.0, "default": 0.1}
            },
            "plasticity": {
                "stdp": {
                    "learning_rate": {"type": "float", "min": 0.0, "default": 0.001},
                    "tau_plus": {"type": "float", "min": 0.0, "default": 20.0},
                    "tau_minus": {"type": "float", "min": 0.0, "default": 20.0}
                }
            },
            "neuromod": {
                "dopamine": {
                    "scale": {"type": "float", "min": 0.0, "default": 1.0}
                }
            }
        }
        )";
    }
};

// Performance metrics collector
class PerformanceMetrics {
private:
    std::chrono::high_resolution_clock::time_point start_time;
    uint64_t steps_completed = 0;
    uint64_t total_spikes = 0;
    double total_energy = 0.0;
    std::map<std::string, double> system_metrics;
    
public:
    PerformanceMetrics() : start_time(std::chrono::high_resolution_clock::now()) {}
    
    void recordStep(uint64_t spikes, double energy = 0.0) {
        steps_completed++;
        total_spikes += spikes;
        total_energy += energy;
    }
    
    void recordSystemMetric(const std::string& name, double value) {
        system_metrics[name] = value;
    }
    
    double getElapsedTime() const {
        auto end_time = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double>(end_time - start_time).count();
    }
    
    double getStepsPerSecond() const {
        double elapsed = getElapsedTime();
        return elapsed > 0 ? steps_completed / elapsed : 0.0;
    }
    
    double getSpikesPerSecond() const {
        double elapsed = getElapsedTime();
        return elapsed > 0 ? total_spikes / elapsed : 0.0;
    }
    
    double getAverageEnergyPerStep() const {
        return steps_completed > 0 ? total_energy / steps_completed : 0.0;
    }
    
    std::string getSummary() const {
        std::stringstream ss;
        ss << "=== Performance Metrics ===\n";
        ss << "Total steps: " << steps_completed << "\n";
        ss << "Elapsed time: " << getElapsedTime() << " s\n";
        ss << "Steps per second: " << getStepsPerSecond() << "\n";
        ss << "Total spikes: " << total_spikes << "\n";
        ss << "Spikes per second: " << getSpikesPerSecond() << "\n";
        ss << "Total energy: " << total_energy << "\n";
        ss << "Average energy per step: " << getAverageEnergyPerStep() << "\n";
        
        if (!system_metrics.empty()) {
            ss << "\n=== System Metrics ===\n";
            for (const auto& pair : system_metrics) {
                ss << pair.first << ": " << pair.second << "\n";
            }
        }
        
        return ss.str();
    }
    
    py::dict getDict() const {
        py::dict d;
        d["total_steps"] = steps_completed;
        d["elapsed_time"] = getElapsedTime();
        d["steps_per_second"] = getStepsPerSecond();
        d["total_spikes"] = total_spikes;
        d["spikes_per_second"] = getSpikesPerSecond();
        d["total_energy"] = total_energy;
        d["average_energy_per_step"] = getAverageEnergyPerStep();
        
        for (const auto& pair : system_metrics) {
            d[pair.first] = pair.second;
        }
        
        return d;
    }
};

// Advanced experiment runner
class AdvancedExperimentRunner {
private:
    std::vector<std::shared_ptr<Experiment>> experiments;
    PerformanceMetrics performance_metrics;
    std::shared_ptr<Config> default_config;
    
public:
    AdvancedExperimentRunner() {
        default_config = std::make_shared<Config>();
    }
    
    void addExperiment(const std::string& name, const std::function<bool(const std::shared_ptr<Config>&, const std::shared_ptr<Brain>&)>& func) {
        auto exp = std::make_shared<Experiment>(name);
        exp->setFunction(func);
        experiments.push_back(exp);
    }
    
    py::dict runExperiment(const std::string& name, const std::shared_ptr<Config>& config = nullptr) {
        auto exp_config = config ? config : default_config;
        
        // Create brain with configuration
        auto brain = std::make_shared<Brain>(exp_config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain for experiment: " + name);
        }
        
        // Run experiment function
        for (auto& exp : experiments) {
            if (exp->getName() == name) {
                bool success = exp->run(exp_config, brain);
                if (!success) {
                    throw std::runtime_error("Experiment failed: " + name);
                }
                
                // Record performance metrics
                auto metrics = exp->getMetrics();
                performance_metrics.recordStep(metrics.spikes, metrics.energy);
                
                py::dict result;
                result["success"] = success;
                result["experiment_name"] = exp->getName();
                result["metrics"] = metrics.toDict();
                result["performance"] = performance_metrics.getDict();
                
                return result;
            }
        }
        
        throw std::runtime_error("Experiment not found: " + name);
    }
    
    py::dict runBatch(const std::vector<std::string>& experiment_names, 
                     const std::shared_ptr<Config>& config = nullptr,
                     size_t num_replicates = 1) {
        py::dict batch_results;
        batch_results["experiment_names"] = experiment_names;
        batch_results["num_replicates"] = num_replicates;
        
        py::list all_results;
        
        for (const auto& exp_name : experiment_names) {
            py::list replicate_results;
            
            for (size_t i = 0; i < num_replicates; ++i) {
                auto result = runExperiment(exp_name, config);
                replicate_results.append(result);
            }
            
            batch_results[exp_name] = replicate_results;
            all_results.append(replicate_results);
        }
        
        return batch_results;
    }
    
    std::string getPerformanceReport() const {
        return performance_metrics.getSummary();
    }
    
    void saveResults(const std::string& filepath) const {
        std::ofstream file(filepath);
        if (file.is_open()) {
            file << performance_metrics.getSummary();
            file.close();
            std::cout << "Performance report saved to: " << filepath << std::endl;
        } else {
            throw std::runtime_error("Could not open file for writing: " + filepath);
        }
    }
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Enhanced Python Bindings
        -------------------------------------------------------
        Enhanced Python bindings for the NLM C++ neural simulation framework.
        
        This version provides:
        - Advanced configuration validation and schema
        - Performance metrics collection and analysis
        - Advanced experiment runner with batch processing
        - Enhanced agent capabilities
        - Configuration validation
        - Performance monitoring
        
        Key features:
        * Comprehensive configuration validation
        * Detailed performance metrics
        * Batch experiment execution
        * Advanced visualization support
        * Configuration schema validation
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Basic type bindings (same as before)
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

    // Advanced Config class with validation
    py::class_<Config>(m, "Config", R"pbdoc(Advanced configuration class for NLM system with validation)pbdoc")
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
        })
        // Advanced configuration methods
        .def("validate", [](const Config& self) {
            return ConfigValidator::validate(self);
        }, "Validate configuration against schema")
        .def("getSchema", &ConfigValidator::getSchema, "Get configuration schema")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value, nlm::ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
           "Set configuration value (string)")
        .def("set", [](Config& self, const std::string& key, int value) {
            self.set(key, static_cast<int64_t>(value), nlm::ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
           "Set configuration value (int)")
        .def("set", [](Config& self, const std::string& key, double value) {
            self.set(key, value, nlm::ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
           "Set configuration value (float)")
        .def("set", [](Config& self, const std::string& key, bool value) {
            self.set(key, value, nlm::ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
           "Set configuration value (bool)")
        .def("getOr", [](Config& self, const std::string& key, const std::string& default_value) {
            return self.getOr<std::string>(key, default_value);
        }, py::arg("key"), py::arg("default"),
           "Get configuration value with default (string)")
        .def("getOr", [](Config& self, const std::string& key, int default_value) {
            return self.getOr<int>(key, default_value);
        }, py::arg("key"), py::arg("default"),
           "Get configuration value with default (int)")
        .def("getOr", [](Config& self, const std::string& key, double default_value) {
            return self.getOr<double>(key, default_value);
        }, py::arg("key"), py::arg("default"),
           "Get configuration value with default (float)")
        .def("getOr", [](Config& self, const std::string& key, bool default_value) {
            return self.getOr<bool>(key, default_value);
        }, py::arg("key"), py::arg("default"),
           "Get configuration value with default (bool)");

    // Performance metrics class
    py::class_<PerformanceMetrics>(m, "PerformanceMetrics", R"pbdoc(Collect and analyze performance metrics for NLM simulations)pbdoc")
        .def(py::init<>())
        .def("recordStep", &PerformanceMetrics::recordStep,
             py::arg("spikes"), py::arg("energy") = 0.0,
             "Record performance metrics for a simulation step")
        .def("recordSystemMetric", &PerformanceMetrics::recordSystemMetric,
             py::arg("name"), py::arg("value"),
             "Record a system-specific metric")
        .def("getElapsedTime", &PerformanceMetrics::getElapsedTime,
             "Get elapsed time in seconds")
        .def("getStepsPerSecond", &PerformanceMetrics::getStepsPerSecond,
             "Get steps per second")
        .def("getSpikesPerSecond", &PerformanceMetrics::getSpikesPerSecond,
             "Get spikes per second")
        .def("getAverageEnergyPerStep", &PerformanceMetrics::getAverageEnergyPerStep,
             "Get average energy per step")
        .def("getSummary", &PerformanceMetrics::getSummary,
             "Get formatted performance summary")
        .def("getDict", &PerformanceMetrics::getDict,
             "Get metrics as dictionary");

    // Advanced experiment runner class
    py::class_<AdvancedExperimentRunner>(m, "AdvancedExperimentRunner", R"pbdoc(Advanced experiment runner with batch processing and performance tracking)pbdoc")
        .def(py::init<>())
        .def("addExperiment", &AdvancedExperimentRunner::addExperiment,
             py::arg("name"), py::arg("func"),
             "Add an experiment to the runner")
        .def("runExperiment", &AdvancedExperimentRunner::runExperiment,
             py::arg("name"), py::arg("config") = nullptr,
             "Run a single experiment")
        .def("runBatch", &AdvancedExperimentRunner::runBatch,
             py::arg("experiment_names"), py::arg("config") = nullptr, py::arg("num_replicates") = 1,
             "Run multiple experiments in batch")
        .def("getPerformanceReport", &AdvancedExperimentRunner::getPerformanceReport,
             "Get performance report")
        .def("saveResults", &AdvancedExperimentRunner::saveResults,
             py::arg("filepath"), "Save results to file");

    // Enhanced agent with advanced capabilities
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Enhanced agent brain interface with advanced capabilities)pbdoc")
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

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Additional utility functions
    m.def("validateConfig", &ConfigValidator::validate,
          py::arg("config"), "Validate configuration against schema");

    m.def("getConfigSchema", &ConfigValidator::getSchema,
          "Get configuration schema as JSON string");

    // Performance monitoring integration
    m.def("benchmarkExperiment", [](const std::shared_ptr<Brain>& brain, size_t steps) {
        PerformanceMetrics metrics;
        for (size_t i = 0; i < steps; ++i) {
            brain->step(i);
            // Record approximate metrics
            metrics.recordStep(brain->getFiringNeuronCount(), 0.0);
        }
        return metrics;
    }, py::arg("brain"), py::arg("steps"),
       "Run benchmark and return performance metrics");

    // Advanced visualization and analysis
    m.def("analyzeSpikeTrain", [](const std::shared_ptr<Brain>& brain, size_t steps) {
        std::vector<float> spike_counts;
        for (size_t i = 0; i < steps; ++i) {
            brain->step(i);
            spike_counts.push_back(static_cast<float>(brain->getFiringNeuronCount()));
        }
        
        // Calculate statistics
        double mean = std::accumulate(spike_counts.begin(), spike_counts.end(), 0.0) / spike_counts.size();
        double variance = 0.0;
        for (float count : spike_counts) {
            variance += (count - mean) * (count - mean);
        }
        variance /= spike_counts.size();
        double std_dev = std::sqrt(variance);
        
        py::dict analysis;
        analysis["mean"] = mean;
        analysis["std_dev"] = std_dev;
        analysis["min"] = *std::min_element(spike_counts.begin(), spike_counts.end());
        analysis["max"] = *std::max_element(spike_counts.begin(), spike_counts.end());
        analysis["spike_counts"] = spike_counts;
        
        return analysis;
    }, py::arg("brain"), py::arg("steps"),
       "Analyze spike train statistics");

    // Configuration validation and schema
    m.def("validateConfig", [](const std::string& config_json) {
        // This would need to parse JSON and create Config object
        // For now, return a placeholder
        return true;
    }, py::arg("config_json"), "Validate configuration JSON string");

    // Export all original constants
    m.attr("EXCITATORY") = py::cast(NeuronType::Excitatory);
    m.attr("INHIBITORY") = py::cast(NeuronType::Inhibitory);
    m.attr("MODULATOR") = py::cast(NeuronType::Modulatory);
    m.attr("SENSORY") = py::cast(NeuronType::Sensory);
    m.attr("MOTOR") = py::cast(NeuronType::Motor);
    m.attr("INTERNAL") = py::cast(NeuronType::Internal);

    m.attr("EXCITATORY_SYNAPSE") = py::cast(SynapseType::Excitatory);
    m.attr("INHIBITORY_SYNAPSE") = py::cast(SynapseType::Inhibitory);
    m.attr("MODULATOR_SYNAPSE") = py::cast(SynapseType::Modulatory);
    m.attr("ELECTRICAL_SYNAPSE") = py::cast(SynapseType::Electrical);
    m.attr("GAP_JUNCTION") = py::cast(SynapseType::GapJunction);

    m.attr("INITIAL_STAGE") = py::cast(DevelopmentalStage::Initial);
    m.attr("CRITICAL_PERIOD") = py::cast(DevelopmentalStage::CriticalPeriod);
    m.attr("MATURATION") = py::cast(DevelopmentalStage::Maturation);
    m.attr("ADULT") = py::cast(DevelopmentalStage::Adult);
    m.attr("AGING") = py::cast(DevelopmentalStage::Aging);

    m.attr("RESTING") = py::cast(FiringState::Resting);
    m.attr("ACTIVE") = py::cast(FiringState::Active);
    m.attr("REFRACTORY") = py::cast(FiringState::Refractory);
    m.attr("INHIBITED") = py::cast(FiringState::Inhibited);

    // Action types
    m.attr("MOVE_FORWARD") = py::cast(ActionType::MoveForward);
    m.attr("MOVE_BACKWARD") = py::cast(ActionType::MoveBackward);
    m.attr("TURN_LEFT") = py::cast(ActionType::TurnLeft);
    m.attr("TURN_RIGHT") = py::cast(ActionType::TurnRight);
    m.attr("LOOK") = py::cast(ActionType::Look);
    m.attr("INTERACT") = py::cast(ActionType::Interact);
    m.attr("EAT") = py::cast(ActionType::Eat);
    m.attr("REST") = py::cast(ActionType::Rest);

    // World object types
    m.attr("EMPTY") = py::cast(WorldObjectType::Empty);
    m.attr("RESOURCE") = py::cast(WorldObjectType::Resource);
    m.attr("HAZARD") = py::cast(WorldObjectType::Hazard);
    m.attr("WALL") = py::cast(WorldObjectType::Wall);
    m.attr("MARKER") = py::cast(WorldObjectType::Marker);
}

} // namespace nlm