#pragma once

#include "../core/Types/Types.hpp"
#include <chrono>
#include <string>
#include <memory>
#include <vector>

namespace nlm {

// Forward declarations
class Brain;
class Config;
class Logger;

// Simple SynapticChangeRecord for learning tracking
struct SynapticChangeRecord {
    std::chrono::steady_clock::time_point timestamp;
    NeuronId preNeuron;
    NeuronId postNeuron;
    float oldWeight;
    float newWeight;
    SynapseId synapseId;
    SynapseType synapseType;
    
    SynapticChangeRecord() : preNeuron(), postNeuron(), oldWeight(0.0f), newWeight(0.0f),
        synapseId(), synapseType(SynapseType::Excitatory) {}
    
    SynapticChangeRecord(NeuronId pre, NeuronId post, float oldW, float newW)
        : preNeuron(pre), postNeuron(post), oldWeight(oldW), newWeight(newW),
          synapseId(), synapseType(SynapseType::Excitatory) {}
    
    float getWeightChange() const { return newWeight - oldWeight; }
    bool isPotentiation() const { return newWeight > oldWeight; }
    bool isDepression() const { return newWeight < oldWeight; }
};

// High-level learning experiment for tracking neural plasticity
class LearningExperiment {
public:
    LearningExperiment();
    ~LearningExperiment();
    
    void addBrain(const std::string& brainName, std::shared_ptr<Brain> brain);
    void recordChange(const SynapticChangeRecord& change);
    size_t getTotalChangesRecorded() const;
    void reset();
    void exportToJSON(const std::string& filename) const;
    void exportToCSV(const std::string& filename) const;
    std::string getTrainingSummary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm