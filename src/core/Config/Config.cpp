#pragma once

#include "Config.hpp"
#include "Errors.hpp"
#include "ErrorHandling.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include <openssl/sha.h>
#include <chrono>
#include <set>
#include <stdexcept>
#include <filesystem>

namespace nlm {

// Error reporting for Config
class ConfigErrorReporter {
public:
    static void reportFileError(const std::string& filepath, const std::string& operation, const std::exception& e) {
        NLM_LOG_ERROR("Config file error during " << operation << " on file " << filepath << ": " << e.what());
    }

    static void reportValidationError(const std::string& key, const std::string& expectedType, const std::string& actualType) {
        NLM_LOG_ERROR("Config validation error for key " << key << ": expected " << expectedType << ", got " << actualType);
    }

    static void reportSchemaError(const std::string& schemaError) {
        NLM_LOG_ERROR("Config schema error: " << schemaError);
    }
};

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unique_ptr<ConfigSchema> schema;
    std::unique_ptr<ConfigTemplate> template_;
    std::unique_ptr<ConfigHistory> history;
    std::map<std::string, ConfigNode> nested_config;
    std::string version;
    std::mutex file_mutex_;
    std::map<std::string, std::mutex*> file_locks_;
    std::map<std::string, std::chrono::system_clock::time_point> file_lock_times_;
    static constexpr int MAX_FILE_LOCK_TIME_MS = 5000;  // 5 seconds

    Impl() : version("1.0") {
        // Initialize with default schema if available
        if (template_) {
            applyTemplate();
        }
    }
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_DEBUG("Config instance created");
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    NLM_LOG_DEBUG("Loading config from file: " << filepath);
    
    NLM_TRY_CATCH(false) // Don't rethrow, handle gracefully
    
    std::string format = detectFileFormat(filepath);
    bool success = false;
    
    if (format == "json") {
        success = loadFromJson(filepath);
    } else if (format == "yaml" || format == "yml") {
        success = loadFromYaml(filepath);
    } else {
        // Fallback to old format
        NLM_LOG_DEBUG("Using legacy config format for file: " << filepath);
        success = loadFromLegacyFormat(filepath);
    }
    
    if (success) {
        NLM_LOG_INFO("Config loaded successfully from: " << filepath);
        backupFile(filepath);
    } else {
        NLM_LOG_WARNING("Failed to load config from: " << filepath);
    }
    
    return success;
}

bool Config::loadFromJson(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->file_mutex_);
    
    if (!lockFile(filepath)) {
        NLM_THROW(ErrorCode::CONFIG_FILE_NOT_FOUND, "Cannot lock file for JSON config: " + filepath);
    }
    
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            unlockFile(filepath);
            NLM_THROW(ErrorCode::CONFIG_FILE_NOT_FOUND, "Cannot open JSON config file: " + filepath);
        }
        
        json j;
        try {
            file >> j;
        } catch (const json::parse_error& e) {
            unlockFile(filepath);
            NLM_THROW(ErrorCode::CONFIG_INVALID_FORMAT, "Invalid JSON format in config file " + filepath + ": " + std::string(e.what()));
        }
        
        clear();
        
        validateJsonKeys(j);
        
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string key = it.key();
            
            // Convert JSON value to ConfigValue with error handling
            ConfigValue value;
            const auto& val = *it;
            
            try {
                value = jsonValueToConfigValue(val);
            } catch (const std::exception& e) {
                unlockFile(filepath);
                NLM_THROW(ErrorCode::CONFIG_TYPE_MISMATCH, "Failed to convert JSON value for key '" + key + ": " + std::string(e.what()));
            }
            
            set(key, value, ConfigSource::File);
        }
        
        unlockFile(filepath);
        NLM_LOG_INFO("JSON config loaded successfully: " << filepath);
        return true;
        
    } catch (const std::exception& e) {
        unlockFile(filepath);
        ConfigErrorReporter::reportFileError(filepath, "JSON loading", e);
        NLM_THROW(ErrorCode::CONFIG_INVALID_FORMAT, "Failed to load JSON config " + filepath + ": " + std::string(e.what()));
    }
}

bool Config::loadFromYaml(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->file_mutex_);
    
    if (!lockFile(filepath)) {
        NLM_THROW(ErrorCode::CONFIG_FILE_NOT_FOUND, "Cannot lock file for YAML config: " + filepath);
    }
    
    try {
        YAML::Node doc;
        try {
            doc = YAML::LoadFile(filepath);
        } catch (const YAML::Exception& e) {
            unlockFile(filepath);
            NLM_THROW(ErrorCode::CONFIG_INVALID_FORMAT, "Invalid YAML format in config file " + filepath + ": " + std::string(e.what()));
        }
        
        clear();
        
        validateYamlStructure(doc);
        
        for (const auto& node : doc) {
            std::string key = node.first.as<std::string>();
            
            // Convert YAML node to ConfigValue with error handling
            ConfigValue value;
            const YAML::Node& val = node.second;
            
            try {
                value = yamlNodeToConfigValue(val);
            } catch (const std::exception& e) {
                unlockFile(filepath);
                NLM_THROW(ErrorCode::CONFIG_TYPE_MISMATCH, "Failed to convert YAML value for key '" + key + ": " + std::string(e.what()));
            }
            
            set(key, value, ConfigSource::File);
        }
        
        unlockFile(filepath);
        NLM_LOG_INFO("YAML config loaded successfully: " << filepath);
        return true;
        
    } catch (const std::exception& e) {
        unlockFile(filepath);
        ConfigErrorReporter::reportFileError(filepath, "YAML loading", e);
        NLM_THROW(ErrorCode::CONFIG_INVALID_FORMAT, "Failed to load YAML config " + filepath + ": " + std::string(e.what()));
    }
}

bool Config::loadFromLegacyFormat(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_THROW(ErrorCode::CONFIG_FILE_NOT_FOUND, "Cannot open legacy config file: " + filepath);
        }
        
        std::string line;
        int lineNumber = 0;
        while (std::getline(file, line)) {
            lineNumber++;
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            size_t pos = line.find('=');
            if (pos == std::string::npos) {
                NLM_LOG_WARNING("Invalid config line " << lineNumber << " in " << filepath << ": missing '='");
                continue;
            }
            
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
        
        NLM_LOG_INFO("Legacy config loaded successfully: " << filepath);
        return true;
        
    } catch (const std::exception& e) {
        ConfigErrorReporter::reportFileError(filepath, "legacy loading", e);
        NLM_THROW(ErrorCode::CONFIG_INVALID_FORMAT, "Failed to load legacy config " + filepath + ": " + std::string(e.what()));
    }
}

void Config::validateJsonKeys(const json& j) {
    // Custom validation for JSON keys
    for (auto it = j.begin(); it != j.end(); ++it) {
        std::string key = it.key();
        
        if (key.empty()) {
            NLM_THROW(ErrorCode::CONFIG_VALIDATION_FAILED, "Empty key found in JSON config");
        }
        
        // Check for invalid characters in keys
        if (key.find_first_of(" ") != std::string::npos) {
            NLM_THROW(ErrorCode::CONFIG_VALIDATION_FAILED, "Config key contains spaces: '" + key + "'");
        }
    }
}

void Config::validateYamlStructure(const YAML::Node& doc) {
    // Custom validation for YAML structure
    if (!doc.IsMap()) {
        NLM_THROW(ErrorCode::CONFIG_INVALID_FORMAT, "YAML config root must be a mapping");
    }
    
    for (const auto& node : doc) {
        std::string key = node.first.as<std::string>();
        if (key.empty()) {
            NLM_THROW(ErrorCode::CONFIG_VALIDATION_FAILED, "Empty key found in YAML config");
        }
    }
}

ConfigValue Config::jsonValueToConfigValue(const json& val) {
    try {
        if (val.is_string()) {
            return val.get<std::string>();
        } else if (val.is_number_integer()) {
            return val.get<int>();
        } else if (val.is_number_float()) {
            return val.get<double>();
        } else if (val.is_boolean()) {
            return val.get<bool>();
        } else if (val.is_array()) {
            return jsonArrayToConfigValue(val);
        } else if (val.is_object()) {
            return jsonObjectToConfigValue(val);
        } else {
            NLM_THROW(ErrorCode::CONFIG_TYPE_MISMATCH, "Unsupported JSON value type");
        }
    } catch (const std::exception& e) {
        NLM_THROW(ErrorCode::CONFIG_TYPE_MISMATCH, "Failed to convert JSON value: " + std::string(e.what()));
    }
}

ConfigValue Config::yamlNodeToConfigValue(const YAML::Node& val) {
    try {
        if (val.IsScalar()) {
            if (val.IsInteger()) {
                return val.as<int>();
            } else if (val.IsFloat()) {
                return val.as<double>();
            } else if (val.IsBoolean()) {
                return val.as<bool>();
            } else {
                return val.as<std::string>();
            }
        } else if (val.IsSequence()) {
            return yamlSequenceToConfigValue(val);
        } else if (val.IsMap()) {
            return yamlMappingToConfigValue(val);
        } else {
            NLM_THROW(ErrorCode::CONFIG_TYPE_MISMATCH, "Unsupported YAML node type");
        }
    } catch (const std::exception& e) {
        NLM_THROW(ErrorCode::CONFIG_TYPE_MISMATCH, "Failed to convert YAML node: " + std::string(e.what()));
    }
}

ConfigValue Config::jsonArrayToConfigValue(const json& arr) {
    if (arr.empty()) {
        return std::vector<std::string>();
    }
    
    if (arr[0].is_number_integer()) {
        std::vector<int> int_arr;
        for (const auto& item : arr) {
            int_arr.push_back(item.get<int>());
        }
        return int_arr;
    } else if (arr[0].is_number_float()) {
        std::vector<double> double_arr;
        for (const auto& item : arr) {
            double_arr.push_back(item.get<double>());
        }
        return double_arr;
    } else {
        std::vector<std::string> string_arr;
        for (const auto& item : arr) {
            string_arr.push_back(item.get<std::string>());
        }
        return string_arr;
    }
}

ConfigValue Config::yamlSequenceToConfigValue(const YAML::Node& seq) {
    if (seq.empty()) {
        return std::vector<std::string>();
    }
    
    if (seq[0].IsInteger()) {
        std::vector<int> int_arr;
        for (const auto& item : seq) {
            int_arr.push_back(item.as<int>());
        }
        return int_arr;
    } else if (seq[0].IsFloat()) {
        std::vector<double> double_arr;
        for (const auto& item : seq) {
            double_arr.push_back(item.as<double>());
        }
        return double_arr;
    } else {
        std::vector<std::string> string_arr;
        for (const auto& item : seq) {
            string_arr.push_back(item.as<std::string>());
        }
        return string_arr;
    }
}

ConfigValue Config::yamlMappingToConfigValue(const YAML::Node& map) {
    ConfigValue dict;
    for (const auto& node : map) {
        std::string key = node.first.as<std::string>();
        ConfigValue value = yamlNodeToConfigValue(node.second);
        dict[key] = value;
    }
    return dict;
}

ConfigValue Config::jsonObjectToConfigValue(const json& obj) {
    ConfigValue dict;
    for (auto it = obj.begin(); it != obj.end(); ++it) {
        std::string key = it.key();
        ConfigValue value = jsonValueToConfigValue(*it);
        dict[key] = value;
    }
    return dict;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::string format = detectFileFormat(filepath);
    bool success = false;
    
    NLM_TRY_CATCH(false) // Don't rethrow, handle gracefully
    
    if (format == "json") {
        success = saveToJson(filepath);
    } else if (format == "yaml" || format == "yml") {
        success = saveToYaml(filepath);
    } else {
        // Fallback to old format
        NLM_LOG_DEBUG("Using legacy config format for save: " << filepath);
        success = saveToLegacyFormat(filepath);
    }
    
    if (success) {
        NLM_LOG_INFO("Config saved successfully to: " << filepath);
    } else {
        NLM_LOG_WARNING("Failed to save config to: " << filepath);
    }
    
    return success;
}

