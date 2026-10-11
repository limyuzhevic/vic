#include "Config.hpp"
#include "Errors.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <ctime>
#include <unordered_set>
#include <system_error>
#include <iomanip>

namespace nlm {

static std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    
    // Validation and schema support
    std::unordered_set<std::string> requiredKeys;
    std::unordered_set<std::string> forbiddenKeys;
    std::unordered_map<std::string, std::function<bool(const ConfigValue&)>> validationRules;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default validation rules
    setupDefaultValidationRules();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

void Config::setupDefaultValidationRules() {
    // Add common validation rules for typical NLM configuration keys
    // These can be extended or overridden
    
    // Neuron count validation
    validationRules["neuron_count"] = [](const ConfigValue& val) {
        if (const int* v = std::get_if<int>(&val)) {
            return *v > 0 && *v <= 1000000;  // Reasonable bounds
        } else if (const int64_t* v = std::get_if<int64_t>(&val)) {
            return *v > 0 && *v <= 1000000;
        }
        return false;
    };
    
    // Connection probability validation
    validationRules["connection_probability"] = [](const ConfigValue& val) {
        if (const double* v = std::get_if<double>(&val)) {
            return *v >= 0.0 && *v <= 1.0;
        } else if (const float* v = std::get_if<float>(&val)) {
            return *v >= 0.0f && *v <= 1.0f;
        }
        return false;
    };
    
    // Simulation timestep validation
    validationRules["simulation_timestep"] = [](const ConfigValue& val) {
        if (const double* v = std::get_if<double>(&val)) {
            return *v > 0.0 && *v <= 0.1;  // Reasonable timestep
        } else if (const float* v = std::get_if<float>(&val)) {
            return *v > 0.0f && *v <= 0.1f;
        }
        return false;
    };
}

bool Config::loadFromFile(const std::string& filepath) {
    ErrorContext ctx("loadFromFile", filepath);
    ctx.addMetadata("timestamp", getCurrentTimestamp());
    
    try {
        NLM_LOG_INFO("Loading configuration from file: " + filepath + " at " + getCurrentTimestamp());
        
        // Verify file exists and is readable
        if (!std::filesystem::exists(filepath)) {
            throw FileIOError("Configuration file not found", 
                            "File path: " + filepath + " was not found or is inaccessible",
                            ErrorCode::CONFIG_FILE_NOT_FOUND,
                            __FILE__, __LINE__, __func__);
        }
        
        if (!std::filesystem::is_regular_file(filepath)) {
            throw FileIOError("Invalid configuration file", 
                            "Path is not a regular file: " + filepath,
                            ErrorCode::FILE_READ_FAILED,
                            __FILE__, __LINE__, __func__);
        }
        
        // Check file size to prevent loading extremely large files
        auto fileSize = std::filesystem::file_size(filepath);
        if (fileSize > 10 * 1024 * 1024) { // 10MB limit
            throw FileIOError("Configuration file too large", 
                            "File size: " + std::to_string(fileSize) + " bytes exceeds 10MB limit",
                            ErrorCode::FILE_CORRUPTED,
                            __FILE__, __LINE__, __func__);
        }
        
        // Try JSON format first, fall back to simple key=value format for backward compatibility
        bool success = tryLoadJSON(filepath);
        if (!success) {
            NLM_LOG_INFO("JSON parsing failed, trying simple format for " + filepath);
            success = tryLoadSimpleFormat(filepath);
        }
        
        if (success) {
            NLM_LOG_INFO("Configuration loaded successfully from: " + filepath);
            ctx.addMetadata("status", "success");
            NLM_LOG_INFO("Configuration entries: " + std::to_string(pImpl->entries.size()));
            
            // Validate configuration after loading
            bool validationResult = validate();
            if (!validationResult) {
                NLM_LOG_WARNING("Configuration validation warnings for file: " + filepath);
            }
            
            return true;
        } else {
            throw FileIOError("Failed to load configuration", 
                            "Unable to parse file: " + filepath + " in any supported format",
                            ErrorCode::CONFIG_INVALID_FORMAT,
                            __FILE__, __LINE__, __func__);
        }
        
    } catch (const FileIOError& e) {
        NLM_LOG_ERROR("FileIOError loading configuration from " + filepath + ": " + e.what());
        ErrorReporterRegistry::reportError(e);
        throw;
    } catch (const ConfigurationException& e) {
        NLM_LOG_ERROR("ConfigurationException loading configuration from " + filepath + ": " + e.what());
        ErrorReporterRegistry::reportError(e);
        throw;
    } catch (const std::exception& e) {
        ErrorContext errorCtx("loadFromFile error", filepath);
        errorCtx.addMetadata("operation", "loadFromFile");
        errorCtx.addMetadata("file", filepath);
        errorCtx.addMetadata("original_error", e.what());
        
        NLM_LOG_ERROR("Unexpected error loading configuration from " + filepath + ": " + e.what());
        throw ConfigurationException("Failed to load configuration from " + filepath + ": " + e.what(),
                                   errorCtx.getMetadataSummary(),
                                   ErrorCode::CONFIG_INVALID_FORMAT,
                                   __FILE__, __LINE__, __func__);
    }
}

bool Config::tryLoadJSON(const std::string& filepath) {
    ErrorContext ctx("tryLoadJSON", filepath);
    ctx.addMetadata("parser", "custom_json_parser");
    
    try {
        NLM_LOG_INFO("Attempting JSON parsing for configuration file: " + filepath);
        
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw FileIOError("Cannot open configuration file for reading", 
                            "File path: " + filepath + ", error: " + std::strerror(errno),
                            ErrorCode::FILE_OPEN_FAILED,
                            __FILE__, __LINE__, __func__);
        }
        
        // Check if file is empty
        file.seekg(0, std::ios::end);
        if (file.tellg() == 0) {
            throw FileIOError("Configuration file is empty", 
                            "File: " + filepath + " has zero bytes",
                            ErrorCode::FILE_CORRUPTED,
                            __FILE__, __LINE__, __func__);
        }
        file.seekg(0, std::ios::beg);
        
        // Peek at the first character to determine format
        file.seekg(0, std::ios::beg);
        char firstChar;
        if (!(file >> firstChar)) {
            throw FileIOError("Cannot read first character of configuration file", 
                            "File: " + filepath + " cannot be read properly",
                            ErrorCode::FILE_PARSE_FAILED,
                            __FILE__, __LINE__, __func__);
        }
        
        file.clear();
        file.seekg(0, std::ios::beg);
        
        // Check if it's JSON (starts with { or [")
        if (firstChar == '{' || firstChar == '[') {
            NLM_LOG_DEBUG("File " + filepath + " appears to be JSON format");
            return loadJSON(file);
        }
        
        NLM_LOG_DEBUG("File " + filepath + " is not JSON format, trying simple format");
        // Not JSON, fall through to simple format
        file.clear();
        return false;
        
    } catch (const FileIOError& e) {
        NLM_LOG_ERROR("FileIOError in tryLoadJSON for " + filepath + ": " + e.what());
        ErrorReporterRegistry::reportError(e);
        throw;
    } catch (const ConfigurationException& e) {
        NLM_LOG_ERROR("ConfigurationException in tryLoadJSON for " + filepath + ": " + e.what());
        ErrorReporterRegistry::reportError(e);
        throw;
    } catch (const std::exception& e) {
        ErrorContext errorCtx("JSON parsing error", filepath);
        errorCtx.addMetadata("parser", "custom_json_parser");
        errorCtx.addMetadata("original_error", e.what());
        
        NLM_LOG_ERROR("Unexpected error in tryLoadJSON for " + filepath + ": " + e.what());
        throw ConfigurationException("JSON parsing failed for " + filepath + ": " + e.what(),
                                   errorCtx.getMetadataSummary(),
                                   ErrorCode::CONFIG_INVALID_FORMAT,
                                   __FILE__, __LINE__, __func__);
    }
}

