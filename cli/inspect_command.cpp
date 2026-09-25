// NLM Inspect Command - Inspect brain state and configuration
// Handles inspecting brain state, configuration, and statistics

#pragma once

#include "cli/nlm_cli.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <json/json.h> // Assuming JSON library is available

namespace nlm {

class InspectCommand {
public:
    static bool execute(NLMCLI& cli, const std::map<std::string, std::string>& args);
    static void showHelp();
    static bool inspectBrainState(std::shared_ptr<Brain> brain, const std::map<std::string, std::string>& options);
    static bool inspectConfiguration(std::shared_ptr<Config> config, const std::map<std::string, std::string>& options);
    static bool inspectStatistics(std::shared_ptr<Brain> brain, const std::map<std::string, std::string>& options);
    static bool exportBrainData(const std::string& format, const std::string& filename, std::shared_ptr<Brain> brain);
    static void printBrainStats(std::shared_ptr<Brain> brain);
    static void printConfigurationStats(std::shared_ptr<Config> config);
    static std::string formatNumber(double value, int precision = 2);
};

bool InspectCommand::execute(NLMCLI& cli, const std::map<std::string, std::string>& args) {
    std::cout << "=== NLM Inspect Command ===" << std::endl;
    
    std::shared_ptr<Brain> brain;
    std::shared_ptr<Config> config;
    
    // Load brain state if requested
    if (args.count("load")) {
        std::string loadFile = args.at("load");
        if (!cli.loadBrainState(loadFile, brain)) {
            std::cerr << "Error: Failed to load brain state" << std::endl;
            return false;
        }
        std::cout << "Brain state loaded from: " << loadFile << std::endl;
    }
    
    // Load configuration
    std::string configFile = cli.getConfigFile();
    config = cli.loadConfiguration(configFile);
    if (!config) {
        std::cerr << "Error: Failed to load configuration" << std::endl;
        return false;
    }
    
    // Create brain if not loaded
    if (!brain) {
        brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "Error: Failed to initialize brain" << std::endl;
            return false;
        }
        std::cout << "Brain created with configuration from: " << configFile << std::endl;
    }
    
    // Determine inspection options
    bool detailed = args.count("detailed") && args.at("detailed") == "true";
    std::string outputFormat = args.count("format") ? args.at("format") : "table";
    
    std::cout << "Inspection options:" << std::endl;
    std::cout << "  Detailed: " << (detailed ? "Yes" : "No") << std::endl;
    std::cout << "  Output format: " << outputFormat << std::endl;
    
    // Perform inspection
    bool success = true;
    
    if (detailed || !args.count("brain-only")) {
        std::cout << std::endl << "=== Brain State Inspection ===" << std::endl;
        success = inspectBrainState(brain, args);
    }
    
    if (detailed || !args.count("config-only")) {
        std::cout << std::endl << "=== Configuration Inspection ===" << std::endl;
        success = inspectConfiguration(config, args) && success;
    }
    
    if (detailed || !args.count("stats-only")) {
        std::cout << std::endl << "=== Statistics Inspection ===" << std::endl;
        success = inspectStatistics(brain, args) && success;
    }
    
    // Export if requested
    if (args.count("export")) {
        std::string exportFormat = args.at("export");
        std::string exportFile = args.count("export-file") ? args.at("export-file") : "brain_state." + exportFormat;
        success = exportBrainData(exportFormat, exportFile, brain) && success;
    }
    
    return success;
}

void InspectCommand::showHelp() {
    std::cout << "=== NLM Inspect Command Help ===" << std::endl;
    std::cout << "Usage: nlm inspect [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Inspect brain state, configuration, and statistics:" << std::endl;
    std::cout << "  -l, --load FILE              Load brain state from FILE" << std::endl;
    std::cout << "  -c, --config FILE            Load configuration from FILE" << std::endl;
    std::cout << "  --detailed                   Show detailed information" << std::endl;
    std::cout << "  --format FORMAT              Output format (table, json, yaml)" << std::endl;
    std::cout << "  --export FORMAT              Export data in specified format" << std::endl;
    std::cout << "  --export-file FILE           Export file name" << std::endl;
    std::cout << "  --brain-only                 Only inspect brain state" << std::endl;
    std::cout << "  --config-only                Only inspect configuration" << std::endl;
    std::cout << "  --stats-only                 Only inspect statistics" << std::endl;
    std::cout << "  -h, --help                   Show this help message" << std::endl;
}

bool InspectCommand::inspectBrainState(std::shared_ptr<Brain> brain, const std::map<std::string, std::string>& options) {
    std::cout << "Brain State Information:" << std::endl;
    printBrainStats(brain);
    
    // Show region information if detailed
    if (options.count("detailed") && options.at("detailed") == "true") {
        std::cout << std::endl << "Region Details:" << std::endl;
        for (size_t i = 1; i <= brain->getRegionCount(); ++i) {
            auto* region = brain->getRegion(RegionId(i));
            if (region) {
                std::cout << "  Region " << i << ":" << std::endl;
                std::cout << "    Neurons: " << region->getAllNeurons().size() << std::endl;
                std::cout << "    Synapses: " << region->getSynapses().size() << std::endl;
                std::cout << "    Total spike activity: " << region->getTotalSpikeCount() << std::endl;
            }
        }
    }
    
    return true;
}

bool InspectCommand::inspectConfiguration(std::shared_ptr<Config> config, const std::map<std::string, std::string>& options) {
    std::cout << "Configuration Information:" << std::endl;
    printConfigurationStats(config);
    
    // Show all configuration values if detailed
    if (options.count("detailed") && options.at("detailed") == "true") {
        std::cout << std::endl << "All Configuration Values:" << std::endl;
        // Note: This is a simplified version - the actual implementation would need
        // to iterate through all config keys and values
        std::cout << "  Configuration keys would be listed here" << std::endl;
    }
    
    return true;
}

bool InspectCommand::inspectStatistics(std::shared_ptr<Brain> brain, const std::map<std::string, std::string>& options) {
    std::cout << "Brain Statistics:" << std::endl;
    
    size_t totalNeurons = brain->getTotalNeuronCount();
    size_t totalSynapses = brain->getTotalSynapseCount();
    size_t totalRegions = brain->getRegionCount();
    size_t totalSpikes = brain->getTotalSpikeCount();
    size_t firingNeurons = brain->getFiringNeuronCount();
    
    std::cout << "  Total Neurons: " << totalNeurons << std::endl;
    std::cout << "  Total Synapses: " << totalSynapses << std::endl;
    std::cout << "  Total Regions: " << totalRegions << std::endl;
    std::cout << "  Total Spikes: " << totalSpikes << std::endl;
    std::cout << "  Firing Neurons: " << firingNeurons << std::endl;
    
    if (totalNeurons > 0) {
        std::cout << "  Average spikes per neuron: " << formatNumber(static_cast<double>(totalSpikes) / totalNeurons) << std::endl;
        std::cout << "  Average synapses per neuron: " << formatNumber(static_cast<double>(totalSynapses) / totalNeurons) << std::endl;
    }
    
    if (options.count("detailed") && options.at("detailed") == "true") {
        std::cout << std::endl << "Advanced Statistics:" << std::endl;
        
        // Calculate firing rates
        double firingRate = totalNeurons > 0 ? static_cast<double>(firingNeurons) / totalNeurons * 100 : 0.0;
        std::cout << "  Firing rate: " << formatNumber(firingRate) << "%" << std::endl;
        
        // Calculate network density
        double density = totalNeurons > 0 ? static_cast<double>(totalSynapses) / (totalNeurons * totalNeurons) * 100 : 0.0;
        std::cout << "  Network density: " << formatNumber(density) << "%" << std::endl;
        
        // Calculate average neuron activity
        // (This would need additional brain API calls)
        std::cout << "  Average neuron activity: [API call needed]" << std::endl;
    }
    
    return true;
}

bool InspectCommand::exportBrainData(const std::string& format, const std::string& filename, std::shared_ptr<Brain> brain) {
    std::cout << "Exporting brain data to " << filename << " in " << format << " format" << std::endl;
    
    if (format == "json") {
        // Export to JSON format
        Json::Value root;
        root["neurons"] = brain->getTotalNeuronCount();
        root["synapses"] = brain->getTotalSynapseCount();
        root["regions"] = brain->getRegionCount();
        root["total_spikes"] = brain->getTotalSpikeCount();
        root["firing_neurons"] = brain->getFiringNeuronCount();
        
        std::ofstream file(filename);
        file << root << std::endl;
        file.close();
        
    } else if (format == "csv") {
        // Export to CSV format
        std::ofstream file(filename);
        file << "neurons,synapses,regions,total_spikes,firing_neurons" << std::endl;
        file << brain->getTotalNeuronCount() << ","
             << brain->getTotalSynapseCount() << ","
             << brain->getRegionCount() << ","
             << brain->getTotalSpikeCount() << ","
             << brain->getFiringNeuronCount() << std::endl;
        file.close();
        
    } else if (format == "yaml") {
        // Export to YAML format (simplified)
        std::ofstream file(filename);
        file << "neurons: " << brain->getTotalNeuronCount() << std::endl;
        file << "synapses: " << brain->getTotalSynapseCount() << std::endl;
        file << "regions: " << brain->getRegionCount() << std::endl;
        file << "total_spikes: " << brain->getTotalSpikeCount() << std::endl;
        file << "firing_neurons: " << brain->getFiringNeuronCount() << std::endl;
        file.close();
    }
    
    std::cout << "Data exported successfully to: " << filename << std::endl;
    return true;
}

void InspectCommand::printBrainStats(std::shared_ptr<Brain> brain) {
    size_t totalNeurons = brain->getTotalNeuronCount();
    size_t totalSynapses = brain->getTotalSynapseCount();
    size_t totalRegions = brain->getRegionCount();
    size_t totalSpikes = brain->getTotalSpikeCount();
    size_t firingNeurons = brain->getFiringNeuronCount();
    
    std::cout << "  Neurons: " << totalNeurons << std::endl;
    std::cout << "  Synapses: " << totalSynapses << std::endl;
    std::cout << "  Regions: " << totalRegions << std::endl;
    std::cout << "  Total Spikes: " << totalSpikes << std::endl;
    std::cout << "  Firing Neurons: " << firingNeurons << std::endl;
}

void InspectCommand::printConfigurationStats(std::shared_ptr<Config> config) {
    if (!config) {
        std::cout << "  No configuration available" << std::endl;
        return;
    }
    
    // Print key configuration parameters
    std::vector<std::string> keys = {
        "neuron_count",
        "region_count",
        "connection_probability",
        "simulation_timestep",
        "random_seed"
    };
    
    for (const auto& key : keys) {
        try {
            if (config->hasKey(key)) {
                // Try different types
                if (config->getType(key) == ConfigType::Int64) {
                    auto64_t value = config->getOr<int64_t>(key, 0);
                    std::cout << "  " << key << ": " << value << std::endl;
                } else if (config->getType(key) == ConfigType::Float) {
                    float value = config->getOr<float>(key, 0.0f);
                    std::cout << "  " << key << ": " << formatNumber(value) << std::endl;
                } else if (config->getType(key) == ConfigType::String) {
                    std::string value = config->getOr<std::string>(key, "");
                    std::cout << "  " << key << ": " << value << std::endl;
                }
            }
        } catch (const std::exception& e) {
            std::cout << "  " << key << ": [Error reading]" << std::endl;
        }
    }
}

std::string InspectCommand::formatNumber(double value, int precision) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(precision) << value;
    return ss.str();
}
