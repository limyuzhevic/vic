#pragma once

#include "AgentBrain.hpp"
#include "../brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include <memory>
#include <vector>
#include <unordered_map>
#include <random>

namespace nlm {

// Advanced Agent Behaviors
// 
// Extends basic AgentBrain with sophisticated cognitive and behavioral capabilities
// including goal-directed behavior, social learning, meta-cognition, and adaptive strategies.
// 
// Biological inspiration:
// - Hierarchical control and goal systems (Botvinick et al., 2009)
// - Meta-learning and metacognition (Nelson & Dumka, 2013)
// - Social learning and cultural transmission (Laland & Janik, 2010)
// - Intrinsic motivation and curiosity (Barto, 2013)
// - Working memory and planning (Baddeley, 2012)

class AdvancedAgent {
public:
    AdvancedAgent(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world);
    ~AdvancedAgent();
    
    // Disable copying, enable moving
    AdvancedAgent(const AdvancedAgent&) = delete;
    AdvancedAgent& operator=(const AdvancedAgent&) = delete;
    AdvancedAgent(AdvancedAgent&&) noexcept;
    AdvancedAgent& operator=(AdvancedAgent&&) noexcept;
    
    // Initialize with configuration
    void initialize(const AdvancedAgentConfig& config);
    
    // Main agent loop
    AgentStepResult step(double timestep);
    
    // Goal management
    void setGoal(const Goal& goal);
    void addGoal(const Goal& goal);
    void removeGoal(int goalId);
    std::vector<Goal> getActiveGoals() const;
    
    // Planning and decision making
    void generatePlans();
    std::vector<Plan> getCurrentPlans() const;
    Plan selectBestPlan() const;
    
    // Working memory integration
    void updateWorkingMemory(const AgentStepResult& result);
    std::shared_ptr<NeuralWorkingMemory> getWorkingMemory() const;
    
    // Episodic memory management
    void storeExperience(const AgentStepResult& result);
    std::vector<EpisodicMemory> getRelevantMemories(const std::string& context) const;
    
    // Social learning
    void observeOtherAgent(const AdvancedAgent& otherAgent);
    void learnFromObservation(const SocialObservation& observation);
    
    // Meta-cognitive monitoring
    void updateMetaCognitiveState();
    float getConfidence() const;
    float getUncertainty() const;
    bool shouldExplore() const;
    bool shouldExploit() const;
    
    // Adaptation and learning
    void adaptStrategy(float reward, float predictionError, double timestep);
    void updateBehavioralStyle(const BehavioralProfile& profile);
    
    // Curiosity and exploration
    void updateCuriosityDrivenExploration();
    float calculateIntrinsicMotivation() const;
    MotorCommand selectExplorationAction() const;
    
    // Performance evaluation
    float evaluatePerformance(const AgentStepResult& result) const;
    void updatePerformanceMetrics(const AgentStepResult& result);
    PerformanceStats getPerformanceStats() const;
    
    // Configuration
    void enableMetaCognitiveMonitoring(bool enable) { metaCognitiveMonitoringEnabled_ = enable; }
    void enableSocialLearning(bool enable) { socialLearningEnabled_ = enable; }
    void enableGoalBasedBehavior(bool enable) { goalBasedBehaviorEnabled_ = enable; }
    void enableWorkingMemory(bool enable) { workingMemoryEnabled_ = enable; }
    
    // State accessors
    std::shared_ptr<Brain> getBrain() const { return brain_; }
    std::shared_ptr<SimpleWorld> getWorld() const { return world_; }
    AgentState getAgentState() const;
    
    // Reset and cleanup
    void reset();
    void resetForNewEpisode();
    
    // Serialization
    std::string serializeState() const;
    bool deserializeState(const std::string& state);
    
    // Get name
    const char* getName() const;
    
private:
    // Core components
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimpleWorld> world_;
    
    // Advanced cognitive systems
    std::shared_ptr<NeuralPlanner> planner_;
    std::shared_ptr<ConceptFormation> conceptFormation_;
    std::shared_ptr<Curiosity> curiositySystem_;
    std::shared_ptr<PredictionError> predictionError_;
    
    // Working and episodic memory
    std::shared_ptr<NeuralWorkingMemory> workingMemory_;
    std::shared_ptr<NeuralEpisodicMemory> episodicMemory_;
    
    // Goal and planning systems
    std::vector<Goal> goals_;
    std::vector<Plan> plans_;
    GoalSelectionStrategy goalSelectionStrategy_;
    
    // Social learning
    std::vector<AdvancedAgent*> observedAgents_;
    std::vector<SocialObservation> socialObservations_;
    SocialLearningMechanism socialLearningMechanism_;
    
    // Meta-cognitive state
    MetaCognitiveState metaCognitiveState_;
    bool metaCognitiveMonitoringEnabled_;
    bool socialLearningEnabled_;
    bool goalBasedBehaviorEnabled_;
    bool workingMemoryEnabled_;
    
    // Behavioral adaptation
    BehavioralProfile behavioralProfile_;
    ExplorationStrategy explorationStrategy_;
    ExploitationStrategy exploitationStrategy_;
    
    // Performance monitoring
    PerformanceStats performanceStats_;
    LearningProgress learningProgress_;
    
    // State management
    AgentState currentState_;
    std::unordered_map<std::string, float> internalState_;
    
    // Random generation
    std::mt19937 rng_;
    
    // Internal helper methods
    void initializeCuriositySystem();
    void initializeMemorySystems();
    void initializePlanningSystem();
    void initializeMetaCognitiveSystem();
    
    Goal selectGoalFromMemory() const;
    MotorCommand executeSelectedPlan() const;
    float calculateIntrinsicMotivationComponents() const;
    void updateSocialKnowledge();
    void updateMetaCognitiveAssessment();
    
    // Configuration
    AdvancedAgentConfig config_;
};

// Advanced Agent Configuration
struct AdvancedAgentConfig {
    // Cognitive capabilities
    bool enableMetaCognitiveMonitoring;
    bool enableSocialLearning;
    bool enableGoalBasedBehavior;
    bool enableWorkingMemory;
    bool enableConceptFormation;
    bool enablePlanning;
    
    // Behavioral parameters
    float explorationRate;
    float exploitationRate;
    float curiosityThreshold;
    float uncertaintyThreshold;
    
    // Learning parameters
    float socialLearningRate;
    float metaLearningRate;
    float goalAdaptationRate;
    float memoryConsolidationRate;
    
    // Goal management
    int maxConcurrentGoals;
    float goalPrioritizationStrength;
    float goalFlexibility;
    
    // Planning parameters
    int planningHorizon;
    float planEvaluationConfidence;
    float alternativePlanExploration;
    
    // Social parameters
    float socialObservationWindow;
    float culturalTransmissionRate;
    float socialLearningCapacity;
    
    // Meta-cognitive parameters
    float selfMonitoringFrequency;
    float confidenceCalibrationRate;
    float uncertaintyProcessingDepth;
    
    // Working memory parameters
    float workingMemoryCapacity;
    float memoryConsolidationInterval;
    float retrievalSuccessRate;
    
    // Performance monitoring
    float performanceEvaluationInterval;
    float learningProgressTracking;
    float adaptationSensitivity;
    
    // Defaults constructor
    AdvancedAgentConfig() : 
        enableMetaCognitiveMonitoring(true),
        enableSocialLearning(true),
        enableGoalBasedBehavior(true),
        enableWorkingMemory(true),
        enableConceptFormation(true),
        enablePlanning(true),
        explorationRate(0.3f),
        exploitationRate(0.7f),
        curiosityThreshold(0.5f),
        uncertaintyThreshold(0.3f),
        socialLearningRate(0.2f),
        metaLearningRate(0.05f),
        goalAdaptationRate(0.1f),
        memoryConsolidationRate(0.15f),
        maxConcurrentGoals(5),
        goalPrioritizationStrength(0.8f),
        goalFlexibility(0.5f),
        planningHorizon(10),
        planEvaluationConfidence(0.7f),
        alternativePlanExploration(0.3f),
        socialObservationWindow(0.5f),
        culturalTransmissionRate(0.1f),
        socialLearningCapacity(0.8f),
        selfMonitoringFrequency(0.1f),
        confidenceCalibrationRate(0.2f),
        uncertaintyProcessingDepth(0.4f),
        workingMemoryCapacity(0.8f),
        memoryConsolidationInterval(1000.0),
        retrievalSuccessRate(0.6f),
        performanceEvaluationInterval(100.0),
        learningProgressTracking(0.3f),
        adaptationSensitivity(0.4f) {}
};

// Agent State
struct AgentState {
    // Basic state
    int agentId;
    std::string agentType;
    double age;                    // Developmental age
    float energy;                  // Energy level (0-1)
    float health;                  // Health (0-1)
    
