#ifndef NLM_EXPERIMENT_FACTORY_HPP
#define NLM_EXPERIMENT_FACTORY_HPP

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <unordered_map>
#include <any>

namespace nlm {
namespace experiment {

// Experiment types
enum class ExperimentType {
    Simulation,
    Integration,
    Learning,
    Development,
    Cognitive,
    Performance,
    Custom
};

// Experiment status
enum class ExperimentStatus {
    NotStarted,
    Initializing,
    Running,
    Paused,
    Completed,
    Failed,
    Stopped
};

// Experiment metadata
struct ExperimentMetadata {
    std::string name;
    std::string description;
    std::string author;
    std::string version;
    std::string creationDate;
    std::string lastModified;
    ExperimentType type;
    std::map<std::string, std::string> tags;
    std::map<std::string, std::any> parameters;
    
    ExperimentMetadata() : type(ExperimentType::Custom) {}
};

// Experiment result
struct ExperimentResult {
    std::string experimentName;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    double duration;
    ExperimentStatus status;
    bool success;
    std::string errorMessage;
    std::map<std::string, std::any> metrics;
    std::map<std::string, std::any> results;
    std::vector<std::string> logMessages;
    
    ExperimentResult() : status(ExperimentStatus::NotStarted), success(false), duration(0.0) {}
};

// Experiment interface
class Experiment {
public:
    virtual ~Experiment() = default;
    
    // Experiment metadata
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual ExperimentType getType() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getAuthor() const = 0;
    
    // Experiment lifecycle
    virtual bool initialize(const std::map<std::string, std::any>& config) = 0;
    virtual ExperimentResult run(const std::map<std::string, std::any>& config) = 0;
    virtual bool stop() = 0;
    virtual bool reset() = 0;
    virtual ExperimentStatus getStatus() const = 0;
    
    // Configuration management
    virtual bool configure(const std::string& key, const std::any& value) = 0;
    virtual std::any getConfig(const std::string& key) const = 0;
    virtual std::map<std::string, std::any> getAllConfig() const = 0;
    
    // Experiment control
    virtual bool pause() = 0;
    virtual bool resume() = 0;
    virtual bool isRunning() const = 0;
    
    // Experiment monitoring
    virtual double getProgress() const = 0;
    virtual std::chrono::system_clock::time_point getStartTime() const = 0;
    virtual std::chrono::system_clock::time_point getEndTime() const = 0;
    virtual std::string getCurrentStep() const = 0;
    
    // Experiment results
    virtual ExperimentResult getLastResult() const = 0;
    virtual std::map<std::string, std::any> getMetrics() const = 0;
    virtual std::vector<std::string> getLogMessages() const = 0;
};

// Experiment factory interface
class ExperimentFactory {
public:
    virtual ~ExperimentFactory() = default;
    
    virtual std::string getFactoryName() const = 0;
    virtual std::vector<std::string> getSupportedTypes() const = 0;
    virtual std::unique_ptr<Experiment> createExperiment(const std::string& experimentName) = 0;
    virtual std::vector<std::string> getAvailableExperiments() const = 0;
    virtual bool isExperimentAvailable(const std::string& experimentName) const = 0;
    virtual std::map<std::string, std::string> getExperimentMetadata(const std::string& experimentName) const = 0;
};

// Experiment registry
class ExperimentRegistry {
public:
    ExperimentRegistry();
    ~ExperimentRegistry() = default;
    
    // Experiment registration
    bool registerExperiment(std::unique_ptr<Experiment> experiment);
    bool unregisterExperiment(const std::string& experimentName);
    bool registerExperimentType(const std::string& typeName, 
                               std::function<std::unique_ptr<Experiment>(const std::string&)> factory);
    
    // Experiment management
    bool hasExperiment(const std::string& experimentName) const;
    bool hasExperimentType(const std::string& typeName) const;
    std::vector<std::string> getExperimentNames() const;
    std::vector<std::string> getExperimentTypes() const;
    
    // Experiment creation
    std::unique_ptr<Experiment> createExperiment(const std::string& experimentName);
    std::unique_ptr<Experiment> createExperiment(const std::string& typeName, 
                                                 const std::string& experimentName);
    
    // Experiment management
    bool loadExperimentsFromFile(const std::string& filepath);
    bool saveExperimentsToFile(const std::string& filepath) const;
    bool loadExperimentsFromDirectory(const std::string& directoryPath);
    
    // Experiment type factory management
    void registerTypeFactory(const std::string& typeName, 
                            std::function<std::unique_ptr<Experiment>(const std::string&)> factory);
    bool unregisterTypeFactory(const std::string& typeName);
    std::function<std::unique_ptr<Experiment>(const std::string&)> getTypeFactory(const std::string& typeName) const;
    
    // Experiment metadata
    bool setExperimentMetadata(const std::string& experimentName, 
                              const ExperimentMetadata& metadata);
    ExperimentMetadata getExperimentMetadata(const std::string& experimentName) const;
    std::map<std::string, ExperimentMetadata> getAllExperimentMetadata() const;
    
    // Experiment statistics
    size_t getExperimentCount() const;
    std::string getExperimentVersion(const std::string& experimentName) const;
    std::string getExperimentDescription(const std::string& experimentName) const;
    
private:
    std::map<std::string, std::unique_ptr<Experiment>> registeredExperiments_;
    std::map<std::string, std::function<std::unique_ptr<Experiment>(const std::string&)>> typeFactories_;
    std::map<std::string, ExperimentMetadata> experimentMetadata_;
};

// Experiment template system
class ExperimentTemplate {
public:
    struct Template {
        std::string name;
        std::string description;
        std::string type;
        std::map<std::string, std::any> defaultConfig;
        std::map<std::string, std::string> parameters;
        std::string category;
        std::string difficulty;
        
        Template() : type("custom") {}
    };
    
    ExperimentTemplate();
    ~ExperimentTemplate() = default;
    
    // Template management
    void addTemplate(const Template& template);
    bool removeTemplate(const std::string& name);
    bool loadTemplatesFromFile(const std::string& filepath);
    bool saveTemplatesToFile(const std::string& filepath) const;
    
    // Template operations
    Template* getTemplate(const std::string& name);
    const Template* getTemplate(const std::string& name) const;
    bool hasTemplate(const std::string& name) const;
    std::vector<std::string> getTemplateNames() const;
    std::vector<std::string> getTemplateCategories() const;
    std::vector<std::string> getTemplateDifficulties() const;
    
    // Template instantiation
    std::unique_ptr<Experiment> createExperimentFromTemplate(const std::string& name,
                                                          const std::string& experimentName,
                                                          const std::map<std::string, std::any>& config);
    
private:
    std::map<std::string, Template> templates_;
    std::map<std::string, std::vector<std::string>> categoryMap_;
    std::map<std::string, std::vector<std::string>> difficultyMap_;
};

// Experiment runner
class ExperimentRunner {
public:
    ExperimentRunner();
    ~ExperimentRunner() = default;
    
    // Experiment execution
    ExperimentResult runExperiment(const std::string& experimentName,
                                  const std::map<std::string, std::any>& config);
    ExperimentResult runExperiment(const std::string& typeName,
                                  const std::string& experimentName,
                                  const std::map<std::string, std::any>& config);
    
    // Batch processing
    std::vector<ExperimentResult> runExperiments(const std::vector<std::string>& experimentNames,
                                               const std::map<std::string, std::any>& config);
    std::vector<ExperimentResult> runExperiments(const std::vector<std::pair<std::string, std::string>>& typeExperimentPairs,
                                               const std::map<std::string, std::any>& config);
    
