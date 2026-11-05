#include "Config.hpp"
#include "ConfigurationManager.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    bool loadedFromFile;
    std::string sourceFile;
    
    Impl() : loadedFromFile(false) {}
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_INFO("Config initialized");
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    try {
        // Use ConfigurationManager for better file loading
        auto& configManager = ConfigurationManager::getInstance();
        
        // Clear existing config
        clear();
        
        // Try to load using ConfigurationManager
        bool success = configManager.loadConfiguration(filepath);
        
        if (success) {
            pImpl->loadedFromFile = true;
            pImpl->sourceFile = filepath;
            
            // Copy values from ConfigurationManager to this config
            auto keys = configManager.getAllKeys();
            for (const auto& key : keys) {
                // Get the value and copy it
                auto value = configManager.getOptional<ConfigValue>(key);
                if (value.has_value()) {
                    pImpl->entries.emplace_back(key, value.value(), ConfigSource::File);
                }
            }
            
            NLM_LOG_INFO("Configuration loaded from file: " + filepath);
        } else {
            NLM_LOG_WARN("Failed to load configuration from file: " + filepath);
            
            // Fallback to legacy simple format
            std::ifstream file(filepath);
            if (!file.is_open()) {
                NLM_LOG_ERROR("Could not open configuration file: " + filepath);
                return false;
            }
            
            pImpl->loadedFromFile = true;
            pImpl->sourceFile = filepath;
            
            std::string line;
            int lineNum = 0;
            while (std::getline(file, line)) {
                lineNum++;
                line = trim(line);
                
                if (line.empty() || line[0] == '#' || line[0] == '/') {
                    continue;
                }
                
                size_t pos = line.find('=');
                if (pos != std::string::npos) {
                    std::string key = trim(line.substr(0, pos));
                    std::string value = trim(line.substr(pos + 1));
                    
                    if (value.size() >= 2 && 
                        ((value.front() == '"' && value.back() == '"') ||
                         (value.front() == '\'' && value.back() == '\''))) {
                        value = value.substr(1, value.size() - 2);
                    }
                    
                    set(key, value, ConfigSource::File);
                }
            }
            
            NLM_LOG_INFO("Configuration loaded using legacy parser from file: " + filepath);
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception during configuration file loading: " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    try {
        auto& configManager = ConfigurationManager::getInstance();
        return configManager.loadFromCommandLine(argc, argv);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception during command line argument parsing: " + std::string(e.what()));
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        // Use ConfigurationManager for saving
        auto& configManager = ConfigurationManager::getInstance();
        
        // Clear and reload from current config
        configManager.clear();
        
        for (const auto& entry : pImpl->entries) {
            configManager.set(entry.key, entry.value);
        }
        
        // Determine file format based on extension
        if (filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".json") {
            return configManager.saveToJSON(filepath);
        } else if (filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".yaml" ||
                   filepath.size() >= 4 && filepath.substr(filepath.size() - 4) == ".yml") {
            return configManager.saveToYAML(filepath);
        } else {
            return configManager.saveToFile(filepath);
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception during configuration file saving: " + std::string(e.what()));
        return false;
    }
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

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
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
    pImpl->loadedFromFile = false;
    pImpl->sourceFile.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries, loaded from: ";
    
    if (pImpl->loadedFromFile) {
        oss << pImpl->sourceFile << "):\n";
    } else {
        oss << "runtime memory)\n";
    }
    
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) oss << ", ";
                    if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                        oss << "\"" << arg[i] << "\"";
                    } else {
                        oss << arg[i];
                    }
                }
                oss << "]";
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

// Template instantiations (to avoid linker errors)
template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    // Numeric values convert between int/int64/double/float/etc, so a
    // value stored as double can be read as float and vice versa.
    return std::visit([](auto&& v) -> std::optional<T> {
        using V = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<V, T>) {
            return v;
        } else if constexpr (std::is_arithmetic_v<V> && std::is_arithmetic_v<T>) {
            return static_cast<T>(v);
        } else {
            return std::nullopt;
        }
    }, it->value);
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Template instantiations (moved from header to .cpp to avoid linker errors)
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<uint64_t> Config::get<uint64_t>(const std::string&) const;
template std::optional<float> Config::get<float>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template uint64_t Config::getOr<uint64_t>(const std::string&, const uint64_t&) const;
template float Config::getOr<float>(const std::string&, const float&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
