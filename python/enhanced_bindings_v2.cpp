#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>

// Include all NLM headers
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

// ==============================================================================
// ADVANCED CONVENIENCE FUNCTIONS
// ==============================================================================

/**
 * Create a neural network with optimal configuration for the given task
 * 
 * @param task_type Type of task ("classification", "regression", "reinforcement", "language")
 * @param input_dim Input dimensionality
 * @param output_dim Output dimensionality
 * @param constraints Optional constraints on configuration
 * @return Configured and initialized Brain
 */nlm::shared_ptr<Brain> createOptimizedBrain(
    const std::string& taskType,
    size_t inputDim,
    size_t outputDim,
    const nlm::map<std::string, nlm::variant<int64_t, double, std::string, bool>>& constraints = {}) {
    
    auto config = nlm::createDefaultConfig();
    
    // Set basic configuration
    config->set("neuron_count", static_cast<int64_t>(std::min(size_t(1000), inputDim * outputDim * 2)));
    config->set("region_count", static_cast<int64_t>(1));
    config->set("connection_probability", 0.05f + 0.15f * (inputDim / 100.0f));
    config->set("stdp_ltp_weight", 0.01f);
    config->set("stdp_ltd_weight", 0.012f);
    config->set("synaptogenesis_rate", 0.00005f);
    config->set("pruning_rate", 0.000001f);
    
    // Task-specific adjustments
    if (taskType == "classification") {
        config->set("neuron_count", static_cast<int64_t>(std::min<size_t>(2000, config->getOr<int64_t>("neuron_count", 1000))));
    } else if (taskType == "reinforcement") {
        config->set("neuron_count", static_cast<int64_t>(config->getOr<int64_t>("neuron_count", 1000) * 1.5));
        config->set("stdp_ltp_weight", 0.02f);
    } else if (taskType == "language") {
        config->set("neuron_count", static_cast<int64_t>(config->getOr<int64_t>("neuron_count", 1000) * 2));
        config->set("connection_probability", 0.15f);
    }
    
    // Apply custom constraints
    for (const auto& pair : constraints) {
        const std::string& key = pair.first;
        const nlm::variant<int64_t, double, std::string, bool>& value = pair.second;
        
        nlm::visit([&](auto&& val) {
            using T = nlm::decay_t<decltype(val)>;
            if constexpr (nlm::is_same_v<T, int64_t>) {
                config->set(key, val, nlm::ConfigSource::Runtime);
            } else if constexpr (nlm::is_same_v<T, double>) {
                config->set(key, val, nlm::ConfigSource::Runtime);
            } else if constexpr (nlm::is_same_v<T, std::string>) {
                config->set(key, val, nlm::ConfigSource::Runtime);
            } else if constexpr (nlm::is_same_v<T, bool>) {
                config->set(key, val, nlm::ConfigSource::Runtime);
            }
        }, value);
    }
    
    auto brain = nlm::createBrain(config);
    brain->initialize();
    return brain;
}

/**
 * Create a simulation environment with realistic physics and dynamics
 * 
 * @param env_type Type of environment ("grid_world", "continuous", "hierarchical")
 * @param size Environment size
 * @param obstacles Optional obstacles
 * @return Configured and initialized SimpleWorld
 */nnlm::shared_ptr<SimpleWorld> createRealisticWorld(
    const std::string& envType,
    const nlm::tuple<size_t, size_t>& size,
    const nlm::vector<nnlm::tuple<float, float, float, float>>& obstacles = {}) {
    
    auto world = nlm::createSimpleWorld();
    
    size_t width = std::get<0>(size);
    size_t height = std::get<1>(size);
    world->configure(width, height, std::min(width, 16), std::min(height, 16));
    
    // Add obstacles if provided
    for (const auto& obstacle : obstacles) {
        float x = std::get<0>(obstacle);
        float y = std::get<1>(obstacle);
        float radius = std::get<2>(obstacle);
        WorldObjectType type = static_cast<WorldObjectType>(std::get<3>(obstacle));
        
        nlm::WorldObject obj;
        obj.x = x;
        obj.y = y;
        obj.radius = radius;
        obj.type = type;
        obj.active = true;
        
        world->addObject(obj);
    }
    
    // Environment-specific configuration
    if (envType == "continuous") {
        world->setMaxEnergy(100.0f);
        world->setEnergyDecayRate(0.1f);
    } else if (envType == "hierarchical") {
        world->setMaxEnergy(200.0f);
        world->setEnergyDecayRate(0.05f);
    }
    
    return world;
}

