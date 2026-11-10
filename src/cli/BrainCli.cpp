// Brain CLI Tools Implementation
#include "BrainCli.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

void BrainCli::createBrain(const std::map<std::string, std::string>& options) {
    std::string configFile = Helper::getOption(options, "config", "");
    std::string outputFile = Helper::getOption(options, "output", "brain.bin");
    int neurons = Helper::getOption(options, "neurons", 1000);
    int regions = Helper::getOption(options, "regions", 2);
    
    NLM_LOG_INFO("Creating brain with " + std::to_string(neurons) + " neurons and " + 
                 std::to_string(regions) + " regions");
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set<int64_t>("neuron_count", neurons);
    config->set<int64_t>("region_count", regions);
    
    // Load additional configuration if provided
    if (!configFile.empty()) {
        std::string configContent = Helper::readFile(configFile);
        json configJson = json::parse(configContent);
        
        if (configJson.contains("neuron_count")) {
            config->set<int64_t>("neuron_count", configJson["neuron_count"]);
        }
        if (configJson.contains("region_count")) {
            config->set<int64_t>("region_count", configJson["region_count"]);
        }
    }
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Save brain
    brain->save(outputFile);
    
    NLM_LOG_INFO("Brain created and saved to " + outputFile);
}

void BrainCli::loadBrain(const std::map<std::string, std::string>& options) {
    std::string inputFile = Helper::getOption(options, "input", "");
    std::string outputFile = Helper::getOption(options, "output", "brain.bin");
    
    if (inputFile.empty()) {
        throw NLMConfigError("Input file is required");
    }
    
    NLM_LOG_INFO("Loading brain from " + inputFile);
    
    // Create brain from checkpoint
    auto brain = std::make_shared<Brain>(std::make_shared<Config>());
    bool success = brain->load(inputFile);
    
    if (!success) {
        throw NLMConfigError("Failed to load brain from " + inputFile);
    }
    
    // Save brain if output specified
    if (!outputFile.empty() && outputFile != inputFile) {
        brain->save(outputFile);
        NLM_LOG_INFO("Brain saved to " + outputFile);
    }
    
    NLM_LOG_INFO("Brain loaded successfully");
}

void BrainCli::saveBrain(const std::map<std::string, std::string>& options) {
    std::string inputFile = Helper::getOption(options, "input", "");
    std::string outputFile = Helper::getOption(options, "output", "");
    
    if (inputFile.empty()) {
        throw NLMConfigError("Input file is required");
    }
    
    NLM_LOG_INFO("Saving brain from " + inputFile + " to " + outputFile);
    
    // Load brain
    auto brain = std::make_shared<Brain>(std::make_shared<Config>());
    brain->load(inputFile);
    
    // Save brain
    brain->save(outputFile);
    
    NLM_LOG_INFO("Brain saved successfully");
}

void BrainCli::inspectBrain(const std::map<std::string, std::string>& options) {
    std::string inputFile = Helper::getOption(options, "input", "brain.bin");
    bool details = Helper::getOption(options, "details", false);
    std::string outputFile = Helper::getOption(options, "output", "");
    
    NLM_LOG_INFO("Inspecting brain from " + inputFile);
    
    // Load brain
    auto brain = std::make_shared<Brain>(std::make_shared<Config>());
    brain->load(inputFile);
    
    // Print brain statistics
    std::cout << "=== Brain Inspection Report ===" << std::endl;
    std::cout << "Regions: " << brain->getRegionCount() << std::endl;
    std::cout << "Total Neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
    std::cout << "Active Neurons: " << brain->getActiveNeuronCount() << std::endl;
    std::cout << "Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
    std::cout << "Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "Average Firing Rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
    std::cout << "E/I Ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
    
    if (details) {
        std::cout << "\n=== Detailed Information ===" << std::endl;
        
        // Print region details
        auto regionIds = brain->getRegionIds();
        for (const auto& regionId : regionIds) {
            auto region = brain->getRegion(regionId);
            if (region) {
                std::cout << "Region " << regionId.index() << ": " << region->getName() << std::endl;
                std::cout << "  Neurons: " << region->getTotalNeuronCount() << std::endl;
                std::cout << "  Synapses: " << region->getSynapseCount() << std::endl;
                std::cout << "  Average Weight: " << region->getAverageSynapticWeight() << std::endl;
            }
        }
        
        // Print neuron type distribution
        std::cout << "\n=== Neuron Type Distribution ===" << std::endl;
        auto totalNeurons = brain->getTotalNeuronCount();
        if (totalNeurons > 0) {
            std::cout << "Excitatory: " << (brain->getTotalNeuronCount() * 0.8) << " (80%)" << std::endl;
            std::cout << "Inhibitory: " << (brain->getTotalNeuronCount() * 0.2) << " (20%)" << std::endl;
        }
    }
    
    // Export detailed information if requested
    if (!outputFile.empty()) {
        json inspectionData;
        inspectionData["regions"] = brain->getRegionCount();
        inspectionData["total_neurons"] = brain->getTotalNeuronCount();
        inspectionData["total_synapses"] = brain->getTotalSynapseCount();
        inspectionData["active_neurons"] = brain->getActiveNeuronCount();
        inspectionData["firing_neurons"] = brain->getFiringNeuronCount();
        inspectionData["total_spikes"] = brain->getTotalSpikeCount();
        inspectionData["average_firing_rate"] = brain->getAverageFiringRate();
        inspectionData["e_i_ratio"] = brain->getExcitationInhibitionRatio();
        
        std::ofstream outputFileStream(outputFile);
        outputFileStream << inspectionData.dump(2);
        outputFileStream.close();
        
        NLM_LOG_INFO("Inspection data exported to " + outputFile);
    }
    
    NLM_LOG_INFO("Brain inspection completed");
}

void BrainCli::simulateBrain(const std::map<std::string, std::string>& options) {
    std::string inputFile = Helper::getOption(options, "input", "brain.bin");
    int steps = Helper::getOption(options, "steps", 1000);
    std::string outputFile = Helper::getOption(options, "output", "");
    bool verbose = Helper::getOption(options, "verbose", false);
    
    NLM_LOG_INFO("Simulating brain for " + std::to_string(steps) + " steps");
    
    // Load brain
    auto brain = std::make_shared<Brain>(std::make_shared<Config>());
    brain->load(inputFile);
    
    // Run simulation
    for (int i = 0; i < steps; ++i) {
        brain->step(i);
        
        if (verbose) {
            NLM_LOG_INFO("Step " + std::to_string(i) + ": " + std::to_string(brain->getFiringNeuronCount()) + " neurons firing");
        }
    }
    
    // Save results if requested
    if (!outputFile.empty()) {
        brain->save(outputFile);
        NLM_LOG_INFO("Simulation results saved to " + outputFile);
    }
    
    NLM_LOG_INFO("Brain simulation completed");
}

