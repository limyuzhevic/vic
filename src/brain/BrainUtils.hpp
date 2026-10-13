#pragma once

#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

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