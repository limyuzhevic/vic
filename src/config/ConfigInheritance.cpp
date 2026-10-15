#include "ConfigInheritance.hpp"
#include "../core/Logger/Logger.hpp"
#include <yaml-cpp/yaml.h>
#include <json/json.h>
#include <algorithm>
#include <unordered_map>
#include <set>

namespace nlm {

struct ConfigInheritance::Impl {
    // Path management
    struct InheritancePath {
        std::string path;
        std::string priority;
        double weight;
        bool enabled;
        bool exists; // Whether the file actually exists
        std::chrono::system_clock::time_point lastModified;
        Config config;
        
        InheritancePath(const std::string& p, const std::string& prio = "medium")
            : path(p), priority(prio), weight(calculateWeight(prio)), enabled(true), exists(false) {}
        
        static double calculateWeight(const std::string& priority) {
            if (priority == "high") return 1.0;
            if (priority == "medium") return 0.5;
            if (priority == "low") return 0.1;
            return 0.5;
        }
    };
    
    std::vector<InheritancePath> paths;
    std::mutex mutex;
    
    // Dynamic rules
    std::vector<std::pair<std::string, std::function<bool(const std::string&)>>> dynamicRules;
    
    // Events
    struct EventCallbackEntry {
        std::string eventType;
        InheritanceCallback callback;
    };
    std::vector<EventCallbackEntry> eventCallbacks;
    
    // Monitoring
    bool monitoringEnabled;
    std::chrono::system_clock::time_point lastUpdate;
    
    // Analytics
    struct AnalyticsData {
        std::map<std::string, size_t> pathUsageCounts;
        std::map<std::string, double> changeRates;
        size_t totalConflicts;
        size_t totalMerges;
        std::chrono::system_clock::time_point startTime;
    } analytics;
    
    Logger* logger;
};

ConfigInheritance::ConfigInheritance() : pImpl(std::make_unique<Impl>()) {
    pImpl->logger = Logger::getInstance();
    pImpl->monitoringEnabled = false;
    pImpl->analytics.startTime = std::chrono::system_clock::now();
}

ConfigInheritance::~ConfigInheritance() {
    shutdown();
}

ConfigInheritance::ConfigInheritance(ConfigInheritance&& other) noexcept : pImpl(std::move(other.pImpl)) {}

ConfigInheritance& ConfigInheritance::operator=(ConfigInheritance&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool ConfigInheritance::initialize() {
    pImpl->logger->info("Configuration inheritance initialized");
    return true;
}

void ConfigInheritance::update(double timestep) {
    if (!pImpl->monitoringEnabled) return;
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Check for path changes
    bool pathsChanged = false;
    for (auto& path : pImpl->paths) {
        if (std::filesystem::exists(path.path)) {
            if (!path.exists) {
                path.exists = true;
                pathsChanged = true;
                
                // Load configuration if file exists
                if (path.enabled) {
                    Config config;
                    if (path.path.substr(path.path.find_last_of(".") + 1) == "yaml" ||
                        path.path.substr(path.path.find_last_of(".") + 1) == "yml") {
                        config.loadFromFile(path.path);
                    } else {
                        // Try JSON or other formats
                        config.loadFromFile(path.path);
                    }
                    
                    path.config = config;
                    triggerInheritanceEvent("path_added", path.path, config, true);
                }
            }
            
            // Check for modifications
            auto lastMod = std::filesystem::last_write_time(path.path);
            if (lastMod != path.lastModified) {
                path.lastModified = lastMod;
                pathsChanged = true;
                triggerInheritanceEvent("path_modified", path.path, path.config, false);
            }
        } else {
            if (path.exists) {
                path.exists = false;
                pathsChanged = true;
                triggerInheritanceEvent("path_removed", path.path, Config(), false);
            }
        }
    }
    
    if (pathsChanged) {
        pImpl->lastUpdate = std::chrono::system_clock::now();
        pImpl->analytics.totalConflicts = detectConflicts(getCurrentConfigs()).first;
        pImpl->analytics.totalMerges++;
    }
}

void ConfigInheritance::shutdown() {
    pImpl->logger->info("Configuration inheritance shutdown");
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (auto& path : pImpl->paths) {
        if (path.exists) {
            triggerInheritanceEvent("path_removed", path.path, path.config, false);
        }
    }
    
    pImpl->paths.clear();
    pImpl->dynamicRules.clear();
    pImpl->eventCallbacks.clear();
}

bool ConfigInheritance::addInheritancePath(const std::string& path, const std::string& priority) {
    if (path.empty()) {
        pImpl->logger->error("Cannot add empty inheritance path");
        return false;
    }
    
    // Check if path already exists
    for (const auto& existingPath : pImpl->paths) {
        if (existingPath.path == path) {
            pImpl->logger->warning("Inheritance path already exists: " + path);
            return false;
        }
    }
    
    InheritancePath newPath(path, priority);
    pImpl->paths.push_back(newPath);
    
    // Evaluate dynamic rules
    if (evaluateDynamicRules(path)) {
        newPath.enabled = true;
        pImpl->logger->info("Inheritance path added and enabled: " + path);
    } else {
        pImpl->logger->info("Inheritance path added but disabled by dynamic rules: " + path);
    }
    
    return true;
}

bool ConfigInheritance::removeInheritancePath(const std::string& path) {
    auto it = std::find_if(pImpl->paths.begin(), pImpl->paths.end(),
                          [&path](const InheritancePath& p) { return p.path == path; });n    
    if (it != pImpl->paths.end()) {
        if (it->exists) {
            triggerInheritanceEvent("path_removed", it->path, it->config, false);
        }
        
        pImpl->paths.erase(it);
        pImpl->logger->info("Inheritance path removed: " + path);
        return true;
    }
    
    pImpl->logger->warning("Inheritance path not found: " + path);
    return false;
}

bool ConfigInheritance::updateInheritancePath(const std::string& path, const std::string& newPriority) {
    auto it = std::find_if(pImpl->paths.begin(), pImpl->paths.end(),
                          [&path](const InheritancePath& p) { return p.path == path; });
    
    if (it != pImpl->paths.end()) {
        it->priority = newPriority;
        it->weight = InheritancePath::calculateWeight(newPriority);
        pImpl->logger->info("Inheritance path updated: " + path + " (new priority: " + newPriority + ")");
        return true;
    }
    
    pImpl->logger->warning("Inheritance path not found: " + path);
    return false;
}

std::vector<std::string> ConfigInheritance::getInheritancePaths() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::string> paths;
    paths.reserve(pImpl->paths.size());
    
    for (const auto& path : pImpl->paths) {
        if (path.enabled) {
            paths.push_back(path.path);
        }
    }
    
    return paths;
}

std::map<std::string, std::string> ConfigInheritance::getPathPriorities() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::map<std::string, std::string> priorities;
    
    for (const auto& path : pImpl->paths) {
        if (path.enabled) {
            priorities[path.path] = path.priority;
        }
    }
    
    return priorities;
}

Config ConfigInheritance::resolveInheritance(const std::vector<std::string>& paths) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Determine which paths to resolve
    std::vector<InheritancePath> resolvedPaths;
    if (paths.empty()) {
        // Use all enabled paths
        for (const auto& path : pImpl->paths) {
            if (path.enabled && path.exists) {
                resolvedPaths.push_back(path);
            }
        }
    } else {
        // Use specified paths
        for (const auto& specifiedPath : paths) {
            auto it = std::find_if(pImpl->paths.begin(), pImpl->paths.end(),
                                  [&specifiedPath](const InheritancePath& p) { return p.path == specifiedPath; });
            if (it != pImpl->paths.end() && it->enabled && it->exists) {
                resolvedPaths.push_back(*it);
            }
        }
    }
    
    // Get resolution order
    std::vector<InheritancePath> resolutionOrder = getResolvedInheritanceOrder(paths);
    
    // Merge configurations
    Config result;
    bool first = true;
    
    for (const auto& path : resolutionOrder) {
        if (first) {
            // Start with base configuration
            result = path.config;
            first = false;
        } else {
            // Merge with override
            std::map<std::string, ConfigValue> mergedValues = mergeValues(
                result.getAllValues(), path.config.getAllValues());
            
            result.clear();
            for (const auto& pair : mergedValues) {
                result.set(pair.first, pair.second);
            }
        }
    }
    
    return result;
}

bool ConfigInheritance::validateInheritanceChain(const std::vector<std::string>& paths) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Check if all specified paths exist
    for (const auto& path : paths) {
        bool found = false;
        for (const auto& existingPath : pImpl->paths) {
            if (existingPath.path == path && existingPath.enabled) {
                found = true;
                break;
            }
        }
        
        if (!found) {
            pImpl->logger->error("Path not found in inheritance chain: " + path);
            return false;
        }
    }
    
    // Check for circular references
    if (hasCircularReference(paths)) {
        pImpl->logger->error("Circular reference detected in inheritance chain");
        return false;
    }
    
    return true;
}

std::vector<std::string> ConfigInheritance::getResolvedInheritanceOrder(const std::vector<std::string>& paths) const {
    // Simple priority-based ordering
    std::vector<InheritancePath> orderedPaths;
    
    if (paths.empty()) {
        // Sort all paths by priority
        orderedPaths = pImpl->paths;
        std::sort(orderedPaths.begin(), orderedPaths.end(),
                 [](const InheritancePath& a, const InheritancePath& b) {
                     return a.weight > b.weight; // Higher weight = higher priority
                 });
    } else {
        // Sort specified paths by priority
        for (const auto& specifiedPath : paths) {
            auto it = std::find_if(pImpl->paths.begin(), pImpl->paths.end(),
                                  [&specifiedPath](const InheritancePath& p) { return p.path == specifiedPath; });
            if (it != pImpl->paths.end() && it->enabled) {
                orderedPaths.push_back(*it);
            }
        }
        
        std::sort(orderedPaths.begin(), orderedPaths.end(),
                 [](const InheritancePath& a, const InheritancePath& b) {
                     return a.weight > b.weight;
                 });
    }
    
    std::vector<std::string> pathStrings;
    for (const auto& path : orderedPaths) {
        pathStrings.push_back(path.path);
    }
    
    return pathStrings;
}

Config ConfigInheritance::mergeConfigurations(const std::map<std::string, Config>& configs) const {
    std::vector<Config> configList;
    configList.reserve(configs.size());
    
    for (const auto& pair : configs) {
        configList.push_back(pair.second);
    }
    
    return mergeConfigurations(configList);
}

Config ConfigInheritance::mergeConfigurations(const std::vector<Config>& configs) const {
    if (configs.empty()) {
        return Config();
    }
    
    Config result = configs[0];
    for (size_t i = 1; i < configs.size(); ++i) {
        std::map<std::string, ConfigValue> mergedValues = mergeValues(
            result.getAllValues(), configs[i].getAllValues());
        
        result.clear();
        for (const auto& pair : mergedValues) {
            result.set(pair.first, pair.second);
        }
    }
    
    return result;
}

std::map<std::string, ConfigValue> ConfigInheritance::mergeValues(const std::map<std::string, ConfigValue>& baseValues,
                                                                  const std::map<std::string, ConfigValue>& overrideValues) const {
    std::map<std::string, ConfigValue> result = baseValues;
    
    for (const auto& overridePair : overrideValues) {
        result[overridePair.first] = overridePair.second;
    }
    
    return result;
}

bool ConfigInheritance::detectConflicts(const std::map<std::string, Config>& configs) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<std::string> conflicts = getConflicts(configs);
    
    if (!conflicts.empty()) {
        pImpl->logger->warning("Conflicts detected in inheritance chain");
        for (const auto& conflict : conflicts) {
            pImpl->logger->warning("  - " + conflict);
        }
    }
    
    return !conflicts.empty();
}

std::vector<std::string> ConfigInheritance::getConflicts(const std::map<std::string, Config>& configs) const {
    std::vector<std::string> conflicts;
    
    if (configs.size() <= 1) return conflicts;
    
    std::map<std::string, std::map<std::string, ConfigValue>> configBySource;
    for (const auto& pair : configs) {
        configBySource[pair.first] = pair.second.getAllValues();
    }
    
    for (const auto& sourcePair : configBySource) {
        const std::string& source = sourcePair.first;
        const std::map<std::string, ConfigValue>& sourceValues = sourcePair.second;
        
        for (const auto& otherPair : configBySource) {
            if (source == otherPair.first) continue;
            
            const std::map<std::string, ConfigValue>& otherValues = otherPair.second;
            
            for (const auto& valuePair : sourceValues) {
                const std::string& key = valuePair.first;
                const ConfigValue& sourceValue = valuePair.second;
                
                auto otherIt = otherValues.find(key);
                if (otherIt != otherValues.end()) {
                    const ConfigValue& otherValue = otherIt->second;
                    
                    if (sourceValue != otherValue) {
                        conflicts.push_back("Key '" + key + "' has different values in sources: '" + 
                                          valueToString(sourceValue) + "' vs '" + valueToString(otherValue) + "'");
                    }
                }
            }
        }
    }
    
    return conflicts;
}

std::string ConfigInheritance::resolveConflict(const std::string& key, const std::map<std::string, ConfigValue>& conflictingValues) const {
    // Simple conflict resolution: prefer values from higher priority paths
    // In real implementation, would use more sophisticated strategies
    
    if (conflictingValues.empty()) return "";
    
    // Find the path with highest weight among conflicting sources
    double maxWeight = -1;
    const std::string* bestSource = nullptr;
    
    for (const auto& pair : conflictingValues) {
        // In real implementation, would look up source weight
        // For now, just pick the first one
        if (bestSource == nullptr) {
            bestSource = &pair.first;
        }
    }
    
    if (bestSource) {
        return valueToString(conflictingValues.at(*bestSource));
    }
    
    return "";
}

bool ConfigInheritance::enableInheritanceMonitoring(bool enable) {
    pImpl->monitoringEnabled = enable;
    pImpl->logger->info("Inheritance monitoring " + std::string(enable ? "enabled" : "disabled"));
    return true;
}

bool ConfigInheritance::isInheritanceMonitoringEnabled() const {
    return pImpl->monitoringEnabled;
}

std::string ConfigInheritance::getInheritanceStatus() const {
    std::stringstream ss;
    ss << "Inheritance Status:\n";
    ss << "==================\n";
    ss << "Monitoring enabled: " << (pImpl->monitoringEnabled ? "yes" : "no") << "\n";
    ss << "Active paths: " << pImpl->paths.size() << "\n";
    ss << "Last update: " << std::chrono::duration<double>(std::chrono::system_clock::now() - pImpl->lastUpdate).count() << " seconds ago\n";
    
    return ss.str();
}

std::chrono::system_clock::time_point ConfigInheritance::getLastInheritanceUpdate() const {
    return pImpl->lastUpdate;
}

bool ConfigInheritance::validateInheritancePaths(const std::vector<std::string>& paths) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<std::string> warnings;
    
    for (const auto& path : paths) {
        bool found = false;
        for (const auto& existingPath : pImpl->paths) {
            if (existingPath.path == path) {
                found = true;
                
                if (!existingPath.enabled) {
                    warnings.push_back("Path is disabled: " + path);
                }
                
                if (!existingPath.exists) {
                    warnings.push_back("Path does not exist: " + path);
                }
                
                break;
            }
        }
        
        if (!found) {
            warnings.push_back("Path not found in inheritance: " + path);
        }
    }
    
    if (!warnings.empty()) {
        for (const auto& warning : warnings) {
            pImpl->logger->warning(warning);
        }
        return false;
    }
    
    return true;
}

bool ConfigInheritance::validateConfiguration(const Config& config) const {
    // Basic validation - check for null values and invalid types
    bool valid = true;
    
    for (const auto& key : config.getKeys()) {
        auto value = config.get<ConfigValue>(key);
        
        // Check for empty strings
        if (std::holds_alternative<std::string>(value) && std::get<std::string>(value).empty()) {
            pImpl->logger->warning("Empty string value for key: " + key);
            valid = false;
        }
        
        // Check for null-like values
        if (value.valueless_by_exception()) {
            pImpl->logger->warning("Null/undefined value for key: " + key);
            valid = false;
        }
    }
    
    return valid;
}

std::vector<std::string> ConfigInheritance::getInheritanceWarnings() const {
    std::vector<std::string> warnings;
    
    // Check for disabled paths
    for (const auto& path : pImpl->paths) {
        if (!path.enabled) {
            warnings.push_back("Disabled inheritance path: " + path.path);
        } else if (!path.exists) {
            warnings.push_back("Non-existent inheritance path: " + path.path);
        }
    }
    
    // Check for conflicts
    auto conflicts = getConflicts(getCurrentConfigs());
    for (const auto& conflict : conflicts) {
        warnings.push_back("Inheritance conflict: " + conflict);
    }
    
    return warnings;
}

size_t ConfigInheritance::getInheritanceChainDepth() const {
    // Simple calculation based on priority levels
    return pImpl->paths.size();
}

std::map<std::string, size_t> ConfigInheritance::getConfigurationUsageCounts() const {
    return pImpl->analytics.pathUsageCounts;
}

std::map<std::string, double> ConfigInheritance::getConfigurationChangeRates() const {
    return pImpl->analytics.changeRates;
}

std::string ConfigInheritance::getInheritanceAnalytics() const {
    std::stringstream ss;
    ss << "Inheritance Analytics:\n";
    ss << "====================\n";
    ss << "Total paths: " << pImpl->paths.size() << "\n";
    ss << "Active paths: " << getActivePathCount() << "\n";
    ss << "Conflicts detected: " << pImpl->analytics.totalConflicts << "\n";
    ss << "Merges performed: " << pImpl->analytics.totalMerges << "\n";
    ss << "Path usage counts:\n";
    
    for (const auto& pair : pImpl->analytics.pathUsageCounts) {
        ss << "  " << pair.first << ": " << pair.second << "\n";
    }
    
    if (pImpl->analytics.startTime != std::chrono::system_clock::time_point()) {
        auto now = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::minutes>(now - pImpl->analytics.startTime);
        ss << "Uptime: " << duration.count() << " minutes\n";
    }
    
    return ss.str();
}

bool ConfigInheritance::loadInheritanceFromYAML(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file) {
            pImpl->logger->error("Failed to open inheritance file: " + filepath);
            return false;
        }
        
        YAML::Node root = YAML::Load(file);
        
        // Clear existing paths
        pImpl->paths.clear();
        
        // Parse inheritance configuration
        if (root["inheritance"]) {
            YAML::Node inheritance = root["inheritance"];
            
            if (inheritance["paths"]) {
                YAML::Node paths = inheritance["paths"];
                for (auto it = paths.begin(); it != paths.end(); ++it) {
                    std::string path = it->as<std::string>();
                    std::string priority = "medium"; // Default priority
                    
                    // Check for priority setting
                    if (it->IsMap() && it->as<YAML::Node>().IsDefined()) {
                        // In real implementation, would extract priority from map
                    }
                    
                    addInheritancePath(path, priority);
                }
            }
        }
        
        pImpl->logger->info("Inheritance configuration loaded from: " + filepath);
        return true;
    } catch (const YAML::Exception& e) {
        pImpl->logger->error("YAML parsing error: " + std::string(e.what()));
        return false;
    }
}

bool ConfigInheritance::loadInheritanceFromJSON(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file) {
            pImpl->logger->error("Failed to open JSON inheritance file: " + filepath);
            return false;
        }
        
        Json::Value root;
        Json::CharReaderBuilder builder;
        std::string errors;
        
        if (!builder(&file, &file, &root, &errors)) {
            pImpl->logger->error("JSON parsing error: " + errors);
            return false;
        }
        
        // Parse JSON inheritance configuration
        // Similar to YAML parsing
        if (root.isMember("inheritance")) {
            // In real implementation, would parse JSON structure
            pImpl->logger->info("JSON inheritance configuration loaded: " + filepath);
        }
        
        return true;
    } catch (const std::exception& e) {
        pImpl->logger->error("JSON parsing error: " + std::string(e.what()));
        return false;
    }
}

bool ConfigInheritance::saveInheritanceToYAML(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file) {
            pImpl->logger->error("Failed to open inheritance file for writing: " + filepath);
            return false;
        }
        
        file << exportInheritanceToYAML();
        pImpl->logger->info("Inheritance configuration saved to: " + filepath);
        return true;
    } catch (const std::exception& e) {
        pImpl->logger->error("Failed to save inheritance file: " + std::string(e.what()));
        return false;
    }
}

bool ConfigInheritance::saveInheritanceToJSON(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file) {
            pImpl->logger->error("Failed to open JSON inheritance file for writing: " + filepath);
            return false;
        }
        
        file << exportInheritanceToJSON();
        pImpl->logger->info("Inheritance configuration saved to: " + filepath);
        return true;
    } catch (const std::exception& e) {
        pImpl->logger->error("Failed to save JSON inheritance file: " + std::string(e.what()));
        return false;
    }
}

std::string ConfigInheritance::exportInheritanceToYAML() const {
    YAML::Emitter emitter;
    emitter << YAML::BeginMap;
    
    emitter << YAML::Key << "inheritance";
    emitter << YAML::Value << YAML::BeginMap;
    
    emitter << YAML::Key << "paths";
    emitter << YAML::Value << YAML::BeginSeq;
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (const auto& path : pImpl->paths) {
        emitter << YAML::BeginMap;
        emitter << YAML::Key << "path" << YAML::Value << path.path;
        emitter << YAML::Key << "priority" << YAML::Value << path.priority;
        emitter << YAML::Key << "enabled" << YAML::Value << path.enabled;
        emitter << YAML::EndMap;
    }
    
    emitter << YAML::EndSeq;
    emitter << YAML::EndMap;
    emitter << YAML::EndMap;
    
    return emitter.c_str();
}

std::string ConfigInheritance::exportInheritanceToJSON() const {
    Json::Value root;
    root["inheritance"]["paths"] = Json::Value(Json::arrayValue);
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (const auto& path : pImpl->paths) {
        Json::Value pathObj;
        pathObj["path"] = path.path;
        pathObj["priority"] = path.priority;
        pathObj["enabled"] = path.enabled;
        
        root["inheritance"]["paths"].append(pathObj);
    }
    
    Json::StreamWriterBuilder writer;
    writer["indentation"] = "  ";
    
    std::stringstream ss;
    std::unique_ptr<Json::StreamWriter> writerInstance(writer.newStreamWriter());
    writerInstance->write(root, &ss);
    
    return ss.str();
}

void ConfigInheritance::addDynamicInheritanceRule(const std::string& ruleName,
                                                 std::function<bool(const std::string&)> rule) {
    if (ruleName.empty() || !rule) {
        pImpl->logger->error("Invalid dynamic inheritance rule");
        return;
    }
    
    pImpl->dynamicRules.push_back({ruleName, rule});
    pImpl->logger->info("Dynamic inheritance rule added: " + ruleName);
}

