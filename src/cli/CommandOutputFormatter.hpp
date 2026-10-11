#pragma once

#include <string>
#include <sstream>
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/NeuralRegion.hpp"
#include "brain/NeuralPopulation.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "memory/NeuralAssociativeMemory.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "neuromodulation/Novelty.hpp"

namespace nlm {

class CommandOutputFormatter {
public:
    CommandOutputFormatter(std::shared_ptr<Logger> logger);
    
    // Format different output types
    std::string formatBrainInfo(const Brain* brain);
    std::string formatNeuronInfo(const Neuron* neuron);
    std::string formatSynapseInfo(const Synapse* synapse);
    std::string formatRegionInfo(const NeuralRegion* region);
    std::string formatPopulationInfo(const NeuralPopulation* population);
    std::string formatMemoryStatus(const NeuralWorkingMemory* memory);
    std::string formatEpisodicMemory(const NeuralEpisodicMemory* memory);
    std::string formatAssociativeMemory(const NeuralAssociativeMemory* memory);
    std::string formatNeuromodulation(const Dopamine* dopamine,
                                     const Curiosity* curiosity,
                                     const PredictionError* predictionError,
                                     const Novelty* novelty);
    
    // Analysis output formatting
    std::string formatNeuralActivityAnalysis(const std::string& analysis);
    std::string formatPlasticityAnalysis(const std::string& analysis);
    std::string formatMemoryAnalysis(const std::string& analysis);
    std::string formatPerformanceAnalysis(const std::string& analysis);
    
    // Debug output formatting
    std::string formatSpikeEventInfo(const std::string& event);
    std::string formatSpikeHistory(const std::vector<std::string>& history);
    std::string formatPlasticityHistory(const Synapse* synapse);
    std::string formatMemoryTrace(const std::string& trace);
    std::string formatPlasticChange(const std::string& change);
    
    // Utility formatting
    std::string formatProgress(double value, int width = 50);
    std::string formatMemorySize(size_t bytes);
    std::string formatTime(double seconds);
    std::string formatTimestamp(const std::string& timestamp);
    
    // Command output formatting
    std::string formatCommandHelp(const std::string& command);
    std::string formatCommandError(const std::string& command, const std::string& error);
    std::string formatCommandSuccess(const std::string& command);
    
private:
    std::shared_ptr<Logger> logger;
    
    std::string escapeMarkdown(const std::string& text);
    std::string wrapText(const std::string& text, size_t width);
};
