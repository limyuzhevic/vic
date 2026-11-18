#!/usr/bin/env cpp
// Advanced NLM Example: Learning Agent in Complex Environment
//
// This example demonstrates an advanced NLM agent learning to navigate
// and interact with a complex environment, showcasing the full capabilities
// of the neural learning machine.

#include "brain/Brain.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>

using namespace nlm;

// Advanced Learning Agent Class
class AdvancedLearningAgent {
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<AgentBrain> agent_;
    std::shared_ptr<SimpleWorld> world_;
    
    // Learning statistics
    struct LearningStats {
        double total_reward;
        double avg_curiosity;
        double avg_novelty;
        double curiosity_growth;
        double novelty_growth;
        size_t successful_actions;
        size_t failed_actions;
        
        LearningStats() : total_reward(0.0), avg_curiosity(0.0), avg_novelty(0.0),
                        curiosity_growth(0.0), novelty_growth(0.0),
                        successful_actions(0), failed_actions(0) {}
    };
    
    LearningStats stats_;
    
    // Performance tracking
    struct PerformanceMetrics {
        double total_steps;
        double learning_rate;
        double exploration_rate;
        double adaptation_speed;
        double memory_efficiency;
        
        PerformanceMetrics() : total_steps(0.0), learning_rate(0.0),
                              exploration_rate(0.0), adaptation_speed(0.0),
                              memory_efficiency(0.0) {}
    };
    
    PerformanceMetrics perf_;
    
public:
    AdvancedLearningAgent(const std::string& config_file = "") {
        initialize(config_file);
    }
    
    void initialize(const std::string& config_file = "") {
        std::cout << "Initializing Advanced Learning Agent..." << std::endl;
        
        // Create configuration
        auto config = std::make_shared<Config>();
        
        if (!config_file.empty()) {
            config->loadFromFile(config_file);
        }
        
        // Advanced configuration for complex learning
        config->set("brain.neuron_count", 2000, ConfigSource::Runtime);
        config->set("brain.synapse_density", 0.15, ConfigSource::Runtime);
        config->set("brain.connection_probability", 0.08, ConfigSource::Runtime);
        
        // Enable advanced plasticity
        config->set("plasticity.stdp.enable", true, ConfigSource::Runtime);
        config->set("plasticity.stdp.learning_rate", 0.005, ConfigSource::Runtime);
        config->set("plasticity.hebbian.enable", true, ConfigSource::Runtime);
        config->set("plasticity.hebbian.learning_rate", 0.0005, ConfigSource::Runtime);
        config->set("plasticity.structural.enable", true, ConfigSource::Runtime);
        config->set("plasticity.structural.synaptogenesis_rate", 0.0002, ConfigSource::Runtime);
        config->set("plasticity.structural.pruning_rate", 0.00002, ConfigSource::Runtime);
        
        // Enable advanced neuromodulation
        config->set("neuromod.dopamine.scale", 1.5, ConfigSource::Runtime);
        config->set("neuromod.curiosity.enable", true, ConfigSource::Runtime);
        config->set("neuromod.curiosity.strength", 0.8, ConfigSource::Runtime);
        config->set("neuromod.novelty.enable", true, ConfigSource::Runtime);
        config->set("neuromod.novelty.sensitivity", 0.4, ConfigSource::Runtime);
        
        // Enable development and memory
        config->set("development.enable", true, ConfigSource::Runtime);
        config->set("development.maturation_rate", 0.02, ConfigSource::Runtime);
        
        // Create world
        world_ = std::make_shared<SimpleWorld>();
        world_->configure(
            50,  // width
            50,  // height
            16,  // vision width
            16   // vision height
        );
        
        // Create brain
        brain_ = std::make_shared<Brain>(config);
        brain_->initialize();
        
        // Create agent
        agent_ = std::make_shared<AgentBrain>(brain_);
        agent_->initialize(*world_);
        
        // Enable all advanced features
        agent_->enableAllFeatures();
        
        std::cout << "Agent initialization complete." << std::endl;
    }
    
