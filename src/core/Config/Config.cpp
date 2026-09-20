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
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    std::string line;
    size_t lineNum = 0;
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
            
            // Validate key format
            if (key.empty()) {
                NLM_LOG_WARNING(std::string("Empty key at line ") + std::to_string(lineNum));
                continue;
            }
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Validate before setting
            ConfigValue configValue;
            
            // Try to infer type
            if (value.find('.') != std::string::npos) {
                try {
                    double doubleVal = std::stod(value);
                    configValue = doubleVal;
                } catch (...) {
                    configValue = value;
                }
            } else if (value == "true" || value == "false") {
                configValue = (value == "true");
            } else {
                try {
                    int intVal = std::stoi(value);
                    configValue = intVal;
                } catch (...) {
                    configValue = value;
                }
            }
            
            // Validate and set
            if (!validate(key, configValue)) {
                NLM_LOG_WARNING(std::string("Invalid value for key ") + key + std::string(" at line ") + std::to_string(lineNum));
            }
            
            set(key, configValue, ConfigSource::File);
            
            NLM_LOG_DEBUG(std::string("Loaded config: ") + key + std::string(" = ") + (std::holds_alternative<std::string>(configValue) ? "\"" + std::get<std::string>(configValue) + "\"" : std::visit([](auto&& arg) { return std::to_string(arg); }, configValue)));
        } else {
            NLM_LOG_WARNING(std::string("Invalid line format at ") + std::to_string(lineNum) + std::string(": ") + line);
        }
    }
    
    NLM_LOG_INFO(std::string("Loaded ") + std::to_string(pImpl->entries.size()) + std::string(" configuration entries from ") + filepath);
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
        
        // Write the actual value based on its type
        std::visit([&file, &entry](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            
            if constexpr (std::is_same_v<T, int>) {
                file << entry.key << " = " << arg << "\n";
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << entry.key << " = " << arg << "\n";
            } else if constexpr (std::is_same_v<T, double>) {
                file << entry.key << " = " << arg << "\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << entry.key << " = " << (arg ? "true" : "false") << "\n";
            } else if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << arg << "\"\n";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]\n";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]\n";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << arg[i] << "\"";
                }
                file << "]\n";
            }
        }, entry.value);
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

bool Config::validate(const std::string& key, const ConfigValue& value) const {
    // Basic validation - could be extended with more sophisticated checks
    if (key.empty()) {
        return false;
    }
    
    // Validate based on value type
    std::visit([this](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        
        if constexpr (std::is_same_v<T, int>) {
            // Could add validation for int range, etc.
            return;
        } else if constexpr (std::is_same_v<T, double>) {
            // Could add validation for double range, NaN checks, etc.
            return;
        } else if constexpr (std::is_same_v<T, bool>) {
            // Boolean validation is straightforward
            return;
        } else if constexpr (std::is_same_v<T, std::string>) {
            // Could add validation for string content
            return;
        }
    }, value);
    
    return true;
}

bool Config::validateAll() const {
    for (const auto& entry : pImpl->entries) {
        if (!validate(entry.key, entry.value)) {
            NLM_LOG_WARNING("Invalid configuration key-value pair: " + entry.key);
            return false;
        }
    }
    return true;
}

size_t Config::getEntryCount() const {
    return pImpl->entries.size();
}

std::vector<std::pair<std::string, ConfigSource>> Config::getEntriesWithSource() const {
    std::vector<std::pair<std::string, ConfigSource>> result;
    result.reserve(pImpl->entries.size());
    
    for (const auto& entry : pImpl->entries) {
        result.emplace_back(entry.key, entry.source);
    }
    
    return result;
}
