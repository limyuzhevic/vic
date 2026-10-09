// Improved Config System Implementation
// This provides a more robust and user-friendly configuration system

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <limits>
#include <stdexcept>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    setDefaults();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::string resolvedPath = resolveFilePath(filepath);
    if (resolvedPath.empty()) {
        NLM_LOG_ERROR("Config file not found: " + filepath);
        return false;
    }
    
    std::string extension = extractExtension(resolvedPath);
    
    if (extension == ".json" || extension == ".JSON") {
        return loadJsonFromFile(resolvedPath);
    } else {
        return loadSimpleFromFile(resolvedPath);
    }
}

bool Config::loadJsonFromFile(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Cannot open config file: " + filepath);
            return false;
        }
        
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Clear existing entries
        pImpl->entries.clear();
        
        // Process JSON data
        if (jsonData.is_object()) {
            for (const auto& [key, value] : jsonData.items()) {
                set(key, ConfigValue::fromJson(value), ConfigSource::File);
            }
            
            NLM_LOG_INFO("Loaded configuration from JSON file: " + filepath);
            return true;
        } else {
            NLM_LOG_ERROR("Invalid JSON format in config file: " + filepath);
            return false;
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading JSON config file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::loadSimpleFromFile(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Cannot open config file: " + filepath);
            return false;
        }
        
        std::string line;
        int lineNumber = 0;
        bool hasErrors = false;
        
        // Clear existing entries
        pImpl->entries.clear();
        
        while (std::getline(file, line)) {
            lineNumber++;
            
            // Skip empty lines and comments
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            // Parse key=value pairs
            size_t pos = line.find('=');
            if (pos == std::string::npos) {
                NLM_LOG_ERROR("Invalid config format in " + filepath + " line " + std::to_string(lineNumber) +
                             ": missing '='");
                hasErrors = true;
                continue;
            }
            
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            if (!isValidKey(key)) {
                NLM_LOG_ERROR("Invalid key in " + filepath + " line " + std::to_string(lineNumber) +
                             ": " + key);
                hasErrors = true;
                continue;
            }
            
            // Try to infer value type
            ConfigValue configValue;
            try {
                configValue = stringToValue(value);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Invalid value in " + filepath + " line " + std::to_string(lineNumber) +
                             ": " + value + " (" + std::string(e.what()) + ")");
                hasErrors = true;
                continue;
            }
            
            set(key, configValue, ConfigSource::File);
        }
        
        if (hasErrors) {
            NLM_LOG_WARNING("Config file " + filepath + " loaded with errors");
        } else {
            NLM_LOG_INFO("Loaded configuration from file: " + filepath);
        }
        
        return !hasErrors;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading config file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    std::string extension = extractExtension(filepath);
    
    if (extension == ".json" || extension == ".JSON") {
        return saveAsJson(filepath);
    } else {
        return saveAsSimple(filepath);
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return it->value.get<T>();
    } catch (const std::runtime_error&) {
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    if (!isValidKey(key)) {
        throw std::invalid_argument("Invalid configuration key: " + key);
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<std::string>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<double>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<int>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const nlohmann::json& value, ConfigSource source) {
    set(key, ConfigValue::fromJson(value), source);
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

void Config::setDefaults() {
    // Set default configuration values
    set("random_seed", 42, ConfigSource::Default);
    set("simulation_timestep", 0.001, ConfigSource::Default);
    set("neuron_count", 500, ConfigSource::Default);
    set("region_count", 1, ConfigSource::Default);
    set("connection_probability", 0.15, ConfigSource::Default);
    set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    set("stdp_tau", 20.0f, ConfigSource::Default);
    set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    set("pruning_rate", 0.00001f, ConfigSource::Default);
}

bool Config::validate(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return false;
    }
    
    // Validate based on key
    if (key == "random_seed") {
        int seed = it->value.getInt();
        return seed >= 0;
    } else if (key == "simulation_timestep") {
        double timestep = it->value.getDouble();
        return timestep > 0.0 && timestep < 1.0;
    } else if (key == "neuron_count") {
        int count = it->value.getInt();
        return count > 0 && count <= 10000;
    } else if (key == "region_count") {
        int count = it->value.getInt();
        return count > 0 && count <= 100;
    } else if (key == "connection_probability") {
        double prob = it->value.getDouble();
        return prob >= 0.0 && prob <= 1.0;
    }
    
    return true; // Unknown keys are considered valid
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        try {
            oss << entry.value.toString();
        } catch (const std::exception& e) {
            oss << "error: " << e.what();
        }
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

std::string Config::resolveFilePath(const std::string& filepath) {
    // If it's already an absolute path
    if (std::filesystem::path(filepath).is_absolute()) {
        return filepath;
    }
    
    // Check in current directory
    if (std::filesystem::exists(filepath)) {
        return filepath;
    }
    
    // Check in configs directory (common location)
    std::string configPath = "configs/" + filepath;
    if (std::filesystem::exists(configPath)) {
        return configPath;
    }
    
    return ""; // File not found
}

bool Config::saveAsJson(const std::string& filepath) const {
    try {
        nlohmann::json jsonData;
        
        for (const auto& entry : pImpl->entries) {
            jsonData[entry.key] = entry.value.toJson();
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Cannot open config file for writing: " + filepath);
            return false;
        }
        
        file << std::setw(2) << jsonData;
        file.close();
        
        NLM_LOG_INFO("Saved configuration to JSON file: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving JSON config file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::saveAsSimple(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Cannot open config file for writing: " + filepath);
            return false;
        }
        
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = " << entry.value.toString() << "\n";
            file << "\n";
        }
        
        file.close();
        
        NLM_LOG_INFO("Saved configuration to file: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving config file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

void Config::parseArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                parseEqualsFormat(arg, ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            parseShortFormat(key, value, ConfigSource::CommandLine);
        }
    }
}

void Config::parseKeyValue(const std::string& arg, ConfigSource source) {
    // Simple key=value format for runtime configuration
    size_t pos = arg.find('=');
    if (pos == std::string::npos) {
        return;
    }
    
    std::string key = trim(arg.substr(0, pos));
    std::string value = trim(arg.substr(pos + 1));
    
    set(key, value, source);
}

void Config::parseEqualsFormat(const std::string& arg, ConfigSource source) {
    // Handle --key=value format
    std::string key = arg.substr(2);
    size_t pos = key.find('=');
    if (pos == std::string::npos) {
        return;
    }
    
    key = key.substr(0, pos);
    std::string value = key.substr(pos + 1);
    
    set(key, value, source);
}

void Config::parseShortFormat(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, value, source);
}

std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

bool Config::stringEndsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

std::string Config::extractExtension(const std::string& filename) {
    size_t pos = filename.rfind('.');
    if (pos == std::string::npos) return "";
    
    std::string extension = filename.substr(pos);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    return extension;
}

ConfigValue Config::stringToValue(const std::string& str) const {
    // Try to parse as different types
    if (str == "true" || str == "false") {
        return ConfigValue(str == "true");
    }
    
    // Try integer
    try {
        size_t pos;
        int intVal = std::stoi(str, &pos);
        if (pos == str.size()) {
            return ConfigValue(intVal);
        }
    } catch (...) {}
    
    // Try int64
    try {
        size_t pos;
        int64_t int64Val = std::stoll(str, &pos);
        if (pos == str.size()) {
            return ConfigValue(int64Val);
        }
    } catch (...) {}
    
    // Try double
    try {
        size_t pos;
        double doubleVal = std::stod(str, &pos);
        if (pos == str.size()) {
            return ConfigValue(doubleVal);
        }
    } catch (...) {}
    
    // Check for vector format [a,b,c]
    if (str.front() == '[' && str.back() == ']') {
        std::string content = str.substr(1, str.size() - 2);
        std::vector<std::string> tokens;
        
        // Simple tokenization - this could be improved
        size_t start = 0;
        size_t end = content.find(',');
        while (end != std::string::npos) {
            std::string token = trim(content.substr(start, end - start));
            tokens.push_back(token);
            start = end + 1;
            end = content.find(',', start);
        }
        tokens.push_back(trim(content.substr(start)));
        
        // Check if all tokens can be parsed as strings
        bool allStrings = true;
        for (const auto& token : tokens) {
            if (token.empty() || token.front() == '[' || token.back() == ']') {
                allStrings = false;
                break;
            }
        }
        
        if (allStrings) {
            // Try to determine if it's a string vector or double vector
            bool allDoubles = true;
            for (const auto& token : tokens) {
                try {
                    std::stod(token);
                } catch (...) {
                    allDoubles = false;
                    break;
                }
            }
            
            if (allDoubles) {
                std::vector<double> doubleVec;
                for (const auto& token : tokens) {
                    doubleVec.push_back(std::stod(token));
                }
                return ConfigValue(doubleVec);
            } else {
                std::vector<std::string> stringVec;
                for (const auto& token : tokens) {
                    stringVec.push_back(token);
                }
                return ConfigValue(stringVec);
            }
        }
    }
    
    // Default to string
    return ConfigValue(str);
}

std::string Config::valueToString(const ConfigValue& value) const {
    return value.toString();
}

bool Config::loadFromArgs(int argc, char** argv) {
    parseArgs(argc, argv);
    return true;
}

// Explicit template instantiations
namespace {
    // Helper to register template instantiations
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
}

} // namespace nlm

// Add the nlohmann/json dependency to CMakeLists.txt
// find_package(nlohmann_json REQUIRED)
// target_link_libraries(nlm_core PUBLIC nlohmann_json::nlohmann_json)