#pragma once

/**
 * LearningExperiment: Demonstrates measurable synaptic changes through experience
 * 
 * This experiment captures and analyzes synaptic weight changes over time to measure
 * learning in the neural network. It records initial and final synaptic weights,
 * counts active neurons, and computes statistics to determine if learning has occurred.
 * 
 * Key features:
 * - Records initial and final synaptic weight distributions
 * - Tracks the most active neurons throughout the experiment
 * - Computes detailed statistics on synaptic changes (strengthened/weakened/unchanged)
 * - Detects and reports measurable learning through weight changes
 * 
 * Used by:
 * - runPlasticityExperiment(): Main plasticity learning experiment
 * - runStdpVerification(): Tests STDP (Spike-Timing Dependent Plasticity) learning
 * 
 * The experiment is designed to be reusable across different experiment types,
 * providing consistent analysis of synaptic plasticity and learning.
 */

#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Types/Types.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "dynamics/SpikeSystem.hpp"
#include <memory>
#include <string>
#include <vector>
#include <numeric>

namespace nlm {

/**
 * LearningExperiment captures and analyzes synaptic weight changes to measure learning.
 * 
 * This struct is designed to be reusable across different experiment types that need to
 * measure synaptic plasticity and learning. It provides a standardized way to:
 * - Record the initial state of the neural network
 * - Apply learning stimuli (like plasticity or STDP)
 * - Record the final state after learning
 * - Compute detailed statistics on synaptic changes
 * 
 * The struct tracks synaptic weights from the first brain region (RegionId(1)),
 * counts total synapses, and monitors the most active neurons to correlate activity
 * with weight changes.
 */
struct LearningExperiment {
    /**
     * Pointer to the brain being experimented on.
     */
    std::shared_ptr<Brain> brain;
    
    /**
     * Random seed for reproducible experiments.
     */
    uint64_t seed;
    
    /**
     * Number of synapses at the start of the experiment.
     */
    size_t initialSynapseCount;
    
    /**
     * Record of synaptic weights at experiment start.
     * Only records weights from the first region (RegionId(1)).
     */
    std::vector<float> initialWeights;
    
    /**
     * Record of synaptic weights at experiment end.
     * Only records weights from the first region (RegionId(1)).
     */
    std::vector<float> finalWeights;
    
    /**
     * IDs of the most active neurons during the experiment.
     * The number of neurons recorded is determined by the caller (typically 10).
     */
    std::vector<NeuronId> mostActiveNeurons;
    
    /**
     * Constructor initializes the experiment with a brain and random seed.
     * 
     * @param b Pointer to the brain to experiment on
     * @param s Random seed for reproducibility
     */
    LearningExperiment(std::shared_ptr<Brain> b, uint64_t s) 
        : brain(b), seed(s), initialSynapseCount(0) {}
    
    /**
     * Records the initial state of the neural network before learning.
     * 
     * This records:
     * - Total synapse count
     * - Initial weights from the first region (RegionId(1))
     * - Logs the initial statistics
     */
    void recordInitialState() {
        initialSynapseCount = brain->getTotalSynapseCount();
        initialWeights.clear();
        
        // Record initial weights from first region
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                initialWeights.push_back(syn->getWeight());
            }
        }
        
        NLM_LOG_INFO("Initial state recorded:");
        NLM_LOG_INFO("  Synapses: " + std::to_string(initialSynapseCount));
        if (!initialWeights.empty()) {
            float sum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
            float mean = sum / initialWeights.size();
            NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
        }
    }
    
    /**
     * Records the final state of the neural network after learning.
     * 
     * This records:
     * - Final weights from the first region (RegionId(1))
     * - Most active neurons during the experiment
     * - Total spike count
     * - Final statistics
     */
    void recordFinalState() {
        finalWeights.clear();
        
        // Record final weights from first region
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                finalWeights.push_back(syn->getWeight());
            }
        }
        
        mostActiveNeurons = brain->getSpikeSystem()->getMostActiveNeurons(10);
        
        NLM_LOG_INFO("Final state recorded:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        if (!finalWeights.empty()) {
            float sum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
            float mean = sum / finalWeights.size();
            NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
        }
    }
    
    /**
     * Computes and logs detailed statistics about synaptic changes.
     * 
     * This function computes:
     * - Mean initial and final weights
     * - Weight change (strengthening/weakening)
     * - Counts of strengthened, weakened, and unchanged synapses
     * - Total spike activity
     * - Whether learning occurred (based on significant weight changes)
     */
    void computeStatistics() {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Learning Experiment Results ===");
        NLM_LOG_INFO("");
        
        if (initialWeights.empty() || finalWeights.empty()) {
            NLM_LOG_INFO("ERROR: No weights recorded");
            return;
        }
        
        // Compute weight changes
        float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
        float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
        float initialMean = initialSum / initialWeights.size();
        float finalMean = finalSum / finalWeights.size();
        
        NLM_LOG_INFO("Weight Statistics:");
        NLM_LOG_INFO("  Initial mean weight: " + std::to_string(initialMean));
        NLM_LOG_INFO("  Final mean weight: " + std::to_string(finalMean));
        NLM_LOG_INFO("  Change: " + std::to_string(finalMean - initialMean));
        
        // Count synapses that changed significantly
        size_t strengthened = 0;
        size_t weakened = 0;
        size_t unchanged = 0;
        
        size_t minSize = std::min(initialWeights.size(), finalWeights.size());
        for (size_t i = 0; i < minSize; ++i) {
            float delta = finalWeights[i] - initialWeights[i];
            if (delta > 0.01f) ++strengthened;
            else if (delta < -0.01f) ++weakened;
            else ++unchanged;
        }
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Synaptic Changes:");
        NLM_LOG_INFO("  Strengthened: " + std::to_string(strengthened));
        NLM_LOG_INFO("  Weakened: " + std::to_string(weakened));
        NLM_LOG_INFO("  Unchanged: " + std::to_string(unchanged));
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Spike Activity:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        NLM_LOG_INFO("  Most active neurons recorded: " + std::to_string(mostActiveNeurons.size()));
        
        // Determine if learning occurred
        bool learningOccurred = (std::abs(finalMean - initialMean) > 0.001f) ||
                                (strengthened > 0 || weakened > 0);
        
        NLM_LOG_INFO("");
        if (learningOccurred) {
            NLM_LOG_INFO("✓ LEARNING DETECTED: Synaptic weights changed through experience");
        } else {
            NLM_LOG_INFO("✗ NO LEARNING: Weights did not change significantly");
        }
    }
};

} // namespace nlm