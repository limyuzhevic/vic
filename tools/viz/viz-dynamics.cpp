// Dynamics visualization tool implementation
//
// This file provides the implementation of dynamics visualization CLI tool

#include "viz-dynamics.hpp"
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>

namespace nlm {

ExecutionResult VizDynamicsTool::execute(const std::vector<std::string>& args, Config& config) {
    std::string simulationFile = getStringArg(args, {"--simulation", "-s"}, "");
    std::string outputFile = getStringArg(args, {"--output", "-o"}, "");
    std::string variable = getStringArg(args, {"--variable", "-v"}, "membrane_potential");
    std::string neuronType = getStringArg(args, {"--neuron-type", "--type"}, "all");
    std::string format = getStringArg(args, {"--format", "-f"}, "png");
    double startTime = getDoubleArg(args, {"--start-time", "--start"}, 0.0);
    double endTime = getDoubleArg(args, {"--end-time", "--end"}, -1.0);
    
    // Validate arguments
    std::vector<std::string> requiredFlags = {"--simulation"};
    std::map<std::string, std::string> optionalFlags = {
        {"--output", ""},
        {"--variable", "membrane_potential"},
        {"--neuron-type", "all"},
        {"--format", "png"},
        {"--start-time", "0.0"},
        {"--end-time", "-1.0"}
    };
    
    validateArgs(args, requiredFlags, optionalFlags);
    
    if (endTime < 0 || (endTime > 0 && endTime <= startTime)) {
        printError("Invalid time range. --end must be > --start and > 0");
        return {false, "Invalid time range"};
    }
    
    printInfo("Visualizing dynamics from simulation: " + simulationFile);
    printInfo("Variable: " + variable);
    printInfo("Neuron type: " + neuronType);
    if (!outputFile.empty()) {
        printInfo("Output file: " + outputFile + " with format: " + format);
    }
    if (endTime > 0) {
        printInfo("Time range: " + std::to_string(startTime) + " to " + std::to_string(endTime) + " ms");
    }
    
    // Create visualization
    bool success = visualizeDynamicsImpl(simulationFile, outputFile, variable, neuronType, format, startTime, endTime);
    
    if (success) {
        printSuccess("Dynamics visualization completed successfully");
        return {true, "Dynamics visualization completed"};
    } else {
        printError("Failed to create dynamics visualization");
        return {false, "Dynamics visualization failed"};
    }
}

bool VizDynamicsTool::visualizeDynamicsImpl(const std::string& simulationFile, const std::string& outputFile,
                                           const std::string& variable, const std::string& neuronType,
                                           const std::string& format, double startTime, double endTime) const {
    if (!fileExists(simulationFile)) {
        printError("Simulation file does not exist: " + simulationFile);
        return false;
    }
    
    try {
        nlohmann::json simulation = parseJsonFromFile(simulationFile);
        
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
        
        out << "Neural Dynamics Visualization" << std::endl;
        out << "===========================" << std::endl;
        out << "Simulation File: " << simulationFile << std::endl;
        out << "Variable: " << variable << std::endl;
        out << "Neuron Type: " << neuronType << std::endl;
        out << std::endl;
        
        // Get time series data
        std::vector<nlohmann::json> timeSeries = simulation["time_series"].get<std::vector<nlohmann::json>>();
        
        if (timeSeries.empty()) {
            printError("No time series data found in simulation file");
            return false;
        }
        
        // Filter time range if specified
        std::vector<nlohmann::json> filteredTimeSeries = timeSeries;
        if (endTime > 0) {
            filteredTimeSeries.clear();
            for (const auto& point : timeSeries) {
                double time = point["time"].get<double>();
                if (time >= startTime && time <= endTime) {
                    filteredTimeSeries.push_back(point);
                }
            }
        }
        
        out << "Time Points: " << filteredTimeSeries.size() << std::endl;
        out << "Time Range: " << (filteredTimeSeries.empty() ? 0.0 : filteredTimeSeries[0]["time"].get<double>()) << " to "
            << (filteredTimeSeries.empty() ? 0.0 : filteredTimeSeries.back()["time"].get<double>()) << " ms" << std::endl;
        out << std::endl;
        
        // Extract data based on variable and neuron type
        if (variable == "membrane_potential") {
            out << "Membrane Potential Time Series:" << std::endl;
            for (size_t i = 0; i < std::min(filteredTimeSeries.size(), size_t(20)); ++i) {
                double time = filteredTimeSeries[i]["time"].get<double>();
                double voltage = filteredTimeSeries[i]["membrane_potential"].get<double>();
                out << std::fixed << std::setprecision(3) << time << " ms: " << voltage << " mV" << std::endl;
            }
            if (filteredTimeSeries.size() > 20) {
                out << "... (" << (filteredTimeSeries.size() - 20) << " more points)" << std::endl;
            }
            
            // Calculate statistics
            double maxVoltage = -1e9, minVoltage = 1e9, sumVoltage = 0.0;
            int validPoints = 0;
            for (const auto& point : filteredTimeSeries) {
                double voltage = point["membrane_potential"].get<double>();
                maxVoltage = std::max(maxVoltage, voltage);
                minVoltage = std::min(minVoltage, voltage);
                sumVoltage += voltage;
                validPoints++;
            }
            
            if (validPoints > 0) {
                out << std::endl << "Statistics:" << std::endl;
                out << "  Mean: " << std::fixed << std::setprecision(3) << sumVoltage / validPoints << " mV" << std::endl;
                out << "  Max: " << maxVoltage << " mV" << std::endl;
                out << "  Min: " << minVoltage << " mV" << std::endl;
                out << "  Range: " << (maxVoltage - minVoltage) << " mV" << std::endl;
            }
            
        } else if (variable == "spike_count") {
            out << "Spike Count Time Series:" << std::endl;
            for (size_t i = 0; i < std::min(filteredTimeSeries.size(), size_t(20)); ++i) {
                double time = filteredTimeSeries[i]["time"].get<double>();
                int spikes = filteredTimeSeries[i]["spike_count"].get<int>();
                out << std::fixed << std::setprecision(3) << time << " ms: " << spikes << " spikes" << std::endl;
            }
            if (filteredTimeSeries.size() > 20) {
                out << "... (" << (filteredTimeSeries.size() - 20) << " more points)" << std::endl;
            }
            
        } else if (variable == "population_activity") {
            out << "Population Activity Time Series:" << std::endl;
            for (size_t i = 0; i < std::min(filteredTimeSeries.size(), size_t(20)); ++i) {
                double time = filteredTimeSeries[i]["time"].get<double>();
                double activity = filteredTimeSeries[i]["population_activity"].get<double>();
                out << std::fixed << std::setprecision(3) << time << " ms: " << activity << std::endl;
            }
            if (filteredTimeSeries.size() > 20) {
                out << "... (" << (filteredTimeSeries.size() - 20) << " more points)" << std::endl;
            }
        }
        
        if (out.is_open() && &out != &std::cout) {
            out.close();
            printInfo("Dynamics visualization data written to: " + outputFile);
        }
        
        return true;
        
    } catch (const std::exception& e) {
        printError("Failed to visualize dynamics: " + std::string(e.what()));
        return false;
    }
}

} // namespace nlm
