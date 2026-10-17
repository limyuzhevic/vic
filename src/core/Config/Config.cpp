#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <cmath>
#include <ctime>
#include "../core/Logger/Logger.hpp"
#include "../core/Error/ErrorHandling.hpp"

namespace nlm {

// Initialize static members
std::map<ConfigSource, std::unordered_map<std::string, ConfigEntry>> Config::m_sources;
std::mutex Config::m_mutex;
std::vector<SchemaEntry> Config::m_schema;
bool Config::m_validationEnabled = true;
bool Config::m_schemaValidationEnabled = true;
bool Config::m_dependencyValidationEnabled = true;
std::unordered_map<std::string, ConfigPreset> Config::m_presets;
std::unordered_map<std::string, ConfigTemplate> Config::m_templates;
std::unordered_map<std::string, std::vector<std::string>> Config::m_dependencies;
std::unordered_map<std::string, std::vector<SchemaEntry>> Config::m_inheritance;

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    bool validateOnLoad;
    std::string configFilePath;
    std::unordered_map<std::string, ConfigSource> sourceMap;
    std::vector<std::string> tags;
    
    Impl() : entries(), validateOnLoad(true), configFilePath("") {
        // Set up file modification monitoring
        lastModified = std::filesystem::file_time_type::clock::now();
    }
    
    std::filesystem::file_time_type lastModified;
};

// Simple JSON string manipulation (for basic JSON support)
static std::string jsonEscapeString(const std::string& str) {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '\"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default:
                if (c >= 0 && c < 0x20) {
                    result += "\\u" + std::to_string((unsigned int)c);
                } else {
                    result += c;
                }
        }
    }
    return result;
}

static std::string configValueToJSON(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, double>) {
            oss << std::setprecision(6) << arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << jsonEscapeString(arg) << "\"";
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
                oss << std::setprecision(6) << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << jsonEscapeString(arg[i]) << "\"";
            }
            oss << "]";
        }
    }, value);
    return oss.str();
}

static ConfigValue jsonToConfigValue(const std::string& jsonStr) {
    // Simple JSON parsing - this is a simplified implementation
    // In production, you would use a proper JSON parser
    
    // Trim whitespace
    std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        size_t end = str.find_last_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        return str.substr(start, end - start + 1);
    }
    
    std::string str = trim(jsonStr);
    
    if (str.empty()) {
        return std::string();
    }
    
    if (str == "true") {
        return true;
    } else if (str == "false") {
        return false;
    } else if (str[0] == '\"' && str.back() == '\"') {
        // String with quotes
        return str.substr(1, str.size() - 2);
    } else if (str.find('.') != std::string::npos || str.find('e') != std::string::npos || str.find('E') != std::string::npos) {
        // Double
        try {
            return std::stod(str);
        } catch (...) {
            return std::string();
        }
    } else {
        // Try integer
        try {
            size_t pos;
            int64_t val = std::stoll(str, &pos);
            if (pos == str.size()) {
                return val;
            }
            // Try another integer
            try {
                int val = std::stoi(str);
                return val;
            } catch (...) {
                // Return as string
                return str;
            }
        } catch (...) {
            return str;
        }
    }
}

static std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S")
       << "." << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->validateOnLoad = true;
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Validate filepath before attempting to load
    NLM_CHECK_CONDITION(!filepath.empty(), "File path cannot be empty", "filepath");
    
    try {
        // Check if file exists and is readable
        if (!std::filesystem::exists(filepath)) {
            NLM_LOG_WARNING("Configuration file does not exist: " + filepath);
            return false;
        }
        
        if (!std::filesystem::is_regular_file(filepath)) {
            NLM_THROW_FILE("load", filepath, "Configuration path is not a file", 
                         FileError::ErrorType::IsDirectory);
        }
        
        // Check file modification time
        auto currentModTime = std::filesystem::last_write_time(filepath);
        if (currentModTime <= pImpl->lastModified && !pImpl->configFilePath.empty()) {
            NLM_LOG_INFO("Configuration file not modified: " + filepath);
            return true; // File hasn't changed
        }
        
        // Try to open file with error handling
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_THROW_FILE("load", filepath, "Cannot open configuration file",
                         FileError::ErrorType::PermissionDenied);
        }
        
        // Validate file size (prevent loading huge files)
        file.seekg(0, std::ios::end);
        size_t fileSize = file.tellg();
        file.seekg(0, std::ios::beg);
        
        NLM_CHECK_CONDITION(fileSize < 10 * 1024 * 1024, "Configuration file too large",
                          "file_size"); // 10MB limit
        
        // Parse and validate each line
        std::string line;
        size_t lineNumber = 0;
        while (std::getline(file, line)) {
            ++lineNumber;
            line = trim(line);
            
            // Skip empty lines and comments
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            // Parse simple key=value pairs with validation
            size_t pos = line.find('=');
            if (pos == std::string::npos) {
                NLM_LOG_WARNING("Invalid configuration line (no '='): " + line + ", line " + 
                              std::to_string(lineNumber));
                continue;
            }
            
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            NLM_CHECK_CONDITION(!key.empty(), "Empty key found in configuration", "key");
            NLM_CHECK_CONDITION(!value.empty(), "Empty value for key: " + key, "value");
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            if (!validateConfigValue(key, value)) {
                NLM_LOG_WARNING("Invalid value format for key: " + key + ", value: " + value);
            }
            
            set(key, value, ConfigSource::File);
        }
        
        // Update file modification time
        pImpl->lastModified = currentModTime;
        pImpl->configFilePath = filepath;
        
        NLM_LOG_INFO("Successfully loaded configuration from: " + filepath + 
                     " (" + std::to_string(pImpl->entries.size()) + " entries)");
        return true;
        
    } catch (const FileError& e) {
        NLM_LOG_ERROR("File error loading configuration: " + filepath + ": " + e.what());
        return false;
    } catch (const ValidationError& e) {
        NLM_LOG_ERROR("Configuration validation error: " + e.what() + ": " + filepath);
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Unexpected error loading configuration: ") + e.what() + ": " + filepath);
        return false;
    }
}

// Initialize default presets and templates
static void initializePresetsAndTemplates() {
    // Basic preset
    ConfigPreset basicPreset("basic", "Basic configuration for simple usage");
    basicPreset.values["random_seed"] = 42;
    basicPreset.values["simulation_timestep"] = 0.001;
    basicPreset.values["neuron_count"] = 1000;
    basicPreset.values["region_count"] = 2;
    basicPreset.values["connection_probability"] = 0.1;
    basicPreset.values["stdp_ltp_weight"] = 0.01;
    basicPreset.values["stdp_ltd_weight"] = 0.012;
    basicPreset.values["stdp_time_constant"] = 20.0;
    basicPreset.values["development_synaptogenesis_rate"] = 0.001;
    basicPreset.values["development_pruning_rate"] = 0.0001;
    basicPreset.values["dopamine_baseline"] = 0.1;
    basicPreset.values["reward_discount_factor"] = 0.99;
    basicPreset.values["environment_name"] = "GridWorld";
    basicPreset.values["environment_width"] = 10;
    basicPreset.values["environment_height"] = 10;
    basicPreset.values["log_level"] = std::string("INFO");
    basicPreset.values["log_to_file"] = false;
    basicPreset.values["log_filename"] = std::string("nlm.log");
    basicPreset.values["max_simulation_steps"] = 10000;
    basicPreset.values["simulation_time_limit"] = 0.0;
    basicPreset.values["visualization_enabled"] = false;
    basicPreset.values["visualization_update_rate"] = 30.0;
    Config::m_presets[basicPreset.name] = basicPreset;
    
    // Advanced preset
    ConfigPreset advancedPreset("advanced", "Advanced configuration with all features enabled");
    advancedPreset.values = basicPreset.values;
    advancedPreset.values["plasticity_learning_rate"] = 0.05;
    advancedPreset.values["neuron_count"] = 5000;
    advancedPreset.values["log_to_file"] = true;
    advancedPreset.values["visualization_enabled"] = true;
    Config::m_presets[advancedPreset.name] = advancedPreset;
    
    // Debug preset
    ConfigPreset debugPreset("debug", "Debug configuration with verbose logging and reduced complexity");
    debugPreset.values = basicPreset.values;
    debugPreset.values["neuron_count"] = 100;
    debugPreset.values["log_level"] = std::string("DEBUG");
    debugPreset.values["visualization_enabled"] = true;
    Config::m_presets[debugPreset.name] = debugPreset;
    
    // Production preset
    ConfigPreset productionPreset("production", "Production configuration optimized for performance");
    productionPreset.values = basicPreset.values;
    productionPreset.values["neuron_count"] = 2000;
    productionPreset.values["log_level"] = std::string("WARNING");
    productionPreset.values["log_to_file"] = true;
    productionPreset.values["visualization_enabled"] = false;
    Config::m_presets[productionPreset.name] = productionPreset;
    
    // Default template
    ConfigTemplate defaultTemplate("default", "Default configuration template");
    defaultTemplate.schema.push_back(SchemaEntry("random_seed", "integer", true, ConfigValue(42)));
    defaultTemplate.schema.push_back(SchemaEntry("simulation_timestep", "double", true, ConfigValue(0.001)));
    defaultTemplate.schema.push_back(SchemaEntry("neuron_count", "integer", true, ConfigValue(1000)));
    defaultTemplate.schema.push_back(SchemaEntry("region_count", "integer", true, ConfigValue(2)));
    defaultTemplate.schema.push_back(SchemaEntry("connection_probability", "double", true, ConfigValue(0.1)));
    defaultTemplate.schema.push_back(SchemaEntry("plasticity_learning_rate", "double", false, ConfigValue(0.01)));
    defaultTemplate.schema.push_back(SchemaEntry("stdp_ltp_weight", "double", false, ConfigValue(0.01)));
    defaultTemplate.schema.push_back(SchemaEntry("stdp_ltd_weight", "double", false, ConfigValue(0.012)));
    defaultTemplate.schema.push_back(SchemaEntry("stdp_time_constant", "double", false, ConfigValue(20.0)));
    defaultTemplate.schema.push_back(SchemaEntry("development_synaptogenesis_rate", "double", false, ConfigValue(0.001)));
    defaultTemplate.schema.push_back(SchemaEntry("development_pruning_rate", "double", false, ConfigValue(0.0001)));
    defaultTemplate.schema.push_back(SchemaEntry("dopamine_baseline", "double", false, ConfigValue(0.1)));
    defaultTemplate.schema.push_back(SchemaEntry("reward_discount_factor", "double", false, ConfigValue(0.99)));
    defaultTemplate.schema.push_back(SchemaEntry("environment_name", "string", false, ConfigValue(std::string("GridWorld"))));
    defaultTemplate.schema.push_back(SchemaEntry("environment_width", "integer", false, ConfigValue(10)));
    defaultTemplate.schema.push_back(SchemaEntry("environment_height", "integer", false, ConfigValue(10)));
    defaultTemplate.schema.push_back(SchemaEntry("log_level", "string", false, ConfigValue(std::string("INFO"))));
    defaultTemplate.schema.push_back(SchemaEntry("log_to_file", "boolean", false, ConfigValue(false)));
    defaultTemplate.schema.push_back(SchemaEntry("log_filename", "string", false, ConfigValue(std::string("nlm.log"))));
    defaultTemplate.schema.push_back(SchemaEntry("max_simulation_steps", "integer", false, ConfigValue(10000)));
    defaultTemplate.schema.push_back(SchemaEntry("simulation_time_limit", "double", false, ConfigValue(0.0)));
    defaultTemplate.schema.push_back(SchemaEntry("visualization_enabled", "boolean", false, ConfigValue(false)));
    defaultTemplate.schema.push_back(SchemaEntry("visualization_update_rate", "double", false, ConfigValue(30.0)));
    Config::m_templates[defaultTemplate.name] = defaultTemplate;
    
    // Debug template
    ConfigTemplate debugTemplate("debug", "Debug configuration template with reduced complexity");
    debugTemplate.schema = defaultTemplate.schema;
    debugTemplate.defaultOverrides["neuron_count"] = "100";
    debugTemplate.defaultOverrides["log_level"] = "DEBUG";
    debugTemplate.defaultOverrides["visualization_enabled"] = "true";
    debugTemplate.tags = {"debug", "development"};
    Config::m_templates[debugTemplate.name] = debugTemplate;
    
    // Simple template
    ConfigTemplate simpleTemplate("simple", "Simple configuration template for basic usage");
    simpleTemplate.schema = defaultTemplate.schema;
    simpleTemplate.defaultOverrides["neuron_count"] = "500";
    simpleTemplate.defaultOverrides["region_count"] = "1";
    simpleTemplate.defaultOverrides["log_level"] = "INFO";
    simpleTemplate.tags = {"simple", "basic"};
    Config::m_templates[simpleTemplate.name] = simpleTemplate;
    
    // Initialize dependencies
    Config::m_dependencies["stdp_ltp_weight"] = {"stdp_ltd_weight", "stdp_time_constant"};
    Config::m_dependencies["stdp_ltd_weight"] = {"stdp_ltp_weight", "stdp_time_constant"};
    Config::m_dependencies["development_synaptogenesis_rate"] = {"development_pruning_rate"};
    Config::m_dependencies["development_pruning_rate"] = {"development_synaptogenesis_rate"};
    Config::m_dependencies["dopamine_baseline"] = {};
    Config::m_dependencies["reward_discount_factor"] = {};
    Config::m_dependencies["environment_name"] = {"environment_width", "environment_height"};
    Config::m_dependencies["environment_width"] = {"environment_name"};
    Config::m_dependencies["environment_height"] = {"environment_name"};
    Config::m_dependencies["log_level"] = {};
    Config::m_dependencies["log_to_file"] = {"log_filename"};
    Config::m_dependencies["log_filename"] = {"log_to_file"};
    Config::m_dependencies["max_simulation_steps"] = {};
    Config::m_dependencies["simulation_time_limit"] = {};
    Config::m_dependencies["visualization_enabled"] = {"visualization_update_rate"};
    Config::m_dependencies["visualization_update_rate"] = {"visualization_enabled"};
    
    // Initialize schema
    Config::m_schema = defaultTemplate.schema;
}

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->validateOnLoad = true;
    // Initialize presets and templates if first instance
    if (m_sources.empty()) {
        initializePresetsAndTemplates();
    }
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Validate filepath before attempting to load
    NLM_CHECK_CONDITION(!filepath.empty(), "File path cannot be empty", "filepath");
    
    try {
        // Check if file exists and is readable
        if (!std::filesystem::exists(filepath)) {
            NLM_LOG_WARNING("Configuration file does not exist: " + filepath);
            return false;
        }
        
        if (!std::filesystem::is_regular_file(filepath)) {
            NLM_THROW_FILE("load", filepath, "Configuration path is not a file", 
                         FileError::ErrorType::IsDirectory);
        }
        
        // Check file modification time
        auto currentModTime = std::filesystem::last_write_time(filepath);
        if (currentModTime <= pImpl->lastModified && !pImpl->configFilePath.empty()) {
            NLM_LOG_INFO("Configuration file not modified: " + filepath);
            return true; // File hasn't changed
        }
        
        // Determine file format based on extension
        std::string ext = std::filesystem::path(filepath).extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        
        clear(); // Clear existing entries
        
        if (ext == ".json") {
            return loadFromJSONFile(filepath);
        } else {
            return loadFromConfigFile(filepath);
        }
        
    } catch (const FileError& e) {
        NLM_LOG_ERROR("File error loading configuration: " + filepath + ": " + e.what());
        return false;
    } catch (const ValidationError& e) {
        NLM_LOG_ERROR("Configuration validation error: " + e.what() + ": " + filepath);
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Unexpected error loading configuration: ") + e.what() + ": " + filepath);
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    try {
        NLM_CHECK_CONDITION(argc > 0, "Argument count must be positive", "argc");
        NLM_CHECK_CONDITION(argv != nullptr, "Argument vector cannot be null", "argv");
        
        for (int i = 1; i < argc; ++i) {
            NLM_CHECK_CONDITION(argv[i] != nullptr, "Argument at index " + std::to_string(i) + " is null", "argv");
            
            std::string arg(argv[i]);
            
            // Handle subcommand first
            if (arg == "--run-phase-6" || arg == "-p6") {
                // Process phase 6 arguments
                processPhase6Args(i, argc, argv);
                continue;
            } else if (arg == "--validate-config" || arg == "-v") {
                // Will validate after loading all args
                continue;
            } else if (arg == "--export-config" || arg == "-e") {
                // Will export after loading all args
                continue;
            } else if (arg == "--info" || arg == "-i") {
                // Will show info after loading all args
                continue;
            } else if (arg == "--list-presets" || arg == "-lp") {
                // Will list presets
                continue;
            } else if (arg == "--list-templates" || arg == "-lt") {
                // Will list templates
                continue;
            } else if (arg == "--help" || arg == "-h") {
                // Will show help
                continue;
            } else if (arg == "--preset" || arg == "-pre") {
                if (i + 1 < argc) {
                    std::string presetName = argv[++i];
                    loadFromPreset(presetName);
                }
                continue;
            } else if (arg == "--template" || arg == "-t") {
                if (i + 1 < argc) {
                    std::string templateName = argv[++i];
                    if (i + 1 < argc) {
                        std::string overrides = argv[++i];
                        // Parse overrides string (format: key=value,key2=value2)
                        loadFromTemplate(templateName, parseOverridesString(overrides));
                    } else {
                        loadFromTemplate(templateName, {});
                    }
                }
                continue;
            } else if (arg == "--schema" || arg == "-s") {
                if (i + 1 < argc) {
                    std::string schemaFile = argv[++i];
                    loadSchemaFromFile(schemaFile);
                }
                continue;
            }
            
            // Handle --key=value format with validation
            if (arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    
                    NLM_CHECK_CONDITION(!key.empty(), "Empty key in command line argument", "key");
                    NLM_CHECK_CONDITION(!value.empty(), "Empty value for key: " + key, "value");
                    
                    set(key, value, ConfigSource::CommandLine);
                }
            }
            // Handle -key value format with validation
            else if (arg[0] == '-' && i + 1 < argc) {
                NLM_CHECK_CONDITION(argv[i + 1] != nullptr, "Missing value for argument " + arg, "value");
                
                std::string key = arg.substr(1);
                std::string value = argv[++i];
                
                NLM_CHECK_CONDITION(!key.empty(), "Empty key in command line argument", "key");
                NLM_CHECK_CONDITION(!value.empty(), "Empty value for key: " + key, "value");
                
                set(key, value, ConfigSource::CommandLine);
            } else {
                NLM_LOG_WARNING("Unrecognized command line argument format: " + arg);
            }
        }
        
        return true;
        
    } catch (const ValidationError& e) {
        NLM_LOG_ERROR(std::string("Configuration validation error in command line args: ") + e.what());
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Unexpected error loading from command line args: ") + e.what());
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        NLM_CHECK_CONDITION(!filepath.empty(), "File path cannot be empty", "filepath");
        
        // Create directory if it doesn't exist
        std::filesystem::path dirPath = std::filesystem::path(filepath).parent_path();
        if (!dirPath.empty() && !std::filesystem::exists(dirPath)) {
            if (!std::filesystem::create_directories(dirPath)) {
                NLM_THROW_FILE("save", filepath, "Cannot create directory for configuration file",
                             FileError::ErrorType::PermissionDenied);
            }
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_THROW_FILE("save", filepath, "Cannot open configuration file for writing",
                         FileError::ErrorType::WriteError);
        }
        
        // Write configuration with error handling
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = ";
            
            // Write value with proper formatting
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else {
                    file << arg;
                }
            }, entry.value);
            
            file << "\n\n";
            
            // Check if write failed
            if (!file.good()) {
                NLM_THROW_FILE("save", filepath, "Write error during configuration save",
                             FileError::ErrorType::WriteError);
            }
        }
        
        file.flush();
        
        // Verify file was written correctly
        if (!file.good()) {
            NLM_THROW_FILE("save", filepath, "File write verification failed",
                         FileError::ErrorType::WriteError);
        }
        
        NLM_LOG_INFO("Successfully saved configuration to: " + filepath);
        return true;
        
    } catch (const FileError& e) {
        NLM_LOG_ERROR(std::string("File error saving configuration: ") + e.what());
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Unexpected error saving configuration: ") + e.what());
        return false;
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    NLM_CHECK_CONDITION(!key.empty(), "Key cannot be empty", "key");
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n
    
    if (it == pImpl->entries.end()) {
        NLM_LOG_DEBUG("Configuration key not found: " + key);
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        NLM_LOG_WARNING("Type mismatch for configuration key: " + key);
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    NLM_CHECK_CONDITION(!key.empty(), "Key cannot be empty", "key");
    
    try {
        // Validate value before setting
        if (!validateConfigValue(key, value)) {
            NLM_THROW_CONFIG("Invalid configuration value", key);
        }
        
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it != pImpl->entries.end()) {
            it->value = value;
            it->source = source;
        } else {
            pImpl->entries.emplace_back(key, value, source);
        }
        
    } catch (const ConfigError& e) {
        NLM_LOG_ERROR(std::string("Configuration error setting key ") + key + ": " + e.what());
        throw;
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
    NLM_CHECK_CONDITION(!key.empty(), "Key cannot be empty", "key");
    
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    NLM_CHECK_CONDITION(!key.empty(), "Key cannot be empty", "key");
    
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
        NLM_CHECK_CONDITION(!entry.key.empty(), "Empty key found in configuration entries", "key");
        keys.push_back(entry.key);
    }
    
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->configFilePath.clear();
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

// Helper methods for environment loading
bool Config::loadFromEnvironment(const std::string& prefix) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    try {
        for (const auto& entry : m_sources[ConfigSource::File]) {
            std::string envKey = prefix + "_" + entry.key;
            char* envValue = std::getenv(envKey.c_str());
            if (envValue != nullptr && std::string(envValue) != "") {
                std::string value(envValue);
                set(entry.key, value, ConfigSource::Environment);
                NLM_LOG_INFO("Set from environment variable " + envKey + " = " + value);
            }
        }
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading from environment: " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromPreset(const std::string& presetName) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    try {
        if (m_presets.find(presetName) == m_presets.end()) {
            NLM_LOG_ERROR("Preset not found: " + presetName);
            return false;
        }
        
        const ConfigPreset& preset = m_presets[presetName];
        
        // Apply preset values
        for (const auto& pair : preset.values) {
            set(pair.first, pair.second, ConfigSource::Preset);
        }
        
        NLM_LOG_INFO("Loaded preset: " + presetName);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading preset " + presetName + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromTemplate(const std::string& templateName, const std::unordered_map<std::string, std::string>& overrides) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    try {
        if (m_templates.find(templateName) == m_templates.end()) {
            NLM_LOG_ERROR("Template not found: " + templateName);
            return false;
        }
        
        const ConfigTemplate& template = m_templates[templateName];
        
        // Clear existing entries
        clear();
        
        // Apply schema defaults
        for (const auto& schemaEntry : template.schema) {
            setFromSchema(schemaEntry);
        }
        
        // Apply template overrides
        for (const auto& pair : template.defaultOverrides) {
            if (overrides.find(pair.first) == overrides.end()) {
                set(pair.first, pair.second, ConfigSource::Template);
            }
        }
        
        // Apply user overrides
        for (const auto& pair : overrides) {
            set(pair.first, pair.second, ConfigSource::Runtime);
        }
        
        NLM_LOG_INFO("Loaded template: " + templateName);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading template " + templateName + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    try {
        NLM_CHECK_CONDITION(!filepath.empty(), "File path cannot be empty", "filepath");
        
        // Determine file format based on extension
        std::string ext = std::filesystem::path(filepath).extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        
        // Create directory if it doesn't exist
        std::filesystem::path dirPath = std::filesystem::path(filepath).parent_path();
        if (!dirPath.empty() && !std::filesystem::exists(dirPath)) {
            if (!std::filesystem::create_directories(dirPath)) {
                NLM_THROW_FILE("save", filepath, "Cannot create directory for configuration file",
                             FileError::ErrorType::PermissionDenied);
            }
        }
        
        if (ext == ".json") {
            return saveToJSONFile(filepath);
        } else {
            return saveToConfigFile(filepath);
        }
        
    } catch (const FileError& e) {
        NLM_LOG_ERROR(std::string("File error saving configuration: ") + e.what());
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Unexpected error saving configuration: ") + e.what());
        return false;
    }
}

