// NLM Basic Brain Usage Example
// 
// This example demonstrates basic NLM brain functionality including
// configuration, initialization, sensory processing, and action generation.
// It serves as a starting point for understanding NLM's core capabilities.

#include <nlm/Brain.hpp>
#include <nlm/core/Config/Config.hpp>
#include <nlm/sensory/SensoryInput.hpp>
#include <nlm/motor/Action.hpp>
#include <iostream>
#include <thread>
#include <chrono>

// Example: Simple agent simulation
class SimpleAgent {
public:
    SimpleAgent(std::shared_ptr<nlm::Config> config)
        : config_(config), running_(false), stepCount_(0) {}
    
    ~SimpleAgent() {
        shutdown();
    }
    
    // Initialize agent with configuration
    bool initialize() {
        std::cout << "Initializing NLM agent..." << std::endl;
        
        // Create and configure brain
        brain_ = std::make_shared<nlm::Brain>(config_);
        
        // Initialize advanced brain interface
        nlm::AdvancedBrainInterface::Config advancedConfig;
        advancedConfig.enableAdvancedCommands = true;
        advancedConfig.enableExtensionSystem = true;
        advancedConfig.enablePluginArchitecture = true;
        
        if (!brain_->initializeAdvancedInterface(advancedConfig)) {
            std::cerr << "Failed to initialize advanced brain interface" << std::endl;
            return false;
        }
        
        // Initialize main brain
        if (!brain_->initialize()) {
            std::cerr << "Failed to initialize brain" << std::endl;
            return false;
        }
        
        std::cout << "Agent initialization complete" << std::endl;
        std::cout << "Brain created with " << brain_->getTotalNeuronCount() 
                  << " neurons and " << brain_->getTotalSynapseCount() 
                  << " synapses" << std::endl;
        
        running_ = true;
        return true;
    }
    
    // Main simulation loop
    void run(float durationSeconds = 10.0f) {
        std::cout << "Starting NLM agent simulation for " << durationSeconds 
                  << " seconds..." << std::endl;
        
        auto startTime = std::chrono::steady_clock::now();
        stepCount_ = 0;
        
        while (running_) {
            auto currentTime = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime);
            
            if (elapsed.count() >= durationSeconds) {
                break;
            }
            
            // Process sensory input
            processSensoryInput();
            
            // Update brain state
            brain_->step(stepCount_);
            
            // Generate action
            auto action = brain_->produceAction();
            executeAction(action);
            
            // Apply reward (simplified)
            applyReward(1.0f);
            
            // Log status periodically
            if (stepCount_ % 100 == 0) {
                logStatus();
            }
            
            stepCount_++;
            
            // Small delay to control simulation speed
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        
        std::cout << "Simulation complete. Processed " << stepCount_ 
                  << " simulation steps." << std::endl;
    }
    
    // Shutdown agent
    void shutdown() {
        if (running_) {
            std::cout << "Shutting down agent..." << std::endl;
            running_ = false;
            
            // Shutdown advanced interface
            if (brain_) {
                brain_->shutdownAdvancedInterface();
            }
            
            std::cout << "Agent shutdown complete." << std::endl;
        }
    }
    
private:
    // Process sensory input
    void processSensoryInput() {
        // Create simple sensory input (e.g., random pattern)
        nlm::SensoryInput input;
        
        // Example: Create a 3D position input
        std::vector<float> sensoryData(3);
        sensoryData[0] = 0.5f;  // X position
        sensoryData[1] = 0.0f;  // Y position  
        sensoryData[2] = 0.0f;  // Z position
        
        input.setData(sensoryData);
        
        // Feed to brain
        brain_->receiveSensoryInput(input);
    }
    
    // Execute action
    void executeAction(const std::unique_ptr<nlm::Action>& action) {
        if (!action) {
            return;
        }
        
        std::cout << "Action: " << static_cast<int>(action->getType()) 
                  << " (" << action->getDescription() << ")" << std::endl;
        
        // In a real implementation, this would:
        // - Apply action to world/environment
        // - Handle action consequences
        // - Generate feedback
    }
    
