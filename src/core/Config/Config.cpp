// Simple JSON/YAML Config Implementation
// This implementation focuses on the core requirements:
// 1. Replace placeholder parser with proper JSON/YAML parsing
// 2. Fix saveToFile() bug
// 3. Support different data types
// 4. Maintain backward compatibility with key=value format

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;
Config& Config::operator=(Config&&) noexcept = default;

// Determine if file is JSON based on extension
bool Config::isJsonFile(const std::string& filepath) {
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext == ".json" || ext == ".cfg"; // Backward compatibility with .cfg
}

// Determine if file is YAML based on extension  
bool Config::isYamlFile(const std::string& filepath) {
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext == ".yaml" || ext == ".yml";
}

// Parse simple key=value line for backward compatibility
bool parseSimpleLine(const std::string& line, ConfigEntry& entry) {
    size_t pos = line.find('=');
    if (pos == std::string::npos) {
        return false;
    }
    
    std::string key = line.substr(0, pos);
    std::string value = line.substr(pos + 1);
    
    key = trim(key);
    value = trim(value);
    
    // Remove quotes if present
    if (value.size() >= 2 && 
        ((value.front() == '"' && value.back() == '"') ||
         (value.front() == '\'' && value.back() == '\''))) {
        value = value.substr(1, value.size() - 2);
    }
    
    entry.key = key;
    entry.source = ConfigSource::File;
    entry.description = "Loaded from simple key=value format";
    
    // Try to parse as different types
    if (value.empty()) {
        entry.value = value;
    } else if (value == "true" || value == "false") {
        entry.value = (value == "true");
    } else {
        // Try integer
        try {
            if (value.find('.') == std::string::npos) {
                size_t pos;
                int intVal = std::stoi(value, &pos);
                if (pos == value.size()) {
                    entry.value = intVal;
                }
            }
        } catch (...) {}
        
        // Try double
        if (entry.value.index() == 0) { // No type assigned yet
            try {
                size_t pos;
                double doubleVal = std::stod(value, &pos);
                if (pos == value.size()) {
                    entry.value = doubleVal;
                }
            } catch (...) {}
        }
        
        // Try int64_t for large integers
        if (entry.value.index() == 0) {
            try {
                size_t pos;
                int64_t int64Val = std::stoll(value, &pos);
                if (pos == value.size()) {
                    entry.value = int64Val;
                }
            } catch (...) {}
        }
        
        // Fallback to string
        if (entry.value.index() == 0) {
            entry.value = value;
        }
    }
    
    return true;
}

bool Config::loadFromFile(const std::string& filepath) {
    clear(); // Clear existing entries
    
    // Check file extension for format detection
    if (isJsonFile(filepath)) {
        return fromJsonFile(filepath);
    } else if (isYamlFile(filepath)) {
        return fromYamlFile(filepath);
    } else {
        // Try to detect format based on content
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string firstLine;
        std::getline(file, firstLine);
        file.clear();
        file.seekg(0);
        
        firstLine = trim(firstLine);
        // Check if it starts with { or [
        if (!firstLine.empty() && (firstLine[0] == '{' || firstLine[0] == '[')) {
            return fromJsonFile(filepath);
        } else {
            // Check for YAML-like structure (key: value or key: [values...])
            size_t colonPos = firstLine.find(':');
            if (colonPos != std::string::npos) {
                return fromYamlFile(filepath);
            } else {
                // Try to detect simple key=value format
                std::string line;
                bool hasSimpleFormat = false;
                while (std::getline(file, line)) {
                    line = trim(line);
                    if (!line.empty() && !line[0] == '#' && line.find('=') != std::string::npos) {
                        hasSimpleFormat = true;
                        break;
                    }
                }
                file.clear();
                file.seekg(0);
                
                if (hasSimpleFormat) {
                    return fromSimpleFile(filepath);
                }
            }
        }
    }
    
    return false;
}

bool Config::fromJsonFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json j;
        file >> j;
        return fromJson(j);
    } catch (const std::exception& e) {
        std::cerr << "Error parsing JSON file " << filepath << ": " << e.what() << std::endl;
        return false;
    }
}

bool Config::fromYamlFile(const std::string& filepath) {
    // Note: yaml-cpp is a separate dependency that would need to be added
    // For now, return false and fallback to simple format
    std::cerr << "YAML parsing requires yaml-cpp dependency. Please install libyaml-cpp-dev." << std::endl;
    return false;
}

bool Config::fromSimpleFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        ConfigEntry entry;
        if (parseSimpleLine(line, entry)) {
            pImpl->entries.push_back(entry);
        }
    }
    
    return true;
}

nlohmann::json Config::toJson() const {
    nlohmann::json j = nlohmann::json::object();
    
    for (const auto& entry : pImpl->entries) {
        j[entry.key] = configValueToJson(entry.value);
    }
    
    return j;
}

std::optional<ConfigValue> Config::jsonToConfigValue(const nlohmann::json& j) {
    if (j.is_string()) {
        return j.get<std::string>();
    } else if (j.is_number_integer()) {
        // Check for int64_t range
        int64_t val = j.get<int64_t>();
        if (val >= std::numeric_limits<int>::min() && val <= std::numeric_limits<int>::max()) {
            return static_cast<int>(val);
        }
        return val;
    } else if (j.is_number()) {
        // Check if it's actually an integer (without decimal)
        if (j.is_number_integer()) {
            int64_t val = j.get<int64_t>();
            if (val >= std::numeric_limits<int>::min() && val <= std::numeric_limits<int>::max()) {
                return static_cast<int>(val);
            }
            return val;
        } else {
            return j.get<double>();
        }
    } else if (j.is_boolean()) {
        return j.get<bool>();
    } else if (j.is_array()) {
        if (j.empty()) {
            return std::vector<int>();
        }
        
        // Check array type consistency
        bool allInts = true, allDoubles = true, allStrings = true;
        for (const auto& item : j) {
            if (!item.is_number_integer()) allInts = false;
            if (!item.is_number()) allDoubles = false;
            if (!item.is_string()) allStrings = false;
        }
        
        if (allInts) {
            std::vector<int> vec;
            for (const auto& item : j) {
                vec.push_back(item.get<int>());
            }
            return vec;
        } else if (allDoubles) {
            std::vector<double> vec;
            for (const auto& item : j) {
                vec.push_back(item.get<double>());
            }
            return vec;
        } else if (allStrings) {
            std::vector<std::string> vec;
            for (const auto& item : j) {
                vec.push_back(item.get<std::string>());
            }
            return vec;
        }
        
        // Mixed types - fall back to string
        return j.dump();
    }
    
    return std::nullopt;
}

nlohmann::json Config::configValueToJson(const ConfigValue& cv) {
    return std::visit([](auto&& arg) -> nlohmann::json {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg;
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            return static_cast<int64_t>(arg);
        } else if constexpr (std::is_same_v<T, double>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            nlohmann::json j = nlohmann::json::array();
            for (const auto& item : arg) {
                j.push_back(item);
            }
            return j;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            nlohmann::json j = nlohmann::json::array();
            for (const auto& item : arg) {
                j.push_back(item);
            }
            return j;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            nlohmann::json j = nlohmann::json::array();
            for (const auto& item : arg) {
                j.push_back(item);
            }
            return j;
        } else {
            return "";
        }
    }, cv);
}

bool Config::fromJson(const nlohmann::json& j) {
    clear();
    
    if (!j.is_object()) {
        std::cerr << "Invalid JSON configuration: expected an object" << std::endl;
        return false;
    }
    
    for (const auto& [key, value] : j.items()) {
        std::optional<ConfigValue> configValue = jsonToConfigValue(value);
        if (configValue.has_value()) {
            ConfigEntry entry(key, configValue.value(), ConfigSource::File, "Loaded from JSON");
            pImpl->entries.push_back(entry);
        } else {
            std::cerr << "Warning: Could not parse value for key '" << key << "'" << std::endl;
        }
    }
    
    return true;
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
    
    // Determine file format based on extension
    if (isJsonFile(filepath)) {
        nlohmann::json j = toJson();
        file << j.dump(4); // Pretty print with 4-space indentation
    } else if (isYamlFile(filepath)) {
        // Note: yaml-cpp is a separate dependency
        file << "# YAML format not supported without yaml-cpp library" << std::endl;
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << ": " << configValueToJson(entry.value).dump() << "\n";
        }
    } else {
        // Save in simple key=value format (backward compatibility)
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = ";
            
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (arg ? "true" : "false");
                } else {
                    file << arg;
                }
            }, entry.value);
            file << "\n";
        }
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

// ConfigEntry methods (moved from Config.cpp)
nlohmann::json ConfigEntry::toJson() const {
    nlohmann::json j;
    j["key"] = key;
    j["value"] = configValueToJson(value);
    j["source"] = static_cast<int>(source);
    j["description"] = description;
    return j;
}

std::optional<ConfigEntry> ConfigEntry::fromJson(const nlohmann::json& j) {
    if (!j.is_object()) {
        return std::nullopt;
    }
    
    ConfigEntry entry;
    
    // Parse key
    if (j.contains("key") && j["key"].is_string()) {
        entry.key = j["key"].get<std::string>();
    } else {
        return std::nullopt;
    }
    
    // Parse value
    if (j.contains("value")) {
        std::optional<ConfigValue> configValue = jsonToConfigValue(j["value"]);
        if (configValue.has_value()) {
            entry.value = configValue.value();
        } else {
            return std::nullopt;
        }
    }
    
    // Parse source
    if (j.contains("source") && j["source"].is_number_integer()) {
        entry.source = static_cast<ConfigSource>(j["source"].get<int>());
    }
    
    // Parse description
    if (j.contains("description") && j["description"].is_string()) {
        entry.description = j["description"].get<std::string>();
    }
    
    return entry;
}

} // namespace nlm
