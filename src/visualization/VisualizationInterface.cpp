#pragma once

#include "VisualizationInterface.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <string>
#include <chrono>
#include <thread>
#include <map>

namespace nlm {

struct VisualizationInterface::Impl {
    bool active;
    double updateRate;
    bool useConsole;
    bool useFileOutput;
    std::string outputFile;
    bool realTimeUpdates;
    size_t maxPoints;
    
    // Data collection
    std::vector<std::string> recentLogs;
    std::map<size_t, double> neuronActivityHistory;
    std::map<std::pair<size_t, size_t>, double> weightHistory;
    double simulationTime;
    
    Impl() 
        : active(false)
        , updateRate(30.0)
        , useConsole(true)
        , useFileOutput(false)
        , realTimeUpdates(true)
        , maxPoints(1000)
        , simulationTime(0.0) {}
};

VisualizationInterface::VisualizationInterface() : pImpl(new Impl) {}

VisualizationInterface::~VisualizationInterface() = default;

bool VisualizationInterface::initialize() {
    // Initialize visualization system
    NLM_LOG_INFO("Initializing enhanced visualization interface...");
    
    pImpl->active = true;
    pImpl->simulationTime = 0.0;
    
    // Setup default visualization mode
    if (pImpl->useConsole) {
        NLM_LOG_INFO("Console visualization enabled");
    }
    if (pImpl->useFileOutput) {
        NLM_LOG_INFO("File output visualization enabled to: " + pImpl->outputFile);
    }
    
    return true;
}

void VisualizationInterface::update() {
    if (!pImpl->active) return;
    
    // Update simulation time
    pImpl->simulationTime += 1.0 / pImpl->updateRate;
    
    // Clear old data
    if (pImpl->neuronActivityHistory.size() > pImpl->maxPoints) {
        auto it = pImpl->neuronActivityHistory.begin();
        it++;
        pImpl->neuronActivityHistory.erase(pImpl->neuronActivityHistory.begin(), it);
    }
    
    // In a real implementation, we would collect data from the brain here
    // For now, we'll simulate some data collection
}

void VisualizationInterface::render() {
    if (!pImpl->active) return;
    
    if (pImpl->useConsole) {
        renderConsoleVisualization();
    }
    
    if (pImpl->useFileOutput) {
        writeDataToFile();
    }
}

void VisualizationInterface::close() {
    pImpl->active = false;
    if (pImpl->useFileOutput && pImpl->outputFile.empty() == false) {
        NLM_LOG_INFO("Visualization data written to: " + pImpl->outputFile);
    }
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
    // Enhanced network visualization with statistics
    if (!pImpl->active) return;
    
    std::cout << "\n=== NLM Network Visualization ===" << std::endl;
    std::cout << "Time: " << pImpl->simulationTime << "s" << std::endl;
    std::cout << "Regions: " << brain.getRegionCount() << std::endl;
    std::cout << "Total Neurons: " << brain.getTotalNeuronCount() << std::endl;
    std::cout << "Total Synapses: " << brain.getTotalSynapseCount() << std::endl;
    std::cout << "E/I Ratio: " << brain.getExcitationInhibitionRatio() << std::endl;
    std::cout << "Average Firing Rate: " << brain.getAverageFiringRate() << " Hz" << std::endl;
    std::cout << "Active Neurons: " << brain.getActiveNeuronCount() << std::endl;
    std::cout << "Firing Neurons (this step): " << brain.getFiringNeuronCount() << std::endl;
    
    // Show region details
    const auto& regions = brain.getRegions();
    for (size_t i = 0; i < regions.size(); ++i) {
        const auto& region = regions[i];
        std::cout << "\nRegion " << region->getId().index() << " (" << region->getName() << "):";
        std::cout << " Neurons: " << region->getTotalNeuronCount();
        std::cout << ", Populations: " << region->getPopulationCount();
        std::cout << ", Synapses: " << region->getSynapseCount();
        std::cout << ", Avg Weight: " << region->getAverageSynapticWeight();
    }
}

void VisualizationInterface::visualizeActivity(const Brain& brain) {
    if (!pImpl->active) return;
    
    std::cout << "\n=== Neural Activity Visualization ===" << std::endl;
    
    // Collect activity data
    std::vector<float> activityLevels;
    const auto& regions = brain.getRegions();
    
    for (const auto& region : regions) {
        const auto& populations = region->getPopulations();
        for (const auto& pop : populations) {
            for (const auto* neuron : pop->getNeurons()) {
                const auto& state = neuron->getState();
                float activity = std::abs(state.membranePotential - state.restingPotential);
                activityLevels.push_back(activity);
            }
        }
    }
    
    if (activityLevels.empty()) {
        std::cout << "No activity data available" << std::endl;
        return;
    }
    
    // Calculate statistics
    float minActivity = *std::min_element(activityLevels.begin(), activityLevels.end());
    float maxActivity = *std::max_element(activityLevels.begin(), activityLevels.end());
    float sumActivity = std::accumulate(activityLevels.begin(), activityLevels.end(), 0.0f);
    float avgActivity = sumActivity / activityLevels.size();
    
    std::cout << "Activity Distribution:" << std::endl;
    std::cout << "  Min: " << minActivity << " mV" << std::endl;
    std::cout << "  Max: " << maxActivity << " mV" << std::endl;
    std::cout << "  Average: " << avgActivity << " mV" << std::endl;
    std::cout << "  Total neurons: " << activityLevels.size() << std::endl;
    
    // Show sample activity in console (simplified)
    std::cout << "Sample activity levels:" << std::endl;
    size_t samplesToShow = std::min(static_cast<size_t>(10), activityLevels.size());
    for (size_t i = 0; i < samplesToShow; ++i) {
        std::cout << "  Neuron " << i << ": " << activityLevels[i] << " mV" << std::endl;
    }
}

void VisualizationInterface::visualizeSpikes(const std::vector<SpikeEvent>& spikes) {
    if (!pImpl->active) return;
    
    std::cout << "\n=== Spike Raster Visualization ===" << std::endl;
    
    if (spikes.empty()) {
        std::cout << "No spikes recorded" << std::endl;
        return;
    }
    
    // Group spikes by neuron
    std::map<size_t, std::vector<Timestamp>> spikesByNeuron;
    for (const auto& spike : spikes) {
        spikesByNeuron[spike.neuronId.index()].push_back(spike.timestamp);
    }
    
    // Statistics
    size_t totalSpikes = spikes.size();
    size_t numNeurons = spikesByNeuron.size();
    
    std::cout << "Total spikes: " << totalSpikes << std::endl;
    std::cout << "Active neurons: " << numNeurons << std::endl;
    
    if (totalSpikes > 0 && numNeurons > 0) {
        float avgSpikesPerNeuron = static_cast<float>(totalSpikes) / numNeurons;
        std::cout << "Average spikes per neuron: " << avgSpikesPerNeuron << std::endl;
    }
    
    // Show recent spike activity
    std::cout << "\nRecent spikes (last 20):" << std::endl;
    size_t count = 0;
    for (const auto& spike : spikes) {
        if (count >= 20) break;
        std::cout << "  Neuron " << spike.neuronId.index() 
                  << " at t=" << spike.timestamp << " steps" << std::endl;
        count++;
    }
}

void VisualizationInterface::visualizeWeights(const Brain& brain) {
    if (!pImpl->active) return;
    
    std::cout << "\n=== Synaptic Weight Visualization ===" << std::endl;
    
    const auto& regions = brain.getRegions();
    std::vector<float> weights;
    
    for (const auto& region : regions) {
        const auto& synapses = region->getSynapses();
        for (const auto* syn : synapses) {
            weights.push_back(syn->getWeight());
        }
    }
    
    if (weights.empty()) {
        std::cout << "No synapses found" << std::endl;
        return;
    }
    
    // Calculate weight statistics
    float minWeight = *std::min_element(weights.begin(), weights.end());
    float maxWeight = *std::max_element(weights.begin(), weights.end());
    float sumWeight = std::accumulate(weights.begin(), weights.end(), 0.0f);
    float avgWeight = sumWeight / weights.size();
    
    std::cout << "Weight Distribution:" << std::endl;
    std::cout << "  Min: " << minWeight << std::endl;
    std::cout << "  Max: " << maxWeight << std::endl;
    std::cout << "  Average: " << avgWeight << std::endl;
    std::cout << "  Total synapses: " << weights.size() << std::endl;
    
    // Count positive and negative weights
    size_t positiveWeights = std::count_if(weights.begin(), weights.end(),
                                          [](float w) { return w > 0; });
    size_t negativeWeights = std::count_if(weights.begin(), weights.end(),
                                          [](float w) { return w < 0; });
    
    std::cout << "Excitory synapses: " << positiveWeights << std::endl;
    std::cout << "Inhibitory synapses: " << negativeWeights << std::endl;
}

void VisualizationInterface::renderConsoleVisualization() {
    // Basic console visualization with formatted output
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "NLM VISUALIZATION - " << pImpl->simulationTime << "s" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
}

void VisualizationInterface::writeDataToFile() {
    if (pImpl->outputFile.empty()) return;
    
    std::ofstream file(pImpl->outputFile, std::ios::app);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open visualization output file: " + pImpl->outputFile);
        return;
    }
    
    // Write visualization data
    file << "Time: " << pImpl->simulationTime << std::endl;
    file << "Neurons active: " << (pImpl->neuronActivityHistory.size() > 0 ? "yes" : "no") << std::endl;
    file << "Simulation step completed at " << std::chrono::system_clock::now() << std::endl;
    file << "---" << std::endl;
}

} // namespace nlm