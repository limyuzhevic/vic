#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

// Simple JSON support - check for nlohmann/json.h or json.hpp
// This is a simplified JSON implementation that doesn't require external dependencies
namespace json_internal {

struct JsonValue {
    std::string stringValue;
    int64_t intValue;
    double floatValue;
    bool boolValue;
    bool isString;
    bool isNumber;
    bool isBool;
    
    JsonValue() : isString(false), isNumber(false), isBool(false) {}
};

class SimpleJsonParser {
public:
    static bool parse(const std::string& jsonStr, JsonValue& result) {
        // Very basic JSON parsing for key=value format
        // This is a simplified implementation to demonstrate the concept
        if (jsonStr.empty()) return false;
        
        size_t pos = jsonStr.find('"');
        if (pos == std::string::npos) return false;
        
        size_t endQuote = jsonStr.find('"', pos + 1);
        if (endQuote == std::string::npos) return false;
        
        result.stringValue = jsonStr.substr(pos + 1, endQuote - pos - 1);
        result.isString = true;
        return true;
    }
};

} // namespace json_internal

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try to load as JSON first
    std::ifstream jsonFile(filepath);
    if (jsonFile.is_open()) {
        try {
            json j;
            jsonFile >> j;
            
            // Load JSON entries
            if (j.is_object()) {
                for (auto it = j.begin(); it != j.end(); ++it) {
                    std::string key = it.key();
                    
                    // Try to parse value based on type
                    json value = it.value();
                    if (value.is_string()) {
                        set(key, value.get<std::string>(), ConfigSource::File);
                    } else if (value.is_number_integer()) {
                        set(key, value.get<int64_t>(), ConfigSource::File);
                    } else if (value.is_number_float()) {
                        set(key, value.get<double>(), ConfigSource::File);
                    } else if (value.is_boolean()) {
                        set(key, value.get<bool>(), ConfigSource::File);
                    } else {
                        // Default to string
                        set(key, value.dump(), ConfigSource::File);
                    }
                }
                return true;
            }
        } catch (const json::parse_error&) {
            // JSON parsing failed, try key=value format
            jsonFile.close();
        }
    }
    
    // Fallback to key=value format
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
            
            // Try to convert to appropriate type
            bool converted = false;
            
            // Try integer
            try {
                size_t intPos;
                int64_t intVal = std::stoll(value, &intPos);
                if (intPos == value.size()) {
                    set(key, intVal, ConfigSource::File);
                    converted = true;
                }
            } catch (...) {}
            
            if (!converted) {
                // Try double
                try {
                    size_t doublePos;
                    double doubleVal = std::stod(value, &doublePos);
                    if (doublePos == value.size()) {
                        set(key, doubleVal, ConfigSource::File);
                        converted = true;
                    }
                } catch (...) {}
            }
            
            if (!converted) {
                // Try boolean
                if (value == "true" || value == "false") {
                    bool boolVal = (value == "true");
                    set(key, boolVal, ConfigSource::File);
                    converted = true;
                }
            }
            
            if (!converted) {
                // Default to string
                set(key, value, ConfigSource::File);
            }
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
        
        // Visit to get the value
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                file << arg;
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
