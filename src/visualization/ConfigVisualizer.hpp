#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <chrono>
#include <functional>
#include <iostream>
#include <fstream>
#include <map>
#include <iomanip>

namespace nlm {

/**
 * Configuration visualization system for Phase 6
 * 
 * Provides comprehensive visualization of system configuration,
 * parameter impacts, default values, and inheritance hierarchy.
 */
class ConfigVisualizer {
public:
    /**
     * Structure representing a configuration parameter
     */
    struct ConfigParameter {
        std::string name;
        std::string type; // "int", "double", "string", "bool"
        std::string value; // String representation of value
        std::string defaultValue; // Default value
        std::string source; // "default", "config_file", "command_line", "runtime"
        std::string description; // Human-readable description
        bool isOverridden; // Whether the value was overridden
        std::vector<std::string> dependencies; // Parameters this depends on
        std::vector<std::string> dependents; // Parameters that depend on this
        double impactScore; // Impact on system performance (0-1)
        
        ConfigParameter() : isOverridden(false), impactScore(0.0) {}
    };
    
    /**
     * Structure representing configuration inheritance
     */
    struct InheritanceNode {
        std::string name;
        std::string type; // "section", "parameter", "system"
        std::string value; // Current value
        std::string defaultValue; // Default value
        std::string source; // Where value comes from
        std::vector<InheritanceNode> children; // Sub-sections or dependent parameters
        int inheritanceLevel; // Depth in hierarchy
        bool isOverridden; // Whether value was overridden
        
        InheritanceNode() : inheritanceLevel(0), isOverridden(false) {}
    };
    
    /**
     * Structure representing parameter impact analysis
     */
    struct ParameterImpact {
        std::string parameterName;
        double currentValue;
        double defaultValue;
        double impactOnPerformance; // 0-1 impact on performance
        double impactOnStability; // 0-1 impact on stability
        double impactOnAccuracy; // 0-1 impact on accuracy
        std::vector<std::string> affectedComponents; // Components affected
        std::string impactType; // "positive", "negative", "neutral"
        
        ParameterImpact() : currentValue(0.0), defaultValue(0.0), impactOnPerformance(0.0),
                           impactOnStability(0.0), impactOnAccuracy(0.0) {}
    };
    
    /**
     * Structure representing default values
     */
    struct DefaultValueEntry {
        std::string parameterName;
        std::string parameterType;
        std::string defaultValue;
        std::string description;
        std::string category; // "performance", "stability", "accuracy", "behavior"
        
        DefaultValueEntry() {}
    };
    
    /**
     * Configuration visualization options
     */
    struct ConfigOptions {
        bool showFullHierarchy; // Show entire configuration hierarchy
        bool showOnlyOverrides; // Show only overridden parameters
        bool showDependencies; // Show parameter dependencies
        bool showInheritance; // Show inheritance relationships
        bool showImpactAnalysis; // Show parameter impact analysis
        std::string visualizationType; // "tree", "flat", "grouped"
        std::string theme; // "light", "dark", "high-contrast"
        bool exportToFile; // Export configuration to file
        std::string exportFormat; // "text", "json", "xml"
        bool showDetailedInfo; // Show detailed parameter information
    };
    
    /**
     * Configuration comparison options
     */
    struct ComparisonOptions {
        std::string baselineFile; // Baseline configuration file
        std::string comparisonFile; // Configuration file to compare
        std::string comparisonType; // "values", "impact", "performance"
        bool showDifferences; // Show differences
        bool showSimilarities; // Show similarities
        bool showImpactDifferences; // Show impact differences
    };
    
public:
    ConfigVisualizer();
    ~ConfigVisualizer();
    
    /**
     * Initialize the configuration visualizer
     */
    bool initialize();
    
    /**
     * Update configuration from config object
     */
    void update(std::shared_ptr<class Config> config);
    
    /**
     * Render configuration visualizer
     */
    void renderConfig(const ConfigOptions& options) const;
    
    /**
     * Render parameter inheritance tree
     */
    void renderInheritanceTree() const;
    
    /**
     * Render parameter impact analysis
     */
    void renderParameterImpact() const;
    
    /**
     * Render default values viewer
     */
    void renderDefaultValues() const;
    
    /**
     * Render parameter dependencies
     */
    void renderParameterDependencies() const;
    
    /**
     * Generate configuration report
     */
    void generateConfigReport(const std::string& filename) const;
    
    /**
     * Generate configuration comparison
     */
    void generateConfigComparison(const ComparisonOptions& options) const;
    
    /**
     * Export configuration
     */
    bool exportConfig(const std::string& filename, const std::string& format = "text") const;
    
    /**
     * Get current configuration parameters
     */
    std::vector<ConfigParameter> getConfigParameters() const { return currentConfigParameters_; }
    
    /**
     * Get inheritance hierarchy
     */
    InheritanceNode getInheritanceHierarchy() const { return currentInheritance_; }
    
    /**
     * Get parameter impact analysis
     */
    std::vector<ParameterImpact> getParameterImpacts() const { return currentParameterImpacts_; }
    
    /**
     * Get default values
     */
    std::vector<DefaultValueEntry> getDefaultValues() const { return currentDefaultValues_; }
    
    /**
     * Set config reference
     */
    void setConfig(std::shared_ptr<class Config> config) {
        config_ = config;
    }
    
    /**
     * Is visualization active
     */
    bool isActive() const { return active_; }
    
    /**
     * Set update callback
     */
    void setUpdateCallback(std::function<void(const ConfigParameter&)> callback) {
        parameterChangeCallback_ = callback;
    }
    
    /**
     * Add custom parameter
     */
    void addParameter(const std::string& name, const std::string& type,
                     const std::string& value, const std::string& description,
                     const std::string& source = "default");
    
    /**
     * Remove parameter
     */
    void removeParameter(const std::string& name);
    
    /**
     * Update parameter value
     */
    void updateParameterValue(const std::string& name, const std::string& newValue);
    
    /**
     * Filter parameters by category
     */
    void filterByCategory(const std::vector<std::string>& categories);
    
    /**
     * Filter parameters by source
     */
    void filterBySource(const std::vector<std::string>& sources);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    bool active_;
    double updateRate_;
    
    // Configuration data
    std::vector<ConfigParameter> currentConfigParameters_;
    InheritanceNode currentInheritance_;
    std::vector<ParameterImpact> currentParameterImpacts_;
    std::vector<DefaultValueEntry> currentDefaultValues_;
    
    // Callbacks and references
    std::function<void(const ConfigParameter&)> parameterChangeCallback_;
    std::shared_ptr<class Config> config_;
    
    // Helper methods
    void collectConfigParameters(std::shared_ptr<class Config> config);
    void buildInheritanceTree();
    void analyzeParameterImpacts();
    void collectDefaultValues();
    void updateDependencyRelationships();
    void calculateParameterImpact(const ConfigParameter& param, ParameterImpact& impact);
    InheritanceNode buildSectionTree(const std::vector<ConfigParameter>& parameters);
    std::string getTypeDisplayName(const std::string& type) const;
    std::string getSourceColor(const std::string& source) const;
    std::string getImpactColor(double impact) const;
};

} // namespace nlm
