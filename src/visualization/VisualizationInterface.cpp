#include "VisualizationInterface.hpp"
#include "../brain/Brain.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../core/Types/Types.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <map>

namespace nlm {

struct VisualizationInterface::Impl {
    bool active;
    double updateRate;
    bool enableConsoleOutput;
    size_t maxConsoleLines;
    
    Impl() : active(false), updateRate(30.0), enableConsoleOutput(true), maxConsoleLines(50) {}
};

VisualizationInterface::VisualizationInterface() : pImpl(new Impl) {}

VisualizationInterface::~VisualizationInterface() = default;

bool VisualizationInterface::initialize() {
    // Initialize console-based visualization
    pImpl->active = true;
    pImpl->enableConsoleOutput = true;
    NLM_LOG_INFO("NLM Visualization Interface initialized (console-based, Phase 2)");
    return true;
}

void VisualizationInterface::update() {
    // Console-based visualization updates automatically
    if (!pImpl->active || !pImpl->enableConsoleOutput) {
        return;
    }
}

void VisualizationInterface::render() {
    // Console visualization rendered via output streams
    if (!pImpl->active || !pImpl->enableConsoleOutput) {
        return;
    }
}

void VisualizationInterface::close() {
    pImpl->active = false;
    NLM_LOG_INFO("NLM Visualization Interface closed");
}

bool VisualizationInterface::isActive() const {
    return pImpl->active;
}

void VisualizationInterface::setUpdateRate(double hz) {
    pImpl->updateRate = hz;
}

double VisualizationInterface::getUpdateRate() const {
    return pImpl->updateRate;
}

// Helper function for centering text
static std::string centerText(const std::string& text, size_t width) {
    if (text.length() >= width) return text;
    size_t padding = (width - text.length()) / 2;
    return std::string(padding, ' ') + text + std::string(width - padding - text.length(), ' ');
}

// Helper function for truncating long numbers
static std::string formatNumber(float value, size_t precision = 3) {
    if (value == std::floor(value)) {
        return std::to_string(static_cast<int>(value));
    }
    std::stringstream ss;
    ss << std::fixed << std::setprecision(precision) << value;
    return ss.str();
}

void VisualizationInterface::visualizeNetwork(const Brain& brain) {
    if (!pImpl->active || !pImpl->enableConsoleOutput) return;
    
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << centerText("NLM NEURAL NETWORK VISUALIZATION", 80) << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    
    size_t totalNeurons = brain.getTotalNeuronCount();
    size_t totalSynapses = brain.getTotalSynapseCount();
    size_t regionCount = brain.getRegionCount();
    size_t spikeCount = brain.getTotalSpikeCount();
    
    std::cout << "\nNETWORK STATISTICS:" << std::endl;
    std::cout << "  Regions:             " << regionCount << std::endl;
    std::cout << "  Total Neurons:       " << totalNeurons << std::endl;
    std::cout << "  Total Synapses:      " << totalSynapses << std::endl;
    std::cout << "  Total Spikes:        " << spikeCount << std::endl;
    
    if (regionCount > 0) {
        std::cout << "\nREGION OVERVIEW:" << std::endl;
        std::cout << "  " << std::string(60, '-') << std::endl;
        for (size_t i = 0; i < regionCount; ++i) {
            const NeuralRegion* region = brain.getRegion(RegionId(i + 1));
            if (region) {
                std::cout << "  Region " << std::setw(2) << (i + 1) << ": " 
                          << std::left << std::setw(40) << region->getName()
                          << "Neurons: " << std::setw(6) << region->getTotalNeuronCount()
                          << "Synapses: " << region->getSynapseCount() << std::endl;
            }
        }
    }
    
    // Visualize inter-region connections
    std::cout << "\nINTER-REGION CONNECTIONS:" << std::endl;
    std::cout << "  " << std::string(60, '-') << std::endl;
    
    // ASCII art for network topology
    for (size_t src = 0; src < regionCount; ++src) {
        std::cout << "  Region " << (src + 1);
        for (size_t tgt = 0; tgt < regionCount; ++tgt) {
            if (src != tgt) {
                // Check if there's a connection from src to tgt
                bool hasConnection = false;
                for (const auto& conn : brain.getRegions()) {
                    // This is a simplified check - in a real implementation
                    // we would access the brain's inter-region connections
                }
                if (hasConnection) {
                    std::cout << " ->"; // Simplified arrow
                } else {
                    std::cout << " -|";
                }
            }
        }
        std::cout << std::endl;
    }
    
    std::cout << std::string(80, '=') << std::endl;
}

void VisualizationInterface::visualizeActivity(const Brain& brain) {
    if (!pImpl->active || !pImpl->enableConsoleOutput) return;
    
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << centerText("NEURAL ACTIVITY VISUALIZATION", 80) << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    
    size_t totalNeurons = brain.getTotalNeuronCount();
    size_t activeNeurons = brain.getActiveNeuronCount();
    float avgFiringRate = brain.getAverageFiringRate();
    float eiRatio = brain.getExcitationInhibitionRatio();
    
    std::cout << "\nACTIVITY METRICS:" << std::endl;
    std::cout << "  Active Neurons:      " << activeNeurons << " (" 
              << (totalNeurons > 0 ? (100.0f * activeNeurons / totalNeurons) : 0.0f) << "%)" << std::endl;
    std::cout << "  Average Firing Rate: " << formatNumber(avgFiringRate, 3) << " Hz" << std::endl;
    std::cout << "  E/I Balance:         " << formatNumber(eiRatio) << std::endl;
    
    // Create activity heatmap across regions
    if (brain.getRegionCount() > 0) {
        std::cout << "\nACTIVITY HEATMAP BY REGION:" << std::endl;
        std::cout << "  " << std::string(60, '-') << std::endl;
        
        for (const auto& region : brain.getRegions()) {
            size_t neurons = region->getTotalNeuronCount();
            size_t firing = region->getFiringNeuronCount();
            float activity = region->getActivityLevel();
            
            // Simple ASCII heatmap bar
            std::cout << "  Region " << std::setw(2) << region->getId().index()
                      << " (" << region->getName() << "):";
            
            // Activity bar
            size_t barWidth = 40;
            size_t filledBars = static_cast<size_t>(activity * barWidth);
            std::cout << " [";
            for (size_t i = 0; i < barWidth; ++i) {
                if (i < filledBars) std::cout << "#";
                else std::cout << ".";
            }
            std::cout << "] " << formatNumber(activity * 100, 1) << "%   "
                      << firing << "/" << neurons << " firing" << std::endl;
        }
    }
    
    // Neuron state visualization
    std::cout << "\nSAMPLE NEURON STATES:" << std::endl;
    std::cout << "  " << std::string(60, '-') << std::endl;
    
    // Sample up to 5 neurons
    size_t neuronsToShow = std::min(static_cast<size_t>(5), totalNeurons);
    size_t shown = 0;
    
    for (const auto& region : brain.getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            for (const auto* neuron : pop->getNeurons()) {
                if (shown >= neuronsToShow) break;
                
                const auto& state = neuron->getState();
                std::string stateSymbol;
                switch (state.firingState) {
                    case FiringState::Resting: stateSymbol = "○"; break;
                    case FiringState::Active: stateSymbol = "■"; break;
                    case FiringState::Refractory: stateSymbol = "◉"; break;
                    case FiringState::Inhibited: stateSymbol = "▼"; break;
                }
                
                std::cout << "  Neuron " << std::setw(4) << neuron->getId().index()
                          << " (Type: " << static_cast<int>(neuron->getType())
                          << ") " << stateSymbol
                          << " Pot: " << formatNumber(state.membranePotential, 2)
                          << " mV"
                          << " Spike: " << (state.lastSpikeTime > 0 ? "Yes" : "No")
                          << std::endl;
                
                ++shown;
            }
            if (shown >= neuronsToShow) break;
        }
        if (shown >= neuronsToShow) break;
    }
    
