#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::string defaultConfigFile;
    std::vector<std::string> errors;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->defaultConfigFile = "configs/default.cfg";
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        addError("Failed to open configuration file: " + filepath);
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
        std::string section;
        std::string key;
        std::string value;
        if (!parseKeyValuePair(line, section, key, value)) {
            addError("Invalid line in configuration file " + filepath + ": " + line);
            continue;
        }
        
        if (!key.empty()) {
            set(section, key, value, ConfigSource::File);
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --section.key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string fullKey = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Parse section.key format
                size_t dotPos = fullKey.find('.');
                if (dotPos != std::string::npos) {
                    std::string section = fullKey.substr(0, dotPos);
                    std::string key = fullKey.substr(dotPos + 1);
                    set(section, key, value, ConfigSource::CommandLine);
                } else {
                    set("", fullKey, value, ConfigSource::CommandLine);
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            // Parse section.key format
            size_t dotPos = key.find('.');
            if (dotPos != std::string::npos) {
                std::string section = key.substr(0, dotPos);
                key = key.substr(dotPos + 1);
                set(section, key, value, ConfigSource::CommandLine);
            } else {
                set("", key, value, ConfigSource::CommandLine);
            }
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        addError("Failed to open configuration file for writing: " + filepath);
        return false;
    }
    
    // Write a simple key=value format with comments
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        
        // Write with section prefix if section is not empty
        if (!entry.section.empty()) {
            file << "[" << entry.section << "]\n";
            file << entry.key << " = ";
        } else {
            file << entry.key << " = ";
        }
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else {
                file << arg;
            }
        }, entry.value);
        file << "\n\n";
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& section, const std::string& key) const {
    std::string fullKey = section.empty() ? key : section + "." + key;
    return get<T>(fullKey);
}

template<typename T>
T Config::getOr(const std::string& section, const std::string& key, const T& defaultValue) const {
    std::optional<T> val = get<T>(section, key);
    return val.has_value() ? val.value() : defaultValue;
}

// Backward compatibility
template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    return get<T>("", key);
}

void Config::set(const std::string& section, const std::string& key, const ConfigValue& value, ConfigSource source) {
    std::string fullKey = section.empty() ? key : section + "." + key;
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&fullKey](const ConfigEntry& e) { return e.getFullKey() == fullKey; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(section, key, value, source);
    }
}

void Config::set(const std::string& section, const std::string& key, const std::string& value, ConfigSource source) {
    set(section, key, ConfigValue(value), source);
}

void Config::set(const std::string& section, const std::string& key, int value, ConfigSource source) {
    set(section, key, ConfigValue(value), source);
}

void Config::set(const std::string& section, const std::string& key, double value, ConfigSource source) {
    set(section, key, ConfigValue(value), source);
}

void Config::set(const std::string& section, const std::string& key, bool value, ConfigSource source) {
    set(section, key, ConfigValue(value), source);
}

bool Config::has(const std::string& section, const std::string& key) const {
    std::string fullKey = section.empty() ? key : section + "." + key;
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&fullKey](const ConfigEntry& e) { return e.getFullKey() == fullKey; });
}

void Config::remove(const std::string& section, const std::string& key) {
    std::string fullKey = section.empty() ? key : section + "." + key;
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&fullKey](const ConfigEntry& e) { return e.getFullKey() == fullKey; }),
        pImpl->entries.end()
    );
}

std::vector<std::string> Config::getKeys(const std::string& section) const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        if (section.empty() || entry.section == section) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

std::vector<std::string> Config::getSections() const {
    std::vector<std::string> sections;
    for (const auto& entry : pImpl->entries) {
        if (!entry.section.empty() && 
            std::find(sections.begin(), sections.end(), entry.section) == sections.end()) {
            sections.push_back(entry.section);
        }
    }
    return sections;
}

void Config::clear() {
    pImpl->entries.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  ";
        if (!entry.section.empty()) {
            oss << "[" << entry.section << "] ";
        }
        oss << entry.key << " = [";
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

bool Config::validateParameter(const ConfigParameter& param) const {
    if (param.required && !has(param.section, param.key)) {
        addError("Required parameter missing: " + param.section + "." + param.key);
        return false;
    }
    
    if (has(param.section, param.key)) {
        auto valueOpt = get<std::string>(param.section, param.key);
        if (valueOpt) {
            // For now, basic string validation
            // In the future, we could add more sophisticated type validation
            if (param.validator && !param.validator(param.defaultValue)) {
                addError("Parameter validation failed: " + param.section + "." + param.key);
                return false;
            }
        }
    }
    
    return true;
}

bool Config::validateAllParameters(const std::vector<ConfigParameter>& parameters) const {
    bool valid = true;
    for (const auto& param : parameters) {
        if (!validateParameter(param)) {
            valid = false;
        }
    }
    return valid;
}

void Config::setDefaultConfigFile(const std::string& filepath) {
    pImpl->defaultConfigFile = filepath;
}

std::string Config::getDefaultConfigFile() const {
    return pImpl->defaultConfigFile;
}

template<typename T>
std::string Config::valueToString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        oss << arg;
    }, value);
    return oss.str();
}

template<typename T>
std::optional<T> Config::stringToValue(const std::string& str) {
    std::istringstream iss(str);
    T value;
    if (iss >> value) {
        return value;
    }
    return std::nullopt;
}

void Config::addError(const std::string& error) {
    pImpl->errors.push_back(error);
}

std::vector<std::string> Config::getErrors() const {
    return pImpl->errors;
}

void Config::clearErrors() {
    pImpl->errors.clear();
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

bool Config::parseKeyValuePair(const std::string& line, std::string& section, std::string& key, std::string& value) {
    size_t equalsPos = line.find('=');
    if (equalsPos == std::string::npos) {
        return false;
    }
    
    key = trim(line.substr(0, equalsPos));
    value = trim(line.substr(equalsPos + 1));
    
    // Remove quotes if present
    if (value.size() >= 2 && 
        ((value.front() == '"' && value.back() == '"') ||
         (value.front() == '\'' && value.back() == '\''))) {
        value = value.substr(1, value.size() - 2);
    }
    
    // Parse section.key format
    size_t dotPos = key.find('.');
    if (dotPos != std::string::npos) {
        section = key.substr(0, dotPos);
        key = key.substr(dotPos + 1);
    } else {
        section = "";
    }
    
    return !key.empty();
}

ConfigValue Config::parseValue(const std::string& valueStr) {
    // Determine type based on content
    if (valueStr == "true" || valueStr == "false") {
        return valueStr == "true";
    }
    
    // Check if it's a number
    std::istringstream iss(valueStr);
    double doubleVal;
    int intVal;
    
    if (iss >> doubleVal) {
        // Check if it's an integer
        iss.clear();
        iss >> intVal;
        if (iss && iss.eof()) {
            return intVal;
        }
        return doubleVal;
    }
    
    // String (with or without quotes)
    return valueStr;
}

// Explicit template instantiations
template std::optional<int> Config::get<int>(const std::string&, const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&, const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&, const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&, const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&, const std::string&) const;

template int Config::getOr<int>(const std::string&, const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&, const std::string&) const;

} // namespace nlm
