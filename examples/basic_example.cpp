#!/usr/bin/env bash
"""
NLM Basic C++ Example

This example demonstrates the basic usage of the NLM C++ API for neural simulation.

Building:
- Compile with: g++ -std=c++20 -I./src examples/basic_example.cpp -o example
- Or use CMake: mkdir build && cd build && cmake .. && make

Running:
- ./example
- ./nlm (for the main NLM executable)
- ./nlm_phase6_demo (for Phase 6 integration demo)
"""

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "environment/Environment.hpp"
#include "agent/AgentBrain.hpp"
#include <iostream>
#include <iomanip>

using namespace nlm;

void print_banner() {
    std::cout << R"(
╔═══════════════════════════════════════════════════════════════╗
║                                                               ║
║     NLM — 熙然                                                ║
║     Neural Learning Machine                                   ║
║                                                               ║
║     Phase 6 Integration Demo                                 ║
║                                                               ║
║     This example demonstrates basic NLM usage.                ║
║                                                               ║
╚═══════════════════════════════════════════════════════════════╝
)" << std::endl;
}

void print_usage() {
    std::cout << "Usage: example [OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --neurons N     Number of neurons (default: 1000)" << std::endl;
    std::cout << "  --steps S        Maximum simulation steps (default: 1000)" << std::endl;
    std::cout << "  --quick          Run quick test only" << std::endl;
    std::cout << "  --help           Show this help message" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  ./example --neurons 500 --steps 2000" << std::endl;
    std::cout << "  ./example --quick" << std::endl;
}

void demonstrate_basic_api() {
    std::cout << "\n=== Basic NLM API Demonstration ===" << std::endl;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    config->set("connection_probability", 0.15);
    config->set("simulation_timestep", 0.001);
    config->set("random_seed", 42);
    
    std::cout << "Created configuration with " << config->getOr<int64_t>("neuron_count", 0) 
              << " neurons" << std::endl;
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    
    std::cout << "Created brain with " << brain->getTotalNeuronCount() << " neurons" << std::endl;
    std::cout << "Total synapses: " << brain->getTotalSynapseCount() << std::endl;
    
    // Initialize brain
    if (brain->initialize()) {
        std::cout << "Brain initialized successfully" << std::endl;
        
        // Run a few simulation steps
        std::cout << "Running simulation steps..." << std::endl;
        for (size_t step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001);
            
            if (step % 10 == 0) {
                std::cout << "  Step " << step << ": " 
                         << "firing rate: " << std::fixed << std::setprecision(3) 
                         << brain->getAverageFiringRate() 
                         << ", spikes: " << brain->getTotalSpikeCount() << std::endl;
            }
        }
        
        // Display brain statistics
        std::cout << "\nBrain Statistics:" << std::endl;
        std::cout << "  Total neurons: " << brain->getTotalNeuronCount() << std::endl;
        std::cout << "  Total synapses: " << brain->getTotalSynapseCount() << std::endl;
        std::cout << "  Active neurons: " << brain->getActiveNeuronCount() << std::endl;
        std::cout << "  Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
        std::cout << "  Average firing rate: " << std::fixed << std::setprecision(3) 
                  << brain->getAverageFiringRate() << std::endl;
        std::cout << "  Excitation/inhibition ratio: " << std::setprecision(3) 
                  << brain->getExcitationInhibitionRatio() << std::endl;
        std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        
        // Test configuration
        std::cout << "\nConfiguration Test:" << std::endl;
        std::cout << "  Has neuron_count: " << (brain->getConfig()->has("neuron_count") ? "Yes" : "No") << std::endl;
        std::cout << "  Has region_count: " << (brain->getConfig()->has("region_count") ? "Yes" : "No") << std::endl;
        std::cout << "  Neuron count: " << brain->getConfig()->getOr<int64_t>("neuron_count", 0) << std::endl;
        
    } else {
        std::cout << "Failed to initialize brain" << std::endl;
    }
}

void demonstrate_environment_agent() {
    std::cout << "\n=== Environment and Agent Brain Demonstration ===" << std::endl;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 50);
    config->set("region_count", 1);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cout << "Failed to initialize brain" << std::endl;
        return;
    }
    
    // Create environment
    auto world = std::make_shared<SimpleWorld>();
    world->configure(16, 16, 8, 8);  // width, height, vision width, vision height
    world->setAgentStart(8, 8);
    
    // Create agent brain
    auto agent = std::make_shared<AgentBrain>(brain);
    agent->initialize(*world);
    
    // Display agent information
    std::cout << "Environment created: " << world->getWidth() << " x " << world->getHeight() << " cells" << std::endl;
    std::cout << "Agent brain initialized successfully" << std::endl;
    std::cout << "Agent sensory input size: " << agent->getSensoryInputSize() << std::endl;
    std::cout << "Agent motor output size: " << agent->getMotorOutputSize() << std::endl;
    
    // Run a simple simulation
    std::cout << "\nRunning environment-agent simulation..." << std::endl;
    for (size_t step = 0; step < 20; ++step) {
        // Get sensory percept
        auto percept = world->getSensoryPercept();
        
        // Process sensory input
        agent->processSensoryInput(percept);
        
        // Brain step
        brain->step(step, step * 0.001);
        
        // Get motor command
        auto cmd = agent->decodeMotorCommand();
        
        // Apply action to world
        world->applyMotorCommand(cmd, step * 0.001);
        
        // Display progress
        if (step % 5 == 0) {
            auto body = world->getAgentBody();
            std::cout << "  Step " << step << ": Agent at (" << body->x << ", " << body->y 
                     << "), energy: " << body->energy << std::endl;
        }
    }
    
    // Display final world state
    std::cout << "\nFinal World State:" << std::endl;
    std::cout << "  Width: " << world->getWidth() << " x Height: " << world->getHeight() << std::endl;
    std::cout << "  Agent position: (" << world->getAgentBody()->x << ", " 
              << world->getAgentBody()->y << ")" << std::endl;
    std::cout << "  Simulation time: " << world->getSimulationTime() << " s" << std::endl;
}

void demonstrate_error_handling() {
    std::cout << "\n=== Error Handling Demonstration ===" << std::endl;
    
    // Try to create a brain with invalid configuration
    auto config = std::make_shared<Config>();
    
    std::cout << "Testing configuration validation..." << std::endl;
    
    // Set various values
    config->set("neuron_count", 10);
    config->set("region_count", 1);
    config->set("connection_probability", 0.2);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    
    if (brain->initialize()) {
        std::cout << "Brain initialized successfully" << std::endl;
        
        // Test error conditions
        std::cout << "\nTesting error conditions..." << std::endl;
        
        // Try to get non-existent configuration key
        auto nonExistent = config->getOr<std::string>("non_existent_key", "default_value");
        std::cout << "Non-existent key: '" << nonExistent << "'" << std::endl;
        
        // Try to remove a key
        config->remove("non_existent_key");
        std::cout << "Removed non-existent key (no effect)" << std::endl;
        
        // Check if key exists
        bool hasKey = config->has("neuron_count");
        std::cout << "Has neuron_count: " << (hasKey ? "Yes" : "No") << std::endl;
        
        // Get all keys
        auto keys = config->getKeys();
        std::cout << "Configuration keys: ";
        for (const auto& key : keys) {
            std::cout << key << " ";
        }
        std::cout << std::endl;
        
    } else {
        std::cout << "Failed to initialize brain" << std::endl;
    }
}

void run_quick_test() {
    std::cout << "=== Quick NLM Test ===" << std::endl;
    
    // Create minimal configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 20);
    config->set("region_count", 1);
    config->set("simulation_timestep", 0.001);
    config->set("random_seed", 42);
    
    // Create and initialize brain
    auto brain = std::make_shared<Brain>(config);
    if (brain->initialize()) {
        std::cout << "Brain initialized successfully" << std::endl;
        
        // Run a few steps
        for (size_t step = 0; step < 10; ++step) {
            brain->step(step, step * 0.001);
        }
        
        std::cout << "Quick test completed successfully" << std::endl;
        std::cout << "Final stats: " << brain->getTotalSpikeCount() << " spikes, "
                  << brain->getAverageFiringRate() << " avg firing rate" << std::endl;
        
    } else {
        std::cout << "Quick test failed" << std::endl;
    }
}

int main(int argc, char** argv) {
    print_banner();
    
    // Parse command line arguments
    bool quick_mode = false;
    int neuron_count = 1000;
    int steps = 1000;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--help") {
            print_usage();
            return 0;
        } else if (arg == "--quick") {
            quick_mode = true;
        } else if (arg == "--neurons" && i + 1 < argc) {
            neuron_count = std::atoi(argv[++i]);
        } else if (arg == "--steps" && i + 1 < argc) {
            steps = std::atoi(argv[++i]);
        }
    }
    
    if (quick_mode) {
        run_quick_test();
    } else {
        demonstrate_basic_api();
        demonstrate_environment_agent();
        demonstrate_error_handling();
    }
    
    std::cout << "\n=== Example completed ===" << std::endl;
    return 0;
}
