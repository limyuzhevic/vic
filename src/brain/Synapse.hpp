#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Forward declaration
class Neuron;

// Synapse representing a connection between neurons
// Implements real synaptic transmission with delays and short-term plasticity

class Synapse {
public:
    // Create synapse with IDs
    Synapse(SynapseId id, NeuronId source, NeuronId destination);
    
    ~Synapse();
    
    // Disable copying, enable moving
    Synapse(const Synapse&) = delete;
    Synapse& operator=(const Synapse&) = delete;
    Synapse(Synapse&&) noexcept;
    Synapse& operator=(Synapse&&) noexcept;
    
    // Identity
    SynapseId getId() const;
    
    // Connection
    NeuronId getSourceNeuron() const;
    NeuronId getDestinationNeuron() const;
    
    // Weight
    SynapticWeight getWeight() const;
    void setWeight(SynapticWeight weight);
    void addToWeight(SynapticWeight delta);
    
    // Delay (in simulation steps)
    Delay getDelay() const;
    void setDelay(Delay delay);
    
    // Type
    SynapseType getType() const;
    void setType(SynapseType type);
    
    // Excitatory/Inhibitory
    bool isExcitatory() const;
    bool isInhibitory() const;
    
    // Activity history for plasticity
    void recordPreSpike(Timestamp timestamp);
    void recordPostSpike(Timestamp timestamp);
    const std::vector<Timestamp>& getPreSpikeHistory() const;
    const std::vector<Timestamp>& getPostSpikeHistory() const;
    void clearHistory();
    
    // Enable/disable plasticity rules
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // Check if specific plasticity rule is enabled
    bool isHebbianEnabled() const;
    bool isSTDPenabled() const;
    bool isRewardModulatedEnabled() const;
    
    // Eligibility trace (for reward-modulated learning)
    float getEligibilityTrace() const;
    void setEligibilityTrace(float trace);
    void decayEligibilityTrace(float decayRate);
    
    // Synaptic efficacy (use-dependent modulation)
    float getEfficacy() const;
    void setEfficacy(float efficacy);
    
    // STDP learning - spike timing dependent plasticity
    // Long-term potentiation (LTP) when pre fires before post
    // Long-term depression (LTD) when post fires before pre
    void applySTDP(Timestamp preSpikeTime, Timestamp postSpikeTime, float learningRate = 0.01f);
    
    // Hebbian learning - correlated activity
    // Based on the correlation of pre- and post-synaptic activity
    void applyHebbianLearning(float correlation, float learningRate = 0.001f);
    
    // Reward-modulated learning
    // Modulates synaptic change based on external reward prediction error
    void applyRewardModulatedLearning(float rewardPredictionError, float learningRate = 0.01f);
    
    // Combined plasticity update incorporating all rules
    void updatePlasticity(Timestamp preSpikeTime, Timestamp postSpikeTime, 
                         float correlation = 0.0f, float rewardError = 0.0f);
    
    // Get current synaptic strength (combines weight, efficacy, and eligibility)
    SynapticWeight getEffectiveWeight() const;
    
    // Calculate recent synaptic activity for metaplastic changes
    float calculateRecentActivity() const;
    
    // Update synapse for one simulation step
    // TODO PHASE 2: Implement real synaptic dynamics
    void step(Timestamp currentTime);
    
    // Reset to initial state
    void reset();
    
    // Initialize with random parameters
    void initializeRandom(class RandomGenerator& rng);
    
    // Get short-term plasticity state
    float getShortTermDepression() const;
    float getShortTermFacilitation() const;
    
    // Update short-term plasticity state based on recent activity
    void updateShortTermPlasticity(Timestamp currentTime);
    
    // Get spike history for learning calculations
    const std::vector<Timestamp>& getPreSpikeHistory() const;
    const std::vector<Timestamp>& getPostSpikeHistory() const;
