#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>

namespace nlm {

/**
 * Development Tools for NLM
 * 
 * A comprehensive set of development tools to assist with:
 * - Debugging and testing
 * - Code analysis and validation
 * - Performance monitoring
 * - System integration
 * - User assistance
 * 
 * These tools are designed to improve developer productivity and
 * system reliability during development, testing, and debugging.
 */

class DebugLogger {
public:
    enum class LogLevel {
        DEBUG = 0,
        INFO = 1,
        WARNING = 2,
        ERROR = 3,
        CRITICAL = 4
    };

    struct LogEntry {
        std::chrono::steady_clock::time_point timestamp;
        LogLevel level;
        std::string module;
        std::string message;
        std::string file;
        int line;
        std::unordered_map<std::string, std::string> context;
        
        double getElapsedTime() const {
            auto now = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - timestamp);
            return duration.count() / 1000.0;
        }
    };

    class DebugSession {
    public:
        DebugSession() = default;
        ~DebugSession() = default;
        
        void log(LogLevel level, const std::string& module, const std::string& message,
                 const std::string& file = "", int line = 0);
        
        void setContext(const std::string& key, const std::string& value);
        std::string getContext(const std::string& key) const;
        
        std::vector<LogEntry> getLogs(LogLevel minLevel = LogLevel::DEBUG) const;
        std::vector<LogEntry> getLogsSince(double timeSeconds, LogLevel minLevel = LogLevel::DEBUG) const;
        
        void clearLogs();
        void saveLogs(const std::string& filename) const;
        
        std::string generateReport() const;
        
    private:
        mutable std::mutex mutex_;
        std::vector<LogEntry> logs_;
        std::unordered_map<std::string, std::string> context_;
    };

    static DebugLogger& getInstance();
    
    void setLogLevel(LogLevel level);
    void setOutputFile(const std::string& filename);
    void setConsoleOutput(bool enable);
    void setFileOutput(bool enable);
    
    void log(LogLevel level, const std::string& module, const std::string& message,
             const std::string& file = "", int line = 0);
    
    void debug(const std::string& module, const std::string& message,
               const std::string& file = "", int line = 0);
    void info(const std::string& module, const std::string& message,
              const std::string& file = "", int line = 0);
    void warn(const std::string& module, const std::string& message,
              const std::string& file = "", int line = 0);
    void error(const std::string& module, const std::string& message,
               const std::string& file = "", int line = 0);
    void critical(const std::string& module, const std::string& message,
                  const std::string& file = "", int line = 0);
    
    std::vector<LogEntry> getLogs() const;
    std::vector<LogEntry> getLogsSince(double timeSeconds) const;
    
    void startDebugSession(const std::string& name);
    void endDebugSession(const std::string& name);
    DebugSession* getCurrentSession() { return currentSession_; }
    
    void generateReport(const std::string& filename = "debug_report.txt");
    
    void clear();
    
private:
    DebugLogger();
    ~DebugLogger() = default;
    
    void logToFile(const LogEntry& entry);
    void logToConsole(const LogEntry& entry);
    std::string formatLogEntry(const LogEntry& entry) const;
    
    LogLevel minLogLevel_;
    std::string outputFile_;
    bool consoleOutput_;
    bool fileOutput_;
    mutable std::mutex mutex_;
    
    std::vector<LogEntry> logs_;
    DebugSession* currentSession_;
    std::unordered_map<std::string, DebugSession> debugSessions_;
};

class IntegrationValidator {
public:
    struct IntegrationTestResult {
        std::string testName;
        bool passed;
        std::string message;
        double executionTime;
        std::unordered_map<std::string, std::string> details;
        
        bool operator<(const IntegrationTestResult& other) const {
            return passed && !other.passed;
        }
    };

    IntegrationValidator();
    ~IntegrationValidator() = default;
    
    // Test systems integration
    IntegrationTestResult testBrainInitialization(const std::string& configFile = "");
    IntegrationTestResult testMemorySystemIntegration();
    IntegrationTestResult testNeuromodulationIntegration();
    IntegrationTestResult testCognitiveSystemIntegration();
    IntegrationTestResult testPredictionSystemIntegration();
    IntegrationTestResult testDevelopmentSystemIntegration();
    IntegrationTestResult testPersistenceSystem();
    IntegrationTestResult testPerformanceInfrastructure();
    
    // Integration flow tests
    IntegrationTestResult testSensorimotorLoop();
    IntegrationTestResult testLearningFlow();
    IntegrationTestResult testMemoryConsolidation();
    IntegrationTestResult testDevelopmentProgression();
    IntegrationTestResult testNeuromodulationEffects();
    
    // Integration diagnostics
    std::vector<IntegrationTestResult> runAllTests();
    std::vector<IntegrationTestResult> runPhaseTests(int phase);
    
    // Analysis and reporting
    std::string generateIntegrationReport() const;
    std::string identifyIntegrationGaps() const;
    std::string getIntegrationScore() const;
    
    // Configuration
    void setTestTimeout(double timeoutSeconds) { testTimeout_ = timeoutSeconds; }
    void setVerboseOutput(bool verbose) { verboseOutput_ = verbose; }
    void setStrictMode(bool strict) { strictMode_ = strict; }
    
    // Test management
    void addCustomTest(const std::string& name, std::function<IntegrationTestResult()> test);
    void removeTest(const std::string& name);
    IntegrationTestResult runTest(const std::string& name);
    
    // Results access
    const std::vector<IntegrationTestResult>& getTestResults() const { return testResults_; }
    
private:
    double testTimeout_;
    bool verboseOutput_;
    bool strictMode_;
    
    std::unordered_map<std::string, std::function<IntegrationTestResult()>> customTests_;
    std::vector<IntegrationTestResult> testResults_;
    
    IntegrationTestResult runTestInternal(const std::string& name, std::function<IntegrationTestResult()> test);
    std::string formatTestResult(const IntegrationTestResult& result) const;
};

class CodeAnalyzer {
public:
    struct CodeMetrics {
        size_t totalLines;
        size_t commentLines;
        size_t blankLines;
        size_t codeLines;
        size_t includes;
        size_t functions;
        size_t classes;
        size_t namespaces;
        size_t structs;
        size_t typedefs;
        
        double commentRatio() const { return totalLines > 0 ? commentLines / (double)totalLines * 100.0 : 0.0; }
        double blankRatio() const { return totalLines > 0 ? blankLines / (double)totalLines * 100.0 : 0.0; }
        double codeRatio() const { return totalLines > 0 ? codeLines / (double)totalLines * 100.0 : 0.0; }
    };

    struct FileAnalysis {
        std::string filename;
        std::string path;
        CodeMetrics metrics;
        std::vector<std::string> includes;
        std::vector<std::string> functionNames;
        std::vector<std::string> classNames;
        std::vector<std::string> errorMessages;
        bool hasSyntaxErrors;
    };

    CodeAnalyzer();
    ~CodeAnalyzer() = default;
    
    // File analysis
    FileAnalysis analyzeFile(const std::string& filename);
    std::vector<FileAnalysis> analyzeDirectory(const std::string& directory, const std::vector<std::string>& extensions = {".cpp", ".hpp", ".h"});
    
    // Code quality analysis
    std::string analyzeCodeStyle(const std::vector<FileAnalysis>& files) const;
    std::string identifyCodeSmells(const std::vector<FileAnalysis>& files) const;
    std::string suggestRefactoring(const std::vector<FileAnalysis>& files) const;
    
    // Integration analysis
    std::string analyzeIntegrationPoints(const std::vector<FileAnalysis>& files) const;
    std::string checkCircularDependencies(const std::vector<FileAnalysis>& files) const;
    
    // Configuration
    void setIncludeDirectories(const std::vector<std::string>& includes) { includeDirs_ = includes; }
    void setMaxFileSize(size_t maxSize) { maxFileSize_ = maxSize; }
    void setSkipBinaryFiles(bool skip) { skipBinaryFiles_ = skip; }
    
    // Report generation
    std::string generateCodeReport(const std::vector<FileAnalysis>& files) const;
    std::string generateIntegrationReport(const std::vector<FileAnalysis>& files) const;
    std::string generateQualityReport(const std::vector<FileAnalysis>& files) const;

private:
    std::vector<std::string> includeDirs_;
    size_t maxFileSize_;
    bool skipBinaryFiles_;
    
    FileAnalysis analyzeSingleFile(const std::string& filename) const;
    bool isBinaryFile(const std::string& filename) const;
    std::string getFileExtension(const std::string& filename) const;
    std::vector<std::string> extractIncludes(const std::string& content) const;
    std::vector<std::string> extractFunctionNames(const std::string& content) const;
    std::vector<std::string> extractClassNames(const std::string& content) const;
};

class SystemDebugger {
public:
    struct DebugBreakpoint {
        std::string name;
        std::string condition;
        bool enabled;
        bool hit;
        size_t hitCount;
        std::chrono::steady_clock::time_point lastHit;
        
        DebugBreakpoint(const std::string& n, const std::string& c = "")
            : name(n), condition(c), enabled(true), hit(false), hitCount(0), lastHit() {}
    };

    struct DebugWatchpoint {
        std::string name;
        std::string expression;
        bool enabled;
        std::string lastValue;
        std::chrono::steady_clock::time_point lastModified;
    };

    SystemDebugger();
    ~SystemDebugger() = default;
    
    // Breakpoint management
    void addBreakpoint(const std::string& name, const std::string& condition = "");
    void removeBreakpoint(const std::string& name);
    void enableBreakpoint(const std::string& name, bool enable = true);
    bool isBreakpointEnabled(const std::string& name) const;
    
    // Watchpoint management
    void addWatchpoint(const std::string& name, const std::string& expression);
    void removeWatchpoint(const std::string& name);
    void enableWatchpoint(const std::string& name, bool enable = true);
    bool isWatchpointEnabled(const std::string& name) const;
    
    // Debug control
    void startDebugging();
    void stopDebugging();
    bool isDebugging() const { return debuggingEnabled_; }
    
    void stepInto();
    void stepOver();
    void stepReturn();
    
    // Debugging execution
    bool shouldBreak(const std::string& condition) const;
    void updateWatchpoints();
    std::string evaluateExpression(const std::string& expression) const;
    
    // Breakpoint hit handling
    void onBreakpointHit(const std::string& name);
    void onWatchpointModified(const std::string& name, const std::string& value);
    
    // Debugging control
    void pause();
    void resume();
    bool isPaused() const { return isPaused_; }
    
    // Debugging information
    std::vector<std::string> getActiveBreakpoints() const;
    std::vector<std::string> getActiveWatchpoints() const;
    std::string getDebugState() const;
    
    // Configuration
    void setBreakpointFile(const std::string& file) { breakpointFile_ = file; }
    void setBreakpointLine(size_t line) { breakpointLine_ = line; }

private:
    bool debuggingEnabled_;
    bool isPaused_;
    std::string breakpointFile_;
    size_t breakpointLine_;
    
    std::unordered_map<std::string, DebugBreakpoint> breakpoints_;
    std::unordered_map<std::string, DebugWatchpoint> watchpoints_;
    
    void checkBreakpoints();
    void checkWatchpoints();
};

class ExperimentRunner {
public:
    struct ExperimentConfig {
        std::string name;
        std::string description;
        size_t neuronCount;
        size_t regionCount;
        float connectionProbability;
        float simulationTimestep;
        size_t durationSteps;
        size_t numEpisodes;
        std::string outputFile;
        bool enableAllSystems;
        std::unordered_map<std::string, std::string> customConfig;
        
        bool operator<(const ExperimentConfig& other) const { return name < other.name; }
    };

    struct ExperimentResult {
        std::string name;
        double executionTime;
        size_t stepsCompleted;
        double finalPerformance;
        std::string status;
        std::unordered_map<std::string, double> metrics;
        std::string outputFile;
        
        bool isSuccess() const { return status == "completed" && executionTime > 0; }
    };

    ExperimentRunner();
    ~ExperimentRunner() = default;
    
    // Experiment management
    void addExperiment(const ExperimentConfig& config);
    void removeExperiment(const std::string& name);
    bool runExperiment(const std::string& name);
    bool runAllExperiments();
    
    // Experiment configuration presets
    ExperimentConfig createQuickExperiment() const;
    ExperimentConfig createResearchExperiment() const;
    ExperimentConfig createProductionExperiment() const;
    ExperimentConfig createStressTestExperiment() const;
    ExperimentConfig createIntegrationTestExperiment() const;
    
    // Experiment analysis
    std::vector<ExperimentResult> getExperimentResults() const { return results_; }
    ExperimentResult getExperimentResult(const std::string& name) const;
    
    // Results management
    void saveResults(const std::string& filename) const;
    bool loadResults(const std::string& filename);
    void clearResults() { results_.clear(); }
    
    // Analysis and reporting
    std::string generateExperimentReport() const;
    std::string analyzeExperimentPerformance(const std::string& name) const;
    std::string compareExperimentResults(const std::vector<std::string>& names) const;
    
    // Configuration
    void setOutputDirectory(const std::string& dir) { outputDir_ = dir; }
    void setResultsFile(const std::string& file) { resultsFile_ = file; }

private:
    std::string outputDir_;
    std::string resultsFile_;
    
    std::unordered_map<std::string, ExperimentConfig> experiments_;
    std::vector<ExperimentResult> results_;
    
    ExperimentResult runExperimentInternal(const ExperimentConfig& config);
    std::string generateExperimentConfigFile(const ExperimentConfig& config) const;
    std::string generateExperimentOutput(const ExperimentConfig& config, const ExperimentResult& result) const;
};

class PerformanceBenchmark {
public:
    struct BenchmarkResult {
        std::string benchmarkName;
        double executionTime;
        size_t stepsPerSecond;
        size_t memoryUsage;
        double cpuUsage;
        double integrationScore;
        std::string status;
        std::unordered_map<std::string, double> metrics;
        
        bool operator<(const BenchmarkResult& other) const {
            return executionTime < other.executionTime;
        }
    };

    PerformanceBenchmark();
    ~PerformanceBenchmark() = default;
    
    // Benchmark types
    BenchmarkResult runNeuralCoreBenchmark();
    BenchmarkResult runMemoryBenchmark();
    BenchmarkResult runPredictionBenchmark();
    BenchmarkResult runCognitiveBenchmark();
    BenchmarkResult runIntegrationBenchmark();
    BenchmarkResult runDevelopmentBenchmark();
    
    // Comprehensive benchmark
    std::vector<BenchmarkResult> runAllBenchmarks();
    
    // Analysis and reporting
    std::string generateBenchmarkReport(const std::vector<BenchmarkResult>& results) const;
    std::string identifyBenchmarkTrends(const std::vector<BenchmarkResult>& results) const;
    std::string getPerformanceRecommendations(const std::vector<BenchmarkResult>& results) const;
    
    // Configuration
    void setBenchmarkTimeout(double timeout) { benchmarkTimeout_ = timeout; }
    void setPerformanceThresholds(const std::unordered_map<std::string, double>& thresholds) { 
        performanceThresholds_ = thresholds; 
    }

private:
    double benchmarkTimeout_;
    std::unordered_map<std::string, double> performanceThresholds_;
    
    BenchmarkResult runBenchmark(const std::string& name, std::function<BenchmarkResult()> benchmarkFunc);
    std::string formatBenchmarkResult(const BenchmarkResult& result) const;
};

class DevelopmentWorkspace {
public:
    struct FileChange {
        std::string filename;
        std::string changeType;  // "added", "modified", "deleted"
        std::chrono::steady_clock::time_point timestamp;
        std::string author;
        std::string message;
    };

    DevelopmentWorkspace();
    ~DevelopmentWorkspace() = default;
    
    // File management
    void addFile(const std::string& filename, const std::string& content);
    void modifyFile(const std::string& filename, const std::string& newContent);
    void deleteFile(const std::string& filename);
    bool hasFile(const std::string& filename) const;
    std::string getFileContent(const std::string& filename) const;
    
    // Version control simulation
    void commit(const std::string& message, const std::string& author = "developer");
    void checkout(const std::string& branch);
    void createBranch(const std::string& branchName);
    void mergeBranch(const std::string& branchName);
    
    // Change tracking
    std::vector<FileChange> getRecentChanges(size_t limit = 10) const;
    std::vector<FileChange> getChangesSince(const std::string& date) const;
    
    // Workspace analysis
    std::string analyzeCodeQuality() const;
    std::string checkSyntaxErrors() const;
    std::string validateConfiguration() const;
    
    // Configuration
    void setWorkspaceRoot(const std::string& root) { workspaceRoot_ = root; }
    void setAutoSave(bool autoSave) { autoSave_ = autoSave; }
    void setBackupEnabled(bool backup) { backupEnabled_ = backup; }

private:
    std::string workspaceRoot_;
    bool autoSave_;
    bool backupEnabled_;
    
    std::unordered_map<std::string, std::string> files_;
    std::vector<FileChange> fileChanges_;
    
    void saveWorkspaceState() const;
    void createBackup() const;
    std::string getCurrentTimestamp() const;
};

/**
 * Main Development Tools Interface
 * 
 * Provides access to all development and debugging tools.
 */
class DevelopmentTools {
public:
    DevelopmentTools();
    ~DevelopmentTools() = default;
    
    // Access to individual tools
    DebugLogger& getDebugLogger() { return debugLogger_; }
    IntegrationValidator& getIntegrationValidator() { return integrationValidator_; }
    CodeAnalyzer& getCodeAnalyzer() { return codeAnalyzer_; }
    SystemDebugger& getSystemDebugger() { return systemDebugger_; }
    ExperimentRunner& getExperimentRunner() { return experimentRunner_; }
    PerformanceBenchmark& getPerformanceBenchmark() { return performanceBenchmark_; }
    DevelopmentWorkspace& getDevelopmentWorkspace() { return workspace_; }
    
    // Tool configuration
    void configureAll(bool verbose = false, bool strict = false);
    void initializeDevelopmentEnvironment();
    void shutdownDevelopmentEnvironment();
    
    // Common development workflows
    bool runFullIntegrationTest();
    bool runPerformanceBenchmarkSuite();
    bool runCodeQualityAnalysis();
    bool runExperimentSuite();
    
    // Status and reporting
    std::string getDevelopmentStatus() const;
    std::string generateComprehensiveReport() const;
    
    // Configuration
    void setWorkspacePath(const std::string& path) { workspacePath_ = path; }
    void setConfigFile(const std::string& file) { configFile_ = file; }

private:
    std::string workspacePath_;
    std::string configFile_;
    
    DebugLogger debugLogger_;
    IntegrationValidator integrationValidator_;
    CodeAnalyzer codeAnalyzer_;
    SystemDebugger systemDebugger_;
    ExperimentRunner experimentRunner_;
    PerformanceBenchmark performanceBenchmark_;
    DevelopmentWorkspace workspace_;
};

} // namespace nlm

#endif // DEVELOPMENT_TOOLS_HPP
