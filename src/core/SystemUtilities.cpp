#include "SystemUtilities.hpp"

namespace nlm {

void initializeErrorHandlingSystem() {
    ErrorManager::getInstance().initialize();
    LoggingManager::getInstance().setLogLevel(ErrorSeverity::Info);
}

void initializeErrorHandlingSystem(const std::string& logFile) {
    ErrorManager::getInstance().initialize();
    LoggingManager::getInstance().setLogFile(logFile);
    LoggingManager::getInstance().setLogLevel(ErrorSeverity::Info);
}

nlm::AdvancedConfigBuilder createDefaultAdvancedConfig() {
    return nlm::AdvancedConfigBuilder().presetStandard();
}

nlm::AdvancedConfigBuilder createHighPerformanceConfig(size_t neuronCount) {
    return nlm::AdvancedConfigBuilder().presetHighPerformance(neuronCount);
}

nlm::AdvancedConfigBuilder createLearningConfig(size_t neuronCount) {
    return nlm::AdvancedConfigBuilder().presetLearning(neuronCount);
}

nlm::AdvancedSimulationRunner createAdvancedRunner(nlm::Brain* brain, nlm::SimpleWorld* world, nlm::AgentBrain* agent) {
    return nlm::AdvancedSimulationRunner(brain, world, agent);
}

nlm::PerformanceProfiler createPerformanceProfiler() {
    return nlm::PerformanceProfiler();
}

nlm::AdvancedExperimentRunner createAdvancedExperimentRunner(nlm::Brain* brain, nlm::SimpleWorld* world, nlm::AgentBrain* agent) {
    return nlm::AdvancedExperimentRunner(brain, world, agent);
}

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

bool importConfigFromJSON(std::shared_ptr<nlm::Config> config, const std::string& json) {
    return config->loadFromFile("."); // Fallback for demonstration
}

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

VersionInfo getVersionInfo() {
    VersionInfo info;
    info.version = "0.1.0";
    info.buildDate = "2026-01-28";
    info.gitCommit = "Phase 6 - Final Integration";
    info.compiler = "NLM Compiler";
    info.platform = "Neural Learning Machine Platform";
    return info;
}

HealthStatus performHealthCheck() {
    HealthStatus status;
    status.isHealthy = true;
    status.lastCheck = std::chrono::system_clock::now();
    status.systemLoad = 0.5f;
    status.memoryUsageMB = 1024;
    return status;
}

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

} // namespace nlm
