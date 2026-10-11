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
#include <random>

namespace nlm {

// Cooperative task types
enum class TaskType {
    RESOURCE_COLLECTION,
    TERRITORY_DEFENSE,
    HUNTING_COOPERATIVE,
    SHELTER_BUILDING
};

// Cooperative agent for joint problem solving
class CooperativeAgent : public AgentContainer {
public:
    CooperativeAgent(std::string id, std::shared_ptr<Environment> environment)
        : AgentContainer(std::move(id), nullptr, environment, AgentType::HYBRID) {
        // Initialize cooperative agent state
        body_.x = (std::rand() % 150) / 10.0f - 15.0f;
        body_.y = (std::rand() % 150) / 10.0f - 15.0f;
        body_.orientation = (std::rand() % 360) * 0.01745f;
        
        // Cooperative state
        currentTask_ = TaskType::RESOURCE_COLLECTION;
        taskProgress_ = 0.0f;
        groupMembers_.clear();
        taskObjectives_.clear();
        completedTasks_.clear();
        
        // Cooperation attributes
        cooperationScore_ = 0.0f;
        coordinationLevel_ = 0.0f;
        contributionRate_ = 0.0f;
    }
    
    void update(double timestep) override {
        double currentTime = std::chrono::duration<double>
            (std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        
        // Sense environment
        sense();
        
        // Update current task
        updateTask();
        
        // Coordinate with group members
        coordinateWithGroup();
        
        // Execute cooperative task
        executeTask();
        
        // Learn from cooperation
        learnFromCooperation();
        
        // Update position
        body_.x += body_.velocityX * timestep * 100.0f;
        body_.y += body_.velocityY * timestep * 100.0f;
        
        // Clamp to world bounds
        body_.x = std::max(-100.0f, std::min(100.0f, body_.x));
        body_.y = std::max(-100.0f, std::min(100.0f, body_.y));
        
        body_.isMoving = (body_.velocityX != 0.0f || body_.velocityY != 0.0f);
        
        // Record cooperative behavior
        recordBehavior("Cooperative_Task_" + std::to_string(static_cast<int>(currentTask_)), timestep,
                      "progress=" + std::to_string(taskProgress_) +
                      ",group_size=" + std::to_string(groupMembers_.size()) +
                      ",coop_score=" + std::to_string(cooperationScore_));
    }
    
private:
    void updateTask() {
        // Update task progress based on group coordination
        if (groupMembers_.size() > 1) {
            // With group, make faster progress
            taskProgress_ += coordinationLevel_ * 0.5f * timestep;
        } else {
            // Solo, slower progress
            taskProgress_ += 0.2f * timestep;
        }
        
        // Complete task when progress reaches 100%
        if (taskProgress_ >= 100.0f) {
            completeTask();
        }
    }
    
    void coordinateWithGroup() {
        // Coordinate with other agents in the same group
        if (!isInGroup("") && groupMembers_.size() > 1) {
            // Join or form a group
            formGroup("cooperative_group");
            
            // Communicate task objectives
            for (const auto& memberId : groupMembers_) {
                if (memberId != getId()) {
                    communicate("COORDINATE: Start " + taskToString(currentTask_), memberId);
                    observe(memberId);
                }
            }
        }
        
        // Update coordination level
        if (!groupMembers_.empty()) {
            coordinationLevel_ = calculateCoordinationLevel();
        }
    }
    
    void executeTask() {
        switch (currentTask_) {
            case TaskType::RESOURCE_COLLECTION:
                collectResources();
                break;
            case TaskType::TERRITORY_DEFENSE:
                defendTerritory();
                break;
            case TaskType::HUNTING_COOPERATIVE:
                cooperativeHunting();
                break;
            case TaskType::SHELTER_BUILDING:
                buildShelter();
                break;
        }
    }
    
    void collectResources() {
        // Collect resources with group coordination
        if (groupMembers_.size() > 1) {
            // Share resource collection effort
            float resourceFound = 0.0f;
            
            // Each group member contributes
            for (const auto& memberId : groupMembers_) {
                if (memberId != getId()) {
                    // Other members help find resources
                    contributionRate_ += 0.1f;
                    
                    // Share any found resources
                    shareMemory(memberId, "resource_location", "near_" + std::to_string(body_.x) + "," + std::to_string(body_.y));
                }
            }
            
            resourceFound = 5.0f * coordinationLevel_;  // More efficient with coordination
            body_.energy = std::min(1.0f, body_.energy + resourceFound * 0.01f);
            
            // Update task progress
            taskProgress_ += resourceFound * 2.0f;  // 2% per resource
            
            cooperationScore_ += resourceFound * coordinationLevel_;
        } else {
            // Solo collection
            if (std::rand() % 100 < 15) {  // 15% chance
                body_.energy = std::min(1.0f, body_.energy + 2.0f);
                taskProgress_ += 5.0f;  // 5% progress per resource
            }
        }
    }
    
    void defendTerritory() {
        // Defend territory with group coordination
        if (groupMembers_.size() > 1) {
            // Coordinated defense
            std::string threat = checkForThreats();
            if (!threat.empty()) {
                communicate("THREAT: " + threat + " detected", "");  // Broadcast
                
                // Group members coordinate response
                for (const auto& memberId : groupMembers_) {
                    if (memberId != getId()) {
                        communicate("DEFEND: " + threat + " with me", memberId);
                        observe(memberId);
                    }
                }
                
                // Successfully defend
                taskProgress_ += 10.0f * coordinationLevel_;
                cooperationScore_ += 5.0f * coordinationLevel_;
            }
        }
    }
    
    void cooperativeHunting() {
        // Cooperative hunting of prey
        if (groupMembers_.size() > 1) {
            // Coordinate hunt
            std::string prey = detectPrey();
            if (!prey.empty()) {
                // Coordinate approach
                communicate("HUNT: Target " + prey, "");  // Broadcast
                
                // Each member takes different approach
                float contribution = 0.0f;
                for (const auto& memberId : groupMembers_) {
                    if (memberId != getId()) {
                        contributeToHunt(memberId, prey);
                        contribution += 0.2f;
                    }
                }
                
                // Successful hunt
                taskProgress_ += 15.0f * contribution;
                cooperationScore_ += 10.0f * contribution;
                
                // Share spoils
                shareMemory("", "hunt_success", prey + "_captured");
            }
        }
    }
    
    void buildShelter() {
        // Build shelter with group coordination
        if (groupMembers_.size() > 1) {
            // Coordinated construction
            float materials = gatherMaterials();
            float constructionSpeed = coordinationLevel_ * 0.5f;
            
            if (materials > 10.0f) {
                // Make progress
                float progress = materials * constructionSpeed;
                taskProgress_ += progress;
                cooperationScore_ += progress * 0.5f;
                
                // Share construction knowledge
                for (const auto& memberId : groupMembers_) {
                    if (memberId != getId()) {
                        shareMemory(memberId, "construction_knowledge", "step_by_step");
                    }
                }
            }
        }
    }
    
    void learnFromCooperation() {
        // Learn from cooperative interactions
        auto observations = getObservations();
        
        int successfulCooperations = 0;
        float totalCoordination = 0.0f;
        
        for (const auto& obs : observations) {
            if (obs.observationType.find("cooperation") != std::string::npos) {
                successfulCooperations++;
                
                // Parse observation for coordination level
                float coord = std::stof(obs.observationType.substr(obs.observationType.find("coord_") + 6));
                totalCoordination += coord;
            }
        }
        
        if (successfulCooperations > 0) {
            float avgCoordination = totalCoordination / successfulCooperations;
            coordinationLevel_ = (coordinationLevel_ * 0.9f) + (avgCoordination * 0.1f);
        }
        
        // Update contribution rate based on task success
        if (taskProgress_ > 50.0f) {
            contributionRate_ = std::min(1.0f, contributionRate_ * 1.1f);
        }
    }
    
    float calculateCoordinationLevel() {
        if (groupMembers_.size() <= 1) return 0.0f;
        
        // Calculate coordination based on message exchanges
        float totalMessages = 0.0f;
        for (const auto& memberId : groupMembers_) {
            if (memberId != getId()) {
                auto messages = getMessages();
                totalMessages += messages.size();
            }
        }
        
        return std::min(1.0f, totalMessages / (groupMembers_.size() * 10.0f));
    }
    
    std::string taskToString(TaskType task) {
        switch (task) {
            case TaskType::RESOURCE_COLLECTION: return "collect_resources";
            case TaskType::TERRITORY_DEFENSE: return "defend_territory";
            case TaskType::HUNTING_COOPERATIVE: return "cooperative_hunt";
            case TaskType::SHELTER_BUILDING: return "build_shelter";
            default: return "unknown_task";
        }
    }
    
    void completeTask() {
        // Mark task as completed
        completedTasks_.push_back(currentTask_);
        
        // Reset for next task
        currentTask_ = TaskType::RESOURCE_COLLECTION;
        taskProgress_ = 0.0f;
        
        // Share success with group
        communicate("TASK_COMPLETE: " + taskToString(currentTask_), "");  // Broadcast
        
        // Learn from successful completion
        cooperationScore_ += 20.0f;
        
        // Update strategy based on task completion
        if (completedTasks_.size() >= 3) {
            // Have completed enough tasks, improve strategy
            coordinationLevel_ = std::min(1.0f, coordinationLevel_ * 1.2f);
        }
    }
    
    std::string checkForThreats() {
        auto observations = getObservations();
        for (const auto& obs : observations) {
            if (obs.observationType.find("threat") != std::string::npos) {
                return obs.observationType.substr(obs.observationType.find("threat_") + 7);
            }
        }
        return "";
    }
    
    std::string detectPrey() {
        auto observations = getObservations();
        for (const auto& obs : observations) {
            if (obs.observationType.find("prey") != std::string::npos) {
                return obs.observationType.substr(obs.observationType.find("prey_") + 5);
            }
        }
        return "";
    }
    
    void contributeToHunt(const std::string& memberId, const std::string& prey) {
        communicate("CONTRIBUTE: Helping hunt " + prey, memberId);
        observe(memberId);
        shareMemory(memberId, "hunt_contribution", getId() + "_helped");
    }
    
    float gatherMaterials() {
        // Gather building materials
        return 15.0f;  // Dummy value
    }
    
    // Agent attributes
    TaskType currentTask_;
    float taskProgress_;
    std::vector<std::string> groupMembers_;
    std::vector<TaskType> taskObjectives_;
    std::vector<TaskType> completedTasks_;
    
    float cooperationScore_;
    float coordinationLevel_;
    float contributionRate_;
};

// Cooperative Task World
class CooperativeTaskWorld : public Environment {
public:
    CooperativeTaskWorld(int width = 200, int height = 200) : width_(width), height_(height) {
        NLM_LOG_INFO("CooperativeTaskWorld created: " + std::to_string(width_) + "x" + std::to_string(height_));
        
        // Initialize with task resources
        resourceNodes_ = generateResourceNodes();
        territoryBounds_ = generateTerritoryBounds();
    }
    
    std::vector<std::pair<float, float>> generateResourceNodes() {
        std::vector<std::pair<float, float>> nodes;
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        
        for (int i = 0; i < 8; ++i) {
            float x = (std::rand() % 140) / 10.0f - 14.0f;
            float y = (std::rand() % 140) / 10.0f - 14.0f;
            nodes.push_back(std::make_pair(x, y));
        }
        
        return nodes;
    }
    
    std::vector<std::pair<float, float>> generateTerritoryBounds() {
        std::vector<std::pair<float, float>> bounds;
        
        // Create safe zone
        bounds.push_back(std::make_pair(-50.0f, -50.0f));  // bottom-left
        bounds.push_back(std::make_pair(50.0f, 50.0f));   // top-right
        
        return bounds;
    }
    
    bool isInTerritory(float x, float y) {
        for (size_t i = 0; i < territoryBounds_.size(); i += 2) {
            float x1 = territoryBounds_[i].first;
            float y1 = territoryBounds_[i].second;
            float x2 = territoryBounds_[i+1].first;
            float y2 = territoryBounds_[i+1].second;
            
            if (x >= x1 && x <= x2 && y >= y1 && y <= y2) {
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
        return "CooperativeTaskWorld";
    }
    
    size_t getActionSpaceSize() const override {
        return 6;
    }
    
    size_t getObservationSpaceSize() const override {
        return 200;
    }

private:
    int width_;
    int height_;
    std::vector<std::pair<float, float>> resourceNodes_;
    std::vector<std::pair<float, float>> territoryBounds_;
    mutable EnvironmentState state_;
    float lastReward_ = 0.0f;
    bool done_ = false;
};

// Simple Cooperative Task Simulation
class CooperativeTaskSimulation {
public:
    CooperativeTaskSimulation() {
        NLM_LOG_INFO("CooperativeTaskSimulation initialized");
    }
    
    void run(int numAgents = 15, int steps = 180) {
        std::cout << "===========================================\n";
        std::cout << "COOPERATIVE TASK SIMULATION\n";
        std::cout << "===========================================\n\n";
        
        // Create world
        CooperativeTaskWorld world(200, 200);
        
        // Create agent manager
        AgentManager manager;
        
        // Create cooperative agents
        std::vector<std::string> agentIds;
        for (int i = 0; i < numAgents; ++i) {
            std::string agentId = manager.createAgent(
                AgentType::HYBRID,
                std::make_shared<CooperativeTaskWorld>(world)
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
            
            // Print statistics every 60 steps
            if (step % 60 == 0) {
                auto metrics = manager.getMetrics();
                std::cout << "  Total agents: " << metrics.activeAgents << "\n";
                std::cout << "  Total steps: " << metrics.totalSteps << "\n";
                std::cout << "  Average energy: " << metrics.averageEnergy << "\n";
            }
            
            // Print agent information
            std::cout << "  Agent status:\n";
            int count = 0;
            for (const auto& agentId : agentIds) {
                if (manager.hasAgent(agentId) && count < 3) {
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
        
        std::cout << "\nCooperative task simulation complete.\n";
        std::cout << "Agents worked together on resource collection, territory defense, and cooperative hunting.\n";
    }
};

} // namespace nlm