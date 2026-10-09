#pragma once

#include "Environment.hpp"
#include <string>
#include <unordered_map>
#include <memory>

namespace nlm {

// Environment factory for creating different environment types
class EnvironmentFactory {
public:
    static std::unique_ptr<Environment> createEnvironment(const std::string& name, 
                                                          const std::unordered_map<std::string, std::string>& params = {});
    
    static bool registerEnvironment(const std::string& name, 
                                   std::function<std::unique_ptr<Environment>(const std::unordered_map<std::string, std::string>&)> creator);
    
    static std::vector<std::string> getAvailableEnvironments();
    
private:
    using EnvironmentCreator = std::function<std::unique_ptr<Environment>(const std::unordered_map<std::string, std::string>&)>;
    static std::unordered_map<std::string, EnvironmentCreator>& getRegistry();
};

// Simple maze environment - a more challenging alternative to GridWorld
class MazeEnvironment : public Environment {
public:
    MazeEnvironment(int width = 20, int height = 20, float obstacleDensity = 0.2f);
    ~MazeEnvironment() override;
    
    std::unique_ptr<SensoryInput> reset() override;
    std::unique_ptr<SensoryInput> step(const Action& action) override;
    const EnvironmentState& getState() const override;
    float getLastReward() const override;
    bool isDone() const override;
    const char* getName() const override;
    size_t getActionSpaceSize() const override;
    size_t getObservationSpaceSize() const override;
    
    // Maze-specific methods
    void setStartPosition(int x, int y);
    void setGoalPosition(int x, int y);
    void addObstacle(int x, int y);
    bool isObstacle(int x, int y) const;
    bool isInBounds(int x, int y) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Maze with goals and rewards - advanced environment
class GoalMazeEnvironment : public Environment {
public:
    GoalMazeEnvironment(int width = 15, int height = 15, int numGoals = 5);
    ~GoalMazeEnvironment() override;
    
    std::unique_ptr<SensoryInput> reset() override;
    std::unique_ptr<SensoryInput> step(const Action& action) override;
    const EnvironmentState& getState() const override;
    float getLastReward() const override;
    bool isDone() const override;
    const char* getName() const override;
    size_t getActionSpaceSize() const override;
    size_t getObservationSpaceSize() const override;
    
    // Get goal information
    int getCurrentGoalId() const;
    float getProgressToGoal() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Factory registration functions
void registerGridWorldEnvironment();
void registerMazeEnvironment();
void registerGoalMazeEnvironment();
void registerAllEnvironments();

} // namespace nlm
