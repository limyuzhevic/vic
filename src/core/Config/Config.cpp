#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    // Try JSON format first
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    // Check if it looks like JSON (starts with { or [)
    bool isJson = false;
    if (!content.empty()) {
        char firstChar = content[0];
        if (firstChar == '{' || firstChar == '[') {
            isJson = true;
        }
    }
    
    if (isJson) {
        return loadFromJson(content);
    } else {
        return loadFromSimpleFormat(content);
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
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
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
    // Validate key
    if (key.empty()) {
        NLM_LOG_ERROR("Cannot set configuration with empty key");
        return;
    }
    
    // Validate value ranges for specific keys
    if (key == "neuron_count") {
        try {
            int64_t val = std::get<int64_t>(value);
            if (val <= 0) {
                NLM_LOG_ERROR("neuron_count must be positive, got: " + std::to_string(val));
                return;
            }
        } catch (...) {
            NLM_LOG_ERROR("neuron_count must be an integer, got: " + toString(value));
            return;
        }
    }
    
    if (key == "region_count") {
        try {
            int64_t val = std::get<int64_t>(value);
            if (val <= 0) {
                NLM_LOG_ERROR("region_count must be positive, got: " + std::to_string(val));
                return;
            }
        } catch (...) {
            NLM_LOG_ERROR("region_count must be an integer, got: " + toString(value));
            return;
        }
    }
    
    if (key == "connection_probability") {
        try {
            double val = std::get<double>(value);
            if (val < 0.0 || val > 1.0) {
                NLM_LOG_ERROR("connection_probability must be between 0.0 and 1.0, got: " + std::to_string(val));
                return;
            }
        } catch (...) {
            NLM_LOG_ERROR("connection_probability must be a double, got: " + toString(value));
            return;
        }
    }
    
    if (key == "enable_development") {
        try {
            bool val = std::get<bool>(value);
            if (!val) {
                NLM_LOG_WARNING("enable_development is false - Phase 6 integration may not work properly");
            }
        } catch (...) {
            NLM_LOG_ERROR("enable_development must be a boolean, got: " + toString(value));
            return;
        }
    }
    
    // Valid key-value pair
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        NLM_LOG_DEBUG("Updated config key: " + key + " = " + toString(value));
    } else {
        pImpl->entries.emplace_back(key, value, source);
        NLM_LOG_DEBUG("Added config key: " + key + " = " + toString(value));
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

namespace nlm {

bool Config::loadFromJson(const std::string& jsonString) {
    // Simple JSON parser for basic key-value pairs
    // Remove whitespace
    std::string cleaned;
    for (char c : jsonString) {
        if (!std::isspace(c)) cleaned += c;
    }
    
    if (cleaned.empty() || cleaned[0] != '{') {
        NLM_LOG_ERROR("Invalid JSON format: not an object");
        return false;
    }
    
    // Simple key="value" parsing (no nested structures)
    size_t pos = 1; // Skip {
    while (pos < cleaned.size() && cleaned[pos] != '}') {
        // Find key
        size_t keyStart = pos;
        while (pos < cleaned.size() && cleaned[pos] != '\"' && cleaned[pos] != '\'') {
            pos++;
        }
        if (pos >= cleaned.size() || (cleaned[pos] != '\"' && cleaned[pos] != '\'')) {
            break;
        }
        
        char quoteChar = cleaned[pos];
        pos++; // Skip opening quote
        
        std::string key = cleaned.substr(keyStart, pos - keyStart - 1);
        trim(key);
        
        // Find value
        while (pos < cleaned.size() && cleaned[pos] != '\"' && cleaned[pos] != '\'') {
            pos++;
        }
        if (pos >= cleaned.size()) break;
        
        pos++; // Skip opening quote
        size_t valueStart = pos;
        
        while (pos < cleaned.size() && cleaned[pos] != quoteChar) {
            pos++;
        }
        if (pos >= cleaned.size()) break;
        
        std::string value = cleaned.substr(valueStart, pos - valueStart);
        
        set(key, value, ConfigSource::File);
        pos++; // Skip closing quote
        
        // Skip comma or whitespace
        while (pos < cleaned.size() && (cleaned[pos] == ',' || std::isspace(cleaned[pos]))) {
            pos++;
        }
    }
    
    return true;
}

bool Config::loadFromSimpleFormat(const std::string& content) {
    std::istringstream stream(content);
    std::string line;
    bool success = true;
    
    while (std::getline(stream, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs
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
            NLM_LOG_WARNING("Invalid config line (no '='): " + line);
            success = false;
        }
    }
    
    return success;
}

bool Config::validate() const {
    // Basic validation: check for required keys and valid types
    bool success = true;
    
    // Validate common configuration keys
    if (has("neuron_count")) {
        auto val = get<size_t>("neuron_count");
        if (!val || *val == 0) {
            NLM_LOG_ERROR("neuron_count must be a positive integer");
            success = false;
        }
    }
    
    if (has("region_count")) {
        auto val = get<size_t>("region_count");
        if (!val || *val == 0) {
            NLM_LOG_ERROR("region_count must be a positive integer");
            success = false;
        }
    }
    
    if (has("connection_probability")) {
        auto val = get<double>("connection_probability");
        if (!val || *val < 0.0 || *val > 1.0) {
            NLM_LOG_ERROR("connection_probability must be between 0.0 and 1.0");
            success = false;
        }
    }
    
    // Check for development_plasticity flag
    if (has("enable_development")) {
        auto val = get<bool>("enable_development");
        if (!val) {
            NLM_LOG_ERROR("enable_development must be true for Phase 6 integration");
            success = false;
        }
    }
    
    return success;
}

template<typename T>
bool Config::getRequired(const std::string& key, T& outValue) const {
    auto val = get<T>(key);
    if (!val) {
        NLM_LOG_ERROR("Required configuration key missing or invalid: " + key);
        return false;
    }
    outValue = *val;
    return true;
}

bool Config::isValidType(const std::string& key, const std::string& expectedType) const {
    if (!has(key)) return false;
    
    auto val = get<std::string>(key);
    if (val) {
        if (expectedType == "int" || expectedType == "integer") {
            try {
                std::stoi(*val);
                return true;
            } catch (...) { return false; }
        } else if (expectedType == "float" || expectedType == "double") {
            try {
                std::stod(*val);
                return true;
            } catch (...) { return false; }
        } else if (expectedType == "bool" || expectedType == "boolean") {
            std::string lower = toLower(*val);
            return (lower == "true" || lower == "false" || lower == "1" || lower == "0");
        } else if (expectedType == "string") {
            return true;
        }
    }
    
    return false;
}

} // namespace nlm

namespace nlm {

std::string Config::toString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, double>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::string>) {
            oss << '"' << arg << '"';
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            oss << '[';
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ', ';
                oss << arg[i];
            }
            oss << ']';
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << '[';
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ', ';
                oss << arg[i];
            }
            oss << ']';
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << '[';
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ', ';
                oss << '"' << arg[i] << '"';
            }
            oss << ']';
        } else {
            oss << "unknown_type";
        }
    }, value);
    return oss.str();
}

} // namespace nlm
