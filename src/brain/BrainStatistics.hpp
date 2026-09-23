#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Forward declarations
class Brain;

class BrainStatistics {
public:
    // ===========================================
    // NEURAL STATISTICS
    // ===========================================
    
    static void logNeuralStatistics(const Brain* brain);
    static void logRegionStatistics(const Brain* brain);
    
    // ===========================================
    // MEMORY STATISTICS
    // ===========================================
    
    static void logMemoryStatistics(const Brain* brain);
    
    // ===========================================
    // NEUROMODULATION STATISTICS
    // ===========================================
    
    static void logNeuromodulationStatistics(const Brain* brain);
    
    // ===========================================
    // DEVELOPMENT STATISTICS
    // ===========================================
    
    static void logDevelopmentStatistics(const Brain* brain);
    
    // ===========================================
    // COMPLETE BRAIN STATUS LOG
    // ===========================================
    
    static void logCompleteBrainStatus(const Brain* brain);
};

} // namespace nlm