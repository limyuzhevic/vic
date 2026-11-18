// Research Tools and Extensibility Framework
// Provides tools for custom experiments, analysis, and research customization

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <chrono>
#include <algorithm>
#include <typeindex>
#include <any>
#include <filesystem>
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "experiments/Experiment.hpp"
#include "visualization/VisualizationInterface.hpp"

namespace nlm {

// Research experiment categories
enum class ExperimentCategory {
    Learning,          // Learning and plasticity experiments
    Memory,           // Memory system experiments
    Neuromodulation,  // Neuromodulation and motivation
    Development,      // Development and aging
    Social,           // Social and multi-agent interactions
    Perception,       // Perception and sensory processing
    Action,           // Action selection and motor control
    Cognitive,        // Cognitive processes and reasoning
    Custom           // Custom experiments
};

// Research analysis types
enum class AnalysisType {
    Statistical,       // Statistical analysis
    Pattern,         // Pattern detection
    Correlation,     // Correlation analysis
    TimeSeries,      // Time series analysis
    Network,         // Network analysis
    MachineLearning, // Machine learning analysis
    Custom          // Custom analysis
};

// Research hypothesis
struct ResearchHypothesis {
    std::string hypothesis;               // The hypothesis statement
    std::string prediction;              // Predicted outcome
    std::string variable;                // Variable to test
    double confidence;                   // Confidence level (0-1)
    std::string category;                // Experiment category
    std::string methodology;             // Methodology description
    std::chrono::system_clock::time_point created;
    
    ResearchHypothesis() : hypothesis(), prediction(), variable(), confidence(0.0),
                          category(), methodology(), created() {}
    
    ResearchHypothesis(const std::string& h, const std::string& p,
                      const std::string& v, double c = 1.0,
                      const std::string& cat = "Custom",
                      const std::string& m = "Standard")
        : hypothesis(h), prediction(p), variable(v), confidence(c),
          category(cat), methodology(m) {
        created = std::chrono::system_clock::now();
    }
    
    std::string toString() const {
        std::ostringstream oss;
        oss << "Hypothesis: " << hypothesis << "\n"
            << "Prediction: " << prediction << "\n"
            << "Variable: " << variable << "\n"
            << "Confidence: " << (confidence * 100) << "%\n"
            << "Category: " << category << "\n"
            << "Created: " << formatTime(created) << "\n"
            << "Methodology: " << methodology << "\n";
        return oss.str();
    }
    
private:
    std::string formatTime(const std::chrono::system_clock::time_point& tp) const {
        auto time = std::chrono::system_clock::to_time_t(tp);
        std::ostringstream oss;
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }
};

// Research data structure
struct ResearchData {
    std::string experimentId;                    // Experiment identifier
    std::chrono::system_clock::time_point timestamp; // Data collection time
    std::unordered_map<std::string, std::any> metrics; // Experimental metrics
    std::vector<VisualizationData> visualizationData;  // Visualization data
    std::string analysisResults;                 // Analysis results
    double pValue;                              // Statistical significance
    double effectSize;                          // Effect size
    std::string hypothesisTested;               // Hypothesis being tested
    bool hypothesisSupported;                    // Whether hypothesis is supported
    std::string notes;                           // Additional notes
    
    ResearchData() : timestamp(std::chrono::system_clock::now()),
                     pValue(1.0), effectSize(0.0), hypothesisSupported(false) {}
    
    // Add a metric
    template<typename T>
    void addMetric(const std::string& name, const T& value) {
        metrics[name] = value;
    }
    
    // Get a metric
    template<typename T>
    T getMetric(const std::string& name) const {
        auto it = metrics.find(name);
        if (it != metrics.end() && it->second.type() == typeid(T)) {
            return std::any_cast<T>(it->second);
        }
        throw std::runtime_error("Metric not found or wrong type: " + name);
    }
    
    // Get all metric names
    std::vector<std::string> getMetricNames() const {
        std::vector<std::string> names;
        for (const auto& pair : metrics) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    // Check if metric exists
    bool hasMetric(const std::string& name) const {
        return metrics.find(name) != metrics.end();
    }
    
    std::string toString() const {
        std::ostringstream oss;
        oss << "Research Data Entry" << std::endl;
        oss << "===================" << std::endl;
        oss << "Experiment ID: " << experimentId << std::endl;
        oss << "Timestamp: " << formatTime(timestamp) << std::endl;
        oss << "Hypothesis: " << hypothesisTested << std::endl;
        oss << "Supported: " << (hypothesisSupported ? "Yes" : "No") << std::endl;
        oss << "P-value: " << pValue << std::endl;
        oss << "Effect size: " << effectSize << std::endl;
        oss << "\nMetrics:" << std::endl;
        for (const auto& pair : metrics) {
            oss << "  " << pair.first << ": ";
            oss << anyToString(pair.second) << std::endl;
        }
        oss << "\nAnalysis results:" << std::endl;
        oss << analysisResults << std::endl;
        oss << "\nNotes: " << notes << std::endl;
        return oss.str();
    }
    
private:
    std::string formatTime(const std::chrono::system_clock::time_point& tp) const {
        auto time = std::chrono::system_clock::to_time_t(tp);
        std::ostringstream oss;
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }
    
    std::string anyToString(const std::any& value) const {
        std::ostringstream oss;
        if (value.type() == typeid(int)) {
            oss << std::any_cast<int>(value);
        } else if (value.type() == typeid(double)) {
            oss << std::any_cast<double>(value);
        } else if (value.type() == typeid(std::string)) {
            oss << "\"" << std::any_cast<std::string>(value) << "\"";
        } else if (value.type() == typeid(bool)) {
            oss << (std::any_cast<bool>(value) ? "true" : "false");
        } else {
            oss << "[" << value.type().name() << "]";
        }
        return oss.str();
    }
};

// Research experiment base class
class ResearchExperiment {
public:
    ResearchExperiment() : experimentId(generateId()), enabled(true) {}
    virtual ~ResearchExperiment() = default;
    
    // Experiment metadata
    std::string getId() const { return experimentId; }
    std::string getName() const { return name; }
    std::string getDescription() const { return description; }
    std::string getCategory() const { return category; }
    ExperimentCategory getCategoryEnum() const { return categoryEnum; }
    
    // Experiment configuration
    virtual void configure(const std::unordered_map<std::string, std::string>& config) {}
    virtual bool validateConfiguration() const { return true; }
    
    // Experiment execution
    virtual bool run(std::shared_ptr<Brain> brain,
                    std::shared_ptr<AgentBrain> agent,
                    std::shared_ptr<SimpleWorld> world,
                    std::vector<ResearchData>& results) = 0;
    
    // Experiment management
    void setName(const std::string& n) { name = n; }
    void setDescription(const std::string& d) { description = d; }
    void setCategory(const std::string& c) { category = c; }
    void setCategoryEnum(ExperimentCategory cat) { categoryEnum = cat; }
    void enable(bool e) { enabled = e; }
    bool isEnabled() const { return enabled; }
    
    // Experiment information
    virtual std::string getParameters() const { return "Default parameters"; }
    virtual std::string getResultsSummary() const { return "No results available"; }
    
    // Experiment analysis
    virtual std::string analyzeResults(const std::vector<ResearchData>& results) const { return "No analysis available"; }
    
    // Experiment export
    virtual bool exportResults(const std::vector<ResearchData>& results, const std::string& filepath) const;
    
    // Experiment import
    virtual bool importResults(std::vector<ResearchData>& results, const std::string& filepath);
    
protected:
    static std::string generateId() {
        return "exp_" + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count());
    }
    
    std::string experimentId;
    std::string name;
    std::string description;
    std::string category;
    ExperimentCategory categoryEnum;
    bool enabled;
};

// Research analysis base class
class ResearchAnalysis {
public:
    ResearchAnalysis() : analysisId(generateId()), enabled(true) {}
    virtual ~ResearchAnalysis() = default;
    
    // Analysis metadata
    std::string getId() const { return analysisId; }
    std::string getName() const { return name; }
    std::string getDescription() const { return description; }
    AnalysisType getType() const { return type; }
    
    // Analysis configuration
    virtual void configure(const std::unordered_map<std::string, std::string>& config) {}
    virtual bool validateConfiguration() const { return true; }
    
    // Analysis execution
    virtual std::string performAnalysis(const std::vector<ResearchData>& data) = 0;
    
    // Analysis management
    void setName(const std::string& n) { name = n; }
    void setDescription(const std::string& d) { description = d; }
    void setType(AnalysisType t) { type = t; }
    void enable(bool e) { enabled = e; }
    bool isEnabled() const { return enabled; }
    
protected:
    static std::string generateId() {
        return "ana_" + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count());
    }
    
    std::string analysisId;
    std::string name;
    std::string description;
    AnalysisType type;
    bool enabled;
};

// Research management system
class ResearchManager {
public:
    ResearchManager();
    ~ResearchManager();
    
    // Experiment management
    void registerExperiment(std::unique_ptr<ResearchExperiment> experiment);
    bool unregisterExperiment(const std::string& experimentId);
    ResearchExperiment* getExperiment(const std::string& experimentId);
    const ResearchExperiment* getExperiment(const std::string& experimentId) const;
    std::vector<ResearchExperiment*> getAllExperiments() const;
    std::vector<ResearchExperiment*> getExperimentsByCategory(ExperimentCategory category) const;
    
