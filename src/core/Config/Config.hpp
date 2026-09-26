// High-level factory methods and builder patterns for NLM

#pragma once

#include <string>
#include <memory>
#include <functional>
#include <vector>
#include <map>
#include <sstream>
#include <iomanip>

#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include "../agent/AgentBrain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"

namespace nlm {

// Configuration presets for common use cases
class ConfigPresets {
public:
    // Preset for simple exploration
    static std::shared_ptr<Config> createExplorationConfig(
        uint64_t neuronCount = 500,
        uint64_t worldSize = 16,
        double timestep = 0.001) {
        auto config = std::make_shared<Config>();
        
        // Basic configuration
        config->set("neuron_count", static_cast<int64_t>(neuronCount));
        config->set("region_count", 2);
        config->set("connection_probability", 0.1f);
        config->set("simulation_timestep", timestep);
        config->set("random_seed", static_cast<int64_t>(42));
        
        // Exploration-specific settings
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("stdp_tau", 20.0f);
        config->set("synaptogenesis_rate", 0.0001f);
        config->set("pruning_rate", 0.00001f);
        
        // Neuromodulation for exploration
        config->set("curiosity_enabled", true);
        config->set("novelty_sensitivity", 0.5f);
        config->set("reward_prediction_error", 0.1f);
        
        // Environment
        config->set("environment_width", worldSize);
        config->set("environment_height", worldSize);
        config->set("vision_width", worldSize / 2);
        config->set("vision_height", worldSize / 2);
        
        return config;
    }
    
    // Preset for efficient learning
    static std::shared_ptr<Config> createLearningConfig(
        uint64_t neuronCount = 1000,
        uint64_t worldSize = 32,
        double timestep = 0.001) {
        auto config = std::make_shared<Config>();
        
        // Larger brain for learning
        config->set("neuron_count", static_cast<int64_t>(neuronCount));
        config->set("region_count", 3);
        config->set("connection_probability", 0.15f);
        config->set("simulation_timestep", timestep);
        config->set("random_seed", static_cast<int64_t>(42));
        
        // Learning-specific settings
        config->set("stdp_ltp_weight", 0.02f);
        config->set("stdp_ltd_weight", 0.018f);
        config->set("stdp_tau", 30.0f);
        config->set("synaptogenesis_rate", 0.00005f);
        config->set("pruning_rate", 0.000005f);
        
        // Advanced learning
        config->set("working_memory_size", 100);
        config->set("episodes_per_batch", 10);
        config->set("prediction_horizon", 5);
        
        // Environment
        config->set("environment_width", worldSize);
        config->set("environment_height", worldSize);
        config->set("vision_width", worldSize);
        config->set("vision_height", worldSize);
        
        return config;
    }
    
    // Preset for developmental learning
    static std::shared_ptr<Config> createDevelopmentConfig(
        uint64_t neuronCount = 800,
        uint64_t worldSize = 24,
        double timestep = 0.001) {
        auto config = std::make_shared<Config>();
        
        // Medium-sized brain
        config->set("neuron_count", static_cast<int64_t>(neuronCount));
        config->set("region_count", 2);
        config->set("connection_probability", 0.12f);
        config->set("simulation_timestep", timestep);
        config->set("random_seed", static_cast<int64_t>(42));
        
        // Developmental plasticity
        config->set("stdp_ltp_weight", 0.015f);
        config->set("stdp_ltd_weight", 0.014f);
        config->set("stdp_tau", 25.0f);
        config->set("synaptogenesis_rate", 0.00008f);
        config->set("pruning_rate", 0.000008f);
        
        // Development stages
        config->set("critical_period_start", 100);
        config->set("critical_period_end", 500);
        config->set("development_rate", 0.001);
        config->set("maturation_threshold", 1000);
        
        // Environment
        config->set("environment_width", worldSize);
        config->set("environment_height", worldSize);
        config->set("vision_width", worldSize / 2);
        config->set("vision_height", worldSize / 2);
        
        return config;
    }
    
    // Preset for fast simulation (small scale)
    static std::shared_ptr<Config> createQuickConfig(
        uint64_t neuronCount = 100,
        uint64_t worldSize = 8,
        double timestep = 0.001) {
        auto config = std::make_shared<Config>();
        
        // Small brain for quick tests
        config->set("neuron_count", static_cast<int64_t>(neuronCount));
        config->set("region_count", 1);
        config->set("connection_probability", 0.08f);
        config->set("simulation_timestep", timestep);
        config->set("random_seed", static_cast<int64_t>(42));
        
        // Optimized for speed
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("stdp_tau", 15.0f);
        config->set("synaptogenesis_rate", 0.0002f); // Higher for growth
        config->set("pruning_rate", 0.00002f);
        
        // Environment
        config->set("environment_width", worldSize);
        config->set("environment_height", worldSize);
        config->set("vision_width", worldSize);
        config->set("vision_height", worldSize);
        
        return config;
    }
};

// Builder pattern for complex brain configurations
class BrainConfigBuilder {
private:
    std::shared_ptr<Config> config;
    
public:
    BrainConfigBuilder() : config(std::make_shared<Config>()) {}
    
    // Set basic parameters
    BrainConfigBuilder& withNeuronCount(uint64_t count) {
        config->set("neuron_count", static_cast<int64_t>(count));
        return *this;
    }
    
    BrainConfigBuilder& withRegionCount(uint64_t count) {
        config->set("region_count", static_cast<int64_t>(count));
        return *this;
    }
    
    BrainConfigBuilder& withConnectionProbability(float prob) {
        config->set("connection_probability", prob);
        return *this;
    }
    
    BrainConfigBuilder& withTimestep(double timestep) {
        config->set("simulation_timestep", timestep);
        return *this;
    }
    
    BrainConfigBuilder& withRandomSeed(uint64_t seed) {
        config->set("random_seed", static_cast<int64_t>(seed));
        return *this;
    }
    
    // Set plasticity parameters
    BrainConfigBuilder& withSTDP(float ltp, float ltd, float tau) {
        config->set("stdp_ltp_weight", ltp);
        config->set("stdp_ltd_weight", ltd);
        config->set("stdp_tau", tau);
        return *this;
    }
    
    BrainConfigBuilder& withStructuralPlasticity(float genesis, float pruning) {
        config->set("synaptogenesis_rate", genesis);
        config->set("pruning_rate", pruning);
        return *this;
    }
    
    // Set neuromodulation
    BrainConfigBuilder& withNeuromodulation(bool curiosity, bool novelty, bool reward) {
        config->set("curiosity_enabled", curiosity);
        config->set("novelty_sensitivity", novelty ? 0.5f : 0.0f);
        config->set("reward_prediction_error", reward ? 0.1f : 0.0f);
        return *this;
    }
    
    // Set environment
    BrainConfigBuilder& withEnvironment(uint64_t width, uint64_t height,
                                       uint64_t visionWidth, uint64_t visionHeight) {
        config->set("environment_width", width);
        config->set("environment_height", height);
        config->set("vision_width", visionWidth);
        config->set("vision_height", visionHeight);
        return *this;
    }
    
    // Add custom key-value pair
    BrainConfigBuilder& setCustom(const std::string& key, const std::string& value) {
        config->set(key, value);
        return *this;
    }
    
    BrainConfigBuilder& setCustom(const std::string& key, int value) {
        config->set(key, value);
        return *this;
    }
    
    BrainConfigBuilder& setCustom(const std::string& key, double value) {
        config->set(key, value);
        return *this;
    }
    
    // Get the built configuration
    std::shared_ptr<Config> build() {
        return config;
    }
    
    // Convenience methods for common patterns
    BrainConfigBuilder& presetExploration() {
        return withSTDP(0.01f, 0.012f, 20.0f)
                .withStructuralPlasticity(0.0001f, 0.00001f)
                .withNeuromodulation(true, true, true)
                .withEnvironment(16, 16, 8, 8);
    }
    
    BrainConfigBuilder& presetLearning() {
        return withNeuronCount(1000)
                .withRegionCount(3)
                .withConnectionProbability(0.15f)
                .withSTDP(0.02f, 0.018f, 30.0f)
                .withStructuralPlasticity(0.00005f, 0.000005f)
                .withNeuromodulation(true, true, true)
                .withEnvironment(32, 32, 32, 32);
    }
    
