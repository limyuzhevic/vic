#pragma once

#include "../core/Types/Types.hpp"
#include <array>

namespace nlm {

// Forward declaration
class Neuron;

// Circular buffer for efficient spike history management
class SpikeHistory {
public:
    static constexpr size_t MAX_HISTORY = 100;
    
    SpikeHistory() : size_(0), head_(0) {
        history_.fill(0.0f);
    }
    
    void recordSpike(Timestamp timestamp) {
        history_[head_] = timestamp;
        head_ = (head_ + 1) % MAX_HISTORY;
        if (size_ < MAX_HISTORY) {
            size_++;
        }
    }
    
    Timestamp getSpike(size_t index) const {
        if (index >= size_) {
            return 0.0f;  // Invalid index
        }
        size_t pos = (head_ + MAX_HISTORY - 1 - index) % MAX_HISTORY;
        return history_[pos];
    }
    
    size_t getHistorySize() const {
        return size_;
    }
    
    void clear() {
        size_ = 0;
        head_ = 0;
        history_.fill(0.0f);
    }
    
    // Iterator interface for compatibility
    class iterator {
        const SpikeHistory* parent_;
        size_t index_;
    public:
        iterator(const SpikeHistory* parent, size_t index) : parent_(parent), index_(index) {}
        Timestamp operator*() const { return parent_->getSpike(index_); }
        iterator& operator++() { index_++; return *this; }
        bool operator!=(const iterator& other) const { return index_ != other.index_; }
    };
    
    iterator begin() const { return iterator(this, 0); }
    iterator end() const { return iterator(this, size_); }
    
private:
    std::array<Timestamp, MAX_HISTORY> history_;
    size_t size_;
    size_t head_;
};

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
    const SpikeHistory& getPreSpikeHistory() const;
    const SpikeHistory& getPostSpikeHistory() const;
    void clearHistory();
    
    // Plasticity flags
    const PlasticityFlags& getPlasticityFlags() const;
    PlasticityFlags& getPlasticityFlags();
    
    // Enable/disable plasticity rules
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // Eligibility trace (for reward-modulated learning)
    float getEligibilityTrace() const;
    void setEligibilityTrace(float trace);
    void decayEligibilityTrace(float decayRate);
    
    // Synaptic efficacy (use-dependent modulation)
    float getEfficacy() const;
    void setEfficacy(float efficacy);
    
    // Update synapse for one simulation step
    // TODO PHASE 2: Implement real synaptic dynamics
    void step(Timestamp currentTime);
    
    // Reset to initial state
    void reset();
    
    // Initialize with random parameters
    void initializeRandom(class RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
