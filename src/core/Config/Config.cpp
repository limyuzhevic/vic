#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <unordered_map>
#include <functional>
#include <regex>

namespace nlm {

// Forward declarations for enhanced types
enum class ValidationLevel {
    None,
    Warning,
    Error,
    Strict
};

// Enhanced configuration value types
using ConfigValidator = std::function<bool(const ConfigValue&, std::string& errorMsg)>;

struct ConfigOption {
    std::string key;
    std::string description;
    ConfigSource defaultSource;
    ConfigValidator validator;
    std::vector<std::string> aliases;
    bool required;
    
    ConfigOption(const std::string& k, const std::string& desc, 
                 ConfigSource source = ConfigSource::Default,
                 const ConfigValidator& val = nullptr,
                 const std::vector<std::string>& al = {},
                 bool req = false)
        : key(k), description(desc), defaultSource(source), 
          validator(val), aliases(al), required(req) {}
};

// Enhanced Config implementation with validation and better CLI parsing
struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, ConfigOption> options;
    ValidationLevel validationLevel;
    
    // Built-in option definitions
    Impl() : validationLevel(ValidationLevel::Warning) {
        initializeBuiltInOptions();
    }
    
    void initializeBuiltInOptions() {
        // Core configuration options
        options["random_seed"] = ConfigOption(
            "random_seed", 
            "Random seed for reproducible experiments",
            ConfigSource::Default,
            [](const ConfigValue& v, std::string&) { return std::holds_alternative<int64_t>(v); },
            {"seed", "random-seed"},
            false
        );
        
        options["simulation_timestep"] = ConfigOption(
            "simulation_timestep",
            "Simulation timestep in seconds",
            ConfigSource::Default,
            [](const ConfigValue& v, std::string&) { return std::holds_alternative<double>(v); },
            {"timestep", "dt"},
            false
        );
        
        options["neuron_count"] = ConfigOption(
            "neuron_count",
            "Number of neurons in the network",
            ConfigSource::Default,
            [](const ConfigValue& v, std::string&) { return std::holds_alternative<int64_t>(v) && std::get<int64_t>(v) > 0; },
            {"neurons"},
            false
        );
        
        options["region_count"] = ConfigOption(
            "region_count",
            "Number of neural regions",
            ConfigSource::Default,
            [](const ConfigValue& v, std::string&) { return std::holds_alternative<int64_t>(v) && std::get<int64_t>(v) > 0; },
            {"regions"},
            false
        );
        
        options["connection_probability"] = ConfigOption(
            "connection_probability",
            "Probability of connections between neurons",
            ConfigSource::Default,
            [](const ConfigValue& v, std::string&) { 
                return std::holds_alternative<double>(v) && std::get<double>(v) >= 0.0 && std::get<double>(v) <= 1.0; 
            },
            {"connection_prob", "conn_prob"},
            false
        );
        
        // Plasticity options
        options["stdp_ltp_weight"] = ConfigOption(
            "stdp_ltp_weight",
            "STDP Long-Term Potentiation weight factor",
            ConfigSource::Default,
            [](const ConfigValue& v, std::string&) { return std::holds_alternative<float>(v) && std::get<float>(v) > 0.0f; },
            {"stdp_ltp", "ltp_weight"},
            false
        );
        
        options["stdp_ltd_weight"] = ConfigOption(
            "stdp_ltd_weight",
            "STDP Long-Term Depression weight factor",
            ConfigSource::Default,
            [](const ConfigValue& v, std::string&) { return std::holds_alternative<float>(v) && std::get<float>(v) > 0.0f; },
            {"stdp_ltd", "ltd_weight"},
            false
        );
        
        options["stdp_tau"] = ConfigOption(
            "stdp_tau",
            "STDP time constant (tau)",
            ConfigSource::Default,
            [](const ConfigValue& v, std::string&) { return std::holds_alternative<float>(v) && std::get<float>(v) > 0.0f; },
            {"tau"},
            false
        );
        
        options["synaptogenesis_rate"] = ConfigOption(
            "synaptogenesis_rate",
            "Rate of new synapse formation",
            ConfigSource::Default,
            [](const ConfigValue& v, std::string&) { return std::holds_alternative<float>(v) && std::get<float>(v) >= 0.0f; },
            {"synapse_rate"},
            false
        );
        
        options["pruning_rate"] = ConfigOption(
            "pruning_rate",
            "Rate of synapse elimination",
            ConfigSource::Default,
            [](const ConfigValue& v, std::string&) { return std::holds_alternative<float>(v) && std::get<float>(v) >= 0.0f; },
            {"pruning"},
            false
        );
        
        // Advanced options
        options["performance_monitoring"] = ConfigOption(
            "performance_monitoring",
            "Enable performance monitoring and profiling",
            ConfigSource::Default,
            [](const ConfigValue& v, std::string&) { return std::holds_alternative<bool>(v); },
            {"performance", "perf"},
            false
        );
        
        options["debug_mode"] = ConfigOption(
            "debug_mode",
            "Enable debug mode with additional diagnostics",
            ConfigSource::Default,
            [](const ConfigValue& v, std::string&) { return std::holds_alternative<bool>(v); },
            {"debug"},
            false
        );
        
        options["verbose_level"] = ConfigOption(
            "verbose_level",
            "Verbose logging level (0-3)",
            ConfigSource::Default,
            [](const ConfigValue& v, std::string&) { 
                return std::holds_alternative<int>(v) && std::get<int>(v) >= 0 && std::get<int>(v) <= 3; 
            },
            {"verbose", "verbosity"},
            false
        );
        
        options["output_format"] = ConfigOption(
            "output_format",
            "Output format for results (text, json, csv, markdown)",
            ConfigSource::Default,
            [](const ConfigValue& v, std::string&) {
                std::string format = std::get<std::string>(v);
                return format == "text" || format == "json" || format == "csv" || format == "markdown";
            },
            {"format"},
            false
        );
        
        options["experiment"] = ConfigOption(
            "experiment",
            "Experiment(s) to run (comma-separated list)",
            ConfigSource::CommandLine,
            [](const ConfigValue& v, std::string&) { return std::holds_alternative<std::string>(v); },
            {"experiments"},
            false
        );
        
        options["output_dir"] = ConfigOption(
            "output_dir",
            "Directory for experiment results output",
            ConfigSource::CommandLine,
            [](const ConfigValue& v, std::string&) { return std::holds_alternative<std::string>(v); },
            {"output_directory"},
            false
        );
    }
    
    bool validateConfigEntry(const ConfigEntry& entry) {
        const auto& it = options.find(entry.key);
        if (it == options.end()) {
            // Unknown option - warning unless validation is strict
            if (validationLevel == ValidationLevel::Strict) {
                return false;
            }
            NLM_LOG_WARNING("Unknown configuration option: " + entry.key);
            return true; // Still allow it
        }
        
        const ConfigOption& opt = it->second;
        
        // Check if value type matches
        if (!checkValueType(entry.value, opt)) {
            if (validationLevel >= ValidationLevel::Error) {
                return false;
            }
            NLM_LOG_WARNING("Type mismatch for option '" + entry.key + "': expected " + getTypeName(entry.value) + ".");
        }
        
        // Run custom validator if provided
        if (opt.validator) {
            std::string errorMsg;
            if (!opt.validator(entry.value, errorMsg)) {
                if (validationLevel >= ValidationLevel::Error) {
                    return false;
                }
                NLM_LOG_WARNING("Validation failed for option '" + entry.key + "': " + errorMsg);
            }
        }
        
        return true;
    }
    
    bool checkValueType(const ConfigValue& value, const ConfigOption& option) {
        // Check direct type match
        auto checkDirectType = [&value](auto&& expectedType) {
            return std::holds_alternative<std::decay_t<decltype(expectedType)>>(value);
        };
        
        // Check all possible types
        return checkDirectType(int()) || 
               checkDirectType(int64_t()) ||
               checkDirectType(double()) ||
               checkDirectType(bool()) ||
               checkDirectType(std::string()) ||
               checkDirectType(std::vector<int>()) ||
               checkDirectType(std::vector<double>()) ||
               checkDirectType(std::vector<std::string>());
    }
    
    std::string getTypeName(const ConfigValue& value) {
        if (std::holds_alternative<int>(value)) return "int";
        if (std::holds_alternative<int64_t>(value)) return "int64_t";
        if (std::holds_alternative<double>(value)) return "double";
        if (std::holds_alternative<bool>(value)) return "bool";
        if (std::holds_alternative<std::string>(value)) return "string";
        if (std::holds_alternative<std::vector<int>>(value)) return "int[]";
        if (std::holds_alternative<std::vector<double>>(value)) return "double[]";
        if (std::holds_alternative<std::vector<std::string>>(value)) return "string[]";
        return "unknown";
    }
    
    std::string valueToString(const ConfigValue& value) const {
        std::ostringstream oss;
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << '"' << arg << '"';
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "["; for (size_t i = 0; i < arg.size(); ++i) {
                    oss << arg[i]; if (i < arg.size() - 1) oss << ", ";
                } oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "["; for (size_t i = 0; i < arg.size(); ++i) {
                    oss << arg[i]; if (i < arg.size() - 1) oss << ", ";
                } oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "["; for (size_t i = 0; i < arg.size(); ++i) {
                    oss << '"' << arg[i] << '"'; if (i < arg.size() - 1) oss << ", ";
                } oss << "]";
            } else {
                oss << arg;
            }
        }, value);
        return oss.str();
    }
    
    std::string setValidationLevel(int level) {
        switch (level) {
            case 0: validationLevel = ValidationLevel::None; break;
            case 1: validationLevel = ValidationLevel::Warning; break;
            case 2: validationLevel = ValidationLevel::Error; break;
            case 3: validationLevel = ValidationLevel::Strict; break;
            default: validationLevel = ValidationLevel::Warning;
        }
        return "Validation level set to: " + std::to_string(level);
    }
    
    std::string getValidationReport() const {
        std::ostringstream oss;
        oss << "Configuration Validation Report:" << std::endl;
        oss << "  Level: " << static_cast<int>(validationLevel) << std::endl;
        oss << "  Total options: " << options.size() << std::endl;
        
        int validEntries = 0;
        int invalidEntries = 0;
        
        for (const auto& entry : entries) {
            const auto& it = options.find(entry.key);
            if (it != options.end()) {
                validEntries++;
            } else {
                invalidEntries++;
            }
        }
        
        oss << "  Valid entries: " << validEntries << std::endl;
        oss << "  Invalid entries: " << invalidEntries << std::endl;
        return oss.str();
    }
    
    bool loadFromFile(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_WARNING("Could not open config file: " + filepath);
            return false;
        }
        
        std::string line;
        int lineNum = 0;
        
        while (std::getline(file, line)) {
            lineNum++;
            line = trim(line);
            
            // Skip empty lines and comments
            if (line.empty() || line[0] == '#' || line[0] == '/' || line.find('#') != std::string::npos) {
                continue;
            }
            
            // Parse key=value pairs with better error handling
            size_t pos = line.find('=');
            if (pos == std::string::npos) {
                if (validationLevel >= ValidationLevel::Error) {
                    NLM_LOG_ERROR("Invalid config line " + std::to_string(lineNum) + ": " + line);
                    return false;
                }
                NLM_LOG_WARNING("Skipping invalid config line " + std::to_string(lineNum) + ": " + line);
                continue;
            }
            
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && ((value.front() == '"' && value.back() == '"') || 
                                     (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Try to parse value as different types
            ConfigValue configValue;
            bool parsed = false;
            
            // Try bool
            if (!parsed) {
                if (value == "true" || value == "false") {
                    configValue = (value == "true");
                    parsed = true;
                }
            }
            
            // Try integer
            if (!parsed) {
                try {
                    size_t posEnd;
                    int intVal = std::stoi(value, &posEnd);
                    if (posEnd == value.size()) {
                        configValue = intVal;
                        parsed = true;
                    }
                } catch (...) {}
            }
            
            // Try int64
            if (!parsed) {
                try {
                    size_t posEnd;
                    int64_t int64Val = std::stoll(value, &posEnd);
                    if (posEnd == value.size()) {
                        configValue = int64Val;
                        parsed = true;
                    }
                } catch (...) {}
            }
            
            // Try double
            if (!parsed) {
                try {
                    size_t posEnd;
                    double doubleVal = std::stod(value, &posEnd);
                    if (posEnd == value.size()) {
                        configValue = doubleVal;
                        parsed = true;
                    }
                } catch (...) {}
            }
            
            // Try float
            if (!parsed) {
                try {
                    size_t posEnd;
                    float floatVal = std::stof(value, &posEnd);
                    if (posEnd == value.size()) {
                        configValue = floatVal;
                        parsed = true;
                    }
                } catch (...) {}
            }
            
            // Use as string if not parsed
            if (!parsed) {
                configValue = value;
            }
            
            ConfigEntry entry(key, configValue, ConfigSource::File, "Parsed from config file");
            
            // Validate the entry
            if (!validateConfigEntry(entry)) {
                if (validationLevel >= ValidationLevel::Error) {
                    NLM_LOG_ERROR("Config validation failed for key: " + key);
                    return false;
                }
                NLM_LOG_WARNING("Config validation warning for key: " + key);
            }
            
            entries.push_back(entry);
        }
        
        NLM_LOG_INFO("Loaded " + std::to_string(entries.size()) + " configuration entries from: " + filepath);
        return true;
    }
    
    bool loadFromArgs(int argc, char** argv) {
        // Clear existing command line entries to avoid duplicates
        entries.erase(std::remove_if(entries.begin(), entries.end(),
            [](const ConfigEntry& e) { return e.source == ConfigSource::CommandLine; }),
            entries.end());
        
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            // Enhanced CLI argument parsing with better error handling
            if (arg.find("--") == 0) {
                // Handle --key=value format
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    set(key, value, ConfigSource::CommandLine);
                }
                // Handle --key value format
                else if (i + 1 < argc && argv[i + 1][0] != '-') {
                    std::string key = arg.substr(2);
                    std::string value = argv[++i];
                    set(key, value, ConfigSource::CommandLine);
                }
            }
            // Handle -k value format
            else if (arg[0] == '-' && arg.size() > 1 && arg[1] != '-') {
                std::string key = arg.substr(1);
                if (i + 1 < argc && argv[i + 1][0] != '-') {
                    std::string value = argv[++i];
                    set(key, value, ConfigSource::CommandLine);
                }
            }
        }
        
        return true;
    }
    
    bool saveToFile(const std::string& filepath) const {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for writing: " + filepath);
            return false;
        }
        
        file << "# NLM Configuration File\n";
        file << "# Generated by NLM Phase 2 Enhanced Configuration System\n";
        file << "# " << std::ctime(&std::time(nullptr));
        file << "\n";
        
        for (const auto& entry : entries) {
            file << "# " << entry.description << "\n";
            file << "# Source: " << static_cast<int>(entry.source) << "\n";
            file << entry.key << " = " << valueToString(entry.value) << "\n\n";
        }
        
        file.close();
        NLM_LOG_INFO("Configuration saved to: " + filepath);
        return true;
    }
    
    // Enhanced get methods with better error handling and type conversion
    template<typename T>
    std::optional<T> get(const std::string& key) const {
        auto it = std::find_if(entries.begin(), entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it == entries.end()) {
            return std::nullopt;
        }
        
        try {
            T result = std::get<T>(it->value);
            
            // Validate the value
            const auto& optIt = options.find(key);
            if (optIt != options.end() && optIt->second.validator) {
                std::string errorMsg;
                if (!optIt->second.validator(it->value, errorMsg)) {
                    NLM_LOG_WARNING("Value validation failed for option '" + key + "': " + errorMsg);
                }
            }
            
            return result;
        } catch (const std::bad_variant_access&) {
            NLM_LOG_WARNING("Type mismatch for option '" + key + "': expected " + getTypeName(it->value) + ".");
            return std::nullopt;
        }
    }
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const {
        auto val = get<T>(key);
        if (val.has_value()) {
            return val.value();
        } else {
            // Try to get from aliases
            auto it = options.find(key);
            if (it != options.end()) {
                for (const auto& alias : it->second.aliases) {
                    auto aliasVal = get<T>(alias);
                    if (aliasVal.has_value()) {
                        return aliasVal.value();
                    }
                }
            }
            return defaultValue;
        }
    }
    
    void set(const std::string& key, const ConfigValue& value, ConfigSource source) {
        auto it = std::find_if(entries.begin(), entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it != entries.end()) {
            it->value = value;
            it->source = source;
        } else {
            // Get description from options if available
            std::string description = "User-provided configuration";
            auto optIt = options.find(key);
            if (optIt != options.end()) {
                description = optIt->second.description;
            }
            
            ConfigEntry entry(key, value, source, description);
            
            // Validate the entry
            if (!validateConfigEntry(entry)) {
                if (validationLevel >= ValidationLevel::Error) {
                    throw std::runtime_error("Configuration validation failed for key: " + key);
                }
                NLM_LOG_WARNING("Configuration validation warning for key: " + key);
            }
            
            entries.push_back(entry);
        }
    }
    
    void set(const std::string& key, const std::string& value, ConfigSource source) {
        set(key, ConfigValue(value), source);
    }
    
    void set(const std::string& key, int value, ConfigSource source) {
        set(key, ConfigValue(value), source);
    }
    
    void set(const std::string& key, double value, ConfigSource source) {
        set(key, ConfigValue(value), source);
    }
    
    void set(const std::string& key, bool value, ConfigSource source) {
        set(key, ConfigValue(value), source);
    }
    
    bool has(const std::string& key) const {
        return std::any_of(entries.begin(), entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
    }
    
    void remove(const std::string& key) {
        entries.erase(
            std::remove_if(entries.begin(), entries.end(),
                [&key](const ConfigEntry& e) { return e.key == key; }),
            entries.end()
        );
    }
    
    std::vector<std::string> getKeys() const {
        std::vector<std::string> keys;
        keys.reserve(entries.size());
        for (const auto& entry : entries) {
            keys.push_back(entry.key);
        }
        return keys;
    }
    
    void clear() {
        entries.clear();
    }
    
    std::string summary() const {
        std::ostringstream oss;
        oss << "Enhanced Configuration Summary:" << std::endl;
        oss << "  Total entries: " << entries.size() << std::endl;
        oss << "  Validation level: " << static_cast<int>(validationLevel) << std::endl;
        oss << "  Known options: " << options.size() << std::endl;
        oss << "\nEntries:" << std::endl;
        
        for (const auto& entry : entries) {
            oss << "  " << entry.key << " = " << valueToString(entry.value) 
                << " (" << static_cast<int>(entry.source) << ")" << std::endl;
        }
        
        return oss.str();
    }
    
    std::string validationReport() const {
        return getValidationReport();
    }
    
    static std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }
    
    static std::string toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
    
    // Get list of all available options
    std::vector<std::string> getAvailableOptions() const {
        std::vector<std::string> optionKeys;
        optionKeys.reserve(options.size());
        for (const auto& pair : options) {
            optionKeys.push_back(pair.first);
            // Also include aliases
            for (const auto& alias : pair.second.aliases) {
                optionKeys.push_back(alias);
            }
        }
        return optionKeys;
    }
    
    // Get option information
    std::optional<ConfigOption> getOptionInfo(const std::string& key) const {
        auto it = options.find(key);
        if (it != options.end()) {
            return it->second;
        }
        
        // Check aliases
        for (const auto& pair : options) {
            for (const auto& alias : pair.second.aliases) {
                if (alias == key) {
                    return pair.second;
                }
            }
        }
        
        return std::nullopt;
    }
    
    // Get configuration by source
    std::vector<ConfigEntry> getBySource(ConfigSource source) const {
        std::vector<ConfigEntry> result;
        for (const auto& entry : entries) {
            if (entry.source == source) {
                result.push_back(entry);
            }
        }
        return result;
    }
    
    // Merge configurations
    void merge(const Config& other) {
        for (const auto& entry : other.entries) {
            // Override with higher priority source
            auto it = std::find_if(entries.begin(), entries.end(),
                [&entry](const ConfigEntry& e) { return e.key == entry.key; });
            
            if (it != entries.end()) {
                // Keep the one with higher source priority
                if (static_cast<int>(entry.source) > static_cast<int>(it->source)) {
                    *it = entry;
                }
            } else {
                entries.push_back(entry);
            }
        }
    }
    
    // Get configuration with interpolation support
    std::optional<ConfigValue> getWithInterpolation(const std::string& key) const {
        auto entryIt = std::find_if(entries.begin(), entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (entryIt == entries.end()) {
            return std::nullopt;
        }
        
        ConfigValue value = entryIt->value;
        
        // Handle string interpolation for references like ${other_key}
        if (std::holds_alternative<std::string>(value)) {
            std::string strValue = std::get<std::string>(value);
            std::regex interpolationPattern(R"(\${([^}]+)})");
            bool changed = true;
            
            while (changed) {
                changed = false;
                std::smatch match;
                if (std::regex_search(strValue, match, interpolationPattern)) {
                    changed = true;
                    std::string placeholder = match.str(0);
                    std::string reference = match.str(1);
                    
                    auto refEntry = std::find_if(entries.begin(), entries.end(),
                        [&reference](const ConfigEntry& e) { return e.key == reference; });
                    
                    if (refEntry != entries.end()) {
                        std::string replacement = valueToString(refEntry->value);
                        size_t pos = strValue.find(placeholder);
                        if (pos != std::string::npos) {
                            strValue.replace(pos, placeholder.length(), replacement);
                        }
                    }
                }
            }
            
            value = strValue;
        }
        
        return value;
    }
};

// Explicit template instantiations for enhanced Config
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

// Global function to get enhanced configuration summary
inline std::string getEnhancedConfigSummary(const nlm::Config& config) {
    return config.summary();
}