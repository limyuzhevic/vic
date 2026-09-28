#pragma once

#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Statistics and Logging Integration
// Handles brain statistics collection and logging

class BrainStatistics {
public:
    // Log brain status and statistics
    void logBrainStatus(const Brain::Impl& pImpl) const;
    
    // Get neural population statistics
    void logNeuralStats(const Brain::Impl& pImpl) const;
    
    // Get synaptic statistics
    void logSynapticStats(const Brain::Impl& pImpl) const;
    
    // Get region statistics
    void logRegionStats(const Brain::Impl& pImpl) const;
    
    // Get spike statistics
    void logSpikeStats(const Brain::Impl& pImpl) const;
    
    // Get E/I ratio statistics
    void logEIRatioStats(const Brain::Impl& pImpl) const;
    
private:
    // Helper functions for statistics logging
    void collectAndLogNeuralStats(const Brain::Impl& pImpl) const;
    void collectAndLogSynapticStats(const Brain::Impl& pImpl) const;
    void collectAndLogRegionStats(const Brain::Impl& pImpl) const;
    void collectAndLogSpikeStats(const Brain::Impl& pImpl) const;
    void collectAndLogEIRatioStats(const Brain::Impl& pImpl) const;
    void collectAndLogMemoryStats(const Brain::Impl& pImpl) const;
};

} // namespace nlm
