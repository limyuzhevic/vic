#pragma once

#include "Hebbian.hpp"

namespace nlm {

class HebbianRule {
public:
    HebbianRule();
    ~HebbianRule();
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt);
    void applyWeightChange(Synapse* synapse, SynapticWeight delta);
    const char* getName() const;
    
    // Parameters
    void setLearningRate(float rate);
    float getLearningRate() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm