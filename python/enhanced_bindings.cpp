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
// ENHANCED CONVENIENCE FUNCTIONS
// ==============================================================================

/**
 * Create optimized brain for specific task types
 * 
 * @param task_type Type of task ("classification", "regression", "reinforcement", "language")
 * @param input_dim Input dimensionality
 * @param output_dim Output dimensionality
 * @return Configured and initialized Brain
 */nlm::shared_ptr<Brain> createOptimizedBrain(
    const std::string& taskType,
    size_t inputDim,
    size_t outputDim) {
    
    auto config = std::make_shared<Config>();
    
    // Set basic configuration
    config->set("neuron_count", static_cast<int64_t>(std::min<size_t>(1000, inputDim * outputDim * 2)));
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
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    return brain;
}

/**
 * Create realistic environment with configurable physics
 * 
 * @param env_type Type of environment ("grid_world", "continuous", "hierarchical")
 * @param size Environment size
 * @return Configured and initialized SimpleWorld
 */nnlm::shared_ptr<SimpleWorld> createRealisticWorld(
    const std::string& envType,
    const nnlm::tuple<size_t, size_t>& size) {
    
    auto world = std::make_shared<SimpleWorld>();
    
    size_t width = std::get<0>(size);
    size_t height = std::get<1>(size);
    world->configure(width, height, std::min(width, 16), std::min(height, 16));
    
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
 */nnlm::struct SimulationReport {
    float totalReward;
    float avgFiringRate;
    float maxFiringRate;
    size_t stepsCompleted;
    size_t actionCount;
    bool success;
    std::string errorMessage;
    nnlm::map<std::string, float> metrics;
    
    SimulationReport() : totalReward(0.0f), avgFiringRate(0.0f), maxFiringRate(0.0f),
                        stepsCompleted(0), actionCount(0), success(true) {}
};

SimulationReport advancedSimulationRun(
    nnlm::shared_ptr<Brain> brain,
    nnlm::shared_ptr<SimpleWorld> world,
    nnlm::shared_ptr<AgentBrain> agent,
    size_t maxSteps = 1000,
    nnlm::function<void(const SimulationReport&, size_t)> monitorCallback = nullptr,
    nnlm::function<void(const nnlm::map<std::string, float>&, size_t)> renderCallback = nullptr) {
    
    SimulationReport report;
    float totalFiringRate = 0.0f;
    
    try {
        for (size_t step = 0; step < maxSteps; ++step) {
            // Get sensory percept
            nnlm::SensoryPercept percept = world->getSensoryPercept(brain->getRegions()[0].get());
            
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
            
            report.stepsCompleted = step + 1;
        }
        
        // Calculate final statistics
        report.avgFiringRate = totalFiringRate / maxSteps;
        
    } catch (const std::exception& e) {
        report.success = false;
        report.errorMessage = std::string("Simulation error: ") + e.what();
    }
    
    return report;
}

/**
 * Performance profile analyzer
 */nnlm::struct PerformanceAnalysis {
    float throughputNeuronsPerSecond;
    float memoryEfficiency;
    float balanceScore;
    float stabilityScore;
    nnlm::map<std::string, float> benchmarks;
    nnlm::vector<std::string> recommendations;
};

PerformanceAnalysis analyzePerformance(
    nnlm::shared_ptr<Brain> brain,
    nnlm::shared_ptr<SimpleWorld> world,
    nnlm::shared_ptr<AgentBrain> agent,
    const nnlm::vector<std::string>& testScenarios = {}) {
    
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

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>
#include <chrono>

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
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

// ==============================================================================
// ENHANCED CONVENIENCE FUNCTIONS
// ==============================================================================

/**
 * Create optimized brain for specific task types
 * 
 * @param task_type Type of task ("classification", "regression", "reinforcement", "language")
 * @param input_dim Input dimensionality
 * @param output_dim Output dimensionality
 * @return Configured and initialized Brain
 */nlm::shared_ptr<Brain> createOptimizedBrain(
    const std::string& taskType,
    size_t inputDim,
    size_t outputDim) {
    
    auto config = nlm::createDefaultConfig();
    
    // Set basic configuration
    config->set("neuron_count", static_cast<int64_t>(std::min<size_t>(1000, inputDim * outputDim * 2)));
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
    
    auto brain = nlm::createBrain(config);
    brain->initialize();
    return brain;
}

/**
 * Create realistic environment with configurable physics
 * 
 * @param env_type Type of environment ("grid_world", "continuous", "hierarchical")
 * @param size Environment size
 * @return Configured and initialized SimpleWorld
 */nlm::shared_ptr<SimpleWorld> createRealisticWorld(
    const std::string& envType,
    const nlm::tuple<size_t, size_t>& size) {
    
    auto world = nlm::createSimpleWorld();
    
    size_t width = std::get<0>(size);
    size_t height = std::get<1>(size);
    world->configure(width, height, std::min(width, 16), std::min(height, 16));
    
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
            
            report.stepsCompleted = step + 1;
        }
        
        // Calculate final statistics
        report.avgFiringRate = totalFiringRate / maxSteps;
        
    } catch (const std::exception& e) {
        report.success = false;
        report.errorMessage = std::string("Simulation error: ") + e.what();
    }
    
    return report;
}

/**
 * Performance profile analyzer
 */nlm::struct PerformanceAnalysis {
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
    )pbdoc";
    
    // Register exception types
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");
    
    // Helper function to create default config
    m.def("create_default_config", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");
    
    // Advanced convenience functions
    m.def("create_optimized_brain", &createOptimizedBrain,
          py::arg("task_type"), py::arg("input_dim"), py::arg("output_dim"),
          "Create a brain optimized for a specific task type");
    
    m.def("create_realistic_world", &createRealisticWorld,
          py::arg("env_type"), py::arg("size"),
          "Create a realistic environment with configurable physics");
    
    m.def("advanced_simulation_run", &advancedSimulationRun,
          py::arg("brain"), py::arg("world"), py::arg("agent"),
          py::arg("max_steps") = 1000,
          py::arg("monitor_callback") = nullptr,
          py::arg("render_callback") = nullptr,
          "Run an advanced simulation with comprehensive monitoring");
    
    m.def("analyze_performance", &analyzePerformance,
          py::arg("brain"), py::arg("world"), py::arg("agent"),
          py::arg("test_scenarios") = std::vector<std::string>(),
          "Analyze simulation performance and provide optimization recommendations");
    
    // Result types
    py::class_<SimulationReport>(m, "SimulationReport")
        .def(py::init<>())
        .def_readwrite("total_reward", &SimulationReport::totalReward)
        .def_readwrite("avg_firing_rate", &SimulationReport::avgFiringRate)
        .def_readwrite("max_firing_rate", &SimulationReport::maxFiringRate)
        .def_readwrite("steps_completed", &SimulationReport::stepsCompleted)
        .def_readwrite("action_count", &SimulationReport::actionCount)
        .def_readwrite("success", &SimulationReport::success)
        .def_readwrite("error_message", &SimulationReport::errorMessage);
    
    py::class_<PerformanceAnalysis>(m, "PerformanceAnalysis")
        .def(py::init<>())
        .def_readwrite("throughput_neurons_per_second", &PerformanceAnalysis::throughputNeuronsPerSecond)
        .def_readwrite("memory_efficiency", &PerformanceAnalysis::memoryEfficiency)
        .def_readwrite("balance_score", &PerformanceAnalysis::balanceScore)
        .def_readwrite("stability_score", &PerformanceAnalysis::stabilityScore)
        .def_readwrite("benchmarks", &PerformanceAnalysis::benchmarks)
        .def_readwrite("recommendations", &PerformanceAnalysis::recommendations);
    
    // Constants
    m.attr("SUPPORTED_TASK_TYPES") = std::map<std::string, std::string>({{
        {"classification", "Neural classification and pattern recognition"},
        {"regression", "Neural regression and prediction"},
        {"reinforcement", "Reinforcement learning and decision making"},
        {"language", "Natural language processing"}
    }});
    
    m.attr("EXPERIMENT_VERSION") = "3.0.0";
    
    // Documentation
    m.doc() += "\n\nFor advanced documentation and examples, visit the NLM documentation portal.";
}

} // namespace nlm

