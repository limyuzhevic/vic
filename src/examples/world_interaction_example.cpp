// NLM - World Interaction Example
// Demonstrates complete agent-environment interaction

#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "environment/SimpleWorld.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <vector>
#include <chrono>
#include <cmath>

int main() {
    std::cout << "=== NLM World Interaction Example ===" << std::endl;
    std::cout << "Demonstrates complete agent-environment interaction with sensory processing, motor control, and learning." << std::endl;
    
    // Initialize logger for debugging
    initializeLogger("world_interaction_example.log", LogLevel::Info);
    
    try {
        // Create configuration optimized for world interaction
        nlm::Config config = nlm::Config::createDefaultConfig();
        
        // Configure brain for interaction
        config.set("brain.neuron_count", 800);
        config.set("brain.synapse_density", 0.15);
        config.set("brain.v_thresh", -55.0);
        config.set("brain.v_rest", -65.0);
        config.set("plasticity.stdp.enable", true);
        config.set("plasticity.hebbian.enable", true);
        config.set("plasticity.structural.enable", true);
        config.set("neuromod.dopamine.scale", 1.2);
        config.set("neuromod.curiosity.enable", true);
        config.set("neuromod.novelty.enable", true);
        config.set("development.initial_plasticity", 1.2);
        config.set("development.maturation_rate", 0.002);
        
        std::cout << "Configuration created with " << config.get<int>("brain.neuron_count") 
                  << " neurons for world interaction simulation." << std::endl;
        
        // Create brain
        nlm::Brain brain(config);
        brain.initialize();
        
        // Create agent
        nlm::AgentBrain agent(brain);
        
        // Create and configure world
        nlm::World world;
        world.configure(30, 30, 12, 12); // 30x30 world, 12x12 vision
        world.reset();
        world.setAgentStart(15.0, 15.0); // Start in center
        
        std::cout << "World configured: 30x30, vision 12x12" << std::endl;
        std::cout << "Agent starts at position (15.0, 15.0)" << std::endl;
        
        // Initialize agent with world
        agent.initialize(world);
        
        // Enable learning subsystems
        agent.enableRewardModulation(true);
        agent.enableCuriosity(true);
        agent.enableDevelopment(true);
        agent.enableStructuralPlasticity(true);
        
        std::cout << "Learning subsystems enabled:" << std::endl;
        std::cout << "  - Reward modulation: Enabled" << std::endl;
        std::cout << "  - Curiosity: Enabled" << std::endl;
        std::cout << "  - Development: Enabled" << std::endl;
        std::cout << "  - Structural plasticity: Enabled" << std::endl;
        
        // Run simulation
        std::cout << "\nStarting world interaction simulation..." << std::endl;
        std::cout << "Step, X, Y, Velocity, Reward, Curiosity, Novelty, Firing Neurons" << std::endl;
        
        auto start_time = std::chrono::high_resolution_clock::now();
        double total_reward = 0.0;
        double max_curiosity = 0.0;
        double max_novelty = 0.0;
        
        for (int step = 0; step < 500; ++step) {
            // Update world
            world.update(0.1);
            
            // Get current agent state
            auto agent_body = world.getAgentBody();
            double agent_x = agent_body.position.x;
            double agent_y = agent_body.position.y;
            double velocity = std::sqrt(agent_body.velocity.x * agent_body.velocity.x + 
                                       agent_body.velocity.y * agent_body.velocity.y);
            
            // Get sensory perception
            auto percept = world.getSensoryPercept();
            
            // Process sensory input
            agent.processSensoryInput(percept);
            
            // Run brain step
            brain.step(step);
            
            // Get motor command from brain
            auto action = agent.decodeMotorCommand();
            
            // Apply action to world
            world.applyMotorCommand(action, world.getSimulationTime());
            
            // Get reward from world
            double reward = world.getSensoryPercept().getInternal()[0] 
                           if (world.getSensoryPercept().getInternal()) else 0.0;
            total_reward += reward;
            
            // Apply reward modulation
            agent.applyRewardModulation(reward, 0.0);
            
            // Update development
            agent.updateDevelopment(0.1);
            
            // Record statistics
            double curiosity = agent.getCuriosityLevel();
            double novelty = agent.getNoveltyLevel();
            max_curiosity = std::max(max_curiosity, curiosity);
            max_novelty = std::max(max_novelty, novelty);
            
            // Log progress every 50 steps
            if (step % 50 == 0) {
                std::cout << step << ", "
                          << std::fixed << std::setprecision(1) << agent_x << ", "
                          << agent_y << ", "
                          << velocity << ", "
                          << std::setprecision(3) << reward << ", "
                          << curiosity << ", "
                          << novelty << ", "
                          << brain.getFiringNeuronCount() << std::endl;
            }
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        std::cout << "\nSimulation completed in " << duration.count() << " ms." << std::endl;
        std::cout << "Average steps per second: " << (500.0 / (duration.count() / 1000.0)) << std::endl;
        
        // Print final statistics
        std::cout << "\nFinal Agent Statistics:" << std::endl;
        std::cout << "  Total simulation time: " << world.getSimulationTime() << " seconds" << std::endl;
        std::cout << "  Total reward received: " << total_reward << std::endl;
        std::cout << "  Average reward per step: " << (total_reward / 500.0) << std::endl;
        std::cout << "  Maximum curiosity level: " << max_curiosity << std::endl;
        std::cout << "  Maximum novelty level: " << max_novelty << std::endl;
        std::cout << "  Final brain firing rate: " << brain.getAverageFiringRate() << " Hz" << std::endl;
        std::cout << "  Final development stage: " << brain.getDevelopmentalStage() << std::endl;
        std::cout << "  Final neuromodulation level: " << agent.getNeuromodulationLevel() << std::endl;
        
        // Save final state
        brain.save("world_interaction_final.bin");
        std::cout << "\nAgent state saved to 'world_interaction_final.bin'" << std::endl;
        
        // Demonstrate learning effects
        std::cout << "\nDemonstrating learning effects..." << std::endl;
        
        // Create new brain and load state
        nlm::Brain brain2(config);
        brain2.load("world_interaction_final.bin");
        
        nlm::AgentBrain agent2(brain2);
        agent2.initialize(world);
        agent2.enableRewardModulation(true);
        agent2.enableCuriosity(true);
        agent2.enableDevelopment(true);
        
        // Run additional steps to observe learning
        double learning_reward = 0.0;
        for (int step = 0; step < 100; ++step) {
            world.update(0.1);
            
            auto percept = world.getSensoryPercept();
            agent2.processSensoryInput(percept);
            
            brain2.step(step);
            
            auto action = agent2.decodeMotorCommand();
            world.applyMotorCommand(action, world.getSimulationTime());
            
            double reward = world.getSensoryPercept().getInternal()[0] 
                           if (world.getSensoryPercept().getInternal()) else 0.0;
            learning_reward += reward;
            
            agent2.applyRewardModulation(reward, 0.0);
            agent2.updateDevelopment(0.1);
        }
        
        std::cout << "Learning phase completed:" << std::endl;
        std::cout << "  Additional reward received: " << learning_reward << std::endl;
        std::cout << "  Final curiosity level: " << agent2.getCuriosityLevel() << std::endl;
        std::cout << "  Final development stage: " << brain2.getDevelopmentalStage() << std::endl;
        
        std::cout << "\n=== World Interaction Example Completed Successfully ===" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: Unexpected exception in World Interaction Example: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
