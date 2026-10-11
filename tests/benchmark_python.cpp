// Copyright (c) 2024 NLM Project
// Licensed under the MIT License

/**
 * @file benchmark_python.cpp
 * @brief Performance benchmarks for NLM Python bindings
 * 
 * This file contains performance benchmarks for the enhanced Python bindings,
 * measuring execution time and efficiency of various operations.
 */

#include <pybind11/pybind11.h>
#include <pybind11/embed.h>
#include <iostream>
#include <chrono>
#include <vector>
#include <map>

namespace benchmark_python {

using namespace nlm;

// Simple timer utility
class Timer {
public:
    Timer() : start_(std::chrono::high_resolution_clock::now()) {}
    
    double elapsed() const {
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::microseconds>(end - start_).count() / 1000000.0;
    }
    
    void reset() {
        start_ = std::chrono::high_resolution_clock::now();
    }
    
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start_;
};

// Benchmark configuration creation
void benchmark_config_creation() {
    std::cout << "[BENCHMARK] Configuration Creation" << std::endl;
    
    Timer timer;
    
    // Benchmark default config creation
    int iterations = 1000;
    for (int i = 0; i < iterations; ++i) {
        auto config = std::make_shared<ConfigEnhanced>();
        config->setParameter("brain.neuron_count", std::to_string(1000 + i));
        config->setParameter("brain.synapse_density", std::to_string(0.1 + i * 0.001));
    }
    
    double avg_time = timer.elapsed() / iterations;
    std::cout << "  Average time per config: " << avg_time * 1000000 << " microseconds" << std::endl;
    std::cout << "  Configs per second: " << iterations / timer.elapsed() << std::endl;
}

// Benchmark parameter setting
void benchmark_parameter_setting() {
    std::cout << "[BENCHMARK] Parameter Setting" << std::endl;
    
    Timer timer;
    
    int iterations = 10000;
    auto config = std::make_shared<ConfigEnhanced>();
    
    for (int i = 0; i < iterations; ++i) {
        config->setParameter("brain.neuron_count", std::to_string(i));
        config->setParameter("brain.synapse_density", std::to_string(0.1 + i * 0.0001));
        config->setParameter("plasticity.stdp.enable", (i % 2 == 0) ? "true" : "false");
    }
    
    double avg_time = timer.elapsed() / iterations;
    std::cout << "  Average time per parameter set: " << avg_time * 1000000 << " microseconds" << std::endl;
    std::cout << "  Operations per second: " << iterations / timer.elapsed() << std::endl;
}

// Benchmark brain initialization
void benchmark_brain_initialization() {
    std::cout << "[BENCHMARK] Brain Initialization" << std::endl;
    
    int iterations = 100;
    double total_time = 0.0;
    
    for (int i = 0; i < iterations; ++i) {
        Timer timer;
        
        auto config = std::make_shared<ConfigEnhanced>();
        config->setParameter("brain.neuron_count", "500");
        config->setParameter("brain.region_count", "2");
        config->setParameter("plasticity.stdp.enable", "true");
        
        auto brain = BrainEnhanced::create(config);
        brain->initialize();
        
        total_time += timer.elapsed();
    }
    
    double avg_time = total_time / iterations;
    std::cout << "  Average initialization time: " << avg_time * 1000 << " milliseconds" << std::endl;
    std::cout << "  Brains per minute: " << 60.0 / avg_time << std::endl;
}

// Benchmark brain stepping
void benchmark_brain_stepping() {
    std::cout << "[BENCHMARK] Brain Stepping" << std::endl;
    
    int iterations = 1000;
    double total_time = 0.0;
    
    for (int i = 0; i < iterations; ++i) {
        Timer timer;
        
        auto config = std::make_shared<ConfigEnhanced>();
        config->setParameter("brain.neuron_count", "200");
        
        auto brain = BrainEnhanced::create(config);
        brain->initialize();
        
        for (int step = 0; step < 50; ++step) {
            brain->step(step);
        }
        
        total_time += timer.elapsed();
    }
    
    double avg_time = total_time / iterations;
    std::cout << "  Average 50-step simulation: " << avg_time * 1000 << " milliseconds" << std::endl;
    std::cout << "  Steps per second: " << (50.0 / avg_time) * iterations << std::endl;
}

// Benchmark batch operations
void benchmark_batch_operations() {
    std::cout << "[BENCHMARK] Batch Operations" << std::endl;
    
    int iterations = 100;
    double total_time = 0.0;
    
    for (int i = 0; i < iterations; ++i) {
        Timer timer;
        
        auto config = std::make_shared<ConfigEnhanced>();
        config->setParameter("brain.neuron_count", "300");
        
        auto brain = BrainEnhanced::create(config);
        brain->initialize();
        
        // Test batch step
        brain->stepRange(0, 99, 0.001);
        
        // Test batch simulation
        brain->runSimulation(100, 0.001);
        
        total_time += timer.elapsed();
    }
    
    double avg_time = total_time / iterations;
    std::cout << "  Average batch operation: " << avg_time * 1000 << " milliseconds" << std::endl;
}

// Benchmark agent simulation
void benchmark_agent_simulation() {
    std::cout << "[BENCHMARK] Agent Simulation" << std::endl;
    
    int iterations = 50;
    double total_time = 0.0;
    
    for (int i = 0; i < iterations; ++i) {
        Timer timer;
        
        auto config = std::make_shared<ConfigEnhanced>();
        config->setParameter("brain.neuron_count", "500");
        config->setParameter("neuromod.dopamine.scale", "1.5");
        
        auto brain = BrainEnhanced::create(config);
        brain->initialize();
        
        auto agent = std::make_shared<AgentBrainEnhanced>(brain);
        
        auto world = std::make_shared<SimpleWorldEnhanced>();
        world->configure(20, 20, 8, 8);
        world->reset();
        
        agent->initialize(world);
        
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        
        // Run simulation steps
        for (int step = 0; step < 100; ++step) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(*action, world->getSimulationTime());
            
            agent->applyRewardModulation(0.1, 0.0);
        }
        
        total_time += timer.elapsed();
    }
    
