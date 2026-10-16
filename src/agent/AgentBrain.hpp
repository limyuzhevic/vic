#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../core/Config/Config.hpp"
#include <memory>
#include <vector>
#include <string>
#include <map>

namespace nlm {

// Agent personality traits
struct AgentPersonality {
    // Curiosity and exploration traits
    float curiosityLevel_;           // Base curiosity (0.0-1.0)
    float explorationRate_;          // Random exploration tendency (0.0-1.0)
    float riskTolerance_;           // Willingness to try novel actions (0.0-1.0)
    float persistence_;              // Persistence in pursuing goals (0.0-1.0)
    
    // Learning and adaptation traits
    float learningRate_;            // Speed of learning from experience (0.0-1.0)
    float memoryRetention_;         // How long memories last (0.0-1.0)
    float attentionSpan_;           // Focus duration (seconds)
    
    // Social and cooperation traits
    float socialCooperation_;       // Tendency to help others (0.0-1.0)
    float competitionLevel_;        // Competitive drive (0.0-1.0)
    
    // Emotional traits
    float emotionalStability_;      // Resilience to stress (0.0-1.0)
    float rewardSensitivity_;       // Response to rewards (0.0-1.0)
    float punishmentSensitivity_;   // Response to negative feedback (0.0-1.0)
    
    AgentPersonality()
        : curiosityLevel_(0.5f), explorationRate_(0.3f), riskTolerance_(0.5f), persistence_(0.5f)
        , learningRate_(0.5f), memoryRetention_(0.8f), attentionSpan_(10.0f)
        , socialCooperation_(0.5f), competitionLevel_(0.3f)
        , emotionalStability_(0.7f), rewardSensitivity_(0.7f), punishmentSensitivity_(0.3f) {}
};

// Advanced neuromodulator system
struct NeuromodulatorState {
    // Classical neuromodulators
    float dopamine_;               // Reward prediction error, motivation
    float serotonin_;              // Mood, satisfaction, anxiety
    float acetylcholine_;          // Attention, learning, memory
    float norepinephrine_;         // Arousal, alertness, stress response
    
    // Advanced neuromodulators
    float oxytocin_;               // Social bonding, trust
    float vasopressin_;            // Social behavior, aggression
    float endocannabinoid_;        // Pain, reward, memory modulation
    float melanin_concentrating_;   // Stress adaptation, energy
    
    // Novelty and curiosity system
    float noveltySignal_;           // Response to new stimuli
    float predictionError_;        // Surprise, learning signal
    float curiosityDrive_;         // Drive to explore and learn
    
    NeuromodulatorState()
        : dopamine_(0.0f), serotonin_(0.0f), acetylcholine_(0.0f), norepinephrine_(0.0f)
        , oxytocin_(0.0f), vasopressin_(0.0f), endocannabinoid_(0.0f), melanin_concentrating_(0.0f)
        , noveltySignal_(0.0f), predictionError_(0.0f), curiosityDrive_(0.0f) {}
};

// Behavioral state for decision making
struct BehavioralState {
    // Current goals and motivations
    std::string currentGoal_;       // Primary objective
    std::vector<std::string> activeGoals_;
    
    // Emotional and motivational states
    float currentMotivation_;       // Overall drive level (0.0-1.0)
    float currentMood_;             // Current emotional state (0.0-1.0)
    float stressLevel_;             // Current stress (0.0-1.0)
    
    // Action history and patterns
    std::vector<MotorCommand> recentActions_;
    std::map<MotorCommand, int> actionCounts_;
    double lastActionTime_;
    
    // Memory and experience
    std::vector<std::string> learnedBehaviors_;
    std::vector<float> behaviorValues_;
    std::vector<double> experienceTimestamps_;
    
    // Attention and focus
    std::string focusedActivity_;  // What the agent is currently focusing on
    float attentionLevel_;          // Current attention span (0.0-1.0)
    
    BehavioralState()
        : currentMotivation_(0.5f), currentMood_(0.5f), stressLevel_(0.0f), lastActionTime_(0.0)
        , attentionLevel_(1.0f) {}
};

// Advanced developmental stages with detailed timing
enum class AdvancedDevelopmentalStage {
    Embryonic,                      // Initial formation
    NeuralCrest,                    // Early neuron formation
    Neurogenesis,                    // Rapid neuron production
    Synaptogenesis,                 // Connection formation
    CriticalPeriod,                 // Sensitive learning window
    Puberty,                        // Hormonal changes
    Maturity,                       // Full functionality
    Senescence,                     // Age-related decline
    EvolutionaryStage              // Species-level adaptation
};

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding with advanced AI capabilities
class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain);
    AgentBrain(std::shared_ptr<Brain> brain, const std::shared_ptr<Config>& config);
    ~AgentBrain();
    
    // Initialize with world and configuration
    void initialize(const SimpleWorld& world);
    
    // Advanced initialization with personality and neuromodulation settings
    void initialize(const SimpleWorld& world, const AgentPersonality& personality);
    
    // Get sensory input size expected by brain
    size_t getSensoryInputSize() const;
    
    // Get motor output size expected
    size_t getMotorOutputSize() const;
    
    // Process sensory percept and inject into brain
    void processSensoryInput(const SensoryPercept& percept);
    
    // Decode brain motor activity into motor command
    MotorCommand decodeMotorCommand();
    
    // Apply neuromodulation based on reward
    void applyRewardModulation(float reward, float predictedReward);
    
    // Advanced neuromodulation with multiple signals
    void applyAdvancedNeuromodulation(const NeuromodulatorState& signals);
    
    // Update development system
    void updateDevelopment(double timestep);
    
    // Get current developmental stage
    DevelopmentalStage getDevelopmentalStage() const;
    AdvancedDevelopmentalStage getAdvancedDevelopmentalStage() const;
    
    // Get neuromodulation levels
    float getNeuromodulationLevel() const;
    NeuromodulatorState getNeuromodulatorState() const;
    
    // Get curiosity and exploration levels
    float getCuriosityLevel() const;
    float getNoveltyLevel() const;
    float getPredictionError() const;
    
    // Get personality traits
    AgentPersonality getPersonality() const { return personality_; }
    void setPersonality(const AgentPersonality& personality) { personality_ = personality; }
    
    // Get behavioral state
    BehavioralState getBehavioralState() const { return behavioralState_; }
    void setBehavioralState(const BehavioralState& state) { behavioralState_ = state; }
    
    // Get current goals and motivations
    std::string getCurrentGoal() const { return behavioralState_.currentGoal_; }
    void setCurrentGoal(const std::string& goal) { behavioralState_.currentGoal_ = goal; }
    
    // Add to active goals
    void addActiveGoal(const std::string& goal) { behavioralState_.activeGoals_.push_back(goal); }
    void removeActiveGoal(const std::string& goal);
    
    // Get goal completion status
    bool isGoalCompleted(const std::string& goal) const;
    double getGoalProgress(const std::string& goal) const;
    
    // Enhanced action selection with personality influence
    MotorCommand decodeMotorCommandWithPersonality();
    
    // Save/load agent state
    bool saveState(const std::string& filename) const;
    bool loadState(const std::string& filename);
    
    // Reset agent for new episode
    void reset();
    
    // Get brain pointer
    Brain* getBrain() { return brain_.get(); }
    
    // Configuration methods
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
    // Advanced configuration from config
    void configureFromConfig(const std::shared_ptr<Config>& config);
    
    // Helper methods for behavior analysis
    MotorCommand getMostFrequentAction() const;
    float getActionDiversity() const;
    double getAverageActionInterval() const;
    std::string getBehavioralPattern() const;
    
    // Learning and adaptation methods
    void learnFromExperience(const std::string& action, float outcome, double timestamp);
    void adaptBehavior(double timestep);
    void updatePersonality(double timestep);
    
    // Personality trait modifiers
    void modifyPersonalityTrait(const std::string& trait, float delta);
    void applyPersonalityEvolution(double timestep);
    
    // Advanced curiosity mechanisms
    void updateCuriosityBasedOnNovelty(const std::vector<float>& currentVision);
    void updateExplorationBasedOnPredictiveUncertainty();
    void activateCuriosityDrivenExploration();
    
    // Social behavior methods (for future multi-agent support)
    void engageInSocialLearning(const std::vector<std::shared_ptr<AgentBrain>>& otherAgents);
    void developSocialPreferences();
    void establishSocialHierarchy();
    
    // Stress and emotional regulation
    void processEmotionalState(double timestep);
    void regulateStress(float stressLevel);
    void recoverFromStress(double recoveryTime);
    
    // Goal management
    void setGoalPriority(const std::string& goal, int priority);
    void updateGoalProgress(const std::string& goal, double progress);
    void completeGoal(const std::string& goal);
    
    // Memory consolidation
    void consolidateMemories(double timestep);
    void integrateExperiences();
    void extractGeneralizations();
    
    // Performance metrics
    struct PerformanceMetrics {
        double totalTimeActive_;
        size_t totalActionsTaken_;
        float averageActionSuccessRate_;
        double learningEfficiency_;
        double explorationEfficiency_;
        std::vector<double> performanceHistory_;
        
        PerformanceMetrics() : totalTimeActive_(0.0), totalActionsTaken_(0), 
                              averageActionSuccessRate_(0.5f), learningEfficiency_(0.0), 
                              explorationEfficiency_(0.0) {}
    };
    
    PerformanceMetrics getPerformanceMetrics() const { return performanceMetrics_; }
    void recordActionOutcome(bool success, double timestep);
    
    // Development stage progression with detailed timing
    void advanceDevelopmentStage(double timestep);
    double getTimeInCurrentStage() const;
    bool isDevelopmentStageComplete() const;
    
    // Resource management
    float getAvailableResources() const;
    void consumeResources(float amount);
    void regenerateResources(double timestep);
    
    // Environmental adaptation
    void adaptToEnvironment(const SimpleWorld& world);
    void optimizeForCurrentConditions();
    void prepareForFutureConditions();
    
