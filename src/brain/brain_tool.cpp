// Brain Tool Utility
// Advanced command-line tools for brain analysis and manipulation

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <iomanip>
#include <cmath>

using namespace nlm;

class BrainTool {
public:
    // Initialize with configuration
    bool initialize(const std::string& configFile = "configs/default.cfg") {
        auto config = std::make_shared<Config>();
        if (config->loadFromFile(configFile)) {
            NLM_LOG_INFO("Loaded config from: " + configFile);
        }
        
        // Set default values if not present
        if (!config->has("random_seed")) {
            config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        }
        if (!config->has("neuron_count")) {
            config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        }
        if (!config->has("region_count")) {
            config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        }
        
        brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain");
            return false;
        }
        
        return true;
    }
    
    // Save brain state to checkpoint
    bool saveCheckpoint(const std::string& filepath) {
        return brain->save(filepath);
    }
    
    // Load brain state from checkpoint
    bool loadCheckpoint(const std::string& filepath) {
        return brain->load(filepath);
    }
    
    // Analyze brain connectivity
    std::string analyzeConnectivity() {
        std::ostringstream oss;
        oss << "Brain Connectivity Analysis" << std::endl;
        oss << "=========================" << std::endl;
        
        float eiRatio = brain->getExcitationInhibitionRatio();
        oss << "Excitatory/Inhibitory Ratio: " << eiRatio << std::endl;
        oss << "Total Neurons: " << brain->getTotalNeuronCount() << std::endl;
        oss << "Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
        oss << "Active Neurons: " << brain->getActiveNeuronCount() << std::endl;
        oss << "Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
        oss << "Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
        oss << "Pending Spike Events: " << brain->getPendingSpikeEventCount() << std::endl;
        
        // Analyze regions
        oss << "\nRegion Analysis:" << std::endl;
        for (const auto& region : brain->getRegions()) {
            oss << "  Region " << region->getId().index() << ": " << region->getName()
                << " - " << region->getTotalNeuronCount() << " neurons, "
                << region->getSynapseCount() << " synapses"
                << " (avg weight: " << region->getAverageSynapticWeight() << ")" << std::endl;
        }
        
        return oss.str();
    }
    
    // Generate brain summary report
    std::string generateReport() {
        std::ostringstream oss;
        oss << "NLM Brain Summary Report" << std::endl;
        oss << "=========================" << std::endl;
        oss << "Generated: " << __DATE__ << " " << __TIME__ << std::endl;
        oss << "Version: Phase 6 (Final Integration)" << std::endl;
        oss << std::endl;
        
        oss << "Brain Status:" << std::endl;
        oss << "  Total Neurons: " << brain->getTotalNeuronCount() << std::endl;
        oss << "  Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
        oss << "  Active Neurons: " << brain->getActiveNeuronCount() << std::endl;
        oss << "  Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
        oss << "  Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
        oss << "  Average Firing Rate: " << brain->getAverageFiringRate() << std::endl;
        oss << "  E/I Ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
        oss << std::endl;
        
        oss << "System Integration:" << std::endl;
        // Check memory systems
        if (brain->getWorkingMemory()) {
            oss << "  Working Memory: CONNECTED" << std::endl;
        } else {
            oss << "  Working Memory: NOT CONNECTED" << std::endl;
        }
        
        if (brain->getEpisodicMemory()) {
            oss << "  Episodic Memory: CONNECTED" << std::endl;
        } else {
            oss << "  Episodic Memory: NOT CONNECTED" << std::endl;
        }
        
        // Check neuromodulation
        if (brain->getDopamine()) {
            oss << "  Neuromodulation: CONNECTED" << std::endl;
        } else {
            oss << "  Neuromodulation: NOT CONNECTED" << std::endl;
        }
        
        // Check development
        if (brain->getDevelopmentSystem()) {
            oss << "  Development: CONNECTED" << std::endl;
        } else {
            oss << "  Development: NOT CONNECTED" << std::endl;
        }
        
        oss << std::endl;
        oss << "Regions:" << std::endl;
        for (const auto& region : brain->getRegions()) {
            oss << "  Region " << region->getId().index() << ": " << region->getName()
                << " - " << region->getTotalNeuronCount() << " neurons"
                << " (connectivity: " << (region->getSynapseCount() / std::max(1u, region->getTotalNeuronCount()) * 100) << "%)" << std::endl;
        }
        
        return oss.str();
    }
    
    // Create brain configuration file
    bool createConfigFile(const std::string& filepath, const std::string& name) {
        auto config = std::make_shared<Config>();
        
        // Set basic configuration
        config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Runtime);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Runtime);
        config->set("connection_probability", 0.15f, ConfigSource::Runtime);
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Runtime);
        
        // Add description
        config->set("description", name, ConfigSource::Runtime);
        config->set("created_by", "BrainTool", ConfigSource::Runtime);
        config->set("created_date", "2026-10-27", ConfigSource::Runtime);
        
        return config->saveToFile(filepath);
    }
    
    // Run simulation steps
    bool runSteps(size_t steps) {
        for (SimulationStep step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
        }
        return true;
    }
    
    // Reset brain to initial state
    void resetBrain() {
        brain->reset();
        brain->initialize();
        NLM_LOG_INFO("Brain has been reset to initial state");
    }
    
    // Get brain reference
    std::shared_ptr<Brain> getBrain() { return brain; }
    
    // Print help
    static void printHelp() {
        std::cout << "Brain Tool Utilities" << std::endl;
        std::cout << "===================" << std::endl;
        std::cout << "Available commands:" << std::endl;
        std::cout << "  help              Show this help message" << std::endl;
        std::cout << "  save <file>       Save brain state to checkpoint file" << std::endl;
        std::cout << "  load <file>       Load brain state from checkpoint file" << std::endl;
        std::cout << "  analyze           Analyze brain connectivity and statistics" << std::endl;
        std::cout << "  report            Generate detailed brain report" << std::endl;
        std::cout << "  create-config <name> <file> Create new config file with name" << std::endl;
        std::cout << "  run <steps>       Run specified number of simulation steps" << std::endl;
        std::cout << "  reset             Reset brain to initial state" << std::endl;
        std::cout << "  exit             Exit Brain Tool" << std::endl;
        std::cout << std::endl;
        std::cout << "Example usage:" << std::endl;
        std::cout << "  brain_tool save brain_checkpoint.bin" << std::endl;
        std::cout << "  brain_tool load brain_checkpoint.bin" << std::endl;
        std::cout << "  brain_tool run 1000" << std::endl;
        std::cout << "  brain_tool analyze" << std::endl;
    }
    
