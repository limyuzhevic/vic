// Simple visualization and debugging utilities
#pragma once

#include <string>
#include "core/Logger/Logger.hpp"

namespace nlm {

class DebugVisualizer {
public:
    // Print brain state summary
    static std::string brainStateSummary(const class Brain* brain);
    
    // Print neuron activity heatmap
    static std::string neuronActivityHeatmap(const class Brain* brain, int width = 40, int height = 20);
    
    // Print synaptic weight distribution
    static std::string synapticWeightDistribution(const class Brain* brain, int numBins = 10);
    
    // Print spike activity over time
    static std::string spikeActivityTimeline(const class Brain* brain, size_t historyLength = 100);
    
    // Print development stage information
    static std::string developmentalStateInfo(const class Brain* brain);
    
    // Print comprehensive debug report
    static std::string comprehensiveDebugReport(const class Brain* brain);
    
    // Print to console
    static void printToConsole(const std::string& message);
    
    // Log to file
    static void logToFile(const std::string& filename, const std::string& message);
};

} // namespace nlm
