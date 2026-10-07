#include "VisualizationInterface.hpp"
#include "../brain/Brain.hpp"
#include "../core/Types/Types.hpp"
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>

namespace nlm {

struct VisualizationInterface::Impl {
    bool active;
    double updateRate;
    size_t frameCount;
    
    Impl() : active(false), updateRate(30.0), frameCount(0) {}
};

VisualizationInterface::VisualizationInterface() : pImpl(new Impl) {}

VisualizationInterface::~VisualizationInterface() = default;

bool VisualizationInterface::initialize() {
    // TODO PHASE 2: Initialize real visualization (e.g., with GLFW, SDL, or custom)
    // PLACEHOLDER: No visualization in Phase 1
    pImpl->active = false;
    return true;
}

void VisualizationInterface::update() {
    pImpl->frameCount++;
}

void VisualizationInterface::render() {
    // TODO PHASE 2: Implement real rendering
    // PLACEHOLDER: Console-based visualization for now
    if (pImpl->active) {
        std::cout << "=== NLM Brain Visualization ===\n";
        std::cout << "Frame: " << pImpl->frameCount << "\n";
        std::cout << "Update Rate: " << pImpl->updateRate << " Hz\n";
    }
}

void VisualizationInterface::close() {
    pImpl->active = false;
    std::cout << "Visualization closed.\n";
}

bool VisualizationInterface::isActive() const {
    return pImpl->active;
}

void VisualizationInterface::setUpdateRate(double hz) {
    pImpl->updateRate = hz;
    if (pImpl->active) {
        std::cout << "Update rate set to " << hz << " Hz\n";
    }
}

double VisualizationInterface::getUpdateRate() const {
    return pImpl->updateRate;
}

void VisualizationInterface::visualizeNetwork(const Brain& brain) {
    // TODO PHASE 2: Implement network visualization
    // ENHANCED: Console-based network visualization for debugging
    if (!pImpl->active) return;
    
    std::cout << "\n=== Neural Network Visualization ===\n";
    
    // Get brain statistics
    size_t totalNeurons = brain.getTotalNeuronCount();
    size_t totalSynapses = brain.getTotalSynapseCount();
    size_t regionCount = brain.getRegionCount();
    
    std::cout << "Total Neurons: " << totalNeurons << "\n";
    std::cout << "Total Synapses: " << totalSynapses << "\n";
    std::cout << "Regions: " << regionCount << "\n";
    
    // Show region details
    auto regionIds = brain.getRegionIds();
    for (const auto& regionId : regionIds) {
        const NeuralRegion* region = brain.getRegion(regionId);
        if (region) {
            size_t regionNeurons = region->getTotalNeuronCount();
            size_t regionSynapses = region->getTotalSynapseCount();
            std::cout << "  Region " << regionId.index() << ": " 
                     << regionNeurons << " neurons, " 
                     << regionSynapses << " synapses\n";
        }
    }
    
    // Calculate and show network density
    double density = (totalNeurons > 0) ? 
        (double)totalSynapses / (totalNeurons * totalNeurons) : 0.0;
    std::cout << "Network Density: " << std::fixed << std::setprecision(4) 
              << density << " (sparsity: " << (1.0 - density) * 100 << "%)\n";
}

void VisualizationInterface::visualizeActivity(const Brain& brain) {
    // TODO PHASE 2: Implement activity visualization
    // ENHANCED: Console-based activity visualization
    if (!pImpl->active) return;
    
    std::cout << "\n=== Brain Activity Visualization ===\n";
    
    // Show excitation-inhibition ratio
    float eiRatio = brain.getExcitationInhibitionRatio();
    std::cout << "Excitation-Inhibition Ratio: " << std::fixed << std::setprecision(2) 
              << eiRatio << " (" << (eiRatio > 1.0 ? "Excited" : "Inhibited") << ")\n";
    
    // Show total spike count
    size_t totalSpikes = brain.getTotalSpikeCount();
    std::cout << "Total Spike Activity: " << totalSpikes << "\n";
    
    // Show pending spike events
    size_t pendingSpikes = brain.getPendingSpikeEventCount();
    std::cout << "Pending Spike Events: " << pendingSpikes << "\n";
    
    // Show neuromodulator levels
    float dopamine = brain.getDopamineLevel();
    float acetylcholine = brain.getAcetylcholineLevel();
    float norepinephrine = brain.getNorepinephrineLevel();
    float serotonin = brain.getSerotoninLevel();
    
    std::cout << "\nNeuromodulator Levels:";
    std::cout << "\n  Dopamine:       " << std::fixed << std::setprecision(2) << dopamine << " (" 
              << (dopamine > 0.7 ? "High" : dopamine > 0.3 ? "Medium" : "Low") << ")";
    std::cout << "\n  Acetylcholine:  " << std::fixed << std::setprecision(2) << acetylcholine << " (" 
              << (acetylcholine > 0.7 ? "High" : acetylcholine > 0.3 ? "Medium" : "Low") << ")";
    std::cout << "\n  Norepinephrine: " << std::fixed << std::setprecision(2) << norepinephrine << " (" 
              << (norepinephrine > 0.7 ? "High" : norepinephrine > 0.3 ? "Medium" : "Low") << ")";
    std::cout << "\n  Serotonin:     " << std::fixed << std::setprecision(2) << serotonin << " (" 
              << (serotonin > 0.7 ? "High" : serotonin > 0.3 ? "Medium" : "Low") << ")";
}

void VisualizationInterface::visualizeSpikes(const std::vector<SpikeEvent>& spikes) {
    // TODO PHASE 2: Implement spike raster plot
    // ENHANCED: Console-based spike visualization
    if (!pImpl->active) return;
    
    std::cout << "\n=== Spike Raster Visualization ===\n";
    std::cout << "Total Spikes: " << spikes.size() << "\n";
    
    if (spikes.empty()) {
        std::cout << "No spikes to display.\n";
        return;
    }
    
    // Create simple spike histogram
    std::cout << "Spike rate (last " << pImpl->frameCount << " frames):\n";
    
    // This would normally create a graphical representation
    // For now, just show summary statistics
    double maxTime = 0.0;
    for (const auto& spike : spikes) {
        if (spike.time > maxTime) maxTime = spike.time;
    }
    
    if (maxTime > 0.0) {
        double avgRate = (double)spikes.size() / maxTime * 1000.0; // spikes per second
        std::cout << "Average firing rate: " << std::fixed << std::setprecision(1) 
                  << avgRate << " Hz\n";
        
        // Show recent activity
        size_t recentSpikeCount = std::min(size_t(10), spikes.size());
        std::cout << "Recent spike times: ";
        for (size_t i = 0; i < recentSpikeCount; ++i) {
            size_t idx = spikes.size() - 1 - i;
            std::cout << std::fixed << std::setprecision(1) << spikes[idx].time << "ms ";
        }
        std::cout << "\n";
    }
}

void VisualizationInterface::visualizeWeights(const Brain& brain) {
    // TODO PHASE 2: Implement weight matrix visualization
    // ENHANCED: Console-based weight visualization
    if (!pImpl->active) return;
    
    std::cout << "\n=== Synaptic Weight Visualization ===\n";
    
    // Get weight statistics
    std::vector<float> allWeights;
    
    // This would normally sample from the brain to get actual weights
    // For now, show placeholder statistics
    size_t totalSynapses = brain.getTotalSynapseCount();
    
    if (totalSynapses > 0) {
        // Show weight range and distribution
        std::cout << "Total synapses analyzed: " << totalSynapses << "\n";
        std::cout << "Weight range: [-1.0, 1.0] (placeholder)\n";
        std::cout << "Average weight: 0.0 (placeholder)\n";
        
        // Show connectivity summary
        double avgConnectionsPerNeuron = (double)totalSynapses / brain.getTotalNeuronCount();
        std::cout << "Average connections per neuron: " << std::fixed << std::setprecision(1) 
                  << avgConnectionsPerNeuron << "\n";
        
        // Show plasticity status
        float stdpCount = brain.getSTDP() ? 1.0f : 0.0f;  // Would normally count active STDP synapses
        float hebbianCount = brain.getHebbian() ? 1.0f : 0.0f;  // Would normally count active Hebbian synapses
        
        std::cout << "\nPlasticity Mechanisms:";
        std::cout << "\n  STDP:     " << (stdpCount > 0.5 ? "Active" : "Inactive") << " (" << stdpCount * 100 << "% active)";
        std::cout << "\n  Hebbian:  " << (hebbianCount > 0.5 ? "Active" : "Inactive") << " (" << hebbianCount * 100 << "% active)";
    } else {
        std::cout << "No synapses to visualize.\n";
    }
}

} // namespace nlm
