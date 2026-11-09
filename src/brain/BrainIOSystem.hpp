#pragma once

#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include "BrainUtils.hpp"

namespace nlm {

// BrainIOSystem implementation - I/O and control systems
// Extracted from Brain.cpp lines 694-870 and I/O functionality

class BrainIOSystem {
public:
    // Process sensory input from environment
    static void processSensoryInput(Brain::Impl* impl, const SensoryInput& input);
    
    // Inject current directly into a specific neuron
    static void injectCurrent(Brain::Impl* impl, NeuronId neuron, MembranePotential current);
    
    // Inject current into all neurons of a specific type
    static void injectCurrentToNeurons(Brain::Impl* impl, NeuronType type, MembranePotential current);
    
    // Process motor/action output based on motor neuron activity
    static std::unique_ptr<Action> produceAction(Brain::Impl* impl);
    
    // Get spike system access
    static SpikeSystem* getSpikeSystem(Brain::Impl* impl);
    
    // Calculate excitation/inhibition ratio
    static float getExcitationInhibitionRatio(Brain::Impl* impl);
    
    // Statistics accessors
    static size_t getTotalSpikeCount(Brain::Impl* impl);
    static size_t getPendingSpikeEventCount(Brain::Impl* impl);
    static size_t getTotalNeuronCount(Brain::Impl* impl);
    static size_t getTotalSynapseCount(Brain::Impl* impl);
    static size_t getActiveNeuronCount(Brain::Impl* impl);
    static size_t getFiringNeuronCount(Brain::Impl* impl);
    static float getAverageFiringRate(Brain::Impl* impl);
    static float getLearnedPreference(Brain::Impl* impl);
    static void setLearnedPreference(Brain::Impl* impl, float preference);
    
    // Save brain state to file (checkpointing)
    static bool save(Brain::Impl* impl, const std::string& filepath);
    
    // Load brain state from file
    static bool load(Brain::Impl* impl, const std::string& filepath);
    
    // Reset brain state
    static void reset(Brain::Impl* impl);
    
    // Apply neuromodulatory signals
    static void applyNeuromodulation(Brain::Impl* impl, const Neuromodulator& signal);
    
    // Update plasticity rules (called automatically in step)
    static void updatePlasticity(Brain::Impl* impl);
    
    // Apply developmental changes (called automatically in step)
    static void develop(Brain::Impl* impl);
    
    // Get statistics
    static void logStatus(Brain::Impl* impl);
    
    // Setters and getters for integration state
    static void setDevelopmentalStage(Brain::Impl* impl, DevelopmentalStage stage);
    static DevelopmentalStage getDevelopmentalStage(Brain::Impl* impl);
    static bool isResting(Brain::Impl* impl);
    static void setResting(Brain::Impl* impl, bool resting);
    static float getTimestep(Brain::Impl* impl);
    static size_t getStepsSinceLastEpisode(Brain::Impl* impl);
    static void setStepsSinceLastEpisode(Brain::Impl* impl, size_t steps);
    static float getLearnedPreference(Brain::Impl* impl);
    static void setLearnedPreference(Brain::Impl* impl, float preference);
};

} // namespace nlm
