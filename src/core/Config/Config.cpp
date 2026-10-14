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

#include "Config.hpp"
#include "JSONParser.hpp"
#include "Validator.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unique_ptr<JSONParser> jsonParser;
    std::unique_ptr<ConfigValidator> validator;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->jsonParser = std::make_unique<JSONParser>();
    pImpl->validator = std::make_unique<ConfigValidator>();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try JSON parser first
    if (pImpl->jsonParser->parseToConfig(filepath, std::shared_ptr<Config>(this))) {
        return true;
    }
    
    // Fall back to simple key=value format
    return loadFromSimpleFormat(filepath);
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

bool Config::loadFromSimpleFormat(const std::string& content) {
    std::istringstream stream(content);
    std::string line;
    while (std::getline(stream, line)) {
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

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Save in JSON format (preferred)
    nlohmann::json jsonData = toJSON();
    file << jsonData.dump(2);  // Pretty print with 2-space indentation
    
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

bool Config::validate() const {
    return pImpl->validator->validate(*this).empty();
}

std::vector<std::string> Config::getValidationErrors() const {
    return pImpl->validator->validate(*this);
}

nlohmann::json Config::toJSON() const {
    nlohmann::json jsonData;
    
    // Create nlm section
    jsonData["nlm"] = nlohmann::json::object();
    auto& nlmSection = jsonData["nlm"];
    
    // Parse brain configuration
    for (const auto& key : getKeys()) {
        if (key.find("brain.") == 0) {
            std::string subKey = key.substr(6);
            auto value = get<ConfigValue>(key);
            if (value) {
                nlmSection["brain"][subKey] = *value;
            }
        }
    }
    
    // Parse plasticity configuration
    for (const auto& key : getKeys()) {
        if (key.find("plasticity.") == 0) {
            std::string subKey = key.substr(12);
            auto value = get<ConfigValue>(key);
            if (value) {
                nlmSection["plasticity"][subKey] = *value;
            }
        }
    }
    
    // Parse neuromodulation configuration
    for (const auto& key : getKeys()) {
        if (key.find("neuromod.") == 0) {
            std::string subKey = key.substr(9);
            auto value = get<ConfigValue>(key);
            if (value) {
                nlmSection["neuromodulation"][subKey] = *value;
            }
        }
    }
    
    // Parse development configuration
    for (const auto& key : getKeys()) {
        if (key.find("development.") == 0) {
            std::string subKey = key.substr(12);
            auto value = get<ConfigValue>(key);
            if (value) {
                nlmSection["development"][subKey] = *value;
            }
        }
    }
    
    // Parse agent configuration
    for (const auto& key : getKeys()) {
        if (key.find("agent.") == 0) {
            std::string subKey = key.substr(6);
            auto value = get<ConfigValue>(key);
            if (value) {
                nlmSection["agent"][subKey] = *value;
            }
        }
    }
    
    // Parse environment configuration
    for (const auto& key : getKeys()) {
        if (key.find("environment.") == 0) {
            std::string subKey = key.substr(12);
            auto value = get<ConfigValue>(key);
            if (value) {
                nlmSection["environment"][subKey] = *value;
            }
        }
    }
    
    // Parse simulation configuration
    for (const auto& key : getKeys()) {
        if (key.find("simulation.") == 0) {
            std::string subKey = key.substr(11);
            auto value = get<ConfigValue>(key);
            if (value) {
                nlmSection["simulation"][subKey] = *value;
            }
        }
    }
    
    return jsonData;
}

bool Config::fromJSON(const nlohmann::json& json) {
    clear();
    
    if (!json.contains("nlm")) {
        // For backward compatibility, treat as simple key-value pairs
        for (auto it = json.begin(); it != json.end(); ++it) {
            std::string key = it.key();
            ConfigValue value;
            if (parseJSONValue(it.value(), value, key)) {
                set(key, value, ConfigSource::File);
            }
        }
    } else {
        // Parse nested structure
        const auto& nlmSection = json["nlm"];
        
        // Parse brain configuration
        if (nlmSection.contains("brain")) {
            parseNestedSection(nlmSection["brain"], "brain");
        }
        
        // Parse plasticity configuration
        if (nlmSection.contains("plasticity")) {
            parseNestedSection(nlmSection["plasticity"], "plasticity");
        }
        
        // Parse neuromodulation configuration
        if (nlmSection.contains("neuromodulation")) {
            parseNestedSection(nlmSection["neuromodulation"], "neuromod.");
        }
        
        // Parse development configuration
        if (nlmSection.contains("development")) {
            parseNestedSection(nlmSection["development"], "development.");
        }
        
        // Parse agent configuration
        if (nlmSection.contains("agent")) {
            parseNestedSection(nlmSection["agent"], "agent.");
        }
        
        // Parse environment configuration
        if (nlmSection.contains("environment")) {
            parseNestedSection(nlmSection["environment"], "environment.");
        }
        
        // Parse simulation configuration
        if (nlmSection.contains("simulation")) {
            parseNestedSection(nlmSection["simulation"], "simulation.");
        }
    }
    
    return true;
}

bool Config::applyTemplate(const std::string& templateName) {
    // Implementation would use TemplateManager
    // For now, just return false (placeholder)
    return false;
}

std::vector<std::string> Config::getAvailableTemplates() {
    // Implementation would use TemplateManager
    // For now, return empty list (placeholder)
    return std::vector<std::string>();
}

std::vector<std::string> Config::getConfigurationCategories() {
    // Implementation would use ConfigValidator
    // For now, return empty list (placeholder)
    return std::vector<std::string>();
}

bool Config::parseJSONValue(const nlohmann::json& jsonValue, ConfigValue& configValue,
                           const std::string& key) const {
    if (jsonValue.is_number_integer()) {
        configValue = jsonValue.get<int64_t>();
        return true;
    } else if (jsonValue.is_number_float()) {
        configValue = jsonValue.get<double>();
        return true;
    } else if (jsonValue.is_boolean()) {
        configValue = jsonValue.get<bool>();
        return true;
    } else if (jsonValue.is_string()) {
        configValue = jsonValue.get<std::string>();
        return true;
    } else if (jsonValue.is_array()) {
        // Parse array - convert to vector of doubles for simplicity
        std::vector<double> doubleArray;
        for (const auto& item : jsonValue) {
            if (item.is_number()) {
                doubleArray.push_back(item.get<double>());
            }
        }
        configValue = doubleArray;
        return true;
    }
    
    return false;
}

void Config::parseNestedSection(const nlohmann::json& jsonSection, const std::string& prefix) {
    for (auto it = jsonSection.begin(); it != jsonSection.end(); ++it) {
        std::string key = it.key();
        std::string fullKey = prefix + "." + key;
        
        ConfigValue value;
        if (parseJSONValue(it.value(), value, fullKey)) {
            set(fullKey, value, ConfigSource::File);
        }
    }
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

bool Config::isValidType(const std::string& key, const ConfigValue& value) {
    // Basic type checking - can be extended with validation rules
    if (key.find(".neuron_count") != std::string::npos ||
        key.find(".region_count") != std::string::npos) {
        return std::holds_alternative<int64_t>(value);
    } else if (key.find(".probability") != std::string::npos ||
               key.find(".rate") != std::string::npos) {
        return std::holds_alternative<double>(value);
    } else if (key.find(".enable") != std::string::npos ||
               key.find(".verbose") != std::string::npos) {
        return std::holds_alternative<bool>(value);
    } else if (key.find(".name") != std::string::npos ||
               key.find(".filename") != std::string::npos) {
        return std::holds_alternative<std::string>(value);
    }
    
    // Default: check if it's any of the supported types
    return std::holds_alternative<int>(value) ||
           std::holds_alternative<int64_t>(value) ||
           std::holds_alternative<double>(value) ||
           std::holds_alternative<bool>(value) ||
           std::holds_alternative<std::string>(value);
}

std::string Config::valueToString(const ConfigValue& value) {
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

