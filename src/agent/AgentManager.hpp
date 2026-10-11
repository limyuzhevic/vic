#pragma once

#include "AgentContainer.hpp"
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <chrono>
#include <functional>

namespace nlm {

// Time synchronization information
struct TimeSyncInfo {
    double globalTime;
    double timestep;
    std::unordered_map<std::string, double> agentTimes;
    std::chrono::high_resolution_clock::time_point syncPoint;
};

// Conflict resolution result
struct ConflictResolution {
    std::string conflictId;
    std::string resolvedBy;
    std::string resolution;
    double resolutionTime;
    bool success;
};

// AgentManager coordinates and manages multiple agents
class AgentManager {
public:
    AgentManager();
    ~AgentManager();
    
    // Agent lifecycle management
    std::string createAgent(AgentType type, std::shared_ptr<Environment> environment,
                           const std::string& agentId = "");
    bool removeAgent(const std::string& agentId);
    bool hasAgent(const std::string& agentId) const;
    
    // Agent access
    AgentContainer& getAgent(const std::string& agentId);
    const AgentContainer& getAgent(const std::string& agentId) const;
    std::vector<std::string> getAllAgentIds() const;
    
    // Agent configuration
    void setAgentType(const std::string& agentId, AgentType type);
    void setAgentBrain(const std::string& agentId, std::shared_ptr<AgentBrain> brain);
    void setAgentEnvironment(const std::string& agentId, std::shared_ptr<Environment> environment);
    
    // Simulation control
    void startSimulation(double timestep = 0.01);
    void stopSimulation();
    bool isRunning() const { return isRunning_; }
    
    // Main simulation loop
    void runStep();
    void runUntil(double time);
    double getCurrentTime() const { return currentTime_; }
    
    // Agent behavior coordination
    void coordinateAgents();
    void detectConflicts();
    std::vector<ConflictResolution> resolveConflicts();
    
    // Resource management
    void allocateResources();
    void shareResources(const std::string& sourceAgentId, const std::string& targetAgentId,
                       double amount, const std::string& resourceType);
    
    // Time synchronization
    TimeSyncInfo syncAgents();
    void adjustAgentTiming(const std::string& agentId, double adjustment);
    
    // Group formation and leadership
    void formGroup(const std::string& groupId,
                   const std::vector<std::string>& agentIds,
                   const std::string& leaderId = "");
    void dissolveGroup(const std::string& groupId);
    bool isGroupActive(const std::string& groupId) const;
    std::vector<std::string> getGroupMembers(const std::string& groupId) const;
    
    // Global state management
    void saveCheckpoint(const std::string& checkpointPath);
    bool loadCheckpoint(const std::string& checkpointPath);
    void createReplay(const std::string& replayPath);
    
    // Metrics and monitoring
    struct SimulationMetrics {
        double totalTime;
        int totalSteps;
        int activeAgents;
        int learningAgents;
        float averageEnergy;
        float averageFiringRate;
        std::unordered_map<std::string, int> agentActions;
        std::vector<std::string> emergentBehaviors;
    };
    
    SimulationMetrics getMetrics() const;
    
    // Event callbacks
    using AgentEventCallback = std::function<void(const std::string& agentId, const std::string& eventType, const std::string& data)>;
    void addAgentEventCallback(const AgentEventCallback& callback);
    
    // Simulation statistics
    struct PerformanceStats {
        double simulationSpeed;  // steps per second
        double averageStepTime;
        std::unordered_map<std::string, double> agentPerformance;
    };
    
    PerformanceStats getPerformanceStats() const;

private:
    // Agent registry
    std::unordered_map<std::string, std::unique_ptr<AgentContainer>> agents_;
    mutable std::mutex agentsMutex_;
    
    // Simulation state
    bool isRunning_;
    double currentTime_;
    double timestep_;
    std::chrono::high_resolution_clock::time_point lastStepTime_;
    
    // Time synchronization
    std::atomic<bool> timeSyncScheduled_;
    std::condition_variable timeSyncCV_;
    std::mutex timeSyncMutex_;
    
    // Group management
    std::unordered_map<std::string, std::vector<std::string>> groups_;
    mutable std::mutex groupsMutex_;
    
    // Conflict management
    std::queue<std::pair<std::string, std::string>> pendingConflicts_;
    mutable std::mutex conflictsMutex_;
    
    // Resource management
    std::unordered_map<std::string, double> globalResources_;
    mutable std::mutex resourcesMutex_;
    
    // Event system
    std::vector<AgentEventCallback> eventCallbacks_;
    mutable std::mutex callbacksMutex_;
    
    // Performance tracking
    mutable std::atomic<int> totalSteps_;
    mutable std::chrono::high_resolution_clock::time_point simulationStartTime_;
    
    // Checkpointing
    bool enableCheckpointing_;
    std::string checkpointPath_;
    
    // Replay system
    std::vector<std::string> replayBuffer_;
    bool enableReplay_;
    
    // Internal methods
    void initializeAgent(std::unique_ptr<AgentContainer> agent);
    void updateAgentTiming(const std::string& agentId);
    void updateGroupLeadership(const std::string& groupId);
    void broadcastEvent(const std::string& agentId, const std::string& eventType,
                       const std::string& data);
    std::string generateAgentId() const;
    bool validateAgentConfig(const AgentType& type,
                            const std::shared_ptr<Environment>& environment) const;
};

} // namespace nlm