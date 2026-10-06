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
 * @file Hebbian.hpp
 * @brief Hebbian learning implementation for synaptic plasticity
 * 
 * This file implements the Hebbian class, representing Hebbian learning,
 * one of the most fundamental learning mechanisms in neural networks.
 * The Hebbian learning rule is based on the principle "neurons that fire
 * together, wire together" - synaptic connections are strengthened when
 * pre- and post-synaptic neurons are active simultaneously.
 * 
 * The Hebbian implementation includes:
 * - Classic Hebbian learning with weight potentiation
 * - BCM (Bayesian Control of Mean) theory integration
 * - Synaptic normalization and constraint mechanisms
 * - Activity-dependent learning rates
 * - Neuromodulatory control of Hebbian learning
 * - Spike-timing considerations
 * - Learning history management
 * 
 * @author NLM Project
 * @version 1.0.0
 * @date 2024-2026
 * @copyright NLM Project (2024+)
 * 
 * @see PlasticityRule.hpp - Base class for plasticity mechanisms
 * @see STDP.hpp - Complementary spike-timing-dependent plasticity
 * @see Neuromodulator.hpp - Neuromodulatory control
 * @see Synapse.hpp - Synaptic connections that undergo Hebbian learning
 * @see Neuron.hpp - Neurons that generate Hebbian learning signals
 * @see src/core/Types/Types.hpp - Hebbian parameter types
 * 
 * @note This file uses modern C++17 features and follows Doxygen documentation standards.
 *       The Hebbian implementation supports both biological plausibility and computational efficiency.
 */

#pragma once

// NLM filename guard
#ifndef NLM_HEBBIAN_HPP
#define NLM_HEBBIAN_HPP

#include "PlasticityRule.hpp"
#include "../core/Types/Types.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include <vector>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <optional>
#include <map>

namespace nlm {

/**
 * @struct HebbianParameters
 * @brief Configuration parameters for Hebbian learning
 * 
 * This structure contains all the parameters required for Hebbian learning,
 * including learning rates, weight constraints, and normalization factors.
 * The parameters are designed to support:
 * - Biophysical plausibility
 * - Stable learning dynamics
 * - Activity-dependent modulation
 * - Competition between synapses
 * - Memory capacity constraints
 * 
 * Parameter Values (typical ranges):
 * - learning_rate: [0.001, 0.1] (Hebbian learning rate)
 * - max_weight: [0.5, 2.0] (maximum synaptic weight)
 * - min_weight: [0.01, 0.1] (minimum synaptic weight)
 * - beta: [1.0, 10.0] (BCM threshold parameter)
 * - sigma: [0.5, 2.0] (BCM standard deviation)
 * - normalization_factor: [0.1, 1.0] (weight normalization factor)
 */
struct HebbianParameters {
    float learning_rate;          ///< Hebbian learning rate
    float max_weight;             ///< Maximum synaptic weight
    float min_weight;             ///< Minimum synaptic weight
    float beta;                   ///< BCM threshold parameter
    float sigma;                  ///< BCM standard deviation
    float normalization_factor;   ///< Weight normalization factor
    float activity_threshold;     ///< Activity threshold for learning
    float decay_rate;             ///< Weight decay rate
    
    /**
     * @brief Default constructor with biologically plausible parameters
     * 
     * Creates Hebbian parameters with default values:
     * - Learning rate: 0.01
     * - Max/min weight: 2.0, 0.1
     * - Beta: 2.0
     * - Sigma: 1.0
     * - Normalization factor: 0.1
     */
    HebbianParameters()
        : learning_rate(0.01f)
        , max_weight(2.0f)
        , min_weight(0.1f)
        , beta(2.0f)
        , sigma(1.0f)
        , normalization_factor(0.1f)
        , activity_threshold(0.5f)
        , decay_rate(0.001f) {}
    
    /**
     * @brief Parameterized constructor
     * 
     * @param lr Learning rate
     * @param max_w Maximum weight
     * @param min_w Minimum weight
     */
    HebbianParameters(float lr, float max_w, float min_w)
        : learning_rate(lr)
        , max_weight(max_w)
        , min_weight(min_w)
        , beta(2.0f)
        , sigma(1.0f)
        , normalization_factor(0.1f)
        , activity_threshold(0.5f)
        , decay_rate(0.001f) {}
    
    /**
     * @brief Update parameters from configuration
     * 
     * @param config Parameters to merge
     * @param overwrite If true, overwrite existing parameters (default: true)
     */
    void update(const HebbianParameters& config, bool overwrite = true);
    
    /**
     * @brief Validate parameter values
     * 
     * @return bool true if parameters are valid
     */
    bool validate() const;
    
    /**
     * @brief Get parameter description
     * 
     * @param param_name Name of parameter
     * @return std::string Description of the parameter
     */
    std::string getDescription(const std::string& param_name) const;
};

/**
 * @class Hebbian
 * @brief Classic Hebbian learning implementation
 * 
 * The Hebbian class implements a comprehensive Hebbian learning mechanism
 * based on the BCM (Bayesian Control of Mean) theory. It modifies synaptic
 * strength based on the correlation between pre- and post-synaptic activity.
 * 
 * Key Features:
 * - BCM theory with sliding threshold
 * - Activity-dependent learning rates
 * - Synaptic normalization and competition
 * - Weight decay for stability
 * - Neuromodulatory control
 * - Spike-timing considerations
 * - Learning history management
 * - Efficient correlation computation
 * - Thread-safe operation
 * 
 * The BCM learning rule:
 * - ∑ > θ: potentiation (increase in synaptic weight)
 * - ∑ < θ: depression (decrease in synaptic weight)
 * - ∑ = θ: no change (stable equilibrium)
 * 
 * where θ is the sliding threshold that adapts based on post-synaptic activity.
 * 
 * Usage Examples:
 * @code
 * #include "src/plasticity/Hebbian.hpp"
 * #include "src/brain/Synapse.hpp"
 * 
 * // Create Hebbian instance
 * Hebbian hebbian;
 * 
 * // Configure Hebbian parameters
 * hebbian.setLearningRate(0.01f);
 * hebbian.setMaxWeight(2.0f);
 * hebbian.setMinWeight(0.1f);
 * 
 * // Enable Hebbian learning for a synapse
 * synapse->enablePlasticity(true, false, false);  // Only Hebbian
 * 
 * // Process spikes
 * hebbian.recordPreSpike(time1);
 * hebbian.recordPostSpike(time2);
 * 
 * // Update Hebbian learning
 * float delta = hebbian.computeWeightChange();
 * 
 * // Apply neuromodulatory scaling
 * Neuromodulator neuromodulator;
 * neuromodulator.setLevel(0.8f);  // Reduce learning by 20%
 * float scaled_delta = delta * neuromodulator.getPlasticityFactor();
 * @endcode
 * 
 * @note This class uses the Pimpl idiom for efficient compilation and reduced
 *       coupling. The actual implementation is in Hebbian.cpp.
 */
class Hebbian : public PlasticityRule {
public:
    /**
     * @brief Default constructor with default parameters
     * 
     * Creates a Hebbian instance with biologically plausible default parameters.
     */
    Hebbian();
    
    /**
     * @brief Constructor with parameters
     * 
     * @param params Hebbian parameters
     */
    explicit Hebbian(const HebbianParameters& params);
    
    /**
     * @brief Destructor
     * 
     * Cleans up all allocated resources and ensures proper cleanup of Hebbian state.
     */
    ~Hebbian() override;
    
    /**
     * @brief Disable copy construction
     * 
     * Hebbian instances cannot be copied because they contain:
     * - Dynamic resources requiring unique ownership
     * - Internal state that should not be shared
     * - Learning history that would be corrupted by sharing
     */
    Hebbian(const Hebbian&) = delete;
    Hebbian& operator=(const Hebbian&) = delete;
    
    /**
     * @brief Enable move construction
     * 
     * Allows efficient transfer of Hebbian ownership from one instance
     * to another. After move construction, the source Hebbian is in a
     * valid but unspecified state.
     */
    Hebbian(Hebbian&&) noexcept;
    Hebbian& operator=(Hebbian&&) noexcept;
    
    /**
     * @brief Update synaptic weights based on pre- and post-synaptic activity
     * 
     * @param synapse Synapse to update
     * @param preSpikes Vector of pre-synaptic spike timestamps
     * @param postSpikes Vector of post-synaptic spike timestamps
     * @param dt Time step duration in seconds
     * 
     * This method implements the core Hebbian learning rule, computing weight
     * changes based on the correlation between pre- and post-synaptic activity.
     * It uses efficient correlation computation and BCM theory for stable learning.
     * 
     * @note This method should be called once per timestep when synaptic
     *       plasticity is active.
     */
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    /**
     * @brief Apply weight change directly to a synapse
     * 
     * @param synapse Synapse to update
     * @param delta Weight change to apply
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    /**
     * @brief Get the name of this plasticity rule
     * 
     * @return const char* Name of the plasticity rule
     */
    const char* getName() const override;
    
    /**
     * @brief Get the current Hebbian parameters
     * 
     * @return const HebbianParameters& Current parameters
     */
    const HebbianParameters& getParameters() const;
    
    /**
     * @brief Set Hebbian parameters
     * 
     * @param params New parameters
     */
    void setParameters(const HebbianParameters& params);
    
    /**
     * @brief Configure Hebbian parameters from individual values
     * 
     * @param lr Learning rate
     * @param max_w Maximum weight
     * @param min_w Minimum weight
     */
    void configure(float lr, float max_w, float min_w);
    
    /**
     * @brief Get learning rate
     * 
     * @return float Learning rate
     */
    float getLearningRate() const;
    
    /**
     * @brief Set learning rate
     * 
     * @param rate New learning rate
     */
    void setLearningRate(float rate);
    
