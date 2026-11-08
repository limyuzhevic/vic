#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>

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
        NLM_LOG_WARNING("Cannot open config file: " + filepath);
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();
    
    // Try to parse as JSON first, then fallback to simple key=value format
    if (parseJSON(content)) {
        NLM_LOG_INFO("Loaded configuration from JSON file: " + filepath);
        return true;
    } else if (parseSimpleKeyValue(content)) {
        NLM_LOG_INFO("Loaded configuration from simple key=value file: " + filepath);
        return true;
    } else {
        NLM_LOG_WARNING("Failed to parse config file: " + filepath + " (tried JSON and simple formats)");
        return false;
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

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
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
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) oss << ", ";
                    oss << arg[i];
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

// Explicit template instantiations
// (uint64_t and size_t are the same type on this platform, so only one is listed)
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<uint64_t> Config::get<uint64_t>(const std::string&) const;
template std::optional<float> Config::get<float>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template uint64_t Config::getOr<uint64_t>(const std::string&, const uint64_t&) const;
template float Config::getOr<float>(const std::string&, const float&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

// JSON and key-value parsing helper methods

bool Config::parseJSON(const std::string& content) {
    // Simple JSON parsing implementation
    // Remove comments
    std::string cleaned;
    for (size_t i = 0; i < content.size(); ++i) {
        if (i + 1 < content.size() && content[i] == '/' && content[i + 1] == '/') {
            // Skip line comment
            while (i + 1 < content.size() && content[i] != '\n') {
                ++i;
            }
        } else if (i + 1 < content.size() && content[i] == '/' && content[i + 1] == '*') {
            // Skip block comment
            while (i + 1 < content.size() && !(content[i] == '*' && content[i + 1] == '/')) {
                ++i;
            }
            if (i + 1 < content.size()) {
                i += 2; // Skip */
            }
        } else {
            cleaned += content[i];
        }
    }
    
    // Parse key-value pairs from JSON
    // This is a simplified parser - for production use, consider a proper JSON library
    size_t pos = 0;
    while (pos < cleaned.size()) {
        // Find next key (quoted or unquoted)
        pos = cleaned.find('"', pos);
        if (pos == std::string::npos) break;
        
        size_t keyEnd = cleaned.find('"', pos + 1);
        if (keyEnd == std::string::npos) break;
        
        std::string key = cleaned.substr(pos + 1, keyEnd - pos - 1);
        pos = keyEnd + 1;
        
        // Skip whitespace and colon
        while (pos < cleaned.size() && (cleaned[pos] == ' ' || cleaned[pos] == '\t' || cleaned[pos] == ':')) {
            pos++;
        }
        
        // Find value (could be string, number, boolean, or array)
        if (pos >= cleaned.size()) break;
        
        if (cleaned[pos] == '"') {
            // String value
            size_t valueEnd = cleaned.find('"', pos + 1);
            if (valueEnd == std::string::npos) break;
            std::string value = cleaned.substr(pos + 1, valueEnd - pos - 1);
            set(key, value, ConfigSource::File);
            pos = valueEnd + 1;
        } else if (cleaned[pos] == '[') {
            // Array value - simplified parsing
            size_t arrayEnd = cleaned.find(']', pos);
            if (arrayEnd == std::string::npos) break;
            std::string arrayContent = cleaned.substr(pos + 1, arrayEnd - pos - 1);
            // For now, treat arrays as strings
            set(key, arrayContent, ConfigSource::File);
            pos = arrayEnd + 1;
        } else if (std::isdigit(cleaned[pos]) || cleaned[pos] == '-') {
            // Number value - simplified parsing
            size_t valueEnd = pos;
            while (valueEnd < cleaned.size() && (std::isdigit(cleaned[valueEnd]) || 
                                                  cleaned[valueEnd] == '.' || 
                                                  cleaned[valueEnd] == 'e' || 
                                                  cleaned[valueEnd] == 'E' || 
                                                  cleaned[valueEnd] == '+' || 
                                                  cleaned[valueEnd] == '-')) {
                valueEnd++;
            }
            std::string value = cleaned.substr(pos, valueEnd - pos);
            try {
                if (value.find('.') != std::string::npos || value.find('e') != std::string::npos || value.find('E') != std::string::npos) {
                    set(key, std::stod(value), ConfigSource::File);
                } else {
                    set(key, std::stoll(value), ConfigSource::File);
                }
            } catch (...) {
                set(key, value, ConfigSource::File);
            }
            pos = valueEnd;
        } else if (cleaned[pos] == 't' || cleaned[pos] == 'f') {
            // Boolean value
            size_t valueEnd = pos;
            while (valueEnd < cleaned.size() && (cleaned[valueEnd] == 't' || cleaned[valueEnd] == 'f' || cleaned[valueEnd] == ' ' || cleaned[valueEnd] == ',' || cleaned[valueEnd] == '}')) {
                valueEnd++;
            }
            std::string value = cleaned.substr(pos, valueEnd - pos);
            if (value == "true" || value == "True" || value == "TRUE") {
                set(key, true, ConfigSource::File);
            } else if (value == "false" || value == "False" || value == "FALSE") {
                set(key, false, ConfigSource::File);
            } else {
                set(key, value, ConfigSource::File);
            }
            pos = valueEnd;
        } else {
            // Unknown type, treat as string
            size_t valueEnd = pos;
            while (valueEnd < cleaned.size() && (cleaned[valueEnd] != ',' && cleaned[valueEnd] != '}')) {
                valueEnd++;
            }
            std::string value = cleaned.substr(pos, valueEnd - pos);
            set(key, value, ConfigSource::File);
            pos = valueEnd;
        }
        
        // Skip comma and whitespace
        while (pos < cleaned.size() && (cleaned[pos] == ',' || cleaned[pos] == ' ' || cleaned[pos] == '\t')) {
            pos++;
        }
    }
    
    return !pImpl->entries.empty();
}

bool Config::parseSimpleKeyValue(const std::string& content) {
    // Parse simple key=value format
    std::istringstream stream(content);
    std::string line;
    
    while (std::getline(stream, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/' || line[0] == '{' || line[0] == '}') {
            continue;
        }
        
        // Parse key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove trailing comma if present
            if (!value.empty() && value.back() == ',') {
                value.pop_back();
            }
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return !pImpl->entries.empty();
}

bool Config::validate() const {
    // Basic validation for the configuration
    for (const auto& entry : pImpl->entries) {
        // Check for required keys
        if (entry.key == "brain.neuron_count") {
            auto value = get<int>(entry.key);
            if (value && *value <= 0) {
                throw std::runtime_error("brain.neuron_count must be positive: " + entry.key);
            }
        } else if (entry.key == "brain.synapse_density") {
            auto value = get<double>(entry.key);
            if (value && (*value < 0.0 || *value > 1.0)) {
                throw std::runtime_error("brain.synapse_density must be between 0.0 and 1.0: " + entry.key);
            }
        }
    }
    
    return true;
}

} // namespace nlm