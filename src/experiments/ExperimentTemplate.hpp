#pragma once

#include "EnhancedExperiment.hpp"
#include "EnhancedMetrics.hpp"
#include "../core/Config/Config.hpp"
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>

namespace nlm {

// Experiment template system
class ExperimentTemplate {
public:
    ExperimentTemplate() = default;
    virtual ~ExperimentTemplate() = default;
    
    // Template metadata
    const std::string& getName() const;
    const std::string& getDescription() const;
    const std::string& getCategory() const;
    const std::string& getAuthor() const;
    const std::string& getVersion() const;
    
    // Template structure
    virtual bool configure(std::shared_ptr<EnhancedExperiment> experiment,
                          const std::unordered_map<std::string, std::string>& parameters) = 0;
    virtual bool validate(const std::shared_ptr<EnhancedExperiment>& experiment) const;
    virtual bool run(std::shared_ptr<EnhancedExperiment> experiment) = 0;
    virtual void cleanup(std::shared_ptr<EnhancedExperiment> experiment);
    
    // Template parameters
    virtual std::vector<std::string> getRequiredParameters() const;
    virtual std::vector<std::string> getOptionalParameters() const;
    virtual std::unordered_map<std::string, std::string> getDefaultParameters() const;
    virtual std::unordered_map<std::string, std::string> getParameterRanges() const;
    
    // Template metrics
    virtual std::vector<std::string> getExpectedMetrics() const;
    virtual std::vector<std::string> getPerformanceMetrics() const;
    virtual std::vector<std::string> getDiagnosticMetrics() const;
    
    // Template validation
    virtual bool canRun(const std::shared_ptr<EnhancedExperiment>& experiment) const;
    virtual std::vector<std::string> getValidationRequirements() const;
    
protected:
    std::string name;
    std::string description;
    std::string category;
    std::string author;
    std::string version;
};

// Memory Integration Experiment Template
class MemoryIntegrationTemplate : public ExperimentTemplate {
public:
    MemoryIntegrationTemplate();
    
    bool configure(std::shared_ptr<EnhancedExperiment> experiment,
                  const std::unordered_map<std::string, std::string>& parameters) override;
    bool run(std::shared_ptr<EnhancedExperiment> experiment) override;
    
    std::vector<std::string> getRequiredParameters() const override;
    std::vector<std::string> getOptionalParameters() const override;
    std::unordered_map<std::string, std::string> getDefaultParameters() const override;
    
    std::vector<std::string> getExpectedMetrics() const override;
    std::vector<std::string> getPerformanceMetrics() const override;
    std::vector<std::string> getDiagnosticMetrics() const override;
    
    std::vector<std::string> getValidationRequirements() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Prediction System Validation Template
class PredictionSystemTemplate : public ExperimentTemplate {
public:
    PredictionSystemTemplate();
    
    bool configure(std::shared_ptr<EnhancedExperiment> experiment,
                  const std::unordered_map<std::string, std::string>& parameters) override;
    bool run(std::shared_ptr<EnhancedExperiment> experiment) override;
    
    std::vector<std::string> getRequiredParameters() const override;
    std::vector<std::string> getOptionalParameters() const override;
    std::unordered_map<std::string, std::string> getDefaultParameters() const override;
    
    std::vector<std::string> getExpectedMetrics() const override;
    std::vector<std::string> getPerformanceMetrics() const override;
    std::vector<std::string> getDiagnosticMetrics() const override;
    
    std::vector<std::string> getValidationRequirements() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Cognitive System Benchmarking Template
class CognitiveBenchmarkTemplate : public ExperimentTemplate {
public:
    CognitiveBenchmarkTemplate();
    
    bool configure(std::shared_ptr<EnhancedExperiment> experiment,
                  const std::unordered_map<std::string, std::string>& parameters) override;
    bool run(std::shared_ptr<EnhancedExperiment> experiment) override;
    
    std::vector<std::string> getRequiredParameters() const override;
    std::vector<std::string> getOptionalParameters() const override;
    std::unordered_map<std::string, std::string> getDefaultParameters() const override;
    
    std::vector<std::string> getExpectedMetrics() const override;
    std::vector<std::string> getPerformanceMetrics() const override;
    std::vector<std::string> getDiagnosticMetrics() const override;
    
    std::vector<std::string> getValidationRequirements() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Neuromodulation Effect Template
class NeuromodulationTemplate : public ExperimentTemplate {
public:
    NeuromodulationTemplate();
    
    bool configure(std::shared_ptr<EnhancedExperiment> experiment,
                  const std::unordered_map<std::string, std::string>& parameters) override;
    bool run(std::shared_ptr<EnhancedExperiment> experiment) override;
    
    std::vector<std::string> getRequiredParameters() const override;
    std::vector<std::string> getOptionalParameters() const override;
    std::unordered_map<std::string, std::string> getDefaultParameters() const override;
    
    std::vector<std::string> getExpectedMetrics() const override;
    std::vector<std::string> getPerformanceMetrics() const override;
    std::vector<std::string> getDiagnosticMetrics() const override;
    
    std::vector<std::string> getValidationRequirements() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Development Progression Template
class DevelopmentTemplate : public ExperimentTemplate {
public:
    DevelopmentTemplate();
    
    bool configure(std::shared_ptr<EnhancedExperiment> experiment,
                  const std::unordered_map<std::string, std::string>& parameters) override;
    bool run(std::shared_ptr<EnhancedExperiment> experiment) override;
    
    std::vector<std::string> getRequiredParameters() const override;
    std::vector<std::string> getOptionalParameters() const override;
    std::unordered_map<std::string, std::string> getDefaultParameters() const override;
    
    std::vector<std::string> getExpectedMetrics() const override;
    std::vector<std::string> getPerformanceMetrics() const override;
    std::vector<std::string> getDiagnosticMetrics() const override;
    
    std::vector<std::string> getValidationRequirements() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Multi-Agent Interaction Template
class SocialLearningTemplate : public ExperimentTemplate {
public:
    SocialLearningTemplate();
    
    bool configure(std::shared_ptr<EnhancedExperiment> experiment,
                  const std::unordered_map<std::string, std::string>& parameters) override;
    bool run(std::shared_ptr<EnhancedExperiment> experiment) override;
    
    std::vector<std::string> getRequiredParameters() const override;
    std::vector<std::string> getOptionalParameters() const override;
    std::unordered_map<std::string, std::string> getDefaultParameters() const override;
    
    std::vector<std::string> getExpectedMetrics() const override;
    std::vector<std::string> getPerformanceMetrics() const override;
    std::vector<std::string> getDiagnosticMetrics() const override;
    
    std::vector<std::string> getValidationRequirements() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Experiment Template Factory
class ExperimentTemplateFactory {
public:
    static std::unique_ptr<ExperimentTemplate> createTemplate(const std::string& name);
    static std::vector<std::string> getAvailableTemplateNames();
    static std::unique_ptr<ExperimentTemplate> cloneTemplate(const std::string& name);
    
private:
    static std::unordered_map<std::string, std::function<std::unique_ptr<ExperimentTemplate>()>> templateRegistry;
};

} // namespace nlm
