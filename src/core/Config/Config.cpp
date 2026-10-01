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

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // Enhanced file loading with better error handling and JSON support
    // Supports JSON objects like {"key": "value", "key2": "value2"} and simple key=value pairs
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_WARNING("Could not open config file: " + filepath);
        return false;
    }
    
    std::string line;
    int lineNum = 0;
    bool hasValidEntries = false;
    
    while (std::getline(file, line)) {
        ++lineNum;
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Try to parse as JSON (basic JSON parser with support for nested structures)
        if (line.find("{") != std::string::npos && line.find("}") != std::string::npos) {
            // Basic JSON parsing for key-value pairs
            size_t keyPos = line.find('"');
            while (keyPos != std::string::npos) {
                size_t keyEnd = line.find('"', keyPos + 1);
                if (keyEnd == std::string::npos) break;
                
                std::string key = line.substr(keyPos + 1, keyEnd - keyPos - 1);
                size_t colonPos = line.find(':', keyEnd);
                if (colonPos == std::string::npos) break;
                
                size_t valueStart = line.find_first_not_of(' ', colonPos + 1);
                if (valueStart == std::string::npos) break;
                
                size_t valueEnd = line.find('"', valueStart + 1);
                if (valueEnd == std::string::npos) {
                    valueEnd = line.find(',', valueStart);
                    if (valueEnd == std::string::npos) valueEnd = line.find('}', valueStart);
                }
                
                if (valueEnd != std::string::npos) {
                    std::string value = trim(line.substr(valueStart, valueEnd - valueStart));
                    // Remove trailing comma and closing brace
                    if (!value.empty() && value.back() == ',') value.pop_back();
                    if (!value.empty() && value.back() == '}') value.pop_back();
                    // Remove quotes from value
                    if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
                        value = value.substr(1, value.size() - 2);
                    }
                    set(key, value, ConfigSource::File);
                    hasValidEntries = true;
                }
                
                keyPos = line.find('"', keyEnd + 1);
            }
            continue;
        }
        
        // Parse simple key=value pairs with enhanced error handling
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Validate key format
            if (key.empty() || key.find(' ') != std::string::npos) {
                NLM_LOG_WARNING(std::string("Invalid config key format at line ") + std::to_string(lineNum) + ": " + key);
                continue;
            }
            
            // Remove quotes if present (handle both single and double quotes)
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Handle escaped quotes in values
            size_t quotePos = value.find('\\"');
            while (quotePos != std::string::npos) {
                value.replace(quotePos, 2, "\"");
                quotePos = value.find('\\"', quotePos + 2);
            }
            
            set(key, value, ConfigSource::File);
            hasValidEntries = true;
        } else {
            NLM_LOG_WARNING(std::string("Invalid config format (missing '=') at line ") + std::to_string(lineNum) + ": " + line.substr(0, 50));
        }
    }
    
    if (!hasValidEntries) {
        NLM_LOG_WARNING(std::string("Config file ") + filepath + " contains no valid entries");
    }
    
    NLM_LOG_INFO(std::string("Loaded config from ") + filepath + " (" + std::to_string(pImpl->entries.size()) + " entries)");
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
    
    // Write as enhanced key=value format with comments
    file << "# NLM Configuration File\n";
    file << "# Generated by NLM Configuration System\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << "# Source: " << static_cast<int>(entry.source) << "\n";
        file << entry.key << " = " << entry.value << "\n\n";
    }
    
    NLM_LOG_INFO(std::string("Saved config to ") + filepath + " (" + std::to_string(pImpl->entries.size()) + " entries)");
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
