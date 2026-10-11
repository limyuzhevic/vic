#pragma once

#include "AgentBrain.hpp"
#include "AgentBody.hpp"
#include "Environment.hpp"
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>

namespace nlm {

// Agent types for different behaviors
enum class AgentType {
    ACTIVE,     // Can move, sense, act, learn
    PASSIVE,    // Can sense, learn, but cannot act
    SOCIAL,     // Can interact with other agents
    HYBRID     // Combination of different behaviors
};

// Agent status states
enum class AgentStatus {
    ACTIVE,
    IDLE,
    LEARNING,
    SLEEPING,
    DEAD,
    PAUSED
};

// Agent interface for multi-agent compatibility
class AgentInterface {
public:
    virtual ~AgentInterface() = default;
    
    // Agent identification
    virtual std::string getId() const = 0;
    virtual AgentType getType() const = 0;
    virtual AgentStatus getStatus() const = 0;
    
    // Brain interaction
    virtual AgentBrain& getBrain() = 0;
    virtual const AgentBrain& getBrain() const = 0;
    
    // Physical properties
    virtual AgentBody& getBody() = 0;
    virtual const AgentBody& getBody() const = 0;
    
    // State management
    virtual void update(double timestep) = 0;
    virtual void reset() = 0;
    
    // Agent behaviors
    virtual void sense() = 0;
    virtual void think() = 0;
    virtual void act() = 0;
    virtual void learn() = 0;
    
    // Social interactions
    virtual void communicate(const std::string& message, const std::string& targetId) = 0;
    virtual void observe(const std::string& agentId) = 0;
    virtual void imitate(const std::string& agentId, const std::string& behavior) = 0;
    
    // State sharing
    virtual void shareNeuralPatterns(const std::string& targetId) = 0;
    virtual void shareMemory(const std::string& targetId) = 0;
    
    // Agent group operations
    virtual void formGroup(const std::string& groupId) = 0;
    virtual void leaveGroup(const std::string& groupId) = 0;
    virtual bool isInGroup(const std::string& groupId) const = 0;
    virtual std::vector<std::string> getGroupMembers() const = 0;
    
    // Leadership
    virtual bool isLeader() const = 0;
    virtual void setLeader(bool isLeader) = 0;
    virtual std::string getLeaderId() const = 0;
};

// AgentContainer manages a single agent's complete state
class AgentContainer : public AgentInterface {
public:
    AgentContainer(std::string id, std::shared_ptr<AgentBrain> brain,
                   std::shared_ptr<Environment> environment,
                   AgentType type = AgentType::ACTIVE);
    ~AgentContainer() override;
    
    // Agent identification
    std::string getId() const override { return id_; }
    AgentType getType() const override { return type_; }
    AgentStatus getStatus() const override { return status_; }
    
    // Brain interaction
    AgentBrain& getBrain() override { return *brain_; }
    const AgentBrain& getBrain() const override { return *brain_; }
    
    // Physical properties
    AgentBody& getBody() override { return body_; }
    const AgentBody& getBody() const override { return body_; }
    
    // State management
    void update(double timestep) override;
    void reset() override;
    
    // Agent behaviors
    void sense() override;
    void think() override;
    void act() override;
    void learn() override;
    
    // Social interactions
    void communicate(const std::string& message, const std::string& targetId) override;
    void observe(const std::string& agentId) override;
    void imitate(const std::string& agentId, const std::string& behavior) override;
    
    // State sharing
    void shareNeuralPatterns(const std::string& targetId) override;
    void shareMemory(const std::string& targetId) override;
    
    // Agent group operations
    void formGroup(const std::string& groupId) override;
    void leaveGroup(const std::string& groupId) override;
    bool isInGroup(const std::string& groupId) const override;
    std::vector<std::string> getGroupMembers() const override;
    
    // Leadership
    bool isLeader() const override { return isLeader_; }
    void setLeader(bool isLeader) override { isLeader_ = isLeader; }
    std::string getLeaderId() const override { return leaderId_; }
    
    // Environment interaction
    Environment& getEnvironment() { return *environment_; }
    const Environment& getEnvironment() const { return *environment_; }
    
    // Group management
    void addGroupMember(const std::string& agentId);
    void removeGroupMember(const std::string& agentId);
    
    // Communication
    void receiveMessage(const std::string& fromId, const std::string& message);
    std::vector<std::pair<std::string, std::string>> getMessages() const;
    void clearMessages();
    
    // Observation
    struct ObservationRecord {
        std::string observerId;
        std::string observedId;
        double timestamp;
        std::string observationType;
        std::string data;
    };
    
    std::vector<ObservationRecord> getObservations() const;
    void clearObservations();
    
    // Neural sharing
    void addSharedWeights(const std::string& sourceId, const std::vector<float>& weights);
    std::vector<float> getSharedWeights(const std::string& sourceId) const;
    
    // Memory sharing
    void addSharedMemory(const std::string& sourceId, const std::string& memoryKey, const std::string& memoryValue);
    std::string getSharedMemory(const std::string& sourceId, const std::string& memoryKey) const;
    
    // Conflict resolution
    void registerActionConflict(const std::string& conflictingAgentId, const std::string& action);
    std::vector<std::pair<std::string, std::string>> getActionConflicts() const;
    void resolveConflict(const std::string& conflictingAgentId, const std::string& resolution);
    
    // Emergent behavior tracking
    void recordBehavior(const std::string& behaviorType, double timestamp, const std::string& context);
    std::vector<std::tuple<std::string, double, std::string>> getRecordedBehaviors() const;

private:
    std::string id_;
    AgentType type_;
    AgentStatus status_;
    
    std::shared_ptr<AgentBrain> brain_;
    AgentBody body_;
    std::shared_ptr<Environment> environment_;
    
    // Social state
    std::string leaderId_;
    bool isLeader_ = false;
    std::unordered_map<std::string, std::vector<std::string>> groups_;
    
    // Communication
    std::vector<std::pair<std::string, std::string>> messages_;
    mutable std::mutex messageMutex_;
    
    // Observation
    std::vector<ObservationRecord> observations_;
    mutable std::mutex observationMutex_;
    
    // State sharing
    std::unordered_map<std::string, std::vector<float>> sharedNeuralWeights_;
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> sharedMemory_;
    
    // Conflict tracking
    std::vector<std::pair<std::string, std::string>> actionConflicts_;
    
    // Emergent behavior
    std::vector<std::tuple<std::string, double, std::string>> behaviorHistory_;
    
    double lastUpdateTime_;
    double lastSenseTime_;
    double lastThinkTime_;
    double lastActTime_;
    double lastLearnTime_;
};

} // namespace nlm