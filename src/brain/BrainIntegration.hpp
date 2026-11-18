#pragma once

#include "../core/Types/Types.hpp"
#include <memory>
#include <string>

namespace nlm {

class Brain::Impl;
class Neuromodulator;
class Action;
class CheckpointWriter;
class CheckpointReader;
class Neuron;
class Synapse;

class BrainIntegration {
public:
    BrainIntegration(Brain::Impl* pImpl);
    
    void performNeuromodulation(const Neuromodulator& signal);
    void performActionProduction();
    void performPlasticityUpdate();
    void performDevelopment();
    void performReset();
    void performCheckpointSave(const std::string& filepath) const;
    void performCheckpointLoad(const std::string& filepath);
    
    // Statistics helpers
    size_t performTotalNeuronCount() const;
    size_t performTotalSynapseCount() const;
    
private:
    Brain::Impl* pImpl;
};

} // namespace nlm