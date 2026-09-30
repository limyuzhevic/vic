#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cmath>
#include <chrono>
#include <cryptopp/md5.h>
#include <cryptopp/hex.h>
#include <memory>
#include <system_error>

namespace nlm {

// Configuration error handling constants
namespace ConfigErrorMessages {
    const std::string FILE_NOT_OPEN = "Config::loadFromFile: Could not open file {} for reading";
    const std::string INVALID_FORMAT = "Config::loadFromFile: Invalid or unsupported format for file {}";
    const std::string PARSE_ERROR = "Config::loadFromFile: Parse error in {} at line {}";
    const std::string SAVE_ERROR = "Config::saveToFile: Could not save file {} for writing";
    const std::string NULL_KEY = "Config::{}: Configuration key is null or empty";
    const std::string NULL_VALUE = "Config::{}: Configuration value is null";
    const std::string VALUE_TYPE_MISMATCH = "Config::{}: Value type mismatch for key {}. Expected {}, got {}";
    const std::string VALUE_OUT_OF_RANGE = "Config::{}: Value {} for key {} is out of allowed range [{}, {}]";
    const std::string VALUE_LENGTH_EXCEEDED = "Config::{}: Value length {} for key {} exceeds maximum of {}";
    const std::string VALUE_PATTERN_MISMATCH = "Config::{}: Value for key {} does not match required pattern";
    const std::string ARRAY_SIZE_MISMATCH = "Config::{}: Array has {} items for key {}, minimum/maximum is {}";
    const std::string DUPLICATE_KEY = "Config::{}: Duplicate key detected {}";
    const std::string MISSING_REQUIRED_KEY = "Config::{}: Required key is missing: {}";
    const std::string INVALID_INHERITANCE = "Config::loadWithInheritance: Invalid inheritance configuration";
    const std::string CHECKSUM_MISMATCH = "Config::verifyChecksum: Checksum verification failed";
    const std::string BACKUP_ERROR = "Config::saveWithBackup: Backup creation failed for {}";
    const std::string DIRECTORY_NOT_EXIST = "Config::loadFromFile: Directory does not exist: {}";
    const std::string PERMISSION_DENIED = "Config::loadFromFile: Permission denied accessing: {}";
    const std::string DISK_FULL = "Config::saveToFile: Disk full error for file {}";
    const std::string JSON_PARSE_ERROR = "Config::loadFromJSON: JSON parse error at line {}";
    const std::string YAML_PARSE_ERROR = "Config::loadFromYAML: YAML parse error at line {}";
    const std::string INVALID_FILENAME = "Config::loadFromFile: Invalid filename: {}";
}

// Configuration validation constants
namespace ConfigValidationConstants {
    const size_t MIN_KEY_LENGTH = 1;
    const size_t MAX_KEY_LENGTH = 100;
    const size_t MAX_VALUE_LENGTH = 10000;
    const double MIN_FLOAT = -1e10;
    const double MAX_FLOAT = 1e10;
    const int64_t MIN_INT64 = -1e10;
    const int64_t MAX_INT64 = 1e10;
    const int MIN_INT = -1e10;
    const int MAX_INT = 1e10;
    const size_t MAX_ARRAY_SIZE = 10000;
    const double MAX_TIME_LIMIT = 24.0 * 60 * 60;  // 24 hours in seconds
    const float MAX_SIMULATION_TIMESTEP = 1.0;  // 1 second max
    const size_t MAX_CONCURRENT_FILES = 10;
}

// Configuration class implementation
struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ConfigEntry> entriesMap;  // For fast lookup
    std::vector<SchemaRule> schemaRules;
    std::vector<ConfigValidationError> validationErrors;
    std::string filename;
    FileFormat detectedFormat;
    ConfigChecksum checksum;
    mutable bool isModified;
    int fileOperationCount;
    
    // Statistics
    size_t totalGetCalls;
    size_t totalSetCalls;
    size_t totalValidationCalls;
    
    Impl() : detectedFormat(FileFormat::Unknown), isModified(true), 
             fileOperationCount(0), totalGetCalls(0), totalSetCalls(0), totalValidationCalls(0) {}
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
}

Config::~Config() = default;

// File existence and accessibility checks
bool Config::fileExists(const std::string& filepath) const {
    return std::filesystem::exists(filepath) && std::filesystem::is_regular_file(filepath);
}

bool Config::fileReadable(const std::string& filepath) const {
    std::ifstream file(filepath);
    return file.good();
}

bool Config::fileWritable(const std::string& filepath) const {
    std::ofstream file(filepath, std::ios::app);
    return file.good();
}

bool Config::ensureDirectoryExists(const std::string& filepath) const {
    size_t lastPos = filepath.find_last_of('/');
    if (lastPos != std::string::npos && lastPos > 0) {
        std::string dir = filepath.substr(0, lastPos);
        if (!std::filesystem::exists(dir)) {
            if (!std::filesystem::create_directories(dir)) {
                NLM_LOG_ERROR(ConfigErrorMessages::DIRECTORY_NOT_EXIST, dir);
                return false;
            }
        }
    }
    return true;
}

