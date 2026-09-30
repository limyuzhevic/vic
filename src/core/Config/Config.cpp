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
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
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
    Validation::validateKey(key, "config_get");
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        NLM_THROW_AND_LOG(NLMConfigException, 
                         "Type mismatch for key: " + key + ", expected " + typeid(T).name(),
                         NLMException::Severity::ERROR,
                         0,
                         "config_get_type_mismatch");
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    Validation::validateKey(key, "config_getOr");
    
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    Validation::validateKey(key, "config_set");
    
    // Validate based on source
    if (source == ConfigSource::File) {
        Validation::validateFilepath(key, "config_set_file");
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        NLM_LOG_INFO("Config key updated: " + key);
    } else {
        // Validate the new entry
        if (Validation::isValidConfigState(key, value)) {
            pImpl->entries.emplace_back(key, value, source);
            NLM_LOG_INFO("Config key added: " + key);
        } else {
            NLM_THROW_AND_LOG(NLMConfigException,
                             "Invalid configuration state for key: " + key,
                             NLMException::Severity::ERROR,
                             static_cast<int>(NLMConfigException::ConfigError::VALIDATION_FAILED),
                             "config_set_validation");
        }
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    Validation::validateKey(key, "config_set_string");
    
    // Validate the value
    if (value.empty() && source == ConfigSource::Runtime) {
        NLM_THROW_AND_LOG(NLMConfigException,
                         "Empty string value cannot be used for runtime setting",
                         NLMException::Severity::WARNING,
                         static_cast<int>(NLMConfigException::ConfigError::VALIDATION_FAILED),
                         "config_set_empty_value");
    }
    
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    Validation::validateKey(key, "config_set_int");
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    Validation::validateKey(key, "config_set_double");
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    Validation::validateKey(key, "config_set_bool");
    set(key, ConfigValue(value), source);
}

bool Config::has(const std::string& key) const {
    Validation::validateKey(key, "config_has");
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    Validation::validateKey(key, "config_remove");
    
    auto newEnd = std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (newEnd != pImpl->entries.begin()) {
        pImpl->entries.erase(newEnd, pImpl->entries.end());
        NLM_LOG_INFO("Config key removed: " + key);
    } else {
        NLM_LOG_WARNING("Config key not found for removal: " + key);
    }
}

std::vector<std::string> Config::getKeys() const {
    return std::vector<std::string>(pImpl->entries.begin(), pImpl->entries.end());
}

void Config::clear() {
    if (!pImpl->entries.empty()) {
        size_t oldSize = pImpl->entries.size();
        pImpl->entries.clear();
        NLM_LOG_INFO("Config cleared " + std::to_string(oldSize) + " entries");
    }
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = ";
        // Safe variant visitor with error handling
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << " (" << static_cast<int>(entry.source) << ")\n";
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