    /**
     * @brief Get maximum synaptic weight
     * 
     * @return float Maximum weight
     */
    float getMaxWeight() const;
    
    /**
     * @brief Set maximum synaptic weight
     * 
     * @param weight New maximum weight
     */
    void setMaxWeight(float weight);
    
    /**
     * @brief Get minimum synaptic weight
     * 
     * @return float Minimum weight
     */
    float getMinWeight() const;
    
    /**
     * @brief Set minimum synaptic weight
     * 
     * @param weight New minimum weight
     */
    void setMinWeight(float weight);
    
    /**
     * @brief Get BCM threshold parameter (beta)
     * 
     * @return float BCM threshold parameter
     */
    float getBeta() const;
    
    /**
     * @brief Set BCM threshold parameter
     * 
     * @param beta New BCM threshold parameter
     */
    void setBeta(float beta);
    
    /**
     * @brief Get BCM standard deviation parameter (sigma)
     * 
     * @return float BCM standard deviation parameter
     */
    float getSigma() const;
    
    /**
     * @brief Set BCM standard deviation parameter
     * 
     * @param sigma New BCM standard deviation parameter
     */
    void setSigma(float sigma);
    
    /**
     * @brief Get weight normalization factor
     * 
     * @return float Weight normalization factor
     */
    float getNormalizationFactor() const;
    
    /**
     * @brief Set weight normalization factor
     * 
     * @param factor New weight normalization factor
     */
    void setNormalizationFactor(float factor);
    
    /**
     * @brief Get activity threshold
     * 
     * @return float Activity threshold
     */
    float getActivityThreshold() const;
    
    /**
     * @brief Set activity threshold
     * 
     * @param threshold New activity threshold
     */
    void setActivityThreshold(float threshold);
    
    /**
     * @brief Get weight decay rate
     * 
     * @return float Weight decay rate
     */
    float getDecayRate() const;
    
    /**
     * @brief Set weight decay rate
     * 
     * @param rate New weight decay rate
     */
    void setDecayRate(float rate);
    
    /**
     * @brief Record a pre-synaptic spike
     * 
     * @param timestamp Spike timestamp
     * @param weight Synaptic weight at time of spike
     */
    void recordPreSpike(Timestamp timestamp, SynapticWeight weight = 1.0f);
    
    /**
     * @brief Record a post-synaptic spike
     * 
     * @param timestamp Spike timestamp
     * @param weight Synaptic weight at time of spike
     */
    void recordPostSpike(Timestamp timestamp, SynapticWeight weight = 1.0f);
    
    /**
     * @brief Update Hebbian learning state
     * 
     * @param dt Time step duration
     */
    void updateState(TimestepDuration dt);
    
    /**
     * @brief Compute weight change from current activity
     * 
     * @return float Computed weight change
     * 
     * This method computes the weight change based on the current
     * pre- and post-synaptic activity.
     */
    float computeWeightChange() const;
    
    /**
     * @brief Apply neuromodulatory scaling to weight change
     * 
     * @param delta Original weight change
     * @param neuromodulator Neuromodulatory signal
     * @return float Scaled weight change
     */
    float applyNeuromodulation(float delta, const Neuromodulator& neuromodulator) const;
    
    /**
     * @brief Get Hebbian statistics
     * 
     * @return HebbianStats Statistics about Hebbian learning
     */
    struct HebbianStats {
        size_t pre_spike_count;        ///< Number of pre-synaptic spikes
        size_t post_spike_count;       ///< Number of post-synaptic spikes
        float correlation;            ///< Correlation between pre- and post-synaptic activity
        float average_weight_change; ///< Average weight change from this session
        float learning_efficiency;  ///< Efficiency of Hebbian learning
        Timestamp last_pre_spike;   ///< Time of last pre-synaptic spike
        Timestamp last_post_spike;  ///< Time of last post-synaptic spike
        float post_synaptic_activity; ///< Average post-synaptic activity
    };
    
    HebbianStats getStats() const;
    
    /**
     * @brief Reset Hebbian state
     * 
     * This method resets all Hebbian state, including activity history and statistics.
     */
    void reset();
    
    /**
     * @brief Check if Hebbian has been initialized
     * 
     * @return bool true if Hebbian is initialized
     */
    bool isInitialized() const;
    
    /**
     * @brief Initialize Hebbian with parameters
     * 
     * @param params Hebbian parameters
     * 
     * @post Hebbian is initialized with the specified parameters
     */
    void initialize(const HebbianParameters& params);
    
private:
    /**
     * @brief Implementation details structure
     * 
     * Contains the actual implementation details using the Pimpl idiom.
     * This allows efficient compilation and reduces coupling between
     * header and implementation.
     */
    struct Impl;
    
    Impl* pImpl; ///< Pointer to implementation details
}; // class Hebbian

} // namespace nlm

// Close NLM filename guard
#endif // NLM_HEBBIAN_HPP

