#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include "AdvancedCLI.hpp"
#include "CommandOutputFormatter.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/NeuralRegion.hpp"
#include "brain/NeuralPopulation.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "neuromodulation/Novelty.hpp"

namespace nlm {

class InspectionCommands {
public:
    InspectionCommands(std::shared_ptr<CommandOutputFormatter> formatter,
                       std::shared_ptr<Logger> logger);
    
    // Neuron inspection
    bool inspectNeurons(int count);
    bool inspectNeuron(uint64_t id);
    
    // Synapse inspection
    bool inspectSynapses(int count);
    bool inspectSynapse(uint64_t id);
    
    // Region inspection
    bool inspectRegions(int count);
    bool inspectRegion(uint64_t id);
    
    // Population inspection
    bool inspectPopulations(int count);
    bool inspectPopulation(uint64_t id);
    
    // System inspection
    bool inspectMemory();
    bool inspectEpisodic();
    bool inspectAssociative();
    bool inspectNeuromodulation();
    bool inspectPrediction();
    bool inspectPlanning();
    bool inspectAttention();
    bool inspectConcept();
    
private:
    std::shared_ptr<CommandOutputFormatter> formatter;
    std::shared_ptr<Logger> logger;
    std::shared_ptr<Brain> brain;
    
    // Inspection helpers
    void formatAndLogNeuron(const Neuron* neuron);
    void formatAndLogSynapse(const Synapse* synapse);
    void formatAndLogRegion(const NeuralRegion* region);
    void formatAndLogPopulation(const NeuralPopulation* population);
    void formatAndLogMemory(const NeuralWorkingMemory* memory);
    void formatAndLogNeuromodulation(const Dopamine* dopamine,
                                     const Curiosity* curiosity,
                                     const PredictionError* predictionError,
                                     const Novelty* novelty);
    
    // Validation helpers
    bool validateNeuronId(uint64_t id) const;
    bool validateSynapseId(uint64_t id) const;
    bool validateRegionId(uint64_t id) const;
    bool validatePopulationId(uint64_t id) const;
};