void BrainCli::debugBrain(const std::map<std::string, std::string>& options) {
    std::string inputFile = Helper::getOption(options, "input", "brain.bin");
    std::string outputFile = Helper::getOption(options, "output", "");
    bool trace = Helper::getOption(options, "trace", false);
    
    NLM_LOG_INFO("Debugging brain from " + inputFile);
    
    // Load brain
    auto brain = std::make_shared<Brain>(std::make_shared<Config>());
    brain->load(inputFile);
    
    // Debug information
    std::cout << "=== Brain Debug Information ===" << std::endl;
    std::cout << "Brain state: " << (brain->getDevelopmentalStage() == DevelopmentalStage::Adult ? "Adult" : 
                               brain->getDevelopmentalStage() == DevelopmentalStage::Maturation ? "Maturation" :
                               brain->getDevelopmentalStage() == DevelopmentalStage::CriticalPeriod ? "Critical Period" :
                               brain->getDevelopmentalStage() == DevelopmentalStage::Initial ? "Initial" : "Aging") << std::endl;
    std::cout << "Neurons firing: " << brain->getFiringNeuronCount() << std::endl;
    std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "Active neurons: " << brain->getActiveNeuronCount() << std::endl;
    
    if (trace) {
        std::cout << "\n=== Trace Information ===" << std::endl;
        // Simulate trace data (in a real implementation, this would be more detailed)
        std::cout << "Trace data would be displayed here" << std::endl;
    }
    
    // Export debug information if requested
    if (!outputFile.empty()) {
        json debugData;
        debugData["region_count"] = brain->getRegionCount();
        debugData["neuron_count"] = brain->getTotalNeuronCount();
        debugData["synapse_count"] = brain->getTotalSynapseCount();
        debugData["firing_neuron_count"] = brain->getFiringNeuronCount();
        debugData["total_spike_count"] = brain->getTotalSpikeCount();
        debugData["active_neuron_count"] = brain->getActiveNeuronCount();
        debugData["development_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        
        std::ofstream outputFileStream(outputFile);
        outputFileStream << debugData.dump(2);
        outputFileStream.close();
        
        NLM_LOG_INFO("Debug data exported to " + outputFile);
    }
    
    NLM_LOG_INFO("Brain debugging completed");
}

void BrainCli::analyzeBrain(const std::map<std::string, std::string>& options) {
    std::string inputFile = Helper::getOption(options, "input", "brain.bin");
    std::string outputFile = Helper::getOption(options, "output", "");
    
    NLM_LOG_INFO("Analyzing brain from " + inputFile);
    
    // Load brain
    auto brain = std::make_shared<Brain>(std::make_shared<Config>());
    brain->load(inputFile);
    
    // Perform analysis
    std::cout << "=== Brain Analysis Report ===" << std::endl;
    std::cout << "Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
    std::cout << "Total spikes: " << brain->getTotalSpikeCount() privilege;
    std::cout << "Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
    std::cout << "E/I ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
    
    // Create analysis data
    json analysisData;
    analysisData["firing_neuron_count"] = brain->getFiringNeuronCount();
    analysisData["total_spike_count"] = brain->getTotalSpikeCount();
    analysisData["average_firing_rate"] = brain->getAverageFiringRate();
    analysisData["e_i_ratio"] = brain->getExcitationInhibitionRatio();
    analysisData["total_neuron_count"] = brain->getTotalNeuronCount();
    analysisData["total_synapse_count"] = brain->getTotalSynapseCount();
    analysisData["active_neuron_count"] = brain->getActiveNeuronCount();
    
    // Export analysis if requested
    if (!outputFile.empty()) {
        std::ofstream outputFileStream(outputFile);
        outputFileStream << analysisData.dump(2);
        outputFileStream.close();
        
        NLM_LOG_INFO("Analysis data exported to " + outputFile);
    }
    
    NLM_LOG_INFO("Brain analysis completed");
}

void BrainCli::exportBrain(const std::map<std::string, std::string>& options) {
    std::string inputFile = Helper::getOption(options, "input", "brain.bin");
    std::string format = Helper::getOption(options, "format", "json");
    std::string outputFile = Helper::getOption(options, "output", "");
    
    NLM_LOG_INFO("Exporting brain from " + inputFile + " to " + format + " format");
    
    // Load brain
    auto brain = std::make_shared<Brain>(std::make_shared<Config>());
    brain->load(inputFile);
    
    if (format == "json") {
        json exportData;
        exportData["region_count"] = brain->getRegionCount();
        exportData["neuron_count"] = brain->getTotalNeuronCount();
        exportData["synapse_count"] = brain->getTotalSynapseCount();
        exportData["firing_neuron_count"] = brain->getFiringNeuronCount();
        exportData["total_spike_count"] = brain->getTotalSpikeCount();
        exportData["average_firing_rate"] = brain->getAverageFiringRate();
        
        std::ofstream outputFileStream(outputFile.empty() ? "brain_export.json" : outputFile);
        outputFileStream << exportData.dump(2);
        outputFileStream.close();
    } else {
        throw NLMConfigError("Unsupported export format: " + format);
    }
    
    NLM_LOG_INFO("Brain export completed");
}

void BrainCli::compareBrains(const std::map<std::string, std::string>& options) {
    std::string file1 = Helper::getOption(options, "input1", "");
    std::string file2 = Helper::getOption(options, "input2", "");
    std::string outputFile = Helper::getOption(options, "output", "");
    
    if (file1.empty() || file2.empty()) {
        throw NLMConfigError("Both input files are required for comparison");
    }
    
    NLM_LOG_INFO("Comparing brains: " + file1 + " and " + file2);
    
    // Load both brains
    auto brain1 = std::make_shared<Brain>(std::make_shared<Config>());
    auto brain2 = std::make_shared<Brain>(std::make_shared<Config>());
    
    brain1->load(file1);
    brain2->load(file2);
    
    // Compare statistics
    std::cout << "=== Brain Comparison Report ===" << std::endl;
    std::cout << "Brain 1: " << file1 << std::endl;
    std::cout << "  Neurons: " << brain1->getTotalNeuronCount() << std::endl;
    std::cout << "  Synapses: " << brain1->getTotalSynapseCount() << std::endl;
    std::cout << "  Firing Neurons: " << brain1->getFiringNeuronCount() << std::endl;
    std::cout << "  Total Spikes: " << brain1->getTotalSpikeCount() << std::endl;
    std::cout << "Brain 2: " << file2 << std::endl;
    std::cout << "  Neurons: " << brain2->getTotalNeuronCount() << std::endl;
    std::cout << "  Synapses: " << brain2->getTotalSynapseCount() << std::endl;
    std::cout << "  Firing Neurons: " << brain2->getFiringNeuronCount() << std::endl;
    std::cout << "  Total Spikes: " << brain2->getTotalSpikeCount() << std::endl;
    
    // Calculate differences
    int neuronDiff = brain2->getTotalNeuronCount() - brain1->getTotalNeuronCount();
    int synapseDiff = brain2->getTotalSynapseCount() - brain1->getTotalSynapseCount();
    int firingDiff = brain2->getFiringNeuronCount() - brain1->getFiringNeuronCount();
    int spikeDiff = brain2->getTotalSpikeCount() - brain1->getTotalSpikeCount();
    
    std::cout << "\n=== Differences ===" << std::endl;
    std::cout << "Neuron count difference: " << neuronDiff << std::endl;
    std::cout << "Synapse count difference: " << synapseDiff << std::endl;
    std::cout << "Firing neuron difference: " << firingDiff << std::endl;
    std::cout << "Total spike difference: " << spikeDiff << std::endl;
    
    // Export comparison if requested
    if (!outputFile.empty()) {
        json comparisonData;
        comparisonData["brain1"] = file1;
        comparisonData["brain2"] = file2;
        comparisonData["brain1_stats"]["neuron_count"] = brain1->getTotalNeuronCount();
        comparisonData["brain1_stats"]["synapse_count"] = brain1->getTotalSynapseCount();
        comparisonData["brain1_stats"]["firing_neuron_count"] = brain1->getFiringNeuronCount();
        comparisonData["brain1_stats"]["total_spike_count"] = brain1->getTotalSpikeCount();
        comparisonData["brain2_stats"]["neuron_count"] = brain2->getTotalNeuronCount();
        comparisonData["brain2_stats"]["synapse_count"] = brain2->getTotalSynapseCount();
        comparisonData["brain2_stats"]["firing_neuron_count"] = brain2->getFiringNeuronCount();
        comparisonData["brain2_stats"]["total_spike_count"] = brain2->getTotalSpikeCount();
        
        std::ofstream outputFileStream(outputFile);
        outputFileStream << comparisonData.dump(2);
        outputFileStream.close();
        
        NLM_LOG_INFO("Comparison data exported to " + outputFile);
    }
    
    NLM_LOG_INFO("Brain comparison completed");
}
