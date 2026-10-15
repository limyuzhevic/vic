#pragma once

#include "Brain.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

// Brain IO operations (save/load) declarations
// Refactored from Brain.cpp to improve maintainability

class BrainIO {
public:
    static bool saveBrain(const std::string& filepath, const Brain& brain);
    static bool loadBrain(const std::string& filepath, Brain& brain);
    
    static void writeNeuronsToCheckpoint(CheckpointWriter& writer, const Brain& brain);
    static void writeSynapsesToCheckpoint(CheckpointWriter& writer, const Brain& brain);
    static void applyNeuronStates(const NeuronCheckpointData& neuronData, Brain& brain);
    static void applySynapseStates(const SynapseCheckpointData& synapseData, Brain& brain);
};

}} // namespace nlm