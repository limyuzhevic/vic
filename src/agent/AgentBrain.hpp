#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>

namespace nlm {

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain);
    ~AgentBrain();
    
    // Initialize with world
    void initialize(const SimpleWorld& world);
    
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
    
    // Update development system
    void updateDevelopment(double timestep);
    
    // Get current developmental stage
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Get neuromodulation level
    float getNeuromodulationLevel() const;
    
    // Get curiosity level
    float getCuriosityLevel() const;
    
    // Get novelty level
    float getNoveltyLevel() const;
    
    // Get prediction error
    float getPredictionError() const;
    
    // Reset agent for new episode
    void reset();
    
    // Get brain pointer
    Brain* getBrain() { return brain_.get(); }
    
    // Configuration
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
    // ========== ENHANCED MOTOR CONTROL ==========
    
    // Goal-based motor command selection
    struct Goal {
        enum Type { Navigation, Interaction, Exploration, Survival };
        Type type;
        float priority;
        std::vector<float> targetPosition;
        float deadline;
    };
    
    struct ActionPlan {
        MotorCommand primaryAction;
        std::vector<MotorCommand> alternativeActions;
        float confidence;
        float executionTime;
        float resourceCost;
    };
    
    // Select motor command based on goals and context
    MotorCommand selectMotorCommandBasedOnGoals(const std::vector<Goal>& goals);
    
    // Multi-action planning with confidence scores
    ActionPlan planMultiActionSequence(const std::vector<float>& state, int sequenceLength);
    
    // Memory-guided action selection
    MotorCommand selectMotorCommandBasedOnMemory(const std::vector<float>& state);
    
    // Get confidence scores for motor commands
    std::vector<float> getMotorCommandConfidences() const;
    
    // Get current action plan
    ActionPlan getCurrentActionPlan() const;
    
    // Update action plan with new sensory input
    void updateActionPlan(const SensoryPercept& percept);
    
    // ========== ADVANCED NEUROMODULATION ==========
    
    // Direct neuromodulator access
    float getAChLevel() const; // Acetylcholine - attention and memory consolidation
    float getNELevel() const;   // Norepinephrine - arousal and vigilance
    float get5HTLevel() const;  // Serotonin - mood and social behavior
    
    // Set neuromodulator levels
    void setAChLevel(float level);
    void setNELevel(float level);
    void set5HTLevel(float level);
    
    // Custom neuromodulation profiles
    struct NeuromodulationProfile {
        float baselineACh;
        float baselineNE;
        float baseline5HT;
        float noveltyAmplifier;
        float predictionErrorAmplifier;
        float rewardSensitivity;
        float explorationBias;
    };
    
    void setNeuromodulationProfile(const NeuromodulationProfile& profile);
    NeuromodulationProfile getNeuromodulationProfile() const;
    
    // Neuromodulation-based attention control
    void modulateAttention(float novelty, float predictionError, float reward);
    
    // ========== MEMORY INTEGRATION ==========
    
    // Direct working memory access
    const std::vector<float>* getWorkingMemoryBuffer() const;
    bool hasWorkingMemoryEntry(NeuronId neuron) const;
    float getWorkingMemoryEntry(NeuronId neuron) const;
    void storeInWorkingMemory(NeuronId neuron, float value);
    
    // Episodic memory retrieval
    struct MemoryContext {
        std::vector<float> state;
        std::vector<float> action;
        float reward;
        SimulationStep timestamp;
    };
    
    MemoryContext retrieveEpisodicMemory(float similarityThreshold = 0.7f) const;
    std::vector<MemoryContext> retrieveMemorySequence(size_t count) const;
    
    // Memory-based action planning
    MotorCommand selectActionBasedOnEpisodicMemory(const std::vector<float>& currentState) const;
    float getMemoryBasedActionConfidence(const std::vector<float>& state) const;
    
    // Clear specific memories
    void clearWorkingMemory();
    void clearEpisodicMemory();
    
    // ========== PREDICTION SYSTEM ==========
    
    // Direct prediction system access
    float getPredictionConfidence() const;
    const std::vector<float>* getPredictionErrorHistory() const;
    
    // Prediction error visualization (returns error visualization parameters)
    struct PredictionErrorVisualization {
        std::vector<float> errorMap; // 2D grid visualization
        float maxError;
        float minError;
        std::string visualizationType;
    };
    
    PredictionErrorVisualization visualizePredictionError() const;
    
    // Get prediction metrics
    struct PredictionMetrics {
        float accuracy;      // How accurate are predictions?
        float precision;     // How specific are predictions?
        float recall;        // How complete are predictions?
        float f1Score;       // Combined metric
    };
    
    PredictionMetrics getPredictionMetrics() const;
    
    // Train prediction system with new data
    void trainPredictionSystem(const SensoryInput& observation);
    
    // ========== DEVELOPMENT CONTROLS ==========
    
    // Fine-grained development stage control
    void setDevelopmentalStage(DevelopmentalStage stage);
    void advanceDevelopmentToStage(DevelopmentalStage targetStage);
    std::vector<DevelopmentalStage> getDevelopmentalTrajectory() const;
    
    // Custom developmental trajectories
    struct DevelopmentalTrajectory {
        double startAge;
        double endAge;
        std::vector<DevelopmentalStage> stages;
        std::vector<float> plasticityModifiers;
    };
    
    void setCustomDevelopmentalTrajectory(const DevelopmentalTrajectory& trajectory);
    
    // Intervention-based development
    void applyDevelopmentalIntervention(const std::string& interventionType, float intensity);
    void resetDevelopmentalStage();
    
    // ========== CONFIGURATION API ==========
    
    // Advanced parameter tuning
    template<typename T>
    void setAdvancedParameter(const std::string& category, const std::string& name, const T& value);
    
    template<typename T>
    T getAdvancedParameter(const std::string& category, const std::string& name) const;
    
    // Profile-based configuration
    void loadProfile(const std::string& profileName);
    void saveProfile(const std::string& profileName) const;
    std::vector<std::string> getAvailableProfiles() const;
    
    // Dynamic configuration updates
    void updateConfiguration(const std::string& key, const ConfigValue& value);
    std::vector<ConfigEntry> getConfigurationSnapshot() const;
    
    // Reset to default configuration
    void resetToDefaults();
    
    // ========== PERFORMANCE MONITORING ==========
    
    // Real-time performance metrics
    struct PerformanceMetrics {
        float motorCommandLatency;      // Time to select and execute motor commands
        float sensoryProcessingTime;    // Time to process sensory input
        float predictionUpdateTime;    // Time to update predictions
        float memoryAccessTime;        // Time to access memory systems
        float neuromodulationTime;     // Time to compute neuromodulation
        float memoryUtilization;       // How full is memory?
        float predictionAccuracy;      // How accurate are predictions?
        float explorationRatio;       // How much exploring vs exploiting?
    };
    
    PerformanceMetrics getPerformanceMetrics() const;
    
    // Bottleneck detection
    struct BottleneckInfo {
        std::string component;        // Which component is the bottleneck?
        float currentLoad;            // Current load percentage
        float threshold;              // Threshold for concern
        float suggestedImprovement;   // Suggested improvement percentage
    };
    
    BottleneckInfo detectBottleneck() const;
    
    // Performance optimization suggestions
    std::vector<std::string> getPerformanceSuggestions() const;
    
    // Set performance monitoring interval
    void setPerformanceMonitoringInterval(float interval);
    
    // Get performance statistics
    struct PerformanceStatistics {
        float averageLatency;         // Average over time
        float peakLatency;            // Maximum observed
        float averageAccuracy;        // Average over time
        std::vector<float> history;   // Recent history
    };
    
    PerformanceStatistics getPerformanceStatistics() const;
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    std::shared_ptr<Brain> brain_;
    
    // Motor neuron groups
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    // Sensory neuron groups
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    
    // Neuromodulation state
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    
    // Development state
    double developmentalAge_;
    float plasticityModifier_;
    
    // Configuration flags
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
    
    // ========== ENHANCED MOTOR CONTROL STATE ==========
    ActionPlan currentActionPlan_;
    
    // ========== ADVANCED NEUROMODULATION STATE ==========
    float achLevel_;                    // Acetylcholine level
    float neLevel_;                     // Norepinephrine level
    float serotoninLevel_;              // Serotonin level
    NeuromodulationProfile neuromodulationProfile_;
    
    // ========== MEMORY INTEGRATION STATE ==========
    std::unordered_map<NeuronId, float> workingMemoryBuffer_;
    std::vector<MemoryContext> episodicMemory_;
    
    // ========== PREDICTION SYSTEM STATE ==========
    // Note: Prediction system accessed directly via brain_
    
    // ========== DEVELOPMENT CONTROLS STATE ==========
    DevelopmentalTrajectory customDevelopmentalTrajectory_;
    float plasticityBoost_;
    float developmentSpeedMultiplier_;
    
    // ========== CONFIGURATION STATE ==========
    std::string profileName_;
    std::shared_ptr<Config> config_;
    
    // ========== PERFORMANCE MONITORING STATE ==========
    float performanceMonitoringInterval_;
    struct {
        float averageLatency;
        float peakLatency;
        float averageAccuracy;
        std::vector<float> history;
    } performanceStats_;
};

} // namespace nlm
