#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Synapse.hpp"
#include <memory>
#include <vector>
#include <map>
#include <cmath>
#include <algorithm>

namespace nlm {

// Advanced plasticity rule types
enum class PlasticityRuleType {
    None,
    Hebbian,
    AntiHebbian,
    STDP,
    TripletSTDP,
    BCM,
    Metaplasticity,
    Homeostatic,
    Intrinsic,
    RewardModulated,
    Neuromodulated,
    Structural,
    BalancedExcitatoryInhibitory,
    SpikeTimingBased,
    AdaptiveLearningRate,
    ElasticWeightConsolidation
};

// Plasticity rule interface
class PlasticityRule {
public:
    virtual ~PlasticityRule() = default;
    
    // Update synaptic weights based on pre/post synaptic activity
    virtual void update(Synapse* synapse, 
                        const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        TimestepDuration dt) = 0;
    
    // Apply weight change with additional context
    virtual void applyWeightChange(Synapse* synapse, SynapticWeight delta, 
                                   const std::vector<Timestamp>& preSpikes = {},
                                   const std::vector<Timestamp>& postSpikes = {},
                                   double learningModulator = 1.0,
                                   double eligibilityTrace = 1.0) = 0;
    
    // Get rule name and type
    virtual const char* getName() const = 0;
    virtual PlasticityRuleType getRuleType() const = 0;
    
    // Configuration and parameters
    virtual void setLearningRate(float rate) {}
    virtual float getLearningRate() const { return 1.0f; }
    virtual void setEligibilityTraceDecay(float decay) {}
    virtual float getEligibilityTraceDecay() const { return 0.9f; }
    virtual void setThreshold(float threshold) {}
    virtual float getThreshold() const { return 0.0f; }
    
    // Rule control
    bool isEnabled() const;
    void setEnabled(bool enabled);
    virtual void reset() {} // Reset internal state
    
    // Advanced plasticity features
    virtual void setNeuromodulator(std::shared_ptr<void> modulator) {} // Neuromodulation interface
    virtual void setRewardSignal(float reward) {} // Reward-modulated learning
    virtual void setEligibilityTrace(double trace) {} // Eligibility traces
    
    // Statistical tracking
    virtual double getTotalWeightChange() const { return 0.0; }
    virtual size_t getSpikeCount() const { return 0; }
    virtual std::map<std::string, double> getStatistics() const { return std::map<std::string, double>(); }
    
    // Learning adaptation
    virtual bool shouldAdaptLearningRate() const { return false; }
    virtual void adaptLearningRate(double error) {}
    
    // Structural plasticity
    virtual bool isStructuralPlasticityEnabled() const { return false; }
    virtual void enableStructuralPlasticity(bool enable) {}
    
protected:
    PlasticityRule() : enabled_(true), lastUpdateTime_(0.0) {}
    
    double getTimeSinceLastUpdate() const;
    void updateLastUpdateTime();
    
    // Helper functions
    static double computeCosineSimilarity(const std::vector<Timestamp>& spikes1, const std::vector<Timestamp>& spikes2);
    static double computeSpikeTimingDependentWeightChange(Timestamp preTime, Timestamp postTime, double tau);
    static double computeTripletWeightChange(Timestamp preTime, Timestamp postTime, Timestamp post2Time, double tau1, double tau2);
    static double computeHomeostaticCorrection(double currentWeight, double targetWeight, double learningRate);
    
private:
    bool enabled_;
    double lastUpdateTime_;
};

// Enhanced Hebbian plasticity rule with adaptive learning
class AdaptiveHebbianRule : public PlasticityRule {
public:
    AdaptiveHebbianRule();
    ~AdaptiveHebbianRule() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta,
                           const std::vector<Timestamp>& preSpikes,
                           const std::vector<Timestamp>& postSpikes,
                           double learningModulator,
                           double eligibilityTrace) override;
    const char* getName() const override;
    PlasticityRuleType getRuleType() const override;
    
    // Adaptive learning parameters
    void setBaseLearningRate(float rate);
    void setActivityDependentLearning(float factor);
    void setCalciumDynamics(double baseline, double amplitude, double timeConstant);
    
