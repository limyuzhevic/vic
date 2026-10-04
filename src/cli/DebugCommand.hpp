#pragma once

#include "CLICommand.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <map>
#include <vector>

namespace nlm {

class DebugCommand : public SimpleCLICommand {
public:
    DebugCommand() : SimpleCLICommand(
        "debug",
        "Debug neural simulation and diagnostics",
        "nlm debug <command> [options]",
        "Debug neural simulation and diagnostics.\n\n"
        "Available debug commands:\n"
        "  neurons     Debug neuron state and properties\n"
        "  synapses    Debug synapse connections and weights\n"
        "  connectivity Validate network connectivity\n"
        "  spikes      Analyze spike patterns and timing\n"
        "  plasticity  Debug plasticity mechanisms\n"
        "  config      Debug configuration\n"
        "  memory      Debug memory usage and allocation\n\n"
        "Options:\n"
        "  --region <id>       Target specific brain region\n"
        "  --verbose           Enable verbose output\n"
        "  --export <file>     Export debug information to file\n"
        "  --format json|text  Output format\n\n"
        "Examples:\n"
        "  nlm debug neurons --verbose\n"
        "  nlm debug synapses --region 1\n"
        "  nlm debug connectivity --export debug_report.txt"
    ) {
        options = {
            {"region", "", "Target brain region ID", OptionType::OptionalValue},
            {"verbose", "", "Enable verbose output", OptionType::Flag},
            {"export", "", "Export debug information to file", OptionType::OptionalValue},
            {"format", "", "Output format", OptionType::Choice, false, "text", "json|text"},
            {"level", "", "Debug level (1-5)", OptionType::OptionalValue, false, "3"}
        };
        
        parameters = {
            {"command", "Debug command to execute", true, "string", ""}
        };
    }
    
    int execute(const std::vector<std::string>& args) override {
        auto& errorHandler = CLIErrorHandler::getInstance();
        errorHandler.clear();
        
        // Get required parameter
        std::string command = getRequiredParameter(args, 0);
        
        // Get options
        std::string regionStr = getOptionValue(args, "region");
        bool verbose = hasOption(args, "verbose");
        std::string exportFile = getOptionValue(args, "export");
        std::string format = getOptionValue(args, "format");
        std::string levelStr = getOptionValue(args, "level");
        
        // Parse parameters
        int region = -1;
        if (!regionStr.empty()) {
            try {
                region = std::stoi(regionStr);
            } catch (...) {
                errorHandler.addError("Invalid region value: " + regionStr);
                return errorHandler.exitWithError();
            }
        }
        
        int debugLevel = 3; // default
        if (!levelStr.empty()) {
            try {
                debugLevel = std::stoi(levelStr);
                if (debugLevel < 1 || debugLevel > 5) {
                    errorHandler.addError("Debug level must be between 1 and 5");
                    return errorHandler.exitWithError();
                }
            } catch (...) {
                errorHandler.addError("Invalid debug level: " + levelStr);
                return errorHandler.exitWithError();
            }
        }
        
        // Get brain
        auto brain = getBrain();
        if (!brain) {
            errorHandler.addError("No brain available for debugging");
            return errorHandler.exitWithError();
        }
        
        // Execute debug command
        if (command == "neurons") {
            debugNeurons(brain, region, verbose, debugLevel);
        } else if (command == "synapses") {
            debugSynapses(brain, region, verbose, debugLevel);
        } else if (command == "connectivity") {
            validateConnectivity(brain, verbose, debugLevel);
        } else if (command == "spikes") {
            analyzeSpikes(brain, region, verbose, debugLevel);
        } else if (command == "plasticity") {
            debugPlasticity(brain, region, verbose, debugLevel);
        } else if (command == "config") {
            debugConfiguration(verbose, debugLevel);
        } else if (command == "memory") {
            debugMemory(verbose, debugLevel);
        } else {
            errorHandler.addError("Unknown debug command: " + command);
            return errorHandler.exitWithError();
        }
        
        // Export debug information if requested
        if (!exportFile.empty()) {
            exportDebugInfo(exportFile, format);
        }
        
        return errorHandler.isSuccess() ? 0 : 1;
    }
    
    bool validate(const std::vector<std::string>& args) const override {
        if (args.empty()) {
            return false;
        }
        
        std::string command = args[0];
        
        // Validate supported commands
        static const std::vector<std::string> supportedCommands = {
            "neurons", "synapses", "connectivity", "spikes", "plasticity", "config", "memory"
        };
        
        if (std::find(supportedCommands.begin(), supportedCommands.end(), command) == supportedCommands.end()) {
            return false;
        }
        
        return true;
    }
    
private:
    void debugNeurons(std::shared_ptr<Brain> brain, int region, bool verbose, int level) {
        std::cout << "Neuron Debug Information" << std::endl;
        std::cout << "======================" << std::endl << std::endl;
        
        // Get all neurons
        auto neurons = brain->getAllNeurons();
        
        std::cout << "Total Neurons: " << neurons.size() << std::endl;
        
        // Debug neurons based on region
        if (region >= 0) {
            // Get neurons from specific region
            auto* brainRegion = brain->getRegion(RegionId(region));
            if (brainRegion) {
                auto regionNeurons = brainRegion->getAllNeurons();
                debugNeuronDetails(regionNeurons, "Region " + std::to_string(region), verbose, level);
            }
        } else {
            // Debug all neurons
            debugNeuronDetails(neurons, "All Regions", verbose, level);
        }
    }
    
    void debugSynapses(std::shared_ptr<Brain> brain, int region, bool verbose, int level) {
        std::cout << "Synapse Debug Information" << std::endl;
        std::cout << "========================" << std::endl << std::endl;
        
        // Get all synapses
        auto synapses = brain->getAllSynapses();
        
        std::cout << "Total Synapses: " << synapses.size() << std::endl;
        
        // Debug synapses based on region
        if (region >= 0) {
            // Get synapses in specific region
            auto* brainRegion = brain->getRegion(RegionId(region));
            if (brainRegion) {
                auto regionSynapses = brainRegion->getSynapses();
                debugSynapseDetails(regionSynapses, "Region " + std::to_string(region), verbose, level);
            }
        } else {
            // Debug all synapses
            debugSynapseDetails(synapses, "All Regions", verbose, level);
        }
    }
    
    void validateConnectivity(std::shared_ptr<Brain> brain, bool verbose, int level) {
        std::cout << "Connectivity Validation" << std::endl;
        std::cout << "======================" << std::endl << std::endl;
        
        // Check for isolated neurons
        auto neurons = brain->getAllNeurons();
        int isolatedNeurons = 0;
        
        for (const auto& neuron : neurons) {
            bool hasIncoming = false;
            bool hasOutgoing = false;
            
            // Check incoming connections
            for (const auto& synapse : brain->getAllSynapses()) {
                if (synapse->getPostSynaptic() == neuron) {
                    hasIncoming = true;
                }
                if (synapse->getPreSynaptic() == neuron) {
                    hasOutgoing = true;
                }
            }
            
            if (!hasIncoming && !hasOutgoing) {
                isolatedNeurons++;
            }
        }
        
        std::cout << "Isolated Neurons: " << isolatedNeurons << std::endl;
        
        if (isolatedNeurons > 0) {
            std::cout << "WARNING: " << isolatedNeurons << " neurons have no connections!" << std::endl;
        } else {
            std::cout << "✓ All neurons have at least one connection." << std::endl;
        }
        
        // Check for disconnected regions
        int totalRegions = brain->getRegionCount();
        int disconnectedRegions = 0;
        
        for (RegionId i = 0; i < totalRegions; ++i) {
            auto* region = brain->getRegion(i);
            if (region && region->getAllNeurons().empty()) {
                disconnectedRegions++;
            }
        }
        
        std::cout << "Disconnected Regions: " << disconnectedRegions << std::endl;
        
        if (disconnectedRegions > 0) {
            std::cout << "WARNING: " << disconnectedRegions << " regions have no neurons!" << std::endl;
        }
    }
    
    void analyzeSpikes(std::shared_ptr<Brain> brain, int region, bool verbose, int level) {
        std::cout << "Spike Analysis" << std::endl;
        std::cout << "=============" << std::endl << std::endl;
        
        // Get spike data from status manager
        auto& statusManager = StatusManager::getGlobalStatusManager();
        auto status = statusManager.getBrainStatus();
        
        std::cout << "Total Spikes: " << status.totalSpikeCount << std::endl;
        std::cout << "Average Firing Rate: " << status.averageFiringRate << "%" << std::endl;
        std::cout << "Active Neurons: " << status.activeNeuronCount << std::endl;
        std::cout << "Firing Neurons: " << status.firingNeuronCount << std::endl;
        
        if (verbose) {
            std::cout << "\nDetailed Spike Information:" << std::endl;
            // Add more detailed spike analysis
            std::cout << "  Current Step: " << status.currentStep << std::endl;
            std::cout << "  Current Time: " << status.currentTime << " s" << std::endl;
            std::cout << "  Pending Spike Count: " << status.pendingSpikeCount << std::endl;
        }
    }
    
    void debugPlasticity(std::shared_ptr<Brain> brain, int region, bool verbose, int level) {
        std::cout << "Plasticity Debug Information" << std::endl;
        std::cout << "===========================" << std::endl << std::endl;
        
        // Get all synapses
        auto synapses = brain->getAllSynapses();
        
        std::cout << "Total Synapses: " << synapses.size() << std::endl;
        
        // Check for plasticity-enabled synapses
        int plasticSynapses = 0;
        double totalWeightChange = 0.0;
        
        for (const auto& synapse : synapses) {
            if (synapse->isPlastic()) {
                plasticSynapses++;
                double weightChange = synapse->getWeightChange();
                totalWeightChange += std::abs(weightChange);
                
                if (verbose) {
                    std::cout << "  Synapse " << synapse->getId() << ": "
                              << (synapse->getWeightChange() > 0 ? "+" : "")
                              << synapse->getWeightChange()
                              << " (Plasticity enabled)" << std::endl;
                }
            }
        }
        
        std::cout << "Plastic Synapses: " << plasticSynapses << std::endl;
        std::cout << "Total Weight Change: " << totalWeightChange << std::endl;
        
        if (verbose) {
            std::cout << "\nPlasticity Rules:" << std::endl;
            std::cout << "  - STDP: " << (brain->isSTDPEnabled() ? "Enabled" : "Disabled") << std::endl;
            std::cout << "  - Hebbian: " << (brain->isHebbianPlasticityEnabled() ? "Enabled" : "Disabled") << std::endl;
            std::cout << "  - Structural: " << (brain->isStructuralPlasticityEnabled() ? "Enabled" : "Disabled") << std::endl;
        }
    }
    
    void debugConfiguration(bool verbose, int level) {
        std::cout << "Configuration Debug Information" << std::endl;
        std::cout << "============================" << std::endl << std::endl;
        
        auto config = getConfig();
        auto keys = config->getKeys();
        
        std::cout << "Total Configuration Keys: " << keys.size() << std::endl;
        
        if (verbose) {
            std::cout << "\nConfiguration Details:" << std::endl;
            for (const auto& key : keys) {
                auto value = config->get<std::string>(key);
                if (value.has_value()) {
                    std::cout << "  " << key << ": " << value.value() << std::endl;
                }
            }
        }
        
        // Validate configuration
        std::vector<ValidationError> errors;
        bool isValid = config->isValid(errors);
        
        std::cout << "\nValidation Status: " << (isValid ? "Valid" : "Invalid") << std::endl;
        if (!isValid) {
            std::cout << "Validation Errors: " << errors.size() << std::endl;
            for (const auto& error : errors) {
                std::cout << "  - " << error.message << std::endl;
            }
        }
    }
    
    void debugMemory(bool verbose, int level) {
        std::cout << "Memory Debug Information" << std::endl;
        std::cout << "======================" << std::endl << std::endl;
        
        // Get memory information
        auto& statusManager = StatusManager::getGlobalStatusManager();
        auto status = statusManager.getBrainStatus();
        auto systemStatus = statusManager.getSystemStatus();
        
        std::cout << "Brain Memory Usage: " << CLIFormatters::formatMemory(status.memoryUsage) << std::endl;
        std::cout << "System Memory Total: " << CLIFormatters::formatMemory(systemStatus.memoryTotal) << std::endl;
        std::cout << "System Memory Used: " << CLIFormatters::formatMemory(systemStatus.memoryUsed) << std::endl;
        std::cout << "System Memory Available: " << CLIFormatters::formatMemory(systemStatus.memoryAvailable) << std::endl;
        
        if (verbose) {
            std::cout << "\nMemory Usage Details:" << std::endl;
            std::cout << "  Brain Memory Percentage: " << (systemStatus.memoryTotal > 0 ? 
                (static_cast<double>(status.memoryUsage) / systemStatus.memoryTotal) * 100.0 : 0.0) << "%" << std::endl;
            std::cout << "  System Memory Usage: " << systemStatus.getMemoryUsagePercentage() << "%" << std::endl;
        }
    }
    