    double avg_time = total_time / iterations;
    std::cout << "  Average agent simulation (100 steps): " << avg_time * 1000 << " milliseconds" << std::endl;
    std::cout << "  Agent steps per second: " << (100.0 / avg_time) * iterations << std::endl;
}

// Benchmark memory allocation
void benchmark_memory_allocation() {
    std::cout << "[BENCHMARK] Memory Allocation" << std::endl;
    
    Timer timer;
    
    int iterations = 1000;
    std::vector<std::shared_ptr<ConfigEnhanced>> configs;
    std::vector<std::shared_ptr<BrainEnhanced>> brains;
    std::vector<std::shared_ptr<AgentBrainEnhanced>> agents;
    std::vector<std::shared_ptr<SimpleWorldEnhanced>> worlds;
    
    for (int i = 0; i < iterations; ++i) {
        auto config = std::make_shared<ConfigEnhanced>();
        config->setParameter("brain.neuron_count", std::to_string(100 + i));
        configs.push_back(config);
        
        auto brain = BrainEnhanced::create(config);
        brain->initialize();
        brains.push_back(brain);
        
        auto agent = std::make_shared<AgentBrainEnhanced>(brain);
        agents.push_back(agent);
        
        auto world = std::make_shared<SimpleWorldEnhanced>();
        world->configure(10, 10, 5, 5);
        worlds.push_back(world);
    }
    
    double avg_time = timer.elapsed() / iterations;
    std::cout << "  Average object creation: " << avg_time * 1000000 << " microseconds" << std::endl;
    std::cout << "  Objects per second: " << iterations / timer.elapsed() << std::endl;
}

// Benchmark complete workflow
void benchmark_complete_workflow() {
    std::cout << "[BENCHMARK] Complete Workflow" << std::endl;
    
    int iterations = 20;
    double total_time = 0.0;
    
    for (int i = 0; i < iterations; ++i) {
        Timer timer;
        
        // Complete workflow: config -> brain -> agent -> world -> simulation
        auto config = std::make_shared<ConfigEnhanced>();
        config->setParameter("brain.neuron_count", "400");
        config->setParameter("plasticity.stdp.enable", "true");
        config->setParameter("neuromod.curiosity.enable", "true");
        
        auto brain = BrainEnhanced::create(config);
        brain->initialize();
        
        auto agent = std::make_shared<AgentBrainEnhanced>(brain);
        
        auto world = std::make_shared<SimpleWorldEnhanced>();
        world->configure(25, 25, 8, 8);
        world->reset();
        
        agent->initialize(world);
        
        agent->enableLearning(true);
        agent->enableSubsystem("reward_modulation", true);
        
        for (int step = 0; step < 150; ++step) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(*action, world->getSimulationTime());
            
            agent->applyRewardModulation(0.1, 0.0);
            agent->updateDevelopment(0.1);
        }
        
        total_time += timer.elapsed();
    }
    
    double avg_time = total_time / iterations;
    std::cout << "  Average complete workflow (150 steps): " << avg_time * 1000 << " milliseconds" << std::endl;
    std::cout << "  Workflows per hour: " << (3600.0 / avg_time) * iterations << std::endl;
}

// Run all benchmarks
void run_all_benchmarks() {
    std::cout << "=== NLM Python Bindings Performance Benchmarks ===" << std::endl;
    std::cout << "Benchmarking enhanced Python bindings for NLM neural simulation framework." << std::endl;
    std::cout << std::endl;
    
    benchmark_config_creation();
    std::cout << std::endl;
    
    benchmark_parameter_setting();
    std::cout << std::endl;
    
    benchmark_brain_initialization();
    std::cout << std::endl;
    
    benchmark_brain_stepping();
    std::cout << std::endl;
    
    benchmark_batch_operations();
    std::cout << std::endl;
    
    benchmark_agent_simulation();
    std::cout << std::endl;
    
    benchmark_memory_allocation();
    std::cout << std::endl;
    
    benchmark_complete_workflow();
    
    std::cout << std::endl;
    std::cout << "=== Benchmark Summary ===" << std::endl;
    std::cout << "Performance benchmarks completed successfully!" << std::endl;
    std::cout << "All benchmarks are designed to help users understand performance characteristics" << std::endl;
    std::cout << "and optimize their NLM applications for specific use cases." << std::endl;
}

} // namespace benchmark_python

// Entry point for benchmark execution
int main() {
    benchmark_python::run_all_benchmarks();
    return 0;
}
