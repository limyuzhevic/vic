// NLM Visualization Example
// Simple demonstration of visualization capabilities

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "visualization/VisualizationInterface.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <csignal>

bool running = true;

void signalHandler(int signum) {
    std::cout << "\nReceived interrupt signal " << signum << std::endl;
    running = false;
}

int main(int argc, char** argv) {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Visualization Example                                      ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    std::cout << "NLM Visualization Example" << std::endl;
    std::cout << "Demonstrates real-time brain activity visualization" << std::endl;
    std::cout << std::endl;
    
    // Set up signal handler
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    // Load configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 200, ConfigSource::Default);
    config->set("region_count", 2, ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    
    std::cout << "Configuration loaded:" << std::endl;
    std::cout << "  - " << config->getOr<int64_t>("neuron_count", 200) << " neurons" << std::endl;
    std::cout << "  - " << config->getOr<int64_t>("region_count", 2) << " regions" << std::endl;
    std::cout << std::endl;
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    brain->logStatus();
    std::cout << std::endl;
    
    // Initialize visualization
    VisualizationInterface viz;
    std::cout << "Initializing visualization..." << std::endl;
    if (!viz.initialize()) {
        std::cout << "Warning: Visualization initialization failed (expected for placeholder)" << std::endl;
    } else {
        viz.setUpdateRate(30.0);  // 30 Hz update rate
    }
    std::cout << "Visualization " << (viz.isActive() ? "activated" : "in placeholder mode") << std::endl;
    std::cout << std::endl;
    
    // Create world for context
    SimpleWorld world;
    world.configure(20, 20, 8, 8);
    world.reset();
    
    // Create agent
    AgentBrain agent(brain);
    agent.initialize(world);
    agent.enableCuriosity(true);
    
    std::cout << "Simulation started. Press Ctrl+C to stop." << std::endl;
    std::cout << std::endl;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    uint64_t stepCount = 0;
    
    // Main visualization loop
    while (running) {
        auto stepStart = std::chrono::high_resolution_clock::now();
        
        // Update world
        world.update(0.1);
        
        // Get sensory input
        SensoryPercept percept = world.getSensoryPercept();
        
        // Process input through agent
        agent.processSensoryInput(percept);
        
        // Brain step
        brain->step(stepCount, stepCount * 0.1);
        
        // Get action
        MotorCommand action = agent.decodeMotorCommand();
        
        // Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime());
        
        // Update visualization
        if (viz.isActive()) {
            viz.update();
            viz.render();
        }
        
        // Log progress periodically
        if (++stepCount % 100 == 0) {
            std::cout << "Step " << stepCount << ", " 
                     << brain->getFiringNeuronCount() << " neurons firing, "
                     << brain->getTotalSpikeCount() << " total spikes" << std::endl;
            
            // Show brain status every 500 steps
            if (stepCount % 500 == 0) {
                brain->logStatus();
            }
        }
        
        // Maintain ~10 Hz simulation rate
        auto stepEnd = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(stepEnd - stepStart).count();
        double targetFrameTime = 0.1;  // 10 Hz
        
        if (duration < targetFrameTime) {
            std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>((targetFrameTime - duration) * 1000)));
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    std::cout << std::endl;
    std::cout << "=== Visualization Complete ===" << std::endl;
    std::cout << "Total steps: " << stepCount << std::endl;
    std::cout << "Total time: " << totalTime << "s" << std::endl;
    std::cout << "Simulation rate: " << (stepCount / totalTime) << " Hz" << std::endl;
    
    // Final visualization
    if (viz.isActive()) {
        viz.visualizeNetwork(*brain);
        viz.visualizeActivity(*brain);
        std::cout << "Final network visualization displayed" << std::endl;
        viz.close();
    }
    
    std::cout << "Visualization example finished successfully!" << std::endl;
    
    return 0;
}
