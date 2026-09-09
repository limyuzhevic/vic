#pragma once

#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../agent/AgentBody.hpp"
#include <memory>
#include <vector>
#include <string>
#include <chrono>
#include <algorithm>
#include <random>

namespace nlm {

// Advanced command types for agent behavior
enum class AdvancedCommand {
    NoOp = 0,
    MoveToTarget,
    FollowPath,
    ExploreArea,
    AvoidDanger,
    SeekResource,
    SocialInteraction,
    LearnFromDemonstration,
    PlanAction,
    ExecuteSequence,
    AdaptiveResponse
};

// Pathfinding node for navigation
struct NavigationNode {
    float x, y;
    float cost;
    NavigationNode* parent;
    
    NavigationNode(float xPos, float yPos) : x(xPos), y(yPos), cost(0.0f), parent(nullptr) {}
    
    float distanceTo(const NavigationNode& other) const {
        return std::sqrt(std::pow(x - other.x, 2) + std::pow(y - other.y, 2));
    }
};

// Learning record for demonstration learning
struct LearningRecord {
    std::vector<float> state;
    AdvancedCommand action;
    float reward;
    std::chrono::steady_clock::time_point timestamp;
    std::string context;
    
    LearningRecord(const std::vector<float>& s, AdvancedCommand a, float r, const std::string& c)
        : state(s), action(a), reward(r), context(c) {
        timestamp = std::chrono::steady_clock::now();
    }
};

// AgentBrainAdvanced: Enhanced agent with advanced capabilities
class AgentBrainAdvanced : public AgentBrain {
public:
    explicit AgentBrainAdvanced(std::shared_ptr<Brain> brain);
    ~AgentBrainAdvanced();
    
    // Advanced initialization
    void initialize(const SimpleWorld& world, const AgentBody& body);
    
    // Core advanced functionality
    void update(double timestep);
    AdvancedCommand selectAdvancedAction();
    void learnFromExperience(const std::vector<float>& state, AdvancedCommand action, float reward);
    
    // Navigation
    void setTargetPosition(float x, float y);
    void addWaypoint(float x, float y);
    void clearPath();
    float getDistanceToTarget() const;
    
    // Advanced learning
    void enableDemonstrationLearning(bool enable);
    void enablePathfinding(bool enable);
    void enableSocialLearning(bool enable);
    
    // Command management
    void addCustomCommand(const std::string& name, AdvancedCommand command);
    void removeCommand(AdvancedCommand command);
    std::vector<std::string> getAvailableCommands() const;
    
    // Statistics and monitoring
    float getCuriosityLevelAdvanced() const;
    float getExplorationRate() const;
    size_t getExperienceMemorySize() const;
    
    // State management
    void setExplorationBias(float bias) noexcept;
    void setRiskAversion(float aversion) noexcept;
    void setSocialInfluence(float influence) noexcept;
    
    // Memory management
    void compressMemory();
    void pruneOldExperiences(size_t maxKeep);
    void exportExperiences(const std::string& filename) const;
    void importExperiences(const std::string& filename);
    
    // Advanced decision making
    void setDecisionConfidenceThreshold(float threshold) noexcept;
    void setActionSelectionMethod(int method) noexcept;  // 0: greedy, 1: epsilon-greedy, 2: softmax
    
    // Reinforcement learning
    void updatePolicyGradient(const std::vector<float>& state, AdvancedCommand action, float reward);
    void updateQLearning(const std::vector<float>& state, AdvancedCommand action, float reward, const std::vector<float>& nextState);
    
    // Getters for advanced systems
    [[nodiscard]] std::vector<NavigationNode> getCurrentPath() const { return currentPath_; }
    [[nodiscard]] std::vector<LearningRecord> getRecentExperiences() const { return recentExperiences_; }
    [[nodiscard]] bool isDemonstrationLearningEnabled() const { return demonstrationLearningEnabled_; }
    [[nodiscard]] bool isPathfindingEnabled() const { return pathfindingEnabled_; }
    [[nodiscard]] bool isSocialLearningEnabled() const { return socialLearningEnabled_; }
    