    BrainConfigBuilder& presetDevelopment() {
        return withNeuronCount(800)
                .withRegionCount(2)
                .withConnectionProbability(0.12f)
                .withSTDP(0.015f, 0.014f, 25.0f)
                .withStructuralPlasticity(0.00008f, 0.000008f)
                .withNeuromodulation(true, true, true)
                .setCustom("critical_period_start", 100)
                .setCustom("critical_period_end", 500)
                .setCustom("development_rate", 0.001)
                .withEnvironment(24, 24, 12, 12);
    }
};

// High-level factory methods for creating complete systems
class NLMFactory {
public:
    // Create a simple brain with default settings
    static std::shared_ptr<Brain> createSimpleBrain() {
        auto config = ConfigPresets::createQuickConfig();
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw BrainInitializationError("Failed to initialize simple brain");
        }
        return brain;
    }
    
    // Create a learning brain
    static std::shared_ptr<Brain> createLearningBrain() {
        auto config = ConfigPresets::createLearningConfig();
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw BrainInitializationError("Failed to initialize learning brain");
        }
        return brain;
    }
    
    // Create a developmental brain
    static std::shared_ptr<Brain> createDevelopmentBrain() {
        auto config = ConfigPresets::createDevelopmentConfig();
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw BrainInitializationError("Failed to initialize developmental brain");
        }
        return brain;
    }
    
    // Create an exploration brain
    static std::shared_ptr<Brain> createExplorationBrain() {
        auto config = ConfigPresets::createExplorationConfig();
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw BrainInitializationError("Failed to initialize exploration brain");
        }
        return brain;
    }
    
    // Create a complete agent system (brain + world + agent)
    static std::tuple<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>>
    createAgentSystem(const std::string& preset = "learning") {
        std::shared_ptr<Config> config;
        
        if (preset == "exploration") {
            config = ConfigPresets::createExplorationConfig();
        } else if (preset == "development") {
            config = ConfigPresets::createDevelopmentConfig();
        } else if (preset == "quick") {
            config = ConfigPresets::createQuickConfig();
        } else { // default learning
            config = ConfigPresets::createLearningConfig();
        }
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw BrainInitializationError("Failed to initialize agent brain");
        }
        
        // Create world
        auto world = std::make_shared<SimpleWorld>();
        uint64_t worldSize = config->getOr<int64_t>("environment_width", 16);
        uint64_t visionWidth = config->getOr<int64_t>("vision_width", 8);
        uint64_t visionHeight = config->getOr<int64_t>("vision_height", 8);
        world->configure(worldSize, worldSize, visionWidth, visionHeight);
        
        // Create agent
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        // Configure agent features based on preset
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(preset == "development" || preset == "learning");
        agent->enableCuriosity(preset == "exploration" || preset == "learning");
        
        return std::make_tuple(brain, world, agent);
    }
    
    // Create a simulation environment with pre-configured agent
    static std::shared_ptr<SimpleWorld> createSimulationEnvironment(
        uint64_t steps,
        const std::string& preset = "learning") {
        auto [brain, world, agent] = createAgentSystem(preset);
        
        // Run simulation
        for (uint64_t step = 0; step < steps; ++step) {
            world->update(0.1);
            agent->processSensoryInput(world->getSensoryPercept());
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            agent->applyRewardModulation(world->getAgentBody().energy, 0.0f);
        }
        
        return world;
    }
    
    // Create a learning experiment setup
    static std::tuple<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>>
    createLearningExperiment(uint64_t episodes, uint64_t stepsPerEpisode,
                             const std::string& preset = "learning") {
        auto [brain, world, agent] = createAgentSystem(preset);
        
        // Configure for learning
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        
        return std::make_tuple(brain, world, agent);
    }
    
    // Validate configuration
    static bool validateConfig(const std::shared_ptr<Config>& config) {
        if (!config) return false;
        
        // Check required keys
        std::vector<std::string> requiredKeys = {
            "neuron_count", "region_count", "connection_probability",
            "simulation_timestep", "random_seed"
        };
        
        for (const auto& key : requiredKeys) {
            if (!config->has(key)) {
                return false;
            }
        }
        
        // Validate values
        auto neuronCount = config->getOr<int64_t>("neuron_count", 0);
        if (neuronCount <= 0) return false;
        
        auto regionCount = config->getOr<int64_t>("region_count", 0);
        if (regionCount <= 0) return false;
        
        auto connectionProb = config->getOr<float>("connection_probability", -1.0f);
        if (connectionProb < 0.0f || connectionProb > 1.0f) return false;
        
        auto timestep = config->getOr<double>("simulation_timestep", 0.0);
        if (timestep <= 0.0) return false;
        
        return true;
    }
    
    // Generate configuration report
    static std::string generateConfigReport(const std::shared_ptr<Config>& config) {
        if (!config) return "Invalid configuration";
        
        std::stringstream ss;
        ss << "NLM Configuration Report" << std::endl;
        ss << "======================" << std::endl;
        ss << "Neuron Count: " << config->getOr<int64_t>("neuron_count", 0) << std::endl;
        ss << "Region Count: " << config->getOr<int64_t>("region_count", 0) << std::endl;
        ss << "Connection Probability: " << config->getOr<float>("connection_probability", 0.0f) << std::endl;
        ss << "Simulation Timestep: " << config->getOr<double>("simulation_timestep", 0.0) << "s" << std::endl;
        ss << "Random Seed: " << config->getOr<int64_t>("random_seed", 0) << std::endl;
        
        ss << "\nPlasticity Parameters:" << std::endl;
        ss << "  STDP LTP Weight: " << config->getOr<float>("stdp_ltp_weight", 0.0f) << std::endl;
        ss << "  STDP LTD Weight: " << config->getOr<float>("stdp_ltd_weight", 0.0f) << std::endl;
        ss << "  STDP Tau: " << config->getOr<float>("stdp_tau", 0.0f) << std::endl;
        ss << "  Synaptogenesis Rate: " << config->getOr<float>("synaptogenesis_rate", 0.0f) << std::endl;
        ss << "  Pruning Rate: " << config->getOr<float>("pruning_rate", 0.0f) << std::endl;
        
        ss << "\nNeuromodulation:" << std::endl;
        ss << "  Curiosity Enabled: " << (config->getOr<bool>("curiosity_enabled", false) ? "Yes" : "No") << std::endl;
        ss << "  Novelty Sensitivity: " << config->getOr<float>("novelty_sensitivity", 0.0f) << std::endl;
        ss << "  Reward Prediction Error: " << config->getOr<float>("reward_prediction_error", 0.0f) << std::endl;
        
        ss << "\nEnvironment:" << std::endl;
        ss << "  Width: " << config->getOr<int64_t>("environment_width", 0) << std::endl;
        ss << "  Height: " << config->getOr<int64_t>("environment_height", 0) << std::endl;
        ss << "  Vision Width: " << config->getOr<int64_t>("vision_width", 0) << std::endl;
        ss << "  Vision Height: " << config->getOr<int64_t>("vision_height", 0) << std::endl;
        
        return ss.str();
    }
};

// Convenience functions for quick prototyping
namespace QuickStart {
    // Simplest possible example (matches easy_usage.md)
    inline std::shared_ptr<Brain> simpleBrain() {
        return NLMFactory::createSimpleBrain();
    }
    
    // World interaction example (matches easy_usage.md pattern)
    inline std::tuple<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>>
    worldInteraction(uint64_t steps = 50) {
        auto [brain, world, agent] = NLMFactory::createAgentSystem("quick");
        
        for (uint64_t step = 0; step < steps; ++step) {
            world->update(0.1);
            agent->processSensoryInput(world->getSensoryPercept());
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
        
        return std::make_tuple(brain, world, agent);
    }
    
    // Complete agent example (matches easy_usage.md pattern)
    inline std::tuple<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>>
    completeAgent(uint64_t steps = 100) {
        auto [brain, world, agent] = NLMFactory::createAgentSystem("learning");
        
        // Enable learning
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        
        for (uint64_t step = 0; step < steps; ++step) {
            world->update(0.1);
            agent->processSensoryInput(world->getSensoryPercept());
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            agent->applyRewardModulation(world->getAgentBody().energy, 0.0f);
        }
        
        return std::make_tuple(brain, world, agent);
    }
}} // namespace QuickStart

// Extension of Python bindings with factory methods
namespace py = pybind11;

