// NLM Phase 6 Integration CLI
// Provides enhanced command-line interface for NLM Phase 6 integration

#include "core/NLMCLI.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>
#include <chrono>
#include <sstream>

namespace nlm {

struct PhaseCommand {
    int phaseNumber;
    std::string phaseName;
    std::string description;
    std::vector<std::string> defaultArgs;
    bool requiresBuild;
};

class Phase6CLI {
public:
    Phase6CLI();
    ~Phase6CLI() = default;
    
    // Command execution
    bool execute(int argc, char** argv);
    bool execute(const std::string& command, const std::vector<std::string>& args);
    
    // Phase execution
    bool executePhase(int phase, const std::vector<std::string>& args);
    bool executePhase(const std::string& phaseName, const std::vector<std::string>& args);
    
    // Performance and diagnostics
    bool runPerformanceBenchmark(const std::vector<std::string>& args);
    bool runDiagnostics(const std::vector<std::string>& args);
    bool generatePerformanceReport(const std::string& outputFile = "");
    bool generateDiagnosticsReport(const std::string& outputFile = "");
    
    // Configuration management
    bool loadConfiguration(const std::string& filename);
    bool saveConfiguration(const std::string& filename) const;
    bool validateConfiguration() const;
    bool applyConfigurationPreset(const std::string& preset);
    
    // Monitoring and profiling
    bool startMonitoring(const std::string& outputFile = "monitoring.log");
    bool stopMonitoring();
    bool generateSystemReport(const std::string& outputFile = "system_report.txt");
    
    // Help and information
    bool showHelp(const std::string& command = "");
    bool showPhaseHelp(int phase);
    bool showVersion();
    bool showBuildInfo();
    bool showSystemInfo();
    
    // Utility functions
    bool runQuickTests();
    bool runQuickPhase2();
    bool runQuickPhase3();
    bool runQuickPhase6();
    bool runAllQuickTests();
    
    // Configuration presets
    bool createDevPreset();
    bool createTestPreset();
    bool createProdPreset();
    bool createBenchmarkPreset();

private:
    NLMCLI nlmCLI;
    std::unique_ptr<Logger> logger;
    
    // Phase commands
    std::map<std::string, PhaseCommand> phaseCommands;
    std::map<int, std::string> phaseNumbers;
    
    // Monitoring and performance
    bool monitoringEnabled;
    std::chrono::steady_clock::time_point startTime;
    std::string monitoringOutputFile;
    
    // Performance metrics
    std::unordered_map<std::string, double> performanceMetrics;
    std::unordered_map<std::string, std::string> systemInfo;
    
    // Configuration presets
    std::map<std::string, std::map<std::string, std::string>> presets;
    
    // Command registration and execution helpers
    void registerPhaseCommands();
    void registerSystemCommands();
    bool executeSystemCommand(const std::string& command, const std::string& phase, const std::vector<std::string>& args);
    
    // Performance helpers
    void updatePerformanceMetric(const std::string& name, double value);
    double getPerformanceMetric(const std::string& name) const;
    void collectSystemInfo();
    std::string formatMetricsReport() const;
    
    // Configuration helpers
    bool loadPreset(const std::string& presetName);
    std::string getPhaseBinary(int phase) const;
    std::string getDemoBinary(const std::string& demo) const;
    
    // Output helpers
    void printWelcome() const;
    void printAvailableCommands() const;
    void printAvailablePhases() const;
    void printPresets() const;
    void printPerformanceSummary() const;
    
    // Validation helpers
    bool validatePhase(int phase) const;
    bool validatePhase(const std::string& phaseName) const;
    bool validateDemo(const std::string& demo) const;
};

} // namespace nlm
