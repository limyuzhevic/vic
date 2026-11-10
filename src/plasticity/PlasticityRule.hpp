#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Synapse.hpp"
#include <string>
#include <memory>
#include <vector>
#include <map>

namespace nlm {

// === PLASTICITY RULE ABSTRACTION LAYER ===

// Plasticity rule type enum for easy identification
enum class PlasticityRuleType {
    Hebbian,
    STDP,
    AntiHebbian,
    BCM,
    RewardModulated,
    Developmental,
    Combined
};

// Plasticity rule metadata for introspection and configuration
struct PlasticityRuleMetadata {
    PlasticityRuleType type;
    std::string name;
    std::string description;
    float defaultLearningRate;
    bool requiresPreSynaptic;
    bool requiresPostSynaptic;
    
    PlasticityRuleMetadata(PlasticityRuleType t, const std::string& n, 
                          const std::string& desc, float rate = 0.01f,
                          bool pre = true, bool post = true)
        : type(t), name(n), description(desc), 
          defaultLearningRate(rate), requiresPreSynaptic(pre),
          requiresPostSynaptic(post) {}
};

// Abstract base class for all plasticity rules
// Provides common interface and lifecycle management
class PlasticityRule {
public:
    virtual ~PlasticityRule() = default;
    
    // Core plasticity operation
    virtual void update(Synapse* synapse, 
                        const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        TimestepDuration dt) = 0;
    
    // Apply weight change to synapse
    virtual void applyWeightChange(Synapse* synapse, SynapticWeight delta) = 0;
    
    // Get rule identification
    virtual const char* getName() const = 0;
    virtual PlasticityRuleType getType() const = 0;
    virtual const PlasticityRuleMetadata& getMetadata() const = 0;
    
    // Rule lifecycle management
    bool isEnabled() const { return enabled_; }
    void setEnabled(bool enabled) { enabled_ = enabled; }
    virtual void configure(const std::map<std::string, float>& parameters) {}
    
    // Rule interaction and combination
    virtual void addPrerequisite(std::shared_ptr<PlasticityRule> rule) {}
    virtual void addModifier(std::shared_ptr<PlasticityRule> rule) {}
    virtual std cooksPyramid;
    
    // Statistics and diagnostics
    virtual size_t getUpdateCount() const { return updateCount_; }
    virtual float getLearningEffectiveness() const { return learningEffectiveness_; }
    
protected:
    PlasticityRule() : enabled_(true), updateCount_(0), learningEffectiveness_(0.0f) {}
    
    void recordUpdate(float effectiveness = 1.0f) {
        ++updateCount_;
        // Update running average of effectiveness
        learningEffectiveness_ = (learningEffectiveness_ * (updateCount_ - 1) + effectiveness) / updateCount_;
    }
    
    void setLearningEffectiveness(float effectiveness) { learningEffectiveness_ = effectiveness; }
    
private:
    bool enabled_;
    size_t updateCount_;
    float learningEffectiveness_;
};

// === CONCRETE PLASTICITY RULE IMPLEMENTATIONS ===

// Hebbian plasticity rule: "neurons that fire together, wire together"
// Enhanced implementation with covariance learning and activity-dependent scaling
class HebbianPlasticityRule : public PlasticityRule {
public:
    HebbianPlasticityRule();
    ~HebbianPlasticityRule() override;
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override { return "Hebbian"; }
    PlasticityRuleType getType() const override { return PlasticityRuleType::Hebbian; }
    const PlasticityRuleMetadata& getMetadata() const override;
    
    // Enhanced parameters
    void setLearningRate(float rate) { learningRate_ = rate; }
    float getLearningRate() const { return learningRate_; }
    void setMaxWeight(float max) { maxWeight_ = max; }
    float getMaxWeight() const { return maxWeight_; }
    void setMinWeight(float min) { minWeight_ = min; }
    float getMinWeight() const { return minWeight_; }
    void setCovarianceBaseline(float baseline) { covarianceBaseline_ = baseline; }
    float getCovarianceBaseline() const { return covarianceBaseline_; }
    
