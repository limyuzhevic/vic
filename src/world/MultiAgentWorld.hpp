// Multi-agent world with agent interactions and cooperation
#pragma once

#include "SimpleWorld.hpp"
#include <memory>
#include <unordered_map>
#include <string>
#include <vector>
#include <queue>
#include <mutex>
#include <chrono>

namespace nlm {

// Agent role types
enum class AgentRole {
    Forager,
    Hunter,
    Gatherer,
    Builder,
    Defender,
    Explorer,
    Trader,
    Scientist,
    Leader,
    Follower
};

// Agent personality traits
struct AgentPersonality {
    std::string agentId;
    float exploration;      // 0.0-1.0: tendency to explore new areas
    float social;           // 0.0-1.0: social tendency
    float risk;             // 0.0-1.0: risk tolerance
    float curiosity;        // 0.0-1.0: curiosity level
    float cooperativeness;   // 0.0-1.0: tendency to cooperate
    float competitiveness;  // 0.0-1.0: competitiveness level
    
    AgentPersonality()
        : exploration(0.5f), social(0.5f), risk(0.5f), curiosity(0.5f),
          cooperativeness(0.5f), competitiveness(0.5f) {}
};

// Agent decision making state
struct AgentDecisionState {
    std::string agentId;
    std::vector<float> perception;      // Current sensory perception
    std::vector<float> memory;          // Short-term memory
    float energy;                        // Current energy level
    float hunger;                        // Hunger level (0.0-1.0)
    float fatigue;                       // Fatigue level (0.0-1.0)
    float stress;                        // Stress level (0.0-1.0)
    float motivation;                    // Motivation level (0.0-1.0)
    std::string currentGoal;             // Current goal or task
    float goalProgress;                  // Progress toward current goal (0.0-1.0)
    std::vector<std::string> learnedActions;  // Actions learned from experience
    std::vector<std::string> preferences;   // Resource/object preferences
    std::unordered_map<std::string, float> beliefs;  // Current beliefs about the world
    double lastActionTime;               // Time since last action
};

// Agent action plan
struct ActionPlan {
    std::string agentId;
    std::vector<std::string> plannedActions;  // Sequence of planned actions
    std::vector<float> expectedOutcomes;     // Expected outcomes for each action
    float planEfficiency;                    // Expected efficiency of the plan
    float planRisk;                          // Expected risk of the plan
    double planDuration;                     // Total planned duration
    std::string planContext;                // Context/rationale for the plan
    std::vector<std::string> preconditions; // Preconditions needed to execute
    std::vector<std::string> postconditions; // Postconditions expected after execution
    
    ActionPlan() : planEfficiency(1.0f), planRisk(0.5f), planDuration(0.0), 
                   planEfficiency(1.0f), planRisk(0.5f) {}
};

// Agent task and responsibility
struct AgentTask {
    std::string taskId;
    std::string agentId;
    std::string taskType;                 // Type of task (foraging, hunting, etc.)
    std::string taskDescription;          // Description of the task
    std::unordered_map<std::string, float> requirements;  // Resource/requirements
    std::unordered_map<std::string, float> expectedOutputs; // Expected outputs
    float estimatedDuration;              // Estimated time to complete
    float currentProgress;                // Current progress (0.0-1.0)
    std::string status;                   // Status (pending, in_progress, completed, failed)
    double startTime;                     // When task started
    std::string assignedBy;              // Who assigned the task
    std::vector<std::string> collaborators;  // Other agents involved
    
    AgentTask() : estimatedDuration(1.0f), currentProgress(0.0f), 
                  status("pending"), startTime(0.0) {}
};

// Multi-agent communication
struct CommunicationMessage {
    std::string senderId;
    std::string receiverId;
    std::string messageType;             // Type of message (proposal, request, info, etc.)
    std::string content;                // Message content
    float urgency;                      // Urgency level (0.0-1.0)
    float importance;                    // Importance level (0.0-1.0)
    double timestamp;                   // When message was sent
    bool requiresResponse;             // Does receiver need to respond?
    std::string responseDeadline;      // Deadline for response
    std::unordered_map<std::string, float> metadata;  // Additional metadata
    
    CommunicationMessage() : urgency(0.5f), importance(0.5f), 
                             timestamp(0.0), requiresResponse(false) {}
};

// Cooperation network between agents
struct CooperationNetwork {
    std::string networkId;
    std::unordered_map<std::string, std::vector<std::string>> connections;  // agentId -> list of connected agents
    std::unordered_map<std::string, float> trustLevels;  // agentId -> trust level (0.0-1.0)
    std::unordered_map<std::string, float> obligationLevels;  // agentId -> obligation level (0.0-1.0)
    std::unordered_map<std::string, std::vector<std::string>> sharedGoals;  // agentId -> list of shared goals
    std::unordered_map<std::string, std::vector<std::string>> pastCollaborations;  // agentId -> list of past collaborators
    float networkStrength;              // Overall network strength (0.0-1.0)
    float centrality;                   // Network centrality measure
    
