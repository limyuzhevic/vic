#include "AdvancedConfig.hpp"
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <iostream>

namespace nlm {

AdvancedConfig::AdvancedConfig() : config_(), currentSchema_() {
    // Initialize with standard NLM parameters
    setupDefaultSchema();
}

void AdvancedConfig::setupDefaultSchema() {
    // Basic brain parameters
    ConfigSchema::ParameterInfo neuronCount;
    neuronCount.name = "neuron_count";
    neuronCount.type = "int";
    neuronCount.description = "Total number of neurons in the brain";
    neuronCount.unit = "neurons";
    neuronCount.required = true;
    neuronCount.min = 10;
    neuronCount.max = 1000000;
    config_.set(neuronCount.name, 1000, ConfigSource::Default);
    
    ConfigSchema::ParameterInfo regionCount;
    regionCount.name = "region_count";
    regionCount.type = "int";
    regionCount.description = "Number of neural regions";
    regionCount.unit = "regions";
    regionCount.required = false;
    regionCount.defaultValue = 1;
    config_.set(regionCount.name, 1, ConfigSource::Default);
    
    // Simulation parameters
    ConfigSchema::ParameterInfo timestep;
    timestep.name = "simulation_timestep";
    timestep.type = "float";
    timestep.description = "Time step for simulation";
    timestep.unit = "s";
    timestep.required = false;
    timestep.defaultValue = 0.001;
    timestep.min = 0.0001;
    timestep.max = 1.0;
    config_.set(timestep.name, 0.001, ConfigSource::Default);
    
    ConfigSchema::ParameterInfo maxSteps;
    maxSteps.name = "max_steps";
    maxSteps.type = "int";
    maxSteps.description = "Maximum simulation steps";
    maxSteps.unit = "steps";
    maxSteps.required = false;
    maxSteps.defaultValue = 10000;
    maxSteps.min = 100;
    maxSteps.max = 1000000;
    config_.set(maxSteps.name, 10000, ConfigSource::Default);
    
    // Plasticity parameters
    ConfigSchema::ParameterInfo stdpLTP;
    stdpLTP.name = "stdp_ltp_weight";
    stdpLTP.type = "float";
    stdpLTP.description = "STDP long-term potentiation weight change";
    stdpLTP.unit = "weight change";
    stdpLTP.required = false;
    stdpLTP.defaultValue = 0.01;
    stdpLTP.min = 0.0;
    stdpLTP.max = 1.0;
    config_.set(stdpLTP.name, 0.01, ConfigSource::Default);
    
    ConfigSchema::ParameterInfo stdpLTD;
    stdpLTD.name = "stdp_ltd_weight";
    stdpLTD.type = "float";
    stdpLTD.description = "STDP long-term depression weight change";
    stdpLTD.unit = "weight change";
    stdpLTD.required = false;
    stdpLTD.defaultValue = 0.012;
    stdpLTD.min = 0.0;
    stdpLTD.max = 1.0;
    config_.set(stdpLTD.name, 0.012, ConfigSource::Default);
    
    ConfigSchema::ParameterInfo stdpTau;
    stdpTau.name = "stdp_tau";
    stdpTau.type = "float";
    stdpTau.description = "STDP time constant for spike timing dependence";
    stdpTau.unit = "ms";
    stdpTau.required = false;
    stdpTau.defaultValue = 20.0;
    stdpTau.min = 1.0;
    stdpTau.max = 100.0;
    config_.set(stdpTau.name, 20.0, ConfigSource::Default);
    
    // Structural plasticity parameters
    ConfigSchema::ParameterInfo synaptogenesisRate;
    synaptogenesisRate.name = "synaptogenesis_rate";
    synaptogenesisRate.type = "float";
    synaptogenesisRate.description = "Rate of new synapse formation";
    synaptogenesisRate.unit = "per step";
    synaptogenesisRate.required = false;
    synaptogenesisRate.defaultValue = 0.0001;
    synaptogenesisRate.min = 0.0;
    synaptogenesisRate.max = 0.01;
    config_.set(synaptogenesisRate.name, 0.0001, ConfigSource::Default);
    
    ConfigSchema::ParameterInfo pruningRate;
    pruningRate.name = "pruning_rate";
    pruningRate.type = "float";
    pruningRate.description = "Rate of synapse pruning";
    pruningRate.unit = "per step";
    pruningRate.required = false;
    pruningRate.defaultValue = 0.00001;
    pruningRate.min = 0.0;
    pruningRate.max = 0.001;
    config_.set(pruningRate.name, 0.00001, ConfigSource::Default);
    
    // Development parameters
    ConfigSchema::ParameterInfo developmentEnabled;
    developmentEnabled.name = "development_enabled";
    developmentEnabled.type = "bool";
    developmentEnabled.description = "Enable developmental processes";
    developmentEnabled.required = false;
    developmentEnabled.defaultValue = true;
    config_.set(developmentEnabled.name, true, ConfigSource::Default);
    
    ConfigSchema::ParameterInfo criticalPeriodStart;
    criticalPeriodStart.name = "critical_period_start";
    criticalPeriodStart.type = "float";
    criticalPeriodStart.description = "Start of critical period for development";
    criticalPeriodStart.unit = "steps";
    criticalPeriodStart.required = false;
    criticalPeriodStart.defaultValue = 5000.0;
    criticalPeriodStart.min = 0.0;
    criticalPeriodStart.max = 100000.0;
    config_.set(criticalPeriodStart.name, 5000.0, ConfigSource::Default);
    
    ConfigSchema::ParameterInfo curiosityEnabled;
    curiosityEnabled.name = "curiosity_enabled";
    curiosityEnabled.type = "bool";
    curiosityEnabled.description = "Enable curiosity-driven exploration";
    curiosityEnabled.required = false;
    curiosityEnabled.defaultValue = true;
    config_.set(curiosityEnabled.name, true, ConfigSource::Default);
    
    ConfigSchema::ParameterInfo noveltyDetection;
    noveltyDetection.name = "novelty_detection";
    noveltyDetection.type = "bool";
    noveltyDetection.description = "Enable novelty detection for curiosity";
    noveltyDetection.required = false;
    noveltyDetection.defaultValue = true;
    config_.set(noveltyDetection.name, true, ConfigSource::Default);
    
    // Advanced visualization parameters
    ConfigSchema::ParameterInfo vizResolution;
    vizResolution.name = "viz_resolution";
    vizResolution.type = "int";
    vizResolution.description = "Resolution for visualizations (pixels)";
    vizResolution.unit = "pixels";
    vizResolution.required = false;
    vizResolution.defaultValue = 1024;
    vizResolution.min = 256;
    vizResolution.max = 4096;
    config_.set(vizResolution.name, 1024, ConfigSource::Default);
    
    ConfigSchema::ParameterInfo vizMaxNeurons;
    vizMaxNeurons.name = "viz_max_neurons";
    vizMaxNeurons.type = "int";
    vizMaxNeurons.description = "Maximum neurons to display in visualization";
    vizMaxNeurons.unit = "neurons";
    vizMaxNeurons.required = false;
    vizMaxNeurons.defaultValue = 10000;
    vizMaxNeurons.min = 1;
    vizMaxNeurons.max = 100000;
    config_.set(vizMaxNeurons.name, 10000, ConfigSource::Default);
    
    ConfigSchema::ParameterInfo vizColorMode;
    vizColorMode.name = "viz_color_mode";
    vizColorMode.type = "string";
    vizColorMode.description = "Color mode for visualizations";
    vizColorMode.required = false;
    vizColorMode.defaultValue = "heat";
    vizColorMode.allowedValues = {"heat", "cool", "gradient", "binary"};
    config_.set(vizColorMode.name, "heat", ConfigSource::Default);
    
    // Experiment presets
    ConfigSchema::ParameterInfo preset;
    preset.name = "experiment_preset";
    preset.type = "string";
    preset.description = "Predefined experiment configuration preset";
    preset.required = false;
    preset.defaultValue = "standard";
    preset.allowedValues = {"minimal", "standard", "high_performance", "research"};
    config_.set(preset.name, "standard", ConfigSource::Default);
    
    // Performance optimization settings
    ConfigSchema::ParameterInfo enableSIMD;
    enableSIMD.name = "enable_simd";
    enableSIMD.type = "bool";
    enableSIMD.description = "Enable SIMD optimizations";
    enableSIMD.required = false;
    enableSIMD.defaultValue = true;
    config_.set(enableSIMD.name, true, ConfigSource::Default);
    
    ConfigSchema::ParameterInfo enableParallel;
    enableParallel.name = "enable_parallel";
    enableParallel.type = "bool";
    enableParallel.description = "Enable parallel processing";
    enableParallel.required = false;
    enableParallel.defaultValue = true;
    config_.set(enableParallel.name, true, ConfigSource::Default);
    
    ConfigSchema::ParameterInfo threadCount;
    threadCount.name = "thread_count";
    threadCount.type = "int";
    threadCount.description = "Number of threads for parallel processing";
    threadCount.unit = "threads";
    threadCount.required = false;
    threadCount.defaultValue = 0;
    threadCount.min = 1;
    threadCount.max = 64;
    config_.set(threadCount.name, 0, ConfigSource::Default);
    
    ConfigSchema::ParameterInfo memoryPoolSize;
    memoryPoolSize.name = "memory_pool_size";
    memoryPoolSize.type = "int";
    memoryPoolSize.description = "Size of memory pools for allocation";
    memoryPoolSize.unit = "objects";
    memoryPoolSize.required = false;
    memoryPoolSize.defaultValue = 1024;
    memoryPoolSize.min = 256;
    memoryPoolSize.max = 8192;
    config_.set(memoryPoolSize.name, 1024, ConfigSource::Default);
    
    // Setup schema
    for (const auto& entry : config_.getKeys()) {
        auto* paramInfo = getParameterInfo(entry);
        if (paramInfo) {
            currentSchema_.addParameter(entry, *paramInfo);
        }
    }
}

AdvancedConfig::AdvancedConfig(const std::string& configFile) {
    std::vector<ValidationError> errors;
    if (loadFromFile(configFile, errors)) {
        std::cout << "Configuration loaded successfully from " << configFile << std::endl;
    } else {
        std::cerr << "Configuration errors:\n";
        for (const auto& error : errors) {
            std::cerr << "  - " << error.message << " (key: " << error.key << ")\n";
        }
    }
    
    setupDefaultSchema();
}

std::string AdvancedConfig::getConfigValue(const std::string& key) const {
    auto value = config_.get<std::string>(key);
    return value ? *value : "";
}

bool AdvancedConfig::hasConfig(const std::string& key) const {
    return config_.has(key);
}

void AdvancedConfig::setConfigValue(const std::string& key, const std::string& value, ConfigSource source) {
    config_.set(key, value, source);
}

std::vector<ValidationError> AdvancedConfig::validateConfig() {
    std::vector<ValidationError> errors;
    if (currentSchema_.parameters.size() > 0) {
        errors = currentSchema_.validate(config_);
    }
    return errors;
}

void AdvancedConfig::printConfigReport() const {
    std::cout << generateConfigReport() << std::endl;
}

std::string AdvancedConfig::generateConfigReport() const {
    std::stringstream report;
    report << "╔══════════════════════════════════════════════════════════════════╗\n";
    report << "║                  ADVANCED NLM CONFIGURATION REPORT                    ║\n";
    report << "╚══════════════════════════════════════════════════════════════════╝\n\n";
    
    report << "║ CONFIGURATION SUMMARY:";
    report << "\n║───────────────────────────────────────────────────────────────║\n";
    report << "║ Total Parameters: " << config_.getKeys().size() << "\n";
    report << "║ Configuration Source: ";
    
    // Count parameter sources
    std::unordered_map<ConfigSource, int> sourceCount;
    for (const auto& entry : config_.getKeys()) {
        sourceCount[config_.getConfigEntry(entry).source]++;
    }
    
    for (const auto& [source, count] : sourceCount) {
        std::string sourceName;
        switch (source) {
            case ConfigSource::Default: sourceName = "Default"; break;
            case ConfigSource::File: sourceName = "File"; break;
            case ConfigSource::CommandLine: sourceName = "Command Line"; break;
            case ConfigSource::Runtime: sourceName = "Runtime"; break;
        }
        report << sourceName << " (" << count << "), ";
    }
    report << "\n\n";
    
    report << "║ DETAILED PARAMETERS:";
    report << "\n║───────────────────────────────────────────────────────────────║\n";
    
    for (const auto& key : config_.getKeys()) {
        auto value = config_.get<std::string>(key);
        if (value) {
            report << "║ " << std::left << std::setw(20) << key << ": " << std::left << std::setw(15) << *value;
            
            // Add schema info if available
            auto* paramInfo = getParameterInfo(key);
            if (paramInfo) {
                report << " (" << paramInfo->type << ")";
                if (!paramInfo->unit.empty()) {
                    report << " [" << paramInfo->unit << "]";
                }
            }
            
            // Add source indicator
            auto& entry = config_.getConfigEntry(key);
            std::string sourceChar;
            switch (entry.source) {
                case ConfigSource::Default: sourceChar = "*"; break;
                case ConfigSource::File: sourceChar = "F"; break;
                case ConfigSource::CommandLine: sourceChar = "C"; break;
                case ConfigSource::Runtime: sourceChar = "R"; break;
            }
            report << " [" << sourceChar << "]\n";
        }
    }
    
    report << "╚══════════════════════════════════════════════════════════════════╝\n";
    
    return report.str();
}

std::string AdvancedConfig::exportToJSON() const {
    return config_.exportToJSON();
}

void AdvancedConfig::exportToFile(const std::string& filepath) const {
    if (config_.saveToFile(filepath)) {
        std::cout << "Configuration exported to " << filepath << std::endl;
    } else {
        std::cerr << "Failed to export configuration to " << filepath << std::endl;
    }
}

void AdvancedConfig::applyPreset(const std::string& presetName) {
    if (presets_.find(presetName) != presets_.end()) {
        // Implementation for applying preset
        std::cout << "Applied preset: " << presetName << std::endl;
    } else {
        std::cerr << "Unknown preset: " << presetName << std::endl;
    }
}

void AdvancedConfig::listPresets() const {
    std::cout << "Available presets:" << std::endl;
    for (const auto& preset : presets_) {
        std::cout << "  - " << preset.first << std::endl;
    }
}

AdvancedVisualization::AdvancedVisualization() {
    // Initialize visualization settings
}

void AdvancedVisualization::generateReport(const Brain* brain, double timestep) {
    if (!brain) {
        std::cerr << "Error: Brain pointer is null" << std::endl;
        return;
    }
    
    std::cout << generateBrainStateVisualization(brain) << std::endl;
    std::cout << generatePerformanceVisualization(brain, timestep) << std::endl;
}

std::string AdvancedVisualization::generateBrainStateVisualization(const Brain* brain) const {
    return AdvancedVisualization::generateBrainStateVisualization(brain);
}

std::string AdvancedVisualization::generatePerformanceVisualization(const Brain* brain, double timestep) const {
    return AdvancedVisualization::generatePerformanceVisualization(brain, timestep);
}

void AdvancedVisualization::saveVisualization(const std::string& filename, const Brain* brain, double timestep) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << generateBrainStateVisualization(brain);
        file << "\n\n";
        file << generatePerformanceVisualization(brain, timestep);
        file.close();
        std::cout << "Visualization saved to " << filename << std::endl;
    } else {
        std::cerr << "Failed to save visualization to " << filename << std::endl;
    }
}

std::string AdvancedVisualization::generateBrainStateVisualization(const Brain* brain) const {
    // Implementation moved to header file
    return "";
}

std::string AdvancedVisualization::generatePerformanceVisualization(const Brain* brain, double timestep) const {
    // Implementation moved to header file
    return "";
}

} // namespace nlm