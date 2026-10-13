#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Hebbian learning implementation with enhanced features
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
    
    // Core parameters
    void setLearningRate(float rate);
    float getLearningRate() const;
    
    // Weight bounds
    void setMaxWeight(float maxWeight);
    float getMaxWeight() const;
    void setMinWeight(float minWeight);
    float getMinWeight() const;
    
    // Advanced parameters
    void setTimeWindow(float window);
    float getTimeWindow() const;
    void setBaselineRate(float rate);
    float getBaselineRate() const;
    void setHomeostaticFactor(float factor);
    float getHomeostaticFactor() const;
    
    // Metaplasticity (learning rate adaptation)
    void setLearningRateAdaptation(float adaptation);
    float getLearningRateAdaptation() const;
    
    // Validation and configuration
    bool configureFromConfig(const std::string& prefix = "");
    std::string getDescription() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Advanced Hebbian variants
class OjaLearning : public Hebbian {
public:
    OjaLearning();
    ~OjaLearning() override;
    const char* getName() const override { return "Oja Learning"; }
};

class BatchNormHebbian : public Hebbian {
public:
    BatchNormHebbian();
    ~BatchNormHebbian() override;
    const char* getName() const override { return "BatchNorm Hebbian"; }
};

// Anti-Hebbian variants
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
    
    void setLearningRate(float rate);
    float getLearningRate() const;
    void setMaxDepression(float depression);
    float getMaxDepression() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// BCM (Bienenstock-Cooper-Munro) rule with adaptive threshold
class BCMRule : public PlasticityRule {
public:
    BCMRule();
    ~BCMRule() override;
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    void setThetaM(float thetaM);
    float getThetaM() const;
    void setSigma(float sigma);
    float getSigma() const;
    void setLearningRate(float rate);
    float getLearningRate() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