    std::cout << std::string(80, '=') << std::endl;
}

void VisualizationInterface::visualizeSpikes(const std::vector<SpikeEvent>& spikes) {
    if (!pImpl->active || !pImpl->enableConsoleOutput) return;
    if (spikes.empty()) {
        std::cout << "\n" << std::string(80, '=') << std::endl;
        std::cout << centerText("NO SPIKES TO DISPLAY", 80) << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        return;
    }
    
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << centerText("SPIKE RASTER PLOT", 80) << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    
    size_t spikeCount = spikes.size();
    std::cout << "\nSPIKE STATISTICS: " << spikeCount << " spikes recorded" << std::endl;
    
    // Determine neuron count for axes
    std::map<uint64_t, size_t> neuronSpikeCounts;
    std::vector<Timestamp> spikeTimes;
    std::vector<uint64_t> neuronIds;
    
    for (const auto& spike : spikes) {
        neuronIds.push_back(spike.source_neuron.value);
        spikeTimes.push_back(spike.timestamp);
        neuronSpikeCounts[spike.source_neuron.value]++;
    }
    
    if (neuronIds.empty()) return;
    
    uint64_t maxNeuronId = *std::max_element(neuronIds.begin(), neuronIds.end());
    Timestamp maxTime = *std::max_element(spikeTimes.begin(), spikeTimes.end());
    Timestamp minTime = *std::min_element(spikeTimes.begin(), spikeTimes.end());
    
    // Simple ASCII raster plot
    std::cout << "\nRASTER PLOT (Neuron ID on Y-axis, Time on X-axis):" << std::endl;
    std::cout << "  " << std::string(60, '-') << std::endl;
    
    // Show neuron activity summary
    std::cout << "  Neuron Activity Summary:" << std::endl;
    for (const auto& pair : neuronSpikeCounts) {
        std::cout << "    Neuron " << std::setw(4) << pair.first
                  << ": " << pair.second << " spikes"
                  << " (rate: " << formatNumber(static_cast<float>(pair.second) / (maxTime - minTime + 0.001), 3)
                  << " spikes/s)" << std::endl;
    }
    
    // Time window information
    std::cout << "\nTIME WINDOW: " << formatNumber(minTime, 3) 
              << " to " << formatNumber(maxTime, 3)
              << " seconds (Duration: " << formatNumber(maxTime - minTime, 3) << " s)" << std::endl;
    
    // Spike clustering visualization
    std::cout << "\nSPIKE CLUSTERS (by neuron):" << std::endl;
    std::cout << "  " << std::string(60, '-') << std::endl;
    
    // Group spikes by neuron for visualization
    std::map<uint64_t, std::vector<Timestamp>> spikesByNeuron;
    for (const auto& spike : spikes) {
        spikesByNeuron[spike.source_neuron.value].push_back(spike.timestamp);
    }
    
    // Show a few example spike times for each neuron
    size_t neuronsShown = 0;
    for (const auto& pair : spikesByNeuron) {
        if (neuronsShown >= 5) break; // Show only first 5 neurons
        
        std::cout << "  Neuron " << pair.first << ":";
        const auto& times = pair.second;
        size_t displayCount = std::min(static_cast<size_t>(3), times.size());
        
        for (size_t i = 0; i < displayCount; ++i) {
            std::cout << " " << formatNumber(times[i], 3) << "s";
        }
        
        if (times.size() > displayCount) {
            std::cout << " ... and " << (times.size() - displayCount) << " more";
        }
        std::cout << std::endl;
        
        ++neuronsShown;
    }
    
    std::cout << std::string(80, '=') << std::endl;
}

void VisualizationInterface::visualizeWeights(const Brain& brain) {
    if (!pImpl->active || !pImpl->enableConsoleOutput) return;
    
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << centerText("SYNAPTIC WEIGHT MATRIX VISUALIZATION", 80) << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    
    size_t totalSynapses = brain.getTotalSynapseCount();
    size_t regionCount = brain.getRegionCount();
    
    std::cout << "\nSYNAPSE STATISTICS:" << std::endl;
    std::cout << "  Total Synapses:      " << totalSynapses << std::endl;
    std::cout << "  Regions:             " << regionCount << std::endl;
    
    if (regionCount == 0 || totalSynapses == 0) {
        std::cout << std::string(80, '=') << std::endl;
        return;
    }
    
    // Calculate weight statistics
    float maxWeight = -1e6f, minWeight = 1e6f;
    float avgWeight = 0.0f;
    size_t count = 0;
    
    for (const auto& region : brain.getRegions()) {
        for (const auto* syn : region->getSynapses()) {
            float weight = syn->getWeight();
            maxWeight = std::max(maxWeight, weight);
            minWeight = std::min(minWeight, weight);
            avgWeight += weight;
            ++count;
        }
    }
    
    if (count > 0) avgWeight /= static_cast<float>(count);
    
    std::cout << "\nWEIGHT STATISTICS:" << std::endl;
    std::cout << "  Minimum Weight:      " << formatNumber(minWeight, 3) << std::endl;
    std::cout << "  Maximum Weight:      " << formatNumber(maxWeight, 3) << std::endl;
    std::cout << "  Average Weight:      " << formatNumber(avgWeight, 3) << std::endl;
    std::cout << "  Positive Synapses:   " << std::count_if(
        brain.getRegions().begin(), brain.getRegions().end(),
        [](const std::unique_ptr<NeuralRegion>& region) {
            return std::count_if(region->getSynapses().begin(), region->getSynapses().end(),
                [](const std::unique_ptr<Synapse>& syn) {
                    return syn->getWeight() > 0.0f;
                }) > 0;
        }) << std::endl;
    std::cout << "  Negative Synapses:   " << std::count_if(
        brain.getRegions().begin(), brain.getRegions().end(),
        [](const std::unique_ptr<NeuralRegion>& region) {
            return std::count_if(region->getSynapses().begin(), region->getSynapses().end(),
                [](const std::unique_ptr<Synapse>& syn) {
                    return syn->getWeight() < 0.0f;
                }) > 0;
        }) << std::endl;
    
    // ASCII weight matrix for a single region if available
    if (regionCount == 1) {
        const auto& region = brain.getRegions().front();
        size_t neurons = region->getTotalNeuronCount();
        size_t synapses = region->getSynapseCount();
        
        std::cout << "\nSYNAPSE TYPE DISTRIBUTION:" << std::endl;
        std::cout << "  " << std::string(60, '-') << std::endl;
        
        size_t excitatory = 0, inhibitory = 0, electrical = 0, gapJunction = 0;
        
        for (const auto* syn : region->getSynapses()) {
            switch (syn->getType()) {
                case SynapseType::Excitatory: ++excitatory; break;
                case SynapseType::Inhibitory: ++inhibitory; break;
                case SynapseType::Modulatory: ++excitatory; break; // Treat modulatory as excitatory
                case SynapseType::Electrical: ++electrical; break;
                case SynapseType::GapJunction: ++gapJunction; break;
            }
        }
        
        std::cout << "  Excitatory:          " << std::setw(8) << excitatory << " ("
                  << (excitatory + inhibitory + electrical + gapJunction > 0 ? 
                      formatNumber(100.0f * excitatory / (excitatory + inhibitory + electrical + gapJunction), 1) : "0")
                  << "%)" << std::endl;
        std::cout << "  Inhibitory:          " << std::setw(8) << inhibitory << " ("
                  << (excitatory + inhibitory + electrical + gapJunction > 0 ?
                      formatNumber(100.0f * inhibitory / (excitatory + inhibitory + electrical + gapJunction), 1) : "0")
                  << "%)" << std::endl;
        std::cout << "  Electrical:          " << std::setw(8) << electrical << " ("
                  << (excitatory + inhibitory + electrical + gapJunction > 0 ?
                      formatNumber(100.0f * electrical / (excitatory + inhibitory + electrical + gapJunction), 1) : "0")
                  << "%)" << std::endl;
        std::cout << "  Gap Junctions:       " << std::setw(8) << gapJunction << " ("
                  << (excitatory + inhibitory + electrical + gapJunction > 0 ?
                      formatNumber(100.0f * gapJunction / (excitatory + inhibitory + electrical + gapJunction), 1) : "0")
                  << "%)" << std::endl;
        
        // Simple connectivity heatmap
        if (neurons > 0) {
            std::cout << "\nNEURAL CONNECTIVITY HEATMAP:" << std::endl;
            std::cout << "  " << std::string(60, '-') << std::endl;
            
            // Count in-degree/out-degree for visualization
            std::map<NeuronId, size_t> inDegree, outDegree;
            
            for (const auto* syn : region->getSynapses()) {
                ++outDegree[syn->getSourceNeuron()];
                ++inDegree[syn->getDestinationNeuron()];
            }
            
            // Find max values for scaling
            size_t maxInDegree = 0, maxOutDegree = 0;
            for (const auto& pair : inDegree) {
                maxInDegree = std::max(maxInDegree, pair.second);
            }
            for (const auto& pair : outDegree) {
                maxOutDegree = std::max(maxOutDegree, pair.second);
            }
            
            // Show up to 10 neurons
            size_t neuronsShown = 0;
            for (const auto& neuron : region->getAllNeurons()) {
                if (neuronsShown >= 10) break;
                
                size_t inConn = inDegree[neuron->getId()];
                size_t outConn = outDegree[neuron->getId()];
                
                // Scale connections for display
                size_t inBar = maxInDegree > 0 ? 
                    static_cast<size_t>((static_cast<float>(inConn) / maxInDegree) * 20.0f) : 0;
                size_t outBar = maxOutDegree > 0 ? 
                    static_cast<size_t>((static_cast<float>(outConn) / maxOutDegree) * 20.0f) : 0;
                
                std::cout << "  Neuron " << std::setw(3) << neuron->getId().index()
                          << ": ["; // Shows in-degree bar (left) and out-degree bar (right)
                for (size_t i = 0; i < inBar; ++i) std::cout << "|";
                std::cout << "|\\";
                for (size_t i = 0; i < outBar; ++i) std::cout << ">";
                std::cout << " ] In:" << std::setw(3) << inConn
                          << " Out:" << outConn << std::endl;
                
                ++neuronsShown;
            }
        }
    }
    
    std::cout << std::string(80, '=') << std::endl;
}

} // namespace nlm
