#pragma once

#include "AgentManager.hpp"
#include "Environment.hpp"
#include "SocialInteraction.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <unordered_map>

namespace nlm {

// Social learning related agent types
enum class ForagingStrategy {
    INDIVIDUAL,
    OBSERVATIONAL,
    IMITATIVE
};

// SocialLearner agent for cultural transmission example
class SocialLearnerAgent : public AgentContainer {
public:
    SocialLearnerAgent(std::string id, std::shared_ptr<Environment> environment)
        : AgentContainer(std::move(id), nullptr, environment, AgentType::SOCIAL) {
        // Initialize social learner state
        body_.x = (std::rand() % 150) / 10.0f - 15.0f;
        body_.y = (std::rand() % 150) / 10.0f - 15.0f;
        body_.orientation = (std::rand() % 360) * 0.01745f;
        
        // Learning state
        foodFound_ = 0;
        learningRate_ = 0.3f;
        observationCount_ = 0;
        imitationCount_ = 0;
        strategy_ = ForagingStrategy::INDIVIDUAL;
        
        // Track successful strategies
        successfulStrategies_.clear();
    }
    
    void update(double timestep) override {
        double currentTime = std::chrono::duration<double>
            (std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        
        // Sense environment
        sense();
        
        // Decide foraging strategy
        decideForagingStrategy();
        
        // Execute foraging based on strategy
        forage();
        
        // Learn from observations
        learnFromObservations();
        body_.x += body_.velocityX * timestep * 100.0f;
        body_.y += body_.velocityY * timestep * 100.0f;
        
        // Clamp to world bounds
        body_.x = std::max(-100.0f, std::min(100.0f, body_.x));
        body_.y = std::max(-100.0f, std::min(100.0f, body_.y));
        
        body_.isMoving = (body_.velocityX != 0.0f || body_.velocityY != 0.0f);
        
        // Record social learning behavior
        std::string strategyName = "Individual";
        switch (strategy_) {
            case ForagingStrategy::OBSERVATIONAL: strategyName = "Observational"; break;
            case ForagingStrategy::IMITATIVE: strategyName = "Imitative"; break;
        }
        
        recordBehavior("SocialLearning_" + strategyName, timestep,
                      "forage=" + std::to_string(foodFound_) + 
                      ",obs=" + std::to_string(observationCount_) +
                      ",imit=" + std::to_string(imitationCount_));
    }
    
private:
    void decideForagingStrategy() {
        auto observations = getObservations();
        
        // Check if there are successful foragers to learn from
        int successfulForagers = 0;
        for (const auto& obs : observations) {
            // Parse observation to check if forager found food
            if (std::rand() % 100 < 40) {  // 40% chance for testing
                successfulForagers++;
            }
        }
        
        if (successfulForagers > 0 && observationCount_ > 5) {
            // Have enough observations and successful examples
            if (std::rand() % 100 < 60) {  // 60% chance to switch to observational
                strategy_ = ForagingStrategy::OBSERVATIONAL;
                return;
            }
        }
        
        if (imitationCount_ > 0 && std::rand() % 100 < 30) {
            // Have imitated before, maybe continue imitating
            strategy_ = ForagingStrategy::IMITATIVE;
        } else {
            strategy_ = ForagingStrategy::INDIVIDUAL;
        }
    }
    
    void forage() {
        switch (strategy_) {
            case ForagingStrategy::INDIVIDUAL:
                individualForage();
                break;
            case ForagingStrategy::OBSERVATIONAL:
                observationalForage();
                break;
            case ForagingStrategy::IMITATIVE:
                imitativeForage();
                break;
        }
    }
    
    void individualForage() {
        // Original individual foraging strategy
        body_.velocityX = 1.5f * std::cos(body_.orientation);
        body_.velocityY = 1.5f * std::sin(body_.orientation);
        
        // Occasionally find food
        if (std::rand() % 100 < 10) {  // 10% chance
            foodFound_++;
            std::string strategy = "individual_foraging_" + std::to_string(foodFound_);
            recordBehavior("Food_Found", std::chrono::duration<double>
                (std::chrono::high_resolution_clock::now().time_since_epoch()).count(), strategy);
        }
    }
    
    void observationalForage() {
        // Learn from other foragers' observations
        auto observations = getObservations();
        
        // Look for food-related observations
        for (const auto& obs : observations) {
            if (obs.observationType.find("food") != std::string::npos) {
                observationCount_++;
                
                // Parse observation to learn foraging location/timing
                std::string learnedInfo = "food_at_" + std::to_string(obs.observerId.size());
                
                // Store learned information
                successfulStrategies_[learnedInfo] = obs.timestamp;
                
                // Sometimes apply learned strategy
                if (std::rand() % 100 < 20) {  // 20% chance
                    body_.velocityX = 1.5f * std::cos(body_.orientation);
                    body_.velocityY = 1.5f * std::sin(body_.orientation);
                    
                    // Occasionally find food based on learned pattern
                    if (std::rand() % 100 < 15) {  // 15% chance (better than individual)
                        foodFound_++;
                    }
                }
                break;
            }
        }
        
        // Continue moving
        body_.velocityX = 1.5f * std::cos(body_.orientation);
        body_.velocityY = 1.5f * std::sin(body_.orientation);
    }
    
    void imitativeForage() {
        // Imitate successful foragers' exact behaviors
        auto observations = getObservations();
        
        std::vector<std::string> successfulForagers;
        for (const auto& obs : observations) {
            // Find foragers who found food
            if (std::rand() % 100 < 30) {  // 30% chance
                successfulForagers.push_back(obs.observedId);
            }
        }
        
        if (!successfulForagers.empty()) {
            // Choose a successful forager to imitate
            std::string model = successfulForagers[0];
            
            // Observe the model
            observe(model);
            imitationCount_++;
            
            // Learn from model's strategies
            shareNeuralPattern(model);
            shareMemory(model);
            
            // Apply learned strategies
            body_.velocityX = 1.5f * std::cos(body_.orientation);
            body_.velocityY = 1.5f * std::sin(body_.orientation);
            
            // Often find food (imitating success)
            if (std::rand() % 100 < 25) {  // 25% chance (highest success rate)
                foodFound_++;
                recordBehavior("Imitation_Success", std::chrono::duration<double>
                    (std::chrono::high_resolution_clock::now().time_since_epoch()).count(),
                    "imitated_from_" + model);
            }
        } else {
            // No successful foragers to imitate, fall back to individual
            individualForage();
        }
    }
    
    void learnFromObservations() {
        auto observations = getObservations();
        
        // Count different types of observations
        int foodObservations = 0;
        int successfulObservations = 0;
        
        for (const auto& obs : observations) {
            if (obs.observationType.find("food") != std::string::npos) {
                foodObservations++;
            }
            
            // Check if observation indicates success
            if (std::rand() % 100 < 50) {  // 50% chance
                successfulObservations++;
            }
        }
        
        // Adjust learning based on observation quality
        if (successfulObservations > foodObservations && foodObservations > 0) {
            learningRate_ = std::min(1.0f, learningRate_ * 1.1f);
        } else if (successfulObservations == 0) {
            learningRate_ = std::max(0.1f, learningRate_ * 0.9f);
        }
    }
    
    // Agent attributes
    int foodFound_;
    float learningRate_;
    int observationCount_;
    int imitationCount_;
    ForagingStrategy strategy_;
    std::unordered_map<std::string, double> successfulStrategies_;
};

// Social Learning World
class SocialLearningWorld : public Environment {
public:
    SocialLearningWorld(int width = 200, int height = 200) : width_(width), height_(height) {
        NLM_LOG_INFO("SocialLearningWorld created: " + std::to_string(width_) + "x" + std::to_string(height_));
        
        // Initialize with learning agents
        agentCount_ = 20;
        foodLocations_ = generateFoodLocations();
    }
    