private:
    std::shared_ptr<Brain> brain;
};

int main(int argc, char** argv) {
    printBanner();
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    BrainTool tool;
    
    // Check if config file is provided
    std::string configFile = "configs/default.cfg";
    if (argc > 1 && std::string(argv[1]) != "help" && std::string(argv[1]) != "help") {
        configFile = "configs/default.cfg";  // Use default for now
        if (!tool.initialize(configFile)) {
            NLM_LOG_ERROR("Failed to initialize BrainTool");
            return 1;
        }
    }
    
    std::string command;
    size_t steps;
    std::string filename;
    std::string configName;
    
    while (true) {
        std::cout << "brain_tool> ";
        std::string input;
        std::getline(std::cin, input);
        
        if (input.empty()) {
            continue;
        }
        
        std::istringstream iss(input);
        iss >> command;
        
        if (command == "exit" || command == "quit") {
            break;
        } else if (command == "help") {
            BrainTool::printHelp();
        } else if (command == "save") {
            if (iss >> filename) {
                if (tool.saveCheckpoint(filename)) {
                    NLM_LOG_INFO("Brain state saved to: " + filename);
                } else {
                    NLM_LOG_ERROR("Failed to save brain state to: " + filename);
                }
            } else {
                NLM_LOG_ERROR("Usage: save <filepath>");
            }
        } else if (command == "load") {
            if (iss >> filename) {
                if (tool.loadCheckpoint(filename)) {
                    NLM_LOG_INFO("Brain state loaded from: " + filename);
                } else {
                    NLM_LOG_ERROR("Failed to load brain state from: " + filename);
                }
            } else {
                NLM_LOG_ERROR("Usage: load <filepath>");
            }
        } else if (command == "analyze") {
            std::string result = tool.analyzeConnectivity();
            std::cout << result << std::endl;
        } else if (command == "report") {
            std::string result = tool.generateReport();
            std::cout << result << std::endl;
        } else if (command == "create-config") {
            std::string name, configFile;
            if (iss >> name >> configFile) {
                if (tool.createConfigFile(configFile, name)) {
                    NLM_LOG_INFO("Config file created: " + configFile);
                } else {
                    NLM_LOG_ERROR("Failed to create config file: " + configFile);
                }
            } else {
                NLM_LOG_ERROR("Usage: create-config <name> <filepath>");
            }
        } else if (command == "run") {
            if (iss >> steps) {
                if (tool.runSteps(steps)) {
                    NLM_LOG_INFO("Ran " + std::to_string(steps) + " simulation steps");
                } else {
                    NLM_LOG_ERROR("Failed to run steps");
                }
            } else {
                NLM_LOG_ERROR("Usage: run <steps>");
            }
        } else if (command == "reset") {
            tool.resetBrain();
        } else {
            NLM_LOG_WARNING("Unknown command: " + command);
            BrainTool::printHelp();
        }
    }
    
    NLM_LOG_INFO("Exiting Brain Tool...");
    return 0;
}

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     BRAIN TOOL — Advanced Brain Utilities                    ║
    ║                                                               ║
    ║     Phase 2/6: Neural Computation Tools                        ║
    ║                                                               ║
    ║     Tools for brain analysis, manipulation, and management.   ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}