// Phase6EnhancedExperiment - Enhanced Phase 6 integration test
// Extends Phase6IntegratedExperiment with advanced features

#pragma once

/**
 * Enhanced Phase 6 integration test result
 * Extends Phase6IntegrationResult with advanced metrics
 */
struct Phase6EnhancedIntegrationResult : public Phase6IntegrationResult {
    // Advanced metrics
    float workingMemoryUsage;
    float episodicMemoryUsage;
    float socialLearningScore;
    float conceptFormationScore;
    float learningRate;
    float advancedPlasticityScore;
    float neuronsPerSecond;
    
    // Performance metrics
    float avgSpikingFrequency;
    float synapticPlasticityRate;
    float memoryRetrievalSpeed;
    float attentionEfficiency;
    
    Phase6EnhancedIntegrationResult()
        : workingMemoryUsage(0.0f)
        , episodicMemoryUsage(0.0f)
        , socialLearningScore(0.0f)
        , conceptFormationScore(0.0f)
        , learningRate(0.0f)
        , advancedPlasticityScore(0.0f)
        , neuronsPerSecond(0.0f)
        , avgSpikingFrequency(0.0f)
        , synapticPlasticityRate(0.0f)
        , memoryRetrievalSpeed(0.0f)
        , attentionEfficiency(0.0f) {}
};

/**
 * Enhanced configuration for Phase 6 integration experiment
 * Extends Phase6Config with advanced settings
 */
struct Phase6EnhancedConfig : public Phase6Config {
    // Advanced features
    bool enableAdvancedNeuromodulation;
    bool enableAdvancedMemory;
    bool enableSocialLearning;
    bool enableConceptFormation;
    bool enableAdvancedPlasticity;
    bool enablePerformanceOptimizations;
    
    // Performance parameters
    uint64_t advancedNeuronCount;
    float memoryCapacityMultiplier;
    float plasticityLearningRate;
    float neuromodulationStrength;
    float curiosityDrive;
    
    Phase6EnhancedConfig()
        : enableAdvancedNeuromodulation(true)
        , enableAdvancedMemory(true)
        , enableSocialLearning(true)
        , enableConceptFormation(true)
        , enableAdvancedPlasticity(true)
        , enablePerformanceOptimizations(true)
        , advancedNeuronCount(2000)
        , memoryCapacityMultiplier(2.0f)
        , plasticityLearningRate(0.02f)
        , neuromodulationStrength(1.5f)
        , curiosityDrive(0.8f) {}
};

/**
 * Enhanced Phase 6 integrated experiment runner
 * Extends Phase6IntegratedExperiment with advanced functionality
 */
class Phase6EnhancedExperiment {
public:
    Phase6EnhancedExperiment();
    ~Phase6EnhancedExperiment();
    
    /**
     * Run the complete enhanced integration test
     */
    Phase6EnhancedIntegrationResult run(const Phase6EnhancedConfig& config);
    
    /**
     * Run enhanced integration verification
     */
    bool verifyIntegration();
    
    /**
     * Test advanced memory integration
     */
    bool testAdvancedMemoryIntegration();
    
    /**
     * Test advanced neuromodulation integration
     */
    bool testAdvancedNeuromodulationIntegration();
    
    /**
     * Test advanced plasticity integration
     */
    bool testAdvancedPlasticityIntegration();
    
    /**
     * Test advanced prediction integration
     */
    bool testAdvancedPredictionIntegration();
    
    /**
     * Test enhanced checkpoint save/load
     */
    bool testEnhancedCheckpointing();
    
    /**
     * Test enhanced replay system
     */
    bool testEnhancedReplay();
    
    /**
     * Test social learning integration
     */
    bool testSocialLearningIntegration();
    
    /**
     * Test concept formation integration
     */
    bool testConceptFormationIntegration();
    
    /**
     * Test performance optimizations
     */
    bool testPerformanceOptimizations();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
