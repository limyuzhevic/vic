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

// Implementation of config helper functions
namespace nlm {
    namespace config {
        
        template<typename T>
        bool isType(const ConfigValue& value) {
            return std::holds_alternative<T>(value);
        }
        
        template<typename T>
        std::optional<T> tryGetAs(const ConfigValue& value) {
            if (isType<T>(value)) {
                return std::get<T>(value);
            }
            return std::nullopt;
        }
        
        bool validateRange(const ConfigValue& value, double min, double max) {
            if (std::holds_alternative<int>(value)) {
                return static_cast<double>(std::get<int>(value)) >= min && 
                       static_cast<double>(std::get<int>(value)) <= max;
            } else if (std::holds_alternative<double>(value)) {
                double val = std::get<double>(value);
                return val >= min && val <= max;
            }
            return false;
        }
        
        bool validatePositive(const ConfigValue& value) {
            if (std::holds_alternative<int>(value)) {
                return std::get<int>(value) >= 0;
            } else if (std::holds_alternative<double>(value)) {
                return std::get<double>(value) >= 0.0;
            }
            return false;
        }
        
        bool validateNonEmpty(const ConfigValue& value) {
            if (std::holds_alternative<std::string>(value)) {
                return !std::get<std::string>(value).empty();
            } else if (std::holds_alternative<std::vector<int>>(value)) {
                return !std::get<std::vector<int>>(value).empty();
            } else if (std::holds_alternative<std::vector<double>>(value)) {
                return !std::get<std::vector<double>>(value).empty();
            } else if (std::holds_alternative<std::vector<std::string>>(value)) {
                return !std::get<std::vector<std::string>>(value).empty();
            }
            return false;
        }
        
        template<typename T>
        T getRequired(const Config& config, const std::string& key) {
            auto value = config.get<T>(key);
            if (!value) {
                throw std::runtime_error("Missing required configuration key: " + key);
            }
            return *value;
        }
        
        template<typename T>
        std::optional<T> getOptional(const Config& config, const std::string& key, const T& defaultValue) {
            auto value = config.get<T>(key);
            return value.has_value() ? value.value() : defaultValue;
        }
    }
}

// Explicit template instantiations
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

// Explicit instantiations for helper functions
namespace nlm {
    namespace config {
        template bool isType<int>(const ConfigValue& value);
        template bool isType<int64_t>(const ConfigValue& value);
        template bool isType<double>(const ConfigValue& value);
        template bool isType<bool>(const ConfigValue& value);
        template bool isType<std::string>(const ConfigValue& value);
        
        template std::optional<int> tryGetAs<int>(const ConfigValue& value);
        template std::optional<int64_t> tryGetAs<int64_t>(const ConfigValue& value);
        template std::optional<double> tryGetAs<double>(const ConfigValue& value);
        template std::optional<bool> tryGetAs<bool>(const ConfigValue& value);
        template std::optional<std::string> tryGetAs<std::string>(const ConfigValue& value);
    }
}

// Explicit instantiations for getRequired and getOptional
namespace nlm {
    namespace config {
        template int getRequired<int>(const Config&, const std::string&);
        template int64_t getRequired<int64_t>(const Config&, const std::string&);
        template double getRequired<double>(const Config&, const std::string&);
        template bool getRequired<bool>(const Config&, const std::string&);
        template std::string getRequired<std::string>(const Config&, const std::string&);
        
        template std::optional<int> getOptional<int>(const Config&, const std::string&, const int&);
        template std::optional<int64_t> getOptional<int64_t>(const Config&, const std::string&, const int64_t&);
        template std::optional<double> getOptional<double>(const Config&, const std::string&, const double&);
        template std::optional<bool> getOptional<bool>(const Config&, const std::string&, const bool&);
        template std::optional<std::string> getOptional<std::string>(const Config&, const std::string&, const std::string&);
    }
}

} // namespace nlm
