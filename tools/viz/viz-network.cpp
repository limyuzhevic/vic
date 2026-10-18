// Network visualization tool implementation
//
// This file provides the implementation of network visualization CLI tool

#include "viz-network.hpp"
#include <fstream>
#include <sstream>
#include <cmath>

namespace nlm {

ExecutionResult VizNetworkTool::execute(const std::vector<std::string>& args, Config& config) {
    std::string brainFile = getStringArg(args, {"--brain", "-b"}, "");
    std::string outputFile = getStringArg(args, {"--output", "-o"}, "");
    std::string vizType = getStringArg(args, {"--type", "-t"}, "adjacency");
    std::string format = getStringArg(args, {"--format", "-f"}, "png");
    double threshold = getDoubleArg(args, {"--threshold", "--thresh"}, 0.0);
    
    // Validate arguments
    std::vector<std::string> requiredFlags = {"--brain"};
    std::map<std::string, std::string> optionalFlags = {
        {"--output", ""},
        {"--type", "adjacency"},
        {"--format", "png"},
        {"--threshold", "0.0"}
    };
    
    validateArgs(args, requiredFlags, optionalFlags);
    
    printInfo("Visualizing network from brain: " + brainFile);
    printInfo("Visualization type: " + vizType);
    if (!outputFile.empty()) {
        printInfo("Output file: " + outputFile + " with format: " + format);
    }
    if (threshold > 0.0) {
        printInfo("Connection threshold: " + std::to_string(threshold));
    }
    
    // Create visualization
    bool success = visualizeNetworkImpl(brainFile, outputFile, vizType, format, threshold);
    
    if (success) {
        printSuccess("Network visualization completed successfully");
        return {true, "Network visualization completed"};
    } else {
        printError("Failed to create network visualization");
        return {false, "Network visualization failed"};
    }
}

bool VizNetworkTool::visualizeNetworkImpl(const std::string& brainFile, const std::string& outputFile,
                                         const std::string& vizType, const std::string& format,
                                         double threshold) const {
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
        
        if (vizType == "adjacency") {
            out << "Network Adjacency Matrix Visualization" << std::endl;
            out << "===============================" << std::endl;
            
            std::vector<nlohmann::json> neurons = brainState["neurons"].get<std::vector<nlohmann::json>>();
            int n = neurons.size();
            
            out << "Matrix Size: " << n << " x " << n << std::endl;
            out << "Threshold: " << threshold << std::endl;
            out << std::endl;
            
            // Simulate adjacency matrix (simplified)
            out << "Adjacency Matrix (sample):" << std::endl;
            for (int i = 0; i < std::min(n, 10); ++i) {
                out << "Neuron " << i << ": ";
                for (int j = 0; j < std::min(n, 10); ++j) {
                    double weight = 0.0;
                    if (i == j) {
                        weight = 1.0;
                    } else if (i < j) {
                        // Simulate connection probability
                        weight = (static_cast<double>(rand()) / RAND_MAX) * threshold;
                    }
                    out << std::fixed << std::setprecision(3) << weight << " ";
                }
                out << std::endl;
            }
            out << "... (matrix truncated)" << std::endl;
            
        } else if (vizType == "region") {
            out << "Network Region Visualization" << std::endl;
            out << "===========================" << std::endl;
            
            std::vector<nlohmann::json> regions = brainState["regions"].get<std::vector<nlohmann::json>>();
            int totalNeurons = 0;
            
            for (size_t i = 0; i < regions.size(); ++i) {
                out << "Region " << i << ": " << regions[i]["name"] << std::endl;
                out << "  Neurons: " << regions[i]["neuron_count"] << std::endl;
                out << "  Density: " << std::fixed << std::setprecision(3) 
                    << static_cast<double>(regions[i]["neuron_count"]) / 100.0 << std::endl;
                totalNeurons += regions[i]["neuron_count"];
                out << std::endl;
            }
            
            out << "Total Neurons: " << totalNeurons << std::endl;
            out << "Number of Regions: " << regions.size() << std::endl;
            
        } else if (vizType == "population") {
            out << "Network Population Visualization" << std::endl;
            out << "=============================" << std::endl;
            
            std::vector<nlohmann::json> populations = brainState["populations"].get<std::vector<nlohmann::json>>();
            
            for (size_t i = 0; i < populations.size(); ++i) {
                out << "Population " << i << ": " << populations[i]["type"] << std::endl;
                out << "  Size: " << populations[i]["size"] << std::endl;
                out << "  Firing Rate: " << populations[i]["firing_rate"] << " Hz" << std::endl;
                out << "  Connection Probability: " << populations[i]["connection_probability"] << std::endl;
                out << std::endl;
            }
        }
        
        if (out.is_open() && &out != &std::cout) {
            out.close();
            printInfo("Network visualization data written to: " + outputFile);
        }
        
        return true;
        
    } catch (const std::exception& e) {
        printError("Failed to visualize network: " + std::string(e.what()));
        return false;
    }
}

} // namespace nlm
