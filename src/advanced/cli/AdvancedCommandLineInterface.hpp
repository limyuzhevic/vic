// Advanced Command Line Interface
// Main entry point for the advanced NLM brain manipulation and analysis system
// Integrates advanced CLI features with the existing brain simulation framework

#pragma once

#include "AdvancedCLI.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <memory>

namespace nlm {

class AdvancedCommandLineInterface {
public:
    AdvancedCommandLineInterface();
    ~AdvancedCommandLineInterface();
    
    // Main entry point
    bool run(int argc, char** argv);
    
    // Initialize the advanced CLI
    bool initialize();
    
    // Shutdown the advanced CLI
    void shutdown();
    
    // Get the advanced CLI instance
    static AdvancedCommandLineInterface& getInstance() {
        static AdvancedCommandLineInterface instance;
        return instance;
    }
    
    // Configuration
    void setBrainPath(const std::string& path) { brainPath_ = path; }
    void setConfigPath(const std::string& path) { configPath_ = path; }
    void setOutputPath(const std::string& path) { outputPath_ = path; }
    
    // Command processing
    bool processCommand(const std::string& commandLine);
    bool processAdvancedCLI(const AdvancedCLI::AdvancedCLIOptions& options);
    
    // Status and monitoring
    void printStatus() const;
    void printVersion() const;
    void printHelp() const;
    
    // Plugin management
    void registerPlugin(const std::string& name, std::unique_ptr<AdvancedCLI::PluginInterface> plugin);
    std::vector<std::string> getAvailablePlugins() const;
    
    // Statistics collection
    void collectSystemStatistics();
    void generateStatisticsReport(const std::string& outputFile) const;
    
private:
    std::unique_ptr<AdvancedCLI> advancedCLI_;
    std::unique_ptr<Config> config_;
    std::unique_ptr<Logger> logger_;
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<AgentBrain> agentBrain_;
    
    std::string brainPath_;
    std::string configPath_;
    std::string outputPath_;
    
    bool initializeBrain();
    bool initializeAgentBrain();
    bool loadBrainFromConfig();
    void cleanup();
    
    // Helper functions
    std::string parseCommandLine(int argc, char** argv);
    AdvancedCLI::AdvancedCLIOptions parseAdvancedCLIOptions(const std::string& commandLine);
    bool executeCommand(const AdvancedCLI::AdvancedCLIOptions& options);
    
    // Command routing
    bool routeBrainManipulationCommand(const AdvancedCLI::AdvancedCLIOptions& options);
    bool routeConfigurationCommand(const AdvancedCLI::AdvancedCLIOptions& options);
    bool routePerformanceCommand(const AdvancedCLI::AdvancedCLIOptions& options);
    bool routeDebugCommand(const AdvancedCLI::AdvancedCLIOptions& options);
    bool routeVisualizationCommand(const AdvancedCLI::AdvancedCLIOptions& options);
    bool routeAnalysisCommand(const AdvancedCLI::AdvancedCLIOptions& options);
    bool routeBenchmarkCommand(const AdvancedCLI::AdvancedCLIOptions& options);
    bool routeExportImportCommand(const AdvancedCLI::AdvancedCLIOptions& options);
    bool routeMonitoringCommand(const AdvancedCLI::AdvancedCLIOptions& options);
    bool routeCollaborationCommand(const AdvancedCLI::AdvancedCLIOptions& options);
    
    // Command execution
    bool executeBrainLoadCommand(const std::string& filepath);
    bool executeBrainSaveCommand(const std::string& filepath, const std::string& format);
    bool executeBrainCloneCommand();
    bool executeBrainModifyCommand(const std::map<std::string, std::string>& modifications);
    bool executeBrainResetCommand();
    bool executeConfigValidateCommand(const std::string& configFile);
    bool executeConfigBackupCommand(const std::string& backupFile);
    bool executeConfigRestoreCommand(const std::string& backupFile);
    bool executePerfMonitorCommand(const std::string& outputFile);
    bool executePerfProfileCommand(const std::string& outputFile);
    bool executePerfStatsCommand(const std::string& outputFile);
    bool executeDebugNeuralActivityCommand(const std::string& outputFile);
    bool executeDebugMemorySystemCommand(const std::string& outputFile);
    bool executeDebugNeuromodulationCommand(const std::string& outputFile);
    bool executeDebugDevelopmentCommand(const std::string& outputFile);
    bool executeAnalyzeLearningCommand(const std::string& outputFile);
    bool executeAnalyzePerformanceCommand(const std::string& outputFile);
    bool executeAnalyzeNetworkCommand(const std::string& outputFile);
    bool executeExperimentCreateCommand(const std::string& name, const std::string& directory);
    bool executeExperimentListCommand(const std::string& directory);
    bool executeExperimentLoadCommand(const std::string& name);
    bool executeExperimentSaveCommand(const std::string& name);
    bool executeExperimentCompareCommand(const std::string& name1, const std::string& name2);
    bool executeExperimentAnalyzeCommand(const std::string& name, const std::string& outputFile);
    bool executeVisualizeNeuralActivityCommand(const std::string& outputFile, const std::string& format);
    bool executeVisualizeNetworkTopologyCommand(const std::string& outputFile, const std::string& format);
    bool executeVisualizeLearningProgressCommand(const std::string& outputFile, const std::string& format);
    bool executePatternDetectCommand(const std::string& patternType, const std::string& outputFile);
    bool executePatternAnalyzeCommand(const std::string& patternType, const std::string& outputFile);
    bool executePatternClusterCommand(const std::string& patternType, const std::string& outputFile);
    bool executePatternPredictCommand(const std::string& patternType, const std::string& outputFile);
    bool executeBenchmarkCommand(const std::string& suite);
    bool executeBenchmarkRunCommand(const std::string& suite);
    bool executeBenchmarkCompareCommand(const std::string& suite1, const std::string& suite2);
    bool executeBenchmarkReportCommand(const std::string& suite, const std::string& outputFile);
    bool executeExportNetworkCommand(const std::string& filepath, const std::string& format);
    bool executeExportConfigurationCommand(const std::string& filepath, const std::string& format);
    bool executeExportExperimentDataCommand(const std::string& filepath, const std::string& format);
    bool executeExportResultsCommand(const std::string& filepath, const std::string& format);
    bool executeMonitorRealTimeCommand(const std::string& outputFile);
    bool executeMonitorMetricsCommand(const std::string& outputFile);
    bool executeMonitorHealthCommand(const std::string& outputFile);
    bool executeShareExperimentCommand(const std::string& experimentName, const std::string& server);
    bool executeCompareExperimentsCommand(const std::string& experimentName1, const std::string& experimentName2);
    bool executeConfigureNeuralNetworkCommand(const std::map<std::string, double>& parameters);
    bool executeConfigureMemorySystemCommand(const std::map<std::string, double>& parameters);
    bool executeConfigureNeuromodulationCommand(const std::map<std::string, double>& parameters);
    bool executeConfigureDevelopmentStageCommand(const std::map<std::string, double>& parameters);
    bool executeConfigurePerformanceOptimizationCommand(const std::map<std::string, double>& parameters);
    
    // Statistics collection
    struct SystemStatistics {
        double uptime;
        size_t totalNeurons;
        size_t totalSynapses;
        size_t activeNeurons;
        size_t firingNeurons;
        double averageMembranePotential;
        double maximumMembranePotential;
        double minimumMembranePotential;
        double averageFiringRate;
        double maximumFiringRate;
        size_t totalSpikes;
        double averageSynapticWeight;
        double maximumSynapticWeight;
        double minimumSynapticWeight;
        double energyConsumption;
        double simulationTime;
        size_t memoryUsage;
        size_t cpuUsage;
        std::map<std::string, double> memoryStats;
        std::map<std::string, double> performanceStats;
        std::map<std::string, double> neuromodulationStats;
        std::map<std::string, double> developmentStats;
    };
    
    SystemStatistics collectSystemStatistics() const;
    std::string systemStatisticsToString(const SystemStatistics& stats) const;
    
    // Progress tracking
    struct LearningProgress {
        double overallProgress;
        double accuracy;
        double convergenceRate;
        double stability;
        std::vector<double> learningCurve;
        std::vector<double> performanceMetrics;
        std::map<std::string, std::vector<double>> componentProgress;
        std::vector<std::string> learnedPatterns;
        size_t totalExperience;
        double totalReward;
        double predictionError;
    };
    
    LearningProgress collectLearningProgress() const;
    std::string learningProgressToString(const LearningProgress& progress) const;
    
    // Network topology
    struct NetworkTopology {
        size_t nodeCount;
        size_t edgeCount;
        size_t averageDegree;
        size_t clusteringCoefficient;
        size_t pathLength;
        size_t diameter;
        size_t connectedComponents;
        std::vector<size_t> degreeDistribution;
        std::vector<double> weightDistribution;
        std::map<size_t, size_t> communityStructure;
        std::vector<std::vector<size_t>> communityMembership;
        std::map<size_t, double> betweennessCentrality;
        std::map<size_t, double> closenessCentrality;
        std::map<size_t, double> eigenvectorCentrality;
    };
    
    NetworkTopology collectNetworkTopology() const;
    std::string networkTopologyToString(const NetworkTopology& topology) const;
    
    // Error handling
    void handleError(const std::string& errorMessage);
    void handleWarning(const std::string& warningMessage);
    
    // Resource management
    bool checkResourceAvailability() const;
    void manageResources();
};
