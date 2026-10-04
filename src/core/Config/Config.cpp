#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace nlm {

// JSON serialization helper functions
static void to_json(nlohmann::json& j, const ConfigEntry& entry) {
    j = nlohmann::json::object();
    j["key"] = entry.key;
    j["source"] = static_cast<int>(entry.source);
    j["description"] = entry.description;
    
    // Serialize the value based on its type
    std::visit([&j](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            j["value"] = arg;
            j["type"] = "int";
        } else if constexpr (std::is_same_v<T, int64_t>) {
            j["value"] = arg;
            j["type"] = "int64_t";
        } else if constexpr (std::is_same_v<T, double>) {
            j["value"] = arg;
            j["type"] = "double";
        } else if constexpr (std::is_same_v<T, bool>) {
            j["value"] = arg;
            j["type"] = "bool";
        } else if constexpr (std::is_same_v<T, std::string>) {
            j["value"] = arg;
            j["type"] = "string";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            j["value"] = arg;
            j["type"] = "vector<int>";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            j["value"] = arg;
            j["type"] = "vector<double>";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            j["value"] = arg;
            j["type"] = "vector<string>";
        }
    }, entry.value);
}

static void from_json(const nlohmann::json& j, ConfigEntry& entry) {
    entry.key = j.value("key", "");
    entry.source = static_cast<ConfigSource>(j.value("source", static_cast<int>(ConfigSource::Default)));
    entry.description = j.value("description", "");
    
    if (j.contains("type") && j.contains("value")) {
        std::string type = j["type"].get<std::string>();
        const auto& value = j["value"];
        
        if (type == "int") {
            entry.value = value.get<int>();
        } else if (type == "int64_t") {
            entry.value = value.get<int64_t>();
        } else if (type == "double") {
            entry.value = value.get<double>();
        } else if (type == "bool") {
            entry.value = value.get<bool>();
        } else if (type == "string") {
            entry.value = value.get<std::string>();
        } else if (type == "vector<int>") {
            entry.value = value.get<std::vector<int>>();
        } else if (type == "vector<double>") {
            entry.value = value.get<std::vector<double>>();
        } else if (type == "vector<string>") {
            entry.value = value.get<std::vector<std::string>>();
        }
    }
}

// Helper to check file extension for format detection
static std::string getFileExtension(const std::string& filepath) {
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos == std::string::npos) return "";
    
    std::string ext = filepath.substr(dotPos + 1);
    // Convert to lowercase for comparison
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext;
}

// Parse JSON configuration file
bool Config::parseJSONFile(const std::string& filepath, std::vector<ConfigEntry>& entries) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json j;
        file >> j;
        
        if (!j.is_array()) {
            // Try to parse as object (single config file)
            nlohmann::json configArray = nlohmann::json::array();
            configArray.push_back(j);
            j = configArray;
        }
        
        entries.clear();
        for (const auto& item : j) {
            ConfigEntry entry;
            from_json(item, entry);
            entries.push_back(entry);
        }
        
        return true;
    } catch (const std::exception& e) {
        // Log error if logger is available
        return false;
    }
}

// Parse YAML configuration file (placeholder for future implementation)
bool Config::parseYAMLFile(const std::string& filepath, std::vector<ConfigEntry>& entries) {
    // TODO: Implement YAML parsing when YAML library is available
    // For now, just return false to indicate unsupported format
    return false;
}

// Generate JSON from entries
static nlohmann::json generateJSON(const std::vector<ConfigEntry>& entries) {
    nlohmann::json j = nlohmann::json::array();
    for (const auto& entry : entries) {
        nlohmann::json item;
        to_json(item, entry);
        j.push_back(item);
    }
    return j;
}

// Generate YAML from entries (placeholder for future implementation)
static std::string generateYAML(const std::vector<ConfigEntry>& entries) {
    // TODO: Implement YAML generation when YAML library is available
    return "";
}


// Parse JSON configuration file
bool Config::loadFromFile(const std::string& filepath) {
    // Detect file format based on extension
    std::string ext = getFileExtension(filepath);
    
    if (ext == "json") {
        return parseJSONFile(filepath);
    } else if (ext == "yaml" || ext == "yml") {
        return parseYAMLFile(filepath);
    } else {
        // Fallback to simple key=value format for backward compatibility
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
                
                set(key, value, ConfigSource::File);
            }
        }
        
        return true;
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
    // Detect file format based on extension
    std::string ext = getFileExtension(filepath);
    
    if (ext == "json") {
        return saveAsJSON(filepath);
    } else if (ext == "yaml" || ext == "yml") {
        return saveAsYAML(filepath);
    } else {
        // Fallback to simple key=value format for backward compatibility
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << entry.key << " = \"" << arg << "\"\n";
                } else {
                    file << entry.key << " = " << arg << "\n";
                }
            }, entry.value);
        }
        
        return true;
    }
}

// Save configuration as JSON file
bool Config::saveAsJSON(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json j = generateJSON(pImpl->entries);
        file << j.dump(2); // Pretty print with 2-space indentation
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

// Save configuration as YAML file (placeholder for future implementation)
bool Config::saveAsYAML(const std::string& filepath) const {
    // TODO: Implement YAML saving when YAML library is available
    // For now, just save as JSON as a fallback
    return saveAsJSON(filepath);
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