    // Position and orientation
    float x, y;                    // World position
    float orientation;             // Heading direction (radians)
    float velocityX, velocityY;     // Movement velocity
    float angularVelocity;         // Angular velocity
    
    // Behavioral state
    MotorCommand currentAction;   // Current action being executed
    std::string actionState;       // Current action state
    double actionStartTime;        // When current action started
    
    // Cognitive state
    std::vector<Goal> activeGoals; // Currently active goals
    float currentCuriosity;        // Current curiosity level
    float currentNovelty;          // Current novelty detection
    float predictionError;         // Prediction error signal
    
    // Memory state
    float workingMemoryLoad;       // Current working memory utilization
    size_t episodicMemoryCount;    // Number of stored episodes
    
    // Social state
    int socialGroupId;             // Current social group
    float socialBonding;           // Social bonding level (0-1)
    
    // Meta-cognitive state
    float confidence;              // Confidence in current state
    float uncertainty;             // Uncertainty about current state
    float selfAwareness;           // Level of self-awareness
    
    // Reset state
    void reset() {
        agentId = -1;
        agentType = "Unknown";
        age = 0.0;
        energy = 1.0f;
        health = 1.0f;
        x = y = 0.0f;
        orientation = 0.0f;
        velocityX = velocityY = 0.0f;
        angularVelocity = 0.0f;
        currentAction = MotorCommand::Wait;
        actionState = "Idle";
        actionStartTime = 0.0;
        activeGoals.clear();
        currentCuriosity = 0.0f;
        currentNovelty = 0.0f;
        predictionError = 0.0f;
        workingMemoryLoad = 0.0f;
        episodicMemoryCount = 0;
        socialGroupId = -1;
        socialBonding = 0.0f;
        confidence = 1.0f;
        uncertainty = 0.0f;
        selfAwareness = 0.0f;
    }
};

// Goal
struct Goal {
    int id;
    std::string description;
    float priority;               // Goal priority (0-1)
    float importance;             // Goal importance (0-1)
    float progress;               // Current progress (0-1)
    double deadline;              // Deadline (simulation steps)
    double createdTime;           // When goal was created
    
    // Target and conditions
    std::string target;           // Goal target (e.g., "reach_position_5,5")
    float threshold;              // Success threshold
    std::function<bool(const AgentState&, const std::shared_ptr<Brain>&)> condition;
    
    // Evaluation and feedback
    float expectedValue;          // Expected utility of achieving goal
    float actualValue;             // Actual value experienced
    float learningRate;           // Learning rate for goal adaptation
    
    Goal(int id, const std::string& desc, float priority, float importance,
         double deadline, const std::string& target, float threshold)
        : id(id), description(desc), priority(priority), importance(importance),
          progress(0.0f), deadline(deadline), createdTime(0.0), target(target),
          threshold(threshold), expectedValue(1.0f), actualValue(1.0f),
          learningRate(0.01f) {}
};

// Plan
struct Plan {
    int id;
    std::string name;
    std::vector<ActionStep> steps;
    float expectedUtility;        // Expected utility of the plan
    float actualUtility;          // Actual utility experienced
    float confidence;             // Confidence in plan effectiveness
    double creationTime;          // When plan was created
    
    // Evaluation metrics
    float executionCost;          // Cost to execute plan
    float successProbability;     // Probability of plan success
    float timeToComplete;         // Expected time to complete
    
    Plan(int id, const std::string& name, const std::vector<ActionStep>& planSteps)
        : id(id), name(name), steps(planSteps), expectedUtility(1.0f),
          actualUtility(1.0f), confidence(1.0f), creationTime(0.0),
          executionCost(1.0f), successProbability(0.8f), timeToComplete(10.0) {}
};

// Action Step
struct ActionStep {
    MotorCommand action;          // Action to perform
    double duration;              // Duration of action (seconds)
    float energyCost;             // Energy cost of action
    float rewardProbability;      // Probability of receiving reward
    float rewardMagnitude;        // Magnitude of expected reward
    std::string precondition;     // Preconditions for action
    
    ActionStep(MotorCommand act, double dur, float energyCost,
               float rewardProb, float rewardMag, const std::string& precondition)
        : action(act), duration(dur), energyCost(energyCost),
          rewardProbability(rewardProb), rewardMagnitude(rewardMag),
          precondition(precondition) {}
};

// Social Observation
struct SocialObservation {
    int observingAgentId;          // ID of agent being observed
    double observationTime;       // When observation occurred
    std::string observationType;   // Type of observation (behavior, outcome, etc.)
    std::vector<float> sensoryData; // Sensory data observed
    float outcomeValue;            // Outcome value observed
    std::string context;          // Context of observation
    
    SocialObservation(int observerId, double time, const std::string& type,
                      const std::vector<float>& data, float outcome,
                      const std::string& context)
        : observingAgentId(observerId), observationTime(time),
          observationType(type), sensoryData(data), outcomeValue(outcome),
          context(context) {}
};

// Meta Cognitive State
struct MetaCognitiveState {
    float confidence;              // Confidence in current knowledge
    float uncertainty;             // Uncertainty about current state
    float selfAwareness;           // Level of self-awareness
    float cognitiveLoad;           // Current cognitive processing load
    float metaLearningRate;        // Rate of meta-learning
    float selfMonitoring;          // Degree of self-monitoring
    float reflectiveProcessing;    // Level of reflective thinking
    
    // Assessment metrics
    float predictionAccuracy;     // Accuracy of predictions
    float decisionQuality;         // Quality of decisions made
    float learningProgress;       // Progress in learning
    float adaptabilityScore;      // Ability to adapt to changes
    
    // Reset state
    void reset() {
        confidence = 1.0f;
        uncertainty = 0.0f;
        selfAwareness = 0.0f;
        cognitiveLoad = 0.0f;
        metaLearningRate = 0.01f;
        selfMonitoring = 0.5f;
        reflectiveProcessing = 0.3f;
        predictionAccuracy = 1.0f;
        decisionQuality = 1.0f;
        learningProgress = 0.0f;
        adaptabilityScore = 0.5f;
    }
};

// Behavioral Profile
struct BehavioralProfile {
    float explorationTendency;    // Tendency toward exploration (0-1)
    float exploitationTendency;   // Tendency toward exploitation (0-1)
    float riskTaking;              // Risk taking propensity (0-1)
    float noveltySeeking;          // Novelty seeking propensity (0-1)
    float socialCooperation;       // Social cooperation tendency (0-1)
    float learningFlexibility;     // Flexibility in learning (0-1)
    float goalPersistence;        // Persistence in pursuing goals (0-1)
    float attention span;          // Attention span (seconds)
    
    BehavioralProfile() : 
        explorationTendency(0.3f),
        exploitationTendency(0.7f),
        riskTaking(0.5f),
        noveltySeeking(0.4f),
        socialCooperation(0.6f),
        learningFlexibility(0.5f),
        goalPersistence(0.8f),
        attentionSpan(10.0f) {}
};

// Performance Statistics
struct PerformanceStats {
    float totalReward;             // Total reward accumulated
    float totalSteps;              // Total steps taken
    float averageRewardPerStep;   // Average reward per step
    float explorationEfficiency;   // Efficiency of exploration
    float exploitationEfficiency;  // Efficiency of exploitation
    float learningRate;            // Rate of learning
    float goalAchievementRate;     // Rate of goal achievement
    float memoryEfficiency;        // Efficiency of memory usage
    float socialLearningEffectiveness; // Effectiveness of social learning
    float curiosityDrivenProgress; // Progress driven by curiosity
    
