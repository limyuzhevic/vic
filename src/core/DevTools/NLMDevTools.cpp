#include "core/DevTools/NLMDevTools.hpp"
#include "core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <iomanip>

namespace nlm {

NLMDevTools::NLMDevTools() {
    debugModeEnabled = false;
    performanceMonitoringEnabled = false;
    memoryLeakDetectionEnabled = false;
    stepByStepModeEnabled = false;
    executionPaused = false;
    executionStopped = false;

    performanceInterval = std::chrono::duration<double>(0.1); // 100ms interval
    lastSnapshotTime = std::chrono::high_resolution_clock::now();
}

bool NLMDevTools::validateConfiguration(std::shared_ptr<Config> config) const {
    if (!config) {
        logDebugInfo("ERROR", "Invalid configuration pointer", {
            {"reason", "config is null"}
        });
        return false;
    }

    bool isValid = true;

    // Check required keys
    const std::vector<std::string> requiredKeys = {
        "neuron_count",
        "region_count",
        "connection_probability",
        "simulation_timestep",
        "random_seed"
    };

    for (const auto& key : requiredKeys) {
        if (!config->has(key)) {
            logDebugInfo("WARNING", "Missing required configuration key", {
                {"key", key},
                {"severity", "missing"}
            });
            isValid = false;
        }
    }

    // Validate numeric ranges
    auto validateRange = [this](const std::string& key, double min, double max) {
        if (config->has(key)) {
            auto value = config->getOr<double>(key, 0.0);
            if (value < min || value > max) {
                logDebugInfo("WARNING", "Configuration value out of range", {
                    {"key", key},
                    {"value", std::to_string(value)},
                    {"min", std::to_string(min)},
                    {"max", std::to_string(max)}
                });
                return false;
            }
        }
        return true;
    };

    validateRange("neuron_count", 1, 1000000);
    validateRange("region_count", 1, 1000);
    validateRange("connection_probability", 0.0, 1.0);
    validateRange("simulation_timestep", 0.0001, 1.0);
    validateRange("random_seed", 0, 1000000000);

    // Check for consistency
    if (config->has("neuron_count") && config->has("region_count")) {
        size_t neurons = config->getOr<size_t>("neuron_count", 0);
        size_t regions = config->getOr<size_t>("region_count", 0);
        if (neurons < regions) {
            logDebugInfo("ERROR", "Inconsistent configuration", {
                {"reason", "neurons cannot be less than regions"},
                {"neurons", std::to_string(neurons)},
                {"regions", std::to_string(regions)}
            });
            isValid = false;
        }
    }

    logDebugInfo("INFO", "Configuration validation completed", {
        {"valid", isValid ? "true" : "false"},
        {"keys_checked", std::to_string(requiredKeys.size())}
    });

    return isValid;
}

bool NLMDevTools::validateBrainConfiguration(std::shared_ptr<Brain> brain) const {
    if (!brain) {
        logDebugInfo("ERROR", "Invalid brain pointer", {
            {"reason", "brain is null"}
        });
        return false;
    }

    bool isValid = true;

    // Check brain initialization
    auto config = brain->getConfig();
    if (!config) {
        logDebugInfo("ERROR", "Brain has no configuration", {
            {"reason", "brain is not properly initialized"}
        });
        return false;
    }

    // Validate configuration
    if (!validateConfiguration(config)) {
        isValid = false;
    }

    // Check brain internal consistency
    size_t neuronCount = brain->getTotalNeuronCount();
    size_t regionCount = brain->getRegionCount();
    size_t synapseCount = brain->getTotalSynapseCount();

    if (neuronCount == 0) {
        logDebugInfo("WARNING", "Brain has no neurons", {
            {"reason", "brain may not be initialized properly"}
        });
        isValid = false;
    }

    if (regionCount == 0) {
        logDebugInfo("ERROR", "Brain has no regions", {
            {"reason", "brain initialization failed"}
        });
        isValid = false;
    }

    // Check E/I balance
    float eIRatio = brain->getExcitationInhibitionRatio();
    if (std::isinf(eIRatio) && eIRatio < 0) {
        logDebugInfo("WARNING", "Brain has excessive inhibition", {
            {"e_i_ratio", "negative infinity"}
        });
    }
    else if (eIRatio > 100.0f) {
        logDebugInfo("WARNING", "Brain has highly imbalanced E/I ratio", {
            {"e_i_ratio", std::to_string(eIRatio)}
        });
    }

    logDebugInfo("INFO", "Brain configuration validation completed", {
        {"valid", isValid ? "true" : "false"},
        {"neurons", std::to_string(neuronCount)},
        {"regions", std::to_string(regionCount)},
        {"synapses", std::to_string(synapseCount)}
    });

    return isValid;
}

bool NLMDevTools::checkConfigurationConsistency(std::shared_ptr<Config> config) const {
    if (!config) {
        return false;
    }

    // Check for conflicting settings
    if (config->has("neuron_count") && config->has("region_count")) {
        size_t neurons = config->getOr<size_t>("neuron_count", 0);
        size_t regions = config->getOr<size_t>("region_count", 0);
        if (neurons < regions) {
            return false; // Inconsistent
        }
    }

    // Check for recommended settings
    bool hasRecommendedSettings = true;

    if (!config->has("stdp_ltp_weight")) {
        logDebugInfo("INFO", "Missing recommended plasticity setting", {
            {"key", "stdp_ltp_weight"}
        });
        hasRecommendedSettings = false;
    }

    if (!config->has("stdp_ltd_weight")) {
        logDebugInfo("INFO", "Missing recommended plasticity setting", {
            {"key", "stdp_ltd_weight"}
        });
        hasRecommendedSettings = false;
    }

    logDebugInfo("INFO", "Configuration consistency check completed", {
        {"consistent", "true"},
        {"has_recommended_settings", hasRecommendedSettings ? "true" : "false"}
    });

    return true;
}

void NLMDevTools::enableDebugMode(bool enable) {
    debugModeEnabled = enable;
    logDebugInfo("INFO", "Debug mode " + std::string(enable ? "enabled" : "disabled"), {
        {"mode", enable ? "enabled" : "disabled"}
    });
}

void NLMDevTools::logDebugInfo(const std::string& eventType, const std::string& message,
                             const std::map<std::string, std::string>& metadata) {
    if (!debugModeEnabled) {
        return;
    }

    DebugInfo info;
    info.timestamp = getTimestamp();
    info.eventType = eventType;
    info.message = message;
    info.metadata = metadata;

    // Add stack trace for errors
    if (eventType == "ERROR" || eventType == "EXCEPTION") {
        // In a real implementation, this would capture the actual stack trace
        info.stackTrace = "Stack trace capture not implemented in this build";
    }

    debugLog.push_back(info);

    // Notify callbacks
    notifyDebugCallbacks(info);

    // Log to console
    std::cout << "[" << info.timestamp << "] [" << eventType << "] " << info.message << std::endl;
    for (const auto& pair : info.metadata) {
        std::cout << "  " << pair.first << ": " << pair.second << std::endl;
    }
}

std::vector<NLMDevTools::DebugInfo> NLMDevTools::getDebugLog() const {
    return debugLog;
}

void NLMDevTools::clearDebugLog() {
    debugLog.clear();
    logDebugInfo("INFO", "Debug log cleared", {});
}

void NLMDevTools::enablePerformanceMonitoring(bool enable) {
    performanceMonitoringEnabled = enable;
    if (enable) {
        lastSnapshotTime = std::chrono::high_resolution_clock::now();
    }
    logDebugInfo("INFO", "Performance monitoring " + std::string(enable ? "enabled" : "disabled"), {
        {"mode", enable ? "enabled" : "disabled"}
    });
}

NLMDevTools::PerformanceMetrics NLMDevTools::getCurrentPerformanceMetrics() const {
    PerformanceMetrics metrics;
    auto now = std::chrono::high_resolution_clock::now();
    metrics.lastUpdateTime = std::chrono::duration<double>(now - lastSnapshotTime);

    if (performanceMonitoringEnabled) {
        updatePerformanceMetrics(metrics);
    } else {
        metrics.cpuUsage = 0.0;
        metrics.memoryUsage = 0.0;
        metrics.neuronsActive = 0;
        metrics.spikesPerSecond = 0;
        metrics.averageFiringRate = 0.0;
        metrics.eI_ratio = 0.0;
    }

    return metrics;
}

void NLMDevTools::recordPerformanceSnapshot() {
    if (performanceMonitoringEnabled) {
        PerformanceMetrics metrics = getCurrentPerformanceMetrics();
        performanceHistory.push_back(metrics);

        // Keep only last 1000 samples
        if (performanceHistory.size() > 1000) {
            performanceHistory.erase(performanceHistory.begin());
        }
    }
}

std::vector<NLMDevTools::PerformanceMetrics> NLMDevTools::getPerformanceHistory() const {
    return performanceHistory;
}

NLMDevTools::MemoryInfo NLMDevTools::getMemoryInfo() const {
    MemoryInfo info;
    // In a real implementation, this would use system-specific APIs
    info.totalAllocated = 0;
    info.currentAllocated = 0;
    info.peakAllocated = 0;

    logDebugInfo("INFO", "Memory info retrieved", {
        {"total_allocated", std::to_string(info.totalAllocated)},
        {"current_allocated", std::to_string(info.currentAllocated)},
        {"peak_allocated", std::to_string(info.peakAllocated)}
    });

    return info;
}

void NLMDevTools::enableMemoryLeakDetection(bool enable) {
    memoryLeakDetectionEnabled = enable;
    if (enable) {
        memoryLeaks.clear();
    }
    logDebugInfo("INFO", "Memory leak detection " + std::string(enable ? "enabled" : "disabled"), {
        {"mode", enable ? "enabled" : "disabled"}
    });
}

std::vector<std::string> NLMDevTools::getMemoryLeakReport() const {
    return memoryLeaks;
}

void NLMDevTools::forceGarbageCollection() {
    logDebugInfo("INFO", "Garbage collection requested", {});
    // In a real implementation, this would call system garbage collection
}

bool NLMDevTools::compareBrainStates(std::shared_ptr<Brain> brain1, std::shared_ptr<Brain> brain2,
                                   double tolerance) const {
    if (!brain1 || !brain2) {
        logDebugInfo("ERROR", "Invalid brain pointer for comparison", {
            {"reason", "one or both brains are null"}
        });
        return false;
    }

    // Simple comparison: check neuron and synapse counts
    bool neuronsMatch = (brain1->getTotalNeuronCount() == brain2->getTotalNeuronCount());
    bool synapsesMatch = (brain1->getTotalSynapseCount() == brain2->getTotalSynapseCount());
    bool regionsMatch = (brain1->getRegionCount() == brain2->getRegionCount());

    if (neuronsMatch && synapsesMatch && regionsMatch) {
        logDebugInfo("INFO", "Brain states match", {
            {"neurons", std::to_string(brain1->getTotalNeuronCount())},
            {"synapses", std::to_string(brain1->getTotalSynapseCount())},
            {"regions", std::to_string(brain1->getRegionCount())}
        });
        return true;
    } else {
        logDebugInfo("WARNING", "Brain states differ", {
            {"neurons_match", neuronsMatch ? "true" : "false"},
            {"synapses_match", synapsesMatch ? "true" : "false"},
            {"regions_match", regionsMatch ? "true" : "false"}
        });
        return false;
    }
}

std::string NLMDevTools::generateBrainStateDiff(std::shared_ptr<Brain> brain1, std::shared_ptr<Brain> brain2) const {
    std::stringstream diff;

    diff << "Brain State Difference Report" << std::endl;
    diff << "============================" << std::endl;
    diff << "Generated at: " << getTimestamp() << std::endl;
    diff << std::endl;

    if (!brain1 || !brain2) {
        diff << "ERROR: Invalid brain pointer" << std::endl;
        return diff.str();
    }

    diff << "Neuron Count:" << std::endl;
    diff << "  Brain 1: " << brain1->getTotalNeuronCount() << std::endl;
    diff << "  Brain 2: " << brain2->getTotalNeuronCount() << std::endl;
    diff << "  Difference: " << (brain1->getTotalNeuronCount() - brain2->getTotalNeuronCount()) << std::endl;
    diff << std::endl;

    diff << "Synapse Count:" << std::endl;
    diff << "  Brain 1: " << brain1->getTotalSynapseCount() << std::endl;
    diff << "  Brain 2: " << brain2->getTotalSynapseCount() << std::endl;
    diff << "  Difference: " << (brain1->getTotalSynapseCount() - brain2->getTotalSynapseCount()) << std::endl;
    diff << std::endl;

    diff << "Region Count:" << std::endl;
    diff << "  Brain 1: " << brain1->getRegionCount() << std::endl;
    diff << "  Brain 2: " << brain2->getRegionCount() << std::endl;
    diff << "  Difference: " << (brain1->getRegionCount() - brain2->getRegionCount()) << std::endl;
    diff << std::endl;

    diff << "E/I Ratio:" << std::endl;
    diff << "  Brain 1: " << brain1->getExcitationInhibitionRatio() << std::endl;
    diff << "  Brain 2: " << brain2->getExcitationInhibitionRatio() << std::endl;
    diff << "  Difference: " << (brain1->getExcitationInhibitionRatio() - brain2->getExcitationInhibitionRatio()) << std::endl;

    return diff.str();
}

bool NLMDevTools::detectBrainAnomalies(std::shared_ptr<Brain> brain) const {
    if (!brain) {
        return false;
    }

    bool hasAnomalies = false;

    // Check for extreme values
    auto firingRate = brain->getAverageFiringRate();
    if (firingRate > 100.0f || firingRate < 0.0f) {
        logDebugInfo("ERROR", "Extreme firing rate detected", {
            {"firing_rate", std::to_string(firingRate)},
            {"severity", "critical"}
        });
        hasAnomalies = true;
    }

    auto eIRatio = brain->getExcitationInhibitionRatio();
    if (std::isinf(eIRatio) && eIRatio < 0) {
        logDebugInfo("ERROR", "Extreme inhibition detected", {
            {"e_i_ratio", "negative infinity"},
            {"severity", "critical"}
        });
        hasAnomalies = true;
    }
    else if (eIRatio > 1000.0f) {
        logDebugInfo("WARNING", "Highly imbalanced E/I ratio", {
            {"e_i_ratio", std::to_string(eIRatio)},
            {"severity", "high"}
        });
        hasAnomalies = true;
    }

    auto spikeCount = brain->getTotalSpikeCount();
    if (spikeCount == 0) {
        logDebugInfo("WARNING", "No spikes detected", {
            {"reason", "brain may be inactive"},
            {"severity", "medium"}
        });
        hasAnomalies = true;
    }

    return hasAnomalies;
}

std::vector<std::string> NLMDevTools::getBrainHealthReport(std::shared_ptr<Brain> brain) const {
    std::vector<std::string> report;

    if (!brain) {
        report.push_back("ERROR: Invalid brain pointer");
        return report;
    }

    report.push_back("Brain Health Report");
    report.push_back("==================");
    report.push_back("Generated at: " + getTimestamp());
    report.push_back("");

    report.push_back("Basic Statistics:");
    report.push_back("  Neuron Count: " + std::to_string(brain->getTotalNeuronCount()));
    report.push_back("  Synapse Count: " + std::to_string(brain->getTotalSynapseCount()));
    report.push_back("  Region Count: " + std::to_string(brain->getRegionCount()));
    report.push_back("  Active Neurons: " + std::to_string(brain->getActiveNeuronCount()));
    report.push_back("  Firing Neurons: " + std::to_string(brain->getFiringNeuronCount()));
    report.push_back("  Total Spikes: " + std::to_string(brain->getTotalSpikeCount()));
    report.push_back("");

    report.push_back("Network Metrics:");
    report.push_back("  Average Firing Rate: " + std::to_string(brain->getAverageFiringRate()));
    report.push_back("  E/I Ratio: " + std::to_string(brain->getExcitationInhibitionRatio()));
    report.push_back("");

    report.push_back("Development Status:");
    report.push_back("  Developmental Stage: " + std::to_string(static_cast<int>(brain->getDevelopmentalStage())));

    return report;
}

void NLMDevTools::enableStepByStep(bool enable) {
    stepByStepModeEnabled = enable;
    if (enable) {
        executionPaused = true;
    }
    logDebugInfo("INFO", "Step-by-step mode " + std::string(enable ? "enabled" : "disabled"), {
        {"mode", enable ? "enabled" : "disabled"},
        {"initial_state", executionPaused ? "paused" : "running"}
    });
}

void NLMDevTools::stepOne(bool pauseAfterStep) {
    if (!stepByStepModeEnabled) {
        logDebugInfo("ERROR", "Step-by-step mode not enabled", {
            {"reason", "cannot step one when disabled"}
        });
        return;
    }

    if (executionPaused) {
        // In a real implementation, this would execute one simulation step
        logDebugInfo("INFO", "Executed one simulation step", {});

        if (pauseAfterStep) {
            // Stay paused for user interaction
            logDebugInfo("INFO", "Paused after step", {
                {"action", "awaiting_user_input"}
            });
        } else {
            // Continue execution
            logDebugInfo("INFO", "Resuming execution", {});
            executionPaused = false;
        }
    } else {
        logDebugInfo("ERROR", "Execution not paused", {
            {"reason", "cannot step one when not paused"}
        });
    }
}

bool NLMDevTools::isPaused() const {
    return executionPaused;
}

void NLMDevTools::resumeExecution() {
    if (stepByStepModeEnabled) {
        executionPaused = false;
        logDebugInfo("INFO", "Execution resumed", {});
    } else {
        logDebugInfo("ERROR", "Cannot resume: step-by-step mode not enabled", {
            {"reason", "resume requires step-by-step mode"}
        });
    }
}

void NLMDevTools::stopExecution() {
    executionStopped = true;
    executionPaused = false;
    logDebugInfo("INFO", "Execution stopped", {});
}

bool NLMDevTools::exportBrainState(std::shared_ptr<Brain> brain, const std::string& filepath) const {
    if (!brain) {
        logDebugInfo("ERROR", "Cannot export: invalid brain", {
            {"reason", "brain is null"}
        });
        return false;
    }

    std::ofstream file(filepath);
    if (!file.is_open()) {
        logDebugInfo("ERROR", "Cannot export: cannot open file", {
            {"file", filepath},
            {"reason", "file access denied or invalid path"}
        });
        return false;
    }

    // Export brain state as text (simplified format)
    file << "NLM Brain State Export" << std::endl;
    file << "=======================" << std::endl;
    file << "Exported at: " << getTimestamp() << std::endl;
    file << std::endl;

    file << "Basic Statistics:" << std::endl;
    file << "Neuron Count: " << brain->getTotalNeuronCount() << std::endl;
    file << "Synapse Count: " << brain->getTotalSynapseCount() << std::endl;
    file << "Region Count: " << brain->getRegionCount() << std::endl;
    file << "Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
    file << "Average Firing Rate: " << brain->getAverageFiringRate() << std::endl;
    file << "E/I Ratio: " << brain->getExcitationInhibitionRatio() << std::endl;

    file.close();

    logDebugInfo("INFO", "Brain state exported successfully", {
        {"file", filepath},
        {"neurons", std::to_string(brain->getTotalNeuronCount())},
        {"synapses", std::to_string(brain->getTotalSynapseCount())}
    });

    return true;
}

bool NLMDevTools::importBrainState(std::shared_ptr<Brain> brain, const std::string& filepath) {
    if (!brain) {
        logDebugInfo("ERROR", "Cannot import: invalid brain", {
            {"reason", "brain is null"}
        });
        return false;
    }

    std::ifstream file(filepath);
    if (!file.is_open()) {
        logDebugInfo("ERROR", "Cannot import: cannot open file", {
            {"file", filepath},
            {"reason", "file not found or access denied"}
        });
        return false;
    }

    // Note: Actual brain state import would require more complex serialization
    // This is a placeholder implementation
    std::string line;
    while (std::getline(file, line)) {
        // Parse lines and update brain state
        // In a real implementation, this would properly deserialize the brain state
    }

    file.close();

    logDebugInfo("INFO", "Brain state imported (placeholder)", {
        {"file", filepath},
        {"note", "full import implementation required"}
    });

    return true;
}

bool NLMDevTools::exportConfiguration(std::shared_ptr<Config> config, const std::string& filepath) const {
    if (!config) {
        logDebugInfo("ERROR", "Cannot export: invalid config", {
            {"reason", "config is null"}
        });
        return false;
    }

    std::ofstream file(filepath);
    if (!file.is_open()) {
        logDebugInfo("ERROR", "Cannot export: cannot open file", {
            {"file", filepath},
            {"reason", "file access denied or invalid path"}
        });
        return false;
    }

    // Export configuration (simplified)
    file << "NLM Configuration Export" << std::endl;
    file << "==========================" << std::endl;
    file << "Exported at: " << getTimestamp() << std::endl;
    file << std::endl;

    // Export all configuration keys
    auto keys = config->getKeys();
    for (const auto& key : keys) {
        file << key << std::endl;
    }

    file.close();

    logDebugInfo("INFO", "Configuration exported successfully", {
        {"file", filepath},
        {"keys", std::to_string(keys.size())}
    });

    return true;
}

bool NLMDevTools::importConfiguration(std::shared_ptr<Config> config, const std::string& filepath) {
    if (!config) {
        logDebugInfo("ERROR", "Cannot import: invalid config", {
            {"reason", "config is null"}
        });
        return false;
    }

    std::ifstream file(filepath);
    if (!file.is_open()) {
        logDebugInfo("ERROR", "Cannot import: cannot open file", {
            {"file", filepath},
            {"reason", "file not found or access denied"}
        });
        return false;
    }

    // Note: This is a placeholder implementation
    // In a real implementation, this would properly parse and load the configuration

    std::string line;
    while (std::getline(file, line)) {
        // Parse configuration lines
        // In a real implementation, this would properly import the configuration
    }

    file.close();

    logDebugInfo("INFO", "Configuration imported (placeholder)", {
        {"file", filepath},
        {"note", "full import implementation required"}
    });

    return true;
}

double NLMDevTools::calculateConnectivityMetrics(std::shared_ptr<Brain> brain) const {
    if (!brain || brain->getTotalNeuronCount() == 0) {
        return 0.0;
    }

    size_t totalConnections = 0;
    for (const auto& region : brain->getRegions()) {
        totalConnections += region->getSynapseCount();
    }

    double connectionDensity = static_cast<double>(totalConnections) /
                              (static_cast<double>(brain->getTotalNeuronCount()) * brain->getTotalNeuronCount());

    logDebugInfo("INFO", "Connectivity metrics calculated", {
        {"connection_density", std::to_string(connectionDensity)},
        {"total_connections", std::to_string(totalConnections)}
    });

    return connectionDensity;
}

double NLMDevTools::calculatePlasticityMetrics(std::shared_ptr<Brain> brain) const {
    if (!brain) {
        return 0.0;
    }

    // Simple plasticity metric based on synapse weight changes
    // This is a simplified placeholder
    double plasticityScore = 0.5f; // Placeholder value

    logDebugInfo("INFO", "Plasticity metrics calculated", {
        {"plasticity_score", std::to_string(plasticityScore)}
    });

    return plasticityScore;
}

double NLMDevTools::calculateMemoryMetrics(std::shared_ptr<Brain> brain) const {
    if (!brain) {
        return 0.0;
    }

    // Simple memory metric based on working and episodic memory usage
    // This is a simplified placeholder
    double memoryScore = 0.7f; // Placeholder value

    logDebugInfo("INFO", "Memory metrics calculated", {
        {"memory_score", std::to_string(memoryScore)}
    });

    return memoryScore;
}

std::vector<std::pair<std::string, double>> NLMDevTools::getNeuralSystemStatus(std::shared_ptr<Brain> brain) const {
    std::vector<std::pair<std::string, double>> status;

    if (!brain) {
        status.push_back({"Error", 0.0});
        return status;
    }

    status.push_back({"Neuron Count", static_cast<double>(brain->getTotalNeuronCount())});
    status.push_back({"Synapse Count", static_cast<double>(brain->getTotalSynapseCount())});
    status.push_back({"Average Firing Rate", brain->getAverageFiringRate()});
    status.push_back({"E/I Ratio", brain->getExcitationInhibitionRatio()});
    status.push_back({"Total Spikes", static_cast<double>(brain->getTotalSpikeCount())});

    return status;
}

void NLMDevTools::notifyDebugCallbacks(const DebugInfo& info) const {
    for (const auto& callback : debugCallbacks) {
        try {
            callback(info);
        } catch (const std::exception& e) {
            std::cerr << "Error in debug callback: " << e.what() << std::endl;
        }
    }
}

void NLMDevTools::updatePerformanceMetrics(PerformanceMetrics& metrics) const {
    auto now = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastSnapshotTime);

    metrics.lastUpdateTime = std::chrono::duration<double>(elapsed);

    // In a real implementation, this would gather actual system metrics
    metrics.cpuUsage = 50.0; // Placeholder
    metrics.memoryUsage = 512.0; // Placeholder (MB)
    metrics.neuronsActive = 100; // Placeholder
    metrics.spikesPerSecond = 1000; // Placeholder
    metrics.averageFiringRate = 10.0; // Placeholder
    metrics.eI_ratio = 1.5; // Placeholder
}

void NLMDevTools::checkMemoryLeaks() const {
    if (memoryLeakDetectionEnabled) {
        // In a real implementation, this would check for actual memory leaks
        // This is a placeholder
        memoryLeaks.clear(); // Clear existing leaks for demo
        memoryLeaks.push_back("Memory leak detection requires system-specific implementation");
    }
}

std::string NLMDevTools::getTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::gmtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

void NLMDevTools::addDebugCallback(const DebugCallback& callback) {
    debugCallbacks.push_back(callback);
}

void NLMDevTools::removeDebugCallback(const DebugCallback& callback) {
    debugCallbacks.erase(std::remove(debugCallbacks.begin(), debugCallbacks.end(), callback),
                        debugCallbacks.end());
}

bool NLMDevTools::generateConfigurationTemplate(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    file << "# NLM Configuration Template" << std::endl;
    file << "# Generated by NLM DevTools" << std::endl;
    file << std::endl;
    file << "# Basic Brain Configuration" << std::endl;
    file << "neuron_count = 1000" << std::endl;
    file << "region_count = 1" << std::endl;
    file << "connection_probability = 0.1" << std::endl;
    file << "simulation_timestep = 0.001" << std::endl;
    file << "random_seed = 42" << std::endl;
    file << std::endl;
    file << "# Plasticity Configuration" << std::endl;
    file << "stdp_ltp_weight = 0.01" << std::endl;
    file << "stdp_ltd_weight = 0.012" << std::endl;
    file << "stdp_tau = 20.0" << std::endl;
    file << std::endl;
    file << "# Optional Features" << std::endl;
    file << "enable_checkpointing = false" << std::endl;
    file << "enable_replay = false" << std::endl;
    file << "enable_development = false" << std::endl;
    file << "enable_curiosity = false" << std::endl;
    file << "enable_reward_modulation = false" << std::endl;

    file.close();
    return true;
}

