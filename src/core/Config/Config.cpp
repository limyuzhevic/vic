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
    
    // Validate file path
    if (filepath.empty() || filepath.find('/') == std::string::npos) {
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    size_t lineNum = 0;
    while (std::getline(file, line)) {
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
            
            // Validate key
            if (key.empty()) {
                return false;
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

bool Config::validate() const {
    for (const auto& entry : pImpl->entries) {
        bool valid = false;
        if (std::holds_alternative<int>(entry.value) ||
            std::holds_alternative<int64_t>(entry.value) ||
            std::holds_alternative<double>(entry.value) ||
            std::holds_alternative<bool>(entry.value) ||
            std::holds_alternative<std::string>(entry.value)) {
            valid = true;
        }
        if (std::holds_alternative<std::vector<int>>(entry.value) ||
            std::holds_alternative<std::vector<double>>(entry.value) ||
            std::holds_alternative<std::vector<std::string>>(entry.value)) {
            valid = true;
        }
        
        if (!valid) {
            throw std::runtime_error("Invalid value type for key: " + entry.key);
        }
    }
    return true;
}

size_t Config::getEntryCount() const {
    return pImpl->entries.size();
}

size_t Config::getEntryCount(ConfigSource source) const {
    return std::count_if(pImpl->entries.begin(), pImpl->entries.end(),
        [source](const ConfigEntry& e) { return e.source == source; });
}

bool Config::hasSourceConflict(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    return (it != pImpl->entries.end());
}

ConfigSource Config::getPrimarySource(const std::string& key) const {
    ConfigSource bestSource = ConfigSource::Default;
    int bestPriority = 0;
    
    for (const auto& entry : pImpl->entries) {
        if (entry.key == key) {
            int priority = static_cast<int>(entry.source);
            if (priority > bestPriority) {
                bestPriority = priority;
                bestSource = entry.source;
            }
        }
    }
    
    return bestSource;
}

bool Config::isValidInt(const std::string& value, int64_t& out) {
    try {
        size_t pos;
        int64_t result = std::stoll(value, &pos);
        if (pos != value.size()) {
            return false;
        }
        out = result;
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool Config::isValidDouble(const std::string& value, double& out) {
    try {
        size_t pos;
        double result = std::stod(value, &pos);
        if (pos != value.size()) {
            return false;
        }
        out = result;
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool Config::isValidBool(const std::string& value, bool& out) {
    if (value == "true" || value == "1" || value == "yes") {
        out = true;
        return true;
    } else if (value == "false" || value == "0" || value == "no") {
        out = false;
        return true;
    }
    return false;
}

std::string Config::getLastError() const {
    // Placeholder for error handling system
    return "";
}

void Config::clearLastError() {
    // Placeholder for error handling system
}

void Config::merge(const Config& other, ConfigSource source) {
    for (const auto& entry : other.pImpl->entries) {
        // Override with new value
        set(entry.key, entry.value, source);
    }
}

} // namespace nlm