// Trim whitespace utility
std::string Config::trim(const std::string& str) const {
    size_t start = 0;
    size_t end = str.size();
    
    while (start < end && std::isspace(static_cast<unsigned char>(str[start]))) {
        start++;
    }
    
    while (end > start && std::isspace(static_cast<unsigned char>(str[end - 1]))) {
        end--;
    }
    
    return str.substr(start, end - start);
}

// Value conversion utilities
std::string Config::convertValueToString(const ConfigValue& value) const {
    try {
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
            std::ostringstream oss;
            oss << "[";
            const auto& vec = std::get<std::vector<int>>(value);
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << vec[i];
            }
            oss << "]";
            return oss.str();
        } else if (std::holds_alternative<std::vector<double>>(value)) {
            std::ostringstream oss;
            oss << "[";
            const auto& vec = std::get<std::vector<double>>(value);
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << vec[i];
            }
            oss << "]";
            return oss.str();
        } else if (std::holds_alternative<std::vector<std::string>>(value)) {
            std::ostringstream oss;
            oss << "[";
            const auto& vec = std::get<std::vector<std::string>>(value);
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << vec[i] << "\"";
            }
            oss << "]";
            return oss.str();
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Config::convertValueToString failed: ") + e.what(), "Config::convertValueToString");
    }
    
    return "";
}

// Value validation utilities
bool Config::validateValue(const std::string& key, const ConfigValue& value, 
                          const SchemaRule& rule) const {
    try {
        if (rule.type == ValidationRule::Required) {
            return !key.empty();
        }
        
        // Get actual value as string for pattern checking
        std::string strValue = convertValueToString(value);
        
        // Check range validation
        if (rule.type == ValidationRule::RangeInt || 
            rule.type == ValidationRule::RangeDouble || 
            rule.type == ValidationRule::RangeFloat) {
            
            if (strValue.empty()) return false;
            
            double val = std::stod(strValue);
            auto params = std::get<std::pair<double, double>>(rule.parameters);
            
            if (val < params.first || val > params.second) {
                NLM_LOG_ERROR(ConfigErrorMessages::VALUE_OUT_OF_RANGE, "validateValue", val, key, params.first, params.second);
                return false;
            }
        }
        
        // Check string length validation
        else if (rule.type == ValidationRule::StringLength) {
            size_t length = std::get<size_t>(rule.parameters);
            
            if (strValue.size() > length) {
                NLM_LOG_ERROR(ConfigErrorMessages::VALUE_LENGTH_EXCEEDED, "validateValue", strValue.size(), key, length);
                return false;
            }
        }
        
        // Check string pattern validation
        else if (rule.type == ValidationRule::StringPattern) {
            std::string pattern = std::get<std::string>(rule.parameters);
            
            if (pattern.empty()) return true;  // No pattern, always valid
            
            if (strValue.find(pattern) == std::string::npos) {
                NLM_LOG_ERROR(ConfigErrorMessages::VALUE_PATTERN_MISMATCH, "validateValue", key);
                return false;
            }
        }
        
        // Check array item count validation
        else if (rule.type == ValidationRule::MinItems || rule.type == ValidationRule::MaxItems) {
            size_t count = 0;
            
            if (std::holds_alternative<std::vector<int>>(value)) {
                count = std::get<std::vector<int>>(value).size();
            } else if (std::holds_alternative<std::vector<double>>(value)) {
                count = std::get<std::vector<double>>(value).size();
            } else if (std::holds_alternative<std::vector<std::string>>(value)) {
                count = std::get<std::vector<std::string>>(value).size();
            }
            
            if (rule.type == ValidationRule::MinItems) {
                size_t minItems = std::get<size_t>(rule.parameters);
                if (count < minItems) {
                    NLM_LOG_ERROR(ConfigErrorMessages::ARRAY_SIZE_MISMATCH, "validateValue", count, key, std::to_string(minItems) + "+");
                    return false;
                }
            } else if (rule.type == ValidationRule::MaxItems) {
                size_t maxItems = std::get<size_t>(rule.parameters);
                if (count > maxItems) {
                    NLM_LOG_ERROR(ConfigErrorMessages::ARRAY_SIZE_MISMATCH, "validateValue", count, key, "<=" + std::to_string(maxItems));
                    return false;
                }
            }
        }
        
        // Check enum validation
        else if (rule.type == ValidationRule::Enum) {
            std::string enumValues = std::get<std::string>(rule.parameters);
            
            if (strValue.empty()) return false;
            
            // Simple check for demonstration
            if (enumValues.find("," + strValue + ",") != std::string::npos ||
                enumValues == strValue) {
                return true;
            } else {
                NLM_LOG_ERROR(ConfigErrorMessages::VALUE_PATTERN_MISMATCH, "validateValue", key);
                return false;
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Config::validateValue exception: ") + e.what(), "Config::validateValue");
        return false;
    }
}

// Enhanced JSON parser with better error handling
bool Config::loadFromJSON(const std::string& content) {
    try {
        auto json = nlohmann::json::parse(content);
        
        for (auto& [key, value] : json.items()) {
            if (key.empty()) {
                NLM_LOG_WARNING(ConfigErrorMessages::NULL_KEY, "loadFromJSON");
                continue;
            }
            
            std::string strValue;
            
            if (value.is_string()) {
                strValue = value.get<std::string>();
            } else if (value.is_number_integer()) {
                strValue = std::to_string(value.get<int64_t>());
            } else if (value.is_number_float()) {
                strValue = std::to_string(value.get<double>());
            } else if (value.is_boolean()) {
                strValue = value.get<bool>() ? "true" : "false";
            } else if (value.is_array()) {
                // Convert array to comma-separated string
                std::vector<std::string> parts;
                for (const auto& item : value) {
                    if (item.is_string()) {
                        parts.push_back("'" + item.get<std::string>() + "'");
                    } else if (item.is_number_integer()) {
                        parts.push_back(std::to_string(item.get<int64_t>()));
                    } else if (item.is_number_float()) {
                        parts.push_back(std::to_string(item.get<double>()));
                    } else if (item.is_boolean()) {
                        parts.push_back(item.get<bool>() ? "true" : "false");
                    }
                }
                strValue = "[" + boost::algorithm::join(parts, ", ") + "]";
            }
            
            if (!strValue.empty()) {
                set(key, strValue, ConfigSource::File);
            }
        }
        
        return true;
    } catch (const nlohmann::json::parse_error& e) {
        NLM_LOG_ERROR(std::string("Config::loadFromJSON parse error: ") + e.what(), "Config::loadFromJSON");
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Config::loadFromJSON error: ") + e.what(), "Config::loadFromJSON");
        return false;
    }
}

// Enhanced YAML parser with better error handling
bool Config::loadFromYAML(const std::string& content) {
    try {
        YAML::Node yaml = YAML::Load(content);
        
        for (const auto& key : yaml) {
            if (!key.first.IsScalar()) {
                NLM_LOG_WARNING(ConfigErrorMessages::NULL_KEY, "loadFromYAML");
                continue;
            }
            
            std::string keyStr = key.first.as<std::string>();
            
            if (keyStr.empty()) {
                NLM_LOG_WARNING(ConfigErrorMessages::NULL_KEY, "loadFromYAML");
                continue;
            }
            
            std::string strValue;
            
            if (key.second.IsScalar()) {
                if (key.second.IsDouble()) {
                    strValue = std::to_string(key.second.as<double>());
                } else if (key.second.IsInt()) {
                    strValue = std::to_string(key.second.as<int64_t>());
                } else if (key.second.IsBool()) {
                    strValue = key.second.as<bool>() ? "true" : "false";
                } else {
                    strValue = key.second.as<std::string>();
                }
            } else if (key.second.IsSequence()) {
                // Convert array to comma-separated string
                std::vector<std::string> parts;
                for (const auto& item : key.second) {
                    if (item.IsScalar()) {
                        if (item.IsDouble()) {
                            parts.push_back(std::to_string(item.as<double>()));
                        } else if (item.IsInt()) {
                            parts.push_back(std::to_string(item.as<int64_t>()));
                        } else if (item.IsBool()) {
                            parts.push_back(item.as<bool>() ? "true" : "false");
                        } else {
                            parts.push_back("'" + item.as<std::string>() + "'");
                        }
                    }
                }
                strValue = "[" + boost::algorithm::join(parts, ", ") + "]";
            }
            
            if (!strValue.empty()) {
                set(keyStr, strValue, ConfigSource::File);
            }
        }
        
        return true;
    } catch (const YAML::Exception& e) {
        NLM_LOG_ERROR(std::string("Config::loadFromYAML parse error: ") + e.what(), "Config::loadFromYAML");
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Config::loadFromYAML error: ") + e.what(), "Config::loadFromYAML");
        return false;
    }
}

// Enhanced INI parser with better error handling
bool Config::loadFromINI(const std::string& content) {
    try {
        std::istringstream stream(content);
        std::string line;
        std::string currentSection;
        
        while (std::getline(stream, line)) {
            // Trim whitespace
            line = trim(line);
            
            // Skip empty lines and comments
            if (line.empty() || line[0] == '#' || line[0] == ';') {
                continue;
            }
            
            // Handle section headers
            if (line.front() == '[' && line.back() == ']') {
                currentSection = line.substr(1, line.size() - 2);
                continue;
            }
            
            // Parse key=value pairs
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                
                if (key.empty()) {
                    NLM_LOG_WARNING(ConfigErrorMessages::NULL_KEY, "loadFromINI");
                    continue;
                }
                
                // Remove quotes if present
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                // Add section prefix if not empty
                if (!currentSection.empty()) {
                    key = currentSection + "." + key;
                }
                
                set(key, value, ConfigSource::File);
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Config::loadFromINI error: ") + e.what(), "Config::loadFromINI");
        return false;
    }
}

// Enhanced file loading with format detection and error handling
bool Config::loadFromFileWithFormat(const std::string& filepath, FileFormat format) {
    pImpl->fileOperationCount++;
    
    if (filepath.empty()) {
        NLM_LOG_ERROR(ConfigErrorMessages::INVALID_FILENAME, "loadFromFileWithFormat");
        return false;
    }
    
    // Check if file exists
    if (!fileExists(filepath)) {
        NLM_LOG_ERROR(ConfigErrorMessages::FILE_NOT_OPEN, filepath);
        return false;
    }
    
    // Check if file is readable
    if (!fileReadable(filepath)) {
        NLM_LOG_ERROR(ConfigErrorMessages::PERMISSION_DENIED, filepath);
        return false;
    }
    
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR(ConfigErrorMessages::FILE_NOT_OPEN, filepath);
            return false;
        }
        
        // Check file size (prevent loading huge files)
        file.seekg(0, std::ios::end);
        size_t fileSize = file.tellg();
        file.seekg(0, std::ios::beg);
        
        if (fileSize > ConfigValidationConstants::MAX_VALUE_LENGTH) {
            NLM_LOG_ERROR(std::string("Config file too large: ") + filepath, "loadFromFileWithFormat");
            return false;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        
        pImpl->filename = filepath;
        
        switch (format) {
            case FileFormat::JSON:
                return loadFromJSON(content);
            case FileFormat::YAML:
                return loadFromYAML(content);
            case FileFormat::INI:
                return loadFromINI(content);
            default:
                NLM_LOG_ERROR(ConfigErrorMessages::INVALID_FORMAT, filepath);
                return false;
        }
    } catch (const std::bad_alloc& e) {
        NLM_LOG_ERROR(std::string("Config::loadFromFileWithFormat memory error: ") + e.what(), "loadFromFileWithFormat");
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Config::loadFromFileWithFormat error: ") + e.what(), "loadFromFileWithFormat");
        return false;
    }
}

bool Config::loadFromFile(const std::string& filepath) {
    if (filepath.empty()) {
        NLM_LOG_ERROR(ConfigErrorMessages::INVALID_FILENAME, "loadFromFile");
        return false;
    }
    
    FileFormat format = detectFileFormat(filepath);
    if (format == FileFormat::Unknown) {
        NLM_LOG_ERROR(ConfigErrorMessages::INVALID_FORMAT, filepath);
        return false;
    }
    
    return loadFromFileWithFormat(filepath, format);
}

// Enhanced validation with comprehensive error reporting
bool Config::validate() const {
    pImpl->validationErrors.clear();
    pImpl->validationErrors.reserve(pImpl->schemaRules.size() * 2);
    
    for (const auto& rule : pImpl->schemaRules) {
        if (!validateKey(rule.key)) {
            if (rule.type == ValidationRule::Required) {
                pImpl->validationErrors.emplace_back(
                    rule.key, "Required configuration key is missing", ConfigSource::File);
            }
            continue;
        }
        
        auto value = get<std::string>(rule.key);
        if (!value.has_value()) {
            continue;
        }
        
        // Validate value using the rule
        if (!validateValue(rule.key, value.value(), rule)) {
            // Error already logged by validateValue
            // Add to validation errors list for retrieval
            pImpl->validationErrors.emplace_back(
                rule.key, "Validation failed for key: " + rule.key, ConfigSource::File);
        }
    }
    
    return pImpl->validationErrors.empty();
}

// Get validation errors
std::vector<ConfigValidationError> Config::getValidationErrors() const {
    return pImpl->validationErrors;
}

// Enhanced inheritance with error handling
bool Config::loadWithInheritance(const ConfigInheritance& inheritance) {
    try {
        // Clear current configuration first
        clear();
        
        // Load base configuration
        if (!inheritance.baseFile.empty()) {
            if (!loadFromFile(inheritance.baseFile)) {
                NLM_LOG_ERROR(std::string("Config::loadWithInheritance: Failed to load base file: ") + inheritance.baseFile, "loadWithInheritance");
                return false;
            }
        }
        
        // Load override configuration
        if (!inheritance.overrideFile.empty()) {
            if (!loadFromFile(inheritance.overrideFile)) {
                NLM_LOG_ERROR(std::string("Config::loadWithInheritance: Failed to load override file: ") + inheritance.overrideFile, "loadWithInheritance");
                return false;
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Config::loadWithInheritance exception: ") + e.what(), "loadWithInheritance");
        return false;
    }
}

// Enhanced command line parsing with validation
bool Config::loadFromArgsWithValidation(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                if (key.empty()) {
                    NLM_LOG_WARNING(ConfigErrorMessages::NULL_KEY, "loadFromArgsWithValidation");
                    continue;
                }
                
                // Enhanced validation for key-value pairs
                if (!validateCommandLineKey(key)) {
                    NLM_LOG_WARNING(std::string("Invalid command line key: ") + key, "loadFromArgsWithValidation");
                    continue;
                }
                
                set(key, value, ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            if (key.empty()) {
                NLM_LOG_WARNING(ConfigErrorMessages::NULL_KEY, "loadFromArgsWithValidation");
                continue;
            }
            
            // Enhanced validation for key-value pairs
            if (!validateCommandLineKey(key)) {
                NLM_LOG_WARNING(std::string("Invalid command line key: ") + key, "loadFromArgsWithValidation");
                continue;
            }
            
            set(key, value, ConfigSource::CommandLine);
        }
        // Handle boolean flags
        else if (arg.substr(0, 2) == "--" && (i + 1 == argc || argv[i + 1][0] == '-')) {
            std::string key = arg.substr(2);
            
            if (key.empty()) {
                NLM_LOG_WARNING(ConfigErrorMessages::NULL_KEY, "loadFromArgsWithValidation");
                continue;
            }
            
            // Enhanced validation for boolean flags
            if (!validateCommandLineKey(key)) {
                NLM_LOG_WARNING(std::string("Invalid command line key: ") + key, "loadFromArgsWithValidation");
                continue;
            }
            
            set(key, "true", ConfigSource::CommandLine);
        }
        // Handle help and version flags
        else if (arg == "--help" || arg == "-?" || arg == "--help-config" || arg == "--config-help" || arg == "--list-options") {
            // Help flags are handled by the main program, not config parsing
            continue;
        }
        // Handle invalid arguments
        else if (arg[0] == '-' && arg.length() > 1) {
            // Could be a short flag with value or an unknown flag
            if (arg.find('=') != std::string::npos) {
                std::string key = arg.substr(1, arg.find('=') - 1);
                std::string value = arg.substr(arg.find('=') + 1);
                
                if (!validateCommandLineKey(key)) {
                    NLM_LOG_WARNING(std::string("Invalid command line key: ") + key, "loadFromArgsWithValidation");
                    continue;
                }
                
                set(key, value, ConfigSource::CommandLine);
            } else if (i + 1 < argc && argv[i + 1][0] != '-') {
                std::string key = arg.substr(1);
                std::string value = argv[++i];
                
                if (!validateCommandLineKey(key)) {
                    NLM_LOG_WARNING(std::string("Invalid command line key: ") + key, "loadFromArgsWithValidation");
                    continue;
                }
                
                set(key, value, ConfigSource::CommandLine);
            } else {
                // Unknown or unsupported short flag
                NLM_LOG_WARNING(std::string("Unknown or unsupported command line flag: ") + arg, "loadFromArgsWithValidation");
            }
        } else {
            // Non-flag argument (could be config file or other parameter)
            // Let the main program handle non-config arguments
            continue;
        }
    }
    
    return true;
}

// Simplified command line parsing (legacy)
bool Config::loadFromArgs(int argc, char** argv) {
    // For backward compatibility, use enhanced parsing with validation
    return loadFromArgsWithValidation(argc, argv);
}

// Enhanced command line key validation
bool Config::validateCommandLineKey(const std::string& key) const {
    // Allow alphanumeric keys with underscores and hyphens
    if (key.empty()) return false;
    
    for (char c : key) {
        if (!std::isalnum(c) && c != '_' && c != '-') {
            return false;
        }
    }
    
    return true;
}

// Enhanced command line argument validation
bool Config::validateCommandLineArgs(int argc, char** argv) {
    std::unordered_set<std::string> seenKeys;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Skip help flags
        if (arg == "--help" || arg == "-?" || arg == "--help-config" || arg == "--config-help" || arg == "--list-options") {
            continue;
        }
        
        // Skip config file
        if (arg == "--config" || arg == "-c") {
            if (++i < argc) continue; // Skip next argument
            continue;
        }
        
        // Extract key from --key=value or -key value format
        std::string key;
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                key = arg.substr(2, pos - 2);
            } else {
                key = arg.substr(2);
            }
        } else if (arg[0] == '-' && arg.length() > 1) {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                key = arg.substr(1, pos - 1);
            } else {
                key = arg.substr(1);
            }
        }
        
        if (!key.empty()) {
            if (seenKeys.find(key) != seenKeys.end()) {
                NLM_LOG_WARNING(std::string("Duplicate command line key: ") + key, "validateCommandLineArgs");
                // Allow duplicates for now but log warning
            }
            seenKeys.insert(key);
        }
    }
    
    return true;
}

