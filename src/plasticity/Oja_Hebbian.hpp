#pragma once

#include "Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <vector>

namespace nlm {

class OjaHebbian : public Hebbian {
public:
    OjaHebbian();
    ~OjaHebbian() override;
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    
    // Oja parameters
    void setStability(float stability);  // Stability parameter (lambda)
    float getStability() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
