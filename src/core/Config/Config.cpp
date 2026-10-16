#include "Config.hpp"
#include "../Error/Error.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <cmath>
#include <system_error>

// Include nlohmann/json
#ifdef _WIN32
    #include <windows.h>
#endif

// Include nlohmann/json header - use the actual nlohmann/json
namespace nlohmann {
    class json;
}

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    if (!ErrorHelper::validateFilePath(filepath)) {
        NLM_THROW_RECOVERABLE_ERROR(FileError, true,
            "Invalid file path: " + filepath, filepath);
        return false;
    }
    return loadFromFileInternal(filepath);
}

bool Config::loadFromFileInternal(const std::filesystem::path& filepath) {
    if (!ErrorHelper::validateFilePath(filepath.string())) {
        NLM_THROW_RECOVERABLE_ERROR(FileError, true,
            "Invalid file path: " + filepath.string(), filepath.string());
        return false;
    }
    
    std::string format = getFileFormat(filepath.string());
    
    try {
        if (format == "json" || format.empty()) {
            std::ifstream file(filepath);
            if (!file.is_open()) {
                NLM_THROW_RECOVERABLE_ERROR(FileError, true,
                    "Failed to open file: " + filepath.string(), filepath.string());
                return false;
            }
            
            nlohmann::json jsonData;
            try {
                file >> jsonData;
            } catch (const std::exception& e) {
                NLM_THROW_RECOVERABLE_ERROR(FileError, false,
                    "JSON parsing error: " + std::string(e.what()), filepath.string());
                return false;
            }
            return loadFromJson(jsonData);
        }
        else if (format == "yaml" || format == "yml") {
            std::ifstream file(filepath);
            if (!file.is_open()) {
                NLM_THROW_RECOVERABLE_ERROR(FileError, true,
                    "Failed to open file: " + filepath.string(), filepath.string());
                return false;
            }
            
            std::stringstream buffer;
            buffer << file.rdbuf();
            return loadFromYaml(buffer.str());
        }
        else if (format == "cfg") {
            // Fallback to original .cfg parser
            return loadFromFile(filepath.string());
        }
        else {
            NLM_THROW_RECOVERABLE_ERROR(FileError, false,
                "Unsupported file format: " + format, filepath.string());
            return false;
        }
    }
    catch (const std::bad_alloc&) {
        NLM_THROW_RECOVERABLE_ERROR(MemoryError, true,
            "Memory allocation failed while parsing: " + filepath.string(), filepath.string());
        return false;
    }
    catch (const std::exception& e) {
        NLM_THROW_RECOVERABLE_ERROR(FileError, false,
            "Failed to parse " + format + " file: " + e.what(), filepath.string());
        return false;
    }

bool Config::loadFromJsonString(const std::string& jsonString) {
    if (jsonString.empty()) {
        NLM_THROW_RECOVERABLE_ERROR(ValidationError, true,
            "Empty JSON string provided", "");
        return false;
    }
    
    try {
        auto jsonData = nlohmann::json::parse(jsonString);
        return loadFromJson(jsonData);
    }
    catch (const std::bad_alloc&) {
        NLM_THROW_RECOVERABLE_ERROR(MemoryError, true,
            "Memory allocation failed while parsing JSON string", "");
        return false;
    }
    catch (const std::exception& e) {
        NLM_THROW_RECOVERABLE_ERROR(FileError, false,
            "Failed to parse JSON string: " + std::string(e.what()), "");
        return false;
    }
}

bool Config::loadFromYaml(const std::string& yamlString) {
    if (yamlString.empty()) {
        NLM_THROW_RECOVERABLE_ERROR(ValidationError, true,
            "Empty YAML string provided", "");
        return false;
    }
    
    try {
        // Try to parse as JSON first
        auto jsonData = nlohmann::json::parse(yamlString);
        return loadFromJson(jsonData);
    }
    catch (const std::bad_alloc&) {
        NLM_THROW_RECOVERABLE_ERROR(MemoryError, true,
            "Memory allocation failed while parsing YAML/JSON string", "");
        return false;
    }
    catch (const std::exception& e) {
        NLM_THROW_RECOVERABLE_ERROR(FileError, false,
            "Failed to parse YAML/JSON string: " + std::string(e.what()), "");
        return false;
    }
}

bool Config::loadFromJson(const nlohmann::json& jsonData) {
    try {
        if (!jsonData.is_object()) {
            NLM_THROW_RECOVERABLE_ERROR(ValidationError, false,
                "JSON data must be an object", "");
            return false;
        }
        
        for (const auto& [key, value] : jsonData.items()) {
            try {
                set(key, jsonValueToConfigValue(value), ConfigSource::File);
            } catch (const NLMException& e) {
                // Log warning but continue with other entries
                NLM_THROW_RECOVERABLE_ERROR(ValidationError, true,
                    "Failed to load configuration entry '" + key + "': " + e.what(), key);
                // Continue processing other entries
            }
        }
        return true;
    }
    catch (const std::bad_alloc&) {
        NLM_THROW_RECOVERABLE_ERROR(MemoryError, true,
            "Memory allocation failed while loading JSON data", "");
        return false;
    }
    catch (const std::exception& e) {
        NLM_THROW_RECOVERABLE_ERROR(FileError, false,
            "Failed to load JSON data: " + std::string(e.what()), "");
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    if (!argv || argc <= 0) {
        NLM_THROW_RECOVERABLE_ERROR(ValidationError, false,
            "Invalid arguments: argc must be positive and argv non-null", "");
        return false;
    }
    
    for (int i = 1; i < argc; ++i) {
        if (!argv[i]) {
            NLM_THROW_RECOVERABLE_ERROR(ValidationError, true,
                "Null argument pointer at index " + std::to_string(i), std::to_string(i));
            continue; // Skip this argument
        }
        
        std::string arg(argv[i]);
        
        try {
            // Handle --key=value format
            if (arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    
                    if (key.empty()) {
                        NLM_THROW_RECOVERABLE_ERROR(ValidationError, true,
                            "Empty key in command line argument: " + arg, arg);
                        continue;
                    }
                    
                    set(key, value, ConfigSource::CommandLine);
                } else {
                    NLM_THROW_RECOVERABLE_ERROR(ValidationError, true,
                        "Invalid command line argument format: " + arg, arg);
                }
            }
            // Handle -key value format
            else if (arg[0] == '-' && i + 1 < argc && argv[i + 1]) {
                std::string key = arg.substr(1);
                std::string value = argv[++i];
                
                if (key.empty()) {
                    NLM_THROW_RECOVERABLE_ERROR(ValidationError, true,
                        "Empty key in command line argument: " + arg, arg);
                    continue;
                }
                
                set(key, value, ConfigSource::CommandLine);
            }
            else {
                NLM_THROW_RECOVERABLE_ERROR(ValidationError, true,
                    "Unrecognized command line argument: " + arg, arg);
            }
        }
        catch (const std::bad_alloc&) {
            NLM_THROW_RECOVERABLE_ERROR(MemoryError, true,
                "Memory allocation failed while parsing argument: " + arg, arg);
            // Continue with next argument
        }
        catch (const std::exception& e) {
            NLM_THROW_RECOVERABLE_ERROR(SystemError, false,
                "Error parsing command line argument '" + arg + "': " + e.what(), arg);
            // Continue with next argument
        }
    }
    return true;
}

bool Config::loadFromJson(const nlohmann::json& jsonData) {
    try {
        if (!jsonData.is_object()) {
            NLM_THROW_RECOVERABLE_ERROR(ValidationError, false,
                "JSON data must be an object", "");
            return false;
        }
        
        for (const auto& [key, value] : jsonData.items()) {
            try {
                set(key, jsonValueToConfigValue(value), ConfigSource::File);
            } catch (const NLMException& e) {
                // Log warning but continue with other entries
                NLM_THROW_RECOVERABLE_ERROR(ValidationError, true,
                    "Failed to load configuration entry '" + key + "': " + e.what(), key);
                // Continue processing other entries
            }
        }
        return true;
    }
    catch (const std::bad_alloc&) {
        NLM_THROW_RECOVERABLE_ERROR(MemoryError, true,
            "Memory allocation failed while loading JSON data", "");
        return false;
    }
    catch (const std::exception& e) {
        NLM_THROW_RECOVERABLE_ERROR(FileError, false,
            "Failed to load JSON data: " + std::string(e.what()), "");
        return false;
    }
}

ConfigValue Config::jsonValueToConfigValue(const nlohmann::json& jsonValue) {
    try {
        if (jsonValue.is_null()) {
            NLM_THROW_RECOVERABLE_ERROR(ValidationError, false,
                "JSON null value cannot be converted to ConfigValue", "");
        }
        else if (jsonValue.is_string()) {
            return jsonValue.get<std::string>();
        }
        else if (jsonValue.is_number_integer()) {
            return jsonValue.get<int>();
        }
        else if (jsonValue.is_number_float()) {
            return jsonValue.get<double>();
        }
        else if (jsonValue.is_boolean()) {
            return jsonValue.get<bool>();
        }
        else if (jsonValue.is_array()) {
            // Handle arrays
            if (jsonValue.empty()) {
                return std::vector<std::string>{}; // Default to empty string vector
            }
            
            // Try to infer array type from first element
            const auto& firstElement = jsonValue[0];
            if (firstElement.is_number_integer()) {
                std::vector<int> intArray;
                intArray.reserve(jsonValue.size());
                for (const auto& element : jsonValue) {
                    if (!element.is_number_integer()) {
                        NLM_THROW_RECOVERABLE_ERROR(ValidationError, false,
                            "Inconsistent array types: expected integers", "");
                    }
                    intArray.push_back(element.get<int>());
                }
                return intArray;
            }
            else if (firstElement.is_number_float()) {
                std::vector<double> doubleArray;
                doubleArray.reserve(jsonValue.size());
                for (const auto& element : jsonValue) {
                    if (!element.is_number_float()) {
                        NLM_THROW_RECOVERABLE_ERROR(ValidationError, false,
                            "Inconsistent array types: expected floats", "");
                    }
                    doubleArray.push_back(element.get<double>());
                }
                return doubleArray;
            }
            else if (firstElement.is_string()) {
                std::vector<std::string> stringArray;
                stringArray.reserve(jsonValue.size());
                for (const auto& element : jsonValue) {
                    if (!element.is_string()) {
                        NLM_THROW_RECOVERABLE_ERROR(ValidationError, false,
                            "Inconsistent array types: expected strings", "");
                    }
                    stringArray.push_back(element.get<std::string>());
                }
                return stringArray;
            }
            else {
                NLM_THROW_RECOVERABLE_ERROR(ValidationError, false,
                    "Unsupported array element type in JSON", "");
            }
        }
        else if (jsonValue.is_object()) {
            // Handle nested objects - convert to JSON string for now
            // TODO: Implement proper nested object support
            return jsonValue.dump();
        }
        else {
            NLM_THROW_RECOVERABLE_ERROR(ValidationError, false,
                "Unsupported JSON value type", "");
        }
    }
    catch (const std::bad_alloc&) {
        NLM_THROW_RECOVERABLE_ERROR(MemoryError, true,
            "Memory allocation failed while converting JSON to ConfigValue", "");
    }
    catch (const std::exception& e) {
        NLM_THROW_RECOVERABLE_ERROR(ValidationError, false,
            "Error converting JSON to ConfigValue: " + std::string(e.what()), "");
    }
}

nlohmann::json Config::configValueToJson(const ConfigValue& value) {
    try {
        return std::visit([](auto&& arg) -> nlohmann::json {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                return arg;
            }
            else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                return arg;
            }
            else if constexpr (std::is_same_v<T, double>) {
                return arg;
            }
            else if constexpr (std::is_same_v<T, bool>) {
                return arg;
            }
            else if constexpr (std::is_same_v<T, std::vector<int>>) {
                nlohmann::json jsonArray = nlohmann::json::array();
                for (const auto& item : arg) {
                    jsonArray.push_back(item);
                }
                return jsonArray;
            }
            else if constexpr (std::is_same_v<T, std::vector<double>>) {
                nlohmann::json jsonArray = nlohmann::json::array();
                for (const auto& item : arg) {
                    jsonArray.push_back(item);
                }
                return jsonArray;
            }
            else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                nlohmann::json jsonArray = nlohmann::json::array();
                for (const auto& item : arg) {
                    jsonArray.push_back(item);
                }
                return jsonArray;
            }
            else {
                // Fallback for unsupported types
                return arg;
            }
        }, value);
    }
    catch (const std::bad_alloc&) {
        NLM_THROW_RECOVERABLE_ERROR(MemoryError, true,
            "Memory allocation failed while converting ConfigValue to JSON", "");
    }
    catch (const std::exception& e) {
        NLM_THROW_RECOVERABLE_ERROR(SystemError, false,
            "Failed to convert ConfigValue to JSON: " + std::string(e.what()), "");
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    if (!ErrorHelper::validateFilePath(filepath)) {
        NLM_THROW_RECOVERABLE_ERROR(FileError, true,
            "Invalid file path for saving: " + filepath, filepath);
        return false;
    }
    
    std::string format = getFileFormat(filepath);
    
    try {
        if (format == "json") {
            nlohmann::json jsonData;
            for (const auto& entry : pImpl->entries) {
                try {
                    jsonData[entry.key] = configValueToJson(entry.value);
                } catch (const NLMException& e) {
                    NLM_THROW_RECOVERABLE_ERROR(SystemError, true,
                        "Failed to convert configuration entry '" + entry.key + "' to JSON: " + e.what(), entry.key);
                    return false;
                }
            }
            
            std::ofstream file(filepath);
            if (!file.is_open()) {
                NLM_THROW_RECOVERABLE_ERROR(FileError, true,
                    "Failed to open file for writing: " + filepath, filepath);
                return false;
            }
            
            try {
                file << jsonData.dump(4); // Pretty print with 4-space indentation
            } catch (const std::exception& e) {
                NLM_THROW_RECOVERABLE_ERROR(FileError, true,
                    "Failed to write JSON data to file: " + std::string(e.what()), filepath);
                return false;
            }
            return true;
        }
        else if (format == "yaml" || format == "yml") {
            std::string yamlString = saveToYamlString();
            if (yamlString.empty()) {
                NLM_THROW_RECOVERABLE_ERROR(SystemError, false,
                    "YAML saving not yet implemented - yaml-cpp not available", filepath);
                return false;
            }
            
            std::ofstream file(filepath);
            if (!file.is_open()) {
                NLM_THROW_RECOVERABLE_ERROR(FileError, true,
                    "Failed to open file for writing: " + filepath, filepath);
                return false;
            }
            
            try {
                file << yamlString;
            } catch (const std::exception& e) {
                NLM_THROW_RECOVERABLE_ERROR(FileError, true,
                    "Failed to write YAML data to file: " + std::string(e.what()), filepath);
                return false;
            }
            return true;
        }
        else {
            // Fallback to .cfg format
            std::ofstream file(filepath);
            if (!file.is_open()) {
                NLM_THROW_RECOVERABLE_ERROR(FileError, true,
                    "Failed to open file for writing: " + filepath, filepath);
                return false;
            }
            
            for (const auto& entry : pImpl->entries) {
                try {
                    file << "# " << entry.description << "\n";
                    std::visit([&file](auto&& arg) {
                        using T = std::decay_t<decltype(arg)>;
                        if constexpr (std::is_same_v<T, std::string>) {
                            file << entry.key << " = \"" << arg << "\"\n";
                        }
                        else {
                            file << entry.key << " = " << arg << "\n";
                        }
                    }, entry.value);
                } catch (const std::exception& e) {
                    NLM_THROW_RECOVERABLE_ERROR(SystemError, true,
                        "Failed to write configuration entry '" + entry.key + "' to file: " + std::string(e.what()), filepath);
                    return false;
                }
            }
            return true;
        }
    }
    catch (const std::bad_alloc&) {
        NLM_THROW_RECOVERABLE_ERROR(MemoryError, true,
            "Memory allocation failed while saving to file: " + filepath, filepath);
        return false;
    }
    catch (const std::exception& e) {
        NLM_THROW_RECOVERABLE_ERROR(FileError, false,
            "Failed to save to file: " + std::string(e.what()), filepath);
        return false;
    }
}

