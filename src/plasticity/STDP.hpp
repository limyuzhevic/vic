// STDP (Spike-Timing-Dependent Plasticity) class for NLM
#pragma once

#include <memory>
#include <string>
#include <vector>
#include "Types.hpp"
#include "brain/Synapse.hpp"

namespace nlm {

class STDP {
public:
    // Constructor and destructor
    STDP();
    ~STDP() = default;
    
    // Copy and move operations
    STDP(const STDP&) = delete;
    STDP& operator=(const STDP&) = delete;
    STDP(STDP&& other) noexcept;
    STDP& operator=(STDP&& other) noexcept;
    
    // Core STDP update method
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt = 0.001);
    
    // Helper method for single spike pair
    float updatePair(Synapse* synapse,
                     Timestamp preTime,
                     Timestamp postTime,
                     TimestepDuration dt = 0.001);
    
    // Apply a specific weight change to a synapse
    void applyWeightChange(Synapse* synapse, SynapticWeight delta);
    
    // Configuration methods
    void setLTPWeight(float weight);
    float getLTPWeight() const;
    void setLTDWeight(float weight);
    float getLTDWeight() const;
    void setTimeConstant(float tau);
    float getTimeConstant() const;
    void configure(float ltpWeight, float ltdWeight, float tau);
    
    // Information methods
    const char* getName() const;
    std::string getDescription() const;
    void printParameters() const;
    
    // State management
    void reset();
    bool isValid() const;
    
    // Observer pattern for learning events
    class LearningObserver {
    public:
        virtual ~LearningObserver() = default;
        virtual void onLearningEvent(STDP* stdp,
                                     Synapse* synapse,
                                     float weightChange,
                                     bool isLTP) = 0;
    };
    
    void addLearningObserver(std::shared_ptr<LearningObserver> observer);
    void removeLearningObserver(std::shared_ptr<LearningObserver> observer);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    STDP() = default; // Internal only
    
    // Helper methods for spike processing
    float calculateLTP(Synapse* synapse, Timestamp dt) const;
    float calculateLTD(Synapse* synapse, Timestamp dt) const;
    bool isValidSynapseForLearning(Synapse* synapse) const;
};

} // namespace nlm