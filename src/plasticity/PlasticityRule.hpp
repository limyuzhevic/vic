#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Synapse.hpp"
#include <string>
#include <vector>
#include <memory>

namespace nlm {

// Abstract base class for plasticity rules
// Implements the interface for synaptic plasticity mechanisms

class PlasticityRule {
public:
    virtual ~PlasticityRule() = default;
    
    // Update synaptic weights based on pre/post synaptic activity
    virtual void update(Synapse* synapse, 
                        const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        TimestepDuration dt) = 0;
    
    // Apply weight change to synapse
    virtual void applyWeightChange(Synapse* synapse, SynapticWeight delta) = 0;
    
    // Get rule name identifier
    virtual const char* getName() const = 0;
    
    // Check if rule is enabled
    bool isEnabled() const;
    void setEnabled(bool enabled);
    
    // Get learning rate (can be overridden by subclasses)
    virtual float getLearningRate() const { return 0.0f; }
    virtual void setLearningRate(float rate) {}
    
    // Check if rule is biologically plausible
    virtual bool isBiologicallyPlausible() const { return true; }
    
    // Normalize rule parameters to biologically realistic ranges
    virtual void normalizeParameters() {}
    
protected:
    PlasticityRule() : enabled_(true) {}
    
private:
    bool enabled_;
};

// Factory for creating plasticity rules
class PlasticityRuleFactory {
public:
    static std::shared_ptr<PlasticityRule> createSTDP();
    static std::shared_ptr<PlasticityRule> createHebbian();
    static std::shared_ptr<PlasticityRule> createBCM();
    static std::shared_ptr<PlasticityRule> createAntiHebbian();
    static std::shared_ptr<PlasticityRule> createOja();
    static std::shared_ptr<PlasticityRule> createBMU(); // Batch Mean Unit
    
    // Get available rule names
    static std::vector<std::string> getAvailableRuleNames();
    
    // Create rule from name
    static std::shared_ptr<PlasticityRule> createFromName(const std::string& name);
};

// Complete implementation of Hebbian plasticity rule
// Implements covariance-based Hebbian learning
class HebbianRule : public PlasticityRule {
public:
    HebbianRule();
    ~HebbianRule() override;
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Parameter accessors
    float getLearningRate() const override;
    void setLearningRate(float rate) override;
    
    float getMaxWeight() const;
    void setMaxWeight(float maxWeight);
    
    float getMinWeight() const;
    void setMinWeight(float minWeight);
    
    float getCovarianceThreshold() const;
    void setCovarianceThreshold(float threshold);
    
    // Statistics
    struct Statistics {
        size_t totalUpdates;
        float totalWeightChange;
        float averageWeightChange;
        size_t strongConnections;   // Weights > 0.5
        size_t weakConnections;     // Weights < -0.5
        size_t balancedConnections; // Weights in [-0.5, 0.5]
    };
    
    Statistics getStatistics() const;
    void resetStatistics();
    
    // Configuration for different learning scenarios
    void configureForSupervisedLearning();
    void configureForUnsupervisedLearning();
    void configureForOnlineLearning();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Anti-Hebbian rule: "neurons that fire together, separate together"
class AntiHebbianRule : public PlasticityRule {
public:
    AntiHebbianRule();
    ~AntiHebbianRule() override;
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    float getLearningRate() const override;
    void setLearningRate(float rate) override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Bienenstock-Cooper-Munro (BCM) synaptic stability rule
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
    
    // BCM parameters
    float getTheta() const;           // Threshold parameter
    void setTheta(float theta);
    
    float getSigma() const;           // Soft threshold width
    void setSigma(float sigma);
    
    float getLearningRate() const override;
    void setLearningRate(float rate) override;
    
    // BCM-specific functions
    float computeSlidingThreshold(float postActivity);
    float computeWeightChange(float preActivity, float postActivity, float theta);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Oja learning rule: normalized Hebbian learning
class OjaRule : public PlasticityRule {
public:
    OjaRule();
    ~OjaRule() override;
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    float getLearningRate() const override;
    void setLearningRate(float rate) override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Batch Mean Unit (BMU) rule: batch learning approach
class BMURule : public PlasticityRule {
public:
    BMURule();
    ~BMURule() override;
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    float getLearningRate() const override;
    void setLearningRate(float rate) override;
    
    // BMU-specific batch processing
    void addPattern(const std::vector<float>& prePattern, const std::vector<float>& postPattern);
    void computeBatchStatistics();
    void resetBatchData();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