/**
 * Advanced simulation runner with comprehensive monitoring
 * 
 * @param brain The brain to simulate
 * @param world The environment
 * @param agent The agent interface
 * @param maxSteps Maximum simulation steps
 * @param monitorCallback Callback for monitoring simulation metrics
 * @param renderCallback Optional rendering callback
 * @return Detailed simulation report
 */nlm::struct SimulationReport {
    float totalReward;
    float avgFiringRate;
    float maxFiringRate;
    size_t stepsCompleted;
    size_t actionCount;
    bool success;
    std::string errorMessage;
    nlm::map<std::string, float> metrics;
    
    SimulationReport() : totalReward(0.0f), avgFiringRate(0.0f), maxFiringRate(0.0f),
                        stepsCompleted(0), actionCount(0), success(true) {}
};

SimulationReport advancedSimulationRun(
    nlm::shared_ptr<Brain> brain,
    nlm::shared_ptr<SimpleWorld> world,
    nlm::shared_ptr<AgentBrain> agent,
    size_t maxSteps = 1000,
    nlm::function<void(const SimulationReport&, size_t)> monitorCallback = nullptr,
    nlm::function<void(const nlm::map<std::string, float>&, size_t)> renderCallback = nullptr) {
    
    SimulationReport report;
    float totalFiringRate = 0.0f;
    size_t firingCount = 0;
    
    try {
        for (size_t step = 0; step < maxSteps; ++step) {
            // Get sensory percept
            nlm::SensoryPercept percept = world->getSensoryPercept(brain->getRegions()[0].get());
            
            // Process input and get action
            agent->processSensoryInput(percept);
            nlm::MotorCommand cmd = agent->decodeMotorCommand();
            
            // Apply action and get reward
            world->applyAction(brain->getRegions()[0].get(), cmd);
            float reward = world->computeReward(brain->getRegions()[0].get());
            
            // Update brain and agent
            brain->step(step, step * 0.001);
            agent->applyRewardModulation(reward, 0.0f);
            agent->updateDevelopment(0.001);
            
            // Update statistics
            report.totalReward += reward;
            totalFiringRate += brain->getAverageFiringRate();
            if (brain->getFiringNeuronCount() > 0) firingCount++;
            report.actionCount++;
            
            // Monitor simulation
            if (monitorCallback) {
                report.metrics.clear();
                report.metrics["reward_per_step"] = reward;
                report.metrics["total_reward"] = report.totalReward;
                report.metrics["avg_firing_rate"] = brain->getAverageFiringRate();
                report.metrics["active_neurons"] = static_cast<float>(brain->getActiveNeuronCount());
                monitorCallback(report, step);
            }
            
            // Optional rendering
            if (renderCallback) {
                nlm::map<std::string, float> currentMetrics = {
                    {"step", static_cast<float>(step)},
                    {"reward", reward},
                    {"total_reward", report.totalReward},
                    {"firing_rate", brain->getAverageFiringRate()},
                    {"active_neurons", static_cast<float>(brain->getActiveNeuronCount())}
                };
                renderCallback(currentMetrics, step);
            }
            
            report.stepsCompleted = step + 1;
        }
        
        // Calculate final statistics
        report.avgFiringRate = totalFiringRate / maxSteps;
        report.maxFiringRate = brain->getAverageFiringRate();
        
    } catch (const std::exception& e) {
        report.success = false;
        report.errorMessage = std::string("Simulation error: ") + e.what();
    }
    
    return report;
}

/**
 * Create a meta-simulation with multiple configurations
 * 
 * Runs multiple simulations with different configurations to compare results.
 * 
 * @param configs List of configurations to test
 * @param worldTemplate Template world configuration
 * @param maxSteps Maximum simulation steps per run
 * @return Map of configuration ID to simulation results
 */nnlm::map<std::string, SimulationReport> runMetaSimulation(
    const nlm::vector<nlm::tuple<std::string, nlm::shared_ptr<Config>>>& configs,
    nlm::shared_ptr<SimpleWorld> worldTemplate,
    size_t maxSteps = 1000) {
    
    nlm::map<std::string, SimulationReport> results;
    
    for (const auto& configPair : configs) {
        std::string configId = std::get<0>(configPair);
        nlm::shared_ptr<Config> config = std::get<1>(configPair);
        
        // Create brain with this configuration
        nlm::shared_ptr<Brain> brain = nlm::createBrain(config);
        brain->initialize();
        
        // Create agent
        nlm::shared_ptr<AgentBrain> agent = nlm::createAgentBrain(brain);
        agent->initialize(*worldTemplate);
        
        // Run simulation
        SimulationReport report = advancedSimulationRun(brain, worldTemplate, agent, maxSteps);
        results[configId] = report;
    }
    
    return results;
}

/**
 * Performance profile analyzer
 * 
 * Analyzes performance characteristics of the brain simulation
 * and provides optimization recommendations.
 * 
 * @param brain The brain to analyze
 * @param world The environment
 * @param agent The agent interface
 * @param testScenarios List of test scenarios
 * @return Performance analysis report
 */nnlm::struct PerformanceAnalysis {
    float throughputNeuronsPerSecond;
    float memoryEfficiency;
    float balanceScore;
    float stabilityScore;
    nlm::map<std::string, float> benchmarks;
    nlm::vector<std::string> recommendations;
};

PerformanceAnalysis analyzePerformance(
    nlm::shared_ptr<Brain> brain,
    nlm::shared_ptr<SimpleWorld> world,
    nlm::shared_ptr<AgentBrain> agent,
    const nlm::vector<std::string>& testScenarios = {}) {
    
    PerformanceAnalysis analysis;
    
    // Run performance benchmarks
    for (const auto& scenario : testScenarios) {
        if (scenario == "throughput") {
            // Measure neuron throughput
            auto start = std::chrono::high_resolution_clock::now();
            for (size_t i = 0; i < 100; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Internal, 1.0f);
                brain->step(i, i * 0.001);
            }
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            
            float neuronsPerSecond = brain->getTotalNeuronCount() * 100 * 1000000.0f / duration.count();
            analysis.benchmarks["throughput_neurons_per_sec"] = neuronsPerSecond;
        } else if (scenario == "memory") {
            // Estimate memory usage
            analysis.benchmarks["estimated_memory_mb"] = 
                static_cast<float>(brain->getTotalNeuronCount() * sizeof(Neuron) / (1024 * 1024));
        } else if (scenario == "stability") {
            // Measure stability over time
            float variance = 0.0f;
            float mean = 0.0f;
            for (size_t i = 0; i < 50; ++i) {
                brain->step(i, i * 0.001);
                mean += brain->getAverageFiringRate();
            }
            mean /= 50;
            
            for (size_t i = 0; i < 50; ++i) {
                brain->step(i, i * 0.001);
                float diff = brain->getAverageFiringRate() - mean;
                variance += diff * diff;
            }
            variance /= 50;
            analysis.stabilityScore = std::max(0.0f, 1.0f - variance / 10.0f);
        }
    }
    
    // Calculate derived metrics
    if (analysis.benchmarks.find("throughput_neurons_per_sec") != analysis.benchmarks.end()) {
        analysis.throughputNeuronsPerSecond = analysis.benchmarks["throughput_neurons_per_sec"];
    }
    
    // Default estimates if not measured
    if (analysis.benchmarks.find("estimated_memory_mb") == analysis.benchmarks.end()) {
        analysis.benchmarks["estimated_memory_mb"] = static_cast<float>(brain->getTotalNeuronCount() * 0.001); // Rough estimate
    }
    
    // Calculate balance score (E/I ratio)
    float eiRatio = brain->getExcitationInhibitionRatio();
    analysis.balanceScore = std::min(1.0f, eiRatio / 10.0f);
    
    // Generate recommendations
    if (analysis.throughputNeuronsPerSecond < 1000.0f) {
        analysis.recommendations.push_back("Consider increasing simulation timestep for better performance");
    }
    if (analysis.balanceScore < 0.5f) {
        analysis.recommendations.push_back("Improve excitation/inhibition balance for stability");
    }
    if (analysis.benchmarks.find("estimated_memory_mb") != analysis.benchmarks.end() && 
        analysis.benchmarks["estimated_memory_mb"] > 500.0f) {
        analysis.recommendations.push_back("Large memory footprint - consider optimizing data structures");
    }
    
    if (analysis.recommendations.empty()) {
        analysis.recommendations.push_back("Performance is within acceptable ranges");
    }
    
    return analysis;
}

/**
 * Configuration validator
 * 
 * Validates configuration for consistency and compatibility
 * 
 * @param config The configuration to validate
 * @return Validation result with errors and warnings
 */nnlm::struct ConfigurationValidation {
    bool valid;
    nlm::vector<std::string> errors;
    nlm::vector<std::string> warnings;
    nlm::map<std::string, std::string> details;
    
    ConfigurationValidation() : valid(true) {}
};

ConfigurationValidation validateConfiguration(nlm::shared_ptr<Config> config) {
    ConfigurationValidation validation;
    
    // Check for required keys
    std::vector<std::string> requiredKeys = {"neuron_count", "region_count", "connection_probability"};
    for (const auto& key : requiredKeys) {
        if (!config->has(key)) {
            validation.errors.push_back("Missing required configuration key: " + key);
            validation.valid = false;
        }
    }
    
    // Validate value ranges
    auto neuronCount = config->get<int64_t>("neuron_count");
    if (neuronCount && (*neuronCount <= 0 || *neuronCount > 100000)) {
        validation.warnings.push_back("Neuron count outside typical range (0-100,000)");
    }
    
    auto regionCount = config->get<int64_t>("region_count");
    if (regionCount && (*regionCount <= 0 || *regionCount > 1000)) {
        validation.warnings.push_back("Region count outside typical range (0-1,000)");
    }
    
    auto connProb = config->get<double>("connection_probability");
    if (connProb && (*connProb < 0.0 || *connProb > 1.0)) {
        validation.errors.push_back("Connection probability must be between 0.0 and 1.0");
        validation.valid = false;
    }
    
    // Check for incompatible combinations
    auto stdpLTP = config->get<double>("stdp_ltp_weight");
    auto stdpLTD = config->get<double>("stdp_ltd_weight");
    if (stdpLTP && stdpLTD && *stdpLTP <= 0.0 && *stdpLTD <= 0.0) {
        validation.warnings.push_back("Both STDP weights are non-positive - LTP/LTD may not work correctly");
    }
    
    // Store details
    validation.details["neuron_count"] = neuronCount ? std::to_string(*neuronCount) : "not set";
    validation.details["region_count"] = regionCount ? std::to_string(*regionCount) : "not set";
    validation.details["connection_probability"] = connProb ? std::to_string(*connProb) : "not set";
    
    return validation;
}

