// Improved Python bindings for NLM (Neural Learning Machine)
// Provides enhanced API with better error handling, utility functions, and advanced features

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/eigen.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_set>
#include <algorithm>
#include <numeric>
#include <cmath>
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

// Custom Python exception types
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& msg) : NLMError("Configuration Error: " + msg) {}
};

class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& msg) : NLMError("Simulation Error: " + msg) {}
};

class ValidationError : public NLMError {
public:
    explicit ValidationError(const std::string& msg) : NLMError("Validation Error: " + msg) {}
};

// Helper functions
inline float clamp(float value, float min, float max) {
    return std::max(min, std::min(max, value));
}

inline std::vector<float> normalize(const std::vector<float>& data) {
    if (data.empty()) return data;
    float sum = std::accumulate(data.begin(), data.end(), 0.0f);
    if (sum == 0.0f) return data;
    float scale = 1.0f / sum;
    std::vector<float> result;
    result.reserve(data.size());
    for (float val : data) {
        result.push_back(val * scale);
    }
    return result;
}

inline float dot_product(const std::vector<float>& a, const std::vector<float>& b) {
    if (a.size() != b.size()) throw ValidationError("Vector dimensions must match for dot product");
    float result = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        result += a[i] * b[i];
    }
    return result;
}

inline float euclidean_distance(const std::vector<float>& a, const std::vector<float>& b) {
    if (a.size() != b.size()) throw ValidationError("Vector dimensions must match for distance calculation");
    float sum = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        float diff = a[i] - b[i];
        sum += diff * diff;
    }
    return std::sqrt(sum);
}

inline std::string to_lower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Batch processing utilities
class BatchProcessor {
public:
    struct BatchResult {
        std::vector<float> spike_rates;
        std::vector<float> firing_counts;
        std::vector<float> e_i_ratios;
        float total_time;
        int total_steps;
    };
    
    static BatchResult run_batch_simulation(
        const std::vector<std::shared_ptr<Brain>>& brains,
        const std::vector<std::shared_ptr<SimpleWorld>>& worlds,
        int steps_per_brain,
        float timestep = 0.1f
    ) {
        BatchResult result;
        result.spike_rates.reserve(brains.size());
        result.firing_counts.reserve(brains.size());
        result.e_i_ratios.reserve(brains.size());
        
        for (size_t i = 0; i < brains.size(); ++i) {
            if (!brains[i] || !worlds[i]) {
                throw SimulationError("Invalid brain or world in batch simulation");
            }
            
            brains[i]->reset();
            brains[i]->initialize();
            
            for (int step = 0; step < steps_per_brain; ++step) {
                // Simulate world update
                worlds[i]->update(timestep);
                
                // Process sensory input
                auto percept = worlds[i]->getSensoryPercept();
                
                // Run brain step
                brains[i]->step(step, step * timestep);
            }
            
            result.spike_rates.push_back(brains[i]->getAverageFiringRate());
            result.firing_counts.push_back(static_cast<float>(brains[i]->getTotalSpikeCount()));
            result.e_i_ratios.push_back(brains[i]->getExcitationInhibitionRatio());
        }
        
        result.total_time = steps_per_brain * timestep;
        result.total_steps = steps_per_brain;
        return result;
    }
};