    void debugNeuronDetails(const std::vector<std::shared_ptr<Neuron>>& neurons, 
                           const std::string& regionName, bool verbose, int level) {
        for (size_t i = 0; i < neurons.size(); ++i) {
            const auto& neuron = neurons[i];
            
            std::cout << "  Neuron " << i << " (" << neuron->getId() << "):" << std::endl;
            std::cout << "    Type: " << static_cast<int>(neuron->getType()) << std::endl;
            std::cout << "    Membrane Potential: " << neuron->getMembranePotential() << std::endl;
            std::cout << "    Threshold: " << neuron->getThreshold() << std::endl;
            std::cout << "    Refractory Period: " << neuron->getRefractoryPeriod() << std::endl;
            
            if (level >= 2) {
                std::cout << "    Current: " << neuron->getCurrent() << std::endl;
                std::cout << "    Temperature: " << neuron->getTemperature() << std::endl;
            }
            
            if (level >= 3 && verbose) {
                std::cout << "    Input Synapses: " << neuron->getInputSynapses().size() << std::endl;
                std::cout << "    Output Synapses: " << neuron->getOutputSynapses().size() << std::endl;
            }
            
            std::cout << "    Is Firing: " << neuron->isFiring() << std::endl;
            std::cout << "    Is In Refractory: " << neuron->isInRefractoryPeriod() << std::endl;
        }
    }
    
    void debugSynapseDetails(const std::vector<std::shared_ptr<Synapse>>& synapses,
                             const std::string& regionName, bool verbose, int level) {
        for (size_t i = 0; i < synapses.size(); ++i) {
            const auto& synapse = synapses[i];
            
            std::cout << "  Synapse " << i << " (" << synapse->getId() << "):" << std::endl;
            std::cout << "    Pre-synaptic: " << synapse->getPreSynaptic()->getId() << std::endl;
            std::cout << "    Post-synaptic: " << synapse->getPostSynaptic()->getId() << std::endl;
            std::cout << "    Weight: " << synapse->getWeight() << std::endl;
            std::cout << "    Delay: " << synapse->getDelay() << std::endl;
            
            if (level >= 2) {
                std::cout << "    Is Plastic: " << synapse->isPlastic() << std::endl;
                std::cout <<    "    LTP Enabled: " << synapse->isLTP() << std::endl;
                std::cout << "    LTD Enabled: " << synapse->isLTD() << std::endl;
            }
            
            if (level >= 3 && verbose) {
                std::cout << "    Plasticity Parameters:" << std::endl;
                // Add more detailed plasticity information
            }
        }
    }
    
    void exportDebugInfo(const std::string& filename, const std::string& format) {
        std::ofstream file(filename, std::ios::out);
        if (!file.is_open()) {
            CLIFormatters::printError("Cannot write to file: " + filename);
            return;
        }
        
        if (format == "json") {
            // Export as JSON
            file << "{\n";
            file << "  \"debugInfo\": {\n";
            file << "    \"timestamp\": \"" << CLIUtils::getCurrentTimestamp() << "\",\n";
            file << "    \"command\": \"debug\",\n";
            file << "    \"status\": \"exported\"\n";
            file << "  }\n";
            file << "}\n";
        } else {
            // Export as text
            file << "Debug Information Export" << std::endl;
            file << "=======================" << std::endl << std::endl;
            file << "Exported: " << CLIUtils::getCurrentTimestamp() << std::endl;
            file << "Format: Text" << std::endl;
        }
        
        file.close();
        CLIFormatters::printSuccess("Debug information exported to: " + filename);
    }
    
    // Helper methods
    std::shared_ptr<Brain> getBrain() {
        // Get the global brain
        // This should be implemented based on how brain is managed
        static std::shared_ptr<Brain> brain;
        return brain;
    }
    
    std::shared_ptr<Config> getConfig() {
        // Get the global configuration
        // This should be implemented based on how configuration is managed
        static std::shared_ptr<Config> config = std::make_shared<Config>();
        return config;
    }
};

} // namespace nlm
