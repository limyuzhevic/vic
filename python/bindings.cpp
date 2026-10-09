// Enhanced NLM Python Bindings with High-Level APIs
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <chrono>
#include <algorithm>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/experiments/ExperimentRunner.hpp"
#include "../src/experiments/Phase6IntegratedExperiment.hpp"

namespace py = pybind11;
namespace nlm {

// Forward declarations for forward references in function signatures
class AgentBrain;
class SimpleWorld;

// Type aliases for Python convenience
using ConfigPtr = std::shared_ptr<Config>;
using BrainPtr = std::shared_ptr<Brain>;
using WorldPtr = std::shared_ptr<SimpleWorld>;
using AgentPtr = std::shared_ptr<AgentBrain>;
using ExperimentPtr = std::shared_ptr<Experiment>;
using ExperimentRunnerPtr = std::shared_ptr<ExperimentRunner>;

// =============================================================================
// HIGH-LEVEL CONVENIENCE CLASSES
// =============================================================================

class SimpleBrain {
public:
    SimpleBrain() {
        config = std::make_shared<Config>();
        // Apply sensible defaults for quick setup
        config->set("brain.neuron_count", static_cast<int64_t>(500));
        config->set("brain.region_count", static_cast<int64_t>(2));
        config->set("brain.synapse_density", 0.1f);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        config->set("neuromod.curiosity.enable", true);
        
        brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
    }
    
    SimpleBrain(int neuronCount, int regions) {
        config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(neuronCount));
        config->set("brain.region_count", static_cast<int64_t>(regions));
        config->set("brain.synapse_density", 0.1f);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        config->set("neuromod.curiosity.enable", true);
        
        brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
    }
    
    // Learning from experience
    void learn(int steps, const std::vector<float>& inputPattern = {}) {
        for (int step = 0; step < steps; ++step) {
            if (!inputPattern.empty() && step < inputPattern.size()) {
                // Inject current based on input pattern
                brain->injectCurrentToNeurons(NeuronType::Sensory, inputPattern[step] * 10.0f);
            } else {
                // Default activity pattern
                brain->injectCurrentToNeurons(NeuronType::Sensory, 5.0f);
            }
            
            brain->step(step);
            
            if (step % 100 == 0) {
                NLM_LOG_INFO("Step " + std::to_string(step) + 
                           " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
            }
        }
    }
    
    // Get learning statistics
    std::map<std::string, double> getLearningStats() const {
        std::map<std::string, double> stats;
        stats["total_neurons"] = static_cast<double>(brain->getTotalNeuronCount());
        stats["total_synapses"] = static_cast<double>(brain->getTotalSynapseCount());
        stats["firing_neurons"] = static_cast<double>(brain->getFiringNeuronCount());
        stats["average_firing_rate"] = brain->getAverageFiringRate();
        stats["excitement_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        stats["total_spikes"] = static_cast<double>(brain->getTotalSpikeCount());
        stats["developmental_stage"] = static_cast<double>(brain->getDevelopmentalStage());
        return stats;
    }
    
    // Save/load brain state
    bool save(const std::string& filepath) const {
        return brain->save(filepath);
    }
    
    bool load(const std::string& filepath) {
        if (brain->load(filepath)) {
            return true;
        }
        // Re-initialize if load failed
        brain->reset();
        brain->initialize();
        return false;
    }
    
    // Get underlying brain
    BrainPtr getBrain() const { return brain; }
    
    // Get configuration
    ConfigPtr getConfig() const { return config; }
    
private:
    ConfigPtr config;
    BrainPtr brain;
};

class LearningAgent {
public:
    LearningAgent(int width, int height) {
        world = std::make_shared<SimpleWorld>();
        world->configure(width, height, width, height);
        
        config = std::make_shared<Config>();
        // Reasonable defaults for agent learning
        config->set("brain.neuron_count", static_cast<int64_t>(300));
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.dopamine.scale", 1.0f);
        
        brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        
        // Enable learning subsystems
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        world->reset();
        world->setAgentStart(width / 2.0f, height / 2.0f);
    }
    
    // Run a complete episode
    std::vector<double> runEpisode(int maxSteps = 1000) {
        std::vector<double> rewards;
        std::vector<std::vector<double>> states;
        
        for (int step = 0; step < maxSteps; ++step) {
            // Update world
            world->update(0.1);
            
            // Get sensory percept
            auto percept = world->getSensoryPercept();
            
            // Process in brain
            agent->processSensoryInput(percept);
            
            // Run brain step
            brain->step(step);
            
            // Get action from brain
            auto action = agent->decodeMotorCommand();
            
            // Apply action to world
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Apply reward modulation
            auto internalSignals = percept.getInternal();
            float reward = internalSignals.empty() ? 0.0f : internalSignals[0];
            agent->applyRewardModulation(reward, 0.0);
            
            // Update development
            agent->updateDevelopment(0.1);
            
            rewards.push_back(reward);
            
            if (step % 100 == 0) {
                NLM_LOG_INFO("Agent step " + std::to_string(step) + 
                           " | Reward: " + std::to_string(reward) +
                           " | Curiosity: " + std::to_string(agent->getCuriosityLevel()));
            }
            
            // Check if episode should end
            auto body = world->getAgentBody();
            if (body.health <= 0.0f || body.energy <= 0.0f) {
                break;
            }
        }
        
        return rewards;
    }
    
    // Get agent statistics
    std::map<std::string, double> getAgentStats() const {
        std::map<std::string, double> stats;
        stats["curiosity_level"] = agent->getCuriosityLevel();
        stats["novelty_level"] = agent->getNoveltyLevel();
        stats["neuromodulation_level"] = agent->getNeuromodulationLevel();
        stats["prediction_error"] = agent->getPredictionError();
        stats["developmental_stage"] = static_cast<double>(agent->getDevelopmentalStage());
        stats["total_neurons"] = static_cast<double>(brain->getTotalNeuronCount());
        stats["firing_neurons"] = static_cast<double>(brain->getFiringNeuronCount());
        stats["average_firing_rate"] = brain->getAverageFiringRate();
        return stats;
    }
    
    // Get world information
    std::map<std::string, double> getWorldInfo() const {
        auto body = world->getAgentBody();
        std::map<std::string, double> info;
        info["x"] = body.x;
        info["y"] = body.y;
        info["orientation"] = body.orientation;
        info["energy"] = body.energy;
        info["health"] = body.health;
        info["age"] = body.age;
        return info;
    }
    
    // Check if agent is alive
    bool isAlive() const {
        auto body = world->getAgentBody();
        return body.health > 0.0f && body.energy > 0.0f;
    }
    
    // Get underlying components
    BrainPtr getBrain() const { return brain; }
    ConfigPtr getConfig() const { return config; }
    WorldPtr getWorld() const { return world; }
    AgentPtr getAgent() const { return agent; }
    
private:
    ConfigPtr config;
    WorldPtr world;
    BrainPtr brain;
    AgentPtr agent;
};

class ExperimentManager {
public:
    ExperimentManager() {
        runner = std::make_shared<ExperimentRunner>();
    }
    
    // Create a new experiment
    std::shared_ptr<Experiment> createExperiment(const std::string& name, std::shared_ptr<Brain> brainTemplate) {
        auto experiment = runner->createExperiment(name);
        experiment->setConfigValue("brain.neuron_count", std::to_string(brainTemplate->getTotalNeuronCount()));
        return experiment;
    }
    
    // Run an experiment
    std::shared_ptr<Experiment> runExperiment(const std::string& name, std::shared_ptr<Brain> brain, 
                                            std::shared_ptr<Environment> environment, int maxSteps) {
        auto experiment = runner->getExperiment(name);
        if (experiment) {
            runner->runExperiment(experiment, brain, environment, static_cast<SimulationStep>(maxSteps));
        }
        return experiment;
    }
    
    // Run a batch of experiments
    std::vector<std::shared_ptr<Experiment>> runBatch(const std::vector<std::string>& experimentNames,
                                                     std::shared_ptr<Brain> brainTemplate,
                                                     std::shared_ptr<Environment> environment,
                                                     int maxSteps) {
        std::vector<std::shared_ptr<Experiment>> experiments;
        for (const auto& name : experimentNames) {
            auto experiment = createExperiment(name, brainTemplate);
            experiments.push_back(experiment);
        }
        
        return runner->runBatch(experiments, brainTemplate, environment, static_cast<SimulationStep>(maxSteps));
    }
    
    // Save experiment results
    bool saveResults(const std::string& filepath) {
        return runner->saveResults(filepath);
    }
    
    // Get all experiments
    std::vector<std::string> getExperimentNames() const {
        std::vector<std::string> names;
        for (const auto& exp : runner->getExperiments()) {
            names.push_back(exp->getName());
        }
        return names;
    }
    
    // Get experiment results
    std::map<std::string, std::map<std::string, double>> getExperimentMetrics(const std::string& experimentName) {
        std::map<std::string, std::map<std::string, double>> results;
        
        auto experiment = runner->getExperiment(experimentName);
        if (!experiment) {
            return results;
        }
        
        const auto& metrics = experiment->getMetrics();
        for (const auto& metric : metrics) {
            results[experimentName][metric.first] = metric.second;
        }
        
        return results;
    }
    
    // Clear all experiments
    void clearExperiments() {
        runner->clearExperiments();
    }
    
private:
    ExperimentRunnerPtr runner;
};

// =============================================================================
// ADVANCED PYTHON APIS
// =============================================================================

// Convenience functions for common use cases
ConfigPtr createDefaultConfig() {
    auto config = std::make_shared<Config>();
    
    // Set reasonable defaults
    config->set("brain.neuron_count", static_cast<int64_t>(1000));
    config->set("brain.region_count", static_cast<int64_t>(2));
    config->set("brain.synapse_density", 0.1f);
    config->set("plasticity.stdp.enable", true);
    config->set("plasticity.hebbian.enable", true);
    config->set("plasticity.structural.enable", true);
    config->set("neuromod.curiosity.enable", true);
    config->set("neuromod.novelty.enable", true);
    
    return config;
}

BrainPtr createBrain(int neuronCount, int regions, const std::string& configFile = "") {
    auto config = std::make_shared<Config>();
    
    if (!configFile.empty()) {
        config->loadFromFile(configFile);
    }
    
    config->set("brain.neuron_count", static_cast<int64_t>(neuronCount));
    config->set("brain.region_count", static_cast<int64_t>(regions));
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    return brain;
}

SimpleWorld createSimpleWorld(int width, int height, int visionWidth = -1, int visionHeight = -1) {
    auto world = std::make_shared<SimpleWorld>();
    if (visionWidth == -1) visionWidth = width;
    if (visionHeight == -1) visionHeight = height;
    world->configure(width, height, visionWidth, visionHeight);
    return world;
}

AgentBrain createAgentBrain(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
    auto agent = std::make_shared<AgentBrain>(brain);
    agent->initialize(world);
    return agent;
}

// Advanced configuration functions
void configureLearningConfig(ConfigPtr config, const std::string& learningMode) {
    if (learningMode == "fast") {
        config->set("plasticity.stdp.learning_rate", 0.005f);
        config->set("plasticity.hebbian.learning_rate", 0.02f);
        config->set("brain.synapse_density", 0.2f);
        config->set("neuromod.curiosity.exploration_rate", 0.3f);
    } else if (learningMode == "deep") {
        config->set("plasticity.stdp.learning_rate", 0.001f);
        config->set("plasticity.hebbian.learning_rate", 0.01f);
        config->set("brain.synapse_density", 0.05f);
        config->set("neuromod.curiosity.exploration_rate", 0.1f);
        config->set("plasticity.structural.enable", true);
    } else if (learningMode == "conservative") {
        config->set("plasticity.stdp.learning_rate", 0.0001f);
        config->set("plasticity.hebbian.learning_rate", 0.005f);
        config->set("brain.synapse_density", 0.01f);
        config->set("neuromod.curiosity.exploration_rate", 0.05f);
        config->set("plasticity.structural.enable", false);
    }
}

// Performance monitoring
class PerformanceMonitor {
public:
    PerformanceMonitor(std::shared_ptr<Brain> brain) : brain(brain) {
        startTime = std::chrono::high_resolution_clock::now();
        steps = 0;
        maxMemory = 0.0;
    }
    
    void step() {
        steps++;
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime);
        
        // Simple memory estimation based on brain state
        double memoryEstimate = 
            static_cast<double>(brain->getTotalNeuronCount()) * 100.0 +  // Neuron data
            static_cast<double>(brain->getTotalSynapseCount()) * 50.0 +   // Synapse data
            static_cast<double>(brain->getRegionCount()) * 20.0;          // Region data
        
        maxMemory = std::max(maxMemory, memoryEstimate);
        
        if (steps % 100 == 0) {
            NLM_LOG_DEBUG("Performance step " + std::to_string(steps) + 
                         " | Time: " + std::to_string(duration.count()) + "ms" +
                         " | Memory: " + std::to_string(memoryEstimate) + " KB");
        }
    }
    
    double getStepsPerSecond() const {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - startTime);
        if (duration.count() == 0) return 0.0;
        return static_cast<double>(steps) / duration.count();
    }
    
