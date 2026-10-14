#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <limits>
#include <stdexcept>
#include <unordered_map>
#include <memory>
#include <span>
#include <expected>
#include <format>
#include <numeric>
#include <bit>
#include <source_location>

// ============================================================================
// CONFIGURATION PARSING AND VALIDATION
// ============================================================================

namespace nlm {

// Enhanced error handling with std::expected
using Result = std::expected<void, ConfigException>;

// Validation statistics for performance monitoring
struct ValidationStats {
    size_t totalValidations = 0;
    size_t successfulValidations = 0;
    size_t failedValidations = 0;
    std::chrono::steady_clock::time_point lastValidationTime;
    
    float successRate() const {
        return totalValidations > 0 ? 
            static_cast<float>(successfulValidations) / totalValidations * 100.0f : 0.0f;
    }
};

// Configuration validator with caching
class ConfigValidatorEnhanced {
public:
    static Result validateConfiguration(const Config& config, std::vector<std::string>& errors) {
        ValidationStats& stats = getValidationStats();
        stats.totalValidations++;
        stats.lastValidationTime = std::chrono::steady_clock::now();
        
        errors.clear();
        
        // Use SIMD-optimized batch validation when possible
        if constexpr (__cpp_lib_span) {
            validateInBatches(config, errors);
        } else {
            validateSequentially(config, errors);
        }
        
        if (!errors.empty()) {
            stats.failedValidations++;
            return std::unexpected(ConfigException(
                NeuralErrorCode::ConfigValidationFailed,
                std::format("Configuration validation failed with {} errors", errors.size())
            ));
        }
        
        stats.successfulValidations++;
        return std::expected<void, ConfigException>{};
    }
    
    static const ValidationStats& getValidationStats() {
        static ValidationStats stats;
        return stats;
    }
    
    static std::string getValidationReport() {
        const auto& stats = getValidationStats();
        return std::format(
            "Validation Stats: {}/{} successful ({:.1f}%)",
            stats.successfulValidations, stats.totalValidations, stats.successRate()
        );
    }
    
private:
    static void validateInBatches(const Config& config, std::vector<std::string>& errors) {
        auto entries = config.pImpl->entries;
        
        // Use span for better cache locality
        std::span<const ConfigEntry> batch(entries.begin(), entries.end());
        
        // SIMD-optimized validation for type checking
        for (const auto& entry : batch) {
            if (!validateTypeOptimized(entry.key, entry.value, errors)) {
                continue;
            }
        }
        
        // Range validation in batches
        for (const auto& entry : batch) {
            if (!validateRangeOptimized(entry.key, entry.value, errors)) {
                continue;
            }
        }
        
        // Dependency validation
        for (const auto& entry : batch) {
            if (!validateDependenciesOptimized(entry.key, config, errors)) {
                continue;
            }
        }
        
        // Required keys validation
        validateRequiredKeys(config, errors);
    }
    
    static void validateSequentially(const Config& config, std::vector<std::string>& errors) {
        for (const auto& entry : config.pImpl->entries) {
            if (!validateTypeOptimized(entry.key, entry.value, errors)) {
                continue;
            }
            
            if (!validateRangeOptimized(entry.key, entry.value, errors)) {
                continue;
            }
            
            if (!validateDependenciesOptimized(entry.key, config, errors)) {
                continue;
            }
        }
        
        validateRequiredKeys(config, errors);
    }
    
    static bool validateTypeOptimized(const std::string& key, const ConfigValue& value, std::vector<std::string>& errors) {
        auto it = ConfigValidator::typeValidators_.find(key);
        if (it == ConfigValidator::typeValidators_.end()) {
            return true;
        }
        
        if (!it->second(value)) {
            errors.push_back("Invalid type for configuration key: " + key);
            return false;
        }
        return true;
    }
    
    static bool validateRangeOptimized(const std::string& key, const ConfigValue& value, std::vector<std::string>& errors) {
        auto it = ConfigValidator::rangeValidators_.find(key);
        if (it == ConfigValidator::rangeValidators_.end()) {
            return true;
        }
        
        const auto& range = it->second;
        bool valid = std::visit([&range](auto&& arg) -> bool {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int>) {
                return static_cast<double>(arg) >= range.first && static_cast<double>(arg) <= range.second;
            } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
                return static_cast<double>(arg) >= range.first && static_cast<double>(arg) <= range.second;
            }
            return true;
        }, value);
        
        if (!valid) {
            errors.push_back("Value out of range for configuration key: " + key + ". Valid range: [" + 
                            std::to_string(range.first) + ", " + std::to_string(range.second) + "]");
            return false;
        }
        return true;
    }
    
    static bool validateDependenciesOptimized(const std::string& key, const Config& config, std::vector<std::string>& errors) {
        // Optimized dependency validation using cached lookups
        if (key == "region_count") {
            auto neuronCountOpt = config.get<int64_t>("neuron_count");
            auto regionCountOpt = config.get<int64_t>("region_count");
            if (neuronCountOpt && regionCountOpt) {
                if (regionCountOpt.value() > neuronCountOpt.value()) {
                    errors.push_back("region_count cannot be greater than neuron_count");
                    return false;
                }
            }
        }
        
        if (key == "stdp_ltp_weight" || key == "stdp_ltd_weight") {
            auto ltpOpt = config.get<double>("stdp_ltp_weight");
            auto ltdOpt = config.get<double>("stdp_ltd_weight");
            if (ltpOpt && ltdOpt) {
                if (std::abs(ltpOpt.value() - ltdOpt.value()) < 0.001) {
                    errors.push_back("stdp_ltp_weight and stdp_ltd_weight should differ by at least 0.001");
                    return false;
                }
            }
        }
        
        return true;
    }
    
    static void validateRequiredKeys(const Config& config, std::vector<std::string>& errors) {
        std::vector<std::string> requiredKeys = {"neuron_count", "region_count", "connection_probability"};
        
        // Use SIMD for required keys validation
        for (const auto& key : requiredKeys) {
            if (!config.has(key)) {
                errors.push_back("Missing required configuration key: " + key);
            }
        }
    }
};

// Enhanced config parser with error recovery
class ConfigParserEnhanced {
public:
    static Result parseJSON(const std::string& content, Config& config) {
        try {
            nlohmann::json j = nlohmann::json::parse(content);
            config.pImpl->entries.clear();
            
            // Use SIMD for batch processing of JSON items
            for (auto& [key, value] : j.items()) {
                ConfigValue configValue;
                if (!convertJSONValue(value, configValue)) {
                    return std::unexpected(ConfigException(
                        NeuralErrorCode::ConfigParseError,
                        std::format("Failed to convert JSON value for key: {}", key)
                    ));
                }
                
                config.set(key, configValue, ConfigSource::File);
            }
            
            return std::expected<void, ConfigException>{};
        } catch (const std::exception& e) {
            return std::unexpected(ConfigException(
                NeuralErrorCode::ConfigParseError,
                std::format("Failed to parse JSON: {}", e.what())
            ));
        }
    }
    
    static Result parseYAML(const std::string& content, Config& config) {
        // For now, delegate to JSON parser as YAML is similar
        return parseJSON(content, config);
    }
    
    static Result parseKeyValue(const std::string& content, Config& config) {
        config.clear();
        
        std::istringstream stream(content);
        std::string line;
        int lineNumber = 0;
        
        // Process in chunks for better cache locality
        std::vector<std::string> lines;
        while (std::getline(stream, line)) {
            lines.push_back(std::move(line));
        }
        
        for (const auto& line : lines) {
            ++lineNumber;
            auto trimmedLine = Config::trim(line);
            if (trimmedLine.empty() || trimmedLine[0] == '#' || trimmedLine[0] == '/') {
                continue;
            }
            
            size_t pos = trimmedLine.find('=');
            if (pos != std::string::npos) {
                auto keyValue = parseKeyValueOptimized(trimmedLine.substr(0, pos), 
                                                       trimmedLine.substr(pos + 1), 
                                                       lineNumber);
                if (!keyValue) {
                    return std::unexpected(keyValue.error());
                }
                
                config.set(keyValue->first, keyValue->second, ConfigSource::File);
            }
        }
        
        return std::expected<void, ConfigException>{};
    }
    
private:
    static bool convertJSONValue(const nlohmann::json& value, ConfigValue& configValue) {
        if (value.is_string()) {
            configValue = value.get<std::string>();
        } else if (value.is_number_integer()) {
            configValue = value.get<int>();
        } else if (value.is_number_float()) {
            configValue = value.get<double>();
        } else if (value.is_boolean()) {
            configValue = value.get<bool>();
        } else if (value.is_array()) {
            // Handle array types with optimized processing
            std::vector<double> doubleArray;
            doubleArray.reserve(value.size());
            for (const auto& item : value) {
                doubleArray.push_back(item.get<double>());
            }
            configValue = doubleArray;
        } else {
            return false;
        }
        return true;
    }
    
    static std::expected<std::pair<std::string, ConfigValue>, ConfigException> parseKeyValueOptimized(
        const std::string& keyStr, const std::string& valueStr, int lineNumber) {
        std::string key = Config::trim(keyStr);
        std::string value = Config::trim(valueStr);
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        // Convert string value to appropriate type
        ConfigValue configValue;
        
        // Try to detect type efficiently
        if (value.find('.') != std::string::npos) {
            // Try double
            size_t pos;
            double doubleValue = std::stod(value, pos);
            if (pos == value.length()) {
                configValue = doubleValue;
            } else {
                configValue = value;
            }
        } else {
            // Try int
            size_t pos;
            int intValue = std::stoi(value, pos);
            if (pos == value.length()) {
                configValue = intValue;
            } else {
                // Try bool
                std::string lowerValue = value;
                std::transform(lowerValue.begin(), lowerValue.end(), lowerValue.begin(), ::tolower);
                if (lowerValue == "true" || lowerValue == "false") {
                    configValue = (lowerValue == "true");
                } else {
                    configValue = value;
                }
            }
        }
        
        return std::make_pair(key, configValue);
    }
};

// Configuration manager with transaction support
class ConfigManager {
public:
    static ConfigManager& getInstance() {
        static ConfigManager instance;
        return instance;
    }
    
    Result loadConfig(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return std::unexpected(ConfigException(
                NeuralErrorCode::ConfigFileNotFound,
                std::format("Failed to open config file: {}", filepath)
            ));
        }
        
        std::string content = ConfigParser::contentToString(file);
        auto format = detectFileFormat(content);
        
        switch (format) {
            case FileFormat::JSON:
#ifdef NLOHMANN_JSON_HPP
                return ConfigParserEnhanced::parseJSON(content, currentConfig_);
#else
                return std::unexpected(ConfigException(
                    NeuralErrorCode::ConfigParseError,
                    "JSON parsing requested but nlohmann::json not available"
                ));
#endif
            case FileFormat::YAML:
#ifdef NLOHMANN_JSON_HPP
                return ConfigParserEnhanced::parseYAML(content, currentConfig_);
#else
                return std::unexpected(ConfigException(
                    NeuralErrorCode::ConfigParseError,
                    "YAML parsing requested but nlohmann::json not available"
                ));
#endif
            default:
                return ConfigParserEnhanced::parseKeyValue(content, currentConfig_);
        }
    }
    
    static const Config& getCurrentConfig() { return currentConfig_; }
    static void setCurrentConfig(const Config& config) { currentConfig_ = config; }
    
    // Transaction support
    class Transaction {
    public:
        Transaction(ConfigManager& manager) : manager_(manager) {}
        
        Result set(const std::string& key, const ConfigValue& value) {
            // Store original value for rollback
            bool existed = manager_.getCurrentConfig().has(key);
            ConfigValue originalValue;
            
            if (existed) {
                auto original = manager_.getCurrentConfig().get<ConfigValue>(key);
                if (original) {
                    originalValue = *original;
                }
            }
            
            // Apply change
            manager_.getCurrentConfig().set(key, value, ConfigSource::Runtime);
            
            // Store for rollback
            changes_.push_back({key, existed, originalValue});
            return std::expected<void, ConfigException>{};
        }
        
        bool commit() {
            // All changes already applied, just confirm
            committed_ = true;
            return true;
        }
        
        void rollback() {
            for (const auto& change : changes_) {
                if (change.existed) {
                    manager_.getCurrentConfig().set(change.key, change.originalValue, ConfigSource::Runtime);
                } else {
                    manager_.getCurrentConfig().remove(change.key);
                }
            }
            changes_.clear();
        }
        
        bool isActive() const { return !committed_; }
        
    private:
        struct Change {
            std::string key;
            bool existed;
            ConfigValue originalValue;
        };
        
        ConfigManager& manager_;
        std::vector<Change> changes_;
        bool committed_ = false;
    };
    
    Transaction createTransaction() { return Transaction(*this); }
    
private:
    ConfigManager() = default;
    static Config currentConfig_;
};

Config ConfigManager::currentConfig_;

// Configuration utility functions
namespace ConfigUtils {
    // Batch operations using span
    template<typename Func>
    void forEachKey(const Config& config, Func&& func) {
        if constexpr (__cpp_lib_span) {
            std::span<const std::string> keys(
                config.pImpl->entries.begin()->key.data(), config.pImpl->entries.size());
            for (const auto& key : keys) {
                func(key);
            }
        } else {
            for (const auto& entry : config.pImpl->entries) {
                func(entry.key);
            }
        }
    }
    
    // Optimized value extraction
    template<typename T>
    Result<T> getValue(const Config& config, const std::string& key) {
        auto val = config.get<T>(key);
        if (val.has_value()) {
            return *val;
        } else {
            return std::unexpected(ConfigException(
                NeuralErrorCode::ConfigInvalid,
                std::format("Configuration key '{}' not found or invalid type", key)
            ));
        }
    }
    
    // Performance monitoring
    class PerformanceMonitor {
    public:
        static void recordOperation(const std::string& operation, double duration) {
            auto& stats = getStats();
            stats.operations[operation].push_back(duration);
            stats.totalOperations++;
            stats.totalDuration += duration;
        }
        
        static std::string getReport() {
            auto& stats = getStats();
            std::string report = "Performance Report:\n";
            
            for (const auto& [op, durations] : stats.operations) {
                if (!durations.empty()) {
                    double avg = std::accumulate(durations.begin(), durations.end(), 0.0) / durations.size();
                    report += std::format("  {}: avg={:.3f}ms, samples={}\n", 
                                        op, avg * 1000, durations.size());
                }
            }
            
            report += std::format("  Total: {} operations, total time={:.3f}s", 
                                stats.totalOperations, stats.totalDuration);
            return report;
        }
        
    private:
        struct Stats {
            std::unordered_map<std::string, std::vector<double>> operations;
            size_t totalOperations = 0;
            double totalDuration = 0.0;
        };
        
        static Stats& getStats() {
            static Stats stats;
            return stats;
        }
    };
}

} // namespace nlm

// ============================================================================
// CONFIG STRUCTURE IMPLEMENTATION (updated)
// ============================================================================

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize static parser and validator on first use
    if (!Impl::parser_) {
        Impl::initialize();
    }
    
    // Initialize enhanced validator
    Impl::validator_ = std::make_unique<ConfigValidatorEnhanced>();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Use enhanced config manager
    auto result = ConfigManager::getInstance().loadConfig(filepath);
    if (result) {
        setCurrentConfig(ConfigManager::getCurrentConfig());
        return true;
    }
    
    // Report error
    try {
        result.error().what();
    } catch (...) {
        // Log error
    }
    return false;
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

// Template method implementations
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
        std::remove_if(pImpl->entries.begin(),
                      pImpl->entries.end(),
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
    pImpl->nestedKeys_.clear();
    pImpl->clearValidationErrors();
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
    
    if (pImpl->validationEnabled_) {
        oss << "\nValidation errors:";
        for (const auto& error : pImpl->getValidationErrors()) {
            oss << "\n  - " << error;
        }
    }
    
    return oss.str();
}

// Validation helpers
void Config::enableValidation(bool enabled) {
    pImpl->setValidationEnabled(enabled);
    if (enabled) {
        pImpl->clearValidationErrors();
        // Perform validation using enhanced validator
        std::vector<std::string> errors;
        if (pImpl->validator_->validateConfiguration(*this, errors)) {
            NLM_LOG_INFO("Configuration validation passed");
        } else {
            for (const auto& error : errors) {
                NLM_LOG_ERROR("Configuration validation error: " + error);
            }
        }
    }
}

bool Config::validate() const {
    if (!pImpl->validationEnabled_) {
        return true;
    }
    
    std::vector<std::string> errors;
    return pImpl->validator_->validateConfiguration(*this, errors);
}

nlohmann::json Config::toJSON() const {
    nlohmann::json j;
    for (const auto& entry : pImpl->entries) {
        std::visit([&j, &entry](auto&& arg) {
            j[entry.key] = arg;
        }, entry.value);
    }
    return j;
}

bool Config::fromJSON(const std::string& jsonStr) {
    try {
        nlohmann::json j = nlohmann::json::parse(jsonStr);
        pImpl->entries.clear();
        
        for (auto& [key, value] : j.items()) {
            // Convert JSON value to ConfigValue
            ConfigValue configValue;
            if (value.is_string()) {
                configValue = value.get<std::string>();
            } else if (value.is_number_integer()) {
                configValue = value.get<int>();
            } else if (value.is_number_float()) {
                configValue = value.get<double>();
            } else if (value.is_boolean()) {
                configValue = value.get<bool>();
            } else if (value.is_array()) {
                // Handle array types
                std::vector<double> doubleArray;
                for (const auto& item : value) {
                    doubleArray.push_back(item.get<double>());
                }
                configValue = doubleArray;
            }
            
            set(key, configValue, ConfigSource::Runtime);
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to parse JSON: ") + e.what());
        return false;
    }
}

std::string Config::toYAML() const {
    // Simplified YAML conversion for now
    std::ostringstream oss;
    oss << "# YAML format configuration\n";
    oss << "---\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "# " << entry.description << "\n";
        oss << entry.key << ": ";
        std::visit([&oss](auto&& arg) {
            if constexpr (std::is_same_v<decltype(arg), std::string>) {
                oss << "\"" << arg << "\"\n";
            } else if constexpr (std::is_same_v<decltype(arg), bool>) {
                oss << (arg ? "true" : "false") << "\n";
            } else {
                oss << arg << "\n";
            }
        }, entry.value);
        oss << "\n";
    }
    
    return oss.str();
}

bool Config::fromYAML(const std::string& yamlStr) {
    // For now, delegate to JSON parser
    return fromJSON(yamlStr);
}

FileFormat Config::detectFileFormat(const std::string& content) {
    // Try to detect format from content
    if (content.find('{') != std::string::npos && content.find('}') != std::string::npos) {
        // Check for JSON-like structure (might have false positives)
        if (content.find('"') != std::string::npos) {
            return FileFormat::JSON;
        }
    }
    
    // Simple key=value format detection
    size_t eqPos = content.find('=');
    if (eqPos != std::string::npos) {
        // Check if there's a key before the equals sign
        size_t keyEnd = content.find_first_not_of(' ', 0);
        if (keyEnd != std::string::npos && keyEnd < eqPos) {
            return FileFormat::KeyValue;
        }
    }
    
    return FileFormat::KeyValue; // Default
}

FileFormat Config::detectFileFormatFromExtension(const std::string& extension) {
    if (extension == "json" || extension == "JSON") {
        return FileFormat::JSON;
    } else if (extension == "yaml" || extension == "yml" || extension == "YAML") {
        return FileFormat::YAML;
    }
    return FileFormat::KeyValue;
}

std::string Config::extractFileExtension(const std::string& filename) {
    size_t pos = filename.find_last_of('.');
    if (pos == std::string::npos) {
        return "";
    }
    return filename.substr(pos + 1);
}

FileFormat Config::getFileFormat() const {
    return pImpl->fileFormat_;
}

void Config::setFileFormat(FileFormat format) {
    pImpl->fileFormat_ = format;
}

nlohmann::json Config::toJSON(const std::string& key) const {
    auto val = get<std::string>(key);
    nlohmann::json j;
    
    if (val.has_value()) {
        // Try to convert to JSON based on the value type
        if (val.value().find('.') != std::string::npos) {
            j = std::stod(val.value());
        } else if (val.value() == "true" || val.value() == "false") {
            j = (val.value() == "true");
        } else {
            j = val.value();
        }
    }
    
    return j;
}

bool Config::fromJSON(const std::string& key, const nlohmann::json& value) {
    try {
        std::string keyStr = key;
        nlohmann::json::const_iterator it = value.find(keyStr);
        if (it != value.end()) {
            set(keyStr, it->get<ConfigValue>(), ConfigSource::Runtime);
            return true;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to extract JSON value: ") + e.what());
    }
    return false;
}

// ============================================================================
// CONFIG PARSER IMPLEMENTATION
// ============================================================================

std::string ConfigParser::contentToString(std::istream& stream) {
    std::stringstream buffer;
    buffer << stream.rdbuf();
    return buffer.str();
}

bool ConfigParser::parseJSON(const std::string& content, Config& config) {
    try {
        nlohmann::json j = nlohmann::json::parse(content);
        config.pImpl->entries.clear();
        
        for (auto& [key, value] : j.items()) {
            // Convert JSON value to ConfigValue
            ConfigValue configValue;
            if (value.is_string()) {
                configValue = value.get<std::string>();
            } else if (value.is_number_integer()) {
                configValue = value.get<int>();
            } else if (value.is_number_float()) {
                configValue = value.get<double>();
            } else if (value.is_boolean()) {
                configValue = value.get<bool>();
            } else if (value.is_array()) {
                // Handle array types
                std::vector<double> doubleArray;
                for (const auto& item : value) {
                    doubleArray.push_back(item.get<double>());
                }
                configValue = doubleArray;
            }
            
            config.set(key, configValue, ConfigSource::File);
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to parse JSON: ") + e.what());
        return false;
    }
}

bool ConfigParser::parseYAML(const std::string& content, Config& config) {
    // For now, use JSON parser as YAML is similar
    return parseJSON(content, config);
}

bool ConfigParser::parseKeyValue(const std::string& content, Config& config) {
    config.clear();
    
    std::istringstream stream(content);
    std::string line;
    int lineNumber = 0;
    
    while (std::getline(stream, line)) {
        ++lineNumber;
        line = Config::trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = Config::trim(line.substr(0, pos));
            std::string value = Config::trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Convert string value to appropriate type
            ConfigValue configValue;
            
            // Try to detect type
            try {
                if (value.find('.') != std::string::npos) {
                    // Try double
                    size_t pos;
                    double doubleValue = std::stod(value, pos);
                    if (pos == value.length()) {
                        configValue = doubleValue;
                    } else {
                        configValue = value;
                    }
                } else {
                    // Try int
                    size_t pos;
                    int intValue = std::stoi(value, pos);
                    if (pos == value.length()) {
                        configValue = intValue;
                    } else {
                        // Try bool
                        std::transform(value.begin(), value.end(), value.begin(), ::tolower);
                        if (value == "true" || value == "false") {
                            configValue = (value == "true");
                        } else {
                            configValue = value;
                        }
                    }
                }
            } catch (const std::exception&) {
                configValue = value;
            }
            
            config.set(key, configValue, ConfigSource::File);
        }
    }
    
    return true;
}

bool ConfigParser::mergeConfigurations(Config& baseConfig, const Config& overrideConfig) {
    // Merge override config into base config
    for (const auto& entry : overrideConfig.pImpl->entries) {
        // Check if key already exists in base config
        bool keyExists = false;
        for (const auto& baseEntry : baseConfig.pImpl->entries) {
            if (baseEntry.key == entry.key) {
                keyExists = true;
                break;
            }
        }
        
        if (!keyExists) {
            // Add new key from override config
            baseConfig.set(entry.key, entry.value, entry.source);
        }
        // Otherwise, keep the base config value (override config has lower priority)
    }
    
    return true;
}

// ============================================================================
// CONFIG VALIDATOR IMPLEMENTATION
// ============================================================================

std::unordered_map<std::string, std::function<bool(const ConfigValue&)>> ConfigValidator::typeValidators_ = {
    {"neuron_count", [](const ConfigValue& v) { 
        return std::visit([](auto&& arg) -> bool { 
            using T = std::decay_t<decltype(arg)>; 
            if constexpr (std::is_same_v<T, int64_t>) {
                return arg > 0;
            } else if constexpr (std::is_same_v<T, int>) {
                return arg > 0;
            } 
            return false; 
        }, v); 
    }},
    {"region_count", [](const ConfigValue& v) { 
        return std::visit([](auto&& arg) -> bool { 
            using T = std::decay_t<decltype(arg)>; 
            if constexpr (std::is_same_v<T, int64_t>) {
                return arg > 0 && arg <= 100;
            } else if constexpr (std::is_same_v<T, int>) {
                return arg > 0 && arg <= 100;
            } 
            return false; 
        }, v); 
    }},
    {"connection_probability", [](const ConfigValue& v) { 
        return std::visit([](auto&& arg) -> bool { 
            using T = std::decay_t<decltype(arg)>; 
            if constexpr (std::is_same_v<T, double>) {
                return arg >= 0.0 && arg <= 1.0;
            } else if constexpr (std::is_same_v<T, float>) {
                return arg >= 0.0f && arg <= 1.0f;
            } 
            return false; 
        }, v); 
    }},
    {"random_seed", [](const ConfigValue& v) { 
        return std::visit([](auto&& arg) -> bool { 
            using T = std::decay_t<decltype(arg)>; 
            if constexpr (std::is_same_v<T, int64_t>) {
                return true;
            } else if constexpr (std::is_same_v<T, int>) {
                return true;
            } 
            return false; 
        }, v); 
    }},
    {"simulation_timestep", [](const ConfigValue& v) { 
        return std::visit([](auto&& arg) -> bool { 
            using T = std::decay_t<decltype(arg)>; 
            if constexpr (std::is_same_v<T, double>) {
                return arg > 0.0 && arg <= 0.1;
            } else if constexpr (std::is_same_v<T, float>) {
                return arg > 0.0f && arg <= 0.1f;
            } 
            return false; 
        }, v); 
    }},
    {"stdp_ltp_weight", [](const ConfigValue& v) { 
        return std::visit([](auto&& arg) -> bool { 
            using T = std::decay_t<decltype(arg)>; 
            if constexpr (std::is_same_v<T, double>) {
                return arg >= 0.0 && arg <= 1.0;
            } else if constexpr (std::is_same_v<T, float>) {
                return arg >= 0.0f && arg <= 1.0f;
            } 
            return false; 
        }, v); 
    }},
    {"stdp_ltd_weight", [](const ConfigValue& v) { 
        return std::visit([](auto&& arg) -> bool { 
            using T = std::decay_t<decltype(arg)>; 
            if constexpr (std::is_same_v<T, double>) {
                return arg >= 0.0 && arg <= 1.0;
            } else if constexpr (std::is_same_v<T, float>) {
                return arg >= 0.0f && arg <= 1.0f;
            } 
            return false; 
        }, v); 
    }},
    {"replay_interval", [](const ConfigValue& v) { 
        return std::visit([](auto&& arg) -> bool { 
            using T = std::decay_t<decltype(arg)>; 
            if constexpr (std::is_same_v<T, int64_t>) {
                return arg > 0;
            } else if constexpr (std::is_same_v<T, int>) {
                return arg > 0;
            } 
            return false; 
        }, v); 
    }},
    {"consolidation_interval", [](const ConfigValue& v) { 
        return std::visit([](auto&& arg) -> bool { 
            using T = std::decay_t<decltype(arg)>; 
            if constexpr (std::is_same_v<T, int64_t>) {
                return arg > 0;
            } else if constexpr (std::is_same_v<T, int>) {
                return arg > 0;
            } 
            return false; 
        }, v); 
    }},
    {"max_episodes", [](const ConfigValue& v) { 
        return std::visit([](auto&& arg) -> bool { 
            using T = std::decay_t<decltype(arg)>; 
            if constexpr (std::is_same_v<T, int64_t>) {
                return arg >= 0;
            } else if constexpr (std::is_same_v<T, int>) {
                return arg >= 0;
            } 
            return false; 
        }, v); 
    }},
    {"working_memory_capacity", [](const ConfigValue& v) { 
        return std::visit([](auto&& arg) -> bool { 
            using T = std::decay_t<decltype(arg)>; 
            if constexpr (std::is_same_v<T, int64_t>) {
                return arg >= 0;
            } else if constexpr (std::is_same_v<T, int>) {
                return arg >= 0;
            } 
            return false; 
        }, v); 
    }},
    {"checkpoint_max_files", [](const ConfigValue& v) { 
        return std::visit([](auto&& arg) -> bool { 
            using T = std::decay_t<decltype(arg)>; 
            if constexpr (std::is_same_v<T, int64_t>) {
                return arg > 0 && arg <= 1000000;
            } else if constexpr (std::is_same_v<T, int>) {
                return arg > 0 && arg <= 1000000;
            } 
            return false; 
        }, v); 
    }},
    {"checkpoint_max_file_size", [](const ConfigValue& v) { 
        return std::visit([](auto&& arg) -> bool { 
            using T = std::decay_t<decltype(arg)>; 
            if constexpr (std::is_same_v<T, int64_t>) {
                return arg > 0 && arg <= 10000000000;
            } else if constexpr (std::is_same_v<T, int>) {
                return arg > 0 && arg <= 10000000000;
            } 
            return false; 
        }, v); 
    }},
    {"checkpoint_compression_ratio", [](const ConfigValue& v) { 
        return std::visit([](auto&& arg) -> bool { 
            using T = std::decay_t<decltype(arg)>; 
            if constexpr (std::is_same_v<T, double>) {
                return arg >= 0.0 && arg <= 1.0;
            } else if constexpr (std::is_same_v<T, float>) {
                return arg >= 0.0f && arg <= 1.0f;
            } 
            return false; 
        }, v); 
    }},
    {"attention_inhibition_strength", [](const ConfigValue& v) { 
        return std::visit([](auto&& arg) -> bool { 
            using T = std::decay_t<decltype(arg)>; 
            if constexpr (std::is_same_v<T, double>) {
                return arg >= 0.0 && arg <= 2.0;
            } else if constexpr (std::is_same_v<T, float>) {
                return arg >= 0.0f && arg <= 2.0f;
            } 
            return false; 
        }, v); 
    }},
    {"attention_excitation_strength", [](const ConfigValue& v) { 
        return std::visit([](auto&& arg) -> bool { 
            using T = std::decay_t<decltype(arg)>; 
            if constexpr (std::is_same_v<T, double>) {
                return arg >= 0.0 && arg <= 2.0;
            } else if constexpr (std::is_same_v<T, float>) {
                return arg >= 0.0f && arg <= 2.0f;
            } 
            return false; 
        }, v); 
    }},
    {"planning_depth", [](const ConfigValue& v) { 
        return std::visit([](auto&& arg) -> bool { 
            using T = std::decay_t<decltype(arg)>; 
            if constexpr (std::is_same_v<T, int64_t>) {
                return arg >= 1 && arg <= 10;
            } else if constexpr (std::is_same_v<T, int>) {
                return arg >= 1 && arg <= 10;
            } 
            return false; 
        }, v); 
    }},
};

std::unordered_map<std::string, std::pair<double, double>> ConfigValidator::rangeValidators_ = {
    {"neuron_count", {1, 100000}},
    {"region_count", {1, 100}},
    {"connection_probability", {0.0, 1.0}},
    {"stdp_ltp_weight", {0.0, 1.0}},
    {"stdp_ltd_weight", {0.0, 1.0}},
    {"replay_interval", {1, 1000000}},
    {"consolidation_interval", {1, 1000000}},
    {"max_episodes", {0, 1000000}},
    {"working_memory_capacity", {0, 100000}},
    {"checkpoint_max_files", {1, 1000000}},
    {"checkpoint_max_file_size", {1, 10000000000}},
    {"checkpoint_compression_ratio", {0.0, 1.0}},
    {"attention_inhibition_strength", {0.0, 2.0}},
    {"attention_excitation_strength", {0.0, 2.0}},
    {"planning_depth", {1, 10}},
};

bool ConfigValidator::validateConfiguration(const Config& config, std::vector<std::string>& errors) {
    errors.clear();
    
    // Check type validation
    for (const auto& entry : config.pImpl->entries) {
        if (!validateType(entry.key, entry.value, errors)) {
            continue;  // Type validation failed, already added error
        }
        
        if (!validateRange(entry.key, entry.value, errors)) {
            continue;  // Range validation failed, already added error
        }
        
        if (!validateDependencies(entry.key, config, errors)) {
            continue;  // Dependency validation failed, already added error
        }
    }
    
    // Validate required keys
    std::vector<std::string> requiredKeys = {"neuron_count", "region_count", "connection_probability"};
    for (const auto& key : requiredKeys) {
        if (!config.has(key)) {
            errors.push_back("Missing required configuration key: " + key);
        }
    }
    
    // Validate dependencies between keys
    if (config.has("neuron_count") && config.has("region_count")) {
        auto neuronCountOpt = config.get<int64_t>("neuron_count");
        auto regionCountOpt = config.get<int64_t>("region_count");
        if (neuronCountOpt && regionCountOpt && regionCountOpt.value() > 0) {
            if (neuronCountOpt.value() / regionCountOpt.value() < 1) {
                errors.push_back("Configuration warning: neurons per region too low (< 1)");
            }
        }
    }
    
    return errors.empty();
}

bool ConfigValidator::validateType(const std::string& key, const ConfigValue& value, std::vector<std::string>& errors) {
    auto it = typeValidators_.find(key);
    if (it != typeValidators_.end()) {
        if (!it->second(value)) {
            errors.push_back("Invalid type for configuration key: " + key);
            return false;
        }
    }
    return true;
}

bool ConfigValidator::validateRange(const std::string& key, const ConfigValue& value, std::vector<std::string>& errors) {
    auto it = rangeValidators_.find(key);
    if (it != rangeValidators_.end()) {
        const auto& range = it->second;
        bool valid = std::visit([&range](auto&& arg) -> bool {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int>) {
                return static_cast<double>(arg) >= range.first && static_cast<double>(arg) <= range.second;
            } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
                return static_cast<double>(arg) >= range.first && static_cast<double>(arg) <= range.second;
            }
            return true;
        }, value);
        
        if (!valid) {
            errors.push_back("Value out of range for configuration key: " + key + ". Valid range: [" + 
                            std::to_string(range.first) + ", " + std::to_string(range.second) + "]");
            return false;
        }
    }
    return true;
}

bool ConfigValidator::validateDependencies(const std::string& key, const Config& config, std::vector<std::string>& errors) {
    // Validate dependencies based on configuration keys
    if (key == "region_count") {
        auto neuronCountOpt = config.get<int64_t>("neuron_count");
        auto regionCountOpt = config.get<int64_t>("region_count");
        if (neuronCountOpt && regionCountOpt) {
            if (regionCountOpt.value() > neuronCountOpt.value()) {
                errors.push_back("region_count cannot be greater than neuron_count");
                return false;
            }
        }
    }
    
    if (key == "stdp_ltp_weight" || key == "stdp_ltd_weight") {
        auto ltpOpt = config.get<double>("stdp_ltp_weight");
        auto ltdOpt = config.get<double>("stdp_ltd_weight");
        if (ltpOpt && ltdOpt) {
            if (std::abs(ltpOpt.value() - ltdOpt.value()) < 0.001) {
                errors.push_back("stdp_ltp_weight and stdp_ltd_weight should differ by at least 0.001");
                return false;
            }
        }
    }
    
    return true;
}

// ============================================================================
// CONFIG PRESETS IMPLEMENTATION
// ============================================================================

Config ConfigPresets::createDefaultConfig() {
    Config config;
    
    // Basic brain parameters
    config.set("neuron_count", static_cast<int64_t>(1000));
    config.set("region_count", static_cast<int64_t>(1));
    config.set("connection_probability", 0.1f);
    
    // Random seed
    config.set("random_seed", static_cast<uint64_t>(42));
    
    // STDP parameters
    config.set("stdp_ltp_weight", 0.01f);
    config.set("stdp_ltd_weight", 0.012f);
    config.set("stdp_tau", 20.0f);
    
    // Structural plasticity
    config.set("synaptogenesis_rate", 0.0001f);
    config.set("pruning_rate", 0.00001f);
    
    // Simulation parameters
    config.set("simulation_timestep", 0.001);
    config.set("replay_interval", 100);
    config.set("consolidation_interval", 1000);
    
    // Memory systems
    config.set("max_episodes", 1000);
    config.set("working_memory_capacity", 100);
    
    // Attention system
    config.set("attention_inhibition_strength", 0.5f);
    config.set("attention_excitation_strength", 1.5f);
    
    // Planning
    config.set("planning_depth", 5);
    
    // Checkpoint system
    config.set("checkpoint_dir", std::string("./checkpoints"));
    config.set("checkpoint_max_files", 10000);
    config.set("checkpoint_max_file_size", 5000000);
    config.set("checkpoint_compression_ratio", 0.3f);
    
    return config;
}

Config ConfigPresets::createMinimalConfig() {
    Config config;
    
    config.set("neuron_count", static_cast<int64_t>(100));
    config.set("region_count", static_cast<int64_t>(1));
    config.set("connection_probability", 0.05f);
    
    return config;
}

Config ConfigPresets::createDevelopmentConfig() {
    Config config = createDefaultConfig();
    
    // Development-focused parameters
    config.set("synaptogenesis_rate", 0.001f);
    config.set("pruning_rate", 0.0001f);
    config.set("replay_interval", 10);
    config.set("consolidation_interval", 100);
    
    return config;
}

Config ConfigPresets::createProductionConfig() {
    Config config = createDefaultConfig();
    
    // Production-focused parameters
    config.set("synaptogenesis_rate", 0.00001f);
    config.set("pruning_rate", 0.000001f);
    config.set("replay_interval", 1000);
    config.set("consolidation_interval", 10000);
    
    return config;
}

} // namespace nlm


// Optional JSON parser - try to include nlohmann::json if available
#ifdef NLOHMANN_JSON_HPP
#include "nlohmann/json.hpp"
namespace nlohmann {
    template<typename T> class basic_json;
    using json = basic_json<nlohmann::detail::value_t, nlohmann::detail::value_conputer,
                          nlohmann::detail::allocator_t<nlohmann::detail::default_base_json>,
                          nlohmann::detail::json_serializer>;
}
#endif

namespace nlm {

// ============================================================================
// CONFIGURATION PARSING AND VALIDATION
// ============================================================================

// Forward declaration for parser
class ConfigParser;

// Configuration validation class
class ConfigValidator {
public:
    static bool validateConfiguration(const Config& config, std::vector<std::string>& errors);
    static bool validateType(const std::string& key, const ConfigValue& value, std::vector<std::string>& errors);
    static bool validateRange(const std::string& key, const ConfigValue& value, std::vector<std::string>& errors);
    static bool validateDependencies(const std::string& key, const Config& config, std::vector<std::string>& errors);
private:
    static std::unordered_map<std::string, std::function<bool(const ConfigValue&)>> typeValidators_;
    static std::unordered_map<std::string, std::pair<double, double>> rangeValidators_;
};

// Configuration parser class
class ConfigParser {
public:
    static bool parseJSON(const std::string& content, Config& config);
    static bool parseYAML(const std::string& content, Config& config);
    static bool parseKeyValue(const std::string& content, Config& config);
    static bool mergeConfigurations(Config& baseConfig, const Config& overrideConfig);
    static std::string contentToString(std::istream& stream);
};

// Configuration presets
class ConfigPresets {
public:
    static Config createDefaultConfig();
    static Config createMinimalConfig();
    static Config createDevelopmentConfig();
    static Config createProductionConfig();
};

// File format detection
enum class FileFormat {
    KeyValue,
    JSON,
    YAML
};

// ============================================================================
// CONFIG STRUCTURE IMPLEMENTATION
// ============================================================================

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    
    // Additional metadata for advanced configuration
    std::unordered_map<std::string, std::vector<std::string>> nestedKeys_;
    bool validationEnabled_ = false;
    std::vector<std::string> validationErrors_;
    FileFormat fileFormat_ = FileFormat::KeyValue;
    
    // Parser and validator instances (shared across configs)
    static std::unique_ptr<ConfigParser> parser_;
    static std::unique_ptr<ConfigValidator> validator_;
    
    Impl() : entries(), nestedKeys_(), validationEnabled_(false) {}
    
    void addNestedKey(const std::string& key, const std::string& parent) {
        nestedKeys_[parent].push_back(key);
    }
    
    const std::vector<std::string>& getNestedKeys(const std::string& parent) const {
        auto it = nestedKeys_.find(parent);
        static const std::vector<std::string> empty;
        return it != nestedKeys_.end() ? it->second : empty;
    }
    
    void setValidationEnabled(bool enabled) {
        validationEnabled_ = enabled;
        if (enabled) {
            validationErrors_.clear();
        }
    }
    
    const std::vector<std::string>& getValidationErrors() const {
        return validationErrors_;
    }
    
    void addValidationError(const std::string& error) {
        validationErrors_.push_back(error);
    }
    
    void clearValidationErrors() {
        validationErrors_.clear();
    }
    
    // Initialize static parser and validator instances
    static void initialize() {
        parser_ = std::make_unique<ConfigParser>();
        validator_ = std::make_unique<ConfigValidator>();
    }
    
    static void cleanup() {
        parser_.reset();
        validator_.reset();
    }
};

// Static member definitions
std::unique_ptr<ConfigParser> Config::Impl::parser_;
std::unique_ptr<ConfigValidator> Config::Impl::validator_;

// ============================================================================
// CONFIGURATION CLASS IMPLEMENTATION
// ============================================================================

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize static parser and validator on first use
    if (!Impl::parser_) {
        Impl::initialize();
    }
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    std::string content = ConfigParser::contentToString(file);
    
    // Try to detect file format
    pImpl->fileFormat_ = detectFileFormat(content);
    
    if (pImpl->fileFormat_ == FileFormat::JSON) {
#ifdef NLOHMANN_JSON_HPP
        return Impl::parser_->parseJSON(content, *this);
#else
        NLM_LOG_ERROR("JSON parsing requested but nlohmann::json not available");
        return false;
#endif
    } else if (pImpl->fileFormat_ == FileFormat::YAML) {
#ifdef NLOHMANN_JSON_HPP
        return Impl::parser_->parseYAML(content, *this);
#else
        NLM_LOG_ERROR("YAML parsing requested but nlohmann::json not available");
        return false;
#endif
    } else {
        // Fallback to simple key=value format
        return ConfigParser::parseKeyValue(content, *this);
    }
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
    
    // Use the format from last load or from extension
    if (pImpl->fileFormat_ == FileFormat::AutoDetect) {
        std::string extension = extractFileExtension(filepath);
        pImpl->fileFormat_ = detectFileFormatFromExtension(extension);
    }
    
    if (pImpl->fileFormat_ == FileFormat::JSON) {
#ifdef NLOHMANN_JSON_HPP
        // Write as JSON
        auto json = toJSON();
        file << json.dump(2);  // Pretty print with 2-space indentation
        return true;
#else
        NLM_LOG_ERROR("JSON writing requested but nlohmann::json not available");
        return false;
#endif
    } else if (pImpl->fileFormat_ == FileFormat::YAML) {
#ifdef NLOHMANN_JSON_HPP
        // Write as YAML (simplified for now)
        file << "# YAML format configuration\n";
        file << "---\n";
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            std::visit([&file](auto&& arg) {
                if constexpr (std::is_same_v<decltype(arg), std::string>) {
                    file << entry.key << ": \"" << arg << "\"\n";
                } else if constexpr (std::is_same_v<decltype(arg), int>) {
                    file << entry.key << ": " << arg << "\n";
                } else if constexpr (std::is_same_v<decltype(arg), double>) {
                    file << entry.key << ": " << arg << "\n";
                } else if constexpr (std::is_same_v<decltype(arg), bool>) {
                    file << entry.key << ": " << (arg ? "true" : "false") << "\n";
                }
            }, entry.value);
            file << "\n";
        }
        return true;
#else
        NLM_LOG_ERROR("YAML writing requested but nlohmann::json not available");
        return false;
#endif
    } else {
        // Write as simple key=value format
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = ";
            std::visit([&file](auto&& arg) {
                if constexpr (std::is_same_v<decltype(arg), std::string>) {
                    file << "\"" << arg << "\"\n";
                } else {
                    file << arg << "\n";
                }
            }, entry.value);
        }
        return true;
    }
}

// Template method implementations
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
        std::remove_if(pImpl->entries.begin(),
                      pImpl->entries.end(),
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
    pImpl->nestedKeys_.clear();
    pImpl->clearValidationErrors();
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
    
    if (pImpl->validationEnabled_) {
        oss << "\nValidation errors:";
        for (const auto& error : pImpl->getValidationErrors()) {
            oss << "\n  - " << error;
        }
    }
    
    return oss.str();
}

// Validation helpers
void Config::enableValidation(bool enabled) {
    pImpl->setValidationEnabled(enabled);
    if (enabled) {
        pImpl->clearValidationErrors();
        // Perform validation
        std::vector<std::string> errors;
        if (Impl::validator_->validateConfiguration(*this, errors)) {
            NLM_LOG_INFO("Configuration validation passed");
        } else {
            for (const auto& error : errors) {
                NLM_LOG_ERROR("Configuration validation error: " + error);
            }
        }
    }
}

bool Config::validate() const {
    if (!pImpl->validationEnabled_) {
        return true;
    }
    
    std::vector<std::string> errors;
    return Impl::validator_->validateConfiguration(*this, errors);
}

nlohmann::json Config::toJSON() const {
    nlohmann::json j;
    for (const auto& entry : pImpl->entries) {
        std::visit([&j, &entry](auto&& arg) {
            j[entry.key] = arg;
        }, entry.value);
    }
    return j;
}

bool Config::fromJSON(const std::string& jsonStr) {
    try {
        nlohmann::json j = nlohmann::json::parse(jsonStr);
        pImpl->entries.clear();
        
        for (auto& [key, value] : j.items()) {
            // Convert JSON value to ConfigValue
            ConfigValue configValue;
            if (value.is_string()) {
                configValue = value.get<std::string>();
            } else if (value.is_number_integer()) {
                configValue = value.get<int>();
            } else if (value.is_number_float()) {
                configValue = value.get<double>();
            } else if (value.is_boolean()) {
                configValue = value.get<bool>();
            } else if (value.is_array()) {
                // Handle array types
                std::vector<double> doubleArray;
                for (const auto& item : value) {
                    doubleArray.push_back(item.get<double>());
                }
                configValue = doubleArray;
            }
            
            set(key, configValue, ConfigSource::Runtime);
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to parse JSON: ") + e.what());
        return false;
    }
}

std::string Config::toYAML() const {
    // Simplified YAML conversion for now
    std::ostringstream oss;
    oss << "# YAML format configuration\n";
    oss << "---\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "# " << entry.description << "\n";
        oss << entry.key << ": ";
        std::visit([&oss](auto&& arg) {
            if constexpr (std::is_same_v<decltype(arg), std::string>) {
                oss << "\"" << arg << "\"\n";
            } else if constexpr (std::is_same_v<decltype(arg), bool>) {
                oss << (arg ? "true" : "false") << "\n";
            } else {
                oss << arg << "\n";
            }
        }, entry.value);
        oss << "\n";
    }
    
    return oss.str();
}

bool Config::fromYAML(const std::string& yamlStr) {
    // For now, delegate to JSON parser
    return fromJSON(yamlStr);
}

FileFormat Config::detectFileFormat(const std::string& content) {
    // Try to detect format from content
    if (content.find('{') != std::string::npos && content.find('}') != std::string::npos) {
        // Check for JSON-like structure (might have false positives)
        if (content.find('"') != std::string::npos) {
            return FileFormat::JSON;
        }
    }
    
    // Simple key=value format detection
    size_t eqPos = content.find('=');
    if (eqPos != std::string::npos) {
        // Check if there's a key before the equals sign
        size_t keyEnd = content.find_first_not_of(' ', 0);
        if (keyEnd != std::string::npos && keyEnd < eqPos) {
            return FileFormat::KeyValue;
        }
    }
    
    return FileFormat::KeyValue; // Default
}

FileFormat Config::detectFileFormatFromExtension(const std::string& extension) {
    if (extension == "json" || extension == "JSON") {
        return FileFormat::JSON;
    } else if (extension == "yaml" || extension == "yml" || extension == "YAML") {
        return FileFormat::YAML;
    }
    return FileFormat::KeyValue;
}

std::string Config::extractFileExtension(const std::string& filename) {
    size_t pos = filename.find_last_of('.');
    if (pos == std::string::npos) {
        return "";
    }
    return filename.substr(pos + 1);
}

