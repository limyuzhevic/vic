#pragma once

#include "CLICommand.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <thread>
#include <map>
#include <vector>

namespace nlm {

class ProfileCommand : public SimpleCLICommand {
public:
    ProfileCommand() : SimpleCLICommand(
        "profile",
        "Performance profiling and analysis",
        "nlm profile <command> [options]",
        "Performance profiling and analysis.\n\n"
        "Available profile commands:\n"
        "  time           Measure execution time\n"
        "  memory          Measure memory usage\n"
        "  cpu             Measure CPU usage\n"
        "  network         Measure network performance\n"
        "  disk            Measure disk I/O\n\n"
        "Options:\n"
        "  --duration <seconds>   Duration of profiling (default: 30)\n"
        "  --interval <seconds>   Sampling interval (default: 1)\n"
        "  --output <file>        Output file for profile data\n"
        "  --verbose             Enable verbose output\n"
        "  --export <format>      Export profile data (text, json, csv)\n\n"
        "Examples:\n"
        "  nlm profile time --duration 60\n"
        "  nlm profile memory --verbose --output profile.txt\n"
        "  nlm profile cpu --interval 0.5"
    ) {
        options = {
            {"duration", "", "Duration of profiling", OptionType::OptionalValue, false, "30"},
            {"interval", "", "Sampling interval", OptionType::OptionalValue, false, "1"},
            {"output", "", "Output file for profile data", OptionType::OptionalValue},
            {"verbose", "", "Enable verbose output", OptionType::Flag},
            {"export", "", "Export profile data format", OptionType::Choice, false, "text", "text|json|csv"}
        };
        
        parameters = {
            {"command", "Profile command to execute", true, "string", ""}
        };
    }
    
    int execute(const std::vector<std::string>& args) override {
        auto& errorHandler = CLIErrorHandler::getInstance();
        errorHandler.clear();
        
        // Get required parameter
        std::string profileCommand = getRequiredParameter(args, 0);
        
        // Get options
        std::string durationStr = getOptionValue(args, "duration");
        std::string intervalStr = getOptionValue(args, "interval");
        std::string outputFile = getOptionValue(args, "output");
        bool verbose = hasOption(args, "verbose");
        std::string exportFormat = getOptionValue(args, "export");
        
        // Parse parameters
        double duration = 30.0; // default
        if (!durationStr.empty()) {
            try {
                duration = std::stod(durationStr);
            } catch (...) {
                errorHandler.addError("Invalid duration value: " + durationStr);
                return errorHandler.exitWithError();
            }
        }
        
        double interval = 1.0; // default
        if (!intervalStr.empty()) {
            try {
                interval = std::stod(intervalStr);
            } catch (...) {
                errorHandler.addError("Invalid interval value: " + intervalStr);
                return errorHandler.exitWithError();
            }
        }
        
        // Execute profile command
        if (profileCommand == "time") {
            profileTime(duration, interval, verbose, outputFile, exportFormat);
        } else if (profileCommand == "memory") {
            profileMemory(duration, interval, verbose, outputFile, exportFormat);
        } else if (profileCommand == "cpu") {
            profileCPU(duration, interval, verbose, outputFile, exportFormat);
        } else if (profileCommand == "network") {
            profileNetwork(duration, interval, verbose, outputFile, exportFormat);
        } else if (profileCommand == "disk") {
            profileDisk(duration, interval, verbose, outputFile, exportFormat);
        } else {
            errorHandler.addError("Unknown profile command: " + profileCommand);
            return errorHandler.exitWithError();
        }
        
        return errorHandler.isSuccess() ? 0 : 1;
    }
    
    bool validate(const std::vector<std::string>& args) const override {
        if (args.empty()) {
            return false;
        }
        
        std::string profileCommand = args[0];
        
        // Validate supported commands
        static const std::vector<std::string> supportedCommands = {
            "time", "memory", "cpu", "network", "disk"
        };
        
        if (std::find(supportedCommands.begin(), supportedCommands.end(), profileCommand) == supportedCommands.end()) {
            return false;
        }
        
        // Validate duration and interval parameters
        for (const auto& arg : args) {
            if (arg.find("--duration") != std::string::npos && arg != "--duration") {
                size_t pos = arg.find(" ");
                if (pos == std::string::npos) {
                    return false;
                }
                try {
                    std::stod(arg.substr(pos + 1));
                } catch (...) {
                    return false;
                }
            } else if (arg.find("--interval") != std::string::npos && arg != "--interval") {
                size_t pos = arg.find(" ");
                if (pos == std::string::npos) {
                    return false;
                }
                try {
                    std::stod(arg.substr(pos + 1));
                } catch (...) {
                    return false;
                }
            }
        }
        
        return true;
    }
    
private:
    void profileTime(double duration, double interval, bool verbose, const std::string& outputFile,
                     const std::string& exportFormat) {
        CLIFormatters::printInfo("Profiling execution time...");
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        std::vector<double> times;
        double totalTime = 0.0;
        
        for (double elapsed = 0.0; elapsed < duration; elapsed += interval) {
            // Measure current time
            auto currentTime = std::chrono::high_resolution_clock::now();
            double currentElapsed = std::chrono::duration<double>(currentTime - startTime).count();
            
            times.push_back(currentElapsed);
            totalTime = currentElapsed;
            
            if (verbose) {
                CLIFormatters::printInfo("  Time: " + CLIFormatters::formatDuration(currentElapsed));
            }
            
            std::this_thread::sleep_for(std::chrono::seconds(static_cast<int>(interval)));
        }
        
        // Calculate statistics
        double minTime = *std::min_element(times.begin(), times.end());
        double maxTime = *std::max_element(times.begin(), times.end());
        double avgTime = totalTime / times.size();
        
        CLIFormatters::printSuccess("Time profiling completed");
        CLIFormatters::printInfo("Duration: " + CLIFormatters::formatDuration(duration));
        CLIFormatters::printInfo("Samples: " + std::to_string(times.size()));
        CLIFormatters::printInfo("Average Time: " + CLIFormatters::formatDuration(avgTime));
        CLIFormatters::printInfo("Min Time: " + CLIFormatters::formatDuration(minTime));
        CLIFormatters::printInfo("Max Time: " + CLIFormatters::formatDuration(maxTime));
        
        if (!outputFile.empty()) {
            saveProfileData(outputFile, exportFormat, "time", "Time", times);
        }
    }
    
    void profileMemory(double duration, double interval, bool verbose, const std::string& outputFile,
                       const std::string& exportFormat) {
        CLIFormatters::printInfo("Profiling memory usage...");
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        std::vector<size_t> memoryUsage;
        double totalMemory = 0.0;
        
        for (double elapsed = 0.0; elapsed < duration; elapsed += interval) {
            // Get memory usage
            size_t currentMemory = getMemoryUsage();
            
            memoryUsage.push_back(currentMemory);
            totalMemory += currentMemory;
            
            if (verbose) {
                CLIFormatters::printInfo("  Memory: " + CLIFormatters::formatMemory(currentMemory));
            }
            
            std::this_thread::sleep_for(std::chrono::seconds(static_cast<int>(interval)));
        }
        
        // Calculate statistics
        size_t minMemory = *std::min_element(memoryUsage.begin(), memoryUsage.end());
        size_t maxMemory = *std::max_element(memoryUsage.begin(), memoryUsage.end());
        double avgMemory = totalMemory / memoryUsage.size();
        
        CLIFormatters::printSuccess("Memory profiling completed");
        CLIFormatters::printInfo("Duration: " + CLIFormatters::formatDuration(duration));
        CLIFormatters::printInfo("Samples: " + std::to_string(memoryUsage.size()));
        CLIFormatters::printInfo("Average Memory: " + CLIFormatters::formatMemory(static_cast<size_t>(avgMemory)));
        CLIFormatters::printInfo("Min Memory: " + CLIFormatters::formatMemory(minMemory));
        CLIFormatters::printInfo("Max Memory: " + CLIFormatters::formatMemory(maxMemory));
        
        if (!outputFile.empty()) {
            saveProfileData(outputFile, exportFormat, "memory", "Memory", memoryUsage);
        }
    }
    
    void profileCPU(double duration, double interval, bool verbose, const std::string& outputFile,
                    const std::string& exportFormat) {
        CLIFormatters::printInfo("Profiling CPU usage...");
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        std::vector<double> cpuUsage;
        double totalCPU = 0.0;
        
        for (double elapsed = 0.0; elapsed < duration; elapsed += interval) {
            // Get CPU usage
            double currentCPU = getCPUUsage();
            
            cpuUsage.push_back(currentCPU);
            totalCPU += currentCPU;
            
            if (verbose) {
                CLIFormatters::printInfo("  CPU Usage: " + std::to_string(currentCPU) + "%");
            }
            
            std::this_thread::sleep_for(std::chrono::seconds(static_cast<int>(interval)));
        }
        
        // Calculate statistics
        double minCPU = *std::min_element(cpuUsage.begin(), cpuUsage.end());
        double maxCPU = *std::max_element(cpuUsage.begin(), cpuUsage.end());
        double avgCPU = totalCPU / cpuUsage.size();
        
        CLIFormatters::printSuccess("CPU profiling completed");
        CLIFormatters::printInfo("Duration: " + CLIFormatters::formatDuration(duration));
        CLIFormatters::printInfo("Samples: " + std::to_string(cpuUsage.size()));
        CLIFormatters::printInfo("Average CPU Usage: " + std::to_string(avgCPU) + "%");
        CLIFormatters::printInfo("Min CPU Usage: " + std::to_string(minCPU) + "%");
        CLIFormatters::printInfo("Max CPU Usage: " + std::to_string(maxCPU) + "%");
        
        if (!outputFile.empty()) {
            saveProfileData(outputFile, exportFormat, "cpu", "CPU", cpuUsage);
        }
    }
    
    void profileNetwork(double duration, double interval, bool verbose, const std::string& outputFile,
                        const std::string& exportFormat) {
        CLIFormatters::printInfo("Profiling network performance...");
        
        // Placeholder for network profiling
        CLIFormatters::printWarning("Network profiling not yet implemented");
        
        if (!outputFile.empty()) {
            // Create empty output file
            std::ofstream file(outputFile, std::ios::out);
            file << "Network profiling data (placeholder)" << std::endl;
            file.close();
        }
    }
    
    void profileDisk(double duration, double interval, bool verbose, const std::string& outputFile,
                     const std::string& exportFormat) {
        CLIFormatters::printInfo("Profiling disk I/O...");
        
        // Placeholder for disk profiling
        CLIFormatters::printWarning("Disk profiling not yet implemented");
        
        if (!outputFile.empty()) {
            // Create empty output file
            std::ofstream file(outputFile, std::ios::out);
            file << "Disk profiling data (placeholder)" << std::endl;
            file.close();
        }
    }
    
    void saveProfileData(const std::string& filename, const std::string& format,
                         const std::string& profileType, const std::string& metricName,
                         const std::vector<std::any>& data) {
        std::ofstream file(filename, std::ios::out);
        if (!file.is_open()) {
            CLIFormatters::printError("Cannot write to file: " + filename);
            return;
        }
        
        if (format == "json") {
            file << "{\n";
            file << "  \"profileType\": \"" << profileType << "\",\n";
            file << "  \"metricName\": \"" << metricName << "\",\n";
            file << "  \"timestamp\": \"" << CLIUtils::getCurrentTimestamp() << "\",\n";
            file << "  \"data\": [";
            
            for (size_t i = 0; i < data.size(); ++i) {
                if (i > 0) file << ", ";
                if (std::is_same<std::vector<std::any>::value_type, double>::value) {
                    file << std::any_cast<double>(data[i]);
                } else if (std::is_same<std::vector<std::any>::value_type, size_t>::value) {
                    file << std::any_cast<size_t>(data[i]);
                }
            }
            
            file << "]\n";
            file << "}\n";
        } else if (format == "csv") {
            file << "Timestamp," << metricName << "\n";
            
            auto startTime = std::chrono::high_resolution_clock::now();
            
            for (size_t i = 0; i < data.size(); ++i) {
                double timestamp = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - startTime).count();
                if (std::is_same<std::vector<std::any>::value_type, double>::value) {
                    file << timestamp << "," << std::any_cast<double>(data[i]) << "\n";
                } else if (std::is_same<std::vector<std::any>::value_type, size_t>::value) {
                    file << timestamp << "," << std::any_cast<size_t>(data[i]) << "\n";
                }
            }
        } else {
            // Text format
            file << profileType << " Profile" << std::endl;
            file << "===================" << std::endl << std::endl;
            file << "Profile Type: " << profileType << std::endl;
            file << "Metric Name: " << metricName << std::endl;
            file << "Timestamp: " << CLIUtils::getCurrentTimestamp() << std::endl;
            file << "Data Points: " << data.size() << std::endl;
            file << std::endl;
            
            for (size_t i = 0; i < data.size(); ++i) {
                if (std::is_same<std::vector<std::any>::value_type, double>::value) {
                    file << i << ": " << std::any_cast<double>(data[i]) << std::endl;
                } else if (std::is_same<std::vector<std::any>::value_type, size_t>::value) {
                    file << i << ": " << std::any_cast<size_t>(data[i]) << std::endl;
                }
            }
        }
        
        file.close();
        CLIFormatters::printSuccess("Profile data saved to: " + filename);
    }
    
    size_t getMemoryUsage() {
        // Get memory usage (simplified implementation)
        // This should be replaced with platform-specific implementation
        return 0;
    }
    
    double getCPUUsage() {
        // Get CPU usage (simplified implementation)
        // This should be replaced with platform-specific implementation
        return 0.0;
    }
};

} // namespace nlm