bool Config::validate(const ConfigSchema& schema) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    try {
        // Clear previous validation results
        std::vector<std::string> errors;
        
        // Check required fields
        for (const auto& schemaEntry : schema) {
            if (schemaEntry.required && !has(schemaEntry.key)) {
                errors.push_back("Missing required field: " + schemaEntry.key);
            }
        }
        
        // Validate each entry
        for (const auto& entry : pImpl->entries) {
            // Find corresponding schema entry
            auto schemaIt = std::find_if(schema.begin(), schema.end(),
                                         [&entry](const SchemaEntry& se) { return se.key == entry.key; });
            
            if (schemaIt != schema.end()) {
                // Validate against schema
                if (!validateConfigValue(entry.key, entry.value, *schemaIt)) {
                    errors.push_back("Invalid value for field " + entry.key + ": " + getValidationError(entry.key, entry.value, *schemaIt));
                }
            }
        }
        
        // Log validation results
        if (errors.empty()) {
            NLM_LOG_INFO("Configuration validation passed");
            return true;
        } else {
            NLM_LOG_ERROR("Configuration validation failed with " + std::to_string(errors.size()) + " errors:");
            for (const auto& error : errors) {
                NLM_LOG_ERROR("  " + error);
            }
            return false;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error during validation: " + std::string(e.what()));
        return false;
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    NLM_CHECK_CONDITION(!key.empty(), "Key cannot be empty", "key");
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        NLM_LOG_DEBUG("Configuration key not found: " + key);
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        NLM_LOG_WARNING("Type mismatch for configuration key: " + key);
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    NLM_CHECK_CONDITION(!key.empty(), "Key cannot be empty", "key");
    
    try {
        // Validate value before setting
        if (m_validationEnabled) {
            bool valid = validateConfigValue(key, value);
            if (!valid) {
                NLM_THROW_CONFIG("Invalid configuration value", key);
            }
        }
        
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it != pImpl->entries.end()) {
            it->value = value;
            it->source = source;
        } else {
            ConfigEntry newEntry(key, value, source);
            pImpl->entries.push_back(newEntry);
        }
        
    } catch (const ConfigError& e) {
        NLM_LOG_ERROR(std::string("Configuration error setting key ") + key + ": " + e.what());
        throw;
    }
}

