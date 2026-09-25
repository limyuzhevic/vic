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

bool ExploreCommand::execute(NLMCLI& cli, const std::map<std::string, std::string>& args) {
    std::cout << "=== NLM Explore Command ===" << std::endl;
    
    // Check if specific element to explore is specified
    if (args.count("region") || args.count("neuron") || args.count("synapse") || args.count("connections")) {
        return exploreBrainStructure(cli, args);
    }
    
    // Otherwise, explore entire brain structure
    std::cout << "Exploring complete brain structure..." << std::endl;
    return exploreBrainStructure(cli, args);
}

void ExploreCommand::showHelp() {
    std::cout << "=== NLM Explore Command Help ===" << std::endl;
    std::cout << "Usage: nlm explore [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Explore the brain structure and connections:" << std::endl;
    std::cout << "  -c, --config FILE              Load configuration from FILE" << std::endl;
    std::cout << "  --region INDEX                Explore specific region" << std::endl;
    std::cout << "  --neuron INDEX                Explore specific neuron" << std::endl;
    std::cout << "  --synapse INDEX                Explore specific synapse" << std::endl;
    std::cout << "  --connections                 Show connection statistics" << std::endl;
    std::cout << "  --export FORMAT               Export brain structure to format" << std::endl;
    std::cout << "  --export-file FILE            Export file name" << std::endl;
    std::cout << "  --detailed                    Show detailed information" << std::endl;
    std::cout << "  -h, --help                    Show this help message" << std::endl;
}

bool ExploreCommand::exploreBrainStructure(NLMCLI& cli, const std::map<std::string, std::string>& args) {
    std::cout << "Exploring brain structure..." << std::endl;
    
    // Load configuration
    std::string configFile = cli.getConfigFile();
    auto config = cli.loadConfiguration(configFile);
    if (!config) {
        std::cerr << "Error: Failed to load configuration" << std::endl;
        return false;
    }
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    bool success = true;
    
    // Check if specific elements should be explored
    if (args.count("region")) {
        size_t regionIndex = std::stoul(args.at("region")) - 1; // Convert to 0-based
        success = exploreRegion(cli, {{ "region", std::to_string(regionIndex + 1) }, args}) && success;
    }
    
    if (args.count("neuron")) {
        size_t neuronIndex = std::stoul(args.at("neuron")) - 1; // Convert to 0-based
        success = exploreNeuron(cli, {{ "neuron", std::to_string(neuronIndex + 1) }, args}) && success;
    }
    
    if (args.count("synapse")) {
        size_t synapseIndex = std::stoul(args.at("synapse")) - 1; // Convert to 0-based
        success = exploreSynapse(cli, {{ "synapse", std::to_string(synapseIndex + 1) }, args}) && success;
    }
    
    if (args.count("connections") && args.at("connections") == "true") {
        success = showConnectionStatistics(cli, args) && success;
    }
    
    // If no specific elements were explored, show overview
    if (!args.count("region") && !args.count("neuron") && !args.count("synapse") && 
        !(args.count("connections") && args.at("connections") == "true")) {
        
        std::cout << std::endl << "=== Brain Structure Overview ===" << std::endl;
        printConnectionStats(brain);
        
        std::cout << std::endl << "=== Region Summary ===" << std::endl;
        for (size_t i = 0; i < brain->getRegionCount(); ++i) {
            printRegionInfo(brain, i);
        }
    }
    
    // Export if requested
    if (args.count("export")) {
        success = exportBrainStructure(cli, args) && success;
    }
    
    return success;
}

bool ExploreCommand::exploreRegion(NLMCLI& cli, const std::map<std::string, std::string>& args) {
    std::string regionStr = args.at("region");
    size_t regionIndex = std::stoul(regionStr) - 1; // Convert to 0-based
    
    std::cout << "Exploring Region " << regionIndex + 1 << ":" << std::endl;
    
    auto brain = std::make_shared<Brain>(cli.loadConfiguration(cli.getConfigFile()));
    brain->initialize();
    
    auto* region = brain->getRegion(RegionId(regionIndex + 1));
    if (!region) {
        std::cerr << "Error: Region " << regionIndex + 1 << " not found" << std::endl;
        return false;
    }
    
    std::cout << "  Region ID: " << regionIndex + 1 << std::endl;
    std::cout << "  Neurons: " << region->getAllNeurons().size() << std::endl;
    std::cout << "  Synapses: " << region->getSynapses().size() << std::endl;
    std::cout << "  Total spike activity: " << region->getTotalSpikeCount() << std::endl;
    
    // Show neuron types
    auto neurons = region->getAllNeurons();
    std::map<NeuronType, size_t> neuronCounts;
    for (const auto& neuron : neurons) {
        neuronCounts[neuron->getType()]++;
    }
    
    std::cout << "  Neuron type distribution:" << std::endl;
    for (const auto& [type, count] : neuronCounts) {
        std::cout << "    " << formatNeuronType(type) << ": " << count << std::endl;
    }
    
    // Show synapse types
    auto synapses = region->getSynapses();
    std::map<std::string, size_t> synapseCounts;
    for (const auto& synapse : synapses) {
        std::string type = "unknown";
        // Determine synapse type based on properties
        if (synapse->getWeight() > 0.5) type = "strong_excitatory";
        else if (synapse->getWeight() < 0.2) type = "weak_inhibitory";
        else type = "normal";
        synapseCounts[type]++;
    }
    
    std::cout << "  Synapse type distribution:" << std::endl;
    for (const auto& [type, count] : synapseCounts) {
        std::cout << "    " << type << ": " << count << std::endl;
    }
    
    return true;
}

bool ExploreCommand::exploreNeuron(NLMCLI& cli, const std::map<std::string, std::string>& args) {
    std::string neuronStr = args.at("neuron");
    size_t neuronIndex = std::stoul(neuronStr) - 1; // Convert to 0-based
    
    std::cout << "Exploring Neuron " << neuronIndex + 1 << ":" << std::endl;
    
    auto brain = std::make_shared<Brain>(cli.loadConfiguration(cli.getConfigFile()));
    brain->initialize();
    
    // Find the neuron (simplified - would need actual neuron lookup in real implementation)
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        std::cerr << "Error: No regions found" << std::endl;
        return false;
    }
    
    auto neurons = region->getAllNeurons();
    if (neuronIndex >= neurons.size()) {
        std::cerr << "Error: Neuron index out of range" << std::endl;
        return false;
    }
    
    printNeuronInfo(neurons[neuronIndex], neuronIndex + 1);
    
    return true;
}

bool ExploreCommand::exploreSynapse(NLMCLI& cli, const std::map<std::string, std::string>& args) {
    std::string synapseStr = args.at("synapse");
    size_t synapseIndex = std::stoul(synapseStr) - 1; // Convert to 0-based
    
    std::cout << "Exploring Synapse " << synapseIndex + 1 << ":" << std::endl;
    
    auto brain = std::make_shared<Brain>(cli.loadConfiguration(cli.getConfigFile()));
    brain->initialize();
    
    // Find the synapse (simplified - would need actual synapse lookup in real implementation)
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        std::cerr << "Error: No regions found" << std::endl;
        return false;
    }
    
    auto synapses = region->getSynapses();
    if (synapseIndex >= synapses.size()) {
        std::cerr << "Error: Synapse index out of range" << std::endl;
        return false;
    }
    
    printSynapseInfo(synapses[synapseIndex], synapseIndex + 1);
    
    return true;
}

bool ExploreCommand::showConnectionStatistics(NLMCLI& cli, const std::map<std::string, std::string>& args) {
    std::cout << "=== Connection Statistics ===" << std::endl;
    
    auto brain = std::make_shared<Brain>(cli.loadConfiguration(cli.getConfigFile()));
    brain->initialize();
    
    printConnectionStats(brain);
    
    // Calculate connectivity metrics
    size_t totalNeurons = brain->getTotalNeuronCount();
    size_t totalSynapses = brain->getTotalSynapseCount();
    
    if (totalNeurons > 0) {
        double synapsesPerNeuron = static_cast<double>(totalSynapses) / totalNeurons;
        std::cout << "Synapses per neuron: " << synapsesPerNeuron << std::endl;
        
        double connectionProbability = totalSynapses / (totalNeurons * totalNeurons);
        std::cout << "Connection probability: " << connectionProbability << std::endl;
    }
    
    // Analyze region-to-region connectivity
    std::cout << std::endl << "=== Region-to-Region Connectivity ===" << std::endl;
    for (size_t i = 0; i < brain->getRegionCount(); ++i) {
        auto* region1 = brain->getRegion(RegionId(i + 1));
        if (!region1) continue;
        
        std::cout << "Region " << i + 1 << " connections:" << std::endl;
        
        for (size_t j = 0; j < brain->getRegionCount(); ++j) {
            if (i == j) continue;
            
            auto* region2 = brain->getRegion(RegionId(j + 1));
            if (!region2) continue;
            
            // In a real implementation, you would count actual connections
            // For now, show placeholder
            std::cout << "  -> Region " << j + 1 << ": [To be implemented]" << std::endl;
        }
    }
    
    return true;
}

bool ExploreCommand::exportBrainStructure(NLMCLI& cli, const std::map<std::string, std::string>& args) {
    std::string format = args.at("export");
    std::string filename = args.count("export-file") ? args.at("export-file") : "brain_structure." + format;
    
    std::cout << "Exporting brain structure to " << filename << " in " << format << " format" << std::endl;
    
    auto brain = std::make_shared<Brain>(cli.loadConfiguration(cli.getConfigFile()));
    brain->initialize();
    
    if (format == "json") {
        // Export to JSON format
        exportBrainToJSON(brain, filename);
    } else if (format == "csv") {
        // Export to CSV format
        exportBrainToCSV(brain, filename);
    } else if (format == "yaml") {
        // Export to YAML format
        exportBrainToYAML(brain, filename);
    } else if (format == "dot") {
        // Export to Graphviz DOT format
        exportBrainToDOT(brain, filename);
    } else {
        std::cerr << "Error: Unsupported export format: " << format << std::endl;
        return false;
    }
    
    std::cout << "Brain structure exported successfully to: " << filename << std::endl;
    return true;
}

void ExploreCommand::printRegionInfo(std::shared_ptr<Brain> brain, size_t regionIndex) {
    auto* region = brain->getRegion(RegionId(regionIndex + 1));
    if (!region) {
        std::cout << "Region " << regionIndex + 1 << ": Not found" << std::endl;
        return;
    }
    
    std::cout << "  Region " << regionIndex + 1 << ": "
              << region->getAllNeurons().size() << " neurons, "
              << region->getSynapses().size() << " synapses" << std::endl;
}

void ExploreCommand::printNeuronInfo(std::shared_ptr<Neuron> neuron, size_t neuronIndex) {
    std::cout << "  Neuron " << neuronIndex << ":" << std::endl;
    std::cout << "    Type: " << formatNeuronType(neuron->getType()) << std::endl;
    std::cout << "    Membrane Potential: " << neuron->getMembranePotential() << std::endl;
    std::cout << "    Threshold: " << neuron->getThreshold() << std::endl;
    std::cout << "    Refractory Period: " << neuron->getRefractoryPeriod() << std::endl;
    std::cout << "    Current: " << neuron->getCurrent() << std::endl;
    std::cout << "    Spike Count: " << neuron->getSpikeCount() << std::endl;
}

void ExploreCommand::printSynapseInfo(std::shared_ptr<Synapse> synapse, size_t synapseIndex) {
    std::cout << "  Synapse " << synapseIndex << ":" << std::endl;
    std::cout << "    Source: Neuron " << synapse->getSourceNeuronId() << std::endl;
    std::cout << "    Target: Neuron " << synapse->getTargetNeuronId() << std::endl;
    std::cout << "    Weight: " << synapse->getWeight() << std::endl;
    std::cout << "    Delay: " << synapse->getDelay() << std::endl;
    std::cout << "    Enabled: " << (synapse->isEnabled() ? "Yes" : "No") << std::endl;
    std::cout << "    Plasticity: " << (synapse->isPlasticityEnabled() ? "Yes" : "No") << std::endl;
}

