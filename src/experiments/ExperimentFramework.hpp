// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
// Enhanced version with improved CLI, modular experiments, and performance monitoring
// Modular Experiment Framework

#ifndef EXPERIMENT_FRAMEWORK_HPP
#define EXPERIMENT_FRAMEWORK_HPP

#include "experiments/Experiment.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "experiments/Metrics.hpp"
#include "brain/Brain.hpp"
#include "environment/Environment.hpp"
#include <memory>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <chrono>
#include <fstream>
#include <sstream>

namespace nlm {

// Experiment result structure
struct ExperimentResult {
    std::string name;
    bool success;
    double executionTime;
    std::map<std::string, std::string> metrics;
    std::string output;
    std::map<std::string, std::string> configuration;
    std::string errorMessage;
    
    ExperimentResult(const std::string& n) : name(n), success(false), executionTime(0.0) {}
};

// Experiment metadata
struct ExperimentInfo {
    std::string name;
    std::string description;
    std::string category;
    std::function<bool(const std::string&)> condition;
    std::vector<std::string> dependencies;
    int priority;
    
    ExperimentInfo(const std::string& n, const std::string& desc, 
                   const std::string& cat = "",
                   const std::function<bool(const std::string&)>& cond = nullptr,
                   const std::vector<std::string>& deps = {},
                   int prio = 0)
        : name(n), description(desc), category(cat), condition(cond),
          dependencies(deps), priority(prio) {}
};

// Experiment factory function type
using ExperimentFactory = std::function<std::shared_ptr<Experiment>(const std::string&)>;

// Modular Experiment Framework
class ExperimentFramework {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
public:
    ExperimentFramework();
    ~ExperimentFramework();
    
    // Register an experiment factory
    void registerExperiment(const std::string& name,
                          const ExperimentFactory& factory,
                          const ExperimentInfo& info);
    
    // Register built-in experiments
    void registerBuiltInExperiments();
    
    // Get experiment information
    std::optional<ExperimentInfo> getExperimentInfo(const std::string& name) const;
    
    // Get all available experiment names
    std::vector<std::string> getAvailableExperiments() const;
    
    // Get experiments by category
    std::vector<std::string> getExperimentsByCategory(const std::string& category) const;
    
    // Check if experiment can be run (condition check)
    bool canRunExperiment(const std::string& name) const;
    
    // Validate dependencies for an experiment
    bool validateDependencies(const std::string& name) const;
    
    // Select experiments based on configuration
    std::vector<std::string> selectExperiments(const std::map<std::string, std::string>& config,
                                             const std::vector<std::string>& requestedExperiments) const;
    
    // Create experiment instance
    std::shared_ptr<Experiment> createExperiment(const std::string& name) const;
    
    // Run a single experiment
    ExperimentResult runExperiment(const std::string& name,
                                  std::shared_ptr<Brain> brain,
                                  std::shared_ptr<Environment> environment,
                                  const std::map<std::string, std::string>& config);
    
    // Run a batch of experiments
    std::vector<ExperimentResult> runBatch(const std::vector<std::string>& experimentNames,
                                          std::shared_ptr<Brain> brainTemplate,
                                          std::shared_ptr<Environment> environmentTemplate,
                                          const std::map<std::string, std::string>& config);
    
    // Save experiment results
    bool saveResults(const std::vector<ExperimentResult>& results,
                    const std::string& outputDir,
                    const std::string& format = "json") const;
    
    // Load experiment configuration
    static std::map<std::string, std::string> loadExperimentConfig(const std::string& configFile);
    
    // Generate experiment report
    std::string generateReport(const std::vector<ExperimentResult>& results,
                              const std::string& format = "text") const;
    
    // Get experiment statistics
    std::string getStatistics(const std::vector<ExperimentResult>& results) const;
    
    // Clear all experiments
    void clear();
    
    // Get total experiment count
    size_t getExperimentCount() const;
    
    // Set validation level
    void setValidationLevel(int level);
    
    // Enable/disable experiment logging
    void enableLogging(bool enable);
    
    // Set experiment timeout
    void setTimeout(double timeout);
    
    // Get framework version
    static const char* version() { return "2.0.0"; }
    
    // Get framework description
    static const char* description() { return "Enhanced Experiment Framework for NLM Phase 2"; }
    
    // Print available experiments
    void printAvailableExperiments() const;
    
    // Print experiment information
    void printExperimentInfo(const std::string& name) const;
    
    // Validate configuration
    bool validateConfig(const std::map<std::string, std::string>& config) const;
    
    // Generate default configuration
    static std::map<std::string, std::string> getDefaultConfig();
};

// Convenience functions
inline std::shared_ptr<ExperimentFramework> createExperimentFramework() {
    return std::make_shared<ExperimentFramework>();
}

// Helper utilities
namespace experiment_utils {
    // Convert string to lowercase
    std::string toLower(const std::string& str);
    
    // Trim whitespace from string
    std::string trim(const std::string& str);
    
    // Split string by delimiter
    std::vector<std::string> split(const std::string& str, char delimiter = ',');
    
    // Join vector of strings
    std::string join(const std::vector<std::string>& strings, const std::string& delimiter = ", ");
    
    // Parse comma-separated list with validation
    std::vector<std::string> parseExperimentList(const std::string& input,
                                                const std::vector<std::string>& validExperiments);
    
    // Format duration for display
    std::string formatDuration(double seconds);
    
    // Generate timestamp string
    std::string timestamp();
    
    // Check if value can be converted to target type
    template<typename T>
    bool canConvert(const std::string& str);
    
    // Convert string to target type
    template<typename T>
    std::optional<T> convertTo(const std::string& str);
    
    // Validate experiment name
    bool isValidExperimentName(const std::string& name);
    
    // Generate unique experiment ID
    std::string generateExperimentId(const std::string& prefix = "exp");
}

// Built-in experiment implementations
namespace built_in_experiments {
    std::shared_ptr<Experiment> createBasicConnectivityExperiment(const std::string& config);
    std::shared_ptr<Experiment> createPlasticityExperiment(const std::string& config);
    std::shared_ptr<Experiment> createStdpExperiment(const std::string& config);
    std::shared_ptr<Experiment> createLearningExperiment(const std::string& config);
    std::shared_ptr<Experiment> createBenchmarkExperiment(const std::string& config);
    std::shared_ptr<Experiment> createAblationExperiment(const std::string& config);
    std::shared_ptr<Experiment> createScalingExperiment(const std::string& config);
}

// Experiment result utilities
namespace result_utils {
    // Calculate success rate
    double calculateSuccessRate(const std::vector<ExperimentResult>& results);
    
    // Get average execution time
    double getAverageExecutionTime(const std::vector<ExperimentResult>& results);
    
    // Get fastest experiment
    std::string getFastestExperiment(const std::vector<ExperimentResult>& results);
    
    // Get slowest experiment
    std::string getSlowestExperiment(const std::vector<ExperimentResult>& results);
    
    // Find experiment by name
    const ExperimentResult* findExperiment(const std::vector<ExperimentResult>& results,
                                          const std::string& name);
    
    // Filter experiments by category
    std::vector<ExperimentResult> filterByCategory(const std::vector<ExperimentResult>& results,
                                                  const std::string& category);
    
    // Export results to CSV
    bool exportToCSV(const std::vector<ExperimentResult>& results,
                    const std::string& filepath);
    
    // Export results to JSON
    bool exportToJSON(const std::vector<ExperimentResult>& results,
                     const std::string& filepath);
}

} // namespace nlm

#endif // EXPERIMENT_FRAMEWORK_HPP

// Implementation file (experiment_framework.cpp)
// This would be in a separate .cpp file to keep the header clean