/**
 * Create a simulation experiment with full reproducibility support
 * 
 * @param experimentName Name of the experiment
 * @param config Configuration
 * @param seed Random seed for reproducibility
 * @return Configured experiment setup
 */nnlm::struct ExperimentSetup {
    std::string name;
    nlm::shared_ptr<Config> config;
    size_t seed;
    nlm::vector<nlm::tuple<size_t, nlm::shared_ptr<Brain>, nlm::shared_ptr<SimpleWorld>, nlm::shared_ptr<AgentBrain>>> agents;
    
    ExperimentSetup(std::string n, nlm::shared_ptr<Config> c, size_t s = 42) 
        : name(n), config(c), seed(s) {}
};

ExperimentSetup createExperiment(
    const std::string& name,
    nlm::shared_ptr<Config> config,
    size_t seed = 42) {
    
    ExperimentSetup exp(name, config, seed);
    
    // Create brain with reproducibility
    nlm::mt19937 rng(seed);
    config->set("random_seed", static_cast<int64_t>(seed), nlm::ConfigSource::Runtime);
    
    auto brain = nlm::createBrain(config);
    brain->initialize();
    
    // Create world
    auto world = nlm::createSimpleWorld();
    world->configure(16, 16, 8, 8);
    
    // Create agent
    auto agent = nlm::createAgentBrain(brain);
    agent->initialize(*world);
    
    exp.agents.push_back(nlm::make_tuple(size_t(0), brain, world, agent));
    
    return exp;
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM Python API with Advanced Convenience Functions
        
        This module provides advanced convenience functions for:
        
        - Optimal brain configuration for different task types
        - Realistic environment simulation
        - Advanced simulation monitoring and analysis
        - Meta-simulation and comparative experiments
        - Performance profiling and optimization
        - Configuration validation
        - Reproducible experiment setup
        
        Key features:
        - Task-specific brain optimization
        - Comprehensive environment support
        - Real-time monitoring capabilities
        - Performance analysis and recommendations
        - Configuration validation and error checking
        - Reproducible experimental setup
        
        Basic usage:
            import pynlm
            
            # Create optimized brain for a specific task
            brain = pynlm.create_optimized_brain("reinforcement", 10, 5)
            
            # Create realistic environment
            world = pynlm.create_realistic_world("grid_world", (20, 20))
            
            # Run advanced simulation with monitoring
            report = pynlm.advanced_simulation_run(brain, world, agent, max_steps=5000)
            
            # Analyze performance
            perf = pynlm.analyze_performance(brain, world, agent, ["throughput", "memory", "stability"])
            
            # Validate configuration
            validation = pynlm.validate_configuration(config)
    )pbdoc";
    
    // Register exception types
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");
    py::register_exception<std::out_of_range>(m, "OutOfRangeError");
    
    // Helper function to create default config
    m.def("create_default_config", []() -> nlm::shared_ptr<Config> {
        return nlm::createDefaultConfig();
    }, "Create a default configuration");
    
    // Advanced convenience functions
    m.def("create_optimized_brain", &createOptimizedBrain,
          py::arg("task_type"), py::arg("input_dim"), py::arg("output_dim"),
          py::arg("constraints") = nlm::map<std::string, nlm::variant<int64_t, double, std::string, bool>>(),
          "Create a brain optimized for a specific task type");
    
    m.def("create_realistic_world", &createRealisticWorld,
          py::arg("env_type"), py::arg("size"), py::arg("obstacles") = nlm::vector<nlm::tuple<float, float, float, float>>(),
          "Create a realistic environment with configurable physics");
    
    m.def("advanced_simulation_run", &advancedSimulationRun,
          py::arg("brain"), py::arg("world"), py::arg("agent"),
          py::arg("max_steps") = 1000,
          py::arg("monitor_callback") = nullptr,
          py::arg("render_callback") = nullptr,
          "Run an advanced simulation with comprehensive monitoring");
    
    m.def("run_meta_simulation", &runMetaSimulation,
          py::arg("configs"), py::arg("world_template"),
          py::arg("max_steps") = 1000,
          "Run multiple simulations with different configurations");
    
    m.def("analyze_performance", &analyzePerformance,
          py::arg("brain"), py::arg("world"), py::arg("agent"),
          py::arg("test_scenarios") = nlm::vector<std::string>(),
          "Analyze simulation performance and provide optimization recommendations");
    
    m.def("validate_configuration", &validateConfiguration,
          py::arg("config"), "Validate configuration for errors and warnings");
    
    m.def("create_experiment", &createExperiment,
          py::arg("name"), py::arg("config"), py::arg("seed") = 42,
          "Create a reproducible experimental setup");
    
    // Result types
    py::class_<SimulationResult>(m, "SimulationResult")
        .def(py::init<>())
        .def_readwrite("total_reward", &SimulationResult::totalReward)
        .def_readwrite("avg_firing_rate", &SimulationResult::avgFiringRate)
        .def_readwrite("max_firing_rate", &SimulationResult::maxFiringRate)
        .def_readwrite("steps_completed", &SimulationResult::stepsCompleted)
        .def_readwrite("action_count", &SimulationResult::actionCount)
        .def_readwrite("success", &SimulationResult::success)
        .def_readwrite("error_message", &SimulationResult::errorMessage)
        .def("__repr__", [](const SimulationResult& r) {
            return "SimulationResult(steps=" + std::to_string(r.stepsCompleted) +
                   ", reward=" + std::to_string(r.totalReward) +
                   ", success=" + std::to_string(r.success) + ")";
        });
    
    py::class_<PerformanceAnalysis>(m, "PerformanceAnalysis")
        .def(py::init<>())
        .def_readwrite("throughput_neurons_per_second", &PerformanceAnalysis::throughputNeuronsPerSecond)
        .def_readwrite("memory_efficiency", &PerformanceAnalysis::memoryEfficiency)
        .def_readwrite("balance_score", &PerformanceAnalysis::balanceScore)
        .def_readwrite("stability_score", &PerformanceAnalysis::stabilityScore)
        .def_readwrite("benchmarks", &PerformanceAnalysis::benchmarks)
        .def_readwrite("recommendations", &PerformanceAnalysis::recommendations);
    
    py::class_<ConfigurationValidation>(m, "ConfigurationValidation")
        .def(py::init<>())
        .def_readwrite("valid", &ConfigurationValidation::valid)
        .def_readwrite("errors", &ConfigurationValidation::errors)
        .def_readwrite("warnings", &ConfigurationValidation::warnings)
        .def_readwrite("details", &ConfigurationValidation::details);
    
    py::class_<ExperimentSetup>(m, "ExperimentSetup")
        .def(py::init<>())
        .def_readwrite("name", &ExperimentSetup::name)
        .def_readwrite("config", &ExperimentSetup::config)
        .def_readwrite("seed", &ExperimentSetup::seed);
    
    // Advanced type definitions
    py::class_<nlm::tuple<size_t, nlm::shared_ptr<Brain>, nlm::shared_ptr<SimpleWorld>, nlm::shared_ptr<AgentBrain>>>(m, "AgentSimulation");
    
    // Additional constants
    m.attr("SUPPORTED_TASK_TYPES") = nlm::map<std::string, std::string>({{
        {"classification", "Neural classification and pattern recognition"},
        {"regression", "Neural regression and prediction"},
        {"reinforcement", "Reinforcement learning and decision making"},
        {"language", "Natural language processing"},
        {"control", "Control systems and robotics"}
    }});
    
    m.attr("SUPPORTED_ENVIRONMENT_TYPES") = nlm::map<std::string, std::string>({{
        {"grid_world", "2D grid-based environment"},
        {"continuous", "Continuous state space"},
        {"hierarchical", "Multi-level environment"}
    }});
    
    m.attr("EXPERIMENT_VERSION") = "3.0.0";
    
    // Documentation
    m.doc() += "\n\nFor advanced documentation and examples, visit the NLM documentation portal.";
}

} // namespace nlm
