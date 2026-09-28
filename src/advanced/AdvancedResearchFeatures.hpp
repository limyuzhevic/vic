#pragma once

#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nlm {

// Advanced plasticity rules for expert users
class CustomPlasticityRules {
public:
    CustomPlasticityRules() = default;
    ~CustomPlasticityRules() = default;
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Enable custom STDP rule
    void enableCustomSTDP(float maxWeightChange = 0.02f, 
                         float timeConstant = 20.0f,
                         float learningRate = 0.01f);
    
    // Enable custom Hebbian rule
    void enableCustomHebbian(float strength = 0.5f,
                            float decayRate = 0.01f,
                            float competitionFactor = 0.8f);
    
    // Enable custom structural plasticity
    void enableCustomStructuralPlasticity(float synaptogenesisBase = 0.0001f,
                                         float pruningBase = 0.00001f,
                                         float ageModulation = 0.5f);
    
    // Apply custom neuromodulation scheme
    void enableCustomNeuromodulation(float dopamineGain = 1.0f,
                                     float curiosityGain = 1.0f,
                                     float noveltyGain = 1.0f,
                                     float predictionErrorGain = 1.0f);
    
    // Enable custom development system
    void enableCustomDevelopment(float plasticityCurveExponent = 1.5f,
                                float criticalPeriodStart = 60.0f,
                                float maturityStart = 300.0f,
                                float agingStart = 900.0f);
    
    // Configure experimental parameters for research
    void configureExperimentParameters(const std::string& experimentType,
                                       const std::map<std::string, float>& parameters);
    
    // Get current configuration
    std::string getConfiguration() const;
    
    // Reset to default rules
    void resetToDefaults();

private:
    // Custom rule implementations
    void applyCustomSTDP(Synapse* synapse, const std::vector<Timestamp>& preSpikes,
                       const std::vector<Timestamp>& postSpikes, float timestep);
    
    void applyCustomHebbian(Synapse* synapse, const std::vector<Timestamp>& preSpikes,
                           const std::vector<Timestamp>& postSpikes, float timestep);
    
    void applyCustomStructuralPlasticity(float timeSinceLastUpdate, Brain* brain);
    
    void applyCustomNeuromodulation(Brain* brain, float timestep);
    
    void applyCustomDevelopment(Brain* brain, float timestep);
    
    // Configuration storage
    bool customSTDPEnabled_ = false;
    float customSTDPMaxWeightChange_ = 0.02f;
    float customSTDPTimeConstant_ = 20.0f;
    float customSTDPGrowthRate_ = 0.01f;
    
    bool customHebbianEnabled_ = false;
    float customHebbianStrength_ = 0.5f;
    float customHebbianDecayRate_ = 0.01f;
    float customHebbianCompetitionFactor_ = 0.8f;
    
    bool customStructuralPlasticityEnabled_ = false;
    float customSynaptogenesisBaseRate_ = 0.0001f;
    float customPruningBaseRate_ = 0.00001f;
    float customAgeModulationFactor_ = 0.5f;
    
    bool customNeuromodulationEnabled_ = false;
    float customDopamineGain_ = 1.0f;
    float customCuriosityGain_ = 1.0f;
    float customNoveltyGain_ = 1.0f;
    float customPredictionErrorGain_ = 1.0f;
    
    bool customDevelopmentEnabled_ = false;
    float customPlasticityCurveExponent_ = 1.5f;
    float customCriticalPeriodStart_ = 60.0f;
    float customMaturityStart_ = 300.0f;
    float customAgingStart_ = 900.0f;
    
    Brain* brain_ = nullptr;
};

// Advanced memory systems for research
class AdvancedMemorySystems {
public:
    AdvancedMemorySystems() = default;
    ~AdvancedMemorySystems() = default;
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Enable advanced working memory with temporal filtering
    void enableTemporalWorkingMemory(float temporalDecay = 0.95f,
                                     float predictionGain = 0.3f,
                                     float interferenceStrength = 0.2f);
    
    // Enable advanced episodic memory with replay dynamics
    void enableReplayDynamics(float replayStrength = 0.4f,
                              float consolidationThreshold = 0.7f,
                              float replayFrequency = 0.1f);
    
    // Enable semantic memory with vector space representations
    void enableSemanticMemory(float vectorDimension = 128.0f,
                             float similarityThreshold = 0.6f,
                             float learningRate = 0.05f);
    
    // Enable procedural memory with reinforcement learning
    void enableProceduralMemory(float eligibilityTraceDecay = 0.99f,
                               float rewardPredictionGain = 0.5f,
                               float policyLearningRate = 0.01f);
    
    // Configure memory integration patterns
    void configureMemoryIntegration(const std::string& integrationPattern,
                                    const std::map<std::string, float>& weights);
    
    // Get memory system statistics
    std::string getMemoryStatistics() const;
    
    // Reset memory systems
    void resetMemorySystems();

private:
    // Advanced working memory implementation
    void applyTemporalWorkingMemory(Brain* brain, float timestep);
    
    // Advanced episodic memory implementation
    void applyReplayDynamics(Brain* brain, float timestep);
    
    // Advanced semantic memory implementation
    void applySemanticMemory(Brain* brain, const std::vector<float>& currentActivity);
    
    // Advanced procedural memory implementation
    void applyProceduralMemory(Brain* brain, const std::vector<float>& currentActivity,
                              float reward, float timestep);
    
    // Configuration storage
    bool temporalWorkingMemoryEnabled_ = false;
    float temporalDecay_ = 0.95f;
    float predictionGain_ = 0.3f;
    float interferenceStrength_ = 0.2f;
    
    bool replayDynamicsEnabled_ = false;
    float replayStrength_ = 0.4f;
    float consolidationThreshold_ = 0.7f;
    float replayFrequency_ = 0.1f;
    
    bool semanticMemoryEnabled_ = false;
    float vectorDimension_ = 128.0f;
    float similarityThreshold_ = 0.6f;
    float semanticLearningRate_ = 0.05f;
    
    bool proceduralMemoryEnabled_ = false;
    float eligibilityTraceDecay_ = 0.99f;
    float rewardPredictionGain_ = 0.5f;
    float policyLearningRate_ = 0.01f;
    
    Brain* brain_ = nullptr;
};

// Advanced cognition systems for research
class AdvancedCognitionSystems {
public:
    AdvancedCognitionSystems() = default;
    ~AdvancedCognitionSystems() = default;
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Enable advanced neural planning with temporal abstraction
    void enableTemporalAbstractionPlanning(float planningHorizon = 5.0f,
                                           float temporalDiscount = 0.9f,
                                           float uncertaintyWeight = 0.3f);
    
    // Enable advanced concept formation with hierarchical clustering
    void enableHierarchicalConceptFormation(float hierarchyDepth = 4.0f,
                                            float clusteringThreshold = 0.8f,
                                            float conceptStability = 0.7f);
    
    // Enable advanced attentional selection with resource allocation
    void enableResourceAllocationAttention(float allocationEfficiency = 0.85f,
                                          float noveltyAllocation = 0.3f,
                                          float habituationRate = 0.01f);
    
    // Configure cognitive architecture
    void configureCognitiveArchitecture(const std::string& architectureType,
                                        const std::map<std::string, float>& parameters);
    
    // Get cognitive system statistics
    std::string getCognitionStatistics() const;
    
    // Reset cognitive systems
    void resetCognitionSystems();

private:
    // Advanced neural planning implementation
    void applyTemporalAbstractionPlanning(Brain* brain, float timestep);
    
    // Advanced concept formation implementation
    void applyHierarchicalConceptFormation(Brain* brain, const std::vector<float>& currentActivity);
    
    // Advanced attentional selection implementation
    void applyResourceAllocationAttention(Brain* brain, float timestep);
    
    // Configuration storage
    bool temporalAbstractionPlanningEnabled_ = false;
    float planningHorizon_ = 5.0f;
    float temporalDiscount_ = 0.9f;
    float uncertaintyWeight_ = 0.3f;
    
    bool hierarchicalConceptFormationEnabled_ = false;
    float hierarchyDepth_ = 4.0f;
    float clusteringThreshold_ = 0.8f;
    float conceptStability_ = 0.7f;
    
    bool resourceAllocationAttentionEnabled_ = false;
    float allocationEfficiency_ = 0.85f;
    float noveltyAllocation_ = 0.3f;
    float habituationRate_ = 0.01f;
    
    Brain* brain_ = nullptr;
};

// Main orchestrator for advanced research features
class AdvancedResearchFeatures {
public:
    AdvancedResearchFeatures() = default;
    ~AdvancedResearchFeatures() = default;
    
    // Initialize all advanced features with brain
    void initialize(Brain* brain);
    
    // Enable all advanced features at once
    void enableAllAdvancedFeatures();
    
    // Configure experimental protocol
    void configureExperimentalProtocol(const std::string& protocolName,
                                       const std::map<std::string, std::string>& settings);
    
    // Get feature status
    bool isFeatureEnabled(const std::string& featureName) const;
    
    // Get comprehensive system report
    std::string getSystemReport() const;
    
    // Reset to standard configuration
    void resetToStandardConfiguration();

private:
    CustomPlasticityRules customPlasticityRules_;
    AdvancedMemorySystems advancedMemorySystems_;
    AdvancedCognitionSystems advancedCognitionSystems_;
    
    std::map<std::string, bool> featureStatus_;
    Brain* brain_ = nullptr;
};

} // namespace nlm