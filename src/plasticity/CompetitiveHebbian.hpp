#pragma once

#include "PlasticityRule.hpp"
#include <vector>
#include <algorithm>

namespace nlm {

// Competitive Hebbian learning rule
// Implements competition among synapses to the same neuron, promoting selectivity
class CompetitiveHebbian : public PlasticityRule {
public:
    CompetitiveHebbian();
    ~CompetitiveHebbian() override;

    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;

    // Competitive Hebbian parameters
    void setLearningRate(float rate);                    // Learning rate (η)
    float getLearningRate() const;
    void setCompetitionFactor(float factor);              // Competition strength (α)
    float getCompetitionFactor() const;
    void setMaxWeight(float max);                         // Maximum weight bound
    float getMaxWeight() const;
    void setMinWeight(float min);                         // Minimum weight bound
    float getMinWeight() const;

    // Competition implementation details
    void updateSynapseWeights(std::vector<Synapse*>& synapses, float postActivity, float baseline);  // Update weights for all synapses competing for the same post-synaptic neuron
    float calculateCompetitionScore(float currentWeight, float targetActivity) const;             // Calculate competition score for weight adjustment
    void resetActivityHistory();                               // Reset activity history for new competition cycle
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm