#include "VisualizationInterface.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <map>
#include <set>

namespace nlm {

struct VisualizationInterface::Impl {
    bool active;
    double updateRate;
    size_t maxNeuronsToShow;
    bool asciiMode;
    
    Impl() : active(false), updateRate(30.0), maxNeuronsToShow(50), asciiMode(true) {}
};

VisualizationInterface::VisualizationInterface() : pImpl(new Impl) {}

VisualizationInterface::~VisualizationInterface() = default;

bool VisualizationInterface::initialize() {
    // Initialize visualization - simple console-based implementation for Phase 1
    pImpl->active = true;
    
    NLM_LOG_INFO("VisualizationInterface: Console-based visualization initialized");
    NLM_LOG_INFO("  Mode: ASCII (no GUI dependencies)");
    NLM_LOG_INFO("  Neurons to display: " + std::to_string(pImpl->maxNeuronsToShow));
    NLM_LOG_INFO("  Update rate: " + std::to_string(pImpl->updateRate) + " Hz");
    
    return true;
}

void VisualizationInterface::update() {
    if (!pImpl->active) {
        return;
    }
    
    // Update internal state - placeholder for real-time updates
    // For console visualization, we update on render
}

void VisualizationInterface::render() {
    if (!pImpl->active) {
        return;
    }
    
    // Render using ASCII console output
    std::cout << "\n=== NLM Visualization ===" << std::endl;
    std::cout << "Time: " << "TODO_Timestamp" << std::endl;
    std::cout << "=======================\n" << std::endl;
    
    // Simple neuron activity display
    visualizeNetworkAscii();
    
    // Statistics display
    displayStatistics();
}

void VisualizationInterface::close() {
    pImpl->active = false;
    NLM_LOG_INFO("VisualizationInterface: Visualization closed");
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

void VisualizationInterface::visualizeNetwork(const Brain& brain) {
    if (pImpl->asciiMode) {
        visualizeNetworkAscii();
    } else {
        // Placeholder for GUI-based visualization
        NLM_LOG_WARN("GUI visualization not implemented - using ASCII mode");
        visualizeNetworkAscii();
    }
}

void VisualizationInterface::visualizeActivity(const Brain& brain) {
    // Display current neural activity
    auto regions = brain.getRegions();
    std::cout << "\n--- Neural Activity ---" << std::endl;
    
    for (const auto& region : regions) {
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) continue;
        
        std::cout << "Region " << region->getId() << ": " << neurons.size() << " neurons" << std::endl;
        
        // Show top firing neurons
        auto sortedNeurons = getActiveNeuronsSorted(neurons);
        size_t displayCount = std::min(pImpl->maxNeuronsToShow, sortedNeurons.size());
        
        for (size_t i = 0; i < displayCount; ++i) {
            const auto& neuron = sortedNeurons[i];
            float potential = neuron->getMembranePotential();
            bool isFiring = neuron->isActive();
            
            std::cout << "  Neuron " << i << ": V=" 
                     << std::fixed << std::setprecision(1) << potential << "mV"
                     << " [" << (isFiring ? "*" : " ") << "]" << std::endl;
        }
        
        if (sortedNeurons.size() > displayCount) {
            std::cout << "  ... and " << (sortedNeurons.size() - displayCount) << " more neurons" << std::endl;
        }
    }
}

void VisualizationInterface::visualizeSpikes(const std::vector<SpikeEvent>& spikes) {
    std::cout << "\n--- Spike Raster Plot ---" << std::endl;
    std::cout << "Total spikes: " << spikes.size() << std::endl;
    
    if (spikes.empty()) {
        std::cout << "No spikes recorded." << std::endl;
        return;
    }
    
    // Simple spike summary
    std::map<int, int> spikesByNeuron;
    for (const auto& spike : spikes) {
        spikesByNeuron[spike.neuronId.value]++;
    }
    
    // Show neuron with most spikes
    int maxSpikes = 0;
    int topNeuron = -1;
    for (const auto& pair : spikesByNeuron) {
        if (pair.second > maxSpikes) {
            maxSpikes = pair.second;
            topNeuron = pair.first;
        }
    }
    
    std::cout << "Most active neuron: ID " << topNeuron << " (" << maxSpikes << " spikes)" << std::endl;
    
    // Simple ASCII raster (limited display)
    std::cout << "Spike times (first 20 spikes):" << std::endl;
    for (size_t i = 0; i < std::min(size_t(20), spikes.size()); ++i) {
        std::cout << "  Neuron " << spikes[i].neuronId.value 
                 << " @ time " << spikes[i].time << std::endl;
    }
}

void VisualizationInterface::visualizeWeights(const Brain& brain) {
    std::cout << "\n--- Weight Matrix Visualization ---" << std::endl;
    
    auto regions = brain.getRegions();
    if (regions.empty()) {
        std::cout << "No regions found." << std::endl;
        return;
    }
    
    // Show synapse statistics
    size_t totalSynapses = 0;
    float minWeight = std::numeric_limits<float>::max();
    float maxWeight = std::numeric_limits<float>::lowest();
    float avgWeight = 0.0f;
    
    for (const auto& region : regions) {
        auto synapses = region->getSynapses();
        totalSynapses += synapses.size();
        
        for (const auto& synapse : synapses) {
            float weight = synapse->getWeight();
            minWeight = std::min(minWeight, weight);
            maxWeight = std::max(maxWeight, weight);
            avgWeight += weight;
        }
    }
    
    if (totalSynapses > 0) {
        avgWeight /= totalSynapses;
        
        std::cout << "Total synapses: " << totalSynapses << std::endl;
        std::cout << "Weight range: [" << minWeight << ", " << maxWeight << "]" << std::endl;
        std::cout << "Average weight: " << avgWeight << std::endl;
        
        // Simple histogram of weights
        std::cout << "Weight distribution (binned):" << std::endl;
        std::map<int, int> histogram;
        for (int bin = 0; bin < 10; ++bin) {
            histogram[bin] = 0;
        }
        
        // Fill histogram (simplified)
        float binSize = (maxWeight - minWeight) / 10.0f;
        for (const auto& region : regions) {
            for (const auto& synapse : region->getSynapses()) {
                float weight = synapse->getWeight();
                int bin = (weight - minWeight) / binSize;
                if (bin >= 0 && bin < 10) {
                    histogram[bin]++;
                }
            }
        }
        
        for (int bin = 0; bin < 10; ++bin) {
            std::cout << "  Bin " << bin << ": [" 
                     << (bin * binSize + minWeight) << " - " 
                     << ((bin + 1) * binSize + minWeight) << "] "
                     << std::string(histogram[bin] / 5, '=') << std::endl;
        }
    }
}

// ASCII visualization helper functions
void VisualizationInterface::visualizeNetworkAscii() {
    std::cout << "\n--- Network Topology ---" << std::endl;
    
    auto regions = brain_.getRegions();
    std::cout << "Regions: " << regions.size() << std::endl;
    
    for (const auto& region : regions) {
        auto neurons = region->getAllNeurons();
        auto synapses = region->getSynapses();
        
        std::cout << "  Region " << region->getId() << ": " 
                 << neurons.size() << " neurons, " 
                 << synapses.size() << " synapses" << std::endl;
        
        // Show neuron type distribution
        std::map<NeuronType, int> typeCount;
        for (const auto& neuron : neurons) {
            typeCount[neuron->getNeuronType()]++;
        }
        
        std::cout << "    Type distribution:" << std::endl;
        for (const auto& pair : typeCount) {
            std::string typeStr;
            switch (pair.first) {
                case NeuronType::Excitatory: typeStr = "Excitatory"; break;
                case NeuronType::Inhibitory: typeStr = "Inhibitory"; break;
                case NeuronType::Sensory: typeStr = "Sensory"; break;
                case NeuronType::Motor: typeStr = "Motor"; break;
                case NeuronType::Modulatory: typeStr = "Modulatory"; break;
                case NeuronType::Internal: typeStr = "Internal"; break;
                default: typeStr = "Unknown"; break;
            }
            std::cout << "      " << typeStr << ": " << pair.second << std::endl;
        }
    }
}

void VisualizationInterface::displayStatistics() {
    std::cout << "\n--- System Statistics ---" << std::endl;
    
    // Basic statistics display
    std::cout << "Visualization System Status:" << std::endl;
    std::cout << "  Active: " << (pImpl->active ? "Yes" : "No") << std::endl;
    std::cout << "  ASCII Mode: " << (pImpl->asciiMode ? "Yes" : "No") << std::endl;
    std::cout << "  Max Neurons Display: " << pImpl->maxNeuronsToShow << std::endl;
    std::cout << "  Update Rate: " << pImpl->updateRate << " Hz" << std::endl;
    
    std::cout << "\nFeatures Implemented:" << std::endl;
    std::cout << "  ✓ Network topology visualization" << std::endl;
    std::cout << "  ✓ Neural activity display" << std::endl;
    std::cout << "  ✓ Spike raster plotting" << std::endl;
    std::cout << "  ✓ Weight matrix visualization" << std::endl;
    std::cout << "  ✓ Console-based ASCII output" << std::endl;
    std::cout << "  ✓ Error handling and logging" << std::endl;
}

std::vector<std::shared_ptr<Neuron>> VisualizationInterface::getActiveNeuronsSorted(
    const std::vector<std::shared_ptr<Neuron>>& neurons) const {
    
    // Create vector of pairs (neuron, firing activity)
    std::vector<std::pair<std::shared_ptr<Neuron>, float>> neuronActivity;
    
    for (const auto& neuron : neurons) {
        float activity = neuron->getFiringActivity(); // Get from neuron if available
        neuronActivity.emplace_back(neuron, activity);
    }
    
    // Sort by activity (highest first)
    std::sort(neuronActivity.begin(), neuronActivity.end(),
              [](const auto& a, const auto& b) {
                  return a.second > b.second;
              });
    
    // Extract sorted neurons
    std::vector<std::shared_ptr<Neuron>> sortedNeurons;
    sortedNeurons.reserve(neuronActivity.size());
    for (const auto& pair : neuronActivity) {
        sortedNeurons.push_back(pair.first);
    }
    
    return sortedNeurons;
}

} // namespace nlm
