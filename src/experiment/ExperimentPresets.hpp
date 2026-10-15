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
#include "ExperimentTemplates.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

// Preset category enumeration
enum class PresetCategory {
    Default,
    QuickStart,
    Advanced,
    Custom,
    Research,
    Production,
    Development,
    Performance,
    Optimization,
    Validation,
    Monitoring,
    Analysis
};

// Preset metadata
struct PresetMetadata {
    std::string name;
    PresetCategory category;
    std::string description;
    std::string author;
    std::string version;
    std::chrono::system_clock::time_point created;
    std::chrono::system_clock::time_point lastModified;
    std::vector<std::string> tags;
    std::map<std::string, std::string> metadata;
    
    PresetMetadata() : created(std::chrono::system_clock::now()), 
                      lastModified(std::chrono::system_clock::now()) {}
    PresetMetadata(const std::string& name, PresetCategory category, const std::string& desc)
        : name(name), category(category), description(desc),
          created(std::chrono::system_clock::now()), lastModified(std::chrono::system_clock::now()) {}
};

// Experiment preset
struct ExperimentPreset {
    std::string name;
    PresetCategory category;
    std::string description;
    Config config;
    std::vector<std::string> templateDependencies;
    std::map<std::string, std::string> presetParameters;
    bool isBuiltIn;
    bool isActive;
    std::chrono::system_clock::time_point creationTime;
    
    ExperimentPreset() : isBuiltIn(false), isActive(true), creationTime(std::chrono::system_clock::now()) {}
    ExperimentPreset(const std::string& name, const Config& config)
        : name(name), config(config), isBuiltIn(false), isActive(true), creationTime(std::chrono::system_clock::now()) {}
};

// Preset validation result
struct PresetValidationResult {
    bool isValid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::chrono::system_clock::time_point validationTime;
    
    PresetValidationResult() : isValid(true), validationTime(std::chrono::system_clock::now()) {}
    PresetValidationResult(bool valid, const std::vector<std::string>& err, const std::vector<std::string>& warn)
        : isValid(valid), errors(err), warnings(warn), validationTime(std::chrono::system_clock::now()) {}
};

class ExperimentPresets {
public:
    ExperimentPresets();
    ~ExperimentPresets();
    
    // Disable copying, enable moving
    ExperimentPresets(const ExperimentPresets&) = delete;
    ExperimentPresets& operator=(const ExperimentPresets&) = delete;
    ExperimentPresets(ExperimentPresets&&) noexcept;
    ExperimentPresets& operator=(ExperimentPresets&&) noexcept;
    
    // Built-in preset management
    static ExperimentPreset getDefaultPreset();
    static ExperimentPreset getQuickStartPreset();
    static ExperimentPreset getAdvancedPreset();
    static ExperimentPreset getResearchPreset();
    static ExperimentPreset getProductionPreset();
    static ExperimentPreset getDevelopmentPreset();
    static ExperimentPreset getPerformancePreset();
    static ExperimentPreset getOptimizationPreset();
    static ExperimentPreset getValidationPreset();
    static ExperimentPreset getMonitoringPreset();
    static ExperimentPreset getAnalysisPreset();
    
    // Preset library management
    bool createPreset(const std::string& name, const Config& config, 
                     PresetCategory category = PresetCategory::Custom);
    bool createPreset(const std::string& name, const ExperimentPreset& presetObj);
    bool deletePreset(const std::string& name);
    bool updatePreset(const std::string& name, const Config& config);
    bool updatePreset(const std::string& name, const ExperimentPreset& presetObj);
    
    // Preset retrieval
    ExperimentPreset getPreset(const std::string& name) const;
    Config getPresetConfig(const std::string& name) const;
    PresetMetadata getPresetMetadata(const std::string& name) const;
    std::vector<std::string> getPresetNames() const;
    std::vector<std::string> getPresetNames(PresetCategory category) const;
    std::vector<PresetCategory> getAvailableCategories() const;
    
    // Preset validation
    PresetValidationResult validatePreset(const std::string& name) const;
    PresetValidationResult validatePresetConfig(const Config& config) const;
    bool isValidPresetName(const std::string& name) const;
    bool isPresetBuiltIn(const std::string& name) const;
    
    // Preset activation
    bool activatePreset(const std::string& name);
    bool deactivatePreset(const std::string& name);
    bool togglePreset(const std::string& name);
    bool isPresetActive(const std::string& name) const;
    std::vector<std::string> getActivePresets() const;
    
    // Preset usage
    bool applyPreset(const std::string& presetName);
    bool applyPresetToExperiment(const std::string& presetName, const std::string& experimentName);
    std::vector<std::string> getPresetDependencies(const std::string& name) const;
    bool addPresetDependency(const std::string& name, const std::string& dependentPresetName);
    bool removePresetDependency(const std::string& name, const std::string& dependentPresetName);
    
    // Preset metadata management
    void setPresetMetadata(const std::string& name, const PresetMetadata& metadata);
    PresetMetadata getPresetMetadata(const std::string& name) const;
    void updatePresetMetadata(const std::string& name, const PresetMetadata& metadata);
    void addPresetTag(const std::string& name, const std::string& tag);
    void removePresetTag(const std::string& name, const std::string& tag);
    std::vector<std::string> getPresetTags(const std::string& name) const;
    std::vector<std::string> searchPresetsByTag(const std::string& tag) const;
    
    // Preset recommendations
    ExperimentPreset recommendPreset(const Config& config) const;
    ExperimentPreset recommendPreset(const std::string& useCase) const;
    std::vector<ExperimentPreset> getPresetRecommendations(const Config& config) const;
    
    // Preset analysis
    std::map<std::string, double> analyzePresetUsage(const std::string& name) const;
    std::vector<std::string> getSimilarPresets(const std::string& name) const;
    double calculatePresetSimilarity(const std::string& name1, const std::string& name2) const;
    
    // Preset export and import
    bool exportPreset(const std::string& name, const std::string& filepath) const;
    bool exportAllPresets(const std::string& directoryPath) const;
    bool importPreset(const std::string& filepath);
    bool importPresetsFromDirectory(const std::string& directoryPath);
    
    // Preset statistics
    size_t getPresetCount() const;
    size_t getBuiltInPresetCount() const;
    size_t getCustomPresetCount() const;
    size_t getPresetCount(PresetCategory category) const;
    std::chrono::system_clock::time_point getPresetCreationTime(const std::string& name) const;
    std::chrono::system_clock::time_point getPresetLastModified(const std::string& name) const;
    
    // Preset cleanup
    bool cleanupUnusedPresets();
    bool cleanupExpiredPresets(const std::chrono::system_clock::time_point& cutoffTime);
    bool resetPreset(const std::string& name);
    bool clearAllPresets();
    
    // Preset utilities
    static std::string generatePresetName(const std::string& prefix = "preset") const;
    static bool isValidPresetName(const std::string& name, bool checkBuiltIn = true);
    static PresetCategory stringToCategory(const std::string& categoryStr) const;
    static std::string categoryToString(PresetCategory category) const;
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm