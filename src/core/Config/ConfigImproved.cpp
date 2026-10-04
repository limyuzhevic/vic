#pragma once

#include "ConfigImproved.hpp"
#include "Config.hpp"
#include <filesystem>

namespace nlm {

struct ConfigImproved::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ConfigSchemaEntry> schema;
};

ConfigImproved::ConfigImproved() : pImpl(std::make_unique<Impl>()) {}

ConfigImproved::~ConfigImproved() = default;

ConfigImproved::ConfigImproved(ConfigImproved&&) noexcept = default;

ConfigImproved& ConfigImproved::operator=(ConfigImproved&&) noexcept = default;

void ConfigImproved::setSchema(const std::vector<ConfigSchemaEntry>& schema) {
    pImpl->schema = schema;
}

bool ConfigImproved::loadFromFile(const std::string& filepath) {
    return parseIniFormat(filepath);
}

bool ConfigImproved::loadFromString(const std::string& content) {
    return parseIniFormat(content);
}

bool ConfigImproved::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --help flag
        if (arg == "--help") {
            std::cout << "\nNLM Configuration Options:\n" << std::endl;
            std::cout << "  --help                    Show this help message\n" << std::endl;
            std::cout << "  --config=<file>           Load configuration from file\n" << std::endl;
            std::cout << "  --verbose                 Enable verbose output\n" << std::endl;
            std::cout << "  --debug                   Enable debug output\n" << std::endl;
            std::cout << "  --experiment=<name>       Specify experiment to run\n" << std::endl;
            std::cout << "  --key=<value>             Set configuration key (value must be quoted if spaces)\n" << std::endl;
            std::cout << "  --key value               Alternative format for command line arguments\n" << std::endl;
            return false;  // Exit after showing help
        }
        
        // Handle --verbose flag
        if (arg == "--verbose") {
            set("verbose", true, ConfigSource::CommandLine);
            continue;
        }
        
        // Handle --debug flag
        if (arg == "--debug") {
            set("debug", true, ConfigSource::CommandLine);
            continue;
        }
        
        // Handle --config flag
        if (arg.find("--config=") == 0) {
            std::string configFile = arg.substr(9);  // Skip "--config="
            if (!configFile.empty()) {
                if (loadFromFile(configFile)) {
                    std::cout << "Loaded configuration from: " << configFile << std::endl;
                } else {
                    std::cerr << "Warning: Failed to load configuration from " << configFile << std::endl;
                }
            }
            continue;
        }
        
        // Handle --experiment flag
        if (arg.find("--experiment=") == 0) {
            std::string experiment = arg.substr(13);  // Skip "--experiment="
            set("experiment", experiment, ConfigSource::CommandLine);
            continue;
        }
        
        // Handle --key=value format (general key-value pairs)
        if (arg.find("--") == 0 && arg.find("=") != std::string::npos) {
            size_t pos = arg.find("=");
            std::string key = arg.substr(2, pos - 2);
            std::string value = arg.substr(pos + 1);
            set(key, value, ConfigSource::CommandLine);
            continue;
        }
        
        // Handle -key value format (alternative format)
        if (arg[0] == '-' && arg.size() > 1 && arg[1] != '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
            continue;
        }
    }
    
    return true;
}

bool ConfigImproved::saveToFile(const std::string& filepath) const {
    return saveToIniFile(filepath);
}

bool ConfigImproved::saveToIniFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << formatValue(entry.value) << "\n\n";
    }
    
    return true;
}

template<typename T>
std::optional<T> ConfigImproved::get(const std::string& key) const {
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
T ConfigImproved::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void ConfigImproved::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
    } else {
        pImpl->entries.emplace_back(key, value, "");
    }
}

void ConfigImproved::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void ConfigImproved::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void ConfigImproved::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void ConfigImproved::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

ValidationResult ConfigImproved::validate(const std::string& key) const {
    // Check schema if available
    if (!pImpl->schema.empty()) {
        for (const auto& schemaEntry : pImpl->schema) {
            if (schemaEntry.key == key) {
                auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                    [&key](const ConfigEntry& e) { return e.key == key; });
                
                if (it != pImpl->entries.end()) {
                    return validateValue(key, it->value);
                } else {
                    if (schemaEntry.required) {
                        return ValidationResult::error("Required configuration key '" + key + "' is missing");
                    }
                }
            }
        }
    }
    
    return ValidationResult::success();
}

ValidationResult ConfigImproved::validateAll() const {
    for (const auto& entry : pImpl->entries) {
        ValidationResult result = validate(entry.key);
        if (!result.valid) {
            return result;
        }
    }
    
    // Check for missing required keys
    for (const auto& schemaEntry : pImpl->schema) {
        if (schemaEntry.required && !has(schemaEntry.key)) {
            return ValidationResult::error("Required configuration key '" + schemaEntry.key + "' is missing");
        }
    }
    
    return ValidationResult::success();
}

bool ConfigImproved::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void ConfigImproved::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
}

std::vector<std::string> ConfigImproved::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void ConfigImproved::clear() {
    pImpl->entries.clear();
}

std::string ConfigImproved::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else {
                oss << std::to_string(arg);
            }
        }, entry.value);
        oss << "]\n";
    }
    return oss.str();
}

std::string ConfigImproved::generateUsage(const std::string& programName, 
                                           const std::vector<ConfigSchemaEntry>& schema) {
    std::ostringstream oss;
    oss << "Usage: " << programName << " [options]\n\n";
    oss << "Options:\n";
    oss << "  --help                    Show this help message\n";
    oss << "  --config=<file>           Load configuration from file\n";
    oss << "  --verbose                 Enable verbose output\n";
    oss << "  --debug                   Enable debug output\n";
    oss << "  --experiment=<name>       Specify experiment to run\n";
    oss << "  --key=<value>             Set configuration key (value must be quoted if spaces)\n";
    oss << "  --key value               Alternative format for command line arguments\n";
    oss << "\n";
    
    if (!schema.empty()) {
        oss << "Configuration parameters:\n";
        for (const auto& entry : schema) {
            oss << "  --" << entry.key << "=<value>" << "       " << entry.description << "\n";
            if (!entry.defaultValue.has_value()) {
                oss << "                          (required)" << std::endl;
            } else {
                oss << "                          (default: " << formatAnyToString(entry.defaultValue) << ")" << std::endl;
            }
        }
    }
    
    return oss.str();
}

ConfigFormat ConfigImproved::detectFormat(const std::string& filepath) {
    std::filesystem::path path(filepath);
    std::string extension = path.extension().string();
    
    if (extension == ".cfg" || extension == ".txt" || extension == ".ini" || extension == ".CFG" || extension == ".INI") {
        return ConfigFormat::Ini;
    } else {
        return ConfigFormat::PlainText;
    }
}

std::string ConfigImproved::formatValue(const ConfigValue& value) {
    return std::visit([](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return "\"" + arg + "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg ? "true" : "false";
        } else {
            return std::to_string(arg);
        }
    }, value);
}

ConfigValue ConfigImproved::parseValue(const std::string& str, const std::string& typeHint) {
    if (str.empty()) {
        return std::string("");
    }
    
    // Try to detect type
    if (str == "true" || str == "false") {
        return stringToBool(str);
    } else if (str.front() == '"' && str.back() == '"') {
        return str.substr(1, str.size() - 2);
    } else {
        // Try numeric types
        try {
            if (str.find('.') != std::string::npos) {
                return std::stod(str);
            } else {
                return std::stoi(str);
            }
        } catch (...) {
            // Not a number, return as string
            return str;
        }
    }
}

ValidationResult ConfigImproved::validateValue(const std::string& key, const ConfigValue& value) const {
    for (const auto& schemaEntry : pImpl->schema) {
        if (schemaEntry.key == key) {
            if (schemaEntry.validator) {
                return schemaEntry.validator(value);
            }
        }
    }
    return ValidationResult::success();
}

std::string ConfigImproved::formatAnyToString(const std::any& value) {
    if (value.type() == typeid(int)) {
        return std::to_string(std::any_cast<int>(value));
    } else if (value.type() == typeid(double)) {
        return std::to_string(std::any_cast<double>(value));
    } else if (value.type() == typeid(bool)) {
        return std::any_cast<bool>(value) ? "true" : "false";
    } else if (value.type() == typeid(std::string)) {
        return "\"" + std::any_cast<std::string>(value) + "\"";
    } else {
        return "";
    }
}

std::string ConfigImproved::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string ConfigImproved::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

bool ConfigImproved::stringToBool(const std::string& str) {
    std::string lower = toLower(trim(str));
    return lower == "true" || lower == "yes" || lower == "1" || lower == "on";
}

std::string ConfigImproved::boolToString(bool value) {
    return value ? "true" : "false";
}

bool ConfigImproved::parseIniFormat(const std::string& content) {
    std::istringstream stream(content);
    std::string line;
    
    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == ';') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes
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

bool ConfigImproved::parseIniFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == ';') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes
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

// Explicit template instantiations
template std::optional<int> ConfigImproved::get<int>(const std::string&) const;
template std::optional<int64_t> ConfigImproved::get<int64_t>(const std::string&) const;
template std::optional<double> ConfigImproved::get<double>(const std::string&) const;
template std::optional<bool> ConfigImproved::get<bool>(const std::string&) const;
template std::optional<std::string> ConfigImproved::get<std::string>(const std::string&) const;

template int ConfigImproved::getOr<int>(const std::string&, const int&) const;
template int64_t ConfigImproved::getOr<int64_t>(const std::string&, const int64_t&) const;
template double ConfigImproved::getOr<double>(const std::string&, const double&) const;
template bool ConfigImproved::getOr<bool>(const std::string&, const bool&) const;
template std::string ConfigImproved::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm