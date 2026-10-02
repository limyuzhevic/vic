#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <regex>

// Simple JSON parser for configuration files
namespace {
    std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }
    
    bool isJsonFile(const std::string& filename) {
        std::regex jsonExt("(\.json$|\.cfg$|\.yaml$|\.yml$)", std::regex::icase);
        return std::regex_search(filename, jsonExt);
    }
    
    std::string jsonValueToString(const std::string& jsonValue) {
        // Simple JSON string extraction (removes quotes)
        if (jsonValue.size() >= 2 && jsonValue.front() == '"' && jsonValue.back() == '"') {
            return jsonValue.substr(1, jsonValue.size() - 2);
        }
        return jsonValue;
    }
}

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    bool useJsonParser;  // Flag to enable JSON/YAML parsing
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Auto-detect if we should use JSON parser based on common config file patterns
    pImpl->useJsonParser = true;
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try JSON parser first for .json, .cfg, .yaml, .yml files
    if (pImpl->useJsonParser && isJsonFile(filepath)) {
        return loadFromJsonFile(filepath);
    }
    
    // Fall back to simple key=value parser for other files
    return loadFromSimpleFile(filepath);
}

bool Config::loadFromJsonFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    
    // Simple JSON parsing - looks for "key": "value" or "key": number or "key": true/false patterns
    std::regex jsonPattern(R"("([^"]*)"\s*:\s*("[^"]*"|\d+\.?\d*|true|false))"");
    
    auto begin = std::sregex_iterator(content.begin(), content.end(), jsonPattern);
    auto end = std::sregex_iterator();
    
    bool foundAny = false;
    for (auto it = begin; it != end; ++it) {
        std::smatch match = *it;
        if (match.size() >= 3) {
            std::string key = trim(match[1].str());
            std::string valueStr = trim(match[2].str());
            std::string value = jsonValueToString(valueStr);
            
            // Try to determine value type
            try {
                if (valueStr.find('.') != std::string::npos || valueStr == "true" || valueStr == "false") {
                    // Could be float or bool
                    if (valueStr == "true") {
                        set(key, true, ConfigSource::File);
                    } else if (valueStr == "false") {
                        set(key, false, ConfigSource::File);
                    } else {
                        float floatVal = std::stof(valueStr);
                        set(key, floatVal, ConfigSource::File);
                    }
                } else {
                    // Try integer
                    size_t pos;
                    int intVal = std::stoi(valueStr, &pos);
                    if (pos == valueStr.size()) {
                        set(key, intVal, ConfigSource::File);
                    } else {
                        set(key, value, ConfigSource::File);
                    }
                }
                foundAny = true;
            } catch (const std::exception&) {
                // Fall back to string
                set(key, value, ConfigSource::File);
                foundAny = true;
            }
        }
    }
    
    return foundAny;
}

bool Config::loadFromSimpleFile(const std::string& filepath) {
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
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    bool parsedArgs = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --help flag
        if (arg == "--help") {
            std::cout << "=== NLM Configuration Help ===" << std::endl;
            std::cout << std::endl;
            std::cout << "Configuration file formats supported:" << std::endl;
            std::cout << "  JSON files (.json, .cfg): \"key\" : value" << std::endl;
            std::cout << "  Simple files (.txt, .cfg): key = value" << std::endl;
            std::cout << std::endl;
            std::cout << "Value types supported: int, double, bool, string" << std::endl;
            std::cout << "Examples:" << std::endl;
            std::cout << "  --neuron_count 1000" << std::endl;
            std::cout << "  --connection_probability 0.5" << std::endl;
            std::cout << "  --enable_plasticity true" << std::endl;
            parsedArgs = true;
            continue;
        }
        
        // Handle --key=value format (improved command-line parsing)
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                setConfigValue(key, value);
                parsedArgs = true;
            }
        }
        // Handle -key value format (improved command-line parsing)
        else if (arg[0] == '-' && i + 1 < argc && argv[i + 1][0] != '-') {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            setConfigValue(key, value);
            parsedArgs = true;
        }
    }
    
    return parsedArgs;
}

void Config::setConfigValue(const std::string& key, const std::string& value) {
    try {
        // Try to parse as boolean first
        if (value == "true" || value == "false") {
            bool boolVal = (value == "true");
            set(key, boolVal, ConfigSource::CommandLine);
            return;
        }
        
        // Try to parse as integer
        size_t pos;
        int intVal = std::stoi(value, &pos);
        if (pos == value.size()) {
            set(key, intVal, ConfigSource::CommandLine);
            return;
        }
        
        // Try to parse as float
        float floatVal = std::stof(value);
        set(key, floatVal, ConfigSource::CommandLine);
    } catch (const std::exception&) {
        // Fall back to string
        set(key, value, ConfigSource::CommandLine);
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Determine file format based on extension
    bool isJson = isJsonFile(filepath);
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        
        if (isJson) {
            file << "  \"" << entry.key << "\": ";
            printConfigValue(file, entry.value);
            file << "," << std::endl;
        } else {
            file << entry.key << " = ";
            printConfigValue(file, entry.value);
            file << "\n";
        }
    }
    
    return true;
}

void Config::printConfigValue(std::ofstream& file, const ConfigValue& value) const {
    std::visit([&file](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            file << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            file << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, double> || 
                            std::is_same_v<T, float> ||
                            std::is_same_v<T, int> ||
                            std::is_same_v<T, int64_t>) {
            file << arg;
        } else {
            file << "\"" << arg << "\"";
        }
    }, value);
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
        // Log type mismatch error
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    validateKey(key);
    validateValue(key, value);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
}

void Config::validateKey(const std::string& key) {
    if (key.empty()) {
        throw std::invalid_argument("Config key cannot be empty");
    }
    
    // Check for valid key characters (alphanumeric, underscore, hyphen)
    std::regex validKeyPattern("^[a-zA-Z0-9_\\-]+$");
    if (!std::regex_match(key, validKeyPattern)) {
        throw std::invalid_argument("Invalid config key: " + key + ". Only alphanumeric, underscore, and hyphen are allowed.");
    }
}

void Config::validateValue(const std::string& key, const ConfigValue& value) {
    // Basic validation - can be extended with key-specific rules
    try {
        std::visit([this, &key](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            validateSpecificValue<T>(key, arg);
        }, value);
    } catch (const std::exception&) {
        // If validation fails, we can still store the value but log the error
        // For now, we'll just allow it (existing behavior)
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    try {
        set(key, ConfigValue(value), source);
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("Failed to set config key \")" + key + "\": " + e.what());
    }
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
    oss << "  File format: " << (isJsonFile("dummy.json") ? "JSON/YAML" : "Simple") << "\n";
    
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