bool Config::setFromSchema(const SchemaEntry& entry) {
    try {
        // Validate schema entry
        NLM_CHECK_CONDITION(!entry.key.empty(), "Schema entry key cannot be empty", "key");
        
        // Set default value if present
        if (entry.defaultValue.index() != static_cast<size_t>(-1)) {
            set(entry.key, entry.defaultValue, ConfigSource::Default);
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error setting from schema: " + std::string(e.what()));
        return false;
    }
}

bool Config::has(const std::string& key) const {
    NLM_CHECK_CONDITION(!key.empty(), "Key cannot be empty", "key");
    
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    NLM_CHECK_CONDITION(!key.empty(), "Key cannot be empty", "key");
    
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
        NLM_CHECK_CONDITION(!entry.key.empty(), "Empty key found in configuration entries", "key");
        keys.push_back(entry.key);
    }
    
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->configFilePath.clear();
    pImpl->sourceMap.clear();
    pImpl->tags.clear();
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
    
    oss << "  Precedence: Environment > CLI > File > Programmatic > Default\n";
    oss << "  Source statistics: " << getStats().totalEntries << " total entries";
    
    return oss.str();
}

std::optional<ConfigEntry> Config::getEntry(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    return *it;
}

std::vector<ConfigDiff> Config::compare(const Config& other) const {
    std::vector<ConfigDiff> diffs;
    
    // Get keys from both configurations
    std::set<std::string> allKeys;
    for (const auto& entry : pImpl->entries) {
        allKeys.insert(entry.key);
    }
    for (const auto& entry : other.pImpl->entries) {
        allKeys.insert(entry.key);
    }
    
    // Compare each key
    for (const auto& key : allKeys) {
        ConfigDiff diff;
        diff.key = key;
        
        auto thisIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                                   [&key](const ConfigEntry& e) { return e.key == key; });
        auto otherIt = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
                                    [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (thisIt == pImpl->entries.end()) {
            diff.isAdded = true;
            if (otherIt != other.pImpl->entries.end()) {
                diff.newValue = otherIt->value;
                diff.newSource = otherIt->source;
            }
        } else if (otherIt == other.pImpl->entries.end()) {
            diff.isRemoved = true;
            diff.oldValue = thisIt->value;
            diff.oldSource = thisIt->source;
        } else {
            // Check if values differ
            if (thisIt->value != otherIt->value || thisIt->source != otherIt->source) {
                diff.isModified = true;
                diff.oldValue = thisIt->value;
                diff.oldSource = thisIt->source;
                diff.newValue = otherIt->value;
                diff.newSource = otherIt->source;
            }
        }
        
        if (diff.isAdded || diff.isRemoved || diff.isModified) {
            diffs.push_back(diff);
        }
    }
    
    return diffs;
}

