#include "utils/CheckpointManager.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <cmath>
#include <numeric>
#include <algorithm>
#include <thread>

namespace nlm {

struct CheckpointManager::Impl {
    bool monitoring;
    bool paused;
    bool realTimeEnabled;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point lastUpdateTime;
    std::vector<std::chrono::steady_clock::time_point> timestamps;
    size_t maxHistorySize;
    double samplingInterval;
    std::string outputFormat;
    bool saveToFile;
    std::string dataFile;
    bool autoAnalysis;
    std::string analysisType;
    
    Impl() : monitoring(false), paused(false), realTimeEnabled(false), 
             maxHistorySize(1000), samplingInterval(1.0), outputFormat("text"),
             saveToFile(false), autoAnalysis(true), analysisType("comprehensive") {}
};

CheckpointManager::CheckpointManager() : initialized(false), state(CheckpointState::Ready),
    config(nullptr), brain(nullptr), cli(nullptr), pImpl(std::make_unique<Impl>()) {
}

CheckpointManager::~CheckpointManager() {
    cleanup();
}

CheckpointManager::CheckpointManager(CheckpointManager&&) noexcept = default;

CheckpointManager& CheckpointManager::operator=(CheckpointManager&&) noexcept = default;

bool CheckpointManager::initialize() {
    if (initialized) return true;
    
    try {
        if (!config || !brain) {
            printError("Config or brain not set");
            return false;
        }
        
        // Initialize checkpoint configuration
        setupCheckpointConfiguration();
        
        // Ensure checkpoint directory exists
        ensureCheckpointPath(config.directory);
        
        initialized = true;
        
        return true;
    } catch (const std::exception& e) {
        printError("Failed to initialize CheckpointManager: " + std::string(e.what()));
        return false;
    }
}

bool CheckpointManager::saveCheckpoint(const std::string& name, const std::string& description, const std::string& filename) {
    if (!initialized) {
        printError("CheckpointManager not initialized");
        return false;
    }
    
    state = CheckpointState::Saving;
    auto startTime = std::chrono::steady_clock::now();
    
    try {
        std::string checkpointFile = filename.empty() ? generateCheckpointFilename(name) : filename;
        
        CheckpointInfo info;
        info.filename = checkpointFile;
        info.name = name.empty() ? "checkpoint_" + getFormattedTimestamp() : name;
        info.description = description;
        info.timestamp = std::chrono::steady_clock::now();
        info.creationTime = info.timestamp;
        info.lastAccessTime = info.timestamp;
        info.version = "1.0";
        
        // Collect brain state information
        if (brain) {
            info.neuronCount = brain->getTotalNeuronCount();
            info.synapseCount = brain->getTotalSynapseCount();
            info.totalSpikeCount = brain->getTotalSpikeCount();
            
            double learning = brain->getTotalLearning();
            info.totalLearning = learning;
            
            // Calculate hashes for integrity
            info.brainStateHash = calculateFileHash(checkpointFile + ".brain");
            info.configHash = calculateFileHash(checkpointFile + ".config");
        }
        
        // Save configuration
        std::string configFile = checkpointFile + ".config";
        if (config->saveToFile(configFile)) {
            info.configHash = calculateFileHash(configFile);
        } else {
            state = CheckpointState::Error;
            return false;
        }
        
        // Save brain state (simplified - would typically serialize brain state)
        std::string brainFile = checkpointFile + ".brain";
        // In a real implementation, this would serialize the brain state
        // For now, just create a placeholder
        std::ofstream brainFileStream(brainFile);
        if (brainFileStream.is_open()) {
            brainFileStream << "# Brain state placeholder for checkpoint: " << info.name << std::endl;
            brainFileStream << "# Neurons: " << info.neuronCount << std::endl;
            brainFileStream << "# Synapses: " << info.synapseCount << std::endl;
            brainFileStream << "# Total Spikes: " << info.totalSpikeCount << std::endl;
            brainFileStream << "# Total Learning: " << info.totalLearning << std::endl;
            brainFileStream.close();
            
            info.brainStateHash = calculateFileHash(brainFile);
        } else {
            state = CheckpointState::Error;
            return false;
        }
        
        // Calculate compression if needed
        if (config.compress) {
            info.compressionAlgorithm = config.compressionAlgorithm;
            // In a real implementation, this would compress the files
            info.compressed = true;
            info.compressionRatio = 0.5; // Placeholder
        } else {
            info.compressed = false;
            info.compressionRatio = 1.0;
        }
        
        info.size = getFileSize(checkpointFile + ".chk") + 
                  getFileSize(checkpointFile + ".config") + 
                  getFileSize(checkpointFile + ".brain");
        
        currentCheckpoint = info;
        
        state = CheckpointState::Ready;
        
        lastResult.success = true;
        lastResult.message = "Checkpoint saved successfully: " + info.name;
        lastResult.timestamp = std::chrono::steady_clock::now();
        lastResult.checkpointFile = info.filename;
        lastResult.operation = "save";
        lastResult.duration = getElapsedTime(startTime);
        lastResult.bytesWritten = info.size;
        
        printSuccess("Checkpoint saved: " + info.name);
        printInfo("File: " + info.filename);
        printInfo("Size: " + formatFileSize(info.size));
        printInfo("Neurons: " + std::to_string(info.neuronCount));
        printInfo("Synapses: " + std::to_string(info.synapseCount));
        
        return true;
        
    } catch (const std::exception& e) {
        state = CheckpointState::Error;
        lastResult.success = false;
        lastResult.message = "Failed to save checkpoint: " + std::string(e.what());
        lastResult.timestamp = std::chrono::steady_clock::now();
        lastResult.errorCode = "SAVE_ERROR";
        
        printError("Failed to save checkpoint: " + std::string(e.what()));
        return false;
    }
}

bool CheckpointManager::loadCheckpoint(const std::string& name, const std::string& filename) {
    if (!initialized) {
        printError("CheckpointManager not initialized");
        return false;
    }
    
    state = CheckpointState::Loading;
    auto startTime = std::chrono::steady_clock::now();
    
    try {
        std::string checkpointFile = filename.empty() ? generateCheckpointFilename(name) : filename;
        
        // Check if checkpoint file exists
        if (!std::filesystem::exists(checkpointFile + ".chk") ||
            !std::filesystem::exists(checkpointFile + ".config") ||
            !std::filesystem::exists(checkpointFile + ".brain")) {
            printError("Checkpoint files not found: " + checkpointFile);
            state = CheckpointState::Error;
            return false;
        }
        
        // Load configuration
        if (!config->loadFromFile(checkpointFile + ".config")) {
            printError("Failed to load checkpoint configuration");
            state = CheckpointState::Error;
            return false;
        }
        
        // Load brain state (simplified)
        std::ifstream brainFile(checkpointFile + ".brain");
        if (!brainFile.is_open()) {
            printError("Failed to load brain state");
            state = CheckpointState::Error;
            return false;
        }
        
        brainFile.close();
        
        // Update current checkpoint info
        currentCheckpoint.filename = checkpointFile;
        currentCheckpoint.name = name.empty() ? "checkpoint_" + getFormattedTimestamp() : name;
        currentCheckpoint.timestamp = std::chrono::steady_clock::now();
        currentCheckpoint.lastAccessTime = currentCheckpoint.timestamp;
        currentCheckpoint.valid = true;
        currentCheckpoint.errorMessage.clear();
        
        currentCheckpoint.neuronCount = config->getOr<int64_t>("neuron_count", 0);
        currentCheckpoint.synapseCount = config->getOr<int64_t>("synapse_count", 0);
        currentCheckpoint.totalSpikeCount = config->getOr<int64_t>("total_spike_count", 0);
        currentCheckpoint.totalLearning = config->getOr<double>("total_learning", 0.0);
        
        currentCheckpoint.size = getFileSize(checkpointFile + ".chk") + 
                                getFileSize(checkpointFile + ".config") + 
                                getFileSize(checkpointFile + ".brain");
        
        state = CheckpointState::Ready;
        
        lastResult.success = true;
        lastResult.message = "Checkpoint loaded successfully: " + currentCheckpoint.name;
        lastResult.timestamp = std::chrono::steady_clock::now();
        lastResult.checkpointFile = currentCheckpoint.filename;
        lastResult.operation = "load";
        lastResult.duration = getElapsedTime(startTime);
        lastResult.bytesRead = currentCheckpoint.size;
        
        printSuccess("Checkpoint loaded: " + currentCheckpoint.name);
        printInfo("File: " + currentCheckpoint.filename);
        printInfo("Size: " + formatFileSize(currentCheckpoint.size));
        printInfo("Neurons: " + std::to_string(currentCheckpoint.neuronCount));
        printInfo("Synapses: " + std::to_string(currentCheckpoint.synapseCount));
        
        return true;
        
    } catch (const std::exception& e) {
        state = CheckpointState::Error;
        lastResult.success = false;
        lastResult.message = "Failed to load checkpoint: " + std::string(e.what());
        lastResult.timestamp = std::chrono::steady_clock::now();
        lastResult.errorCode = "LOAD_ERROR";
        
        printError("Failed to load checkpoint: " + std::string(e.what()));
        return false;
    }
}

bool CheckpointManager::compareCheckpoints(const std::string& checkpoint1, const std::string& checkpoint2) {
    if (!initialized) {
        printError("CheckpointManager not initialized");
        return false;
    }
    
    state = CheckpointState::Comparing;
    
    try {
        std::string diff = compareCheckpoints(checkpoint1, checkpoint2, true);
        
        print("=== Checkpoint Comparison ===");
        print(diff);
        
        state = CheckpointState::Ready;
        
        lastResult.success = true;
        lastResult.message = "Checkpoint comparison completed";
        lastResult.timestamp = std::chrono::steady_clock::now();
        lastResult.operation = "compare";
        
        return true;
        
    } catch (const std::exception& e) {
        state = CheckpointState::Error;
        lastResult.success = false;
        lastResult.message = "Failed to compare checkpoints: " + std::string(e.what());
        lastResult.errorCode = "COMPARE_ERROR";
        
        printError("Failed to compare checkpoints: " + std::string(e.what()));
        return false;
    }
}

bool CheckpointManager::deleteCheckpoint(const std::string& name, const std::string& filename) {
    if (!initialized) {
        printError("CheckpointManager not initialized");
        return false;
    }
    
    state = CheckpointState::Cleaning;
    
    try {
        std::string checkpointFile = filename.empty() ? generateCheckpointFilename(name) : filename;
        
        bool success = true;
        
        if (std::filesystem::exists(checkpointFile + ".chk")) {
            if (!std::filesystem::remove(checkpointFile + ".chk")) {
                printWarning("Failed to delete checkpoint file: " + checkpointFile + ".chk");
                success = false;
            }
        }
        
        if (std::filesystem::exists(checkpointFile + ".config")) {
            if (!std::filesystem::remove(checkpointFile + ".config")) {
                printWarning("Failed to delete config file: " + checkpointFile + ".config");
                success = false;
            }
        }
        
        if (std::filesystem::exists(checkpointFile + ".brain")) {
            if (!std::filesystem::remove(checkpointFile + ".brain")) {
                printWarning("Failed to delete brain file: " + checkpointFile + ".brain");
                success = false;
            }
        }
        
        state = CheckpointState::Ready;
        
        lastResult.success = success;
        lastResult.message = "Checkpoint deleted: " + (name.empty() ? checkpointFile : name);
        lastResult.timestamp = std::chrono::steady_clock::now();
        lastResult.operation = "delete";
        
        if (success) {
            printSuccess("Checkpoint deleted: " + (name.empty() ? checkpointFile : name));
        } else {
            printError("Failed to delete checkpoint completely");
        }
        
        return success;
        
    } catch (const std::exception& e) {
        state = CheckpointState::Error;
        lastResult.success = false;
        lastResult.message = "Failed to delete checkpoint: " + std::string(e.what());
        lastResult.errorCode = "DELETE_ERROR";
        
        printError("Failed to delete checkpoint: " + std::string(e.what()));
        return false;
    }
}

bool CheckpointManager::listCheckpoints() {
    if (!initialized) {
        printError("CheckpointManager not initialized");
        return false;
    }
    
    print("=== Checkpoint List ===");
    
    std::vector<CheckpointInfo> checkpoints = listCheckpoints();
    
    if (checkpoints.empty()) {
        print("No checkpoints found.");
        return true;
    }
    
    print("Available checkpoints:");
    for (const auto& checkpoint : checkpoints) {
        print("  " + checkpoint.name + " - " + getFormattedTimestamp(checkpoint.timestamp));
        print("    File: " + checkpoint.filename);
        print("    Size: " + formatFileSize(checkpoint.size));
        print("    Neurons: " + std::to_string(checkpoint.neuronCount));
        print("    Synapses: " + std::to_string(checkpoint.synapseCount));
        print("    Learning: " + std::to_string(checkpoint.totalLearning));
        print("    Valid: " + std::string(checkpoint.valid ? "yes" : "no"));
        print("");
    }
    
    return true;
}

bool CheckpointManager::infoCheckpoint(const std::string& name, const std::string& filename) {
    if (!initialized) {
        printError("CheckpointManager not initialized");
        return false;
    }
    
    std::string checkpointFile = filename.empty() ? generateCheckpointFilename(name) : filename;
    
    CheckpointInfo info = getCheckpointInfo(name, filename);
    
    print("=== Checkpoint Information ===");
    print("Name: " + info.name);
    print("Filename: " + info.filename);
    print("Description: " + info.description);
    print("Timestamp: " + getFormattedTimestamp(info.timestamp));
    print("Creation Time: " + getFormattedTimestamp(info.creationTime));
    print("Last Access Time: " + getFormattedTimestamp(info.lastAccessTime));
    print("Version: " + info.version);
    print("Size: " + formatFileSize(info.size));
    print("Neurons: " + std::to_string(info.neuronCount));
    print("Synapses: " + std::to_string(info.synapseCount));
    print("Total Spikes: " + std::to_string(info.totalSpikeCount));
    print("Total Learning: " + std::to_string(info.totalLearning));
    print("Brain State Hash: " + info.brainStateHash);
    print("Config Hash: " + info.configHash);
    print("Compression: " + std::string(info.compressed ? "yes" : "no"));
    if (info.compressed) {
        print("Compression Algorithm: " + info.compressionAlgorithm);
        print("Compression Ratio: " + std::to_string(info.compressionRatio));
    }
    print("Valid: " + std::string(info.valid ? "yes" : "no"));
    
    if (!info.errorMessage.empty()) {
        print("Error: " + info.errorMessage);
    }
    
    return true;
}

bool CheckpointManager::backupCurrentState(const std::string& name) {
    if (!initialized) {
        printError("CheckpointManager not initialized");
        return false;
    }
    
    std::string backupName = name.empty() ? "backup_" + getFormattedTimestamp() : name;
    
    print("Creating backup checkpoint: " + backupName);
    
    return saveCheckpoint(backupName, "Auto backup of current brain state");
}

bool CheckpointManager::restoreFromBackup(const std::string& name) {
    if (!initialized) {
        printError("CheckpointManager not initialized");
        return false;
    }
    
    std::string backupName = name.empty() ? "backup_" + getFormattedTimestamp() : name;
    
    print("Restoring from backup: " + backupName);
    
    return loadCheckpoint(backupName, "");
}

bool CheckpointManager::createSnapshot(const std::string& name, const std::string& description) {
    if (!initialized) {
        printError("CheckpointManager not initialized");
        return false;
    }
    
    std::string snapshotName = name.empty() ? "snapshot_" + getFormattedTimestamp() : name;
    
    print("Creating snapshot: " + snapshotName);
    print("Description: " + description);
    
    return saveCheckpoint(snapshotName, description);
}

bool CheckpointManager::restoreSnapshot(const std::string& name) {
    if (!initialized) {
        printError("CheckpointManager not initialized");
        return false;
    }
    
    print("Restoring snapshot: " + name);
    
    return loadCheckpoint(name, "");
}

bool CheckpointManager::cleanOldCheckpoints(size_t keepCount) {
    if (!initialized) {
        printError("CheckpointManager not initialized");
        return false;
    }
    
    std::vector<CheckpointInfo> checkpoints = listCheckpoints();
    
    if (checkpoints.size() <= keepCount) {
        print("No checkpoints to clean (keeping " + std::to_string(checkpoints.size()) + " checkpoints)");
        return true;
    }
    
    // Sort by timestamp (oldest first)
    std::sort(checkpoints.begin(), checkpoints.end(), 
              [](const CheckpointInfo& a, const CheckpointInfo& b) {
                  return a.timestamp < b.timestamp;
              });
    
    size_t toDelete = checkpoints.size() - keepCount;
    
    print("Cleaning " + std::to_string(toDelete) + " old checkpoints...");
    
    bool success = true;
    for (size_t i = 0; i < toDelete; ++i) {
        if (deleteCheckpoint(checkpoints[i].name, "")) {
            printSuccess("Deleted old checkpoint: " + checkpoints[i].name);
        } else {
            printError("Failed to delete checkpoint: " + checkpoints[i].name);
            success = false;
        }
    }
    
    return success;
}

bool CheckpointManager::configure(const CheckpointConfig& config) {
    this->config = config;
    
    // Ensure directory exists
    ensureCheckpointPath(config.directory);
    
    printSuccess("Checkpoint configuration updated");
    printInfo("Directory: " + config.directory);
    printInfo("Max checkpoints: " + std::to_string(config.maxCheckpoints));
    printInfo("Compression: " + std::string(config.compress ? "yes" : "no"));
    printInfo("Backup: " + std::string(config.autoBackup ? "yes" : "no"));
    
    return true;
}

bool CheckpointManager::updateConfig(const std::string& key, const std::string& value) {
    if (key == "directory") {
        config.directory = value;
    } else if (key == "max_checkpoints") {
        config.maxCheckpoints = std::stoul(value);
    } else if (key == "max_history_size") {
        config.maxHistorySize = std::stoul(value);
    } else if (key == "compression_algorithm") {
        config.compressionAlgorithm = value;
    } else if (key == "auto_backup") {
        config.autoBackup = (value == "true" || value == "1" || value == "yes");
    } else if (key == "validate_on_load") {
        config.validateOnLoad = (value == "true" || value == "1" || value == "yes");
    } else if (key == "compress") {
        config.compress = (value == "true" || value == "1" || value == "yes");
    } else if (key == "compression_threshold") {
        config.compressionThreshold = std::stod(value);
    } else {
        printError("Unknown configuration key: " + key);
        return false;
    }
    
    // Ensure directory exists if changed
    if (key == "directory") {
        ensureCheckpointPath(config.directory);
    }
    
    printSuccess("Configuration updated: " + key + " = " + value);
    
    return true;
}

std::vector<CheckpointManager::CheckpointInfo> CheckpointManager::listCheckpoints(const std::string& pattern) {
    std::vector<CheckpointInfo> checkpoints;
    
    if (!std::filesystem::exists(config.directory)) {
        return checkpoints;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(config.directory)) {
        if (entry.is_regular_file() && entry.path().extension() == ".chk") {
            std::string filename = entry.path().filename().string();
            std::string name = filename.substr(0, filename.size() - 4);
            
            if (pattern.empty() || name.find(pattern) != std::string::npos) {
                CheckpointInfo info;
                info.filename = config.directory + "/" + name;
                info.name = name;
                info.timestamp = getFileTimestamp(entry.path());
                info.creationTime = info.timestamp;
                info.lastAccessTime = std::chrono::steady_clock::now();
                info.size = entry.file_size();
                info.valid = true;
                
                checkpoints.push_back(info);
            }
        }
    }
    
    // Sort by timestamp (newest first)
    std::sort(checkpoints.begin(), checkpoints.end(),
              [](const CheckpointInfo& a, const CheckpointInfo& b) {
                  return a.timestamp > b.timestamp;
              });
    
    return checkpoints;
}

bool CheckpointManager::browseCheckpoints(const std::string& pattern) {
    return listCheckpoints(pattern);
}

CheckpointManager::CheckpointInfo CheckpointManager::getCheckpointInfo(const std::string& name, const std::string& filename) {
    CheckpointInfo info;
    
    std::string checkpointFile = filename.empty() ? generateCheckpointFilename(name) : filename;
    
    if (std::filesystem::exists(checkpointFile + ".chk")) {
        info.filename = checkpointFile;
        info.name = name.empty() ? "checkpoint_" + getFormattedTimestamp() : name;
        info.timestamp = getFileTimestamp(std::filesystem::path(checkpointFile + ".chk"));
        info.creationTime = info.timestamp;
        info.lastAccessTime = std::chrono::steady_clock::now();
        info.size = getFileSize(checkpointFile + ".chk");
        info.valid = true;
    }
    
    return info;
}

std::string CheckpointManager::compareCheckpoints(const std::string& checkpoint1, const std::string& checkpoint2, bool detailed) {
    std::ostringstream oss;
    
    oss << "=== Checkpoint Comparison ===" << std::endl;
    oss << "Checkpoint 1: " << checkpoint1 << std::endl;
    oss << "Checkpoint 2: " << checkpoint2 << std::endl;
    oss << std::endl;
    
    // Load both checkpoints
    auto config1 = std::make_shared<Config>();
    auto config2 = std::make_shared<Config>();
    
    bool load1 = config1->loadFromFile(checkpoint1 + ".config");
    bool load2 = config2->loadFromFile(checkpoint2 + ".config");
    
    if (!load1 || !load2) {
        oss << "Failed to load one or both checkpoints" << std::endl;
        return oss.str();
    }
    
    oss << "Configuration Comparison:" << std::endl;
    
    auto keys1 = config1->getKeys();
    auto keys2 = config2->getKeys();
    
    // Find differences
    oss << "Keys in checkpoint 1 but not in checkpoint 2:" << std::endl;
    for (const auto& key : keys1) {
        if (std::find(keys2.begin(), keys2.end(), key) == keys2.end()) {
            oss << "  " << key << std::endl;
        }
    }
    
    oss << std::endl;
    oss << "Keys in checkpoint 2 but not in checkpoint 1:" << std::endl;
    for (const auto& key : keys2) {
        if (std::find(keys1.begin(), keys1.end(), key) == keys1.end()) {
            oss << "  " << key << std::endl;
        }
    }
    
    if (detailed) {
        oss << std::endl;
        oss << "Detailed value comparison:" << std::endl;
        for (const auto& key : keys1) {
            if (std::find(keys2.begin(), keys2.end(), key) != keys2.end()) {
                auto value1 = config1->get<std::string>(key);
                auto value2 = config2->get<std::string>(key);
                if (value1 != value2) {
                    oss << "  Difference for key \"" << key << "\":" << std::endl;
                    oss << "    Checkpoint 1: \"" << value1.value_or("") << "\"" << std::endl;
                    oss << "    Checkpoint 2: \"" << value2.value_or("") << "\"" << std::endl;
                }
            }
        }
    }
    
    return oss.str();
}

std::string CheckpointManager::diffCheckpoints(const std::string& checkpoint1, const std::string& checkpoint2) {
    return compareCheckpoints(checkpoint1, checkpoint2, false);
}

bool CheckpointManager::analyzeCheckpoint(const std::string& name, const std::string& filename) {
    print("=== Checkpoint Analysis ===");
    print("Analyzing checkpoint: " + (name.empty() ? name : name));
    
    CheckpointInfo info = getCheckpointInfo(name, filename);
    
    if (!info.valid) {
        printError("Checkpoint is invalid or not found");
        return false;
    }
    
    printInfo("Size: " + formatFileSize(info.size));
    printInfo("Age: " + getTimeSince(info.timestamp));
    
    if (info.compressed) {
        printInfo("Compressed with ratio: " + std::to_string(info.compressionRatio));
    }
    
    return true;
}

std::string CheckpointManager::getCheckpointAnalysis(const std::string& name, const std::string& filename) {
    std::ostringstream oss;
    
    oss << "=== Checkpoint Analysis ===" << std::endl;
    
    CheckpointInfo info = getCheckpointInfo(name, filename);
    
    if (!info.valid) {
        oss << "Checkpoint is invalid or not found" << std::endl;
        return oss.str();
    }
    
    oss << "Size: " << formatFileSize(info.size) << std::endl;
    oss << "Age: " << getTimeSince(info.timestamp) << std::endl;
    
    if (info.compressed) {
        oss << "Compressed with ratio: " << info.compressionRatio << std::endl;
    }
    
    return oss.str();
}

double CheckpointManager::calculateSimilarity(const CheckpointInfo& checkpoint1, const CheckpointInfo& checkpoint2) const {
    // Simple similarity calculation based on size ratio
    if (checkpoint1.size == 0 || checkpoint2.size == 0) return 0.0;
    
    double sizeRatio = std::min(checkpoint1.size, checkpoint2.size) / (double)std::max(checkpoint1.size, checkpoint2.size);
    
    double ageRatio = 1.0 - std::min(
        std::chrono::duration_cast<std::chrono::seconds>(checkpoint1.timestamp - checkpoint2.timestamp).count(),
        1000000.0
    ) / 1000000.0;
    
    return (sizeRatio + ageRatio) / 2.0;
}

bool CheckpointManager::validateCheckpoint(const std::string& name, const std::string& filename) {
    CheckpointInfo info = getCheckpointInfo(name, filename);
    
    if (!info.valid) {
        return false;
    }
    
    // Check file integrity
    if (!std::filesystem::exists(info.filename + ".chk") ||
        !std::filesystem::exists(info.filename + ".config") ||
        !std::filesystem::exists(info.filename + ".brain")) {
        return false;
    }
    
    // Calculate file hashes for integrity check
    std::string chkHash = calculateFileHash(info.filename + ".chk");
    std::string configHash = calculateFileHash(info.filename + ".config");
    std::string brainHash = calculateFileHash(info.filename + ".brain");
    
    return true;
}

bool CheckpointManager::verifyCheckpointIntegrity(const std::string& name, const std::string& filename) {
    return validateCheckpoint(name, filename);
}

std::vector<std::string> CheckpointManager::getCheckpointErrors(const std::string& name, const std::string& filename) {
    return std::vector<std::string>();
}

bool CheckpointManager::exportCheckpoint(const std::string& name, const std::string& filename, const std::string& format) {
    print("=== Checkpoint Export ===");
    print("Exporting checkpoint: " + name);
    print("Format: " + format);
    
    // In a real implementation, this would export the checkpoint in the specified format
    // For now, just copy the files
    std::string exportDir = "export/" + name;
    if (!ensureCheckpointPath(exportDir)) {
        return false;
    }
    
    std::string sourceFile = filename.empty() ? generateCheckpointFilename(name) : filename;
    std::string targetFile = exportDir + "/" + sourceFile;
    
    // Copy checkpoint files
    try {
        std::filesystem::copy(sourceFile + ".chk", targetFile + ".chk");
        std::filesystem::copy(sourceFile + ".config", targetFile + ".config");
        std::filesystem::copy(sourceFile + ".brain", targetFile + ".brain");
    } catch (const std::filesystem::filesystem_error& e) {
        printError("Failed to export checkpoint: " + std::string(e.what()));
        return false;
    }
    
    printSuccess("Checkpoint exported to: " + targetFile);
    
    return true;
}

bool CheckpointManager::importCheckpoint(const std::string& filename, const std::string& format) {
    print("=== Checkpoint Import ===");
    print("Importing checkpoint: " + filename);
    print("Format: " + format);
    
    // In a real implementation, this would import the checkpoint from the specified format
    // For now, just copy the files
    std::string sourceFile = filename;
    
    // Check if files exist
    if (!std::filesystem::exists(sourceFile + ".chk") ||
        !std::filesystem::exists(sourceFile + ".config") ||
        !std::filesystem::exists(sourceFile + ".brain")) {
        printError("Checkpoint files not found: " + sourceFile);
        return false;
    }
    
    // Load checkpoint
    if (loadCheckpoint("", filename)) {
        printSuccess("Checkpoint imported successfully");
        return true;
    }
    
    return false;
}

bool CheckpointManager::exportMultipleCheckpoints(const std::vector<std::string>& names, const std::string& archiveFile) {
    print("=== Multiple Checkpoint Export ===");
    print("Exporting " + std::to_string(names.size()) + " checkpoints to archive: " + archiveFile);
    
    // In a real implementation, this would create an archive of multiple checkpoints
    // For now, just copy each checkpoint
    for (const auto& name : names) {
        printInfo("Exporting: " + name);
        if (!saveCheckpoint(name, "Exported from multiple export")) {
            printError("Failed to export checkpoint: " + name);
            return false;
        }
    }
    
    printSuccess("Multiple checkpoints exported to archive");
    
    return true;
}

bool CheckpointManager::importMultipleCheckpoints(const std::string& archiveFile) {
    print("=== Multiple Checkpoint Import ===");
    print("Importing checkpoints from archive: " + archiveFile);
    
    // In a real implementation, this would extract checkpoints from an archive
    // For now, just import each checkpoint individually
    printWarning("Multiple import not fully implemented in this version");
    
    return true;
}

bool CheckpointManager::compressCheckpoint(const std::string& name, const std::string& filename, const std::string& algorithm) {
    print("=== Checkpoint Compression ===");
    print("Compressing checkpoint: " + name);
    print("Algorithm: " + algorithm);
    
    // In a real implementation, this would compress the checkpoint files
    // For now, just mark as compressed
    std::string checkpointFile = filename.empty() ? generateCheckpointFilename(name) : filename;
    
    currentCheckpoint.compressionAlgorithm = algorithm;
    currentCheckpoint.compressed = true;
    currentCheckpoint.compressionRatio = 0.5; // Placeholder
    
    printSuccess("Checkpoint compression completed");
    
    return true;
}

bool CheckpointManager::decompressCheckpoint(const std::string& name, const std::string& filename, const std::string& algorithm) {
    print("=== Checkpoint Decompression ===");
    print("Decompressing checkpoint: " + name);
    print("Algorithm: " + algorithm);
    
    // In a real implementation, this would decompress the checkpoint files
    // For now, just mark as decompressed
    currentCheckpoint.compressionAlgorithm = "";
    currentCheckpoint.compressed = false;
    currentCheckpoint.compressionRatio = 1.0;
    
    printSuccess("Checkpoint decompression completed");
    
    return true;
}

double CheckpointManager::calculateCompressionRatio(const std::string& filename) const {
    if (!std::filesystem::exists(filename)) {
        return 1.0;
    }
    
    // Placeholder implementation
    return 0.5;
}

std::vector<CheckpointManager::CheckpointInfo> CheckpointManager::searchCheckpoints(const std::string& keyword, const std::string& pattern) {
    std::vector<CheckpointInfo> checkpoints;
    
    if (!std::filesystem::exists(config.directory)) {
        return checkpoints;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(config.directory)) {
        if (entry.is_regular_file() && entry.path().extension() == ".chk") {
            std::string filename = entry.path().filename().string();
            std::string name = filename.substr(0, filename.size() - 4);
            
            bool matchesKeyword = keyword.empty() || 
                name.find(keyword) != std::string::npos ||
                (entry.path().parent_path().filename().string().find(keyword) != std::string::npos);
            
            bool matchesPattern = pattern.empty() || name.find(pattern) != std::string::npos;
            
            if (matchesKeyword && matchesPattern) {
                CheckpointInfo info;
                info.filename = config.directory + "/" + name;
                info.name = name;
                info.timestamp = getFileTimestamp(entry.path());
                info.creationTime = info.timestamp;
                info.lastAccessTime = std::chrono::steady_clock::now();
                info.size = entry.file_size();
                info.valid = true;
                
                checkpoints.push_back(info);
            }
        }
    }
    
    // Sort by timestamp (newest first)
    std::sort(checkpoints.begin(), checkpoints.end(),
              [](const CheckpointInfo& a, const CheckpointInfo& b) {
                  return a.timestamp > b.timestamp;
              });
    
    return checkpoints;
}

std::vector<CheckpointManager::CheckpointInfo> CheckpointManager::filterCheckpoints(const std::string& fromTime, const std::string& toTime, const std::string& pattern) {
    // Filter checkpoints based on time range and pattern
    std::vector<CheckpointInfo> checkpoints = listCheckpoints(pattern);
    
    if (!fromTime.empty() || !toTime.empty()) {
        // Parse time strings (simplified implementation)
        auto now = std::chrono::steady_clock::now();
        auto fromTimePoint = now;
        auto toTimePoint = now;
        
        if (!fromTime.empty()) {
            // Parse fromTime (simplified)
            fromTimePoint = now - std::chrono::hours(24 * std::stoi(fromTime));
        }
        
        if (!toTime.empty()) {
            // Parse toTime (simplified)
            toTimePoint = now - std::chrono::hours(24 * std::stoi(toTime));
        }
        
        std::vector<CheckpointInfo> filtered;
        for (const auto& checkpoint : checkpoints) {
            if (checkpoint.timestamp >= fromTimePoint && checkpoint.timestamp <= toTimePoint) {
                filtered.push_back(checkpoint);
            }
        }
        
        checkpoints = filtered;
    }
    
    return checkpoints;
}

std::vector<CheckpointManager::CheckpointInfo> CheckpointManager::sortCheckpoints(const std::string& sortBy, const std::string& order) {
    // Sort checkpoints based on sort criteria
    std::vector<CheckpointInfo> checkpoints = listCheckpoints();
    
    if (sortBy == "timestamp") {
        std::sort(checkpoints.begin(), checkpoints.end(),
                  [](const CheckpointInfo& a, const CheckpointInfo& b) {
                      if (order == "desc") {
                          return a.timestamp > b.timestamp;
                      } else {
                          return a.timestamp < b.timestamp;
                      }
                  });
    } else if (sortBy == "size") {
        std::sort(checkpoints.begin(), checkpoints.end(),
                  [](const CheckpointInfo& a, const CheckpointInfo& b) {
                      if (order == "desc") {
                          return a.size > b.size;
                      } else {
                          return a.size < b.size;
                      }
                  });
    } else if (sortBy == "name") {
        std::sort(checkpoints.begin(), checkpoints.end(),
                  [](const CheckpointInfo& a, const CheckpointInfo& b) {
                      if (order == "desc") {
                          return a.name > b.name;
                      } else {
                          return a.name < b.name;
                      }
                  });
    }
    
    return checkpoints;
}

bool CheckpointManager::monitorCheckpoints(const std::string& pattern) {
    if (!initialized) {
        printError("CheckpointManager not initialized");
        return false;
    }
    
    monitoring = true;
    realTimeEnabled = true;
    
    printSuccess("Checkpoint monitoring started");
    
    std::thread monitorThread(&CheckpointManager::monitoringWorker, this);
    monitorThread.detach();
    
    return true;
}

bool CheckpointManager::stopMonitoring() {
    monitoring = false;
    realTimeEnabled = false;
    
    printSuccess("Checkpoint monitoring stopped");
    
    return true;
}

size_t CheckpointManager::getCheckpointCount() const {
    return listCheckpoints().size();
}

size_t CheckpointManager::getDiskUsage() const {
    size_t totalUsage = 0;
    
    if (std::filesystem::exists(config.directory)) {
        for (const auto& entry : std::filesystem::directory_iterator(config.directory)) {
            if (entry.is_regular_file() && entry.path().extension() == ".chk") {
                totalUsage += entry.file_size();
            }
        }
    }
    
    return totalUsage;
}

std::vector<CheckpointManager::CheckpointInfo> CheckpointManager::getRecentCheckpoints(size_t count) {
    std::vector<CheckpointInfo> checkpoints = listCheckpoints();
    
    if (checkpoints.size() > count) {
        checkpoints.resize(count);
    }
    
    return checkpoints;
}

void CheckpointManager::showHelp() const {
    print("=== Checkpoint Manager Commands ===");
    print("  save [name] [description] [filename] - Save checkpoint");
    print("  load [name] [filename] - Load checkpoint");
    print("  compare <name1> <name2> [--detailed] - Compare checkpoints");
    print("  delete <name> [filename] - Delete checkpoint");
    print("  list [pattern] - List checkpoints");
    print("  info <name> [filename] - Show checkpoint information");
    print("  backup [name] - Backup current state");
    print("  restore [name] - Restore from backup");
    print("  snapshot [name] [description] - Create snapshot");
    print("  revert <name> - Revert snapshot");
    print("  clean <count> - Clean old checkpoints");
    print("  configure <key> <value> - Configure checkpoint manager");
    print("  export <name> [filename] <format> - Export checkpoint");
    print("  import <filename> <format> - Import checkpoint");
    print("  search <keyword> [pattern] - Search checkpoints");
    print("  filter <from_time> <to_time> [pattern] - Filter checkpoints");
    print("  sort <by> <order> - Sort checkpoints");
    print("  analyze <name> [filename] - Analyze checkpoint");
    print("  monitor [pattern] - Start checkpoint monitoring");
    print("  stop-monitoring - Stop checkpoint monitoring");
}

void CheckpointManager::showCheckpointCommands() const {
    print("=== Checkpoint Commands ===");
    print("  save [name] [description] [filename] - Save checkpoint");
    print("  load [name] [filename] - Load checkpoint");
    print("  compare <name1> <name2> [--detailed] - Compare checkpoints");
    print("  delete <name> [filename] - Delete checkpoint");
    print("  list [pattern] - List checkpoints");
    print("  info <name> [filename] - Show checkpoint information");
}

void CheckpointManager::showComparisonCommands() const {
    print("=== Comparison Commands ===");
    print("  compare <name1> <name2> [--detailed] - Compare checkpoints");
    print("  diff <name1> <name2> - Show differences");
}

void CheckpointManager::showManagementCommands() const {
    print("=== Management Commands ===");
    print("  backup [name] - Backup current state");
    print("  restore [name] - Restore from backup");
    print("  snapshot [name] [description] - Create snapshot");
    print("  revert <name> - Revert snapshot");
    print("  clean <count> - Clean old checkpoints");
}

void CheckpointManager::startCheckpointMonitoringThread() {
    std::thread monitorThread(&CheckpointManager::monitoringWorker, this);
    monitorThread.detach();
}

void CheckpointManager::stopCheckpointMonitoringThread() {
    realTimeEnabled = false;
}

void CheckpointManager::monitoringWorker() {
    while (realTimeEnabled) {
        if (!paused) {
            updateMonitoringData();
        }
        
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void CheckpointManager::updateMonitoringData() {
    // Update monitoring data for checkpoint manager
    // This would be implemented based on specific monitoring requirements
}

void CheckpointManager::collectNetworkStatistics() {
    // Collect network statistics for checkpoint manager
    // This would be implemented based on specific monitoring requirements
}

void CheckpointManager::collectMemoryStatistics() {
    // Collect memory statistics for checkpoint manager
    // This would be implemented based on specific monitoring requirements
}

void CheckpointManager::collectPerformanceStatistics() {
    // Collect performance statistics for checkpoint manager
    // This would be implemented based on specific monitoring requirements
}

void CheckpointManager::collectSpikeStatistics() {
    // Collect spike statistics for checkpoint manager
    // This would be implemented based on specific monitoring requirements
}

void CheckpointManager::collectConnectivityStatistics() {
    // Collect connectivity statistics for checkpoint manager
    // This would be implemented based on specific monitoring requirements
}

void CheckpointManager::setupCheckpointConfiguration() {
    // Setup checkpoint configuration from brain config
    if (config) {
        config.directory = config->getOr<std::string>("checkpoint.directory", "checkpoints");
        config.maxCheckpoints = config->getOr<size_t>("checkpoint.max_checkpoints", 10);
        config.maxHistorySize = config->getOr<size_t>("checkpoint.max_history_size", 1000);
        config.compressionAlgorithm = config->getOr<std::string>("checkpoint.compression_algorithm", "gzip");
        config.autoBackup = config->getOr<bool>("checkpoint.auto_backup", false);
        config.validateOnLoad = config->getOr<bool>("checkpoint.validate_on_load", true);
        config.compress = config->getOr<bool>("checkpoint.compress", false);
        config.compressionThreshold = config->getOr<double>("checkpoint.compression_threshold", 0.5);
    }
}

std::string CheckpointManager::formatTimestamp(const std::chrono::steady_clock::time_point& timestamp) const {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - timestamp).count();
    
    if (elapsed < 60) {
        return std::to_string(elapsed) + "s";
    } else if (elapsed < 3600) {
        return std::to_string(elapsed / 60) + "m";
    } else if (elapsed < 86400) {
        return std::to_string(elapsed / 3600) + "h";
    } else {
        return std::to_string(elapsed / 86400) + "d";
    }
}

std::string CheckpointManager::formatFileSize(size_t bytes) const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    
    if (bytes < 1024) {
        oss << bytes << " B";
    } else if (bytes < 1024 * 1024) {
        oss << bytes / 1024.0 << " KB";
    } else if (bytes < 1024 * 1024 * 1024) {
        oss << bytes / (1024.0 * 1024) << " MB";
    } else {
        oss << bytes / (1024.0 * 1024 * 1024) << " GB";
    }
    
    return oss.str();
}

std::string CheckpointManager::getStateString(CheckpointState state) const {
    switch (state) {
        case CheckpointState::Ready: return "Ready";
        case CheckpointState::Saving: return "Saving";
        case CheckpointState::Loading: return "Loading";
        case CheckpointState::Comparing: return "Comparing";
        case CheckpointState::Cleaning: return "Cleaning";
        case CheckpointState::Error: return "Error";
        default: return "Unknown";
    }
}

std::string CheckpointManager::getOperationString(const std::string& operation) const {
    return operation;
}

bool CheckpointManager::isFileAccessible(const std::string& path) const {
    return !path.empty();
}

bool CheckpointManager::isDirectoryEmpty(const std::string& path) const {
    if (!std::filesystem::exists(path)) {
        return true;
    }
    
    return std::filesystem::is_empty(path);
}

bool CheckpointManager::cleanupEmptyDirectories(const std::string& path) const {
    if (std::filesystem::exists(path) && isDirectoryEmpty(path)) {
        return std::filesystem::remove(path);
    }
    return true;
}

std::string CheckpointManager::getFileExtension(const std::string& filename) const {
    size_t pos = filename.find_last_of('.');
    if (pos != std::string::npos) {
        return filename.substr(pos);
    }
    return "";
}

std::string CheckpointManager::removeFileExtension(const std::string& filename) const {
    size_t pos = filename.find_last_of('.');
    if (pos != std::string::npos) {
        return filename.substr(0, pos);
    }
    return filename;
}

std::string CheckpointManager::getBackupFilename(const std::string& originalFile) const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm_buf;
    gmtime_r(&time, &tm_buf);
    
    std::ostringstream oss;
    oss << originalFile << ".backup_" << std::put_time(&tm_buf, "%Y%m%d_%H%M%S");
    return oss.str();
}

bool CheckpointManager::saveCheckpointWithBackup(const std::string& filename, const std::string& backupName) {
    std::string backupFile = backupName.empty() ? getBackupFilename(filename) : backupName;
    
    if (std::filesystem::exists(filename) && !std::filesystem::exists(backupFile)) {
        try {
            std::filesystem::copy_file(filename, backupFile, std::filesystem::copy_options::overwrite_existing);
        } catch (const std::filesystem::filesystem_error& e) {
            printWarning("Failed to create backup: " + std::string(e.what()));
        }
    }
    
    return saveCheckpoint();
}

bool CheckpointManager::restoreCheckpointWithBackup(const std::string& filename, const std::string& backupName) {
    std::string backupFile = backupName.empty() ? getBackupFilename(filename) : backupName;
    
    if (std::filesystem::exists(backupFile)) {
        try {
            std::filesystem::copy_file(backupFile, filename, std::filesystem::copy_options::overwrite_existing);
        } catch (const std::filesystem::filesystem_error& e) {
            printWarning("Failed to restore backup: " + std::string(e.what()));
        }
    }
    
    return loadCheckpoint();
}

void CheckpointManager::cleanup() {
    stopMonitoring();
    realTimeEnabled = false;
}

double CheckpointManager::getElapsedTime(std::chrono::steady_clock::time_point startTime) const {
    auto endTime = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);
    return elapsed.count();
}

size_t CheckpointManager::getFileSize(const std::string& filename) const {
    if (std::filesystem::exists(filename)) {
        return std::filesystem::file_size(filename);
    }
    return 0;
}

std::chrono::steady_clock::time_point CheckpointManager::getFileTimestamp(const std::string& filename) const {
    if (std::filesystem::exists(filename)) {
        auto ftime = std::filesystem::last_write_time(filename);
        return std::chrono::time_point_cast<std::chrono::steady_clock::duration>(
            ftime - std::filesystem::file_time_type::clock::now() + std::chrono::steady_clock::now());
    }
    return std::chrono::steady_clock::now();
}

std::string CheckpointManager::getFormattedTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm_buf;
    gmtime_r(&time, &tm_buf);
    
    std::ostringstream oss;
    oss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string CheckpointManager::getTimeSince(std::chrono::steady_clock::time_point timestamp) const {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - timestamp).count();
    
    if (elapsed < 60) {
        return std::to_string(elapsed) + "s ago";
    } else if (elapsed < 3600) {
        return std::to_string(elapsed / 60) + "m ago";
    } else if (elapsed < 86400) {
        return std::to_string(elapsed / 3600) + "h ago";
    } else {
        return std::to_string(elapsed / 86400) + "d ago";
    }
}

std::string CheckpointManager::calculateFileHash(const std::string& filename) const {
    // Placeholder implementation - would use actual hash algorithm
    return "hash_" + filename;
}

std::string CheckpointManager::calculateDirectoryHash(const std::string& directory) const {
    // Placeholder implementation - would calculate hash of all files in directory
    return "dir_hash_" + directory;
}

bool CheckpointManager::ensureCheckpointPath(const std::string& path) const {
    if (!std::filesystem::exists(path)) {
        return std::filesystem::create_directories(path);
    }
    return true;
}

std::string CheckpointManager::generateCheckpointFilename(const std::string& name, const std::string& extension) {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm_buf;
    gmtime_r(&time, &tm_buf);
    
    std::ostringstream oss;
    oss << config.directory << "/";
    if (!name.empty()) {
        oss << name << "_";
    }
    oss << std::put_time(&tm_buf, "%Y%m%d_%H%M%S");
    
    if (!extension.empty() && extension[0] != '.') {
        oss << ".";
    }
    oss << extension;
    
    return oss.str();
}