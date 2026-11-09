// Core utility functions for NLM experiments
// Common patterns and helper functions used across multiple experiment files

#ifndef NLM_EXPERIMENT_UTILS_H
#define NLM_EXPERIMENT_UTILS_H

#include "brain/Brain.hpp"
#include "environment/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include "configs/Config.hpp"

namespace nlm {

// Common simulation runner used across experiment phases
// Reduces code duplication and provides consistent simulation behavior
class SimulationRunner {
public:
    // Run an agent simulation with configurable development updates
    static void runAgentSimulation(Brain& brain, SimpleWorld& world, AgentBrain& agent,
                                   uint64_t maxSteps, bool enableDevelopment = false,
                                   bool enableCuriosity = false) {
        for (uint64_t step = 0; step < maxSteps; ++step) {
            world.update(0.001);
            auto percept = world.getSensoryPercept();
            agent.processSensoryInput(percept);
            brain.step(step, step * 0.001);
            auto action = agent.decodeMotorCommand();
            world.applyMotorCommand(action, step * 0.001);
            
            if (enableDevelopment) {
                agent.updateDevelopment(0.001);
            }
            
            if (enableCuriosity) {
                float curiosityDrive = agent.getCuriosityLevel();
                agent.applyCuriosity(0.001, curiosityDrive);
            }
        }
    }
    
    // Run simulation without development or curiosity
    static void runSimpleSimulation(Brain& brain, SimpleWorld& world, AgentBrain& agent,
                                   uint64_t maxSteps) {
        runAgentSimulation(brain, world, agent, maxSteps, false, false);
    }
};

// Configuration builder for creating standard test configs
class ConfigBuilder {
public:
    static std::shared_ptr<Config> createDefaultConfig() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", 1, nlm::ConfigSource::Default);
        config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
        config->set("plasticity_learning_rate", 0.01, nlm::ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.01, nlm::ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.012, nlm::ConfigSource::Default);
        config->set("stdp_time_constant", 20.0, nlm::ConfigSource::Default);
        config->set("development_synaptogenesis_rate", 0.001, nlm::ConfigSource::Default);
        config->set("development_pruning_rate", 0.0001, nlm::ConfigSource::Default);
        config->set("dopamine_baseline", 0.1, nlm::ConfigSource::Default);
        config->set("reward_discount_factor", 0.99, nlm::ConfigSource::Default);
        config->set("environment_name", "GridWorld", nlm::ConfigSource::Default);
        config->set("environment_width", 10, nlm::ConfigSource::Default);
        config->set("environment_height", 10, nlm::ConfigSource::Default);
        config->set("log_level", "INFO", nlm::ConfigSource::Default);
        config->set("max_simulation_steps", 1000, nlm::ConfigSource::Default);
        config->set("visualization_enabled", false, nlm::ConfigSource::Default);
        return config;
    }
    
    static std::shared_ptr<Config> createExtendedConfig() {
        auto config = createDefaultConfig();
        config->set("development_enabled", true, nlm::ConfigSource::Default);
        config->set("enable_curiosity", true, nlm::ConfigSource::Default);
        config->set("enable_reward_modulation", true, nlm::ConfigSource::Default);
        return config;
    }
};

// Metrics collector for tracking simulation statistics
class MetricsCollector {
public:
    struct SimulationMetrics {
        float totalReward = 0.0f;
        float avgFiringRate = 0.0f;
        size_t memoryEpisodesStored = 0;
        float noveltyLevel = 0.0f;
        float curiosityLevel = 0.0f;
        float dopamineLevel = 0.0f;
        bool memoryWorkingMemoryIntegrated = false;
        bool memoryEpisodicMemoryIntegrated = false;
        double totalWallClockTime = 0.0;
    };
    
    static void collectSimulationMetrics(const Brain& brain, const AgentBrain& agent,
                                       uint64_t totalSteps, MetricsCollector::SimulationMetrics& metrics) {
        metrics.totalReward = 0.0f;  // This would be accumulated during simulation
        metrics.avgFiringRate = brain.getAverageFiringRate();
        metrics.memoryEpisodesStored = brain.getEpisodicMemory() ? 
                                      static_cast<size_t>(brain.getEpisodicMemory()->getEpisodeCount()) : 0;
        metrics.noveltyLevel = agent.getNoveltyLevel();
        metrics.curiosityLevel = agent.getCuriosityLevel();
        metrics.dopamineLevel = agent.getNeuromodulationLevel();
        metrics.memoryWorkingMemoryIntegrated = (brain.getWorkingMemory() != nullptr);
        metrics.memoryEpisodicMemoryIntegrated = (brain.getEpisodicMemory() != nullptr);
    }
};

// Test runner framework for consistent test execution
class TestFramework {
public:
    template<typename TestFunction>
    static bool runTest(const std::string& testName, TestFunction&& testFunc) {
        try {
            testFunc();
            NLM_LOG_INFO("[PASS] " + testName);
            return true;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("[FAIL] " + testName + ": " + std::string(e.what()));
            return false;
        }
    }
    
    template<typename TestFunction>
    static bool runTest(const std::string& testName, TestFunction&& testFunc,
                       const std::function<void()>& setupFunc,
                       const std::function<void()>& teardownFunc) {
        try {
            setupFunc();
            testFunc();
            teardownFunc();
            NLM_LOG_INFO("[PASS] " + testName);
            return true;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("[FAIL] " + testName + ": " + std::string(e.what()));
            return false;
        }
    }
};

// Factory class for creating experiment components
class ExperimentFactory {
public:
    static std::shared_ptr<Brain> createBrain(const std::shared_ptr<Config>& config = nullptr) {
        return std::make_shared<Brain>(config ? config : ConfigBuilder::createDefaultConfig());
    }
    
    static std::shared_ptr<SimpleWorld> createWorld(float width = 20.0f, float height = 20.0f,
                                                   uint32_t cols = 16, uint32_t rows = 16) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, cols, rows);
        return world;
    }
    
    static std::unique_ptr<AgentBrain> createAgentBrain(const std::shared_ptr<Brain>& brain) {
        return std::make_unique<AgentBrain>(brain);
    }
};

} // namespace nlm

#endif // NLM_EXPERIMENT_UTILS_H