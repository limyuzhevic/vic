// NLM Explore Command - Explore brain structure and connections
// Handles exploring brain structure, neurons, synapses, and connections

#pragma once

#include "cli/nlm_cli.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/NeuralRegion.hpp"
#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <algorithm>
#include <unordered_map>

namespace nlm {

class ExploreCommand {
public:
    static bool execute(NLMCLI& cli, const std::map<std::string, std::string>& args);
    static void showHelp();
    static bool exploreBrainStructure(NLMCLI& cli, const std::map<std::string, std::string>& args);
    static bool exploreRegion(NLMCLI& cli, const std::map<std::string, std::string>& args);
    static bool exploreNeuron(NLMCLI& cli, const std::map<std::string, std::string>& args);
    static bool exploreSynapse(NLMCLI& cli, const std::map<std::string, std::string>& args);
    static bool showConnectionStatistics(NLMCLI& cli, const std::map<std::string, std::string>& args);
    static bool exportBrainStructure(NLMCLI& cli, const std::map<std::string, std::string>& args);
    static void printRegionInfo(std::shared_ptr<Brain> brain, size_t regionIndex);
    static void printNeuronInfo(std::shared_ptr<Neuron> neuron, size_t neuronIndex);
    static void printSynapseInfo(std::shared_ptr<Synapse> synapse, size_t synapseIndex);
    static void printConnectionStats(std::shared_ptr<Brain> brain);
    static std::string formatConnectionType(const std::string& type);
    static std::string formatNeuronType(NeuronType type);
};