bool NLMDevTools::validateConfigurationSchema(const std::string& schemaFile) const {
    std::ifstream file(schemaFile);
    if (!file.is_open()) {
        return false;
    }

    // In a real implementation, this would validate against a schema
    // This is a placeholder
    std::string line;
    while (std::getline(file, line)) {
        // Parse schema
    }

    file.close();
    return true;
}

std::map<std::string, std::string> NLMDevTools::analyzeConfiguration(std::shared_ptr<Config> config) const {
    std::map<std::string, std::string> analysis;

    if (!config) {
        analysis["error"] = "Invalid configuration pointer";
        return analysis;
    }

    analysis["total_keys"] = std::to_string(config->getKeys().size());
    analysis["config_summary"] = config->summary();

    // Check for common configuration patterns
    auto keys = config->getKeys();
    size_t brainKeys = 0, plasticityKeys = 0, neuromodulationKeys = 0;

    for (const auto& key : keys) {
        if (key.find("brain.") == 0) brainKeys++;
        else if (key.find("plasticity.") == 0) plasticityKeys++;
        else if (key.find("neuromod.") == 0) neuromodulationKeys++;
    }

    analysis["brain_settings"] = std::to_string(brainKeys);
    analysis["plasticity_settings"] = std::to_string(plasticityKeys);
    analysis["neuromodulation_settings"] = std::to_string(n NeuromodulationKeys);

    return analysis;
}
