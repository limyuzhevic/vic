#include "SimulationCommand.hpp"
#include "nlm/core/Config/Config.hpp"
#include "nlm/brain/Brain.hpp"
#include "nlm/core/SimulationClock/SimulationClock.hpp"
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <filesystem>

namespace nlm {

bool SimulationCommand::executeSimulate(CLIContext& context, const std::map<std::string, std::string>& options) {
    std::cout << "=== Running Simulation ===" << std::endl;
    
    // Parse simulation parameters
    std::string configFile = "configs/default.cfg";
    size_t steps = 1000;
    bool headless = false;
    
    auto it = options.find("config");
    if (it != options.end()) {
        configFile = it->second;
    }
    
    it = options.find("steps");
    if (it != options.end()) {
        steps = std::stoul(it->second);
    }
    
    it = options.find("headless");
    if (it != options.end() && it->second == "true") {
        headless = true;
    }
    
    std::cout << "Configuration file: " << configFile << std::endl;
    std::cout << "Steps: " << steps << std::endl;
    std::cout << "Headless mode: " << (headless ? "Yes" : "No") << std::endl;
    
    // Create config object if not exists
    if (!context.config) {
        context.config = std::make_shared<Config>();
    }
    
    // Load configuration from file
    if (!configFile.empty() && std::filesystem::exists(configFile)) {
        std::cout << "Loading configuration from: " << configFile << std::endl;
        if (!context.config->loadFromFile(configFile)) {
            std::cout << "Failed to load configuration, using defaults" << std::endl;
        }
    }
    
    // Set default simulation parameters
    context.config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    context.config->set("simulation_timestep", 0.001, ConfigSource::Default);
    context.config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    context.config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    context.config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // Initialize simulation clock
    double timestep = context.config->getOr<double>("simulation_timestep", 0.001);
    SimulationClock clock(timestep);
    
    // Create brain
    std::cout << "Creating brain..." << std::endl;
    context.brain = std::make_shared<Brain>(context.config);
    
    if (!context.brain->initialize()) {
        std::cout << "Failed to initialize brain!" << std::endl;
        return false;
    }
    
    std::cout << "Brain initialized successfully." << std::endl;
    
    if (!headless) {
        std::cout << "=== Simulation Output ===" << std::endl;
    }
    
    // Run simulation steps
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (SimulationStep step = 0; step < steps; ++step) {
        if (!headless && (step % 100 == 0 || step == steps - 1)) {
            std::cout << "Step " << step << "/" << steps << 
                " | Spikes: " << context.brain->getTotalSpikeCount() << 
                " | Active: " << context.brain->getActiveNeuronCount() << 
                " | Firing: " << context.brain->getFiringNeuronCount() << std::endl;
        }
        
        // Update simulation time
        auto currentTime = clock.getCurrentTime();
        
        // Run brain simulation step
        context.brain->step(step, currentTime);
        
        // Advance simulation clock
        clock.advance();
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << "=== Simulation Complete ===" << std::endl;
    std::cout << "Steps completed: " << steps << std::endl;
    std::cout << "Final spike count: " << context.brain->getTotalSpikeCount() << std::endl;
    std::cout << "Final active neurons: " << context.brain->getActiveNeuronCount() << std::endl;
    std::cout << "Final firing neurons: " << context.brain->getFiringNeuronCount() << std::endl;
    std::cout << "Simulation time: " << duration.count() / 1000.0 << " seconds" << std::endl;
    
    return true;
}

} // namespace nlm
