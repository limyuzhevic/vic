// Visualization helper functions for updating state from brain systems
namespace VisualizationHelpers {

// Update visualization state from Brain class
void updateBrainVisualization(VisualizationInterface::Impl::BrainState& state, const Brain& brain) {
    state.neuronCount = brain.getTotalNeuronCount();
    state.activeNeuronCount = brain.getActiveNeuronCount();
    state.firingRate = brain.getAverageFiringRate();
    state.totalSpikes = brain.getTotalSpikeCount();
    state.excitationInhibitionRatio = brain.getExcitationInhibitionRatio();
    
    // Get regions for visualization
    const auto& regions = brain.getRegions();
    for (size_t i = 0; i < regions.size() && i < 10; ++i) {
        if (regions[i]) {
            // Get population activity
            state.populationActivity.push_back(std::make_pair(
                PopulationId(static_cast<uint64_t>(i)),
                regions[i]->getAverageActivity()
            ));
        }
    }
}

// Update visualization state from SpikeSystem class
void updateSpikeVisualization(VisualizationInterface::Impl::BrainState& state, const SpikeSystem& spikeSystem) {
    const auto& spikeHistory = spikeSystem.getSpikeHistory();
    
    // Keep only recent spikes (last 1000)
    if (spikeHistory.size() > 1000) {
        state.recentSpikes.clear();
        for (size_t i = spikeHistory.size() - 1000; i < spikeHistory.size(); ++i) {
            const auto& event = spikeHistory[i];
            state.recentSpikes.push_back(SpikeEvent(event.source, event.timestamp, event.step));
        }
    } else {
        state.recentSpikes.clear();
        for (const auto& event : spikeHistory) {
            state.recentSpikes.push_back(SpikeEvent(event.source, event.timestamp, event.step));
        }
    }
}

// Update visualization state from memory systems
void updateMemoryVisualization(VisualizationInterface::Impl::BrainState& state, const Brain& brain) {
    auto* workingMemory = brain.getWorkingMemory();
    if (workingMemory) {
        state.workingMemoryTraces = workingMemory->getActiveTraces();
    }
    
    auto* episodicMemory = brain.getEpisodicMemory();
    if (episodicMemory) {
        state.episodicEpisodes = episodicMemory->getEpisodeCount();
        state.experimentReward = episodicMemory->getAverageReward();
    }
    
    auto* associativeMemory = brain.getAssociativeMemory();
    if (associativeMemory) {
        state.associativeAssociations = associativeMemory->getAssociationCount();
    }
}

// Update visualization state from neuromodulation systems
void updateNeuromodulationVisualization(VisualizationInterface::Impl::BrainState& state, const Brain& brain) {
    auto* dopamine = brain.getDopamine();
    if (dopamine) {
        state.dopamineLevel = dopamine->getLevel();
    }
    
    auto* curiosity = brain.getCuriosity();
    if (curiosity) {
        state.curiosityLevel = curiosity->getLevel();
    }
    
    auto* novelty = brain.getNovelty();
    if (novelty) {
        state.noveltyLevel = novelty->getLevel();
    }
    
    auto* predictionError = brain.getPredictionErrorSignal();
    if (predictionError) {
        state.predictionErrorLevel = predictionError->getLevel();
    }
}

// Update visualization state from development system
void updateDevelopmentVisualization(VisualizationInterface::Impl::BrainState& state, const Brain& brain) {
    auto* developmentSystem = brain.getDevelopmentSystem();
    if (developmentSystem) {
        state.developmentalStage = developmentSystem->getStage();
        state.developmentalAge = developmentSystem->getDevelopmentalAge();
        state.plasticityModifier = developmentSystem->getPlasticityModifier();
    }
}

// Update visualization state from prediction system
void updatePredictionVisualization(VisualizationInterface::Impl::BrainState& state, const Brain& brain) {
    auto* predictionSystem = brain.getPredictionSystem();
    if (predictionSystem) {
        // TODO: Get prediction metrics from prediction system
        // For now, just mark as integrated
        state.predictionIntegrated = true;
    }
}

// Update visualization state from cognition systems
void updateCognitionVisualization(VisualizationInterface::Impl::BrainState& state, const Brain& brain) {
    auto* planner = brain.getPlanner();
    if (planner) {
        state.predictionIntegrated = true; // Mark as integrated
    }
    
    auto* conceptFormation = brain.getConceptFormation();
    if (conceptFormation) {
        state.predictionIntegrated = true; // Mark as integrated
    }
    
    auto* attention = brain.getAttention();
    if (attention) {
        state.predictionIntegrated = true; // Mark as integrated
    }
}

// Calculate average synaptic weight
float calculateAverageSynapticWeight(const Brain& brain) {
    float totalWeight = 0.0f;
    size_t synapseCount = 0;
    
    const auto& regions = brain.getRegions();
    for (const auto& region : regions) {
        if (region) {
            const auto& synapses = region->getSynapses();
            for (const auto& synapse : synapses) {
                totalWeight += synapse->getWeight();
                synapseCount++;
            }
        }
    }
    
    if (synapseCount > 0) {
        return totalWeight / synapseCount;
    }
    
    return 0.0f;
}

} // namespace VisualizationHelpers
