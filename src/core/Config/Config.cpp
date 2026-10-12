#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::shared_ptr<spdlog::logger> logger;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize logger
    pImpl->logger = spdlog::get("nlm");
    if (!pImpl->logger) {
        pImpl->logger = spdlog::stdout_color_mt("nlm");
        pImpl->logger->set_level(spdlog::level::info);
    }
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    namespace fs = std::filesystem;
    
    // Validate filepath
    if (filepath.empty()) {
        NLM_LOG_ERROR("Empty filepath provided");
        return false;
    }
    
    fs::path path(filepath);
    
    // Check if file exists
    if (!fs::exists(path)) {
        NLM_LOG_WARN("Configuration file does not exist: " + filepath);
        return false;
    }
    
    // Check if it's a file
    if (!fs::is_regular_file(path)) {
        NLM_LOG_ERROR("Path is not a regular file: " + filepath);
        return false;
    }
    
    try {
        // Try to parse as JSON first
        std::ifstream file(filepath);
        if (file.is_open()) {
            nlohmann::json jsonData;
            file >> jsonData;
            
            // Convert JSON to config entries
            for (auto& [key, value] : jsonData.items()) {
                ConfigValue configValue;
                
                if (value.is_number_integer()) {
                    configValue = value.get<int64_t>();
                } else if (value.is_number_unsigned()) {
                    configValue = value.get<uint64_t>();
                } else if (value.is_number()) {
                    configValue = value.get<double>();
                } else if (value.is_boolean()) {
                    configValue = value.get<bool>();
                } else if (value.is_string()) {
                    configValue = value.get<std::string>();
                } else if (value.is_array()) {
                    // Handle arrays
                    if (value.empty()) {
                        continue;
                    }
                    
                    // Check array element type consistency
                    bool allInts = std::all_of(value.begin(), value.end(), 
                                             [](const auto& v) { return v.is_number_integer(); });
                    bool allDoubles = std::all_of(value.begin(), value.end(), 
                                                [](const auto& v) { return v.is_number(); });
                    bool allStrings = std::all_of(value.begin(), value.end(), 
                                                [](const auto& v) { return v.is_string(); });
                    
                    if (allInts) {
                        std::vector<int64_t> vec;
                        vec.reserve(value.size());
                        for (const auto& v : value) {
                            vec.push_back(v.get<int64_t>());
                        }
                        configValue = vec;
                    } else if (allDoubles) {
                        std::vector<double> vec;
                        vec.reserve(value.size());
                        for (const auto& v : value) {
                            vec.push_back(v.get<double>());
                        }
                        configValue = vec;
                    } else if (allStrings) {
                        std::vector<std::string> vec;
                        vec.reserve(value.size());
                        for (const auto& v : value) {
                            vec.push_back(v.get<std::string>());
                        }
                        configValue = vec;
                    } else {
                        NLM_LOG_WARN("Skipping array with mixed types for key: " + key);
                        continue;
                    }
                } else {
                    NLM_LOG_WARN("Unsupported JSON value type for key: " + key);
                    continue;
                }
                
                set(key, configValue, ConfigSource::File);
            }
            
            NLM_LOG_INFO("Successfully loaded configuration from JSON file: " + filepath);
            return true;
        }
        
        // Fallback to legacy key=value format
        std::ifstream legacyFile(filepath);
        if (!legacyFile.is_open()) {
            NLM_LOG_ERROR("Cannot open configuration file: " + filepath);
            return false;
        }
        
        return loadLegacyConfig(legacyFile, filepath);
        
    } catch (const nlohmann::json::exception& e) {
        NLM_LOG_ERROR("JSON parsing error in " + filepath + ": " + std::string(e.what()));
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading configuration from " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    // Enhanced CLI argument parsing
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format with JSON-like syntax
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::stringnpos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Parse value based on type hints
                setConfigValueFromString(key, value, ConfigSource::CommandLine);
            } else {
                // Handle boolean flags
                std::string key = arg.substr(2);
                set(key, true, ConfigSource::CommandLine);
            }
        } 
        // Handle -key value format
        else if (arg[0] == '-') {
            if (i + 1 < argc) {
                std::string key = arg.substr(1);
                std::string value = argv[++i];
                setConfigValueFromString(key, value, ConfigSource::CommandLine);
            } else {
                NLM_LOG_WARN("Missing value for argument: " + arg);
            }
        }
    }
    
    return true;
}

void Config::setConfigValueFromString(const std::string& key, const std::string& value, ConfigSource source) {
    // Try to parse as boolean
    if (value == "true" || value == "1" || value == "yes" || value == "on") {
        set(key, true, source);
        return;
    } else if (value == "false" || value == "0" || value == "no" || value == "off") {
        set(key, false, source);
        return;
    }
    
    // Try to parse as integer
    try {
        size_t pos;
        int64_t intVal = std::stoll(value, &pos);
        if (pos == value.length()) {
            set(key, intVal, source);
            return;
        }
    } catch (...) {}
    
    // Try to parse as double
    try {
        size_t pos;
        double doubleVal = std::stod(value, &pos);
        if (pos == value.length()) {
            set(key, doubleVal, source);
            return;
        }
    } catch (...) {}
    
    // Check for array format [1,2,3] or ["a","b","c"]
    if (value.size() >= 2 && value.front() == '[' && value.back() == ']') {
        // Parse array
        std::string inner = value.substr(1, value.length() - 2);
        std::vector<std::string> tokens;
        std::stringstream ss(inner);
        std::string token;
        
        while (std::getline(ss, token, ',')) {
            tokens.push_back(trim(token));
        }
        
        if (!tokens.empty()) {
            // Try to determine array type
            bool allInts = std::all_of(tokens.begin(), tokens.end(), 
                                     [](const std::string& t) {
                                         try { std::stoll(t); return true; } 
                                         catch (...) { return false; }
                                     });
            
            bool allDoubles = std::all_of(tokens.begin(), tokens.end(), 
                                        [](const std::string& t) {
                                            try { std::stod(t); return true; } 
                                            catch (...) { return false; }
                                        });
            
            if (allInts) {
                std::vector<int64_t> intArray;
                for (const auto& token : tokens) {
                    intArray.push_back(std::stoll(trim(token)));
                }
                set(key, intArray, source);
                return;
            } else if (allDoubles) {
                std::vector<double> doubleArray;
                for (const auto& token : tokens) {
                    doubleArray.push_back(std::stod(trim(token)));
                }
                set(key, doubleArray, source);
                return;
            } else {
                // Assume string array
                set(key, tokens, source);
                return;
            }
        }
    }
    
    // Default to string
    set(key, value, source);
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open file for writing: " + filepath);
        return false;
    }
    
    try {
        // Save in JSON format for better readability and tool compatibility
        nlohmann::json jsonData;
        
        for (const auto& entry : pImpl->entries) {
            // Store value with description as comment
            jsonData[entry.key] = std::visit([](auto&& arg) -> nlohmann::json {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    return arg;
                } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
                    return arg;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    return arg;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    return arg;
                } else {
                    return arg;
                }
            }, entry.value);
        }
        
        // Pretty print JSON
        file << jsonData.dump(2);
        file << std::endl;
        
        NLM_LOG_INFO("Successfully saved configuration to JSON file: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving configuration to " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate key
    if (key.empty()) {
        NLM_LOG_ERROR("Cannot set empty configuration key");
        return;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        if (pImpl->logger) {
            pImpl->logger->info("Updated config '{}' to type: {}", key, getTypeName(value));
        }
    } else {
        pImpl->entries.emplace_back(key, value, source);
        if (pImpl->logger) {
            pImpl->logger->info("Added new config '{}' of type: {}", key, getTypeName(value));
        }
    }
}

std::string Config::getTypeName(const ConfigValue& value) const {
    return std::visit([](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) return "int";
        else if constexpr (std::is_same_v<T, int64_t>) return "int64_t";
        else if constexpr (std::is_same_v<T, uint64_t>) return "uint64_t";
        else if constexpr (std::is_same_v<T, double>) return "double";
        else if constexpr (std::is_same_v<T, bool>) return "bool";
        else if constexpr (std::is_same_v<T, std::string>) return "string";
        else if constexpr (std::is_same_v<T, std::vector<int64_t>>) return "int_array";
        else if constexpr (std::is_same_v<T, std::vector<double>>) return "double_array";
        else if constexpr (std::is_same_v<T, std::vector<std::string>>) return "string_array";
        else return "unknown";
    }, value);
}

bool Config::loadLegacyConfig(std::ifstream& file, const std::string& filepath) {
    std::string line;
    int lineNumber = 0;
    int parsedLines = 0;
    
    while (std::getline(file, line)) {
        ++lineNumber;
        
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            setConfigValueFromString(key, value, ConfigSource::File);
            ++parsedLines;
        } else {
            NLM_LOG_WARN("Skipping invalid line in " + filepath + ":" + std::to_string(lineNumber));
        }
    }
    
    NLM_LOG_INFO("Parsed " + std::to_string(parsedLines) + " config entries from " + filepath);
    return parsedLines > 0;
}

// Explicit template instantiations
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