std::string Config::saveToJsonString() const {
    nlohmann::json jsonData;
    for (const auto& entry : pImpl->entries) {
        jsonData[entry.key] = configValueToJson(entry.value);
    }
    return jsonData.dump(4);
}

std::string Config::saveToYamlString() const {
    try {
        // TODO: Implement YAML serialization with yaml-cpp if available
        // For now, throw an error that indicates what's needed
        NLM_THROW_RECOVERABLE_ERROR(SystemError, false,
            "YAML saving not yet implemented - yaml-cpp not available", "");
    }
    catch (const NLMException&) {
        throw; // Re-throw NLM exceptions
    }
    catch (const std::exception& e) {
        NLM_THROW_RECOVERABLE_ERROR(SystemError, false,
            "Failed to convert configuration to YAML: " + std::string(e.what()), "");
    }
}

std::string Config::getFileFormat(const std::string& filepath) {
    try {
        return getFileFormat(std::filesystem::path(filepath));
    }
    catch (const std::exception& e) {
        NLM_THROW_RECOVERABLE_ERROR(ValidationError, false,
            "Failed to get file format for path: " + std::string(e.what()), filepath);
    }
}

std::string Config::getFileFormat(const std::filesystem::path& filepath) {
    try {
        std::string extension = filepath.extension().string();
        if (extension.empty()) {
            return "";
        }
        
        // Remove the leading dot and convert to lowercase
        extension = extension.substr(1);
        if (extension.empty()) {
            return "";
        }
        
        // Validate extension characters
        for (char c : extension) {
            if (!std::isalnum(c) && c != '.' && c != '-') {
                NLM_THROW_RECOVERABLE_ERROR(ValidationError, false,
                    "Invalid file extension characters: " + extension, extension);
                return "";
            }
        }
        
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
        
        return extension;
    }
    catch (const std::exception& e) {
        NLM_THROW_RECOVERABLE_ERROR(SystemError, false,
            "Failed to extract file format: " + std::string(e.what()), filepath.string());
        return "";
    }
}

void Config::throwParsingError(const std::string& message, const std::string& context) {
    try {
        std::string fullMessage = message;
        if (!context.empty()) {
            fullMessage = "Error parsing '" + context + "': " + message;
        }
        throw std::runtime_error(fullMessage);
    }
    catch (const std::exception& e) {
        NLM_THROW_RECOVERABLE_ERROR(SystemError, false,
            "Failed to generate parsing error: " + std::string(e.what()), context);
    }
}
