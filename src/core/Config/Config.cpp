#include "../core/Exception/NLMException.hpp"

namespace nlm {

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&& other) noexcept : pImpl(std::move(other.pImpl)) {}

Config& Config::operator=(Config&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool Config::loadFromFile(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file) {
            throw ConfigFileNotFoundException("Configuration file not found: " + filepath);
        }
        
        // Parse JSON file
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        
        // Simple JSON parsing (in real implementation, would use a proper JSON library)
        size_t pos = 0;
        while ((pos = content.find("{", pos)) != std::string::npos) {
            pos = content.find("}", pos + 1);
        }
        
        return true;
    } catch (const ConfigFileNotFoundException& e) {
        // Log error but don't throw - allow fallback to defaults
        NLM_LOG_ERROR("Failed to load config from file: " + std::string(e.what()));
        return false;
    } catch (const ConfigParseException& e) {
        NLM_LOG_ERROR("Failed to parse config file: " + std::string(e.what()));
        return false;
    } catch (...) {
        NLM_LOG_ERROR("Unexpected error loading config file: " + filepath);
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    try {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            // Check for --key=value pattern
            if (arg.find("--") == 0) {
                if (arg.find("=") != std::string::npos) {
                    // --key=value format
                    std::string key = arg.substr(2, arg.find("=") - 2);
                    std::string value = arg.substr(arg.find("=") + 1);
                    set(key, value, ConfigSource::CommandLine);
                } else if (i + 1 < argc && argv[i + 1][0] != '-') {
                    // --key value format
                    std::string key = arg.substr(2);
                    std::string value = argv[++i];
                    set(key, value, ConfigSource::CommandLine);
                }
            }
        }
        return true;
    } catch (...) {
        NLM_LOG_ERROR("Error parsing command line arguments");
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file) {
            throw ConfigFileWriteException("Cannot write configuration file: " + filepath);
        }
        
        file << "{\n";
        for (size_t i = 0; i < pImpl->entries.size(); ++i) {
            const auto& entry = pImpl->entries[i];
            file << "  \"" << entry.key << "\": ";
            
            // Handle different value types
            if (std::holds_alternative<int>(entry.value)) {
                file << std::get<int>(entry.value);
            } else if (std::holds_alternative<int64_t>(entry.value)) {
                file << std::get<int64_t>(entry.value);
            } else if (std::holds_alternative<double>(entry.value)) {
                file << std::get<double>(entry.value);
            } else if (std::holds_alternative<bool>(entry.value)) {
                file << (std::get<bool>(entry.value) ? "true" : "false");
            } else if (std::holds_alternative<std::string>(entry.value)) {
                file << "\"" << std::get<std::string>(entry.value) << "\"";
            }
            
            if (i < pImpl->entries.size() - 1) {
                file << ",\n";
            }
        }
        
        file << "}\n";
        return true;
    } catch (const ConfigFileWriteException& e) {
        NLM_LOG_ERROR(std::string(e.what()));
        return false;
    } catch (...) {
        NLM_LOG_ERROR("Unexpected error saving config file: " + filepath);
        return false;
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    try {
        // Convert key to lowercase for case-insensitive lookup
        std::string lowerKey = toLower(key);
        
        for (const auto& entry : pImpl->entries) {
            if (toLower(entry.key) == lowerKey) {
                // Check if value type matches requested type
                if constexpr (std::is_same_v<T, int> && std::holds_alternative<int>(entry.value)) {
                    return std::get<int>(entry.value);
                } else if constexpr (std::is_same_v<T, int64_t> && std::holds_alternative<int64_t>(entry.value)) {
                    return std::get<int64_t>(entry.value);
                } else if constexpr (std::is_same_v<T, double> && std::holds_alternative<double>(entry.value)) {
                    return std::get<double>(entry.value);
                } else if constexpr (std::is_same_v<T, bool> && std::holds_alternative<bool>(entry.value)) {
                    return std::get<bool>(entry.value);
                } else if constexpr (std::is_same_v<T, std::string> && std::holds_alternative<std::string>(entry.value)) {
                    return std::get<std::string>(entry.value);
                }
            }
        }
        return std::nullopt;
    } catch (...) {
        NLM_LOG_ERROR("Error getting config value for key: " + key);
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto value = get<T>(key);
    if (value) {
        return *value;
    }
    return defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    try {
        // Validate key
        if (key.empty()) {
            throw ConfigValidationException("Configuration key cannot be empty");
        }
        
        // Validate value based on key
        validateKeyValue(key, value);
        
        // Store entry
        pImpl->entries.emplace_back(key, value, source, "Auto-generated");
        
    } catch (const ConfigValidationException& e) {
        NLM_LOG_ERROR(std::string(e.what()));
        throw; // Re-throw validation errors
    } catch (...) {
        NLM_LOG_ERROR("Unexpected error setting config key: " + key);
        throw ConfigException("Failed to set config key: " + key);
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
    try {
        std::string lowerKey = toLower(key);
        return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
                         [&lowerKey](const ConfigEntry& entry) {
                             return toLower(entry.key) == lowerKey;
                         });
    } catch (...) {
        NLM_LOG_ERROR("Error checking config key: " + key);
        return false;
    }
}

void Config::remove(const std::string& key) {
    try {
        std::string lowerKey = toLower(key);
        auto it = std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
                                [&lowerKey](const ConfigEntry& entry) {
                                    return toLower(entry.key) == lowerKey;
                                });
        pImpl->entries.erase(it, pImpl->entries.end());
    } catch (...) {
        NLM_LOG_ERROR("Error removing config key: " + key);
        throw ConfigException("Failed to remove config key: " + key);
    }
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    try {
        for (const auto& entry : pImpl->entries) {
            keys.push_back(entry.key);
        }
    } catch (...) {
        NLM_LOG_ERROR("Error getting config keys");
        throw ConfigException("Failed to get config keys");
    }
    return keys;
}

void Config::clear() {
    try {
        pImpl->entries.clear();
    } catch (...) {
        NLM_LOG_ERROR("Error clearing config");
        throw ConfigException("Failed to clear config");
    }
}

std::string Config::summary() const {
    std::string result = "Configuration Summary:\n";
    for (const auto& entry : pImpl->entries) {
        result += "  " + entry.key + ": ";
        
        if (std::holds_alternative<int>(entry.value)) {
            result += std::to_string(std::get<int>(entry.value));
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            result += std::to_string(std::get<int64_t>(entry.value));
        } else if (std::holds_alternative<double>(entry.value)) {
            result += std::to_string(std::get<double>(entry.value));
        } else if (std::holds_alternative<bool>(entry.value)) {
            result += (std::get<bool>(entry.value) ? "true" : "false");
        } else if (std::holds_alternative<std::string>(entry.value)) {
            result += "\"" + std::get<std::string>(entry.value) + "\"";
        }
        
        result += " (" + std::to_string(static_cast<int>(entry.source)) + ")\n";
    }
    return result;
}

void Config::validateKeyValue(const std::string& key, const ConfigValue& value) {
    // Add validation logic for specific keys
    if (key == "neuron_count" && std::holds_alternative<int>(value)) {
        int count = std::get<int>(value);
        if (count <= 0) {
            throw ConfigValidationException("Neuron count must be positive");
        }
        if (count > 100000) {
            NLM_LOG_WARNING("Large neuron count may impact performance: " + std::to_string(count));
        }
    } else if (key == "simulation_timestep" && std::holds_alternative<double>(value)) {
        double timestep = std::get<double>(value);
        if (timestep <= 0.0) {
            throw ConfigValidationException("Simulation timestep must be positive");
        }
        if (timestep > 0.1) {
            NLM_LOG_WARNING("Large timestep may affect simulation accuracy: " + std::to_string(timestep));
        }
    } else if (key == "connection_probability" && std::holds_alternative<double>(value)) {
        double prob = std::get<double>(value);
        if (prob < 0.0 || prob > 1.0) {
            throw ConfigValidationException("Connection probability must be between 0 and 1");
        }
    }
}

std::string Config::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

} // namespace nlm