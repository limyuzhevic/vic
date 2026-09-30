#include "core/Config/Config.hpp"
#include "core/ErrorHandling/ErrorHandling.hpp"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cmath>

namespace nlm {

// Enhanced Config implementation with comprehensive error handling

Config::Config() : pImpl(std::make_unique<Impl>()), lastError("") {
    // Initialize with default error handling
    ErrorHandlingSystem::initialize();
}

Config::~Config() = default;

bool Config::validate() const {
    try {
        // Validate all entries
        for (const auto& entry : pImpl->entries) {
            if (entry.key.empty()) {
                ErrorManager::getInstance().report(
                    ErrorCode::InvalidConfigValue,
                    ErrorCategory::Configuration,
                    ErrorSeverity::Error,
                    "Empty key in configuration"
                );
                return false;
            }
            
            // Check type consistency
            if (!isValidConfigValueType(entry)) {
                ErrorManager::getInstance().report(
                    ErrorCode::InvalidConfigValue,
                    ErrorCategory::Configuration,
                    ErrorSeverity::Warning,
                    "Type mismatch for key: " + entry.key
                );
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        ErrorManager::getInstance().report(
            ErrorCode::ValidationFailed,
            ErrorCategory::Validation,
            ErrorSeverity::Error,
            std::string("Config validation exception: ") + e.what()
        );
        return false;
    }
}

bool Config::isValidConfigValueType(const ConfigEntry& entry) const {
    return std::visit([this](auto&& arg) -> bool {
        using T = std::decay_t<decltype(arg)>;
        
        if constexpr (std::is_same_v<T, int>) {
            return entry.type == "int" || entry.type == "integer";
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return entry.type == "int" || entry.type == "integer" || entry.type == "int64";
        } else if constexpr (std::is_same_v<T, double>) {
            return entry.type == "double" || entry.type == "float" || entry.type == "real";
        } else if constexpr (std::is_same_v<T, bool>) {
            return entry.type == "bool" || entry.type == "boolean";
        } else if constexpr (std::is_same_v<T, std::string>) {
            return entry.type == "string" || entry.type.empty();
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            return entry.type == "vector<int>" || entry.type == "int[]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return entry.type == "vector<double>" || entry.type == "double[]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return entry.type == "vector<string>" || entry.type == "string[]";
        } else {
            return false;
        }
    }, entry.value);
}

void Config::validateConfigValue(const ConfigEntry& entry) {
    if (entry.key == "neuron_count") {
        auto value = std::get<int64_t>(entry.value);
        if (value <= 0) {
            ErrorManager::getInstance().report(
                ErrorCode::InvalidConfigValue,
                ErrorCategory::Configuration,
                ErrorSeverity::Error,
                "neuron_count must be positive, got: " + std::to_string(value)
            );
        }
    } else if (entry.key == "connection_probability") {
        auto value = std::get<double>(entry.value);
        if (value < 0.0 || value > 1.0) {
            ErrorManager::getInstance().report(
                ErrorCode::InvalidConfigValue,
                ErrorCategory::Configuration,
                ErrorSeverity::Error,
                "connection_probability must be between 0.0 and 1.0, got: " + std::to_string(value)
            );
        }
    } else if (entry.key == "simulation_timestep") {
        auto value = std::get<double>(entry.value);
        if (value <= 0.0) {
            ErrorManager::getInstance().report(
                ErrorCode::InvalidConfigValue,
                ErrorCategory::Configuration,
                ErrorSeverity::Error,
                "simulation_timestep must be positive, got: " + std::to_string(value)
            );
        }
    }
}

std::string Config::generateErrorReport() const {
    std::ostringstream oss;
    oss << "Configuration Error Report (" << pImpl->entries.size() << " entries):";
    oss << std::endl;
    
    for (const auto& entry : pImpl->entries) {
        oss << "Key: " << entry.key << "; Value: [";
        oss << configValueToString(entry.value) << "]";
        oss << "; Source: " << static_cast<int>(entry.source);
        oss << "; Type: " << entry.type;
        oss << std::endl;
    }
    
    if (!lastError.empty()) {
        oss << "Last Error: " << lastError << std::endl;
    }
    
    return oss.str();
}

bool Config::hasCriticalErrors() const {
    return !lastError.empty();
}

std::string Config::getLastError() const {
    return lastError;
}

} // namespace nlm