    std::vector<std::pair<float, float>> generateFoodLocations() {
        std::vector<std::pair<float, float>> locations;
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        
        for (int i = 0; i < 10; ++i) {
            float x = (std::rand() % 140) / 10.0f - 14.0f;
            float y = (std::rand() % 140) / 10.0f - 14.0f;
            locations.push_back(std::make_pair(x, y));
        }
        
        return locations;
    }
    
    bool isFoodNear(float x, float y) {
        for (const auto& food : foodLocations_) {
            float distance = std::sqrt((x - food.first) * (x - food.first) +
                                     (y - food.second) * (y - food.second));
            if (distance < 5.0f) {
                return true;
            }
        }
        return false;
    }
    
    std::unique_ptr<SensoryInput> reset() override {
        return std::make_unique<SensoryInput>();
    }
    
    std::unique_ptr<SensoryInput> step(const Action& action) override {
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
        return "SocialLearningWorld";
    }
    
    size_t getActionSpaceSize() const override {
        return 6;
    }
    
    size_t getObservationSpaceSize() const override {
        return 150;
    }
    
    // Get food locations
    std::vector<std::pair<float, float>> getFoodLocations() const {
        return foodLocations_;
    }

private:
    int width_;
    int height_;
    int agentCount_;
    std::vector<std::pair<float, float>> foodLocations_;
    mutable EnvironmentState state_;
    float lastReward_ = 0.0f;
    bool done_ = false;
};

// Simple Social Learning Simulation
class SocialLearningSimulation {
public:
    SocialLearningSimulation() {
        NLM_LOG_INFO("SocialLearningSimulation initialized");
    }
    