std::string Config::toJSON() const {
    nlohmann::json j;
    
    for (const auto& entry : pImpl->entries) {
        j[entry.key] = configValueToJSON(entry.value);
        j[entry.key]["_source"] = static_cast<int>(entry.source);
        j[entry.key]["_description"] = entry.description;
        j[entry.key]["_type"] = entry.type;
        if (entry.min) j[entry.key]["_min"] = *entry.min;
        if (entry.max) j[entry.key]["_max"] = *entry.max;
        if (entry.pattern) j[entry.key]["_pattern"] = *entry.pattern;
        if (!entry.choices.empty()) j[entry.key]["_choices"] = entry.choices;
    }
    
    return j.dump(2);
}

bool Config::fromJSON(const std::string& json) {
    try {
        clear();
        
        nlohmann::json j = nlohmann::json::parse(json);
        
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string key = it.key();
            
            // Skip internal fields (starting with _)
            if (key.size() > 0 && key[0] == '_') {
                continue;
            }
            
            nlohmann::json value = it.value();
            ConfigValue configValue = jsonToConfigValue(value);
            
            // Create ConfigEntry
            ConfigEntry entry;
            entry.key = key;
            entry.value = configValue;
            entry.source = ConfigSource::Runtime;
            entry.description = value.contains("_description") ? value["_description"].get<std::string>() : "";
            entry.type = value.contains("_type") ? value["_type"].get<std::string>() : "auto";
            entry.min = value.contains("_min") ? std::optional<double>(value["_min"].get<double>()) : std::nullopt;
            entry.max = value.contains("_max") ? std::optional<double>(value["_max"].get<double>()) : std::nullopt;
            entry.pattern = value.contains("_pattern") ? std::optional<std::string>(value["_pattern"].get<std::string>()) : std::nullopt;
            if (value.contains("_choices")) {
                entry.choices = value["_choices"].get<std::vector<std::string>>();
            }
            
            // Add entry
            pImpl->entries.push_back(entry);
        }
        
        NLM_LOG_INFO("Loaded configuration from JSON (" + std::to_string(pImpl->entries.size()) + " entries)");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error parsing JSON: " + std::string(e.what()));
        return false;
    }
}