    // Spike statistics
    double getAverageFiringRate() const;
    double getPrePostCorrelation() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Spike-Timing-Dependent Plasticity (STDP) with triplet component
class TripletSTDPRule : public PlasticityRule {
public:
    TripletSTDPRule();
    ~TripletSTDPRule() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta,
                           const std::vector<Timestamp>& preSpikes,
                           const std::vector<Timestamp>& postSpikes,
                           double learningModulator,
                           double eligibilityTrace) override;
    const char* getName() const override;
    PlasticityRuleType getRuleType() const override;
    
    // Triplet STDP parameters
    void setAplus(float aplus); // LTP for pre before post
    void setAminus(float aminus); // LTD for post before pre
    void setWplus(float wplus); // LTP for triple spike pattern
    void setWminus(float wminus); // LTD for triple spike pattern
    void setTauPlus(float tauPlus); // LTP time constant
    void setTauMinus(float tauMinus); // LTD time constant
    void setTauW(float tauW); // Triplet time constant
    
    // Eligibility traces for triplet STDP
    void updateEligibilityTraces(const std::vector<Timestamp>& preSpikes, TimestepDuration dt);
    double getPreTrace(int spikeIndex) const;
    double getPostTrace(int spikeIndex) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Bienenstock-Cooper-Munro (BCM) rule with sliding threshold
class BCMRule : public PlasticityRule {
public:
    BCMRule();
    ~BCMRule() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta,
                           const std::vector<Timestamp>& preSpikes,
                           const std::vector<Timestamp>& postSpikes,
                           double learningModulator,
                           double eligibilityTrace) override;
    const char* getName() const override;
    PlasticityRuleType getRuleType() const override;
    
    // BCM-specific parameters
    void setThetaPlus(float thetaPlus); // Sliding threshold for LTP
    void setThetaMinus(float thetaMinus); // Sliding threshold for LTD
    void setMu(float mu); // Modification of threshold
    void setEta(float eta); // Learning rate for threshold modification
    void setH(float h); // Threshold modification time constant
    
    // Populated activity integration
    void setMaxPopActivity(float maxPop); // Maximum population activity
    void setMinPopActivity(float minPop); // Minimum population activity
    
    double getCurrentTheta() const;
    double getPopActivity(const std::vector<Timestamp>& spikes) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Metaplasticity rule: plasticity of plasticity
class MetaplasticityRule : public PlasticityRule {
public:
    MetaplasticityRule();
    ~MetaplasticityRule() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta,
                           const std::vector<Timestamp>& preSpikes,
                           const std::vector<Timestamp>& postSpikes,
                           double learningModulator,
                           double eligibilityTrace) override;
    const char* getName() const override;
    PlasticityRuleType getRuleType() const override;
    
    // Metaplasticity parameters
    void setMetaLearningRate(float rate); // Learning rate for plasticity parameters
    void setMetaPlasticityThreshold(float threshold); // Threshold for metaplasticity
    void setMetaPlasticitySlope(float slope); // Slope of metaplasticity curve
    void setMetaPlasticityAsymmetry(float asymmetry); // Asymmetry between LTP and LTD
    
    // Historical plasticity tracking
    void recordHistoricalPlasticity(const std::vector<std::pair<double, double>>& weightChanges);
    double getPlasticityHistoryScore() const;
    
    // Adaptation parameters
    void setAdaptationTarget(float target);
    void setAdaptationRate(float rate);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Homeostatic plasticity rule
class HomeostaticPlasticityRule : public PlasticityRule {
public:
    HomeostaticPlasticityRule();
    ~HomeostaticPlasticityRule() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta,
                           const std::vector<Timestamp>& preSpikes,
                           const std::vector<Timestamp>& postSpikes,
                           double learningModulator,
                           double eligibilityTrace) override;
    const char* getName() const override;
    PlasticityRuleType getRuleType() const override;
    
    // Homeostatic parameters
    void setTargetExcitatoryRatio(float ratio); // Target E/I ratio
    void setHomeostaticLearningRate(float rate); // Learning rate for homeostasis
    void setHomeostaticTimeConstant(float tau); // Time constant for homeostatic dynamics
    void setIntracellularCalciumThreshold(float threshold); // Intracellular calcium threshold
    
