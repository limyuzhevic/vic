#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default configuration values
    // Core simulation parameters
    set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    set("connection_probability", 0.1f, ConfigSource::Default);
    set("simulation_timestep", 0.001, ConfigSource::Default);
    set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    
    // Plasticity parameters
    set("plasticity.stdp.enable", true, ConfigSource::Default);
    set("plasticity.stdp.learning_rate", 0.001f, ConfigSource::Default);
    set("plasticity.stdp.tau_plus", 20.0f, ConfigSource::Default);
    set("plasticity.stdp.tau_minus", 20.0f, ConfigSource::Default);
    set("plasticity.hebbian.enable", true, ConfigSource::Default);
    set("plasticity.hebbian.learning_rate", 0.0001f, ConfigSource::Default);
    set("plasticity.structural.enable", true, ConfigSource::Default);
    set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Neuromodulation parameters
    set("neuromod.dopamine.scale", 1.0f, ConfigSource::Default);
    set("neuromod.curiosity.enable", true, ConfigSource::Default);
    set("neuromod.novelty.enable", true, ConfigSource::Default);
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Load configuration from file (phase 1: simple key=value format)
    // TODO PHASE 2: Implement proper JSON/YAML parser for advanced configuration
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
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
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }

            // Parse string values (quoted strings remain unchanged)
            bool isStringValue = false;
            if ((value.front() == '"' && value.back() == '"') ||
                (value.front() == '\'' && value.back() == '\'')) {
                isStringValue = true;
            }

            // Parse numeric values
            if (!isStringValue) {
                if (value.find('.') != std::string::npos) {
                    double doubleValue = std::stod(value);
                    set(key, doubleValue, ConfigSource::File);
                } else {
                    try {
                        int intValue = std::stoi(value);
                        set(key, intValue, ConfigSource::File);
                    } catch (...) {
                        try {
                            int64_t int64Value = std::stoll(value);
                            set(key, int64Value, ConfigSource::File);
                        } catch (...) {
                            // If not a number, treat as string
                            set(key, value, ConfigSource::File);
                        }
                    }
                }
            } else {
                // Keep quoted strings as-is
                set(key, value, ConfigSource::File);
            }
        } else {
            // Warn about malformed lines but don't fail
            NLM_LOG_WARNING("Malformed config line " + std::to_string(lineNumber) + ": " + line);
        }
    }

    return true;
}

std::string Config::summary() const {
    // Generate human-readable configuration summary
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        
        // Handle different value types with proper formatting
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, int>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                oss << std::fixed << std::setprecision(6) << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << std::fixed << std::setprecision(6) << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << "\\" << arg[i] << "\\"";
                }
                oss << "]";
            } else {
                // Fallback for unknown types
                oss << "<unknown type>";
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

std::string Config::trim(const std::string& str) {
    // Remove leading and trailing whitespace from a string
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

} // namespace nlm
