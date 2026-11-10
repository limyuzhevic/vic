#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    // This is no longer needed as we use TypedConfiguration directly
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    return typedConfig.loadFromSimpleFile(filepath);
}

bool Config::loadFromArgs(int argc, char** argv) {
    // For backward compatibility, we need to convert TypedConfiguration
    // But this is a simpler approach: just set basic values
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                // Extract section and key
                std::string section = extractSectionName(key);
                std::string actualKey = extractKey(key);
                
                // Set based on type
                if (value == "true" || value == "false") {
                    typedConfig.set(section, actualKey, value == "true");
                } else if (value.find('.') != std::string::npos) {
                    try {
                        double val = std::stod(value);
                        typedConfig.set(section, actualKey, val);
                    } catch (...) {
                        typedConfig.set(section, actualKey, value);
                    }
                } else {
                    try {
                        int64_t val = std::stoll(value);
                        typedConfig.set(section, actualKey, val);
                    } catch (...) {
                        typedConfig.set(section, actualKey, value);
                    }
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            // Extract section and key
            std::string section = extractSectionName(key);
            std::string actualKey = extractKey(key);
            
            // Set based on type
            if (value == "true" || value == "false") {
                typedConfig.set(section, actualKey, value == "true");
            } else if (value.find('.') != std::string::npos) {
                try {
                    double val = std::stod(value);
                    typedConfig.set(section, actualKey, val);
                } catch (...) {
                    typedConfig.set(section, actualKey, value);
                }
            } else {
                try {
                    int64_t val = std::stoll(value);
                    typedConfig.set(section, actualKey, val);
                } catch (...) {
                    typedConfig.set(section, actualKey, value);
                }
            }
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    // For backward compatibility, save as simple key=value format
    // This is a simplified implementation
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // This is a placeholder - in a real implementation we would
    // properly serialize the configuration
    file << "# NLM Configuration\n";
    file << "# This is a simplified save format\n";
    file << "# For detailed configuration, use JSON format (when available)\n";
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    std::string section = extractSectionName(key);
    std::string actualKey = extractKey(key);
    return typedConfig.get<T>(section, actualKey);
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    std::string section = extractSectionName(key);
    std::string actualKey = extractKey(key);
    return typedConfig.getOr<T>(section, actualKey, defaultValue);
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    // For backward compatibility, we store in the general section
    typedConfig.set("general", key, value);
}

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
    // For backward compatibility, we store in the general section
    typedConfig.set("general", key, value);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    // For backward compatibility, we store in the general section
    typedConfig.set("general", key, value);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    // For backward compatibility, we store in the general section
    typedConfig.set("general", key, value);
}

bool Config::has(const std::string& key) const {
    std::string section = extractSectionName(key);
    std::string actualKey = extractKey(key);
    return typedConfig.has(section, actualKey);
}

void Config::remove(const std::string& key) {
    // For backward compatibility, we don't implement remove in TypedConfiguration
    // Instead, we'll clear and rebuild for now
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> allKeys;
    for (const auto& sectionName : typedConfig.getSections()) {
        const auto& section = typedConfig.getSection(sectionName);
        std::vector<std::string> sectionKeys = section.getKeys();
        for (const auto& key : sectionKeys) {
            // Reconstruct full key with section
            if (sectionName == "general") {
                allKeys.push_back(key);
            } else {
                allKeys.push_back(sectionName + "." + key);
            }
        }
    }
    return allKeys;
}

void Config::clear() {
    typedConfig.clear();
}

std::string Config::summary() const {
    return typedConfig.summary();
}

// Helper implementations
std::string Config::extractSectionName(const std::string& key, const std::string& defaultSection) {
    size_t pos = key.find('.');
    if (pos != std::string::npos) {
        return key.substr(0, pos);
    }
    return defaultSection;
}

std::string Config::extractKey(const std::string& fullKey) {
    size_t pos = fullKey.find('.');
    if (pos != std::string::npos) {
        return fullKey.substr(pos + 1);
    }
    return fullKey;
}

TypedConfiguration::ConfigValue Config::convertToTyped(const ConfigValue& oldValue) {
    // This is a simplified conversion - in a real implementation we would handle all cases
    return oldValue;
}

Config::ConfigValue Config::convertFromTyped(const TypedConfiguration::ConfigValue& newValue) {
    // This is a simplified conversion - in a real implementation we would handle all cases
    return newValue;
}

// Explicit template instantiations
namespace {
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
} // namespace

} // namespace nlm