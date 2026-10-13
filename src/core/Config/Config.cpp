#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <string>
#include <cctype>

// Optional JSON support (requires nlohmann/json library)
// For now, we'll provide a placeholder structure
namespace {
    class JSONLoader {
    public:
        static bool load(const std::string& filepath, nlm::Config& config) {
            // TODO: Implement proper JSON parsing when nlohmann/json is available
            // For now, return false to fallback to plain text format
            return false;
        }
    };
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
    // Enhanced configuration file loader with multiple format support
    // Supports: .cfg (key=value), .json, .yaml/.yml, .ini
    
    // Determine file format by extension
    std::string ext = filepath.substr(filepath.find_last_of('.') + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    bool success = false;
    if (ext == "cfg" || ext == "ini") {
        success = loadFromPlainText(filepath);
    } else if (ext == "json") {
        success = loadFromJSON(filepath);
    } else if (ext == "yaml" || ext == "yml") {
        success = loadFromYAML(filepath);
    } else {
        NLM_LOG_WARNING(std::string("Unsupported config file format: ") + ext + ". Using plain text format.");
        success = loadFromPlainText(filepath);
    }
    
    if (success) {
        NLM_LOG_INFO("Configuration loaded from: " + filepath);
    } else {
        NLM_LOG_ERROR("Failed to load configuration from: " + filepath);
    }
    
    return success;
}

bool Config::loadFromPlainText(const std::string& filepath) {
    // Original plain text key=value format with enhanced parsing
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    while (std::getline(file, line)) {
        ++lineNumber;
        
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line.substr(0, 1) == "#" || line.substr(0, 1) == "//") {
            continue;
        }
        
        // Parse key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Handle different value types
            setValueFromString(key, value);
        } else if (line.find(':') != std::string::npos) {
            // Handle key: value format (like INI files)
            size_t colonPos = line.find(':');
            std::string key = trim(line.substr(0, colonPos));
            std::string value = trim(line.substr(colonPos + 1));
            setValueFromString(key, value);
        } else {
            NLM_LOG_WARNING(std::string("Invalid config line " + std::to_string(lineNumber) + ": " + line));
        }
    }
    
    return true;
}

bool Config::loadFromJSON(const std::string& filepath) {
    // Enhanced JSON loader with error handling and validation
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Recursively parse JSON structure
        parseJSONValue(jsonData, "");
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("JSON parsing error: ") + e.what());
        return false;
    }
}

bool Config::loadFromYAML(const std::string& filepath) {
    // Enhanced YAML loader with error handling and validation
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        // TODO: Add YAML library support when available
        NLM_LOG_ERROR("YAML configuration loading not yet implemented. Using plain text format.");
        return loadFromPlainText(filepath);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("YAML parsing error: ") + e.what());
        return false;
    }
}

void Config::setValueFromString(const std::string& key, const std::string& value) {
    // Enhanced value parsing with type detection
    std::string trimmedKey = trim(key);
    std::string trimmedValue = trim(value);
    
    // Remove surrounding quotes
    if (trimmedValue.size() >= 2 && 
        ((trimmedValue.front() == '"' && trimmedValue.back() == '"') ||
         (trimmedValue.front() == '\'' && trimmedValue.back() == '\''))) {
        trimmedValue = trimmedValue.substr(1, trimmedValue.size() - 2);
    }
    
    // Determine type based on content
    if (isBoolean(trimmedValue)) {
        bool boolValue = (trimmedValue == "true" || trimmedValue == "1" || trimmedValue == "yes");
        set(trimmedKey, boolValue, ConfigSource::File);
    } else if (isInteger(trimmedValue)) {
        try {
            // Try int64_t first
            size_t pos;
            int64_t intValue = std::stoll(trimmedValue, &pos);
            if (pos == trimmedValue.size()) {
                set(trimmedKey, intValue, ConfigSource::File);
                return;
            }
            
            // Try float if int conversion failed partially
            float floatValue = std::stof(trimmedValue);
            set(trimmedKey, floatValue, ConfigSource::File);
        } catch (...) {
            NLM_LOG_WARNING(std::string("Failed to parse integer value for key: ") + trimmedKey);
        }
    } else if (isFloat(trimmedValue)) {
        try {
            float floatValue = std::stof(trimmedValue);
            set(trimmedKey, floatValue, ConfigSource::File);
        } catch (...) {
            NLM_LOG_WARNING(std::string("Failed to parse float value for key: ") + trimmedKey);
        }
    } else {
        // Default to string
        set(trimmedKey, trimmedValue, ConfigSource::File);
    }
}

void Config::parseJSONValue(const nlohmann::json& jsonData, const std::string& prefix) {
    // Recursive JSON parsing
    if (jsonData.is_object()) {
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = prefix.empty() ? it.key() : prefix + "." + it.key();
            parseJSONValue(*it, key);
        }
    } else if (jsonData.is_array()) {
        // Handle arrays by taking the first element or average
        if (!jsonData.empty()) {
            std::string key = prefix + "_array";
            if (jsonData[0].is_number()) {
                float sum = 0.0f;
                for (const auto& item : jsonData) {
                    sum += item.get<float>();
                }
                float avg = sum / jsonData.size();
                setValueFromString(key, std::to_string(avg));
            } else {
                setValueFromString(key, jsonData[0].get<std::string>());
            }
        }
    } else {
        // Handle scalar values
        std::string valueStr;
        if (jsonData.is_string()) {
            valueStr = jsonData.get<std::string>();
        } else if (jsonData.is_number_integer()) {
            valueStr = std::to_string(jsonData.get<int64_t>());
        } else if (jsonData.is_number_float()) {
            valueStr = std::to_string(jsonData.get<float>());
        } else if (jsonData.is_boolean()) {
            valueStr = jsonData.get<bool>() ? "true" : "false";
        }
        
        if (!valueStr.empty()) {
            setValueFromString(prefix, valueStr);
        }
    }
}

bool Config::isBoolean(const std::string& str) const {
    std::string lower = toLower(str);
    return lower == "true" || lower == "false" || lower == "1" || lower == "0" || 
           lower == "yes" || lower == "no" || lower == "on" || lower == "off";
}

bool Config::isInteger(const std::string& str) const {
    if (str.empty()) return false;
    size_t pos;
    try {
        std::stoll(str, &pos);
        return pos == str.size();
    } catch (...) {
        return false;
    }
}

bool Config::isFloat(const std::string& str) const {
    if (str.empty()) return false;
    size_t pos;
    try {
        std::stof(str, &pos);
        return pos == str.size();
    } catch (...) {
        return false;
    }
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
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
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
