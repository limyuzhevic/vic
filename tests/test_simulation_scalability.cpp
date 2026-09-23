// Simulation Scalability Tests
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <vector>
#include <chrono>

namespace test_simulation_scalability {

void testSimulationScalabilityBasic() {
    // Test with small simulation
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::AgentBrain agent;
    agent.initialize(&brain);
    
    // Run small simulation
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
        
        // Agent processes and acts
        nlm::MotorCommand action = agent.decodeMotorCommand();
        assert(action != nullptr);
        
        if (step >= 100) {
            break;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << "    testSimulationScalabilityBasic passed (100 steps in " 
              << duration.count() << " ms)" << std::endl;
}

void testSimulationScalabilityMedium() {
    // Test with medium simulation
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::AgentBrain agent;
    agent.initialize(&brain);
    
    // Run medium simulation
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (nlm::SimulationStep step = 0; step < 500; ++step) {
        brain.step(step);
        
        nlm::MotorCommand action = agent.decodeMotorCommand();
        assert(action != nullptr);
        
        if (step >= 500) {
            break;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << "    testSimulationScalabilityMedium passed (500 steps in " 
              << duration.count() << " ms)" << std::endl;
}

void testSimulationScalabilityLarge() {
    // Test with large simulation
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(5), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::AgentBrain agent;
    agent.initialize(&brain);
    
    // Run large simulation
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (nlm::SimulationStep step = 0; step < 1000; ++step) {
        brain.step(step);
        
        nlm::MotorCommand action = agent.decodeMotorCommand();
        assert(action != nullptr);
        
        if (step >= 1000) {
            break;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << "    testSimulationScalabilityLarge passed (1000 steps in " 
              << duration.count() << " ms)" << std::endl;
}

void testSimulationScalabilityWithMemory() {
    // Test scalability with memory systems enabled
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::AgentBrain agent;
    agent.initialize(&brain);
    
    // Get memory systems
    auto* workingMem = brain.getWorkingMemory();
    auto* episodicMem = brain.getEpisodicMemory();
    
    // Run simulation with memory updates
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (nlm::SimulationStep step = 0; step < 800; ++step) {
        brain.step(step);
        
        // Update memory systems
        if (workingMem) {
            workingMem->update(0.001);
        }
        if (episodicMem) {
            episodicMem->update(0.001);
        }
        
        nlm::MotorCommand action = agent.decodeMotorCommand();
        assert(action != nullptr);
        
        if (step >= 800) {
            break;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << "    testSimulationScalabilityWithMemory passed (800 steps with memory in " 
              << duration.count() << " ms)" << std::endl;
}

void testSimulationScalabilityMultipleAgents() {
    // Test scalability with multiple agents
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    std::vector<nlm::AgentBrain> agents;
    for (int i = 0; i < 3; ++i) {
        nlm::AgentBrain agent;
        agent.initialize(&brain);
        agents.push_back(std::move(agent));
    }
    
    // Run simulation with multiple agents
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (nlm::SimulationStep step = 0; step < 300; ++step) {
        brain.step(step);
        
        // Each agent takes action
        for (auto& agent : agents) {
            nlm::MotorCommand action = agent.decodeMotorCommand();
            assert(action != nullptr);
        }
        
        if (step >= 300) {
            break;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << "    testSimulationScalabilityMultipleAgents passed (300 steps with 3 agents in " 
              << duration.count() << " ms)" << std::endl;
}

void runAll() {
    std::cout << "Running simulation scalability tests..." << std::endl;
    testSimulationScalabilityBasic();
    testSimulationScalabilityMedium();
    testSimulationScalabilityLarge();
    testSimulationScalabilityWithMemory();
    testSimulationScalabilityMultipleAgents();
}

} // namespace test_simulation_scalability