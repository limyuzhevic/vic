#include "ExperimentPresets.hpp"

namespace nlm {

ExperimentPresets::ExperimentPresets() {
    // Constructor initialization
}

ExperimentPresets::~ExperimentPresets() {
    // Cleanup resources
}

ExperimentPresets::ExperimentPresets(ExperimentPresets&& other) noexcept {
    // Move constructor implementation
}

ExperimentPresets& ExperimentPresets::operator=(ExperimentPresets&& other) noexcept {
    // Move assignment operator implementation
    return *this;
}

// Built-in preset implementations
ExperimentPreset ExperimentPresets::getDefaultPreset() {
    Config config;
    config.set("experiment.name", std::string("Default Experiment"));
    config.set("experiment.maxSteps", 1000);
    config.set("experiment.neuronCount", 1000);
    config.set("experiment.regionCount", 2);
    config.set("experiment.enableNeuromodulation", true);
    config.set("experiment.enableMemory", true);
    config.set("experiment.enableDevelopment", true);
    
    ExperimentPreset presetObj("DefaultPreset", config);
    presetObj.category = PresetCategory::Default;
    presetObj.description = "Default experiment preset";
    presetObj.isBuiltIn = true;
    presetObj.presetParameters["preset_type"] = "default";
    
    return presetObj;
}

ExperimentPreset ExperimentPresets::getQuickStartPreset() {
    Config config;
    config.set("experiment.name", std::string("Quick Start Experiment"));
    config.set("experiment.maxSteps", 100);
    config.set("experiment.neuronCount", 100);
    config.set("experiment.regionCount", 1);
    config.set("experiment.enableNeuromodulation", false);
    config.set("experiment.enableMemory", false);
    config.set("experiment.enableDevelopment", false);
    config.set("experiment.enableLearning", true);
    
    ExperimentPreset presetObj("QuickStartPreset", config);
    presetObj.category = PresetCategory::QuickStart;
    presetObj.description = "Quick start experiment preset for beginners";
    presetObj.isBuiltIn = true;
    presetObj.presetParameters["preset_type"] = "quick_start";
    
    return presetObj;
}

ExperimentPreset ExperimentPresets::getAdvancedPreset() {
    Config config;
    config.set("experiment.name", std::string("Advanced Experiment"));
    config.set("experiment.maxSteps", 5000);
    config.set("experiment.neuronCount", 5000);
    config.set("experiment.regionCount", 4);
    config.set("experiment.enableNeuromodulation", true);
    config.set("experiment.enableMemory", true);
    config.set("experiment.enableDevelopment", true);
    config.set("experiment.enableOptimization", true);
    config.set("experiment.enableValidation", true);
    
    ExperimentPreset presetObj("AdvancedPreset", config);
    presetObj.category = PresetCategory::Advanced;
    presetObj.description = "Advanced experiment preset with all features enabled";
    presetObj.isBuiltIn = true;
    presetObj.presetParameters["preset_type"] = "advanced";
    
    return presetObj;
}

ExperimentPreset ExperimentPresets::getResearchPreset() {
    Config config;
    config.set("experiment.name", std::string("Research Experiment"));
    config.set("experiment.maxSteps", 2000);
    config.set("experiment.neuronCount", 2000);
    config.set("experiment.regionCount", 3);
    config.set("experiment.enableNeuromodulation", true);
    config.set("experiment.enableMemory", true);
    config.set("experiment.enableDevelopment", true);
    config.set("experiment.enableAnalysis", true);
    config.set("experiment.research_focus", "neuroscience");
    
    ExperimentPreset presetObj("ResearchPreset", config);
    presetObj.category = PresetCategory::Research;
    presetObj.description = "Research experiment preset for scientific studies";
    presetObj.isBuiltIn = true;
    presetObj.presetParameters["preset_type"] = "research";
    
    return presetObj;
}

ExperimentPreset ExperimentPresets::getProductionPreset() {
    Config config;
    config.set("experiment.name", std::string("Production Experiment"));
    config.set("experiment.maxSteps", 10000);
    config.set("experiment.neuronCount", 10000);
    config.set("experiment.regionCount", 8);
    config.set("experiment.enableNeuromodulation", true);
    config.set("experiment.enableMemory", true);
    config.set("experiment.enableDevelopment", false);
    config.set("experiment.enableMonitoring", true);
    config.set("experiment.enableOptimization", true);
    config.set("experiment.performance_target", "production");
    
    ExperimentPreset presetObj("ProductionPreset", config);
    presetObj.category = PresetCategory::Production;
    presetObj.description = "Production experiment preset for industrial applications";
    presetObj.isBuiltIn = true;
    presetObj.presetParameters["preset_type"] = "production";
    
    return presetObj;
}

ExperimentPreset ExperimentPresets::getDevelopmentPreset() {
    Config config;
    config.set("experiment.name", std::string("Development Experiment"));
    config.set("experiment.maxSteps", 500);
    config.set("experiment.neuronCount", 500);
    config.set("experiment.regionCount", 2);
    config.set("experiment.enableNeuromodulation", false);
    config.set("experiment.enableMemory", false);
    config.set("experiment.enableDevelopment", true);
    config.set("experiment.enableLearning", true);
    config.set("experiment.validation_type", "rapid");
    
    ExperimentPreset presetObj("DevelopmentPreset", config);
    presetObj.category = PresetCategory::Development;
    presetObj.description = "Development experiment preset for rapid prototyping";
    presetObj.isBuiltIn = true;
    presetObj.presetParameters["preset_type"] = "development";
    
    return presetObj;
}

ExperimentPreset ExperimentPresets::getPerformancePreset() {
    Config config;
    config.set("experiment.name", std::string("Performance Experiment"));
    config.set("experiment.maxSteps", 3000);
    config.set("experiment.neuronCount", 3000);
    config.set("experiment.regionCount", 3);
    config.set("experiment.enableNeuromodulation", true);
    config.set("experiment.enableMemory", true);
    config.set("experiment.enableDevelopment", false);
    config.set("experiment.enableMonitoring", true);
    config.set("experiment.performance_target", "high_throughput");
    
    ExperimentPreset presetObj("PerformancePreset", config);
    presetObj.category = PresetCategory::Performance;
    presetObj.description = "Performance experiment preset for high-throughput applications";
    presetObj.isBuiltIn = true;
    presetObj.presetParameters["preset_type"] = "performance";
    
    return presetObj;
}

ExperimentPreset ExperimentPresets::getOptimizationPreset() {
    Config config;
    config.set("experiment.name", std::string("Optimization Experiment"));
    config.set("experiment.maxSteps", 1000);
    config.set("experiment.neuronCount", 1000);
    config.set("experiment.regionCount", 2);
    config.set("experiment.enableNeuromodulation", true);
    config.set("experiment.enableMemory", false);
    config.set("experiment.enableDevelopment", false);
    config.set("experiment.enableOptimization", true);
    config.set("experiment.optimization_method", "bayesian");
    
    ExperimentPreset presetObj("OptimizationPreset", config);
    presetObj.category = PresetCategory::Optimization;
    presetObj.description = "Optimization experiment preset for parameter optimization";
    presetObj.isBuiltIn = true;
    presetObj.presetParameters["preset_type"] = "optimization";
    
    return presetObj;
}

ExperimentPreset ExperimentPresets::getValidationPreset() {
    Config config;
    config.set("experiment.name", std::string("Validation Experiment"));
    config.set("experiment.maxSteps", 200);
    config.set("experiment.neuronCount", 200);
    config.set("experiment.regionCount", 1);
    config.set("experiment.enableNeuromodulation", false);
    config.set("experiment.enableMemory", false);
    config.set("experiment.enableDevelopment", false);
    config.set("experiment.enableValidation", true);
    config.set("experiment.validation_type", "statistical");
    
    ExperimentPreset presetObj("ValidationPreset", config);
    presetObj.category = PresetCategory::Validation;
    presetObj.description = "Validation experiment preset for statistical validation";
    presetObj.isBuiltIn = true;
    presetObj.presetParameters["preset_type"] = "validation";
    
    return presetObj;
}

ExperimentPreset ExperimentPresets::getMonitoringPreset() {
    Config config;
    config.set("experiment.name", std::string("Monitoring Experiment"));
    config.set("experiment.maxSteps", 1500);
    config.set("experiment.neuronCount", 1500);
    config.set("experiment.regionCount", 2);
    config.set("experiment.enableNeuromodulation", true);
    config.set("experiment.enableMemory", true);
    config.set("experiment.enableDevelopment", false);
    config.set("experiment.enableMonitoring", true);
    config.set("experiment.monitoring_interval", 0.5);
    
    ExperimentPreset presetObj("MonitoringPreset", config);
    presetObj.category = PresetCategory::Monitoring;
    presetObj.description = "Monitoring experiment preset for real-time monitoring";
    presetObj.isBuiltIn = true;
    presetObj.presetParameters["preset_type"] = "monitoring";
    
    return presetObj;
}

ExperimentPreset ExperimentPresets::getAnalysisPreset() {
    Config config;
    config.set("experiment.name", std::string("Analysis Experiment"));
    config.set("experiment.maxSteps", 1200);
    config.set("experiment.neuronCount", 1200);
    config.set("experiment.regionCount", 2);
    config.set("experiment.enableNeuromodulation", true);
    config.set("experiment.enableMemory", true);
    config.set("experiment.enableDevelopment", true);
    config.set("experiment.enableAnalysis", true);
    config.set("experiment.analysis_method", "machine_learning");
    
    ExperimentPreset presetObj("AnalysisPreset", config);
    presetObj.category = PresetCategory::Analysis;
    presetObj.description = "Analysis experiment preset for comprehensive analysis";
    presetObj.isBuiltIn = true;
    presetObj.presetParameters["preset_type"] = "analysis";
    
    return presetObj;
}

// ... (additional method implementations can be added as needed)

} // namespace nlm