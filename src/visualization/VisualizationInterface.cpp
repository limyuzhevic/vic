// NLM Visualization Interface
// Phase 6: Basic Brain State Visualization for Debugging

#include "visualization/VisualizationInterface.hpp"
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>

namespace nlm {

VisualizationInterface::VisualizationInterface() {
    NLM_LOG_INFO("Visualization Interface initialized");
}

VisualizationInterface::~VisualizationInterface() {
    NLM_LOG_INFO("Visualization Interface destroyed");
}

void VisualizationInterface::showBrainState(const Brain& brain, const std::string& title) const {
    std::cout << std::endl;
    std::cout << "=== " << title << " ===" << std::endl;
    std::cout << std::endl;
    
    // Basic brain statistics
    std::cout << "Brain Statistics:" << std::endl;
    std::cout << "  Total Neurons: " << brain.getTotalNeuronCount() << std::endl;
    std::cout << "  Total Synapses: " << brain.getTotalSynapseCount() << std::endl;
    std::cout << "  Active Neurons: " << brain.getActiveNeuronCount() << std::endl;
    std::cout << "  Firing Neurons: " << brain.getFiringNeuronCount() << std::endl;
    std::cout << "  Average Firing Rate: " << std::fixed << std::setprecision(3) << brain.getAverageFiringRate() << " Hz" << std::endl;
    std::cout << "  Total Spikes: " << brain.getTotalSpikeCount() << std::endl;
    std::cout << "  Pending Spike Events: " << brain.getPendingSpikeEventCount() << std::endl;
    std::cout << std::endl;
    
    // Region statistics
    std::cout << "Region Statistics:" << std::endl;
    auto regionIds = brain.getRegionIds();
    for (size_t i = 0; i < regionIds.size(); ++i) {
        const auto* region = brain.getRegion(regionIds[i]);
        if (region) {
            std::cout << "  Region " << (i + 1) << ": "
                      << "Neurons: " << region->getTotalNeuronCount()
                      << ", Populations: " << region->getPopulationCount()
                      << ", Synapses: " << region->getSynapseCount() << std::endl;
        }
    }
    std::cout << std::endl;
    
    // Memory system status
    if (brain.getWorkingMemory()) {
        std::cout << "Working Memory:" << std::endl;
        std::cout << "  Active Traces: " << brain.getWorkingMemory()->getActiveTraces() << std::endl;
        std::cout << "  Capacity: " << brain.getWorkingMemory()->getCapacity() << std::endl;
    } else {
        std::cout << "Working Memory: Not available" << std::endl;
    }
    
    if (brain.getEpisodicMemory()) {
        std::cout << "Episodic Memory:" << std::endl;
        std::cout << "  Episodes: " << brain.getEpisodicMemory()->getEpisodeCount() << std::endl;
        std::cout << "  Max Episodes: " << brain.getEpisodicMemory()->getMaxEpisodes() << std::endl;
    } else {
        std::cout << "Episodic Memory: Not available" << std::endl;
    }
    
    // Neuromodulation levels
    auto* dopamine = brain.getDopamine();
    if (dopamine) {
        std::cout << "Neuromodulation:" << std::endl;
        std::cout << "  Dopamine Level: " << std::fixed << std::setprecision(3) << dopamine->getLevel() << std::endl;
        std::cout << "  Dopamine Plasticity Factor: " << std::fixed << std::setprecision(3) << dopamine->getPlasticityFactor() << std::endl;
    }
    
    auto* curiosity = brain.getCuriosity();
    if (curiosity) {
        std::cout << "  Curiosity Level: " << std::fixed << std::setprecision(3) << curiosity->getLevel() << std::endl;
    }
    
    auto* novelty = brain.getNovelty();
    if (novelty) {
        std::cout << "  Novelty Level: " << std::fixed << std::setprecision(3) << novelty->getLevel() << std::endl;
    }
    
    // Prediction system status
    if (brain.getPredictionSystem()) {
        std::cout << "Prediction System: Available" << std::endl;
    } else {
        std::cout << "Prediction System: Not available" << std::endl;
    }
    
    // Cognitive systems status
    if (brain.getPlanner()) {
        std::cout << "Cognitive Systems:" << std::endl;
        std::cout << "  Neural Planner: Available" << std::endl;
    }
    
    if (brain.getConceptFormation()) {
        std::cout << "  Concept Formation: Available (" << brain.getConceptFormation()->getConceptCount() << " concepts)" << std::endl;
    }
    
    if (brain.getAttention()) {
        std::cout << "  Attentional Selection: Available" << std::endl;
    }
    
    // Development status
    if (brain.getDevelopmentSystem()) {
        std::cout << "Development System: Available (Stage: " << static_cast<int>(brain.getDevelopmentalStage()) << ")" << std::endl;
    } else {
        std::cout << "Development System: Not available" << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "=========================" << std::endl;
}

void VisualizationInterface::showSimulationStep(const Brain& brain, size_t step) const {
    std::cout << "Step " << step << ": "
              << "Firing Rate: " << std::fixed << std::setprecision(3) << brain.getAverageFiringRate() << " Hz"
              << ", Active: " << brain.getActiveNeuronCount()
              << ", Spikes: " << brain.getTotalSpikeCount()
              << ", Memory: " << (brain.getWorkingMemory() ? brain.getWorkingMemory()->getActiveTraces() : 0)
              << std::endl;
}

void VisualizationInterface::showSynapseActivity(const Brain& brain, size_t topN) const {
    // This is a simplified visualization - in a full implementation, this would show
    // detailed synapse information, weights, and activity patterns
    std::cout << "=== Synapse Activity (Top " << topN << ") ===" << std::endl;
    std::cout << "Synapse activity visualization would be shown here." << std::endl;
    std::cout << "Full visualization would require more detailed tracking." << std::endl;
}

void VisualizationInterface::showNeuromodulationActivity(const Brain& brain) const {
    std::cout << "=== Neuromodulation Activity ===" << std::endl;
    
    auto* dopamine = brain.getDopamine();
    if (dopamine) {
        std::cout << "Dopamine: " << std::fixed << std::setprecision(3) << dopamine->getLevel() << std::endl;
    }
    
    auto* curiosity = brain.getCuriosity();
    if (curiosity) {
        std::cout << "Curiosity: " << std::fixed << std::setprecision(3) << curiosity->getLevel() << std::endl;
    }
    
    auto* novelty = brain.getNovelty();
    if (novelty) {
        std::cout << "Novelty: " << std::fixed << std::setprecision(3) << novelty->getLevel() << std::endl;
    }
    
    auto* predictionError = brain.getPredictionError();
    if (predictionError) {
        std::cout << "Prediction Error: " << std::fixed << std::setprecision(3) << predictionError->getError() << std::endl;
    }
}

void VisualizationInterface::showMemoryStats(const Brain& brain) const {
    std::cout << "=== Memory System Statistics ===" << std::endl;
    
    if (brain.getWorkingMemory()) {
        std::cout << "Working Memory:" << std::endl;
        std::cout << "  Active Traces: " << brain.getWorkingMemory()->getActiveTraces() << std::endl;
        std::cout << "  Capacity: " << brain.getWorkingMemory()->getCapacity() << std::endl;
        std::cout << "  Decay Rate: " << brain.getWorkingMemory()->getDecayRate() << std::endl;
    }
    
    if (brain.getEpisodicMemory()) {
        std::cout << "Episodic Memory:" << std::endl;
        std::cout << "  Episodes: " << brain.getEpisodicMemory()->getEpisodeCount() << std::endl;
        std::cout << "  Max Episodes: " << brain.getEpisodicMemory()->getMaxEpisodes() << std::endl;
        
        auto episodes = brain.getEpisodicMemory()->getEpisodesForReplay(5);
        if (!episodes.empty()) {
            std::cout << "  Recent Episodes:" << std::endl;
            for (size_t i = 0; i < episodes.size() && i < 5; ++i) {
                const auto* episode = episodes[i];
                if (episode) {
                    std::cout << "    Episode " << episode->timestamp << ": "
                              << "Reward: " << episode->reward
                              << ", Action: " << static_cast<int>(episode->action)
                              << ", Age: " << episode->age << std::endl;
                }
            }
        }
    }
    
    if (brain.getAssociativeMemory()) {
        std::cout << "Associative Memory:" << std::endl;
        std::cout << "  Associations: " << brain.getAssociativeMemory()->getAssociationCount() << std::endl;
    }
}

void VisualizationInterface::showPerformanceMetrics(const Brain& brain, double wallClockTime) const {
    std::cout << "=== Performance Metrics ===" << std::endl;
    std::cout << "Wall Clock Time: " << std::fixed << std::setprecision(3) << wallClockTime << " seconds" << std::endl;
    std::cout << "Steps per Second: " << std::fixed << std::setprecision(1) << 
        (brain.getTotalSpikeCount() / wallClockTime) << std::endl;
    std::cout << "Neurons per Second: " << std::fixed << std::setprecision(1) << 
        (brain.getTotalNeuronCount() / wallClockTime) << std::endl;
}

void VisualizationInterface::showDevelopmentStatus(const Brain& brain) const {
    std::cout << "=== Development Status ===" << std::endl;
    
    if (brain.getDevelopmentSystem()) {
        std::cout << "Developmental Stage: " << static_cast<int>(brain.getDevelopmentalStage()) << std::endl;
        
        // Show developmental modifiers
        std::cout << "Plasticity Modifiers:" << std::endl;
        std::cout << "  Synaptogenesis Rate: " << std::fixed << std::setprecision(6) << 
            (brain.getStructuralPlasticity() ? brain.getStructuralPlasticity()->getSynaptogenesisRate() : 0.0f) << std::endl;
        
        std::cout << "  Pruning Rate: " << std::fixed << std::setprecision(6) << 
            (brain.getStructuralPlasticity() ? brain.getStructuralPlasticity()->getPruningRate() : 0.0f) << std::endl;
        
        std::cout << "  Overall Plasticity: " << std::fixed << std::setprecision(3) << 
            (brain.getDevelopmentSystem() ? brain.getDevelopmentSystem()->getPlasticityModifier() : 1.0f) << std::endl;
    }
}

void VisualizationInterface::showCheckpointStatus(const Brain& brain) const {
    std::cout << "=== Checkpoint Status ===" << std::endl;
    
    auto* checkpointManager = brain.getCheckpointManager();
    if (checkpointManager) {
        std::cout << "Checkpoint Manager: Available" << std::endl;
        // Note: CheckpointManager doesn't expose these details in current interface
        std::cout << "Checkpoints: " << "Enabled (checkpoints/ directory)" << std::endl;
    } else {
        std::cout << "Checkpoint Manager: Not available" << std::endl;
    }
}

} // namespace nlm
