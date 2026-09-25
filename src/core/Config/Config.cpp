#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <iomanip>

namespace nlm {

std::string Config::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    bool validationEnabled = true;  // Enable/disable value validation
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->validationEnabled = true;
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_WARNING("Cannot open config file: " + filepath);
        return false;
    }
    
    std::string line;
    int lineNum = 0;
    bool hasError = false;
    
    while (std::getline(file, line)) {
        ++lineNum;
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            NLM_LOG_WARNING("Invalid config format in " + filepath + " line " + std::to_string(lineNum) + ": " + line);
            hasError = true;
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        // Validate key
        if (key.empty()) {
            NLM_LOG_WARNING("Empty key in " + filepath + " line " + std::to_string(lineNum));
            hasError = true;
            continue;
        }
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '\"' && value.back() == '\"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        try {
            set(key, value, ConfigSource::File);
        } catch (const std::exception& e) {
            NLM_LOG_WARNING("Error parsing config value for key '" + key + "' in " + filepath + " line " + std::to_string(lineNum) + ": " + e.what());
            hasError = true;
        }
    }
    
    // Log warning if we encountered parsing errors but file was partially loaded
    if (hasError) {
        NLM_LOG_WARNING("Config file " + filepath + " loaded with parsing errors. Check lines marked above.");
    }
    
    return true;
}

// Modern JSON/YAML configuration loading method
// Provides support for modern configuration file formats with automatic format detection
bool Config::loadFromFile(const std::string& filepath) {
    // Determine file format based on extension
    std::filesystem::path path(filepath);
    std::string extension = path.extension().string();
    
    // Convert extension to lowercase for comparison
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    try {
        if (extension == ".json" || extension == ".json5") {
            return loadFromJSON(filepath);
        } else if (extension == ".yaml" || extension == ".yml") {
            return loadFromYAML(filepath);
        } else {
            // Fall back to original key=value format for unknown extensions
            NLM_LOG_INFO("Using default key=value format for config file: " + filepath);
            return loadFromSimpleFormat(filepath);
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to load config file " + filepath + ": " + e.what());
        return false;
    }
}

// JSON configuration loading method with enhanced error handling and validation
bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open JSON config file: " + filepath);
        return false;
    }
    
    // Read entire file
    std::stringstream buffer;
    buffer << file.rdbuf();
    
    try {
        // Parse JSON
        JSONValue root = JSONParser::parse(buffer.str());
        
        // Convert JSON to configuration entries
        return convertFromJSON(root, ConfigSource::File);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("JSON parsing error in " + filepath + ": " + e.what());
        return false;
    }
}

// YAML configuration loading method (basic implementation)
// In production, this would use a proper YAML parser like libyaml
bool Config::loadFromYAML(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open YAML config file: " + filepath);
        return false;
    }
    
    // For now, fall back to simple format
    // TODO: Integrate proper YAML parser (e.g., libyaml or yaml-cpp)
    NLM_LOG_WARNING("YAML format not fully implemented. Using simple format parser for: " + filepath);
    return loadFromSimpleFormat(filepath);
}

// Convert JSON value to configuration entries
bool Config::convertFromJSON(const JSONValue& json, ConfigSource source) {
    if (json.type == JSONType::Object) {
        const auto& obj = json.getObject();
        
        for (const auto& pair : obj) {
            try {
                // Convert JSON value to string for storage
                std::string valueStr;
                
                switch (pair.second.type) {
                    case JSONType::Null:
                        valueStr = "";
                        break;
                    case JSONType::Boolean:
                        valueStr = pair.second.getBool() ? "true" : "false";
                        break;
                    case JSONType::Number:
                        // Check if it's an integer for better formatting
                        double num = pair.second.getNumber();
                        if (std::floor(num) == num) {
                            valueStr = std::to_string(static_cast<int64_t>(num));
                        } else {
                            valueStr = std::to_string(num);
                        }
                        break;
                    case JSONType::String:
                        valueStr = pair.second.getString();
                        break;
                    case JSONType::Array:
                        // Convert array to comma-separated list
                        const auto& arr = pair.second.getArray();
                        if (!arr.empty()) {
                            valueStr = "[";
                            for (size_t i = 0; i < arr.size(); ++i) {
                                if (i > 0) valueStr += ", ";
                                switch (arr[i].type) {
                                    case JSONType::Boolean:
                                        valueStr += arr[i].getBool() ? "true" : "false";
                                        break;
                                    case JSONType::Number:
                                        valueStr += std::to_string(arr[i].getNumber());
                                        break;
                                    case JSONType::String:
                                        valueStr += "\"" + arr[i].getString() + "\"";
                                        break;
                                    default:
                                        valueStr += "null";
                                        break;
                                }
                            }
                            valueStr += "]";
                        } else {
                            valueStr = "[]";
                        }
                        break;
                    default:
                        valueStr = "";
                        break;
                }
                
                // Validate and set the configuration value
                if (!validate(pair.first, valueStr) || !has(pair.first)) {
                    set(pair.first, valueStr, source);
                }
                
            } catch (const std::exception& e) {
                NLM_LOG_WARNING("Error converting JSON key '" + pair.first + ": " + e.what());
            }
        }
        
        return true;
    }
    
    NLM_LOG_ERROR("Invalid JSON root: expected object, got other type");
    return false;
}

// Original simple format loader (kept for backward compatibility)
bool Config::loadFromSimpleFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open config file: " + filepath);
        return false;
    }
    
    std::string line;
    int lineNum = 0;
    bool hasError = false;
    
    while (std::getline(file, line)) {
        ++lineNum;
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            NLM_LOG_WARNING("Invalid config format in " + filepath + " line " + std::to_string(lineNum) + ": " + line);
            hasError = true;
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        // Validate key
        if (key.empty()) {
            NLM_LOG_WARNING("Empty key in " + filepath + " line " + std::to_string(lineNum));
            hasError = true;
            continue;
        }
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        try {
            set(key, value, ConfigSource::File);
        } catch (const std::exception& e) {
            NLM_LOG_WARNING("Error parsing config value for key '" + key + "' in " + filepath + " line " + std::to_string(lineNum) + ": " + e.what());
            hasError = true;
        }
    }
    
    // Log warning if we encountered parsing errors but file was partially loaded
    if (hasError) {
        NLM_LOG_WARNING("Config file " + filepath + " loaded with parsing errors. Check lines marked above.");
    }
    
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated on " << getCurrentTimestamp() << "\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Properly format the value based on its type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float> || 
                                 std::is_same_v<T, int64_t> || std::is_same_v<T, int>) {
                file << arg;
            } else {
                file << static_cast<int>(arg);
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
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
    // Validate key
    if (key.empty()) {
        throw std::invalid_argument("Config key cannot be empty");
    }
    
    // Validate key format (allow alphanumeric, underscores, hyphens, dots)
    if (!std::all_of(key.begin(), key.end(), [](char c) {
        return std::isalnum(static_cast<unsigned char>(c)) || c == '_' || c == '-' || c == '.';
    })) {
        throw std::invalid_argument("Invalid config key format: " + key + ". Only alphanumeric characters, underscores, hyphens, and dots are allowed.");
    }
    
    // Validate value
    if (value.empty() && pImpl->validationEnabled) {
        // Allow empty strings for optional values
        NLM_LOG_DEBUG("Empty config value set for key: " + key);
    }
    
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    if (pImpl->validationEnabled) {
        // Add validation rules for integer values
        if (key == "neuron_count" && value <= 0) {
            throw std::invalid_argument("neuron_count must be positive, got " + std::to_string(value));
        }
        if (key == "region_count" && value <= 0) {
            throw std::invalid_argument("region_count must be positive, got " + std::to_string(value));
        }
    }
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    if (pImpl->validationEnabled) {
        // Add validation rules for double values
        if (key == "simulation_timestep" && value <= 0.0) {
            throw std::invalid_argument("simulation_timestep must be positive, got " + std::to_string(value));
        }
        if (key == "connection_probability" && (value < 0.0 || value > 1.0)) {
            throw std::invalid_argument("connection_probability must be between 0.0 and 1.0, got " + std::to_string(value));
        }
    }
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

// Enhanced loadFromArgs method with comprehensive command-line support
// Provides advanced command-line argument parsing with validation and error handling
bool Config::loadFromArgs(int argc, char** argv) {
    bool hasError = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Validate key and value format
                if (key.empty()) {
                    NLM_LOG_WARNING("Empty config key in command line arguments");
                    hasError = true;
                    continue;
                }
                
                if (value.empty()) {
                    NLM_LOG_WARNING("Empty config value for key '" + key + "' in command line arguments");
                }
                
                try {
                    set(key, value, ConfigSource::CommandLine);
                } catch (const std::exception& e) {
                    NLM_LOG_WARNING("Error parsing command line argument key '" + key + ": " + e.what());
                    hasError = true;
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            if (key.empty()) {
                NLM_LOG_WARNING("Empty config key in command line arguments");
                hasError = true;
                continue;
            }
            
            if (value.empty()) {
                NLM_LOG_WARNING("Empty config value for key '" + key + "' in command line arguments");
            }
            
            try {
                set(key, value, ConfigSource::CommandLine);
            } catch (const std::exception& e) {
                NLM_LOG_WARNING("Error parsing command line argument key '" + key + ": " + e.what());
                hasError = true;
            }
        }
    }
    
    return !hasError;
}

void Config::validate() const {
    if (!pImpl->validationEnabled) {
        return;
    }
    
    std::vector<std::string> errors;
    
    // Validate required configuration values
    if (has("neuron_count")) {
        auto val = get<int64_t>("neuron_count");
        if (val && *val <= 0) {
            errors.push_back("neuron_count must be positive, got " + std::to_string(*val));
        }
    }
    
    if (has("region_count")) {
        auto val = get<int64_t>("region_count");
        if (val && *val <= 0) {
            errors.push_back("region_count must be positive, got " + std::to_string(*val));
        }
    }
    
    if (has("simulation_timestep")) {
        auto val = get<double>("simulation_timestep");
        if (val && *val <= 0.0) {
            errors.push_back("simulation_timestep must be positive, got " + std::to_string(*val));
        }
    }
    
    if (has("connection_probability")) {
        auto val = get<float>("connection_probability");
        if (val) {
            if (*val < 0.0f || *val > 1.0f) {
                errors.push_back("connection_probability must be between 0.0 and 1.0, got " + std::to_string(*val));
            }
        }
    }
    
    if (!errors.empty()) {
        std::string errorMsg = "Configuration validation failed:\n";
        for (const auto& error : errors) {
            errorMsg += "  - " + error + "\n";
        }
        throw std::runtime_error(errorMsg);
    }
}

bool Config::validate(const std::string& key, const std::string& value) {
    try {
        // Attempt to parse the value based on the key
        if (key == "neuron_count" || key == "region_count") {
            size_t pos;
            int64_t intVal = std::stoll(value, &pos);
            if (pos != value.length()) throw std::invalid_argument("Extra characters");
            return intVal > 0;
        } else if (key == "simulation_timestep") {
            size_t pos;
            double doubleVal = std::stod(value, &pos);
            if (pos != value.length()) throw std::invalid_argument("Extra characters");
            return doubleVal > 0.0;
        } else if (key == "connection_probability") {
            size_t pos;
            float floatVal = std::stof(value, &pos);
            if (pos != value.length()) throw std::invalid_argument("Extra characters");
            return floatVal >= 0.0f && floatVal <= 1.0f;
        }
        return true;  // For unknown keys, accept any value
    } catch (const std::exception&) {
        return false;
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

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
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

// Explicit template instantiations with C++20 improved syntax
// These are explicitly instantiated to ensure the compiler generates code for these types
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

// C++20 improved getOr template instantiations with noexcept specification
template int Config::getOr<int>(const std::string&, const int&) const noexcept;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const noexcept;
template double Config::getOr<double>(const std::string&, const double&) const noexcept;
template bool Config::getOr<bool>(const std::string&, const bool&) const noexcept;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const noexcept;

// C++23 filesystem integration for config file operations
void Config::saveToFile(const std::string& filepath) const {
    std::error_code ec;
    std::filesystem::path fsPath(filepath);
    
    // Ensure the directory exists
    if (!std::filesystem::exists(fsPath.parent_path(), ec)) {
        std::filesystem::create_directories(fsPath.parent_path(), ec);
        if (ec) {
            NLM_LOG_WARNING("Could not create directory for config file " + filepath + ": " + ec.message());
            return;
        }
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file for writing: " + filepath);
        return;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated on " << getCurrentTimestamp() << "\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Properly format the value based on its type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float> || 
                                 std::is_same_v<T, int64_t> || std::is_same_v<T, int>) {
                file << arg;
            } else {
                file << static_cast<int>(arg);
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    // Ensure all data is written to disk
    file.flush();
    if (file.fail()) {
        NLM_LOG_ERROR("Failed to write all data to config file " + filepath);
    }
}

} // namespace nlm