    CooperationNetwork() : networkStrength(0.5f), centrality(0.5f) {}
};

// Multi-agent world with complex agent interactions
class MultiAgentWorld : public SimpleWorld {
public:
    MultiAgentWorld();
    ~MultiAgentWorld();
    
    // Agent management
    void addAgent(uint64_t agentId, float x, float y, float radius = 0.3f,
                 const AgentPersonality& personality = AgentPersonality());
    void removeAgent(uint64_t agentId);
    AgentBody* getAgentBody(uint64_t agentId);
    const AgentBody* getAgentBody(uint64_t agentId) const;
    AgentDecisionState* getAgentState(uint64_t agentId);
    const AgentDecisionState* getAgentState(uint64_t agentId) const;
    
    // Agent state management
    void updateAgentState(uint64_t agentId, double timestep);
    void updateAgentEnergy(uint64_t agentId, float energyChange);
    void updateAgentHunger(uint64_t agentId, float hungerChange);
    void updateAgentFatigue(uint64_t agentId, float fatigueChange);
    void updateAgentStress(uint64_t agentId, float stressChange);
    void updateAgentMotivation(uint64_t agentId, float motivationChange);
    
    // Agent behavior and decision making
    void updateAgentBehavior(uint64_t agentId, double timestep);
    ActionPlan generateActionPlan(uint64_t agentId);
    void executeActionPlan(uint64_t agentId, const ActionPlan& plan);
    void learnFromExperience(uint64_t agentId, const std::vector<float>& outcomes);
    
    // Task management
    void assignTask(const AgentTask& task);
    void completeTask(const std::string& taskId);
    void failTask(const std::string& taskId);
    AgentTask* getTask(const std::string& taskId);
    std::vector<AgentTask> getAgentTasks(uint64_t agentId) const;
    
    // Communication system
    void sendMessage(const CommunicationMessage& message);
    CommunicationMessage receiveMessage(uint64_t agentId);
    std::vector<CommunicationMessage> getMessages(uint64_t agentId) const;
    void processIncomingMessages(uint64_t agentId);
    
    // Cooperation and collaboration
    void establishConnection(uint64_t agentId1, uint64_t agentId2);
    void removeConnection(uint64_t agentId1, uint64_t agentId2);
    bool areConnected(uint64_t agentId1, uint64_t agentId2) const;
    void updateTrustLevel(uint64_t agentId1, uint64_t agentId2, float trustChange);
    void updateObligationLevel(uint64_t agentId1, uint64_t agentId2, float obligationChange);
    void shareGoal(uint64_t agentId, const std::string& goalId);
    void removeSharedGoal(uint64_t agentId, const std::string& goalId);
    
    // Social learning and culture
    void transmitKnowledge(uint64_t senderId, uint64_t receiverId,
                          const std::vector<float>& knowledge);
    void teachAction(uint64_t teacherId, uint64_t studentId,
                    const std::string& actionName, const std::vector<float>& parameters);
    void adoptCulture(uint64_t agentId, const std::string& culturalTrait,
                    float strength = 1.0f);
    void transmitCulture(const std::string& culturalTrait, float strength = 1.0f);
    float getCulturalValue(uint64_t agentId, const std::string& culturalTrait) const;
    
    // Negotiation and conflict resolution
    void initiateNegotiation(uint64_t agentId1, uint64_t agentId2,
                           const std::string& negotiationType);
    void proposeTerms(uint64_t agentId, const std::string& negotiationId,
                     const std::unordered_map<std::string, float>& terms);
    void acceptTerms(uint64_t agentId, const std::string& negotiationId);
    void rejectTerms(uint64_t agentId, const std::string& negotiationId);
    void resolveConflict(uint64_t agentId1, uint64_t agentId2,
                       const std::string& conflictType);
    
    // Resource sharing and cooperation
    void shareResources(uint64_t agentId1, uint64_t agentId2,
                       const std::unordered_map<std::string, float>& resources);
    void requestResources(uint64_t requesterId, uint64_t providerId,
                         const std::unordered_map<std::string, float>& requestedResources);
    void allocateSharedResources(uint64_t agentId,
                               const std::unordered_map<std::string, float>& resources);
    
    // Statistics and analysis
    size_t getAgentCount() const { return agents_.size(); }
    float getAverageCooperationLevel() const;
    float getAverageTrustLevel() const;
    float getAverageTaskCompletionRate() const;
    std::string getCooperationNetworkGraph() const;
    std::string getCommunicationTrafficReport() const;
    std::string getCulturalTransmissionReport() const;
    
