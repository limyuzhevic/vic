#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

#ifdef _WIN32
    #include <filesystem>
#else
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/stat.h>
#endif

// Include JSON and YAML libraries if available
#if defined(HAVE_NLOHMANN_JSON)
    #include <nlohmann/json.hpp>
#endif

#if defined(HAVE_YAML_CPP)
    #include <yaml-cpp/yaml.h>
#endif

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
        return false;
    }
    
    // Try to detect file format based on extension and content
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json") {
        return loadFromJSON(filepath);
    } else if (ext == ".yaml" || ext == ".yml") {
        return loadFromYAML(filepath);
    } else {
        // Default to key=value format for .cfg, .txt, .ini, etc.
        return loadFromKeyValue(filepath);
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

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Key-value format loader (original implementation)
bool Config::loadFromKeyValue(const std::string& filepath) {
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
                ((value.front() == '\"' && value.back() == '\"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

#if defined(HAVE_NLOHMANN_JSON)
bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json j;
        file >> j;
        
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string key = it.key();
            
            if (it.value().is_string()) {
                set(key, it.value().get<std::string>(), ConfigSource::File);
            } else if (it.value().is_number_integer()) {
                set(key, it.value().get<int64_t>(), ConfigSource::File);
            } else if (it.value().is_number_float()) {
                set(key, it.value().get<double>(), ConfigSource::File);
            } else if (it.value().is_boolean()) {
                set(key, it.value().get<bool>(), ConfigSource::File);
            } else if (it.value().is_null()) {
                set(key, std::string(""), ConfigSource::File);
            } else {
                // For complex types, convert to JSON string
                set(key, it.value().dump(), ConfigSource::File);
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to parse JSON config file '" + filepath + "': " + std::string(e.what()));
        return false;
    }
}
#endif

#if defined(HAVE_YAML_CPP)
bool Config::loadFromYAML(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        YAML::Node doc = YAML::LoadFile(filepath);
        
        for (const auto& key : doc) {
            std::string keyName = key.first.as<std::string>();
            
            if (doc[keyName].IsScalar()) {
                set(keyName, doc[keyName].as<std::string>(), ConfigSource::File);
            } else if (doc[keyName].IsInteger()) {
                set(keyName, doc[keyName].as<int64_t>(), ConfigSource::File);
            } else if (doc[keyName].IsDouble()) {
                set(keyName, doc[keyName].as<double>(), ConfigSource::File);
            } else if (doc[keyName].IsBoolean()) {
                set(keyName, doc[keyName].as<bool>(), ConfigSource::File);
            } else if (doc[keyName].IsNull()) {
                set(keyName, std::string(""), ConfigSource::File);
            } else {
                // For complex types, convert to YAML string
                std::stringstream ss;
                ss << doc[keyName];
                set(keyName, ss.str(), ConfigSource::File);
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to parse YAML config file '" + filepath + "': " + std::string(e.what()));
        return false;
    }
}
#endif

// Configuration file generator
bool Config::generateConfigFile(const std::string& filepath, const std::string& format) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    if (format == "json" || format.empty()) {
        file << "{\n";
        bool first = true;
        for (const auto& entry : pImpl->entries) {
            if (!first) file << ",\n";
            first = false;
            
            file << "  \"" << entry.key << "\"": ";
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    file << arg;
                } else if constexpr (std::is_same_v<T, double>) {
                    file << arg;
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (arg ? "true" : "false");
                }
            }, entry.value);
        }
        file << "\n}\n";
    } else if (format == "yaml" || format == "yml") {
        file << "# NLM Configuration File\n";
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << entry.key << " = \"" << arg << "\"\n";
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    file << entry.key << " = " << arg << "\n";
                } else if constexpr (std::is_same_v<T, double>) {
                    file << entry.key << " = " << arg << "\n";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << entry.key << " = " << (arg ? "true" : "false") << "\n";
                }
            }, entry.value);
            file << "\n";
        }
    } else { // key=value format (default)
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << entry.key << " = \"" << arg << "\"\n";
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    file << entry.key << " = " << arg << "\n";
                } else if constexpr (std::is_same_v<T, double>) {
                    file << entry.key << " = " << arg << "\n";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << entry.key << " = " << (arg ? "true" : "false") << "\n";
                }
            }, entry.value);
            file << "\n";
        }
    }
    
    return true;
}

// Configuration validation
bool Config::validateConfig() const {
    // Basic validation - check for required keys
    const std::vector<std::string> requiredKeys = {
        "random_seed",
        "neuron_count",
        "region_count"
    };
    
    for (const auto& key : requiredKeys) {
        if (!has(key)) {
            NLM_LOG_WARNING("Configuration missing required key: '" + key + "' (using default)");
        }
    }
    
    // Validate value ranges
    if (has("neuron_count")) {
        auto neuronCount = get<int64_t>("neuron_count");
        if (neuronCount && *neuronCount < 1) {
            NLM_LOG_ERROR("Invalid neuron_count: " + std::to_string(*neuronCount));
            return false;
        }
    }
    
    if (has("connection_probability")) {
        auto prob = get<double>("connection_probability");
        if (prob && (*prob < 0.0 || *prob > 1.0)) {
            NLM_LOG_ERROR("Invalid connection_probability: " + std::to_string(*prob));
            return false;
        }
    }
    
    return true;
}

// Get configuration format from filepath
std::string Config::getConfigFormat(const std::string& filepath) {
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext;
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
