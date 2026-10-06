// Copyright (c) 2024+ NLM Project
// All rights reserved.
// 
// This file is part of the Neural Learning Machine (NLM) project.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
// 3. Neither the name of the copyright holder nor the names of its contributors
//    may be used to endorse or promote products derived from this software
//    without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

/**
 * @file WorkingMemory.hpp
 * @brief Neural Working Memory System for transient information storage
 * 
 * This file implements the WorkingMemory class, which provides a neural-based
 * working memory system for maintaining active information representations.
 * 
 * The WorkingMemory system is built upon the principles of:
 * - Persistent neural activity (reverberating activity loops)
 * - Synaptic facilitation and short-term plasticity
 * - Population coding for distributed representations
 * - Competitive dynamics for selective attention
 * 
 * @author NLM Project
 * @version 1.0.0
 * @date 2024-2026
 * @copyright NLM Project (2024+)
 * 
 * @see src/memory/NeuralWorkingMemory.hpp - Neural-based working memory implementation
 * @see src/core/Types/Types.hpp - Memory-related type definitions
 * @see src/brain/Neuron.hpp - Neuron class for active storage
 * @see src/plasticity/STDP.hpp - Plasticity rules for memory maintenance
 * @see src/neuromodulation/Neuromodulator.hpp - Neuromodulatory control signals
 */

#pragma once

// NLM filename guard
#ifndef NLM_WORKINGMEMORY_HPP
#define NLM_WORKINGMEMORY_HPP

#include "src/core/Types/Types.hpp"
#include "src/brain/Neuron.hpp"
#include "src/plasticity/STDP.hpp"
#include "src/neuromodulation/Neuromodulator.hpp"
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <optional>

namespace nlm {

/**
 * @class WorkingMemory
 * @brief Main working memory system for temporary information storage and processing
 * 
 * The WorkingMemory class implements a hybrid neural-symbolic working memory system
 * that combines neural representations with symbolic operations for efficient
 * maintenance and manipulation of information.
 * 
 * Key Features:
 * - Multi-modal storage (neural patterns, symbolic representations)
 * - Adaptive capacity based on cognitive load
 * - Dynamic decay and interference management
 * - Attention-based selection and prioritization
 * - Integration with neural plasticity mechanisms
 * 
 * The system supports different memory types:
 * - Item memory: individual facts and values
 * - Event memory: sequential experiences and transitions
 * - Procedural memory: learned actions and routines
 * - Associative memory: relationships and connections
 * 
 * Usage Example:
 * @code
 * #include "src/core/Config/Config.hpp"
 * #include "src/memory/WorkingMemory.hpp"
 * 
 * int main() {
 *     // Initialize working memory with configuration
 *     auto config = std::make_shared<Config>();
 *     config->set("working_memory.capacity", 1000);
 *     config->set("working_memory.decay_rate", 0.01f);
 *     
 *     // Create working memory instance
 *     WorkingMemory wm;
 *     wm.initialize(config);
 *     
 *     // Store information
 *     wm.store(NeuronId(42), 3.14f);  // Store PI value for neuron 42
 *     wm.storePattern(std::vector<float>{0.5f, 0.3f, 0.8f});
 *     
 *     // Retrieve and process
 *     float value = wm.retrieve(NeuronId(42));
 *     auto pattern = wm.getCurrentPattern();
 *     
 *     // Update memory (apply decay and interference)
 *     wm.update(0.016f);  // 16ms timestep
 *     
 *     // Get statistics
 *     size_t active_items = wm.getActiveItemCount();
 *     float utilization = wm.getMemoryUtilization();
 *     
 *     return 0;
 * }
 * @endcode
 * 
 * @see NeuralWorkingMemory - Neural-based working memory implementation
 * @see EpisodicMemory - Event-based memory storage
 * @see SemanticMemory - Knowledge-based memory
 * @see ProceduralMemory - Action-based memory
 */

class WorkingMemory {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a working memory instance with default parameters:
     * - Capacity: 1000 items
     * - Decay rate: 0.01 per timestep
     * - Interference strength: 0.5
     * - Competition enabled
     * 
     * @note Call initialize() after construction to configure with proper
     *       parameters and register with the neural system.
     */

    WorkingMemory();

    /**
     * @brief Virtual destructor
     * 
     * Cleans up working memory resources and removes registered callbacks.
     */

    virtual ~WorkingMemory();

    /**
     * @brief Disable copying - working memory cannot be copied
     * 
     * Working memory maintains internal state and references that make
     * copying unsafe and semantically incorrect.
     */

