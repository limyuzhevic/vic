// Synapse.hpp

#ifndef NLM_SYNAPSE_HPP
#define NLM_SYNAPSE_HPP

#include <memory>
#include <vector>
#include "../core/Types/Types.hpp"

namespace nlm {

class Synapse {
private:
    struct Impl;
    Impl* pImpl;

public:
    Synapse(SynapseId id, NeuronId source, NeuronId destination);
    ~Synapse();
    
    // Prevent copy operations
    Synapse(const Synapse&) = delete;
    Synapse& operator=(const Synapse&) = delete;
    
    // Move operations
    Synapse(Synapse&& other) noexcept;
    Synapse& operator=(Synapse&& other) noexcept;

    // ========== IDENTIFICATION ==========
    SynapseId getId() const;
    NeuronId getSourceNeuron() const;
    NeuronId getDestinationNeuron() const;

    // ========== WEIGHT MANAGEMENT ==========
    SynapticWeight getWeight() const;
    void setWeight(SynapticWeight weight);
    void addToWeight(SynapticWeight delta);
    
    // Weight bounds enforcement
    static constexpr float getMinWeight() { return -1.0f; }
    static constexpr float getMaxWeight() { return 1.0f; }
    
    bool clampWeight();  // Ensure weight is within bounds, returns true if changed

    // ========== SYNAPSE PROPERTIES ==========
    SynapseType getType() const;
    void setType(SynapseType type);
    
    Delay getDelay() const;
    void setDelay(Delay delay);

    // ========== SPIKE HISTORY ==========
    void recordPreSpike(Timestamp time);
    void recordPostSpike(Timestamp time);
    
    const std::vector<Timestamp>& getPreSpikeHistory() const;
    const std::vector<Timestamp>& getPostSpikeHistory() const;
    
    void clearSpikeHistory();

    // ========== PLASTICITY ==========
    PlasticityFlags getPlasticityFlags() const;
    void setPlasticityFlags(const PlasticityFlags& flags);
    
    void setEligibilityTrace(float trace);
    float getEligibilityTrace() const;

    // ========== SHORT-TERM PLASTICITY ==========
    void updateShortTermPlasticity(Timestamp currentTime);
    float getUtilizationFactor() const;
    float getFacilitationFactor() const;
    float getSynapticEfficacy() const;

    // ========== SPIKE RECORDING ==========
    void update(Timestamp currentTime);

    // ========== STATISTICS ==========
    size_t getPreSpikeCount() const;
    size_t getPostSpikeCount() const;
    float getAveragePreSpikeInterval() const;
    float getAveragePostSpikeInterval() const;
    
    // ========== DEBUG/UTILITY ==========
    std::string debugInfo() const;
};

} // namespace nlm

#endif // NLM_SYNAPSE_HPP