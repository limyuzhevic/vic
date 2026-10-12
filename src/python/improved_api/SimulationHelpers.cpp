#include "NLMImprovedAPI.hpp"
#include <iostream>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <thread>

namespace nlm {
namespace SimulationHelpers {

SimulationResult run_brain_simulation(std::shared_ptr<Brain> brain,
                                       size_t steps,
                                       double timestep) {
    SimulationResult result;
    result.total_steps = steps;
    result.total_spikes = 0;
    result.avg_firing_rate = 0.0f;
    result.final_reward = 0.0f;
    result.memory_episodes_stored = 0;
    result.wall_clock_time = 0.0;
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    size_t initial_spikes = brain->getTotalSpikeCount();
    
    for (size_t i = 0; i < steps; ++i) {
        brain->step(static_cast<SimulationStep>(i), 
                   static_cast<Timestamp>(i * timestep));
    }
    
    size_t final_spikes = brain->getTotalSpikeCount();
    result.total_spikes = final_spikes - initial_spikes;
    
    auto end_time = std::chrono::high_resolution_clock::now();
    result.wall_clock_time = std::chrono::duration<double>(end_time - start_time);
    
    if (result.wall_clock_time.count() > 0) {
        result.avg_firing_rate = static_cast<float>(result.total_spikes / result.wall_clock_time.count());
    }
    
    result.active_neurons = brain->getActiveNeuronCount();
    
    // Add some log messages
    result.log_messages.push_back("Simulation completed " + Utils::format_number(steps) + " steps");
    result.log_messages.push_back("Generated " + Utils::format_number(result.total_spikes) + " spikes");
    result.log_messages.push_back("Average firing rate: " + Utils::format_number(static_cast<size_t>(result.avg_firing_rate)) + " Hz");
    
    return result;
}

SimulationResult run_agent_simulation(std::shared_ptr<AgentBrain> agent,
                                       std::shared_ptr<SimpleWorld> world,
                                       size_t steps, double timestep) {
    SimulationResult result;
    result.total_steps = steps;
    result.total_spikes = 0;
    result.avg_firing_rate = 0.0f;
    result.final_reward = 0.0f;
    result.memory_episodes_stored = 0;
    result.wall_clock_time = 0.0;
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    world->reset();
    
    for (size_t i = 0; i < steps; ++i) {
        world->update(timestep);
        
        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);
        
        // Get the underlying brain for statistics
        auto brain = agent->getBrain();
        brain->step(static_cast<SimulationStep>(i), 
                   static_cast<Timestamp>(i * timestep));
        
        auto action = agent->decodeMotorCommand();
        world->applyMotorCommand(action, world->getSimulationTime());
        
        // Get reward for statistics
        result.final_reward += world->getSensoryPercept().getInternal()[0];
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    result.wall_clock_time = std::chrono::duration<double>(end_time - start_time);
    
    if (result.wall_clock_time.count() > 0) {
        result.avg_firing_rate = static_cast<float>(result.total_spikes / result.wall_clock_time.count());
    }
    
    result.active_neurons = agent->getBrain()->getActiveNeuronCount();
    result.memory_episodes_stored = static_cast<float>(steps / 100); // Rough estimate
    
    // Add some log messages
    result.log_messages.push_back("Agent simulation completed " + Utils::format_number(steps) + " steps");
    result.log_messages.push_back("Total reward: " + Utils::format_number(result.final_reward));
    result.log_messages.push_back("Average firing rate: " + Utils::format_number(static_cast<size_t>(result.avg_firing_rate)) + " Hz");
    
    return result;
}

SimulationResult run_learning_episode(std::shared_ptr<AgentBrain> agent,
                                      std::shared_ptr<SimpleWorld> world,
                                      size_t steps, bool enable_learning) {
    SimulationResult result = run_agent_simulation(agent, world, steps);
    
    if (enable_learning) {
        // Apply reward modulation based on final reward
        result.final_reward = result.final_reward / steps;
        agent->applyRewardModulation(result.final_reward, 0.0f);
        
        result.log_messages.push_back("Applied reward modulation to agent");
        result.log_messages.push_back("Learning curiosity level: " + Utils::format_number(agent->getCuriosityLevel()));
        result.log_messages.push_back("Learning novelty level: " + Utils::format_number(agent->getNoveltyLevel()));
        result.log_messages.push_back("Learning prediction error: " + Utils::format_number(agent->getPredictionError()));
    }
    
    return result;
}

void run_research_pattern(std::shared_ptr<Brain> brain, size_t steps) {
    std::cout << "Running Research Pattern (" << steps << " steps)" << std::endl;
    
    for (size_t i = 0; i < steps; ++i) {
        // Inject random current to stimulate learning
        brain->injectCurrentToNeurons(NeuronType::Sensory, 
                                      brain->getRandomGenerator()->uniformReal(20.0f, 50.0f));
        brain->step(static_cast<SimulationStep>(i));
        
        if (i % 100 == 0) {
            std::cout << "Step " << i << ": " 
                      << brain->getFiringNeuronCount() << " neurons firing, "
                      << brain->getTotalSpikeCount() << " total spikes" << std::endl;
        }
    }
    
    std::cout << "Research pattern completed successfully" << std::endl;
}

void run_rl_pattern(std::shared_ptr<AgentBrain> agent,
                    std::shared_ptr<SimpleWorld> world, size_t steps) {
    std::cout << "Running RL Pattern (" << steps << " steps)" << std::endl;
    
    world->reset();
    agent->enableRewardModulation(true);
    agent->enableCuriosity(true);
    
    for (size_t i = 0; i < steps; ++i) {
        world->update(0.1);
        
        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);
        
        // Get underlying brain
        auto brain = agent->getBrain();
        brain->step(static_cast<SimulationStep>(i), 
                   static_cast<Timestamp>(i * 0.1));
        
        auto action = agent->decodeMotorCommand();
        world->applyMotorCommand(action, world->getSimulationTime());
        
        // Simple reward function
        float reward = 0.0f;
        if (world->getAgentBody().energy > 50.0f) {
            reward = 1.0f; // Positive reward for having energy
        }
        
        agent->applyRewardModulation(reward, 0.0f);
        
        if (i % 50 == 0) {
            std::cout << "Step " << i << ": Reward " << reward << ", "
                      << "Curiosity: " << agent->getCuriosityLevel() << ", "
                      << "Novelty: " << agent->getNoveltyLevel() << std::endl;
        }
    }
    
