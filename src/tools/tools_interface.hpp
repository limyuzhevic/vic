#pragma once

// Advanced NLM Tools and Utilities
// Comprehensive toolset for brain simulation management, analysis, and debugging

#include <string>
#include <memory>
#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include "src/experiments/Phase6Demo.hpp"

namespace nlm {
namespace tools {

// Forward declarations for tool classes
class AdvancedCLI;
class MemoryProfiler;
class DebugSession;
class PerformanceMonitor;
class ExperimentRunner;

// Tool factory for creating tool instances
class ToolFactory {
public:
    static std::unique_ptr<AdvancedCLI> createAdvancedCLI();
    static std::unique_ptr<MemoryProfiler> createMemoryProfiler();
    static std::unique_ptr<DebugSession> createDebugSession(const std::string& checkpointFile);
    static std::unique_ptr<PerformanceMonitor> createPerformanceMonitor();
    static std::unique_ptr<ExperimentRunner> createExperimentRunner();
};

// Tool interface base class
class ToolInterface {
public:
    virtual ~ToolInterface() = default;
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    virtual bool isRunning() const = 0;
    virtual void run() = 0;
};

// Command line tool wrapper
class CommandLineTool : public ToolInterface {
public:
    CommandLineTool(const std::string& name, const std::string& description,
                   const std::string& command, const std::vector<std::string>& args = {});
    
    std::string getName() const override { return name_; }
    std::string getDescription() const override { return description_; }
    bool initialize() override;
    void shutdown() override;
    bool isRunning() const override;
    void run() override;
    
    void addArgument(const std::string& arg);
    void setWorkingDirectory(const std::string& dir) { workingDirectory_ = dir; }
    
private:
    std::string name_;
    std::string description_;
    std::string command_;
    std::vector<std::string> args_;
    std::string workingDirectory_;
    std::string processId_;
    bool initialized_;
};

// Tool result container for capturing tool outputs
class ToolResult {
public:
    enum class Status {
        SUCCESS,
        FAILURE,
        WARNING,
        RUNNING,
        PAUSED,
        CANCELLED
    };
    
    ToolResult();
    ToolResult(Status status, const std::string& message);
    
    Status getStatus() const { return status_; }
    const std::string& getMessage() const { return message_; }
    const std::string& getOutput() const { return output_; }
    
    void setStatus(Status status) { status_ = status; }
    void setMessage(const std::string& message) { message_ = message; }
    void addOutput(const std::string& output) { output_ += output + "\n"; }
    
    // Tool-specific result data
    template<typename T>
    void setData(const std::string& key, const T& data) {
        data_[key] = std::make_shared<DataContainer>(new DataContainer<T>(data));
    }
    
    template<typename T>
    std::shared_ptr<T> getData(const std::string& key) const {
        auto it = data_.find(key);
        if (it != data_.end()) {
            auto container = std::dynamic_pointer_cast<DataContainer<T>>(it->second);
            return container ? container->data : nullptr;
        }
        return nullptr;
    }
    
private:
    template<typename T>
    class DataContainer {
    public:
        T data;
        explicit DataContainer(const T& d) : data(d) {}
    };
    
    Status status_;
    std::string message_;
    std::string output_;
    std::map<std::string, std::shared_ptr<void>> data_;
};

// Tool event system for tool communication and notifications
class ToolEvent {
public:
    enum class Type {
        STATUS_CHANGE,
        PROGRESS_UPDATE,
        OUTPUT_AVAILABLE,
        ERROR_OCCURRED,
        TOOL_STARTED,
        TOOL_FINISHED,
        PARAMETER_CHANGE
    };
    
    ToolEvent(Type type, const std::string& source, const std::string& message);
    
    Type getType() const { return type_; }
    const std::string& getSource() const { return source_; }
    const std::string& getMessage() const { return message_; }
    const std::chrono::system_clock::time_point& getTimestamp() const { return timestamp_; }
    
    template<typename T>
    void setData(const std::string& key, const T& data) {
        data_[key] = std::make_shared<DataContainer>(new DataContainer<T>(data));
    }
    
    template<typename T>
    std::shared_ptr<T> getData(const std::string& key) const {
        auto it = data_.find(key);
        if (it != data_.end()) {
            auto container = std::dynamic_pointer_cast<DataContainer<T>>(it->second);
            return container ? container->data : nullptr;
        }
        return nullptr;
    }
    
private:
    template<typename T>
    class DataContainer {
    public:
        T data;
        explicit DataContainer(const T& d) : data(d) {}
    };
    
    Type type_;
    std::string source_;
    std::string message_;
    std::chrono::system_clock::time_point timestamp_;
    std::map<std::string, std::shared_ptr<void>> data_;
};

// Tool event listener interface
class ToolEventListener {
public:
    virtual ~ToolEventListener() = default;
    virtual void onEvent(const ToolEvent& event) = 0;
};

// Tool manager for coordinating multiple tools
class ToolManager {
public:
    ToolManager();
    ~ToolManager();
    
    // Tool lifecycle management
    bool addTool(std::unique_ptr<ToolInterface> tool);
    bool removeTool(const std::string& name);
    std::shared_ptr<ToolInterface> getTool(const std::string& name) const;
    std::vector<std::shared_ptr<ToolInterface>> getAllTools() const;
    
    // Tool execution
    bool startTool(const std::string& name);
    bool stopTool(const std::string& name);
    bool pauseTool(const std::string& name);
    bool resumeTool(const std::string& name);
    
    // Tool communication
    void sendEvent(const ToolEvent& event);
    void addEventListener(const std::string& toolName, std::shared_ptr<ToolEventListener> listener);
    void removeEventListener(const std::string& toolName, std::shared_ptr<ToolEventListener> listener);
    
    // Tool results management
    bool setToolResult(const std::string& toolName, const ToolResult& result);
    ToolResult getToolResult(const std::string& toolName) const;
    std::map<std::string, ToolResult> getAllResults() const;
    
    // Configuration management
    void setToolConfig(const std::string& toolName, const std::map<std::string, std::string>& config);
    std::map<std::string, std::map<std::string, std::string>> getAllConfigs() const;
    
    // Status reporting
    bool isToolRunning(const std::string& name) const;
    std::string getToolStatus(const std::string& name) const;
    std::string getSystemStatus() const;
    
    // Utility functions
    static std::string generateToolName(const std::string& baseName);
    static bool validateToolName(const std::string& name);
    
private:
    std::map<std::string, std::unique_ptr<ToolInterface>> tools_;
    std::map<std::string, std::vector<std::shared_ptr<ToolEventListener>>> eventListeners_;
    std::map<std::string, ToolResult> toolResults_;
    std::map<std::string, std::map<std::string, std::string>> toolConfigs_;
    mutable std::mutex mutex_;
    size_t nextToolId_;
};

// Advanced configuration tool for NLM
class AdvancedConfigurationTool {
public:
    AdvancedConfigurationTool();
    ~AdvancedConfigurationTool();
    
    // Configuration generation
    bool generateDefaultConfig(const std::string& outputFile) const;
    bool generateOptimizedConfig(const std::string& outputFile, 
                                 const std::map<std::string, double>& performanceTargets) const;
    bool generateExperimentConfig(const std::string& outputFile,
                                  const std::string& experimentType,
                                  const std::map<std::string, std::string>& parameters) const;
    
    // Configuration validation
    bool validateConfig(const std::string& configFile) const;
    std::map<std::string, std::string> analyzeConfig(const std::string& configFile) const;
    
    // Configuration optimization
    std::map<std::string, double> optimizeConfig(const std::map<std::string, double>& currentConfig,
                                                 const std::map<std::string, double>& targets) const;
    
    // Configuration comparison
    std::string compareConfigs(const std::string& configFile1, const std::string& configFile2) const;
    
    // Configuration backup and versioning
    bool backupConfig(const std::string& configFile, const std::string& backupPath = "") const;
    std::vector<std::string> listConfigVersions(const std::string& configFile) const;
    bool restoreConfigVersion(const std::string& configFile, const std::string& version) const;
    
private:
    std::string generateConfigFileContent(const std::map<std::string, std::string>& config) const;
    std::map<std::string, std::string> parseConfigFile(const std::string& configFile) const;
};

// Brain analysis tool for comprehensive brain state analysis
class BrainAnalysisTool {
public:
    BrainAnalysisTool();
    ~BrainAnalysisTool();
    
    // Brain state analysis
    struct BrainAnalysisResult {
        std::chrono::system_clock::time_point analysisTime;
        size_t neuronCount;
        size_t synapseCount;
        double averageFiringRate;
        double eIRatio;
        double memoryUtilization;
        double plasticityActivity;
        double neuromodulationLevel;
        std::map<std::string, double> componentHealth;
        std::vector<std::string> warnings;
        std::string detailedReport;
    };
    
    BrainAnalysisResult analyzeBrain(const std::shared_ptr<Brain>& brain) const;
    
    // Pattern analysis
    std::map<std::string, double> detectPatterns(const std::shared_ptr<Brain>& brain,
                                                  const std::string& patternType) const;
    std::vector<std::string> findCorrelations(const std::shared_ptr<Brain>& brain,
                                              const std::vector<std::string>& variables) const;
    
    // Health monitoring
    std::map<std::string, double> getBrainHealthScore(const std::shared_ptr<Brain>& brain) const;
    std::vector<std::string> getHealthWarnings(const std::shared_ptr<Brain>& brain) const;
    
    // Performance analysis
    struct PerformanceMetrics {
        double cpuUsage;
        double memoryUsage;
        double computationTime;
        double spikeRate;
        double connectivityDensity;
        double learningRate;
    };
    
    PerformanceMetrics analyzePerformance(const std::shared_ptr<Brain>& brain) const;
    
    // Visualization data generation
    std::string generateNetworkVisualizationData(const std::shared_ptr<Brain>& brain) const;
    std::string generateActivityHeatmapData(const std::shared_ptr<Brain>& brain) const;
    std::string generateTemporalPatternsData(const std::shared_ptr<Brain>& brain) const;
    
private:
    std::string generateNetworkMetrics(const std::shared_ptr<Brain>& brain) const;
    std::string generateActivityMetrics(const std::shared_ptr<Brain>& brain) const;
    std::string generatePlasticityMetrics(const std::shared_ptr<Brain>& brain) const;
};

// Utility functions
namespace utils {
    // String utilities
    std::string trim(const std::string& str);
    std::string toLower(const std::string& str);
    std::string toUpper(const std::string& str);
    bool startsWith(const std::string& str, const std::string& prefix);
    bool endsWith(const std::string& str, const std::string& suffix);
    
    // File utilities
    bool fileExists(const std::string& path);
    std::string getFileExtension(const std::string& filename);
    std::string getFileBasename(const std::string& filename);
    bool createDirectory(const std::string& path);
    std::vector<std::string> listFiles(const std::string& directory, const std::string& pattern = "*");
    
    // Mathematical utilities
    double calculateMean(const std::vector<double>& values);
    double calculateStandardDeviation(const std::vector<double>& values);
    double calculateCorrelation(const std::vector<double>& x, const std::vector<double>& y);
    double clamp(double value, double min, double max);
    
    // Time utilities
    std::string formatTime(const std::chrono::system_clock::time_point& time);
    double getCurrentTime();
    std::chrono::system_clock::time_point parseTime(const std::string& timeStr);
    
    // Random utilities
    void seedRandom(unsigned int seed = 0);
    double randomDouble(double min = 0.0, double max = 1.0);
    int randomInt(int min = 0, int max = 100);
}

} // namespace tools
} // namespace nlm
