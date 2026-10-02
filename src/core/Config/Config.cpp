#include <unordered_map>
#include <filesystem>
#include <fstream>
#include <vector>
#include <stdexcept>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    // Optimized lookup table for fast key access
    std::unordered_map<std::string, size_t> keyToIndex;
    bool needsRebuild = false;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with empty optimized lookup
    pImpl->keyToIndex.reserve(100); // Pre-allocate for common configs
}

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
        // Output the actual value based on its type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << arg << "\"\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << entry.key << " = " << (arg ? "true" : "false") << "\n";
            } else if constexpr (std::is_same_v<T, int> || 
                                std::is_same_v<T, int64_t> ||
                                std::is_same_v<T, double>) {
                file << entry.key << " = " << arg << "\n";
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

// Helper implementations

bool Config::validateKey(const std::string& key) const {
    if (key.empty() || key.size() > 100) {
        return false;
    }
    
    // Check for valid characters
    for (char c : key) {
        if (!std::isalnum(c) && c != '_' && c != '-') {
            return false;
        }
    }
    
    return true;
}

bool Config::validateValue(const ConfigValue& value) const {
    std::visit([](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            if (arg.size() > 1000) {
                throw std::runtime_error("String value too large");
            }
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            if (arg.size() > 1000) {
                throw std::runtime_error("Integer vector too large");
            }
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            if (arg.size() > 1000) {
                throw std::runtime_error("Double vector too large");
            }
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            if (arg.size() > 100) {
                throw std::runtime_error("String vector too large");
            }
            for (const auto& str : arg) {
                if (str.size() > 100) {
                    throw std::runtime_error("String vector element too large");
                }
            }
        }
    }, value);
    
    return true;
}

bool Config::valuesEqual(const ConfigValue& a, const ConfigValue& b, bool ignoreSource) {
    if (std::get_if<int>(&a) && std::get_if<int>(&b)) {
        return std::get<int>(a) == std::get<int>(b);
    } else if (std::get_if<int64_t>(&a) && std::get_if<int64_t>(&b)) {
        return std::get<int64_t>(a) == std::get<int64_t>(b);
    } else if (std::get_if<double>(&a) && std::get_if<double>(&b)) {
        return std::abs(std::get<double>(a) - std::get<double>(b)) < 0.0001f;
    } else if (std::get_if<bool>(&a) && std::get_if<bool>(&b)) {
        return std::get<bool>(a) == std::get<bool>(b);
    } else if (std::get_if<std::string>(&a) && std::get_if<std::string>(&b)) {
        return std::get<std::string>(a) == std::get<std::string>(b);
    } else if (std::get_if<std::vector<int>>(&a) && std::get_if<std::vector<int>>(&b)) {
        const auto& vecA = std::get<std::vector<int>>(a);
        const auto& vecB = std::get<std::vector<int>>(b);
        if (vecA.size() != vecB.size()) return false;
        for (size_t i = 0; i < vecA.size(); ++i) {
            if (vecA[i] != vecB[i]) return false;
        }
        return true;
    } else if (std::get_if<std::vector<double>>(&a) && std::get_if<std::vector<double>>(&b)) {
        const auto& vecA = std::get<std::vector<double>>(a);
        const auto& vecB = std::get<std::vector<double>>(b);
        if (vecA.size() != vecB.size()) return false;
        for (size_t i = 0; i < vecA.size(); ++i) {
            if (std::abs(vecA[i] - vecB[i]) >= 0.0001f) return false;
        }
        return true;
    } else if (std::get_if<std::vector<std::string>>(&a) && std::get_if<std::vector<std::string>>(&b)) {
        const auto& vecA = std::get<std::vector<std::string>>(a);
        const auto& vecB = std::get<std::vector<std::string>>(b);
        if (vecA.size() != vecB.size()) return false;
        for (size_t i = 0; i < vecA.size(); ++i) {
            if (vecA[i] != vecB[i]) return false;
        }
        return true;
    }
    
    return false;
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

bool Config::loadBatch(const std::vector<std::string>& filepaths) {
    if (filepaths.empty()) return true;
    
    bool allSuccess = true;
    for (const auto& filepath : filepaths) {
        if (!loadFromFile(filepath)) {
            allSuccess = false;
        }
    }
    return allSuccess;
}

bool Config::saveBatch(const std::vector<std::string>& filepaths) const {
    if (filepaths.empty()) return true;
    
    bool allSuccess = true;
    for (const auto& filepath : filepaths) {
        if (!saveToFile(filepath)) {
            allSuccess = false;
        }
    }
    return allSuccess;
}

bool Config::validateConfig(std::vector<std::string>& errors) const {
    errors.clear();
    bool isValid = true;
    
    for (const auto& entry : pImpl->entries) {
        // Validate key
        if (!validateKey(entry.key)) {
            errors.push_back("Invalid key name: \"" + entry.key + "\"");
            isValid = false;
        }
        
        // Validate value based on key type
        if (!validateValue(entry.value)) {
            errors.push_back("Invalid value for key \"" + entry.key + "\"");
            isValid = false;
        }
        
        // Validate source
        if (entry.source == ConfigSource::Default && entry.key.find("default_") != std::string::npos) {
            errors.push_back("Default source used for key \"" + entry.key + "\"");
            isValid = false;
        }
    }
    
    return isValid;
}

bool Config::compareConfigs(const Config& other, bool ignoreSource) const {
    if (pImpl->entries.size() != other.pImpl->entries.size()) {
        return false;
    }
    
    for (const auto& thisEntry : pImpl->entries) {
        bool found = false;
        for (const auto& otherEntry : other.pImpl->entries) {
            if (thisEntry.key == otherEntry.key) {
                found = true;
                if (!valuesEqual(thisEntry.value, otherEntry.value, ignoreSource)) {
                    return false;
                }
                break;
            }
        }
        if (!found) {
            return false;
        }
    }
    return true;
}

std::string Config::diff(const Config& other) const {
    std::ostringstream oss;
    
    // Check for keys only in this config
    for (const auto& thisEntry : pImpl->entries) {
        bool foundInOther = false;
        for (const auto& otherEntry : other.pImpl->entries) {
            if (thisEntry.key == otherEntry.key) {
                foundInOther = true;
                if (!valuesEqual(thisEntry.value, otherEntry.value, false)) {
                    oss << "Difference in key \"" << thisEntry.key << "\":\n";
                    oss << "  This: ";
                    std::visit([&oss](auto&& arg) {
                        using T = std::decay_t<decltype(arg)>;
                        if constexpr (std::is_same_v<T, std::string>) {
                            oss << "\"" << arg << "\"";
                        } else {
                            oss << arg;
                        }
                    }, thisEntry.value);
                    oss << "\n";
                    oss << "  Other: ";
                    std::visit([&oss](auto&& arg) {
                        using T = std::decay_t<decltype(arg)>;
                        if constexpr (std::is_same_v<T, std::string>) {
                            oss << "\"" << arg << "\"";
                        } else {
                            oss << arg;
                        }
                    }, otherEntry.value);
                    oss << "\n";
                }
                break;
            }
        }
        if (!foundInOther) {
            oss << "Key \"" << thisEntry.key << "\" only in this config\n";
        }
    }
    
    // Check for keys only in other config
    for (const auto& otherEntry : other.pImpl->entries) {
        bool foundInThis = false;
        for (const auto& thisEntry : pImpl->entries) {
            if (thisEntry.key == otherEntry.key) {
                foundInThis = true;
                break;
            }
        }
        if (!foundInThis) {
            oss << "Key \"" << otherEntry.key << "\" only in other config\n";
        }
    }
    
    if (oss.str().empty()) {
        return "Configurations are identical\n";
    }
    
    return oss.str();
}

bool Config::saveTemplate(const std::string& name) {
    if (name.empty() || name.find('/') != std::string::npos) {
        return false;
    }
    
    std::string templatePath = TEMPLATES_DIR + "/" + name + CONFIG_EXTENSION;
    return saveToFile(templatePath);
}

bool Config::loadTemplate(const std::string& name) {
    if (name.empty() || name.find('/') != std::string::npos) {
        return false;
    }
    
    std::string templatePath = TEMPLATES_DIR + "/" + name + CONFIG_EXTENSION;
    return loadFromFile(templatePath);
}

bool Config::applyTemplate(const Config& templateConfig) {
    bool allSuccess = true;
    
    // Clear current config
    clear();
    
    // Apply template entries
    for (const auto& entry : templateConfig.pImpl->entries) {
        set(entry.key, entry.value, entry.source);
    }
    
    return allSuccess;
}

void Config::removeKeys(const std::vector<std::string>& keys) {
    for (const auto& key : keys) {
        remove(key);
    }
}
