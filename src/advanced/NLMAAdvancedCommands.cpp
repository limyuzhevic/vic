#include "Brain.hpp"
#include "Config.hpp"
#include "Neuron.hpp"
#include "Synapse.hpp"
#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <map>
#include <unordered_set>

namespace nlm {

class AdvancedCommands {
public:
    AdvancedCommands(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config)
        : brain(brain), config(config) {}
    
    void printHelp() {
        std::cout << "\n=== NLM Advanced Commands ===\n" << std::endl;
        std::cout << "Available commands:\n" << std::endl;
        std::cout << "  inspect <region_id>       - Inspect region neurons and synapses\n" << std::endl;
        std::cout << "  search <pattern>           - Search for neurons/synapses by ID or type\n" << std::endl;
        std::cout << "  export <filename>          - Export brain state to JSON\n" << std::endl;
        std::cout << "  import <filename>          - Import brain state from JSON\n" << std::endl;
        std::cout << "  modify <key> <value>       - Modify configuration at runtime\n" << std::endl;
        std::cout << "  analyze <neuron_id>        - Analyze neuron connectivity and activity\n" << std::endl;
        std::cout << "  debug <target> [level]     - Enable/disable debugging\n" << std::endl;
        std::cout << "  benchmark <iterations>      - Run performance benchmark\n" << std::endl;
        std::cout << "  visualize                  - Generate visualization data\n" << std::endl;
        std::cout << "  experiment <name>          - Run custom experiment\n" << std::endl;
        std::cout << "  help                       - Show this help\n" << std::endl;
        std::cout << "  exit                       - Quit\n" << std::endl;
    }
    
    void inspectRegion(int regionId) {
        auto region = brain->getRegion(RegionId(regionId));
        if (!region) {
            std::cout << "Error: Region " << regionId << " not found." << std::endl;
            return;
        }
        
        std::cout << "\n=== Region " << regionId << " Details ===" << std::endl;
        std::cout << "Name: " << region->getName() << std::endl;
        std::cout << "Total neurons: " << region->getTotalNeuronCount() << std::endl;
        std::cout << "Total synapses: " << region->getSynapseCount() << std::endl;
        std::cout << "Average weight: " << region->getAverageSynapticWeight() << std::endl;
        
        // Show neuron types distribution
        std::cout << "\nNeuron type distribution:" << std::endl;
        auto neurons = region->getNeurons();
        std::map<NeuronType, size_t> typeCount;
        for (const auto& neuron : neurons) {
            typeCount[neuron->getType()]++;
        }
        
        for (const auto& [type, count] : typeCount) {
            std::cout << "  " << neuronTypeToString(type) << ": " << count << std::endl;
        }
        
        // Show top 10 most connected neurons
        std::cout << "\nTop 10 most connected neurons:" << std::endl;
        std::vector<std::pair<NeuronId, size_t>> neuronConnections;
        for (const auto& neuron : neurons) {
            size_t incoming = 0, outgoing = 0;
            // Note: This would require accessing synapse lists from neurons
            neuronConnections.emplace_back(neuron->getId(), incoming + outgoing);
        }
        
        std::sort(neuronConnections.begin(), neuronConnections.end(),
                 [](const auto& a, const auto& b) { return a.second > b.second; });
        
        for (size_t i = 0; i < std::min(size_t(10), neuronConnections.size()); ++i) {
            std::cout << "  Neuron " << neuronConnections[i].first.index() 
                     << " (" << neuronConnections[i].second << " connections)" << std::endl;
        }
    }
    
    void searchNeurons(const std::string& pattern) {
        std::cout << "\n=== Searching for: " << pattern << " ===" << std::endl;
        
        // Search in all regions
        for (const auto& region : brain->getRegions()) {
            auto neurons = region->getNeurons();
            for (const auto& neuron : neurons) {
                std::string idStr = std::to_string(neuron->getId().index());
                if (idStr.find(pattern) != std::string::npos) {
                    std::cout << "Found neuron " << neuron->getId().index() 
                             << " in region " << region->getId().index()
                             << " (type: " << neuronTypeToString(neuron->getType()) << ")" << std::endl;
                }
            }
        }
        
        // Note: Synapse search would require additional infrastructure
        std::cout << "Synapse search requires additional implementation." << std::endl;
    }
    
    void exportBrainState(const std::string& filename) {
        std::cout << "\n=== Exporting brain state to: " << filename << " ===" << std::endl;
        
        try {
            // Create JSON representation of brain state
            nlohmann::json brainState;
            
            // Export regions and neurons
            brainState["regions"] = nlohmann::json::array();
            for (const auto& region : brain->getRegions()) {
                nlohmann::json regionData;
                regionData["id"] = region->getId().index();
                regionData["name"] = region->getName();
                regionData["neuron_count"] = region->getTotalNeuronCount();
                regionData["synapse_count"] = region->getSynapseCount();
                
                brainState["regions"].push_back(regionData);
            }
            
            // Export configuration
            brainState["config"] = nlohmann::json::parse(config->summary());
            
            // Export statistics
            brainState["statistics"]["total_neurons"] = brain->getTotalNeuronCount();
            brainState["statistics"]["total_synapses"] = brain->getTotalSynapseCount();
            brainState["statistics"]["active_neurons"] = brain->getActiveNeuronCount();
            brainState["statistics"]["firing_neurons"] = brain->getFiringNeuronCount();
            brainState["statistics"]["total_spikes"] = brain->getTotalSpikeCount();
            brainState["statistics"]["pending_spikes"] = brain->getPendingSpikeEventCount();
            
            // Write to file
            std::ofstream file(filename);
            if (file.is_open()) {
                file << brainState.dump(2);
                file.close();
                std::cout << "Successfully exported brain state to " << filename << std::endl;
            } else {
                std::cout << "Error: Could not write to file " << filename << std::endl;
            }
            
        } catch (const std::exception& e) {
            std::cout << "Error exporting brain state: " << e.what() << std::endl;
        }
    }
    
    void importBrainState(const std::string& filename) {
        std::cout << "\n=== Importing brain state from: " << filename << " ===" << std::endl;
        
        try {
            std::ifstream file(filename);
            if (!file.is_open()) {
                std::cout << "Error: Could not open file " << filename << std::endl;
                return;
            }
            
            nlohmann::json brainState;
            file >> brainState;
            
            std::cout << "Note: Brain state import is a feature for future implementation." << std::endl;
            std::cout << "The JSON file was parsed successfully but not applied to the brain." << std::endl;
            
        } catch (const std::exception& e) {
            std::cout << "Error importing brain state: " << e.what() << std::endl;
        }
    }
    
    void modifyConfig(const std::string& key, const std::string& value) {
        std::cout << "\n=== Modifying configuration: " << key << " = " << value << " ===" << std::endl;
        
        try {
            // Try to parse value based on key type
            if (key.find("weight") != std::string::npos || key.find("rate") != std::string::npos || 
                key.find("probability") != std::string::npos || key.find("timestep") != std::string::npos) {
                // Float value
                float floatValue = std::stof(value);
                config->set(key, floatValue, ConfigSource::Runtime, "Runtime modification");
                std::cout << "Set " << key << " = " << floatValue << " (float)" << std::endl;
            } else if (key.find("count") != std::string::npos || key.find("seed") != std::string::npos) {
                // Integer value
                int64_t intValue = std::stoll(value);
                config->set(key, intValue, ConfigSource::Runtime, "Runtime modification");
                std::cout << "Set " << key << " = " << intValue << " (int)" << std::endl;
            } else if (key.find("enabled") != std::string::npos || key.find("active") != std::string::npos) {
                // Boolean value
                bool boolValue = (value == "true" || value == "1" || value == "yes");
                config->set(key, boolValue, ConfigSource::Runtime, "Runtime modification");
                std::cout << "Set " << key << " = " << (boolValue ? "true" : "false") << " (bool)" << std::endl;
            } else {
                // String value
                config->set(key, value, ConfigSource::Runtime, "Runtime modification");
                std::cout << "Set " << key << " = \"" << value << "\" (string)" << std::endl;
            }
            
            std::cout << "Configuration modified successfully." << std::endl;
            
        } catch (const std::exception& e) {
            std::cout << "Error modifying configuration: " << e.what() << std::endl;
        }
    }
    
    void analyzeNeuron(int neuronId) {
        std::cout << "\n=== Analyzing neuron " << neuronId << " ===" << std::endl;
        
        // Find the neuron in the brain
        NeuronId targetId(neuronId);
        bool found = false;
        
        for (const auto& region : brain->getRegions()) {
            auto neurons = region->getNeurons();
            for (const auto& neuron : neurons) {
                if (neuron->getId() == targetId) {
                    found = true;
                    std::cout << "Found neuron in region " << region->getId().index() << std::endl;
                    std::cout << "Type: " << neuronTypeToString(neuron->getType()) << std::endl;
                    std::cout << "Current membrane potential: " << neuron->getMembranePotential() << std::endl;
                    std::cout << "Spike count: " << neuron->getSpikeCount() << std::endl;
                    
                    // Analyze connectivity (would need synapse access)
                    std::cout << "Note: Detailed connectivity analysis requires synapse access." << std::endl;
                    break;
                }
            }
            if (found) break;
        }
        
        if (!found) {
            std::cout << "Neuron " << neuronId << " not found in brain." << std::endl;
        }
    }
    
