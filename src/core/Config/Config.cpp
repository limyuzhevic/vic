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
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <iostream>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ConfigSchemaEntry> schema;
    std::vector<std::string> validationErrors;
    std::unordered_map<std::string, std::string> typeCache;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default schema entries for common NLM parameters
    initializeDefaultSchema();
}

Config::~Config() = default;

Config::Config(Config&& other) noexcept : pImpl(std::move(other.pImpl)) {}

Config& Config::operator=(Config&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

void Config::initializeDefaultSchema() {
    // Add common NLM configuration parameters
    addSchemaEntry({
        "brain.neuron_count", 
        ConfigSource::Default | ConfigSource::File | ConfigSource::CommandLine,
        [](const ConfigValue& v) {
            auto val = std::get<int>(v);
            return val > 0 && val <= 10000;
        },
        "Number of neurons in the brain",
        false,
        1000,
        1,
        10000
    });
    
    addSchemaEntry({
        "brain.synapse_density",
        ConfigSource::Default | ConfigSource::File | ConfigSource::CommandLine,
        [](const ConfigValue& v) {
            auto val = std::get<double>(v);
            return val >= 0.0 && val <= 1.0;
        },
        "Synapse connectivity density (0.0 to 1.0)",
        false,
        0.1,
        0.0,
        1.0
    });
    
    addSchemaEntry({
        "simulation.timestep",
        ConfigSource::Default | ConfigSource::File | ConfigSource::CommandLine,
        [](const ConfigValue& v) {
            auto val = std::get<double>(v);
            return val > 0.0 && val <= 0.1;
        },
        "Simulation time step in seconds",
        false,
        0.001,
        0.0001,
        0.1
    });
}

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw ConfigException("Cannot open configuration file: " + filepath, 
                            ConfigException::Type::LoadError);
    }
    
    // Try to parse as JSON first
    file.clear();
    file.seekg(0, std::ios::beg);
    
    std::string firstLine;
    if (std::getline(file, firstLine)) {
        // Check if it looks like JSON (starts with { or [
        if (firstLine.find('{') == 0 || firstLine.find('[') == 0) {
            file.clear();
            file.seekg(0, std::ios::beg);
            return parseJSON(file, ConfigSource::File);
        }
    }
    
    // Fall back to simple key=value format
    file.clear();
    file.seekg(0, std::ios::beg);
    return parseSimpleFormat(file, ConfigSource::File);
}

bool Config::parseJSON(std::ifstream& file, ConfigSource source) {
    try {
        std::stringstream buffer;
        buffer << file.rdbuf();
        
        // TODO: Implement proper JSON parsing using a JSON library
        // For now, just read as string and check if it contains key=value pairs
        std::string content = buffer.str();
        if (content.find('{') == std::string::npos && content.find('[') == std::string::npos) {
            // Not JSON, try simple format
            file.clear();
            file.seekg(0, std::ios::beg);
            return parseSimpleFormat(file, source);
        }
        
        // TODO: Implement actual JSON parsing
        throw ConfigException("JSON parsing not yet implemented", ConfigException::Type::LoadError);
        
    } catch (const std::exception& e) {
        throw ConfigException(std::string("JSON parsing failed: ") + e.what(),
                            ConfigException::Type::LoadError);
    }
}

bool Config::parseSimpleFormat(std::ifstream& file, ConfigSource source) {
    std::string line;
    size_t lineNumber = 0;
    
    while (std::getline(file, line)) {
        ++lineNumber;
        line = trim(line);
        
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            try {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                
                // Remove quotes if present
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                set(key, value, source);
                
            } catch (const std::exception& e) {
                throw ConfigException(std::string("Error parsing line ") + 
                                    std::to_string(lineNumber) + ": " + e.what(),
                                    ConfigException::Type::LoadError);
            }
        }
    }
    
    return validateAllEntries();
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
        }
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
    }
    
    return validateAllEntries();
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Write as simple key=value format for compatibility
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Convert ConfigValue to string
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
            } else {
                file << arg;
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    try {
        auto value = std::get<T>(getInternalValue(key));
        return value;
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
    if (validateKey(key) && validateValue(key, value)) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it != pImpl->entries.end()) {
            it->value = value;
            it->source = source;
        } else {
            ConfigEntry entry(key, value, source);
            if (getSchemaEntry(key)) {
                entry.description = getSchemaEntry(key)->description;
            }
            pImpl->entries.push_back(entry);
        }
    }
}

template<typename T>
void Config::set(const std::string& key, T value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

bool Config::has(const std::string& key) const {
    return getInternalValue(key) != nullptr;
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
    oss << "Configuration (" << pImpl->entries.size() << " entries, ";
    oss << pImpl->schema.size() << " schema rules):\n";
    
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
        oss << "] (" << static_cast<int>(entry.source) << ") - " << entry.description << "\n";
    }
    
    // Show validation errors if any
    if (!pImpl->validationErrors.empty()) {
        oss << "\nValidation errors:\n";
        for (const auto& error : pImpl->validationErrors) {
            oss << "  ERROR: " << error << "\n";
        }
    }
    
    return oss.str();
}

void Config::addSchemaEntry(const ConfigSchemaEntry& entry) {
    pImpl->schema.push_back(entry);
}

bool Config::validateSchema() const {
    pImpl->validationErrors.clear();
    bool isValid = true;
    
    for (const auto& entry : pImpl->entries) {
        bool valid = validateEntryAgainstSchema(entry);
        if (!valid) {
            isValid = false;
            pImpl->validationErrors.push_back(
                "Entry '" + entry.key + "' violates schema validation");
        }
    }
    
    return isValid;
}

bool Config::validateAllEntries() const {
    pImpl->validationErrors.clear();
    bool isValid = true;
    
    for (const auto& entry : pImpl->entries) {
        bool valid = validateValue(entry.key, entry.value);
        if (!valid) {
            isValid = false;
            pImpl->validationErrors.push_back(
                "Invalid value for key '" + entry.key + "'");
        }
    }
    
    return isValid;
}

std::string Config::getSchemaErrors() const {
    if (pImpl->validationErrors.empty()) {
        return "No schema validation errors.";
    }
    
    std::ostringstream oss;
    oss << "Schema validation errors (" << pImpl->validationErrors.size() << "):\n";
    for (const auto& error : pImpl->validationErrors) {
        oss << "  - " << error << "\n";
    }
    return oss.str();
}

template<typename T>
std::optional<T> Config::convertConfigValue(const ConfigValue& value) {
    try {
        return std::get<T>(value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

size_t Config::getEntryCount() const {
    return pImpl->entries.size();
}

size_t Config::getEntryCount(ConfigSource source) const {
    return std::count_if(pImpl->entries.begin(), pImpl->entries.end(),
        [source](const ConfigEntry& e) { return e.source == source; });
}

Config Config::copy() const {
    Config copy;
    copy.pImpl->entries = pImpl->entries;
    copy.pImpl->schema = pImpl->schema;
    return copy;
}

void Config::merge(const Config& other, ConfigSource source) {
    for (const auto& entry : other.pImpl->entries) {
        set(entry.key, entry.value, source);
    }
}

// Helper implementations
const ConfigSchemaEntry* Config::getSchemaEntry(const std::string& key) const {
    auto it = std::find_if(pImpl->schema.begin(), pImpl->schema.end(),
        [&key](const ConfigSchemaEntry& e) { return e.key == key; });n    return (it != pImpl->schema.end()) ? &(*it) : nullptr;
}

bool Config::validateKey(const std::string& key) const {
    // Basic key validation: not empty, no special characters
    return !key.empty() && key.find(' ') == std::string::npos;
}

bool Config::validateEntryAgainstSchema(const ConfigEntry& entry) const {
    const ConfigSchemaEntry* schema = getSchemaEntry(entry.key);
    if (!schema) {
        // No schema defined, allow entry
        return true;
    }
    
    // Check if source is allowed
    if ((schema->allowedSources & static_cast<ConfigSource>(entry.source)) == ConfigSource::Default) {
        pImpl->validationErrors.push_back(
            "Entry '" + entry.key + "' has invalid source");
        return false;
    }
    
    // Run custom validator if provided
    if (schema->validator && !schema->validator(entry.value)) {
        pImpl->validationErrors.push_back(
            "Entry '" + entry.key + "' validation failed");
        return false;
    }
    
    return validateValue(entry.key, entry.value);
}

bool Config::validateValue(const std::string& key, const ConfigValue& value) const {
    const ConfigSchemaEntry* schema = getSchemaEntry(key);
    if (!schema) {
        // No schema, perform basic validation
        if (std::holds_alternative<std::string>(value)) {
            return !std::get<std::string>(value).empty();
        }
        return true; // Non-string values are generally valid
    }
    
    // Validate type
    bool typeValid = false;
    if (schema->validator) {
        typeValid = schema->validator(value);
    }
    
    // Validate against bounds if specified
    bool boundsValid = true;
    if (!std::holds_alternative<std::monostate>(schema->minValue)) {
        boundsValid = validateAgainstBounds(value, schema->minValue, ConfigValue());
    }
    if (boundsValid && !std::holds_alternative<std::monostate>(schema->maxValue)) {
        boundsValid = validateAgainstBounds(value, ConfigValue(), schema->maxValue);
    }
    
    return typeValid && boundsValid;
}

const ConfigValue* Config::getInternalValue(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n    return (it != pImpl->entries.end()) ? &it->value : nullptr;
}

bool Config::validateAgainstBounds(const ConfigValue& value, const ConfigValue& min, const ConfigValue& max) const {
    try {
        if (!std::holds_alternative<std::monostate>(min)) {
            if (std::holds_alternative<int>(value) && std::holds_alternative<int>(min)) {
                return std::get<int>(value) >= std::get<int>(min);
            } else if (std::holds_alternative<double>(value) && std::holds_alternative<double>(min)) {
                return std::get<double>(value) >= std::get<double>(min);
            } else if (std::holds_alternative<int64_t>(value) && std::holds_alternative<int64_t>(min)) {
                return std::get<int64_t>(value) >= std::get<int64_t>(min);
            }
        }
        
        if (!std::holds_alternative<std::monostate>(max)) {
            if (std::holds_alternative<int>(value) && std::holds_alternative<int>(max)) {
                return std::get<int>(value) <= std::get<int>(max);
            } else if (std::holds_alternative<double>(value) && std::holds_alternative<double>(max)) {
                return std::get<double>(value) <= std::get<double>(max);
            } else if (std::holds_alternative<int64_t>(value) && std::holds_alternative<int64_t>(max)) {
                return std::get<int64_t>(value) <= std::get<int64_t>(max);
            }
        }
        
    } catch (const std::bad_variant_access&) {
        return false;
    }
    
    return true;
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

bool Config::isNumeric(const std::string& str) {
    return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit);
}

std::string Config::boolToString(bool value) {
    return value ? "true" : "false";
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

template void Config::set<int>(const std::string&, int, ConfigSource);

template void Config::set<double>(const std::string&, double, ConfigSource);

template void Config::set<bool>(const std::string&, bool, ConfigSource);

} // namespace nlm