    // Synaptic scaling parameters
    void setScalingFactor(float factor); // Global scaling factor
    void setScalingRange(float min, float max); // Scaling range
    void setActivityBasedScaling(bool enable); // Enable activity-dependent scaling
    
    double getCurrentExcitatoryRatio() const;
    double getMeanFiringRate() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Intrinsic plasticity rule
class IntrinsicPlasticityRule : public PlasticityRule {
public:
    IntrinsicPlasticityRule();
    ~IntrinsicPlasticityRule() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta,
                           const std::vector<Timestamp>& preSpikes,
                           const std::vector<Timestamp>& postSpikes,
                           double learningModulator,
                           double eligibilityTrace) override;
    const char* getName() const override;
    PlasticityRuleType getRuleType() const override;
    
    // Intrinsic plasticity parameters
    void setIntrinsicLearningRate(float rate); // Intrinsic learning rate
    void setCalciumDynamics(double baseline, double target, double timeConstant);
    void setIntrinsicPlasticityThreshold(float threshold); // Threshold for intrinsic plasticity
    void setIntrinsicPlasticityAsymmetry(float asymmetry); // Asymmetry between excitation and inhibition
    
    // Intracellular dynamics
    double getCurrentIntracellularCalcium() const;
    double computeCalciumDynamics(double current, double target, double dt);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Reward-modulated plasticity rule
class RewardModulatedPlasticityRule : public PlasticityRule {
public:
    RewardModulatedPlasticityRule();
    ~RewardModulatedPlasticityRule() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta,
                           const std::vector<Timestamp>& preSpikes,
                           const std::vector<Timestamp>& postSpikes,
                           double learningModulator,
                           double eligibilityTrace) override;
    const char* getName() const override;
    PlasticityRuleType getRuleType() const override;
    
    // Reward modulation parameters
    void setRewardLearningRate(float rate); // Learning rate modulated by reward
    void setPredictionError(double predictionError);
    void setPolicyGradient(double gradient);
    void setActorCriticParameters(double actorLearningRate, double criticLearningRate);
    
    // TD learning parameters
    void setTDLambda(double lambda); // Eligibility trace decay for TD learning
    void setTDGamma(double gamma); // Discount factor for TD learning
    void setTDDelta(double delta); // Learning rate for TD learning
    
    double getCurrentRewardEstimate() const;
    double computeTemporalDifferenceError(float reward, double nextValue, double discountFactor);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Elastic Weight Consolidation (EWC) rule
class ElasticWeightConsolidationRule : public PlasticityRule {
public:
    ElasticWeightConsolidationRule();
    ~ElasticWeightConsolidationRule() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta,
                           const std::vector<Timestamp>& preSpikes,
                           const std::vector<Timestamp>& postSpikes,
                           double learningModulator,
                           double eligibilityTrace) override;
    const char* getName() const override;
    PlasticityRuleType getRuleType() const override;
    
    // EWC parameters
    void setFisherInformation(double fisherInfo); // Fisher information matrix approximation
    void setRegularizationStrength(float strength); // Regularization strength
    void setTaskDiversity(float diversity); // Diversity across tasks
    void setInterTaskForgetting(float forgetting); // Forgetting across tasks
    
    // Task-specific parameters
    void setTaskId(int id);
    void setTaskImportance(double importance);
    
    // Gaussian approximation of Fisher information
    void updateFisherInformation(const std::vector<double>& gradients);
    double computeTaskSimilarity(int taskId) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Balanced excitatory-inhibitory plasticity rule
class BalancedExcitatoryInhibitoryRule : public PlasticityRule {
public:
    BalancedExcitatoryInhibitoryRule();
    ~BalancedExcitatoryInhibitoryRule() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta,
                           const std::vector<Timestamp>& preSpikes,
                           const std::vector<Timestamp>& postSpikes,
                           double learningModulator,
                           double eligibilityTrace) override;
    const char* getName() const override;
    PlasticityRuleType getRuleType() const override;
    