void ExploreCommand::printConnectionStats(std::shared_ptr<Brain> brain) {
    std::cout << "  Total neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "  Total synapses: " << brain->getTotalSynapseCount() << std::endl;
    std::cout << "  Total regions: " << brain->getRegionCount() << std::endl;
    
    // Calculate average neuron activity
    size_t totalSpikes = brain->getTotalSpikeCount();
    size_t totalNeurons = brain->getTotalNeuronCount();
    if (totalNeurons > 0) {
        double avgSpikesPerNeuron = static_cast<double>(totalSpikes) / totalNeurons;
        std::cout << "  Average spikes per neuron: " << avgSpikesPerNeuron << std::endl;
    }
}

std::string ExploreCommand::formatConnectionType(const std::string& type) {
    // Convert connection type to readable format
    if (type == "excitatory") return "Excitatory";
    if (type == "inhibitory") return "Inhibitory";
    if (type == "plastic") return "Plastic";
    if (type == "fixed") return "Fixed";
    return type;
}

std::string ExploreCommand::formatNeuronType(NeuronType type) {
    switch (type) {
        case NeuronType::Sensory: return "Sensory";
        case NeuronType::Motor: return "Motor";
        case NeuronType::Interneuron: return "Interneuron";
        case NeuronType::Excitatory: return "Excitatory";
        case NeuronType::Inhibitory: return "Inhibitory";
        case NeuronType::Plastic: return "Plastic";
        default: return "Unknown";
    }
}

void ExploreCommand::exportBrainToJSON(std::shared_ptr<Brain> brain, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file for writing: " << filename << std::endl;
        return;
    }
    
    file << "{\n";
    file << "  \"neurons\": " << brain->getTotalNeuronCount() << ",\n";
    file << "  \"synapses\": " << brain->getTotalSynapseCount() << ",\n";
    file << "  \"regions\": " << brain->getRegionCount() << ",\n";
    file << "  \"total_spikes\": " << brain->getTotalSpikeCount() << ",\n";
    file << "  \"firing_neurons\": " << brain->getFiringNeuronCount() << "\n";
    file << "}" << std::endl;
    
    file.close();
}

void ExploreCommand::exportBrainToCSV(std::shared_ptr<Brain> brain, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file for writing: " << filename << std::endl;
        return;
    }
    
    file << "metric,value" << std::endl;
    file << "neurons," << brain->getTotalNeuronCount() << std::endl;
    file << "synapses," << brain->getTotalSynapseCount() << std::endl;
    file << "regions," << brain->getRegionCount() << std::endl;
    file << "total_spikes," << brain->getTotalSpikeCount() << std::endl;
    file << "firing_neurons," << brain->getFiringNeuronCount() << std::endl;
    
    file.close();
}

void ExploreCommand::exportBrainToYAML(std::shared_ptr<Brain> brain, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file for writing: " << filename << std::endl;
        return;
    }
    
    file << "neurons: " << brain->getTotalNeuronCount() << std::endl;
    file << "synapses: " << brain->getTotalSynapseCount() << std::endl;
    file << "regions: " << brain->getRegionCount() << std::endl;
    file << "total_spikes: " << brain->getTotalSpikeCount() << std::endl;
    file << "firing_neurons: " << brain->getFiringNeuronCount() << std::endl;
    
    file.close();
}

void ExploreCommand::exportBrainToDOT(std::shared_ptr<Brain> brain, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file for writing: " << filename << std::endl;
        return;
    }
    
    file << "digraph G {\n";
    file << "  rankdir=LR;\n";
    file << "  node [shape=circle];\n";
    file << "\n";
    
    // Add nodes (neurons)
    file << "  // " << brain->getTotalNeuronCount() << " neurons would be listed here\n";
    file << "\n";
    
    // Add edges (synapses)
    file << "  // " << brain->getTotalSynapseCount() << " synapses would be listed here\n";
    file << "}\n";
    
    file.close();
}