    void runTraining(int num_episodes, int steps_per_episode) {
        std::cout << "Starting advanced training..." << std::endl;
        std::cout << "Episodes: " << num_episodes << ", Steps per episode: " << steps_per_episode << std::endl;
        
        resetStats();
        
        for (int episode = 0; episode < num_episodes; ++episode) {
            std::cout << "\nEpisode " << (episode + 1) << "/" << num_episodes << ":" << std::endl;
            
            // Reset for new episode
            resetEpisode();
            
            // Run episode
            runEpisode(steps_per_episode, episode);
            
            // Print episode summary
            printEpisodeSummary(episode);
        }
        
        // Print final training summary
        printTrainingSummary();
    }
    
    void runEpisode(int steps, int episode) {
        double episode_reward = 0.0;
        double episode_curiosity = 0.0;
        double episode_novelty = 0.0;
        
        for (int step = 0; step < steps; ++step) {
            // Update world
            world_->update(0.1);
            
            // Get sensory input
            auto percept = world_->getSensoryPercept();
            
            // Process sensory input
            agent_->processSensoryInput(*percept);
            
            // Brain computation
            brain_->step(step);
            
            // Get action
            auto action = agent_->decodeMotorCommand();
            
            // Apply action in world
            auto result = world_->applyMotorCommand(action, world_->getSimulationTime());
            
            // Apply reward modulation
            if (result) {
                double reward = result->reward;
                episode_reward += reward;
                agent_->applyRewardModulation(reward, 0.0);
                
                if (reward > 0.0) {
                    stats_.successful_actions++;
                } else if (reward < 0.0) {
                    stats_.failed_actions++;
                }
            }
            
            // Update development
            agent_->updateDevelopment(0.1);
            
            // Collect statistics
            episode_curiosity += agent_->getCuriosityLevel();
            episode_novelty += agent_->getNoveltyLevel();
            
            // Progress reporting
            if (step % 100 == 0) {
                printProgress(step, steps, episode_reward, episode_curiosity/(step+1), episode_novelty/(step+1));
            }
        }
        
        // Update statistics
        stats_.total_reward += episode_reward;
        stats_.avg_curiosity += episode_curiosity / steps;
        stats_.avg_novelty += episode_novelty / steps;
        
        perf_.total_steps += steps;
    }
    
    void printProgress(int step, int total, double reward, double curiosity, double novelty) {
        std::cout << "  Step " << step << "/" << total
                  << " | Reward: " << std::fixed << std::setprecision(2) << reward
                  << " | Curiosity: " << std::fixed << std::setprecision(3) << curiosity
                  << " | Novelty: " << std::fixed << std::setprecision(3) << novelty << std::endl;
    }
    
    void printEpisodeSummary(int episode) {
        std::cout << "    Total reward: " << std::fixed << std::setprecision(2) << stats_.total_reward << std::endl;
        std::cout << "    Avg curiosity: " << std::fixed << std::setprecision(3) << (stats_.avg_curiosity / (episode + 1)) << std::endl;
        std::cout << "    Avg novelty: " << std::fixed << std::setprecision(3) << (stats_.avg_novelty / (episode + 1)) << std::endl;
        std::cout << "    Success/Failure: " << stats_.successful_actions << "/" << stats_.failed_actions << std::endl;
    }
    
