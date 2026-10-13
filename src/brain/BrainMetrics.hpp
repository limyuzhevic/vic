#pragma once

#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// ========== BRAIN METRICS DECLARATIONS ==========

namespace BrainMetrics {
    
    size_t getTotalNeuronCount(const Brain::Impl* impl);
    
    size_t getTotalSynapseCount(const Brain::Impl* impl);
    
    size_t getActiveNeuronCount(const Brain::Impl* impl);
    
    size_t getFiringNeuronCount(const Brain::Impl* impl);
    
    float getAverageFiringRate(const Brain::Impl* impl);
    
    float getExcitationInhibitionRatio(const Brain::Impl* impl);
    
    size_t getTotalSpikeCount(const Brain::Impl* impl);
    
    size_t getPendingSpikeEventCount(const Brain::Impl* impl);
    
    DevelopmentalStage getDevelopmentalStage(const Brain::Impl* impl);
}

// ========== BRAIN UTILS DECLARATIONS ==========

namespace BrainUtils {
    
    void createRegions(Brain::Impl* impl, size_t regionCount);
    
    void setupNeurons(Brain::Impl* impl, size_t neuronCount, size_t regionCount);
    
    void setupConnectivity(Brain::Impl* impl, size_t regionCount, float connectionProbability);
    
    void collectSensoryAndMotorNeurons(Brain::Impl* impl, size_t neuronsPerRegion, size_t regionIndex);
    
    void logBrainStatus(const Brain::Impl* impl);
    
    std::string formatLogMessage(const std::string& message);
    
    bool validateConfiguration(Brain::Impl* impl);
}

namespace BrainPerformance {
    
    float calculateConnectionDensity(const Brain::Impl* impl);
    
    size_t estimateMemoryUsage(const Brain::Impl* impl);
}

} // namespace nlm