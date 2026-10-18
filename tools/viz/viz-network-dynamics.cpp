// Network dynamics visualization tool implementation
//
// This file provides the implementation of network dynamics visualization CLI tool

#include "viz-network-dynamics.hpp"
#include <fstream>
#include <sstream>
#include <cmath>

namespace nlm {

ExecutionResult VizNetworkDynamicsTool::execute(const std::vector<std::string>& args, Config& config) {
    std::string simulationFile = getStringArg(args, {"--simulation", "-s"}, "");
    std::string outputFile = getStringArg(args, {"--output", "-o"}, "");
    std::string frequency = getStringArg(args, {"--frequency", "-f"}, "");
    std::string mode = getStringArg(args, {"--mode", "-m"}, "oscillation");
    std::string format = getStringArg(args, {"--format", "-t"}, "png");
    
    // Validate arguments
    std::vector<std::string> requiredFlags = {"--simulation"};
    std::map<std::string, std::string> optionalFlags = {
        {"--output", ""},
        {"--frequency", ""},
        {"--mode", "oscillation"},
        {"--format", "png"}
    };
    
    validateArgs(args, requiredFlags, optionalFlags);
    
    printInfo("Visualizing network dynamics from simulation: " + simulationFile);
    printInfo("Analysis mode: " + mode);
    if (!frequency.empty()) {
        printInfo("Frequency range: " + frequency);
    }
    if (!outputFile.empty()) {
        printInfo("Output file: " + outputFile + " with format: " + format);
    }
    
    // Create visualization
    bool success = visualizeNetworkDynamicsImpl(simulationFile, outputFile, frequency, mode, format);
    
    if (success) {
        printSuccess("Network dynamics visualization completed successfully");
        return {true, "Network dynamics visualization completed"};
    } else {
        printError("Failed to create network dynamics visualization");
        return {false, "Network dynamics visualization failed"};
    }
}

bool VizNetworkDynamicsTool::visualizeNetworkDynamicsImpl(const std::string& simulationFile, const std::string& outputFile,
                                                         const std::string& frequency, const std::string& mode,
                                                         const std::string& format) const {
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
        
        out << "Network Dynamics Visualization" << std::endl;
        out << "=============================" << std::endl;
        out << "Simulation File: " << simulationFile << std::endl;
        out << "Analysis Mode: " << mode << std::endl;
        if (!frequency.empty()) {
            out << "Frequency Range: " << frequency << std::endl;
        }
        out << std::endl;
        
        if (mode == "oscillation") {
            out << "Oscillation Analysis:" << std::endl;
            
            std::vector<nlohmann::json> oscillators = simulation["oscillators"].get<std::vector<nlohmann::json>>();
            out << "Number of Oscillators: " << oscillators.size() << std::endl;
            
            double totalFrequency = 0.0;
            for (const auto& osc : oscillators) {
                totalFrequency += osc["frequency"].get<double>();
            }
            
            if (!oscillators.empty()) {
                out << "Average Frequency: " << std::fixed << std::setprecision(3)
                    << totalFrequency / oscillators.size() << " Hz" << std::endl;
            }
            
        } else if (mode == "propagation") {
            out << "Wave Propagation Analysis:" << std::endl;
            
            std::vector<nlohmann::json> waves = simulation["waves"].get<std::vector<nlohmann::json>>();
            out << "Number of Waves: " << waves.size() << std::endl;
            
            for (size_t i = 0; i < std::min(waves.size(), size_t(5)); ++i) {
                out << "  Wave " << i << ": " << waves[i]["type"] << std::endl;
                out << "    Speed: " << waves[i]["speed"] << " m/s" << std::endl;
                out << "    Amplitude: " << waves[i]["amplitude"] << std::endl;
            }
            
        } else if (mode == "diffusion") {
            out << "Diffusion Analysis:" << std::endl;
            
            double diffusionCoefficient = simulation["diffusion_coefficient"].get<double>();
            out << "Diffusion Coefficient: " << diffusionCoefficient << std::endl;
            
            // Calculate characteristic length
            double characteristicLength = std::sqrt(diffusionCoefficient * 100.0); // Simplified
            out << "Characteristic Length: " << characteristicLength << " mm" << std::endl;
        }
        
        if (out.is_open() && &out != &std::cout) {
            out.close();
            printInfo("Network dynamics visualization data written to: " + outputFile);
        }
        
        return true;
        
    } catch (const std::exception& e) {
        printError("Failed to visualize network dynamics: " + std::string(e.what()));
        return false;
    }
}

} // namespace nlm
