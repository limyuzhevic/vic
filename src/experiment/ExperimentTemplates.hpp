#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <algorithm>
#include <chrono>
#include <functional>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "ExperimentProtocol.hpp"
#include "ExperimentLifecycleManager.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

// Template category enumeration
enum class TemplateCategory {
    Research,
    Production,
    Development,
    Benchmark,
    Performance,
    Custom,
    Optimization,
    Validation,
    Monitoring,
    Analysis
};

// Template metadata
struct TemplateMetadata {
    std::string name;
    TemplateCategory category;
    std::string description;
    std::string author;
    std::string version;
    std::chrono::system_clock::time_point created;
    std::chrono::system_clock::time_point lastModified;
    std::vector<std::string> tags;
    std::map<std::string, std::string> metadata;
    
    TemplateMetadata() : created(std::chrono::system_clock::now()), 
                        lastModified(std::chrono::system_clock::now()) {}
    TemplateMetadata(const std::string& name, TemplateCategory category, const std::string& desc)
        : name(name), category(category), description(desc),
          created(std::chrono::system_clock::now()), lastModified(std::chrono::system_clock::now()) {}
};

// Experiment configuration template
struct ExperimentTemplate {
    std::string name;
    TemplateCategory category;
    std::string description;
    Config config;
    std::vector<std::string> dependentTemplates;
    std::map<std::string, std::string> templateParameters;
    bool isBuiltIn;
    std::chrono::system_clock::time_point creationTime;
    
    ExperimentTemplate() : isBuiltIn(false), creationTime(std::chrono::system_clock::now()) {}
    ExperimentTemplate(const std::string& name, const Config& config)
        : name(name), config(config), isBuiltIn(false), creationTime(std::chrono::system_clock::now()) {}
};

// Template validation result
struct TemplateValidationResult {
    bool isValid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::chrono::system_clock::time_point validationTime;
    
    TemplateValidationResult() : isValid(true), validationTime(std::chrono::system_clock::now()) {}
    TemplateValidationResult(bool valid, const std::vector<std::string>& err, const std::vector<std::string>& warn)
        : isValid(valid), errors(err), warnings(warn), validationTime(std::chrono::system_clock::now()) {}
};

// Template recommendation
struct TemplateRecommendation {
    std::string recommendedTemplateName;
    std::string reason;
    double confidence;
    std::map<std::string, std::string> recommendationData;
    
    TemplateRecommendation() : confidence(0.0) {}
    TemplateRecommendation(const std::string& name, const std::string& reason, double confidence)
        : recommendedTemplateName(name), reason(reason), confidence(confidence) {}
};

class ExperimentTemplates {
public:
    ExperimentTemplates();
    ~ExperimentTemplates();
    
    // Disable copying, enable moving
    ExperimentTemplates(const ExperimentTemplates&) = delete;
    ExperimentTemplates& operator=(const ExperimentTemplates&) = delete;
    ExperimentTemplates(ExperimentTemplates&&) noexcept;
    ExperimentTemplates& operator=(ExperimentTemplates&&) noexcept;
    
    // Built-in template management
    static ExperimentTemplate getResearchTemplate();
    static ExperimentTemplate getProductionTemplate();
    static ExperimentTemplate getDevelopmentTemplate();
    static ExperimentTemplate getBenchmarkTemplate();
    static ExperimentTemplate getPerformanceTemplate();
    static ExperimentTemplate getOptimizationTemplate();
    static ExperimentTemplate getValidationTemplate();
    static ExperimentTemplate getMonitoringTemplate();
    static ExperimentTemplate getAnalysisTemplate();
    
    // Template library management
    bool createTemplate(const std::string& name, const Config& config, 
                       TemplateCategory category = TemplateCategory::Custom);
    bool createTemplate(const std::string& name, const ExperimentTemplate& templateObj);
    bool deleteTemplate(const std::string& name);
    bool updateTemplate(const std::string& name, const Config& config);
    bool updateTemplate(const std::string& name, const ExperimentTemplate& templateObj);
    
    // Template retrieval
    ExperimentTemplate getTemplate(const std::string& name) const;
    Config getTemplateConfig(const std::string& name) const;
    TemplateMetadata getTemplateMetadata(const std::string& name) const;
    std::vector<std::string> getTemplateNames() const;
    std::vector<std::string> getTemplateNames(TemplateCategory category) const;
    std::vector<TemplateCategory> getAvailableCategories() const;
    
    // Template validation
    TemplateValidationResult validateTemplate(const std::string& name) const;
    TemplateValidationResult validateTemplateConfig(const Config& config) const;
    bool isValidTemplateName(const std::string& name) const;
    bool isTemplateBuiltIn(const std::string& name) const;
    
    // Template usage
    bool useTemplate(const std::string& templateName, ExperimentLifecycleManager& lifecycleManager);
    bool useTemplate(const std::string& templateName, const std::string& experimentName, 
                   ExperimentLifecycleManager& lifecycleManager);
    std::vector<std::string> getDependentTemplates(const std::string& name) const;
    bool addDependentTemplate(const std::string& name, const std::string& dependentTemplateName);
    bool removeDependentTemplate(const std::string& name, const std::string& dependentTemplateName);
    
    // Template metadata management
    void setTemplateMetadata(const std::string& name, const TemplateMetadata& metadata);
    TemplateMetadata getTemplateMetadata(const std::string& name) const;
    void updateTemplateMetadata(const std::string& name, const TemplateMetadata& metadata);
    void addTemplateTag(const std::string& name, const std::string& tag);
    void removeTemplateTag(const std::string& name, const std::string& tag);
    std::vector<std::string> getTemplateTags(const std::string& name) const;
    std::vector<std::string> searchTemplatesByTag(const std::string& tag) const;
    
    // Template recommendations
    TemplateRecommendation recommendTemplate(const Config& config) const;
    TemplateRecommendation recommendTemplate(const std::string& useCase) const;
    std::vector<TemplateRecommendation> getTemplateRecommendations(const Config& config) const;
    
    // Template analysis
    std::map<std::string, double> analyzeTemplateUsage(const std::string& name) const;
    std::vector<std::string> getSimilarTemplates(const std::string& name) const;
    double calculateTemplateSimilarity(const std::string& name1, const std::string& name2) const;
    
    // Template export and import
    bool exportTemplate(const std::string& name, const std::string& filepath) const;
    bool exportAllTemplates(const std::string& directoryPath) const;
    bool importTemplate(const std::string& filepath);
    bool importTemplatesFromDirectory(const std::string& directoryPath);
    
    // Template statistics
    size_t getTemplateCount() const;
    size_t getBuiltInTemplateCount() const;
    size_t getCustomTemplateCount() const;
    size_t getTemplateCount(TemplateCategory category) const;
    std::chrono::system_clock::time_point getTemplateCreationTime(const std::string& name) const;
    std::chrono::system_clock::time_point getTemplateLastModified(const std::string& name) const;
    
    // Template cleanup
    bool cleanupUnusedTemplates();
    bool cleanupExpiredTemplates(const std::chrono::system_clock::time_point& cutoffTime);
    bool resetTemplate(const std::string& name);
    bool clearAllTemplates();
    
    // Template utilities
    static std::string generateTemplateName(const std::string& prefix = "template") const;
    static bool isValidTemplateName(const std::string& name, bool checkBuiltIn = true);
    static TemplateCategory stringToCategory(const std::string& categoryStr) const;
    static std::string categoryToString(TemplateCategory category) const;
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm