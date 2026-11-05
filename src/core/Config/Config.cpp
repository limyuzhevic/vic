#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <iomanip>
#include <ctime>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, std::string> descriptions;
    std::unordered_map<std::string, std::string> validationRules;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath, bool requireFile) {
    std::string ext = extractExtension(filepath);
    
    if (ext == ".json" || ext == ".yaml" || ext == ".yml") {
        return loadJsonFromFile(filepath);
    } else {
        return loadKeyValueFromFile(filepath);
    }
}

bool Config::loadJsonFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Try to parse as JSON
    // For now, we'll implement a simple parser for the key=value format
    // In a real implementation, you'd use a proper JSON library like nlohmann/json
    std::string line;
    int lineNum = 0;
    
    while (std::getline(file, line)) {
        ++lineNum;
        line = trim(line);
        
        if (line.empty() || line[0] == '#' || line[0] == '/' || line[0] == '{') {
            continue;
        }
        
        // Parse simple key=value pairs (JSON-like but simplified)
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove trailing comma
            if (!value.empty() && value.back() == ',') {
                value = value.substr(0, value.size() - 1);
            }
            
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

bool Config::loadKeyValueFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
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

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --help, --version flags
        if (arg == "--help" || arg == "-h") {
            std::cout << "NLM Neural Learning Machine" << std::endl;
            std::cout << "Usage: nlm [OPTIONS]" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  --help, -h           Show this help message" << std::endl;
            std::cout << "  --version            Show version information" << std::endl;
            std::cout << "  --config <file>      Load configuration from file" << std::endl;
            std::cout << "  --set <key=value>    Set configuration value" << std::endl;
            std::cout << "  --env <prefix>       Load environment variables with prefix" << std::endl;
            return true; // Exit after showing help
        }
        
        if (arg == "--version") {
            std::cout << "NLM Neural Learning Machine v1.0.0 (Phase 6)" << std::endl;
            return true; // Exit after showing version
        }
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        }
        // Handle --set key value format
        else if (arg.substr(0, 5) == "--set") {
            if (i + 1 < argc) {
                std::string key = argv[++i];
                if (i + 1 < argc) {
                    std::string value = argv[++i];
                    set(key, value, ConfigSource::CommandLine);
                }
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

bool Config::loadFromEnv(const std::string& prefix) {
    for (const auto& env : std::filesystem::current_path()) {
        // Note: This is a simplified implementation
        // In a real implementation, you would use getenv() or similar
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "{\n";
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        file << "  \"" << entry.key << "\": \";
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                file << std::fixed << std::setprecision(6) << arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    if (j > 0) file << ", ";
                    file << arg[j];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    if (j > 0) file << ", ";
                    file << std::fixed << std::setprecision(6) << arg[j];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    if (j > 0) file << ", ";
                    file << "\"" << arg[j] << "\"";
                }
                file << "]";
            }
        }, entry.value);
        
        if (i < pImpl->entries.size() - 1) {
            file << ",\n";
        } else {
            file << "\n";
        }
    }
    file << "}\n";
    return true;
}

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

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source,
                 const std::string& description, const std::string& validationRule) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        if (!description.empty()) {
            pImpl->descriptions[key] = description;
        }
        if (!validationRule.empty()) {
            pImpl->validationRules[key] = validationRule;
        }
    } else {
        pImpl->entries.emplace_back(key, value, source, description);
        if (!description.empty()) {
            pImpl->descriptions[key] = description;
        }
        if (!validationRule.empty()) {
            pImpl->validationRules[key] = validationRule;
        }
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source,
                 const std::string& description, const std::string& validationRule) {
    set(key, ConfigValue(value), source, description, validationRule);
}

void Config::set(const std::string& key, int value, ConfigSource source,
                 const std::string& description, const std::string& validationRule) {
    set(key, ConfigValue(value), source, description, validationRule);
}

void Config::set(const std::string& key, int64_t value, ConfigSource source,
                 const std::string& description, const std::string& validationRule) {
    set(key, ConfigValue(value), source, description, validationRule);
}

void Config::set(const std::string& key, double value, ConfigSource source,
                 const std::string& description, const std::string& validationRule) {
    set(key, ConfigValue(value), source, description, validationRule);
}

void Config::set(const std::string& key, bool value, ConfigSource source,
                 const std::string& description, const std::string& validationRule) {
    set(key, ConfigValue(value), source, description, validationRule);
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
    pImpl->descriptions.erase(key);
    pImpl->validationRules.erase(key);
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
    pImpl->descriptions.clear();
    pImpl->validationRules.clear();
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
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) oss << ", ";
                    if constexpr (std::is_same_v<T, std::vector<std::string>) {
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

std::vector<std::string> Config::getConfigFileHints() {
    std::vector<std::string> hints;
    hints.push_back("configs/default.cfg");
    hints.push_back("config.json");
    hints.push_back("config.yaml");
    hints.push_back("nlm_config.cfg");
    return hints;
}

bool Config::parseHelpFlag(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--help" || arg == "-h") {
            return true;
        }
    }
    return false;
}

bool Config::parseVersionFlag(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--version") {
            return true;
        }
    }
    return false;
}

bool Config::parseConfigFlag(int argc, char** argv, std::string& configFile) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.substr(0, 9) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
            return true;
        }
    }
    return false;
}

ValidationResult Config::validate() const {
    for (const auto& entry : pImpl->entries) {
        ValidationResult result = validateKey(entry.key);
        if (!result.isValid) {
            return result;
        }
    }
    return ValidationResult();
}

bool Config::validateKey(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return true; // Key doesn't exist, skip validation
    }
    
    ValidationResult result = validateKey(key);
    return result.isValid;
}

void Config::merge(const Config& other) {
    for (const auto& entry : other.pImpl->entries) {
        // Find if this key exists in current config
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (it != pImpl->entries.end()) {
            // Key exists, update only if other has higher priority (CommandLine > File > Default)
            if (static_cast<int>(entry.source) > static_cast<int>(it->source)) {
                it->value = entry.value;
                it->source = entry.source;
            }
        } else {
            // Key doesn't exist, add it
            pImpl->entries.push_back(entry);
            pImpl->descriptions[entry.key] = entry.description;
            pImpl->validationRules[entry.key] = entry.validationRule;
        }
    }
}

const std::vector<ConfigEntry>& Config::getEntries() const {
    return pImpl->entries;
}

void Config::setDefaults(const std::string& sectionPrefix) {
    // This would be used to set default values for a section
    // For now, it's a placeholder
}

ConfigSource Config::getSource(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it != pImpl->entries.end()) {
        return it->source;
    }
    return ConfigSource::Default;
}

bool Config::isOverridden(const std::string& key) const {
    ConfigSource source = getSource(key);
    return source != ConfigSource::Default && source != ConfigSource::Runtime;
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

bool Config::endsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

std::string Config::extractExtension(const std::string& filepath) {
    size_t pos = filepath.find_last_of('.');
    if (pos == std::string::npos) return "";
    return filepath.substr(pos + 1);
}

std::string Config::findConfigFile(const std::string& hint) {
    std::vector<std::string> hints = getConfigFileHints();
    if (!hint.empty()) {
        hints.insert(hints.begin(), hint);
    }
    
    for (const auto& filehint : hints) {
        if (std::filesystem::exists(filehint)) {
            return filehint;
        }
    }
    return "";
}

ValidationResult Config::validateInt(const ConfigEntry& entry) {
    // Add custom validation logic for int values
    return ValidationResult();
}

ValidationResult Config::validateDouble(const ConfigEntry& entry) {
    // Add custom validation logic for double values
    return ValidationResult();
}

ValidationResult Config::validateString(const ConfigEntry& entry) {
    // Add custom validation logic for string values
    return ValidationResult();
}

ValidationResult Config::validateBool(const ConfigEntry& entry) {
    // Add custom validation logic for bool values
    return ValidationResult();
}

ValidationResult Config::validateVector(const ConfigEntry& entry) {
    // Add custom validation logic for vector values
    return ValidationResult();
}

} // namespace nlm