    // Simulation and evolution
    void updateAgentPopulationDynamics(double timestep);
    void evolveAgentBehaviors(double timestep);
    void culturalEvolution(double timestep);
    void ecologicalAgentCoupling(double timestep);
    
    // Visualization and analysis
    std::string getMultiAgentWorldStateSnapshot() const;
    void visualizeCooperationNetwork() const;
    void visualizeCommunicationFlow() const;
    void visualizeCulturalTransmission() const;
    
private:
    // Agent management
    struct AgentInfo {
        uint64_t agentId;
        AgentPersonality personality;
        AgentDecisionState decisionState;
        std::vector<AgentTask> tasks;
        std::queue<CommunicationMessage> messageQueue;
        std::vector<float> learnedKnowledge;
        std::vector<std::string> learnedActions;
        std::unordered_map<std::string, float> culturalBeliefs;
        std::vector<std::string> socialConnections;
        
        AgentInfo() : agentId(0), personality(), decisionState(), 
                     lastInteractionTime(0.0), socialEnergy(10.0f), 
                     socialNeed(0.5f), stressLevel(0.0f) {}
        
        double lastInteractionTime;
        float socialEnergy;
        float socialNeed;
        float stressLevel;
    };
    
    // Communication management
    struct NegotiationInfo {
        std::string negotiationId;
        std::string agent1Id;
        std::string agent2Id;
        std::string negotiationType;
        std::unordered_map<std::string, float> proposedTerms;
        std::unordered_map<std::string, float> acceptedTerms;
        std::unordered_map<std::string, float> rejectedTerms;
        std::string status;  // active, accepted, rejected, completed
        double startTime;
        double deadline;
        
        NegotiationInfo() : startTime(0.0), deadline(0.0), status("active") {}
    };
    
    // Core data
    std::unordered_map<uint64_t, AgentInfo> agents_;
    std::unordered_map<std::string, AgentTask> tasks_;
    std::unordered_map<std::string, CommunicationMessage> communications_;
    std::unordered_map<std::string, NegotiationInfo> negotiations_;
    
    // Cooperation network
    CooperationNetwork cooperationNetwork_;
    
    // Cultural system
    std::unordered_map<std::string, float> culturalValues_;
    std::unordered_map<std::string, std::vector<float>> culturalTrends_;
    
    // Task and resource management
    std::unordered_map<std::string, float> sharedResources_;
    std::unordered_map<std::string, std::vector<uint64_t>> resourceOwners_;
    
    // Statistics
    double lastStatsUpdate_;
    float avgCooperationLevel_;
    float avgTrustLevel_;
    float avgTaskCompletionRate_;
    
    // Helper functions
    void updateAgentSocialState(AgentInfo& agent, double timestep);
    void processAgentQueue(AgentInfo& agent, double timestep);
    void applySocialInfluence(AgentInfo& agent, const std::vector<uint64_t>& neighbors);
    void updateAgentGoals(AgentInfo& agent, double timestep);
    void handleAgentConflicts(const AgentInfo& agent1, const AgentInfo& agent2);
    
    void generateActionPlansForAgents(double timestep);
    void executeAgentActionPlans(double timestep);
    void facilitateAgentLearning(double timestep);
    void manageAgentTasks(double timestep);
    void processAgentCommunications(double timestep);
    void updateCooperationNetwork(double timestep);
    
    void updateCulturalEvolution(double timestep);
    void transmitCulturalKnowledge(double timestep);
    void adoptCulturalTraits(double timestep);
    
    std::string generateCooperationNetworkGraph() const;
    std::string generateCommunicationReport() const;
    std::string generateTaskReport() const;
    std::string generateCulturalReport() const;
    
    void updateWorldStatistics();
    void analyzeCooperationPatterns();
    void analyzeCommunicationPatterns();
    void analyzeCulturalTransmission();
    
    // Event processing
    void processResourceEvents(double timestep);
    void processSocialEvents(double timestep);
    void processCulturalEvents(double timestep);
    void processConflictEvents(double timestep);
    
    // Utility functions
    static float calculateSocialInfluence(const AgentInfo& agent, 
                                         const std::vector<uint64_t>& neighbors);
    static float calculateCooperationTendency(const AgentInfo& agent);
    static float calculateTrustBetweenAgents(const AgentInfo& agent1, 
                                            const AgentInfo& agent2);
    static bool isConflictPossible(const AgentInfo& agent1, const AgentInfo& agent2);
    static std::string generateEventDescription(const std::string& eventType,
                                               const std::string& agentId);
    
    // Event recording
    struct MultiAgentEvent {
        double timestamp;
        std::string eventType;
        std::string agentId;
        std::unordered_map<std::string, float> data;
        std::string description;
    };
    
    std::vector<MultiAgentEvent> multiAgentEventLog_;
    size_t maxEventLogSize_;
};

} // namespace nlm
