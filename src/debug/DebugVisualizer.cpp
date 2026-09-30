// Simple visualization and debugging utilities
#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <sstream>

namespace nlm {

// Simple brain state visualization
class DebugVisualizer {
public:
    // Print brain state summary
    static std::string brainStateSummary(const class Brain* brain) {
        if (!brain) return "Null brain pointer";
        
        std::ostringstream oss;
        oss << "=== Brain State Summary ===\n";
        oss << "Total Neurons: " << brain->getTotalNeuronCount() << "\n";
        oss << "Total Synapses: " << brain->getTotalSynapseCount() << "\n";
        oss << "Active Neurons: " << brain->getFiringNeuronCount() << "\n";
        oss << "Average Firing Rate: " << std::fixed << std::setprecision(2) 
            << brain->getAverageFiringRate() << " Hz\n";
        oss << "Total Spikes: " << brain->getTotalSpikeCount() << "\n";
        
        // Region information
        oss << "\nRegions:";
        auto regionIds = brain->getRegionIds();
        for (const auto& rid : regionIds) {
            auto* region = brain->getRegion(rid);
            if (region) {
                oss << "\n  Region " << rid.value << ": " 
                    << region->getPopulationCount() << " populations, "
                    << region->getNeuronCount() << " neurons";
            }
        }
        
        oss << "\n=== End Summary ===";
        return oss.str();
    }
    
    // Print neuron activity heatmap
    static std::string neuronActivityHeatmap(const class Brain* brain, int width = 40, int height = 20) {
        if (!brain) return "Null brain pointer";
        
        std::ostringstream oss;
        oss << "=== Neuron Activity Heatmap ===\n";
        
        auto neurons = brain->getAllNeurons();
        if (neurons.empty()) {
            oss << "No neurons in brain\n";
            return oss.str();
        }
        
        // Find firing rate range
        float maxFiringRate = 0.0f;
        float minFiringRate = 1000.0f;
        
        for (auto* neuron : neurons) {
            float firingRate = neuron->getState().firingRate;
            if (firingRate > maxFiringRate) maxFiringRate = firingRate;
            if (firingRate < minFiringRate) minFiringRate = firingRate;
        }
        
        if (maxFiringRate == minFiringRate) {
            maxFiringRate = minFiringRate + 1.0f;
        }
        
        // Create heatmap
        oss << "Firing Rate Range: [" << minFiringRate << ", " << maxFiringRate << "] Hz\n\n";
        
        for (int y = height - 1; y >= 0; --y) {
            oss << "  ";
            for (int x = 0; x < width; ++x) {
                // Sample neurons for visualization
                size_t neuronIndex = (static_cast<size_t>(y) * width + x) % neurons.size();
                auto* neuron = neurons[neuronIndex];
                float firingRate = neuron->getState().firingRate;
                
                // Normalize to heatmap range
                float normalized = (firingRate - minFiringRate) / (maxFiringRate - minFiringRate);
                normalized = std::max(0.0f, std::min(1.0f, normalized));
                
                int intensity = static_cast<int>(normalized * 10);
                if (intensity >= 9) oss << "█";
                else if (intensity >= 7) oss << "▓";
                else if (intensity >= 5) oss << "▒";
                else if (intensity >= 3) oss << "░";
                else oss << " ";
            }
            oss << "\n";
        }
        
        oss << "\nLegend: █████ High | ▓▓ Medium | ▒░ Low |   No activity\n";
        oss << "=== End Heatmap ===";
        return oss.str();
    }
    
    // Print synaptic weight distribution
    static std::string synapticWeightDistribution(const class Brain* brain, int numBins = 10) {
        if (!brain) return "Null brain pointer";
        
        std::ostringstream oss;
        oss << "=== Synaptic Weight Distribution ===\n";
        
        std::vector<float> weights;
        for (const auto& region : brain->getRegions()) {
            for (const auto* syn : region->getSynapses()) {
                weights.push_back(syn->getWeight());
            }
        }
        
        if (weights.empty()) {
            oss << "No synapses found\n";
            return oss.str();
        }
        
        // Find weight range
        float minWeight = weights[0];
        float maxWeight = weights[0];
        for (float weight : weights) {
            if (weight < minWeight) minWeight = weight;
            if (weight > maxWeight) maxWeight = weight;
        }
        
        // Create histogram
        std::vector<int> histogram(numBins, 0);
        float binWidth = (maxWeight - minWeight) / numBins;
        
        for (float weight : weights) {
            if (maxWeight == minWeight) {
                histogram[0]++;
            } else {
                int bin = static_cast<int>((weight - minWeight) / binWidth);
                if (bin == numBins) bin = numBins - 1;
                if (bin >= 0 && bin < numBins) {
                    histogram[bin]++;
                }
            }
        }
        
        float totalWeights = static_cast<float>(weights.size());
        oss << "Total synapses: " << weights.size() << "\n";
        oss << "Weight range: [" << minWeight << ", " << maxWeight << "]\n\n";
        
        // Print histogram
        for (int i = numBins - 1; i >= 0; --i) {
            float binMin = minWeight + i * binWidth;
            float binMax = binMin + binWidth;
            int count = histogram[i];
            float percentage = (count / totalWeights) * 100.0f;
            
            oss << std::fixed << std::setprecision(3) << binMin << " - " << binMax 
                << " | " << std::string(static_cast<size_t>(percentage / 5), '=') << " " 
                << count << " (" << percentage << "%)" << "\n";
        }
        
        oss << "=== End Distribution ===";
        return oss.str();
    }
    
    // Print spike activity over time
    static std::string spikeActivityTimeline(const class Brain* brain, size_t historyLength = 100) {
        if (!brain) return "Null brain pointer";
        
        std::ostringstream oss;
        oss << "=== Spike Activity Timeline ===\n";
        
        auto* spikeSystem = brain->getSpikeSystem();
        if (!spikeSystem) {
            oss << "No spike system available\n";
            return oss.str();
        }
        
        const auto& spikeHistory = spikeSystem->getSpikeHistory();
        if (spikeHistory.empty()) {
            oss << "No spike history available\n";
            return oss.str();
        }
        
        oss << "Total spikes recorded: " << spikeHistory.size() << "\n";
        oss << "Most recent spike time: " << spikeHistory.back().timestamp << " ms\n";
        oss << "Average spike interval: " << std::fixed << std::setprecision(3);
        
        if (spikeHistory.size() > 1) {
            float totalDuration = spikeHistory.back().timestamp - spikeHistory.front().timestamp;
            oss << totalDuration / (spikeHistory.size() - 1) << " ms\n";
        } else {
            oss << "N/A (need at least 2 spikes)\n";
        }
        
        oss << "\nRecent spikes (last " << std::min(historyLength, spikeHistory.size()) << "):\n";
        
        size_t startIndex = std::max(size_t(0), spikeHistory.size() - historyLength);
        for (size_t i = startIndex; i < spikeHistory.size(); ++i) {
            const auto& spike = spikeHistory[i];
            oss << "  Step " << spike.step << ", Time " << spike.timestamp << " ms\n";
        }
        
        oss << "=== End Timeline ===";
        return oss.str();
    }
    
    // Print development stage information
    static std::string developmentalStateInfo(const class Brain* brain) {
        if (!brain) return "Null brain pointer";
        
        std::ostringstream oss;
        oss << "=== Developmental State ===\n";
        
        auto* agentBrain = dynamic_cast<const class AgentBrain*>(brain);
        if (agentBrain) {
            oss << "Developmental Stage: " << static_cast<int>(agentBrain->getDevelopmentalStage()) << "\n";
            oss << "Developmental Age: " << agentBrain->getNeuromodulationLevel() << " (arbitrary units)\n";
            oss << "Plasticity Modifier: " << agentBrain->getCuriosityLevel() << "\n";
            oss << "Dopamine Level: " << agentBrain->getPredictionError() << "\n";
            oss << "Curiosity Level: " << agentBrain->getNoveltyLevel() << "\n";
        } else {
            oss << "Not an AgentBrain (development info not available)\n";
        }
        
        oss << "=== End Development Info ===";
        return oss.str();
    }
    
    // Print comprehensive debug report
    static std::string comprehensiveDebugReport(const class Brain* brain) {
        if (!brain) return "Null brain pointer";
        
        std::ostringstream oss;
        oss << "=== COMPREHENSIVE DEBUG REPORT ===\n\n";
        
        oss << brainStateSummary(brain) << "\n\n";
        oss << neuronActivityHeatmap(brain) << "\n\n";
        oss << synapticWeightDistribution(brain) << "\n\n";
        oss << spikeActivityTimeline(brain) << "\n\n";
        oss << developmentalStateInfo(brain) << "\n\n";
        
        oss << "=== END COMPREHENSIVE REPORT ===";
        return oss.str();
    }
    
    // Print to console
    static void printToConsole(const std::string& message) {
        std::cout << message << std::endl;
    }
    
    // Log to file
    static void logToFile(const std::string& filename, const std::string& message) {
        std::ofstream file(filename, std::ios::app);
        if (file.is_open()) {
            file << message << std::endl;
            file.close();
        }
    }
};

} // namespace nlm