std::string Config::getValueType(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return "unknown";
    }
    
    std::visit([this](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) return "integer";
        else if constexpr (std::is_same_v<T, int64_t>) return "integer64";
        else if constexpr (std::is_same_v<T, double>) return "double";
        else if constexpr (std::is_same_v<T, bool>) return "boolean";
        else if constexpr (std::is_same_v<T, std::string>) return "string";
        else if constexpr (std::is_same_v<T, std::vector<int>>) return "integer_array";
        else if constexpr (std::is_same_v<T, std::vector<double>>) return "double_array";
        else if constexpr (std::is_same_v<T, std::vector<std::string>>) return "string_array";
        else return "unknown";
    }, it->value);
    
    return "unknown";
}

bool Config::validatePattern(const std::string& key, const std::string& value) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return false;
    }
    
    if (!it->pattern) {
        return true; // No pattern defined
    }
    
    try {
        std::regex pattern(*it->pattern);
        return std::regex_match(value, pattern);
    } catch (const std::exception&) {
        return false;
    }
}

bool Config::validateRange(const std::string& key, double value) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return false;
    }
    
    if (it->min && value < *it->min) {
        return false;
    }
    
    if (it->max && value > *it->max) {
        return false;
    }
    
    return true;
}

bool Config::validateChoices(const std::string& key, const std::string& value) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end() || !it->choices) {
        return true; // No choices defined
    }
    
    return std::find(it->choices->begin(), it->choices->end(), value) != it->choices->end();
}

std::vector<std::string> Config::getDependencies(const std::string& key) const {
    if (m_dependencies.find(key) != m_dependencies.end()) {
        return m_dependencies.at(key);
    }
    return std::vector<std::string>();
}

bool Config::validateDependencies() const {
    if (!m_dependencyValidationEnabled) {
        return true;
    }
    
    std::vector<std::string> errors;
    
    for (const auto& entry : pImpl->entries) {
        for (const auto& depKey : entry.dependsOn) {
            if (!has(depKey)) {
                errors.push_back("Dependency not satisfied: " + entry.key + " requires " + depKey);
            }
        }
    }
    
    if (!errors.empty()) {
        for (const auto& error : errors) {
            NLM_LOG_ERROR(error);
        }
        return false;
    }
    
    return true;
}

Config::Stats Config::getStats() const {
    Stats stats;
    stats.totalEntries = pImpl->entries.size();
    
    for (const auto& entry : pImpl->entries) {
        switch (entry.source) {
            case ConfigSource::File: stats.fileEntries++; break;
            case ConfigSource::Environment: stats.envEntries++; break;
            case ConfigSource::CommandLine: stats.cliEntries++; break;
            case ConfigSource::Runtime: stats.runtimeEntries++; break;
            case ConfigSource::Preset: stats.presetEntries++; break;
            case ConfigSource::Template: stats.templateEntries++; break;
        }
    }
    
    return stats;
}

void Config::setValidationEnabled(bool enabled) {
    m_validationEnabled = enabled;
}

void Config::setSchemaValidationEnabled(bool enabled) {
    m_schemaValidationEnabled = enabled;
}

void Config::setDependencyValidationEnabled(bool enabled) {
    m_dependencyValidationEnabled = enabled;
}

std::vector<std::string> Config::getAvailablePresets() {
    std::vector<std::string> names;
    for (const auto& pair : m_presets) {
        names.push_back(pair.first);
    }
    return names;
}

std::vector<std::string> Config::getAvailableTemplates() {
    std::vector<std::string> names;
    for (const auto& pair : m_templates) {
        names.push_back(pair.first);
    }
    return names;
}

std::vector<ConfigPreset> Config::getAllPresets() {
    std::vector<ConfigPreset> presets;
    for (const auto& pair : m_presets) {
        presets.push_back(pair.second);
    }
    return presets;
}

std::vector<ConfigTemplate> Config::getAllTemplates() {
    std::vector<ConfigTemplate> templates;
    for (const auto& pair : m_templates) {
        templates.push_back(pair.second);
    }
    return templates;
}

bool Config::addSchema(const SchemaEntry& entry) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    try {
        // Check if schema entry already exists
        auto it = std::find_if(m_schema.begin(), m_schema.end(),
            [&entry](const SchemaEntry& e) { return e.key == entry.key; });
        
        if (it != m_schema.end()) {
            *it = entry;
        } else {
            m_schema.push_back(entry);
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error adding schema: " + std::string(e.what()));
        return false;
    }
}

bool Config::removeSchema(const std::string& key) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = std::find_if(m_schema.begin(), m_schema.end(),
        [&key](const SchemaEntry& e) { return e.key == key; });
    
    if (it == m_schema.end()) {
        return false;
    }
    
    m_schema.erase(it);
    return true;
}

std::vector<SchemaEntry> Config::getSchema() {
    return m_schema;
}

bool Config::loadSchemaFromFile(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    try {
        if (!std::filesystem::exists(filepath)) {
            NLM_LOG_ERROR("Schema file does not exist: " + filepath);
            return false;
        }
        
        // Parse schema file (JSON format)
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Cannot open schema file: " + filepath);
            return false;
        }
        
        nlohmann::json j;
        file >> j;
        
        m_schema.clear();
        
        for (const auto& item : j.items()) {
            std::string key = item.key();
            nlohmann::json value = item.value();
            
            SchemaEntry entry;
            entry.key = key;
            entry.type = value.value("type", "auto");
            entry.required = value.value("required", false);
            entry.defaultValue = jsonToConfigValue(value.value("default", nlohmann::json(nullptr)));
            
            if (value.contains("min")) entry.min = value["min"].get<double>();
            if (value.contains("max")) entry.max = value["max"].get<double>();
            if (value.contains("pattern")) entry.pattern = value["pattern"].get<std::string>();
            if (value.contains("choices")) entry.choices = value["choices"].get<std::vector<std::string>>();
            if (value.contains("description")) entry.description = value["description"].get<std::string>();
            if (value.contains("dependsOn")) entry.dependsOn = value["dependsOn"].get<std::vector<std::string>>();
            
            m_schema.push_back(entry);
        }
        
        NLM_LOG_INFO("Loaded schema from: " + filepath + " (" + std::to_string(m_schema.size()) + " entries)");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading schema: " + std::string(e.what()));
        return false;
    }
}

bool Config::saveSchemaToFile(const std::string& filepath) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    try {
        nlohmann::json j;
        
        for (const auto& entry : m_schema) {
            nlohmann::json schemaEntry;
            schemaEntry["type"] = entry.type;
            schemaEntry["required"] = entry.required;
            if (entry.defaultValue.index() != static_cast<size_t>(-1)) {
                schemaEntry["default"] = configValueToJSON(entry.defaultValue);
            }
            if (entry.min) schemaEntry["min"] = *entry.min;
            if (entry.max) schemaEntry["max"] = *entry.max;
            if (entry.pattern) schemaEntry["pattern"] = *entry.pattern;
            if (!entry.choices.empty()) schemaEntry["choices"] = entry.choices;
            if (!entry.description.empty()) schemaEntry["description"] = entry.description;
            if (!entry.dependsOn.empty()) schemaEntry["dependsOn"] = entry.dependsOn;
            
            j[entry.key] = schemaEntry;
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Cannot open schema file for writing: " + filepath);
            return false;
        }
        
        file << j.dump(2);
        file.flush();
        
        NLM_LOG_INFO("Saved schema to: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving schema: " + std::string(e.what()));
        return false;
    }
}

bool Config::exportConfig(const std::string& filepath, const std::string& format) const {
    if (format == "json" || format == "JSON") {
        return saveToJSONFile(filepath);
    } else {
        return saveToConfigFile(filepath);
    }
}

bool Config::importConfig(const std::string& filepath, const std::string& format) {
    if (format == "json" || format == "JSON") {
        return loadFromJSONFile(filepath);
    } else {
        return loadFromConfigFile(filepath);
    }
}

bool Config::validateConfigValue(const std::string& key, const ConfigValue& value, const SchemaEntry& schema) {
    // Check type matching
    std::string expectedType = schema.type;
    std::string actualType = "unknown";
    
    std::visit([&actualType](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) actualType = "integer";
        else if constexpr (std::is_same_v<T, int64_t>) actualType = "integer64";
        else if constexpr (std::is_same_v<T, double>) actualType = "double";
        else if constexpr (std::is_same_v<T, bool>) actualType = "boolean";
        else if constexpr (std::is_same_v<T, std::string>) actualType = "string";
        else if constexpr (std::is_same_v<T, std::vector<int>>) actualType = "integer_array";
        else if constexpr (std::is_same_v<T, std::vector<double>>) actualType = "double_array";
        else if constexpr (std::is_same_v<T, std::vector<std::string>>) actualType = "string_array";
    }, value);
    
    if (expectedType != "auto" && expectedType != actualType) {
        return false;
    }
    
    // Validate range if specified
    if (schema.min || schema.max) {
        double numericValue = 0.0;
        bool isNumeric = false;
        
        std::visit([&numericValue, &isNumeric](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) { numericValue = arg; isNumeric = true; }
            else if constexpr (std::is_same_v<T, int64_t>) { numericValue = arg; isNumeric = true; }
            else if constexpr (std::is_same_v<T, double>) { numericValue = arg; isNumeric = true; }
            else if constexpr (std::is_same_v<T, float>) { numericValue = arg; isNumeric = true; }
        }, value);
        
        if (isNumeric) {
            if (schema.min && numericValue < *schema.min) return false;
            if (schema.max && numericValue > *schema.max) return false;
        }
    }
    
    return true;
}

bool Config::validateConfigValue(const std::string& key, const std::string& value, const SchemaEntry& schema) {
    try {
        ConfigValue configValue = stringToValue(value, schema.type);
        return validateConfigValue(key, configValue, schema);
    } catch (const std::exception&) {
        return false;
    }
}

std::string Config::getValidationError(const std::string& key, const ConfigValue& value, const SchemaEntry& schema) const {
    std::stringstream error;
    
    // Check type matching
    std::string expectedType = schema.type;
    std::string actualType = "unknown";
    
    std::visit([&actualType](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) actualType = "integer";
        else if constexpr (std::is_same_v<T, int64_t>) actualType = "integer64";
        else if constexpr (std::is_same_v<T, double>) actualType = "double";
        else if constexpr (std::is_same_v<T, bool>) actualType = "boolean";
        else if constexpr (std::is_same_v<T, std::string>) actualType = "string";
        else if constexpr (std::is_same_v<T, std::vector<int>>) actualType = "integer_array";
        else if constexpr (std::is_same_v<T, std::vector<double>>) actualType = "double_array";
        else if constexpr (std::is_same_v<T, std::vector<std::string>>) actualType = "string_array";
    }, value);
    
    if (expectedType != "auto" && expectedType != actualType) {
        error << "Type mismatch for " << key << ": expected " << expectedType << ", got " << actualType;
        return error.str();
    }
    
    // Validate range
    double numericValue = 0.0;
    bool isNumeric = false;
    
    std::visit([&numericValue, &isNumeric](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) { numericValue = arg; isNumeric = true; }
        else if constexpr (std::is_same_v<T, int64_t>) { numericValue = arg; isNumeric = true; }
        else if constexpr (std::is_same_v<T, double>) { numericValue = arg; isNumeric = true; }
        else if constexpr (std::is_same_v<T, float>) { numericValue = arg; isNumeric = true; }
    }, value);
    
    if (isNumeric) {
        if (schema.min && numericValue < *schema.min) {
            error << "Value " << numericValue << " for " << key << " is below minimum " << *schema.min;
            return error.str();
        }
        if (schema.max && numericValue > *schema.max) {
            error << "Value " << numericValue << " for " << key << " is above maximum " << *schema.max;
            return error.str();
        }
    }
    
    return "";
}

void Config::initializeDefaults() {
    // Set up default values
    if (m_sources.find(ConfigSource::Default) == m_sources.end()) {
        std::unordered_map<std::string, ConfigEntry> defaultEntries;
        m_sources[ConfigSource::Default] = defaultEntries;
    }
}

std::string Config::getDefaultConfigFile() {
    return "configs/default.cfg";
}

std::string Config::getDefaultPreset() {
    return "basic";
}

std::string Config::getDefaultTemplate() {
    return "default";
}

} // namespace nlm