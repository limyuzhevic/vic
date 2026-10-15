# Improved Python Bindings with Better Error Handling and Documentation

This file provides enhanced Python bindings with improved error handling and comprehensive documentation.

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

namespace py = pybind11;
namespace nlm {

// Enhanced exception classes for better error handling
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& message) : std::runtime_error(message) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& message) : NLMError("Configuration Error: " + message) {}
};

class BrainError : public NLMError {
public:
    explicit BrainError(const std::string& message) : NLMError("Brain Error: " + message) {}
};

class WorldError : public NLMError {
public:
    explicit WorldError(const std::string& message) : NLMError("World Error: " + message) {}
};

class AgentError : public NLMError {
public:
    explicit AgentError(const std::string& message) : NLMError("Agent Error: " + message) {}
};

// Helper function to validate configuration keys
inline bool validateRequiredConfigKeys(const Config& config, 
                                      const std::vector<std::string>& requiredKeys) {
    std::vector<std::string> missingKeys;
    for (const auto& key : requiredKeys) {
        if (!config.has(key)) {
            missingKeys.push_back(key);
        }
    }
    
    if (!missingKeys.empty()) {
        std::stringstream ss;
        ss << "Missing required configuration keys: ";
        for (size_t i = 0; i < missingKeys.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << "\"" << missingKeys[i] << "\"";
        }
        throw ConfigurationError(ss.str());
    }
    return true;
}

// Helper function to create config with validation
inline std::shared_ptr<Config> createValidatedConfig(const py::dict& config_dict,
                                                     const std::vector<std::string>& requiredKeys = {}) {
    auto config = std::make_shared<Config>();
    
    // Convert Python dict to C++ config
    for (auto item : config_dict) {
        std::string key = py::str(item.first);
        std::string value_str = py::str(item.second);
        
        // Try to parse as different types
        try {
            float float_val = std::stof(value_str);
            config->set(key, float_val, ConfigSource::Runtime);
        } catch (...) {
            try {
                int64_t int_val = std::stoll(value_str);
                config->set(key, int_val, ConfigSource::Runtime);
            } catch (...) {
                try {
                    bool bool_val = (value_str == "true" || value_str == "1");
                    config->set(key, bool_val, ConfigSource::Runtime);
                } catch (...) {
                    config->set(key, value_str, ConfigSource::Runtime);
                }
            }
        }
    }
    
    // Validate required keys
    if (!requiredKeys.empty()) {
        validateRequiredConfigKeys(*config, requiredKeys);
    }
    
    return config;
}

// Enhanced configuration helper functions
inline std::shared_ptr<Config> createBasicConfig(int neuronCount = 1000, 
                                                   int regionCount = 1,
                                                   float connectionProbability = 0.1) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(neuronCount));
    config->set("region_count", static_cast<int64_t>(regionCount));
    config->set("connection_probability", connectionProbability);
    return config;
}

inline std::shared_ptr<Config> createDevelopmentConfig(int neuronCount = 500,
                                                         bool enableDevelopment = true,
                                                         bool enableCuriosity = true,
                                                         bool enableRewardModulation = true) {
    auto config = createBasicConfig(neuronCount);
    
    // Enable all plasticity systems
    config->set("plasticity.stdp.enable", true);
    config->set("plasticity.hebbian.enable", true);
    config->set("plasticity.structural.enable", true);
    
    // Enable neuromodulation
    config->set("neuromod.dopamine.scale", 1.0f);
    config->set("neuromod.curiosity.enable", enableCuriosity);
    config->set("neuromod.novelty.enable", true);
    config->set("neuromod.prediction_error.enable", true);
    
    // Enable development
    config->set("development.enable", enableDevelopment);
    if (enableDevelopment) {
        config->set("development.critical_period_duration", 1000.0f);
        config->set("development.plasticity_decay_rate", 0.01f);
    }
    
    // Enable prediction system
    config->set("prediction.enable", true);
    config->set("prediction.confidence_threshold", 0.5f);
    
    return config;
}

