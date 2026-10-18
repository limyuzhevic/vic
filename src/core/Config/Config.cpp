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
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
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
            
            set(key, value, ConfigSource::File);
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
            // Handle --flag (boolean true) format
            else if (arg.size() >= 3) {
                std::string key = arg.substr(2);
                set(key, true, ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
        // Handle standalone --key (for disabling boolean flags)
        else if (arg.substr(0, 2) == "--" && arg.size() > 2) {
            std::string key = arg.substr(2);
            // Check if it's a negative flag (--no-key)
            if (key.substr(0, 3) == "no-") {
                std::string trueKey = key.substr(3);
                set(trueKey, false, ConfigSource::CommandLine);
            } else {
                set(key, true, ConfigSource::CommandLine);
            }
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
        
        // Helper function to convert ConfigValue to string
        auto valueToString = [this](const ConfigValue& val) -> std::string {
            std::ostringstream oss;
            std::visit([&oss](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    oss << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    oss << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        oss << arg[i];
                        if (i < arg.size() - 1) oss << ", ";
                    }
                    oss << "]";
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    oss << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        oss << arg[i];
                        if (i < arg.size() - 1) oss << ", ";
                    }
                    oss << "]";
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    oss << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        oss << "\"" << arg[i] << "\"";
                        if (i < arg.size() - 1) oss << ", ";
                    }
                    oss << "]";
                } else {
                    oss << arg;
                }
            }, val);
            return oss.str();
        };
        
        file << valueToString(entry.value) << "\n";
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
    
    // Special handling for vector types since they require different type checking
    using T = std::decay_t<T>;
    if constexpr (std::is_same_v<T, std::vector<int>> ||
                  std::is_same_v<T, std::vector<double>> ||
                  std::is_same_v<T, std::vector<std::string>>) {
        // For vector types, we need to visit the variant
        try {
            return std::get<T>(it->value);
        } catch (const std::bad_variant_access&) {
            // Also try to find if it's stored as a different vector type
            if constexpr (std::is_same_v<T, std::vector<int>>) {
                if (auto val = std::get_if<std::vector<double>>(&it->value)) {
                    std::vector<int> result;
                    result.reserve(val->size());
                    for (double v : *val) result.push_back(static_cast<int>(v));
                    return result;
                } else if (auto val = std::get_if<std::vector<std::string>>(&it->value)) {
                    // Try to convert strings to ints
                    std::vector<int> result;
                    result.reserve(val->size());
                    for (const std::string& s : *val) {
                        try {
                            result.push_back(std::stoi(s));
                        } catch (...) {
                            result.push_back(0);
                        }
                    }
                    return result;
                }
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                if (auto val = std::get_if<std::vector<int>>(&it->value)) {
                    std::vector<double> result;
                    result.reserve(val->size());
                    for (int v : *val) result.push_back(static_cast<double>(v));
                    return result;
                } else if (auto val = std::get_if<std::vector<std::string>>(&it->value)) {
                    std::vector<double> result;
                    result.reserve(val->size());
                    for (const std::string& s : *val) {
                        try {
                            result.push_back(std::stod(s));
                        } catch (...) {
                            result.push_back(0.0);
                        }
                    }
                    return result;
                }
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                if (auto val = std::get_if<std::vector<int>>(&it->value)) {
                    std::vector<std::string> result;
                    result.reserve(val->size());
                    for (int v : *val) result.push_back(std::to_string(v));
                    return result;
                } else if (auto val = std::get_if<std::vector<double>>(&it->value)) {
                    std::vector<std::string> result;
                    result.reserve(val->size());
                    for (double v : *val) result.push_back(std::to_string(v));
                    return result;
                }
            }
            
            return std::nullopt;
        }
    } else {
        // For simple types, use the standard get
        try {
            return std::get<T>(it->value);
        } catch (const std::bad_variant_access&) {
            return std::nullopt;
        }
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

void Config::set(const std::string& key, const std::vector<int>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<double>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<std::string>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
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