bool Config::loadJSON(std::ifstream& file) {
    ErrorContext ctx("loadJSON", "");
    ctx.addMetadata("parser", "custom_json_parser");
    
    try {
        NLM_LOG_INFO("Starting JSON parsing of configuration");
        
        // Simple JSON parsing without external library
        // For now, we'll implement a minimal JSON parser for common cases
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        
        // Check for valid JSON content
        if (content.empty()) {
            throw ConfigurationException("Empty JSON content", 
                                        "JSON string is empty",
                                        ErrorCode::CONFIG_INVALID_FORMAT,
                                        __FILE__, __LINE__, __func__);
        }
        
        // Count parsing attempts for statistics
        size_t pos = 0;
        size_t entriesParsed = 0;
        
        while (pos < content.length()) {
            // Find next " key": pattern
            size_t keyStart = content.find('"', pos);
            if (keyStart == std::string::npos) {
                NLM_LOG_DEBUG("No more JSON keys found");
                break;
            }
            
            size_t keyEnd = content.find('"', keyStart + 1);
            if (keyEnd == std::string::npos) {
                throw ConfigurationException("Malformed JSON: unclosed string", 
                                            "At position " + std::to_string(pos) + " in JSON content",
                                            ErrorCode::CONFIG_INVALID_FORMAT,
                                            __FILE__, __LINE__, __func__);
            }
            
            std::string key = content.substr(keyStart + 1, keyEnd - keyStart - 1);
            key = trim(key);
            
            if (key.empty()) {
                throw ConfigurationException("Malformed JSON: empty key", 
                                            "Found empty string key in JSON",
                                            ErrorCode::CONFIG_INVALID_FORMAT,
                                            __FILE__, __LINE__, __func__);
            }
            
            NLM_LOG_DEBUG("Parsing JSON key: " + key);
            
            // Find ": value" pattern
            size_t colonPos = content.find(':', keyEnd);
            if (colonPos == std::string::npos) {
                throw ConfigurationException("Malformed JSON: missing colon after key", 
                                            "Key: " + key + ", position: " + std::to_string(keyEnd),
                                            ErrorCode::CONFIG_INVALID_FORMAT,
                                            __FILE__, __LINE__, __func__);
            }
            
            size_t valueStart = content.find_first_not_of(' ', colonPos + 1);
            if (valueStart == std::string::npos) {
                throw ConfigurationException("Malformed JSON: no value after colon", 
                                            "Key: " + key,
                                            ErrorCode::CONFIG_INVALID_FORMAT,
                                            __FILE__, __LINE__, __func__);
            }
            
            // Parse value based on first character
            char valueChar = content[valueStart];
            std::string valueStr;
            size_t valueEnd;
            
            if (valueChar == '"') {
                // String value
                valueEnd = content.find('"', valueStart + 1);
                if (valueEnd == std::string::npos) {
                    throw ConfigurationException("Malformed JSON: unclosed string value", 
                                                "Key: " + key,
                                                ErrorCode::CONFIG_INVALID_FORMAT,
                                                __FILE__, __LINE__, __func__);
                }
                valueStr = content.substr(valueStart + 1, valueEnd - valueStart - 1);
                NLM_LOG_DEBUG("Parsed string value for key " + key + ": " + valueStr);
                
            } else if (valueChar == '[') {
                // Array value (simple implementation)
                size_t bracketEnd = content.find(']', valueStart);
                if (bracketEnd == std::string::npos) {
                    throw ConfigurationException("Malformed JSON: unclosed array", 
                                                "Key: " + key,
                                                ErrorCode::CONFIG_INVALID_FORMAT,
                                                __FILE__, __LINE__, __func__);
                }
                valueStr = content.substr(valueStart, bracketEnd - valueStart + 1);
                NLM_LOG_DEBUG("Parsed array value for key " + key + ": " + valueStr);
                
            } else if (valueChar == '{') {
                // Object value (skip for now)
                size_t braceEnd = content.find('}', valueStart);
                if (braceEnd == std::string::npos) {
                    throw ConfigurationException("Malformed JSON: unclosed object", 
                                                "Key: " + key,
                                                ErrorCode::CONFIG_INVALID_FORMAT,
                                                __FILE__, __LINE__, __func__);
                }
                valueStr = content.substr(valueStart, braceEnd - valueStart + 1);
                NLM_LOG_DEBUG("Parsed object value for key " + key + ": " + valueStr);
                
            } else {
                // Number or boolean
                valueEnd = content.find_first_of(", }\n\r", valueStart);
                if (valueEnd == std::string::npos) {
                    valueEnd = content.length();
                }
                valueStr = content.substr(valueStart, valueEnd - valueStart);
                NLM_LOG_DEBUG("Parsed scalar value for key " + key + ": " + valueStr);
            }
            
            // Convert string value to appropriate type
            try {
                ConfigValue configValue = parseJSONValue(valueStr);
                set(key, configValue, ConfigSource::File);
                entriesParsed++;
            } catch (const ConfigurationException& e) {
                NLM_LOG_ERROR("Error parsing JSON value for key " + key + ": " + e.what());
                // Continue parsing other keys instead of failing completely
                NLM_LOG_WARNING("Skipping key " + key + " due to parsing error, continuing with other keys");
            }
            
            pos = valueEnd + 1;
        }
        
        NLM_LOG_INFO("JSON parsing completed: " + std::to_string(entriesParsed) + " entries parsed");
        ctx.addMetadata("entries_parsed", std::to_string(entriesParsed));
        return true;
        
    } catch (const ConfigurationException& e) {
        NLM_LOG_ERROR("ConfigurationException in loadJSON: " + e.what());
        ErrorReporterRegistry::reportError(e);
        throw;
    } catch (const std::exception& e) {
        ErrorContext errorCtx("JSON load error", "");
        errorCtx.addMetadata("parser", "custom_json_parser");
        errorCtx.addMetadata("original_error", e.what());
        
        NLM_LOG_ERROR("Unexpected error in loadJSON: " + e.what());
        throw ConfigurationException("Failed to load JSON configuration: " + e.what(),
                                   errorCtx.getMetadataSummary(),
                                   ErrorCode::CONFIG_INVALID_FORMAT,
                                   __FILE__, __LINE__, __func__);
    }
}