// Memory-efficient save operations
bool Config::saveToFile(const std::string& filepath) const {
    pImpl->fileOperationCount++;
    
    if (filepath.empty()) {
        NLM_LOG_ERROR(ConfigErrorMessages::INVALID_FILENAME, "saveToFile");
        return false;
    }
    
    // Ensure directory exists
    if (!ensureDirectoryExists(filepath)) {
        return false;
    }
    
    try {
        std::ofstream file(filepath, std::ios::trunc);
        if (!file.is_open()) {
            NLM_LOG_ERROR(ConfigErrorMessages::SAVE_ERROR, filepath);
            return false;
        }
        
        // Choose format based on extension
        FileFormat format = detectFileFormat(filepath);
        
        switch (format) {
            case FileFormat::JSON:
                return saveToJSON(filepath);
            case FileFormat::YAML:
                return saveToYAML(filepath);
            case FileFormat::INI:
                return saveToINI(filepath);
            default:
                NLM_LOG_ERROR(std::string("Config::saveToFile: Unknown format for file: ") + filepath, "saveToFile");
                return false;
        }
    } catch (const std::bad_alloc& e) {
        NLM_LOG_ERROR(std::string("Config::saveToFile memory error: ") + e.what(), "saveToFile");
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Config::saveToFile error: ") + e.what(), "saveToFile");
        return false;
    }
}

// Backup with error handling
bool Config::saveWithBackup(const std::string& filepath) const {
    if (filepath.empty()) {
        NLM_LOG_ERROR(ConfigErrorMessages::INVALID_FILENAME, "saveWithBackup");
        return false;
    }
    
    try {
        // Create backup
        std::string backupPath = filepath + ".backup";
        
        // Remove existing backup if it exists
        if (std::filesystem::exists(backupPath)) {
            if (!std::filesystem::remove(backupPath)) {
                NLM_LOG_ERROR(std::string("Config::saveWithBackup: Cannot remove existing backup: ") + backupPath, "saveWithBackup");
                return false;
            }
        }
        
        // Move existing file to backup if it exists
        if (fileExists(filepath)) {
            if (!std::filesystem::rename(filepath, backupPath)) {
                NLM_LOG_ERROR(std::string("Config::saveWithBackup: Cannot create backup: ") + backupPath, "saveWithBackup");
                return false;
            }
        }
        
        // Save new file
        bool result = saveToFile(filepath);
        
        if (!result) {
            NLM_LOG_ERROR(ConfigErrorMessages::BACKUP_ERROR, filepath);
        }
        
        return result;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Config::saveWithBackup exception: ") + e.what(), "saveWithBackup");
        return false;
    }
}

