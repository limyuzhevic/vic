// StatsCollector.hpp - Extracted statistics collection from Brain
#pragma once

#include "../core/Types/Types.hpp"
#include <memory>
#include <vector>

namespace nlm {

// Forward declarations
class Brain;
class NeuralRegion;

// Statistics collection for neural network analysis
class StatsCollector {
public:
    StatsCollector();
    
    // Collect comprehensive statistics from brain
    struct BrainStats {
        // Basic network statistics
        size_t totalNeurons = 0;
        size_t totalSynapses = 0;
        size_t activeNeurons = 0;
        size_t firingNeurons = 0;
        
        // Firing dynamics
        size_t totalSpikes = 0;
        size_t pendingSpikes = 0;
        float averageFiringRate = 0.0f;
        float excitationInhibitionRatio = 0.0f;
        
        // Network health metrics
        float meanMembranePotential = 0.0f;
        float varianceMembranePotential = 0.0f;
        float meanSynapticWeight = 0.0f;
        float synapticDensity = 0.0f;
        
        // Regional breakdown
        struct RegionStats {
            std::string name;
            size_t neuronCount = 0;
            size_t synapseCount = 0;
            float firingRate = 0.0f;
            float weightMean = 0.0f;
        };
        
        std::vector<RegionStats> regionStats;
        
        // Performance metrics
        double simulationTime = 0.0;
        size_t stepsCompleted = 0;
        double performanceFPS = 0.0;
        
        // Memory usage
        size_t memoryUsageBytes = 0;
        size_t checkpointSizeBytes = 0;
        
        // Quality metrics
        float stabilityScore = 0.0f;  // 0.0 to 1.0, higher is more stable
        float learningScore = 0.0f;   // 0.0 to 1.0, higher indicates more learning
        float coherenceScore = 0.0f;  // 0.0 to 1.0, network synchronization
    };
    
    // Collect statistics from brain
    static BrainStats collect(const Brain* brain);
    
    // Collect statistics from specific region
    static BrainStats::RegionStats collectRegionStats(const NeuralRegion* region);
    
    // Format statistics as string
    static std::string formatStats(const BrainStats& stats, const std::string& title = "Brain Statistics");
    
    // Format region statistics
    static std::string formatRegionStats(const BrainStats::RegionStats& stats);
    
    // Calculate derived metrics
    static float calculateStability(const BrainStats& stats);
    static float calculateLearning(const BrainStats& stats, const BrainStats& previousStats);
    static float calculateCoherence(const BrainStats& stats);
    
    // Validate statistics
    static bool validateStats(const BrainStats& stats);
    
    // Export statistics to formats
    static std::string exportToJSON(const BrainStats& stats);
    static std::string exportToCSV(const BrainStats& stats);
    static std::string exportToMarkdown(const BrainStats& stats);
};

} // namespace nlm