    // Override base class methods for advanced behavior
    void processSensoryInput(const SensoryPercept& percept) override;
    MotorCommand decodeMotorCommand() override;
    
private:
    // Core advanced systems
    class AdvancedDecisionMaker {
    public:
        AdvancedDecisionMaker();
        AdvancedCommand selectAction(const std::vector<float>& state, 
                                    const std::vector<AdvancedCommand>& availableCommands,
                                    float explorationRate);
        void updatePolicy(const std::vector<float>& state, AdvancedCommand action, float reward);
        
    private:
        std::vector<std::vector<float>> stateSpace_;
        std::vector<AdvancedCommand> actionSpace_;
        std::vector<std::vector<float>> QValues_;
        float learningRate_;
        float discountFactor_;
        float explorationRate_;
    };
    
    class Pathfinder {
    public:
        std::vector<NavigationNode> findPath(const NavigationNode& start, const NavigationNode& goal);
        float heuristic(const NavigationNode& a, const NavigationNode& b) const;
        
    private:
        struct CompareNode {
            bool operator()(const NavigationNode* a, const NavigationNode* b) {
                return a->cost > b->cost;
            }
        };
    };
    
    class MemorySystem {
    public:
        void storeExperience(const LearningRecord& experience);
        std::vector<LearningRecord> getRelevantExperiences(const std::string& context) const;
        void clearOldExperiences();
        
    private:
        std::vector<LearningRecord> experiences_;
        size_t maxMemorySize_;
    };
    
    // Advanced agent state
    struct AdvancedAgentState {
        float x, y;               // Position
        float orientation;         // Direction
        float energy;             // Energy level
        float curiosity;          // Intrinsic motivation
        float stress;             // Stress level
        float socialBond;         // Social connection strength
        double lastActionTime;    // Time since last action
        std::string currentGoal;  // Current goal description
        std::vector<float> emotionalState; // Emotional vector
        
        AdvancedAgentState() : x(0.0f), y(0.0f), orientation(0.0f), 
                              energy(1.0f), curiosity(0.5f), stress(0.0f),
                              socialBond(0.0f), lastActionTime(0.0), currentGoal("") {}
    };
    
    // Implementation
    void updateAdvancedDecisionMaking(double timestep);
    void updateNavigation(double timestep);
    void updateLearningSystems(double timestep);
    void updateSocialSystems(double timestep);
    void updateEmotionalState(double timestep);
    
    AdvancedCommand executePathfinding();
    AdvancedCommand executeExploration();
    AdvancedCommand executeSocialBehavior();
    AdvancedCommand executeAdaptiveBehavior();
    
    // Helper functions
    std::vector<float> extractFeatures(const SensoryPercept& percept, const AgentBody& body) const;
    float calculateActionValue(const std::vector<float>& state, AdvancedCommand action) const;
    void updateEmotionalResponse(float reward, float predictionError);
    
    // Advanced systems
    AdvancedDecisionMaker decisionMaker_;
    Pathfinder pathfinder_;
    MemorySystem memorySystem_;
    
    // Advanced state
    AdvancedAgentState advancedState_;
    NavigationNode targetPosition_;
    std::vector<NavigationNode> waypoints_;
    std::vector<NavigationNode> currentPath_;
    
    // Learning and adaptation
    std::vector<LearningRecord> recentExperiences_;
    std::vector<LearningRecord> demonstrationExperiences_;
    
    // Configuration
    bool demonstrationLearningEnabled_;
    bool pathfindingEnabled_;
    bool socialLearningEnabled_;
    float explorationBias_;
    float riskAversion_;
    float socialInfluence_;
    float decisionConfidenceThreshold_;
    int actionSelectionMethod_;
    
    // Statistics
    size_t totalAdvancedActions_;
    float averageActionValue_;
    double lastLearningUpdate_;
};

} // namespace nlm