// Enhanced brain statistics with error handling
inline std::string getBrainStatistics(const Brain& brain, 
                                     const std::vector<std::string>& statsTypes = {"basic"}) {
    std::stringstream ss;
    ss << "=== NLM Brain Statistics ===" << std::endl;
    
    for (const auto& statType : statsTypes) {
        if (statType == "basic") {
            ss << "Basic Statistics:" << std::endl;
            ss << "  Total Neurons: " << brain.getTotalNeuronCount() << std::endl;
            ss << "  Total Synapses: " << brain.getTotalSynapseCount() << std::endl;
            ss << "  Active Neurons: " << brain.getActiveNeuronCount() << std::endl;
            ss << "  Firing Neurons: " << brain.getFiringNeuronCount() << std::endl;
            ss << "  Total Spikes: " << brain.getTotalSpikeCount() << std::endl;
            ss << "  Average Firing Rate: " << brain.getAverageFiringRate() << " Hz" << std::endl;
            ss << "  Excitatory/Inhibition Ratio: " << brain.getExcitationInhibitionRatio() << std::endl;
            ss << "  Developmental Stage: " << getDevelopmentStageName(brain.getDevelopmentalStage()) << std::endl;
        }
        else if (statType == "connectivity") {
            ss << "Connectivity Statistics:" << std::endl;
            ss << "  Total synapses: " << brain.getTotalSynapseCount() << std::endl;
            if (brain.getTotalSynapseCount() > 0) {
                float totalWeight = 0.0f;
                size_t synapseCount = 0;
                for (const auto& region : brain.getRegions()) {
                    for (const auto& synapse : region->getSynapses()) {
                        totalWeight += synapse->getWeight();
                        synapseCount++;
                    }
                }
                if (synapseCount > 0) {
                    ss << "  Average synapse weight: " << (totalWeight / synapseCount) << std::endl;
                }
            }
        }
        else if (statType == "activity") {
            ss << "Activity Statistics:" << std::endl;
            ss << "  Total spikes: " << brain.getTotalSpikeCount() << std::endl;
            ss << "  Active neurons: " << brain.getActiveNeuronCount() << std::endl;
            ss << "  Firing neurons: " << brain.getFiringNeuronCount() << std::endl;
            ss << "  Average firing rate: " << brain.getAverageFiringRate() << " Hz" << std::endl;
            ss << "  Spike frequency: " << (brain.getAverageFiringRate() * 60.0f) << " spikes/minute" << std::endl;
        }
    }
    
    return ss.str();
}

// Enhanced simulation helper with error handling and validation
inline std::string runValidatedSimulation(PyAgentBrain& agent, 
                                           PySimpleWorld& world, 
                                           size_t steps,
                                           bool verbose = false) {
    if (steps == 0) {
        throw AgentError("Simulation steps must be greater than 0");
    }
    
    std::stringstream ss;
    ss << "=== Running NLM Simulation ===" << std::endl;
    ss << "Steps: " << steps << std::endl;
    ss << "Initial world: " << world.getWorldStatus() << std::endl;
    
    float totalReward = 0.0f;
    float totalFiringRate = 0.0f;
    size_t firingSteps = 0;
    
    for (size_t step = 0; step < steps; ++step) {
        try {
            // Update world
            world.update(0.1);
            
            // Get sensory input
            const SensoryPercept& percept = world.getSensoryPercept();
            
            // Process sensory input
            agent.processSensoryInput(percept);
            
            // Get action
            MotorCommand action = agent.decodeMotorCommand();
            
            // Collect statistics
            float curiosity = agent.getCuriosityLevel();
            float novelty = agent.getNoveltyLevel();
            float predictionError = agent.getPredictionError();
            float neuromodulation = agent.getNeuromodulationLevel();
            
            totalReward += neuromodulation;
            totalFiringRate += curiosity; // Using curiosity as proxy for firing rate
            if (curiosity > 0.1f) firingSteps++;
            
            // Log step details if verbose
            if (verbose && step % 100 == 0) {
                ss << "Step " << step << ": "
                   << "Curiosity: " << curiosity << ", "
                   << "Novelty: " << novelty << ", "
                   << "Reward: " << neuromodulation << std::endl;
            }
            
        } catch (const std::exception& e) {
            throw AgentError(std::string("Error at step ") + std::to_string(step) + ": " + e.what());
        }
    }
    
    // Calculate final statistics
    float avgFiringRate = (steps > 0) ? totalFiringRate / steps : 0.0f;
    float avgCuriosity = (steps > 0) ? totalReward / steps : 0.0f;
    
    ss << "=== Simulation Complete ===" << std::endl;
    ss << "Total steps: " << steps << std::endl;
    ss << "Average curiosity: " << avgCuriosity << std::endl;
    ss << "Average firing rate: " << avgFiringRate << std::endl;
    ss << "Steps with activity: " << firingSteps << " (" << (100.0f * firingSteps / steps) << "%)" << std::endl;
    ss << "Final world: " << world.getWorldStatus() << std::endl;
    
    return ss.str();
}

// Enhanced configuration validation and analysis
inline std::string analyzeConfig(const Config& config) {
    std::stringstream ss;
    ss << "=== Configuration Analysis ===" << std::endl;
    ss << "Keys: " << config.getKeys().size() << std::endl;
    
    ss << "Key values:" << std::endl;
    for (const auto& key : config.getKeys()) {
        if (config.has(key)) {
            try {
                auto float_val = config.getOr<float>(key, 0.0f);
                ss << "  \"" << key << "\": " << float_val << " (float)" << std::endl;
            } catch (...) {
                try {
                    auto int_val = config.getOr<int64_t>(key, 0);
                    ss << "  \"" << key << "\": " << int_val << " (int)" << std::endl;
                } catch (...) {
                    try {
                        auto bool_val = config.getOr<bool>(key, false);
                        ss << "  \"" << key << "\": " << (bool_val ? "true" : "false") << " (bool)" << std::endl;
                    } catch (...) {
                        auto str_val = config.getOr<std::string>(key, "");
                        ss << "  \"" << key << "\": \"" << str_val << "\" (string)" << std::endl;
                    }
                }
            }
        }
    }
    
    return ss.str();
}

// Memory usage and performance monitoring
inline std::string getPerformanceStats(const Brain& brain) {
    std::stringstream ss;
    ss << "=== Performance Statistics ===" << std::endl;
    
    // Basic metrics
    ss << "Neural Activity:" << std::endl;
    ss << "  Neurons: " << brain.getTotalNeuronCount() << std::endl;
    ss << "  Synapses: " << brain.getTotalSynapseCount() << std::endl;
    ss << "  Active: " << brain.getActiveNeuronCount() << std::endl;
    ss << "  Firing: " << brain.getFiringNeuronCount() << std::endl;
    ss << "  Spikes: " << brain.getTotalSpikeCount() << std::endl;
    ss << "  Firing Rate: " << brain.getAverageFiringRate() << " Hz" << std::endl;
    
    // Network properties
    ss << "Network Properties:" << std::endl;
    ss << "  Excitatory/Inhibition Ratio: " << brain.getExcitationInhibitionRatio() << std::endl;
    ss << "  Developmental Stage: " << getDevelopmentStageName(brain.getDevelopmentalStage()) << std::endl;
    
    // Memory usage (estimated)
    size_t memoryEstimate = brain.getTotalNeuronCount() * sizeof(NeuronState) +
                           brain.getTotalSynapseCount() * sizeof(Synapse) +
                           brain.getRegionCount() * sizeof(NeuralRegion);
    
    ss << "Memory Estimate:" << std::endl;
    ss << "  Approx. size: " << (memoryEstimate / 1024.0f) << " KB" << std::endl;
    
    return ss.str();
}

// Enhanced error checking and validation
inline void validateBrainState(const Brain& brain) {
    if (brain.getTotalNeuronCount() == 0) {
        throw BrainError("Brain has no neurons initialized");
    }
    
    if (brain.getRegionCount() == 0) {
        throw BrainError("Brain has no regions defined");
    }
    
    // Check for any inconsistencies
    size_t totalNeuronsFromRegions = 0;
    for (const auto& regionId : brain.getRegionIds()) {
        if (auto* region = brain.getRegion(regionId)) {
            totalNeuronsFromRegions += region->getAllNeurons().size();
        }
    }
    
    if (totalNeuronsFromRegions != brain.getTotalNeuronCount()) {
        throw BrainError("Neuron count inconsistency: region total (" + 
                        std::to_string(totalNeuronsFromRegions) + ") != brain total (" +
                        std::to_string(brain.getTotalNeuronCount()) + ")");
    }
}

// Create enhanced configuration from parameters
inline std::shared_ptr<Config> createConfigFromParams(const py::kwargs& kwargs) {
    auto config = std::make_shared<Config>();
    
    // Set basic parameters
    if (kwargs.contains("neuron_count")) {
        config->set("neuron_count", py::cast<int64_t>(kwargs["neuron_count"]));
    }
    if (kwargs.contains("region_count")) {
        config->set("region_count", py::cast<int64_t>(kwargs["region_count"]));
    }
    if (kwargs.contains("connection_probability")) {
        config->set("connection_probability", py::cast<float>(kwargs["connection_probability"]));
    }
    
    // Set plasticity parameters
    if (kwargs.contains("stdp_enabled")) {
        config->set("plasticity.stdp.enable", py::cast<bool>(kwargs["stdp_enabled"]));
    }
    if (kwargs.contains("hebbian_enabled")) {
        config->set("plasticity.hebbian.enable", py::cast<bool>(kwargs["hebbian_enabled"]));
    }
    if (kwargs.contains("structural_enabled")) {
        config->set("plasticity.structural.enable", py::cast<bool>(kwargs["structural_enabled"]));
    }
    
    // Set neuromodulation parameters
    if (kwargs.contains("dopamine_scale")) {
        config->set("neuromod.dopamine.scale", py::cast<float>(kwargs["dopamine_scale"]));
    }
    if (kwargs.contains("curiosity_enabled")) {
        config->set("neuromod.curiosity.enable", py::cast<bool>(kwargs["curiosity_enabled"]));
    }
    if (kwargs.contains("novelty_enabled")) {
        config->set("neuromod.novelty.enable", py::cast<bool>(kwargs["novelty_enabled"]));
    }
    
    // Set development parameters
    if (kwargs.contains("development_enabled")) {
        config->set("development.enable", py::cast<bool>(kwargs["development_enabled"]));
    }
    if (kwargs.contains("critical_period_duration")) {
        config->set("development.critical_period_duration", 
                   py::cast<float>(kwargs["critical_period_duration"]));
    }
    
    // Set prediction parameters
    if (kwargs.contains("prediction_enabled")) {
        config->set("prediction.enable", py::cast<bool>(kwargs["prediction_enabled"]));
    }
    
    return config;
}

