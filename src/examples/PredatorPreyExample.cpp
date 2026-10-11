#pragma once

#include "AgentManager.hpp"
#include "Environment.hpp"
#include "SocialInteraction.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

// Predator-Prey agent types
enum class PreyBehavior {
    FORAGING,
    EVASION,
    SOCIAL_LEARNING
};

enum class PredatorBehavior {
    HUNTING,
    PATROLLING,
    SOCIAL_LEARNING
};

// Prey agent for predator-prey dynamics
class PreyAgent : public AgentContainer {
public:
    PreyAgent(std::string id, std::shared_ptr<Environment> environment)
        : AgentContainer(std::move(id), nullptr, environment, AgentType::ACTIVE) {
        // Initialize prey state
        body_.x = (std::rand() % 150) / 10.0f - 15.0f;
        body_.y = (std::rand() % 150) / 10.0f - 15.0f;
        body_.orientation = (std::rand() % 360) * 0.01745f;
        
        energy_ = 100.0f;
        lastForageTime_ = 0.0;
        behavior_ = PreyBehavior::FORAGING;
        
        // Prey characteristics
        speed_ = 2.0f;
        visionRange_ = 30.0f;
        hungerThreshold_ = 30.0f;
    }
    
    void update(double timestep) override {
        double currentTime = std::chrono::duration<double>
            (std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        
        // Update energy
        energy_ -= 0.5f * timestep;
        
        // Sense environment
        sense();
        
        // Check for predators
        bool predatorNear = checkForPredators();
        
        // Decide behavior based on state
        if (energy_ < hungerThreshold_) {
            behavior_ = PreyBehavior::EVASION;
        } else if (predatorNear) {
            behavior_ = PreyBehavior::EVASION;
        } else {
            behavior_ = PreyBehavior::FORAGING;
        }
        
        // Execute behavior
        switch (behavior_) {
            case PreyBehavior::FORAGING:
                forage();
                break;
            case PreyBehavior::EVASION:
                evadePredators();
                break;
            case PreyBehavior::SOCIAL_LEARNING:
                socialLearning();
                break;
        }
        
        // Update position
        body_.x += body_.velocityX * timestep * 100.0f;
        body_.y += body_.velocityY * timestep * 100.0f;
        
        // Clamp to world bounds
        body_.x = std::max(-100.0f, std::min(100.0f, body_.x));
        body_.y = std::max(-100.0f, std::min(100.0f, body_.y));
        
        body_.isMoving = (body_.velocityX != 0.0f || body_.velocityY != 0.0f);
        
        // Record behavior
        recordBehavior("Prey_" + std::to_string(static_cast<int>(behavior_)), timestep, 
                      "energy=" + std::to_string(energy_));
    }
    
private:
    void forage() {
        // Move towards food/resources
        body_.velocityX = speed_ * std::cos(body_.orientation);
        body_.velocityY = speed_ * std::sin(body_.orientation);
        
        // Occasionally change direction
        if (std::rand() % 100 < 5) {  // 5% chance
            body_.orientation += ((std::rand() % 20) - 10) * 0.01745f;
        }
        
        lastForageTime_ = std::chrono::duration<double>
            (std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    }
    
    void evadePredators() {
        // Simple evasion: run away from predators
        // Would need predator positions from observations
        body_.velocityX = -body_.velocityX * 1.5f;  // Reverse direction
        body_.velocityY = -body_.velocityY * 1.5f;
        
        // Change direction randomly
        body_.orientation += ((std::rand() % 20) - 10) * 0.01745f;
        
        body_.velocityX = speed_ * std::cos(body_.orientation);
        body_.velocityY = speed_ * std::sin(body_.orientation);
    }
    
    void socialLearning() {
        // Learn from other prey agents
        auto observations = getObservations();
        
        // Find successful foragers (those with high energy)
        std::vector<std::string> successfulForagers;
        for (const auto& obs : observations) {
            // Parse observation to check forager's energy
            // In real implementation, would extract from observation data
            if (std::rand() % 100 < 30) {  // 30% chance for testing
                successfulForagers.push_back(obs.observedId);
            }
        }
        
        if (!successfulForagers.empty()) {
            // Imitate successful foraging behavior
            std::string model = successfulForagers[0];
            observe(model);
            
            // Learn from model
            auto sharedMemory = getSharedMemory(model, "foraging_strategy");
            if (!sharedMemory.empty()) {
                // Apply learned strategy
                // In real implementation, would modify foraging behavior
            }
        }
    }
    
    bool checkForPredators() {
        auto observations = getObservations();
        for (const auto& obs : observations) {
            // Check if observation is from a predator
            if (obs.observationType.find("predator") != std::string::npos) {
                return true;
            }
        }
        return false;
    }
    
    // Prey attributes
    float energy_;
    double lastForageTime_;
    PreyBehavior behavior_;
    float speed_;
    float visionRange_;
    float hungerThreshold_;
};

// Predator agent for predator-prey dynamics
class PredatorAgent : public AgentContainer {
public:
    PredatorAgent(std::string id, std::shared_ptr<Environment> environment)
        : AgentContainer(std::move(id), nullptr, environment, AgentType::ACTIVE) {
        // Initialize predator state
        body_.x = (std::rand() % 150) / 10.0f - 15.0f + 50.0f;  // Offset to avoid prey
        body_.y = (std::rand() % 150) / 10.0f - 15.0f + 50.0f;
        body_.orientation = (std::rand() % 360) * 0.01745f;
        
        energy_ = 100.0f;
        lastHuntTime_ = 0.0;
        behavior_ = PredatorBehavior::HUNTING;
        
        // Predator characteristics
        speed_ = 3.0f;
        huntingRange_ = 25.0f;
        energyThreshold_ = 30.0f;
    }
    
    void update(double timestep) override {
        double currentTime = std::chrono::duration<double>
            (std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        
        // Update energy
        energy_ -= 0.5f * timestep;
        
        // Sense environment
        sense();
        
        // Check for prey
        bool preyNear = checkForPrey();
        
        // Decide behavior based on state
        if (energy_ < energyThreshold_) {
            behavior_ = PredatorBehavior::PATROLLING;
        } else if (preyNear) {
            behavior_ = PredatorBehavior::HUNTING;
        } else {
            behavior_ = PredatorBehavior::PATROLLING;
        }
        
        // Execute behavior
        switch (behavior_) {
            case PredatorBehavior::HUNTING:
                hunt();
                break;
            case PredatorBehavior::PATROLLING:
                patrol();
                break;
            case PredatorBehavior::SOCIAL_LEARNING:
                socialLearning();
                break;
        }
        
        // Update position
        body_.x += body_.velocityX * timestep * 100.0f;
        body_.y += body_.velocityY * timestep * 100.0f;
        
        // Clamp to world bounds
        body_.x = std::max(-100.0f, std::min(100.0f, body_.x));
        body_.y = std::max(-100.0f, std::min(100.0f, body_.y));
        
        body_.isMoving = (body_.velocityX != 0.0f || body_.velocityY != 0.0f);
        
        // Record behavior
        recordBehavior("Predator_" + std::to_string(static_cast<int>(behavior_)), timestep,
                      "energy=" + std::to_string(energy_));
    }
    
private:
    void hunt() {
        // Simple hunting: move toward prey
        // Would get prey positions from observations
        body_.velocityX = speed_ * std::cos(body_.orientation);
        body_.velocityY = speed_ * std::sin(body_.orientation);
        
        // Occasionally change direction
        if (std::rand() % 100 < 10) {  // 10% chance
            body_.orientation += ((std::rand() % 20) - 10) * 0.01745f;
        }
        
        lastHuntTime_ = std::chrono::duration<double>
            (std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    }
    
    void patrol() {
        // Patrol around territory
        double patrolTime = 60.0;  // seconds
        double timeInPatrol = std::chrono::duration<double>
            (std::chrono::high_resolution_clock::now().time_since_epoch()).count() - lastHuntTime_;
        
        if (timeInPatrol > patrolTime) {
            // Change patrol point
            patrolTargetX_ = (std::rand() % 200) / 10.0f - 10.0f;
            patrolTargetY_ = (std::rand() % 200) / 10.0f - 10.0f;
            lastHuntTime_ = std::chrono::duration<double>
                (std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        }
        
        // Move toward patrol target
        float dx = patrolTargetX_ - body_.x;
        float dy = patrolTargetY_ - body_.y;
        float distance = std::sqrt(dx * dx + dy * dy);
        
        if (distance > 1.0f) {
            body_.orientation = std::atan2(dy, dx);
            body_.velocityX = speed_ * dx / distance;
            body_.velocityY = speed_ * dy / distance;
        }
    }
    
    void socialLearning() {
        // Learn from other predators
        auto observations = getObservations();
        
        // Find successful hunters
        std::vector<std::string> successfulHunters;
        for (const auto& obs : observations) {
            // Check if observation is from successful predator
            if (obs.observationType.find("predator") != std::string::npos) {
                successfulHunters.push_back(obs.observedId);
            }
        }
        
        if (!successfulHunters.empty()) {
            // Imitate successful hunting strategies
            std::string model = successfulHunters[0];
            observe(model);
            
            // Learn hunting techniques
            auto sharedMemory = getSharedMemory(model, "hunting_technique");
            if (!sharedMemory.empty()) {
                // Apply learned technique
                // In real implementation, would modify hunting behavior
            }
        }
    }
    
    bool checkForPrey() {
        auto observations = getObservations();
        for (const auto& obs : observations) {
            // Check if observation is from prey
            if (obs.observationType.find("prey") != std::string::npos) {
                return true;
            }
        }
        return false;
    }
    
    // Predator attributes
    float energy_;
    double lastHuntTime_;
    PredatorBehavior behavior_;
    float speed_;
    float huntingRange_;
    float energyThreshold_;
    float patrolTargetX_;
    float patrolTargetY_;
};

// Predator-Prey World
class PredatorPreyWorld : public Environment {
public:
    PredatorPreyWorld(int width = 200, int height = 200) : width_(width), height_(height) {
        NLM_LOG_INFO("PredatorPreyWorld created: " + std::to_string(width_) + "x" + std::to_string(height_));
        
        // Initialize with some prey and predators
        preyCount_ = 15;
        predatorCount_ = 3;
    }
    
    std::unique_ptr<SensoryInput> reset() override {
        // Reset world
        return std::make_unique<SensoryInput>();
    }
    
    std::unique_ptr<SensoryInput> step(const Action& action) override {
        // Apply action to world
        // In real implementation, would handle interactions between agents
        
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
        return "PredatorPreyWorld";
    }
    
    size_t getActionSpaceSize() const override {
        return 6;
    }
    
    size_t getObservationSpaceSize() const override {
        return 200; // Larger observation space for predator-prey
    }
    
    // Get agent counts
    int getPreyCount() const { return preyCount_; }
    int getPredatorCount() const { return predatorCount_; }

private:
    int width_;
    int height_;
    int preyCount_;
    int predatorCount_;
    mutable EnvironmentState state_;
    float lastReward_ = 0.0f;
    bool done_ = false;
};

// Simple Predator-Prey Simulation
class PredatorPreySimulation {
public:
    PredatorPreySimulation() {
        NLM_LOG_INFO("PredatorPreySimulation initialized");
    }
    
    void run(int numPrey = 15, int numPredators = 3, int steps = 200) {
        std::cout << "===========================================\n";
        std::cout << "PREDATOR-PREY SIMULATION\n";
        std::cout << "===========================================\n\n";
        
        // Create world
        PredatorPreyWorld world(200, 200);
        
        // Create agent manager
        AgentManager manager;
        
        // Create prey agents
        std::vector<std::string> preyIds;
        for (int i = 0; i < numPrey; ++i) {
            std::string agentId = manager.createAgent(
                AgentType::ACTIVE,
                std::make_shared<PredatorPreyWorld>(world)
            );
            
            if (!agentId.empty()) {
                preyIds.push_back(agentId);
            }
        }
        
        // Create predator agents
        std::vector<std::string> predatorIds;
        for (int i = 0; i < numPredators; ++i) {
            std::string agentId = manager.createAgent(
                AgentType::ACTIVE,
                std::make_shared<PredatorPreyWorld>(world)
            );
            
            if (!agentId.empty()) {
                predatorIds.push_back(agentId);
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
            std::cout << "  Agent positions:\n";
            int count = 0;
            for (const auto& agentId : preyIds) {
                if (manager.hasAgent(agentId) && count < 3) {
                    auto& agent = manager.getAgent(agentId);
                    std::cout << "    " << agentId << " (prey): (" 
                              << agent.getBody().x << ", " << agent.getBody().y << ")\n";
                    count++;
                }
            }
            for (const auto& agentId : predatorIds) {
                if (manager.hasAgent(agentId) && count < 6) {
                    auto& agent = manager.getAgent(agentId);
                    std::cout << "    " << agentId << " (predator): (" 
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
        
        std::cout << "\nPredator-prey simulation complete.\n";
        std::cout << "Prey attempt to forage and avoid predators while predators hunt for prey.\n";
    }
};

} // namespace nlm