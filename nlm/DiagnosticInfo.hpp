/*
 * nlm/DiagnosticInfo.hpp - Detailed error context and diagnostic information for the NLM system
 * Comprehensive diagnostic information for debugging and error analysis
 */

#pragma once

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <memory>
#include <sstream>
#include <iomanip>

namespace nlm {

// System information structure
struct SystemInfo {
    std::string applicationName;
    std::string applicationVersion;
    std::string buildNumber;
    std::string operatingSystem;
    std::string cpuArchitecture;
    size_t totalMemory;
    size_t availableMemory;
    std::string compiler;
    std::string compilationDate;
    
    SystemInfo() : totalMemory(0), availableMemory(0) {}
    
    std::string getFormattedSystemInfo() const {
        std::ostringstream oss;
        oss << "Application: " << applicationName << " " << applicationVersion << "\n"
            << "Build: " << buildNumber << "\n"
            << "OS: " << operatingSystem << "\n"
            << "Architecture: " << cpuArchitecture << "\n"
            << "Total Memory: " << (totalMemory / (1024 * 1024)) << " MB\n"
            << "Available Memory: " << (availableMemory / (1024 * 1024)) << " MB\n"
            << "Compiler: " << compiler << "\n"
            << "Build Date: " << compilationDate;
        return oss.str();
    }
};

// Stack trace information
struct StackTraceInfo {
    std::vector<std::string> frames;
    std::string errorAddress;
    std::string moduleName;
    
    std::string getFormattedStackTrace() const {
        std::ostringstream oss;
        oss << "Stack Trace:";
        for (size_t i = 0; i < frames.size(); ++i) {
            oss << "\n  " << i << ". " << frames[i];
        }
        if (!errorAddress.empty()) {
            oss << "\n  Error Address: " << errorAddress;
        }
        if (!moduleName.empty()) {
            oss << "\n  Module: " << moduleName;
        }
        return oss.str();
    }
};

// Memory usage information
struct MemoryUsageInfo {
    size_t residentSetSize;
    size_t virtualMemorySize;
    size_t sharedMemorySize;
    size_t textSegmentSize;
    size_t dataSegmentSize;
    size_t stackSegmentSize;
    size_t librarySize;
    size_t pageFaults;
    
    MemoryUsageInfo() : residentSetSize(0), virtualMemorySize(0), sharedMemorySize(0),
                       textSegmentSize(0), dataSegmentSize(0), stackSegmentSize(0),
                       librarySize(0), pageFaults(0) {}
    
    std::string getFormattedMemoryInfo() const {
        std::ostringstream oss;
        oss << "Memory Usage:";
        oss << "\n  Resident Set Size: " << (residentSetSize / 1024) << " KB";
        oss << "\n  Virtual Memory Size: " << (virtualMemorySize / 1024) << " KB";
        oss << "\n  Shared Memory Size: " << (sharedMemorySize / 1024) << " KB";
        oss << "\n  Text Segment: " << (textSegmentSize / 1024) << " KB";
        oss << "\n  Data Segment: " << (dataSegmentSize / 1024) << " KB";
        oss << "\n  Stack Segment: " << (stackSegmentSize / 1024) << " KB";
        oss << "\n  Libraries: " << (librarySize / 1024) << " KB";
        oss << "\n  Page Faults: " << pageFaults;
        return oss.str();
    }
};

// Performance metrics
struct PerformanceMetrics {
    double cpuUsage;
    double memoryUsage;
    double diskUsage;
    double networkUsage;
    uint64_t uptime;
    uint64_t totalProcessorTime;
    uint64_t idleProcessorTime;
    size_t threadCount;
    size_t openFileHandles;
    
    PerformanceMetrics() : cpuUsage(0.0), memoryUsage(0.0), diskUsage(0.0),
                           networkUsage(0.0), uptime(0), totalProcessorTime(0),
                           idleProcessorTime(0), threadCount(0), openFileHandles(0) {}
    
    std::string getFormattedPerformanceInfo() const {
        std::ostringstream oss;
        oss << "Performance Metrics:";
        oss << "\n  CPU Usage: " << std::fixed << std::setprecision(2) << cpuUsage << "%";
        oss << "\n  Memory Usage: " << std::fixed << std::setprecision(2) << memoryUsage << "%";
        oss << "\n  Disk Usage: " << std::fixed << std::setprecision(2) << diskUsage << "%";
        oss << "\n  Network Usage: " << std::fixed << std::setprecision(2) << networkUsage << " MB/s";
        oss << "\n  Uptime: " << uptime << " seconds";
        oss << "\n  Processor Time: " << totalProcessorTime << " ms";
        oss << "\n  Idle Time: " << idleProcessorTime << " ms";
        oss << "\n  Thread Count: " << threadCount;
        oss << "\n  Open File Handles: " << openFileHandles;
        return oss.str();
    }
};

// Diagnostic information
struct DiagnosticInfo {
    std::string diagnosticId;
    std::chrono::system_clock::time_point timestamp;
    std::string sessionId;
    std::string userId;
    std::string machineName;
    std::string processId;
    std::string threadId;
    
    SystemInfo systemInfo;
    MemoryUsageInfo memoryUsage;
    PerformanceMetrics performance;
    std::vector<StackTraceInfo> stackTraces;
    std::map<std::string, std::string> environmentVariables;
    std::map<std::string, std::string> configuration;
    
    DiagnosticInfo() : timestamp(std::chrono::system_clock::now()) {
        // Set default values
        std::stringstream ss;
        ss << std::this_thread::get_id();
        threadId = ss.str();
        
        std::stringstream pid;
        pid << getpid();
        processId = pid.str();
        
        char hostname[256];
        hostname[0] = '\0';
        if (gethostname(hostname, sizeof(hostname)) == 0) {
            machineName = hostname;
        }
    }
    
    std::string getFormattedDiagnosticInfo() const {
        std::ostringstream oss;
        oss << "Diagnostic Information:";
        oss << "\n  Diagnostic ID: " << diagnosticId;
        oss << "\n  Timestamp: " << getFormattedTimestamp();
        oss << "\n  Session ID: " << sessionId;
        oss << "\n  User ID: " << userId;
        oss << "\n  Machine: " << machineName;
        oss << "\n  Process ID: " << processId;
        oss << "\n  Thread ID: " << threadId;
        oss << "\n\n" << systemInfo.getFormattedSystemInfo();
        oss << "\n\n" << memoryUsage.getFormattedMemoryInfo();
        oss << "\n\n" << performance.getFormattedPerformanceInfo();
        
        if (!stackTraces.empty()) {
            oss << "\n\nStack Traces:";
            for (size_t i = 0; i < stackTraces.size(); ++i) {
                oss << "\n  Trace " << i << ":";
                oss << stackTraces[i].getFormattedStackTrace();
            }
        }
        
        if (!environmentVariables.empty()) {
            oss << "\n\nEnvironment Variables:";
            for (const auto& pair : environmentVariables) {
                oss << "\n  " << pair.first << " = " << pair.second;
            }
        }
        
        if (!configuration.empty()) {
            oss << "\n\nConfiguration:";
            for (const auto& pair : configuration) {
                oss << "\n  " << pair.first << " = " << pair.second;
            }
        }
        
        return oss.str();
    }
    
    std::string getFormattedTimestamp() const {
        auto time = std::chrono::system_clock::to_time_t(timestamp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    void addStackTrace(const std::vector<std::string>& frames,
                       const std::string& errorAddress = "",
                       const std::string& moduleName = "") {
        StackTraceInfo trace;
        trace.frames = frames;
        trace.errorAddress = errorAddress;
        trace.moduleName = moduleName;
        stackTraces.push_back(trace);
    }
    
    void addEnvironmentVariable(const std::string& key, const std::string& value) {
        environmentVariables[key] = value;
    }
    
    void addConfiguration(const std::string& key, const std::string& value) {
        configuration[key] = value;
    }
    
    // Get current system information
    static SystemInfo getCurrentSystemInfo();
    
    // Get current memory usage information
    static MemoryUsageInfo getCurrentMemoryUsage();
    
    // Get current performance metrics
    static PerformanceMetrics getCurrentPerformanceMetrics();
    
    // Get current stack trace
    static StackTraceInfo getCurrentStackTrace();
};

} // namespace nlm
