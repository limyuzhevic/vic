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
        // Use std::filesystem to check if file exists first
        if (!std::filesystem::exists(filepath)) {
            return false;
        }
        
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Set exception handling for file operations
        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        
        std::string line;
        int lineNum = 0;
        try {
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
                    
                    // Validate key is not empty
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
                }
            }
        } catch (const std::ifstream::failure& e) {
            // File read error - close file and return failure
            file.close();
            return false;
        }
        
        // Check if file is still good
        if (!file.good()) {
            file.close();
            return false;
        }
        
        file.close();
        return true;
        
    } catch (const std::exception& e) {
        // Any other exception - return failure
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    try {
        if (argc <= 1) {
            return true; // No arguments to process
        }
        
        for (int i = 1; i < argc; ++i) {
            try {
                std::string arg(argv[i]);
                
                // Handle --key=value format
                if (arg.substr(0, 2) == "--") {
                    size_t pos = arg.find('=');
                    if (pos != std::string::npos) {
                        std::string key = arg.substr(2, pos - 2);
                        std::string value = arg.substr(pos + 1);
                        
                        // Validate key is not empty
                        if (!key.empty()) {
                            set(key, value, ConfigSource::CommandLine);
                        }
                    }
                }
                // Handle -key value format
                else if (arg[0] == '-' && i + 1 < argc) {
                    std::string key = arg.substr(1);
                    std::string value = argv[++i];
                    
                    // Validate key is not empty
                    if (!key.empty()) {
                        set(key, value, ConfigSource::CommandLine);
                    }
                }
            } catch (const std::exception& e) {
                // If one argument fails, continue with others
                // Don't let malformed arguments break the entire process
                continue;
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        // Any other exception - return failure
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Set exception handling for file operations
        file.exceptions(std::ofstream::failbit | std::ofstream::badbit);
        
        try {
            for (const auto& entry : pImpl->entries) {
                // Write description if available
                if (!entry.description.empty()) {
                    file << "# " << entry.description << "\n";
                }
                file << entry.key << " = ";
                std::visit([&file](auto&& arg) {
                    if constexpr (std::is_same_v<decltype(arg), std::string>) {
                        file << "\"" << arg << "\"";
                    } else if constexpr (std::is_same_v<decltype(arg), bool>) {
                        file << (arg ? "true" : "false");
                    } else {
                        file << arg;
                    }
                }, entry.value);
                file << "\n";
            }
        } catch (const std::ofstream::failure& e) {
            // File write error - close file and return failure
            file.close();
            return false;
        }
        
        // Check if file is still good
        if (!file.good()) {
            file.close();
            return false;
        }
        
        file.close();
        return true;
        
    } catch (const std::exception& e) {
        // Any other exception - return failure
        return false;
    }
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    try {
        // Validate key is not empty
        if (key.empty()) {
            return; // Don't allow empty keys
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
        // If setting fails, we can't do much but we shouldn't crash
        // The config should be resilient to individual failures
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
    try {
        // Validate key is not empty
        if (key.empty()) {
            return; // Don't allow empty keys
        }
        
        pImpl->entries.erase(
            std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
                [&key](const ConfigEntry& e) { return e.key == key; }),
            pImpl->entries.end()
        );
    } catch (const std::exception& e) {
        // If removal fails, we can't do much but we shouldn't crash
        // The config should be resilient to individual failures
    }
}

std::vector<std::string> Config::getKeys() const {
    try {
        std::vector<std::string> keys;
        keys.reserve(pImpl->entries.size());
        for (const auto& entry : pImpl->entries) {
            keys.push_back(entry.key);
        }
        return keys;
    } catch (const std::exception& e) {
        // If getting keys fails, return empty vector
        // The config should be resilient to individual failures
        return std::vector<std::string>();
    }
}

void Config::clear() {
    try {
        pImpl->entries.clear();
    } catch (const std::exception& e) {
        // If clearing fails, we can't do much but we shouldn't crash
        // The config should be resilient to individual failures
    }
}

std::string Config::summary() const {
    try {
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
    } catch (const std::exception& e) {
        // If generating summary fails, return basic information
        return "Configuration error: Could not generate summary\n";
    }
}

std::string Config::trim(const std::string& str) {
    try {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    } catch (const std::exception& e) {
        // If trimming fails, return the original string (or empty string)
        return str;
    }
}

std::string Config::toLower(const std::string& str) {
    try {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    } catch (const std::exception& e) {
        // If conversion fails, return the original string
        return str;
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    try {
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
    } catch (const std::exception& e) {
        // If getting value fails, return nullopt
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    try {
        auto val = get<T>(key);
        return val.has_value() ? val.value() : defaultValue;
    } catch (const std::exception& e) {
        // If getting value fails, return default value
        return defaultValue;
    }
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

} // namespace nlm
