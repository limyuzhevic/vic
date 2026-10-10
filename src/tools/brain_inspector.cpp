// NLM Advanced CLI - Brain Inspector Implementation
#include "tools/brain_inspector.hpp"
#include <iostream>
#include <iomanip>

namespace nlm {

brain_inspector::brain_inspector(std::shared_ptr<Brain> brain)
    : brain_(brain) {
}

brain_inspector::~brain_inspector() = default;

void brain_inspector::inspectAll() {
    if (!brain_) {
        std::cout << "Error: No brain provided for inspection." << std::endl;
        return;
    }
    
    std::cout << "=== Complete Brain Inspection ===\n" << std::endl;
    
    // Basic brain info
    std::cout << "Brain Overview:" << std::endl;
    std::cout << "  ID: " << brain_->getBrainId() << std::endl;
    std::cout << "  Neurons: " << brain_->getTotalNeuronCount() << std::endl;
    std::cout << "  Synapses: " << brain_->getTotalSynapseCount() << std::endl;
    std::cout << "  Regions: " << brain_->getRegionCount() << std::endl;
    std::cout << "  Steps: " << brain_->getCurrentStep() << std::endl;
    std::cout << "  Time: " << brain_->getSimulationTime() << " s" << std::endl;
    std::cout << "  Firing neurons: " << brain_->getFiringNeuronCount() << std::endl;
    std::cout << "  Total spikes: " << brain_->getTotalSpikeCount() << std::endl;
    
    inspectNeurons();
    inspectSynapses();
    inspectRegions();
    inspectPlasticity();
    inspectDevelopment();
    inspectNeuromodulation();
    
    std::cout << "\nInspection completed.\n" << std::endl;
}

void brain_inspector::inspectNeurons() {
    std::cout << "\nNeuron Inspection:" << std::endl;
    
    if (!brain_) return;
    
    // Show neurons by type
    std::cout << "  Neuron types:" << std::endl;
    
    // This would require access to neuron type counts
    // For now, provide basic structure
    auto* spikeSystem = brain_->getSpikeSystem();
    if (spikeSystem) {
        auto mostActive = spikeSystem->getMostActiveNeurons(10);
        std::cout << "    Most active neurons: " << mostActive.size() << std::endl;
        for (size_t i = 0; i < mostActive.size() && i < 5; ++i) {
            std::cout << "      Neuron " << mostActive[i] << std::endl;
        }
    }
    
    std::cout << "  Total neuron populations: " << brain_->getTotalNeuronCount() << std::endl;
}

void brain_inspector::inspectSynapses() {
    std::cout << "\nSynapse Inspection:" << std::endl;
    
    if (!brain_) return;
    
    std::cout << "  Total synapses: " << brain_->getTotalSynapseCount() << std::endl;
    
    // Show synapse statistics
    if (brain_->getTotalSynapseCount() > 0) {
        std::cout << "  Average weight would be displayed here." << std::endl;
        std::cout << "  Synaptic strength distribution would be shown here." << std::endl;
    }
}

void brain_inspector::inspectRegions() {
    std::cout << "\nRegion Inspection:" << std::endl;
    
    if (!brain_) return;
    
    auto regionIds = brain_->getAllRegionIds();
    std::cout << "  Number of regions: " << regionIds.size() << std::endl;
    
    for (const auto& regionId : regionIds) {
        auto* region = brain_->getRegion(regionId);
        if (region) {
            std::cout << "    Region " << regionId << ": " << region->getName() << std::endl;
            std::cout << "      Neurons: " << region->getNeuronCount() << std::endl;
            std::cout << "      Synapses: " << region->getSynapseCount() << std::endl;
        }
    }
}

void brain_inspector::inspectPlasticity() {
    std::cout << "\nPlasticity Inspection:" << std::endl;
    
    if (!brain_) return;
    
    std::cout << "  Plasticity enabled: " << (brain_->isPlasticityEnabled() ? "Yes" : "No") << std::endl;
    
    // Show plasticity rule status
    std::cout << "  Plasticity rules would be detailed here." << std::endl;
}

void brain_inspector::inspectDevelopment() {
    std::cout << "\nDevelopment Inspection:" << std::endl;
    
    if (!brain_) return;
    
    std::cout << "  Development stage: " << static_cast<int>(brain_->getDevelopmentalStage()) << std::endl;
    std::cout << "  Development enabled: would be shown here." << std::endl;
}

void brain_inspector::inspectNeuromodulation() {
    std::cout << "\nNeuromodulation Inspection:" << std::endl;
    
    std::cout << "  Neuromodulation parameters would be shown here." << std::endl;
    std::cout << "  Current levels would be displayed here." << std::endl;
}

brain_inspector::brain_inspector() : brain_(nullptr) {
}

} // namespace nlm