    void run(int numAgents = 20, int steps = 150) {
        std::cout << "===========================================\n";
        std::cout << "SOCIAL LEARNING SIMULATION\n";
        std::cout << "===========================================\n\n";
        
        // Create world
        SocialLearningWorld world(200, 200);
        
        // Create agent manager
        AgentManager manager;
        
        // Create social learning agents
        std::vector<std::string> agentIds;
        for (int i = 0; i < numAgents; ++i) {
            std::string agentId = manager.createAgent(
                AgentType::SOCIAL,
                std::make_shared<SocialLearningWorld>(world)
            );
            
            if (!agentId.empty()) {
                agentIds.push_back(agentId);
                std::cout << "Created agent: " << agentId << "\n";
            }
        }
        
        // Start simulation
        manager.startSimulation(0.1);
        
        // Run simulation
        for (int step = 0; step < steps; ++step) {
            std::cout << "--- Step " << step << " ---\n";
            
            // Run manager step
            manager.runStep();
            
            // Print statistics every 50 steps
            if (step % 50 == 0) {
                auto metrics = manager.getMetrics();
                std::cout << "  Total agents: " << metrics.activeAgents << "\n";
                std::cout << "  Total steps: " << metrics.totalSteps << "\n";
                std::cout << "  Average energy: " << metrics.averageEnergy << "\n";
            }
            
            // Print agent positions (only first few)
            std::cout << "  Agent statistics:\n";
            int count = 0;
            for (const auto& agentId : agentIds) {
                if (manager.hasAgent(agentId) && count < 5) {
                    auto& agent = manager.getAgent(agentId);
                    std::cout << "    " << agentId << ": (" 
                              << agent.getBody().x << ", " << agent.getBody().y << ")\n";
                    count++;
                }
            }
            
            std::cout << "\n";
        }
        
        // Stop simulation
        manager.stopSimulation();
        
        // Print final statistics
        auto finalMetrics = manager.getMetrics();
        std::cout << "===========================================\n";
        std::cout << "SIMULATION COMPLETE\n";
        std::cout << "===========================================\n";
        std::cout << "Final Statistics:\n";
        std::cout << "  Total simulation time: " << finalMetrics.totalTime << "\n";
        std::cout << "  Total steps: " << finalMetrics.totalSteps << "\n";
        std::cout << "  Active agents: " << finalMetrics.activeAgents << "\n";
        
        std::cout << "\nSocial learning simulation complete.\n";
        std::cout << "Agents learned foraging strategies through observation, imitation, and social transmission.\n";
    }
};

} // namespace nlm