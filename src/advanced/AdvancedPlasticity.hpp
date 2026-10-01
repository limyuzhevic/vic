#pragma once

#include "../plasticity/PlasticityRule.hpp"
#include "../brain/Synapse.hpp"
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>

namespace nlm {

// Advanced plasticity rule types
enum class PlasticityRuleType {
    STDP,
    Hebbian,
    BCM,
    Oja,
    HTT,
    RewardModulatedSTDP,
    TripletSTDP,
    EligibilityTrace,
    SynapticTagging,
    Homeostatic,
    Custom
};

// Advanced plasticity configuration
struct AdvancedPlasticityConfig {
    // STDP variants
    float stdpLTPWeight;          // A+ for potentiation
    float stdpLTDWeight;          // A- for depression
    float stdpTimeConstant;       // Tau for exponential window
    float stdpTraceDecay;         // Eligibility trace decay rate
    
    // Hebbian variants
    float hebbianLearningRate;
    float hebbianThreshold;
    
    // BCM (Bienenstock–Cooper–Munro) rule
    float bcmLearningRate;
    float bcmThreshold;
    float bcmSlidingThreshold;
    
    // Oja rule (for PCA)
    float ojaLearningRate;
    float ojaRegularization;
    
    // HTT (Hebbian then Temporal) rule
    float httPreWeight;
    float httPostWeight;
    
    // Homeostatic plasticity
    float homeostaticTarget;
    float homeostaticLearningRate;
    float homeostaticTimeConstant;
    
    // Neuromodulation factors
    float rewardModulationFactor;
    float predictionErrorModulation;
    float noveltyModulation;
    
    // Synaptic constraints
    float minWeight;
    float maxWeight;
    float weightChangeLimit;
    
    // Multiple trace mechanisms
    struct Trace {
        std::vector<float> preTrace;
        std::vector<float> postTrace;
        std::vector<float> eligibilityTrace;
    };
    
    std::map<std::string, Trace> traces;
    
    // Synaptic tagging system
    struct Tag {
        std::vector<float> tags;
        float tagDecay;
        float tagThreshold;
    };
    
    std::map<std::string, Tag> tags;
    
    // Custom rule parameters
    std::map<std::string, float> customParameters;
};

// Advanced Plasticity Engine
class AdvancedPlasticity {
public:
    AdvancedPlasticity();
    ~AdvancedPlasticity();
    
    // Disable copying, enable moving
    AdvancedPlasticity(const AdvancedPlasticity&) = delete;
    AdvancedPlasticity& operator=(const AdvancedPlasticity&) = delete;
    AdvancedPlasticity(AdvancedPlasticity&&) noexcept = default;
    AdvancedPlasticity& operator=(AdvancedPlasticity&&) noexcept = default;
    
    // Configuration
    void configure(const AdvancedPlasticityConfig& config);
    void updateConfig(const std::string& key, float value);
    float getConfigValue(const std::string& key) const;
    
    // Rule management
    void addPlasticityRule(const std::string& name, PlasticityRuleType type, 
                          const std::map<std::string, float>& parameters = {});
    bool removePlasticityRule(const std::string& name);
    bool setActiveRule(const std::string& name);
    std::string getActiveRule() const;
    
    // Update synapses with advanced plasticity
    void updateSynapse(Synapse* synapse, const std::vector<Timestamp>& preSpikes,
                      const std::vector<Timestamp>& postSpikes, TimestepDuration dt,
                      float neuromodulationFactor = 1.0f);
    
    // Multiple trace mechanisms
    void updateTrace(Synapse* synapse, const std::vector<Timestamp>& spikeTimes,
                    const std::string& traceType = "eligibility",
                    TimestepDuration dt = 0.001f);
    float getTraceValue(Synapse* synapse, const std::string& traceType) const;
    
    // Synaptic tagging and capture
    void tagSynapse(Synapse* synapse, const std::string& tagName, 
                   float tagStrength = 1.0f, float tagLifetime = 100.0f);
    bool hasTag(Synapse* synapse, const std::string& tagName) const;
    float getTagStrength(Synapse* synapse, const std::string& tagName) const;
    
    // Homeostatic plasticity
    void adjustHomeostasis(const std::vector<Synapse*>& synapses, 
                          TimestepDuration dt, float targetActivity = 0.02f);
    
    // Pattern-based plasticity
    void applyPatternLearning(Synapse* synapse, const std::vector<float>& pattern,
                             float learningRate = 0.01f, 
                             const std::string& learningRule = "Oja");
    
    // Neuromodulation-dependent plasticity
    void applyRewardModulation(Synapse* synapse, float reward, 
                               float decayRate = 0.95f);
    void applyPredictionError(Synapse* synapse, float error, 
                            float learningRate = 0.1f);
    
    // Advanced STDP variants
    void applyTripletSTDP(Synapse* synapse, const std::vector<Timestamp>& preSpikes,
                         const std::vector<Timestamp>& postSpikes, TimestepDuration dt);
    void applyRewardModulatedSTDP(Synapse* synapse, const std::vector<Timestamp>& preSpikes,
                                  const std::vector<Timestamp>& postSpikes, 
                                  float reward, TimestepDuration dt);
    
    // BCM (Bienenstock-Cooper-Munro) rule
    void applyBCM(Synapse* synapse, float postsynapticActivity,
                 const std::vector<Timestamp>& postSpikes, TimestepDuration dt);
    
    // Oja rule for PCA
    void applyOja(Synapse* synapse, const std::vector<float>& output,
                 float learningRate = 0.01f);
    
    // Hebbian with constraints
    void applyConstrainedHebbian(Synapse* synapse, 
                                const std::vector<Timestamp>& preSpikes,
                                const std::vector<Timestamp>& postSpikes,
                                float maxWeightChange = 0.01f);
    
    // Plasticity statistics
    struct PlasticityStats {
        float totalWeightChange;
        float plasticityRate;
        std::map<std::string, float> ruleStats;
        std::map<std::string, float> traceStats;
        float homeostaticActivity;
        float neuromodulationEffect;
    };
    
    PlasticityStats calculateStats() const;
    
    // Export and import
    bool exportConfiguration(const std::string& filepath) const;
    bool importConfiguration(const std::string& filepath);
    
    // Get all plasticity rules
    std::vector<std::string> getRuleNames() const;
    PlasticityRuleType getRuleType(const std::string& name) const;
    
    // Set custom learning function
    void setCustomLearningRule(const std::string& name,
                              std::function<void(Synapse*, const std::vector<Timestamp>&,
                                               const std::vector<Timestamp>&, TimestepDuration, 
                                               float)> customRule);
    
    // Get current configuration
    const AdvancedPlasticityConfig& getConfig() const;
    
    // Reset plasticity traces
    void resetTraces(Synapse* synapse);
    
    // Apply synaptic constraints
    void enforceWeightLimits(Synapse* synapse);
    
    // Logging
    void logStatus() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
