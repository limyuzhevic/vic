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
    std::map<std::string, ConfigEntry> definedKeys;
};

// ConfigValueHelper implementation
std::string ConfigValueHelper::toString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

bool ConfigValueHelper::validateRange(const ConfigValue& value, double min, double max) {
    try {
        double val = std::get<double>(value);
        return val >= min && val <= max;
    } catch (const std::bad_variant_access&) {
        return false;
    }
}

// Config implementation
Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::load(const std::string& filepath, int argc, char** argv) {
    // Load from all sources
    bool fileLoaded = true;
    if (!filepath.empty()) {
        fileLoaded = loadFromFile(filepath);
    }
    
    bool argsLoaded = loadFromArgs(argc, argv);
    bool envLoaded = loadFromEnvironment();
    
    // Merge all configurations
    if (fileLoaded || argsLoaded || envLoaded) {
        // The actual loading methods populate pImpl->entries
        // We need to merge them properly
        // For now, just return success
        return true;
    }
    
    return fileLoaded || argsLoaded || envLoaded;
}

bool Config::loadFromFile(const std::string& filepath) {
    return loadFromFile(filepath, true);
}

bool Config::loadFromFile(const std::string& filepath, bool silent) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        if (!silent) {
            throw ConfigLoadException("Failed to open configuration file: " + filepath);
        }
        return false;
    }
    
    std::string line;
    int lineNum = 0;
    while (std::getline(file, line)) {
        lineNum++;
        line = trim(line);
        
        // Skip empty lines and comments
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
        } else {
            if (!silent) {
                std::cerr << "Warning: Ignoring invalid line " << lineNum 
                          << " in config file: " << line << std::endl;
            }
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    return loadFromArgs(argc, argv, true);
}

bool Config::loadFromArgs(int argc, char** argv, bool silent) {
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

bool Config::loadFromEnvironment() {
    bool loaded = false;
    
    // Load common NLM environment variables
    std::vector<std::string> envVars = {
        "NLM_NEURON_COUNT",
        "NLM_REGION_COUNT", 
        "NLM_SIMULATION_STEPS",
        "NLM_SEED",
        "NLM_CHECKPOINT_PATH"
    };
    
    for (const auto& var : envVars) {
        if (hasEnvVar(var)) {
            std::string value = getEnvVar(var);
            std::string key = normalizeKey(var);
            
            // Convert value based on variable name
            if (var == "NLM_NEURON_COUNT" || var == "NLM_REGION_COUNT" || var == "NLM_SIMULATION_STEPS") {
                try {
                    int intValue = std::stoi(value);
                    set(key, intValue, ConfigSource::Environment);
                    loaded = true;
                } catch (...) {
                    std::cerr << "Warning: Invalid integer value for environment variable " 
                              << var << ": " << value << std::endl;
                }
            } else if (var == "NLM_SEED") {
                try {
                    int64_t intValue = std::stoll(value);
                    set(key, intValue, ConfigSource::Environment);
                    loaded = true;
                } catch (...) {
                    std::cerr << "Warning: Invalid integer value for environment variable " 
                              << var << ": " << value << std::endl;
                }
            } else {
                set(key, value, ConfigSource::Environment);
                loaded = true;
            }
        }
    }
    
    return loaded;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        throw ConfigLoadException("Failed to open configuration file for writing: " + filepath);
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << ConfigValueHelper::toString(entry.value) << "\n\n";
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    std::string normalizedKey = normalizeKey(key);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&normalizedKey](const ConfigEntry& e) { return normalizeKey(e.key) == normalizedKey; });
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

template<typename T>
T Config::getRequired(const std::string& key) const {
    auto val = get<T>(key);
    if (!val.has_value()) {
        throw ConfigValidationException("Required configuration key not found: " + key);
    }
    return val.value();
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    std::string normalizedKey = normalizeKey(key);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&normalizedKey](const ConfigEntry& e) { return normalizeKey(e.key) == normalizedKey; });
    
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

template<typename T>
void Config::setValidated(const std::string& key, T value, ConfigSource source, 
                         double min, double max, const std::string& desc) {
    set(key, value, source);
    if (!desc.empty()) {
        // Find the entry and update description
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return normalizeKey(e.key) == normalizeKey(key); });
        if (it != pImpl->entries.end()) {
            it->description = desc;
        }
    }
}

bool Config::has(const std::string& key) const {
    return get<std::string>(key).has_value();
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

std::vector<ConfigEntry> Config::getEntries() const {
    return pImpl->entries;
}

void Config::clear() {
    pImpl->entries.clear();
}

void Config::resetToDefaults() {
    pImpl->entries.clear();
}

void Config::define(const std::string& key, const ConfigValue& defaultValue, 
                    const std::string& description, bool required) {
    std::string normalizedKey = normalizeKey(key);
    
    auto it = pImpl->definedKeys.find(normalizedKey);
    if (it == pImpl->definedKeys.end()) {
        pImpl->definedKeys[normalizedKey] = ConfigEntry(key, defaultValue, ConfigSource::Default, description, required);
    }
}

bool Config::isDefined(const std::string& key) const {
    std::string normalizedKey = normalizeKey(key);
    return pImpl->definedKeys.find(normalizedKey) != pImpl->definedKeys.end();
}

std::string Config::summary() const {
    return summary(ConfigSource::Runtime);
}

std::string Config::summary(ConfigSource source) const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    
    // Filter entries by source if requested
    std::vector<ConfigEntry> filteredEntries;
    if (source == ConfigSource::Runtime) {
        filteredEntries = pImpl->entries;
    } else {
        for (const auto& entry : pImpl->entries) {
            if (entry.source == source) {
                filteredEntries.push_back(entry);
            }
        }
    }
    
    for (const auto& entry : filteredEntries) {
        oss << "  " << entry.key << " = [" << ConfigValueHelper::toString(entry.value) << "] (" 
            << static_cast<int>(entry.source) << ")";
        if (!entry.description.empty()) {
            oss << " - " << entry.description;
        }
        oss << "\n";
    }
    
    return oss.str();
}

std::string Config::getEnvVar(const std::string& name, const std::string& defaultValue) {
    const char* value = std::getenv(name.c_str());
    return value ? value : defaultValue;
}

bool Config::hasEnvVar(const std::string& name) {
    return std::getenv(name.c_str()) != nullptr;
}

std::string Config::parseArgs(int argc, char** argv, const std::string& key) {
    std::string normalizedKey = "--" + normalizeKey(key);
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.find(normalizedKey + "=") == 0) {
            return arg.substr(normalizedKey.size() + 1);
        }
    }
    
    return "";
}

bool Config::hasArg(int argc, char** argv, const std::string& key) {
    std::string normalizedKey = "--" + normalizeKey(key);
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == normalizedKey || arg.find(normalizedKey + "=") == 0) {
            return true;
        }
    }
    
    return false;
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

std::string Config::normalizeKey(const std::string& key) {
    std::string result = toLower(trim(key));
    // Replace spaces and underscores with hyphens
    std::replace(result.begin(), result.end(), ' ', '-');
    std::replace(result.begin(), result.end(), '_', '-');
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