    // Reset statistics
    void reset() {
        totalReward = 0.0f;
        totalSteps = 0;
        averageRewardPerStep = 0.0f;
        explorationEfficiency = 1.0f;
        exploitationEfficiency = 1.0f;
        learningRate = 0.0f;
        goalAchievementRate = 0.0f;
        memoryEfficiency = 1.0f;
        socialLearningEffectiveness = 1.0f;
        curiosityDrivenProgress = 0.0f;
    }
};

// Learning Progress
struct LearningProgress {
    double lastUpdateTime;          // Time of last learning update
    float knowledgeGrowth;          // Growth in knowledge (0-1)
    float skillDevelopment;        // Development of skills (0-1)
    float behavioralAdaptation;   // Adaptation of behavior (0-1)
    float metaLearningProgress;    // Progress in meta-learning (0-1)
    float socialLearningProgress;  // Progress in social learning (0-1)
    
    // Reset progress
    void reset() {
        lastUpdateTime = 0.0;
        knowledgeGrowth = 0.0f;
        skillDevelopment = 0.0f;
        behavioralAdaptation = 0.0f;
        metaLearningProgress = 0.0f;
        socialLearningProgress = 0.0f;
    }
};

// Agent Step Result
struct AgentStepResult {
    double timestep;                // Current timestep
    double time;                    // Current time (seconds)
    AgentState stateBefore;         // State before step
    AgentState stateAfter;          // State after step
    MotorCommand actionTaken;       // Action taken
    float rewardReceived;           // Reward received
    float predictionError;          // Prediction error
    float curiosityLevel;           // Curiosity level during step
    bool actionSuccessful;          // Whether action was successful
    std::string actionResult;       // Result of action
    std::vector<float> sensoryInput; // Sensory input received
    std::vector<float> motorOutput; // Motor output produced
    
    // Evaluation metrics
    float stepUtility;              // Utility of this step
    float learningSignal;           // Learning signal received
    float behavioralChange;         // Behavioral change magnitude
    
    // Reset result
    void reset() {
        timestep = 0.0;
        time = 0.0;
        rewardReceived = 0.0f;
        predictionError = 0.0f;
        curiosityLevel = 0.0f;
        actionSuccessful = false;
        stepUtility = 0.0f;
        learningSignal = 0.0f;
        behavioralChange = 0.0f;
    }
};

// Goal Selection Strategy
enum class GoalSelectionStrategy {
    PriorityBased,          // Select highest priority goal
    NeedBased,              // Select goal based on needs
    RewardBased,            // Select goal based on expected reward
    Random,                 // Random goal selection
    Adaptive                // Adaptive goal selection based on context
};

// Social Learning Mechanism
enum class SocialLearningMechanism {
    Imitation,              // Direct imitation of observed behaviors
    Instruction,            // Learning from verbal instructions
    Teaching,              // Active teaching by others
    JointAttention,         // Learning through shared attention
    CulturalTransmission     // Learning from cultural norms
};

// Exploration Strategy
enum class ExplorationStrategy {
    RandomWalk,              // Random movement
    CuriousExploration,      // Curiosity-driven exploration
    RewardThreshold,         // Explore if reward below threshold
    NoveltySearch,           // Seek novel experiences
    InformationGain         // Maximize information gain
};

// Exploitation Strategy
enum class ExploitationStrategy {
    Greedy,                 // Always choose highest reward action
    EpsilonGreedy,           // Sometimes try random actions
    Softmax,                // Probability-based action selection
    ThompsonSampling,       // Bayesian parameter estimation
    UCB1                   // Upper confidence bound
};

} // namespace nlm

// Advanced Agent Implementation (forward declarations for complex types)
namespace nlm {
    class AdvancedAgent {
        // Implementation details
    };
}