// Enhanced configuration utilities
class ConfigUtils {
public:
    static std::shared_ptr<Config> create_advanced_config(
        int64_t neuron_count = 1000,
        int64_t region_count = 2,
        float connection_probability = 0.15,
        float plasticity_rate = 0.01,
        float development_rate = 0.001,
        std::string environment = "SimpleWorld"
    ) {
        auto config = std::make_shared<Config>();
        
        // Basic settings
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", neuron_count, ConfigSource::Default);
        config->set("region_count", region_count, ConfigSource::Default);
        config->set("connection_probability", connection_probability, ConfigSource::Default);
        
        // Plasticity settings
        config->set("plasticity_learning_rate", plasticity_rate, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        config->set("stdp_time_constant", 20.0f, ConfigSource::Default);
        
        // Development settings
        config->set("development_synaptogenesis_rate", development_rate, ConfigSource::Default);
        config->set("development_pruning_rate", 0.00001f, ConfigSource::Default);
        
        // Neuromodulation settings
        config->set("dopamine_baseline", 0.1f, ConfigSource::Default);
        config->set("dopamine_scale", 1.0f, ConfigSource::Default);
        
        // Environment settings
        config->set("environment_name", environment, ConfigSource::Default);
        config->set("environment_width", 20, ConfigSource::Default);
        config->set("environment_height", 20, ConfigSource::Default);
        config->set("vision_width", 8, ConfigSource::Default);
        config->set("vision_height", 8, ConfigSource::Default);
        config->set("vision_channels", 3, ConfigSource::Default);
        
        return config;
    }
    
    static std::map<std::string, std::string> get_config_summary(const std::shared_ptr<Config>& config) {
        std::map<std::string, std::string> summary;
        if (!config) return summary;
        
        auto keys = config->getKeys();
        for (const auto& key : keys) {
            std::string value_str;
            if (config->has(key)) {
                if (key.find("neuron_count") != std::string::npos) {
                    value_str = std::to_string(config->getOr<int64_t>(key, 0));
                } else if (key.find("connection_probability") != std::string::npos ||
                           key.find("dopamine_baseline") != std::string::npos ||
                           key.find("plasticity_learning_rate") != std::string::npos) {
                    value_str = std::to_string(config->getOr<float>(key, 0.0f));
                } else if (key.find("enabled") != std::string::npos ||
                           key.find("simulation_timestep") != std::string::npos) {
                    value_str = config->getOr<bool>(key, false) ? "true" : "false";
                } else {
                    value_str = config->getOr<std::string>(key, "");
                }
                summary[key] = value_str;
            }
        }
        return summary;
    }
};

// Enhanced brain utilities
class BrainUtils {
public:
    struct BrainStats {
        int64_t total_neurons;
        int64_t total_synapses;
        int64_t firing_neurons;
        float average_firing_rate;
        float excitation_inhibition_ratio;
        float development_progress;
        int64_t total_spikes;
        float energy_usage;
        int64_t memory_usage;
    };
    
    static BrainStats compute_comprehensive_stats(const std::shared_ptr<Brain>& brain) {
        if (!brain) throw NLMError("Brain is null");
        
        BrainStats stats;
        stats.total_neurons = brain->getTotalNeuronCount();
        stats.total_synapses = brain->getTotalSynapseCount();
        stats.firing_neurons = brain->getFiringNeuronCount();
        stats.average_firing_rate = brain->getAverageFiringRate();
        stats.excitation_inhibition_ratio = brain->getExcitationInhibitionRatio();
        stats.development_progress = brain->getDevelopmentalStage() == DevelopmentalStage::Adult ? 1.0f : 0.0f;
        stats.total_spikes = brain->getTotalSpikeCount();
        
        // Estimate energy and memory usage
        stats.energy_usage = stats.total_neurons * 0.1f;  // Rough estimate
        stats.memory_usage = (stats.total_neurons * sizeof(NeuronId) + 
                            stats.total_synapses * sizeof(float)) / 1024;  // KB estimate
        
        return stats;
    }
    
    static std::vector<float> get_neuron_activity_pattern(
        const std::shared_ptr<Brain>& brain,
        NeuronType neuron_type = NeuronType::Excitatory,
        int max_neurons = 100
    ) {
        if (!brain) throw NLMError("Brain is null");
        
        auto region = brain->getRegion(RegionId(1));
        if (!region) return {};
        
        auto neurons = region->getAllNeurons();
        std::vector<float> activities;
        activities.reserve(std::min(max_neurons, (int)neurons.size()));
        
        for (size_t i = 0; i < std::min(static_cast<size_t>(max_neurons), neurons.size()); ++i) {
            if (neurons[i]->getNeuronType() == neuron_type) {
                float activity = neurons[i]->getMembranePotential();
                activities.push_back(clamp(activity, -1.0f, 1.0f));
            }
        }
        
        return activities;
    }
    
    static std::shared_ptr<Brain> create_brain_with_pattern(
        const std::shared_ptr<Config>& config,
        const std::vector<float>& initial_pattern,
        float scale = 1.0f
    ) {
        if (!config) throw ConfigurationError("Config is null");
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Apply initial pattern to neurons
        if (!initial_pattern.empty() && config->getOr<int64_t>("neuron_count", 1000) > 0) {
            auto region = brain->getRegion(RegionId(1));
            if (region) {
                auto neurons = region->getAllNeurons();
                size_t pattern_size = std::min(initial_pattern.size(), neurons.size());
                for (size_t i = 0; i < pattern_size; ++i) {
                    float scaled_activity = initial_pattern[i] * scale;
                    neurons[i]->injectCurrent(scaled_activity);
                }
            }
        }
        
        return brain;
    }
};

// Enhanced agent utilities
class AgentUtils {
public:
    struct AgentPerformance {
        float curiosity_level;
        float novelty_level;
        float prediction_error;
        float neuromodulation_level;
        float exploration_rate;
        float learning_efficiency;
        float action_diversity;
        float adaptation_rate;
    };
    
    static AgentPerformance analyze_agent_performance(const std::shared_ptr<AgentBrain>& agent) {
        if (!agent) throw NLMError("Agent is null");
        
        AgentPerformance perf;
        perf.curiosity_level = agent->getCuriosityLevel();
        perf.novelty_level = agent->getNoveltyLevel();
        perf.prediction_error = agent->getPredictionError();
        perf.neuromodulation_level = agent->getNeuromodulationLevel();
        perf.exploration_rate = agent->getCuriosityLevel() * 0.5f;  // Rough estimate
        
        // Calculate learning efficiency
        perf.learning_efficiency = perf.curiosity_level * (1.0f - std::abs(perf.prediction_error));
        
        // Calculate action diversity
        perf.action_diversity = std::min(1.0f, perf.curiosity_level * 0.8f);
        
        // Calculate adaptation rate
        perf.adaptation_rate = perf.exploration_rate * perf.learning_efficiency;
        
        return perf;
    }
    
    static std::vector<std::string> get_recommended_actions(
        const std::shared_ptr<AgentBrain>& agent,
        const std::shared_ptr<SimpleWorld>& world,
        int max_suggestions = 5
    ) {
        if (!agent || !world) throw NLMError("Agent or world is null");
        
        std::vector<std::string> suggestions;
        auto percept = world->getSensoryPercept();
        
        // Basic action suggestions based on agent state
        if (agent->getCuriosityLevel() > 0.7f) {
            suggestions.push_back("Explore new area");
            suggestions.push_back("Investigate anomaly");
        }
        
        if (agent->getNoveltyLevel() > 0.5f) {
            suggestions.push_back("Try different approach");
        }
        
        if (agent->getPredictionError() > 0.3f) {
            suggestions.push_back("Reevaluate current strategy");
        }
        
        if (agent->getNeuromodulationLevel() < 0.3f) {
            suggestions.push_back("Rest and recover");
        }
        
        // Limit suggestions
        if (suggestions.size() > static_cast<size_t>(max_suggestions)) {
            suggestions.resize(max_suggestions);
        }
        
        return suggestions;
    }
};

// World utilities
class WorldUtils {
public:
    static std::vector<float> get_environment_statistics(const std::shared_ptr<SimpleWorld>& world) {
        if (!world) throw NLMError("World is null");
        
        std::vector<float> stats;
        stats.reserve(8);
        
        stats.push_back(world->getWidth());
        stats.push_back(world->getHeight());
        stats.push_back(world->getSimulationTime());
        stats.push_back(world->getMaxEnergy());
        stats.push_back(world->getEnergyDecayRate());
        
        // Get agent body stats
        auto body = world->getAgentBody();
        stats.push_back(body.x);
        stats.push_back(body.y);
        
        return stats;
    }
    
    static std::shared_ptr<SimpleWorld> create_world(
        int width = 20,
        int height = 20,
        float vision_width = 8.0f,
        float vision_height = 8.0f,
        int max_energy = 100,
        float energy_decay = 0.01f
    ) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, vision_width, vision_height);
        world->setMaxEnergy(max_energy);
        world->setEnergyDecayRate(energy_decay);
        world->reset();
        return world;
    }
};

// Main binding function
PYBIND11_MODULE(pynlm_enhanced, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        Enhanced Python binding for the NLM C++ neural simulation framework.
        Provides advanced API with utility functions, batch processing, and comprehensive error handling.
    )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMError>(m, "NLMError");
    py::register_exception<ConfigurationError>(m, "ConfigurationError");
    py::register_exception<SimulationError>(m, "SimulationError");
    py::register_exception<ValidationError>(m, "ValidationError");

    // Register basic types (reusing existing bindings)
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

    // Register all existing classes (simplified for brevity)
    // In practice, this would include all the existing bindings from bindings.cpp

    // Enhanced Config class
    py::class_<Config>(m, "Config", R"pbdoc(Advanced configuration class for NLM system)pbdoc")
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
        .def("getOr", [](Config& self, const std::string& key, int64_t default_val) {
            return self.getOr<int64_t>(key, default_val);
        }, py::arg("key"), py::arg("default_val"), "Get configuration value with integer default")
        .def("getOr", [](Config& self, const std::string& key, float default_val) {
            return self.getOr<float>(key, default_val);
        }, py::arg("key"), py::arg("default_val"), "Get configuration value with float default")
        .def("getOr", [](Config& self, const std::string& key, bool default_val) {
            return self.getOr<bool>(key, default_val);
        }, py::arg("key"), py::arg("default_val"), "Get configuration value with bool default")
        .def("set", &Config::set, py::arg("key"), py::arg("value"), py::arg("source"),
             "Set configuration value")
        .def("validate", [](Config& self) {
            auto keys = self.getKeys();
            bool valid = true;
            for (const auto& key : keys) {
                if (key.empty()) {
                    valid = false;
                    break;
                }
            }
            return valid;
        }, "Validate configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<EnhancedConfig: " + cfg.summary() + ">";
        });

    // Advanced utility functions
    m.def("normalize", &normalize, py::arg("data"),
          "Normalize vector data to [0, 1] range");
    
    m.def("clamp", &clamp, py::arg("value"), py::arg("min"), py::arg("max"),
          "Clamp value to [min, max] range");
    
    m.def("euclidean_distance", &euclidean_distance, py::arg("a"), py::arg("b"),
          "Calculate Euclidean distance between two vectors");
    
    m.def("dot_product", &dot_product, py::arg("a"), py::arg("b"),
          "Calculate dot product of two vectors");

    // Enhanced brain creation with utilities
    m.def("createAdvancedConfig", &ConfigUtils::create_advanced_config,
          py::arg("neuron_count") = 1000,
          py::arg("region_count") = 2,
          py::arg("connection_probability") = 0.15,
          py::arg("plasticity_rate") = 0.01,
          py::arg("development_rate") = 0.001,
          py::arg("environment") = "SimpleWorld",
          "Create advanced configuration with optimized settings");
    
    m.def("getConfigSummary", &ConfigUtils::get_config_summary,
          py::arg("config"), "Get configuration summary as dictionary");
    
    // Batch processing utilities
    m.def("runBatchSimulation", &BatchProcessor::run_batch_simulation,
          py::arg("brains"), py::arg("worlds"), py::arg("steps_per_brain"),
          py::arg("timestep") = 0.1f,
          "Run batch simulation across multiple brains");
    
    // Enhanced brain statistics
    m.def("computeBrainStats", &BrainUtils::compute_comprehensive_stats,
          py::arg("brain"), "Compute comprehensive brain statistics");
    
    m.def("getNeuronActivityPattern", &BrainUtils::get_neuron_activity_pattern,
          py::arg("brain"), py::arg("neuron_type") = NeuronType::Excitatory,
          py::arg("max_neurons") = 100,
          "Get neuron activity pattern for analysis");
    
    m.def("createBrainWithPattern", &BrainUtils::create_brain_with_pattern,
          py::arg("config"), py::arg("initial_pattern"), py::arg("scale") = 1.0f,
          "Create brain with initial activity pattern");
    
    // Enhanced agent analysis
    m.def("analyzeAgentPerformance", &AgentUtils::analyze_agent_performance,
          py::arg("agent"), "Analyze agent performance metrics");
    
    m.def("getRecommendedActions", &AgentUtils::get_recommended_actions,
          py::arg("agent"), py::arg("world"), py::arg("max_suggestions") = 5,
          "Get recommended actions based on agent state");
    
    // World utilities
    m.def("createWorld", &WorldUtils::create_world,
          py::arg("width") = 20, py::arg("height") = 20,
          py::arg("vision_width") = 8.0f, py::arg("vision_height") = 8.0f,
          py::arg("max_energy") = 100, py::arg("energy_decay") = 0.01f,
          "Create configured world instance");
    
    m.def("getEnvironmentStats", &WorldUtils::get_environment_statistics,
          py::arg("world"), "Get environment statistics as vector");

    // Factory functions (maintaining compatibility with existing API)
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

    // Version info
    m.attr("__version__") = "1.1.0-enhanced";
    m.attr("__description__") = "Enhanced NLM Python bindings with advanced features";
}

} // namespace nlm