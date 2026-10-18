// Synapse visualization tool implementation
//
// This file provides the implementation of synapse visualization CLI tool

#include "viz-synapses.hpp"
#include <fstream>
#include <sstream>
#include <cmath>

namespace nlm {

ExecutionResult VizSynapsesTool::execute(const std::vector<std::string>& args, Config& config) {
    std::string brainFile = getStringArg(args, {"--brain", "-b"}, "");
    bool plasticity = hasArg(args, {"--plasticity", "--plasticity-enabled"});
    std::string outputFile = getStringArg(args, {"--output", "-o"}, "");
    std::string format = getStringArg(args, {"--format", "-f"}, "png");
    std::string vizType = getStringArg(args, {"--type", "-t"}, "structural");
    
    // Validate arguments
    std::vector<std::string> requiredFlags = {"--brain"};
    std::map<std::string, std::string> optionalFlags = {
        {"--plasticity", "false"},
        {"--output", ""},
        {"--format", "png"},
        {"--type", "structural"}
    };
    
    validateArgs(args, requiredFlags, optionalFlags);
    
    printInfo("Visualizing synapses from brain: " + brainFile);
    printInfo("Plasticity enabled: " + std::string(plasticity ? "true" : "false"));
    if (!outputFile.empty()) {
        printInfo("Output file: " + outputFile + " with format: " + format);
    }
    printInfo("Visualization type: " + vizType);
    
    // Create visualization
    bool success = visualizeSynapsesImpl(brainFile, plasticity, outputFile, format, vizType);
    
    if (success) {
        printSuccess("Synapse visualization completed successfully");
        return {true, "Synapse visualization completed"};
    } else {
        printError("Failed to create synapse visualization");
        return {false, "Synapse visualization failed"};
    }
}

bool VizSynapsesTool::visualizeSynapsesImpl(const std::string& brainFile, bool plasticity,
                                           const std::string& outputFile, const std::string& format,
                                           const std::string& vizType) const {
    if (!fileExists(brainFile)) {
        printError("Brain state file does not exist: " + brainFile);
        return false;
    }
    
    try {
        nlohmann::json brainState = parseJsonFromFile(brainFile);
        
        std::ofstream out;
        if (!outputFile.empty()) {
            out.open(outputFile);
            if (!out.is_open()) {
                printError("Cannot open output file: " + outputFile);
                return false;
            }
        } else {
            out = std::cout;
        }
        
        out << "Synaptic Connectivity Visualization" << std::endl;
        out << "================================" << std::endl;
        out << "Brain File: " << brainFile << std::endl;
        out << "Plasticity Enabled: " + std::string(plasticity ? "true" : "false") << std::endl;
        out << "Visualization Type: " + vizType << std::endl;
        out << std::endl;
        
        if (vizType == "structural") {
            out << "Structural Synapse Connections:" << std::endl;
            
            std::vector<nlohmann::json> synapses = brainState["synapses"].get<std::vector<nlohmann::json>>();
            out << "Total Synapses: " << synapses.size() << std::endl;
            
            // Group by type
            std::map<std::string, int> synapseTypes;
            for (const auto& synapse : synapses) {
                std::string type = synapse["type"].get<std::string>();
                synapseTypes[type]++;
            }
            
            out << "Synapse Types:" << std::endl;
            for (const auto& pair : synapseTypes) {
                out << "  " << pair.first << ": " << pair.second << std::endl;
            }
            
            // Show sample connections
            out << std::endl << "Sample Connections (first 10):" << std::endl;
            int count = 0;
            for (const auto& synapse : synapses) {
                if (count >= 10) break;
                out << "  Pre: " << synapse["pre_synaptic"] << ", Post: " << synapse["post_synaptic"]
                    << ", Weight: " << std::fixed << std::setprecision(3) << synapse["weight"] << std::endl;
                count++;
            }
            
        } else if (vizType == "functional") {
            out << "Functional Synapse Activity:" << std::endl;
            
            // Simulate functional connectivity analysis
            std::vector<nlohmann::json> functionalConns = brainState["functional_connectivity"].get<std::vector<nlohmann::json>>();
            out << "Functional Connections: " << functionalConns.size() << std::endl;
            
            double totalStrength = 0.0;
            for (const auto& conn : functionalConns) {
                totalStrength += conn["strength"].get<double>();
            }
            
            if (!functionalConns.empty()) {
                out << "Average Functional Strength: " << std::fixed << std::setprecision(3) 
                    << totalStrength / functionalConns.size() << std::endl;
            }
            
        } else if (vizType == "temporal") {
            out << "Temporal Synapse Dynamics:" << std::endl;
            
            // Simulate temporal plasticity data
            std::vector<nlohmann::json> temporalData = brainState["temporal_data"].get<std::vector<nlohmann::json>>();
            out << "Time Points: " << temporalData.size() << std::endl;
            
            if (!temporalData.empty()) {
                out << "Time Range: " << temporalData[0]["time"].get<double>() << " to "
                    << temporalData.back()["time"].get<double>() << " ms" << std::endl;
                
                // Calculate plasticity metrics
                double avgPlasticity = 0.0;
                int validPoints = 0;
                for (const auto& point : temporalData) {
                    if (point.contains("plasticity_magnitude")) {
                        avgPlasticity += point["plasticity_magnitude"].get<double>();
                        validPoints++;
                    }
                }
                
                if (validPoints > 0) {
                    out << "Average Plasticity Magnitude: " << std::fixed << std::setprecision(3)
                        << avgPlasticity / validPoints << std::endl;
                }
            }
        }
        
        if (out.is_open() && &out != &std::cout) {
            out.close();
            printInfo("Synapse visualization data written to: " + outputFile);
        }
        
        return true;
        
    } catch (const std::exception& e) {
        printError("Failed to visualize synapses: " + std::string(e.what()));
        return false;
    }
}

} // namespace nlm
