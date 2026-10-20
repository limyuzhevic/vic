#pragma once

#include "PlasticityRule.hpp"
#include <vector>
#include "../core/Types/Types.hpp"

namespace nlm {

// Hebbian learning implementation with advanced variants
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
    
    // Configuration methods
    void setLearningRate(float rate);
    float getLearningRate() const;
    void setMaxWeight(float maxWeight);
    float getMaxWeight() const;
    
    // Advanced rule selection and configuration
    void setLearningRule(HebbianRuleType ruleType, float learningRate);
    void configureAdvanced(HebbianRuleType ruleType, float learningRate,
                           bool useNormalization, float competitionStrength,
                           float metaplasticDecay);
    
    // Specialized Hebbian rule variants
    void enableMetaplasticity(bool enable, float decayRate);
    void enableCompetitiveLearning(bool enable, float strength);
    void enableOjaNormalization(bool enable);
    void enableHardLimit(bool enable);
    
    // Activity history and state management
    float getBaselineActivity() const;
    void resetActivityHistory();
    
    // Helper methods for advanced rule implementations
    float applyAdvancedCovarianceRule(Synapse* synapse,
                                       const std::vector<Timestamp>& preSpikes,
                                       const std::vector<Timestamp>& postSpikes,
                                       TimestepDuration dt);
    float applyOjaHebbian(Synapse* synapse,
                          const std::vector<Timestamp>& preSpikes,
                          const std::vector<Timestamp>& postSpikes,
                          TimestepDuration dt);
    float applyCompetitiveHebbian(Synapse* synapse,
                                  const std::vector<Timestamp>& preSpikes,
                                  const std::vector<Timestamp>& postSpikes,
                                  TimestepDuration dt);
    float applyMetaplasticHebbian(Synapse* synapse,
                                  const std::vector<Timestamp>& preSpikes,
                                  const std::vector<Timestamp>& postSpikes,
                                  TimestepDuration dt);
    float applyHardLimitHebbian(Synapse* synapse,
                                const std::vector<Timestamp>& preSpikes,
                                const std::vector<Timestamp>& postSpikes,
                                TimestepDuration dt);
    
    const char* getRuleName(HebbianRuleType ruleType) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Advanced Hebbian rule types
enum class HebbianRuleType {
    BASIC,
    COVARIANCE,
    OJA,
    COMPETITIVE,
    METAPLASTIC,
    HARD_LIMIT
};

// Advanced rule helpers
class AdvancedCovarianceHebbian;
class OjaHebbian;
class CompetitiveHebbian;
class MetaplasticHebbian;
class HardLimitHebbian;

} // namespace nlm
