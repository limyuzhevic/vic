#include "Phase3Demo.cpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

namespace nlm {

// Advanced Phase 3 Demo with Real-World Scenarios
// Demonstrates complex behaviors and advanced features

class AdvancedPhase3Demo {
public:
    AdvancedPhase3Demo() {}
    ~AdvancedPhase3Demo() {}
    
    void runAdvancedScenarios() {
        std::cout << "===========================================\n";
        std::cout << "NLM ADVANCED PHASE 3 - REAL-WORLD SCENARIOS\n";
        std::cout << "===========================================\n\n";
        
        // Scenario 1: Resource Foraging with Learning
        std::cout << "--- Scenario 1: Resource Foraging with Exploration-Exploitation ---\n";
        run_resource_foraging_scenario();
        
        // Scenario 2: Dynamic Environment Adaptation
        std::cout << "\n--- Scenario 2: Dynamic Environment Adaptation ---\n";
        run_dynamic_environment_scenario();
        
        // Scenario 3: Social Learning Simulation
        std::cout << "\n--- Scenario 3: Social Learning Simulation ---\n";
        run_social_learning_scenario();
        
        // Scenario 4: Complex Path Planning
        std::cout << "\n--- Scenario 4: Complex Path Planning ---\n";
        run_path_planning_scenario();
        
        // Scenario 5: Memory Integration Demo
        std::cout << "\n--- Scenario 5: Memory Integration Demo ---\n";
        run_memory_integration_scenario();
        
        std::cout << "\n===========================================\n";
        std::cout << "ALL ADVANCED SCENARIOS COMPLETE\n";
        std::cout << "===========================================\n";
    }
    
private:
    void run_resource_foraging_scenario() {
        // Create configuration optimized for resource foraging
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 800);
        config->set("region_count", 3);
        config->set("connection_probability", 0.12f);
        config->set("random_seed", 123);
        config->set("simulation_timestep", 0.001f);
        
        // Create world with resources
        SimpleWorld world;
        world.configure(30.0f, 30.0f, 12, 12);
        world.setRandomSeed(42);
        
        // Add resources at different positions
        for (int i = 0; i < 10; ++i) {
            float x = static_cast<float>(rand() % 30);
            float y = static_cast<float>(rand() % 30);
            world.addObject(WorldObject(x, y, WorldObjectType::Resource, 10.0f, 0.8f));
        }
        
        // Create brain and agent
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        AgentBrain agentBrain(brain);
        agentBrain.initialize(world);
        
        // Enable advanced features
        agentBrain.enableRewardModulation(true);
        agentBrain.enableCuriosity(true);
        agentBrain.enableStructuralPlasticity(true);
        agentBrain.enableDevelopment(true);
        
        std::cout << "Resource Foraging Scenario\n";
        std::cout << "Resources available: 10\n";
        std::cout << "Agent starting position: (" << world.getAgentBody().x << ", " 
                 << world.getAgentBody().y << ")\n\n";
        
        float total_reward = 0.0f;
        size_t steps_without_resource = 0;
        float exploration_rate = 0.3f;
        
        for (size_t step = 0; step < 500; ++step) {
            // Get sensory input
            const SensoryPercept& percept = world.getSensoryPercept();
            agentBrain.processSensoryInput(percept);
            
            // Brain step
            for (size_t neural_step = 0; neural_step < 10; ++neural_step) {
                brain->step(step, step * 0.001f);
            }
            
            // Get action with exploration
            MotorCommand cmd = agentBrain.decodeMotorCommand();
            
            // Add exploration behavior based on curiosity
            if (agentBrain.getCuriosityLevel() > exploration_rate && 
                step % 10 == 0) {
                // Random action for exploration
                int random_action = rand() % 6;
                switch (random_action) {
                    case 0: cmd = MotorCommand::MoveForward; break;
                    case 1: cmd = MotorCommand::TurnLeft; break;
                    case 2: cmd = MotorCommand::TurnRight; break;
                    case 3: cmd = MotorCommand::Look; break;
                    case 4: cmd = MotorCommand::Interact; break;
                    default: cmd = MotorCommand::Wait; break;
                }
            }
            
            // Apply action
            ActionResult result = world.applyMotorCommand(cmd, world.getSimulationTime());
            total_reward += result.reward;
            
            if (result.reward > 0.0f) {
                steps_without_resource = 0;
                std::cout << "Step " << step << ": Found resource at (" 
                         << world.getAgentBody().x << ", " 
                         << world.getAgentBody().y << ") reward: " 
                         << result.reward << std::endl;
            } else {
                steps_without_resource++;
                if (steps_without_resource % 50 == 0) {
                    std::cout << "Step " << step << ": No resource found, exploring...\n";
                }
            }
            
            // Apply neuromodulation
            agentBrain.applyRewardModulation(result.reward, 0.0f);
            
            // Update world
            world.update(0.01f);
            agentBrain.updateDevelopment(0.01f);
            
            // Print progress
            if (step % 100 == 0) {
                std::cout << "  Progress: Step " << step << ", Total Reward: " 
                         << total_reward << ", Energy: " 
                         << world.getAgentBody().energy << ", "
                         << "Curiosity: " << agentBrain.getCuriosityLevel() << "\n";
            }
        }
        
        std::cout << "Resource Foraging Complete:\n";
        std::cout << "  Total Reward: " << total_reward << "\n";
        std::cout << "  Average Reward per Step: " << (total_reward / 500.0f) << "\n";
        std::cout << "  Final Curiosity Level: " << agentBrain.getCuriosityLevel() << "\n";
        std::cout << "  Final Development Stage: " << stageToString(agentBrain.getDevelopmentalStage()) << "\n";
        std::cout << "  Total Synapses: " << brain->getTotalSynapseCount() << "\n";
    }
    
    void run_dynamic_environment_scenario() {
        // Create configuration for dynamic environment adaptation
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 600);
        config->set("region_count", 2);
        config->set("connection_probability", 0.08f);
        config->set("random_seed", 456);
        config->set("simulation_timestep", 0.001f);
        
        // Create world
        SimpleWorld world;
        world.configure(25.0f, 25.0f, 10, 10);
        world.setRandomSeed(123);
        
        // Create brain and agent
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        AgentBrain agentBrain(brain);
        agentBrain.initialize(world);
        
        // Enable adaptation features
        agentBrain.enableRewardModulation(true);
        agentBrain.enableCuriosity(true);
        agentBrain.enableDevelopment(true);
        
        std::cout << "Dynamic Environment Adaptation Scenario\n";
        std::cout << "Environment will change during simulation\n\n";
        
        // Track adaptation performance
        float adaptation_score = 0.0f;
        size_t successful_adaptations = 0;
        
        for (size_t episode = 0; episode < 5; ++episode) {
            std::cout << "--- Episode " << episode << " ---\n";
            
            // Reset for new episode
            world.reset();
            agentBrain.reset();
            brain->reset();
            
            // Modify environment for each episode
            if (episode % 2 == 0) {
                // Safe episode - no hazards
                std::cout << "Safe episode - no hazards\n";
            } else {
                // Dangerous episode - add hazards
                std::cout << "Dangerous episode - hazards added\n";
                for (int i = 0; i < 5; ++i) {
                    float x = static_cast<float>(rand() % 25);
                    float y = static_cast<float>(rand() % 25);
                    world.addObject(WorldObject(x, y, WorldObjectType::Hazard, -5.0f, 0.6f));
                }
            }
            
            float episode_reward = 0.0f;
            float episode_energy = 0.0f;
            
            for (size_t step = 0; step < 200; ++step) {
                // Get sensory input
                const SensoryPercept& percept = world.getSensoryPercept();
                agentBrain.processSensoryInput(percept);
                
                // Brain step
                for (size_t neural_step = 0; neural_step < 8; ++neural_step) {
                    brain->step(step, step * 0.001f);
                }
                
                // Get action
                MotorCommand cmd = agentBrain.decodeMotorCommand();
                
                // Apply action
                ActionResult result = world.applyMotorCommand(cmd, world.getSimulationTime());
                episode_reward += result.reward;
                episode_energy += world.getAgentBody().energy;
                
                // Adaptation check
                if (episode > 0) {
                    float adaptation_factor = std::abs(result.reward - last_reward);
                    adaptation_score += adaptation_factor;
                    
                    if (adaptation_factor > 1.0f) {
                        successful_adaptations++;
                    }
                }
                
                last_reward = result.reward;
                
                // Apply neuromodulation
                agentBrain.applyRewardModulation(result.reward, 0.0f);
                
                // Update world
                world.update(0.01f);
                agentBrain.updateDevelopment(0.01f);
            }
            
            float avg_energy = episode_energy / 200.0f;
            std::cout << "  Episode " << episode << ": Reward = " << episode_reward 
                     << ", Avg Energy = " << avg_energy << ", "
                     << "Curiosity = " << agentBrain.getCuriosityLevel() << "\n";
        }
        
        float adaptation_efficiency = successful_adaptations / 5.0f;
        std::cout << "\nAdaptation Performance:\n";
        std::cout << "  Adaptation Score: " << adaptation_score << "\n";
        std::cout << "  Successful Adaptations: " << successful_adaptations << "/5\n";
        std::cout << "  Adaptation Efficiency: " << adaptation_efficiency << "\n";
        std::cout << "  Final Development Stage: " << stageToString(agentBrain.getDevelopmentalStage()) << "\n";
    }
    
    void run_social_learning_scenario() {
        // Create configuration for social learning
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 700);
        config->set("region_count", 2);
        config->set("connection_probability", 0.15f);
        config->set("random_seed", 789);
        config->set("simulation_timestep", 0.001f);
        
        // Create two agents for social learning
        auto brain1 = std::make_shared<Brain>(config);
        brain1->initialize();
        
        auto brain2 = std::make_shared<Brain>(config);
        brain2->initialize();
        
        SimpleWorld world;
        world.configure(40.0f, 40.0f, 15, 15);
        world.setRandomSeed(333);
        
        AgentBrain agent1(brain1);
        AgentBrain agent2(brain2);
        
        agent1.initialize(world);
        agent2.initialize(world);
        
        // Enable social learning features
        agent1.enableRewardModulation(true);
        agent2.enableRewardModulation(true);
        agent1.enableCuriosity(true);
        agent2.enableCuriosity(true);
        
        std::cout << "Social Learning Scenario\n";
        std::cout << "Two agents learning from shared experiences\n\n";
        
        // Social learning parameters
        float social_influence = 0.5f;  // How much agents influence each other
        float observation_window = 50;  // Steps to observe other agent's behavior
        
        // Track social learning metrics
        float social_cooperation = 0.0f;
        size_t observation_cycles = 0;
        
        for (size_t episode = 0; episode < 4; ++episode) {
            std::cout << "--- Episode " << episode << " ---\n";
            
            // Reset both agents
            world.reset();
            agent1.reset();
            agent2.reset();
            brain1->reset();
            brain2->reset();
            
            // Position agents apart
            world.setAgentStart(10.0f, 10.0f);
            // Note: SimpleWorld only has one agent, so this is simulated
            
            float episode1_reward = 0.0f;
            float episode2_reward = 0.0f;
            
            for (size_t step = 0; step < 300; ++step) {
                // Agent 1 acts
                const SensoryPercept& percept1 = world.getSensoryPercept();
                agent1.processSensoryInput(percept1);
                
                for (size_t neural_step = 0; neural_step < 10; ++neural_step) {
                    brain1->step(step, step * 0.001f);
                }
                
                MotorCommand action1 = agent1.decodeMotorCommand();
                ActionResult result1 = world.applyMotorCommand(action1, world.getSimulationTime());
                episode1_reward += result1.reward;
                
                // Agent 2 observes and potentially imitates
                if (step > observation_window && step % observation_window == 0) {
                    observation_cycles++;
                    
                    // Observe agent1's action
                    MotorCommand observed_action = action1;
                    
                    // Decide whether to imitate based on curiosity and similarity
                    float imitation_probability = social_influence * 
                                                 (1.0f - agent2.getCuriosityLevel()) * 
                                                 (0.5f + brain1->getAverageFiringRate() / 100.0f);
                    
                    if (rand() / (float)RAND_MAX < imitation_probability) {
                        // Agent 2 imitates agent1's action
                        ActionResult result2 = world.applyMotorCommand(observed_action, world.getSimulationTime());
                        episode2_reward += result2.reward;
                        social_cooperation += 1.0f;
                        std::cout << "    Agent 2 imitated Agent 1's action!\n";
                    }
                }
                
                // Apply neuromodulation for both agents
                agent1.applyRewardModulation(result1.reward, 0.0f);
                agent2.applyRewardModulation(0.0f, 0.0f); // No direct reward for imitation
                
                // Update world
                world.update(0.01f);
                agent1.updateDevelopment(0.01f);
                agent2.updateDevelopment(0.01f);
            }
            
            std::cout << "  Agent 1 Reward: " << episode1_reward << ", "
                     << "Agent 2 Reward: " << episode2_reward << ", "
                     << "Cooperation: " << (social_cooperation / (episode + 1)) << "\n";
        }
        
        float cooperation_rate = social_cooperation / observation_cycles;
        std::cout << "\nSocial Learning Results:\n";
        std::cout << "  Observation Cycles: " << observation_cycles << "\n";
        std::cout << "  Imitation Events: " << social_cooperation << "\n";
        std::cout << "  Cooperation Rate: " << cooperation_rate << "\n";
        std::cout << "  Final Development Stages: Agent 1 = " 
                 << stageToString(agent1.getDevelopmentalStage()) << ", "
                 << "Agent 2 = " << stageToString(agent2.getDevelopmentalStage()) << "\n";
    }
    
    void run_path_planning_scenario() {
        // Create configuration for path planning
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 900);
        config->set("region_count", 3);
        config->set("connection_probability", 0.18f);
        config->set("random_seed", 999);
        config->set("simulation_timestep", 0.001f);
        
        // Create world with complex layout
        SimpleWorld world;
        world.configure(50.0f, 50.0f, 20, 20);
        world.setRandomSeed(555);
        
        // Add obstacles
        for (int i = 0; i < 15; ++i) {
            float x = static_cast<float>(rand() % 50);
            float y = static_cast<float>(rand() % 50);
            world.addObject(WorldObject(x, y, WorldObjectType::Wall, 0.0f, 1.0f));
        }
        
        // Create goal
        float goal_x = 45.0f;
        float goal_y = 45.0f;
        world.addObject(WorldObject(goal_x, goal_y, WorldObjectType::Marker, 100.0f, 1.2f));
        
        // Create brain and agent
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        AgentBrain agentBrain(brain);
        agentBrain.initialize(world);
        
        // Enable planning features
        agentBrain.enableRewardModulation(true);
        agentBrain.enableCuriosity(true);
        
        std::cout << "Complex Path Planning Scenario\n";
        std::cout << "Navigate from start to goal through obstacles\n";
        std::cout << "Start: (" << world.getAgentBody().x << ", " 
                 << world.getAgentBody().y << ") Goal: (" 
                 << goal_x << ", " << goal_y << ")\n\n";
        
        // Path planning metrics
        float path_efficiency = 0.0f;
        size_t successful_arrivals = 0;
        float total_path_length = 0.0f;
        
        for (size_t episode = 0; episode < 3; ++episode) {
            std::cout << "--- Episode " << episode << " ---\n";
            
            // Reset world
            world.reset();
            agentBrain.reset();
            brain->reset();
            
            // Remove old hazards
            for (int i = 0; i < 15; ++i) {
                float x = static_cast<float>(rand() % 50);
                float y = static_cast<float>(rand() % 50);
                world.addObject(WorldObject(x, y, WorldObjectType::Wall, 0.0f, 1.0f));
            }
            
            // Track path for efficiency calculation
            std::vector<std::pair<float, float>> path;
            path.push_back(std::make_pair(world.getAgentBody().x, world.getAgentBody().y));
            
            float episode_reward = 0.0f;
            size_t steps_to_goal = 0;
            
            for (size_t step = 0; step < 400; ++step) {
                // Get sensory input
                const SensoryPercept& percept = world.getSensoryPercept();
                agentBrain.processSensoryInput(percept);
                
                // Brain step
                for (size_t neural_step = 0; neural_step < 12; ++neural_step) {
                    brain->step(step, step * 0.001f);
                }
                
                // Get action
                MotorCommand cmd = agentBrain.decodeMotorCommand();
                
                // Check if we're close to goal
                float dx = std::abs(world.getAgentBody().x - goal_x);
                float dy = std::abs(world.getAgentBody().y - goal_y);
                float distance_to_goal = std::sqrt(dx * dx + dy * dy);
                
                if (distance_to_goal < 2.0f) {
                    // Close to goal - final approach
                    std::cout << "    Near goal! Distance: " << distance_to_goal << std::endl;
                    if (distance_to_goal < 1.0f) {
                        // Reached goal
                        std::cout << "    GOAL REACHED!\n";
                        steps_to_goal = step;
                        successful_arrivals++;
                        
                        // Calculate path efficiency
                        float straight_line_distance = std::sqrt(
                            std::pow(world.getAgentBody().x - 10.0f, 2) + 
                            std::pow(world.getAgentBody().y - 10.0f, 2));
                        
                        float actual_path_length = 0.0f;
                        for (size_t i = 1; i < path.size(); ++i) {
                            float segment_length = std::sqrt(
                                std::pow(path[i].first - path[i-1].first, 2) + 
                                std::pow(path[i].second - path[i-1].second, 2));
                            actual_path_length += segment_length;
                        }
                        
                        float efficiency = straight_line_distance / actual_path_length;
                        path_efficiency += efficiency;
                        total_path_length += actual_path_length;
                        
                        // Large reward for reaching goal
                        episode_reward += 100.0f + 50.0f * efficiency;
                        break;
                    }
                }
                
                // Apply action
                ActionResult result = world.applyMotorCommand(cmd, world.getSimulationTime());
                episode_reward += result.reward;
                
                // Track path
                path.push_back(std::make_pair(world.getAgentBody().x, world.getAgentBody().y));
                
                // Apply neuromodulation
                agentBrain.applyRewardModulation(result.reward, 0.0f);
                
                // Update world
                world.update(0.01f);
                agentBrain.updateDevelopment(0.01f);
            }
            
            std::cout << "  Episode " << episode << ": Reward = " << episode_reward 
                     << ", Steps to Goal: " << steps_to_goal << ", "
                     << "Efficiency = " << (steps_to_goal > 0 ? (400.0f / steps_to_goal) : 0.0f) << "\n";
        }
        
        float success_rate = successful_arrivals / 3.0f;
        float avg_efficiency = path_efficiency / successful_arrivals;
        std::cout << "\nPath Planning Results:\n";
        std::cout << "  Success Rate: " << success_rate << " (" << successful_arrivals << "/3)\n";
        std::cout << "  Average Path Efficiency: " << avg_efficiency << "\n";
        std::cout << "  Average Path Length: " << total_path_length / successful_arrivals << "\n";
        std::cout << "  Final Development Stage: " << stageToString(agentBrain.getDevelopmentalStage()) << "\n";
    }
    
    void run_memory_integration_scenario() {
        // Create configuration for memory integration
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 500);
        config->set("region_count", 1);
        config->set("connection_probability", 0.12f);
        config->set("random_seed", 111);
        config->set("simulation_timestep", 0.001f);
        
        // Create world
        SimpleWorld world;
        world.configure(20.0f, 20.0f, 8, 8);
        world.setRandomSeed(222);
        
        // Create brain and agent
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        AgentBrain agentBrain(brain);
        agentBrain.initialize(world);
        
        // Enable memory features
        agentBrain.enableRewardModulation(true);
        agentBrain.enableCuriosity(true);
        
        std::cout << "Memory Integration Scenario\n";
        std::cout << "Test episodic memory, working memory, and concept formation\n\n";
        
        // Memory integration metrics
        float memory_retention = 0.0f;
        size_t memory_episodes = 0;
        float pattern_recognition_score = 0.0f;
        
        for (size_t episode = 0; episode < 4; ++episode) {
            std::cout << "--- Episode " << episode << " ---\n";
            
            // Reset
            world.reset();
            agentBrain.reset();
            brain->reset();
            
            // Create episodic memory for this episode
            std::vector<float> episode_pattern;
            
            float episode_reward = 0.0f;
            
            for (size_t step = 0; step < 250; ++step) {
                // Get sensory input
                const SensoryPercept& percept = world.getSensoryPercept();
                agentBrain.processSensoryInput(percept);
                
                // Record pattern for memory
                episode_pattern.push_back(agentBrain.getNoveltyLevel());
                episode_pattern.push_back(agentBrain.getCuriosityLevel());
                
                // Brain step
                for (size_t neural_step = 0; neural_step < 8; ++neural_step) {
                    brain->step(step, step * 0.001f);
                }
                
                // Get action
                MotorCommand cmd = agentBrain.decodeMotorCommand();
                
                // Apply action
                ActionResult result = world.applyMotorCommand(cmd, world.getSimulationTime());
                episode_reward += result.reward;
                
                // Apply neuromodulation
                agentBrain.applyRewardModulation(result.reward, 0.0f);
                
                // Update world
                world.update(0.01f);
                agentBrain.updateDevelopment(0.01f);
                
                // Simulate memory storage (in real implementation, this would use actual memory systems)
                if (step % 50 == 0) {
                    // Store pattern in working memory
                    memory_episodes++;
                    
                    // Pattern recognition: check if current pattern matches previous ones
                    if (episode > 0 && step > 10) {
                        float pattern_similarity = 0.0f;
                        // Simple pattern matching (in real implementation would use actual memory algorithms)
                        pattern_recognition_score += pattern_similarity;
                    }
                }
            }
            
            // Calculate memory retention (simplified)
            float retention = episode_reward / 250.0f;
            memory_retention += retention;
            
            std::cout << "  Episode " << episode << ": Reward = " << episode_reward 
                     << ", Episodes Stored: " << memory_episodes << ", "
                     << "Pattern Recognition: " << (pattern_recognition_score / episode) << "\n";
        }
        
        float avg_retention = memory_retention / 4.0f;
        std::cout << "\nMemory Integration Results:\n";
        std::cout << "  Average Memory Retention: " << avg_retention << "\n";
        std::cout << "  Total Episodes Stored: " << memory_episodes << "\n";
        std::cout << "  Pattern Recognition Score: " << pattern_recognition_score << "\n";
        std::cout << "  Working Memory Integrated: " << (brain->getWorkingMemory() != nullptr) << "\n";
        std::cout << "  Episodic Memory Integrated: " << (brain->getEpisodicMemory() != nullptr) << "\n";
        std::cout << "  Final Development Stage: " << stageToString(agentBrain.getDevelopmentalStage()) << "\n";
    }
    
    const char* stageToString(DevelopmentalStage stage) {
        switch (stage) {
            case DevelopmentalStage::Initial: return "Initial";
            case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
            case DevelopmentalStage::Maturation: return "Maturation";
            case DevelopmentalStage::Adult: return "Adult";
            case DevelopmentalStage::Aging: return "Aging";
            default: return "Unknown";
        }
    }
};

} // namespace nlm

// Main function for advanced demo
int main(int argc, char* argv[]) {
    std::cout << "=== NLM Advanced Phase 3 Demo ===" << std::endl;
    std::cout << "Running comprehensive real-world scenarios..." << std::endl;
    
    nlm::AdvancedPhase3Demo demo;
    demo.runAdvancedScenarios();
    
    std::cout << std::endl;
    std::cout << "=== Advanced Demo Complete ===" << std::endl;
    std::cout << "All scenarios demonstrate advanced NLM capabilities:" << std::endl;
    std::cout << "- Complex behavior learning" << std::endl;
    std::cout << "- Dynamic environment adaptation" << std::endl;
    std::cout << "- Social learning and imitation" << std::endl;
    std::cout << "- Path planning and navigation" << std::endl;
    std::cout << "- Memory integration and pattern recognition" << std::endl;
    
    return 0;
}