    double getMaxMemoryUsage() const { return maxMemory; }
    
    double getAverageFiringRate() const { return brain->getAverageFiringRate(); }
    
    std::map<std::string, double> getSummary() const {
        std::map<std::string, double> summary;
        summary["total_steps"] = static_cast<double>(steps);
        summary["steps_per_second"] = getStepsPerSecond();
        summary["max_memory_kb"] = maxMemory;
        summary["average_firing_rate"] = getAverageFiringRate();
        summary["neuron_count"] = static_cast<double>(brain->getTotalNeuronCount());
        summary["synapse_count"] = static_cast<double>(brain->getTotalSynapseCount());
        summary["firing_neuron_count"] = static_cast<double>(brain->getFiringNeuronCount());
        return summary;
    }
    
private:
    std::shared_ptr<Brain> brain;
    std::chrono::high_resolution_clock::time_point startTime;
    int steps;
    double maxMemory;
};

// Visualization utilities (simplified version)
class SimpleVisualizer {
public:
    static void plotFiringRate(std::shared_ptr<Brain> brain, int windowWidth = 800, int windowHeight = 600) {
        // Simplified visualization - in practice, this would use matplotlib/numpy
        NLM_LOG_INFO("Visualization: Plot firing rate over time");
        NLM_LOG_INFO("Brain has " + std::to_string(brain->getTotalNeuronCount()) + " neurons");
        NLM_LOG_INFO("Average firing rate: " + std::to_string(brain->getAverageFiringRate()) + " Hz");
    }
    
    static void plotSynapseWeights(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("Visualization: Plot synaptic weight distribution");
        
        // This would create histograms of synaptic weights
        // For now, just log summary statistics
        NLM_LOG_INFO("Synapse weight analysis would show mean, std, min, max");
    }
    
    static void plotActivityPattern(std::shared_ptr<Brain> brain, int regionId = 1) {
        NLM_LOG_INFO("Visualization: Plot neural activity pattern for region " + std::to_string(regionId));
        NLM_LOG_INFO("This would show spatially organized neural activity");
    }
};

PYBIND11_MODULE(pynlm_enhanced, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        -------------------------------------------------------
        
        Enhanced version with high-level convenience classes, advanced APIs,
        experiment management, and performance monitoring.
        
        Key features:
        - SimpleBrain: High-level brain interface for quick setup
        - LearningAgent: Complete agent-world integration
        - ExperimentManager: Comprehensive experiment management
        - PerformanceMonitor: Performance tracking and analysis
        - SimpleVisualizer: Basic visualization utilities
        - Advanced configuration utilities
        - Convenience functions for common tasks
        
        Designed for both rapid prototyping and sophisticated research.
    )pbdoc";
    
    // Register the base types and enums (same as original)
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    
    py::class_<NeuronId>(m, "NeuronId")
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
    
    // Register other basic types (abbreviated for brevity)
    // ... (same basic type registrations as original)
    
    // Register enhanced classes
    py::class_<SimpleBrain>(m, "SimpleBrain", R"pbdoc(High-level brain interface for rapid setup and experimentation)pbdoc")
        .def(py::init<>())
        .def(py::init<int, int>(), py::arg("neuronCount"), py::arg("regions"))
        .def("learn", &SimpleBrain::learn, py::arg("steps"), py::arg("inputPattern") = std::vector<float>(),
              "Learn from experience by running simulation steps")
        .def("getLearningStats", &SimpleBrain::getLearningStats,
             "Get statistics about learning performance")
        .def("save", &SimpleBrain::save, py::arg("filepath"), "Save brain state to file")
        .def("load", &SimpleBrain::load, py::arg("filepath"), "Load brain state from file")
        .def("getBrain", &SimpleBrain::getBrain, "Get the underlying Brain object");
    
    py::class_<LearningAgent>(m, "LearningAgent", R"pbdoc(Complete agent-world integration for reinforcement learning)pbdoc")
        .def(py::init<int, int>(), py::arg("width"), py::arg("height"))
        .def("runEpisode", &LearningAgent::runEpisode, py::arg("maxSteps") = 1000,
             "Run a complete episode and return rewards")
        .def("getAgentStats", &LearningAgent::getAgentStats, "Get agent performance statistics")
        .def("getWorldInfo", &LearningAgent::getWorldInfo, "Get world state information")
        .def("isAlive", &LearningAgent::isAlive, "Check if agent is alive")
        .def("getBrain", &LearningAgent::getBrain, "Get the underlying Brain object")
        .def("getWorld", &LearningAgent::getWorld, "Get the underlying World object");
    
    py::class_<ExperimentManager>(m, "ExperimentManager", R"pbdoc(Manager for running and analyzing experiments)pbdoc")
        .def(py::init<>())
        .def("createExperiment", &ExperimentManager::createExperiment,
             py::arg("name"), py::arg("brainTemplate"), "Create a new experiment")
        .def("runExperiment", &ExperimentManager::runExperiment,
             py::arg("name"), py::arg("brain"), py::arg("environment"), py::arg("maxSteps"),
             "Run a single experiment")
        .def("runBatch", &ExperimentManager::runBatch,
             py::arg("experimentNames"), py::arg("brainTemplate"), py::arg("environment"), py::arg("maxSteps"),
             "Run multiple experiments in batch")
        .def("saveResults", &ExperimentManager::saveResults,
             py::arg("filepath"), "Save experiment results")
        .def("getExperimentNames", &ExperimentManager::getExperimentNames,
             "Get names of all experiments")
        .def("clearExperiments", &ExperimentManager::clearExperiments,
             "Clear all experiments");
    
    py::class_<PerformanceMonitor>(m, "PerformanceMonitor", R"pbdoc(Track performance metrics during brain simulation)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("step", &PerformanceMonitor::step, "Record a simulation step")
        .def("getStepsPerSecond", &PerformanceMonitor::getStepsPerSecond,
             "Get processing speed in steps per second")
        .def("getMaxMemoryUsage", &PerformanceMonitor::getMaxMemoryUsage,
             "Get maximum memory usage in KB")
        .def("getSummary", &PerformanceMonitor::getSummary,
             "Get comprehensive performance summary");
    
    py::class_<SimpleVisualizer>(m, "SimpleVisualizer", R"pbdoc(Basic visualization utilities for NLM data)pbdoc")
        .def_static("plotFiringRate", &SimpleVisualizer::plotFiringRate,
                   py::arg("brain"), py::arg("windowWidth") = 800, py::arg("windowHeight") = 600,
                   "Plot firing rate over time")
        .def_static("plotSynapseWeights", &SimpleVisualizer::plotSynapseWeights,
                   py::arg("brain"), "Plot synaptic weight distribution")
        .def_static("plotActivityPattern", &SimpleVisualizer::plotActivityPattern,
                   py::arg("brain"), py::arg("regionId") = 1, "Plot neural activity pattern");
    
    // Enhanced convenience functions
    m.def("createDefaultConfig", &createDefaultConfig,
          "Create a default configuration with sensible learning parameters");
    
    m.def("createBrain", &createBrain, py::arg("neuronCount"), py::arg("regions"), py::arg("configFile") = "",
          "Create a brain with specified parameters and optional config file");
    
    m.def("createSimpleWorld", &createSimpleWorld,
          py::arg("width"), py::arg("height"), py::arg("visionWidth") = -1, py::arg("visionHeight") = -1,
          "Create a simple 2D world with optional vision parameters");
    
    m.def("createAgentBrain", &createAgentBrain,
          py::arg("brain"), py::arg("world"), "Create an agent brain interface");
    
    m.def("configureLearningConfig", &configureLearningConfig,
          py::arg("config"), py::arg("learningMode"), "Configure learning parameters based on mode");
    
    // High-level experiment types
    py::enum_<ExperimentType>(m, "ExperimentType", R"pbdoc(High-level experiment types)pbdoc")
        .value("ConnectivityTest", ExperimentType::ConnectivityTest)
        .value("LearningExperiment", ExperimentType::LearningExperiment)
        .value("PlasticityTest", ExperimentType::PlasticityTest)
        .value("NeuromodulationTest", ExperimentType::NeuromodulationTest)
        .value("DevelopmentTest", ExperimentType::DevelopmentTest)
        .value("Benchmark", ExperimentType::Benchmark)
        .export_values();
    
    m.def("createExperiment", [](std::shared_ptr<Config> config, ExperimentType type) {
        auto experiment = std::make_shared<Experiment>(static_cast<int>(type));
        experiment->setConfigValue("experiment.type", std::to_string(static_cast<int>(type)));
        return experiment;
    }, py::arg("config"), py::arg("type"), "Create an experiment with specified type");
    
    m.def("runPhase6Experiment", &runPhase6Experiment,
          py::arg("brain"), py::arg("world"), py::arg("steps") = 1000,
          "Run a Phase 6 integrated experiment");
    
    // Add version and metadata
    m.attr("version") = "enhanced_0.1.0";
    m.attr("author") = "NLM Enhanced Python Bindings";
    m.attr("description") = "High-level Python interface for NLM neural simulation";
    
    // Backward compatibility - re-export original classes
    py::class_<Config>(m, "Config", py::module_local())
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"))
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"))
        .def("has", &Config::has, py::arg("key"))
        .def("getKeys", &Config::getKeys);
    
    py::class_<Brain>(m, "Brain", py::module_local())
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize)
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step))
        .def("reset", &Brain::reset)
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount)
        .def("getAverageFiringRate", &Brain::getAverageFiringRate);
    
    py::class_<SimpleWorld>(m, "SimpleWorld", py::module_local())
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure)
        .def("update", &SimpleWorld::update)
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand);
    
    py::class_<AgentBrain>(m, "AgentBrain", py::module_local())
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"))
        .def("processSensoryInput", &AgentBrain::processSensoryInput, py::arg("percept"))
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand);
    
    // Utility functions
    m.def("createDefaultConfig", []() { return std::make_shared<Config>(); }, 
          "Create default configuration (original API for backward compatibility)");
    
    m.def("createBrain", [](std::shared_ptr<Config> config) { 
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("config"), "Create brain (original API)");
    
    m.def("createSimpleWorld", []() { return std::make_shared<SimpleWorld>(); }, 
          "Create simple world (original API)");
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) { 
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create agent brain (original API)");
    
} // namespace nlm

} // PYBIND11_MODULE

// Phase 6 Integrated Experiment Function
ExperimentPtr runPhase6Experiment(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, SimulationStep steps) {
    auto experiment = std::make_shared<Phase6IntegratedExperiment>();
    
    // Configure experiment
    experiment->setConfigValue("brain.neuron_count", std::to_string(brain->getTotalNeuronCount()));
    experiment->setConfigValue("world.width", std::to_string(world->getWidth()));
    experiment->setConfigValue("world.height", std::to_string(world->getHeight()));
    experiment->setConfigValue("steps", std::to_string(steps));
    
    // Run the experiment
    experiment->run(brain, world, steps);
    
    return experiment;
}

} // namespace nlm