bool Config::saveToJson(const std::string& filepath) const {
    std::lock_guard<std::mutex> lock(pImpl->file_mutex_);
    
    if (!lockFile(filepath)) {
        NLM_THROW(ErrorCode::CONFIG_FILE_NOT_FOUND, "Cannot lock file for JSON save: " + filepath);
    }
    
    try {
        json j;
        
        for (const auto& entry : pImpl->entries) {
            j[entry.key] = entry.value;
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            unlockFile(filepath);
            NLM_THROW(ErrorCode::CONFIG_FILE_NOT_FOUND, "Cannot open JSON config file for writing: " + filepath);
        }
        
        try {
            file << std::setw(2) << j;
            file.flush();
        } catch (const std::exception& e) {
            unlockFile(filepath);
            ConfigErrorReporter::reportFileError(filepath, "JSON writing", e);
            NLM_THROW(ErrorCode::IO_ERROR, "Failed to write JSON config to " + filepath + ": " + std::string(e.what()));
        }
        
        unlockFile(filepath);
        backupFile(filepath);
        NLM_LOG_INFO("JSON config saved successfully: " << filepath);
        return true;
        
    } catch (const std::exception& e) {
        unlockFile(filepath);
        ConfigErrorReporter::reportFileError(filepath, "JSON saving", e);
        NLM_THROW(ErrorCode::IO_ERROR, "Failed to save JSON config " + filepath + ": " + std::string(e.what()));
    }
}

bool Config::saveToYaml(const std::string& filepath) const {
    std::lock_guard<std::mutex> lock(pImpl->file_mutex_);
    
    if (!lockFile(filepath)) {
        NLM_THROW(ErrorCode::CONFIG_FILE_NOT_FOUND, "Cannot lock file for YAML save: " + filepath);
    }
    
    try {
        YAML::Emitter out;
        out << YAML::BeginDoc;
        out << YAML::BeginMap;
        
        for (const auto& entry : pImpl->entries) {
            out << YAML::Key << entry.key;
            out << YAML::Value << configValueToYaml(entry.value);
        }
        
        out << YAML::EndMap;
        out << YAML::EndDoc;
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            unlockFile(filepath);
            NLM_THROW(ErrorCode::CONFIG_FILE_NOT_FOUND, "Cannot open YAML config file for writing: " + filepath);
        }
        
        try {
            file << out.c_str();
            file.flush();
        } catch (const std::exception& e) {
            unlockFile(filepath);
            ConfigErrorReporter::reportFileError(filepath, "YAML writing", e);
            NLM_THROW(ErrorCode::IO_ERROR, "Failed to write YAML config to " + filepath + ": " + std::string(e.what()));
        }
        
        unlockFile(filepath);
        backupFile(filepath);
        NLM_LOG_INFO("YAML config saved successfully: " << filepath);
        return true;
        
    } catch (const std::exception& e) {
        unlockFile(filepath);
        ConfigErrorReporter::reportFileError(filepath, "YAML saving", e);
        NLM_THROW(ErrorCode::IO_ERROR, "Failed to save YAML config " + filepath + ": " + std::string(e.what()));
    }
}

bool Config::saveToLegacyFormat(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_THROW(ErrorCode::CONFIG_FILE_NOT_FOUND, "Cannot open legacy config file for writing: " + filepath);
        }
        
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = " << configValueToJson(entry.value) << "\n";
        }
        
        file.flush();
        NLM_LOG_INFO("Legacy config saved successfully: " << filepath);
        return true;
        
    } catch (const std::exception& e) {
        ConfigErrorReporter::reportFileError(filepath, "legacy saving", e);
        NLM_THROW(ErrorCode::IO_ERROR, "Failed to save legacy config " + filepath + ": " + std::string(e.what()));
    }
}

std::string Config::configValueToJson(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, int>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, double>) {
            oss << std::setprecision(6) << arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << std::boolalpha << arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << std::setprecision(6) << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << arg[i] << "\"";
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::map<std::string, ConfigValue>>) {
            oss << "{";
            for (auto it = arg.begin(); it != arg.end(); ++it) {
                if (it != arg.begin()) oss << ", ";
                oss << "\"" << it->first << "\": " << configValueToJson(it->second);
            }
            oss << "}";
        }
    }, value);
    
    return oss.str();
}

std::string Config::configValueToYaml(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, int>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, double>) {
            oss << std::setprecision(6) << arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << std::boolalpha << arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << std::setprecision(6) << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << arg[i] << "\"";
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::map<std::string, ConfigValue>>) {
            oss << "{";
            for (auto it = arg.begin(); it != arg.end(); ++it) {
                if (it != arg.begin()) oss << ", ";
                oss << "\"" << it->first << "\": " << configValueToYaml(it->second);
            }
            oss << "}";
        }
    }, value);
    
    return oss.str();
}

bool Config::validate(std::vector<std::string>& errors) const {
    errors.clear();
    
    // Basic validation: check for required keys and types
    // This is a placeholder for more sophisticated validation
    for (const auto& entry : pImpl->entries) {
        if (entry.key.empty()) {
            errors.push_back("Empty key found in configuration");
        }
    }
    
    // Check for duplicate keys
    std::set<std::string> seenKeys;
    for (const auto& entry : pImpl->entries) {
        if (seenKeys.count(entry.key)) {
            errors.push_back("Duplicate config key: '" + entry.key + "'");
        } else {
            seenKeys.insert(entry.key);
        }
    }
    
    // Schema validation if schema is available
    if (pImpl->schema) {
        try {
            if (!pImpl->schema->validate(*this, errors)) {
                NLM_THROW(ErrorCode::SCHEMA_VALIDATION_FAILED, "Config validation failed");
            }
        } catch (const std::exception& e) {
            errors.push_back("Schema validation error: " + std::string(e.what()));
        }
    }
    
    return errors.empty();
}

void Config::applyTemplate() {
    // Apply template to current config
    if (pImpl->template_) {
        try {
            pImpl->template_->apply(*this);
        } catch (const std::exception& e) {
            NLM_LOG_WARNING("Failed to apply config template: " << e.what());
        }
    }
}

bool Config::lockFile(const std::string& filepath) {
    std::string lockFile = filepath + ".lock";
    
    // Check if file is already locked
    auto now = std::chrono::system_clock::now();
    auto lockTimeIt = pImpl->file_lock_times_.find(filepath);
    
    if (lockTimeIt != pImpl->file_lock_times_.end()) {
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - lockTimeIt->second);
        if (duration.count() < MAX_FILE_LOCK_TIME_MS) {
            return false; // File is locked
        } else {
            // Lock expired, remove it
            pImpl->file_lock_times_.erase(lockTimeIt);
        }
    }
    
    // Create lock file
    std::ofstream lockFileStream(lockFile);
    if (!lockFileStream.is_open()) {
        return false;
    }
    
    // Record lock
    pImpl->file_locks_[filepath] = new std::mutex();
    pImpl->file_lock_times_[filepath] = now;
    
    return true;
}

void Config::unlockFile(const std::string& filepath) {
    auto lockIt = pImpl->file_locks_.find(filepath);
    if (lockIt != pImpl->file_locks_.end()) {
        delete lockIt->second;
        pImpl->file_locks_.erase(lockIt);
    }
    
    pImpl->file_lock_times_.erase(filepath);
    
    // Remove lock file
    std::string lockFile = filepath + ".lock";
    std::filesystem::remove(lockFile);
}

void Config::cleanupExpiredLocks() {
    auto now = std::chrono::system_clock::now();
    
    for (auto it = pImpl->file_lock_times_.begin(); it != pImpl->file_lock_times_.end();) {
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - it->second);
        if (duration.count() >= MAX_FILE_LOCK_TIME_MS) {
            unlockFile(it->first);
            it = pImpl->file_lock_times_.erase(it);
        } else {
            ++it;
        }
    }
}

void Config::backupFile(const std::string& filepath) const {
    if (!std::filesystem::exists(filepath)) {
        return;
    }
    
    try {
        auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);
        std::tm tm_buf;
        gmtime_r(&time_t_now, &tm_buf);
        
        std::ostringstream oss;
        oss << std::put_time(&tm_buf, "%Y%m%d_%H%M%S");
        
        std::string backupFile = filepath + ".backup_" + oss.str();
        std::filesystem::copy_file(filepath, backupFile, std::filesystem::copy_options::overwrite_existing);
        
        NLM_LOG_DEBUG("Created backup of config file: " << backupFile);
        
    } catch (const std::exception& e) {
        NLM_LOG_WARNING("Failed to create backup of config file " << filepath << ": " << e.what());
    }
}

} // namespace nlm