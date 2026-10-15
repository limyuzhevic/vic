#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "ExperimentProtocol.hpp"
#include "ExperimentStateManager.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

// Analyzer type
enum class AnalyzerType {
    Statistical,
    MachineLearning,
    Optimization,
    Visualization,
    QualityControl,
    Performance,
    Custom
};

// Analysis result
struct AnalysisResult {
    std::string analysisId;
    std::string analyzerName;
    std::string experimentName;
    bool success;
    std::map<std::string, double> metrics;
    std::vector<std::string> findings;
    std::vector<std::string> recommendations;
    std::chrono::system_clock::time_point timestamp;
    std::string errorMessage;
    
    AnalysisResult() : success(false), timestamp(std::chrono::system_clock::now()) {}
    AnalysisResult(const std::string& analysisId, const std::string& analyzerName, const std::string& experimentName, bool success)
        : analysisId(analysisId), analyzerName(analyzerName), experimentName(experimentName), success(success),
          timestamp(std::chrono::system_clock::now()) {}
};

// Analyzer base
class Analyzer {
public:
    virtual ~Analyzer() = default;
    
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::string getVersion() const = 0;
    virtual bool initialize(const Config& config) = 0;
    virtual void update(double timestep) = 0;
    virtual void shutdown() = 0;
    virtual bool isEnabled() const = 0;
    virtual void setEnabled(bool enabled) = 0;
    virtual AnalyzerType getType() const = 0;
    virtual AnalysisResult analyze(const std::map<std::string, std::vector<double>>& data) = 0;
    virtual std::string getAnalyzerFormat() const = 0;
};

class ExperimentAnalyzers {
public:
    ExperimentAnalyzers();
    ~ExperimentAnalyzers();
    
    // Disable copying, enable moving
    ExperimentAnalyzers(const ExperimentAnalyzers&) = delete;
    ExperimentAnalyzers& operator=(const ExperimentAnalyzers&) = delete;
    ExperimentAnalyzers(ExperimentAnalyzers&&) noexcept;
    ExperimentAnalyzers& operator=(ExperimentAnalyzers&&) noexcept;
    
    // Analyzer management
    bool registerAnalyzer(const std::string& name, std::shared_ptr<Analyzer> analyzer);
    bool unregisterAnalyzer(const std::string& name);
    bool updateAnalyzer(const std::string& name, std::shared_ptr<Analyzer> analyzer);
    std::shared_ptr<Analyzer> getAnalyzer(const std::string& name) const;
    std::vector<std::string> getAllAnalyzerNames() const;
    
    // Analyzer lifecycle
    bool initializeAnalyzer(const std::string& name, const Config& config);
    bool shutdownAnalyzer(const std::string& name);
    bool updateAnalyzer(const std::string& name, double timestep);
    bool enableAnalyzer(const std::string& name);
    bool disableAnalyzer(const std::string& name);
    
    // Analysis execution
    AnalysisResult analyzeData(const std::string& analyzerName, const std::map<std::string, std::vector<double>>& data);
    std::vector<AnalysisResult> analyzeMultipleData(const std::vector<std::pair<std::string, std::map<std::string, std::vector<double>>>>& data);
    std::string exportAnalysisResult(const std::string& analyzerName, const AnalysisResult& result, const std::string& format);
    
    // Analyzer status
    bool isAnalyzerEnabled(const std::string& name) const;
    bool isAnalyzerInitialized(const std::string& name) const;
    std::chrono::system_clock::time_point getAnalyzerLastUpdate(const std::string& name) const;
    std::string getAnalyzerStatus(const std::string& name) const;
    
    // Analyzer analysis
    std::map<std::string, double> getAnalyzerPerformance(const std::string& name) const;
    std::vector<std::string> getAnalyzerDependencies(const std::string& name) const;
    std::vector<std::string> getAnalyzersByType(AnalyzerType type) const;
    
    // Analyzer backup and recovery
    bool backupAnalyzerData(const std::string& name, const std::string& backupPath) const;
    bool restoreAnalyzerData(const std::string& name, const std::string& backupPath);
    
    // Analyzer cleanup
    bool cleanupAnalyzerData(const std::string& name);
    bool cleanupAllAnalyzerData();
    
    // Analyzer system management
    bool enableAnalyzerMonitoring(const std::string& name, bool enable);
    bool isAnalyzerMonitoringEnabled(const std::string& name) const;
    bool enableRealTimeAnalyzerUpdates(bool enable);
    bool isRealTimeAnalyzerUpdatesEnabled() const;
    
    // Analyzer utilities
    static std::string generateAnalyzerName(const std::string& prefix = "analyzer") const;
    static bool isValidAnalyzerName(const std::string& name) const;
    static AnalyzerType stringToAnalyzerType(const std::string& typeStr) const;
    static std::string analyzerTypeToString(AnalyzerType type) const;
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm