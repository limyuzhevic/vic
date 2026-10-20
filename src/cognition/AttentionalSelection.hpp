#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>

namespace nlm {

class AttentionalSelection {
public:
    AttentionalSelection();
    ~AttentionalSelection();
    
    bool initialize(class Brain* brain);
    void update(TimestepDuration timestep);
    void reset();
    
    // Process competition among memory traces
    void processCompetition(const std::vector<NeuronId>& competitors);
    
    // Configuration
    void setInhibitionStrength(float strength) { inhibitionStrength = strength; }
    void setExcitationStrength(float strength) { excitationStrength = strength; }
    
    // Statistics
    size_t getActiveSelections() const;
    
private:
    struct Impl;
    Impl* pImpl;
    
    float inhibitionStrength;
    float excitationStrength;
};

} // namespace nlm