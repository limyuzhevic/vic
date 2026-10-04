// Common helper functions for NLM users
// This file provides utilities that make the NLM framework easier to use

#include "agent/AgentBrain.hpp"
#include "agent/SensoryPercept.hpp"
#include "world/SimpleWorld.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <memory>

namespace nlm {

// Helper function: Create a simple agent configuration
std::shared_ptr<Config> createAgentConfig() {
    auto config = std::make_shared<Config>();
    
    // Basic agent configuration
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    
    // Enable all agent features by default
    config->set("enable_reward_modulation", true, ConfigSource::Default);
    config->set("enable_structural_plasticity", true, ConfigSource::Default);
    config->set("enable_development", true, ConfigSource::Default);
    config->set("enable_curiosity", true, ConfigSource::Default);
    
    return config;
}

// Helper function: Initialize agent for world interaction
bool initializeAgentForWorld(AgentBrain& agent, SimpleWorld& world, 
                           size_t visionWidth = 8, size_t visionHeight = 8) {
    try {
        world.configure(visionWidth, visionHeight, visionWidth, visionHeight);
        world.reset();
        agent.initialize(world);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error initializing agent for world: " << e.what() << std::endl;
        return false;
    }
}

// Helper function: Run a complete agent simulation episode
bool runAgentEpisode(Brain& brain, SimpleWorld& world, AgentBrain& agent, 
                    size_t numSteps, float timestep = 0.1f) {
    try {
        // Store initial state for comparison
        auto initialPercept = world.getSensoryPercept();
        size_t initialFiring = brain.getFiringNeuronCount();
        float initialEnergy = world.getAgentBody().energy;
        
        // Reset for clean episode
        agent.reset();
        world.reset();
        
        // Configure world for this agent
        world.configure(10, 10, 8, 8);
        agent.initialize(world);
        
        float time = 0.0f;
        
        for (size_t step = 0; step < numSteps; ++step) {
            // Update world
            world.update(timestep);
            time += timestep;
            
            // Get sensory percept
            auto percept = world.getSensoryPercept();
            
            // Process sensory input
            agent.processSensoryInput(percept);
            
            // Brain thinking
            brain.step(step, time);
            
            // Get action
            auto command = agent.decodeMotorCommand();
            
            // Apply action
            world.applyMotorCommand(command, time);
            
            // Optional: log progress every 10 steps
            if (step % 10 == 0) {
                std::cout << "  Step " << step << ": "
                         << brain.getFiringNeuronCount() << " neurons firing, "
                         << world.getAgentBody().energy << " energy remaining" << std::endl;
            }
        }
        
        // Log episode summary
        std::cout << "Episode complete:" << std::endl;
        std::cout << "  Final firing rate: " << brain.getAverageFiringRate() << " Hz" << std::endl;
        std::cout << "  Total spikes: " << brain.getTotalSpikeCount() << std::endl;
        std::cout << "  Final energy: " << world.getAgentBody().energy << std::endl;
        std::cout << "  Development stage: " << static_cast<int>(agent.getDevelopmentalStage()) << std::endl;
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error running agent episode: " << e.what() << std::endl;
        return false;
    }
}

// Helper function: Create a standard learning environment
std::tuple<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>>
createLearningEnvironment(size_t visionWidth = 8, size_t visionHeight = 8) {
    // Create brain with agent-friendly configuration
    auto config = createAgentConfig();
    auto brain = std::make_shared<Brain>(config);
    
    // Create world and agent
    auto world = std::make_shared<SimpleWorld>();
    auto agent = std::make_shared<AgentBrain>(brain);
    
    // Initialize agent in world
    if (!initializeAgentForWorld(*agent, *world, visionWidth, visionHeight)) {
        throw std::runtime_error("Failed to initialize agent in world");
    }
    
    // Initialize brain
    brain->initialize();
    
    return std::make_tuple(brain, world, agent);
}

// Helper function: Validate sensory percept compatibility with agent
bool validatePerceptForAgent(const SensoryPercept& percept, 
                            const AgentBrain& agent) {
    try {
        // Check if percept size matches agent's expected input size
        size_t expectedSize = agent.getSensoryInputSize();
        size_t actualSize = percept.getTotalSensoryInputSize();
        
        if (actualSize != expectedSize) {
            std::cerr << "Percept size mismatch: expected " << expectedSize 
                     << ", got " << actualSize << std::endl;
            return false;
        }
        
        // Validate individual dimensions
        if (percept.validate()) {
            std::cout << "Percept validation passed for agent use" << std::endl;
            return true;
        } else {
            std::cerr << "Percept validation failed for agent use" << std::endl;
            return false;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error validating percept for agent: " << e.what() << std::endl;
        return false;
    }
}

// Helper function: Run agent learning with reward feedback
bool runAgentLearningSimulation(Brain& brain, SimpleWorld& world, AgentBrain& agent,
                               size_t numEpisodes, size_t stepsPerEpisode = 50) {
    try {
        std::cout << "=== Starting Agent Learning Simulation ===" << std::endl;
        std::cout << "Episodes: " << numEpisodes << ", Steps per episode: " << stepsPerEpisode << std::endl;
        std::cout << std::endl;
        
        float totalReward = 0.0f;
        size_t totalSteps = 0;
        
        for (size_t episode = 1; episode <= numEpisodes; ++episode) {
            std::cout << "Episode " << episode << "/" << numEpisodes << ":" << std::endl;
            
            // Reset episode
            agent.reset();
            world.reset();
            agent.initialize(world);
            
            float episodeReward = 0.0f;
            size_t episodeSteps = 0;
            
            while (episodeSteps < stepsPerEpisode) {
                // Update world
                world.update(0.1f);
                
                // Get sensory percept
                auto percept = world.getSensoryPercept();
                
                // Validate percept before processing
                if (!validatePerceptForAgent(percept, agent)) {
                    std::cerr << "Invalid percept, skipping step" << std::endl;
                    episodeSteps++;
                    continue;
                }
                
                // Process sensory input
                agent.processSensoryInput(percept);
                
                // Brain thinking
                brain.step(totalSteps, totalSteps * 0.001f);
                
                // Get action
                auto command = agent.decodeMotorCommand();
                
                // Apply action and get result
                auto result = world.applyMotorCommand(command, totalSteps * 0.001f);
                episodeReward += result.reward;
                totalReward += result.reward;
                
                // Apply reward modulation
                agent.applyRewardModulation(result.reward, agent.getPredictionError());
                
                // Update development
                agent.updateDevelopment(0.1f);
                
                totalSteps++;
                episodeSteps++;
                
                // Log progress every 10 steps
                if (episodeSteps % 10 == 0) {
                    std::cout << "    Step " << episodeSteps << ": reward = " << result.reward 
                             << ", total = " << episodeReward 
                             << ", firing neurons = " << brain.getFiringNeuronCount() << std::endl;
                }
            }
            
            std::cout << "    Episode complete: total reward = " << episodeReward 
                     << ", final development stage = " << static_cast<int>(agent.getDevelopmentalStage()) << std::endl;
            std::cout << std::endl;
            
            // Brief pause between episodes
            if (episode < numEpisodes) {
                std::cout << "Pausing before next episode... (press enter to continue)" << std::endl;
                std::cin.get();
            }
        }
        
        std::cout << "=== Learning Simulation Complete ===" << std::endl;
        std::cout << "Total reward: " << totalReward << std::endl;
        std::cout << "Total steps: " << totalSteps << std::endl;
        std::cout << "Average reward per episode: " << (numEpisodes > 0 ? totalReward / numEpisodes : 0.0f) << std::endl;
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error in learning simulation: " << e.what() << std::endl;
        return false;
    }
}

// Helper function: Create and run a complete agent demonstration
void demonstrateAgent(Brain& brain, SimpleWorld& world, AgentBrain& agent,
                     size_t demonstrationSteps = 100) {
    try {
        std::cout << "=== NLM Agent Demonstration ===" << std::endl;
        std::cout << "This demonstration shows how an artificial brain interacts with an environment." << std::endl;
        std::cout << std::endl;
        
        // Reset and initialize
        agent.reset();
        world.reset();
        agent.initialize(world);
        brain.initialize();
        
        std::cout << "Initial state:" << std::endl;
        std::cout << "  Firing neurons: " << brain.getFiringNeuronCount() << std::endl;
        std::cout << "  Energy: " << world.getAgentBody().energy << std::endl;
        std::cout << "  Development: " << static_cast<int>(agent.getDevelopmentalStage()) << std::endl;
        std::cout << "  Curiosity: " << agent.getCuriosityLevel() << std::endl;
        std::cout << std::endl;
        
        std::cout << "Running demonstration for " << demonstrationSteps << " steps:" << std::endl;
        std::cout << "(This will show the agent learning and adapting to its environment)" << std::endl;
        std::cout << std::endl;
        
        for (size_t step = 0; step < demonstrationSteps; ++step) {
            // Update world
            world.update(0.1f);
            
            // Get sensory percept
            auto percept = world.getSensoryPercept();
            
            // Process sensory input
            agent.processSensoryInput(percept);
            
            // Brain thinking
            brain.step(step, step * 0.001f);
            
            // Get action
            auto command = agent.decodeMotorCommand();
            
            // Apply action
            world.applyMotorCommand(command, step * 0.001f);
            
            // Optional: log every 20 steps
            if (step % 20 == 0 || step == demonstrationSteps - 1) {
                std::cout << "Step " << step << ": "
                         << brain.getFiringNeuronCount() << " firing, "
                         << world.getAgentBody().energy << " energy, "
                         << (command == MotorCommand::Wait ? "waiting" : 
                            command == MotorCommand::MoveForward ? "moving forward" :
                            command == MotorCommand::TurnLeft ? "turning left" :
                            command == MotorCommand::TurnRight ? "turning right" :
                            command == MotorCommand::Interact ? "interacting" : "unknown") << std::endl;
            }
        }
        
        std::cout << std::endl;
        std::cout << "=== Demonstration Complete ===" << std::endl;
        std::cout << "Final state:" << std::endl;
        std::cout << "  Firing neurons: " << brain.getFiringNeuronCount() << std::endl;
        std::cout << "  Total spikes: " << brain.getTotalSpikeCount() << std::endl;
        std::cout << "  Average firing rate: " << brain.getAverageFiringRate() << " Hz" << std::endl;
        std::cout << "  Energy remaining: " << world.getAgentBody().energy << std::endl;
        std::cout << "  Development stage: " << static_cast<int>(agent.getDevelopmentalStage()) << std::endl;
        std::cout << "  Neuromodulation level: " << agent.getNeuromodulationLevel() << std::endl;
        std::cout << "  Curiosity level: " << agent.getCuriosityLevel() << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error in agent demonstration: " << e.what() << std::endl;
        std::cout << "The demonstration encountered an error but may have produced partial results." << std::endl;
    }
}

} // namespace nlm
