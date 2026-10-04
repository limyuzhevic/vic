#pragma once

#include "Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <vector>

namespace nlm {

class BCMHebbian : public Hebbian {
public:
    BCMHebbian();
    ~BCMHebbian() override;
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    
    // BCM parameters
    void setBCMThreshold(float threshold);    // BCM threshold (theta_m)
    float getBCMThreshold() const;
    void setBCMLearningRate(float rate);      // BCM learning rate (eta)
    float getBCMLearningRate() const;
    void setBCMRegularization(float reg);     // BCM regularization parameter (lambda)
    float getBCMRegularization() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
