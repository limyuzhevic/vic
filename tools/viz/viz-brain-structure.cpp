// Brain structure visualization tool implementation
//
// This file provides the implementation of brain structure visualization CLI tool

#include "viz-brain-structure.hpp"
#include <fstream>
#include <sstream>

namespace nlm {

ExecutionResult VizBrainStructureTool::execute(const std::vector<std::string>& args, Config& config) {
    std::string brainFile = getStringArg(args, {"--brain", "-b"}, "");
    std::string regionType = getStringArg(args, {"--region-type", "--type"}, "all");
    std::string outputFile = getStringArg(args, {"--output", "-o"}, "");
    std::string format = getStringArg(args, {"--format", "-f"}, "png");
    bool detailed = hasArg(args, {"--detailed", "--detailed-view"});
    
    // Validate arguments
    std::vector<std::string> requiredFlags = {"--brain"};
    std::map<std::string, std::string> optionalFlags = {
        {"--region-type", "all"},
        {"--output", ""},
        {"--format", "png"},
        {"--detailed", "false"}
    };
    
    validateArgs(args, requiredFlags, optionalFlags);
    
    printInfo("Visualizing brain structure from: " + brainFile);
    printInfo("Region type: " + regionType);
    printInfo("Detailed view: " + std::string(detailed ? "true" : "false"));
    if (!outputFile.empty()) {
        printInfo("Output file: " + outputFile + " with format: " + format);
    }
    
    // Create visualization
    bool success = visualizeBrainStructureImpl(brainFile, regionType, outputFile, format, detailed);
    
    if (success) {
        printSuccess("Brain structure visualization completed successfully");
        return {true, "Brain structure visualization completed"};
    } else {
        printError("Failed to create brain structure visualization");
        return {false, "Brain structure visualization failed"};
    }
}

bool VizBrainStructureTool::visualizeBrainStructureImpl(const std::string& brainFile, const std::string& regionType,
                                                       const std::string& outputFile, const std::string& format,
                                                       bool detailed) const {
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
        
        out << "Brain Structure Visualization" << std::endl;
        out << "============================" << std::endl;
        out << "Brain File: " << brainFile << std::endl;
        out << "Region Type: " << regionType << std::endl;
        out << "Detailed View: " + std::string(detailed ? "true" : "false") << std::endl;
        out << std::endl;
        
        if (regionType == "all" || regionType == "cortex") {
            out << "Cortical Regions:" << std::endl;
            
            std::vector<nlohmann::json> cortexRegions = brainState["cortex_regions"].get<std::vector<nlohmann::json>>();
            for (const auto& region : cortexRegions) {
                out << "  " << region["name"] << ": " << region["neuron_count"] << " neurons" << std::endl;
                if (detailed) {
                    out << "    Area: " << region["area"] << " mm²" << std::endl;
                    out << "    Layer Count: " << region["layer_count"] << std::endl;
                    out << "    Connectivity: " << region["connectivity"] << std::endl;
                }
            }
        }
        
        if (regionType == "all" || regionType == "subcortex") {
            out << std::endl << "Subcortical Regions:" << std::endl;
            
            std::vector<nlohmann::json> subcortexRegions = brainState["subcortex_regions"].get<std::vector<nlohmann::json>>();
            for (const auto& region : subcortexRegions) {
                out << "  " << region["name"] << ": " << region["neuron_count"] << " neurons" << std::endl;
                if (detailed) {
                    out << "    Volume: " << region["volume"] << " mm³" << std::endl;
                    out << "    Function: " << region["function"] << std::endl;
                    out << "    Blood Flow: " << region["blood_flow"] << " ml/min" << std::endl;
                }
            }
        }
        
        if (regionType == "all" || regionType == "brainstem") {
            out << std::endl << "Brainstem Regions:" << std::endl;
            
            std::vector<nlohmann::json> brainstemRegions = brainState["brainstem_regions"].get<std::vector<nlohmann::json>>();
            for (const auto& region : brainstemRegions) {
                out << "  " << region["name"] << ": " << region["neuron_count"] << " neurons" << std::endl;
                if (detailed) {
                    out << "    Function: " << region["function"] << std::endl;
                    out << "    Control: " << region["control"] << std::endl;
                }
            }
        }
        
        // Overall statistics
        out << std::endl << "Overall Statistics:" << std::endl;
        out << "  Total Neurons: " << brainState["total_neurons"].get<int64_t>() << std::endl;
        out << "  Total Regions: " << brainState["region_count"].get<int64_t>() << std::endl;
        out << "  Total Volume: " << brainState["volume"].get<double>() << " mm³" << std::endl;
        
        if (out.is_open() && &out != &std::cout) {
            out.close();
            printInfo("Brain structure visualization data written to: " + outputFile);
        }
        
        return true;
        
    } catch (const std::exception& e) {
        printError("Failed to visualize brain structure: " + std::string(e.what()));
        return false;
    }
}

} // namespace nlm