    // Experiment control
    bool stopExperiment(const std::string& experimentName);
    bool pauseExperiment(const std::string& experimentName);
    bool resumeExperiment(const std::string& experimentName);
    
    // Experiment monitoring
    ExperimentStatus getExperimentStatus(const std::string& experimentName) const;
    double getExperimentProgress(const std::string& experimentName) const;
    std::chrono::system_clock::time_point getExperimentStartTime(const std::string& experimentName) const;
    std::chrono::system_clock::time_point getExperimentEndTime(const std::string& experimentName) const;
    std::string getExperimentCurrentStep(const std::string& experimentName) const;
    
    // Experiment results
    ExperimentResult getExperimentResult(const std::string& experimentName) const;
    std::map<std::string, std::any> getExperimentMetrics(const std::string& experimentName) const;
    std::vector<std::string> getExperimentLogs(const std::string& experimentName) const;
    
    // Experiment management
    bool cleanupExperiment(const std::string& experimentName);
    void cleanupAllExperiments();
    
    // Experiment configuration
    bool configureExperiment(const std::string& experimentName,
                           const std::map<std::string, std::any>& config);
    std::map<std::string, std::any> getExperimentConfig(const std::string& experimentName) const;
    
private:
    std::map<std::string, std::unique_ptr<Experiment>> runningExperiments_;
    std::map<std::string, ExperimentResult> experimentResults_;
    std::mutex experimentsMutex_;
    
    std::string generateExperimentId(const std::string& experimentName);
    bool isExperimentRunning(const std::string& experimentName) const;
};

// Experiment loader interface
class ExperimentLoader {
public:
    virtual ~ExperimentLoader() = default;
    
    virtual bool canLoad(const std::string& path) const = 0;
    virtual std::unique_ptr<Experiment> loadExperiment(const std::string& path) = 0;
    virtual std::vector<std::string> getSupportedExtensions() const = 0;
    virtual std::string getLoaderName() const = 0;
};

// File-based experiment loader
class FileExperimentLoader : public ExperimentLoader {
public:
    FileExperimentLoader();
    ~FileExperimentLoader() override = default;
    
    bool canLoad(const std::string& path) const override;
    std::unique_ptr<Experiment> loadExperiment(const std::string& path) override;
    std::vector<std::string> getSupportedExtensions() const override;
    std::string getLoaderName() const override { return "FileExperimentLoader"; }
    
private:
    bool loadPythonExperiment(const std::string& path);
    bool loadCppExperiment(const std::string& path);
    bool loadJSONExperiment(const std::string& path);
};

// Experiment configuration
class ExperimentConfig {
public:
    struct ExperimentEntry {
        std::string name;
        std::string type;
        std::map<std::string, std::any> config;
        bool autoLoad;
        bool enabled;
        std::map<std::string, std::string> tags;
        
        ExperimentEntry() : autoLoad(true), enabled(true) {}
    };
    
    ExperimentConfig();
    ~ExperimentConfig() = default;
    
    // Configuration management
    void addExperiment(const ExperimentEntry& entry);
    bool removeExperiment(const std::string& name);
    bool updateExperiment(const std::string& name, const ExperimentEntry& entry);
    
    // Configuration retrieval
    ExperimentEntry* getExperiment(const std::string& name);
    const ExperimentEntry* getExperiment(const std::string& name) const;
    bool hasExperiment(const std::string& name) const;
    std::vector<std::string> getExperimentNames() const;
    
    // Configuration loading and saving
    bool loadFromFile(const std::string& filepath);
    bool saveToFile(const std::string& filepath) const;
    bool loadFromString(const std::string& content);
    std::string saveToString() const;
    
    // Configuration validation
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    
private:
    std::map<std::string, ExperimentEntry> experiments_;
    std::vector<std::string> validationErrors_;
    
    static std::string trim(const std::string& str);
    static bool isValidExperimentName(const std::string& name);
    static bool isValidTypeName(const std::string& name);
};

// Default experiment implementations
// These provide basic experiment implementations that can be extended by users

// Default simulation experiment
class DefaultSimulationExperiment : public Experiment {
public:
    DefaultSimulationExperiment(const std::string& name);
    ~DefaultSimulationExperiment() override = default;
    
    std::string getName() const override;
    std::string getDescription() const override;
    ExperimentType getType() const override;
    std::string getVersion() const override;
    std::string getAuthor() const override;
    
    bool initialize(const std::map<std::string, std::any>& config) override;
    ExperimentResult run(const std::map<std::string, std::any>& config) override;
    bool stop() override;
    bool reset() override;
    ExperimentStatus getStatus() const override;
    
    bool configure(const std::string& key, const std::any& value) override;
    std::any getConfig(const std::string& key) const override;
    std::map<std::string, std::any> getAllConfig() const override;
    
    bool pause() override;
    bool resume() override;
    bool isRunning() const override;
    
    double getProgress() const override;
    std::chrono::system_clock::time_point getStartTime() const override;
    std::chrono::system_clock::time_point getEndTime() const override;
    std::string getCurrentStep() const override;
    
    ExperimentResult getLastResult() const override;
    std::map<std::string, std::any> getMetrics() const override;
    std::vector<std::string> getLogMessages() const override;
    
private:
    std::string name_;
    ExperimentStatus status_;
    std::map<std::string, std::any> config_;
    ExperimentResult lastResult_;
    std::chrono::system_clock::time_point startTime_;
    std::chrono::system_clock::time_point endTime_;
    std::vector<std::string> logMessages_;
};

// Default integration experiment
class DefaultIntegrationExperiment : public Experiment {
public:
    DefaultIntegrationExperiment(const std::string& name);
    ~DefaultIntegrationExperiment() override = default;
    
    std::string getName() const override;
    std::string getDescription() const override;
    ExperimentType getType() const override;
    std::string getVersion() const override;
    std::string getAuthor() const override;
    
    bool initialize(const std::map<std::string, std::any>& config) override;
    ExperimentResult run(const std::map<std::string, std::any>& config) override;
    bool stop() override;
    bool reset() override;
    ExperimentStatus getStatus() const override;
    
    bool configure(const std::string& key, const std::any& value) override;
    std::any getConfig(const std::string& key) const override;
    std::map<std::string, std::any> getAllConfig() const override;
    
    bool pause() override;
    bool resume() override;
    bool isRunning() const override;
    
    double getProgress() const override;
    std::chrono::system_clock::time_point getStartTime() const override;
    std::chrono::system_clock::time_point getEndTime() const override;
    std::string getCurrentStep() const override;
    
    ExperimentResult getLastResult() const override;
    std::map<std::string, std::any> getMetrics() const override;
    std::vector<std::string> getLogMessages() const override;
    
private:
    std::string name_;
    ExperimentStatus status_;
    std::map<std::string, std::any> config_;
    ExperimentResult lastResult_;
    std::chrono::system_clock::time_point startTime_;
    std::chrono::system_clock::time_point endTime_;
    std::vector<std::string> logMessages_;
    std::map<std::string, std::any> metrics_;
};

// Experiment factory functions
inline std::unique_ptr<ExperimentFactory> createDefaultExperimentFactory() {
    return std::make_unique<FileExperimentLoader>();
}

// Experiment type metadata
namespace experiment_types {
    std::map<std::string, std::string> getSimulationTypeMetadata();
    std::map<std::string, std::string> getIntegrationTypeMetadata();
    std::map<std::string, std::string> getLearningTypeMetadata();
    std::map<std::string, std::string> getDevelopmentTypeMetadata();
    std::map<std::string, std::string> getCognitiveTypeMetadata();
    std::map<std::string, std::string> getPerformanceTypeMetadata();
}

} // namespace experiment
} // namespace nlm

#endif // NLM_EXPERIMENT_FACTORY_HPP