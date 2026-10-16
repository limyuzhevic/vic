#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>

// JSON conversion helpers
namespace {
    // Convert nlohmann::json to ConfigValue
    ConfigValue jsonToConfigValue(const nlohmann::json& j) {
        if (j.is_string()) {
            return j.get<std::string>();
        } else if (j.is_number_integer()) {
            return j.get<int64_t>();
        } else if (j.is_number_unsigned()) {
            return j.get<uint64_t>();
        } else if (j.is_number_float()) {
            return j.get<double>();
        } else if (j.is_boolean()) {
            return j.get<bool>();
        } else if (j.is_array()) {
            // Handle arrays - determine type based on first element
            if (j.empty()) {
                return std::vector<std::string>{};
            }
            
            // Try to create homogeneous array based on first element type
            const auto& first = j[0];
            if (first.is_string()) {
                std::vector<std::string> vec;
                vec.reserve(j.size());
                for (const auto& item : j) {
                    vec.push_back(item.get<std::string>());
                }
                return vec;
            } else if (first.is_number_integer()) {
                std::vector<int> vec;
                vec.reserve(j.size());
                for (const auto& item : j) {
                    vec.push_back(item.get<int64_t>());
                }
                return vec;
            } else if (first.is_number_float()) {
                std::vector<double> vec;
                vec.reserve(j.size());
                for (const auto& item : j) {
                    vec.push_back(item.get<double>());
                }
                return vec;
            } else if (first.is_boolean()) {
                std::vector<bool> vec;
                vec.reserve(j.size());
                for (const auto& item : j) {
                    vec.push_back(item.get<bool>());
                }
                return vec;
            }
        }
        
        // Fallback for complex types - convert to string
        return j.dump();
    }
    
    // Convert ConfigValue to nlohmann::json
    nlohmann::json configValueToJson(const ConfigValue& value) {
        if (std::holds_alternative<int>(value)) {
            return std::get<int>(value);
        } else if (std::holds_alternative<int64_t>(value)) {
            return std::get<int64_t>(value);
        } else if (std::holds_alternative<double>(value)) {
            return std::get<double>(value);
        } else if (std::holds_alternative<bool>(value)) {
            return std::get<bool>(value);
        } else if (std::holds_alternative<std::string>(value)) {
            return std::get<std::string>(value);
        } else if (std::holds_alternative<std::vector<int>>(value)) {
            return std::get<std::vector<int>>(value);
        } else if (std::holds_alternative<std::vector<double>>(value)) {
            return std::get<std::vector<double>>(value);
        } else if (std::holds_alternative<std::vector<std::string>>(value)) {
            return std::get<std::vector<std::string>>(value);
        } else if (std::holds_alternative<std::vector<bool>>(value)) {
            return std::get<std::vector<bool>>(value);
        } else {
            // Unknown type - convert to string
            return std::visit([](auto&& arg) -> nlohmann::json {
                if constexpr (std::is_same_v<decltype(arg), std::string>) {
                    return arg;
                } else if constexpr (std::is_same_v<decltype(arg), int64_t>) {
                    return arg;
                } else if constexpr (std::is_same_v<decltype(arg), double>) {
                    return arg;
                } else if constexpr (std::is_same_v<decltype(arg), bool>) {
                    return arg;
                } else {
                    return "";
                }
            }, value);
        }
    }
}

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try to parse as JSON first (if file starts with '{' or '[' or contains valid JSON)
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read the first few characters to determine file type
    std::string firstChars(3, '\0');
    file.read(&firstChars[0], 3);
    file.clear();
    file.seekg(0);
    
    bool isJson = false;
    if (firstChars.find('{') != std::string::npos || firstChars.find('[') != std::string::npos) {
        // Looks like JSON, try to parse it
        try {
            auto json = nlohmann::json::parse(file);
            
            // Process JSON object recursively
            processJsonObject(json, ConfigSource::File, "");
            
            return true;
        } catch (const nlohmann::json::parse_error& e) {
            // Not valid JSON, fall back to simple format
            NLM_LOG_WARNING("JSON parsing failed: " + std::string(e.what()) + ". Falling back to simple format.");
            file.clear();
            file.seekg(0);
        }
    }
    
    // Original simple key=value format parsing (backward compatibility)
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

void Config::processJsonObject(const nlohmann::json& json, ConfigSource source, const std::string& prefix) {
    if (json.is_object()) {
        for (auto it = json.begin(); it != json.end(); ++it) {
            std::string key = prefix + it.key();
            processJsonValue(*it, key, source);
        }
    } else if (json.is_array() && !prefix.empty()) {
        // For arrays, convert to string representation
        ConfigValue value = jsonToConfigValue(json);
        set(prefix, value, source);
    }
}

void Config::processJsonValue(const nlohmann::json& json, const std::string& key, ConfigSource source) {
    // Check if value is nested (object or array)
    if (json.is_object()) {
        // Recursively process nested object
        processJsonObject(json, source, key + ".");
    } else if (json.is_array()) {
        // For arrays, check if they're homogeneous
        if (json.empty()) {
            set(key, std::vector<std::string>{});
        } else {
            ConfigValue value = jsonToConfigValue(json);
            set(key, value, source);
        }
    } else {
        // Simple value
        ConfigValue value = jsonToConfigValue(json);
        set(key, value, source);
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "{\n";
    
    size_t count = 0;
    for (const auto& entry : pImpl->entries) {
        file << "  \"" << entry.key << "\": ";
        
        // Convert ConfigValue to JSON
        nlohmann::json j = configValueToJson(entry.value);
        file << j.dump();
        
        // Add comment if description exists
        if (!entry.description.empty()) {
            file << "  // " << entry.description;
        }
        
        if (count < pImpl->entries.size() - 1) {
            file << ",\n";
        } else {
            file << "\n";
        }
        
        count++;
    }
    
    file << "}\n";
    
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

} // namespace nlm
