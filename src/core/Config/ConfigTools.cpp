namespace nlm {

// ConfigDiff implementation

std::vector<ConfigDiff::Change> ConfigDiff::compare(const Config& config1, const Config& config2) {
    std::vector<Change> changes;
    
    // Get keys from both configurations
    std::vector<std::string> keys1 = config1.getKeys();
    std::vector<std::string> keys2 = config2.getKeys();
    
    // Find keys in config1 but not in config2
    for (const auto& key : keys1) {
        if (!config2.has(key)) {
            ConfigValue value;
            std::visit([&value](const auto& v) { value = v; }, 
                      config1.get<decltype(value)>(key).value());
            changes.push_back({key, configValueToJson(value).dump(), "", ConfigSource::Default, "Key removed"});
        }
    }
    
    // Find keys in config2 but not in config1
    for (const auto& key : keys2) {
        if (!config1.has(key)) {
            ConfigValue value;
            std::visit([&value](const auto& v) { value = v; }, 
                      config2.get<decltype(value)>(key).value());
            changes.push_back({key, "", configValueToJson(value).dump(), ConfigSource::Default, "Key added"});
        }
    }
    
    // Find keys in both but with different values
    for (const auto& key : keys1) {
        if (config2.has(key)) {
            ConfigValue value1, value2;
            std::visit([&value1](const auto& v) { value1 = v; }, 
                      config1.get<decltype(value1)>(key).value());
            std::visit([&value2](const auto& v) { value2 = v; }, 
                      config2.get<decltype(value2)>(key).value());
            
            auto json1 = configValueToJson(value1);
            auto json2 = configValueToJson(value2);
            
            if (json1 != json2) {
                changes.push_back({key, json1.dump(), json2.dump(), ConfigSource::Default, "Value changed"});
            }
        }
    }
    
    return changes;
}

std::string ConfigDiff::generateDiffReport(const std::vector<Change>& changes) {
    std::ostringstream oss;
    oss << "=== Configuration Diff Report ===\n\n";
    
    if (changes.empty()) {
        oss << "No changes detected.\n";
        return oss.str();
    }
    
    oss << "Changes:" << std::endl;
    for (const auto& change : changes) {
        oss << "  " << change.key << std::endl;
        if (!change.oldValue.empty()) {
            oss << "    Old: " << change.oldValue << std::endl;
        }
        if (!change.newValue.empty()) {
            oss << "    New: " << change.newValue << std::endl;
        }
        oss << "    Description: " << change.description << std::endl;
    }
    
    oss << "\nTotal changes: " << changes.size() << std::endl;
    return oss.str();
}

// ConfigValidator implementation

std::vector<ConfigValidator::ValidationIssue> ConfigValidator::validate(const Config& config) {
    std::vector<ValidationIssue> issues;
    
    // Check for required configuration
    std::vector<std::string> requiredKeys = {"simulation_timestep", "neuron_count", "region_count"};
    for (const auto& key : requiredKeys) {
        if (!config.has(key)) {
            issues.push_back({key, "Required key missing", 
                            "Add " + key + " to configuration", 
                            true});
        }
    }
    
    // Check for valid values
    if (config.has("simulation_timestep")) {
        auto timestep = config.get<double>("simulation_timestep");
        if (timestep && *timestep <= 0.0) {
            issues.push_back({"simulation_timestep", "Invalid timestep value",
                            "Timestep must be positive, suggested: 0.001",
                            false});
        }
    }
    
    if (config.has("neuron_count")) {
        auto neuronCount = config.get<int64_t>("neuron_count");
        if (neuronCount && *neuronCount < 0) {
            issues.push_back({"neuron_count", "Invalid neuron count",
                            "Neuron count must be non-negative",
                            false});
        }
    }
    
    if (config.has("region_count")) {
        auto regionCount = config.get<int64_t>("region_count");
        if (regionCount && *regionCount < 1) {
            issues.push_back({"region_count", "Invalid region count",
                            "Region count must be at least 1",
                            false});
        }
    }
    
    // Check for configuration consistency
    if (config.has("neuron_count") && config.has("region_count")) {
        auto neuronCount = config.get<int64_t>("neuron_count");
        auto regionCount = config.get<int64_t>("region_count");
        if (neuronCount && regionCount && *neuronCount / *regionCount > 10000) {
            issues.push_back({"distribution", "Potential performance issue",
                            "High neuron-to-region ratio may impact performance",
                            true});
        }
    }
    
    return issues;
}

std::string ConfigValidator::generateValidationReport(const std::vector<ValidationIssue>& issues) {
    std::ostringstream oss;
    oss << "=== Configuration Validation Report ===\n\n";
    
    if (issues.empty()) {
        oss << "Configuration is valid!\n";
        return oss.str();
    }
    
    int errorCount = 0, warningCount = 0;
    
    oss << "Validation Issues:" << std::endl;
    for (const auto& issue : issues) {
        std::string icon = issue.isWarning ? "!" : "X";
        oss << "  " << icon << " " << issue.key << ": " << issue.message << std::endl;
        if (!issue.suggestion.empty()) {
            oss << "      Suggestion: " << issue.suggestion << std::endl;
        }
        if (issue.isWarning) {
            warningCount++;
        } else {
            errorCount++;
        }
    }
    
    oss << "\nSummary:" << std::endl;
    oss << "  Errors: " << errorCount << std::endl;
    oss << "  Warnings: " << warningCount << std::endl;
    
    if (errorCount > 0) {
        oss << "\nConfiguration is NOT valid! Please fix the errors above.";
    } else if (warningCount > 0) {
        oss << "\nConfiguration has warnings but is valid.";
    }
    
    return oss.str();
}

bool ConfigValidator::canAutoFix(const ValidationIssue& issue) {
    return issue.suggestion.find("suggested:") != std::string::npos ||
           issue.suggestion.find("0.001") != std::string::npos;
}

// RuntimeConfigManager implementation

RuntimeConfigManager::RuntimeConfigManager(std::shared_ptr<Config> baseConfig)
    : baseConfig_(baseConfig), activeConfig_(std::make_shared<Config>(*baseConfig)),
      overlayConfig_(std::make_shared<Config>()), autoSaveEnabled_(false),
      backupDirectory_("./backups") {
    if (!std::filesystem::exists(backupDirectory_)) {
        std::filesystem::create_directories(backupDirectory_);
    }
}

bool RuntimeConfigManager::applyOverlay(const std::string& key, const ConfigValue& value) {
    overlayConfig_->set(key, value, ConfigSource::Runtime);
    
    // Update active config
    activeConfig_->set(key, value, ConfigSource::Runtime);
    
    // Record history
    configHistory_[key] = std::chrono::steady_clock::now();
    
    if (autoSaveEnabled_) {
        saveToFile("active_config.cfg", false);
    }
    
    return true;
}

bool RuntimeConfigManager::removeOverlay(const std::string& key) {
    overlayConfig_->remove(key);
    
    // Remove from active config if it's from overlay
    if (activeConfig_->has(key)) {
        auto entry = std::find_if(overlayConfig_->getKeys().begin(), 
                                 overlayConfig_->getKeys().end(),
                                 [&key](const std::string& k) { return k == key; });
        if (entry != overlayConfig_->getKeys().end()) {
            activeConfig_->remove(key);
        }
    }
    
    return true;
}

bool RuntimeConfigManager::clearOverlays() {
    overlayConfig_->clear();
    return true;
}

std::shared_ptr<Config> RuntimeConfigManager::getActiveConfig() const {
    return activeConfig_;
}

std::shared_ptr<Config> RuntimeConfigManager::getBaseConfig() const {
    return baseConfig_;
}

std::shared_ptr<Config> RuntimeConfigManager::getOverlayConfig() const {
    return overlayConfig_;
}

std::vector<ConfigError> RuntimeConfigManager::validate() const {
    std::vector<ConfigError> errors;
    
    // Validate base config
    auto validator = ConfigValidator();
    auto baseIssues = validator.validate(*baseConfig_);
    for (const auto& issue : baseIssues) {
        errors.push_back({issue.message, issue.key, ConfigError::Type::ValidationError});
    }
    
    // Validate active config
    auto activeIssues = validator.validate(*activeConfig_);
    for (const auto& issue : activeIssues) {
        errors.push_back({issue.message, issue.key, ConfigError::Type::ValidationError});
    }
    
    return errors;
}

bool RuntimeConfigManager::isValid() const {
    return validate().empty();
}

bool RuntimeConfigManager::saveToFile(const std::string& filepath, bool asBase) {
    std::shared_ptr<Config> configToSave;
    if (asBase) {
        configToSave = baseConfig_;
    } else {
        configToSave = activeConfig_;
    }
    
    return configToSave->saveToFile(filepath);
}

bool RuntimeConfigManager::backupCurrent(const std::string& name) {
    std::string backupName = name.empty() ? 
        "backup_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) : 
        name;
    
    std::string filepath = backupDirectory_ + "/" + backupName + ".json";
    return saveToFile(filepath, false);
}

bool RuntimeConfigManager::restoreBackup(const std::string& name) {
    std::string filepath = backupDirectory_ + "/" + name + ".json";
    if (!std::filesystem::exists(filepath)) {
        return false;
    }
    
    // Create new config and load from backup
    auto backupConfig = std::make_shared<Config>();
    bool success = backupConfig->loadFromFile(filepath);
    
    if (success) {
        activeConfig_ = backupConfig;
    }
    
    return success;
}

std::vector<std::string> RuntimeConfigManager::getAvailableBackups() const {
    std::vector<std::string> backups;
    
    for (const auto& entry : std::filesystem::directory_iterator(backupDirectory_)) {
        if (entry.path().extension() == ".json") {
            backups.push_back(entry.path().filename().string());
        }
    }
    
    return backups;
}

RuntimeConfigManager::ConfigProfile RuntimeConfigManager::profile() const {
    ConfigProfile profile;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Simulate some operations
    size_t accessCount = 0;
    for (const auto& key : activeConfig_->getKeys()) {
        auto value = activeConfig_->get<std::string>(key);
        (void)value; // Suppress unused variable warning
        accessCount++;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    
    profile.memoryUsage = activeConfig_->summary().size();
    profile.entryCount = activeConfig_->getKeys().size();
    profile.keyCount = activeConfig_->getKeys().size();
    profile.loadTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        endTime - startTime).count();
    profile.accessTime = 1; // Placeholder
    
    return profile;
}

bool RuntimeConfigManager::enableHistory(bool enable) {
    if (!enable) {
        configHistory_.clear();
    }
    return true;
}

std::vector<std::string> RuntimeConfigManager::getConfigHistory() const {
    std::vector<std::string> history;
    for (const auto& pair : configHistory_) {
        auto time = std::chrono::steady_clock::now() - pair.second;
        history.push_back(pair.first + " (modified " + 
                         std::to_string(std::chrono::duration_cast<std::chrono::seconds>(time).count()) +
                         "s ago)");
    }
    return history;
}

bool RuntimeConfigManager::revertToSnapshot(const std::string& timestamp) {
    clearOverlays();
    return true;
}

// ConfigMigration implementation

bool ConfigMigration::migrate(std::shared_ptr<Config> config, 
                              const std::vector<MigrationRule>& rules) {
    bool success = true;
    
    for (const auto& rule : rules) {
        bool ruleApplied = false;
        
        // Check if any keys match the pattern
        for (const auto& key : config->getKeys()) {
            if (key.find(rule.oldKeyPattern) != std::string::npos) {
                // Apply migration
                ConfigValue value;
                std::visit([&value](const auto& v) { value = v; }, 
                          config->get<decltype(value)>(key).value());
                
                ConfigValue newValue = rule.converter(value);
                config->remove(key);
                config->set(rule.newKey, newValue, ConfigSource::Runtime);
                
                ruleApplied = true;
            }
        }
        
        if (!ruleApplied && !rule.oldKeyPattern.empty()) {
            // Look for exact match
            if (config->has(rule.oldKeyPattern)) {
                ConfigValue value;
                std::visit([&value](const auto& v) { value = v; }, 
                          config->get<decltype(value)>(rule.oldKeyPattern).value());
                
                ConfigValue newValue = rule.converter(value);
                config->remove(rule.oldKeyPattern);
                config->set(rule.newKey, newValue, ConfigSource::Runtime);
                
                ruleApplied = true;
            }
        }
        
        if (!ruleApplied) {
            std::cerr << "Warning: Migration rule not applied: " << rule.description << std::endl;
            success = false;
        }
    }
    
    return success;
}

std::vector<std::string> ConfigMigration::detectConfigFormat(const std::string& filepath) {
    std::vector<std::string> formats;
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return formats;
    }
    
    std::string firstLine;
    std::getline(file, firstLine);
    
    // Check for JSON format
    if ((firstLine.find('{') != std::string::npos && firstLine.find('}') != std::string::npos) ||
        (firstLine.find('[') != std::string::npos && firstLine.find(']') != std::string::npos)) {
        formats.push_back("json");
    }
    
    // Check for key=value format
    if (firstLine.find('=') != std::string::npos || 
        (firstLine.find(':') != std::string::npos && firstLine.find('{') == std::string::npos)) {
        formats.push_back("key=value");
    }
    
    // Check for YAML format (simple heuristic)
    if (firstLine.find(": ") != std::string::npos || 
        firstLine.find("  ") != std::string::npos ||
        firstLine.find("\t") != std::string::npos) {
        formats.push_back("yaml");
    }
    
    return formats;
}

bool ConfigMigration::convertFromLegacy(std::shared_ptr<Config> config, 
                                        const std::string& legacyFormat) {
    auto legacyConfig = std::make_shared<Config>();
    
    if (legacyFormat == "key=value") {
        std::ifstream file("legacy_config.txt");
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            line = Config::trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = Config::trim(line.substr(0, pos));
                std::string value = Config::trim(line.substr(pos + 1));
                
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                legacyConfig->set(key, value, ConfigSource::File);
            }
        }
        
        for (const auto& key : legacyConfig->getKeys()) {
            auto value = legacyConfig->get<std::string>(key);
            if (value) {
                config->set(key, *value, ConfigSource::File);
            }
        }
        
        return true;
    }
    
    return false;
}

// ConfigDiagnostics implementation

std::vector<ConfigDiagnostics::DiagnosticResult> ConfigDiagnostics::runDiagnostics(const Config& config) {
    std::vector<DiagnosticResult> results;
    
    // Check basic structure
    DiagnosticResult structureCheck;
    structureCheck.component = "Basic Structure";
    structureCheck.isHealthy = !config.getKeys().empty();
    structureCheck.status = structureCheck.isHealthy ? "OK" : "Empty";
    structureCheck.details = {"Keys count: " + std::to_string(config.getKeys().size())};
    structureCheck.performanceScore = 1.0 / (1.0 + config.getKeys().size());
    results.push_back(structureCheck);
    
    // Check for errors
    DiagnosticResult errorCheck;
    errorCheck.component = "Error Handling";
    errorCheck.isHealthy = !config.hasError();
    errorCheck.status = errorCheck.isHealthy ? "OK" : "Has errors";
    errorCheck.details = {"Last error: " + config.getLastError()};
    errorCheck.performanceScore = errorCheck.isHealthy ? 1.0 : 0.5;
    results.push_back(errorCheck);
    
    // Check configuration size
    DiagnosticResult sizeCheck;
    sizeCheck.component = "Memory Usage";
    sizeCheck.isHealthy = config.summary().size() < 1024 * 1024; // < 1MB
    sizeCheck.status = sizeCheck.isHealthy ? "OK" : "Large";
    sizeCheck.details = {"Size: " + std::to_string(config.summary().size()) + " bytes"};
    sizeCheck.performanceScore = sizeCheck.isHealthy ? 1.0 : 0.7;
    results.push_back(sizeCheck);
    
    return results;
}

std::string ConfigDiagnostics::generateDiagnosticReport(const std::vector<DiagnosticResult>& results) {
    std::ostringstream oss;
    oss << "=== Configuration Diagnostics Report ===\n\n";
    
    int healthyCount = 0;
    for (const auto& result : results) {
        std::string status = result.isHealthy ? "✓" : "✗";
        oss << status << " " << result.component << ": " << result.status << std::endl;
        for (const auto& detail : result.details) {
            oss << "  " << detail << std::endl;
        }
        oss << "  Performance score: " << result.performanceScore * 100 << "%" << std::endl;
        oss << std::endl;
        
        if (result.isHealthy) {
            healthyCount++;
        }
    }
    
    oss << "Summary: " << healthyCount << "/" << results.size() << " components healthy\n";
    
    if (healthyCount == results.size()) {
        oss << "All diagnostics passed!";
    } else {
        oss << "Some diagnostics failed. Please review the issues above.";
    }
    
    return oss.str();
}

bool ConfigDiagnostics::checkCorruption(const Config& config) {
    for (const auto& key : config.getKeys()) {
        if (key.empty()) {
            return false;
        }
        
        auto value = config.get<std::string>(key);
        if (!value) {
            return false;
        }
    }
    
    return true;
}

std::map<std::string, double> ConfigDiagnostics::analyzeMemoryUsage(const Config& config) {
    std::map<std::string, double> analysis;
    
    analysis["total_size"] = config.summary().size();
    analysis["entry_count"] = config.getKeys().size();
    analysis["average_entry_size"] = config.getKeys().empty() ? 0.0 : 
        static_cast<double>(config.summary().size()) / config.getKeys().size();
    
    std::map<std::string, size_t> typeCounts;
    for (const auto& key : config.getKeys()) {
        ConfigValue value;
        std::visit([&value, &typeCounts](const auto& v) {
            typeCounts[typeid(v).name()]++;
            value = v;
        }, config.get<decltype(value)>(key).value());
    }
    
    for (const auto& pair : typeCounts) {
        analysis["type_" + pair.first] = static_cast<double>(pair.second);
    }
    
    return analysis;
}

// ConfigTemplate implementation

ConfigTemplate::ConfigTemplate(const std::string& name, const std::string& description)
    : name_(name), description_(description) {}

void ConfigTemplate::addEntry(const std::string& key, const ConfigValue& defaultValue,
                              const std::string& description, bool isRequired,
                              const std::vector<std::string>& dependsOn) {
    entries_.push_back({key, defaultValue, description, isRequired, dependsOn});
}

bool ConfigTemplate::validate(const Config& config) const {
    for (const auto& entry : entries_) {
        if (entry.isRequired && !config.has(entry.key)) {
            return false;
        }
    }
    return true;
}

std::shared_ptr<Config> ConfigTemplate::createConfig() const {
    auto config = std::make_shared<Config>();
    for (const auto& entry : entries_) {
        config->set(entry.key, entry.defaultValue, ConfigSource::Default);
    }
    return config;
}

std::vector<std::string> ConfigTemplate::getAvailableTemplates() {
    return {"default", "performance", "debug"};
}

std::shared_ptr<ConfigTemplate> ConfigTemplate::loadTemplate(const std::string& name) {
    auto template = std::make_shared<ConfigTemplate>("temp", "");
    
    if (name == "default") {
        template->addEntry("simulation_timestep", 0.001, "Simulation timestep", true);
        template->addEntry("neuron_count", 1000, "Number of neurons", true);
        template->addEntry("region_count", 10, "Number of regions", true);
    } else if (name == "performance") {
        template->addEntry("simulation_timestep", 0.01, "Faster timestep", true);
        template->addEntry("neuron_count", 5000, "Fewer neurons", true);
        template->addEntry("region_count", 5, "Fewer regions", true);
    }
    
    return template;
}

bool ConfigTemplate::saveTemplate(const ConfigTemplate& template) {
    std::cout << "Saving template: " << template.getName() << std::endl;
    return true;
}

// ConfigBackupManager implementation

ConfigBackupManager::ConfigBackupManager(const std::string& directory, 
                                       size_t maxBackups, 
                                       bool compressionEnabled)
    : backupDirectory_(directory), maxBackups_(maxBackups),
      compressionEnabled_(compressionEnabled) {
    if (!std::filesystem::exists(backupDirectory_)) {
        std::filesystem::create_directories(backupDirectory_);
    }
}

bool ConfigBackupManager::createBackup(const Config& config, const std::string& name) {
    std::string backupName = name.empty() ? 
        "backup_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) : 
        name;
    
    std::string filepath = backupDirectory_ + "/" + backupName + ".json";
    return config.saveToFile(filepath);
}

bool ConfigBackupManager::restoreBackup(const std::string& name, std::shared_ptr<Config> config) {
    std::string filepath = backupDirectory_ + "/" + name + ".json";
    if (!std::filesystem::exists(filepath)) {
        return false;
    }
    
    return config->loadFromFile(filepath);
}

std::vector<std::string> ConfigBackupManager::listBackups() const {
    std::vector<std::string> backups;
    
    for (const auto& entry : std::filesystem::directory_iterator(backupDirectory_)) {
        if (entry.path().extension() == ".json") {
            backups.push_back(entry.path().filename().string());
        }
    }
    
    return backups;
}

bool ConfigBackupManager::deleteBackup(const std::string& name) {
    std::string filepath = backupDirectory_ + "/" + name;
    if (std::filesystem::exists(filepath + ".json")) {
        return std::filesystem::remove(filepath + ".json");
    }
    return false;
}

bool ConfigBackupManager::cleanupOldBackups() {
    auto backups = getBackupInfo();
    
    if (backups.size() <= maxBackups_) {
        return true;
    }
    
    std::sort(backups.begin(), backups.end(), 
              [](const BackupInfo& a, const BackupInfo& b) {
                  return a.timestamp < b.timestamp;
              });
    
    size_t toDelete = backups.size() - maxBackups_;
    for (size_t i = 0; i < toDelete; ++i) {
        deleteBackup(backups[i].name);
    }
    
    return true;
}

std::vector<ConfigBackupManager::BackupInfo> ConfigBackupManager::getBackupInfo() const {
    std::vector<BackupInfo> info;
    
    for (const auto& entry : std::filesystem::directory_iterator(backupDirectory_)) {
        if (entry.path().extension() == ".json") {
            BackupInfo backupInfo;
            backupInfo.name = entry.path().filename().string();
            backupInfo.timestamp = std::chrono::system_clock::now();
            backupInfo.size = std::filesystem::file_size(entry.path());
            backupInfo.checksum = "sha256_placeholder";
            
            info.push_back(backupInfo);
        }
    }
    
    return info;
}

// ConfigProfiler implementation

ConfigProfiler::ConfigProfiler() : isProfiling_(false) {}

void ConfigProfiler::startProfiling() {
    std::lock_guard<std::mutex> lock(mutex_);
    isProfiling_ = true;
    session_.startTime = std::chrono::steady_clock::now();
    session_.totalAccesses = 0;
    session_.successfulAccesses = 0;
    session_.failedAccesses = 0;
}

void ConfigProfiler::stopProfiling() {
    std::lock_guard<std::mutex> lock(mutex_);
    isProfiling_ = false;
}

void ConfigProfiler::recordAccess(const std::string& operation, 
                                   const std::chrono::milliseconds& duration,
                                   bool success) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!isProfiling_) return;
    
    session_.totalAccesses++;
    if (success) {
        session_.successfulAccesses++;
    } else {
        session_.failedAccesses++;
    }
    
    session_.operationTimes.push_back({operation, duration});
}

std::string ConfigProfiler::generateProfileReport() const {
    std::ostringstream oss;
    oss << "=== Configuration Profile Report ===\n\n";
    
    if (session_.totalAccesses == 0) {
        oss << "No profiling data available.\n";
        return oss.str();
    }
    
    oss << "Profiling Statistics:" << std::endl;
    oss << "  Total accesses: " << session_.totalAccesses << std::endl;
    oss << "  Successful: " << session_.successfulAccesses << std::endl;
    oss << "  Failed: " << session_.failedAccesses << std::endl;
    
    if (session_.totalAccesses > 0) {
        double successRate = static_cast<double>(session_.successfulAccesses) / session_.totalAccesses * 100;
        oss << "  Success rate: " << successRate << "%" << std::endl;
    }
    
    if (!session_.operationTimes.empty()) {
        oss << "\nOperation Times:" << std::endl;
        for (const auto& pair : session_.operationTimes) {
            oss << "  " << pair.first << ": " << pair.second.count() << " ms" << std::endl;
        }
    }
    
    return oss.str();
}

std::map<std::string, double> ConfigProfiler::getPerformanceStats() const {
    std::map<std::string, double> stats;
    
    if (session_.totalAccesses == 0) {
        return stats;
    }
    
    double totalTime = 0;
    for (const auto& pair : session_.operationTimes) {
        totalTime += pair.second.count();
    }
    
    stats["average_access_time_ms"] = totalTime / session_.totalAccesses;
    stats["total_time_ms"] = totalTime;
    stats["accesses_per_second"] = session_.totalAccesses / (totalTime / 1000.0);
    
    return stats;
}

// ConfigEditor implementation

bool ConfigEditor::startInteractiveEditor(std::shared_ptr<Config> config) {
    std::cout << "=== Configuration Editor ===" << std::endl;
    std::cout << "Available commands:" << std::endl;
    std::cout << "  help - Show this help" << std::endl;
    std::cout << "  show - Show current configuration" << std::endl;
    std::cout << "  set <key> <value> - Set a key-value pair" << std::endl;
    std::cout << "  get <key> - Get a value" << std::endl;
    std::cout << "  remove <key> - Remove a key" << std::endl;
    std::cout << "  save <file> - Save configuration to file" << std::endl;
    std::cout << "  load <file> - Load configuration from file" << std::endl;
    std::cout << "  exit - Exit editor" << std::endl;
    std::cout << std::endl;
    
    EditSession session;
    session.config = config;
    session.currentPosition = 0;
    
    std::string line;
    while (std::getline(std::cin, line)) {
        std::istringstream iss(line);
        std::string command;
        iss >> command;
        
        if (command == "help") {
            std::cout << generateConfigEditorHelp() << std::endl;
        } else if (command == "show") {
            std::cout << "Current configuration:" << std::endl;
            std::cout << config->summary() << std::endl;
        } else if (command == "set") {
            std::string key, value;
            if (iss >> key >> value) {
                config->set(key, value, ConfigSource::Runtime);
                std::cout << "Set " << key << " = " << value << std::endl;
            } else {
                std::cout << "Usage: set <key> <value>" << std::endl;
            }
        } else if (command == "get") {
            std::string key;
            if (iss >> key) {
                auto val = config->get<std::string>(key);
                if (val) {
                    std::cout << key << " = " << *val << std::endl;
                } else {
                    std::cout << "Key not found: " << key << std::endl;
                }
            } else {
                std::cout << "Usage: get <key>" << std::endl;
            }
        } else if (command == "remove") {
            std::string key;
            if (iss >> key) {
                config->remove(key);
                std::cout << "Removed " << key << std::endl;
            } else {
                std::cout << "Usage: remove <key>" << std::endl;
            }
        } else if (command == "save") {
            std::string filepath;
            if (iss >> filepath) {
                bool success = config->saveToFile(filepath);
                if (success) {
                    std::cout << "Configuration saved to " << filepath << std::endl;
                } else {
                    std::cout << "Failed to save configuration to " << filepath << std::endl;
                }
            } else {
                std::cout << "Usage: save <file>" << std::endl;
            }
        } else if (command == "load") {
            std::string filepath;
            if (iss >> filepath) {
                bool success = config->loadFromFile(filepath);
                if (success) {
                    std::cout << "Configuration loaded from " << filepath << std::endl;
                } else {
                    std::cout << "Failed to load configuration from " << filepath << std::endl;
                }
            } else {
                std::cout << "Usage: load <file>" << std::endl;
            }
        } else if (command == "exit") {
            break;
        } else {
            std::cout << "Unknown command: " << command << std::endl;
            std::cout << "Type 'help' for available commands" << std::endl;
        }
    }
    
    return true;
}

std::string ConfigEditor::generateConfigEditorHelp() {
    return "=== Configuration Editor Help ===\n\n"
           "This is an interactive editor for NLM configuration files.\n\n"
           "Commands:\n"
           "  help     - Show this help message\n"
           "  show     - Display the current configuration\n"
           "  set key value - Set a configuration value\n"
           "  get key     - Get a configuration value\n"
           "  remove key  - Remove a configuration key\n"
           "  save file   - Save configuration to file\n"
           "  load file   - Load configuration from file\n"
           "  exit        - Exit the editor\n\n"
           "The editor supports both JSON and key=value format configurations.\n";
}

bool ConfigEditor::exportConfig(const Config& config, const std::string& filepath) {
    return config.saveToFile(filepath);
}

bool ConfigEditor::importConfig(std::shared_ptr<Config> config, const std::string& filepath) {
    return config->loadFromFile(filepath);
}

std::string ConfigEditor::formatConfig(const Config& config, const std::string& format) {
    if (format == "json") {
        return config.saveToJSONString();
    } else if (format == "key=value") {
        return config.saveToStringLegacy();
    } else if (format == "auto") {
        if (!config.saveToJSONString().empty()) {
            return config.saveToJSONString();
        }
        return config.saveToStringLegacy();
    }
    return "";
}

bool ConfigEditor::parseConfig(std::shared_ptr<Config> config, const std::string& content, const std::string& format) {
    if (format == "auto") {
        if (content.find('{') != std::string::npos && content.find('}') != std::string::npos) {
            return config->loadFromJSONString(content);
        } else {
            return config->loadFromStringLegacy(content);
        }
    } else if (format == "json") {
        return config->loadFromJSONString(content);
    } else if (format == "key=value") {
        return config->loadFromStringLegacy(content);
    }
    return false;
}

} // namespace nlm
