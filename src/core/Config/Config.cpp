// Advanced configuration implementation with comprehensive validation support
// Enhanced version with advanced validation capabilities

#include "Config.hpp"
#include "core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <cctype>
#include <chrono>
#include <thread>
#include <random>
#include <map>
#include <stdexcept>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ConfigSchemaEntry> schema;
    std::string lastLoadedFile;
    std::string lastBackedUpFile;
    std::unordered_map<std::string, std::any> tempData;
    bool inTransaction;
    int retryCount;
    std::vector<ValidationGroup> validationGroups;
    std::vector<ValidationDependency> validationDependencies;
    bool enableCrossValidation;
    bool enableBusinessLogicValidation;
    ValidationLevel defaultValidationLevel;
    std::unordered_map<std::string, ValidationResult> validationCache;
    std::string lastValidationKey;
    std::chrono::system_clock::time_point lastValidationTime;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize validation manager
    validationManager_ = std::make_unique<ValidationManager>();
    
    // Initialize with default schema validation
    addSchemaEntry("random_seed", typeid(int), false, "Random seed for reproducible experiments");
    addSchemaEntry("simulation_timestep", typeid(double), false, "Simulation timestep in seconds");
    addSchemaEntry("neuron_count", typeid(int), false, "Number of neurons in the brain");
    addSchemaEntry("region_count", typeid(int), false, "Number of brain regions");
    addSchemaEntry("connection_probability", typeid(double), false, "Connection probability for random connectivity");
    addSchemaEntry("plasticity_learning_rate", typeid(double), false, "Plasticity learning rate");
    addSchemaEntry("log_level", typeid(std::string), false, "Log level (DEBUG, INFO, WARNING, ERROR, CRITICAL)");
    addSchemaEntry("log_to_file", typeid(bool), false, "Enable logging to file");
    addSchemaEntry("environment_name", typeid(std::string), false, "Environment name");
    addSchemaEntry("environment_width", typeid(int), false, "Environment width");
    addSchemaEntry("environment_height", typeid(int), false, "Environment height");
    addSchemaEntry("max_simulation_steps", typeid(int), false, "Maximum simulation steps");
    addSchemaEntry("simulation_time_limit", typeid(double), false, "Simulation time limit");
    addSchemaEntry("visualization_enabled", typeid(bool), false, "Enable visualization");
    
    // Initialize validation manager
    pImpl->enableCrossValidation = true;
    pImpl->enableBusinessLogicValidation = true;
    pImpl->defaultValidationLevel = ValidationLevel::Strict;
    pImpl->inTransaction = false;
    pImpl->retryCount = 0;
    pImpl->lastValidationTime = std::chrono::system_clock::now();
}

Config::~Config() {
    // Ensure cleanup on destruction
    if (pImpl->inTransaction) {
        logWarning("Config destructor", "Transaction in progress - attempting cleanup");
        rollbackTransaction();
    }
    cleanupTempFiles();
}

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// ConfigFileResult implementation
ConfigFileResult Config::checkFileOperation(const std::string& filepath, bool write) {
    ConfigFileResult result;
    
    try {
        std::filesystem::path fsPath(filepath);
        
        if (write) {
            // For write operations, check if parent directory exists and is writable
            if (!std::filesystem::exists(fsPath.parent_path())) {
                result.success = false;
                result.message = "Parent directory does not exist: " + fsPath.parent_path().string();
                result.errorCode = ConfigErrorCode::FileNotFound;
                return result;
            }
            
            if (!std::filesystem::is_directory(fsPath.parent_path())) {
                result.success = false;
                result.message = "Parent path is not a directory: " + fsPath.parent_path().string();
                result.errorCode = ConfigErrorCode::OperationFailed;
                return result;
            }
            
            // Check if we can write to the directory
            std::string tempFile = fsPath.parent_path().string() + "/test_write_" + 
                                   std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
            std::ofstream testFile(tempFile);
            if (!testFile.is_open()) {
                result.success = false;
                result.message = "Permission denied: Cannot write to directory " + fsPath.parent_path().string();
                result.errorCode = ConfigErrorCode::PermissionDenied;
            } else {
                testFile.close();
                std::remove(tempFile.c_str());
                result.success = true;
                result.message = "Directory is writable";
            }
        } else {
            // For read operations, check if file exists
            if (!std::filesystem::exists(fsPath)) {
                result.success = false;
                result.message = "File not found: " + filepath;
                result.errorCode = ConfigErrorCode::FileNotFound;
                return result;
            }
            
            if (!std::filesystem::is_regular_file(fsPath)) {
                result.success = false;
                result.message = "Path is not a regular file: " + filepath;
                result.errorCode = ConfigErrorCode::OperationFailed;
                return result;
            }
            
            // Check file size
            uintmax_t fileSize = std::filesystem::file_size(fsPath);
            if (fileSize > 10 * 1024 * 1024) { // 10MB limit
                result.success = false;
                result.message = "File too large: " + filepath + " (" + std::to_string(fileSize) + " bytes)";
                result.errorCode = ConfigErrorCode::FileTooLarge;
                return result;
            }
            
            result.success = true;
            result.message = "File exists and is readable";
        }
        
    } catch (const std::filesystem::filesystem_error& e) {
        result.success = false;
        result.message = std::string("Filesystem error: ") + e.what();
        result.errorCode = ConfigErrorCode::IOError;
    } catch (const std::exception& e) {
        result.success = false;
        result.message = std::string("Error checking file: ") + e.what();
        result.errorCode = ConfigErrorCode::Unknown;
    }
    
    return result;
}

std::string Config::generateBackupFilename(const std::string& originalPath) {
    std::filesystem::path path(originalPath);
    std::string timestamp = std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    std::string backupName = path.stem().string() + "_backup_" + timestamp + path.extension().string();
    return backupName;
}

bool Config::createBackup(const std::string& originalPath) {
    try {
        std::filesystem::path original(originalPath);
        std::filesystem::path backupPath = generateBackupFilename(originalPath);
        
        // Check if original file exists
        if (!std::filesystem::exists(original)) {
            logWarning("createBackup", "Original file does not exist: " + originalPath);
            return false;
        }
        
        // Create backup
        std::filesystem::copy_file(original, backupPath, std::filesystem::copy_options::overwrite_existing);
        
        pImpl->lastBackedUpFile = backupPath.string();
        
        logInfo("createBackup", "Created backup: " + backupPath.string() + " from " + originalPath);
        return true;
        
    } catch (const std::filesystem::filesystem_error& e) {
        logError("createBackup", "Backup failed: " + std::string(e.what()) + " (original: " + originalPath + ")");
        return false;
    } catch (const std::exception& e) {
        logError("createBackup", "Backup failed with exception: " + std::string(e.what()) + " (original: " + originalPath + ")");
        return false;
    }
}

bool Config::atomicWrite(const std::string& filepath, const std::string& content) {
    std::filesystem::path targetPath(filepath);
    std::filesystem::path tempPath = targetPath;
    tempPath += ".tmp_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    
    try {
        // Create temporary file
        std::ofstream tempFile(tempPath.string(), std::ios::trunc);
        if (!tempFile.is_open()) {
            throw ConfigException(ConfigErrorCode::IOError, "Failed to create temporary file: " + tempPath.string());
        }
        
        tempFile << content;
        tempFile.close();
        
        // Verify content was written correctly
        std::ifstream verifyFile(tempPath.string());
        std::string verifyContent((std::istreambuf_iterator<char>(verifyFile)), std::istreambuf_iterator<char>());
        if (verifyContent != content) {
            throw ConfigException(ConfigErrorCode::IOError, "Content verification failed for temp file: " + tempPath.string());
        }
        
        // Move temp file to target (atomic on most systems)
        std::filesystem::rename(tempPath.string(), targetPath.string());
        
        logInfo("atomicWrite", "Atomic write successful: " + targetPath.string());
        return true;
        
    } catch (const ConfigException& e) {
        logError("atomicWrite", "Atomic write failed: " + e.message() + " (target: " + filepath + ")");
        
        // Cleanup temp file if it exists
        if (std::filesystem::exists(tempPath)) {
            try {
                std::filesystem::remove(tempPath);
            } catch (...) {
                logWarning("atomicWrite", "Failed to cleanup temp file: " + tempPath.string());
            }
        }
        
        return false;
    } catch (const std::filesystem::filesystem_error& e) {
        logError("atomicWrite", "Atomic write filesystem error: " + std::string(e.what()) + " (target: " + filepath + ")");
        
        // Cleanup temp file if it exists
        if (std::filesystem::exists(tempPath)) {
            try {
                std::filesystem::remove(tempPath);
            } catch (...) {
                logWarning("atomicWrite", "Failed to cleanup temp file: " + tempPath.string());
            }
        }
        
        return false;
    } catch (const std::exception& e) {
        logError("atomicWrite", "Atomic write exception: " + std::string(e.what()) + " (target: " + filepath + ")");
        
        // Cleanup temp file if it exists
        if (std::filesystem::exists(tempPath)) {
            try {
                std::filesystem::remove(tempPath);
            } catch (...) {
                logWarning("atomicWrite", "Failed to cleanup temp file: " + tempPath.string());
            }
        }
        
        return false;
    }
}

void Config::cleanupTempFiles() {
    try {
        std::vector<std::string> tempPatterns = {".tmp_", "_backup_"};
        for (const auto& pattern : tempPatterns) {
            for (const auto& entry : std::filesystem::directory_iterator(".")) {
                std::string filename = entry.path().filename().string();
                if (filename.find(pattern) != std::string::npos) {
                    try {
                        std::filesystem::remove(entry.path());
                    } catch (...) {
                        // Ignore cleanup failures
                    }
                }
            }
        }
    } catch (...) {
        // Ignore any cleanup errors
    }
}

void Config::rollbackTransaction() {
    if (!pImpl->lastLoadedFile.empty()) {
        logInfo("rollbackTransaction", "Rolling back to last loaded file: " + pImpl->lastLoadedFile);
        clear();
        loadFromFile(pImpl->lastLoadedFile);
    } else {
        logWarning("rollbackTransaction", "No backup file to rollback to");
    }
    pImpl->inTransaction = false;
}

bool Config::retryOperation(const std::function<bool()>& operation, const std::string& operationName, 
                           int maxRetries, int delayMs) {
    int attempts = 0;
    bool success = false;
    
    while (attempts < maxRetries && !success) {
        if (attempts > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(delayMs * (1 << attempts)));
            logInfo("retryOperation", operationName + " attempt " + std::to_string(attempts + 1) + "/" + std::to_string(maxRetries));
        }
        
        try {
            success = operation();
            if (success) {
                pImpl->retryCount = attempts + 1;
                logInfo("retryOperation", operationName + " succeeded after " + std::to_string(attempts + 1) + " attempts");
                return true;
            }
        } catch (const ConfigException& e) {
            if (attempts == maxRetries - 1) {
                logError("retryOperation", "Operation failed after " + std::to_string(maxRetries) + " attempts: " + e.message());
            } else if (!e.isRecoverable()) {
                logError("retryOperation", "Operation failed with non-recoverable error: " + e.message());
                return false;
            }
        } catch (const std::exception& e) {
            if (attempts == maxRetries - 1) {
                logError("retryOperation", "Operation failed after " + std::to_string(maxRetries) + " attempts: " + std::string(e.what()));
            }
        }
        
        attempts++;
    }
    
    logError("retryOperation", "Operation failed after " + std::to_string(maxRetries) + " attempts");
    return false;
}

bool Config::loadFromFile(const std::string& filepath) {
    ConfigFileResult checkResult = checkFileOperation(filepath, false);
    if (!checkResult.success) {
        logError("loadFromFile", checkResult.message, filepath);
        return false;
    }
    
    std::string content;
    std::ifstream file(filepath, std::ios::binary);
    
    if (!file.is_open()) {
        logError("loadFromFile", "Failed to open file: " + filepath);
        return false;
    }
    
    // Check file size before reading
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    // Read entire file with error handling
    try {
        content.resize(fileSize);
        if (fileSize > 0) {
            file.read(&content[0], fileSize);
        }
    } catch (const std::exception& e) {
        logError("loadFromFile", "Failed to read file content: " + std::string(e.what()), filepath);
        return false;
    }
    
    // Use retry logic for loading
    auto loadOperation = [&]() {
        bool success = loadFromString(content, filepath);
        if (success) {
            pImpl->lastLoadedFile = filepath;
        }
        return success;
    };
    
    return retryOperation(loadOperation, "loadFromFile");
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        // Create backup before writing
        createBackup(filepath);
        
        std::string content;
        content.reserve(1024); // Reserve some space to reduce reallocations
        content += "{\n";
        
        for (size_t i = 0; i < pImpl->entries.size(); ++i) {
            const auto& entry = pImpl->entries[i];
            content += "  \"" + entry.key + "\": ";
            
            // Convert config value to JSON-appropriate format
            std::visit([&content](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    content += "\"" + arg + "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    content += (arg ? "true" : "false");
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    content += "[";
                    for (size_t j = 0; j < arg.size(); ++j) {
                        content += std::to_string(arg[j]);
                        if (j < arg.size() - 1) content += ", ";
                    }
                    content += "]";
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    content += "[";
                    for (size_t j = 0; j < arg.size(); ++j) {
                        content += std::to_string(arg[j]);
                        if (j < arg.size() - 1) content += ", ";
                    }
                    content += "]";
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    content += "[";
                    for (size_t j = 0; j < arg.size(); ++j) {
                        content += "\"" + arg[j] + "\"";
                        if (j < arg.size() - 1) content += ", ";
                    }
                    content += "]";
                } else {
                    std::ostringstream oss;
                    oss << arg;
                    content += oss.str();
                }
            }, entry.value);
            
            if (i < pImpl->entries.size() - 1) {
                content += ",\n";
            } else {
                content += "\n";
            }
        }
        
        content += "}\n";
        
        // Use atomic write for safety
        bool success = atomicWrite(filepath, content);
        
        if (success) {
            logInfo("saveToFile", "Configuration saved successfully: " + filepath);
        } else {
            logError("saveToFile", "Failed to save configuration: " + filepath);
        }
        
        return success;
        
    } catch (const ConfigException& e) {
        logError("saveToFile", "Save operation failed: " + e.message(), filepath);
        return false;
    } catch (const std::exception& e) {
        logError("saveToFile", "Save operation failed with exception: " + std::string(e.what()), filepath);
        return false;
    }
}

// Helper functions implementation
std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string Config::removeQuotes(const std::string& str) {
    if (str.size() >= 2 && ((str[0] == '"' && str[str.size() - 1] == '"') ||
                           (str[0] == '\'' && str[str.size() - 1] == '\''))) {
        return str.substr(1, str.size() - 2);
    }
    return str;
}

bool Config::isQuoted(const std::string& str) {
    return (str.size() >= 2 && ((str[0] == '"' && str[str.size() - 1] == '"') ||
                                (str[0] == '\'' && str[str.size() - 1] == '\'')));
}

// Type conversion helpers
template<typename T>
std::optional<T> Config::anyToType(const std::any& value) {
    try {
        return std::any_cast<T>(value);
    } catch (const std::bad_any_cast&) {
        return std::nullopt;
    }
}

// Schema validation helpers
std::string Config::typeIndexToString(std::type_index type) {
    if (type == typeid(int)) return "int";
    if (type == typeid(int64_t)) return "int64_t";
    if (type == typeid(double)) return "double";
    if (type == typeid(bool)) return "bool";
    if (type == typeid(std::string)) return "std::string";
    if (type == typeid(std::vector<int>)) return "std::vector<int>";
    if (type == typeid(std::vector<double>)) return "std::vector<double>";
    if (type == typeid(std::vector<std::string>)) return "std::vector<std::string>";
    return "unknown";
}

// File format detection helpers
bool Config::hasJsonExtension(const std::string& filepath) {
    std::filesystem::path path(filepath);
    return path.extension() == ".json";
}

bool Config::hasYamlExtension(const std::string& filepath) {
    std::filesystem::path path(filepath);
    return path.extension() == ".yaml" || path.extension() == ".yml";
}

bool Config::hasSimpleExtension(const std::string& filepath) {
    std::filesystem::path path(filepath);
    return path.extension() == ".cfg" || path.extension() == ".ini" || path.extension() == ".conf";
}

// Json parsing helpers
bool Config::parseJsonValue(const nlohmann::json& jsonValue, ConfigValue& configValue) {
    if (jsonValue.is_boolean()) {
        configValue = jsonValue.get<bool>();
    } else if (jsonValue.is_number_integer()) {
        configValue = jsonValue.get<int>();
    } else if (jsonValue.is_number()) {
        configValue = jsonValue.get<double>();
    } else if (jsonValue.is_string()) {
        configValue = jsonValue.get<std::string>();
    } else if (jsonValue.is_array()) {
        // Check array element type
        if (!jsonValue.empty()) {
            if (jsonValue[0].is_number_integer()) {
                std::vector<int> intArray;
                intArray.reserve(jsonValue.size());
                for (const auto& item : jsonValue) {
                    intArray.push_back(item.get<int>());
                }
                configValue = intArray;
            } else if (jsonValue[0].is_number()) {
                std::vector<double> doubleArray;
                doubleArray.reserve(jsonValue.size());
                for (const auto& item : jsonValue) {
                    doubleArray.push_back(item.get<double>());
                }
                configValue = doubleArray;
            } else {
                std::vector<std::string> stringArray;
                stringArray.reserve(jsonValue.size());
                for (const auto& item : jsonValue) {
                    stringArray.push_back(item.get<std::string>());
                }
                configValue = stringArray;
            }
        }
    } else {
        return false;
    }
    return true;
}

// Yaml parsing helpers
bool Config::parseYamlValue(const YAML::Node& yamlValue, ConfigValue& configValue) {
    if (yamlValue.IsScalar()) {
        const std::string& value = yamlValue.Scalar();
        if (value == "true" || value == "false") {
            configValue = (value == "true");
        } else if (std::all_of(value.begin(), value.end(), ::isdigit)) {
            configValue = std::stoi(value);
        } else if (std::isdigit(value[0]) || value.find('.') != std::string::npos) {
            configValue = std::stod(value);
        } else {
            configValue = value;
        }
    } else if (yamlValue.IsSequence()) {
        // Check sequence element type
        if (!yamlValue.empty()) {
            if (yamlValue[0].IsScalar() && std::all_of(yamlValue[0].Scalar().begin(), yamlValue[0].Scalar().end(), ::isdigit)) {
                std::vector<int> intArray;
                intArray.reserve(yamlValue.size());
                for (const auto& item : yamlValue) {
                    if (item.IsScalar() && std::all_of(item.Scalar().begin(), item.Scalar().end(), ::isdigit)) {
                        intArray.push_back(std::stoi(item.Scalar()));
                    } else {
                        return false;
                    }
                }
                configValue = intArray;
            } else if (yamlValue[0].IsScalar() && (std::isdigit(yamlValue[0].Scalar()[0]) || yamlValue[0].Scalar().find('.') != std::string::npos)) {
                std::vector<double> doubleArray;
                doubleArray.reserve(yamlValue.size());
                for (const auto& item : yamlValue) {
                    if (item.IsScalar() && (std::isdigit(item.Scalar()[0]) || item.Scalar().find('.') != std::string::npos)) {
                        doubleArray.push_back(std::stod(item.Scalar()));
                    } else {
                        return false;
                    }
                }
                configValue = doubleArray;
            } else {
                std::vector<std::string> stringArray;
                stringArray.reserve(yamlValue.size());
                for (const auto& item : yamlValue) {
                    if (item.IsScalar()) {
                        stringArray.push_back(item.Scalar());
                    } else {
                        return false;
                    }
                }
                configValue = stringArray;
            }
        }
    } else {
        return false;
    }
    return true;
}

// Validation helper functions for schema entry manipulation
void Config::addAdvancedValidator(const std::string& key, std::unique_ptr<ValidatorBase> validator) {
    for (auto& entry : pImpl->schema) {
        if (entry.key == key) {
            entry.addAdvancedValidator(std::move(validator));
            return;
        }
    }
}

bool Config::validateSchema() const {
    // Basic schema validation
    bool valid = true;
    for (const auto& entry : pImpl->schema) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                               [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        if (it == pImpl->entries.end()) {
            if (entry.required) {
                logError("validateSchema", "Required key missing: " + entry.key);
                valid = false;
            }
        } else {
            // Validate type
            if (!validateType(it->value, entry.type)) {
                logError("validateSchema", "Type mismatch for key \"" + entry.key + "\": expected " + entry.getTypeString());
                valid = false;
            }
            // Note: Advanced validators are validated through the full validation manager
        }
    }
    return valid;
}

ValidationResult Config::validate(const std::unordered_map<std::string, std::any>& configMap) const {
    if (!validationManager_) {
        ValidationResult result;
        result.addError(ValidationError("Validation", ValidationErrorCode::SchemaValidationFailed,
                                       "Validation manager not initialized"));
        return result;
    }
    
    return validationManager_->validate(configMap);
}

ValidationResult Config::validateFull(const std::unordered_map<std::string, std::any>& configMap) const {
    ValidationResult result = validate(configMap);
    
    // Generate report if there are errors
    if (!result.isValid()) {
        validationManager_->generateValidationReport(result);
    }
    
    return result;
}

// Validation helper methods implementation
void Config::addValidationGroup(const ValidationGroup& group) {
    if (validationManager_) {
        validationManager_->addValidationGroup(group);
    }
    pImpl->validationGroups.push_back(group);
}

void Config::addValidationDependency(const ValidationDependency& dependency) {
    if (validationManager_) {
        validationManager_->addValidationDependency(dependency);
    }
    pImpl->validationDependencies.push_back(dependency);
}

void Config::enableCrossValidation(bool enable) {
    pImpl->enableCrossValidation = enable;
    if (validationManager_) {
        validationManager_->enableCrossValidation(enable);
    }
}

void Config::enableBusinessLogicValidation(bool enable) {
    pImpl->enableBusinessLogicValidation = enable;
    if (validationManager_) {
        validationManager_->enableBusinessLogicValidation(enable);
    }
}

void Config::setValidationLevel(ValidationLevel level) {
    pImpl->defaultValidationLevel = level;
    if (validationManager_) {
        validationManager_->setDefaultValidationLevel(level);
    }
}

// Config get/set methods implementation
template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                          [&key](const ConfigEntry& e) { return e.key == key; });n    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    return anyToType<T>(it->value);
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto value = get<T>(key);
    if (value) {
        return *value;
    }
    return defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                          [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
                      [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
                                      [&key](const ConfigEntry& e) { return e.key == key; }),
                        pImpl->entries.end());
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->schema.clear();
    pImpl->tempData.clear();
    pImpl->validationGroups.clear();
    pImpl->validationDependencies.clear();
}

std::string Config::summary() const {
    std::stringstream ss;
    ss << "Configuration Summary:" << std::endl;
    ss << "Total entries: " << pImpl->entries.size() << std::endl;
    ss << "Total schema entries: " << pImpl->schema.size() << std::endl;
    ss << "In transaction: " << (pImpl->inTransaction ? "yes" : "no") << std::endl;
    ss << "Retry count: " << pImpl->retryCount << std::endl;
    ss << "Validation groups: " << pImpl->validationGroups.size() << std::endl;
    ss << "Dependencies: " << pImpl->validationDependencies.size() << std::endl;
    
    if (!pImpl->lastLoadedFile.empty()) {
        ss << "Last loaded: " << pImpl->lastLoadedFile << std::endl;
    }
    
    ss << "\nEntries:" << std::endl;
    for (const auto& entry : pImpl->entries) {
        ss << "  \"" << entry.key << "\" (source: " << static_cast<int>(entry.source) << ")";
        if (!entry.description.empty()) {
            ss << " - " << entry.description;
        }
        ss << std::endl;
    }
    
    return ss.str();
}

FileFormat Config::detectFileFormat(const std::string& filepath) {
    if (hasJsonExtension(filepath)) {
        return FileFormat::JSON;
    } else if (hasYamlExtension(filepath)) {
        return FileFormat::YAML;
    } else if (hasSimpleExtension(filepath)) {
        return FileFormat::Simple;
    }
    return FileFormat::Unknown;
}

bool Config::isJSONFile(const std::string& content) {
    try {
        auto json = nlohmann::json::parse(content);
        return !json.is_discarded();
    } catch (...) {
        return false;
    }
}

bool Config::isYAMLOFile(const std::string& content) {
    try {
        auto yaml = YAML::Load(content);
        return !yaml.IsNull();
    } catch (...) {
        return false;
    }
}

bool Config::isSimpleFormat(const std::string& content) {
    // Simple format check (key=value pairs)
    return content.find('=') != std::string::npos;
}

std::optional<ConfigValue> Config::stringToConfigValue(const std::string& str) {
    // Try boolean
    if (str == "true" || str == "false") {
        return str == "true";
    }
    
    // Try integer
    if (std::all_of(str.begin(), str.end(), ::isdigit)) {
        try {
            return std::stoi(str);
        } catch (...) {
            return std::nullopt;
        }
    }
    
    // Try double
    if (std::isdigit(str[0]) || str.find('.') != std::string::npos) {
        try {
            return std::stod(str);
        } catch (...) {
            return std::nullopt;
        }
    }
    
    // Default to string
    return str;
}

std::string Config::configValueToString(const ConfigValue& value) {
    return std::visit([](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg ? "true" : "false";
        } else if constexpr (std::is_same_v<T, int>) {
            return std::to_string(arg);
        } else if constexpr (std::is_same_v<T, double>) {
            return std::to_string(arg);
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            std::stringstream ss;
            ss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                ss << arg[i];
                if (i < arg.size() - 1) ss << ", ";
            }
            ss << "]";
            return ss.str();
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            std::stringstream ss;
            ss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                ss << arg[i];
                if (i < arg.size() - 1) ss << ", ";
            }
            ss << "]";
            return ss.str();
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            std::stringstream ss;
            ss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                ss << "\"" << arg[i] << "\"";
                if (i < arg.size() - 1) ss << ", ";
            }
            ss << "]";
            return ss.str();
        }
        return "";
    }, value);
}

bool Config::validateType(const ConfigValue& value, std::type_index expectedType) {
    return std::type_index(value.type()) == expectedType;
}

void Config::addSchemaEntry(const std::string& key, std::type_index type, bool required,
                           const std::string& description, const TypeValidator& validator) {
    ConfigSchemaEntry entry(key, type, required, description, validator);
    pImpl->schema.push_back(entry);
}

bool Config::logError(const std::string& operation, const std::string& message, 
                     const std::string& filepath, int line) {
    std::string fullMessage = "[" + operation + "] " + message;
    if (!filepath.empty()) {
        fullMessage += " (file: " + filepath + ")";
    }
    if (line > 0) {
        fullMessage += " (line: " + std::to_string(line) + ")";
    }
    
    Logger::error(fullMessage);
    return true;
}

bool Config::logWarning(const std::string& operation, const std::string& message,
                       const std::string& filepath, int line) {
    std::string fullMessage = "[" + operation + "] " + message;
    if (!filepath.empty()) {
        fullMessage += " (file: " + filepath + ")";
    }
    if (line > 0) {
        fullMessage += " (line: " + std::to_string(line) + ")";
    }
    
    Logger::warning(fullMessage);
    return true;
}

bool Config::parseJSON(const std::string& content, std::unordered_map<std::string, std::any>& output) {
    try {
        auto json = nlohmann::json::parse(content);
        
        for (auto it = json.begin(); it != json.end(); ++it) {
            ConfigValue configValue;
            if (parseJsonValue(it.value(), configValue)) {
                output[it.key()] = configValue;
            } else {
                return false;
            }
        }
        
        return true;
    } catch (...) {
        return false;
    }
}

bool Config::parseYAML(const std::string& content, std::unordered_map<std::string, std::any>& output) {
    try {
        auto yaml = YAML::Load(content);
        
        for (const auto& pair : yaml) {
            ConfigValue configValue;
            if (parseYamlValue(pair.second, configValue)) {
                output[pair.first.Scalar()] = configValue;
            } else {
                return false;
            }
        }
        
        return true;
    } catch (...) {
        return false;
    }
}

bool Config::parseSimple(const std::string& content, std::unordered_map<std::string, std::any>& output) {
    std::istringstream stream(content);
    std::string line;
    
    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        // Remove quotes from value if present
        value = removeQuotes(value);
        
        auto configValue = stringToConfigValue(value);
        if (configValue) {
            output[key] = *configValue;
        }
    }
    
    return true;
}

// Template instantiations
// Note: The actual template definitions would need to be properly implemented
// For now, we'll provide placeholder implementations

// Helper function for validation builder
void Config::addAdvancedValidator(const std::string& key, std::unique_ptr<ValidatorBase> validator) {
    for (auto& entry : pImpl->schema) {
        if (entry.key == key) {
            entry.addAdvancedValidator(std::move(validator));
            break;
        }
    }
}

// ValidationResult JSON serialization
std::string ValidationResult::toJSON() const {
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"valid\": " << (valid ? "true" : "false") << ",\n";
    ss << "  \"error_count\": " << errors.size() << ",\n";
    ss << "  \"warning_count\": " << warnings.size() << ",\n";
    ss << "  \"success_rate\": " << successRate << ",\n";
    ss << "  \"validation_time_ms\": " << validationTime.count() << ",\n";
    ss << "  \"errors\": [\n";
    for (size_t i = 0; i < errors.size(); ++i) {
        ss << "    {\n";
        ss << "      \"key\": \"" << errors[i].key << "\",\n";
        ss << "      \"error_code\": \"" << errors[i].errorCodeName << "\",\n";
        ss << "      \"message\": \"" << errors[i].message << "\",\n";
        ss << "      \"category\": \"" << errors[i].categoryName() << "\"\n";
        if (i < errors.size() - 1) ss << "    },\n";
        else ss << "    }\n";
    }
    ss << "  ]\n";
    ss << "}\n";
    return ss.str();
}

// ValidationErrorCode to string mapping
std::string ValidationError::validationErrorCodeToString(ValidationErrorCode code) {
    switch (code) {
        case ValidationErrorCode::TypeMismatch: return "TypeMismatch";
        case ValidationErrorCode::ValueOutOfRange: return "ValueOutOfRange";
        case ValidationErrorCode::PatternMismatch: return "PatternMismatch";
        case ValidationErrorCode::InvalidEnumValue: return "InvalidEnumValue";
        case ValidationErrorCode::ConditionalValidationFailed: return "ConditionalValidationFailed";
        case ValidationErrorCode::DependencyValidationFailed: return "DependencyValidationFailed";
        case ValidationErrorCode::CrossValidationFailed: return "CrossValidationFailed";
        case ValidationErrorCode::FormatValidationFailed: return "FormatValidationFailed";
        case ValidationErrorCode::BusinessLogicValidationFailed: return "BusinessLogicValidationFailed";
        case ValidationErrorCode::RequiredKeyMissing: return "RequiredKeyMissing";
        case ValidationErrorCode::SchemaValidationFailed: return "SchemaValidationFailed";
        default: return "UnknownValidationError";
    }
}

// ValidationManager implementation details
ValidationManager::ValidationManager() : defaultValidationLevel_(ValidationLevel::Strict),
                                         enableCrossValidation_(true), enableBusinessLogicValidation_(true) {
    lastValidationTime_ = std::chrono::system_clock::now();
}

void ValidationManager::setDefaultValidationLevel(ValidationLevel level) {
    defaultValidationLevel_ = level;
}

void ValidationManager::enableCrossValidation(bool enable) {
    enableCrossValidation_ = enable;
}

void ValidationManager::enableBusinessLogicValidation(bool enable) {
    enableBusinessLogicValidation_ = enable;
}

void ValidationManager::addSchemaEntry(ConfigSchemaEntry entry) {
    schemaEntries_.push_back(std::move(entry));
}

void ValidationManager::addValidationGroup(ValidationGroup group) {
    validationGroups_.push_back(std::move(group));
}

void ValidationManager::addValidationDependency(const ValidationDependency& dependency) {
    validationDependencies_.push_back(dependency);
}

ValidationResult ValidationManager::validate(const std::unordered_map<std::string, std::any>& configMap) {
    ValidationResult result;
    auto startTime = std::chrono::system_clock::now();
    
    // Check cache first
    std::string cacheKey = calculateCacheKey(configMap);
    if (shouldUseCache(cacheKey)) {
        auto cachedResult = getCachedValidation(cacheKey);
        if (cachedResult.valid) {
            result = cachedResult;
            result.validationTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now() - startTime);
            return result;
        }
    }
    
    // Basic schema validation
    for (const auto& entry : schemaEntries_) {
        auto it = configMap.find(entry.key);
        if (it == configMap.end()) {
            if (entry.required) {
                ValidationError error(entry.key, ValidationErrorCode::RequiredKeyMissing,
                                    "Required key is missing");
                result.addError(error);
            }
            continue;
        }
        
        // Type validation
        if (!validateType(it->second, entry.type)) {
            ValidationError error(entry.key, ValidationErrorCode::TypeMismatch,
                                "Type mismatch", entry.getTypeString(),
                                ValidatorBase::typeIndexToString(std::type_index(it->second.type())));
            result.addError(error);
        }
        
        // Basic validator
        if (entry.validator && !entry.validator(it->second)) {
            ValidationError error(entry.key, ValidationErrorCode::SchemaValidationFailed,
                                "Basic validation failed");
            result.addError(error);
        }
        
        // Advanced validators
        for (const auto& validator : entry.advancedValidators) {
            if (!validator->validate(it->second)) {
                ValidationError error(entry.key, validator->getErrorCode(),
                                    validator->getErrorMessage(it->second),
                                    validator->getExpectedType(),
                                    ValidatorBase::typeIndexToString(std::type_index(it->second.type())));
                error.validatorType = getValidatorTypeName(*validator);
                result.addError(error);
            }
        }
    }
    
    // Cross-validation
    if (enableCrossValidation_) {
        performCrossValidation(configMap, result);
    }
    
    // Business logic validation
    if (enableBusinessLogicValidation_) {
        performBusinessLogicValidation(configMap, result);
    }
    
    // Dependency validation
    performDependencyValidation(configMap, result);
    
    // Calculate metrics
    result.validationTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now() - startTime);
    result.successRate = result.calculateSuccessRate();
    
    // Cache result if appropriate
    if (result.cacheable) {
        cacheValidation(cacheKey, result);
    }
    
    lastValidationKey_ = cacheKey;
    lastValidationTime_ = std::chrono::system_clock::now();
    
    return result;
}

std::string ValidationManager::calculateCacheKey(const std::unordered_map<std::string, std::any>& configMap) const {
    std::stringstream ss;
    for (const auto& pair : configMap) {
        ss << pair.first << "=" << std::any_cast<std::string>(pair.second) << ";";
    }
    return ss.str();
}

bool ValidationManager::shouldUseCache(const std::string& cacheKey) const {
    if (!validationCache_.count(cacheKey)) return false;
    
    auto cachedResult = validationCache_[cacheKey];
    auto now = std::chrono::system_clock::now();
    auto age = std::chrono::duration_cast<std::chrono::seconds>(now - cachedResult.validationTime);
    
    return age < std::chrono::seconds(300); // 5 minute cache
}

ValidationResult ValidationManager::getCachedValidation(const std::string& cacheKey) const {
    if (validationCache_.count(cacheKey)) {
        return validationCache_[cacheKey];
    }
    return ValidationResult{};
}

void ValidationManager::cacheValidation(const std::string& cacheKey, const ValidationResult& result) {
    validationCache_[cacheKey] = result;
}

void ValidationManager::clearCache() {
    validationCache_.clear();
}

void ValidationManager::performCrossValidation(const std::unordered_map<std::string, std::any>& configMap,
                                               ValidationResult& result) {
    // Example cross-validation: if optimization_enabled is true, then max_iterations must be positive
    auto optIt = configMap.find("optimization_enabled");
    if (optIt != configMap.end() && std::any_cast<bool>(optIt->second)) {
        auto iterIt = configMap.find("max_iterations");
        if (iterIt != configMap.end()) {
            auto maxIter = std::any_cast<int>(iterIt->second);
            if (maxIter <= 0) {
                ValidationError error("max_iterations", ValidationErrorCode::CrossValidationFailed,
                                    "max_iterations must be positive when optimization_enabled is true");
                result.addError(error);
            }
        }
    }
}

void ValidationManager::performBusinessLogicValidation(const std::unordered_map<std::string, std::any>& configMap,
                                                      ValidationResult& result) {
    // Example business logic validation: learning_rate * batch_size must be < 1000
    auto lrIt = configMap.find("learning_rate");
    auto bsIt = configMap.find("batch_size");
    if (lrIt != configMap.end() && bsIt != configMap.end()) {
        auto lr = std::any_cast<double>(lrIt->second);
        auto bs = std::any_cast<int>(bsIt->second);
        if (lr * bs >= 1000.0) {
            ValidationError error("learning_rate", ValidationErrorCode::BusinessLogicValidationFailed,
                                "learning_rate * batch_size must be < 1000");
            result.addError(error);
        }
    }
}

void ValidationManager::performDependencyValidation(const std::unordered_map<std::string, std::any>& configMap,
                                                    ValidationResult& result) {
    for (const auto& dependency : validationDependencies_) {
        if (!dependency.isValid(configMap)) {
            ValidationError error(dependency.dependentKey,
                                ValidationErrorCode::DependencyValidationFailed,
                                "Dependency validation failed: " + dependency.dependentKey + 
                                " should have value " + std::any_cast<std::string>(dependency.dependencyValue) +
                                " when " + dependency.relationship + " other conditions");
            result.addError(error);
        }
    }
}

std::string ValidationManager::getValidatorTypeName(const ValidatorBase& validator) const {
    const std::type_info& type = typeid(validator);
    if (type == typeid(RangeValidator)) return "RangeValidator";
    if (type == typeid(PatternValidator)) return "PatternValidator";
    if (type == typeid(EnumValidator)) return "EnumValidator";
    if (type == typeid(ConditionalValidator)) return "ConditionalValidator";
    if (type == typeid(CompositeValidator)) return "CompositeValidator";
    if (type == typeid(FormatValidator)) return "FormatValidator";
    return "UnknownValidator";
}

bool ValidationManager::validateType(const std::any& value, std::type_index expectedType) const {
    return std::type_index(value.type()) == expectedType;
}

const std::vector<ConfigSchemaEntry>& ValidationManager::getSchemaEntries() const {
    return schemaEntries_;
}

const std::vector<ValidationGroup>& ValidationManager::getValidationGroups() const {
    return validationGroups_;
}

const std::vector<ValidationDependency>& ValidationManager::getValidationDependencies() const {
    return validationDependencies_;
}

void ValidationManager::generateValidationReport(const ValidationResult& result) const {
    std::cout << result.formattedResult() << std::endl;
    std::cout << "\nDetailed Report:" << std::endl;
    std::cout << "Status: " << (result.valid ? "VALID" : "INVALID") << std::endl;
    std::cout << "Errors: " << result.errors.size() << std::endl;
    std::cout << "Warnings: " << result.warnings.size() << std::endl;
    std::cout << "Success Rate: " << result.successRate << "%" << std::endl;
    std::cout << "Validation Time: " << result.validationTime.count() << " ms" << std::endl;
    
    if (!result.errors.empty()) {
        std::cout << "\nError Categories:" << std::endl;
        std::unordered_map<ValidationErrorCategory, int> categoryCounts;
        for (const auto& error : result.errors) {
            categoryCounts[error.category]++;
        }
        
        for (const auto& pair : categoryCounts) {
            std::cout << "  " << ValidationError::validationErrorCodeToString(
                static_cast<ValidationErrorCode>(static_cast<int>(pair.first) * 2)) 
                      << ": " << pair.second << std::endl;
        }
    }
}

// RangeValidator implementation
bool RangeValidator::validate(const std::any& value) const {
    if (std::is_integral_v<T>) {
        auto val = std::any_cast<T>(value);
        auto min = std::any_cast<T>(minValue_);
        auto max = std::any_cast<T>(maxValue_);
        return inclusive_ ? (val >= min && val <= max) : (val > min && val < max);
    } else {
        auto val = std::any_cast<T>(value);
        auto min = std::any_cast<T>(minValue_);
        auto max = std::any_cast<T>(maxValue_);
        return inclusive_ ? (val >= min && val <= max) : (val > min && val < max);
    }
}

std::string RangeValidator::getErrorMessage(const std::any& value) const {
    std::stringstream ss;
    ss << "Value out of range: ";
    
    if (std::is_integral_v<T>) {
        auto val = std::any_cast<T>(value);
        auto min = std::any_cast<T>(minValue_);
        auto max = std::any_cast<T>(maxValue_);
        ss << val << " is not in range [" << min << ", " << max << "]";
    } else {
        auto val = std::any_cast<T>(value);
        auto min = std::any_cast<T>(minValue_);
        auto max = std::any_cast<T>(maxValue_);
        ss << val << " is not in range [" << min << ", " << max << "]";
    }
    
    return ss.str();
}

std::string RangeValidator::getExpectedType() const {
    return "numeric range [" + std::to_string(std::any_cast<T>(minValue_)) + 
           ", " + std::to_string(std::any_cast<T>(maxValue_)) + "]";
}

ConfigErrorCode RangeValidator::getErrorCode() const {
    return ConfigErrorCode::ValueOutOfRange;
}

std::string RangeValidator::getDescription() const {
    return description_;
}

std::unique_ptr<ValidatorBase> RangeValidator::clone() const {
    return std::make_unique<RangeValidator>(*this);
}

// PatternValidator implementation
bool PatternValidator::validate(const std::any& value) const {
    auto str = std::any_cast<std::string>(value);
    if (!caseSensitive_) {
        std::string lowerStr = str;
        std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
        std::string lowerPattern = std::string(pattern_.str());
        std::transform(lowerPattern.begin(), lowerPattern.end(), lowerPattern.begin(), ::tolower);
        std::regex ignoreCasePattern(lowerPattern);
        return std::regex_match(lowerStr, ignoreCasePattern);
    }
    return std::regex_match(str, pattern_);
}

std::string PatternValidator::getErrorMessage(const std::any& value) const {
    std::stringstream ss;
    auto str = std::any_cast<std::string>(value);
    ss << "String '" << str << "' does not match pattern: " << pattern_.str();
    return ss.str();
}

std::string PatternValidator::getExpectedType() const {
    return "string matching pattern: " + pattern_.str();
}

ConfigErrorCode PatternValidator::getErrorCode() const {
    return ConfigErrorCode::PatternMismatch;
}

std::string PatternValidator::getDescription() const {
    return description_;
}

std::unique_ptr<ValidatorBase> PatternValidator::clone() const {
    return std::make_unique<PatternValidator>(*this);
}

// EnumValidator implementation
bool EnumValidator::validate(const std::any& value) const {
    auto str = std::any_cast<std::string>(value);
    if (!caseSensitive_) {
        std::string lowerStr = str;
        std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
        for (const auto& allowed : allowedValues_) {
            std::string lowerAllowed = allowed;
            std::transform(lowerAllowed.begin(), lowerAllowed.end(), lowerAllowed.begin(), ::tolower);
            if (lowerStr == lowerAllowed) return true;
        }
        return false;
    }
    return allowedValues_.find(str) != allowedValues_.end();
}

std::string EnumValidator::getErrorMessage(const std::any& value) const {
    std::stringstream ss;
    auto str = std::any_cast<std::string>(value);
    ss << "Value '" << str << "' is not in allowed enum values:";
    for (const auto& val : allowedValues_) {
        ss << " '" << val << "'";
    }
    return ss.str();
}

std::string EnumValidator::getExpectedType() const {
    std::stringstream ss;
    ss << "enum values: [";
    bool first = true;
    for (const auto& val : allowedValues_) {
        if (!first) ss << ", ";
        ss << "\"" << val << "\"";
        first = false;
    }
    ss << "]";
    return ss.str();
}

ConfigErrorCode EnumValidator::getErrorCode() const {
    return ConfigErrorCode::InvalidEnumValue;
}

std::string EnumValidator::getDescription() const {
    return description_;
}

std::unique_ptr<ValidatorBase> EnumValidator::clone() const {
    return std::make_unique<EnumValidator>(*this);
}

// ConditionalValidator implementation
bool ConditionalValidator::validate(const std::any& value) const {
    auto configMap = std::any_cast<std::unordered_map<std::string, std::any>>(value);
    
    auto it = configMap.find(dependencyKey_);
    if (it == configMap.end()) return true; // Dependency not present, no validation needed
    
    if (it->second != dependencyValue_) return true; // Dependency value doesn't match, no validation
    
    return thenValidator_->validate(configMap);
}

std::string ConditionalValidator::getErrorMessage(const std::any& value) const {
    std::stringstream ss;
    auto configMap = std::any_cast<std::unordered_map<std::string, std::any>>(value);
    
    auto it = configMap.find(dependencyKey_);
    if (it != configMap.end() && it->second == dependencyValue_) {
        ss << "Conditional validation failed: when \"" << dependencyKey_ << "\" = \"" 
           << std::any_cast<std::string>(dependencyValue_) << "\", thenValidator failed: "
           << thenValidator_->getErrorMessage(value);
    } else {
        ss << "Conditional dependency not met: \"" << dependencyKey_ << "\" != \"" 
           << std::any_cast<std::string>(dependencyValue_) << "\"";
    }
    return ss.str();
}

std::string ConditionalValidator::getExpectedType() const {
    return "conditional validation based on dependency \"" + dependencyKey_ + "\" = \"" 
           + std::any_cast<std::string>(dependencyValue_) + "\"";
}

ConfigErrorCode ConditionalValidator::getErrorCode() const {
    return ConfigErrorCode::ConditionalValidationFailed;
}

std::string ConditionalValidator::getDescription() const {
    return description_;
}

std::unique_ptr<ValidatorBase> ConditionalValidator::clone() const {
    return std::make_unique<ConditionalValidator>(
        dependencyKey_, dependencyValue_, thenValidator_->clone(), description_);
}

// CompositeValidator implementation
bool CompositeValidator::validate(const std::any& value) const {
    bool allValid = true;
    for (const auto& validator : validators_) {
        if (!validator->validate(value)) {
            allValid = false;
            if (requireAll_) break; // Fail fast if requiring all
        }
    }
    return allValid;
}

std::string CompositeValidator::getErrorMessage(const std::any& value) const {
    std::stringstream ss;
    ss << "Composite validation failed:";
    for (const auto& validator : validators_) {
        if (!validator->validate(value)) {
            ss << " " << validator->getErrorMessage(value);
        }
    }
    return ss.str();
}

std::string CompositeValidator::getExpectedType() const {
    return "composite validation of " + std::to_string(validators_.size()) + " validator(s)";
}

ConfigErrorCode CompositeValidator::getErrorCode() const {
    return ConfigErrorCode::SchemaValidationFailed;
}

std::string CompositeValidator::getDescription() const {
    return description_;
}

std::unique_ptr<ValidatorBase> CompositeValidator::clone() const {
    std::vector<std::unique_ptr<ValidatorBase>> clonedValidators;
    clonedValidators.reserve(validators_.size());
    for (const auto& validator : validators_) {
        clonedValidators.push_back(validator->clone());
    }
    return std::make_unique<CompositeValidator>(clonedValidators, description_, requireAll_);
}

// FormatValidator implementation
bool FormatValidator::validate(const std::any& value) const {
    auto str = std::any_cast<std::string>(value);
    
    switch (formatType_) {
        case FormatType::Email:
            return std::regex_match(str, std::regex(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)"));
        case FormatType::IPv4:
            return std::regex_match(str, std::regex(R"(^(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$)"));
        case FormatType::IPv6:
            return std::regex_match(str, std::regex(R"(^([0-9a-fA-F]{1,4}:){7}[0-9a-fA-F]{1,4})$"));
        case FormatType::MAC:
            return std::regex_match(str, std::regex(R"(^([0-9A-Fa-f]{2}[:-]){5}[0-9A-Fa-f]{2})$"));
        case FormatType::URL:
            return std::regex_match(str, std::regex(R"(^(https?|ftp)://[^\s/$.?#].[^\s]*$)"));
        case FormatType::DateTime:
            return std::regex_match(str, std::regex(R"(^\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}(?:\.\d+)?(?:Z|[+-]\d{2}:\d{2})?)$"));
        case FormatType::Custom:
            return std::regex_match(str, std::regex(customPattern_));
        default:
            return false;
    }
}

std::string FormatValidator::getErrorMessage(const std::any& value) const {
    auto str = std::any_cast<std::string>(value);
    std::stringstream ss;
    
    switch (formatType_) {
        case FormatType::Email: ss << "Invalid email format: '" << str << "'";
            break;
        case FormatType::IPv4: ss << "Invalid IPv4 address: '" << str << "'";
            break;
        case FormatType::IPv6: ss << "Invalid IPv6 address: '" << str << "'";
            break;
        case FormatType::MAC: ss << "Invalid MAC address: '" << str << "'";
            break;
        case FormatType::URL: ss << "Invalid URL: '" << str << "'";
            break;
        case FormatType::DateTime: ss << "Invalid date/time format: '" << str << "' (expected ISO 8601)";
            break;
        case FormatType::Custom: ss << "Value does not match custom pattern: '" << str << "'";
            break;
        default: ss << "Value does not match expected format";
    }
    
    return ss.str();
}

std::string FormatValidator::getExpectedType() const {
    switch (formatType_) {
        case FormatType::Email: return "email address";
        case FormatType::IPv4: return "IPv4 address";
        case FormatType::IPv6: return "IPv6 address";
        case FormatType::MAC: return "MAC address";
        case FormatType::URL: return "URL";
        case FormatType::DateTime: return "date/time (ISO 8601)";
        case FormatType::Custom: return "custom pattern: " + customPattern_;
        default: return "unknown format";
    }
}

ConfigErrorCode FormatValidator::getErrorCode() const {
    return ConfigErrorCode::FormatValidationFailed;
}

std::string FormatValidator::getDescription() const {
    return description_;
}

std::unique_ptr<ValidatorBase> FormatValidator::clone() const {
    if (formatType_ == FormatType::Custom) {
        return std::make_unique<FormatValidator>(customPattern_, description_);
    } else {
        return std::make_unique<FormatValidator>(formatType_, description_);
    }
}

// ValidationGroup implementation
std::string ValidationGroup::getSummary() const {
    std::stringstream ss;
    ss << "Group: " << name;
    if (!description.empty()) ss << " (" << description << ")";
    ss << " - " << keys.size() << " key(s), " << validators.size() << " validator(s)";
    if (!enabled) ss << " [DISABLED]";
    return ss.str();
}

// ValidationDependency implementation
bool ValidationDependency::isValid(const std::unordered_map<std::string, std::any>& configMap) const {
    auto it = configMap.find(dependentKey);
    if (it == configMap.end()) return false; // Key not present
    
    if (relationship == "equals") {
        return it->second == dependencyValue;
    } else if (relationship == "contains") {
        auto str1 = std::any_cast<std::string>(dependencyValue);
        auto str2 = std::any_cast<std::string>(it->second);
        return str2.find(str1) != std::string::npos;
    } else if (relationship == "greater_than") {
        auto num1 = std::any_cast<double>(dependencyValue);
        auto num2 = std::any_cast<double>(it->second);
        return num2 > num1;
    } else if (relationship == "less_than") {
        auto num1 = std::any_cast<double>(dependencyValue);
        auto num2 = std::any_cast<double>(it->second);
        return num2 < num1;
    }
    
    return false;
}

// Config implementation details
Config::Impl& Config::getImpl() {
    return *pImpl;
}

const Config::Impl& Config::getImpl() const {
    return *pImpl;
}

// Helper function implementations for SchemaBuilder
void Config::addAdvancedValidator(const std::string& key, std::unique_ptr<ValidatorBase> validator) {
    for (auto& entry : pImpl->schema) {
        if (entry.key == key) {
            entry.addAdvancedValidator(std::move(validator));
            return;
        }
    }
}

// TypeConverter specialization helper

std::string ValidationResult::toJSON() const {
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"valid\": " << (valid ? "true" : "false") << ",\n";
    ss << "  \"error_count\": " << errors.size() << ",\n";
    ss << "  \"warning_count\": " << warnings.size() << ",\n";
    ss << "  \"success_rate\": " << successRate << ",\n";
    ss << "  \"validation_time_ms\": " << validationTime.count() << ",\n";
    ss << "  \"errors\": [\n";
    for (size_t i = 0; i < errors.size(); ++i) {
        ss << "    {\n";
        ss << "      \"key\": \"" << errors[i].key << "\",\n";
        ss << "      \"error_code\": \"" << errors[i].errorCodeName << "\",\n";
        ss << "      \"message\": \"" << errors[i].message << "\",\n";
        ss << "      \"category\": \"" << errors[i].categoryName() << "\"\n";
        if (i < errors.size() - 1) ss << "    },\n";
        else ss << "    }\n";
    }
    ss << "  ]\n";
    ss << "}\n";
    return ss.str();
}

std::string ValidationError::validationErrorCodeToString(ValidationErrorCode code) {
    switch (code) {
        case ValidationErrorCode::TypeMismatch: return "TypeMismatch";
        case ValidationErrorCode::ValueOutOfRange: return "ValueOutOfRange";
        case ValidationErrorCode::PatternMismatch: return "PatternMismatch";
        case ValidationErrorCode::InvalidEnumValue: return "InvalidEnumValue";
        case ValidationErrorCode::ConditionalValidationFailed: return "ConditionalValidationFailed";
        case ValidationErrorCode::DependencyValidationFailed: return "DependencyValidationFailed";
        case ValidationErrorCode::CrossValidationFailed: return "CrossValidationFailed";
        case ValidationErrorCode::FormatValidationFailed: return "FormatValidationFailed";
        case ValidationErrorCode::BusinessLogicValidationFailed: return "BusinessLogicValidationFailed";
        case ValidationErrorCode::RequiredKeyMissing: return "RequiredKeyMissing";
        case ValidationErrorCode::SchemaValidationFailed: return "SchemaValidationFailed";
        default: return "UnknownValidationError";
    }
}

// Missing declarations for range validators

bool RangeValidator::validate(const std::any& value) const {
    if (std::is_integral_v<T>) {
        auto val = std::any_cast<T>(value);
        auto min = std::any_cast<T>(minValue_);
        auto max = std::any_cast<T>(maxValue_);
        return inclusive_ ? (val >= min && val <= max) : (val > min && val < max);
    } else {
        auto val = std::any_cast<T>(value);
        auto min = std::any_cast<T>(minValue_);
        auto max = std::any_cast<T>(maxValue_);
        return inclusive_ ? (val >= min && val <= max) : (val > min && val < max);
    }
}

std::string RangeValidator::getErrorMessage(const std::any& value) const {
    std::stringstream ss;
    ss << "Value out of range: ";
    
    if (std::is_integral_v<T>) {
        auto val = std::any_cast<T>(value);
        auto min = std::any_cast<T>(minValue_);
        auto max = std::any_cast<T>(maxValue_);
        ss << val << " is not in range [" << min << ", " << max << "]";
    } else {
        auto val = std::any_cast<T>(value);
        auto min = std::any_cast<T>(minValue_);
        auto max = std::any_cast<T>(maxValue_);
        ss << val << " is not in range [" << min << ", " << max << "]";
    }
    
    return ss.str();
}

std::string RangeValidator::getExpectedType() const {
    return "numeric range [" + std::to_string(std::any_cast<T>(minValue_)) + 
           ", " + std::to_string(std::any_cast<T>(maxValue_)) + "]";
}

ConfigErrorCode RangeValidator::getErrorCode() const {
    return ConfigErrorCode::ValueOutOfRange;
}

std::string RangeValidator::getDescription() const {
    return description_;
}

std::unique_ptr<ValidatorBase> RangeValidator::clone() const {
    return std::make_unique<RangeValidator>(*this);
}

bool PatternValidator::validate(const std::any& value) const {
    auto str = std::any_cast<std::string>(value);
    if (!caseSensitive_) {
        std::string lowerStr = str;
        std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
        std::string lowerPattern = std::string(pattern_.str());
        std::transform(lowerPattern.begin(), lowerPattern.end(), lowerPattern.begin(), ::tolower);
        std::regex ignoreCasePattern(lowerPattern);
        return std::regex_match(lowerStr, ignoreCasePattern);
    }
    return std::regex_match(str, pattern_);
}

std::string PatternValidator::getErrorMessage(const std::any& value) const {
    std::stringstream ss;
    auto str = std::any_cast<std::string>(value);
    ss << "String '" << str << "' does not match pattern: " << pattern_.str();
    return ss.str();
}

std::string PatternValidator::getExpectedType() const {
    return "string matching pattern: " + pattern_.str();
}

ConfigErrorCode PatternValidator::getErrorCode() const {
    return ConfigErrorCode::PatternMismatch;
}

std::string PatternValidator::getDescription() const {
    return description_;
}

std::unique_ptr<ValidatorBase> PatternValidator::clone() const {
    return std::make_unique<PatternValidator>(*this);
}

bool EnumValidator::validate(const std::any& value) const {
    auto str = std::any_cast<std::string>(value);
    if (!caseSensitive_) {
        std::string lowerStr = str;
        std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
        for (const auto& allowed : allowedValues_) {
            std::string lowerAllowed = allowed;
            std::transform(lowerAllowed.begin(), lowerAllowed.end(), lowerAllowed.begin(), ::tolower);
            if (lowerStr == lowerAllowed) return true;
        }
        return false;
    }
    return allowedValues_.find(str) != allowedValues_.end();
}

std::string EnumValidator::getErrorMessage(const std::any& value) const {
    std::stringstream ss;
    auto str = std::any_cast<std::string>(value);
    ss << "Value '" << str << "' is not in allowed enum values:";
    for (const auto& val : allowedValues_) {
        ss << " '" << val << "'";
    }
    return ss.str();
}

std::string EnumValidator::getExpectedType() const {
    std::stringstream ss;
    ss << "enum values: [";
    bool first = true;
    for (const auto& val : allowedValues_) {
        if (!first) ss << ", ";
        ss << "\"" << val << "\"";
        first = false;
    }
    ss << "]";
    return ss.str();
}

ConfigErrorCode EnumValidator::getErrorCode() const {
    return ConfigErrorCode::InvalidEnumValue;
}

std::string EnumValidator::getDescription() const {
    return description_;
}

std::unique_ptr<ValidatorBase> EnumValidator::clone() const {
    return std::make_unique<EnumValidator>(*this);
}

bool ConditionalValidator::validate(const std::any& value) const {
    auto configMap = std::any_cast<std::unordered_map<std::string, std::any>>(value);
    
    auto it = configMap.find(dependencyKey_);
    if (it == configMap.end()) return true; // Dependency not present, no validation needed
    
    if (it->second != dependencyValue_) return true; // Dependency value doesn't match, no validation
    
    return thenValidator_->validate(configMap);
}

std::string ConditionalValidator::getErrorMessage(const std::any& value) const {
    std::stringstream ss;
    auto configMap = std::any_cast<std::unordered_map<std::string, std::any>>(value);
    
    auto it = configMap.find(dependencyKey_);
    if (it != configMap.end() && it->second == dependencyValue_) {
        ss << "Conditional validation failed: when \"" << dependencyKey_ << "\" = \"" 
           << std::any_cast<std::string>(dependencyValue_) << "\", thenValidator failed: "
           << thenValidator_->getErrorMessage(value);
    } else {
        ss << "Conditional dependency not met: \"" << dependencyKey_ << "\" != \"" 
           << std::any_cast<std::string>(dependencyValue_) << "\"";
    }
    return ss.str();
}

std::string ConditionalValidator::getExpectedType() const {
    return "conditional validation based on dependency \"" + dependencyKey_ + "\" = \"" 
           + std::any_cast<std::string>(dependencyValue_) + "\"";
}

ConfigErrorCode ConditionalValidator::getErrorCode() const {
    return ConfigErrorCode::ConditionalValidationFailed;
}

std::string ConditionalValidator::getDescription() const {
    return description_;
}

std::unique_ptr<ValidatorBase> ConditionalValidator::clone() const {
    return std::make_unique<ConditionalValidator>(
        dependencyKey_, dependencyValue_, thenValidator_->clone(), description_);
}

bool CompositeValidator::validate(const std::any& value) const {
    bool allValid = true;
    for (const auto& validator : validators_) {
        if (!validator->validate(value)) {
            allValid = false;
            if (requireAll_) break; // Fail fast if requiring all
        }
    }
    return allValid;
}

std::string CompositeValidator::getErrorMessage(const std::any& value) const {
    std::stringstream ss;
    ss << "Composite validation failed:";
    for (const auto& validator : validators_) {
        if (!validator->validate(value)) {
            ss << " " << validator->getErrorMessage(value);
        }
    }
    return ss.str();
}

std::string CompositeValidator::getExpectedType() const {
    return "composite validation of " + std::to_string(validators_.size()) + " validator(s)";
}

ConfigErrorCode CompositeValidator::getErrorCode() const {
    return ConfigErrorCode::SchemaValidationFailed;
}

std::string CompositeValidator::getDescription() const {
    return description_;
}

std::unique_ptr<ValidatorBase> CompositeValidator::clone() const {
    std::vector<std::unique_ptr<ValidatorBase>> clonedValidators;
    clonedValidators.reserve(validators_.size());
    for (const auto& validator : validators_) {
        clonedValidators.push_back(validator->clone());
    }
    return std::make_unique<CompositeValidator>(clonedValidators, description_, requireAll_);
}

bool FormatValidator::validate(const std::any& value) const {
    auto str = std::any_cast<std::string>(value);
    
    switch (formatType_) {
        case FormatType::Email:
            return std::regex_match(str, std::regex(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)"));
        case FormatType::IPv4:
            return std::regex_match(str, std::regex(R"(^(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$)"));
        case FormatType::IPv6:
            return std::regex_match(str, std::regex(R"(^([0-9a-fA-F]{1,4}:){7}[0-9a-fA-F]{1,4})$"));
        case FormatType::MAC:
            return std::regex_match(str, std::regex(R"(^([0-9A-Fa-f]{2}[:-]){5}[0-9A-Fa-f]{2})$"));
        case FormatType::URL:
            return std::regex_match(str, std::regex(R"(^(https?|ftp)://[^\s/$.?#].[^\s]*$)"));
        case FormatType::DateTime:
            return std::regex_match(str, std::regex(R"(^\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}(?:\.\d+)?(?:Z|[+-]\d{2}:\d{2})?)$"));
        case FormatType::Custom:
            return std::regex_match(str, std::regex(customPattern_));
        default:
            return false;
    }
}

std::string FormatValidator::getErrorMessage(const std::any& value) const {
    auto str = std::any_cast<std::string>(value);
    std::stringstream ss;
    
    switch (formatType_) {
        case FormatType::Email: ss << "Invalid email format: '" << str << "'";
            break;
        case FormatType::IPv4: ss << "Invalid IPv4 address: '" << str << "'";
            break;
        case FormatType::IPv6: ss << "Invalid IPv6 address: '" << str << "'";
            break;
        case FormatType::MAC: ss << "Invalid MAC address: '" << str << "'";
            break;
        case FormatType::URL: ss << "Invalid URL: '" << str << "'";
            break;
        case FormatType::DateTime: ss << "Invalid date/time format: '" << str << "' (expected ISO 8601)";
            break;
        case FormatType::Custom: ss << "Value does not match custom pattern: '" << str << "'";
            break;
        default: ss << "Value does not match expected format";
    }
    
    return ss.str();
}

std::string FormatValidator::getExpectedType() const {
    switch (formatType_) {
        case FormatType::Email: return "email address";
        case FormatType::IPv4: return "IPv4 address";
        case FormatType::IPv6: return "IPv6 address";
        case FormatType::MAC: return "MAC address";
        case FormatType::URL: return "URL";
        case FormatType::DateTime: return "date/time (ISO 8601)";
        case FormatType::Custom: return "custom pattern: " + customPattern_;
        default: return "unknown format";
    }
}

ConfigErrorCode FormatValidator::getErrorCode() const {
    return ConfigErrorCode::FormatValidationFailed;
}

std::string FormatValidator::getDescription() const {
    return description_;
}

std::unique_ptr<ValidatorBase> FormatValidator::clone() const {
    if (formatType_ == FormatType::Custom) {
        return std::make_unique<FormatValidator>(customPattern_, description_);
    } else {
        return std::make_unique<FormatValidator>(formatType_, description_);
    }
}

std::string ValidationGroup::getSummary() const {
    std::stringstream ss;
    ss << "Group: " << name;
    if (!description.empty()) ss << " (" << description << ")";
    ss << " - " << keys.size() << " key(s), " << validators.size() << " validator(s)";
    if (!enabled) ss << " [DISABLED]";
    return ss.str();
}

bool ValidationDependency::isValid(const std::unordered_map<std::string, std::any>& configMap) const {
    auto it = configMap.find(dependentKey);
    if (it == configMap.end()) return false; // Key not present
    
    if (relationship == "equals") {
        return it->second == dependencyValue;
    } else if (relationship == "contains") {
        auto str1 = std::any_cast<std::string>(dependencyValue);
        auto str2 = std::any_cast<std::string>(it->second);
        return str2.find(str1) != std::string::npos;
    } else if (relationship == "greater_than") {
        auto num1 = std::any_cast<double>(dependencyValue);
        auto num2 = std::any_cast<double>(it->second);
        return num2 > num1;
    } else if (relationship == "less_than") {
        auto num1 = std::any_cast<double>(dependencyValue);
        auto num2 = std::any_cast<double>(it->second);
        return num2 < num1;
    }
    
    return false;
}

Config::Impl& Config::getImpl() {
    return *pImpl;
}

const Config::Impl& Config::getImpl() const {
    return *pImpl;
}

std::string ValidationResult::toJSON() const {
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"valid\": " << (valid ? "true" : "false") << ",\n";
    ss << "  \"error_count\": " << errors.size() << ",\n";
    ss << "  \"warning_count\": " << warnings.size() << ",\n";
    ss << "  \"success_rate\": " << successRate << ",\n";
    ss << "  \"validation_time_ms\": " << validationTime.count() << ",\n";
    ss << "  \"errors\": [\n";
    for (size_t i = 0; i < errors.size(); ++i) {
        ss << "    {\n";
        ss << "      \"key\": \"" << errors[i].key << "\",\n";
        ss << "      \"error_code\": \"" << errors[i].errorCodeName << "\",\n";
        ss << "      \"message\": \"" << errors[i].message << "\",\n";
        ss << "      \"category\": \"" << errors[i].categoryName() << "\"\n";
        if (i < errors.size() - 1) ss << "    },\n";
        else ss << "    }\n";
    }
    ss << "  ]\n";
    ss << "}\n";
    return ss.str();
}

std::string ValidationError::validationErrorCodeToString(ValidationErrorCode code) {
    switch (code) {
        case ValidationErrorCode::TypeMismatch: return "TypeMismatch";
        case ValidationErrorCode::ValueOutOfRange: return "ValueOutOfRange";
        case ValidationErrorCode::PatternMismatch: return "PatternMismatch";
        case ValidationErrorCode::InvalidEnumValue: return "InvalidEnumValue";
        case ValidationErrorCode::ConditionalValidationFailed: return "ConditionalValidationFailed";
        case ValidationErrorCode::DependencyValidationFailed: return "DependencyValidationFailed";
        case ValidationErrorCode::CrossValidationFailed: return "CrossValidationFailed";
        case ValidationErrorCode::FormatValidationFailed: return "FormatValidationFailed";
        case ValidationErrorCode::BusinessLogicValidationFailed: return "BusinessLogicValidationFailed";
        case ValidationErrorCode::RequiredKeyMissing: return "RequiredKeyMissing";
        case ValidationErrorCode::SchemaValidationFailed: return "SchemaValidationFailed";
        default: return "UnknownValidationError";
    }
}

// Template instantiations for type conversion

template<typename T>
std::optional<T> Config::anyToType(const std::any& value) {
    try {
        return std::any_cast<T>(value);
    } catch (const std::bad_any_cast&) {
        return std::nullopt;
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                          [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    return anyToType<T>(it->value);
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto value = get<T>(key);
    if (value) {
        return *value;
    }
    return defaultValue;
}

// SchemaBuilder implementation
Config::SchemaBuilder::SchemaBuilder(Config& config) : config_(config) {}

Config::SchemaBuilder& Config::SchemaBuilder::addString(const std::string& key, bool required,
                                                         const std::string& description) {
    config_.addSchemaEntry(key, typeid(std::string), required, description);
    return *this;
}

Config::SchemaBuilder& Config::SchemaBuilder::addInt(const std::string& key, bool required,
                                                     const std::string& description) {
    config_.addSchemaEntry(key, typeid(int), required, description);
    return *this;
}

Config::SchemaBuilder& Config::SchemaBuilder::addDouble(const std::string& key, bool required,
                                                        const std::string& description) {
    config_.addSchemaEntry(key, typeid(double), required, description);
    return *this;
}

Config::SchemaBuilder& Config::SchemaBuilder::addBool(const std::string& key, bool required,
                                                      const std::string& description) {
    config_.addSchemaEntry(key, typeid(bool), required, description);
    return *this;
}

Config::SchemaBuilder& Config::SchemaBuilder::addRange(const std::string& key, int min, int max,
                                                       const std::string& description, bool inclusive) {
    auto validator = std::make_unique<RangeValidator>(min, max, description, inclusive);
    config_.getImpl().addAdvancedValidator(key, std::move(validator));
    return *this;
}

Config::SchemaBuilder& Config::SchemaBuilder::addPattern(const std::string& key, const std::string& pattern,
                                                          const std::string& description, bool caseSensitive) {
    auto validator = std::make_unique<PatternValidator>(pattern, description, caseSensitive);
    config_.getImpl().addAdvancedValidator(key, std::move(validator));
    return *this;
}

Config::SchemaBuilder& Config::SchemaBuilder::addEnum(const std::string& key, const std::vector<std::string>& values,
                                                      const std::string& description, bool caseSensitive) {
    auto validator = std::make_unique<EnumValidator>(values, description, caseSensitive);
    config_.getImpl().addAdvancedValidator(key, std::move(validator));
    return *this;
}

Config::SchemaBuilder& Config::SchemaBuilder::addFormat(const std::string& key, FormatValidator::FormatType format,
                                                        const std::string& description) {
    auto validator = std::make_unique<FormatValidator>(format, description);
    config_.getImpl().addAdvancedValidator(key, std::move(validator));
    return *this;
}

Config::SchemaBuilder& Config::SchemaBuilder::addConditional(const std::string& key, const std::any& value,
                                                             std::unique_ptr<ValidatorBase> thenValidator,
                                                             const std::string& description) {
    auto validator = std::make_unique<ConditionalValidator>(key, value, std::move(thenValidator), description);
    config_.getImpl().addAdvancedValidator(key, std::move(validator));
    return *this;
}

Config::SchemaBuilder& Config::SchemaBuilder::addComposite(const std::string& key,
                                                           const std::vector<std::unique_ptr<ValidatorBase>>& validators,
                                                           const std::string& description, bool requireAll) {
    auto validator = std::make_unique<CompositeValidator>(validators, description, requireAll);
    config_.getImpl().addAdvancedValidator(key, std::move(validator));
    return *this;
}

std::string Config::getValidatorTypeName(const ValidatorBase& validator) const {
    const std::type_info& type = typeid(validator);
    if (type == typeid(RangeValidator)) return "RangeValidator";
    if (type == typeid(PatternValidator)) return "PatternValidator";
    if (type == typeid(EnumValidator)) return "EnumValidator";
    if (type == typeid(ConditionalValidator)) return "ConditionalValidator";
    if (type == typeid(CompositeValidator)) return "CompositeValidator";
    if (type == typeid(FormatValidator)) return "FormatValidator";
    return "UnknownValidator";
}