    // Balance parameters
    void setTargetEIBalance(float balance); // Target E/I balance ratio
    void setEIBalanceLearningRate(float rate); // Learning rate for E/I balance
    void setEIBalanceTimeConstant(float tau); // Time constant for E/I balance dynamics
    void setExcitatoryInhibitoryCoupling(float coupling); // Coupling strength between E and I populations
    
    // Population activity tracking
    void setExcitatoryNeuronCount(size_t count);
    void setInhibitoryNeuronCount(size_t count);
    double getExcitatoryActivity() const;
    double getInhibitoryActivity() const;
    double computeCurrentEIBalance() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Advanced plasticity rule factory
class PlasticityRuleFactory {
public:
    static std::unique_ptr<PlasticityRule> createRule(PlasticityRuleType type);
    static std::unique_ptr<PlasticityRule> createRule(const std::string& name);
    static std::vector<std::string> getAvailableRuleNames();
    static std::vector<PlasticityRuleType> getAvailableRuleTypes();
    static std::map<std::string, std::string> getRuleDescriptions();
    
    // Configuration templates
    static std::unique_ptr<PlasticityRule> createBalancedRule(); // Balanced plasticity (Hebbian + homeostatic)
    static std::unique_ptr<PlasticityRule> createLearningRule(); // Learning-focused plasticity (STDP + reward)
    static std::unique_ptr<PlasticityRule> createStabilityRule(); // Stability-focused plasticity (homeostatic + metaplastic)
    static std::unique_ptr<PlasticityRule> createDevelopmentRule(); // Development-focused plasticity (intrinsic + metaplastic)
    
    // Task-specific rules
    static std::unique_ptr<PlasticityRule> createPatternRecognitionRule(); // For pattern recognition tasks
    static std::unique_ptr<PlasticityRule> createMotorLearningRule(); // For motor learning tasks
    static std::unique_ptr<PlasticityRule> createMemoryRule(); // For memory consolidation tasks
    static std::unique_ptr<PlasticityRule> createSocialLearningRule(); // For social learning tasks
};

} // namespace nlm

// Convenience functions
inline std::unique_ptr<nlm::PlasticityRule> createHebbianRule() {
    return nlm::PlasticityRuleFactory::createRule(nlm::PlasticityRuleType::Hebbian);
}

inline std::unique_ptr<nlm::PlasticityRule> createSTDPRule() {
    return nlm::PlasticityRuleFactory::createRule(nlm::PlasticityRuleType::STDP);
}

inline std::unique_ptr<nlm::PlasticityRule> createTripletSTDPRule() {
    return nlm::PlasticityRuleFactory::createRule(nlm::PlasticityRuleType::TripletSTDP);
}

inline std::unique_ptr<nlm::PlasticityRule> createBCMRule() {
    return nlm::PlasticityRuleFactory::createRule(nlm::PlasticityRuleType::BCM);
}

inline std::unique_ptr<nlm::PlasticityRule> createMetaplasticityRule() {
    return nlm::PlasticityRuleFactory::createRule(nlm::PlasticityRuleType::Metaplasticity);
}

inline std::unique_ptr<nlm::PlasticityRule> createHomeostaticRule() {
    return nlm::PlasticityRuleFactory::createRule(nlm::PlasticityRuleType::Homeostatic);
}

inline std::unique_ptr<nlm::PlasticityRule> createIntrinsicRule() {
    return nlm::PlasticityRuleFactory::createRule(nlm::PlasticityRuleType::Intrinsic);
}

inline std::unique_ptr<nlm::PlasticityRule> createRewardModulatedRule() {
    return nlm::PlasticityRuleFactory::createRule(nlm::PlasticityRuleType::RewardModulated);
}

inline std::unique_ptr<nlm::PlasticityRule> createBalancedRule() {
    return nlm::PlasticityRuleFactory::createBalancedRule();
}

inline std::unique_ptr<nlm::PlasticityRule> createLearningRule() {
    return nlm::PlasticityRuleFactory::createLearningRule();
}

inline std::unique_ptr<nlm::PlasticityRule> createStabilityRule() {
    return nlm::PlasticityRuleFactory::createStabilityRule();
}

inline std::unique_ptr<nlm::PlasticityRule> createDevelopmentRule() {
    return nlm::PlasticityRuleFactory::createDevelopmentRule();
}