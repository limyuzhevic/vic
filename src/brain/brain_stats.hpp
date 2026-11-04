#pragma once

#include "../core/Types/Types.hpp"
#include <string>

namespace nlm {

// Statistics and monitoring
class BrainStats {
public:
    BrainStats() = default;
    ~BrainStats() = default;
    
    // Calculate excitation/inhibition ratio
    float calculateExcitationInhibitionRatio(
        const std::vector<std::unique_ptr<class NeuralRegion>>& regions) const;
    
    // Calculate average firing rate
    float calculateAverageFiringRate(
        const std::vector<std::unique_ptr<class NeuralRegion>>& regions) const;
    
    // Get active neuron count
    size_t getActiveNeuronCount(
        const std::vector<std::unique_ptr<class NeuralRegion>>& regions) const;
    
    // Get firing neuron count
    size_t getFiringNeuronCount(
        const std::vector<Neuron*>& motorNeurons) const;
    
    // Generate status log
    std::string generateStatusLog(size_t totalSpikesTotal,
                                 size_t totalSpikesThisStep,
                                 size_t totalNeuronCount,
                                 size_t totalSynapseCount,
                                 size_t regionCount,
                                 size_t sensoryNeuronsCount,
                                 size_t motorNeuronsCount,
                                 const DevelopmentalStage& developmentalStage) const;
    
    // Get statistics summary
    std::string getStatisticsSummary(size_t totalSpikesTotal,
                                    size_t totalSpikesThisStep,
                                    size_t totalNeuronCount,
                                    size_t totalSynapseCount) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
