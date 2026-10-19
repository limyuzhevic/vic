#pragma once

#include "PlasticityRule.hpp"
#include <memory>
#include <string>

namespace nlm
{

// BCM (Bienenstock-Cooper-Munro) plasticity implementation
// Implements BCM theory with sliding threshold for synaptic stability
// Includes anti-Hebbian interactions and homeostatic regulation

class BCMPlasticity : public PlasticityRule {
public:
    BCMPlasticity();
    ~BCMPlasticity() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // BCM parameters
    void setTheta(float theta);      // Sliding threshold (θ)
    void setSigma(float sigma);      // Threshold sensitivity (σ)
    void setLearningRate(float rate);
    void setMaximumWeight(float maxWeight);
    void setMinimumWeight(float minWeight);
    void setThetaAdaptationRate(float rate);
    void setPostSynapticGain(float gain);
    
    float getTheta() const;
    float getSigma() const;
    float getLearningRate() const;
    float getMaximumWeight() const;
    float getMinimumWeight() const;
    float getThetaAdaptationRate() const;
    float getPostSynapticGain() const;
    
    // BCM dynamics
    void updateSlidingThreshold(float postActivity);
    float getSlidingThreshold() const;
    float calculateBCMChange(float postActivity, float theta) const;
    
    // BCM variants
    void enableAntiHebbianBCM(bool enable);
    void enableHomeostaticBCM(bool enable);
    void enableNeuromodulatorBCM(bool enable);
    
    bool isAntiHebbianBCMEnabled() const;
    bool isHomeostaticBCMEnabled() const;
    bool isNeuromodulatorBCMEnabled() const;
    
    // BCM statistics
    void recordBCMEvent(float postActivity, float theta, float delta);
    float getAveragePostActivity() const;
    float getThetaVolatility() const;
    float getBCMLearningRate() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Anti-Hebbian BCM
class AntiHebbianBCM : public PlasticityRule {
public:
    AntiHebbianBCM();
    ~AntiHebbianBCM() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Anti-Hebbian BCM parameters
    void setAntiHebbianRate(float rate);
    void setAntiHebbianThreshold(float threshold);
    void setAntiHebbianGain(float gain);
    
    float getAntiHebbianRate() const;
    float getAntiHebbianThreshold() const;
    float getAntiHebbianGain() const;
    
    // Anti-Hebbian BCM calculations
    float calculateAntiHebbianBCMChange(float postActivity, float theta) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Homeostatic BCM
class HomeostaticBCM : public PlasticityRule {
public:
    HomeostaticBCM();
    ~HomeostaticBCM() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Homeostatic BCM parameters
    void setHomeostaticTarget(float target);
    void setHomeostaticDeviation(float deviation);
    void setBCMHomeostaticRate(float rate);
    
    float getHomeostaticTarget() const;
    float getHomeostaticDeviation() const;
    float getBCMHomeostaticRate() const;
    
    // Homeostatic BCM calculations
    float calculateHomeostaticBCMChange(float postActivity, float theta) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Neuromodulator-enhanced BCM
class NeuromodulatorBCM : public PlasticityRule {
public:
    NeuromodulatorBCM();
    ~NeuromodulatorBCM() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Neuromodulator parameters
    void setDopamineLevel(float level);
    void setAcetylcholineLevel(float level);
    void setNorepinephrineLevel(float level);
    void setSerotoninLevel(float level);
    
    void setDopamineBCMModulation(float modulation);
    void setAcetylcholineBCMModulation(float modulation);
    void setNorepinephrineBCMModulation(float modulation);
    void setSerotoninBCMModulation(float modulation);
    
    float getDopamineLevel() const;
    float getAcetylcholineLevel() const;
    float getNorepinephrineLevel() const;
    float getSerotoninLevel() const;
    
    float getDopamineBCMModulation() const;
    float getAcetylcholineBCMModulation() const;
    float getNorepinephrineBCMModulation() const;
    float getSerotoninBCMModulation() const;
    
    // Neuromodulator BCM calculations
    float calculateNeuromodulatorBCMChange(float postActivity, float theta) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// BCM system coordinator
class BCMSystem {
public:
    BCMSystem();
    ~BCMSystem();
    
    // Add BCM plasticity rule
    void addBCMPlasticityRule(std::unique_ptr<PlasticityRule> rule);
    
    // Get specific BCM rule types
    BCMPlasticity* getBCMPlasticity() const;
    AntiHebbianBCM* getAntiHebbianBCM() const;
    HomeostaticBCM* getHomeostaticBCM() const;
    NeuromodulatorBCM* getNeuromodulatorBCM() const;
    
    // Get all BCM rules
    const std::vector<std::unique_ptr<PlasticityRule>>& getAllBCMRules() const;
    
    // System-wide BCM plasticity update
    void updateAll(Brain* brain, SimulationStep currentStep, Timestamp currentTime);
    
    // BCM system configuration
    void configureFromDefaults();
    void configureFromConfig(const class Config& config);
    
    // BCM system statistics and diagnostics
    void logBCMSystemState() const;
    float getSystemBCMPlasticityLoad() const;
    float getNetworkBCMStability() const;
    
    // Reset system
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm