#include "Config.hpp"
#include "EnhancedErrorHandlingIntegration.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    EnhancedLogger& logger = EnhancedErrorHandlingIntegration::get_enhanced_logger();
    logger.info("config", "Configuration object initialized", "Config::Config");
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    EnhancedLogger& logger = EnhancedErrorHandlingIntegration::get_enhanced_logger();
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        logger.error("config", "Failed to open configuration file: " + filepath, "Config::loadFromFile");
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
            
            // Validate key format
            if (key.empty()) {
                logger.error("config", "Invalid configuration key at line " + std::to_string(lineNumber), "Config::loadFromFile");
                continue;
            }
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Validate value before setting
            try {
                set(key, value, ConfigSource::File);
                logger.debug("config", "Loaded configuration: " + key + " = " + value, "Config::loadFromFile");
            } catch (const NLMConfigurationException& e) {
                logger.error("config", "Failed to set configuration key " + key + ": " + e.what(), "Config::loadFromFile");
                continue;
            }
        } else {
            logger.error("config", "Invalid configuration format at line " + std::to_string(lineNumber), "Config::loadFromFile");
        }
    }
    
    logger.info("config", std::string("Loaded configuration from file: ") + filepath + ", lines: " + std::to_string(lineNumber), "Config::loadFromFile");
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    EnhancedLogger& logger = EnhancedErrorHandlingIntegration::get_enhanced_logger();
    
    if (argc <= 1) {
        logger.info("config", "No command line arguments provided", "Config::loadFromArgs");
        return true;
    }
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Validate key format
                if (key.empty()) {
                    logger.error("config", "Invalid command line key: " + arg, "Config::loadFromArgs");
                    continue;
                }
                
                // Validate value before setting
                try {
                    set(key, value, ConfigSource::CommandLine);
                    logger.debug("config", "Loaded configuration from args: " + key + " = " + value, "Config::loadFromArgs");
                } catch (const NLMConfigurationException& e) {
                    logger.error("config", "Failed to set command line key " + key + ": " + e.what(), "Config::loadFromArgs");
                    continue;
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            // Validate key format
            if (key.empty()) {
                logger.error("config", "Invalid command line key: " + arg, "Config::loadFromArgs");
                continue;
            }
            
            // Validate value before setting
            try {
                set(key, value, ConfigSource::CommandLine);
                logger.debug("config", "Loaded configuration from args: " + key + " = " + value, "Config::loadFromArgs");
            } catch (const NLMConfigurationException& e) {
                logger.error("config", "Failed to set command line key " + key + ": " + e.what(), "Config::loadFromArgs");
                continue;
            }
        } else {
            logger.error("config", "Invalid command line argument format: " + arg, "Config::loadFromArgs");
        }
    }
    
    logger.info("config", "Loaded configuration from command line arguments", "Config::loadFromArgs");
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    EnhancedLogger& logger = EnhancedErrorHandlingIntegration::get_enhanced_logger();
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        logger.error("config", "Failed to create configuration file: " + filepath, "Config::saveToFile");
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
    }
    
    logger.info("config", std::string("Saved configuration to file: ") + filepath, "Config::saveToFile");
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    EnhancedLogger& logger = EnhancedErrorHandlingIntegration::get_enhanced_logger();
    
    // Validate key format
    if (key.empty()) {
        logger.error("config", "Invalid configuration key: empty", "Config::get");
        return std::nullopt;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        logger.debug("config", "Configuration key not found: " + key, "Config::get");
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        logger.error("config", "Type mismatch for configuration key: " + key, "Config::get");
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    EnhancedLogger& logger = EnhancedErrorHandlingIntegration::get_enhanced_logger();
    
    try {
        auto val = get<T>(key);
        if (val.has_value()) {
            logger.debug("config", "Retrieved configuration: " + key + " = " + std::to_string(val.value()), "Config::getOr");
            return val.value();
        }
    } catch (...) {
        logger.warning("config", "Failed to retrieve configuration key: " + key, "Config::getOr");
    }
    
    logger.debug("config", "Using default value for configuration key: " + key, "Config::getOr");
    return defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    EnhancedLogger& logger = EnhancedErrorHandlingIntegration::get_enhanced_logger();
    
    // Validate key format
    if (key.empty()) {
        throw NLMConfigurationException("Configuration key cannot be empty", "Config::set");
    }
    
    // Validate value format
    if (value.index() >= static_cast<size_t>(ConfigValue::variant_index_end)) {
        throw NLMConfigurationException("Invalid configuration value type", "Config::set");
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        // Key exists, validate type compatibility
        if (it->value.index() != value.index()) {
            logger.error("config", std::string("Type mismatch for configuration key: ") + key,
                        "Config::set");
            throw NLMConfigurationException("Type mismatch for configuration key", "Config::set");
        }
        
        it->value = value;
        it->source = source;
        
        logger.debug("config", "Updated configuration: " + key, "Config::set");
    } else {
        pImpl->entries.emplace_back(key, value, source);
        
        logger.debug("config", "Added configuration: " + key, "Config::set");
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
    EnhancedLogger& logger = EnhancedErrorHandlingIntegration::get_enhanced_logger();
    
    bool exists = std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (!exists) {
        logger.debug("config", "Configuration key not found: " + key, "Config::has");
    }
    
    return exists;
}

void Config::remove(const std::string& key) {
    EnhancedLogger& logger = EnhancedErrorHandlingIntegration::get_enhanced_logger();
    
    size_t removedCount = pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
    
    if (removedCount > 0) {
        logger.info("config", std::string("Removed configuration key: ") + key + ", entries removed: " + std::to_string(removedCount),
                   "Config::remove");
    } else {
        logger.warning("config", std::string("Configuration key not found for removal: ") + key, "Config::remove");
    }
}

std::vector<std::string> Config::getKeys() const {
    EnhancedLogger& logger = EnhancedErrorHandlingIntegration::get_enhanced_logger();
    
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    
    logger.debug("config", std::string("Retrieved configuration keys: ") + std::to_string(keys.size()), "Config::getKeys");
    return keys;
}

void Config::clear() {
    EnhancedLogger& logger = EnhancedErrorHandlingIntegration::get_enhanced_logger();
    
    size_t removedCount = pImpl->entries.size();
    pImpl->entries.clear();
    
    logger.info("config", std::string("Cleared configuration: ") + std::to_string(removedCount) + " entries removed",
               "Config::clear");
}

std::string Config::summary() const {
    EnhancedLogger& logger = EnhancedErrorHandlingIntegration::get_enhanced_logger();
    
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
    
    std::string result = oss.str();
    logger.debug("config", "Generated configuration summary", "Config::summary");
    return result;
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
