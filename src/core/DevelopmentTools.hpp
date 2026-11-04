#pragma once

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <map>
#include <set>

namespace nlm {

// Neural network analysis utilities
class NeuralNetworkAnalyzer {
public:
    struct ConnectionInfo {
        std::string sourceNeuron;
        std::string targetNeuron;
        double weight;
        double delay;
        std::string type; // "excitatory", "inhibitory", "modulatory"
    };
    
    struct Statistics {
        double meanWeight;
        double stddevWeight;
        double minWeight;
        double maxWeight;
        size_t totalConnections;
        std::map<std::string, size_t> connectionTypes;
    };
    
    NeuralNetworkAnalyzer();
    ~NeuralNetworkAnalyzer();
    
    // Load neural network from configuration
    bool loadFromConfig(const std::string& configFile);
    
    // Get connections from the neural network
    std::vector<ConnectionInfo> getConnections() const;
    
    // Get statistics about the network
    Statistics computeStatistics() const;
    
    // Find strongly connected components
    std::vector<std::vector<std::string>> findStronglyConnectedComponents() const;
    
    // Detect community structure
    std::vector<std::vector<std::string>> detectCommunities() const;
    
    // Analyze network connectivity
    double computeConnectivity() const;
    
    // Get degree distribution
    std::map<size_t, size_t> getDegreeDistribution() const;
    
    // Find critical nodes
    std::vector<std::string> findCriticalNodes(double threshold = 0.1) const;
    
    // Analyze neural dynamics
    struct DynamicsAnalysis {
        double firingRate;
        double synchronyLevel;
        double stability;
        std::vector<double> frequencySpectrum;
    };
    
    DynamicsAnalysis analyzeDynamics(const std::string& timeSeriesData) const;
    
    // Visualize network (placeholder for now)
    void visualizeGraph(const std::string& outputFile, const std::string& format = "png") const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Simulation utilities
class SimulationUtilities {
public:
    struct SimulationSnapshot {
        double timestamp;
        std::map<std::string, double> neuralActivity;
        std::map<std::string, double> synapticWeights;
        std::map<std::string, double> neuromodulatorLevels;
        std::map<std::string, double> rewardSignals;
    };
    
    SimulationUtilities();
    ~SimulationUtilities();
    
    // Run simulation with given configuration
    bool runSimulation(const std::string& configFile, const std::string& outputFile);
    
    // Load simulation data
    std::vector<SimulationSnapshot> loadSimulationData(const std::string& dataFile);
    
    // Analyze simulation results
    struct SimulationMetrics {
        double totalReward;
        double avgFiringRate;
        double rewardEfficiency;
        double learningRate;
        size_t totalSteps;
        size_t successfulActions;
    };
    
    SimulationMetrics analyzeResults(const std::vector<SimulationSnapshot>& data);
    
    // Generate performance report
    std::string generatePerformanceReport(const SimulationMetrics& metrics);
    
    // Export data for analysis
    bool exportForAnalysis(const std::vector<SimulationSnapshot>& data, const std::string& outputFormat);
    
    // Validate simulation output
    bool validateSimulationOutput(const std::string& outputFile);
    
    // Utility functions for simulation
    static double interpolate(double x, double x1, double y1, double x2, double y2);
    static double normalize(double value, double min, double max);
    static std::string formatTime(double seconds);
    static std::string formatNumber(double value, int precision = 6);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Debugging utilities
class DebugUtilities {
public:
    struct MemoryProfile {
        size_t totalMemory;
        size_t usedMemory;
        size_t peakMemory;
        std::map<std::string, size_t> memoryByComponent;
    };
    
    DebugUtilities();
    ~DebugUtilities();
    
    // Memory profiling
    MemoryProfile getMemoryProfile() const;
    
    // Performance monitoring
    struct PerformanceMetrics {
        double cpuUsage;
        double memoryUsage;
        double diskUsage;
        std::vector<double> cpuHistory;
        std::vector<double> memoryHistory;
    };
    
    PerformanceMetrics getPerformanceMetrics() const;
    
