#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <map>
#include <chrono>
#include <regex>
#include <iostream>
#include <stdexcept>

namespace nlm {

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <map>
#include <chrono>
#include <regex>
#include <iostream>
#include <stdexcept>

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <map>
#include <chrono>
#include <regex>
#include <iostream>
#include <stdexcept>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, ConfigDefinition> definitions;
    static std::unordered_map<std::string, ConfigDefinition> globalDefinitions;
    static ConfigVersion currentVersion;
    std::map<ConfigSource, int> sourcePriority;
    
    Impl() {
        // Set source priority order (lower number = higher priority)
        sourcePriority[ConfigSource::Runtime] = 0;
        sourcePriority[ConfigSource::CommandLine] = 1;
        sourcePriority[ConfigSource::File] = 2;
        sourcePriority[ConfigSource::Default] = 3;
    }
};

std::unordered_map<std::string, ConfigDefinition> Config::Impl::globalDefinitions;
ConfigVersion Config::Impl::currentVersion = ConfigVersion(1, 0, 0);

Config::Config() : pImpl(std::make_unique<Impl>()) {
    if (Impl::globalDefinitions.empty()) {
        initializeDefaultDefinitions();
    }
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

void Config::initializeFromDefinitions(const std::vector<ConfigDefinition>& definitions) {
    for (const auto& def : definitions) {
        Impl::globalDefinitions[def.key] = def;
    }
}

bool Config::loadFromFile(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            Logger::getGlobal().warning("Config file not found: " + filepath);
            return false;
        }
        
        // TODO: Implement proper JSON/YAML parser
        // PLACEHOLDER - Phase 1 uses a simple key=value format
        
        std::string line;
        int lineNum = 0;
        while (std::getline(file, line)) {
            lineNum++;
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
                
                if (!set(key, value, ConfigSource::File)) {
                    Logger::getGlobal().warning("Failed to parse config line " + std::to_string(lineNum) + 
                                               " in file " + filepath + ": " + key);
                }
            } else {
                Logger::getGlobal().warning("Invalid config format line " + std::to_string(lineNum) + 
                                           " in file " + filepath + ": " + line);
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        Logger::getGlobal().error("Error loading config from file " + filepath + ": " + e.what());
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    try {
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
    } catch (const std::exception& e) {
        Logger::getGlobal().error("Error loading config from args: " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromEnvironment(const std::string& prefix) {
    try {
        // In a real implementation, we would iterate over environment variables
        // For now, this is a placeholder
        
        // TODO: Implement environment variable parsing
        // PLACEHOLDER
        return true;
    } catch (const std::exception& e) {
        Logger::getGlobal().error("Error loading config from environment: " + std::string(e.what()));
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            Logger::getGlobal().error("Cannot open file for writing: " + filepath);
            return false;
        }
        
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = " << valueToString(entry.value) << "\n";
        }
        
        return true;
    } catch (const std::exception& e) {
        Logger::getGlobal().error("Error saving config to file " + filepath + ": " + e.what());
        return false;
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    try {
        // Check if key exists
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it == pImpl->entries.end()) {
            // Check if there's a default definition
            auto defIt = pImpl->definitions.find(key);
            if (defIt != pImpl->definitions.end() && defIt->second.defaultValue.has_value()) {
                // We need to convert default value to T
                return convertValue<T>(defIt->second.defaultValue.value());
            }
            return std::nullopt;
        }
        
        // Get value from entry
        const ConfigValue& value = it->value;
        
        // Type check
        if (!isTypeValid<T>(value)) {
            Logger::getGlobal().warning("Config key " + key + " has incompatible type. Expected " + 
                                       getTypeName<T>() + " but got " + getTypeName(valueToType(value)));
            return std::nullopt;
        }
        
        // Convert to T
        return convertValue<T>(value);
        
    } catch (const std::exception& e) {
        Logger::getGlobal().error("Error getting config key " + key + ": " + e.what());
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    try {
        auto val = get<T>(key);
        return val.has_value() ? val.value() : defaultValue;
    } catch (const std::exception& e) {
        Logger::getGlobal().error("Error getting config key " + key + " with default: " + e.what());
        return defaultValue;
    }
}

bool Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    try {
        // Check if key exists
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        // Check definition if not found
        auto defIt = pImpl->definitions.find(key);
        bool hasDefinition = defIt != pImpl->definitions.end();
        
        if (hasDefinition) {
            const ConfigDefinition& def = defIt->second;
            
            // Check type compatibility
            if (!validateValue(value, def.type)) {
                throw ValidationError("Invalid type for config key " + key + ". Expected " + 
                                    getTypeName(def.type) + "; got " + getTypeName(valueToType(value)));
            }
            
            // Check validator if defined
            if (def.validator && !def.validator(value)) {
                throw ValidationError("Validator failed for config key " + key);
            }
        }
        
        // Check source priority if updating existing key
        if (it != pImpl->entries.end()) {
            if (sourcePriority[source] > sourcePriority[it->source]) {
                throw SourceConflictError(key, source, it->source);
            }
        }
        
        // Add or update entry
        if (it != pImpl->entries.end()) {
            it->value = value;
            it->source = source;
            it->version = Impl::currentVersion;
        } else {
            pImpl->entries.emplace_back(key, value, source, 
                                       hasDefinition ? defIt->second.description : "",
                                       (source == ConfigSource::File || source == ConfigSource::CommandLine) ? 
                                       "file/command" : "runtime");
        }
        
        return true;
    } catch (const std::exception& e) {
        Logger::getGlobal().error("Error setting config key " + key + ": " + e.what());
        return false;
    }
}

ConfigSource Config::getSourcePriority(ConfigSource source) const {
    auto it = pImpl->sourcePriority.find(source);
    return it != pImpl->sourcePriority.end() ? it->second : 999;
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

bool Config::isValid(const std::string& key) const {
    try {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it == pImpl->entries.end()) {
            // Check if key is defined
            return pImpl->definitions.find(key) != pImpl->definitions.end();
        }
        
        // Check if value is valid according to definition
        auto defIt = pImpl->definitions.find(key);
        if (defIt != pImpl->definitions.end()) {
            return validateValue(it->value, defIt->second.type);
        }
        
        return true; // No definition, consider valid
    } catch (...) {
        return false;
    }
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

std::optional<ConfigEntry> Config::getEntry(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    return *it;
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

ConfigVersion Config::getCurrentVersion() {
    return Impl::currentVersion;
}

void Config::setVersion(const ConfigVersion& version) {
    Impl::currentVersion = version;
}

std::string Config::getStats() const {
    std::ostringstream oss;
    oss << "Configuration Statistics:\n";
    oss << "  Total entries: " << pImpl->entries.size() << "\n";
    
    std::map<ConfigSource, int> sourceCount;
    for (const auto& entry : pImpl->entries) {
        sourceCount[entry.source]++;
    }
    
    oss << "  By source:\n";
    for (const auto& pair : sourceCount) {
        oss << "    " << static_cast<int>(pair.first) << ": " << pair.second << "\n";
    }
    
    oss << "  Defined keys: " << pImpl->definitions.size() << "\n";
    oss << "  Valid entries: " << std::count_if(pImpl->entries.begin(), pImpl->entries.end(),
        [this](const ConfigEntry& e) { return isValid(e.key); }) << "\n";
    
    return oss.str();
}

bool Config::validate(const std::string& key, const ConfigValue& value) const {
    return validateValue(value, valueToType(value));
}

bool Config::merge(const Config& other, ConfigSource source) {
    for (const auto& entry : other.pImpl->entries) {
        set(entry.key, entry.value, source);
    }
    return true;
}

bool Config::parseEnvironment(const std::string& envKey, std::string& configKey, ConfigValue& value, const std::string& prefix) {
    // Parse environment variable key to config key
    if (!prefix.empty() && startsWith(envKey, prefix)) {
        configKey = envKey.substr(prefix.length());
    } else {
        configKey = envKey;
    }
    
    // In a real implementation, we would convert the environment variable value
    // to the appropriate ConfigValue type based on the key prefix or naming convention
    return true;
}

std::unique_ptr<Config> Config::create(const std::string& name) {
    return ConfigFactory::getInstance().createConfig(name);
}

bool Config::registerConfig(const std::string& name, const std::function<void(Config&)>& builder) {
    return ConfigFactory::getInstance().registerConfig(name, builder);
}

template<typename T>
T Config::convertValue(const ConfigValue& value) {
    return std::get<T>(value);
}

template<typename T>
ConfigType Config::getTypeFromTemplate() {
    if constexpr (std::is_same_v<T, int>) return ConfigType::Int;
    else if constexpr (std::is_same_v<T, int64_t>) return ConfigType::Int64;
    else if constexpr (std::is_same_v<T, double>) return ConfigType::Double;
    else if constexpr (std::is_same_v<T, bool>) return ConfigType::Bool;
    else if constexpr (std::is_same_v<T, std::string>) return ConfigType::String;
    else if constexpr (std::is_same_v<T, std::vector<int>>) return ConfigType::IntVector;
    else if constexpr (std::is_same_v<T, std::vector<double>>) return ConfigType::DoubleVector;
    else if constexpr (std::is_same_v<T, std::vector<std::string>>) return ConfigType::StringVector;
    else return ConfigType::Any;
}

template<typename T>
std::string Config::getTypeName() {
    ConfigType type = getTypeFromTemplate<T>();
    switch (type) {
        case ConfigType::Int: return "int";
        case ConfigType::Int64: return "int64_t";
        case ConfigType::Double: return "double";
        case ConfigType::Bool: return "bool";
        case ConfigType::String: return "std::string";
        case ConfigType::IntVector: return "std::vector<int>";
        case ConfigType::DoubleVector: return "std::vector<double>";
        case ConfigType::StringVector: return "std::vector<std::string>";
        case ConfigType::Any: return "any";
        default: return "unknown";
    }
}

std::string Config::getTypeName(ConfigType type) {
    switch (type) {
        case ConfigType::Int: return "int";
        case ConfigType::Int64: return "int64_t";
        case ConfigType::Double: return "double";
        case ConfigType::Bool: return "bool";
        case ConfigType::String: return "std::string";
        case ConfigType::IntVector: return "std::vector<int>";
        case ConfigType::DoubleVector: return "std::vector<double>";
        case ConfigType::StringVector: return "std::vector<std::string>";
        case ConfigType::Any: return "any";
        default: return "unknown";
    }
}

std::string Config::getTypeName(const ConfigValue& value) {
    ConfigType type = valueToType(value);
    return getTypeName(type);
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

ConfigType Config::valueToType(const ConfigValue& value) {
    return std::visit([](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) return ConfigType::Int;
        else if constexpr (std::is_same_v<T, int64_t>) return ConfigType::Int64;
        else if constexpr (std::is_same_v<T, double>) return ConfigType::Double;
        else if constexpr (std::is_same_v<T, bool>) return ConfigType::Bool;
        else if constexpr (std::is_same_v<T, std::string>) return ConfigType::String;
        else if constexpr (std::is_same_v<T, std::vector<int>>) return ConfigType::IntVector;
        else if constexpr (std::is_same_v<T, std::vector<double>>) return ConfigType::DoubleVector;
        else if constexpr (std::is_same_v<T, std::vector<std::string>>) return ConfigType::StringVector;
        else return ConfigType::Any;
    }, value);
}

std::optional<ConfigValue> Config::convertToType(const std::string& str, ConfigType type) {
    try {
        switch (type) {
            case ConfigType::Int:
                return std::stoi(str);
            case ConfigType::Int64:
                return static_cast<int64_t>(std::stoll(str));
            case ConfigType::Double:
                return std::stod(str);
            case ConfigType::Bool:
                if (str == "true" || str == "1" || str == "yes") return true;
                if (str == "false" || str == "0" || str == "no") return false;
                throw ValidationError("Invalid boolean value: " + str);
            case ConfigType::String:
                return str;
            case ConfigType::IntVector:
            case ConfigType::DoubleVector:
            case ConfigType::StringVector:
                // Could parse comma-separated values
                throw ValidationError("Vector type conversion not implemented yet");
            case ConfigType::Any:
                return str;
            default:
                throw ValidationError("Unknown config type: " + std::to_string(static_cast<int>(type)));
        }
    } catch (const std::exception& e) {
        Logger::getGlobal().error("Error converting string to type " + std::to_string(static_cast<int>(type)) + ": " + e.what());
        return std::nullopt;
    }
}

bool Config::validateValue(const ConfigValue& value, ConfigType type) {
    ConfigType actualType = valueToType(value);
    
    // Check type compatibility
    if (type != ConfigType::Any && actualType != type) {
        return false;
    }
    
    // Additional validation based on type
    return std::visit([type](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        switch (type) {
            case ConfigType::Int:
                return std::is_same_v<T, int>;
            case ConfigType::Int64:
                return std::is_same_v<T, int64_t> || std::is_same_v<T, int>;
            case ConfigType::Double:
                return std::is_same_v<T, double> || std::is_same_v<T, int> || std::is_same_v<T, int64_t>;
            case ConfigType::Bool:
                return std::is_same_v<T, bool>;
            case ConfigType::String:
                return std::is_same_v<T, std::string>;
            case ConfigType::IntVector:
                return std::is_same_v<T, std::vector<int>>;
            case ConfigType::DoubleVector:
                return std::is_same_v<T, std::vector<double>>;
            case ConfigType::StringVector:
                return std::is_same_v<T, std::vector<std::string>>;
            case ConfigType::Any:
                return true;
            default:
                return false;
        }
    }, value);
}

std::string Config::valueToString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                             std::is_same_v<T, std::vector<double>> ||
                             std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            bool first = true;
            for (const auto& item : arg) {
                if (!first) oss << ", ";
                if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    oss << "\"" << item << "\"";
                } else {
                    oss << item;
                }
                first = false;
            }
            oss << "]";
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

// Template implementations

template<typename T>
bool Config::isTypeValid(const ConfigValue& value) const {
    ConfigType expectedType = getTypeFromTemplate<T>();
    return validateValue(value, expectedType);
}

template<typename T>
T Config::convertValue(const ConfigValue& value) const {
    return std::get<T>(value);
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