    // Hebbian-specific operation
    float getCovariance(const std::vector<Timestamp>& preSpikes, 
                       const std::vector<Timestamp>& postSpikes) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
    
    static const PlasticityRuleMetadata metadata_;
};

// Spike-Timing-Dependent Plasticity (STDP) rule
// Real biological implementation based on spike timing correlations
class STDPPlasticityRule : public PlasticityRule {
public:
    STDPPlasticityRule();
    ~STDPPlasticityRule() override;
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override { return "STDP"; }
    PlasticityRuleType getType() const override { return PlasticityRuleType::STDP; }
    const PlasticityRuleMetadata& getMetadata() const override;
    
    // STDP-specific parameters
    void configure(const std::map<std::string, float>& parameters) override;
    void setLTPWeight(float weight) { ltpWeight_ = weight; }
    float getLTPWeight() const { return ltpWeight_; }
    void setLTDWeight(float weight) { ltdWeight_ = weight; }
    float getLTDWeight() const { return ltdWeight_; }
    void setTimeConstant(float tau) { timeConstant_ = tau; }
    float getTimeConstant() const { return timeConstant_; }
    
    // STDP curve analysis
    float calculateWeightChange(float dtMs) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
    
    static const PlasticityRuleMetadata metadata_;
};

// Anti-Hebbian plasticity rule: "neurons that fire together, unwire together"
// Implements depression-based learning for stability
class AntiHebbianPlasticityRule : public PlasticityRule {
public:
    AntiHebbianPlasticityRule();
    ~AntiHebbianPlasticityRule() override;
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override { return "AntiHebbian"; }
    PlasticityRuleType getType() const override { return PlasticityRuleType::AntiHebbian; }
    const PlasticityRuleMetadata& getMetadata() const override;
    
    // Anti-Hebbian specific parameters
    void setDepressionRate(float rate) { depressionRate_ = rate; }
    float getDepressionRate() const { return depressionRate_; }
    void setMaxDepression(float max) { maxDepression_ = max; }
    float getMaxDepression() const { return maxDepression_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
    
    static const PlasticityRuleMetadata metadata_;
};

// Bienenstock-Cooper-Munro (BCM) theory rule
// Threshold-based learning with sliding threshold
class BCMPlasticityRule : public PlasticityRule {
public:
    BCMPlasticityRule();
    ~BCMPlasticityRule() override;
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override { return "BCM"; }
    PlasticityRuleType getType() const override { return PlasticityRuleType::BCM; }
    const PlasticityRuleMetadata& getMetadata() const override;
    
    // BCM-specific parameters
    void configure(const std::map<std::string, float>& parameters) override;
    void setThreshold(float theta) { threshold_ = theta; }
    float getThreshold() const { return threshold_; }
    void setSlidingThresholdRate(float rate) { slidingThresholdRate_ = rate; }
    float getSlidingThresholdRate() const { return slidingThresholdRate_; }
    void setLearningRate(float rate) { learningRate_ = rate; }
    float getLearningRate() const { return learningRate_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
    
    static const PlasticityRuleMetadata metadata_;
};

// Plasticity rule factory for easy creation and management
class PlasticityRuleFactory {
public:
    static std::shared_ptr<PlasticityRule> createRule(PlasticityRuleType type);
    static std::shared_ptr<PlasticityRule> createRule(const std::string& name);
    static std::vector<PlasticityRuleType> getAvailableRuleTypes();
    static PlasticityRuleMetadata getRuleMetadata(PlasticityRuleType type);
    
    // Rule composition for advanced learning
    static std::shared_ptr<PlasticityRule> createCombinedRule(
        const std::vector<PlasticityRuleType>& ruleTypes);
    
    // Configuration presets
    static std::map<std::string, std::map<std::string, float>> getPresets();
    static std::shared_ptr<PlasticityRule> createFromPreset(const std::string& presetName);
};

// Convenience typedefs for common usage
using PlasticityRulePtr = std::shared_ptr<PlasticityRule>;
using PlasticityRuleConstPtr = std::shared_ptr<const PlasticityRule>;

} // namespace nlm