void ConfigInheritance::removeDynamicInheritanceRule(const std::string& ruleName) {
    auto it = std::find_if(pImpl->dynamicRules.begin(), pImpl->dynamicRules.end(),
                          [&ruleName](const auto& pair) { return pair.first == ruleName; });
    
    if (it != pImpl->dynamicRules.end()) {
        pImpl->dynamicRules.erase(it);
        pImpl->logger->info("Dynamic inheritance rule removed: " + ruleName);
    }
}

bool ConfigInheritance::evaluateDynamicRules(const std::string& path) const {
    for (const auto& rule : pImpl->dynamicRules) {
        if (!rule.second(path)) {
            return false;
        }
    }
    return true;
}

void ConfigInheritance::registerInheritanceCallback(const std::string& eventType, InheritanceCallback callback) {
    if (eventType.empty() || !callback) {
        pImpl->logger->error("Invalid inheritance event or callback");
        return;
    }
    
    pImpl->eventCallbacks.push_back({eventType, callback});
}

void ConfigInheritance::triggerInheritanceEvent(const std::string& eventType, const std::string& path, const Config& config, bool added) {
    InheritanceEvent event(eventType, path, config, added);
    
    for (const auto& callbackEntry : pImpl->eventCallbacks) {
        if (callbackEntry.eventType == eventType) {
            try {
                callbackEntry.callback(path, config, added);
            } catch (const std::exception& e) {
                pImpl->logger->error("Error in inheritance callback: " + std::string(e.what()));
            }
        }
    }
}

ConfigInheritance::InheritanceStats ConfigInheritance::getInheritanceStats() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    InheritanceStats stats;
    stats.totalPaths = pImpl->paths.size();
    stats.activePaths = getActivePathCount();
    stats.conflictsDetected = pImpl->analytics.totalConflicts;
    stats.mergesPerformed = pImpl->analytics.totalMerges;
    stats.lastUpdate = pImpl->lastUpdate;
    stats.pathUsageCounts = pImpl->analytics.pathUsageCounts;
    stats.averageConflictResolutionTime = pImpl->analytics.averageConflictResolutionTime;
    
    return stats;
}

std::map<std::string, Config> ConfigInheritance::getCurrentConfigs() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::map<std::string, Config> currentConfigs;
    
    for (const auto& path : pImpl->paths) {
        currentConfigs[path.path] = path.config;
    }
    
    return currentConfigs;
}

size_t ConfigInheritance::getActivePathCount() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    size_t count = 0;
    for (const auto& path : pImpl->paths) {
        if (path.enabled && path.exists) {
            count++;
        }
    }
    return count;
}

bool ConfigInheritance::hasCircularReference(const std::vector<std::string>& paths) const {
    // Simple circular reference detection
    std::set<std::string> visited;
    
    for (const auto& path : paths) {
        if (visited.find(path) != visited.end()) {
            return true; // Found a cycle
        }
        
        visited.insert(path);
    }
    
    return false;
}

std::string ConfigInheritance::valueToString(const ConfigValue& value) const {
    if (std::holds_alternative<int>(value)) {
        return std::to_string(std::get<int>(value));
    } else if (std::holds_alternative<int64_t>(value)) {
        return std::to_string(std::get<int64_t>(value));
    } else if (std::holds_alternative<double>(value)) {
        return std::to_string(std::get<double>(value));
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? "true" : "false";
    } else if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        std::stringstream ss;
        ss << "[";
        const auto& vec = std::get<std::vector<int>>(value);
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << vec[i];
        }
        ss << "]";
        return ss.str();
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        std::stringstream ss;
        ss << "[";
        const auto& vec = std::get<std::vector<double>>(value);
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << vec[i];
        }
        ss << "]";
        return ss.str();
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        std::stringstream ss;
        ss << "[";
        const auto& vec = std::get<std::vector<std::string>>(value);
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << "\"" << vec[i] << "\"";
        }
        ss << "]";
        return ss.str();
    }
    
    return "";
}

} // namespace nlm
