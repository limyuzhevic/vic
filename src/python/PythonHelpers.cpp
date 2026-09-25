// NLM Python Helper Functions Implementation
// Provides convenient factory functions for creating common NLM objects

#include "python/include/nlm/PythonHelpers.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "benchmark/Benchmark.hpp"
#include <memory>
#include <unordered_map>
#include <chrono>

namespace nlm {

// Forward declarations for helper function implementations
std::shared_ptr<Config> createStandardConfig();
std::shared_ptr<Brain> createTestBrain();
BenchmarkResult runBenchmark(std::shared_ptr<Brain> brain, 
                           std::shared_ptr<SimpleWorld> world, 
                           std::shared_ptr<AgentBrain> agent, 
                           SimulationStep steps);

// Create a standard configuration for common use cases
std::shared_ptr<Config> createStandardConfig() {
    auto config = std::make_shared<Config>();
    
    // Set reasonable defaults for a typical neural network
    config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("random_seed", 42, ConfigSource::Default);
    
    // Memory system defaults
    config->set("working_memory_capacity", 100, ConfigSource::Default);
    config->set("max_episodes", 1000, ConfigSource::Default);
    
    // Cognitive system defaults
    config->set("planning_depth", 5, ConfigSource::Default);
    config->set("inhibition_strength", 0.5f, ConfigSource::Default);
    config->set("excitation_strength", 1.5f, ConfigSource::Default);
    
    return config;
}

// Create a test brain with default configuration
std::shared_ptr<Brain> createTestBrain() {
    auto config = createStandardConfig();
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    return brain;
}

// Run benchmark and return performance metrics
BenchmarkResult runBenchmark(std::shared_ptr<Brain> brain, 
                           std::shared_ptr<SimpleWorld> world, 
                           std::shared_ptr<AgentBrain> agent, 
                           SimulationStep steps) {
    BenchmarkResult result;
    
    // Initialize the brain if needed
    if (!brain->getTotalNeuronCount()) {
        brain->initialize();
    }
    
    // Run simulation steps with world and agent integration
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (SimulationStep step = 0; step < steps; ++step) {
        double currentTime = static_cast<double>(step) * 0.001;
        
        // Get sensory percept from world
        const SensoryPercept& percept = world->getSensoryPercept();
        
        // Process sensory input through agent
        if (agent) {
            agent->processSensoryInput(percept);
        }
        
        // Perform brain step
        brain->step(step, currentTime);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    // Collect results
    result.neurons = brain->getTotalNeuronCount();
    result.synapses = brain->getTotalSynapseCount();
    result.steps = steps;
    result.totalTime = totalTime;
    result.stepsPerSecond = steps / totalTime;
    result.spikesPerStep = static_cast<double>(brain->getTotalSpikeCount()) / steps;
    result.synapsesPerNeuron = static_cast<double>(result.synapses) / result.neurons;
    
    return result;
}

} // namespace nlm
