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
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            // Log file open error
            return false;
        }
        
        // Check if file is readable (try to read first character)
        file.peek();
        if (file.fail()) {
            return false;
        }
        
        std::string line;
        size_t lineNumber = 0;
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
                
                // Validate key
                if (key.empty()) {
                    continue; // Skip malformed lines
                }
                
                // Remove quotes if present
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                set(key, value, ConfigSource::File);
            } else {
                // Log malformed line but continue processing
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        // Log exception but don't throw - maintain backward compatibility
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    try {
        for (int i = 1; i < argc; ++i) {
            if (argv[i] == nullptr) {
                continue; // Skip null pointers
            }
            
            std::string arg(argv[i]);
            
            // Handle --key=value format
            if (arg.size() >= 3 && arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    
                    // Validate key
                    if (!key.empty()) {
                        set(key, value, ConfigSource::CommandLine);
                    }
                }
            }
            // Handle -key value format
            else if (arg.size() >= 2 && arg[0] == '-' && i + 1 < argc && argv[i + 1] != nullptr) {
                std::string key = arg.substr(1);
                std::string value = argv[++i];
                
                // Validate key
                if (!key.empty()) {
                    set(key, value, ConfigSource::CommandLine);
                }
            }
        }
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Set output precision and formatting
        file.precision(6);
        file.setf(std::ios::fixed);
        
        for (const auto& entry : pImpl->entries) {
            // Write description as comment
            file << "# " << entry.description << "\n";
            
            // Write key-value pair with proper formatting
            file << entry.key << " = ";
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (arg ? "true" : "false");
                } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
                    file << arg;
                } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                    file << arg;
                } else {
                    file << arg;
                }
            }, entry.value);
            file << "\n\n";
        }
        
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    try {
        // Validate key
        if (key.empty()) {
            return std::nullopt;
        }
        
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
        
    } catch (const std::exception& e) {
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    try {
        auto val = get<T>(key);
        return val.has_value() ? val.value() : defaultValue;
    } catch (const std::exception& e) {
        return defaultValue;
    }
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    try {
        // Validate key
        if (key.empty()) {
            return; // Silently ignore empty keys
        }
        
        // Validate source
        if (source < ConfigSource::Default || source > ConfigSource::Runtime) {
            source = ConfigSource::Runtime; // Default to runtime for invalid sources
        }
        
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it != pImpl->entries.end()) {
            it->value = value;
            it->source = source;
        } else {
            pImpl->entries.emplace_back(key, value, source);
        }
        
    } catch (const std::exception& e) {
        // Silently ignore errors - maintain backward compatibility
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    try {
        set(key, ConfigValue(value), source);
    } catch (const std::exception& e) {
        // Silently ignore errors
    }
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    try {
        set(key, ConfigValue(value), source);
    } catch (const std::exception& e) {
        // Silently ignore errors
    }
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    try {
        set(key, ConfigValue(value), source);
    } catch (const std::exception& e) {
        // Silently ignore errors
    }
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    try {
        set(key, ConfigValue(value), source);
    } catch (const std::exception& e) {
        // Silently ignore errors
    }
}

bool Config::has(const std::string& key) const {
    try {
        // Validate key
        if (key.empty()) {
            return false;
        }
        
        return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
    } catch (const std::exception& e) {
        return false;
    }
}

void Config::remove(const std::string& key) {
    try {
        // Validate key
        if (key.empty()) {
            return; // Silently ignore empty keys
        }
        
        pImpl->entries.erase(
            std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
                [&key](const ConfigEntry& e) { return e.key == key; }),
            pImpl->entries.end()
        );
        
    } catch (const std::exception& e) {
        // Silently ignore errors
    }
}

std::vector<std::string> Config::getKeys() const {
    try {
        std::vector<std::string> keys;
        keys.reserve(pImpl->entries.size());
        
        for (const auto& entry : pImpl->entries) {
            // Validate key before adding
            if (!entry.key.empty()) {
                keys.push_back(entry.key);
            }
        }
        
        return keys;
        
    } catch (const std::exception& e) {
        return std::vector<std::string>();
    }
}

void Config::clear() {
    try {
        pImpl->entries.clear();
    } catch (const std::exception& e) {
        // Silently ignore errors
    }
}

std::string Config::summary() const {
    try {
        std::ostringstream oss;
        oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
        
        for (const auto& entry : pImpl->entries) {
            // Validate entry before processing
            if (entry.key.empty()) {
                continue; // Skip malformed entries
            }
            
            oss << "  " << entry.key << " = [";
            try {
                std::visit([&oss](auto&& arg) {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, std::string>) {
                        oss << "\"" << arg << "\"";
                    } else if constexpr (std::is_same_v<T, bool>) {
                        oss << (arg ? "true" : "false");
                    } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
                        oss << arg;
                    } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                        oss << arg;
                    } else {
                        oss << arg;
                    }
                }, entry.value);
            } catch (const std::exception& e) {
                oss << "ERROR: " << e.what();
            }
            oss << "] (" << static_cast<int>(entry.source) << ")\n";
        }
        
        return oss.str();
        
    } catch (const std::exception& e) {
        return "Error generating configuration summary: " + std::string(e.what());
    }
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
