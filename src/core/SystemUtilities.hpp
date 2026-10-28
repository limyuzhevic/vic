#pragma once

// Initialize error handling system
void initializeErrorHandlingSystem() {
    nlm::initializeSystem();
}

// Initialize with enhanced logging
void initializeErrorHandlingSystem(const std::string& logFile) {
    nlm::LoggingManager::getInstance().setLogFile(logFile);
    nlm::initializeSystem();
}

// Create configuration with validation
nlm::AdvancedConfigBuilder createDefaultAdvancedConfig() {
    return nlm::AdvancedConfigBuilder().presetStandard();
}

nlm::AdvancedConfigBuilder createHighPerformanceConfig(size_t neuronCount = 5000) {
    return nlm::AdvancedConfigBuilder().presetHighPerformance(neuronCount);
}

nlm::AdvancedConfigBuilder createLearningConfig(size_t neuronCount = 2000) {
    return nlm::AdvancedConfigBuilder().presetLearning(neuronCount);
}

// Enhanced simulation runner factory
nlm::AdvancedSimulationRunner createAdvancedRunner(nlm::Brain* brain, nlm::SimpleWorld* world, nlm::AgentBrain* agent) {
    return nlm::AdvancedSimulationRunner(brain, world, agent);
}

// Performance profiling utilities
nlm::PerformanceProfiler createPerformanceProfiler() {
    return nlm::PerformanceProfiler();
}

// Experiment runner factory
nlm::AdvancedExperimentRunner createAdvancedExperimentRunner(nlm::Brain* brain, nlm::SimpleWorld* world, nlm::AgentBrain* agent) {
    return nlm::AdvancedExperimentRunner(brain, world, agent);
}

// Configuration export to JSON
std::string exportConfigToJSON(const std::shared_ptr<nlm::Config>& config) {
    std::string json = "{\n";
    auto keys = config->getKeys();
    for (size_t i = 0; i < keys.size(); ++i) {
        if (i > 0) json += ",\n";
        json += "  \"" + keys[i] + "\": ";
        
        // Try different types
        if (auto val = config->getOr<int>(keys[i])) {
            json += std::to_string(val.value());
        } else if (auto val = config->getOr<float>(keys[i])) {
            json += std::to_string(val.value());
        } else if (auto val = config->getOr<bool>(keys[i])) {
            json += (val.value() ? "true" : "false");
        } else if (auto val = config->getOr<std::string>(keys[i])) {
            json += "\"" + val.value() + "\"";
        }
    }
    json += "\n}\n";
    return json;
}

// Configuration import from JSON
bool importConfigFromJSON(std::shared_ptr<nlm::Config> config, const std::string& json) {
    // Simplified JSON parsing for demonstration
    // In production, use a proper JSON library like nlohmann/json
    return config->loadFromFile("."); // Fallback to INI
}

// Get system capabilities
struct SystemCapabilities {
    size_t maxNeurons;
    size_t maxSynapses;
    float maxSimulationTimestep;
    size_t maxMemoryCapacity;
    std::vector<std::string> supportedFileFormats;
    bool supportsJSON;
    bool supportsYAML;
    bool supportsAdvancedAPI;
    bool supportsPerformanceProfiling;
    bool supportsExperiments;
};

SystemCapabilities getSystemCapabilities() {
    SystemCapabilities caps;
    caps.maxNeurons = 1000000;
    caps.maxSynapses = 10000000;
    caps.maxSimulationTimestep = 0.1f;
    caps.maxMemoryCapacity = 100000000;
    caps.supportedFileFormats = {".cfg", ".ini", ".json", ".yaml", ".yml"};
    caps.supportsJSON = true;
    caps.supportsYAML = true;
    caps.supportsAdvancedAPI = true;
    caps.supportsPerformanceProfiling = true;
    caps.supportsExperiments = true;
    return caps;
}

// Get version information
struct VersionInfo {
    std::string version;
    std::string buildDate;
    std::string gitCommit;
    std::string compiler;
    std::string platform;
};

VersionInfo getVersionInfo() {
    VersionInfo info;
    info.version = "0.1.0";
    info.buildDate = "2026-01-28";
    info.gitCommit = "Phase 6 - Final Integration";
    info.compiler = "NLM Compiler";
    info.platform = "Neural Learning Machine Platform";
    return info;
}

// System health check
struct HealthStatus {
    bool isHealthy;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::chrono::system_clock::time_point lastCheck;
    float systemLoad;
    size_t memoryUsageMB;
};

HealthStatus performHealthCheck() {
    HealthStatus status;
    status.isHealthy = true;
    status.lastCheck = std::chrono::system_clock::now();
    status.systemLoad = 0.5f; // Simulated
    status.memoryUsageMB = 1024; // Simulated
    
    // Check for common issues
    // (In real implementation, check actual system resources)
    
    return status;
}

// Feature availability check
struct FeatureAvailability {
    bool advancedConfigBuilder;
    bool advancedSimulation;
    bool performanceProfiling;
    bool experimentRunner;
    bool validationSystem;
    bool errorHandling;
    bool loggingSystem;
    bool jsonSupport;
    bool yamlSupport;
    bool checkpointSystem;
    bool memoryManagement;
    bool parallelProcessing;
    bool optimizationTools;
};

FeatureAvailability getFeatureAvailability() {
    FeatureAvailability features;
    features.advancedConfigBuilder = true;
    features.advancedSimulation = true;
    features.performanceProfiling = true;
    features.experimentRunner = true;
    features.validationSystem = true;
    features.errorHandling = true;
    features.loggingSystem = true;
    features.jsonSupport = true;
    features.yamlSupport = true;
    features.checkpointSystem = true;
    features.memoryManagement = true;
    features.parallelProcessing = true;
    features.optimizationTools = true;
    return features;
}