// Enhanced example with error handling and validation
inline std::string runValidatedExample(int scenario, const py::kwargs& kwargs) {
    try {
        switch (scenario) {
            case 1: { // Silent brain
                auto config = createBasicConfig(
                    py::cast<int>(kwargs.get("neuron_count", 1000)),
                    py::cast<int>(kwargs.get("region_count", 1))
                );
                
                auto brain = createBrain(config);
                
                std::stringstream ss;
                ss << "=== Silent Brain Example ===" << std::endl;
                ss << "Created brain with " << brain->getTotalNeuronCount() << " neurons" << std::endl;
                ss << "Running " << py::cast<int>(kwargs.get("steps", 10)) << " steps..." << std::endl;
                
                for (int i = 0; i < py::cast<int>(kwargs.get("steps", 10)); ++i) {
                    brain->step(i);
                }
                
                ss << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
                ss << "Final firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
                return ss.str();
            }
            
            case 2: { // Complete agent
                auto config = createDevelopmentConfig(
                    py::cast<int>(kwargs.get("neuron_count", 500)),
                    py::cast<bool>(kwargs.get("development_enabled", true)),
                    py::cast<bool>(kwargs.get("curiosity_enabled", true)),
                    py::cast<bool>(kwargs.get("reward_enabled", true))
                );
                
                auto brain = createBrain(config);
                PyAgentBrain agent(brain);
                PySimpleWorld world;
                
                world.configure(
                    py::cast<int>(kwargs.get("world_width", 20)),
                    py::cast<int>(kwargs.get("world_height", 20)),
                    py::cast<int>(kwargs.get("vision_width", 8)),
                    py::cast<int>(kwargs.get("vision_height", 8))
                );
                
                world.reset();
                agent.initialize(world);
                
                // Enable subsystems
                agent.enableRewardModulation(py::cast<bool>(kwargs.get("reward_enabled", true)));
                agent.enableStructuralPlasticity(py::cast<bool>(kwargs.get("structural_enabled", true)));
                agent.enableDevelopment(py::cast<bool>(kwargs.get("development_enabled", true)));
                agent.enableCuriosity(py::cast<bool>(kwargs.get("curiosity_enabled", true)));
                
                auto result = runValidatedSimulation(agent, world, 
                    py::cast<size_t>(kwargs.get("steps", 100)),
                    py::cast<bool>(kwargs.get("verbose", false)));
                
                return "=== Complete Agent Example ===\n" + result;
            }
            
            default:
                throw AgentError("Unknown example scenario: " + std::to_string(scenario));
        }
    } catch (const std::exception& e) {
        throw AgentError(std::string("Example execution failed: ") + e.what());
    }
}

