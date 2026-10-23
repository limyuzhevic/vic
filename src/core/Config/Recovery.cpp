// Recovery.cpp
// Implementation of configuration error handling and recovery for NLM

#include "Recovery.hpp"
#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <variant>
#include <typeinfo>
#include <regex>
#include <iostream>
#include <stdexcept>
#include <chrono>
#include <ctime>

namespace nlm {

struct ConfigRecovery::Impl {
    std::function<void(const ConfigOperationError&)> errorHandler;
    bool autoRecoveryEnabled;
    size_t errorCount;
    std::vector<ConfigOperationError> recentErrors;
    
    Impl() : autoRecoveryEnabled(true), errorCount(0) {}
};

ConfigRecovery::ConfigRecovery() : pImpl(std::make_unique<Impl>()) {
    // Set default error handler
    setErrorHandler([](const ConfigOperationError& error) {
        std::cerr << "Configuration Error: " << error.what();
        if (!error.getContext().empty()) {
            std::cerr << " (Context: " << error.getContext() << ")";
        }
        std::cerr << std::endl;
    });
}

ConfigRecovery::~ConfigRecovery() = default;

ConfigRecovery::ConfigRecovery(ConfigRecovery&&) noexcept = default;

ConfigRecovery& ConfigRecovery::operator=(ConfigRecovery&&) noexcept = default;

void ConfigRecovery::setErrorHandler(std::function<void(const ConfigOperationError&)> handler) {
    pImpl->errorHandler = handler;
}

std::function<void(const ConfigOperationError&)> ConfigRecovery::getErrorHandler() const {
    return pImpl->errorHandler;
}

void ConfigRecovery::setAutoRecovery(bool enable) {
    pImpl->autoRecoveryEnabled = enable;
}

bool ConfigRecovery::isAutoRecoveryEnabled() const {
    return pImpl->autoRecoveryEnabled;
}

bool ConfigRecovery::handleError(const ConfigOperationError& error) {
    // Call the error handler
    if (pImpl->errorHandler) {
        pImpl->errorHandler(error);
    }
    
    // Record the error
    pImpl->recentErrors.push_back(error);
    pImpl->errorCount++;
    
    // If auto-recovery is enabled, attempt to recover
    if (pImpl->autoRecoveryEnabled) {
        return true; // Error handling is successful
    }
    
    return false;
}

bool ConfigRecovery::validateAndRecover(class Config& config) {
    try {
        // Validate the configuration
        const ConfigSchema& schema = config.getSchema();
        std::map<std::string, ConfigEntry> tempEntries;
        
        // Convert config entries to map for validation
        for (const auto& entry : config.getKeys()) {
            auto value = config.get<std::string>(entry);
            if (value.has_value()) {
                tempEntries[entry] = ConfigEntry(entry, ConfigValue(*value), ConfigSource::Runtime);
            }
        }
        
        // Apply schema validation
        schema.validateAll(tempEntries);
        
        // If validation passes, apply the changes
        config.clear();
        for (const auto& pair : tempEntries) {
            config.set(pair.first, pair.second.value, ConfigSource::Runtime);
        }
        
        return true;
    } catch (const ConfigValidationError& e) {
        ConfigOperationError error(
            "Configuration validation failed: " + std::string(e.what()),
            "Validation failed for key: " + e.getKey()
        );
        handleError(error);
        return false;
    } catch (const std::exception& e) {
        ConfigOperationError error(
            "Configuration recovery failed: " + std::string(e.what()),
            "Recovery operation failed"
        );
        handleError(error);
        return false;
    }
}

std::string ConfigRecovery::getStatistics() const {
    std::ostringstream oss;
    oss << "Configuration Recovery Statistics:" << std::endl;
    oss << "  Auto recovery enabled: " << (pImpl->autoRecoveryEnabled ? "yes" : "no") << std::endl;
    oss << "  Total errors handled: " << pImpl->errorCount << std::endl;
    oss << "  Recent errors: " << pImpl->recentErrors.size() << std::endl;
    
    if (!pImpl->recentErrors.empty()) {
        oss << "  Recent error types:" << std::endl;
        for (const auto& error : pImpl->recentErrors) {
            oss << "    - " << error.what() << std::endl;
        }
    }
    
    return oss.str();
}

struct ConfigBackup::Impl {
    std::map<std::string, std::string> backups; // label -> backup file path
    std::string backupDir;
    
    Impl() : backupDir("backups") {
        // Ensure backup directory exists
        std::filesystem::create_directories(backupDir);
    }
};

ConfigBackup::ConfigBackup() : pImpl(std::make_unique<Impl>()) {}

ConfigBackup::~ConfigBackup() = default;

ConfigBackup::ConfigBackup(ConfigBackup&&) noexcept = default;

ConfigBackup& ConfigBackup::operator=(ConfigBackup&&) noexcept = default;

void ConfigBackup::createBackup(class Config& config, const std::string& label) {
    std::string timestamp = getCurrentTime();
    std::string filename = pImpl->backupDir + "/" + label + "_" + timestamp + ".cfg";
    
    if (config.saveToFile(filename)) {
        pImpl->backups[label] = filename;
    } else {
        throw ConfigOperationError("Failed to create backup: " + filename, "Backup creation failed");
    }
}

bool ConfigBackup::restoreBackup(class Config& config, const std::string& label) {
    if (pImpl->backups.find(label) == pImpl->backups.end()) {
        return false;
    }
    
    std::string filename = pImpl->backups[label];
    if (config.loadFromFile(filename)) {
        return true;
    }
    
    return false;
}

void ConfigBackup::removeBackup(const std::string& label) {
    if (pImpl->backups.find(label) != pImpl->backups.end()) {
        std::string filename = pImpl->backups[label];
        std::filesystem::remove(filename);
        pImpl->backups.erase(label);
    }
}

std::vector<std::string> ConfigBackup::listBackups() const {
    std::vector<std::string> labels;
    for (const auto& pair : pImpl->backups) {
        labels.push_back(pair.first);
    }
    return labels;
}

void ConfigBackup::clearBackups() {
    for (const auto& pair : pImpl->backups) {
        std::filesystem::remove(pair.second);
    }
    pImpl->backups.clear();
}

std::string ConfigBackup::getBackupInfo(const std::string& label) const {
    if (pImpl->backups.find(label) == pImpl->backups.end()) {
        return "Backup not found: " + label;
    }
    
    std::string filename = pImpl->backups.at(label);
    auto fileTime = std::filesystem::last_write_time(filename);
    auto fileAge = std::chrono::duration_cast<std::chrono::minutes>(
        std::chrono::system_clock::now().time_since_epoch() - 
        std::chrono::time_point_cast<std::chrono::system_clock::duration>(
            fileTime.time_since_epoch()
        ).time_since_epoch()
    );
    
    std::ostringstream oss;
    oss << "Backup: " << label << std::endl;
    oss << "  File: " << filename << std::endl;
    oss << "  Age: " << fileAge.count() << " minutes" << std::endl;
    oss << "  Size: " << std::filesystem::file_size(filename) << " bytes" << std::endl;
    
    return oss.str();
}

std::string getCurrentTime() {
    std::time_t now = std::time(nullptr);
    std::tm* timeinfo = std::gmtime(&now);
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%Y%m%d_%H%M%S", timeinfo);
    return std::string(buffer);
}

struct ConfigValidator::Impl {
    bool strictValidation;
    std::vector<std::string> customRules;
    
    Impl() : strictValidation(true) {}
};

ConfigValidator::ConfigValidator() : pImpl(std::make_unique<Impl>()) {}

ConfigValidator::~ConfigValidator() = default;

ConfigValidator::ConfigValidator(ConfigValidator&&) noexcept = default;

ConfigValidator& ConfigValidator::operator=(ConfigValidator&&) noexcept = default;

std::vector<ConfigOperationError> ConfigValidator::validate(class Config& config) {
    std::vector<ConfigOperationError> errors;
    
    // Validate each key in the configuration
    for (const auto& key : config.getKeys()) {
        // Get the schema entry for this key
        const ConfigSchemaEntry* schemaEntry = config.getSchema().getEntry(key);
        if (schemaEntry) {
            // Try to get the value as string for validation
            auto valueStr = config.get<std::string>(key);
            if (valueStr.has_value()) {
                auto validationErrors = validateParameter(config, key, ConfigValue(*valueStr));
                errors.insert(errors.end(), validationErrors.begin(), validationErrors.end());
            }
        } else {
            errors.emplace_back(
                "Unknown configuration key: " + key,
                "Validation of unknown key"
            );
        }
    }
    
    return errors;
}

std::vector<ConfigOperationError> ConfigValidator::validateParameter(class Config& config, const std::string& key, const ConfigValue& value) {
    std::vector<ConfigOperationError> errors;
    
    const ConfigSchemaEntry* schemaEntry = config.getSchema().getEntry(key);
    if (!schemaEntry) {
        errors.emplace_back(
            "Unknown configuration key: " + key,
            "Parameter validation failed"
        );
        return errors;
    }
    
    // Type validation
    if (!config.getSchema().checkType(schemaEntry->type, value)) {
        errors.emplace_back(
            "Invalid value type for key \"" + key + "\": expected " + schemaEntry->type,
            "Type validation failed"
        );
    }
    
    // Range validation
    try {
        config.getSchema().validateRange(*schemaEntry, value);
    } catch (const ConfigValidationError& e) {
        errors.emplace_back(
            e.what(),
            "Range validation failed for key: " + key
        );
    }
    
    return errors;
}

std::vector<std::string> ConfigValidator::getValidationSuggestions(class Config& config, const std::string& key, const ConfigValue& value) {
    std::vector<std::string> suggestions;
    
    const ConfigSchemaEntry* schemaEntry = config.getSchema().getEntry(key);
    if (!schemaEntry) {
        suggestions.push_back("Add this key to the configuration schema");
        return suggestions;
    }
    
    // Get suggestions based on the value type
    if (std::holds_alternative<double>(value)) {
        double val = std::get<double>(value);
        if (schemaEntry->min_value.has_value() && val < schemaEntry->min_value.value()) {
            suggestions.push_back("Increase the value to at least " + std::to_string(schemaEntry->min_value.value()));
        }
        if (schemaEntry->max_value.has_value() && val > schemaEntry->max_value.value()) {
            suggestions.push_back("Decrease the value to at most " + std::to_string(schemaEntry->max_value.value()));
        }
    }
    
    return suggestions;
}

void ConfigValidator::setStrictValidation(bool strict) {
    pImpl->strictValidation = strict;
}

bool ConfigValidator::isStrictValidation() const {
    return pImpl->strictValidation;
}

} // namespace nlm
