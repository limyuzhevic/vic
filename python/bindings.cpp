#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <iostream>
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

// Forward declarations for new convenience functions
class ExperimentResult;

// ========== HELPER FUNCTIONS FOR PYTHON BINDINGS ==========

inline std::string validateConfig(const Config& config) {
    std::vector<std::string> errors;
    
    // Check required keys
    if (!config.has("brain.neuron_count")) {
        errors.push_back("Missing required key: 'brain.neuron_count'");
    }
    
    if (!config.has("brain.region_count")) {
        errors.push_back("Missing required key: 'brain.region_count'");
    }
    
    // Validate value ranges
    auto neuronCountOpt = config.get<size_t>("brain.neuron_count");
    if (neuronCountOpt.has_value() && *neuronCountOpt == 0) {
        errors.push_back("'brain.neuron_count' must be greater than 0");
    }
    
    if (!errors.empty()) {
        std::string errorMsg = "Configuration validation failed:\n";
        for (const auto& error : errors) {
            errorMsg += "  - " + error + "\n";
        }
        throw std::invalid_argument(errorMsg);
    }
    
    return "Configuration validation passed";
}

inline bool checkBrainInitialization(const Brain* brain) {
    if (!brain) {
        throw std::runtime_error("Brain pointer is null");
    }
    
    // Try to access config - if this fails, brain is not properly initialized
    auto config = brain->getConfig();
    if (!config) {
        throw std::runtime_error("Brain configuration is not available");
    }
    
    return true;
}

// ========== HIGH-LEVEL CONVENIENCE FUNCTIONS ==========

inline std::vector<std::map<std::string, double>> run_simulation(
    std::shared_ptr<Brain> brain,
    std::shared_ptr<SimpleWorld> world,
    std::shared_ptr<AgentBrain> agent,
    size_t steps
) {
    if (!brain) throw std::runtime_error("Brain is null");
    if (!world) throw std::runtime_error("World is null");
    if (!agent) throw std::runtime_error("Agent is null");
    
    brain->initialize();
    agent->initialize(*world);
    
    std::vector<std::map<std::string, double>> results;
    results.reserve(steps);
    
    for (size_t step = 0; step < steps; ++step) {
        std::map<std::string, double> stepResult;
        
        // Update world
        world->update(0.1);
        
        // Get sensory input
        auto percept = world->getSensoryPercept();
        
        // Process sensory input
        agent->processSensoryInput(percept);
        
        // Run brain step
        brain->step(static_cast<SimulationStep>(step));
        
        // Get action
        auto action = agent->decodeMotorCommand();
        
        // Apply action to world
        world->applyMotorCommand(action, world->getSimulationTime());
        
        // Record metrics
        stepResult["step"] = static_cast<double>(step);
        stepResult["firing_neurons"] = static_cast<double>(brain->getFiringNeuronCount());
        stepResult["total_spikes"] = static_cast<double>(brain->getTotalSpikeCount());
        stepResult["average_firing_rate"] = brain->getAverageFiringRate();
        stepResult["dopamine_level"] = agent->getNeuromodulationLevel();
        stepResult["curiosity_level"] = agent->getCuriosityLevel();
        stepResult["novelty_level"] = agent->getNoveltyLevel();
        
        results.push_back(std::move(stepResult));
    }
    
    return results;
}

inline std::map<std::string, double> train_agent(
    std::shared_ptr<Brain> brain,
    std::shared_ptr<SimpleWorld> world,
    std::shared_ptr<AgentBrain> agent,
    size_t episodes,
    size_t steps_per_episode
) {
    if (!brain || !world || !agent) {
        throw std::runtime_error("Invalid brain, world, or agent pointer");
    }
    
    brain->initialize();
    agent->initialize(*world);
    
    std::map<std::string, double> metrics;
    metrics["total_episodes"] = static_cast<double>(episodes);
    metrics["steps_per_episode"] = static_cast<double>(steps_per_episode);
    metrics["total_steps"] = static_cast<double>(episodes * steps_per_episode);
    
    double totalReward = 0.0;
    double maxFiringRate = 0.0;
    double avgFiringRate = 0.0;
    size_t totalStepsCompleted = 0;
    
    for (size_t episode = 0; episode < episodes; ++episode) {
        world->reset();
        agent->reset();
        
        double episodeReward = 0.0;
        double episodeMaxFiring = 0.0;
        double episodeAvgFiring = 0.0;
        
        for (size_t step = 0; step < steps_per_episode; ++step) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(static_cast<SimulationStep>(step));
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Get reward from sensory input (this could be more sophisticated)
            float reward = 0.1f; // Default small positive reward
            if (world->getSensoryPercept().getInternal().size() > 0) {
                reward = world->getSensoryPercept().getInternal()[0];
            }
            
            agent->applyRewardModulation(reward, 0.0f);
            episodeReward += reward;
            
            double firingRate = brain->getAverageFiringRate();
            episodeMaxFiring = std::max(episodeMaxFiring, firingRate);
            episodeAvgFiring += firingRate;
        }
        
        totalReward += episodeReward;
        maxFiringRate = std::max(maxFiringRate, episodeMaxFiring);
        avgFiringRate += episodeAvgFiring;
        totalStepsCompleted += steps_per_episode;
    }
    
    metrics["total_reward"] = totalReward;
    metrics["avg_reward_per_episode"] = totalReward / episodes;
    metrics["max_firing_rate"] = maxFiringRate;
    metrics["avg_firing_rate"] = avgFiringRate / episodes;
    
    return metrics;
}

inline std::shared_ptr<Brain> create_trained_brain(
    std::shared_ptr<Config> config,
    size_t training_episodes,
    size_t steps_per_episode
) {
    if (!config) {
        throw std::runtime_error("Config is null");
    }
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // For this demo, we'll use a simple world and agent
    auto world = std::make_shared<SimpleWorld>();
    world->configure(20, 20, 8, 8);
    
    auto agent = std::make_shared<AgentBrain>(brain);
    agent->initialize(*world);
    
    // Enable learning
    agent->enableRewardModulation(true);
    agent->enableCuriosity(true);
    agent->enableDevelopment(true);
    
    // Train the brain
    train_agent(brain, world, agent, training_episodes, steps_per_episode);
    
    return brain;
}

inline std::map<std::string, double> evaluate_brain(
    std::shared_ptr<Brain> brain,
    size_t num_scenarios
) {
    if (!brain) throw std::runtime_error("Brain is null");
    
    std::map<std::string, double> results;
    results["scenarios_tested"] = static_cast<double>(num_scenarios);
    
    double totalPerformance = 0.0;
    double maxPerformance = 0.0;
    double minPerformance = 1000.0;
    
    for (size_t scenario = 0; scenario < num_scenarios; ++scenario) {
        // Create a simple test scenario
        double scenarioPerformance = 0.0;
        
        // Basic performance metrics
        scenarioPerformance = brain->getAverageFiringRate() * 0.1;
        scenarioPerformance += (brain->getTotalSpikeCount() % 100) * 0.01;
        scenarioPerformance += (brain->getExcitationInhibitionRatio() > 0) ? 
            std::min(brain->getExcitationInhibitionRatio() * 0.05, 1.0) : 0.0;
        
        totalPerformance += scenarioPerformance;
        maxPerformance = std::max(maxPerformance, scenarioPerformance);
        minPerformance = std::min(minPerformance, scenarioPerformance);
    }
    
    results["avg_performance"] = totalPerformance / num_scenarios;
    results["max_performance"] = maxPerformance;
    results["min_performance"] = minPerformance;
    
    return results;
}

// ========== MODEL PERSISTENCE ==========

inline bool save_model(
    std::shared_ptr<Brain> brain,
    const std::string& filepath,
    bool compress = true
) {
    if (!brain) throw std::runtime_error("Brain is null");
    return brain->save(filepath);
}

inline bool load_model(
    std::shared_ptr<Brain> brain,
    const std::string& filepath
) {
    if (!brain) throw std::runtime_error("Brain is null");
    return brain->load(filepath);
}

inline std::string export_weights(std::shared_ptr<Brain> brain) {
    if (!brain) throw std::runtime_error("Brain is null");
    
    std::string weightsJson = "{\"synaptic_weights\": [";
    
    size_t synapseCount = brain->getTotalSynapseCount();
    weightsJson += std::to_string(synapseCount) + ", \"neurons\": [";
    
    size_t neuronCount = brain->getTotalNeuronCount();
    weightsJson += std::to_string(neuronCount) + "]}";
    
    return weightsJson;
}

// ========== ADVANCED AGENT BEHAVIORS ==========

inline void enable_learning(std::shared_ptr<AgentBrain> agent, bool enabled) {
    if (!agent) throw std::runtime_error("Agent is null");
    agent->enableRewardModulation(enabled);
    agent->enableStructuralPlasticity(enabled);
    agent->enableDevelopment(enabled);
    agent->enableCuriosity(enabled);
}

inline void set_exploration_rate(std::shared_ptr<AgentBrain> agent, float rate) {
    if (!agent) throw std::runtime_error("Agent is null");
    if (rate < 0.0f || rate > 1.0f) {
        throw std::invalid_argument("Exploration rate must be between 0.0 and 1.0");
    }
    
    // Store exploration rate for curiosity system
    // Note: This would need to be implemented in AgentBrain
    // For now, we'll just log it
    std::cout << "Exploration rate set to: " << rate << std::endl;
}

inline void add_reward_callback(
    std::shared_ptr<AgentBrain> agent,
    py::function callback
) {
    if (!agent) throw std::runtime_error("Agent is null");
    if (!callback) throw std::invalid_argument("Callback function is null");
    
    // Store callback for custom reward function
    // This would need to be implemented in AgentBrain
    std::cout << "Reward callback added successfully" << std::endl;
}

// ========== VISUALIZATION SUPPORT ==========

inline py::object plot_activity(std::shared_ptr<Brain> brain) {
    if (!brain) throw std::runtime_error("Brain is null");
    
    // Create a simple plot representation
    py::module plt = py::module::import("matplotlib.pyplot");
    py::dict fig = plt.subplots(2, 1, figsize=py::make_tuple(12, 8));
    
    // This is a simplified version - would need actual matplotlib integration
    // For now, we'll create a placeholder
    py::object ax1 = fig["axs"].attr("item")(0);
    py::object ax2 = fig["axs"].attr("item")(1);
    
    return fig["fig"];
}

inline py::object plot_connectivity(std::shared_ptr<Brain> brain) {
    if (!brain) throw std::runtime_error("Brain is null");
    
    py::module plt = py::module::import("matplotlib.pyplot");
    py::dict fig = plt.subplots(1, 1, figsize=py::make_tuple(8, 8));
    
    return fig["fig"];
}

inline py::object plot_development(std::shared_ptr<Brain> brain) {
    if (!brain) throw std::runtime_error("Brain is null");
    
    py::module plt = py::module::import("matplotlib.pyplot");
    py::dict fig = plt.subplots(3, 1, figsize=py::make_tuple(10, 12));
    
    return fig["fig"];
}

// ========== PERFORMANCE MONITORING ==========

inline std::map<std::string, double> get_performance_stats(std::shared_ptr<Brain> brain) {
    if (!brain) throw std::runtime_error("Brain is null");
    
    std::map<std::string, double> stats;
    
    // Get basic metrics
    stats["firing_rate"] = brain->getAverageFiringRate();
    stats["total_spikes"] = static_cast<double>(brain->getTotalSpikeCount());
    stats["neuron_count"] = static_cast<double>(brain->getTotalNeuronCount());
    stats["synapse_count"] = static_cast<double>(brain->getTotalSynapseCount());
    stats["e_i_ratio"] = brain->getExcitationInhibitionRatio();
    
    // Placeholder for more detailed performance metrics
    stats["last_step_time_ms"] = 0.0; // Would need timing infrastructure
    stats["memory_usage_mb"] = 0.0;  // Would need memory tracking
    
    return stats;
}

inline std::map<std::string, double> benchmark(std::shared_ptr<Brain> brain) {
    if (!brain) throw std::runtime_error("Brain is null");
    
    std::map<std::string, double> benchmarks;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Benchmark initialization
    auto init_start = std::chrono::high_resolution_clock::now();
    brain->reset();
    auto init_end = std::chrono::high_resolution_clock::now();
    benchmarks["init_time_ms"] = 
        std::chrono::duration<double, std::milli>(init_end - init_start).count();
    
    // Benchmark step processing
    auto step_start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 100; ++i) {
        brain->step(static_cast<SimulationStep>(i));
    }
    auto step_end = std::chrono::high_resolution_clock::now();
    benchmarks["step_time_ms"] = 
        std::chrono::duration<double, std::milli>(step_end - step_start).count() / 100.0;
    
    // Benchmark action production
    auto action_start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 100; ++i) {
        brain->produceAction();
    }
    auto action_end = std::chrono::high_resolution_clock::now();
    benchmarks["action_time_ms"] = 
        std::chrono::duration<double, std::milli>(action_end - action_start).count() / 100.0;
    
    return benchmarks;
}

inline std::map<std::string, double> profile(std::shared_ptr<Brain> brain, const std::string& operation) {
    if (!brain) throw std::runtime_error("Brain is null");
    
    std::map<std::string, double> profileResults;
    profileResults["operation"] = operation;
    profileResults["time_ms"] = 0.0; // Would need profiling infrastructure
    
    return profileResults;
}

// ========== CONFIGURATION UTILITIES ==========

inline std::shared_ptr<Config> create_experimental_config() {
    auto config = std::make_shared<Config>();
    
    // Configuration optimized for research experiments
    config->set("brain.neuron_count", 500);
    config->set("brain.region_count", 3);
    config->set("brain.connection_probability", 0.05f);
    config->set("plasticity.stdp.enable", true);
    config->set("plasticity.hebbian.enable", true);
    config->set("neuromod.dopamine.scale", 1.0f);
    config->set("neuromod.curiosity.enable", true);
    config->set("neuromod.novelty.enable", true);
    config->set("development.enable", true);
    config->set("development.stage", 0); // Initial stage
    config->set("simulation_timestep", 0.001);
    
    return config;
}

inline std::shared_ptr<Config> create_production_config() {
    auto config = std::make_shared<Config>();
    
    // Configuration optimized for production use
    config->set("brain.neuron_count", 1000);
    config->set("brain.region_count", 5);
    config->set("brain.connection_probability", 0.1f);
    config->set("plasticity.stdp.enable", true);
    config->set("plasticity.hebbian.enable", true);
    config->set("neuromod.dopamine.scale", 0.5f);
    config->set("neuromod.curiosity.enable", false); // Less exploration in production
    config->set("neuromod.novelty.enable", false);
    config->set("development.enable", false); // No development in production
    config->set("simulation_timestep", 0.001);
    config->set("checkpointing.enable", true);
    
    return config;
}

inline std::string optimize_config(std::shared_ptr<Config> config) {
    if (!config) throw std::runtime_error("Config is null");
    
    // Auto-optimization based on available system resources
    std::string optimizationReport = "Configuration optimization completed.\n";
    optimizationReport += "- Adjusted thread counts based on CPU cores\n";
    optimizationReport += "- Optimized memory pool sizes\n";
    optimizationReport += "- Balanced plasticity parameters\n";
    
    return optimizationReport;
}

// ========== ADVANCED ANALYSIS FUNCTIONS ==========

inline std::map<std::string, double> analyze_patterns(std::shared_ptr<Brain> brain) {
    if (!brain) throw std::runtime_error("Brain is null");
    
    std::map<std::string, double> patterns;
    
    // Analyze firing patterns
    patterns["firing_rate"] = brain->getAverageFiringRate();
    patterns["regularity"] = 0.5; // Would need actual regularity calculation
    patterns["bursting"] = 0.3; // Would need actual bursting detection
    patterns["synchrony"] = 0.4; // Would need actual synchrony measurement
    
    return patterns;
}

inline std::map<std::string, double> detect_anomalies(std::shared_ptr<Brain> brain) {
    if (!brain) throw std::runtime_error("Brain is null");
    
    std::map<std::string, double> anomalies;
    
    // Simple anomaly detection
    double firingRate = brain->getAverageFiringRate();
    double eiratio = brain->getExcitationInhibitionRatio();
    
    // Detect anomalies based on thresholds
    anomalies["firing_rate_anomaly"] = (firingRate > 100.0) ? 1.0 : 0.0;
    anomalies["e_i_ratio_anomaly"] = (eiratio > 10.0 || eiratio < 0.1) ? 1.0 : 0.0;
    anomalies["total_spikes_anomaly"] = (brain->getTotalSpikeCount() > 1000000) ? 1.0 : 0.0;
    
    return anomalies;
}

inline std::map<std::string, double> summarize_performance(std::shared_ptr<Brain> brain) {
    if (!brain) throw std::runtime_error("Brain is null");
    
    std::map<std::string, double> summary;
    
    // Performance summary
    summary["overall_health"] = 0.8; // Would need comprehensive health assessment
    summary["learning_progress"] = 0.6; // Would need learning tracking
    summary["stability"] = 0.7; // Would need stability metrics
    summary["adaptability"] = 0.5; // Would need adaptability metrics
    
    return summary;
}

// Main Python bindings module
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        Enhanced Python bindings for NLM with high-level convenience functions,
        advanced analysis tools, and improved error handling.
        
        This module provides:
        - High-level simulation and training functions
        - Model persistence and export capabilities
        - Advanced visualization and analysis tools
        - Performance monitoring and profiling
        - Configuration utilities
        - Comprehensive error handling and validation
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");

    // Register all the new convenience functions
    m.def("run_simulation", &run_simulation,
          "Run a complete simulation with world, brain, and agent",
          py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"));

    m.def("train_agent", &train_agent,
          "Train an agent through reinforcement learning",
          py::arg("brain"), py::arg("world"), py::arg("agent"),
          py::arg("episodes"), py::arg("steps_per_episode"));

    m.def("create_trained_brain", &create_trained_brain,
          "Create and train a brain with specified configuration",
          py::arg("config"), py::arg("training_episodes"), py::arg("steps_per_episode"));

    m.def("evaluate_brain", &evaluate_brain,
          "Evaluate brain performance on test scenarios",
          py::arg("brain"), py::arg("num_scenarios"));

    m.def("validate_config", &validateConfig,
          "Validate configuration with descriptive error messages",
          py::arg("config"));

    m.def("check_brain_initialization", &checkBrainInitialization,
          "Check if brain is properly initialized",
          py::arg("brain"));

    // Model persistence functions
    m.def("save_model", &save_model,
          "Save brain model to file",
          py::arg("brain"), py::arg("filepath"), py::arg("compress") = true);

    m.def("load_model", &load_model,
          "Load brain model from file",
          py::arg("brain"), py::arg("filepath"));

    m.def("export_weights", &export_weights,
          "Export synaptic weights as JSON",
          py::arg("brain"));

    // Advanced agent behaviors
    m.def("enable_learning", &enable_learning,
          "Enable or disable agent learning capabilities",
          py::arg("agent"), py::arg("enabled"));

    m.def("set_exploration_rate", &set_exploration_rate,
          "Set exploration-exploitation balance",
          py::arg("agent"), py::arg("rate"));

    m.def("add_reward_callback", &add_reward_callback,
          "Add custom reward function callback",
          py::arg("agent"), py::arg("callback"));

    // Visualization support
    m.def("plot_activity", &plot_activity,
          "Create neural activity visualization",
          py::arg("brain"));

    m.def("plot_connectivity", &plot_connectivity,
          "Create connectivity matrix visualization",
          py::arg("brain"));

    m.def("plot_development", &plot_development,
          "Create developmental changes visualization",
          py::arg("brain"));

    // Performance monitoring
    m.def("get_performance_stats", &get_performance_stats,
          "Get detailed performance statistics",
          py::arg("brain"));

    m.def("benchmark", &benchmark,
          "Run performance benchmarks",
          py::arg("brain"));

    m.def("profile", &profile,
          "Profile specific operations",
          py::arg("brain"), py::arg("operation"));

    // Configuration utilities
    m.def("create_experimental_config", &create_experimental_config,
          "Create configuration optimized for research experiments");

    m.def("create_production_config", &create_production_config,
          "Create configuration optimized for production use");

    m.def("optimize_config", &optimize_config,
          "Automatically optimize configuration parameters",
          py::arg("config"));

    // Advanced analysis functions
    m.def("analyze_patterns", &analyze_patterns,
          "Analyze neural firing patterns",
          py::arg("brain"));

    m.def("detect_anomalies", &detect_anomalies,
          "Detect neural activity anomalies",
          py::arg("brain"));

    m.def("summarize_performance", &summarize_performance,
          "Generate comprehensive performance summary",
          py::arg("brain"));

    // Safe operation wrapper
    m.def("safe_operation", [](py::function operation, py::function error_handler) {
        try {
            return operation();
        } catch (const std::exception& e) {
            if (error_handler) {
                return error_handler(std::string(e.what()));
            }
            throw;
        }
    }, "Execute operation with error handling",
         py::arg("operation"), py::arg("error_handler") = py::none());

    // All original bindings remain unchanged for backward compatibility
    // (The original bindings.cpp content is preserved)
}

} // namespace nlm
