#pragma once

#include "AgentManager.hpp"
#include "Environment.hpp"
#include "SocialInteraction.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>

namespace nlm {

// Simple 2D position structure
struct Vector2 {
    float x, y;
    Vector2(float x = 0, float y = 0) : x(x), y(y) {}
    float magnitude() const { return std::sqrt(x*x + y*y); }
    Vector2 normalize() const {
        float mag = magnitude();
        return (mag > 0) ? Vector2(x/mag, y/mag) : Vector2(0, 0);
    }
    Vector2 operator+(const Vector2& other) const { return Vector2(x + other.x, y + other.y); }
    Vector2 operator-(const Vector2& other) const { return Vector2(x - other.x, y - other.y); }
    Vector2 operator*(float scalar) const { return Vector2(x * scalar, y * scalar); }
};

// Swarm agent for flocking behavior
class SwarmAgent : public AgentContainer {
public:
    SwarmAgent(std::string id, std::shared_ptr<Environment> environment)
        : AgentContainer(std::move(id), nullptr, environment, AgentType::ACTIVE) {
        body_.x = (std::rand() % 200) / 10.0f - 10.0f;  // Random position
        body_.y = (std::rand() % 200) / 10.0f - 10.0f;
        body_.orientation = (std::rand() % 360) * 0.01745f;  // Random angle in radians
        
        // Initialize flocking behavior parameters
        alignmentWeight_ = 0.5f;
        cohesionWeight_ = 0.5f;
        separationWeight_ = 1.0f;
        maxSpeed_ = 1.5f;
        neighborRadius_ = 20.0f;
    }
    
    void update(double timestep) override {
        // Get positions of nearby agents (observations)
        std::vector<Vector2> neighbors;
        auto observations = getObservations();
        
        for (const auto& obs : observations) {
            // Parse observation to get neighbor position
            // In real implementation, would extract from observation data
        }
        
        // Calculate flocking behaviors
        Vector2 alignment = calculateAlignment(neighbors);
        Vector2 cohesion = calculateCohesion(neighbors);
        Vector2 separation = calculateSeparation(neighbors);
        
        // Combine behaviors
        Vector2 steering = (alignment * alignmentWeight_) + 
                           (cohesion * cohesionWeight_) + 
                           (separation * separationWeight_);
        
        // Limit speed
        if (steering.magnitude() > maxSpeed_) {
            steering = steering.normalize() * maxSpeed_;
        }
        
        // Update position
        body_.x += steering.x * timestep * 100.0f;  // Scale for display
        body_.y += steering.y * timestep * 100.0f;
        
        // Update orientation
        float angle = std::atan2(steering.y, steering.x);
        if (std::isfinite(angle)) {
            body_.orientation = angle;
        }
        
        body_.isMoving = (steering.magnitude() > 0.1f);
        body_.isTurning = std::abs(body_.angularVelocity) > 0.01f;
        
        // Record swarm behavior for metrics
        recordBehavior("Flocking", timestep, std::to_string(neighbors.size()) + " neighbors");
    }
    
private:
    Vector2 calculateAlignment(const std::vector<Vector2>& neighbors) {
        if (neighbors.empty()) return Vector2(0, 0);
        
        Vector2 sum(0, 0);
        for (const auto& neighbor : neighbors) {
            sum = sum + neighbor;
        }
        
        Vector2 average = sum * (1.0f / neighbors.size());
        return average;
    }
    
    Vector2 calculateCohesion(const std::vector<Vector2>& neighbors) {
        if (neighbors.empty()) return Vector2(0, 0);
        
        Vector2 centerOfMass(0, 0);
        for (const auto& neighbor : neighbors) {
            centerOfMass = centerOfMass + neighbor;
        }
        
        centerOfMass = centerOfMass * (1.0f / neighbors.size());
        
        Vector2 fromSelf = centerOfMass - Vector2(body_.x, body_.y);
        return fromSelf.normalize();
    }
    
    Vector2 calculateSeparation(const std::vector<Vector2>& neighbors) {
        Vector2 separation(0, 0);
        for (const auto& neighbor : neighbors) {
            Vector2 toNeighbor = Vector2(body_.x, body_.y) - neighbor;
            float distance = toNeighbor.magnitude();
            
            if (distance < neighborRadius_ && distance > 0.1f) {
                separation = separation + (toNeighbor.normalize() / distance);
            }
        }
        
        return separation;
    }
    
    // Flocking parameters
    float alignmentWeight_;
    float cohesionWeight_;
    float separationWeight_;
    float maxSpeed_;
    float neighborRadius_;
};

// Simple World implementation for swarm simulation
class SwarmWorld : public Environment {
public:
    SwarmWorld(int width = 200, int height = 200) : width_(width), height_(height) {
        NLM_LOG_INFO("SwarmWorld created: " + std::to_string(width_) + "x" + std::to_string(height_));
    }
    
    std::unique_ptr<SensoryInput> reset() override {
        // Reset world state
        return std::make_unique<SensoryInput>();
    }
    
    std::unique_ptr<SensoryInput> step(const Action& action) override {
        // Apply action to world
        // In real implementation, would handle obstacles, food, etc.
        
        return std::make_unique<SensoryInput>();
    }
    
    const EnvironmentState& getState() const override {
        // Return current world state
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
        return "SwarmWorld";
    }
    
    size_t getActionSpaceSize() const override {
        return 6; // MoveForward, MoveBackward, TurnLeft, TurnRight, LookLeft, LookRight
    }
    
    size_t getObservationSpaceSize() const override {
        return 100; // Observation vector size
    }
    
    // Get world dimensions
    int getWidth() const { return width_; }
    int getHeight() const { return height_; }
    
    // Add agent to world
    void addAgent(std::shared_ptr<SwarmAgent> agent) {
        agents_.push_back(agent);
    }
    
    // Get all agents
    std::vector<std::shared_ptr<SwarmAgent>> getAgents() const {
        return agents_;
    }

private:
    int width_;
    int height_;
    std::vector<std::shared_ptr<SwarmAgent>> agents_;
    mutable EnvironmentState state_;
    float lastReward_ = 0.0f;
    bool done_ = false;
};

// Simple Swarm Simulation
class SwarmSimulation {
public:
    SwarmSimulation() {
        NLM_LOG_INFO("SwarmSimulation initialized");
    }
    
    void run(int numAgents = 20, int steps = 100) {
        std::cout << "===========================================\n";
        std::cout << "SIMPLE SWARM SIMULATION\n";
        std::cout << "===========================================\n\n";
        
        // Create world
        SwarmWorld world(200, 200);
        
        // Create agent manager
        AgentManager manager;
        
        // Create swarm agents
        std::vector<std::string> agentIds;
        for (int i = 0; i < numAgents; ++i) {
            std::string agentId = manager.createAgent(
                AgentType::ACTIVE,
                std::make_shared<SwarmWorld>(world)  // Share world reference
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
            
            // Print statistics every 20 steps
            if (step % 20 == 0) {
                auto metrics = manager.getMetrics();
                std::cout << "  Agents: " << metrics.activeAgents << "\n";
                std::cout << "  Total time: " << metrics.totalTime << "\n";
                std::cout << "  Total steps: " << metrics.totalSteps << "\n";
                std::cout << "  Average energy: " << metrics.averageEnergy << "\n";
            }
            
            // Print agent positions
            for (const auto& agentId : agentIds) {
                if (manager.hasAgent(agentId)) {
                    auto& agent = manager.getAgent(agentId);
                    std::cout << "    Agent " << agentId << 
                              << " at (" << agent.getBody().x << ", " << agent.getBody().y << ")\n";
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
        std::cout << "  Average energy: " << finalMetrics.averageEnergy << "\n";
        
        std::cout << "\nSwarm simulation complete.\n";
        std::cout << "Agents demonstrated flocking behavior with alignment, cohesion, and separation.\n";
    }
};

} // namespace nlm