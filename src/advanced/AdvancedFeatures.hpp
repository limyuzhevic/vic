// Advanced Features Module - Interface Definitions
// This file contains the interface definitions for all advanced user features
// and command-line interface enhancements for NLM Phase 6+

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>

namespace nlm {

// Forward declarations
class Config;
class Brain;
class Logger;
class ConsoleLogger;
class LogEntry;

// Advanced Configuration Management Interface
class AdvancedConfigManager {
public:
    virtual ~AdvancedConfigManager() = default;
    
    // Load configuration from multiple sources with priority
    virtual bool loadConfiguration(const std::vector<std::string>& configFiles, 
                                  const std::string& environmentPrefix = "NLM_") = 0;
    
    // Export configuration with different formats and options
    virtual bool exportConfiguration(const std::string& outputFile, 
                                    const std::string& format = "json",
                                    bool includeDefaults = false,
                                    bool includeSecrets = false) = 0;
    
    // Create configuration template
    virtual bool createConfigTemplate(const std::string& templateFile, 
                                     const std::string& description = "NLM Configuration Template") = 0;
    
    // Validate configuration against constraints
    virtual std::vector<std::string> validateConfiguration(const std::string& constraintsFile = "") = 0;
};

// Performance Monitoring Interface
class PerformanceMonitor {
public:
    virtual ~PerformanceMonitor() = default;
    
    // Get comprehensive performance statistics
    virtual std::string getPerformanceReport() const = 0;
    
    // Get detailed performance metrics
    virtual std::map<std::string, double> getPerformanceMetrics() const = 0;
    
    // Check if performance is within acceptable limits
    virtual bool isPerformanceAcceptable(double maxMemoryUsage = 1000.0, 
                                         double maxFiringRate = 1000.0) const = 0;
};

// Visualization Tool Interface
class VisualizationTool {
public:
    virtual ~VisualizationTool() = default;
    
    // Generate neural network visualization
    virtual std::string generateNetworkVisualization() const = 0;
    
    // Generate detailed statistics report
    virtual std::string generateDetailedReport(const std::string& reportType = "full") const = 0;
    
    // Export statistics to file
    virtual bool exportStatistics(const std::string& outputFile, 
                                 const std::string& format = "txt",
                                 const std::string& reportType = "full") const = 0;
};

// Batch Processing Interface
class BatchProcessor {
public:
    virtual ~BatchProcessor() = default;
    
    // Run simulation sequence with different configurations
    virtual bool runBatch(const std::vector<std::string>& configFiles, 
                         const std::string& outputPrefix = "batch_output",
                         int stepsPerConfig = 100) = 0;
    
    // Run multiple experiments with different random seeds
    virtual bool runExperiments(const std::vector<uint64_t>& seeds, 
                               const std::string& outputPrefix = "experiment",
                               int stepsPerExperiment = 500) = 0;
};

// Advanced CLI Interface
class AdvancedCLI {
public:
    virtual ~AdvancedCLI() = default;
    
    // Parse advanced command-line arguments
    virtual bool parseAdvancedArgs(int argc, char** argv) = 0;
    
    // Show help
    virtual void printHelp() = 0;
    
    // Show version
    virtual void printVersion() = 0;
    
    // Handle batch mode
    virtual void handleBatchMode(const std::string& batchFile) = 0;
    
    // Handle export mode
    virtual void handleExportMode(const std::string& format, const std::string& outputFile) = 0;
    
    // Handle validate mode
    virtual void handleValidateMode() = 0;
    
    // Handle monitor mode
    virtual void handleMonitorMode(int duration) = 0;
    
    // Handle profile mode
    virtual void handleProfileMode() = 0;
};

// Factory function for creating advanced tools
namespace AdvancedFeatures {
    // Create advanced configuration manager
    std::shared_ptr<AdvancedConfigManager> createAdvancedConfigManager(std::shared_ptr<Config> config);
    
    // Create performance monitor
    std::shared_ptr<PerformanceMonitor> createPerformanceMonitor(std::shared_ptr<Brain> brain);
    
    // Create visualization tool
    std::shared_ptr<VisualizationTool> createVisualizationTool(std::shared_ptr<Brain> brain);
    
    // Create batch processor
    std::shared_ptr<BatchProcessor> createBatchProcessor(std::shared_ptr<Brain> brain);
    
    // Create advanced CLI
    std::shared_ptr<AdvancedCLI> createAdvancedCLI(std::shared_ptr<Brain> brain, 
                                                 std::shared_ptr<Config> config);
    
    // Run advanced demonstration
    int runAdvancedDemo(int argc, char** argv);
}

} // namespace nlm