    WorkingMemory(const WorkingMemory&) = delete;
    WorkingMemory& operator=(const WorkingMemory&) = delete;

    /**
     * @brief Enable move construction
     * 
     * Allows moving ownership of the working memory state from one instance
     * to another. The source instance is left in a valid but unspecified state.
     */

    WorkingMemory(WorkingMemory&&) noexcept;
    WorkingMemory& operator=(WorkingMemory&&) noexcept;

    /**
     * @brief Initialize working memory with configuration
     * 
     * Sets up the working memory system with parameters from the configuration
     * object. This must be called before any other operations.
     * 
     * @param config Shared pointer to configuration object containing
     *              working memory parameters
     * @param brain Pointer to the main brain instance for neural integration
     * @throw std::invalid_argument if config is null
     * 
     * Configuration parameters supported:
     * - "working_memory.capacity": Maximum number of items (default: 1000)
     * - "working_memory.decay_rate": Decay per timestep (default: 0.01)
     * - "working_memory.interference_strength": Memory interference factor (default: 0.5)
     * - "working_memory.enable_competition": Enable competition (default: true)
     * - "working_memory.pattern_length": Length of neural patterns (default: 64)
     * 
     * @see Config.hpp for configuration details
     * @see Brain.hpp for brain integration
     */

    void initialize(std::shared_ptr<const Config> config, Brain* brain = nullptr);

    /**
     * @brief Store a value associated with a neuron
     * 
     * Encodes information as neural activation patterns and stores them
     * in working memory with the specified strength.
     * 
     * @param neuron Neuron ID that will store the information
     * @param value Information value to store (must be finite)
     * @param strength Memory strength factor (default: 1.0f)
     * @param decay_factor Additional decay rate (default: 1.0f)
     * 
     * @throw std::invalid_argument if neuron ID is invalid
     * @throw std::invalid_argument if value is not finite
     * 
     * The storage process involves:
     * 1. Pattern encoding (convert value to neural activation pattern)
     * 2. Synaptic potentiation (strengthen connections to neuron)
     * 3. Memory consolidation (transfer to persistent traces)
     * 4. Interference management (apply to competing memories)
     * 
     * @code
     * NeuronId neuronId(42);
     * wm.store(neuronId, 3.14f);  // Store PI value
     * wm.store(neuronId, -1.0f, 0.5f);  // Store with 50% strength
     * @endcode
     */

    void store(NeuronId neuron, float value, float strength = 1.0f, float decay_factor = 1.0f);

    /**
     * @brief Store a complex pattern of neural activations
     * 
     * Stores a distributed representation as a vector of neural activations,
     * allowing for complex information storage beyond single values.
     * 
     * @param pattern Vector of activation values representing the information
     * @param strength Memory strength factor (default: 1.0f)
     * @param timestamp Timestamp when pattern was stored (default: current)
     * 
     * @throw std::invalid_argument if pattern is empty or contains NaN/inf values
     * 
     * The pattern storage involves:
     * - Distributed representation in neural population
     * - Pattern stabilization via recurrent connections
     * - Noise reduction and pattern completion
     * - Pattern separation from similar memories
     * 
     * @code
     * std::vector<float> pattern{0.5f, 0.3f, 0.8f, 0.1f, 0.9f};
     * wm.storePattern(pattern);
     * @endcode
     */

    void storePattern(const std::vector<float>& pattern, float strength = 1.0f, Timestamp timestamp = 0.0);

    /**
     * @brief Store specific neuron activation directly
     * 
     * Stores activation levels for individual neurons, bypassing pattern
     * encoding for direct neural manipulation.
     * 
     * @param neuron Neuron ID to store activation for
     * @param activation Neural activation level (typically [0, 1])
     * @param force If true, overwrite existing activation regardless of
     *              competition (default: false)
     * 
     * @throw std::invalid_argument if neuron ID is invalid
     * @throw std::invalid_argument if activation is not in [0, 1] range
     * 
     * This method is useful for:
     * - Direct neural control
     * - Testing memory mechanisms
     * - Debugging working memory behavior
     */

    void storeToNeuron(NeuronId neuron, float activation, bool force = false);

    /**
     * @brief Retrieve a value stored for a specific neuron
     * 
     * Extracts the information value associated with the specified neuron,
     * performing pattern completion if necessary.
     * 
     * @param neuron Neuron ID to retrieve value from
     * @return Stored value, or NaN if no value found
     * 
     * @throw std::invalid_argument if neuron ID is invalid
     * 
     * The retrieval process involves:
     * 1. Checking for direct value storage
     * 2. Pattern completion from neural representation
     * 3. Interpolation between similar patterns
     * 4. Interference recovery (if interfered)
     * 
     * @code
     * float value = wm.retrieve(NeuronId(42));
     * if (std::isnan(value)) {
     *     // No value stored for this neuron
     * }
     * @endcode
     */

    float retrieve(NeuronId neuron) const;

    /**
     * @brief Retrieve the current working memory pattern
     * 
     * Returns the dominant neural pattern currently active in working memory,
     * which represents the most salient information.
     * 
     * @return Current neural pattern, empty if no pattern stored
     * 
     * The pattern is computed as:
     * 1. Sum all stored patterns
     * 2. Apply winner-take-all competition
     * 3. Normalize to [0, 1] range
     * 4. Apply decay to weaker patterns
     */

    std::vector<float> retrievePattern() const;

    /**
     * @brief Check if a neuron has stored information
     * 
     * Queries whether the specified neuron is currently part of working memory
     * content, either through direct storage or pattern participation.
     * 
     * @param neuron Neuron ID to check
     * @return true if neuron contains stored information, false otherwise
     * 
     * @throw std::invalid_argument if neuron ID is invalid
     */

    bool contains(NeuronId neuron) const;

    /**
     * @brief Get activation level of a specific neuron in working memory
     * 
     * Returns the current activation level of the specified neuron within
     * the working memory representation.
     * 
     * @param neuron Neuron ID to query
     * @return Activation level in range [0, 1], or -1.0 if neuron not in memory
     * 
     * @throw std::invalid_argument if neuron ID is invalid
     */

    float getNeuronActivation(NeuronId neuron) const;

    /**
     * @brief Check if working memory is empty
     * 
     * @return true if no information is stored, false otherwise
     */

    bool isEmpty() const;

    /**
     * @brief Get the number of items currently stored
     * 
     * @return Current number of active memory items
     */

    size_t getItemCount() const;

    /**
     * @brief Get the maximum capacity
     * 
     * @return Maximum number of items that can be stored
     */

    size_t getCapacity() const;

    /**
     * @brief Set memory capacity
     * 
     * Adjusts the maximum number of items that can be stored. This may
     * trigger automatic cleanup of oldest/lowest priority items.
     * 
     * @param capacity New maximum capacity
     * @param force If true, immediately clear oldest items to make room
     *              (default: false)
     * 
     * @throw std::invalid_argument if capacity is zero
     */

    void setCapacity(size_t capacity, bool force = false);

    /**
     * @brief Get current memory utilization ratio
     * 
     * @return Utilization as fraction in range [0, 1]
     */

    float getMemoryUtilization() const;

    /**
     * @brief Get decay rate
     * 
     * @return Decay rate per timestep (typically in [0, 1])
     */

    float getDecayRate() const;

    /**
     * @brief Set decay rate
     * 
     * @param rate New decay rate (typically clamped to [0, 1])
     */

    void setDecayRate(float rate);

    /**
     * @brief Get interference strength
     * 
     * @return Interference factor in range [0, 1]
     */

    float getInterferenceStrength() const;

    /**
     * @brief Set interference strength
     * 
     * @param strength New interference strength (typically clamped to [0, 1])
     */

    void setInterferenceStrength(float strength);

    /**
     * @brief Update working memory state
     * 
     * Advances the working memory by one timestep, applying decay, interference,
     * and competition mechanisms. This should be called regularly during
     * simulation.
     * 
     * @param dt Time step duration in seconds
     * 
     * The update process involves:
     * 1. Apply decay to all stored items
     * 2. Apply interference between competing items
     * 3. Update recurrent connections
     * 4. Run competition for winner selection
     * 5. Apply neuromodulatory signals
     * 6. Check for consolidation to long-term memory
     * 
     * @code
     * for (size_t i = 0; i < total_steps; ++i) {
     *     wm.update(0.016f);  // 16ms timestep
     *     // ... process other systems
     * }
     * @endcode
     */

    void update(TimestepDuration dt);

    /**
     * @brief Clear all working memory
     * 
     * Removes all stored information and resets the working memory to
     * its initial empty state.
     */

    void clear();

    /**
     * @brief Get all active neurons in working memory
     * 
     * @return Vector of Neuron IDs that are part of current memory content
     */

    const std::vector<NeuronId>& getActiveNeurons() const;

    /**
     * @brief Get current memory trace strength distribution
     * 
     * @return Vector of trace strengths (one per active item)
     */

    const std::vector<float>& getTraceStrengths() const;

    /**
     * @brief Get memory trace timestamps
     * 
     * @return Vector of timestamps when each trace was created
     */

    const std::vector<Timestamp>& getTraceTimestamps() const;

    /**
     * @brief Apply external boost to working memory
     * 
     * Enhances memory strength for all items, useful for attention
     * or learning events.
     * 
     * @param factor Boost factor (1.0 = no change, >1.0 = strengthen)
     */

    void applyBoost(float factor);

    /**
     * @brief Apply external suppression to working memory
     * 
     * Reduces memory strength for all items, useful for forgetting
     * or interference events.
     * 
     * @param factor Suppression factor (1.0 = no change, <1.0 = weaken)
     */

    void applySuppression(float factor);

    /**
     * @brief Get working memory statistics
     * 
     * @return Structure containing comprehensive working memory statistics
     * 
     * Statistics include:
     * - Item count and capacity
     * - Memory utilization
     * - Decay and interference statistics
     * - Pattern statistics (length, variance, etc.)
     * - Activity levels and trends
     */

    struct MemoryStats {
        size_t itemCount = 0;              ///< Number of active items
        size_t capacity = 0;               ///< Maximum capacity
        float utilization = 0.0f;          ///< Memory utilization ratio [0, 1]
        float decayRate = 0.0f;            ///< Current decay rate
        float interferenceStrength = 0.0f; ///< Current interference strength
        float averageStrength = 0.0f;      ///< Average trace strength
        float minStrength = 0.0f;          ///< Minimum trace strength
        float maxStrength = 0.0f;          ///< Maximum trace strength
        size_t patternLength = 0;          ///< Current pattern length
        float patternVariance = 0.0f;      ///< Pattern variance
        float patternEnergy = 0.0f;        ///< Total pattern energy
        Timestamp oldestTimestamp = 0.0;   ///< Age of oldest trace (steps)
        Timestamp newestTimestamp = 0.0;   ///< Age of newest trace (steps)
    };

    MemoryStats getStats() const;

    /**
     * @brief Check if working memory has been initialized
     * 
     * @return true if working memory has been properly initialized
     */

    bool isInitialized() const;

    /**
     * @brief Get the associated brain instance
     * 
     * @return Pointer to brain instance, or nullptr if not initialized
     */

    Brain* getBrain() const;

    /**
     * @brief Set callback for memory update events
     * 
     * Registers a callback function that will be called whenever working
     * memory undergoes significant updates (store, retrieve, update, clear).
     * 
     * @param callback Function to call with memory statistics before update
     * @note Callback is called from the update thread, not from constructor/destructor
     */

    void setUpdateCallback(std::function<void(const MemoryStats&)> callback);

    /**
     * @brief Get working memory configuration
     * 
     * @return Shared pointer to configuration object
     */

    std::shared_ptr<const Config> getConfig() const;

    /**
     * @brief Synchronize with external knowledge source
     * 
     * Fetches new information from an external source and integrates it
     * into working memory, applying appropriate decay and interference.
     * 
     * @param source Information source identifier
     * @param data Information data to integrate
     * @param priority Priority of incoming information (higher = more important)
     * 
     * This is useful for:
     * - Sensory input processing
     * - Long-term memory consolidation
     * - Learning system integration
     * - External data streams
     */

    void synchronize(const std::string& source, const std::vector<float>& data, float priority = 1.0f);

    /**
     * @brief Generate prediction for next memory state
     * 
     * Uses current memory state and dynamics to predict the next state,
     * useful for anticipation and planning.
     * 
     * @param dt Prediction horizon in seconds
     * @return Predicted memory pattern
     */

    std::vector<float> predictNextState(TimestepDuration dt) const;

    /**
     * @brief Perform consolidation to long-term memory
     * 
     * Moves stable, high-priority memory items to long-term storage
     * based on relevance and stability criteria.
     * 
     * @param relevanceThreshold Minimum relevance score for consolidation
     * @return Number of items consolidated
     */

    size_t consolidate(float relevanceThreshold = 0.7f);

    /**
     * @brief Freeze memory items
     * 
     * Prevents decay of specific memory items, useful for critical
     * information that must be preserved.
     * 
     * @param neurons Neuron IDs to freeze
     */

    void freezeMemory(const std::vector<NeuronId>& neurons);

    /**
     * @brief Unfreeze memory items
     * 
     * Re-enables decay for specific memory items.
     * 
     * @param neurons Neuron IDs to unfreeze
     */

    void unfreezeMemory(const std::vector<NeuronId>& neurons);

    /**
     * @brief Check if memory item is frozen
     * 
     * @param neuron Neuron ID to check
     * @return true if memory for this neuron is frozen
     */

    bool isMemoryFrozen(NeuronId neuron) const;

private:
    /**
     * @brief Initialize implementation details
     * 
     * Sets up the internal implementation structure and allocates
     * necessary resources for working memory operations.
     */

    void initializeImpl();

    /**
     * @brief Clean up implementation details
     * 
     * Releases all allocated resources and cleans up internal
     * implementation structures.
     */

    void cleanupImpl();

    /**
     * @brief Encode value to neural pattern
     * 
     * Converts a scalar value to a distributed neural representation
     * using population coding.
     * 
     * @param value Scalar value to encode
     * @return Neural pattern vector
     */

    std::vector<float> encodeValue(float value) const;

    /**
     * @brief Decode neural pattern to value
     * 
     * Extracts a scalar value from a neural pattern using decoding
     * algorithms.
     * 
     * @param pattern Neural pattern to decode
     * @return Decoded scalar value
     */

    float decodePattern(const std::vector<float>& pattern) const;

    /**
     * @brief Apply memory decay
     * 
     * Decays memory trace strengths according to decay rate and
     * interference mechanisms.
     * 
     * @param dt Time step duration
     */

    void applyDecay(TimestepDuration dt);

    /**
     * @brief Apply memory interference
     * 
     * Applies interference between competing memory items, reducing
     * strengths of items that are similar or close in time.
     */

    void applyInterference();

    /**
     * @brief Run competition for winner selection
     * 
     * Selects winning memory traces through competitive dynamics,
     * strengthening winners and weakening losers.
     */

    void runCompetition();

    /**
     * @brief Update recurrent connections
     * 
     * Maintains persistent activity through recurrent synaptic
     * connections between neurons storing related information.
     */

    void updateRecurrentConnections();

    /**
     * @brief Create recurrent connection
     * 
     * Establishes a recurrent connection between two neurons for
     * maintaining information stability.
     * 
     * @param from Source neuron ID
     * @param to Target neuron ID
     * @param strength Connection strength
     */

    void createRecurrentConnection(NeuronId from, NeuronId to, float strength);

    /**
     * @brief Decay weak traces
     * 
     * Identifies and removes weak memory traces that have decayed
     * below a threshold, freeing up memory capacity.
     */

    void decayWeakTraces();

    /**
     * @brief Update neuromodulatory signals
     * 
     * Applies neuromodulatory signals from the brain system to
     * modulate working memory dynamics.
     */

    void updateNeuromodulation();

    /**
     * @brief Compute memory relevance
     * 
     * Computes relevance scores for memory items based on stability,
     * strength, and age.
     * 
     * @param index Item index
     * @return Relevance score in range [0, 1]
     */

    float computeRelevance(size_t index) const;

    /**
     * @brief Merge overlapping memory patterns
     * 
     * Merges similar patterns to reduce redundancy and improve efficiency.
     * 
     * @param pattern1 First pattern
     * @param pattern2 Second pattern
     * @return Merged pattern
     */

    std::vector<float> mergePatterns(const std::vector<float>& pattern1,
                                     const std::vector<float>& pattern2) const;

    /**
     * @brief Pattern completion algorithm
     * 
     * Completes partial patterns by interpolating between similar stored patterns.
     * 
     * @param partial_pattern Partial pattern to complete
     * @return Completed pattern
     */

    std::vector<float> completePattern(const std::vector<float>& partial_pattern) const;

    // Implementation details structure
    struct Impl;
    std::unique_ptr<Impl> pImpl_;

    // Configuration and brain reference
    std::shared_ptr<const Config> config_;
    Brain* brain_ = nullptr;

    // Memory content
    std::vector<NeuronId> memoryNeurons_;
    std::vector<float> memoryValues_;
    std::vector<Timestamp> memoryTimestamps_;
    std::vector<float> memoryStrengths_;

    // Frozen state tracking
    std::vector<bool> memoryFrozen_;

    // Current pattern
    std::vector<float> currentPattern_;
    Timestamp patternTimestamp_ = 0.0;

    // Active trace management
    std::vector<size_t> activeTraceIndices_;

    // Recurrent connections
    std::vector<std::pair<NeuronId, NeuronId>> recurrentConnections_;
    std::vector<float> connectionStrengths_;

    // Competition and attention
    std::vector<size_t> winningTraceIndices_;
    std::vector<float> neuronActivations_;

    // Statistics
    mutable MemoryStats stats_;

    // Callbacks
    std::function<void(const MemoryStats&)> updateCallback_;

    // Update timing
    Timestamp lastUpdateTime_ = 0.0;
}; // class WorkingMemory

} // namespace nlm

// Close NLM filename guard
#endif // NLM_WORKINGMEMORY_HPP