PYBIND11_MODULE(pynlm_enhanced, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        --------------------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        
        This enhanced version provides additional convenience methods and Pythonic APIs
        with improved error handling and comprehensive documentation.
        
        Key Features:
        - Enhanced error handling with specific exception types
        - Configuration validation and analysis tools
        - Performance monitoring and statistics
        - Example scenarios with validation
        - Advanced configuration helpers
        - Simulation validation and statistics
    )pbdoc";

    // Register exception classes
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const NLMError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::exception& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });

    // Re-export original exception class for compatibility
    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Enhanced configuration helper functions
    m.def("createValidatedConfig", &createValidatedConfig, 
          py::arg("config_dict"), py::arg("requiredKeys"), 
          "Create configuration from Python dict with validation");
    
    m.def("createBasicConfig", &createBasicConfig,
          py::arg("neuronCount") = 1000, py::arg("regionCount") = 1,
          py::arg("connectionProbability") = 0.1,
          "Create a basic configuration with specified parameters");
    
    m.def("createDevelopmentConfig", &createDevelopmentConfig,
          py::arg("neuronCount") = 500, py::arg("enableDevelopment") = true,
          py::arg("enableCuriosity") = true, py::arg("enableRewardModulation") = true,
          "Create a development-focused configuration");
    
    m.def("createConfigFromParams", &createConfigFromParams,
          py::arg("kwargs"), "Create configuration from keyword arguments");

    // Enhanced analysis and validation functions
    m.def("validateRequiredConfigKeys", &validateRequiredConfigKeys,
          py::arg("config"), py::arg("requiredKeys"),
          "Validate that configuration contains all required keys");
    
    m.def("getBrainStatistics", &getBrainStatistics,
          py::arg("brain"), py::arg("statsTypes"), 
          "Get brain statistics with specified analysis types");
    
    m.def("analyzeConfig", &analyzeConfig,
          py::arg("config"), "Analyze configuration structure and values");
    
    m.def("getPerformanceStats", &getPerformanceStats,
          py::arg("brain"), "Get performance and memory statistics for brain");
    
    m.def("validateBrainState", &validateBrainState,
          py::arg("brain"), "Validate brain state for consistency and completeness");

    // Enhanced simulation helpers
    m.def("runValidatedSimulation", &runValidatedSimulation,
          py::arg("agent"), py::arg("world"), py::arg("steps"), py::arg("verbose") = false,
          "Run validated simulation with error handling and statistics");

    // Enhanced example functions
    m.def("runExample", &runValidatedExample,
          py::arg("scenario"), py::arg("kwargs"),
          "Run validated example scenario with specified parameters");

    // Enhanced factory functions
    m.def("createBrain", &createBrain, py::arg("config"),
          "Create and automatically initialize a brain");
    
    m.def("createBrainAndInitialize", &createBrainAndInitialize,
          py::arg("config"), "Create brain with initialization context manager");

    // Version and metadata
    m.attr("__version__") = "enhanced-0.2.0";
    m.attr("__description__") = "Enhanced Python bindings for NLM with improved error handling";
    
    // Error message templates for common issues
    m.def("getErrorMessage", [](const std::string& errorType, const std::string& details) {
        static const std::unordered_map<std::string, std::string> templates = {
            {"brain_not_initialized", "Brain has not been initialized. Call brain.initialize() first."},
            {"world_not_configured", "World has not been configured. Call world.configure() first."},
            {"agent_not_initialized", "Agent has not been initialized. Call agent.initialize() first."},
            {"step_too_large", "Simulation step count is too large. Try reducing the number of steps."},
            {"insufficient_neurons", "Brain has insufficient neurons for the requested operation."},
            {"invalid_config", "Configuration is invalid or incomplete."}
        };
        
        auto it = templates.find(errorType);
        if (it != templates.end()) {
            return it->second + (details.empty() ? "" : " Details: " + details);
        }
        return "Unknown error: " + errorType + (details.empty() ? "" : " Details: " + details);
    }, py::arg("errorType"), py::arg("details") = "",
          "Get formatted error message for common error types");

    // Usage examples
    m.def("example_1_silent_brain", R"pbdoc(
        Example: Create and run a silent brain (just neurons)
        
        This example shows the simplest way to create a virtual brain
        that can think without interacting with a world.
        
        Returns: Human-readable explanation of the example
    )pbdoc", []() {
        return runValidatedExample(1, py::kwargs());
    });
    
    m.def("example_2_complete_agent", R"pbdoc(
        Example: Complete agent with learning and development
        
        This example shows a complete agent that can learn from experience,
        develop over time, and adapt to its environment.
        
        Returns: Simulation results and statistics
    )pbdoc", []() {
        return runValidatedExample(2, py::kwargs());
    });
    
    m.def("example_3_advanced_agent", R"pbdoc(
        Example: Advanced agent with full feature set
        
        This example demonstrates advanced usage with all subsystems enabled,
        including development, curiosity, reward modulation, and structural plasticity.
        
        Returns: Advanced simulation results with detailed statistics
    )pbdoc", []() {
        py::kwargs kwargs;
        kwargs["steps"] = 200;
        kwargs["neuron_count"] = 800;
        kwargs["world_width"] = 30;
        kwargs["world_height"] = 30;
        kwargs["vision_width"] = 12;
        kwargs["vision_height"] = 12;
        kwargs["development_enabled"] = true;
        kwargs["curiosity_enabled"] = true;
        kwargs["reward_enabled"] = true;
        kwargs["structural_enabled"] = true;
        kwargs["verbose"] = false;
        
        return runValidatedExample(2, kwargs);
    });
}
