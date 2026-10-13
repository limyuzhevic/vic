#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Memory leak tracking
struct MemoryAllocation {
    void* pointer;
    size_t size;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    bool isFreed;
};

// Performance profiling data
struct ProfileData {
    std::string functionName;
    std::chrono::duration<double> duration;
    size_t callCount;
    double cpuTime;
    double memoryUsage;
    std::chrono::system_clock::time_point timestamp;
};

// Neural debugging information
struct NeuralDebugInfo {
    std::string message;
    std::chrono::system_clock::time_point timestamp;
    std::map<std::string, double> metrics;
};

// Configuration validation results
struct ValidationResult {
    std::string key;
    std::string section;
    bool isValid;
    std::string errorMessage;
    std::string warningMessage;
};

class MemoryLeakDetector {
public:
    MemoryLeakDetector();
    ~MemoryLeakDetector();
    
    void startDetection(double samplingInterval = 0.1);
    void stopDetection();
    bool isDetecting() const;
    
    bool detectLeaks(const std::string& outputFile);
    bool detectFragmentation(const std::string& outputFile);
    
    std::vector<MemoryAllocation> getAllocations() const;
    std::vector<std::pair<std::string, size_t>> getLeaks() const;
    double getTotalLeakSize() const;
    
    void logAllocation(void* ptr, size_t size, const std::string& file, int line, const std::string& function);
    void logDeallocation(void* ptr);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    void detectionLoop();
    void analyzeLeaks();
};

class PerformanceProfiler {
public:
    PerformanceProfiler();
    ~PerformanceProfiler();
    
    void startProfiling(const std::string& profileName);
    void stopProfiling();
    bool isProfiling() const;
    
    void profileFunction(const std::string& functionName);
    void profileMemoryAllocation(void* ptr, size_t size);
    void profileMemoryDeallocation(void* ptr);
    
    bool generateReport(const std::string& outputFile, const std::string& format = "text");
    bool compareProfiles(const std::string& profileFile1, const std::string& profileFile2, const std::string& outputFile);
    
    std::vector<ProfileData> getProfileData() const;
    std::map<std::string, double> getMetrics() const;
    std::vector<std::string> getCallStack() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    void generateTextReport(std::ofstream& file);
    void generateCsvReport(std::ofstream& file);
    void generateJsonReport(std::ofstream& file);
    void cleanupOldProfiles();
};

class NeuralActivityDebugger {
public:
    NeuralActivityDebugger();
    ~NeuralActivityDebugger();
    
    void setBrain(std::shared_ptr<Brain> brain);
    std::shared_ptr<Brain> getBrain() const;
    
    bool debugActivity(const std::string& outputFile);
    bool visualizeActivity(const std::string& outputFile, const std::string& format = "html");
    
    std::vector<NeuralDebugInfo> getDebugInfo() const;
    std::map<std::string, double> getNeuralMetrics() const;
    
    // Connection analysis
    bool validateConnections(const std::string& outputFile);
    bool detectDeadNeurons(const std::string& outputFile);
    bool analyzeSpikingPatterns(const std::string& outputFile);
    
    // Neural state analysis
    bool analyzeNeuralState(const std::string& outputFile);
    bool compareNeuralStates(const std::string& state1, const std::string& state2, const std::string& outputFile);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    void collectNeuralDebugInfo();
    void generateHtmlVisualization(std::ofstream& file);
    void generateSvgVisualization(std::ofstream& file);
    void validateNeuralConnections(std::ofstream& file);
    void detectDeadNeuronsInternal(std::ofstream& file);
    void analyzeSpikingPatternsInternal(std::ofstream& file);
};

class ConfigurationAnalyzer {
public:
    ConfigurationAnalyzer();
    ~ConfigurationAnalyzer();
    
    void setConfig(std::shared_ptr<Config> config);
    std::shared_ptr<Config> getConfig() const;
    
    bool analyze(const std::string& outputFile);
    bool validate(const std::string& configFile, const std::string& schemaFile = "");
    bool generateReport(const std::string& outputFile);
    
    std::vector<ValidationResult> getValidationResults() const;
    std::map<std::string, double> getConfigurationStatistics() const;
    
    // Configuration comparison
    bool compare(const std::string& configFile1, const std::string& configFile2, const std::string& outputFile);
    
    // Schema management
    bool loadSchema(const std::string& filepath);
    bool saveSchema(const std::string& filepath) const;
    bool validateAgainstSchema(const std::string& configFile, const std::string& schemaName);
    
    // Inheritance analysis
    bool analyzeInheritance(const std::string& configFile);
    bool validateInheritanceChain(const std::string& configFile);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    void analyzeConfigValues(std::vector<ValidationResult>& results);
    void generateAnalysisReport(std::ofstream& file);
    void compareConfigurations(std::ofstream& file, const Config& config1, const Config& config2);
    void loadValidationSchema(const std::string& filepath);
    void analyzeInheritanceChain(const Config& config, std::ofstream& file);
    void validateInheritanceChain(const Config& config, std::ofstream& file);
};

} // namespace nlm