    // Analysis management
    void registerAnalysis(std::unique_ptr<ResearchAnalysis> analysis);
    bool unregisterAnalysis(const std::string& analysisId);
    ResearchAnalysis* getAnalysis(const std::string& analysisId);
    const ResearchAnalysis* getAnalysis(const std::string& analysisId) const;
    std::vector<ResearchAnalysis*> getAllAnalyses() const;
    
    // Research hypothesis management
    void addHypothesis(const ResearchHypothesis& hypothesis);
    bool removeHypothesis(const std::string& hypothesisId);
    ResearchHypothesis* getHypothesis(const std::string& hypothesisId);
    std::vector<ResearchHypothesis> getAllHypotheses() const;
    std::vector<ResearchHypothesis> getHypothesesByCategory(ExperimentCategory category) const;
    
    // Experiment execution
    bool runExperiment(const std::string& experimentId,
                      std::shared_ptr<Brain> brain,
                      std::shared_ptr<AgentBrain> agent,
                      std::shared_ptr<SimpleWorld> world,
                      std::vector<ResearchData>& results,
                      const std::unordered_map<std::string, std::string>& parameters = {});
    
    // Batch experiment execution
    bool runAllExperiments(std::shared_ptr<Brain> brain,
                          std::shared_ptr<AgentBrain> agent,
                          std::shared_ptr<SimpleWorld> world,
                          std::vector<ResearchData>& results,
                          const std::unordered_map<std::string, std::string>& parameters = {});
    
    // Data management
    bool saveResults(const std::vector<ResearchData>& results, const std::string& filepath) const;
    bool loadResults(std::vector<ResearchData>& results, const std::string& filepath);
    bool exportResults(const std::vector<ResearchData>& results,
                      const std::string& format,
                      const std::string& filepath) const;
    
    // Experiment templates
    static std::unique_ptr<ResearchExperiment> createExperimentTemplate(const std::string& experimentType);
    static std::unique_ptr<ResearchAnalysis> createAnalysisTemplate(const std::string& analysisType);
    
    // Export/import templates
    static bool exportTemplates(const std::string& directory);
    static bool importTemplates(const std::string& directory);
    
    // Research statistics
    struct ResearchStats {
        size_t totalExperiments = 0;
        size_t enabledExperiments = 0;
        size_t totalAnalyses = 0;
        size_t enabledAnalyses = 0;
        size_t totalHypotheses = 0;
        size_t totalResultsGenerated = 0;
        double totalExecutionTime = 0.0;
        
        std::string toString() const {
            std::ostringstream oss;
            oss << "Research Statistics:" << std::endl;
            oss << "  Total experiments: " << totalExperiments << std::endl;
            oss << "  Enabled experiments: " << enabledExperiments << std::endl;
            oss << "  Total analyses: " << totalAnalyses << std::endl;
            oss << "  Enabled analyses: " << enabledAnalyses << std::endl;
            oss << "  Total hypotheses: " << totalHypotheses << std::endl;
            oss << "  Total results generated: " << totalResultsGenerated << std::endl;
            oss << "  Total execution time: " << totalExecutionTime << " seconds" << std::endl;
            return oss.str();
        }
    };
    
    ResearchStats getStats() const;
    
    // Configuration
    void setConfig(const std::string& key, const std::string& value);
    std::string getConfig(const std::string& key) const;
    void loadConfig(const std::string& filepath);
    void saveConfig(const std::string& filepath) const;
    
    // Research workflow
    bool runResearchCycle(std::shared_ptr<Brain> brain,
                        std::shared_ptr<AgentBrain> agent,
                        std::shared_ptr<SimpleWorld> world,
                        const std::vector<std::string>& experimentIds = {});
    
    // Data validation
    bool validateResearchData(const ResearchData& data) const;
    std::vector<std::string> validateResearchDataBatch(const std::vector<ResearchData>& data) const;
    
    // Error handling
    using ErrorCallback = std::function<void(const std::string& error)>;
    void setErrorCallback(ErrorCallback callback);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Research experiment implementations

// Learning experiment: Study plasticity and learning
class LearningExperiment : public ResearchExperiment {
public:
    LearningExperiment();
    
    bool run(std::shared_ptr<Brain> brain,
             std::shared_ptr<AgentBrain> agent,
             std::shared_ptr<SimpleWorld> world,
             std::vector<ResearchData>& results) override;
    
    std::string getParameters() const override;
    std::string getResultsSummary() const override;
    std::string analyzeResults(const std::vector<ResearchData>& results) const override;
};

// Memory experiment: Study memory systems
class MemoryExperiment : public ResearchExperiment {
public:
    MemoryExperiment();
    
