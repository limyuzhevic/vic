#pragma once

#include "Brain.hpp"
#include <string>

namespace nlm {

class BrainStatistics {
public:
    BrainStatistics(std::shared_ptr<Brain> brain);
    
    // Calculate excitation/inhibition ratio
    float calculateExcitationInhibitionRatio() const;
    
    // Get total spike count
    size_t getTotalSpikeCount() const { return totalSpikeCount; }
    
    // Get firing statistics
    size_t getFiringNeuronCount() const { return firingNeuronCount; }
    float getAverageFiringRate() const { return averageFiringRate; }
    
    // Get neural population statistics
    size_t getTotalNeuronCount() const { return totalNeuronCount; }
    size_t getTotalSynapseCount() const { return totalSynapseCount; }
    size_t getActiveNeuronCount() const { return activeNeuronCount; }
    
    // Get developmental statistics
    DevelopmentalStage getDevelopmentalStage() const { return developmentalStage; }
    
    // Generate comprehensive status report
    std::string generateStatusReport() const;
    
    // Reset statistics
    void reset() {
        totalSpikeCount = 0;
        firingNeuronCount = 0;
        averageFiringRate = 0.0f;
        totalNeuronCount = 0;
        totalSynapseCount = 0;
        activeNeuronCount = 0;
        developmentalStage = DevelopmentalStage::Initial;
    }
    
private:
    std::shared_ptr<Brain> brain;
    
    // Cached statistics
    size_t totalSpikeCount;
    size_t firingNeuronCount;
    float averageFiringRate;
    size_t totalNeuronCount;
    size_t totalSynapseCount;
    size_t activeNeuronCount;
    DevelopmentalStage developmentalStage;
    
    // Helper methods
    size_t calculateActiveNeuronCount() const;
    float calculateAverageFiringRate() const;
};

} // namespace nlm