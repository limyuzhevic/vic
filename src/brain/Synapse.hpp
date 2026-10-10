// Synapse class for NLM - Enhanced with modern C++ patterns
#pragma once

#include <memory>
#include <string>
#include <vector>
#include <cmath>
#include "Types.hpp"
#include "Random/Random.hpp"

namespace nlm {

class Synapse {
public:
    // Constructor and destructor
    Synapse(SynapseId id, NeuronId source, NeuronId destination);
    ~Synapse() = default;
    
    // Copy and move operations
    Synapse(const Synapse&) = delete;
    Synapse& operator=(const Synapse&) = delete;
    Synapse(Synapse&& other) noexcept;
    Synapse& operator=(Synapse&& other) noexcept;
    
    // Basic synapse properties
    SynapseId getId() const;
    NeuronId getSourceNeuron() const;
    NeuronId getDestinationNeuron() const;
    
    // Weight management
    SynapticWeight getWeight() const;
    void setWeight(SynapticWeight weight);
    void addToWeight(SynapticWeight delta);
    SynapticWeight getEffectiveWeight() const; // Weight with efficacy applied
    
    // Delay
    Delay getDelay() const;
    void setDelay(Delay delay);
    
    // Type
    SynapseType getType() const;
    void setType(SynapseType type);
    bool isExcitatory() const;
    bool isInhibitory() const;
    
    // Short-term plasticity state
    float getShortTermDepression() const;
    float getShortTermFacilitation() const;
    float getEfficacy() const;
    void setEfficacy(float efficacy);
    
    // Spike history for STDP
    void recordPreSpike(Timestamp timestamp);
    void recordPostSpike(Timestamp timestamp);
    void clearHistory();
    const std::vector<Timestamp>& getPreSpikeHistory() const;
    const std::vector<Timestamp>& getPostSpikeHistory() const;
    size_t getPreSpikeCount() const;
    size_t getPostSpikeCount() const;
    
    // Eligibility trace for reward-modulated learning
    float getEligibilityTrace() const;
    void setEligibilityTrace(float trace);
    void decayEligibilityTrace(float decayRate);
    
    // Plasticity flags
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    const PlasticityFlags& getPlasticityFlags() const;
    PlasticityFlags& getPlasticityFlags();
    
    // State management
    void step(Timestamp currentTime);
    void reset();
    void initializeRandom(RandomGenerator& rng);
    
    // Utility methods
    std::string getSynapseDescription() const;
    bool isValid() const;
    double getLastPreSpikeTime() const; // For short-term plasticity calculations
    double getLastPostSpikeTime() const;
    
    // Observer pattern for weight changes
    class WeightChangeObserver {
    public:
        virtual ~WeightChangeObserver() = default;
        virtual void onWeightChange(Synapse* synapse, float oldWeight, float newWeight) = 0;
    };
    
    void addWeightChangeObserver(std::shared_ptr<WeightChangeObserver> observer);
    void removeWeightChangeObserver(std::shared_ptr<WeightChangeObserver> observer);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Synapse() = default; // Internal only
    friend class Brain;
};

} // namespace nlm