// Configuration management
void Config::clear() {
    pImpl->entries.clear();
    pImpl->validationErrors.clear();
    pImpl->isModified = true;
    pImpl->fileOperationCount++;
}

bool Config::has(const std::string& key) const {
    pImpl->totalGetCalls++;
    
    if (key.empty()) {
        NLM_LOG_WARNING(ConfigErrorMessages::NULL_KEY, "has");
        return false;
    }
    
    return pImpl->entriesMap.find(key) != pImpl->entriesMap.end();
}

std::optional<std::string> Config::get(const std::string& key) const {
    pImpl->totalGetCalls++;
    
    if (key.empty()) {
        NLM_LOG_WARNING(ConfigErrorMessages::NULL_KEY, "get");
        return std::nullopt;
    }
    
    auto it = pImpl->entriesMap.find(key);
    if (it == pImpl->entriesMap.end()) {
        return std::nullopt;
    }
    
    return std::get<std::string>(it->second.value);
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    pImpl->totalSetCalls++;
    
    if (key.empty()) {
        NLM_LOG_WARNING(ConfigErrorMessages::NULL_KEY, "set");
        return;
    }
    
    if (value.empty() && source == ConfigSource::File) {
        NLM_LOG_WARNING(ConfigErrorMessages::NULL_VALUE, "set");
        return;
    }
    
    // Check for duplicate keys
    auto it = pImpl->entriesMap.find(key);
    if (it != pImpl->entriesMap.end() && it->second.source != ConfigSource::File) {
        NLM_LOG_WARNING(ConfigErrorMessages::DUPLICATE_KEY, "set", key);
    }
    
    ConfigEntry entry;
    entry.key = key;
    entry.value = value;
    entry.source = source;
    entry.timestamp = std::chrono::system_clock::now();
    
    if (it != pImpl->entriesMap.end()) {
        it->second = entry;
    } else {
        pImpl->entriesMap[key] = entry;
    }
    
    pImpl->isModified = true;
}

bool Config::remove(const std::string& key) {
    pImpl->totalSetCalls++;
    
    if (key.empty()) {
        NLM_LOG_WARNING(ConfigErrorMessages::NULL_KEY, "remove");
        return false;
    }
    
    auto it = pImpl->entriesMap.find(key);
    if (it == pImpl->entriesMap.end()) {
        return false;
    }
    
    pImpl->entriesMap.erase(it);
    pImpl->isModified = true;
    return true;
}

ConfigChecksum Config::computeChecksum() const {
    try {
        std::ostringstream oss;
        
        for (const auto& entry : pImpl->entriesMap) {
            oss << entry.first << "=" << entry.second.value;
        }
        
        std::string content = oss.str();
        std::string hash = computeMD5(content);
        
        return ConfigChecksum(hash);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Config::computeChecksum exception: ") + e.what(), "computeChecksum");
        return ConfigChecksum();
    }
}

bool Config::verifyChecksum(const ConfigChecksum& checksum) const {
    try {
        ConfigChecksum current = computeChecksum();
        if (current.value.empty()) {
            return false;
        }
        
        return current.value == checksum.value;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Config::verifyChecksum exception: ") + e.what(), "verifyChecksum");
        return false;
    }
}

