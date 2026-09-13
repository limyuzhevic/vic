#!/usr/bin/env g++
// Example script showing the simplified usage patterns
// This file demonstrates easy entry points for beginners and advanced users

#include <iostream>
#include <memory>
#include <string>
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"

// Example 1: Ultra-simple brain test (beginner level)
void beginnerExample() {
    std::cout << "=== Beginner Example: Simplest Brain Test ===" << std::endl;
    
    auto config = std::make_shared<Config>();
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize!" << std::endl;
        return;
    }
    
    std::cout << "Brain created and initialized successfully!" << std::endl;
    std::cout << "Total neurons: " << brain->getTotalNeuronCount() << std::endl;
    
    // Quick simulation
    for (int i = 0; i < 50; ++i) {
        brain->step(i);
    }
    
    std::cout << "Simulation completed. Spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "" << std::endl;
}

// Example 2: Complete agent workflow (medium complexity)
void mediumExample() {
    std::cout << "=== Medium Example: Complete Agent Workflow ===" << std::endl;
    
    // 1. Create brain
    auto config = std::make_shared<Config>();
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // 2. Create world
    SimpleWorld world;
    world.configure(20, 20, 8, 8);
    world.reset();
    world.setAgentStart(10.0, 10.0);
    
    // 3. Create agent interface
    AgentBrain agent(brain);
    agent.initialize(world);
    agent.enableRewardModulation(true);
    agent.enableDevelopment(true);
    
    // 4. Run simulation
    for (int step = 0; step < 100; ++step) {
        world.update(0.1);
        agent.processSensoryInput(world.getSensoryPercept());
        brain->step(step);
        
        MotorCommand action = agent.decodeMotorCommand();
        world.applyMotorCommand(action, world.getSimulationTime());
        
        float reward = world.getEnergy() / 100.0f;
        agent.applyRewardModulation(reward, 0.0f);
        
        if (step % 25 == 0) {
            std::cout << "Step " << step << ": " 
                      << brain->getFiringNeuronCount() << " firing neurons" << std::endl;
        }
    }
    
    std::cout << "Agent simulation completed successfully!" << std::endl;
    std::cout << "" << std::endl;
}

// Example 3: Advanced development scenario
void advancedExample() {
    std::cout << "=== Advanced Example: Development & Learning ===" << std::endl;
    
    // Configure for development scenario
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 500);
    config->set("region_count", 2);
    config->set("connection_probability", 0.15f);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Enable all subsystems
    brain->getWorkingMemory()->enablePlasticity(true);
    brain->getEpisodicMemory()->enableReplay(true);
    brain->getDevelopmentSystem()->setMaturationSpeed(2.0f);
    
    std::cout << "Advanced brain configured with development enabled." << std::endl;
    
    // Simulate development over time
    for (int step = 0; step < 500; ++step) {
        // Phase 1: Initial learning (high plasticity)
        brain->step(step);
        
        // Phase 2: Development effects
        if (step == 200) {
            brain->getDevelopmentSystem()->triggerMaturationEvent();
            std::cout << "Development stage: Maturation triggered" << std::endl;
        }
        
        // Phase 3: Consolidation
        if (step == 400) {
            brain->getEpisodicMemory()->consolidate(0.5f);
            std::cout << "Memory consolidation completed" << std::endl;
        }
        
        if (step % 100 == 0) {
            std::cout << "Step " << step << ": " 
                      << brain->getAverageFiringRate() << " avg firing rate" << std::endl;
        }
    }
    
    std::cout << "Advanced simulation with development complete!" << std::endl;
    std::cout << "" << std::endl;
}

// Example 4: Performance benchmark
void performanceExample() {
    std::cout << "=== Performance Example: Benchmark ===" << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    std::cout << "Running performance benchmark with " << config->getOr<int64_t>("neuron_count", 1000) 
              << " neurons..." << std::endl;
    
    // Warm-up
    for (int i = 0; i < 50; ++i) brain->step(i);
    
    // Benchmark
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 50; i < 1000; ++i) {
        brain->step(i);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(end - start);
    
    std::cout << "Benchmark completed in " << duration.count() << " seconds" << std::endl;
    std::cout << "Average steps per second: " << 950.0 / duration.count() << std::endl;
    std::cout << "" << std::endl;
}

int main() {
    std::cout << "=== NLM Usage Examples ===" << std::endl;
    std::cout << "Demonstrating various usage patterns for different skill levels" << std::endl;
    std::cout << "" << std::endl;
    
    // Initialize logging
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Run examples
    beginnerExample();
    mediumExample();
    advancedExample();
    performanceExample();
    
    std::cout << "=== All Examples Completed Successfully ===" << std::endl;
    return 0;
}
