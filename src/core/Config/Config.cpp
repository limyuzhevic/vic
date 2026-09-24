#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <unordered_map>
#include <stdexcept>

// Include JSON library for modern config parsing
#ifdef HAS_JSON_LIB
#include <nlohmann/json.hpp>
using json = nlohmann::json;
#endif

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, std::string> parsedValues; // Cached values for quick access
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_WARNING("Config: Cannot open file for loading: " + filepath);
        return false;
    }
    
    // Try to detect file format
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    // Trim whitespace
    size_t first = content.find_first_not_of(" \t\n\r");
    size_t last = content.find_last_not_of(" \t\n\r");
    if (first == std::string::npos) {
        NLM_LOG_WARNING("Config: File is empty: " + filepath);
        return false;
    }
    
    std::string trimmedContent = content.substr(first, last - first + 1);
    
    // Try JSON parsing first (modern format)
    if (trimmedContent.find('{') != std::string::npos && trimmedContent.find('}') != std::string::npos) {
        try {
#ifdef HAS_JSON_LIB
            json j = json::parse(trimmedContent);
            // Convert JSON to Config entries
            for (auto& [key, value] : j.items()) {
                std::string valueStr;
                if (value.is_string()) {
                    valueStr = value.get<std::string>();
                } else if (value.is_number_integer()) {
                    valueStr = std::to_string(value.get<int64_t>());
                } else if (value.is_number_unsigned()) {
                    valueStr = std::to_string(value.get<uint64_t>());
                } else if (value.is_number_float()) {
                    valueStr = std::to_string(value.get<double>());
                } else if (value.is_boolean()) {
                    valueStr = value.get<bool>() ? "true" : "false";
                } else {
                    NLM_LOG_WARNING("Config: Unsupported JSON value type for key " + key);
                    continue;
                }
                set(key, valueStr, ConfigSource::File);
            }
            return true;
#else
            NLM_LOG_INFO("Config: JSON library not available, falling back to simple format");
#endif
        } catch (const std::exception& e) {
            NLM_LOG_INFO("Config: JSON parsing failed: " + std::string(e.what()) + ", falling back to simple format");
        }
    }
    
    // Fall back to simple key=value format for backward compatibility
    std::istringstream stream(trimmedContent);
    std::string line;
    size_t lineNum = 0;
    
    while (std::getline(stream, line)) {
        ++lineNum;
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
            NLM_LOG_WARNING("Config: Line " + std::to_string(lineNum) + " does not contain '=': " + line);
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    if (argc < 1 || argv == nullptr) {
        return true; // Nothing to parse
    }
    
    for (int i = 1; i < argc; ++i) {
        if (argv[i] == nullptr) {
            continue; // Skip null pointers
        }
        
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            } else {
                NLM_LOG_WARNING("Config: Invalid command line argument format: " + arg);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc && argv[i + 1] != nullptr) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        } else {
            NLM_LOG_WARNING("Config: Unrecognized command line argument: " + arg);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Config: Cannot open file for writing: " + filepath);
        return false;
    }
    
    // Write in a modern format (JSON if available)
#ifdef HAS_JSON_LIB
    try {
        json j;
        for (const auto& entry : pImpl->entries) {
            std::string key = entry.key;
            if constexpr (std::is_same_v<decltype(entry.value), std::variant<std::string, int64_t, double, bool>>) {
                std::visit([&j, &key](auto&& arg) {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, std::string>) {
                        j[key] = arg;
                    } else if constexpr (std::is_same_v<T, int64_t>) {
                        j[key] = arg;
                    } else if constexpr (std::is_same_v<T, double>) {
                        j[key] = arg;
                    } else if constexpr (std::is_same_v<T, bool>) {
                        j[key] = arg;
                    }
                }, entry.value);
            }
        }
        file << j.dump(4) << std::endl;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Config: Failed to write JSON: " + std::string(e.what()) + ", falling back to simple format");
        // Fall back to simple format
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            std::visit([&file, &entry](auto&& arg) {
                file << entry.key << " = ";
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else {
                    file << arg;
                }
            }, entry.value);
            file << "\n\n";
        }
    }
#else
    // Fallback to simple format
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        std::visit([&file, &entry](auto&& arg) {
            file << entry.key << " = ";
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else {
                file << arg;
            }
        }, entry.value);
        file << "\n\n";
    }
#endif
    
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
