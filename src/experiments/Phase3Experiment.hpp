#pragma once

/**
 * NLM Phase 3: World Interaction Framework
 * 
 * Phase 3 represents the emergence of world interaction capabilities in the
 * NLM (Neural Learning Machine). This phase demonstrates how artificial
 * developmental systems can engage with and learn from their environment
 * through direct interaction, sensory processing, and motor output.
 * 
 * Phase 3 capabilities include:
 * - Basic sensory perception and processing
 * - Motor control and action execution
 * - Reward-based learning through environmental interaction
 * - Development of motor schemas and behavioral patterns
 * - Integration with world state and environmental dynamics
 * 
 * This phase bridges the gap between internal neural processing and
 * external environmental interaction, establishing the foundation for
 * more complex cognitive and social behaviors in later phases.
 */

#include "../agent/AgentBody.hpp"
#include <string>
#include <vector>
#include <memory>

namespace nlm {

/**
 * ExperimentMetrics struct - Data collection for Phase 3 experiments
 * 
 * Comprehensive metrics collector for Phase 3 world interaction experiments.
 * Records all aspects of agent-environment interaction including sensory
 * processing, motor behavior, reward accumulation, and developmental changes.
 * 
 * This struct provides a standardized way to quantify and analyze the
 * agent's interaction with its environment, supporting both simple
 * performance measurement and complex behavioral analysis.
 * 
 * Key metrics categories:
 * - Episode progress (episode, steps)
 * - Reward acquisition and reinforcement
 * - Neuromodulation signals (novelty, curiosity, energy)
 * - Developmental progression (age, stage)
 * - Neural network state (synapses, spikes, firing rates)
 * - Motor behavior patterns (action counts)
 * 
 * Usage example:
 * @code
 * ExperimentMetrics metrics;
 * metrics.episode = 1;
 * metrics.totalReward = 10.5f;
 * metrics.finalNovelty = 0.7f;
 * metrics.developmentalStage = "Exploration";
 * @endcode
 */
struct ExperimentMetrics {
    // Episode and time tracking
    int episode;                    ///< Current episode number
    int steps;                      ///< Number of steps in current episode
    float totalReward;              ///< Total cumulative reward
    
    // Neuromodulation signals
    float finalEnergy;              ///< Final energy level
    float finalNovelty;             ///< Final novelty signal (exploration drive)
    float finalCuriosity;           ///< Final curiosity level
    float predictionError;          ///< Prediction error signal
    
    // Development tracking
    float developmentalAge;         ///< Agent's developmental age (steps)
    std::string developmentalStage; ///< Current developmental stage
    
    // Neural network state
    size_t totalSynapses;           ///< Total number of synapses
    float averageWeight;            ///< Average synaptic weight
    float maxWeight;                ///< Maximum synaptic weight
    size_t totalSpikes;             ///< Total neural spikes in episode
    float averageFiringRate;        ///< Average firing rate (Hz)
    
    // Motor behavior patterns
    int forwardCount;               ///< Number of forward movements
    int backwardCount;              ///< Number of backward movements
    int turnLeftCount;              ///< Number of left turns
    int turnRightCount;             ///< Number of right turns
    int interactCount;              ///< Number of interactions with environment
    int waitCount;                  ///< Number of wait/idle actions
    
    /**
     * Default constructor
     * 
     * Initializes all metrics to their default/starting values.
     * Useful for resetting metrics between episodes or experiments.
     */
    ExperimentMetrics();
    
    /**
     * Reset metrics to initial state
     * 
     * Resets all metrics to their starting values, preparing the
     * metrics collector for a new episode or experiment.
     */
    void reset();
    
    /**
     * Convert metrics to CSV format
     * 
     * Generates a CSV-formatted string representation of the metrics
     * for easy storage, analysis, or import into external tools.
     * 
     * @return CSV-formatted string with all metric values
     */
    std::string toCSV() const;
};

} // namespace nlm
