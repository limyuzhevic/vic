#ifndef NLM_BRAIN_CORE_HPP
#define NLM_BRAIN_CORE_HPP

#include "../core/Config/Config.hpp"
#include "Brain.hpp"
#include "AgentBrain.hpp"
#include "SimpleWorld.hpp"
#include "SensoryPercept.hpp"
#include "Action.hpp"
#include <memory>

namespace nlm {

// Factory functions for Phase 6 integration

// Create a default configuration optimized for Phase 6
inline std::shared_ptr<Config> createDefaultConfig() {
    auto config = std::make_shared<Config>();
    
    // Basic brain configuration
    config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    
    // Simulation parameters
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    
    // Memory system configuration
    config->set("working_memory_capacity", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("episodic_memory_max_episodes", static_cast<int64_t>(1000), ConfigSource::Default);
    config->set("associative_memory_capacity", static_cast<int64_t>(500), ConfigSource::Default);
    
    // Neuromodulation configuration
    config->set("neuromod.dopamine.scale", 1.0f, ConfigSource::Default);
    config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
    config->set("neuromod.novelty.enable", true, ConfigSource::Default);
    
    // Plasticity configuration
    config->set("plasticity.stdp.enable", true, ConfigSource::Default);
    config->set("plasticity.stdp.learning_rate", 0.01f, ConfigSource::Default);
    config->set("plasticity.stdp.tau_plus", 20.0f, ConfigSource::Default);
    config->set("plasticity.stdp.tau_minus", 20.0f, ConfigSource::Default);
    config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
    config->set("plasticity.structural.enable", true, ConfigSource::Default);
    
    // Development configuration
    config->set("development.initial_plasticity", 1.0f, ConfigSource::Default);
    config->set("development.critical_period_end", 300.0, ConfigSource::Default);
    config->set("development.maturation_end", 900.0, ConfigSource::Default);
    
    // Prediction system configuration
    config->set("prediction.prediction_horizon", 10, ConfigSource::Default);
    config->set("prediction.error_threshold", 0.1f, ConfigSource::Default);
    
    // Cognitive system configuration
    config->set("attention.inhibition_strength", 0.5f, ConfigSource::Default);
    config->set("attention.excitation_strength", 1.5f, ConfigSource::Default);
    config->set("planner.planning_depth", 5, ConfigSource::Default);
    
    // Agent configuration
    config->set("agent.reward_modulation_enabled", true, ConfigSource::Default);
    config->set("agent.structural_plasticity_enabled", true, ConfigSource::Default);
    config->set("agent.development_enabled", true, ConfigSource::Default);
    config->set("agent.curiosity_enabled", true, ConfigSource::Default);
    
    return config;
}

// Create brain with configuration
inline std::shared_ptr<Brain> createBrain(std::shared_ptr<Config> config = nullptr) {
    if (!config) {
        config = createDefaultConfig();
    }
    return std::make_shared<Brain>(config);
}

// Create simple world
inline std::unique_ptr<SimpleWorld> createSimpleWorld() {
    return std::make_unique<SimpleWorld>();
}

// Create agent brain interface
inline std::unique_ptr<AgentBrain> createAgentBrain(std::shared_ptr<Brain> brain) {
    if (!brain) {
        return nullptr;
    }
    return std::make_unique<AgentBrain>(brain);
}

// Create agent body
inline std::unique_ptr<AgentBody> createAgentBody(std::shared_ptr<Brain> brain) {
    if (!brain) {
        return nullptr;
    }
    return std::make_unique<AgentBody>(brain);
}

// Create complete agent system (brain + world + agent)
inline std::tuple<std::shared_ptr<Brain>, std::unique_ptr<SimpleWorld>, std::unique_ptr<AgentBrain>>
createAgentSystem(std::shared_ptr<Config> config = nullptr) {
    auto brain = createBrain(config);
    auto world = createSimpleWorld();
    auto agent = createAgentBrain(brain);
    
    if (agent) {
        agent->initialize(*world);
    }
    
    return std::make_tuple(brain, world, agent);
}

// Create complete agent system with agent body
inline std::tuple<std::shared_ptr<Brain>, std::unique_ptr<SimpleWorld>, std::unique_ptr<AgentBrain>, std::unique_ptr<AgentBody>>
createAgentSystemWithBody(std::shared_ptr<Config> config = nullptr) {
    auto brain = createBrain(config);
    auto world = createSimpleWorld();
    auto agent = createAgentBrain(brain);
    auto agentBody = createAgentBody(brain);
    
    if (agent) {
        agent->initialize(*world);
    }
    
    if (agentBody) {
        agentBody->initialize();
    }
    
    return std::make_tuple(brain, world, agent, agentBody);
}

// Run complete simulation loop
inline void runSimulation(
    std::shared_ptr<Brain> brain,
    std::unique_ptr<SimpleWorld>& world,
    std::unique_ptr<AgentBrain>& agent,
    size_t steps,
    bool enableLogging = false
) {
    if (!brain || !world || !agent) {
        return;
    }
    
    agent->reset();
    world->reset();
    
    for (size_t step = 0; step < steps; ++step) {
        // Update world
        world->update(0.1);
        
        // Get sensory input
        auto percept = world->getSensoryPercept();
        
        // Process in agent
        agent->processSensoryInput(*percept);
        
        // Update brain
        brain->step(step);
        
        // Get action
        auto action = agent->decodeMotorCommand();
        
        // Apply action to world
        world->applyMotorCommand(action, world->getSimulationTime());
        
        // Apply reward modulation
        float reward = 0.0f;
        if (auto internal = percept->getInternal()) {
            reward = internal->get(0) ? internal->get(0).value() : 0.0f;
        }
        agent->applyRewardModulation(reward, 0.0);
        
        // Update development
        agent->updateDevelopment(0.1);
        
        if (enableLogging && step % 100 == 0) {
            // Log simulation progress
        }
    }
}

// Run complete simulation with agent body
inline void runSimulationWithBody(
    std::shared_ptr<Brain> brain,
    std::unique_ptr<SimpleWorld>& world,
    std::unique_ptr<AgentBrain>& agent,
    std::unique_ptr<AgentBody>& agentBody,
    size_t steps,
    bool enableLogging = false
) {
    if (!brain || !world || !agent || !agentBody) {
        return;
    }
    
    agent->reset();
    agentBody->reset();
    world->reset();
    
    for (size_t step = 0; step < steps; ++step) {
        // Update world
        world->update(0.1);
        
        // Get sensory input
        auto percept = world->getSensoryPercept();
        
        // Process in agent
        agent->processSensoryInput(*percept);
        
        // Process in agent body
        agentBody->processSensoryInput(*percept);
        
        // Update brain
        brain->step(step);
        
        // Get action
        auto action = agent->decodeMotorCommand();
        
        // Apply action to world
        world->applyMotorCommand(action, world->getSimulationTime());
        
        // Apply reward modulation
        float reward = 0.0f;
        if (auto internal = percept->getInternal()) {
            reward = internal->get(0) ? internal->get(0).value() : 0.0f;
        }
        agent->applyRewardModulation(reward, 0.0);
        
        // Update development
        agent->updateDevelopment(0.1);
        agentBody->updateDevelopment(0.1);
        
        if (enableLogging && step % 100 == 0) {
            // Log simulation progress
        }
    }
}

// Performance benchmark
inline double benchmarkPerformance(
    std::shared_ptr<Brain> brain,
    size_t steps,
    size_t neuronCount = 0
) {
    if (!brain) {
        return 0.0;
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (size_t step = 0; step < steps; ++step) {
        brain->step(step, step * 0.001);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);
    
    return duration.count();
}

// Stress test with different network sizes
inline std::map<size_t, double> stressTest(
    const std::vector<size_t>& neuronCounts,
    size_t stepsPerTest,
    bool warmup = true
) {
    std::map<size_t, double> results;
    
    for (size_t neuronCount : neuronCounts) {
        auto config = createDefaultConfig();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Programmatic);
        
        auto brain = createBrain(config);
        if (brain->initialize()) {
            if (warmup) {
                // Warmup run
                for (size_t step = 0; step < 100; ++step) {
                    brain->step(step, step * 0.001);
                }
            }
            
            double time = benchmarkPerformance(brain, stepsPerTest, neuronCount);
            results[neuronCount] = time;
        }
    }
    
    return results;
}

// Memory usage monitoring
inline struct {
    size_t neurons;
    size_t synapses;
    float memoryUsageMB;
    float spikeRate;
    float firingRate;
} getBrainStats(std::shared_ptr<Brain> brain) {
    struct Stats {
        size_t neurons;
        size_t synapses;
        float memoryUsageMB;
        float spikeRate;
        float firingRate;
    } stats = {0, 0, 0.0f, 0.0f, 0.0f};
    
    if (!brain) {
        return stats;
    }
    
    stats.neurons = brain->getTotalNeuronCount();
    stats.synapses = brain->getTotalSynapseCount();
    
    // Estimate memory usage (rough approximation)
    stats.memoryUsageMB = (stats.neurons * sizeof(float) + stats.synapses * sizeof(float)) / (1024.0 * 1024.0);
    
    stats.spikeRate = static_cast<float>(brain->getTotalSpikeCount()) / 1000.0f; // Per 1000 steps
    stats.firingRate = brain->getAverageFiringRate();
    
    return stats;
}

} // namespace nlm

#endif // NLM_BRAIN_CORE_HPP