    // Debug breakpoint utilities
    void setBreakpoint(const std::string& condition);
    bool checkBreakpoint(const std::string& condition);
    void stepThroughDebug() const;
    
    // Stack trace and crash reporting
    std::string getStackTrace() const;
    void reportCrash(const std::exception& e);
    
    // Interactive debugging
    void startInteractiveSession();
    
    // Log file analysis
    struct LogAnalysis {
        size_t totalLogEntries;
        size_t errors;
        size_t warnings;
        std::map<std::string, size_t> logLevels;
        std::vector<std::string> errorMessages;
    };
    
    LogAnalysis analyzeLogFile(const std::string& logFile);
    
    // Network packet capture (for network-based simulations)
    void startPacketCapture(const std::string& interface);
    void stopPacketCapture();
    std::vector<std::string> getCapturedPackets();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Development tools
class DevelopmentTools {
public:
    DevelopmentTools();
    ~DevelopmentTools();
    
    // Code generation utilities
    bool generateCode(const std::string& templateFile, const std::map<std::string, std::string>& variables);
    
    // Project structure analysis
    struct ProjectAnalysis {
        std::vector<std::string> sourceFiles;
        std::vector<std::string> headerFiles;
        std::vector<std::string> testFiles;
        std::map<std::string, size_t> fileSizes;
        std::set<std::string> dependencies;
        std::map<std::string, std::vector<std::string>> includeGraph;
    };
    
    ProjectAnalysis analyzeProjectStructure(const std::string& projectRoot);
    
    // Build system integration
    bool runBuild(const std::string& target = "all");
    bool runTests();
    bool runBenchmarks();
    
    // Environment setup
    bool setupDevelopmentEnvironment();
    bool setupTestEnvironment();
    
    // Package management
    bool installDependencies();
    bool updateDependencies();
    bool checkDependencies();
    
    // Documentation generation
    bool generateDocumentation(const std::string& outputDir);
    bool generateAPIReference(const std::string& outputDir);
    bool generateUserGuide(const std::string& outputDir);
    
    // Version control integration
    bool commitChanges(const std::string& message);
    bool pushChanges(const std::string& remote = "origin", const std::string& branch = "main");
    bool createBranch(const std::string& branchName);
    bool mergeBranch(const std::string& branchName);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// High-level development workflow
class DevelopmentWorkflow {
public:
    struct WorkflowStep {
        std::string name;
        std::function<bool()> execute;
        std::function<bool()> validate;
        std::string description;
        bool required;
    };
    
    DevelopmentWorkflow();
    ~DevelopmentWorkflow();
    
    // Add workflow steps
    void addStep(const std::string& name, const std::function<bool()>& execute,
                 const std::function<bool()>& validate, const std::string& description,
                 bool required = false);
    
    // Run complete workflow
    bool run(const std::string& workflowName = "default");
    
    // Get workflow status
    struct WorkflowStatus {
        std::string name;
        std::vector<std::string> completedSteps;
        std::vector<std::string> failedSteps;
        std::vector<std::string> skippedSteps;
        bool success;
        double progress; // 0.0 to 1.0
    };
    
    WorkflowStatus getStatus(const std::string& workflowName = "default");
    
    // Save/load workflow
    bool saveWorkflow(const std::string& filename);
    bool loadWorkflow(const std::string& filename);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Command-line utility for development
class DevCommandLine {
public:
    DevCommandLine();
    ~DevCommandLine();
    
    // Available commands
    void registerCommand(const std::string& name, const std::string& description,
                         const std::function<bool(const std::vector<std::string>&)>& handler);
    
    // Parse and execute command
    bool execute(const std::vector<std::string>& args);
    
    // Print help
    void printHelp() const;
    
    // Built-in commands
    bool handleAnalyze(const std::vector<std::string>& args);
    bool handleBuild(const std::vector<std::string>& args);
    bool handleTest(const std::vector<std::string>& args);
    bool handleDebug(const std::vector<std::string>& args);
    bool handleRun(const std::vector<std::string>& args);
    bool handleDeploy(const std::vector<std::string>& args);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm