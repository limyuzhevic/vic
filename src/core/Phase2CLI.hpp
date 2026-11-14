// Phase 2: Real Neural Computation CLI
// Provides comprehensive command-line interface for NLM Phase 2 operations

#include "core/NLMCLI.hpp"
#include <limits>
#include <numeric>

namespace nlm {

// Performance monitoring and profiling
class PerformanceProfiler {
public:
    struct Metric {
        std::string name;
        double value;
        std::chrono::steady_clock::time_point timestamp;
        std::string unit;
    };
    
    PerformanceProfiler();
    ~PerformanceProfiler() = default;
    
    void startProfiling(const std::string& operation);
    void stopProfiling(const std::string& operation);
    void recordMetric(const std::string& name, double value, const std::string& unit = "");
    double getMetric(const std::string& name) const;
    std::string getMetricsReport() const;
    void saveMetrics(const std::string& filename) const;
    void clear();
    
    // Phase 2 specific metrics
    void recordNeuronMetrics(size_t neuronCount, size_t spikeCount, double avgFiringRate);
    void recordSynapseMetrics(size_t synapseCount, double avgWeight);
    void recordConnectivityMetrics(float connectionProbability);

private:
    std::unordered_map<std::string, std::vector<Metric>> metrics;
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> profilingSessions;
    std::chrono::steady_clock::time_point sessionStart;
};

// Diagnostics and system information
class SystemDiagnostics {
public:
    struct SystemInfo {
        std::string platform;
        std::string compiler;
        std::string buildType;
        size_t totalMemory;
        size_t availableMemory;
        int cpuCores;
        double cpuUsage;
        std::string osVersion;
    };
    
    SystemDiagnostics();
    ~SystemDiagnostics() = default;
    
    void collectSystemInfo();
    SystemInfo getSystemInfo() const { return currentSystemInfo; }
    std::string getSystemInfoReport() const;
    bool generateSystemReport(const std::string& filename = "system_info.txt");
    
    // Memory diagnostics
    bool checkMemoryLeaks();
    bool checkMemoryUsage();
    
    // Neural network diagnostics
    bool checkNeuralConnectivity();
    bool checkPlasticityOperation();
    bool checkSTDPFunction();
    
    // Performance diagnostics
    bool checkPerformanceBottlenecks();
    bool checkMemoryAccessPatterns();

private:
    SystemInfo currentSystemInfo;
    std::vector<std::string> memoryLeaks;
    std::vector<std::string> performanceIssues;
    
    void detectMemoryLeaks();
    void detectPerformanceIssues();
    void detectNeuralIssues();
};

// Configuration validation and error handling
class ConfigurationValidator {
public:
    struct ValidationResult {
        bool valid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        std::map<std::string, std::string> validatedConfig;
    };
    
    ValidationResult validate(const std::string& filename);
    ValidationResult validate(const std::map<std::string, std::string>& config);
    bool validateValue(const std::string& key, const std::string& value, const std::string& type = "auto");
    std::string getValidationError(const std::string& key) const;
    std::vector<std::string> getWarnings() const { return warnings; }
    
    // Configuration file format validation
    bool validateINIFormat(const std::string& content);
    bool validateJSONFormat(const std::string& content);
    
    // Configuration value validation
    bool validateNumeric(const std::string& value, double min = -std::numeric_limits<double>::infinity(), 
                         double max = std::numeric_limits<double>::infinity());
    bool validateString(const std::string& value, size_t minLength = 0, size_t maxLength = std::numeric_limits<size_t>::max());
    bool validateEnum(const std::string& value, const std::vector<std::string>& validValues);

private:
    std::vector<std::string> warnings;
    std::map<std::string, std::string> validationRules;
};

// Main Phase 2 CLI interface
class Phase2CLI {
public:
    Phase2CLI();
    ~Phase2CLI() = default;
    
    // Configuration and setup
    bool loadConfiguration(const std::string& filename);
    bool saveConfiguration(const std::string& filename) const;
    bool validateConfiguration();
    
    // Demo execution
    bool runPhase2Demo(const std::vector<std::string>& args);
    bool runQuickPhase2(const std::vector<std::string>& args);
    bool runExtendedPhase2(const std::vector<std::string>& args);
    
    // Performance and diagnostics
    bool runPerformanceBenchmark(const std::vector<std::string>& args);
    bool runDiagnostics(const std::vector<std::string>& args);
    bool generatePerformanceReport(const std::string& outputFile = "");
    bool generateDiagnosticsReport(const std::string& outputFile = "");
    
    // Monitoring and profiling
    bool startMonitoring(const std::string& outputFile = "nlm_monitoring.log");
    bool stopMonitoring();
    bool generateSystemReport(const std::string& outputFile = "nlm_system_report.txt");
    
    // Help and information
    bool showHelp(const std::string& command = "");
    bool showVersion();
    bool showBuildInfo();
    bool showSystemInfo();
    
    // Convenience commands
    bool runQuickTests();
    bool runPhase2ConnectivityTest();
    bool runPhase2PlasticityTest();
    bool runPhase2STDPTest();
    bool runPhase2LearningTest();
    
    // Configuration presets
    bool createDevPreset();
    bool createTestPreset();
    bool createProdPreset();

private:
    std::unique_ptr<NLMCLI> nlmCLI;
    std::unique_ptr<PerformanceProfiler> profiler;
    std::unique_ptr<SystemDiagnostics> diagnostics;
    std::unique_ptr<ConfigurationValidator> validator;
    
    std::string currentConfigFile;
    bool monitoringEnabled;
    std::string monitoringOutputFile;
    
    // Configuration presets
    std::map<std::string, std::map<std::string, std::string>> presets;
    
    // Command registration
    void registerCommands();
    void registerConvenienceCommands();
    
    // Execution helpers
    bool executePhase2Command(const std::string& command, const std::vector<std::string>& args);
    bool executeQuickCommand(const std::string& command, const std::vector<std::string>& args);
    bool validateAndExecute(const std::string& command, const std::vector<std::string>& args);
    
    // Output helpers
    void printWelcome() const;
    void printAvailableCommands() const;
    void printPhase2Commands() const;
    void printPerformanceSummary() const;
    void printDiagnosticsSummary() const;
    
    // Configuration helpers
    bool loadPreset(const std::string& presetName);
    bool applyPresetToConfig(const std::map<std::string, std::string>& preset);
    bool saveCurrentConfig() const;
    
    // Validation helpers
    bool validatePhase2Config() const;
    bool validateDemoArgs(const std::string& demo, const std::vector<std::string>& args) const;
};

} // namespace nlm
