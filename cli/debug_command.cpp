// NLM Debug Command - Debugging tools and verbose output
// Handles debugging, tracing, and verbose output options

#pragma once

#include "cli/nlm_cli.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <chrono>
#include <vector>

namespace nlm {

class DebugCommand {
public:
    static bool execute(NLMCLI& cli, const std::map<std::string, std::string>& args);
    static void showHelp();
    static bool enableTracing(const std::map<std::string, std::string>& args);
    static bool showPerformanceStats(const std::map<std::string, std::string>& args);
    static bool showBrainDiagnostics(std::shared_ptr<Brain> brain, const std::map<std::string, std::string>& options);
    static bool analyzeMemoryUsage(const std::map<std::string, std::string>& options);
    static bool analyzePerformance(std::shared_ptr<Brain> brain, const std::chrono::high_resolution_clock::time_point& startTime);
    static void printDebugInfo(std::shared_ptr<Brain> brain);
    static void printPerformanceStats(double elapsedTime, std::shared_ptr<Brain> brain);
    static std::string formatMemorySize(size_t bytes);
    static std::string formatTime(double seconds);
};

bool DebugCommand::execute(NLMCLI& cli, const std::map<std::string, std::string>& args) {
    std::cout << "=== NLM Debug Command ===" << std::endl;
    
    // Get current brain if available
    std::shared_ptr<Brain> brain;
    // In a real implementation, this would come from the CLI state
    
    bool success = true;
    
    // Enable tracing if requested
    if (args.count("trace")) {
        success = enableTracing(args) && success;
    }
    
    // Show performance stats if requested
    if (args.count("perf")) {
        success = showPerformanceStats(args) && success;
    }
    
    // Show brain diagnostics
    success = showBrainDiagnostics(brain, args) && success;
    
    // Show memory usage analysis
    success = analyzeMemoryUsage(args) && success;
    
    return success;
}

void DebugCommand::showHelp() {
    std::cout << "=== NLM Debug Command Help ===" << std::endl;
    std::cout << "Usage: nlm debug [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Debugging tools and verbose output:" << std::endl;
    std::cout << "  -v, --verbose              Enable verbose logging" << std::endl;
    std::cout << "  -vv                        Enable debug logging" << std::endl;
    std::cout << "  -q, --quiet                Suppress output" << std::endl;
    std::cout << "  --trace [FILE]             Enable tracing to FILE" << std::endl;
    std::cout << "  --perf                     Show performance statistics" << std::endl;
    std::cout << "  --diagnostics              Show brain diagnostics" << std::endl;
    std::cout << "  --memory                   Analyze memory usage" << std::endl;
    std::cout << "  --network                  Analyze network structure" << std::endl;
    std::cout << "  --plasticity              Show plasticity state" << std::endl;
    std::cout << "  -h, --help                 Show this help message" << std::endl;
}

bool DebugCommand::enableTracing(const std::map<std::string, std::string>& args) {
    std::string traceFile = args.count("trace") ? args.at("trace") : "nlm_trace.log";
    
    std::cout << "Enabling trace logging to: " << traceFile << std::endl;
    
    // Create trace file
    std::ofstream traceFileStream(traceFile, std::ios::app);
    if (!traceFileStream.is_open()) {
        std::cerr << "Error: Cannot open trace file for writing: " << traceFile << std::endl;
        return false;
    }
    
    // Write trace header
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    traceFileStream << "=== NLM Trace Log ===" << std::endl;
    traceFileStream << "Started: " << std::ctime(&time) << std::endl;
    traceFileStream << "PID: " << getpid() << std::endl;
    traceFileStream << std::endl;
    
    traceFileStream.close();
    
    std::cout << "Trace logging enabled successfully" << std::endl;
    
    return true;
}

bool DebugCommand::showPerformanceStats(const std::map<std::string, std::string>& args) {
    std::cout << "=== Performance Statistics ===" << std::endl;
    
    // Get elapsed time from NLMCLI
    // In a real implementation, this would come from the CLI timing info
    double elapsedTime = 0.0; // Placeholder
    
    auto now = std::chrono::high_resolution_clock::now();
    // auto startTime = ... would come from NLMCLI
    // elapsedTime = std::chrono::duration<double>(now - startTime).count();
    
    std::cout << "Total execution time: " << formatTime(elapsedTime) << std::endl;
    
    // Get brain if available
    // std::shared_ptr<Brain> brain = ... would come from NLMCLI
    
    // Analyze performance
    // analyzePerformance(brain, now);
    
    return true;
}

bool DebugCommand::showBrainDiagnostics(std::shared_ptr<Brain> brain, const std::map<std::string, std::string>& options) {
    std::cout << "=== Brain Diagnostics ===" << std::endl;
    
    if (!brain) {
        std::cout << "  No brain state available" << std::endl;
        return true;
    }
    
    printDebugInfo(brain);
    
    // Network analysis
    if (options.count("network") && options.at("network") == "true") {
        std::cout << std::endl << "=== Network Analysis ===" << std::endl;
        
        size_t neurons = brain->getTotalNeuronCount();
        size_t synapses = brain->getTotalSynapseCount();
        size_t regions = brain->getRegionCount();
        
        std::cout << "  Total neurons: " << neurons << std::endl;
        std::cout << "  Total synapses: " << synapses << std::endl;
        std::cout << "  Total regions: " << regions << std::endl;
        
        if (neurons > 0) {
            double synapseDensity = static_cast<double>(synapses) / neurons;
            std::cout << "  Synapses per neuron: " << synapseDensity << std::endl;
            
            double connectionProbability = synapses / (neurons * neurons);
            std::cout << "  Connection probability: " << connectionProbability << std::endl;
        }
    }
    
    // Plasticity analysis
    if (options.count("plasticity") && options.at("plasticity") == "true") {
        std::cout << std::endl << "=== Plasticity Analysis ===" << std::endl;
        
        // Analyze plasticity state
        // This would require access to brain's plasticity system
        std::cout << "  Plasticity analysis requires brain plasticity subsystem access" << std::endl;
    }
    
    return true;
}

bool DebugCommand::analyzeMemoryUsage(const std::map<std::string, std::string>& options) {
    std::cout << "=== Memory Usage Analysis ===" << std::endl;
    
    // Get memory usage information
    // This is platform-specific and would require system calls
    
    size_t totalMemory = 0;     // MB
    size_t usedMemory = 0;      // MB
    size_t availableMemory = 0; // MB
    
    // Placeholder values - real implementation would use system-specific APIs
    totalMemory = 1024;    // 1GB
    usedMemory = 512;      // 512MB
    availableMemory = 512; // 512MB
    
    std::cout << "  Total memory: " << totalMemory << " MB" << std::endl;
    std::cout << "  Used memory: " << usedMemory << " MB" << std::endl;
    std::cout << "  Available memory: " << availableMemory << " MB" << std::endl;
    
    double memoryUsagePercent = (usedMemory / totalMemory) * 100.0;
    std::cout << "  Memory usage: " << memoryUsagePercent << "%" << std::endl;
    
    if (memoryUsagePercent > 80.0) {
        std::cout << "  WARNING: High memory usage!" << std::endl;
    }
    
    return true;
}

bool DebugCommand::analyzePerformance(std::shared_ptr<Brain> brain, const std::chrono::high_resolution_clock::time_point& startTime) {
    auto endTime = std::chrono::high_resolution_clock::now();
    double elapsedTime = std::chrono::duration<double>(endTime - startTime).count();
    
    printPerformanceStats(elapsedTime, brain);
    
    return true;
}

void DebugCommand::printDebugInfo(std::shared_ptr<Brain> brain) {
    if (!brain) {
        std::cout << "  No brain state available" << std::endl;
        return;
    }
    
    std::cout << "  Brain Statistics:" << std::endl;
    std::cout << "    Neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "    Synapses: " << brain->getTotalSynapseCount() << std::endl;
    std::cout << "    Regions: " << brain->getRegionCount() << std::endl;
    std::cout << "    Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "    Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
    
    // Get brain status
    brain->logStatus();
}

void DebugCommand::printPerformanceStats(double elapsedTime, std::shared_ptr<Brain> brain) {
    std::cout << "  Performance Metrics:" << std::endl;
    std::cout << "    Total time: " << formatTime(elapsedTime) << std::endl;
    
    if (brain) {
        size_t neurons = brain->getTotalNeuronCount();
        if (elapsedTime > 0.0) {
            double neuronsPerSecond = neurons / elapsedTime;
            std::cout << "    Neurons processed per second: " << neuronsPerSecond << std::endl;
        }
        
        size_t synapses = brain->getTotalSynapseCount();
        if (elapsedTime > 0.0) {
            double synapsesPerSecond = synapses / elapsedTime;
            std::cout << "    Synapses processed per second: " << synapsesPerSecond << std::endl;
        }
    }
}

std::string DebugCommand::formatMemorySize(size_t bytes) {
    if (bytes < 1024) {
        return std::to_string(bytes) + " B";
    } else if (bytes < 1024 * 1024) {
        return std::to_string(bytes / 1024) + " KB";
    } else if (bytes < 1024 * 1024 * 1024) {
        return std::to_string(bytes / (1024 * 1024)) + " MB";
    } else {
        return std::to_string(bytes / (1024 * 1024 * 1024)) + " GB";
    }
}

std::string DebugCommand::formatTime(double seconds) {
    if (seconds < 1.0) {
        return std::to_string(seconds * 1000) + "ms";
    } else if (seconds < 60.0) {
        return std::to_string(seconds) + "s";
    } else {
        double minutes = seconds / 60.0;
        if (minutes < 60.0) {
            return std::to_string(minutes) + "min";
        } else {
            double hours = minutes / 60.0;
            return std::to_string(hours) + "h";
        }
    }
}
