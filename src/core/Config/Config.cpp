#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    // Track validation state for each entry
    mutable std::map<std::string, bool> validationCache;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
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
        } else {
            NLM_LOG_WARNING("Invalid config line " + std::to_string(lineNumber) + ": " + line);
        }
    }
    
    return true;
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
        else {
            NLM_LOG_WARNING("Unknown command line argument: " + arg);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for writing: " + filepath);
        return false;
    }
    
    // Create directory if it doesn't exist
    std::filesystem::path dir = std::filesystem::path(filepath).parent_path();
    if (!dir.empty() && !std::filesystem::exists(dir)) {
        std::filesystem::create_directories(dir);
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        std::visit([&file](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << val << "\"\n";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << val[i];
                }
                file << "]\n";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << val[i];
                }
                file << "]\n";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << val[i] << "\"";
                }
                file << "]\n";
            } else if constexpr (std::is_same_v<T, std::map<std::string, std::string>>) {
                file << entry.key << " = {";
                for (auto it = val.begin(); it != val.end(); ++it) {
                    if (it != val.begin()) file << ", ";
                    file << "\"" << it->first << "\": \"" << it->second << "\"";
                }
                file << "}\n";
            } else {
                file << entry.key << " = " << val << "\n";
            }
        }, entry.value);
        file << "\n";
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    // Check validation cache first
    if (pImpl->validationCache.find(key) != pImpl->validationCache.end() && 
        pImpl->validationCache[key] && 
        validateKey(key).valid) {
        // Use cached value if valid
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        if (it != pImpl->entries.end()) {
            try {
                return std::get<T>(it->value);
            } catch (const std::bad_variant_access&) {
                return std::nullopt;
            }
        }
    }
    
    // Validate and cache the result
    auto validation = validateKey(key);
    if (!validation.valid) {
        return std::nullopt;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        T result = std::get<T>(it->value);
        pImpl->validationCache[key] = true;
        return result;
    } catch (const std::bad_variant_access&) {
        pImpl->validationCache[key] = false;
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    if (key.empty()) {
        NLM_LOG_WARNING("Cannot set empty key");
        return;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    ConfigEntry newEntry(key, value, source);
    newEntry.validated = false;  // Mark as needing validation
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        it->validated = false;  // Mark as needing validation
    } else {
        pImpl->entries.emplace_back(newEntry);
    }
    
    // Invalidate cache for this key and all keys with similar patterns
    pImpl->validationCache.erase(key);
    for (auto& cacheEntry : pImpl->validationCache) {
        if (cacheEntry.first.find(key) != std::string::npos || 
            key.find(cacheEntry.first) != std::string::npos) {
            cacheEntry.second = false;
        }
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

void Config::set(const std::string& key, const std::vector<int>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<double>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<std::string>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

ValidationResult Config::validate() const {
    ValidationResult result = ValidationResult::success();
    
    for (const auto& entry : pImpl->entries) {
        ValidationResult entryResult = validateEntry(entry);
        if (!entryResult.valid) {
            if (result.valid) {
                result = ValidationResult::error("Configuration validation failed:");
            }
            result.errorMessage += "\n  Key \"" + entry.key + "\": " + entryResult.errorMessage;
        }
    }
    
    return result;
}

ValidationResult Config::validateKey(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return ValidationResult::success();  // Missing keys are valid (can be defaulted)
    }
    
    return validateEntry(*it);
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
    
    // Clean up validation cache
    pImpl->validationCache.erase(key);
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

std::vector<std::string> Config::getKeys(const std::string& prefix) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.key.rfind(prefix, 0) == 0) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->validationCache.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << val << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << val[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << val[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << "\"" << val[i] << "\"";
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::map<std::string, std::string>>) {
                oss << "{";
                for (auto it = val.begin(); it != val.end(); ++it) {
                    if (it != val.begin()) oss << ", ";
                    oss << "\"" << it->first << "\": \"" << it->second << "\"";
                }
                oss << "}";
            } else {
                oss << val;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")";
        
        // Add validation status
        if (!entry.validated) {
            oss << " [UNVALIDATED]";
        }
        oss << "\n";
    }
    
    return oss.str();
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
    return source != ConfigSource::Default && source != ConfigSource::File;
}

template<typename T>
std::optional<T> Config::tryConvert(const ConfigValue& value) {
    try {
        return std::get<T>(value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

template<typename T>
bool Config::validateType(const ConfigValue& value) {
    if constexpr (std::is_same_v<T, int>) {
        return std::holds_alternative<int>(value);
    } else if constexpr (std::is_same_v<T, int64_t>) {
        return std::holds_alternative<int64_t>(value);
    } else if constexpr (std::is_same_v<T, double>) {
        return std::holds_alternative<double>(value);
    } else if constexpr (std::is_same_v<T, bool>) {
        return std::holds_alternative<bool>(value);
    } else if constexpr (std::is_same_v<T, std::string>) {
        return std::holds_alternative<std::string>(value);
    } else if constexpr (std::is_same_v<T, std::vector<int>>) {
        return std::holds_alternative<std::vector<int>>(value);
    } else if constexpr (std::is_same_v<T, std::vector<double>>) {
        return std::holds_alternative<std::vector<double>>(value);
    } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
        return std::holds_alternative<std::vector<std::string>>(value);
    } else {
        return false;
    }
}

void Config::apply(const std::map<std::string, ConfigValue>& values) {
    for (const auto& pair : values) {
        set(pair.first, pair.second, ConfigSource::Runtime);
    }
}

void Config::apply(const Config& other) {
    for (const auto& entry : other.pImpl->entries) {
        set(entry.key, entry.value, entry.source);
    }
}

Config Config::copy(ConfigSource source) const {
    Config result;
    
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            result.set(entry.key, entry.value, entry.source);
            if (entry.validator) {
                result.pImpl->entries.back().validator = entry.validator;
            }
        }
    }
    
    return result;
}

void Config::merge(const Config& other, ConfigSource source) {
    for (const auto& entry : other.pImpl->entries) {
        // Only merge if not already set from a higher priority source
        ConfigSource currentSource = getSource(entry.key);
        if (source >= currentSource) {
            set(entry.key, entry.value, source);
            if (entry.validator) {
                pImpl->entries.back().validator = entry.validator;
            }
        }
    }
}

bool Config::equals(const Config& other) const {
    if (pImpl->entries.size() != other.pImpl->entries.size()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        auto otherIt = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (otherIt == other.pImpl->entries.end() || otherIt->value != entry.value) {
            return false;
        }
    }
    
    return true;
}

ValidationResult Config::validateEntry(const ConfigEntry& entry) const {
    // Check type validation
    if (!entry.validated) {
        // Basic type validation based on key patterns
        if (entry.key.find("count") != std::string::npos || 
            entry.key.find("_count") != std::string::npos) {
            if (!validateType<int64_t>(entry.value) && 
                !validateType<int>(entry.value)) {
                return ValidationResult::error("Expected integer for count key");
            }
        } else if (entry.key.find("rate") != std::string::npos || 
                   entry.key.find("_rate") != std::string::npos) {
            if (!validateType<double>(entry.value)) {
                return ValidationResult::error("Expected float/double for rate key");
            }
        } else if (entry.key.find("enabled") != std::string::npos || 
                   entry.key.find("_enabled") != std::string::npos) {
            if (!validateType<bool>(entry.value)) {
                return ValidationResult::error("Expected boolean for enabled key");
            }
        } else if (entry.key.find("name") != std::string::npos || 
                   entry.key.find("_name") != std::string::npos) {
            if (!validateType<std::string>(entry.value)) {
                return ValidationResult::error("Expected string for name key");
            }
        }
        
        entry.validated = true;
    }
    
    return ValidationResult::success();
}

void Config::markAllUnvalidated() const {
    for (auto& entry : pImpl->entries) {
        entry.validated = false;
    }
    pImpl->validationCache.clear();
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

bool Config::isNumber(const std::string& str) {
    if (str.empty()) return false;
    size_t start = 0;
    if (str[0] == '-' || str[0] == '+') start = 1;
    
    for (size_t i = start; i < str.size(); ++i) {
        if (!std::isdigit(str[i])) {
            return false;
        }
    }
    return true;
}

bool Config::isBoolean(const std::string& str) {
    return str == "true" || str == "false" || str == "1" || str == "0";
}

ConfigValue Config::parseValue(const std::string& str, ConfigSource source) {
    std::string trimmed = trim(str);
    
    if (trimmed.empty()) {
        return std::string("");
    }
    
    if (trimmed == "true" || trimmed == "false") {
        return trimmed == "true";
    }
    
    if (isNumber(trimmed)) {
        // Check if it's an integer or float
        size_t pos = trimmed.find('.');
        if (pos != std::string::npos) {
            return std::stod(trimmed);
        } else {
            // Try int64_t first, then int
            try {
                return std::stoll(trimmed);
            } catch (...) {
                return std::stoi(trimmed);
            }
        }
    }
    
    // Check if it's a list
    if (trimmed.front() == '[' && trimmed.back() == ']') {
        std::string content = trimmed.substr(1, trimmed.size() - 2);
        std::vector<std::string> parts;
        std::string part;
        std::istringstream stream(content);
        
        while (std::getline(stream, part, ',')) {
            part = trim(part);
            if (part.front() == '\"' && part.back() == '\"') {
                parts.push_back(part.substr(1, part.size() - 2));
            } else if (part == "true" || part == "false") {
                parts.push_back(part);
            } else if (isNumber(part)) {
                // Try to parse as number
                try {
                    std::stoll(part);
                    parts.push_back(part);
                } catch (...) {
                    parts.push_back(part);
                }
            } else {
                parts.push_back(part);
            }
        }
        
        // Determine the list type
        bool allInt = true;
        bool allBool = true;
        bool allFloat = true;
        
        for (const auto& part : parts) {
            if (!isNumber(part)) {
                allInt = false;
                allFloat = false;
            } else {
                try {
                    std::stoll(part);
                } catch (...) {
                    allInt = false;
                }
                try {
                    std::stod(part);
                } catch (...) {
                    allFloat = false;
                }
            }
            if (part != "true" && part != "false") {
                allBool = false;
            }
        }
        
        if (allBool) {
            std::vector<bool> boolList;
            for (const auto& part : parts) {
                boolList.push_back(part == "true" || part == "1");
            }
            return boolList;
        } else if (allInt) {
            std::vector<int> intList;
            for (const auto& part : parts) {
                intList.push_back(std::stoi(part));
            }
            return intList;
        } else if (allFloat) {
            std::vector<double> floatList;
            for (const auto& part : parts) {
                floatList.push_back(std::stod(part));
            }
            return floatList;
        } else {
            std::vector<std::string> stringList;
            for (const auto& part : parts) {
                stringList.push_back(part);
            }
            return stringList;
        }
    }
    
    // Check if it's a map/object
    if (trimmed.front() == '{' && trimmed.back() == '}') {
        std::string content = trimmed.substr(1, trimmed.size() - 2);
        std::map<std::string, std::string> object;
        
        // Simple parser for key:value pairs
        size_t pos = 0;
        while (pos < content.size()) {
            size_t colon = content.find(':', pos);
            if (colon == std::string::npos) break;
            
            std::string key = trim(content.substr(pos, colon - pos));
            size_t startVal = colon + 1;
            size_t endVal = content.find(',', startVal);
            if (endVal == std::string::npos) endVal = content.size();
            
            std::string value = trim(content.substr(startVal, endVal - startVal));
            
            // Remove quotes
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            if (!key.empty()) {
                object[key] = value;
            }
            
            pos = endVal + 1;
            while (pos < content.size() && (content[pos] == ' ' || content[pos] == '\t')) {
                pos++;
            }
        }
        
        return object;
    }
    
    // Default to string
    return trimmed;
}

// Explicit template instantiations
namespace {
    // Helper to trigger template instantiations
    struct TemplateInstantiator {
        TemplateInstantiator() {
            // These instantiations ensure the templates are compiled
            Config config;
            
            // Trigger instantiation of get templates
            config.get<int>("test");
            config.get<double>("test");
            config.get<std::string>("test");
            config.get<bool>("test");
            
            // Trigger instantiation of getOr templates
            config.getOr<int>("test", 0);
            config.getOr<double>("test", 0.0);
            config.getOr<std::string>("test", "");
            config.getOr<bool>("test", false);
            
            // Trigger instantiation of validation templates
            Config::validateType<int>(ConfigValue(42));
            Config::validateType<double>(ConfigValue(3.14));
            Config::validateType<std::string>(ConfigValue(std::string("test")));
            Config::validateType<bool>(ConfigValue(true));
        }
    };
    
    TemplateInstantiator instantiator;
}

} // namespace nlm