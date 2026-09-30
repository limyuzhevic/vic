// HelperFunctions.hpp
#pragma once

#include "../brain/Brain.hpp"
#include "brain/NeuralRegion.hpp"
#include <vector>
#include <string>

namespace nlm {

// Helper functions to eliminate repetitive patterns
namespace Helper {

// Population creation and management
std::vector<std::unique_ptr<NeuralPopulation>> createBalancedPopulations(
    size_t totalNeurons, size_t regionCount);

std::vector<Neuron*> extractSensoryNeurons(NeuralRegion* region);
std::vector<Neuron*> extractMotorNeurons(NeuralRegion* region);

// Neuron and synapse utilities
float computeNeuronActivation(const Neuron* neuron);
std::vector<NeuronId> findActiveNeurons(const std::vector<Neuron*>& neurons);
size_t countFiringNeurons(const std::vector<Neuron*>& neurons);
float calculateAverageFiringRate(const std::vector<Neuron*>& neurons);

// Memory system utilities
std::vector<NeuronId> getActiveMemoryNeurons(const NeuralWorkingMemory* memory);
float computeMemoryActivity(const NeuralWorkingMemory* memory);

// Logging and monitoring helpers
void logRegionInfo(const NeuralRegion* region);
void logMemorySystemStatus(const MemorySystemInterface& systems);
void logStepProgress(int stepNumber, const std::string& description);

// Configuration helpers
std::string getLogPrefix(const std::string& systemName);
float getConfigOrDefault(const Config& config, const std::string& key, float defaultValue);
size_t getConfigOrDefault(const Config& config, const std::string& key, size_t defaultValue);

// Batch operations helpers
void injectCurrentToAllNeurons(NeuralRegion* region, NeuronType type, 
                              MembranePotential current);
void collectNeuronStatistics(const std::vector<Neuron*>& neurons,
                            size_t& total, size_t& firing, float& avgRate);

} // namespace Helper

} // namespace nlm