FileFormat Config::getFileFormat() const {
    return pImpl->fileFormat_;
}

void Config::setFileFormat(FileFormat format) {
    pImpl->fileFormat_ = format;
}

nlohmann::json Config::toJSON(const std::string& key) const {
    auto val = get<std::string>(key);
    nlohmann::json j;
    
    if (val.has_value()) {
        // Try to convert to JSON based on the value type
        if (val.value().find('.') != std::string::npos) {
            j = std::stod(val.value());
        } else if (val.value() == "true" || val.value() == "false") {
            j = (val.value() == "true");
        } else {
            j = val.value();
        }
    }
    
    return j;
}

bool Config::fromJSON(const std::string& key, const nlohmann::json& value) {
    try {
        std::string keyStr = key;
        nlohmann::json::const_iterator it = value.find(keyStr);
        if (it != value.end()) {
            set(keyStr, it->get<ConfigValue>(), ConfigSource::Runtime);
            return true;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to extract JSON value: ") + e.what());
    }
    return false;
}

// ============================================================================
// CONFIG PARSER IMPLEMENTATION
// ============================================================================

std::string ConfigParser::contentToString(std::istream& stream) {
    std::stringstream buffer;
    buffer << stream.rdbuf();
    return buffer.str();
}

bool ConfigParser::parseJSON(const std::string& content, Config& config) {
    try {
        nlohmann::json j = nlohmann::json::parse(content);
        config.pImpl->entries.clear();
        
        for (auto& [key, value] : j.items()) {
            // Convert JSON value to ConfigValue
            ConfigValue configValue;
            if (value.is_string()) {
                configValue = value.get<std::string>();
            } else if (value.is_number_integer()) {
                configValue = value.get<int>();
            } else if (value.is_number_float()) {
                configValue = value.get<double>();
            } else if (value.is_boolean()) {
                configValue = value.get<bool>();
            } else if (value.is_array()) {
                // Handle array types
                std::vector<double> doubleArray;
                for (const auto& item : value) {
                    doubleArray.push_back(item.get<double>());
                }
                configValue = doubleArray;
            }
            
            config.set(key, configValue, ConfigSource::File);
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to parse JSON: ") + e.what());
        return false;
    }
}

bool ConfigParser::parseYAML(const std::string& content, Config& config) {
    // For now, use JSON parser as YAML is similar
    return parseJSON(content, config);
}

bool ConfigParser::parseKeyValue(const std::string& content, Config& config) {
    config.clear();
    
    std::istringstream stream(content);
    std::string line;
    int lineNumber = 0;
    
    while (std::getline(stream, line)) {
        ++lineNumber;
        line = Config::trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = Config::trim(line.substr(0, pos));
            std::string value = Config::trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Convert string value to appropriate type
            ConfigValue configValue;
            
            // Try to detect type
            try {
                if (value.find('.') != std::string::npos) {
                    // Try double
                    size_t pos;
                    double doubleValue = std::stod(value, pos);
                    if (pos == value.length()) {
                        configValue = doubleValue;
                    } else {
                        configValue = value;
                    }
                } else {
                    // Try int
                    size_t pos;
                    int intValue = std::stoi(value, pos);
                    if (pos == value.length()) {
                        configValue = intValue;
                    } else {
                        // Try bool
                        std::transform(value.begin(), value.end(), value.begin(), ::tolower);
                        if (value == "true" || value == "false") {
                            configValue = (value == "true");
                        } else {
                            configValue = value;
                        }
                    }
                }
            } catch (const std::exception&) {
                configValue = value;
            }
            
            config.set(key, configValue, ConfigSource::File);
        }
    }
    
    return true;
}

bool ConfigParser::mergeConfigurations(Config& baseConfig, const Config& overrideConfig) {
    // Merge override config into base config
    for (const auto& entry : overrideConfig.pImpl->entries) {
        // Check if key already exists in base config
        bool keyExists = false;
        for (const auto& baseEntry : baseConfig.pImpl->entries) {
            if (baseEntry.key == entry.key) {
                keyExists = true;
                break;
            }
        }
        
        if (!keyExists) {
            // Add new key from override config
            baseConfig.set(entry.key, entry.value, entry.source);
        }
        // Otherwise, keep the base config value (override config has lower priority)
    }
    
    return true;
}

// ============================================================================
// CONFIG VALIDATOR IMPLEMENTATION
// ============================================================================

std::unordered_map<std::string, std::function<bool(const ConfigValue&)>> ConfigValidator::typeValidators_ = {
    {"neuron_count", [](const ConfigValue& v) { 
        return std::visit([](auto&& arg) -> bool { 
            using T = std::decay_t<decltype(arg)>; 
            if constexpr (std::is_same_v<T, int64_t>) {
                return arg > 0;
            } else if constexpr (std::is_same_v<T, int>) {
                return arg > 0;
            } 
            return false; 
        }, v); 
    }},
    {"region_count", [](const ConfigValue& v) { 
        return std::visit([](auto&& arg) -> bool { 
            using T = std::decay_t<decltype(arg)>; 
            if constexpr (std::is_same_v<T, int64_t>) {
                return arg > 0 && arg <= 100;
            } else if constexpr (std::is_same_v<T, int>) {
                return arg > 0 && arg <= 100;
            } 
            return false; 
        }, v); 
    }},
    {"connection_probability", [](const ConfigValue& v) { 
        return std::visit([](auto&& arg) -> bool { 
            using T = std::decay_t<decltype(arg)>; 
            if constexpr (std::is_same_v<T, double>) {
                return arg >= 0.0 && arg <= 1.0;
            } else if constexpr (std::is_same_v<T, float>) {
                return arg >= 0.0f && arg <= 1.0f;
            } 
            return false; 
        }, v); 
    }},
    {"random_seed", [](const ConfigValue& v) { 
        return std::visit([](auto&& arg) -> bool { 
            using T = std::decay_t<decltype(arg)>; 
            if constexpr (std::is_same_v<T, int64_t>) {
                return true;
            } else if constexpr (std::is_same_v<T, int>) {
                return true;
            } 
            return false; 
        }, v); 
    }},
    {"simulation_timestep", [](const ConfigValue& v) { 
        return std::visit([](auto&& arg) -> bool { 
            using T = std::decay_t<decltype(arg)>; 
            if constexpr (std::is_same_v<T, double>) {
                return arg > 0.0 && arg <= 0.1;
            } else if constexpr (std::is_same_v<T, float>) {
                return arg > 0.0f && arg <= 0.1f;
            } 
            return false; 
        }, v); 
    }},
    {"stdp_ltp_weight", [](const ConfigValue& v) { 
        return std::visit([](auto&& arg) -> bool { 
            using T = std::decay_t<decltype(arg)>; 
            if constexpr (std::is_same_v<T, double>) {
                return arg >= 0.0 && arg <= 1.0;
            } else if constexpr (std::is_same_v<T, float>) {
                return arg >= 0.0f && arg <= 1.0f;
            } 
            return false; 
        }, v); 
    }},
    {"stdp_ltd_weight", [](const ConfigValue& v) { 
        return std::visit([](auto&& arg) -> bool { 
            using T = std::decay_t<decltype(arg)>; 
            if constexpr (std::is_same_v<T, double>) {
                return arg >= 0.0 && arg <= 1.0;
            } else if constexpr (std::is_same_v<T, float>) {
                return arg >= 0.0f && arg <= 1.0f;
            } 
            return false; 
        }, v); 
    }},
    {"replay_interval", [](const ConfigValue& v) { 
        return std::visit([](auto&& arg) -> bool { 
            using T = std::decay_t<decltype(arg)>; 
            if constexpr (std::is_same_v<T, int64_t>) {
                return arg > 0;
            } else if constexpr (std::is_same_v<T, int>) {
                return arg > 0;
            } 
            return false; 
        }, v); 
    }},
    {"consolidation_interval", [](const ConfigValue& v) { 
        return std::visit([](auto&& arg) -> bool { 
            using T = std::decay_t<decltype(arg)>; 
            if constexpr (std::is_same_v<T, int64_t>) {
                return arg > 0;
            } else if constexpr (std::is_same_v<T, int>) {
                return arg > 0;
            } 
            return false; 
        }, v); 
    }},
    {"max_episodes", [](const ConfigValue& v) { 
        return std::visit([](auto&& arg) -> bool { 
            using T = std::decay_t<decltype(arg)>; 
            if constexpr (std::is_same_v<T, int64_t>) {
                return arg >= 0;
            } else if constexpr (std::is_same_v<T, int>) {
                return arg >= 0;
            } 
            return false; 
        }, v); 
    }},
    {"working_memory_capacity", [](const ConfigValue& v) { 
        return std::visit([](auto&& arg) -> bool { 
            using T = std::decay_t<decltype(arg)>; 
            if constexpr (std::is_same_v<T, int64_t>) {
                return arg >= 0;
            } else if constexpr (std::is_same_v<T, int>) {
                return arg >= 0;
            } 
            return false; 
        }, v); 
    }},
    {"checkpoint_max_files", [](const ConfigValue& v) { 
        return std::visit([](auto&& arg) -> bool { 
            using T = std::decay_t<decltype(arg)>; 
            if constexpr (std::is_same_v<T, int64_t>) {
                return arg > 0 && arg <= 1000000;
            } else if constexpr (std::is_same_v<T, int>) {
                return arg > 0 && arg <= 1000000;
            } 
            return false; 
        }, v); 
    }},
    {"checkpoint_max_file_size", [](const ConfigValue& v) { 
        return std::visit([](auto&& arg) -> bool { 
            using T = std::decay_t<decltype(arg)>; 
            if constexpr (std::is_same_v<T, int64_t>) {
                return arg > 0 && arg <= 10000000000;
            } else if constexpr (std::is_same_v<T, int>) {
                return arg > 0 && arg <= 10000000000;
            } 
            return false; 
        }, v); 
    }},
};

std::unordered_map<std::string, std::pair<double, double>> ConfigValidator::rangeValidators_ = {
    {"neuron_count", {1, 100000}},
    {"region_count", {1, 100}},
    {"connection_probability", {0.0, 1.0}},
    {"stdp_ltp_weight", {0.0, 1.0}},
    {"stdp_ltd_weight", {0.0, 1.0}},
    {"replay_interval", {1, 1000000}},
    {"consolidation_interval", {1, 1000000}},
    {"max_episodes", {0, 1000000}},
    {"working_memory_capacity", {0, 100000}},
    {"checkpoint_max_files", {1, 1000000}},
    {"checkpoint_max_file_size", {1, 10000000000}},
};

bool ConfigValidator::validateConfiguration(const Config& config, std::vector<std::string>& errors) {
    errors.clear();
    
    // Check type validation
    for (const auto& entry : config.pImpl->entries) {
        if (!validateType(entry.key, entry.value, errors)) {
            continue;  // Type validation failed, already added error
        }
        
        if (!validateRange(entry.key, entry.value, errors)) {
            continue;  // Range validation failed, already added error
        }
        
        if (!validateDependencies(entry.key, config, errors)) {
            continue;  // Dependency validation failed, already added error
        }
    }
    
    // Validate required keys
    std::vector<std::string> requiredKeys = {"neuron_count", "region_count", "connection_probability"};
    for (const auto& key : requiredKeys) {
        if (!config.has(key)) {
            errors.push_back("Missing required configuration key: " + key);
        }
    }
    
    // Validate dependencies between keys
    if (config.has("neuron_count") && config.has("region_count")) {
        auto neuronCountOpt = config.get<int64_t>("neuron_count");
        auto regionCountOpt = config.get<int64_t>("region_count");
        if (neuronCountOpt && regionCountOpt && regionCountOpt.value() > 0) {
            if (neuronCountOpt.value() / regionCountOpt.value() < 1) {
                errors.push_back("Configuration warning: neurons per region too low (< 1)");
            }
        }
    }
    
    return errors.empty();
}

bool ConfigValidator::validateType(const std::string& key, const ConfigValue& value, std::vector<std::string>& errors) {
    auto it = typeValidators_.find(key);
    if (it != typeValidators_.end()) {
        if (!it->second(value)) {
            errors.push_back("Invalid type for configuration key: " + key);
            return false;
        }
    }
    return true;
}

bool ConfigValidator::validateRange(const std::string& key, const ConfigValue& value, std::vector<std::string>& errors) {
    auto it = rangeValidators_.find(key);
    if (it != rangeValidators_.end()) {
        const auto& range = it->second;
        bool valid = std::visit([&range](auto&& arg) -> bool {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int>) {
                return static_cast<double>(arg) >= range.first && static_cast<double>(arg) <= range.second;
            } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
                return static_cast<double>(arg) >= range.first && static_cast<double>(arg) <= range.second;
            }
            return true;
        }, value);
        
        if (!valid) {
            errors.push_back("Value out of range for configuration key: " + key + ". Valid range: [" + 
                           std::to_string(range.first) + ", " + std::to_string(range.second) + "]");
            return false;
        }
    }
    return true;
}

bool ConfigValidator::validateDependencies(const std::string& key, const Config& config, std::vector<std::string>& errors) {
    // Validate dependencies based on configuration keys
    if (key == "region_count") {
        auto neuronCountOpt = config.get<int64_t>("neuron_count");
        auto regionCountOpt = config.get<int64_t>("region_count");
        if (neuronCountOpt && regionCountOpt) {
            if (regionCountOpt.value() > neuronCountOpt.value()) {
                errors.push_back("region_count cannot be greater than neuron_count");
                return false;
            }
        }
    }
    
    if (key == "stdp_ltp_weight" || key == "stdp_ltd_weight") {
        auto ltpOpt = config.get<double>("stdp_ltp_weight");
        auto ltdOpt = config.get<double>("stdp_ltd_weight");
        if (ltpOpt && ltdOpt) {
            if (std::abs(ltpOpt.value() - ltdOpt.value()) < 0.001) {
                errors.push_back("stdp_ltp_weight and stdp_ltd_weight should differ by at least 0.001");
                return false;
            }
        }
    }
    
    return true;
}

// ============================================================================
// CONFIG PRESETS IMPLEMENTATION
// ============================================================================

Config ConfigPresets::createDefaultConfig() {
    Config config;
    
    // Basic brain parameters
    config.set("neuron_count", static_cast<int64_t>(1000));
    config.set("region_count", static_cast<int64_t>(1));
    config.set("connection_probability", 0.1f);
    
    // Random seed
    config.set("random_seed", static_cast<uint64_t>(42));
    
    // STDP parameters
    config.set("stdp_ltp_weight", 0.01f);
    config.set("stdp_ltd_weight", 0.012f);
    config.set("stdp_tau", 20.0f);
    
    // Structural plasticity
    config.set("synaptogenesis_rate", 0.0001f);
    config.set("pruning_rate", 0.00001f);
    
    // Simulation parameters
    config.set("simulation_timestep", 0.001);
    config.set("replay_interval", 100);
    config.set("consolidation_interval", 1000);
    
    // Memory systems
    config.set("max_episodes", 1000);
    config.set("working_memory_capacity", 100);
    
    // Attention system
    config.set("attention_inhibition_strength", 0.5f);
    config.set("attention_excitation_strength", 1.5f);
    
    // Planning
    config.set("planning_depth", 5);
    
    // Checkpoint system
    config.set("checkpoint_dir", std::string("./checkpoints"));
    config.set("checkpoint_max_files", 10000);
    config.set("checkpoint_max_file_size", 5000000);
    config.set("checkpoint_compression_ratio", 0.3f);
    
    return config;
}

Config ConfigPresets::createMinimalConfig() {
    Config config;
    
    config.set("neuron_count", static_cast<int64_t>(100));
    config.set("region_count", static_cast<int64_t>(1));
    config.set("connection_probability", 0.05f);
    
    return config;
}

Config ConfigPresets::createDevelopmentConfig() {
    Config config = createDefaultConfig();
    
    // Development-focused parameters
    config.set("synaptogenesis_rate", 0.001f);
    config.set("pruning_rate", 0.0001f);
    config.set("replay_interval", 10);
    config.set("consolidation_interval", 100);
    
    return config;
}

Config ConfigPresets::createProductionConfig() {
    Config config = createDefaultConfig();
    
    // Production-focused parameters
    config.set("synaptogenesis_rate", 0.00001f);
    config.set("pruning_rate", 0.000001f);
    config.set("replay_interval", 1000);
    config.set("consolidation_interval", 10000);
    
    return config;
}

} // namespace nlm