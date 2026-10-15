#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <functional>
#include <vector>

namespace nlm {

// Forward declarations
class RandomGenerator;

// Development system: orchestrates neural development
// Implements developmental stages, plasticity modulation, and structural changes

class DevelopmentSystem {
public:
    DevelopmentSystem();
    ~DevelopmentSystem();
    
    // Get current developmental stage
    DevelopmentalStage getStage() const;
    void setStage(DevelopmentalStage stage);
    
    // Advance developmental stage
    void advanceStage();
    
    // Get stage name
    const char* getStageName() const;
    
    // Update development (simple version)
    void update(Brain* brain, SimulationStep currentStep);
    
    // Update development with full parameters
    void update(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    
    // Get plasticity modifier for current stage
    float getPlasticityModifier() const;
    
    // Set plasticity factor (for neuromodulator integration)
    void setPlasticityFactor(float factor);
    
    // Get critical period info
    bool isCriticalPeriod() const;
    float getCriticalPeriodProgress() const;
    
    // Get developmental age (in simulation time)
    double getDevelopmentalAge() const { return age_; }
    
    // Set developmental age
    void setDevelopmentalAge(double age) { age_ = age; }
    
    // ========== DEVELOPMENT EFFECTS ON PLASTICITY ==========
    
    // Use developmental stage to modulate STDP and Hebbian learning rates
    void applyPlasticityModulation(STDP* stdp, Hebbian* hebbian, float plasticityFactor = 1.0f) const;
    
    // Use development to modulate synaptic eligibility traces
    void applyEligibilityTraceModulation(Synapse* synapse, float& eligibilityTrace, float plasticityFactor = 1.0f) const;
    
    // Use development to enhance or reduce plasticity based on stage
    void applyStageBasedPlasticity(Synapse* synapse, float plasticityAmount, float& totalPlasticity) const;
    
    // Get stage-specific STDP learning rate multiplier
    float getSTDPModulationFactor() const;
    
    // Get stage-specific Hebbian learning rate multiplier
    float getHebbianModulationFactor() const;
    
    // Get stage-specific eligibility trace time constant
    TimestepDuration getEligibilityTraceTimeConstant() const;
    
    // ========== DEVELOPMENT EFFECTS ON NEURAL DYNAMICS ==========
    
    // Use developmental stage to affect neural excitability
    void applyExcitabilityModulation(Neuron* neuron, float& excitability) const;
    
    // Use development to modulate firing thresholds and reset potentials
    void applyThresholdModulation(Neuron* neuron, float& threshold, float& resetPotential) const;
    
    // Use development to enhance or reduce spike reliability
    void applySpikeReliabilityModulation(Neuron* neuron, float& reliability) const;
    
    // Get stage-specific membrane time constant multiplier
    float getTimeConstantMultiplier() const;
    
    // Get stage-specific refractory period multiplier
    float getRefractoryMultiplier() const;
    
    // Get stage-specific input resistance multiplier
    float getInputResistanceMultiplier() const;
    
    // ========== DEVELOPMENT EFFECTS ON MEMORY SYSTEMS ==========
    
    // Use developmental stage to modulate memory consolidation
    void applyMemoryConsolidationModulation(float& consolidationFactor) const;
    
    // Use development to affect working memory capacity
    size_t getWorkingMemoryCapacity() const;
    
    // Use development to enhance episodic memory encoding
    void applyEpisodicMemoryEncodingModulation(float& encodingStrength) const;
    
    // Get stage-specific memory decay rate
    float getMemoryDecayRate() const;
    
    // Get stage-specific pattern completion strength
    float getPatternCompletionStrength() const;
    
    // ========== DEVELOPMENT EFFECTS ON NEUROMODULATION ==========
    
    // Use developmental stage to affect neuromodulator production
    void applyNeuromodulatorProductionModulation(float& dopamineProduction, float& acetylcholineProduction, 
                                                float& norepinephrineProduction, float& serotoninProduction) const;
    
    // Use development to modulate neuromodulator sensitivity
    void applyNeuromodulatorSensitivityModulation(float& dopamineSensitivity, float& acetylcholineSensitivity, 
                                                   float& norepinephrineSensitivity, float& serotoninSensitivity) const;
    
    // Use development to affect neuromodulator receptor density
    void applyReceptorDensityModulation(float& dopamineReceptors, float& acetylcholineReceptors, 
                                        float& norepinephrineReceptors, float& serotoninReceptors) const;
    
    // Get stage-specific neuromodulator production baseline
    float getDopamineProductionBaseline() const;
    float getAcetylcholineProductionBaseline() const;
    float getNorepinephrineProductionBaseline() const;
    float getSerotoninProductionBaseline() const;
    
    // Get current memory consolidation modulation factor
    float getMemoryConsolidationModulation() const;
    
    // ========== DEVELOPMENT EFFECTS ON NEUROMODULATION ==========
    
    // Use developmental stage to affect neuromodulator production
    void applyNeuromodulatorProductionModulation(float& dopamineProduction, float& acetylcholineProduction, 
                                                float& norepinephrineProduction, float& serotoninProduction) const;
    
    // Use development to modulate neuromodulator sensitivity
    void applyNeuromodulatorSensitivityModulation(float& dopamineSensitivity, float& acetylcholineSensitivity, 
                                                   float& norepinephrineSensitivity, float& serotoninSensitivity) const;
    
    // Use development to affect neuromodulator receptor density
    void applyReceptorDensityModulation(float& dopamineReceptors, float& acetylcholineReceptors, 
                                        float& norepinephrineReceptors, float& serotoninReceptors) const;
    
    // Get stage-specific neuromodulator production baseline
    float getDopamineProductionBaseline() const;
    float getAcetylcholineProductionBaseline() const;
    float getNorepinephrineProductionBaseline() const;
    float getSerotoninProductionBaseline() const;
    
    // ========== DEVELOPMENT EFFECTS ON COGNITION ==========
    
    // Use developmental stage to affect attention modulation strength
    void applyAttentionModulationModulation(float& attentionStrength) const;
    
    // Use development to modulate concept formation rate
    void applyConceptFormationModulation(float& formationRate) const;
    
    // Use development to affect planning capabilities
    void applyPlanningCapabilityModulation(float& planningCapability) const;
    
    // Get stage-specific attention gain factor
    float getAttentionGainFactor() const;
    
    // Get stage-specific concept formation efficiency
    float getConceptFormationEfficiency() const;
    
    // Get stage-specific planning depth capability
    size_t getPlanningDepthCapability() const;
    
    // ========== DEVELOPMENT EFFECTS ON PREDICTION ==========
    
    // Use developmental stage to affect prediction accuracy
    void applyPredictionAccuracyModulation(float& accuracy) const;
    
    // Use development to modulate prediction error sensitivity
    void applyPredictionErrorSensitivityModulation(float& errorSensitivity) const;
    
    // Use development to affect temporal prediction capabilities
    void applyTemporalPredictionModulation(float& temporalAccuracy) const;
    
    // Get stage-specific prediction time horizon
    TimestepDuration getPredictionTimeHorizon() const;
    
    // Get stage-specific prediction update rate
    float getPredictionUpdateRate() const;
    
    // Get stage-specific prediction error tolerance
    float getPredictionErrorTolerance() const;
    
    // ========== DEVELOPMENT SYSTEM INTEGRATION ==========
    
    // Call development system methods directly in brain step loop
    void integrateWithBrainSystems(Brain* brain) const;
    
    // Use development system output to affect multiple systems
    void applyDevelopmentEffectsToAllSystems(Brain* brain, float plasticityFactor = 1.0f) const;
    
    // Create feedback loops from development to other systems
    void createFeedbackLoops(Brain* brain) const;
    
    // Get comprehensive developmental profile
    struct DevelopmentalProfile {
        DevelopmentalStage stage;
        float plasticity;
        float excitability;
        size_t workingMemoryCapacity;
        float memoryConsolidation;
        float attentionStrength;
        float predictionAccuracy;
        size_t planningDepth;
        double age;
        float stageProgress;  // Progress through current stage (0-1)
    };
    
    DevelopmentalProfile getDevelopmentalProfile() const;
    
private:
    void resetToStageDefaults();
    
private:
    struct Impl;
    Impl* pImpl;
    double age_;
};

} // namespace nlm