// Statistics and debugging
ConfigStats Config::getStats() const {
    ConfigStats stats;
    stats.getCalls = pImpl->totalGetCalls;
    stats.setCalls = pImpl->totalSetCalls;
    stats.validationCalls = pImpl->totalValidationCalls;
    stats.fileOperations = pImpl->fileOperationCount;
    stats.entryCount = pImpl->entriesMap.size();
    stats.validationErrorCount = pImpl->validationErrors.size();
    stats.isModified = pImpl->isModified;
    
    return stats;
}

void Config::resetStats() {
    pImpl->totalGetCalls = 0;
    pImpl->totalSetCalls = 0;
    pImpl->totalValidationCalls = 0;
    pImpl->fileOperationCount = 0;
}

// Generate usage information
std::string Config::generateUsage(const std::string& programName) {
    std::string usage = "";
    usage += programName + " [OPTIONS]\n\n";
    usage += "OPTIONS:\n";
    usage += "  -h, --help              Show this help message\n";
    usage += "  -c, --config <file>      Load configuration from file\n";
    usage += "  -s, --seed <number>      Set random seed\n";
    usage += "  -t, --timestep <number>  Set simulation timestep\n";
    usage += "  -n, --neurons <number>   Set neuron count\n";
    usage += "\n";
    usage += "For more options, use --help for full usage information.\n";
    return usage;
}

// Get configuration help for specific key
std::string Config::getKeyHelp(const std::string& key) {
    // Return help for a specific configuration key
    // This can be implemented with a comprehensive help system
    std::map<std::string, std::string> helpMap = {
        {"seed", "Random seed for reproducible results (0-2147483647)"},
        {"timestep", "Simulation timestep in seconds (0.0001-0.1)"},
        {"neuron_count", "Number of neurons (1-1000000)"},
        {"region_count", "Number of brain regions (1-100)"},
        {"connection_probability", "Connection probability between neurons (0.0-1.0)"},
        {"stdp_ltp_weight", "STDP LTP weight (0.0-1.0)"},
        {"stdp_ltd_weight", "STDP LTD weight (0.0-1.0)"},
        {"stdp_tau", "STDP time constant (0.0-1000.0)"},
        {"synaptogenesis_rate", "Synaptogenesis rate (0.0-0.1)"},
        {"pruning_rate", "Pruning rate (0.0-0.01)"},
        {"log_level", "Log level (DEBUG, INFO, WARNING, ERROR)"},
        {"log_to_file", "Enable logging to file"},
        {"log_filename", "Log filename"},
        {"max_simulation_steps", "Maximum simulation steps (1-1000000)"},
        {"simulation_time_limit", "Simulation time limit in seconds (0.0-3600.0)"},
        {"visualization_enabled", "Enable visualization"},
        {"visualization_update_rate", "Visualization update rate in Hz (1.0-1000.0)"},
        {"inherit_config", "Base configuration file for inheritance"},
        {"override_config", "Override configuration file"},
        {"merge_config", "Merge inheritance configuration"},
        {"checksum", "Verify configuration checksum"},
        {"backup_config", "Save configuration with backup"}
    };
    
    auto it = helpMap.find(key);
    if (it != helpMap.end()) {
        return it->second;
    }
    
    return "No help available for this key.";
}

// Get configuration help
std::vector<ConfigHelpEntry> Config::getConfigurationHelp() {
    std::vector<ConfigHelpEntry> help;
    
    // Add common configuration help entries
    help.emplace_back("seed", "Random seed for reproducible results", "int64_t", "42", "123", false);
    help.emplace_back("timestep", "Simulation timestep in seconds", "double", "0.001", "0.01", false);
    help.emplace_back("neuron_count", "Number of neurons", "int64_t", "500", "2000", false);
    help.emplace_back("region_count", "Number of brain regions", "int64_t", "1", "5", false);
    help.emplace_back("connection_probability", "Connection probability between neurons", "float", "0.15", "0.2", false);
    help.emplace_back("stdp_ltp_weight", "STDP LTP weight", "float", "0.02", "0.03", false);
    help.emplace_back("stdp_ltd_weight", "STDP LTD weight", "float", "0.015", "0.02", false);
    help.emplace_back("stdp_tau", "STDP time constant", "float", "20.0", "50.0", false);
    help.emplace_back("synaptogenesis_rate", "Synaptogenesis rate", "float", "0.0001", "0.001", false);
    help.emplace_back("pruning_rate", "Pruning rate", "float", "0.00001", "0.0001", false);
    help.emplace_back("log_level", "Log level", "string", "INFO", "DEBUG", false);
    help.emplace_back("log_to_file", "Enable logging to file", "bool", "false", "true", false);
    help.emplace_back("log_filename", "Log filename", "string", "nlm.log", "debug.log", false);
    help.emplace_back("max_simulation_steps", "Maximum simulation steps", "int64_t", "10000", "50000", false);
    help.emplace_back("simulation_time_limit", "Simulation time limit in seconds", "double", "0.0", "300.0", false);
    help.emplace_back("visualization_enabled", "Enable visualization", "bool", "false", "true", false);
    help.emplace_back("visualization_update_rate", "Visualization update rate in Hz", "float", "30.0", "60.0", false);
    
    return help;
}

