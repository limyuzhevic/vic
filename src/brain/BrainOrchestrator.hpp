#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../performance/CheckpointSystem.hpp"
#include "../core/PoolManager.hpp"
    
    // Update individual subsystems
    
    // ========== CORE NEURAL COMPUTATION ==========
    
    /**
     * Process pending delayed spike events
     * Step 1 from original step()
     */
    void processDelayedSpikeEvents(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * Update all neuron membrane potentials (LIF dynamics)
     * Step 2 from original step()
     */
    void updateNeuronDynamics(Timestamp currentTime, TimestepDuration timestep);
    
    /**
     * Detect spikes and schedule spike events
     * Step 3 from original step()
     */
    void detectAndProcessSpikes(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * Process immediate spikes
     */
    void processImmediateSpikes(SimulationStep currentStep);
    
    // ========== MEMORY SYSTEMS ==========
    
    /**
     * Update working memory (maintenance and competition)
     * Step 4 from original step()
     */
    void updateWorkingMemory(TimestepDuration dt);
    
    // ========== NEUROMODULATION ==========
    
    /**
     * Apply neuromodulation effects on neural excitability
     * Step 5 from original step()
     */
    void applyNeuromodulationEffects();
    
    // ========== PLASTICITY ==========
    
    /**
     * Apply plasticity rules (STDP and Hebbian)
     * Step 6 from original step()
     */
    void applyPlasticityRules(float plasticityMod);
    
    /**
     * Update structural plasticity (synaptogenesis, pruning)
     * Step 11 from original step()
     */
    void updateStructuralPlasticity(SimulationStep currentStep);
    
    // ========== COGNITIVE SYSTEMS ==========
    
    /**
     * Update episodic memory with current experience
     * Step 7 from original step()
     */
    void updateEpisodicMemory(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * Update prediction system
     * Step 8 from original step()
     */
    void updatePredictionSystem();
    
    /**
     * Update attention system
     * Step 9 from original step()
     */
    void updateAttentionSystem();
    
    /**
     * Update concept formation
     * Step 10 from original step()
     */
    void updateConceptFormation();
    
    // ========== DEVELOPMENT AND MEMORY ==========
    
    /**
     * Replay important memories
     * Step 12 from original step()
     */
    void replayMemories(SimulationStep currentStep);
    
    /**
     * Apply development effects
     * Step 13 from original step()
     */
    void applyDevelopmentEffects(SimulationStep currentStep);
    
    /**
     * Periodic memory consolidation
     * Step 14 from original step()
     */
    void consolidateMemories(SimulationStep currentStep);
    
    // ========== CHECKPOINTING ==========
    
    /**
     * Checkpoint management
     * Step 15 from original step()
     */
    void updateCheckpointManagement(SimulationStep currentStep, Timestamp currentTime);
    
    // ========== SPECIFIC SYSTEM UPDATES ==========
    
    /**
     * Apply neuromodulation signals to system
     */
    void applyNeuromodulation(const Neuromodulator& signal);
    
    /**
     * Update plasticity rules (called automatically in step)
     */
    void updatePlasticity();
    
    /**
     * Apply developmental changes (called automatically in step)
     */
    void develop();
    
    // ========== DEVELOPMENT EFFECTS ==========
    
    /**
     * Apply developmental effects on plasticity rates
     */
    void applyDevelopmentToPlasticity(DevelopmentalStage stage);
    
    /**
     * Get developmental stage
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * Set developmental stage
     */
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== PLACILITY RULES ==========
    
    /**
     * Apply neuromodulation effects on plasticity
     */
    void applyNeuromodulationToPlasticity(float modulation);
    
    // ========== REPLAY AND CONSOLIDATION ==========
    
    /**
     * Get episodes for replay
     */
    std::vector<const EpisodicMemoryItem*> getEpisodesForReplay(size_t count) const;
    
    /**
     * Replay episode
     */
    void replayEpisode(const EpisodicMemoryItem* episode);
    
    /**
     * Consolidate memories
     */
    void consolidateMemories(float threshold);
    
    // ========== STATISTICS ==========
    
    /**
     * Collect statistics
     */
    void collectStatistics();
    
    /**
     * Get excitation/inhibition ratio
     */
    float getExcitationInhibitionRatio() const;
    
    /**
     * Get total spike count
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * Get pending spike event count
     */
    size_t getPendingSpikeEventCount() const;
    
    // ========== ACCESSORS ==========
    
    /**
     * Add a sensory neuron for input injection
     */
    void addSensoryNeuron(Neuron* neuron) { pImpl->sensoryNeurons.push_back(neuron); }
    
    /**
     * Add a motor neuron for output
     */
    void addMotorNeuron(Neuron* neuron) { pImpl->motorNeurons.push_back(neuron); }
    
    /**
     * Get next region ID
     */
    RegionId getNextRegionId() const { return pImpl->nextRegionId; }
    
    /**
     * Add a region (owned by orchestrator)
     */
    void addRegion(std::unique_ptr<NeuralRegion> region) {
        region->setId(RegionId(pImpl->nextRegionId++));
        pImpl->regions.push_back(std::move(region));
    }
    
    /**
     * Get a region by ID
     */
    NeuralRegion* getRegion(RegionId id) {
        for (auto& region : pImpl->regions) {
            if (region->getId() == id) {
                return region.get();
            }
        }
        return nullptr;
    }
    
    const NeuralRegion* getRegion(RegionId id) const {
        for (const auto& region : pImpl->regions) {
            if (region->getId() == id) {
                return region.get();
            }
        }
        return nullptr;
    }
    
    /**
     * Get region count
     */
    size_t getRegionCount() const { return pImpl->regions.size(); }
    
    /**
     * Get all region IDs
     */
    std::vector<RegionId> getRegionIds() const {
        std::vector<RegionId> ids;
        ids.reserve(pImpl->regions.size());
        for (const auto& region : pImpl->regions) {
            ids.push_back(region->getId());
        }
        return ids;
    }
    
    /**
     * Add inter-region connection
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1) {
        pImpl->interRegionConnections.emplace_back(source, target, weight, delay);
    }
    
    /**
     * Remove inter-region connection
     */
    void removeInterRegionConnection(RegionId source, RegionId target) {
        pImpl->interRegionConnections.erase(
            std::remove_if(pImpl->interRegionConnections.begin(),
                          pImpl->interRegionConnections.end(),
                          [source, target](const InterRegionConnection& conn) {
                              return conn.sourceRegion == source && conn.targetRegion == target;
                          }),
            pImpl->interRegionConnections.end()
        );
    }
    
    /**
     * Get inter-region connections
     */
    const std::vector<InterRegionConnection>& getInterRegionConnections() const {
        return pImpl->interRegionConnections;
    }
    
    /**
     * Get total neuron count
     */
    size_t getTotalNeuronCount() const {
        size_t total = 0;
        for (const auto& region : pImpl->regions) {
            total += region->getTotalNeuronCount();
        }
        return total;
    }
    
    /**
     * Get total synapse count
     */
    size_t getTotalSynapseCount() const {
        size_t total = 0;
        for (const auto& region : pImpl->regions) {
            total += region->getSynapseCount();
        }
        total += pImpl->interRegionConnections.size();
        return total;
    }
    
    /**
     * Get active neuron count
     */
    size_t getActiveNeuronCount() const {
        size_t total = 0;
        for (const auto& region : pImpl->regions) {
            total += region->getActiveNeuronCount();
        }
        return total;
    }
    
    /**
     * Get firing neuron count
     */
    size_t getFiringNeuronCount() const {
        return pImpl->totalSpikesThisStep;
    }
    
    /**
     * Get average firing rate
     */
    float getAverageFiringRate() const {
        if (pImpl->regions.empty()) return 0.0f;
        float sum = 0.0f;
        for (const auto& region : pImpl->regions) {
            sum += region->getAverageFiringRate();
        }
        return sum / static_cast<float>(pImpl->regions.size());
    }
    
private:
    struct Impl;
    Impl* pImpl;
    
    Brain* brain_;
    std::unique_ptr<ProcessingStrategy> currentStrategy_;
};

} // namespace nlm
