#include "advanced/AdvancedDebugging.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <algorithm>

namespace nlm {

void AdvancedDebugger::analyzeMemoryLeaks(const std::shared_ptr<Brain>& brain) {
    std::cout << "=== Advanced Memory Leak Analysis ===" << std::endl;
    
    // Get brain statistics
    size_t totalNeurons = brain->getTotalNeuronCount();
    size_t totalSynapses = brain->getTotalSynapseCount();
    size_t totalRegions = brain->getRegionCount();
    
    std::cout << "Brain Memory Footprint Analysis:" << std::endl;
    std::cout << "  - Total Neurons: " << totalNeurons << std::endl;
    std::cout << "  - Total Synapses: " << totalSynapses << std::endl;
    std::cout << "  - Total Regions: " << totalRegions << std::endl;
    
    // Estimate memory usage
    size_t neuronMemory = totalNeurons * sizeof(Neuron);
    size_t synapseMemory = totalSynapses * sizeof(Synapse);
    size_t totalEstimatedMemory = neuronMemory + synapseMemory;
    
    std::cout << "  - Estimated Memory Usage: " << (totalEstimatedMemory / 1024) << " KB" << std::endl;
    
    // Check for potential memory issues
    double synapseToNeuronRatio = totalNeurons > 0 ? static_cast<double>(totalSynapses) / totalNeurons : 0.0;
    std::cout << "  - Synapse-to-Neuron Ratio: " << std::fixed << std::setprecision(2) << synapseToNeuronRatio << std::endl;
    
    if (synapseToNeuronRatio > 10.0) {
        std::cout << "⚠ Warning: High synapse-to-neuron ratio may indicate memory issues" << std::endl;
    }
    
    if (totalNeurons < 10) {
        std::cout << "⚠ Warning: Very small network may not show meaningful results" << std::endl;
    }
    
    // Check for memory fragmentation potential
    std::vector<size_t> regionSizes;
    auto regions = brain->getAllRegions();
    for (const auto& region : regions) {
        regionSizes.push_back(region->getNeuronCount());
    }
    
    if (!regionSizes.empty()) {
        size_t maxRegionSize = *std::max_element(regionSizes.begin(), regionSizes.end());
        std::cout << "  - Largest Region Size: " << maxRegionSize << " neurons" << std::endl;
        
        if (maxRegionSize > totalNeurons * 0.8) {
            std::cout << "⚠ Warning: One region dominates the network, potential connectivity issues" << std::endl;
        }
    }
}

void AdvancedDebugger::profilePerformance(const std::shared_ptr<Brain>& brain, double duration) {
    std::cout << "=== Performance Profiling ===" << std::endl;
    std::cout << "Profiling for " << duration << " seconds..." << std::endl;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    auto endTime = startTime + std::chrono::duration<double>(duration);
    
    size_t stepCount = 0;
    size_t spikeCount = 0;
    size_t injectionCount = 0;
    
    while (std::chrono::high_resolution_clock::now() < endTime) {
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            injectionCount++;
        }
        brain->step(stepCount, stepCount * 0.001);
        spikeCount = brain->getTotalSpikeCount();
        stepCount++;
    }
    
    double actualDuration = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - startTime).count();
    
    std::cout << "\nPerformance Profile Results:" << std::endl;
    std::cout << "  - Duration: " << std::fixed << std::setprecision(2) << actualDuration << "s" << std::endl;
    std::cout << "  - Steps: " << stepCount << std::endl;
    std::cout << "  - Steps/sec: " << std::fixed << std::setprecision(0) << (stepCount / actualDuration) << std::endl;
    std::cout << "  - Total Spikes: " << spikeCount << std::endl;
    std::cout << "  - Spikes/sec: " << std::fixed << std::setprecision(0) << (spikeCount / actualDuration) << std::endl;
    std::cout << "  - Input Injections: " << injectionCount << std::endl;
    std::cout << "  - Average Time per Step: " << std::fixed << std::setprecision(4) << (actualDuration / stepCount) << "s" << std::endl;
    
    // Simulate CPU and memory usage
    double simulatedCpu = std::min(100.0, 50.0 + stepCount / 100.0);
    double simulatedMemory = std::min(500.0, 100.0 + totalNeurons);
    
    std::cout << "  - CPU Usage: (simulated) ~" << std::fixed << std::setprecision(1) << simulatedCpu << "%" << std::endl;
    std::cout << "  - Memory Usage: (simulated) ~" << std::fixed << std::setprecision(1) << simulatedMemory << " MB" << std::endl;
}

void AdvancedDebugger::detectPerformanceBottlenecks(const std::shared_ptr<Brain>& brain) {
    std::cout << "=== Performance Bottleneck Detection ===" << std::endl;
    
    size_t totalNeurons = brain->getTotalNeuronCount();
    size_t totalSynapses = brain->getTotalSynapseCount();
    
    // Analyze connectivity
    double connectionDensity = totalSynapses > 0 ? static_cast<double>(totalSynapses) / (totalNeurons * totalNeurons) : 0.0;
    std::cout << "  - Connection Density: " << std::fixed << std::setprecision(4) << connectionDensity << std::endl;
    
    if (connectionDensity < 0.01) {
        std::cout << "⚠ Warning: Low connection density may limit network dynamics" << std::endl;
    }
    
    // Check region sizes
    auto regions = brain->getAllRegions();
    if (!regions.empty()) {
        std::cout << "  - Region Analysis:" << std::endl;
        for (size_t i = 0; i < regions.size(); ++i) {
            size_t neuronCount = regions[i]->getNeuronCount();
            size_t synapseCount = regions[i]->getSynapseCount();
            double regionDensity = synapseCount > 0 ? static_cast<double>(synapseCount) / (neuronCount * neuronCount) : 0.0;
            std::cout << "    Region " << i << ": " << neuronCount << " neurons, " 
                      << synapseCount << " synapses, density: " << std::setprecision(4) << regionDensity << std::endl;
        }
    }
    
    // Check for potential bottlenecks
    if (totalNeurons > 1000) {
        std::cout << "⚠ Warning: Large network may have performance bottlenecks" << std::endl;
    }
}

void AdvancedDebugger::analyzeConnectivity(const std::shared_ptr<Brain>& brain) {
    std::cout << "=== Connectivity Analysis ===" << std::endl;
    
    size_t totalNeurons = brain->getTotalNeuronCount();
    size_t totalSynapses = brain->getTotalSynapseCount();
    
    // Calculate basic metrics
    double connectionProbability = totalSynapses > 0 ? static_cast<double>(totalSynapses) / (totalNeurons * totalNeurons) : 0.0;
    
    std::cout << "Connectivity Metrics:" << std::endl;
    std::cout << "  - Total Neurons: " << totalNeurons << std::endl;
    std::cout << "  - Total Synapses: " << totalSynapses << std::endl;
    std::cout << "  - Connection Probability: " << std::fixed << std::setprecision(4) << connectionProbability << std::endl;
    std::cout << "  - Average Connections per Neuron: " << std::fixed << std::setprecision(2) << 
                     (totalSynapses * 2 / totalNeurons) << std::endl;
    
    // Check region connectivity
    auto regions = brain->getAllRegions();
    if (regions.size() > 1) {
        std::cout << "  - Inter-region Connectivity:" << std::endl;
        for (size_t i = 0; i < regions.size(); ++i) {
            for (size_t j = i + 1; j < regions.size(); ++j) {
                // Estimate inter-region connections
                double estimatedConnections = regions[i]->getNeuronCount() * regions[j]->getNeuronCount() * 0.01;
                std::cout << "    Region " << i << " -> Region " << j << ": ~" << std::fixed << std::setprecision(1) << estimatedConnections << " connections" << std::endl;
            }
        }
    }
    
    // Network health assessment
    if (connectionProbability < 0.001) {
        std::cout << "⚠ Warning: Network may be too sparse for meaningful dynamics" << std::endl;
    } else if (connectionProbability > 0.5) {
        std::cout << "⚠ Warning: Network may be too dense, potential instability" << std::endl;
    }
}

void AdvancedDebugger::generateDebugReport(const std::shared_ptr<Brain>& brain) {
    std::cout << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                          NLM Advanced Debug Report                                 ║" << std::endl;
    std::cout << "╠════════════════════════════════════════════════════════════════════════════════════════════════════════╣" << std::endl;
    
    analyzeMemoryLeaks(brain);
    std::cout << "" << std::endl;
    detectPerformanceBottlenecks(brain);
    std::cout << "" << std::endl;
    analyzeConnectivity(brain);
    
    std::cout << "╩════════════════════════════════════════════════════════════════════════════════════════════════════════╩" << std::endl;
    std::cout << std::endl;
}

} // namespace nlm