// Advanced NLM Example: Power User Features
// Demonstrates advanced configuration and usage patterns

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "environment/Environment.hpp"
#include "agent/AgentBrain.hpp"
#include <iostream>
#include <vector>
#include <memory>
#include <chrono>

void demonstrateAdvancedConfiguration() {
    std::cout << "=== Advanced Configuration Example ===" << std::endl;
    
    // Create configuration with advanced settings
    auto config = std::make_shared<nlm::Config>();
    
    // Performance optimizations
    config->set("simulation_timestep", 0.0005, nlm::ConfigSource::Runtime);  // Smaller timestep for higher precision
    config->set("neuron_count", 5000, nlm::ConfigSource::Runtime);          // Larger network
    config->set("region_count", 4, nlm::ConfigSource::Runtime);            // Multiple regions
    config->set("connection_probability", 0.15, nlm::ConfigSource::Runtime); // More connections
    
    // Advanced plasticity settings
    config->set("stdp_ltp_weight", 0.025, nlm::ConfigSource::Runtime);
    config->set("stdp_ltd_weight", 0.02, nlm::ConfigSource::Runtime);
    config->set("stdp_tau", 25.0, nlm::ConfigSource::Runtime);
    config->set("synaptogenesis_rate", 0.002, nlm::ConfigSource::Runtime);
    config->set("pruning_rate", 0.0005, nlm::ConfigSource::Runtime);
    
    // Neuromodulation settings
    config->set("dopamine_baseline", 0.1, nlm::ConfigSource::Runtime);
    config->set("curiosity_sensitivity", 2.0, nlm::ConfigSource::Runtime);
    config->set("novelty_threshold", 0.3, nlm::ConfigSource::Runtime);
    
    // Memory system settings
    config->set("working_memory_capacity", 500, nlm::ConfigSource::Runtime);
    config->set("episodic_memory_max_episodes", 5000, nlm::ConfigSource::Runtime);
    config->set("associative_memory_capacity", 1000, nlm::ConfigSource::Runtime);
    
    // Development settings
    config->set("developmental_stage_duration", 2000, nlm::ConfigSource::Runtime);
    config->set("critical_period_enabled", true, nlm::ConfigSource::Runtime);
    
    std::cout << "✓ Advanced configuration created" << std::endl;
    std::cout << "  - Network size: 5000 neurons in 4 regions" << std::endl;
    std::cout << "  - High-precision simulation (0.0005s timestep)" << std::endl;
    std::cout << "  - Enhanced plasticity with custom STDP parameters" << std::endl;
    std::cout << "  - Advanced neuromodulation and memory systems" << std::endl;
}

void demonstrateComplexSimulation() {
    std::cout << "\n=== Complex Simulation Example ===" << std::endl;
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 2000);
    config->set("simulation_timestep", 0.001);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Create world and agent
    auto world = std::make_shared<nlm::SimpleWorld>();
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    
    // Configure world for complex environment
    world->configure(width=50, height=50, visionWidth=12, visionHeight=12);
    world->setRandomSeed(12345);
    world->reset();
    
    // Enable advanced subsystems
    agent->enableRewardModulation(true);
    agent->enableStructuralPlasticity(true);
    agent->enableDevelopment(true);
    agent->enableCuriosity(true);
    
    agent->initialize(world);
    
    std::cout << "✓ Complex simulation setup complete" << std::endl;
    std::cout << "  - Large-scale neural network (2000 neurons)" << std::endl;
    std::cout << "  - Complex environment (50x50 with 12x12 vision)" << std::endl;
    std::cout << "  - All learning subsystems enabled" << std::endl;
    
    // Demonstrate adaptive behavior through multiple episodes
    std::cout << "\nRunning simulation with learning..." << std::endl;
    
    for (int episode = 0; episode < 5; ++episode) {
        std::cout << "  Episode " << episode << ": ";
        
        for (int step = 0; step < 200; ++step) {
            // Update world
            world->update(0.1);
            
            // Get sensory input
            auto percept = world->getSensoryPercept();
            
            // Process input
            agent->processSensoryInput(percept);
            brain->step(step);
            
            // Get action and execute
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Apply reward-based learning
            auto internalSignals = percept.getInternal();
            float reward = internalSignals.empty() ? 0.0f : internalSignals[0];
            agent->applyRewardModulation(reward, 0.0f);
            
            // Update development
            agent->updateDevelopment(0.1);
        }
        
        // Log episode statistics
        std::cout << "Firing: " << brain->getFiringNeuronCount() 
                  << ", E/I Ratio: " << brain->getExcitationInhibitionRatio()
                  << ", Stage: " << static_cast<int>(brain->getDevelopmentalStage());
        
        if (episode < 4) std::cout << ", ";
        else std::cout << "\n";
        
        // Reset for next episode
        world->reset();
        agent->reset();
        brain->reset();
        brain->initialize();
    }
    
    std::cout << "✓ Complex simulation completed successfully" << std::endl;
}

void demonstrateRealTimeInteraction() {
    std::cout << "\n=== Real-Time Interaction Example ===" << std::endl;
    
    // Configuration optimized for real-time interaction
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 1000);
    config->set("simulation_timestep", 0.0002);  // Very fast for real-time
    config->set("working_memory_capacity", 200);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    auto world = std::make_shared<nlm::SimpleWorld>();
    world->configure(width=30, height=30, visionWidth=10, visionHeight=10);
    world->reset();
    
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    agent->enableRewardModulation(true);
    agent->enableCuriosity(true);
    agent->enableDevelopment(true);
    
    agent->initialize(world);
    
    std::cout << "✓ Real-time interaction setup complete" << std::endl;
    std::cout << "  - Ultra-fast simulation (0.0002s timesteps)" << std::endl;
    std::cout << "  - Small working memory for quick processing" << std::endl;
    std::cout << "  - Optimized for responsive interaction" << std::endl;
    
    // Demonstrate real-time adaptive behavior
    std::cout << "\nSimulating real-time task..." << std::endl;
    
    float targetActivation = 0.0f;
    bool taskCompleted = false;
    
    for (int step = 0; step < 500 && !taskCompleted; ++step) {
        // Dynamic task based on brain state
        if (brain->getActiveNeuronCount() > brain->getTotalNeuronCount() * 0.3f) {
            targetActivation += 0.02f;  // Increase target activity
        }
        
        // Get sensory input
        auto percept = world->getSensoryPercept();
        
        // Process and adapt
        agent->processSensoryInput(percept);
        brain->step(step);
        
        // Check if target reached
        float currentActivity = brain->getAverageFiringRate() / 100.0f;  // Normalize
        if (currentActivity > targetActivation * 0.8f) {
            taskCompleted = true;
            std::cout << "    Task reached: " << currentActivity << " > " << targetActivation * 0.8f << "\n";
        }
        
        // Adaptive action selection
        auto action = agent->decodeMotorCommand();
        
        // Apply reward based on task progress
        float reward = taskCompleted ? 1.0f : 0.0f;
        agent->applyRewardModulation(reward, 0.0f);
        
        world->applyMotorCommand(action, world->getSimulationTime());
        
        if (step % 100 == 0) {
            std::cout << "    Step " << step << ": Activity " << currentActivity << ", Target " << targetActivation << "\n";
        }
    }
    
    std::cout << "✓ Real-time interaction completed" << std::endl;
}

void demonstratePerformanceBenchmarking() {
    std::cout << "\n=== Performance Benchmarking Example ===" << std::endl;
    
    std::vector<int> networkSizes = {500, 1000, 2000};
    std::vector<double> timestepOptions = {0.001, 0.0005, 0.0002};
    
    std::cout << "Benchmarking different configurations..." << std::endl;
    
    for (int networkSize : networkSizes) {
        std::cout << "\n  Network size: " << networkSize << " neurons" << std::endl;
        
        for (double timestep : timestepOptions) {
            auto start = std::chrono::high_resolution_clock::now();
            
            // Create configuration
            auto config = std::make_shared<nlm::Config>();
            config->set("neuron_count", networkSize);
            config->set("simulation_timestep", timestep);
            config->set("region_count", 2);
            
            // Create and initialize brain
            auto brain = std::make_shared<nlm::Brain>(config);
            brain->initialize();
            
            // Run simulation
            for (int step = 0; step < 1000; ++step) {
                brain->step(step);
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            
            std::cout << "    Timestep " << timestep << ": " << duration.count() << "ms" << std::endl;
        }
    }
    
    std::cout << "\n✓ Performance benchmarking completed" << std::endl;
}

int main() {
    std::cout << "NLM Advanced Usage Examples" << std::endl;
    std::cout << "===========================" << std::endl;
    
    try {
        demonstrateAdvancedConfiguration();
        demonstrateComplexSimulation();
        demonstrateRealTimeInteraction();
        demonstratePerformanceBenchmarking();
        
        std::cout << "\n" << std::string(50, '=') << std::endl;
        std::cout << "All advanced examples completed successfully!" << std::endl;
        std::cout << std::string(50, '=') << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "\nError in advanced examples: " << e.what() << std::endl;
        return 1;
    }
}