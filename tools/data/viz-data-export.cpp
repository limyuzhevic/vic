// Data export for visualization tool implementation
//
// This file provides the implementation of data export for visualization CLI tool

#include "viz-data-export.hpp"
#include <fstream>
#include <sstream>
#include <cmath>

namespace nlm {

ExecutionResult VizDataExportTool::execute(const std::vector<std::string>& args, Config& config) {
    std::string simulationFile = getStringArg(args, {"--simulation", "-s"}, "");
    std::string outputFile = getStringArg(args, {"--output", "-o"}, "");
    std::string format = getStringArg(args, {"--format", "-f"}, "csv");
    
    // Additional options
    bool includeMetadata = hasArg(args, {"--include-metadata", "--metadata"});
    bool compress = hasArg(args, {"--compress", "--compress-data"});
    double samplingRate = getDoubleArg(args, {"--sampling-rate", "--sample"}, 1.0);
    
    // Validate arguments
    std::vector<std::string> requiredFlags = {"--simulation", "--output"};
    std::map<std::string, std::string> optionalFlags = {
        {"--format", "csv"},
        {"--include-metadata", "false"},
        {"--compress", "false"},
        {"--sampling-rate", "1.0"}
    };
    
    validateArgs(args, requiredFlags, optionalFlags);
    
    printInfo("Exporting data from simulation: " + simulationFile);
    printInfo("Output file: " + outputFile);
    printInfo("Format: " + format);
    printInfo("Include metadata: " + std::string(includeMetadata ? "true" : "false"));
    printInfo("Compress data: " + std::string(compress ? "true" : "false"));
    printInfo("Sampling rate: " + std::to_string(samplingRate));
    
    // Create visualization
    bool success = exportDataImpl(simulationFile, outputFile, format,
                                {{ "include_metadata", includeMetadata ? "true" : "false" },
                                 { "compress", compress ? "true" : "false" },
                                 { "sampling_rate", std::to_string(samplingRate) }});
    
    if (success) {
        printSuccess("Data export completed successfully");
        return {true, "Data export completed"};
    } else {
        printError("Failed to export data");
        return {false, "Data export failed"};
    }
}

bool VizDataExportTool::exportDataImpl(const std::string& simulationFile, const std::string& outputFile,
                                       const std::string& format, const std::map<std::string, std::string>& options) const {
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
        
        if (format == "csv") {
            // Export as CSV
            out << "time,membrane_potential,spike_count,population_activity" << std::endl;
            
            std::vector<nlohmann::json> timeSeries = simulation["time_series"].get<std::vector<nlohmann::json>>();
            
            for (size_t i = 0; i < timeSeries.size(); ++i) {
                double time = timeSeries[i]["time"].get<double>();
                double voltage = timeSeries[i]["membrane_potential"].get<double>();
                int spikes = timeSeries[i]["spike_count"].get<int>();
                double activity = timeSeries[i]["population_activity"].get<double>();
                
                out << std::fixed << std::setprecision(6) << time << 
                    "," << voltage << 
                    "," << spikes << 
                    "," << activity << std::endl;
            }
            
        } else if (format == "json") {
            // Export as JSON (modified)
            nlohmann::json exportData = simulation;
            exportData["export_timestamp"] = getCurrentTimestamp();
            exportData["export_format"] = "nlm_visualization_export";
            
            // Add metadata if requested
            if (options.find("include_metadata") != options.end() && options.at("include_metadata") == "true") {
                exportData["metadata"]["source"] = simulationFile;
                exportData["metadata"]["created"] = getCurrentTimestamp();
                exportData["metadata"]["version"] = "1.0";
            }
            
            out << exportData.dump(2);
            
        } else if (format == "hdf5") {
            // HDF5 export would go here
            out << "HDF5 export not yet implemented" << std::endl;
            return false;
        }
        
        if (out.is_open() && &out != &std::cout) {
            out.close();
            printInfo("Data exported successfully to: " + outputFile);
        }
        
        return true;
        
    } catch (const std::exception& e) {
        printError("Failed to export data: " + std::string(e.what()));
        return false;
    }
}

} // namespace nlm