private:
    // Core initialization
    void initializeAdvancedSystems();
    void setupMotorNeuronGroups();
    void setupSensoryNeuronGroups();
    
    // Advanced motor decoding with personality influence
    MotorCommand decodeFromMotorNeuronsWithPersonality();
    MotorCommand selectWithCuriosityAndPersonality(MotorCommand defaultCmd);
    MotorCommand selectWithExplorationAndGoals(MotorCommand defaultCmd);
    
    // Enhanced neuromodulation calculations
    void calculateAdvancedNeuromodulators(const SensoryPercept& percept, float reward);
    void updateDopamineLevel(float predictionError, float reward);
    void updateSerotoninLevel(float satisfaction);
    void updateAcetylcholineLevel(const SensoryPercept& percept);
    void updateNorepinephrineLevel(float stress, float arousal);
    void updateAdvancedNeuromodulators();
    
    // Behavioral decision making
    MotorCommand makeGoalDirectedChoice(const std::vector<MotorCommand>& options);
    MotorCommand makeEmotionallyBiasedChoice(const std::vector<MotorCommand>& options);
    MotorCommand makeCuriosityDrivenChoice(const std::vector<MotorCommand>& options);
    
    // Memory and learning systems
    void storeExperience(const std::string& context, const std::string& action, 
                        float outcome, double timestamp);
    void retrieveRelevantMemories(const std::string& context, 
                                 std::vector<std::pair<std::string, float>>& memories) const;
    
    // Development stage management
    void updateAdvancedDevelopmentStage(double timestep);
    void handleStageTransition(AdvancedDevelopmentalStage oldStage, 
                              AdvancedDevelopmentalStage newStage);
    
    // Performance optimization
    void optimizeNeuralActivity(double timestep);
    void balanceExcitatoryInhibitoryActivity();
    void maintainHomeostasis();
    
    // Error handling and recovery
    void handleProcessingErrors(const std::string& errorContext);
    void recoverFromErrors(double recoveryTime);
    void implementErrorCorrection();
    
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<Config> config_;
    
    // Advanced state systems
    NeuromodulatorState neuromodulators_;
    BehavioralState behavioralState_;
    AgentPersonality personality_;
    
    // Core systems (maintained for backward compatibility)
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    
    // Legacy neuromodulation state (for backward compatibility)
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    
    // Legacy development state (for backward compatibility)
    double developmentalAge_;
    float plasticityModifier_;
    
    // Legacy configuration flags (for backward compatibility)
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
    
    // Advanced developmental timing
    AdvancedDevelopmentalStage advancedDevelopmentalStage_;
    double stageStartTime_;
    double stageDuration_; // Planned duration for current stage
    
    // Performance metrics
    PerformanceMetrics performanceMetrics_;
    
    // Resource management
    float availableResources_;
    float resourceRegenerationRate_;
    
    // Environmental adaptation
    std::vector<float> environmentalSignatures_;
    double lastEnvironmentalUpdate_;
    
    // Learning and memory systems
    std::vector<std::string> learnedPatterns_;
    std::vector<float> patternWeights_;
    std::vector<double> patternTimestamps_;
    
    // Stress and emotional regulation
    float currentStressLevel_;
    float stressRecoveryRate_;
    double lastStressEventTime_;
    
    // Goal management
    std::map<std::string, double> goalProgress_;
    std::map<std::string, int> goalPriority_;
    std::map<std::string, bool> goalCompleted_;
    
    // Social behavior (future expansion)
    std::vector<std::shared_ptr<AgentBrain>> socialConnections_;
    std::map<std::string, float> socialPreferences_;
    
    // Error handling
    int consecutiveErrors_;
    double lastErrorTime_;
    bool inErrorRecoveryMode_;
};

} // namespace nlm
