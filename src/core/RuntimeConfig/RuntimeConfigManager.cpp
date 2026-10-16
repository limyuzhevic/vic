#include "RuntimeConfigManager.hpp"
#include "Config.hpp"
#include <sstream>
#include <iomanip>
#include <mutex>
#include <chrono>

namespace nlm {

struct ConfigHistory::Impl {
    struct ChangeRecord {
        RuntimeConfigChange change;
        std::vector<std::string> diffs;
    };
    
    std::vector<ChangeRecord> changes;
    std::stack<ChangeRecord> undoStack;
    std::vector<ConfigSnapshot> snapshots;
    std::unordered_map<std::string, ConfigSnapshot> snapshotMap;
    
    // Statistics
    size_t maxHistorySize;
    size_t maxSnapshots;
    size_t changesThisSecond;
    std::chrono::system_clock::time_point lastChangeTime;
    double totalChangeTime = 0.0;
    
    // State tracking
    std::string lastStateHash;
    std::unordered_map<std::string, std::vector<RuntimeConfigChange>> changesByKey;
    
    Impl(size_t maxHistorySize, size_t maxSnapshots)
        : maxHistorySize(maxHistorySize), maxSnapshots(maxSnapshots),
          changesThisSecond(0), lastChangeTime(std::chrono::system_clock::now()) {}
};

ConfigHistory::ConfigHistory(size_t maxHistorySize, size_t maxSnapshots)
    : pImpl(std::make_unique<Impl>(maxHistorySize, maxSnapshots)) {}

bool ConfigHistory::recordChange(const RuntimeConfigChange& change) {
    // Trim history if needed
    if (pImpl->changes.size() >= pImpl->maxHistorySize) {
        pImpl->changes.erase(pImpl->changes.begin());
    }
    
    // Record the change
    Impl::ChangeRecord record;
    record.change = change;
    record.diffs = getChangeDiff(change);
    
    pImpl->changes.push_back(record);
    pImpl->changesByKey[change.key].push_back(change);
    
    // Update undo stack
    pImpl->undoStack.push(record);
    
    // Update statistics
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration<double>(now - pImpl->lastChangeTime).count();
    pImpl->totalChangeTime += duration;
    pImpl->lastChangeTime = now;
    pImpl->changesThisSecond++;
    
    // Calculate average interval
    if (pImpl->changes.size() > 1) {
        pImpl->changesThisSecond = 1; // Reset for next second
    }
    
    // Call change callback
    if (changeCallback_) {
        ConfigChangeEvent event(change.key, change.oldValue, change.newValue, change.source);
        changeCallback_(event);
    }
    
    return true;
}

bool ConfigHistory::applyChange(const ConfigOperation& operation, Config& config,
                                const std::vector<std::shared_ptr<ChangeValidator>>& validators) {
    // Validate the operation
    for (const auto& validator : validators) {
        if (!validator->validate(operation, config)) {
            std::vector<std::string> errors = validator->getValidationErrors(operation, config);
            if (validationCallback_) {
                validationCallback_(operation, errors);
            }
            return false;
        }
    }
    
    // Generate change ID
    std::string changeId = "change_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    
    // Get old value
    ConfigValue oldValue;
    if (config.has(operation.key)) {
        try {
            oldValue = config.get<ConfigValue>(operation.key).value();
        } catch (...) {
            oldValue = ConfigValue(); // Empty variant
        }
    }
    
    // Apply the change
    try {
        switch (operation.type) {
            case RuntimeChangeType::Enable:
                config.set(operation.key, true, operation.source, true);
                break;
            case RuntimeChangeType::Disable:
                config.set(operation.key, false, operation.source, true);
                break;
            case RuntimeChangeType::ParameterAdjustment:
                config.set(operation.key, operation.value, operation.source, true);
                break;
            case RuntimeChangeType::Switch:
                config.set(operation.key, operation.value, operation.source, true);
                break;
            case RuntimeChangeType::Insert:
                config.set(operation.key, operation.value, operation.source, true);
                break;
            case RuntimeChangeType::Remove:
                config.remove(operation.key);
                break;
            case RuntimeChangeType::Replace:
                config.set(operation.key, operation.value, operation.source, true);
                break;
            case RuntimeChangeType::Custom:
                config.set(operation.key, operation.value, operation.source, true);
                break;
        }
        
        // Record the change
        RuntimeConfigChange change(changeId, operation.key, oldValue, operation.value, operation.source);
        change.applied = true;
        
        if (!recordChange(change)) {
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        std::vector<std::string> errors = {std::string("Application failed: ") + e.what()};
        if (validationCallback_) {
            validationCallback_(operation, errors);
        }
        return false;
    }
}

bool ConfigHistory::undoChange(Config& config) {
    if (pImpl->undoStack.empty()) {
        return false;
    }
    
    const auto& record = pImpl->undoStack.top();
    const auto& change = record.change;
    
    if (!change.applied) {
        pImpl->undoStack.pop();
        return false;
    }
    
    try {
        // Restore old value
        config.set(change.key, change.oldValue, ConfigSource::Runtime, true);
        
        // Mark as undone (create a reverse change)
        RuntimeConfigChange reverseChange("undo_" + change.changeId, change.key, 
                                         change.newValue, change.oldValue, ConfigSource::Runtime);
        reverseChange.applied = false; // Mark as undo
        
        pImpl->changes.push_back({reverseChange, record.diffs});
        
        pImpl->undoStack.pop();
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool ConfigHistory::redoChange(Config& config) {
    // This would need a redo stack implementation
    // For now, we'll implement a simpler approach
    if (pImpl->changes.empty() || pImpl->undoStack.empty()) {
        return false;
    }
    
    // Find the last non-applied change
    auto it = std::find_if(pImpl->changes.rbegin(), pImpl->changes.rend(),
                          [](const Impl::ChangeRecord& record) {
                              return !record.change.applied;
                          });
    
    if (it == pImpl->changes.rend()) {
        return false;
    }
    
    const auto& record = *it;
    const auto& change = record.change;
    
    try {
        config.set(change.key, change.newValue, ConfigSource::Runtime, true);
        change.applied = true;
        
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

std::string ConfigHistory::createSnapshot(const std::string& name, const std::string& description) {
    ConfigSnapshot snapshot(name, description);
    
    // Copy relevant changes (last 100 changes for snapshot)
    size_t startIdx = pImpl->changes.size() > 100 ? pImpl->changes.size() - 100 : 0;
    for (size_t i = startIdx; i < pImpl->changes.size(); ++i) {
        snapshot.changes.push_back(pImpl->changes[i].change);
    }
    
    // Check if snapshot name already exists
    if (pImpl->snapshotMap.find(name) != pImpl->snapshotMap.end()) {
        return "";
    }
    
    pImpl->snapshotMap[name] = snapshot;
    pImpl->snapshots.push_back(snapshot);
    
    // Trim snapshots if needed
    if (pImpl->snapshots.size() > pImpl->maxSnapshots) {
        pImpl->snapshots.erase(pImpl->snapshots.begin());
        // Also remove from map
        auto it = pImpl->snapshotMap.begin();
        pImpl->snapshotMap.erase(it);
    }
    
    return name;
}

bool ConfigHistory::rollbackToSnapshot(const std::string& name, Config& config) {
    auto it = pImpl->snapshotMap.find(name);
    if (it == pImpl->snapshotMap.end()) {
        return false;
    }
    
    const auto& snapshot = it->second;
    
    // Clear current config
    config.clear();
    
    // Restore changes from snapshot
    for (const auto& change : snapshot.changes) {
        config.set(change.key, change.newValue, ConfigSource::Runtime, false);
    }
    
    return true;
}

std::vector<RuntimeConfigChange> ConfigHistory::getChanges() const {
    std::vector<RuntimeConfigChange> result;
    for (const auto& record : pImpl->changes) {
        result.push_back(record.change);
    }
    return result;
}

std::vector<ConfigSnapshot> ConfigHistory::getSnapshots() const {
    return pImpl->snapshots;
}

std::vector<RuntimeConfigChange> ConfigHistory::getChanges(
    const std::chrono::system_clock::time_point& from,
    const std::chrono::system_clock::time_point& to) const {
    std::vector<RuntimeConfigChange> result;
    for (const auto& record : pImpl->changes) {
        if (record.change.timestamp >= from && record.change.timestamp <= to) {
            result.push_back(record.change);
        }
    }
    return result;
}

std::string ConfigHistory::getChangeDiff(const RuntimeConfigChange& change) const {
    std::ostringstream oss;
    oss << "Change: " << change.key << "\n";
    oss << "  From: ";
    
    try {
        if (std::holds_alternative<int>(change.oldValue)) {
            oss << std::get<int>(change.oldValue);
        } else if (std::holds_alternative<int64_t>(change.oldValue)) {
            oss << std::get<int64_t>(change.oldValue);
        } else if (std::holds_alternative<double>(change.oldValue)) {
            oss << std::get<double>(change.oldValue);
        } else if (std::holds_alternative<bool>(change.oldValue)) {
            oss << (std::get<bool>(change.oldValue) ? "true" : "false");
        } else if (std::holds_alternative<std::string>(change.oldValue)) {
            oss << std::get<std::string>(change.oldValue);
        } else if (std::holds_alternative<std::vector<int>>(change.oldValue)) {
            const auto& vec = std::get<std::vector<int>>(change.oldValue);
            oss << "[";
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << vec[i];
            }
            oss << "]";
        } else {
            oss << "[unknown]";
        }
    } catch (...) {
        oss << "[error]";
    }
    
    oss << "\n  To: ";
    
    try {
        if (std::holds_alternative<int>(change.newValue)) {
            oss << std::get<int>(change.newValue);
        } else if (std::holds_alternative<int64_t>(change.newValue)) {
            oss << std::get<int64_t>(change.newValue);
        } else if (std::holds_alternative<double>(change.newValue)) {
            oss << std::get<double>(change.newValue);
        } else if (std::holds_alternative<bool>(change.newValue)) {
            oss << (std::get<bool>(change.newValue) ? "true" : "false");
        } else if (std::holds_alternative<std::string>(change.newValue)) {
            oss << std::get<std::string>(change.newValue);
        } else if (std::holds_alternative<std::vector<int>>(change.newValue)) {
            const auto& vec = std::get<std::vector<int>>(change.newValue);
            oss << "[";
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << vec[i];
            }
            oss << "]";
        } else {
            oss << "[unknown]";
        }
    } catch (...) {
        oss << "[error]";
    }
    
    oss << "\n  Source: " << static_cast<int>(change.source);
    oss << "\n  Timestamp: " << std::chrono::duration_cast<std::chrono::milliseconds>
        (change.timestamp.time_since_epoch()).count();
    
    return oss.str();
}

std::string ConfigHistory::getHistorySummary() const {
    std::ostringstream oss;
    oss << "Configuration History Summary:" << std::endl;
    oss << "  Total changes: " << pImpl->changes.size() << std::endl;
    oss << "  Unique keys modified: " << pImpl->changesByKey.size() << std::endl;
    oss << "  Snapshots: " << pImpl->snapshots.size() << std::endl;
    oss << "  Max history size: " << pImpl->maxHistorySize << std::endl;
    oss << "  Max snapshots: " << pImpl->maxSnapshots << std::endl;
    
    if (!pImpl->changes.empty()) {
        oss << "  Most recent change: " << pImpl->changes.back().change.key << std::endl;
        oss << "  Last change time: " << 
            std::chrono::duration_cast<std::chrono::milliseconds>
            (pImpl->lastChangeTime.time_since_epoch()).count() << " ms ago" << std::endl;
    }
    
    return oss.str();
}

void ConfigHistory::clearHistory() {
    pImpl->changes.clear();
    std::stack<Impl::ChangeRecord>().swap(pImpl->undoStack);
    pImpl->snapshots.clear();
    pImpl->snapshotMap.clear();
    pImpl->changesByKey.clear();
    pImpl->changesThisSecond = 0;
    pImpl->lastChangeTime = std::chrono::system_clock::now();
    pImpl->totalChangeTime = 0.0;
}

bool ConfigHistory::exportHistory(const std::string& filepath, const std::string& format) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        if (format == "json") {
            // Simple JSON export
            file << "{\n  \"changes\": [\n";
            for (size_t i = 0; i < pImpl->changes.size(); ++i) {
                const auto& record = pImpl->changes[i];
                file << "    {\"key\": \"" << record.change.key << "\", \"oldValue\": \"" 
                     << valueToString(record.change.oldValue) << "\", \"newValue\": \""
                     << valueToString(record.change.newValue) << "\", \"source\": "
                     << static_cast<int>(record.change.source) << "\"";
                if (i < pImpl->changes.size() - 1) {
                    file << ",";
                }
                file << "\n";
            }
            file << "  ],\n  \"snapshots\": [\n";
            for (size_t i = 0; i < pImpl->snapshots.size(); ++i) {
                const auto& snapshot = pImpl->snapshots[i];
                file << "    {\"name\": \"" << snapshot.name << "\", \"description\": \""
                     << snapshot.description << "\"";
                if (i < pImpl->snapshots.size() - 1) {
                    file << ",";
                }
                file << "\n";
            }
            file << "  ]\n}";
        } else {
            // Default to text format
            file << getHistorySummary() << std::endl;
            file << "\nChanges:" << std::endl;
            for (const auto& record : pImpl->changes) {
                file << getChangeDiff(record.change) << std::endl << std::endl;
            }
        }
        
        file.close();
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool ConfigHistory::importHistory(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Simple implementation - just clear current history
        // In a real implementation, we would parse the file format
        clearHistory();
        
        file.close();
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

std::string ConfigHistory::getCurrentStateHash(const Config& config) const {
    std::ostringstream oss;
    oss << "state_" << config.summary();
    
    // Simple hash implementation
    std::hash<std::string> hasher;
    return std::to_string(hasher(oss.str()));
}

bool ConfigHistory::isValidType(const std::string& key, const ConfigValue& value, const Config& config) const {
    // Check if schema has parameter for this key
    if (config.getSchema() && config.getSchema()->hasParameter(key)) {
        auto param = config.getSchema()->getParameter(key);
        if (param) {
            // Validate based on type
            if (std::holds_alternative<int>(value)) {
                return std::is_same_v<int, std::decay_t<decltype(std::get<int>(value))>>;
            } else if (std::holds_alternative<int64_t>(value)) {
                return std::is_same_v<int64_t, std::decay_t<decltype(std::get<int64_t>(value))>>;
            } else if (std::holds_alternative<double>(value)) {
                return std::is_same_v<double, std::decay_t<decltype(std::get<double>(value))>>;
            } else if (std::holds_alternative<bool>(value)) {
                return std::is_same_v<bool, std::decay_t<decltype(std::get<bool>(value))>>;
            } else if (std::holds_alternative<std::string>(value)) {
                return std::is_same_v<std::string, std::decay_t<decltype(std::get<std::string>(value))>>;
            } else if (std::holds_alternative<std::vector<int>>(value)) {
                return std::is_same_v<std::vector<int>, std::decay_t<decltype(std::get<std::vector<int>>(value))>>;
            } else if (std::holds_alternative<std::vector<double>>(value)) {
                return std::is_same_v<std::vector<double>, std::decay_t<decltype(std::get<std::vector<double>>(value))>>;
            } else if (std::holds_alternative<std::vector<std::string>>(value)) {
                return std::is_same_v<std::vector<std::string>, std::decay_t<decltype(std::get<std::vector<std::string>>(value))>>;
            }
        }
    }
    
    // No schema or parameter not found - accept any type
    return true;
}

struct ConfigMonitor::Impl {
    std::vector<MonitorCallback> callbacks;
    MonitorStats stats;
    std::mutex mutex;
    
    Impl() {
        stats.totalChanges = 0;
        stats.changesThisSecond = 0;
        stats.lastChangeTime = std::chrono::system_clock::now();
        stats.avgChangeInterval = 0.0;
    }
};

ConfigMonitor::ConfigMonitor() : pImpl(std::make_unique<Impl>()) {}

void ConfigMonitor::addMonitorCallback(MonitorCallback callback) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->callbacks.push_back(callback);
}

void ConfigMonitor::removeMonitorCallback(MonitorCallback callback) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto it = pImpl->callbacks.begin(); it != pImpl->callbacks.end(); ++it) {
        if (it->target<void(*)(const ConfigChangeEvent&)>() == callback.target<void(*)(const ConfigChangeEvent&)>()) {
            pImpl->callbacks.erase(it);
            break;
        }
    }
}

ConfigMonitor::MonitorStats ConfigMonitor::getStats() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->stats;
}

void ConfigMonitor::resetStats() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->stats.totalChanges = 0;
    pImpl->stats.changesThisSecond = 0;
    pImpl->stats.lastChangeTime = std::chrono::system_clock::now();
    pImpl->stats.avgChangeInterval = 0.0;
}

bool ConfigMonitor::exportMonitorData(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        MonitorStats stats = getStats();
        file << "Monitor Statistics:" << std::endl;
        file << "Total changes: " << stats.totalChanges << std::endl;
        file << "Changes this second: " << stats.changesThisSecond << std::endl;
        file << "Last change time: " << 
            std::chrono::duration_cast<std::chrono::milliseconds>
            (stats.lastChangeTime.time_since_epoch()).count() << " ms ago" << std::endl;
        file << "Average change interval: " << stats.avgChangeInterval << " s" << std::endl;
        file << "\nChanges by key:" << std::endl;
        for (const auto& pair : stats.changesByKey) {
            file << "  " << pair.first << ": " << pair.second << std::endl;
        }
        
        file.close();
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

struct RuntimeConfigManager::Impl {
    std::chrono::system_clock::time_point startTime;
    std::string lastChangeKey;
    std::string lastChangeDescription;
    size_t totalChanges;
    bool enabled;
    bool paused;
    std::mutex mutex;
    
    Impl() : startTime(std::chrono::system_clock::now()), totalChanges(0),
             enabled(true), paused(false) {}
};

RuntimeConfigManager::RuntimeConfigManager(std::shared_ptr<Config> config)
    : config_(config), enabled_(true), paused_(false) {
    history_ = std::make_shared<ConfigHistory>();
    monitor_ = std::make_shared<ConfigMonitor>();
    pImpl = std::make_unique<Impl>();
}

bool RuntimeConfigManager::applyRuntimeChange(const std::string& key, const ConfigValue& value,
                                             const std::string& description,
                                             const std::vector<std::shared_ptr<ChangeValidator>>& validators) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!enabled_ || paused_) {
        return false;
    }
    
    // Create operation
    ConfigOperation operation(key, RuntimeChangeType::ParameterAdjustment, value, description);
    
    // Apply with validation
    return history_->applyChange(operation, *config_, validators);
}

bool RuntimeConfigManager::applyOperation(const ConfigOperation& operation,
                                         const std::vector<std::shared_ptr<ChangeValidator>>& validators) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!enabled_ || paused_) {
        return false;
    }
    
    // Apply with validation
    bool success = history_->applyChange(operation, *config_, validators);
    
    if (success) {
        pImpl->totalChanges++;
        pImpl->lastChangeKey = operation.key;
        pImpl->lastChangeDescription = operation.description;
    }
    
    return success;
}

RuntimeConfigManager::RuntimeStatus RuntimeConfigManager::getStatus() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    RuntimeStatus status;
    status.isInitialized = config_ != nullptr;
    status.lastChangeTime = pImpl->lastChangeTime;
    status.lastChangeKey = pImpl->lastChangeKey;
    status.lastChangeDescription = pImpl->lastChangeDescription;
    status.totalChanges = pImpl->totalChanges;
    
    auto now = std::chrono::system_clock::now();
    status.uptimeSeconds = std::chrono::duration<double>(now - pImpl->startTime).count();
    
    return status;
}

void RuntimeConfigManager::addChangeValidator(std::shared_ptr<ChangeValidator> validator) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    validators_.push_back(validator);
}

void RuntimeConfigManager::removeChangeValidator(std::shared_ptr<ChangeValidator> validator) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto it = validators_.begin(); it != validators_.end(); ++it) {
        if (it->get() == validator.get()) {
            validators_.erase(it);
            break;
        }
    }
}

std::vector<std::string> RuntimeConfigManager::validateChange(const std::string& key, 
                                                             const ConfigValue& value) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<std::string> errors;
    
    if (!enabled_) {
        errors.push_back("Runtime configuration is disabled");
    }
    
    if (paused_) {
        errors.push_back("Runtime configuration is paused");
    }
    
    // Check if config has schema
    if (config_->getSchema()) {
        auto param = config_->getSchema()->getParameter(key);
        if (param) {
            // Validate type
            if (!history_->isValidType(key, value, *config_)) {
                errors.push_back("Value type does not match schema for key: " + key);
            }
            
            // Validate range
            try {
                double val;
                if (std::holds_alternative<int>(value)) {
                    val = std::get<int>(value);
                } else if (std::holds_alternative<int64_t>(value)) {
                    val = std::get<int64_t>(value);
                } else if (std::holds_alternative<double>(value)) {
                    val = std::get<double>(value);
                } else {
                    errors.push_back("Key " + key + " requires numeric type");
                    return errors;
                }
                
                if (param->minValue.has_value() && val < param->minValue.value()) {
                    errors.push_back("Value " + std::to_string(val) + " < minimum " + 
                                   std::to_string(param->minValue.value()) + " for key: " + key);
                }
                
                if (param->maxValue.has_value() && val > param->maxValue.value()) {
                    errors.push_back("Value " + std::to_string(val) + " > maximum " + 
                                   std::to_string(param->maxValue.value()) + " for key: " + key);
                }
            } catch (...) {
                errors.push_back("Error validating value for key: " + key);
            }
        }
    }
    
    return errors;
}

std::string RuntimeConfigManager::createRollbackPoint(const std::string& description) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return history_->createSnapshot("rollback_" + std::to_string(pImpl->totalChanges), description);
}

bool RuntimeConfigManager::applyRollback(const std::string& rollbackPointId) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return history_->rollbackToSnapshot(rollbackPointId, *config_);
}

// ChangeValidator implementations
RangeValidator::RangeValidator(const std::string& name, const std::string& key, 
                             double minValue, double maxValue)
    : name_(name), key_(key), minValue_(minValue), maxValue_(maxValue) {}

bool RangeValidator::validate(const ConfigOperation& operation, const Config& currentConfig) {
    if (operation.key != key_) {
        return true;
    }
    
    try {
        if (std::holds_alternative<double>(operation.value)) {
            double val = std::get<double>(operation.value);
            return val >= minValue_ && val <= maxValue_;
        } else if (std::holds_alternative<int>(operation.value)) {
            int val = std::get<int>(operation.value);
            return static_cast<double>(val) >= minValue_ && static_cast<double>(val) <= maxValue_;
        } else if (std::holds_alternative<int64_t>(operation.value)) {
            int64_t val = std::get<int64_t>(operation.value);
            return static_cast<double>(val) >= minValue_ && static_cast<double>(val) <= maxValue_;
        }
    } catch (...) {}
    
    return false;
}

std::vector<std::string> RangeValidator::getValidationErrors(const ConfigOperation& operation, 
                                                             const Config& currentConfig) {
    std::vector<std::string> errors;
    
    if (operation.key != key_) {
        return errors;
    }
    
    try {
        if (std::holds_alternative<double>(operation.value)) {
            double val = std::get<double>(operation.value);
            if (val < minValue_) {
                errors.push_back(std::string("Value ") + std::to_string(val) + 
                               " < minimum " + std::to_string(minValue_) + " for key: " + key_);
            } else if (val > maxValue_) {
                errors.push_back(std::string("Value ") + std::to_string(val) + 
                               " > maximum " + std::to_string(maxValue_) + " for key: " + key_);
            }
        } else if (std::holds_alternative<int>(operation.value)) {
            int val = std::get<int>(operation.value);
            if (static_cast<double>(val) < minValue_) {
                errors.push_back(std::string("Value ") + std::to_string(val) + 
                               " < minimum " + std::to_string(minValue_) + " for key: " + key_);
            } else if (static_cast<double>(val) > maxValue_) {
                errors.push_back(std::string("Value ") + std::to_string(val) + 
                               " > maximum " + std::to_string(maxValue_) + " for key: " + key_);
            }
        } else if (std::holds_alternative<int64_t>(operation.value)) {
            int64_t val = std::get<int64_t>(operation.value);
            if (static_cast<double>(val) < minValue_) {
                errors.push_back(std::string("Value ") + std::to_string(val) + 
                               " < minimum " + std::to_string(minValue_) + " for key: " + key_);
            } else if (static_cast<double>(val) > maxValue_) {
                errors.push_back(std::string("Value ") + std::to_string(val) + 
                               " > maximum " + std::to_string(maxValue_) + " for key: " + key_);
            }
        } else {
            errors.push_back("Key " + key_ + " requires numeric type");
        }
    } catch (...) {
        errors.push_back("Error validating range for key: " + key_);
    }
    
    return errors;
}

DependencyValidator::DependencyValidator(const std::string& name, const std::string& key,
                                         const std::unordered_map<std::string, ConfigValue>& requiredState)
    : name_(name), key_(key), requiredState_(requiredState) {}

bool DependencyValidator::validate(const ConfigOperation& operation, const Config& currentConfig) {
    if (operation.key != key_) {
        return true;
    }
    
    // Check if all required dependencies are satisfied
    for (const auto& pair : requiredState_) {
        if (!currentConfig.has(pair.first)) {
            return false;
        }
        
        try {
            ConfigValue currentValue = currentConfig.get<ConfigValue>(pair.first).value();
            if (currentValue != pair.second) {
                return false;
            }
        } catch (...) {
            return false;
        }
    }
    
    return true;
}

std::vector<std::string> DependencyValidator::getValidationErrors(const ConfigOperation& operation, 
                                                                  const Config& currentConfig) {
    std::vector<std::string> errors;
    
    if (operation.key != key_) {
        return errors;
    }
    
    // Check dependencies
    for (const auto& pair : requiredState_) {
        if (!currentConfig.has(pair.first)) {
            errors.push_back("Required key missing: " + pair.first);
        } else {
            try {
                ConfigValue currentValue = currentConfig.get<ConfigValue>(pair.first).value();
                if (currentValue != pair.second) {
                    errors.push_back(std::string("Required key mismatch: ") + pair.first +
                                   " (expected: " + valueToString(pair.second) +
                                   ", current: " + valueToString(currentValue) + ")");
                }
            } catch (...) {
                errors.push_back("Error checking value for key: " + pair.first);
            }
        }
    }
    
    return errors;
}

StateValidator::StateValidator(const std::string& name, std::function<bool(const Config&)> validator)
    : name_(name), validator_(validator) {}

bool StateValidator::validate(const ConfigOperation& operation, const Config& currentConfig) {
    return validator_(currentConfig);
}

std::vector<std::string> StateValidator::getValidationErrors(const ConfigOperation& operation, 
                                                            const Config& currentConfig) {
    std::vector<std::string> errors;
    
    if (!validator_(currentConfig)) {
        errors.push_back("State validation failed");
    }
    
    return errors;
}

// Runtime configuration commands
namespace RuntimeConfigCommands {
    bool pauseSimulation(std::shared_ptr<RuntimeConfigManager> runtimeConfig) {
        if (!runtimeConfig) return false;
        runtimeConfig->pause();
        return true;
    }
    
    bool stepSimulation(std::shared_ptr<RuntimeConfigManager> runtimeConfig) {
        if (!runtimeConfig) return false;
        runtimeConfig->resume();
        return true;
    }
    
    bool applyRuntimeConfig(std::shared_ptr<RuntimeConfigManager> runtimeConfig, 
                           const std::string& key, const std::string& value) {
        if (!runtimeConfig) return false;
        
        // Convert string to appropriate type
        ConfigValue configValue;
        
        if (value == "true" || value == "false") {
            configValue = (value == "true");
        } else if (value.find('.') != std::string::npos) {
            try {
                configValue = std::stod(value);
            } catch (...) {
                configValue = value;
            }
        } else {
            try {
                configValue = std::stoi(value);
            } catch (...) {
                configValue = value;
            }
        }
        
        return runtimeConfig->applyRuntimeChange(key, configValue, "Runtime configuration change");
    }
    
    void listChanges(std::shared_ptr<RuntimeConfigManager> runtimeConfig) {
        if (!runtimeConfig) return;
        
        auto history = runtimeConfig->getHistory();
        if (!history) return;
        
        auto changes = history->getChanges();
        if (changes.empty()) {
            std::cout << "No configuration changes recorded." << std::endl;
            return;
        }
        
        std::cout << "Configuration Changes:" << std::endl;
        for (const auto& change : changes) {
            std::cout << "  " << change.key << ": " 
                     << (change.oldValue != ConfigValue() ? valueToString(change.oldValue) : "[none]")
                     << " -> " << valueToString(change.newValue) << " ("
                     << std::chrono::duration_cast<std::chrono::milliseconds>
                         (change.timestamp.time_since_epoch()).count() << " ms)" << std::endl;
        }
    }
    
    void showHistory(std::shared_ptr<RuntimeConfigManager> runtimeConfig) {
        if (!runtimeConfig) return;
        
        auto history = runtimeConfig->getHistory();
        if (!history) return;
        
        std::cout << history->getHistorySummary() << std::endl;
    }
    
    std::string createSnapshot(std::shared_ptr<RuntimeConfigManager> runtimeConfig,
                              const std::string& name, const std::string& description) {
        if (!runtimeConfig) return "";
        
        auto history = runtimeConfig->getHistory();
        if (!history) return "";
        
        return history->createSnapshot(name, description);
    }
    
    bool rollbackToSnapshot(std::shared_ptr<RuntimeConfigManager> runtimeConfig,
                           const std::string& name) {
        if (!runtimeConfig) return false;
        
        auto history = runtimeConfig->getHistory();
        if (!history) return false;
        
        return history->rollbackToSnapshot(name, *runtimeConfig->getConfig());
    }
}

} // namespace nlm