// Improved Config.cpp with JSON/YAML parser implementation
// Replaces placeholder with full-featured configuration management

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <map>
#include <variant>
#include <vector>
#include <string>
#include <cctype>
#include <stdexcept>
#include <nlohmann/json.hpp>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, std::vector<std::string>> nestedKeys;
    bool parseErrors;
    
    Impl() : parseErrors(false) {}
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&& other) noexcept : pImpl(std::move(other.pImpl)) {}

Config& Config::operator=(Config&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool Config::loadFromFile(const std::string& filepath) {
    namespace fs = std::filesystem;
    
    // Check if file exists
    if (!fs::exists(filepath)) {
        NLM_LOG_ERROR("Configuration file not found: " + filepath);
        return false;
    }
    
    // Determine file type based on extension
    std::string ext = "." + fs::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    try {
        if (ext == ".json") {
            return loadFromJSON(filepath);
        } else if (ext == ".yaml" || ext == ".yml") {
            return loadFromYAML(filepath);
        } else if (ext == ".ini" || ext == ".cfg") {
            return loadFromINI(filepath);
        } else {
            // Try to auto-detect format
            return loadFromJSON(filepath);  // Try JSON first
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to load configuration file " + filepath + ": " + std::string(e.what()));
        pImpl->parseErrors = true;
        return false;
    }
}

bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open JSON configuration file: " + filepath);
        return false;
    }
    
    try {
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Clear existing entries
        pImpl->entries.clear();
        pImpl->nestedKeys.clear();
        
        // Convert JSON to Config entries
        flattenJSON(jsonData, "", ConfigSource::File);
        
        NLM_LOG_INFO("Loaded JSON configuration from: " + filepath);
        return true;
        
    } catch (const nlohmann::json::parse_error& e) {
        NLM_LOG_ERROR("JSON parse error in " + filepath + ": " + std::string(e.what()));
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error reading JSON configuration: " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromYAML(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open YAML configuration file: " + filepath);
        return false;
    }
    
    try {
        // Try to use YAML library if available
        #ifdef HAVE_YAML
        nlohmann::json jsonData;
        try {
            // Convert YAML to JSON first (common approach)
            auto yamlData = yaml::parse(filepath);
            jsonData = nlohmann::json::parse(yamlData.dump());
        } catch (...) {
            // If YAML conversion fails, try to parse directly as JSON
            file.clear();
            file.seekg(0);
            file >> jsonData;
        }
        
        // Clear existing entries
        pImpl->entries.clear();
        pImpl->nestedKeys.clear();
        
        // Convert JSON to Config entries
        flattenJSON(jsonData, "", ConfigSource::File);
        
        NLM_LOG_INFO("Loaded YAML configuration from: " + filepath);
        return true;
        
        #else
        // YAML library not available, fallback to JSON
        NLM_LOG_WARN("YAML library not available, falling back to JSON for " + filepath);
        return loadFromJSON(filepath);
        
        #endif
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error reading YAML configuration: " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromINI(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open INI configuration file: " + filepath);
        return false;
    }
    
    try {
        // Clear existing entries
        pImpl->entries.clear();
        pImpl->nestedKeys.clear();
        
        std::string line;
        std::string currentSection = "";
        
        while (std::getline(file, line)) {
            // Trim whitespace
            line = trim(line);
            
            // Skip empty lines and comments
            if (line.empty() || line[0] == '#' || line[0] == ';') {
                continue;
            }
            
            // Check for section header
            if (line[0] == '[' && line[line.size() - 1] == ']') {
                currentSection = line.substr(1, line.size() - 2);
                continue;
            }
            
            // Parse key=value pairs
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
                
                // Add section prefix if not empty
                std::string fullKey = currentSection.empty() ? key : currentSection + "." + key;
                parseINIValue(fullKey, value);
            }
        }
        
        NLM_LOG_INFO("Loaded INI configuration from: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error reading INI configuration: " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    try {
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            // Handle --key=value format
            if (arg.find("--") == 0) {
                size_t pos = arg.find('=');
                if (pos != std::string::npos && pos > 2) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    set(key, value, ConfigSource::CommandLine);
                } else {
                    NLM_LOG_WARN("Invalid command line argument: " + arg);
                }
            }
            // Handle -key value format
            else if (arg.find("-") == 0 && i + 1 < argc) {
                std::string key = arg.substr(1);
                std::string value = argv[++i];
                set(key, value, ConfigSource::CommandLine);
            }
        }
        
        NLM_LOG_INFO("Loaded configuration from command line arguments");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error parsing command line arguments: " + std::string(e.what()));
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    namespace fs = std::filesystem;
    
    // Create directory if it doesn't exist
    fs::path path(filepath);
    if (!fs::exists(path.parent_path())) {
        if (!fs::create_directories(path.parent_path())) {
            NLM_LOG_ERROR("Cannot create configuration directory: " + path.parent_path().string());
            return false;
        }
    }
    
    // Determine file type based on extension
    std::string ext = "." + path.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    try {
        if (ext == ".json") {
            return saveToJSON(filepath);
        } else if (ext == ".yaml" || ext == ".yml") {
            return saveToYAML(filepath);
        } else {
            // Default to JSON
            NLM_LOG_WARN("Unknown file extension " + ext + ", saving as JSON");
            return saveToJSON(filepath);
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to save configuration file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::saveToJSON(const std::string& filepath) const {
    nlohmann::json jsonData;
    
    // Convert Config entries to JSON
    for (const auto& entry : pImpl->entries) {
        std::vector<std::string> path = splitKey(entry.key);
        nlohmann::json& current = jsonData;
        
        // Navigate to the right nested level
        for (size_t i = 0; i < path.size(); ++i) {
            if (i == path.size() - 1) {
                // Set the final value
                current[path[i]] = entry.value;
            } else {
                current = current[path[i]];
            }
        }
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open file for writing: " + filepath);
        return false;
    }
    
    try {
        file << std::setw(2) << jsonData;
        NLM_LOG_INFO("Saved JSON configuration to: " + filepath);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error writing JSON configuration: " + std::string(e.what()));
        return false;
    }
}

bool Config::saveToYAML(const std::string& filepath) const {
    // Try to use YAML library if available
    #ifdef HAVE_YAML
    nlohmann::json jsonData;
    
    // Convert Config entries to JSON
    for (const auto& entry : pImpl->entries) {
        std::vector<std::string> path = splitKey(entry.key);
        nlohmann::json& current = jsonData;
        
        // Navigate to the right nested level
        for (size_t i = 0; i < path.size(); ++i) {
            if (i == path.size() - 1) {
                // Set the final value
                current[path[i]] = entry.value;
            } else {
                current = current[path[i]];
            }
        }
    }
    
    try {
        // Convert JSON to YAML
        std::string yamlContent = yaml::dump(jsonData);
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Cannot open file for writing: " + filepath);
            return false;
        }
        
        file << yamlContent;
        NLM_LOG_INFO("Saved YAML configuration to: " + filepath);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error writing YAML configuration: " + std::string(e.what()));
        return false;
    }
    
    #else
    NLM_LOG_ERROR("YAML library not available for saving configuration");
    return false;
    #endif
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    try {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                               [&key](const ConfigEntry& e) { return e.key == key; });n        
        if (it == pImpl->entries.end()) {
            return std::nullopt;
        }
        
        // Try to convert to requested type
        return convertToType<T>(it->value);
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error getting configuration value for key " + key + ": " + std::string(e.what()));
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto value = get<T>(key);
    return value.has_value() ? value.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    try {
        // Remove existing entry with same key
        pImpl->entries.erase(
            std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
                          [&key](const ConfigEntry& e) { return e.key == key; }),
            pImpl->entries.end()
        );
        
        // Add new entry
        pImpl->entries.emplace_back(key, value, source);
        
        // Update nested keys index
        std::vector<std::string> path = splitKey(key);
        for (const auto& segment : path) {
            pImpl->nestedKeys[segment].push_back(key);
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error setting configuration key " + key + ": " + std::string(e.what()));
        throw;
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    ConfigValue configValue = value;
    set(key, configValue, source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    ConfigValue configValue = value;
    set(key, configValue, source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    ConfigValue configValue = value;
    set(key, configValue, source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    ConfigValue configValue = value;
    set(key, configValue, source);
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
    
    // Update nested keys index
    std::vector<std::string> path = splitKey(key);
    for (const auto& segment : path) {
        auto it = std::find(pImpl->nestedKeys[segment].begin(), 
                           pImpl->nestedKeys[segment].end(), key);
        if (it != pImpl->nestedKeys[segment].end()) {
            pImpl->nestedKeys[segment].erase(it);
        }
    }
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
    pImpl->nestedKeys.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):";
    
    for (const auto& entry : pImpl->entries) {
        oss << "\n  " << entry.key << " = [";
        oss << valueToString(entry.value) << "] (" << static_cast<int>(entry.source) << ")";
    }
    
    return oss.str();
}

void Config::flattenJSON(const nlohmann::json& json, const std::string& prefix, ConfigSource source) {
    if (json.is_object()) {
        for (auto it = json.begin(); it != json.end(); ++it) {
            std::string key = prefix.empty() ? it.key() : prefix + "." + it.key();
            flattenJSON(*it, key, source);
        }
    } else {
        // Convert JSON value to ConfigValue
        ConfigValue configValue;
        
        if (json.is_null()) {
            configValue = "null";
        } else if (json.is_boolean()) {
            configValue = json.get<bool>();
        } else if (json.is_number_integer()) {
            configValue = json.get<int64_t>();
        } else if (json.is_number_float()) {
            configValue = json.get<double>();
        } else if (json.is_string()) {
            configValue = json.get<std::string>();
        } else if (json.is_array()) {
            // Handle arrays (convert to vector)
            std::vector<double> vec;
            for (const auto& item : json) {
                if (item.is_number()) {
                    vec.push_back(item.get<double>());
                } else {
                    vec.push_back(std::stod(item.get<std::string>()));
                }
            }
            configValue = vec;
        }
        
        set(key, configValue, source, "Configuration value from file");
    }
}

void Config::parseINIValue(const std::string& key, const std::string& value) {
    // Try to infer type
    if (value == "true" || value == "false") {
        bool boolValue = (value == "true");
        set(key, boolValue, ConfigSource::File);
    } else if (std::all_of(value.begin(), value.end(), ::isdigit)) {
        int intValue = std::stoi(value);
        set(key, intValue, ConfigSource::File);
    } else if (std::all_of(value.begin(), value.end(), [](char c) { 
        return std::isdigit(c) || c == '.' || c == '-' || c == '+'; 
    })) {
        double doubleValue = std::stod(value);
        set(key, doubleValue, ConfigSource::File);
    } else {
        // String value
        set(key, value, ConfigSource::File);
    }
}

std::vector<std::string> Config::splitKey(const std::string& key) const {
    std::vector<std::string> segments;
    std::string segment;
    std::istringstream stream(key);
    
    while (std::getline(stream, segment, '.')) {
        if (!segment.empty()) {
            segments.push_back(segment);
        }
    }
    
    return segments;
}

std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) {
        return "";
    }
    
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

template<typename T>
std::optional<T> Config::convertToType(const ConfigValue& value) const {
    try {
        if constexpr (std::is_same_v<T, int>) {
            if (std::holds_alternative<int>(value)) {
                return std::get<int>(value);
            } else if (std::holds_alternative<int64_t>(value)) {
                return static_cast<int>(std::get<int64_t>(value));
            } else if (std::holds_alternative<double>(value)) {
                return static_cast<int>(std::get<double>(value));
            } else if (std::holds_alternative<bool>(value)) {
                return std::get<bool>(value) ? 1 : 0;
            } else if (std::holds_alternative<std::string>(value)) {
                return std::stoi(std::get<std::string>(value));
            }
        } else if constexpr (std::is_same_v<T, int64_t>) {
            if (std::holds_alternative<int64_t>(value)) {
                return std::get<int64_t>(value);
            } else if (std::holds_alternative<int>(value)) {
                return static_cast<int64_t>(std::get<int>(value));
            } else if (std::holds_alternative<double>(value)) {
                return static_cast<int64_t>(std::get<double>(value));
            } else if (std::holds_alternative<bool>(value)) {
                return std::get<bool>(value) ? 1 : 0;
            } else if (std::holds_alternative<std::string>(value)) {
                return std::stoll(std::get<std::string>(value));
            }
        } else if constexpr (std::is_same_v<T, double>) {
            if (std::holds_alternative<double>(value)) {
                return std::get<double>(value);
            } else if (std::holds_alternative<int>(value)) {
                return static_cast<double>(std::get<int>(value));
            } else if (std::holds_alternative<int64_t>(value)) {
                return static_cast<double>(std::get<int64_t>(value));
            } else if (std::holds_alternative<bool>(value)) {
                return std::get<bool>(value) ? 1.0 : 0.0;
            } else if (std::holds_alternative<std::string>(value)) {
                return std::stod(std::get<std::string>(value));
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            if (std::holds_alternative<bool>(value)) {
                return std::get<bool>(value);
            } else if (std::holds_alternative<int>(value)) {
                return std::get<int>(value) != 0;
            } else if (std::holds_alternative<int64_t>(value)) {
                return std::get<int64_t>(value) != 0;
            } else if (std::holds_alternative<double>(value)) {
                return std::get<double>(value) != 0.0;
            } else if (std::holds_alternative<std::string>(value)) {
                std::string str = std::get<std::string>(value);
                return str == "true" || str == "1" || str == "yes";
            }
        } else if constexpr (std::is_same_v<T, std::string>) {
            if (std::holds_alternative<std::string>(value)) {
                return std::get<std::string>(value);
            } else if (std::holds_alternative<int>(value)) {
                return std::to_string(std::get<int>(value));
            } else if (std::holds_alternative<int64_t>(value)) {
                return std::to_string(std::get<int64_t>(value));
            } else if (std::holds_alternative<double>(value)) {
                return std::to_string(std::get<double>(value));
            } else if (std::holds_alternative<bool>(value)) {
                return std::get<bool>(value) ? "true" : "false";
            }
        }
        
        return std::nullopt;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error converting configuration value to type " + std::string(typeid(T).name()) + ": " + std::string(e.what()));
        return std::nullopt;
    }
}

std::string Config::valueToString(const ConfigValue& value) const {
    std::ostringstream oss;
    
    if (std::holds_alternative<int>(value)) {
        oss << std::get<int>(value);
    } else if (std::holds_alternative<int64_t>(value)) {
        oss << std::get<int64_t>(value);
    } else if (std::holds_alternative<double>(value)) {
        oss << std::get<double>(value);
    } else if (std::holds_alternative<bool>(value)) {
        oss << (std::get<bool>(value) ? "true" : "false");
    } else if (std::holds_alternative<std::string>(value)) {
        oss << "\"" << std::get<std::string>(value) << "\"";
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        oss << "[";
        const auto& vec = std::get<std::vector<int>>(value);
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << vec[i];
        }
        oss << "]";
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        oss << "[";
        const auto& vec = std::get<std::vector<double>>(value);
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << vec[i];
        }
        oss << "]";
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        oss << "[";
        const auto& vec = std::get<std::vector<std::string>>(value);
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << "\"" << vec[i] << "\"";
        }
        oss << "]";
    } else {
        oss << "null";
    }
    
    return oss.str();
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

// JSON library initialization
void Config::initializeJSON() {
    static bool initialized = false;
    if (!initialized) {
        nlohmann::json::parser_callback_t callback = [](int depth, nlohmann::json::parse_event_t event,
                                                        nlohmann::json::const_reference) -> bool {
            // Custom JSON parsing configuration
            return true;
        };
        nlohmann::json::set_default_callback(callback);
        initialized = true;
    }
}

} // namespace nlm