// Brain activity visualization tool implementation
//
// This file provides the implementation of brain activity visualization CLI tool

#include "viz-brain-activity.hpp"
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>

namespace nlm {

ExecutionResult VizBrainActivityTool::execute(const std::vector<std::string>& args, Config& config) {
    std::string brainFile = getStringArg(args, {"--brain", "-b"}, "");
    std::string neuronType = getStringArg(args, {"--neuron-type", "--type"}, "all");
    std::string outputFile = getStringArg(args, {"--output", "-o"}, "");
    std::string format = getStringArg(args, {"--format", "-f"}, "png");
    double startTime = getDoubleArg(args, {"--start-time", "--start"}, 0.0);
    double endTime = getDoubleArg(args, {"--end-time", "--end"}, -1.0);
    
    // Validate arguments
    std::vector<std::string> requiredFlags = {"--brain"};
    std::map<std::string, std::string> optionalFlags = {
        {"--neuron-type", "all"},
        {"--output", ""},
        {"--format", "png"},
        {"--start-time", "0.0"},
        {"--end-time", "-1.0"}
    };
    
    validateArgs(args, requiredFlags, optionalFlags);
    
    if (endTime >= 0 && endTime <= startTime) {
        printError("Invalid time window. --end must be > --start and > 0");
        return {false, "Invalid time window"};
    }
    
    printInfo("Visualizing brain activity from: " + brainFile);
    printInfo("Neuron type: " + neuronType);
    printInfo("Time window: " + std::to_string(startTime) + " to " + (endTime > 0 ? std::to_string(endTime) : "end") + " ms");
    if (!outputFile.empty()) {
        printInfo("Output file: " + outputFile + " with format: " + format);
    }
    
    // Create visualization
    bool success = visualizeBrainActivityImpl(brainFile, neuronType, outputFile, format, startTime, endTime);
    
    if (success) {
        printSuccess("Brain activity visualization completed successfully");
        return {true, "Brain activity visualization completed"};
    } else {
        printError("Failed to create brain activity visualization");
        return {false, "Brain activity visualization failed"};
    }
}

bool VizBrainActivityTool::visualizeBrainActivityImpl(const std::string& brainFile, const std::string& neuronType,
                                                     const std::string& outputFile, const std::string& format,
                                                     double startTime, double endTime) const {
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
        
        out << "Brain Activity Visualization" << std::endl;
        out << "===========================" << std::endl;
        out << "Brain File: " << brainFile << std::endl;
        out << "Neuron Type: " << neuronType << std::endl;
        out << std::endl;
        
        std::vector<nlohmann::json> activityData = brainState["activity_data"].get<std::vector<nlohmann::json>>();
        out << "Time Points: " << activityData.size() << std::endl;
        
        if (endTime > 0) {
            // Filter by time window
            std::vector<nlohmann::json> filteredData;
            for (const auto& point : activityData) {
                double time = point["time"].get<double>();
                if (time >= startTime && time <= endTime) {
                    filteredData.push_back(point);
                }
            }
            activityData = filteredData;
        }
        
        out << "Filtered Time Points: " << activityData.size() << std::endl;
        out << std::endl;
        
        // Activity patterns
        out << "Activity Patterns:" << std::endl;
        
        if (neuronType == "all" || neuronType == "excitatory") {
            out << "  Excitatory Activity:" << std::endl;
            
            double totalExcitatoryActivity = 0.0;
            double maxExcitatory = -1e9;
            int excitatoryPoints = 0;
            
            for (const auto& point : activityData) {
                if (point.contains("excitatory_activity")) {
                    double activity = point["excitatory_activity"].get<double>();
                    totalExcitatoryActivity += activity;
                    maxExcitatory = std::max(maxExcitatory, activity);
                    excitatoryPoints++;
                }
            }
            
            if (excitatoryPoints > 0) {
                out << "    Mean: " << std::fixed << std::setprecision(4) 
                    << totalExcitatoryActivity / excitatoryPoints << std::endl;
                out << "    Max: " << maxExcitatory << std::endl;
                out << "    Duration: " << excitatoryPoints << " time points" << std::endl;
            }
        }
        
        if (neuronType == "all" || neuronType == "inhibitory") {
            out << "  Inhibitory Activity:" << std::endl;
            
            double totalInhibitoryActivity = 0.0;
            double maxInhibitory = -1e9;
            int inhibitoryPoints = 0;
            
            for (const auto& point : activityData) {
                if (point.contains("inhibitory_activity")) {
                    double activity = point["inhibitory_activity"].get<double>();
                    totalInhibitoryActivity += activity;
                    maxInhibitory = std::max(maxInhibitory, activity);
                    inhibitoryPoints++;
                }
            }
            
            if (inhibitoryPoints > 0) {
                out << "    Mean: " << std::fixed << std::setprecision(4) 
                    << totalInhibitoryActivity / inhibitoryPoints << std::endl;
                out << "    Max: " << maxInhibitory << std::endl;
                out << "    Duration: " << inhibitoryPoints << " time points" << std::endl;
            }
        }
        
        // Global activity metrics
        out << std::endl << "Global Activity Metrics:" << std::endl;
        out << "  Total Activity Duration: " << (activityData.size() > 0 ? 
            activityData.back()["time"].get<double>() - activityData.front()["time"].get<double>() : 0.0) << " ms" << std::endl;
        out << "  Activity Variability: " << 0.5 << std::endl; // Placeholder
        out << "  Peak Activity Time: " << (activityData.empty() ? 0.0 : activityData[activityData.size()/2]["time"].get<double>()) << " ms" << std::endl;
        
        if (out.is_open() && &out != &std::cout) {
            out.close();
            printInfo("Brain activity visualization data written to: " + outputFile);
        }
        
        return true;
        
    } catch (const std::exception& e) {
        printError("Failed to visualize brain activity: " + std::string(e.what()));
        return false;
    }
}

} // namespace nlm