    // Apply reward signal
    void applyReward(float reward) {
        // In a real implementation, this would:
        // - Use curiosity system for exploration bonus
        // - Apply prediction error for learning
        // - Update neuromodulators
        
        // For simplicity, just log reward
        if (reward > 0.1f) {
            std::cout << "Reward: " << reward << std::endl;
        }
    }
    
    // Log system status
    void logStatus() {
        std::cout << "--- Step " << stepCount_ << " Status ---" << std::endl;
        
        if (brain_) {
            brain_->logStatus();
            
            // Additional metrics
            std::cout << "  Simulation metrics:" << std::endl;
            std::cout << "    Steps processed: " << stepCount_ << std::endl;
            std::cout << "    Average firing rate: " 
                      << brain_->getAverageFiringRate() << " Hz" << std::endl;
            std::cout << "    Active neurons: " 
                      << brain_->getActiveNeuronCount() << std::endl;
        }
        
        std::cout << "---------------------" << std::endl;
    }
    
    std::shared_ptr<nlm::Config> config_;
    std::shared_ptr<nlm::Brain> brain_;
    bool running_;
    uint64_t stepCount_;
};

// Example: Configuration setup
std::shared_ptr<nlm::Config> createDefaultConfig() {
    auto config = std::make_shared<nlm::Config>("BasicNLMConfig");
    
    // Basic configuration parameters
    config->set("neuron_count", 1000);      // Number of neurons
    config->set("region_count", 1);        // Number of brain regions
    config->set("connection_probability", 0.1f); // Connection probability
    config->set("timestep", 0.01f);         // Simulation timestep
    
    // Performance configuration
    config->set("enable_memory_pools", true);
    config->set("enable_cache_optimization", true);
    config->set("enable_batch_processing", true);
    
    // Memory pool sizes
    config->set("neuron_pool_size", 1024);
    config->set("synapse_pool_size", 4096);
    config->set("working_memory_pool_size", 512);
    config->set("plasticity_pool_size", 2048);
    
    // Checkpoint configuration
    config->set("checkpoint_dir", "./checkpoints");
    config->set("checkpoint_interval", 1000);
    
    // Development configuration
    config->set("enable_development", true);
    config->set("development_stage", "maturation");
    
    // Sensory configuration
    config->set("sensory_neurons", 50);
    
    // Motor configuration
    config->set("motor_neurons", 50);
    
    return config;
}

// Example: Main function
int main() {
    std::cout << "=== NLM Basic Brain Usage Example ===" << std::endl;
    std::cout << "This example demonstrates basic NLM brain functionality." << std::endl;
    std::cout << std::endl;
    
    try {
        // Create configuration
        auto config = createDefaultConfig();
        
        // Create and initialize agent
        SimpleAgent agent(config);
        
        if (!agent.initialize()) {
            std::cerr << "Failed to initialize agent" << std::endl;
            return 1;
        }
        
        // Run simulation
        agent.run(5.0f);  // Run for 5 seconds
        
        // Shutdown gracefully
        agent.shutdown();
        
        std::cout << std::endl;
        std::cout << "=== Example completed successfully ===" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}

/*
Expected Output:
=== NLM Basic Brain Usage Example ===
This example demonstrates basic NLM brain functionality.

Initializing NLM agent...
Agent initialization complete
Brain created with 1000 neurons and ~10000 synapses
Starting NLM agent simulation for 5 seconds...
--- Step 0 Status ---
[Various brain status output]
--- Step 100 Status ---
[Periodic status updates]
Simulation complete. Processed ~500 simulation steps.
Shutting down agent...
Agent shutdown complete.

=== Example completed successfully ===

Key Features Demonstrated:
1. Basic brain configuration and initialization
2. Sensory input processing
3. Brain step execution
4. Action generation and execution
5. Reward application
6. System monitoring and logging
7. Graceful shutdown

This example provides a foundation for more complex NLM applications.
*/
