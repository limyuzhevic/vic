#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()), schema_(nullptr), validationCallback_(nullptr), validationErrors_() {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    try {
        std::string format = detectFormat(filepath);
        
        if (format == "json") {
            pImpl->entries = loadJSON(filepath);
        } else if (format == "yaml" || format == "yml") {
            pImpl->entries = loadYAML(filepath);
        } else {
            pImpl->entries = loadSimple(filepath);
        }
        return true;
    } catch (const std::exception& e) {
        reportValidationError(std::string("Error loading config file ') + filepath + "': " + e.what());
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        } else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath, const std::string& format) const {
    try {
        std::string fmt = format;
        if (fmt == "auto") {
            fmt = detectFormat(filepath);
        }
        
        if (fmt == "json") {
            return saveJSON(filepath, pImpl->entries);
        } else if (fmt == "yaml" || fmt == "yml") {
            return saveYAML(filepath, pImpl->entries);
        } else {
            return saveSimple(filepath, pImpl->entries);
        }
    } catch (const std::exception& e) {
        reportValidationError(std::string("Error saving config file ') + filepath + "': " + e.what());
        return false;
    }
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

void Config::set(const std::string& key, const ConfigValue& value,
                 ConfigSource source, bool validate) {
    ConfigValue oldValue;
    bool existed = false;
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        oldValue = it->value;
        existed = true;
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
        oldValue = ConfigValue(); // Empty variant
    }
    
    // Validate if requested
    if (validate) {
        if (!validateValue(key, value)) {
            throw ConfigValidationError(key, "Value validation failed");
        }
    }
    
    // Call validation callback if set
    if (validationCallback_) {
        validationCallback_(ConfigChangeEvent(key, oldValue, value, source));
    }
}

void Config::set(const std::string& key, const std::string& value,
                 ConfigSource source, bool validate) {
    set(key, ConfigValue(value), source, validate);
}

void Config::set(const std::string& key, int value,
                 ConfigSource source, bool validate) {
    set(key, ConfigValue(value), source, validate);
}

void Config::set(const std::string& key, double value,
                 ConfigSource source, bool validate) {
    set(key, ConfigValue(value), source, validate);
}

void Config::set(const std::string& key, bool value,
                 ConfigSource source, bool validate) {
    set(key, ConfigValue(value), source, validate);
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
    validationErrors_.clear();
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

std::string Config::getSchemaSummary() const {
    if (!schema_) {
        return "No schema defined";
    }
    return schema_->getSchemaSummary();
}

std::string Config::diff(const Config& other) const {
    std::ostringstream oss;
    oss << "Configuration differences:\n";
    
    std::unordered_map<std::string, ConfigValue> otherMap;
    for (const auto& entry : other.pImpl->entries) {
        otherMap[entry.key] = entry.value;
    }
    
    for (const auto& entry : pImpl->entries) {
        auto it = otherMap.find(entry.key);
        if (it == otherMap.end()) {
            oss << "+ " << entry.key << " = " << valueToString(entry.value) << " (new)\n";
        } else if (it->second != entry.value) {
            oss << "~ " << entry.key << "\n";
            oss << "  from: " << valueToString(it->second) << "\n";
            oss << "  to:   " << valueToString(entry.value) << "\n";
        }
    }
    
    for (const auto& pair : otherMap) {
        if (!has(pair.first)) {
            oss << "- " << pair.first << " = " << valueToString(pair.second) << " (removed)\n";
        }
    }
    
    return oss.str();
}

bool Config::saveJSON(const std::string& filepath,
                     const std::vector<ConfigEntry>& entries) noexcept {
    try {
        nlohmann::json jsonObj;
        
        for (const auto& entry : entries) {
            jsonObj[entry.key] = {
                {"value", valueToString(entry.value)},
                {"source", static_cast<int>(entry.source)},
                {"description", entry.description}
            };
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << jsonObj.dump(2);
        return true;
    } catch (...) {
        return false;
    }
}

bool Config::saveYAML(const std::string& filepath,
                     const std::vector<ConfigEntry>& entries) noexcept {
    try {
        YAML::Emitter emitter;
        emitter << YAML::BeginMap;
        
        for (const auto& entry : entries) {
            emitter << YAML::Key << entry.key;
            emitter << YAML::Value;
            emitter << YAML::BeginMap;
            emitter << YAML::Key << "value";
            emitter << YAML::Value << valueToString(entry.value);
            emitter << YAML::Key << "source";
            emitter << YAML::Value << static_cast<int>(entry.source);
            if (!entry.description.empty()) {
                emitter << YAML::Key << "description";
                emitter << YAML::Value << entry.description;
            }
            emitter << YAML::EndMap;
        }
        
        emitter << YAML::EndMap;
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << emitter.c_str();
        return true;
    } catch (...) {
        return false;
    }
}

bool Config::saveSimple(const std::string& filepath,
                       const std::vector<ConfigEntry>& entries) noexcept {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        for (const auto& entry : entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = " << valueToString(entry.value) << "\n";
            file << "# Source: " << static_cast<int>(entry.source) << "\n\n";
        }
        
        return true;
    } catch (...) {
        return false;
    }
}

std::vector<ConfigEntry> Config::loadJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw ConfigException(ConfigErrorCode::FileNotFound,
                             "Cannot open JSON config file: " + filepath);
    }
    
    nlohmann::json jsonObj;
    file >> jsonObj;
    
    std::vector<ConfigEntry> entries;
    for (auto it = jsonObj.begin(); it != jsonObj.end(); ++it) {
        std::string key = it.key();
        std::string description = "";
        ConfigSource source = ConfigSource::Default;
        std::string valueStr = "";
        
        if (it->contains("value")) {
            valueStr = it->at("value").get<std::string>();
        }
        if (it->contains("source")) {
            source = static_cast<ConfigSource>(it->at("source").get<int>());
        }
        if (it->contains("description")) {
            description = it->at("description").get<std::string>();
        }
        
        ConfigValue value = stringToValue(valueStr);
        entries.emplace_back(key, value, source, description);
    }
    
    return entries;
}

std::vector<ConfigEntry> Config::loadYAML(const std::string& filepath) {
    YAML::Loader loader;
    YAML::Node doc = YAML::LoadFile(filepath);
    
    std::vector<ConfigEntry> entries;
    for (const auto& node : doc) {
        std::string key = node.first.as<std::string>();
        std::string description = "";
        ConfigSource source = ConfigSource::Default;
        std::string valueStr = "";
        
        if (node.second["value"]) {
            valueStr = node.second["value"].as<std::string>();
        }
        if (node.second["source"]) {
            source = static_cast<ConfigSource>(node.second["source"].as<int>());
        }
        if (node.second["description"]) {
            description = node.second["description"].as<std::string>();
        }
        
        ConfigValue value = stringToValue(valueStr);
        entries.emplace_back(key, value, source, description);
    }
    
    return entries;
}

std::vector<ConfigEntry> Config::loadSimple(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw ConfigException(ConfigErrorCode::FileNotFound,
                             "Cannot open config file: " + filepath);
    }
    
    std::vector<ConfigEntry> entries;
    std::string line;
    std::string currentDescription;
    
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty()) continue;
        
        if (line[0] == '#') {
            currentDescription = line.substr(1);
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
            
            entries.emplace_back(key, stringToValue(value), ConfigSource::File, currentDescription);
            currentDescription.clear();
        }
    }
    
    return entries;
}

std::string Config::detectFormat(const std::string& filepath) {
    if (fileExists(filepath)) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return "simple";
        }
        
        std::string firstLine;
        if (std::getline(file, firstLine)) {
            firstLine = trim(toLower(firstLine));
            if (startsWith(firstLine, "{") || startsWith(firstLine, "[")) {
                return "json";
            } else if (startsWith(firstLine, "---") || startsWith(firstLine, "key:")) {
                return "yaml";
            } else if (firstLine.find('=') != std::string::npos) {
                return "simple";
            }
        }
    }
    
    std::string ext = getFileExtension(filepath);
    if (ext == "json" || ext == "json5" || ext == "jsonc") {
        return "json";
    } else if (ext == "yaml" || ext == "yml") {
        return "yaml";
    }
    
    return "simple";
}

} // namespace nlm