// Get configuration examples
std::string Config::getConfigurationExamples() {
    std::string examples = "";
    examples += "JSON Configuration Example:\n";
    examples += "{\n";
    examples += "  \"random_seed\": 42,\n";
    examples += "  \"simulation_timestep\": 0.001,\n";
    examples += "  \"neuron_count\": 500,\n";
    examples += "  \"region_count\": 1,\n";
    examples += "  \"connection_probability\": 0.15,\n";
    examples += "  \"stdp_ltp_weight\": 0.02,\n";
    examples += "  \"stdp_ltd_weight\": 0.015,\n";
    examples += "  \"stdp_tau\": 20.0,\n";
    examples += "  \"synaptogenesis_rate\": 0.0001,\n";
    examples += "  \"pruning_rate\": 0.00001,\n";
    examples += "  \"log_level\": \"INFO\",\n";
    examples += "  \"visualization_enabled\": false\n";
    examples += "}\n\n";
    
    examples += "YAML Configuration Example:\n";
    examples += "random_seed: 42\n";
    examples += "simulation_timestep: 0.001\n";
    examples += "neuron_count: 500\n";
    examples += "region_count: 1\n";
    examples += "connection_probability: 0.15\n";
    examples += "stdp_ltp_weight: 0.02\n";
    examples += "stdp_ltd_weight: 0.015\n";
    examples += "stdp_tau: 20.0\n";
    examples += "synaptogenesis_rate: 0.0001\n";
    examples += "pruning_rate: 0.00001\n";
    examples += "log_level: INFO\n";
    examples += "visualization_enabled: false\n\n";
    
    examples += "INI Configuration Example:\n";
    examples += "[general]\n";
    examples += "random_seed = 42\n";
    examples += "simulation_timestep = 0.001\n";
    examples += "neuron_count = 500\n";
    examples += "region_count = 1\n";
    examples += "connection_probability = 0.15\n";
    examples += "stdp_ltp_weight = 0.02\n";
    examples += "stdp_ltd_weight = 0.015\n";
    examples += "stdp_tau = 20.0\n";
    examples += "synaptogenesis_rate = 0.0001\n";
    examples += "pruning_rate = 0.00001\n";
    examples += "log_level = INFO\n";
    examples += "visualization_enabled = false\n\n";
    
    examples += "Example Command Line Usage:\n";
    examples += "./nlm --seed 123 --timestep 0.01 --neurons 2000\n";
    examples += "./nlm --config myconfig.yaml --log-level DEBUG\n";
    examples += "./nlm --validate-only --config myconfig.json\n";
    examples += "./nlm --inherit-config base.json --override-config overrides.json\n";
    examples += "./nlm --enable-profiling --profile-interval 100\n";
    examples += "./nlm --memory-limit 2048 --thread-count 8\n\n";
    
    return examples;
}

// Get available options
std::string Config::getAvailableOptions() {
    std::string options = "";
    options += "Available Command Line Options:\n";
    options += "\n";
    options += "Core Options:\n";
    options += "  -h, --help              Show help information\n";
    options += "  -c, --config <file>      Load configuration from file\n";
    options += "  -s, --seed <number>      Set random seed\n";
    options += "  -t, --timestep <number>  Set simulation timestep\n";
    options += "  -n, --neurons <number>   Set neuron count\n";
    options += "  -r, --regions <number>   Set region count\n";
    options += "  -p, --probability <num>  Set connection probability\n";
    options += "\n";
    options += "Plasticity Options:\n";
    options += "  --stdp-ltp <weight>      Set STDP LTP weight\n";
    options += "  --stdp-ltd <weight>      Set STDP LTD weight\n";
    options += "  --stdp-tau <value>       Set STDP time constant\n";
    options += "  --synaptogenesis-rate <rate> Set synaptogenesis rate\n";
    options += "  --pruning-rate <rate>    Set pruning rate\n";
    options += "\n";
    options += "Logging Options:\n";
    options += "  -ll, --log-level <level> Set log level (DEBUG, INFO, WARNING, ERROR)\n";
    options += "  -lt, --log-to-file       Enable logging to file\n";
    options += "  -lf, --log-filename <name> Set log filename\n";
    options += "\n";
    options += "Simulation Options:\n";
    options += "  -ms, --max-steps <steps>  Set maximum simulation steps\n";
    options += "  -stl, --time-limit <time> Set simulation time limit\n";
    options += "\n";
    options += "Advanced Options:\n";
    options += "  --inherit-config <file>   Base configuration file\n";
    options += "  --override-config <file>  Override configuration file\n";
    options += "  --merge-config            Merge inheritance configuration\n";
    options += "  --checksum <hash>        Verify checksum\n";
    options += "  --backup-config           Save with backup\n";
    options += "\n";
    options += "For complete help, use --help for full usage information.\n";
    return options;
}