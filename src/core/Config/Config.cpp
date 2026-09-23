#include "Config.hpp"
#include "ConfigManager.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::shared_ptr<ConfigManager> configManager;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize ConfigManager as singleton
    initializeConfigManager();
    pImpl->configManager = std::shared_ptr<ConfigManager>(&getConfigManager(), [](ConfigManager*) {});
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    if (!pImpl->configManager) return false;
    
    bool success = pImpl->configManager->loadFromFile(filepath);
    
    // Sync Config entries with ConfigManager
    if (success) {
        syncWithConfigManager();
    }
    
    return success;
}

bool Config::loadFromArgs(int argc, char** argv) {
    if (!pImpl->configManager) return false;
    
    bool success = pImpl->configManager->loadFromCommandLine(argc, argv);
    
    // Sync Config entries with ConfigManager
    if (success) {
        syncWithConfigManager();
    }
    
    return success;
}

bool Config::saveToFile(const std::string& filepath) const {
    if (!pImpl->configManager) return false;
    return pImpl->configManager->saveToFile(filepath);
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

// Helper methods for managing parameter groups
void Config::syncWithConfigManager() {
    if (!pImpl->configManager) return;
    
    // Clear old entries
    pImpl->entries.clear();
    
    // Sync from ConfigManager
    auto paramNames = pImpl->configManager->getAllParameterNames();
    for (const auto& name : paramNames) {
        // Try to get parameter in various types
        std::optional<int> intVal = pImpl->configManager->getParameter<int>(name);
        if (intVal.has_value()) {
            pImpl->entries.emplace_back(name, ConfigValue(*intVal), ConfigSource::Default, "");
            continue;
        }
        
        std::optional<double> doubleVal = pImpl->configManager->getParameter<double>(name);
        if (doubleVal.has_value()) {
            pImpl->entries.emplace_back(name, ConfigValue(*doubleVal), ConfigSource::Default, "");
            continue;
        }
        
        std::optional<bool> boolVal = pImpl->configManager->getParameter<bool>(name);
        if (boolVal.has_value()) {
            pImpl->entries.emplace_back(name, ConfigValue(*boolVal), ConfigSource::Default, "");
            continue;
        }
        
        std::optional<std::string> stringVal = pImpl->configManager->getParameter<std::string>(name);
        if (stringVal.has_value()) {
            pImpl->entries.emplace_back(name, ConfigValue(*stringVal), ConfigSource::Default, "");
        }
    }
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