    void printTrainingSummary() {
        std::cout << "\n" << std::string(60, '=') << std::endl;
        std::cout << "TRAINING SUMMARY" << std::endl;
        std::cout << std::string(60, '=') << std::endl;
        
        std::cout << "\nPerformance Metrics:" << std::endl;
        std::cout << "  Total steps: " << perf_.total_steps << std::endl;
        std::cout << "  Average steps per episode: " << perf_.total_steps / stats_.successful_actions << std::endl;
        std::cout << "  Success rate: " << (stats_.successful_actions * 100.0 / (stats_.successful_actions + stats_.failed_actions)) << "%" << std::endl;
        
        std::cout << "\nLearning Metrics:" << std::endl;
        std::cout << "  Total reward: " << std::fixed << std::setprecision(2) << stats_.total_reward << std::endl;
        std::cout << "  Average curiosity: " << std::fixed << std::setprecision(3) << (stats_.avg_curiosity / stats_.successful_actions) << std::endl;
        std::cout << "  Average novelty: " << std::fixed << std::setprecision(3) << (stats_.avg_novelty / stats_.successful_actions) << std::endl;
        
        std::cout << "\nDevelopment Status:" << std::endl;
        std::cout << "  Developmental stage: " << static_cast<int>(brain_->getDevelopmentalStage()) << std::endl;
        std::cout << "  Neural maturity: " << (brain_->getAverageFiringRate() / 100.0) * 100 << "%" << std::endl;
        
        // Save brain state
        brain_->save("advanced_learning_agent.bin");
        std::cout << "\nBrain state saved to: advanced_learning_agent.bin" << std::endl;
    }
    
    void resetStats() {
        stats_ = LearningStats();
        perf_ = PerformanceMetrics();
    }
    
    void resetEpisode() {
        world_->reset();
        agent_->reset();
        brain_->reset();
        brain_->initialize();
    }
    
    std::shared_ptr<Brain> getBrain() const { return brain_; }
    std::shared_ptr<AgentBrain> getAgent() const { return agent_; }
    std::shared_ptr<SimpleWorld> getWorld() const { return world_; }
};

int main(int argc, char* argv[]) {
    printBanner();
    
    std::cout << "Advanced NLM Learning Agent Example" << std::endl;
    std::cout << "===================================" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Parse command line arguments
    std::string config_file = "";
    int num_episodes = 10;
    int steps_per_episode = 500;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--config" && i + 1 < argc) {
            config_file = argv[++i];
        } else if (arg == "--episodes" && i + 1 < argc) {
            num_episodes = std::stoi(argv[++i]);
        } else if (arg == "--steps" && i + 1 < argc) {
            steps_per_episode = std::stoi(argv[++i]);
        } else if (arg == "--help" || arg == "-h") {
            printHelp();
            return 0;
        }
    }
    
    // Create and run advanced learning agent
    AdvancedLearningAgent agent(config_file);
    agent.runTraining(num_episodes, steps_per_episode);
    
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "ADVANCED LEARNING AGENT EXAMPLE COMPLETE" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    std::cout << "\nThe advanced learning agent has successfully demonstrated:" << std::endl;
    std::cout << "  - Complex world navigation and interaction" << std::endl;
    std::cout << "  - Advanced learning through neuromodulation" << std::endl;
    std::cout << "  - Developmental progression and adaptation" << std::endl;
    std::cout << "  - Curiosity-driven exploration" << std::endl;
    std::cout << "  - Novelty-based learning and adaptation" << std::endl;
    std::cout << "  - Efficient action selection and reward optimization" << std::endl;
    
    return 0;
}

void printBanner() {
    std::cout << "Advanced NLM Learning Agent Example" << std::endl;
    std::cout << "===================================" << std::endl;
    std::cout << "\nThis example demonstrates an advanced artificial agent that learns" << std::endl;
    std::cout << "to navigate and interact with a complex environment using NLM's" << std::endl;
    std::cout << "full capabilities including memory systems, neuromodulation, and" << std::endl;
    std::cout << "developmental plasticity.\n";
}

void printHelp() {
    std::cout << "Advanced NLM Learning Agent Example" << std::endl;
    std::cout << "===================================" << std::endl;
    std::cout << "\nUsage: ./advanced_learning_agent [OPTIONS]" << std::endl;
    std::cout << "\nOptions:" << std::endl;
    std::cout << "  --config <file>      Configuration file path" << std::endl;
    std::cout << "  --episodes <n>       Number of training episodes (default: 10)" << std::endl;
    std::cout << "  --steps <n>          Steps per episode (default: 500)" << std::endl;
    std::cout << "  --help, -h           Show this help message" << std::endl;
    std::cout << "\nExample:" << std::endl;
    std::cout << "  ./advanced_learning_agent --episodes 20 --steps 1000 --config my_config.cfg" << std::endl;
}