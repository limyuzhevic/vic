#pragma once

#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <map>

namespace nlm {

class HomeostaticSTDP : public STDP {
public:
    HomeostaticSTDP();
    ~HomeostaticSTDP() override;
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    
    // Homeostatic parameters
    void setTargetActivity(float target);     // Target firing rate
    float getTargetActivity() const;
    void setHomeostaticRate(float rate);      // Homeostatic learning rate
    float getHomeostaticRate() const;
    void setHomeostaticTimeConstant(float tau); // Homeostatic time constant
    float getHomeostaticTimeConstant() const;
    
    // Metaplasticity - modulate plasticity itself
    void setMetaplasticityFactor(float factor); // Metaplasticity modulation
    float getMetaplasticityFactor() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
