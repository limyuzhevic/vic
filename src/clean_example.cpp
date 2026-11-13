// NLM - Clean Example and Entry Point
// A simple, maintainable example that demonstrates basic usage
// and serves as a starting point for users

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include <iostream>
#include <memory>

using namespace nlm;

// Simplified test function for basic brain operation
void runBasicBrainTest() {
    std::cout << "=== Basic Brain Test ===" << std::endl;
    
    // Create and configure brain
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100, ConfigSource::Default);
    config->set("region_count", 1, ConfigSource::Default);
    config->set("random_seed", 42, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return;
    }
    
    // Run simulation
    const size_t numSteps = 100;
    for (size_t step = 0; step < numSteps; ++step) {
        brain->step(static_cast<SimulationStep>(step));
        
        if (step % 25 == 0) {
            std::cout << "Step " << step << ": " 
                     << brain->getTotalSpikeCount() << " spikes, "
                     << brain->getFiringNeuronCount() << " firing neurons" << std::endl;
        }
    }
    
    std::cout << "Basic brain test complete!" << std::endl;
}

// Simplified demonstration of brain-world interaction
void runAgentWorldDemo() {
    std::cout << "=== Agent-World Demo ===" << std::endl;
    
    // Create brain
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 200, ConfigSource::Default);
    config->set("region_count", 1, ConfigSource::Default);
    config->set("random_seed", 123, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Create world
    auto world = std::make_unique<SimpleWorld>();
    world->configure(width: 10, height: 10, visionWidth: 8, visionHeight: 8);
    world->reset();
    
    // Create agent
    auto agent = std::make_unique<AgentBrain>(brain);
    agent->initialize(*world);
    
    // Run agent in world
    const size_t numSteps = 50;
    for (size_t step = 0; step < numSteps; ++step) {
        // Update world
        world->update(0.1);
        
        // Get perception
        auto percept = world->getSensoryPercept();
        
        // Process and think
        agent->processSensoryInput(percept);
        brain->step(static_cast<SimulationStep>(step));
        
        // Act
        auto action = agent->decodeMotorCommand();
        world->applyMotorCommand(action, world->getSimulationTime());
        
        if (step % 10 == 0) {
            std::cout << "Step " << step << ": Action " << static_cast<int>(action)
                     << ", Spikes: " << brain->getTotalSpikeCount() << std::endl;
        }
    }
    
    std::cout << "Agent-world demo complete!" << std::endl;
}

// Configuration management example
void runConfigDemo() {
    std::cout << "=== Configuration Demo ===" << std::endl;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    
    // Set various parameters
    config->set("neuron_count", 500, ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    
    // Configure plasticity
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    
    // Configure development
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    std::cout << "Configuration loaded successfully!" << std::endl;
    
    // Print current values
    std::cout << "  neuron_count: " << config->getOr<int64_t>("neuron_count", 500) << std::endl;
    std::cout << "  timestep: " << config->getOr<double>("simulation_timestep", 0.001) << std::endl;
    std::cout << "  connection_probability: " << config->getOr<float>("connection_probability", 0.1f) << std::endl;
}

int main(int argc, char** argv) {
    // Initialize logging
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    std::cout << "NLM - Neural Learning Machine - Clean Example" << std::endl;
    std::cout << "=========================================" << std::endl;
    
    // Simple argument handling
    bool runAllExamples = true;
    bool runBasicTest = false;
    bool runAgentDemo = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--basic") runBasicTest = true;
        else if (arg == "--agent") runAgentDemo = true;
        else if (arg == "--all") runAllExamples = true;
        else if (arg == "--help") {
            std::cout << "Usage: " << argv[0] << " [options]" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  --basic      Run basic brain test" << std::endl;
            std::cout << "  --agent      Run agent-world demo" << std::endl;
            std::cout << "  --all        Run all examples (default)" << std::endl;
            std::cout << "  --help       Show this help" << std::endl;
            return 0;
        }
        }
    }
    
    if (runBasicTest) {
        runBasicBrainTest();
    }
    
    if (runAgentDemo) {
        runAgentWorldDemo();
    }
    
    if (runAllExamples || (runBasicTest == false && runAgentDemo == false)) {
        if (!runBasicTest) runBasicBrainTest();
        if (!runAgentDemo) runAgentWorldDemo();
        runConfigDemo();
    }
    
    std::cout << "\nAll examples completed successfully!" << std::endl;
    return 0;
}