void addFactoryBindings(py::module_& m) {
    // Config preset functions
    m.def("createExplorationConfig", &ConfigPresets::createExplorationConfig,
          py::arg("neuronCount") = 500,
          py::arg("worldSize") = 16,
          py::arg("timestep") = 0.001,
          "Create an exploration-focused configuration");
    
    m.def("createLearningConfig", &ConfigPresets::createLearningConfig,
          py::arg("neuronCount") = 1000,
          py::arg("worldSize") = 32,
          py::arg("timestep") = 0.001,
          "Create a learning-focused configuration");
    
    m.def("createDevelopmentConfig", &ConfigPresets::createDevelopmentConfig,
          py::arg("neuronCount") = 800,
          py::arg("worldSize") = 24,
          py::arg("timestep") = 0.001,
          "Create a development-focused configuration");
    
    m.def("createQuickConfig", &ConfigPresets::createQuickConfig,
          py::arg("neuronCount") = 100,
          py::arg("worldSize") = 8,
          py::arg("timestep") = 0.001,
          "Create a quick-test configuration");
    
    // Brain factory functions
    m.def("createSimpleBrain", &NLMFactory::createSimpleBrain,
          "Create a simple brain with default settings");
    
    m.def("createLearningBrain", &NLMFactory::createLearningBrain,
          "Create a learning brain");
    
    m.def("createDevelopmentBrain", &NLMFactory::createDevelopmentBrain,
          "Create a developmental brain");
    
    m.def("createExplorationBrain", &NLMFactory::createExplorationBrain,
          "Create an exploration brain");
    
    // Agent system factory functions
    m.def("createAgentSystem", [](const std::string& preset) {
        auto result = NLMFactory::createAgentSystem(preset);
        return py::make_tuple(std::get<0>(result), std::get<1>(result), std::get<2>(result));
    }, py::arg("preset") = "learning",
       "Create a complete agent system (brain + world + agent)");
    
    // Builder pattern
    py::class_<BrainConfigBuilder>(m, "BrainConfigBuilder")
        .def(py::init<>())
        .def("withNeuronCount", &BrainConfigBuilder::withNeuronCount)
        .def("withRegionCount", &BrainConfigBuilder::withRegionCount)
        .def("withConnectionProbability", &BrainConfigBuilder::withConnectionProbability)
        .def("withTimestep", &BrainConfigBuilder::withTimestep)
        .def("withRandomSeed", &BrainConfigBuilder::withRandomSeed)
        .def("withSTDP", &BrainConfigBuilder::withSTDP)
        .def("withStructuralPlasticity", &BrainConfigBuilder::withStructuralPlasticity)
        .def("withNeuromodulation", &BrainConfigBuilder::withNeuromodulation)
        .def("withEnvironment", &BrainConfigBuilder::withEnvironment)
        .def("setCustom", static_cast<BrainConfigBuilder& (BrainConfigBuilder::*)(const std::string&, int)>(&BrainConfigBuilder::setCustom))
        .def("setCustom", static_cast<BrainConfigBuilder& (BrainConfigBuilder::*)(const std::string&, double)>(&BrainConfigBuilder::setCustom))
        .def("setCustom", static_cast<BrainConfigBuilder& (BrainConfigBuilder::*)(const std::string&, const std::string&)>(&BrainConfigBuilder::setCustom))
        .def("presetExploration", &BrainConfigBuilder::presetExploration)
        .def("presetLearning", &BrainConfigBuilder::presetLearning)
        .def("presetDevelopment", &BrainConfigBuilder::presetDevelopment)
        .def("build", &BrainConfigBuilder::build);
    
    // Validation and utility functions
    m.def("validateConfig", &NLMFactory::validateConfig, py::arg("config"),
          "Validate a configuration object");
    
    m.def("generateConfigReport", &NLMFactory::generateConfigReport, py::arg("config"),
          "Generate a human-readable report from a configuration");
    
    // Quick start functions
    m.def("simpleBrain", &QuickStart::simpleBrain,
          "Create the simplest possible brain (one-liner)");
    
    m.def("worldInteraction", &QuickStart::worldInteraction,
          py::arg("steps") = 50,
          "Create a world interaction setup");
    
    m.def("completeAgent", &QuickStart::completeAgent,
          py::arg("steps") = 100,
          "Create a complete agent with learning");
}

} // namespace nlm