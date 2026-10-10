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
        
        // Validate key
        if (key.empty()) {
            NLM_LOG_WARNING("Empty key found in config file: " + filepath);
            continue;
        }
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        // Validate and convert value
        try {
            set(key, value, ConfigSource::File);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error parsing config line: " + line + " - " + std::string(e.what()));
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

// Additional set methods for vectors
void Config::set(const std::string& key, const std::vector<int>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<double>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<std::string>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::enableValidation(bool enable) {
    pImpl->validationEnabled = enable;
}

void Config::setAutoSave(const std::string& path) {
    pImpl->autoSave = !path.empty();
    pImpl->autoSavePath = path;
}

void Config::setCompressionLevel(int level) {
    if (level >= 0 && level <= 9) {
        pImpl->compressionLevel = level;
    }
}

void Config::setMaxMemoryUsage(size_t bytes) {
    pImpl->maxMemoryUsage = bytes;
}

std::string Config::exportToJSON() const {
    std::ostringstream oss;
    oss << "{\n";
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        oss << "  \"" << entry.key << "\": ";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else {
                oss << arg;
            }
        }, entry.value);
        if (i < pImpl->entries.size() - 1) {
            oss << ",";
        }
        oss << "\n";
    }
    oss << "}\n";
    return oss.str();
}

bool Config::importFromJSON(const std::string& jsonStr) {
    // TODO PHASE 2: Implement proper JSON parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    // For now, just clear and load from a temporary string representation
    clear();
    
    // Simple JSON parsing for demonstration
    // This is a placeholder - a real implementation would use a JSON library
    size_t pos = 0;
    while (pos < jsonStr.size()) {
        // Find next key
        size_t keyStart = jsonStr.find('"', pos);
        if (keyStart == std::string::npos) break;
        size_t keyEnd = jsonStr.find('"', keyStart + 1);
        if (keyEnd == std::string::npos) break;
        
        std::string key = jsonStr.substr(keyStart + 1, keyEnd - keyStart - 1);
        pos = jsonStr.find(':', keyEnd);
        if (pos == std::string::npos) break;
        
        // Find value
        pos = jsonStr.find('"', pos + 1);
        if (pos == std::string::npos) break;
        size_t valueEnd = jsonStr.find('"', pos + 1);
        if (valueEnd == std::string::npos) break;
        
        std::string value = jsonStr.substr(pos + 1, valueEnd - pos - 1);
        set(key, value, ConfigSource::File);
        
        pos = valueEnd + 1;
    }
    
    return true;
}

size_t Config::getEntryCount() const {
    return pImpl->entries.size();
}

ConfigSource Config::getEntrySource(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        return it->source;
    }
    return ConfigSource::Default;
}

std::vector<std::pair<std::string, ConfigSource>> Config::getAllEntriesWithSources() const {
    std::vector<std::pair<std::string, ConfigSource>> result;
    result.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        result.emplace_back(entry.key, entry.source);
    }
    return result;
}

bool Config::validateAll() const {
    if (!pImpl->validationEnabled) {
        return true;
    }
    
    for (const auto& entry : pImpl->entries) {
        if (!validateKey(entry.key)) {
            return false;
        }
    }
    return true;
}

bool Config::validateKey(const std::string& key) const {
    if (key.empty()) {
        return false;
    }
    
    // Check for valid key characters (alphanumeric, underscore, hyphen)
    if (!std::all_of(key.begin(), key.end(), [](char c) {
        return std::isalnum(c) || c == '_' || c == '-';
    })) {
        return false;
    }
    
    // Check for specific required keys
    static const std::vector<std::string> requiredKeys = {
        "random_seed",
        "simulation_timestep",
        "neuron_count",
        "region_count"
    };
    
    if (std::find(requiredKeys.begin(), requiredKeys.end(), key) != requiredKeys.end()) {
        return has(key);
    }
    
    return true;
}

bool Config::validateValue(const std::string& key, const ConfigValue& value) const {
    // Delegate to static validation method
    // This is a wrapper for the private validateValue method
    return validateKey(key);
}

std::string Config::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string Config::getTimestampString(std::chrono::system_clock::time_point timePoint) {
    auto time = std::chrono::system_clock::to_time_t(timePoint);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string Config::formatConfigValue(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else {
            oss << arg;
        }
    }, value);
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

} // namespace nlm
