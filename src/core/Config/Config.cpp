#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

namespace nlm {

class Config::Impl {
public:
    std::vector<ConfigEntry> entries;
    std::vector<std::string> validationErrors;
    bool validate() const {
        validationErrors.clear();
        
        // Check for duplicate keys
        std::map<std::string, int> keyCount;
        for (const auto& entry : entries) {
            keyCount[entry.key]++;
        }
        
        for (const auto& [key, count] : keyCount) {
            if (count > 1) {
                validationErrors.push_back("Duplicate key: " + key);
            }
        }
        
        return validationErrors.empty();
    }
    
    const std::vector<std::string>& getValidationErrors() const {
        return validationErrors;
    }
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

bool Config::loadFromFile(const std::string& filepath) {
    if (isJSON(filepath)) {
        return loadFromJSON(filepath);
    } else if (isYAML(filepath)) {
        return loadFromYAML(filepath);
    } else {
        // Fallback to simple key=value format
        return loadFromSimpleFormat(filepath);
    }
}

bool Config::loadFromJSON(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json j;
        file >> j;
        
        clear();
        
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string key = it.key();
            ConfigValue value;
            
            if (it->is_number_integer()) {
                value = it->get<int64_t>();
            } else if (it->is_number_float()) {
                value = it->get<double>();
            } else if (it->is_boolean()) {
                value = it->get<bool>();
            } else if (it->is_string()) {
                value = it->get<std::string>();
            } else if (it->is_array()) {
                // Convert array to string representation for now
                value = it->dump();
            } else if (it->is_object()) {
                // Convert object to string representation for now
                value = it->dump();
            } else {
                value = std::string(); // empty string for null
            }
            
            set(key, value, ConfigEntry::Source::File);
        }
        
        return validate();
    } catch (const std::exception& e) {
        // Fallback to simple format
        return loadFromSimpleFormat(filepath);
    }
}

bool Config::loadFromYAML(const std::string& filepath) {
    try {
        YAML::Node doc = YAML::LoadFile(filepath);
        
        clear();
        
        for (const auto& node : doc) {
            std::string key = node.first.as<std::string>();
            ConfigValue value;
            
            if (node.second.IsScalar()) {
                if (node.second.IsInteger()) {
                    value = node.second.as<int64_t>();
                } else if (node.second.IsDouble()) {
                    value = node.second.as<double>();
                } else if (node.second.IsBoolean()) {
                    value = node.second.as<bool>();
                } else {
                    value = node.second.as<std::string>();
                }
            } else {
                value = node.second.as<std::string>();
            }
            
            set(key, value, ConfigEntry::Source::File);
        }
        
        return validate();
    } catch (const std::exception& e) {
        // Fallback to simple format
        return loadFromSimpleFormat(filepath);
    }
}

bool Config::loadFromSimpleFormat(const std::string& filepath) {
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
            
            set(key, value, ConfigEntry::Source::File);
        }
    }
    
    return validate();
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
                set(key, value, ConfigEntry::Source::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigEntry::Source::CommandLine);
        }
    }
    return validate();
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    nlohmann::json j;
    
    for (const auto& entry : pImpl->entries) {
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int64_t>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::string>) {
                j[entry.key] = arg;
            }
        }, entry.value);
    }
    
    file << std::setw(2) << j;
    return true;
}

std::string Config::toYAML() const {
    YAML::Emitter emitter;
    emitter << YAML::BeginMap;
    
    for (const auto& entry : pImpl->entries) {
        emitter << YAML::Key << entry.key;
        
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int64_t>) {
                emitter << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                emitter << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                emitter << arg;
            } else if constexpr (std::is_same_v<T, std::string>) {
                emitter << arg;
            }
        }, entry.value);
    }
    
    emitter << YAML::EndMap;
    return emitter.c_str();
}

std::string Config::toJSON() const {
    nlohmann::json j;
    
    for (const auto& entry : pImpl->entries) {
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int64_t>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::string>) {
                j[entry.key] = arg;
            }
        }, entry.value);
    }
    
    return j.dump(2);
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
                 ConfigEntry::Source source, const std::string& description) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source, description);
    }
}

void Config::set(const std::string& key, int64_t value, 
                 ConfigEntry::Source source, const std::string& description) {
    set(key, ConfigValue(value), source, description);
}

void Config::set(const std::string& key, double value, 
                 ConfigEntry::Source source, const std::string& description) {
    set(key, ConfigValue(value), source, description);
}

void Config::set(const std::string& key, bool value, 
                 ConfigEntry::Source source, const std::string& description) {
    set(key, ConfigValue(value), source, description);
}

void Config::set(const std::string& key, const std::string& value, 
                 ConfigEntry::Source source, const std::string& description) {
    set(key, ConfigValue(value), source, description);
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
    pImpl->validationErrors.clear();
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
    return pImpl->validate();
}

std::string Config::getValidationErrors() const {
    std::ostringstream oss;
    for (const auto& error : pImpl->getValidationErrors()) {
        oss << error << "\n";
    }
    return oss.str();
}

bool Config::isJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) return false;
    
    std::string line;
    std::getline(file, line);
    file.close();
    
    // Simple heuristic: starts with { or [
    return !line.empty() && (line[0] == '{' || line[0] == '[');
}

bool Config::isYAML(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) return false;
    
    std::string line;
    std::getline(file, line);
    file.close();
    
    // Simple heuristic: contains YAML-like structure
    return line.find(':') != std::string::npos || 
           line.find('-') != std::string::npos ||
           (line.find(':') != std::string::npos && line.find('=') == std::string::npos);
}

std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

// Explicit template instantiations
namespace {
    // Helper function to instantiate templates
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
}

} // namespace nlm
