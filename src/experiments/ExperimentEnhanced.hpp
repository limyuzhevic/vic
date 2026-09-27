#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <variant>
#include <unordered_map>

namespace nlm {

// Configuration parameter types for experiments
using ExperimentParameter = std::variant<
    int64_t,
    double,
    float,
    bool,
    std::string
>;

// Configuration parameter definition
struct ParameterDefinition {
    std::string name;
    ExperimentParameter defaultValue;
    std::string type;
    std::string description;
    bool required = false;
    std::vector<ExperimentParameter> allowedValues;
};

// Enhanced Experiment: Records data from experimental runs with numerical parameters
class Experiment {
public:
    Experiment();
    Experiment(const std::string& name);
    ~Experiment();
    
    // Disable copying, enable moving
    Experiment(const Experiment&) = delete;
    Experiment& operator=(const Experiment&) = delete;
    Experiment(Experiment&&) noexcept;
    Experiment& operator=(Experiment&&) noexcept;
    
    // Experiment metadata
    const std::string& getName() const;
    void setName(const std::string& name);
    
    // Random seed
    uint64_t getSeed() const;
    void setSeed(uint64_t seed);
    
    // Configuration - string and numerical parameters
    void setConfigValue(const std::string& key, const std::string& value);
    std::string getConfigValue(const std::string& key) const;
    
    // Numerical parameter support
    template<typename T>
    void setParameter(const std::string& key, T value);
    
    template<typename T>
    T getParameter(const std::string& key) const;
    
    bool hasParameter(const std::string& key) const;
    
    // Parameter definitions and validation
    void addParameterDefinition(const ParameterDefinition& param);
    const ParameterDefinition* getParameterDefinition(const std::string& key) const;
    bool validateParameters() const;
    
    // Metrics
    void recordMetric(const std::string& name, double value);
    void recordMetric(const std::string& name, float value);
    void recordMetric(const std::string& name, int value);
    void recordMetric(const std::string& name, int64_t value);
    
    // Get all metrics
    const std::vector<std::pair<std::string, double>>& getMetrics() const;
    
    // Get metric value
    double getMetric(const std::string& name) const;
    
    // Check if metric exists
    bool hasMetric(const std::string& name) const;
    
    // Recording step data
    void recordStep(SimulationStep step, size_t neuronCount, size_t synapseCount,
                   size_t firingCount, float avgFiringRate);
    
    // Get step data
    struct StepData {
        SimulationStep step;
        size_t neuronCount;
        size_t synapseCount;
        size_t firingCount;
        float avgFiringRate;
    };
    const std::vector<StepData>& getStepData() const;
    
    // Duration
    SimulationStep getStartStep() const;
    void setStartStep(SimulationStep step);
    SimulationStep getEndStep() const;
    void setEndStep(SimulationStep step);
    
    // Notes
    void addNote(const std::string& note);
    const std::vector<std::string>& getNotes() const;
    
    // Experiment metadata
    const std::string& getDescription() const;
    void setDescription(const std::string& description);
    
    const std::string& getVersion() const;
    void setVersion(const std::string& version);
    
    // Statistics
    size_t getStepCount() const;
    size_t getMetricCount() const;
    double getMeanMetric(const std::string& name) const;
    double getStdDevMetric(const std::string& name) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Experiment type identifier for factory pattern
using ExperimentType = std::string;

// Experiment factory interface
class ExperimentFactory {
public:
    virtual ~ExperimentFactory() = default;
    virtual std::unique_ptr<Experiment> create(const std::string& name) = 0;
    virtual ExperimentType getType() const = 0;
    virtual const std::string& getDescription() const = 0;
    virtual bool validateConfig(const std::map<std::string, ExperimentParameter>& config) const = 0;
};

// Experiment catalog for managing experiment types
class ExperimentCatalog {
public:
    ExperimentCatalog();
    ~ExperimentCatalog();
    
    // Register a new experiment type
    void registerFactory(std::unique_ptr<ExperimentFactory> factory);
    
    // Get experiment type from name
    bool hasType(const std::string& type) const;
    std::unique_ptr<Experiment> createExperiment(const std::string& type, const std::string& name = "");
    
    // Get all registered experiment types
    std::vector<ExperimentType> getExperimentTypes() const;
    const std::string& getExperimentDescription(const std::string& type) const;
    
    // Configuration validation
    bool validateConfig(const std::string& type, const std::map<std::string, ExperimentParameter>& config) const;
    
    // Get default configuration for experiment type
    std::map<std::string, ExperimentParameter> getDefaultConfig(const std::string& type) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Predefined experiment types
namespace ExperimentTypes {
    const std::string PHASE6_INTEGRATED = "phase6_integrated";
    const std::string PHASE5_LIFELONG = "phase5_lifelong";
    const std::string PHASE4_COGNITION = "phase4_cognition";
    const std::string PHASE3_LEARNING = "phase3_learning";
    const std::string SCALING_BENCHMARK = "scaling_benchmark";
    const std::string ABLATION_STUDY = "ablation_study";
    const std::string CUSTOM = "custom";
}

} // namespace nlm