    std::cout << "RL pattern completed successfully" << std::endl;
    std::cout << "Final dopamine level: " << agent->getNeuromodulationLevel() << std::endl;
}

void run_development_pattern(std::shared_ptr<Brain> brain, size_t steps, double lifespan) {
    std::cout << "Running Development Pattern (" << steps << " steps, " << lifespan << "s lifespan)" << std::endl;
    
    brain->setDevelopmentalStage(DevelopmentalStage::Initial);
    brain->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    
    double current_time = 0.0;
    double timestep = lifespan / steps;
    
    for (size_t i = 0; i < steps; ++i) {
        // Inject current to promote development
        brain->injectCurrentToNeurons(NeuronType::Sensory, 10.0f);
        
        brain->step(static_cast<SimulationStep>(i), current_time);
        
        current_time += timestep;
        
        if (i % 100 == 0) {
            std::cout << "Step " << i << ": "
                      << "Developmental stage: " << static_cast<int>(brain->getDevelopmentalStage()) << ", "
                      << "Plasticity: " << brain->getAverageFiringRate() << std::endl;
        }
    }
    
    std::cout << "Development pattern completed successfully" << std::endl;
    std::cout << "Final developmental stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
}

std::vector<float> collect_firing_rates(std::shared_ptr<Brain> brain,
                                        size_t samples,
                                        double timestep) {
    std::vector<float> firing_rates;
    firing_rates.reserve(samples);
    
    for (size_t i = 0; i < samples; ++i) {
        brain->step(static_cast<SimulationStep>(i), 
                   static_cast<Timestamp>(i * timestep));
        firing_rates.push_back(brain->getAverageFiringRate());
        
        // Small delay to avoid overwhelming the system
        if (i % 10 == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
    
    return firing_rates;
}

std::vector<float> collect_reward_history(std::shared_ptr<AgentBrain> agent,
                                          std::shared_ptr<SimpleWorld> world,
                                          size_t steps) {
    std::vector<float> rewards;
    rewards.reserve(steps);
    
    world->reset();
    
    for (size_t i = 0; i < steps; ++i) {
        world->update(0.1);
        
        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);
        
        auto action = agent->decodeMotorCommand();
        world->applyMotorCommand(action, world->getSimulationTime());
        
        float reward = world->getSensoryPercept().getInternal()[0];
        rewards.push_back(reward);
    }
    
    return rewards;
}

} // namespace SimulationHelpers

} // namespace nlm