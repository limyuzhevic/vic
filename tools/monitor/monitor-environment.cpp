// Environment monitoring tool implementation
//
// This file provides the implementation of environment monitoring CLI tool

#include "monitor-environment.hpp"
#include <fstream>
#include <sstream>

namespace nlm {

ExecutionResult MonitorEnvironmentTool::execute(const std::vector<std::string>& args, Config& config) {
    std::string outputFile = getStringArg(args, {"--output", "-o"}, "");
    double interval = getDoubleArg(args, {"--interval", "--interval"}, 1.0);
    std::string envType = getStringArg(args, {"--env-type", "--type"}, "normal");
    double duration = getDoubleArg(args, {"--duration", "--duration"}, -1.0);
    
    // Validate arguments
    std::vector<std::string> requiredFlags = {"--output"};
    std::map<std::string, std::string> optionalFlags = {
        {"--interval", "1.0"},
        {"--env-type", "normal"},
        {"--duration", "-1.0"}
    };
    
    validateArgs(args, requiredFlags, optionalFlags);
    
    printInfo("Monitoring environment performance to: " + outputFile);
    printInfo("Environment type: " + envType);
    printInfo("Monitoring interval: " + std::to_string(interval) + " seconds");
    if (duration > 0.0) {
        printInfo("Duration: " + std::to_string(duration) + " seconds");
    }
    
    // Create visualization
    bool success = monitorEnvironmentImpl(outputFile, interval, envType, duration);
    
    if (success) {
        printSuccess("Environment monitoring completed successfully");
        return {true, "Environment monitoring completed"};
    } else {
        printError("Failed to complete environment monitoring");
        return {false, "Environment monitoring failed"};
    }
}

bool MonitorEnvironmentTool::monitorEnvironmentImpl(const std::string& outputFile, double interval, const std::string& envType, double duration) const {
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
    
    out << "Environment Performance Monitoring" << std::endl;
    out << "================================" << std::endl;
    out << "Monitoring started at: " << getCurrentTimestamp() << std::endl;
    out << "Environment Type: " << envType << std::endl;
    out << "Monitoring Interval: " << interval << " seconds" << std::endl;
    if (duration > 0.0) {
        out << "Duration: " << duration << " seconds" << std::endl;
        out << "End time: " << getCurrentTimestamp() << std::endl;
    }
    out << std::endl;
    
    // Simulate environment monitoring
    for (int i = 0; i < (duration > 0.0 ? static_cast<int>(duration / interval) : 5); ++i) {
        double temperature = 20.0 + ((static_cast<double>(rand()) / RAND_MAX) * 20.0 - 10.0);
        double humidity = ((static_cast<double>(rand()) / RAND_MAX) * 100.0);
        double pressure = 1013.0 + ((static_cast<double>(rand()) / RAND_MAX) * 50.0 - 25.0);
        double windSpeed = ((static_cast<double>(rand()) / RAND_MAX) * 30.0);
        double radiation = ((static_cast<double>(rand()) / RAND_MAX) * 10.0);
        
        out << "Sample " << (i + 1) << ": " << getCurrentTimestamp() << std::endl;
        out << "  Temperature: " << std::fixed << std::setprecision(2) << temperature << " °C" << std::endl;
        out << "  Humidity: " << std::fixed << std::setprecision(2) << humidity << " %" << std::endl;
        out << "  Pressure: " << std::fixed << std::setprecision(2) << pressure << " hPa" << std::endl;
        out << "  Wind Speed: " << std::fixed << std::setprecision(2) << windSpeed << " m/s" << std::endl;
        out << "  Radiation: " << std::fixed << std::setprecision(2) << radiation << " mSv" << std::endl;
        out << std::endl;
        
        if (duration > 0.0) {
            std::this_thread::sleep_for(std::chrono::duration<double>(interval));
        }
    }
    
    out << "Environment monitoring completed at: " << getCurrentTimestamp() << std::endl;
    out << "Environment type analysis: " << envType << std::endl;
    
    if (out.is_open() && &out != &std::cout) {
        out.close();
        printInfo("Environment monitoring data written to: " + outputFile);
    }
    
    return true;
}

} // namespace nlm
