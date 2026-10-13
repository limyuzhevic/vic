#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Synapse.hpp"

namespace nlm {

// Abstract base class for plasticity rules
// Enhanced implementation with logging and error handling

class PlasticityRule {
public:
    virtual ~PlasticityRule() = default;
    
    // Update synaptic weights based on pre/post synaptic activity
    // IMPLEMENTED: Real spike-timing dependent plasticity
    virtual void update(Synapse* synapse, 
                        const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        TimestepDuration dt) = 0;
    
    // Apply weight change
    virtual void applyWeightChange(Synapse* synapse, SynapticWeight delta) = 0;
    
    // Get rule name
    virtual const char* getName() const = 0;
    
    // Check if rule is enabled
    bool isEnabled() const;
    void setEnabled(bool enabled);
    
    // Configure rule from parameters
    virtual void configure(const std::string& configPrefix = "") = 0;
    
    // Get rule description
    virtual std::string getDescription() const = 0;
    
protected:
    PlasticityRule() : enabled_(true) {}
    
private:
    bool enabled_;
};

// Enhanced Hebbian plasticity rule: "neurons that fire together, wire together"
// IMPLEMENTED: Real Hebbian learning with timing precision and homeostasis
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
    
    // Parameters
    void setLearningRate(float rate);
    float getLearningRate() const;
    
    // Advanced features
    void setTimeWindow(float window);
    float getTimeWindow() const;
    void setHomeostaticFactor(float factor);
    float getHomeostaticFactor() const;
    
    // Configuration
    void configure(const std::string& configPrefix = "") override;
    std::string getDescription() const override;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Anti-Hebbian rule: decrease weight when neurons fire together
// IMPLEMENTED: Real anti-Hebbian learning for balanced networks
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
    
    void setLearningRate(float rate);
    float getLearningRate() const;
    
    // Configuration
    void configure(const std::string& configPrefix = "") override;
    std::string getDescription() const override;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Bienenstock-Cooper-Munro (BCM) rule with adaptive threshold
// IMPLEMENTED: Real BCM rule for stability-plasticity balance
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
    
    // Configuration
    void configure(const std::string& configPrefix = "") override;
    std::string getDescription() const override;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
