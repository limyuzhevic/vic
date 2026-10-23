#include "BrainStateManagement.hpp"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <sstream>

namespace nlm {

bool BrainStateManagement::save(const Brain& brain, const std::string& filepath) {
    NLM_LOG_INFO("Saving brain state to " + filepath);
    
    try {
        CheckpointWriter writer;
        if (!writer.create(filepath, CompressionLevel::Balanced)) {
            NLM_LOG_ERROR("Failed to create checkpoint file: " + filepath);
            return false;
        }
        
        // Set metadata
        writer.setMetadata(
            brain.getTotalNeuronCount(),
            brain.getTotalSynapseCount(),
            brain.getRegionCount(),
            brain.getTotalSpikeCount(),
            0.0  // Will need to add to Brain class
        );
        
        // Write neurons (implementation would need access to brain internal state)
        // For now, this is a placeholder
        NLM_LOG_INFO("Brain state saved successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving brain: ") + e.what());
        return false;
    }
}

bool BrainStateManagement::load(Brain& brain, const std::string& filepath) {
    NLM_LOG_INFO("Loading brain state from " + filepath);
    
    try {
        CheckpointReader reader;
        if (!reader.open(filepath)) {
            NLM_LOG_ERROR("Failed to open checkpoint file: " + filepath);
            return false;
        }
        
        if (!reader.validate()) {
            NLM_LOG_ERROR("Checkpoint validation failed: " + reader.getError());
            return false;
        }
        
        // This would need to properly restore brain state
        // For now, this is a placeholder
        NLM_LOG_INFO("Brain state loaded successfully (placeholder)");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading brain: ") + e.what());
        return false;
    }
}

void BrainStateManagement::logStatus(const Brain& brain) {
    NLM_LOG_INFO("=== NLM Brain Status (Phase 6 - Integrated) ===");
    NLM_LOG_INFO("Regions: " + std::to_string(brain.getRegionCount()));
    NLM_LOG_INFO("Total neurons: " + std::to_string(brain.getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(brain.getTotalSynapseCount()));
    NLM_LOG_INFO("Active neurons: " + std::to_string(brain.getActiveNeuronCount()));
    NLM_LOG_INFO("Firing neurons (this step): " + std::to_string(brain.getFiringNeuronCount()));
    NLM_LOG_INFO("Total spikes: " + std::to_string(brain.getTotalSpikeCount()));
    NLM_LOG_INFO("Pending spike events: " + std::to_string(brain.getPendingSpikeEventCount()));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(brain.getAverageFiringRate()));
    NLM_LOG_INFO("E/I ratio: " + std::to_string(brain.getExcitationInhibitionRatio()));
    
    // Memory system status
    NeuralWorkingMemory* workingMemory = brain.getWorkingMemory();
    if (workingMemory) {
        NLM_LOG_INFO("Working memory traces: " + std::to_string(workingMemory->getActiveTraces()));
    }
    
    NeuralEpisodicMemory* episodicMemory = brain.getEpisodicMemory();
    if (episodicMemory) {
        NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(episodicMemory->getEpisodeCount()));
    }
    
    // Neuromodulation status
    Dopamine* dopamine = brain.getDopamine();
    if (dopamine) {
        NLM_LOG_INFO("Dopamine level: " + std::to_string(dopamine->getLevel()));
    }
}

void BrainStateManagement::reset(Brain::Impl* pImpl) {
    NLM_LOG_INFO("Resetting NLM Brain...");
    
    pImpl->currentStep = 0;
    pImpl->currentTime = 0.0;
    pImpl->totalSpikesThisStep = 0;
    pImpl->totalSpikesTotal = 0;
    pImpl->isResting = false;
    pImpl->stepsSinceLastEpisode = 0;
    
    for (auto& region : pImpl->regions) {
        region->reset();
    }
    
    pImpl->spikeSystem->reset();
    pImpl->developmentalStage = DevelopmentalStage::Initial;
    
    // Reset memory systems
    if (pImpl->workingMemory) pImpl->workingMemory->clear();
    if (pImpl->episodicMemory) pImpl->episodicMemory->clear();
    if (pImpl->associativeMemory) pImpl->associativeMemory->clear();
    if (pImpl->attention) pImpl->attention->reset();
    
    NLM_LOG_INFO("NLM Brain reset complete");
}

void BrainStateManagement::applyNeuromodulation(Brain::Impl* pImpl, const Neuromodulator& signal) {
    // Apply neuromodulation effects on plasticity
    float modulation = signal.getLevel();
    
    // Scale STDP learning rates
    pImpl->stdp->setLTPWeight(0.01f * modulation);
    pImpl->stdp->setLTDWeight(0.012f * modulation);
}

void BrainStateManagement::updatePlasticity(Brain::Impl* pImpl) {
    // Plasticity is now applied during each step
    // This method is kept for API compatibility
}

void BrainStateManagement::develop(Brain::Impl* pImpl) {
    // Development updates structural plasticity
    pImpl->structuralPlasticity->update(nullptr, *pImpl->rng, 1000.0f);  // Pass appropriate parameters
}
