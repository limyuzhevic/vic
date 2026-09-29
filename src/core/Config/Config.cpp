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

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Config file not found: " + filepath);
        return false;
    }
    
    // Read entire file
    std::string content((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());
    file.close();
    
    // Determine file type and parse accordingly
    std::string extension = filepath.substr(filepath.find_last_of(".") + 1);
    
    if (extension == "json") {
        return parseJson(content, ConfigSource::File);
    } else if (extension == "yaml" || extension == "yml") {
        return parseYaml(content, ConfigSource::File);
    } else {
        // Default to key=value format for backward compatibility
        return parseKeyValueFormat(content, ConfigSource::File);
    }
}

bool Config::parseJson(const std::string& content, ConfigSource source) {
    try {
        // Use nlohmann/json if available, otherwise fallback
        // For now, implement basic JSON parsing support
        auto json = nlohmann::json::parse(content);
        convertFromJson(json, source);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("JSON parsing failed: ") + e.what());
        return false;
    }
}

bool Config::parseYaml(const std::string& content, ConfigSource source) {
    try {
        // Use yaml-cpp if available
        // For now, implement basic YAML parsing support
        auto yaml = yaml-cpp::Load(content);
        convertFromYaml(yaml, source);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("YAML parsing failed: ") + e.what());
        return false;
    }
}

bool Config::parseKeyValueFormat(const std::string& content, ConfigSource source) {
    std::istringstream stream(content);
    std::string line;
    bool success = true;
    
    while (std::getline(stream, line)) {
        // Skip empty lines and comments
        size_t commentPos = line.find('#');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }
        
        line = trim(line);
        if (line.empty()) {
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
            
            if (!key.empty() && !value.empty()) {
                set(key, value, source);
            }
        } else {
            NLM_LOG_WARNING("Invalid config line (missing '='): " + line);
            success = false;
        }
    }
    
    return success;
}

void Config::convertFromJson(const nlohmann::json& json, ConfigSource source) {
    if (!json.is_object()) {
        NLM_LOG_WARNING("JSON config root is not an object");
        return;
    }
    
    for (const auto& [key, value] : json.items()) {
        ConfigValue configValue;
        
        if (value.is_string()) {
            configValue = value.get<std::string>();
        } else if (value.is_number_integer()) {
            configValue = value.get<int64_t>();
        } else if (value.is_number_float()) {
            configValue = value.get<double>();
        } else if (value.is_boolean()) {
            configValue = value.get<bool>();
        } else if (value.is_array()) {
            // Convert JSON array to vector
            std::vector<double> vec;
            for (const auto& item : value) {
                vec.push_back(item.get<double>());
            }
            configValue = vec;
        } else {
            NLM_LOG_WARNING(std::string("Unsupported JSON type for key: ") + key);
            continue;
        }
        
        set(key, configValue, source);
    }
}

void Config::convertFromYaml(const YAML::Node& yaml, ConfigSource source) {
    if (!yaml.IsMap()) {
        NLM_LOG_WARNING("YAML config root is not a map");
        return;
    }
    
    for (const auto& node : yaml) {
        std::string key = node.first.as<std::string>();
        const YAML::Node& value = node.second;
        
        ConfigValue configValue;
        
        if (value.IsScalar()) {
            std::string strValue = value.as<std::string>();
            if (strValue == "true" || strValue == "false") {
                configValue = strValue == "true";
            } else if (strValue.find('.') != std::string::npos) {
                configValue = std::stod(strValue);
            } else {
                // Try to parse as integer
                try {
                    configValue = std::stoll(strValue);
                } catch (...) {
                    configValue = strValue;
                }
            }
        } else if (value.IsSequence()) {
            std::vector<double> vec;
            for (const auto& item : value) {
                vec.push_back(item.as<double>());
            }
            configValue = vec;
        } else {
            NLM_LOG_WARNING(std::string("Unsupported YAML type for key: ") + key);
            continue;
        }
        
        set(key, configValue, source);
    }
}

std::string Config::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.push_back(ConfigEntry(key, value, source));
    }
}

void Config::remove(const std::string& key) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        pImpl->entries.erase(it);
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
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration Summary (" << pImpl->entries.size() << " entries):\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << ": ";
        
        try {
            if (std::holds_alternative<int64_t>(entry.value)) {
                oss << std::get<int64_t>(entry.value);
            } else if (std::holds_alternative<double>(entry.value)) {
                oss << std::get<double>(entry.value);
            } else if (std::holds_alternative<bool>(entry.value)) {
                oss << (std::get<bool>(entry.value) ? "true" : "false");
            } else if (std::holds_alternative<std::string>(entry.value)) {
                oss << std::get<std::string>(entry.value);
            } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
                oss << "[int array]";
            } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
                oss << "[double array]";
            } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
                oss << "[string array]";
            }
        } catch (...) {
            oss << "[invalid]";
        }
        
        oss << " (from " << configSourceToString(entry.source) << ")\n";
    }
    
    return oss.str();
}

std::string Config::configSourceToString(ConfigSource source) {
    switch (source) {
        case ConfigSource::Default: return "default";
        case ConfigSource::File: return "file";
        case ConfigSource::CommandLine: return "command line";
        case ConfigSource::Runtime: return "runtime";
        default: return "unknown";
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
