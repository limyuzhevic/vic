// Implementation of performance and debugging tools for Python bindings

#include "include/nlm_performance_debug.h"
#include "src/brain/Brain.hpp"
#include "src/brain/Neuron.hpp"
#include "src/brain/Synapse.hpp"
#include "src/brain/NeuralPopulation.hpp"
#include "src/brain/NeuralRegion.hpp"
#include <sstream>
#include <iomanip>
#include <cmath>

// Implementation of visualization utilities
namespace performance {
    std::string VisualizationTools::createPerformanceChart(const PerformanceStats& stats) {
        std::stringstream ss;
        ss << "PERFORMANCE CHART\n";
        ss << "==================\n\n";
        
        auto times = stats.getOperationTimes();
        if (times.empty()) {
            ss << "No performance data available.\n";
            return ss.str();
        }
        
        double maxTime = 0.0;
        for (const auto& pair : times) {
            if (!pair.second.empty()) {
                double opMax = *std::max_element(pair.second.begin(), pair.second.end());
                maxTime = std::max(maxTime, opMax);
            }
        }
        
        ss << "Operation Statistics:\n";
        for (const auto& pair : times) {
            if (!pair.second.empty()) {
                double avg = stats.getAverage(pair.first);
                double max = *std::max_element(pair.second.begin(), pair.second.end());
                double min = *std::min_element(pair.second.begin(), pair.second.end());
                ss << "  " << std::left << std::setw(20) << pair.first << ": "
                   << "avg=" << std::fixed << std::setprecision(3) << avg << "ms, "
                   << "min=" << min << "ms, "
                   << "max=" << max << "ms, "
                   << "count=" << pair.second.size() << "\n";
            }
        }
        
        ss << "\nOverall Statistics:\n";
        ss << "  Total Operations: " << stats.getTotalOps() << "\n";
        ss << "  Total Time: " << std::fixed << std::setprecision(3) << stats.getTotalTime() << "ms\n";
        ss << "  Average Time: " << std::fixed << std::setprecision(3) << stats.getOverallAverage() << "ms\n";
        
        return ss.str();
    }
    
    std::string VisualizationTools::createMemoryUsageChart(const Brain& brain) {
        std::stringstream ss;
        ss << "MEMORY USAGE CHART\n";
        ss << "===================\n\n";
        
        ss << "Brain Memory Statistics:\n";
        ss << "  Total Neurons: " << brain.getTotalNeuronCount() << "\n";
        ss << "  Total Synapses: " << brain.getTotalSynapseCount() << "\n";
        ss << "  Active Neurons: " << brain.getActiveNeuronCount() << "\n";
        ss << "  Firing Neurons: " << brain.getFiringNeuronCount() << "\n";
        ss << "  Total Spike Count: " << brain.getTotalSpikeCount() << "\n";
        
        // Estimate memory usage
        size_t neuronMemory = brain.getTotalNeuronCount() * sizeof(nlm::Neuron);
        size_t synapseMemory = brain.getTotalSynapseCount() * sizeof(nlm::Synapse);
        
        ss << "\nEstimated Memory Usage:\n";
        ss << "  Neurons: " << (neuronMemory / 1024) << " KB\n";
        ss << "  Synapses: " << (synapseMemory / 1024) << " KB\n";
        ss << "  Total: " << ((neuronMemory + synapseMemory) / 1024) << " KB\n";
        
        return ss.str();
    }
    
    std::string VisualizationTools::createNetworkActivityGraph(const Brain& brain) {
        std::stringstream ss;
        ss << "NETWORK ACTIVITY GRAPH\n";
        ss << "========================\n\n";
        
        ss << "Activity Metrics:\n";
        ss << "  Firing Rate: " << std::fixed << std::setprecision(3) 
           << brain.getAverageFiringRate() << " spikes/ms\n";
        ss << "  Excitatory/Inhibitory Ratio: " 
           << std::fixed << std::setprecision(3) 
           << brain.getExcitationInhibitionRatio() << ":1\n";
        ss << "  Active Neuron Percentage: " << std::fixed << std::setprecision(1)
           << (brain.getActiveNeuronCount() * 100.0 / std::max(1.0, (double)brain.getTotalNeuronCount()))
           << "%\n";
        
        return ss.str();
    }
}

// Implementation of debug utilities
namespace debug {
    std::vector<uint64_t> MemoryInspector::getNeuronMemoryUsage(const Brain& brain) {
        std::vector<uint64_t> memoryUsage;
        
        // Get all regions
        const auto& regions = brain.getRegions();
        for (const auto& regionPtr : regions) {
            if (regionPtr) {
                const auto& populations = regionPtr->getPopulations();
                for (const auto& popPtr : populations) {
                    if (popPtr) {
                        // Estimate memory per neuron in this population
                        size_t neuronMemory = popPtr->size() * sizeof(nlm::Neuron);
                        memoryUsage.push_back(neuronMemory);
                    }
                }
            }
        }
        
        return memoryUsage;
    }
    
    std::vector<uint64_t> MemoryInspector::getSynapseMemoryUsage(const Brain& brain) {
        std::vector<uint64_t> memoryUsage;
        
        // Get synapse count from regions
        const auto& regions = brain.getRegions();
        for (const auto& regionPtr : regions) {
            if (regionPtr) {
                // Estimate memory per synapse in this region
                size_t synapseMemory = regionPtr->getPopulationCount() * 100 * sizeof(nlm::Synapse);
                memoryUsage.push_back(synapseMemory);
            }
        }
        
        return memoryUsage;
    }
    
    std::vector<float> MemoryInspector::getMemoryHeatMap(const Brain& brain) {
        std::vector<float> heatMap;
        
        // Get all regions
        const auto& regions = brain.getRegions();
        for (const auto& regionPtr : regions) {
            if (regionPtr) {
                // Calculate activity-based heat for each region
                const auto& populations = regionPtr->getPopulations();
                for (const auto& popPtr : populations) {
                    if (popPtr) {
                        float activity = static_cast<float>(popPtr->getFiringCount()) / 
                                       std::max(1.0f, (float)popPtr->size());
                        heatMap.push_back(activity);
                    }
                }
            }
        }
        
        return heatMap;
    }
}

// Legacy backward compatibility functions (deprecated but still available)
namespace legacy {
    void deprecatedPerformanceFeatures() {
        // Empty implementation for backward compatibility
    }
}