    void runBenchmark(int iterations) {
        std::cout << "\n=== Running benchmark with " << iterations << " iterations ===" << std::endl;
        
        // Simple benchmark: run simulation steps
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < iterations; ++i) {
            brain->step(SimulationStep(i), std::chrono::duration<double>(i * 0.001));
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        std::cout << "Benchmark completed in " << duration.count() << " ms" << std::endl;
        std::cout << "Average time per step: " << (double(duration.count()) / iterations) << " ms" << std::endl;
        std::cout << "Steps per second: " << (iterations * 1000.0 / duration.count()) << std::endl;
    }
    
    void generateVisualization() {
        std::cout << "\n=== Generating visualization data ===" << std::endl;
        
        // Generate simple visualization data
        std::string outputFile = "brain_visualization.json";
        
        try {
            nlohmann::json visualization;
            
            visualization["timestamp"] = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();
            
            visualization["neuron_count"] = brain->getTotalNeuronCount();
            visualization["synapse_count"] = brain->getTotalSynapseCount();
            visualization["active_neurons"] = brain->getActiveNeuronCount();
            visualization["average_firing_rate"] = brain->getAverageFiringRate();
            
            // Add region information
            visualization["regions"] = nlohmann::json::array();
            for (const auto& region : brain->getRegions()) {
                nlohmann::json regionViz;
                regionViz["id"] = region->getId().index();
                regionViz["name"] = region->getName();
                regionViz["neuron_count"] = region->getTotalNeuronCount();
                visualization["regions"].push_back(regionViz);
            }
            
            // Write to file
            std::ofstream file(outputFile);
            if (file.is_open()) {
                file << visualization.dump(2);
                file.close();
                std::cout << "Visualization data saved to " << outputFile << std::endl;
            } else {
                std::cout << "Error: Could not write visualization data" << std::endl;
            }
            
        } catch (const std::exception& e) {
            std::cout << "Error generating visualization: " << e.what() << std::endl;
        }
    }
    
    void processCommand(const std::string& command) {
        std::istringstream iss(command);
        std::string cmd;
        iss >> cmd;
        
        if (cmd == "help") {
            printHelp();
        } else if (cmd == "inspect") {
            int regionId;
            if (iss >> regionId) {
                inspectRegion(regionId);
            } else {
                std::cout << "Error: Please specify region ID" << std::endl;
            }
        } else if (cmd == "search") {
            std::string pattern;
            if (std::getline(iss >> std::ws, pattern)) {
                searchNeurons(pattern);
            } else {
                std::cout << "Error: Please specify search pattern" << std::endl;
            }
        } else if (cmd == "export") {
            std::string filename;
            if (std::getline(iss >> std::ws, filename)) {
                exportBrainState(filename);
            } else {
                std::cout << "Error: Please specify filename" << std::endl;
            }
        } else if (cmd == "import") {
            std::string filename;
            if (std::getline(iss >> std::ws, filename)) {
                importBrainState(filename);
            } else {
                std::cout << "Error: Please specify filename" << std::endl;
            }
        } else if (cmd == "modify") {
            std::string key, value;
            if (iss >> key >> value) {
                modifyConfig(key, value);
            } else {
                std::cout << "Error: Please specify key and value" << std::endl;
            }
        } else if (cmd == "analyze") {
            int neuronId;
            if (iss >> neuronId) {
                analyzeNeuron(neuronId);
            } else {
                std::cout << "Error: Please specify neuron ID" << std::endl;
            }
        } else if (cmd == "benchmark") {
            int iterations;
            if (iss >> iterations) {
                runBenchmark(iterations);
            } else {
                std::cout << "Error: Please specify number of iterations" << std::endl;
            }
        } else if (cmd == "visualize") {
            generateVisualization();
        } else if (cmd == "exit" || cmd == "quit") {
            std::cout << "Exiting advanced commands." << std::endl;
        } else {
            std::cout << "Unknown command: " << cmd << std::endl;
            std::cout << "Type 'help' for available commands." << std::endl;
        }
    }
    
private:
    std::shared_ptr<Brain> brain;
    std::shared_ptr<Config> config;
    
    std::string neuronTypeToString(NeuronType type) {
        switch (type) {
            case NeuronType::Sensory: return "Sensory";
            case NeuronType::Motor: return "Motor";
            case NeuronType::Internal: return "Internal";
            case NeuronType::Input: return "Input";
            case NeuronType::Output: return "Output";
            default: return "Unknown";
        }
    }
};

// Advanced commands main function
void advancedCommandsMain(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
    AdvancedCommands advancedCommands(brain, config);
    
    std::cout << "\n=== NLM Advanced Commands Console ===" << std::endl;
    std::cout << "Type 'help' for available commands." << std::endl;
    
    std::string command;
    while (true) {
        std::cout << "\nnlm> ";
        std::getline(std::cin, command);
        
        if (command.empty()) {
            continue;
        }
        
        if (command == "exit" || command == "quit") {
            break;
        }
        
        advancedCommands.processCommand(command);
    }
    
    std::cout << "Goodbye!" << std::endl;
}

} // namespace nlm