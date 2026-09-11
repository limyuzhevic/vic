#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Hebbian plasticity rule implementation
// 
// Implements Hebbian learning: "neurons that fire together, wire together"
// This is a fundamental learning mechanism in neuroscience, observed in
// synaptic strengthening at convergent synapses in the hippocampus and
// visual cortex. Hebbian plasticity is associative and typically leads to
// the formation of neural assemblies that can reactivate together.
// 
// Mathematical formulation (Covariance rule):
// Δw = η * (⟨x*y⟩ - ⟨x⟩⟨y⟩)
// Where x and y are pre- and post-synaptic activity, respectively
// 
// This implementation provides a simplified but biologically plausible
// version suitable for spiking neural network simulations, with adaptations
// for real-world constraints like weight bounds and learning rate limits.
// 
// References:
// - Bienenstock, Cooper, Munro (1982) Theory for the development of neuron
//   selectivity: tolerance and stability in perceptual categorization
// - Gerstner et al. (1996) Synaptic plasticity in spiking networks

class Hebbian : public PlasticityRule {
public:
    /// \brief Constructor - initializes with default biological parameters
    /// \note Default parameters are based on experimental data from mammalian
    ///       cortical circuits, particularly from hippocampal and cortical studies
    Hebbian();
    
    /// \brief Virtual destructor for proper cleanup
    ~Hebbian() override;
    
    /// \brief Update synaptic weights based on spike history using Hebbian learning
    /// \param synapse The synapse to modify
    /// \param preSpikes Vector of pre-synaptic spike timestamps
    /// \param postSpikes Vector of post-synaptic spike timestamps
    /// \param dt Current simulation timestep
    /// \note Implements spike-based Hebbian learning, computing correlation
    ///       between pre- and post-synaptic activity within a biologically
    ///       plausible time window (typically 20-100ms)
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    /// \brief Apply direct weight change to synapse with bounds checking
    /// \param synapse The synapse to modify
    /// \param delta Weight change to apply
    /// \note Ensures weights remain within biologically plausible bounds
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    /// \brief Get rule name
    /// \return Constant string "Hebbian"
    const char* getName() const override;
    
    /// \brief Configure Hebbian learning parameters
    /// \param learningRate Learning rate (typically 0.001-0.01)
    /// \param maxWeight Maximum synaptic weight (typically 1.0-2.0)
    /// \param minWeight Minimum synaptic weight (typically -1.0 to 0.0)
    /// \param covarianceThreshold Optional threshold for covariance-based learning
    void configure(float learningRate, float maxWeight, float minWeight,
                    float covarianceThreshold = 0.0f);
    
    /// \brief Set learning rate with validation
    /// \param rate New learning rate (clamped to [0.0, 1.0])
    void setLearningRate(float rate);
    
    /// \brief Get current learning rate
    /// \return Current learning rate
    float getLearningRate() const;
    
    /// \brief Set maximum weight bound
    /// \param max New maximum weight (clamped to [0.0, 10.0])
    void setMaxWeight(float max);
    
    /// \brief Get maximum weight bound
    /// \return Current maximum weight
    float getMaxWeight() const;
    
    /// \brief Set minimum weight bound
    /// \param min New minimum weight (clamped to [-10.0, 0.0])
    void setMinWeight(float min);
    
    /// \brief Get minimum weight bound
    /// \return Current minimum weight
    float getMinWeight() const;
    
    /// \brief Get covariance threshold for advanced Hebbian variants
    /// \return Current covariance threshold
    float getCovarianceThreshold() const;
    
    /// \brief Set covariance threshold
    /// \param threshold New covariance threshold
    void setCovarianceThreshold(float threshold);

private:
    /// \brief Internal implementation details for efficient storage
    struct Impl;
    
    /// \brief Pointer to private implementation
    Impl* pImpl;
};

} // namespace nlm
