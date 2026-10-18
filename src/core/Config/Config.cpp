#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iomanip>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() {
    delete pImpl;
}

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Determine file format based on extension
    std::string ext = filepath.substr(filepath.find_last_of('.') + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == "json" || ext == "json") {
        return loadFromJSONFile(file);
    } else if (ext == "yaml" || ext == "yml") {
        return loadFromYAMLFile(file);
    } else {
        return loadFromSimpleFormat(file);
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
        file << entry.key << " = ";
        // Use std::visit to properly serialize the variant value
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << std::setprecision(6) << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << arg[i] << "\"";
                }
                file << "]";
            } else {
                file << arg;
            }
        }, entry.value);
        file << "\n";
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

// Type conversion utilities
bool ConfigConverter::stringToValue(const std::string& str, ConfigValue& out) {
    // Try to determine type
    if (str == "true" || str == "false") {
        out = (str == "true");
        return true;
    }
    
    try {
        size_t pos;
        int intVal = std::stoi(str, &pos);
        if (pos == str.size()) {
            out = intVal;
            return true;
        }
        
        int64_t int64Val = std::stoll(str, &pos);
        if (pos == str.size()) {
            out = int64Val;
            return true;
        }
        
        double doubleVal = std::stod(str, &pos);
        if (pos == str.size()) {
            out = doubleVal;
            return true;
        }
        
        // Check for array format [1,2,3]
        if (str.front() == '[' && str.back() == ']') {
            std::string content = str.substr(1, str.size() - 2);
            std::vector<double> numbers;
            std::stringstream ss(content);
            std::string token;
            while (std::getline(ss, token, ',')) {
                try {
                    double num = std::stod(trim(token));
                    numbers.push_back(num);
                } catch (...) {
                    return false;
                }
            }
            if (!numbers.empty()) {
                out = numbers;
                return true;
            }
        }
        
        out = str;
        return true;
    } catch (...) {
        out = str;
        return true;
    }
}

std::string ConfigConverter::valueToString(const ConfigValue& value) {
    std::stringstream ss;
    std::visit([&ss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            ss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            ss << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, int> || 
                             std::is_same_v<T, int64_t> ||
                             std::is_same_v<T, double>) {
            ss << arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            ss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << arg[i];
            }
            ss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            ss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << std::setprecision(6) << arg[i];
            }
            ss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            ss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << "\"" << arg[i] << "\"";
            }
            ss << "]";
        } else {
            ss << "unknown";
        }
    }, value);
    return ss.str();
}

bool ConfigConverter::isNumericType(const ConfigValue& value) {
    return std::holds_alternative<int>(value) ||
           std::holds_alternative<int64_t>(value) ||
           std::holds_alternative<double>(value);
}

std::string ConfigConverter::getTypeName(const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) return "int";
    if (std::holds_alternative<int64_t>(value)) return "int64_t";
    if (std::holds_alternative<double>(value)) return "double";
    if (std::holds_alternative<bool>(value)) return "bool";
    if (std::holds_alternative<std::string>(value)) return "string";
    if (std::holds_alternative<std::vector<int>>(value)) return "vector<int>";
    if (std::holds_alternative<std::vector<double>>(value)) return "vector<double>";
    if (std::holds_alternative<std::vector<std::string>>(value)) return "vector<string>";
    return "unknown";
}

// Configuration validation
bool ConfigValidator::validate(const Config& config) {
    // Basic validation - could be extended with schema validation
    for (const auto& entry : config.getKeys()) {
        auto value = config.get<std::string>(entry);
        if (!value.has_value()) {
            return false;
        }
    }
    return true;
}

std::vector<std::string> ConfigValidator::getValidationErrors(const Config& config) {
    std::vector<std::string> errors;
    for (const auto& entry : config.getKeys()) {
        auto value = config.get<std::string>(entry);
        if (!value.has_value()) {
            errors.push_back("Missing value for key: " + entry);
        }
    }
    return errors;
}

// Enhanced Config methods implementation

// Load from JSON file
bool Config::loadFromJSONFile(std::ifstream& file) {
    // For now, treat JSON files as simple key=value format
    // This is a placeholder - would need nlohmann/json for proper JSON parsing
    return loadFromSimpleFormat(file);
}

// Load from YAML file
bool Config::loadFromYAMLFile(std::ifstream& file) {
    // For now, treat YAML files as simple key=value format
    // This is a placeholder - would need yaml-cpp for proper YAML parsing
    return loadFromSimpleFormat(file);
}

// Load from simple format file
bool Config::loadFromSimpleFormat(std::ifstream& file) {
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

// Export to JSON format
std::string Config::exportToJSON() const {
    std::ostringstream oss;
    oss << "{\n";
    
    bool first = true;
    for (const auto& entry : pImpl->entries) {
        if (!first) oss << ",\n";
        oss << "  \"" << entry.key << "\": " << serializeConfigValue(entry.value);
        first = false;
    }
    
    oss << "\n}";
    return oss.str();
}

// Export to YAML format
std::string Config::exportToYAML() const {
    std::ostringstream oss;
    oss << "# NLM Configuration\n\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "# " << entry.description << "\n";
        oss << entry.key << ": " << serializeConfigValue(entry.value) << "\n";
    }
    
    return oss.str();
}

// Parse file format and load
bool Config::parseFileFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open configuration file: " + filepath);
        return false;
    }
    
    std::string ext = filepath.substr(filepath.find_last_of('.') + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    bool success = false;
    if (ext == "json") {
        success = loadFromJSONFile(file);
    } else if (ext == "yaml" || ext == "yml") {
        success = loadFromYAMLFile(file);
    } else {
        success = loadFromSimpleFormat(file);
    }
    
    if (success) {
        NLM_LOG_INFO("Loaded configuration from: " + filepath);
    } else {
        NLM_LOG_ERROR("Failed to parse configuration file: " + filepath);
    }
    
    return success;
}

// Add description to entries
void Config::addDescription(const std::string& key, const std::string& description) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it != pImpl->entries.end()) {
        it->description = description;
    }
}

// Statistics methods
size_t Config::getEntryCount() const {
    return pImpl->entries.size();
}

size_t Config::getEntryCountBySource(ConfigSource source) const {
    return std::count_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&source](const ConfigEntry& e) { return e.source == source; });
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