    bool run(std::shared_ptr<Brain> brain,
             std::shared_ptr<AgentBrain> agent,
             std::shared_ptr<SimpleWorld> world,
             std::vector<ResearchData>& results) override;
    
    std::string getParameters() const override;
    std::string getResultsSummary() const override;
    std::string analyzeResults(const std::vector<ResearchData>& results) const override;
};

// Neuromodulation experiment: Study motivation and emotion
class NeuromodulationExperiment : public ResearchExperiment {
public:
    NeuromodulationExperiment();
    
    bool run(std::shared_ptr<Brain> brain,
             std::shared_ptr<AgentBrain> agent,
             std::shared_ptr<SimpleWorld> world,
             std::vector<ResearchData>& results) override;
    
    std::string getParameters() const override;
    std::string getResultsSummary() const override;
    std::string analyzeResults(const std::vector<ResearchData>& results) const override;
};

// Development experiment: Study aging and development
class DevelopmentExperiment : public ResearchExperiment {
public:
    DevelopmentExperiment();
    
    bool run(std::shared_ptr<Brain> brain,
             std::shared_ptr<AgentBrain> agent,
             std::shared_ptr<SimpleWorld> world,
             std::vector<ResearchData>& results) override;
    
    std::string getParameters() const override;
    std::string getResultsSummary() const override;
    std::string analyzeResults(const std::vector<ResearchData>& results) const override;
};

// Social experiment: Study multi-agent interactions
class SocialExperiment : public ResearchExperiment {
public:
    SocialExperiment();
    
    bool run(std::shared_ptr<Brain> brain,
             std::shared_ptr<AgentBrain> agent,
             std::shared_ptr<SimpleWorld> world,
             std::vector<ResearchData>& results) override;
    
    std::string getParameters() const override;
    std::string getResultsSummary() const override;
    std::string analyzeResults(const std::vector<ResearchData>& results) const override;
};

// Research analysis implementations

// Statistical analysis
class StatisticalAnalysis : public ResearchAnalysis {
public:
    StatisticalAnalysis();
    
    std::string performAnalysis(const std::vector<ResearchData>& data) override;
};

// Pattern analysis
class PatternAnalysis : public ResearchAnalysis {
public:
    PatternAnalysis();
    
    std::string performAnalysis(const std::vector<ResearchData>& data) override;
};

// Time series analysis
class TimeSeriesAnalysis : public ResearchAnalysis {
public:
    TimeSeriesAnalysis();
    
    std::string performAnalysis(const std::vector<ResearchData>& data) override;
};

// Network analysis
class NetworkAnalysis : public ResearchAnalysis {
public:
    NetworkAnalysis();
    
    std::string performAnalysis(const std::vector<ResearchData>& data) override;
};

// Research experiment and analysis factory
class ResearchFactory {
public:
    static std::unique_ptr<ResearchExperiment> createLearningExperiment(const std::string& name = "Learning Experiment");
    static std::unique_ptr<ResearchExperiment> createMemoryExperiment(const std::string& name = "Memory Experiment");
    static std::unique_ptr<ResearchExperiment> createNeuromodulationExperiment(const std::string& name = "Neuromodulation Experiment");
    static std::unique_ptr<ResearchExperiment> createDevelopmentExperiment(const std::string& name = "Development Experiment");
    static std::unique_ptr<ResearchExperiment> createSocialExperiment(const std::string& name = "Social Experiment");
    static std::unique_ptr<ResearchExperiment> createCustomExperiment(const std::string& name,
                                                                   const std::string& description,
                                                                   ExperimentCategory category);
    
    static std::unique_ptr<ResearchAnalysis> createStatisticalAnalysis(const std::string& name = "Statistical Analysis");
    static std::unique_ptr<ResearchAnalysis> createPatternAnalysis(const std::string& name = "Pattern Analysis");
    static std::unique_ptr<ResearchAnalysis> createTimeSeriesAnalysis(const std::string& name = "Time Series Analysis");
    static std::unique_ptr<ResearchAnalysis> createNetworkAnalysis(const std::string& name = "Network Analysis");
    static std::unique_ptr<ResearchAnalysis> createCustomAnalysis(const std::string& name,
                                                               const std::string& description,
                                                               AnalysisType type);
    
    // Template management
    static bool registerTemplate(const std::string& typeName, std::unique_ptr<ResearchExperiment> template);
    static std::unique_ptr<ResearchExperiment> getTemplate(const std::string& typeName);
    static bool registerAnalysisTemplate(const std::string& typeName, std::unique_ptr<ResearchAnalysis> template);
    static std::unique_ptr<ResearchAnalysis> getAnalysisTemplate(const std::string& typeName);
};

} // namespace nlm
