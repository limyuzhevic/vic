#pragma once

#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {

class Brain::Impl;

class BrainStatistics {
public:
    BrainStatistics(Brain::Impl* pImpl);
    
    float performExcitationInhibitionRatio() const;
    size_t performTotalSpikeCount() const;
    size_t performPendingSpikeEventCount() const;
    size_t performTotalNeuronCount() const;
    size_t performTotalSynapseCount() const;
    size_t performActiveNeuronCount() const;
    size_t performFiringNeuronCount() const;
    float performAverageFiringRate() const;
    void performLogStatus() const;
    
private:
    Brain::Impl* pImpl;
};

} // namespace nlm