bool Config::tryLoadSimpleFormat(const std::string& filepath) {
    ErrorContext ctx("tryLoadSimpleFormat", filepath);
    ctx.addMetadata("parser", "simple_key_value");
    
    try {
        NLM_LOG_INFO("Attempting simple key=value parsing for configuration file: " + filepath);
        
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw FileIOError("Cannot open configuration file for reading", 
                            "File path: " + filepath + ", error: " + std::strerror(errno),
                            ErrorCode::FILE_OPEN_FAILED,
                            __FILE__, __LINE__, __func__);
        }
        
        std::string line;
        size_t linesProcessed = 0;
        size_t entriesProcessed = 0;
        
        while (std::getline(file, line)) {
            linesProcessed++;
            // Skip empty lines and comments
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            // Parse simple key=value pairs
            size_t pos = line.find('=');
            if (pos == std::string::npos) {
                NLM_LOG_WARNING("Skipping line (no '=' found): " + line);
                continue;
            }
            
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            if (key.empty()) {
                NLM_LOG_WARNING("Skipping line with empty key: " + line);
                continue;
            }
            
            NLM_LOG_DEBUG("Processing key-value pair: " + key + " = " + value);
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            try {
                set(key, value, ConfigSource::File);
                entriesProcessed++;
            } catch (const ConfigurationException& e) {
                NLM_LOG_ERROR("Error processing key-value pair " + key + ": " + e.what());
                // Continue processing other lines
            }
        }
        
        NLM_LOG_INFO("Simple format parsing completed: " + std::to_string(linesProcessed) + 
                    " lines processed, " + std::to_string(entriesProcessed) + " entries added");
        ctx.addMetadata("lines_processed", std::to_string(linesProcessed));
        ctx.addMetadata("entries_processed", std::to_string(entriesProcessed));
        
        return true;
        
    } catch (const FileIOError& e) {
        NLM_LOG_ERROR("FileIOError in tryLoadSimpleFormat for " + filepath + ": " + e.what());
        ErrorReporterRegistry::reportError(e);
        throw;
    } catch (const ConfigurationException& e) {
        NLM_LOG_ERROR("ConfigurationException in tryLoadSimpleFormat for " + filepath + ": " + e.what());
        ErrorReporterRegistry::reportError(e);
        throw;
    } catch (const std::exception& e) {
        ErrorContext errorCtx("Simple format parsing error", filepath);
        errorCtx.addMetadata("parser", "simple_key_value");
        errorCtx.addMetadata("original_error", e.what());
        
        NLM_LOG_ERROR("Unexpected error in tryLoadSimpleFormat for " + filepath + ": " + e.what());
        throw ConfigurationException("Failed to parse simple configuration format from " + filepath + ": " + e.what(),
                                   errorCtx.getMetadataSummary(),
                                   ErrorCode::CONFIG_INVALID_FORMAT,
                                   __FILE__, __LINE__, __func__);
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    try {
        NLM_LOG_INFO("Loading configuration from command line arguments");
        
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
        
        NLM_LOG_INFO("Configuration loaded from command line arguments: " + std::to_string(pImpl->entries.size()) + " entries");
        return true;
        
    } catch (const ConfigurationException& e) {
        NLM_LOG_ERROR("ConfigurationException loading from command line: " + e.what());
        ErrorReporterRegistry::reportError(e);
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error loading from command line: " + e.what());
        throw ConfigurationException("Failed to load configuration from command line: " + e.what(),
                                   "Operation: loadFromArgs",
                                   ErrorCode::CONFIG_INVALID_FORMAT,
                                   __FILE__, __LINE__, __func__);
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    ErrorContext ctx("saveToFile", filepath);
    ctx.addMetadata("timestamp", getCurrentTimestamp());
    
    try {
        NLM_LOG_INFO("Saving configuration to file: " + filepath + " at " + getCurrentTimestamp());
        
        // Verify directory exists
        std::string dirPath = std::filesystem::path(filepath).parent_path();
        if (!dirPath.empty() && !std::filesystem::exists(dirPath)) {
            throw FileIOError("Cannot save configuration: directory does not exist", 
                            "Directory: " + dirPath + " does not exist",
                            ErrorCode::FILE_WRITE_FAILED,
                            __FILE__, __LINE__, __func__);
        }
        
        // Determine format based on file extension or content
        if (hasJSONContent()) {
            return saveJSON(filepath);
        } else {
            return saveSimpleFormat(filepath);
        }
        
    } catch (const FileIOError& e) {
        NLM_LOG_ERROR("FileIOError saving configuration to " + filepath + ": " + e.what());
        ErrorReporterRegistry::reportError(e);
        throw;
    } catch (const ConfigurationException& e) {
        NLM_LOG_ERROR("ConfigurationException saving configuration to " + filepath + ": " + e.what());
        ErrorReporterRegistry::reportError(e);
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error saving configuration to " + filepath + ": " + e.what());
        throw ConfigurationException("Failed to save configuration to " + filepath + ": " + e.what(),
                                   "Operation: saveToFile",
                                   ErrorCode::FILE_WRITE_FAILED,
                                   __FILE__, __LINE__, __func__);
    }
}

bool Config::saveJSON(const std::string& filepath) const {
    try {
        NLM_LOG_INFO("Saving configuration as JSON to: " + filepath);
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw FileIOError("Cannot create configuration file for writing", 
                            "File path: " + filepath + ", error: " + std::strerror(errno),
                            ErrorCode::FILE_OPEN_FAILED,
                            __FILE__, __LINE__, __func__);
        }
        
        file << "{\n";
        for (size_t i = 0; i < pImpl->entries.size(); ++i) {
            const auto& entry = pImpl->entries[i];
            file << "  \"" << entry.key << "\": ";
            
            // Serialize based on type
            bool serialized = false;
            if (const int* v = std::get_if<int>(&entry.value)) {
                file << *v;
                serialized = true;
            } else if (const int64_t* v = std::get_if<int64_t>(&entry.value)) {
                file << *v;
                serialized = true;
            } else if (const double* v = std::get_if<double>(&entry.value)) {
                file << *v;
                serialized = true;
            } else if (const float* v = std::get_if<float>(&entry.value)) {
                file << *v;
                serialized = true;
            } else if (const bool* v = std::get_if<bool>(&entry.value)) {
                file << (*v ? "true" : "false");
                serialized = true;
            } else if (const std::string* v = std::get_if<std::string>(&entry.value)) {
                file << "\"" << escapeJSON(*v) << "\"";
                serialized = true;
            } else if (const std::vector<int>* v = std::get_if<std::vector<int>>(&entry.value)) {
                file << "[";
                for (size_t j = 0; j < v->size(); ++j) {
                    if (j > 0) file << ", ";
                    file << (*v)[j];
                }
                file << "]";
                serialized = true;
            } else if (const std::vector<double>* v = std::get_if<std::vector<double>>(&entry.value)) {
                file << "[";
                for (size_t j = 0; j < v->size(); ++j) {
                    if (j > 0) file << ", ";
                    file << (*v)[j];
                }
                file << "]";
                serialized = true;
            } else if (const std::vector<std::string>* v = std::get_if<std::vector<std::string>>(&entry.value)) {
                file << "[";
                for (size_t j = 0; j < v->size(); ++j) {
                    if (j > 0) file << ", ";
                    file << "\"" << escapeJSON((*v)[j]) << "\"";
                }
                file << "]";
                serialized = true;
            }
            
            if (!serialized) {
                file << "null";  // Unknown type, serialize as null
            }
            
            if (i < pImpl->entries.size() - 1) {
                file << ",\n";
            } else {
                file << "\n";
            }
        }
        
        file << "}\n";
        NLM_LOG_INFO("JSON configuration saved successfully to " + filepath);
        return true;
        
    } catch (const FileIOError& e) {
        NLM_LOG_ERROR("FileIOError in saveJSON for " + filepath + ": " + e.what());
        ErrorReporterRegistry::reportError(e);
        throw;
    } catch (const ConfigurationException& e) {
        NLM_LOG_ERROR("ConfigurationException in saveJSON for " + filepath + ": " + e.what());
        ErrorReporterRegistry::reportError(e);
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error in saveJSON for " + filepath + ": " + e.what());
        throw ConfigurationException("Failed to save JSON configuration to " + filepath + ": " + e.what(),
                                   "Operation: saveJSON",
                                   ErrorCode::FILE_WRITE_FAILED,
                                   __FILE__, __LINE__, __func__);
    }
}

bool Config::saveSimpleFormat(const std::string& filepath) const {
    try {
        NLM_LOG_INFO("Saving configuration as simple format to: " + filepath);
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw FileIOError("Cannot create configuration file for writing", 
                            "File path: " + filepath + ", error: " + std::strerror(errno),
                            ErrorCode::FILE_OPEN_FAILED,
                            __FILE__, __LINE__, __func__);
        }
        
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = ";
            
            // Serialize value based on type
            bool serialized = false;
            if (const int* v = std::get_if<int>(&entry.value)) {
                file << *v;
                serialized = true;
            } else if (const int64_t* v = std::get_if<int64_t>(&entry.value)) {
                file << *v;
                serialized = true;
            } else if (const double* v = std::get_if<double>(&entry.value)) {
                file << *v;
                serialized = true;
            } else if (const float* v = std::get_if<float>(&entry.value)) {
                file << *v;
                serialized = true;
            } else if (const bool* v = std::get_if<bool>(&entry.value)) {
                file << (*v ? "true" : "false");
                serialized = true;
            } else if (const std::string* v = std::get_if<std::string>(&entry.value)) {
                file << "\"" << *v << "\"";
                serialized = true;
            } else if (const std::vector<int>* v = std::get_if<std::vector<int>>(&entry.value)) {
                file << "[";
                for (size_t j = 0; j < v->size(); ++j) {
                    if (j > 0) file << ", ";
                    file << (*v)[j];
                }
                file << "]";
                serialized = true;
            } else if (const std::vector<double>* v = std::get_if<std::vector<double>>(&entry.value)) {
                file << "[";
                for (size_t j = 0; j < v->size(); ++j) {
                    if (j > 0) file << ", ";
                    file << (*v)[j];
                }
                file << "]";
                serialized = true;
            } else if (const std::vector<std::string>* v = std::get_if<std::vector<std::string>>(&entry.value)) {
                file << "[";
                for (size_t j = 0; j < v->size(); ++j) {
                    if (j > 0) file << ", ";
                    file << "\"" << (*v)[j] << "\"";
                }
                file << "]";
                serialized = true;
            }
            
            if (!serialized) {
                file << "UNKNOWN_TYPE";
            }
            
            file << "\n";
        }
        
        NLM_LOG_INFO("Simple configuration saved successfully to " + filepath);
        return true;
        
    } catch (const FileIOError& e) {
        NLM_LOG_ERROR("FileIOError in saveSimpleFormat for " + filepath + ": " + e.what());
        ErrorReporterRegistry::reportError(e);
        throw;
    } catch (const ConfigurationException& e) {
        NLM_LOG_ERROR("ConfigurationException in saveSimpleFormat for " + filepath + ": " + e.what());
        ErrorReporterRegistry::reportError(e);
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error in saveSimpleFormat for " + filepath + ": " + e.what());
        throw ConfigurationException("Failed to save configuration to " + filepath + ": " + e.what(),
                                   "Operation: saveSimpleFormat",
                                   ErrorCode::FILE_WRITE_FAILED,
                                   __FILE__, __LINE__, __func__);
    }
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
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
}

bool Config::validate() const {
    ErrorContext ctx("validate", "");
    ctx.addMetadata("operation", "validate");
    ctx.addMetadata("total_entries", std::to_string(pImpl->entries.size()));
    
    try {
        NLM_LOG_INFO("Validating configuration with " + std::to_string(pImpl->entries.size()) + " entries");
        
        std::vector<std::string> errors;
        
        // Check required keys
        for (const auto& requiredKey : pImpl->requiredKeys) {
            if (!has(requiredKey)) {
                errors.push_back("Required configuration key missing: " + requiredKey);
            }
        }
        
        // Check forbidden keys
        for (const auto& forbiddenKey : pImpl->forbiddenKeys) {
            if (has(forbiddenKey)) {
                errors.push_back("Forbidden configuration key present: " + forbiddenKey);
            }
        }
        
        // Validate individual keys against rules
        for (const auto& entry : pImpl->entries) {
            auto ruleIt = pImpl->validationRules.find(entry.key);
            if (ruleIt != pImpl->validationRules.end()) {
                if (!ruleIt->second(entry.value)) {
                    errors.push_back("Invalid value for configuration key '" + entry.key + "'");
                }
            }
        }
        
        if (!errors.empty()) {
            ctx.addMetadata("validation_errors", std::to_string(errors.size()));
            NLM_LOG_WARNING("Configuration validation found " + std::to_string(errors.size()) + " issues");
            for (const auto& error : errors) {
                NLM_LOG_ERROR("Validation error: " + error);
            }
            return false;
        }
        
        NLM_LOG_INFO("Configuration validation passed");
        return true;
        
    } catch (const ConfigurationException& e) {
        NLM_LOG_ERROR("ConfigurationException during validation: " + e.what());
        ErrorReporterRegistry::reportError(e);
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error during configuration validation: " + e.what());
        throw ConfigurationException("Configuration validation failed: " + e.what(),
                                   ctx.getMetadataSummary(),
                                   ErrorCode::VALIDATION_FAILED,
                                   __FILE__, __LINE__, __func__);
    }
}

void Config::setRequiredKey(const std::string& key) {
    pImpl->requiredKeys.insert(key);
}

void Config::setForbiddenKey(const std::string& key) {
    pImpl->forbiddenKeys.insert(key);
}

void Config::addValidationRule(const std::string& key, std::function<bool(const ConfigValue&)> rule) {
    pImpl->validationRules[key] = rule;
}

std::string Config::toDict() const {
    std::ostringstream oss;
    oss << "{\n";
    
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        oss << "  \"" << entry.key << "\": ";
        
        bool serialized = false;
        if (const int* v = std::get_if<int>(&entry.value)) {
            oss << *v;
            serialized = true;
        } else if (const int64_t* v = std::get_if<int64_t>(&entry.value)) {
            oss << *v;
            serialized = true;
        } else if (const double* v = std::get_if<double>(&entry.value)) {
            oss << *v;
            serialized = true;
        } else if (const float* v = std::get_if<float>(&entry.value)) {
            oss << *v;
            serialized = true;
        } else if (const bool* v = std::get_if<bool>(&entry.value)) {
            oss << (*v ? "true" : "false");
            serialized = true;
        } else if (const std::string* v = std::get_if<std::string>(&entry.value)) {
            oss << "\"" << escapeJSON(*v) << "\"";
            serialized = true;
        }
        
        if (!serialized) {
            oss << "null";
        }
        
        if (i < pImpl->entries.size() - 1) {
            oss << ",\n";
        } else {
            oss << "\n";
        }
    }
    
    oss << "}\n";
    return oss.str();
}

