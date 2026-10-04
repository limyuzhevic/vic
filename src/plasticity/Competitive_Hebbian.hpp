#pragma once

#include "Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <vector>
#include <map>

namespace nlm {

class CompetitiveHebbian : public Hebbian {
public:
    CompetitiveHebbian();
    ~CompetitiveHebbian() override;
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    
    // Competitive parameters
    void setCompetitionStrength(float strength);  // Competition strength (alpha)
    float getCompetitionStrength() const;
    void setLearningRate(float rate);              // Learning rate (eta)
    float getLearningRate() const;
    
    // Competition group management
    void addToCompetitionGroup(NeuronId neuron);   // Add neuron to competition group
    void removeFromCompetitionGroup(NeuronId neuron); // Remove neuron from competition group
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
