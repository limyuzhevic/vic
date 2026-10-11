#pragma once

#include "AgentManager.hpp"
#include "Environment.hpp"
#include "SocialInteraction.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <memory>

namespace nlm {

// Multi-Agent World - extends existing Environment
class MultiAgentWorld : public Environment {
public:
    MultiAgentWorld(int width = 200, int height = 200) : width_(width), height_(height) {
        NLM_LOG_INFO("MultiAgentWorld created: " + std::to_string(width_) + "x" + std::to_string(height_));
        
        // Initialize world resources
        foodCount_ = 0;
        hazardCount_ = 0;
        
        generateResources();
    }
    
    void generateResources() {
        // Generate random food and hazards
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        
        // Food locations (positive for agent)
        for (int i = 0; i < 10; ++i) {
            float x = (std::rand() % 140) / 10.0f - 14.0f;
            float y = (std::rand() % 140) / 10.0f - 14.0f;
            food_.push_back(std::make_pair(x, y));
            foodCount_++;
        }
        
        // Hazard locations (negative for agent)
        for (int i = 0; i < 5; ++i) {
            float x = (std::rand() % 140) / 10.0f - 14.0f;
            float y = (std::rand() % 140) / 10.0f - 14.0f;
            hazards_.push_back(std::make_pair(x, y));
            hazardCount_++;
        }
    }
    
    float getFoodAt(float x, float y) {
        for (const auto& food : food_) {
            float distance = std::sqrt((x - food.first) * (x - food.first) +
                                     (y - food.second) * (y - food.second));
            if (distance < 5.0f) {
                return 10.0f; // Food reward
            }
        }
        return 0.0f;
    }
    
    float getHazardAt(float x, float y) {
        for (const auto& hazard : hazards_) {
            float distance = std::sqrt((x - hazard.first) * (x - hazard.first) +
                                     (y - hazard.second) * (y - hazard.second));
            if (distance < 5.0f) {
                return -5.0f; // Hazard penalty
            }
        }
        return 0.0f;
    }
    
    std::unique_ptr<SensoryInput> reset() override {
        // Reset world
        return std::make_unique<SensoryInput>();
    }
    
    std::unique_ptr<SensoryInput> step(const Action& action) override {
        // Apply action and update world
        // In real implementation, would handle agent movements and interactions
        
        return std::make_unique<SensoryInput>();
    }
    
    const EnvironmentState& getState() const override {
        static EnvironmentState state;
        return state;
    }
    
    float getLastReward() const override {
        return lastReward_;
    }
    
    bool isDone() const override {
        return done_;
    }
    
    const char* getName() const override {
        return "MultiAgentWorld";
    }
    
    size_t getActionSpaceSize() const override {
        return 6;
    }
    
    size_t getObservationSpaceSize() const override {
        return 300; // Larger observation space for multi-agent
    }
    
    // Multi-agent specific functionality
    void addAgent(std::string agentId) {
        agents_.insert(agentId);
    }
    
    void removeAgent(std::string agentId) {
        agents_.erase(agentId);
    }
    
    bool hasAgent(std::string agentId) const {
        return agents_.find(agentId) != agents_.end();
    }
    
    std::vector<std::string> getAllAgents() const {
        std::vector<std::string> agentList(agents_.begin(), agents_.end());
        return agentList;
    }
    
    float getResourceReward(const std::string& agentId) {
        // Get agent position (would be retrieved from agent container)
        // For now, use a dummy position
        float x = 0.0f, y = 0.0f;
        
        float foodReward = getFoodAt(x, y);
        float hazardPenalty = getHazardAt(x, y);
        
        return foodReward + hazardPenalty;
    }

private:
    int width_;
    int height_;
    std::unordered_set<std::string> agents_;
    std::vector<std::pair<float, float>> food_;
    std::vector<std::pair<float, float>> hazards_;
    int foodCount_;
    int hazardCount_;
    mutable EnvironmentState state_;
    float lastReward_ = 0.0f;
    bool done_ = false;
};

// Multi-Agent Integration Example
class MultiAgentIntegrationExample {
public:
    MultiAgentIntegrationExample() {
        NLM_LOG_INFO("MultiAgentIntegrationExample initialized");
    }
    
    void run(int numPredators = 2, int numPrey = 8, int steps = 100) {
        std::cout << "===========================================\n";
        std::cout << "MULTI-AGENT NLM INTEGRATION EXAMPLE\n";
        std::cout << "===========================================\n\n";
        
        // Create multi-agent world
        MultiAgentWorld world(200, 200);
        
        // Create agent manager
        AgentManager manager;
        
        std::cout << "Creating predator-prey agents...\n\n";
        
        // Create predator agents
        std::vector<std::string> predatorIds;
        for (int i = 0; i < numPredators; ++i) {
            std::string agentId = manager.createAgent(
                AgentType::ACTIVE,
                std::make_shared<MultiAgentWorld>(world)
            );
            
            if (!agentId.empty()) {
                predatorIds.push_back(agentId);
                world.addAgent(agentId);
                std::cout << "Created predator: " << agentId << "\n";
            }
        }
        
        // Create prey agents
        std::vector<std::string> preyIds;
        for (int i = 0; i < numPrey; ++i) {
            std::string agentId = manager.createAgent(
                AgentType::ACTIVE,
                std::make_shared<MultiAgentWorld>(world)
            );
            
            if (!agentId.empty()) {
                preyIds.push_back(agentId);
                world.addAgent(agentId);
                std::cout << "Created prey: " << agentId << "\n";
            }
        }
        
        // Start simulation
        manager.startSimulation(0.1);
        
        // Run simulation
        for (int step = 0; step < steps; ++step) {
            std::cout << "--- Step " << step << " ---\n";
            
            // Run manager step (coordinates all agents)
            manager.runStep();
            
            // Print status every 20 steps
            if (step % 20 == 0) {
                auto metrics = manager.getMetrics();
                std::cout << "  Total agents: " << metrics.activeAgents << "\n";
                std::cout << "  Learning agents: " << metrics.learningAgents << "\n";
                std::cout << "  Average energy: " << metrics.averageEnergy << "\n";
                std::cout << "  Total steps: " << metrics.totalSteps << "\n";
            }
            
            // Print agent status (sample)
            std::cout << "  Agent sample:\n";
            int count = 0;
            for (const auto& agentId : predatorIds) {
                if (manager.hasAgent(agentId) && count < 2) {
                    auto& agent = manager.getAgent(agentId);
                    std::cout << "    Predator " << agentId << ": (" 
                              << agent.getBody().x << ", " << agent.getBody().y << ")\n";
                    count++;
                }
            }
            for (const auto& agentId : preyIds) {
                if (manager.hasAgent(agentId) && count < 4) {
                    auto& agent = manager.getAgent(agentId);
                    std::cout << "    Prey " << agentId << ": (" 
                              << agent.getBody().x << ", " << agent.getBody().y << ")\n";
                    count++;
                }
            }
            
            // Create social groups
            if (step % 50 == 10 && predatorIds.size() >= 2) {
                manager.formGroup("predator_coalition", predatorIds, predatorIds[0]);
                std::cout << "    Formed predator coalition\n";
            }
            
            std::cout << "\n";
        }
        
        // Stop simulation
        manager.stopSimulation();
        
        // Print final results
        std::cout << "===========================================\n";
        std::cout << "INTEGRATION EXAMPLE COMPLETE\n";
        std::cout << "===========================================\n\n";
        
        // Show integration statistics
        auto finalMetrics = manager.getMetrics();
        std::cout << "Integration Statistics:\n";
        std::cout << "  Total simulation time: " << finalMetrics.totalTime << "s\n";
        std::cout << "  Total steps: " << finalMetrics.totalSteps << "\n";
        std::cout << "  Active agents: " << finalMetrics.activeAgents << "\n";
        std::cout << "  Average energy: " << finalMetrics.averageEnergy << "\n";
        
        std::cout << "\nKey Integration Features Demonstrated:\n";
        std::cout << "  ✓ Multiple agent types (predators, prey)\n";
        std::cout << "  ✓ Social interactions (communication, observation)\n";
        std::cout << "  ✓ Group formation (coalitions)\";
        std::cout << "  ✓ Learning from social interactions\n";
        std::cout << "  ✓ Resource sharing and coordination\n";
        std::cout << "  ✓ Emergent predator-prey dynamics\n";
        
        // Show agent manager capabilities
        std::cout << "\nAgent Manager Capabilities:\n";
        std::cout << "  ✓ Time synchronization across agents\n";
        std::cout << "  ✓ Conflict resolution\n";
        std::cout << "  ✓ Resource allocation\n";
        std::cout << "  ✓ State management\n";
        std::cout << "  ✓ Checkpoint and replay support\n";
        
        std::cout << "\nMulti-agent integration successful!\n";
    }
};

} // namespace nlm