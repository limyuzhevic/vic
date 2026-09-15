#pragma once

#include "PlasticityRule.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

// Enhanced Hebbian learning with covariance-based learning (Oja's rule, BCM theory)
class Hebbian : public PlasticityRule {
public:
    Hebbian();
    ~Hebbian() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Basic Hebbian parameters
    void setLearningRate(float rate);
    float getLearningRate() const;
    void setMaxWeight(float maxWeight);
    float getMaxWeight() const;
    
    // BCM theory (Bienenstock-Cooper-Munro) parameters
    void setTheta(float theta);           // BCM threshold variable
    float getTheta() const;
    void setTauTheta(float tau);          // Adaptation time constant for theta
    float getTauTheta() const;
    void setBCMLearning(float learningRate);
    float getBCMLearning() const;
    
    // Oja's rule parameters
    void setOjaAlpha(float alpha);        // Oja's rule parameter
    float getOjaAlpha() const;
    
    // Covariance-based learning parameters
    void setCovarianceLearningRate(float rate);
    float getCovarianceLearningRate() const;
    void setActivityHistorySize(size_t size);
    size_t getActivityHistorySize() const;
    
    // Activity-dependent learning rate
    void setActivityDependentLearning(float rate); // Learning rate based on activity
    float getActivityDependentLearning() const;
    
    // Synaptic tagging and capture
    void setTaggingThreshold(float threshold);
    float getTaggingThreshold() const;
    void setCaptureDuration(float duration); // Time window for capture
    float getCaptureDuration() const;
    
    // Neuromodulation integration
    void setModulationFactor(float factor); // Neuromodulator effect on learning
    float getModulationFactor() const;
    
    // Configure from various sources
    void configure(float learningRate, float maxWeight = 1.0f);
    void configureBCM(float theta, float learningRate);
    void configureOja(float alpha);
    void configureCovariance(float rate, size_t historySize = 1000);
    
    // Get statistics
    float getAveragePreActivity(const Synapse* synapse) const;
    float getAveragePostActivity(const Synapse* synapse) const;
    float getCovariance(const Synapse* synapse) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Anti-Hebbian learning for inhibitory connections
// Implements "neurons that fire together, wire away" for inhibitory synapses
class AntiHebbian : public PlasticityRule {
public:
    AntiHebbian();
    ~AntiHebbian() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Parameters
    void setLearningRate(float rate);
    float getLearningRate() const;
    void setMaxDepression(float depression); // Maximum depression strength
    float getMaxDepression() const;
    void setMinWeight(float min);            // Minimum weight (more negative)
    float getMinWeight() const;
    
    void configure(float learningRate, float maxDepression = 0.1f);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Homeostatic synaptic scaling
// Maintains overall activity levels across synapses and neurons
class HomeostaticSynapticScaling : public PlasticityRule {
public:
    HomeostaticSynapticScaling();
    ~HomeostaticSynapticScaling() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Homeostatic parameters
    void setTargetActivity(float target);    // Target firing rate (Hz)
    float getTargetActivity() const;
    void setScalingRate(float rate);         // How fast scaling occurs
    float getScalingRate() const;
    void setExponent(float exp);             // Power law exponent
    float getExponent() const;
    void setMinWeight(float min);            // Minimum weight for scaling
    float getMinWeight() const;
    void setMaxWeight(float max);            // Maximum weight for scaling
    float getMaxWeight() const;
    
    void configure(float target, float rate = 0.01f);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm