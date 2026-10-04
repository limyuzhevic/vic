#pragma once

#include "Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <vector>

namespace nlm {

class AntiHebbian : public Hebbian {
public:
    AntiHebbian();
    ~AntiHebbian() override;
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    
    // Anti-Hebbian parameters
    void setDepressionRate(float rate);   // Depression rate (mu)
    float getDepressionRate() const;
    void setStabilizer(float stabilizer); // Stabilizer term (sigma)
    float getStabilizer() const;
    
    // Variant types
    enum AntiHebbianType {
        SIMPLE_ANTIHEBBIAN,      // Simple anti-correlation
        THRESHOLD_ANTIHEBBIAN,   // Threshold-based
        NORMALIZED_ANTIHEBBIAN,  // Normalized version
        INTERGRANIC_ANTIHEBBIAN  // Inter-granic variant
    };
    
    void setAntiHebbianType(AntiHebbianType type);
    AntiHebbianType getAntiHebbianType() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