std::string Config::escapeJSON(const std::string& str) {
    std::string escaped;
    for (char c : str) {
        switch (c) {
            case '\\': escaped += "\\\\"; break;
            case '\"': escaped += "\\\""; break;
            case '\b': escaped += "\\b"; break;
            case '\f': escaped += "\\f"; break;
            case '\n': escaped += "\\n"; break;
            case '\r': escaped += "\\r"; break;
            case '\t': escaped += "\\t"; break;
            default:
                if (c < 0x20) {
                    escaped += "\\u" + formatHex(static_cast<unsigned int>(c));
                } else {
                    escaped += c;
                }
                break;
        }
    }
    return escaped;
}

std::string Config::formatHex(unsigned int value) {
    std::stringstream ss;
    ss << std::hex << std::setw(4) << std::setfill('0') << value;
    return ss.str();
}

std::map<std::string, std::string> Config::getStringDict() const {
    std::map<std::string, std::string> dict;
    for (const auto& entry : pImpl->entries) {
        if (const std::string* v = std::get_if<std::string>(&entry.value)) {
            dict[entry.key] = *v;
        }
    }
    return dict;
}

void Config::updateFromDict(const std::map<std::string, std::string>& dict) {
    for (const auto& pair : dict) {
        set(pair.first, pair.second, ConfigSource::Runtime);
    }
}

ConfigValue Config::parseJSONValue(const std::string& jsonValue) {
    if (jsonValue.empty()) {
        return std::string("");
    }
    
    char firstChar = jsonValue[0];
    
    if (firstChar == '"') {
        // String value
        std::string str = jsonValue.substr(1, jsonValue.length() - 2);
        return str;
    } else if (firstChar == 't' || firstChar == 'f') {
        // Boolean value
        std::string boolStr = jsonValue.substr(0, 5);
        if (boolStr == "true") {
            return true;
        } else if (boolStr == "false") {
            return false;
        }
    } else if (firstChar == 'n') {
        // null value
        return std::string("") ;
    } else if (firstChar == '-' || (firstChar >= '0' && firstChar <= '9')) {
        // Number - try double first
        try {
            size_t pos;
            double doubleVal = std::stod(jsonValue, &pos);
            if (pos == jsonValue.length()) {
                return doubleVal;
            }
        } catch (...) {}
        
        try {
            size_t pos;
            int intVal = std::stoi(jsonValue, &pos);
            if (pos == jsonValue.length()) {
                return intVal;
            }
        } catch (...) {}
        
        // If all fails, return as string
        return jsonValue;
    } else if (firstChar == '[') {
        // Array - simplified parsing
        std::vector<double> array;
        std::string numStr;
        bool inQuotes = false;
        
        for (size_t i = 1; i < jsonValue.length() - 1; ++i) {
            char c = jsonValue[i];
            if (c == '"') {
                inQuotes = !inQuotes;
            } else if (!inQuotes && c == ',') {
                if (!numStr.empty()) {
                    try {
                        double val = std::stod(numStr);
                        array.push_back(val);
                    } catch (...) {}
                    numStr.clear();
                }
            } else {
                numStr += c;
            }
        }
        
        if (!numStr.empty()) {
            try {
                double val = std::stod(numStr);
                array.push_back(val);
            } catch (...) {}
        }
        
        return array;
    }
    
    // Default to string
    return jsonValue;
}

std::string Config::getTypeName(const ConfigValue& value) const {
    if (std::holds_alternative<int>(value)) return "int";
    if (std::holds_alternative<int64_t>(value)) return "int64_t";
    if (std::holds_alternative<double>(value)) return "double";
    if (std::holds_alternative<float>(value)) return "float";
    if (std::holds_alternative<bool>(value)) return "bool";
    if (std::holds_alternative<std::string>(value)) return "string";
    if (std::holds_alternative<std::vector<int>>(value)) return "vector<int>";
    if (std::holds_alternative<std::vector<double>>(value)) return "vector<double>";
    if (std::holds_alternative<std::vector<std::string>>(value)) return "vector<string>";
    return "unknown";
}

std::string Config::toDetailedString() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "  Key: " << entry.key << "\n";
        oss << "    Type: " << getTypeName(entry.value) << "\n";
        oss << "    Value: ";
        
        bool printed = false;
        if (const int* v = std::get_if<int>(&entry.value)) {
            oss << *v;
            printed = true;
        } else if (const int64_t* v = std::get_if<int64_t>(&entry.value)) {
            oss << *v;
            printed = true;
        } else if (const double* v = std::get_if<double>(&entry.value)) {
            oss << *v;
            printed = true;
        } else if (const float* v = std::get_if<float>(&entry.value)) {
            oss << *v;
            printed = true;
        } else if (const bool* v = std::get_if<bool>(&entry.value)) {
            oss << (*v ? "true" : "false");
            printed = true;
        } else if (const std::string* v = std::get_if<std::string>(&entry.value)) {
            oss << "\"" << *v << "\"";
            printed = true;
        }
        
        if (!printed) {
            oss << "[unsupported type]";
        }
        
        oss << "\n    Source: " << static_cast<int>(entry.source) << "\n";
        oss << "    Description: " << entry.description << "\n";
    }
    
    return oss.str();
}

std::string Config::summary() const {
    return toDetailedString();
}

} // namespace nlm
