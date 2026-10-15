#include "reproducibility_manager.hpp"
#include "enhanced_experiments.hpp"
#include "experiment_factory.hpp"
#include "../core/Logger/Logger.hpp"
#include <nlohmann/json.hpp>
#include <sstream>

using json = nlohmann::json;

namespace nlm {

// ReproducibilityManager implementation
ReproducibilityManager::ExperimentSnapshot ReproducibilityManager::createSnapshot(
    std::shared_ptr<BaseExperiment> experiment) {
    ExperimentSnapshot snapshot;
    
    snapshot.experimentName = experiment->getName();
    snapshot.timestamp = ExperimentUtils::getCurrentTimestamp();
    snapshot.gitCommit = getCurrentGitCommit();
    
    // Create config hash
    if (config_) {
        json configJson;
        for (const auto& key : config_->getKeys()) {
            configJson[key] = config_->getValue(key);
        }
        snapshot.configHash = std::to_string(std::hash<std::string>{}(configJson.dump()));
    } else {
        snapshot.configHash = "no_config";
    }
    
    // Save experiment parameters
    auto params = experiment->getParameters();
    json paramsJson;
    for (const auto& param : params) {
        paramsJson[param.first] = param.second;
    }
    snapshot.experimentParameters = paramsJson.dump();
    
    // Save neural network state (placeholder)
    snapshot.neuralNetworkState = "neural_network_state_placeholder";
    
    // Save results
    auto metrics = experiment->getMetrics();
    json resultsJson;
    for (const auto& metric : metrics) {
        resultsJson[metric.first] = metric.second;
    }
    snapshot.results = resultsJson.dump();
    
    // Save dependencies (placeholder)
    snapshot.dependencies = "dependencies_placeholder";
    
    // Store snapshot
    std::lock_guard<std::mutex> lock(mutex_);
    currentSnapshots_[snapshot.experimentName] = snapshot;
    
    return snapshot;
}

bool ReproducibilityManager::restoreSnapshot(const ExperimentSnapshot& snapshot,
                                             std::shared_ptr<BaseExperiment>& experiment) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    try {
        // Create experiment from snapshot
        ExperimentFactory factory(config_);
        
        // Parse experiment parameters
        json paramsJson = json::parse(snapshot.experimentParameters);
        
        // Try to find experiment type (in real implementation, this would be stored in snapshot)
        std::string experimentType = "plasticity"; // Placeholder
        
        experiment = factory.createExperiment(experimentType, snapshot.experimentName);
        if (!experiment) {
            return false;
        }
        
        // Set parameters
        for (const auto& item : paramsJson.items()) {
            experiment->setParameter(item.key(), item.value().get<std::string>());
        }
        
        // Load experiment state (placeholder)
        // In real implementation, would load from neural network state file
        
        Logger::getInstance().log("Restored experiment from snapshot: " + snapshot.experimentName, LogLevel::Info);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error restoring snapshot: " << e.what() << std::endl;
        return false;
    }
}

std::string ReproducibilityManager::createVersion(const std::string& experimentName, const std::string& description) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::string versionId = generateVersionId();
    
    ExperimentVersion version;
    version.versionId = versionId;
    version.timestamp = ExperimentUtils::getCurrentTimestamp();
    version.description = description;
    
    // Find parent version (latest version)
    auto it = versionHistory_.find(experimentName);
    if (it != versionHistory_.end() && !it->second.empty()) {
        version.parentVersion = it->second.back().versionId;
    }
    
    // Get current snapshot
    if (currentSnapshots_.find(experimentName) != currentSnapshots_.end()) {
        version.snapshot = currentSnapshots_[experimentName];
    }
    
    // Add version to history
    versionHistory_[experimentName].push_back(version);
    
    Logger::getInstance().log("Created version " + versionId + " for experiment: " + experimentName, LogLevel::Info);
    
    return versionId;
}

ReproducibilityManager::VersionComparison ReproducibilityManager::compareVersions(
    const std::string& version1, const std::string& version2) {
    VersionComparison comparison;
    comparison.version1 = version1;
    comparison.version2 = version2;
    comparison.similarityScore = 0.0;
    
    // In real implementation, would compare actual snapshots
    comparison.differences.push_back("Config hash difference");
    comparison.differences.push_back("Neural network state difference");
    comparison.differences.push_back("Results difference");
    
    return comparison;
}

std::vector<std::string> ReproducibilityManager::getVersionHistory(const std::string& experimentName) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = versionHistory_.find(experimentName);
    if (it != versionHistory_.end()) {
        std::vector<std::string> versions;
        for (const auto& version : it->second) {
            versions.push_back(version.versionId);
        }
        return versions;
    }
    
    return std::vector<std::string>();
}

ReproducibilityManager::ExperimentSnapshot ReproducibilityManager::getSnapshot(
    const std::string& experimentName, const std::string& version) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (version == "latest") {
        auto it = currentSnapshots_.find(experimentName);
        if (it != currentSnapshots_.end()) {
            return it->second;
        }
    } else {
        auto it = versionHistory_.find(experimentName);
        if (it != versionHistory_.end()) {
            for (const auto& versionInfo : it->second) {
                if (versionInfo.versionId == version) {
                    return versionInfo.snapshot;
                }
            }
        }
    }
    
    // Return empty snapshot if not found
    ExperimentSnapshot emptySnapshot;
    emptySnapshot.experimentName = experimentName;
    return emptySnapshot;
}

void ReproducibilityManager::cleanupOldVersions(size_t maxVersionsToKeep) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (auto& pair : versionHistory_) {
        if (pair.second.size() > maxVersionsToKeep) {
            // Remove oldest versions
            size_t toRemove = pair.second.size() - maxVersionsToKeep;
            pair.second.erase(pair.second.begin(), pair.second.begin() + toRemove);
        }
    }
}

bool ReproducibilityManager::exportExperiment(const std::string& experimentName,
                                              const std::string& outputPath,
                                              const std::string& format) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto snapshotIt = currentSnapshots_.find(experimentName);
    if (snapshotIt == currentSnapshots_.end()) {
        return false;
    }
    
    try {
        if (format == "json") {
            json j;
            j["experiment_name"] = snapshotIt->second.experimentName;
            j["timestamp"] = snapshotIt->second.timestamp;
            j["git_commit"] = snapshotIt->second.gitCommit;
            j["config_hash"] = snapshotIt->second.configHash;
            j["experiment_parameters"] = json::parse(snapshotIt->second.experimentParameters);
            j["neural_network_state"] = snapshotIt->second.neuralNetworkState;
            j["results"] = json::parse(snapshotIt->second.results);
            j["dependencies"] = snapshotIt->second.dependencies;
            
            std::ofstream file(outputPath);
            if (file.is_open()) {
                file << j.dump(2);
                file.close();
                return true;
            }
        } else if (format == "csv") {
            // Generate CSV from results
            json resultsJson = json::parse(snapshotIt->second.results);
            std::ofstream file(outputPath);
            
            if (file.is_open()) {
                file << "Metric,Value" << std::endl;
                for (const auto& item : resultsJson.items()) {
                    file << item.key() << "," << item.value() << std::endl;
                }
                file.close();
                return true;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error exporting experiment: " << e.what() << std::endl;
    }
    
    return false;
}

bool ReproducibilityManager::importExperiment(const std::string& inputPath, const std::string& versionDescription) {
    try {
        std::ifstream file(inputPath);
        if (!file.is_open()) {
            return false;
        }
        
        json j;
        file >> j;
        
        std::string experimentName = j["experiment_name"].get<std::string>();
        
        ExperimentSnapshot snapshot;
        snapshot.experimentName = experimentName;
        snapshot.timestamp = j["timestamp"].get<std::string>();
        snapshot.gitCommit = j["git_commit"].get<std::string>();
        snapshot.configHash = j["config_hash"].get<std::string>();
        snapshot.experimentParameters = j["experiment_parameters"].dump();
        snapshot.neuralNetworkState = j["neural_network_state"].get<std::string>();
        snapshot.results = j["results"].dump();
        snapshot.dependencies = j["dependencies"].get<std::string>();
        
        std::lock_guard<std::mutex> lock(mutex_);
        currentSnapshots_[experimentName] = snapshot;
        
        // Create version
        createVersion(experimentName, versionDescription);
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error importing experiment: " << e.what() << std::endl;
        return false;
    }
}

std::string ReproducibilityManager::generateVersionId() const {
    // Generate a simple version ID
    static int versionCounter = 0;
    return "v" + std::to_string(++versionCounter) + "_" + 
           std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::system_clock::now().time_since_epoch()).count());
}

std::string ReproducibilityManager::generateConfigHash(const std::map<std::string, std::string>& config) const {
    json j = config;
    return std::to_string(std::hash<std::string>{}(j.dump()));
}

std::string ReproducibilityManager::getCurrentGitCommit() const {
    // Try to get git commit (placeholder)
    return "git_commit_placeholder";
}

} // namespace nlm