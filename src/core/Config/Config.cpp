#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <rapidjson/prettywriter.h>
#include <memory>

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
ConfigValue parseJSONValue(const rapidjson::Value& value) {
    if (value.IsInt()) {
        return value.GetInt();
    } else if (value.IsInt64()) {
        return value.GetInt64();
    } else if (value.IsUint64()) {
        return static_cast<int64_t>(value.GetUint64());
    } else if (value.IsDouble()) {
        return value.GetDouble();
    } else if (value.IsBool()) {
        return value.GetBool();
    } else if (value.IsString()) {
        return std::string(value.GetString());
    } else if (value.IsArray()) {
        // Handle arrays
        std::vector<double> doubleArray;
        std::vector<int> intArray;
        std::vector<std::string> stringArray;
        
        for (const auto& item : value.GetArray()) {
            if (item.IsInt()) {
                intArray.push_back(item.GetInt());
            } else if (item.IsDouble()) {
                doubleArray.push_back(item.GetDouble());
            } else if (item.IsString()) {
                stringArray.push_back(std::string(item.GetString()));
            }
        }
        
        // Prefer most specific type
        if (!doubleArray.empty()) {
            return doubleArray;
        } else if (!intArray.empty()) {
            return intArray;
        } else if (!stringArray.empty()) {
            return stringArray;
        }
        
        return std::vector<double>{}; // empty array
    }
    
    return 0; // default for unknown types
}

// Helper to convert ConfigValue to JSON
void writeJSONValue(rapidjson::Writer<rapidjson::StringBuffer>& writer, const ConfigValue& value) {
    std::visit([&writer](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            writer.Int(arg);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            writer.Int64(arg);
        } else if constexpr (std::is_same_v<T, double>) {
            writer.Double(arg);
        } else if constexpr (std::is_same_v<T, bool>) {
            writer.Bool(arg);
        } else if constexpr (std::is_same_v<T, std::string>) {
            writer.String(arg.c_str());
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            writer.StartArray();
            for (int val : arg) writer.Int(val);
            writer.EndArray();
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            writer.StartArray();
            for (double val : arg) writer.Double(val);
            writer.EndArray();
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            writer.StartArray();
            for (const std::string& val : arg) writer.String(val.c_str());
            writer.EndArray();
        }
    }, value);
}

// Enhanced JSON parser with better error handling
bool parseJSONFile(const std::string& filepath, std::vector<ConfigEntry>& entries) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    // Try to parse as JSON
    rapidjson::Document doc;
    doc.ParseStream<rapidjson::kParseCommentsFlag>(file);
    
    if (doc.HasParseError()) {
        NLM_LOG_ERROR("JSON parse error in config file " + filepath + ": " + 
                     std::string(rapidjson::GetParseError_En(doc.GetParseError())) + 
                     " at offset " + std::to_string(doc.GetErrorOffset()));
        return false;
    }
    
    if (!doc.IsObject()) {
        NLM_LOG_ERROR("Invalid JSON format in config file " + filepath + ": root must be an object");
        return false;
    }
    
    // Parse JSON object
    for (auto it = doc.MemberBegin(); it != doc.MemberEnd(); ++it) {
        std::string key = it->name.GetString();
        ConfigValue value = parseJSONValue(it->value);
        
        // Generate simple description
        std::string description = "Configuration value for " + key;
        
        entries.emplace_back(key, value, ConfigSource::File, description);
    }
    
    NLM_LOG_INFO("Successfully loaded JSON config from " + filepath + 
                " (" + std::to_string(entries.size()) + " entries)");
    return true;
}

// Write config as JSON
bool writeJSONFile(const std::string& filepath, const std::vector<ConfigEntry>& entries) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to save config to file: " + filepath);
        return false;
    }
    
    rapidjson::Document doc;
    doc.SetObject();
    auto& allocator = doc.GetAllocator();
    
    for (const auto& entry : entries) {
        rapidjson::Value key(key.c_str(), allocator);
        
        rapidjson::Value value;
        std::visit([&value, &allocator](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                value.SetInt(arg);
            } else if constexpr (std::is_same_v<T, int64_t>) {
                value.SetInt64(arg);
            } else if constexpr (std::is_same_v<T, double>) {
                value.SetDouble(arg);
            } else if constexpr (std::is_same_v<T, bool>) {
                value.SetBool(arg);
            } else if constexpr (std::is_same_v<T, std::string>) {
                value.SetString(arg.c_str(), allocator);
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                value.SetArray();
                for (int val : arg) {
                    value.PushBack(val, allocator);
                }
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                value.SetArray();
                for (double val : arg) {
                    value.PushBack(val, allocator);
                }
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                value.SetArray();
                for (const std::string& val : arg) {
                    rapidjson::Value str(val.c_str(), allocator);
                    value.PushBack(str, allocator);
                }
            }
        }, entry.value);
        
        doc.AddMember(key, value, allocator);
    }
    
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    writer.SetIndent(' ', 2);
    
    doc.Accept(writer);
    
    file << buffer.GetString();
    NLM_LOG_INFO("Config saved as JSON to file: " + filepath + 
                " (" + std::to_string(entries.size()) + " entries)");
    return true;
}

} // namespace

bool Config::loadFromFile(const std::string& filepath) {
    // Clear existing entries
    clear();
    
    // Try JSON parsing first
    if (parseJSONFile(filepath, pImpl->entries)) {
        return true;
    }
    
    // Fall back to simple key=value format
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
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
            std::string valueStr = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (valueStr.size() >= 2 && 
                ((valueStr.front() == '"' && valueStr.back() == '"') ||
                 (valueStr.front() == '\'' && valueStr.back() == '\''))) {
                valueStr = valueStr.substr(1, valueStr.size() - 2);
            }
            
            // Parse value based on content
            ConfigValue value;
            bool parsed = false;
            
            // Try boolean
            if (valueStr == "true" || valueStr == "false") {
                value = (valueStr == "true");
                parsed = true;
            }
            // Try integer
            else {
                try {
                    size_t posEnd;
                    int64_t intVal = std::stoll(valueStr, &posEnd);
                    if (posEnd == valueStr.size()) {
                        value = intVal;
                        parsed = true;
                    }
                } catch (...) {}
            }
            
            if (!parsed) {
                try {
                    size_t posEnd;
                    double doubleVal = std::stod(valueStr, &posEnd);
                    if (posEnd == valueStr.size()) {
                        value = doubleVal;
                        parsed = true;
                    }
                } catch (...) {}
            }
            
            if (!parsed) {
                // Treat as string
                value = valueStr;
                parsed = true;
            }
            
            if (parsed) {
                set(key, value, ConfigSource::File);
            } else {
                NLM_LOG_WARNING("Could not parse value in config file " + filepath + 
                              " at line " + std::to_string(lineNumber) + ": " + valueStr);
            }
        } else {
            NLM_LOG_WARNING("Invalid config line in file " + filepath + 
                          " at line " + std::to_string(lineNumber) + ": " + line);
        }
    }
    
    NLM_LOG_INFO("Loaded configuration from: " + filepath + 
                " (" + std::to_string(pImpl->entries.size()) + " entries)");
    return !pImpl->entries.empty();
}

bool Config::loadFromArgs(int argc, char** argv) {
    // Define supported command line options
    std::map<std::string, ConfigSource> supportedOptions = {
        {"--config", ConfigSource::File},
        {"-config", ConfigSource::File},
        {"--neuron-count", ConfigSource::CommandLine},
        {"--region-count", ConfigSource::CommandLine},
        {"--connection-probability", ConfigSource::CommandLine},
        {"--random-seed", ConfigSource::CommandLine},
        {"--simulation-timestep", ConfigSource::CommandLine},
        {"--stdp-ltp-weight", ConfigSource::CommandLine},
        {"--stdp-ltd-weight", ConfigSource::CommandLine},
        {"--stdp-tau", ConfigSource::CommandLine},
        {"--synaptogenesis-rate", ConfigSource::CommandLine},
        {"--pruning-rate", ConfigSource::CommandLine},
        {"--dopamine-baseline", ConfigSource::CommandLine},
        {"--reward-discount-factor", ConfigSource::CommandLine},
        {"--log-level", ConfigSource::CommandLine},
        {"--max-simulation-steps", ConfigSource::CommandLine},
        {"--enable-plasticity", ConfigSource::CommandLine},
        {"--enable-neuromodulation", ConfigSource::CommandLine},
        {"--enable-development", ConfigSource::CommandLine},
        {"--enable-curiosity", ConfigSource::CommandLine},
        {"--enable-reward-modulation", ConfigSource::CommandLine},
        {"--world-width", ConfigSource::CommandLine},
        {"--world-height", ConfigSource::CommandLine},
    };
    
    // Map command line option names to config keys
    std::map<std::string, std::string> optionToKey = {
        {"--config", "config-file"},
        {"-config", "config-file"},
        {"--neuron-count", "neuron_count"},
        {"--region-count", "region_count"},
        {"--connection-probability", "connection_probability"},
        {"--random-seed", "random_seed"},
        {"--simulation-timestep", "simulation_timestep"},
        {"--stdp-ltp-weight", "stdp_ltp_weight"},
        {"--stdp-ltd-weight", "stdp_ltd_weight"},
        {"--stdp-tau", "stdp_tau"},
        {"--synaptogenesis-rate", "synaptogenesis_rate"},
        {"--pruning-rate", "pruning_rate"},
        {"--dopamine-baseline", "dopamine_baseline"},
        {"--reward-discount-factor", "reward_discount_factor"},
        {"--log-level", "log_level"},
        {"--max-simulation-steps", "max_simulation_steps"},
        {"--enable-plasticity", "enable_plasticity"},
        {"--enable-neuromodulation", "enable_neuromodulation"},
        {"--enable-development", "enable_development"},
        {"--enable-curiosity", "enable_curiosity"},
        {"--enable-reward-modulation", "enable_reward_modulation"},
        {"--world-width", "world_width"},
        {"--world-height", "world_height"},
    };
    
    bool hasErrors = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        // Handle --key=value format
        if (arg.find('=') != std::string::npos) {
            size_t pos = arg.find('=');
            std::string keyPrefix = arg.substr(0, pos);
            std::string value = arg.substr(pos + 1);
            
            // Normalize key (remove leading dashes)
            std::string normalizedKey = keyPrefix;
            if (normalizedKey.find("--") == 0) {
                normalizedKey = normalizedKey.substr(2);
            } else if (normalizedKey.find('-') == 0) {
                normalizedKey = normalizedKey.substr(1);
            }
            
            // Convert to underscore notation
            for (char& c : normalizedKey) {
                if (c == '-') c = '_';
            }
            
            // Check if it's a supported option
            if (supportedOptions.find("--" + normalizedKey) != supportedOptions.end() ||
                supportedOptions.find("-" + normalizedKey) != supportedOptions.end()) {
                
                // Convert to actual config key
                std::string configKey = "";
                if (optionToKey.find("--" + normalizedKey) != optionToKey.end()) {
                    configKey = optionToKey["--" + normalizedKey];
                } else if (optionToKey.find("-" + normalizedKey) != optionToKey.end()) {
                    configKey = optionToKey["-" + normalizedKey];
                }
                
                if (!configKey.empty()) {
                    set(configKey, value, supportedOptions["--" + normalizedKey]);
                } else {
                    NLM_LOG_WARNING("Unknown command line option: " + keyPrefix);
                    hasErrors = true;
                }
            } else {
                // Treat as simple config file
                set("config-file", arg, ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            
            // Convert hyphenated key to underscore notation
            std::string configKey = "";
            for (char& c : key) {
                if (c == '-') c = '_';
            }
            
            // Map to actual config key
            if (optionToKey.find("--" + key) != optionToKey.end()) {
                configKey = optionToKey["--" + key];
            } else if (optionToKey.find("-" + key) != optionToKey.end()) {
                configKey = optionToKey["-" + key];
            } else if (optionToKey.find("--" + key) != optionToKey.end()) {
                // Already converted
                configKey = optionToKey["--" + key];
            }
            
            if (!configKey.empty()) {
                std::string value = argv[++i];
                set(configKey, value, ConfigSource::CommandLine);
            } else {
                NLM_LOG_WARNING("Unknown command line option: -" + key);
                hasErrors = true;
                i++; // Skip next argument
            }
        }
        // Handle boolean flags
        else if (arg == "--enable-plasticity" || arg == "--plasticity" ||
                 arg == "--enable-neuromodulation" || arg == "--neuromodulation" ||
                 arg == "--enable-development" || arg == "--development" ||
                 arg == "--enable-curiosity" || arg == "--curiosity" ||
                 arg == "--enable-reward-modulation" || arg == "--reward-modulation") {
            
            // Convert flag to config key
            std::string configKey = "";
            if (arg == "--enable-plasticity" || arg == "--plasticity") {
                configKey = "enable_plasticity";
            } else if (arg == "--enable-neuromodulation" || arg == "--neuromodulation") {
                configKey = "enable_neuromodulation";
            } else if (arg == "--enable-development" || arg == "--development") {
                configKey = "enable_development";
            } else if (arg == "--enable-curiosity" || arg == "--curiosity") {
                configKey = "enable_curiosity";
            } else if (arg == "--enable-reward-modulation" || arg == "--reward-modulation") {
                configKey = "enable_reward_modulation";
            }
            
            if (!configKey.empty()) {
                set(configKey, true, ConfigSource::CommandLine);
            }
        }
        // Handle disable flags
        else if (arg == "--disable-plasticity" || arg == "--no-plasticity" ||
                 arg == "--disable-neuromodulation" || arg == "--no-neuromodulation" ||
                 arg == "--disable-development" || arg == "--no-development" ||
                 arg == "--disable-curiosity" || arg == "--no-curiosity" ||
                 arg == "--disable-reward-modulation" || arg == "--no-reward-modulation") {
            
            std::string configKey = "";
            if (arg == "--disable-plasticity" || arg == "--no-plasticity") {
                configKey = "enable_plasticity";
            } else if (arg == "--disable-neuromodulation" || arg == "--no-neuromodulation") {
                configKey = "enable_neuromodulation";
            } else if (arg == "--disable-development" || arg == "--no-development") {
                configKey = "enable_development";
            } else if (arg == "--disable-curiosity" || arg == "--no-curiosity") {
                configKey = "enable_curiosity";
            } else if (arg == "--disable-reward-modulation" || arg == "--no-reward-modulation") {
                configKey = "enable_reward_modulation";
            }
            
            if (!configKey.empty()) {
                set(configKey, false, ConfigSource::CommandLine);
            }
        }
        else {
            NLM_LOG_WARNING("Unknown command line argument: " + arg);
            hasErrors = true;
        }
    }
    
    if (hasErrors) {
        NLM_LOG_WARNING("Some command line arguments were ignored");
    }
    
    NLM_LOG_INFO("Processed command line arguments");
    return !hasErrors;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to save config to file: " + filepath);
        return false;
    }
    
    // Write comments and values
    for (const auto& entry : pImpl->entries) {
        if (!entry.description.empty()) {
            file << "# " << entry.description << "\n";
        }
        
        file << entry.key << " = ";
        
        // Write value based on type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else {
                file << arg;
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    NLM_LOG_INFO("Config saved to file: " + filepath + " (" + std::to_string(pImpl->entries.size()) + " entries)");
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

// Type-safe configuration access implementation
template<typename T>
T Config::getRequired(const std::string& key, const T& defaultValue, 
                     const ConfigValidationCallback& validator) const {
    auto value = get<T>(key);
    
    if (!value.has_value()) {
        NLM_LOG_WARNING("Using default value for missing key: " + key + 
                       " (default: " + std::to_string(defaultValue) + ")");
        return defaultValue;
    }
    
    // Apply custom validator if provided
    if (validator && !validator(key, ConfigValue(value.value()))) {
        NLM_LOG_WARNING("Validator rejected value for key: " + key + ". Using default.");
        return defaultValue;
    }
    
    return value.value();
}

// Enhanced set method with description
void Config::set(const std::string& key, const ConfigValue& value, 
                 const std::string& description, ConfigSource source) {
    // Find existing entry
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        // Update existing entry, preserve source unless explicitly specified
        it->value = value;
        if (source != ConfigSource::Runtime) {
            it->source = source;
        }
        if (!description.empty()) {
            it->description = description;
        }
    } else {
        // Create new entry
        pImpl->entries.emplace_back(key, value, source, description);
    }
}

// Specialized template for vector types
void Config::set(const std::string& key, const std::vector<int>& values,
                 const std::string& description, ConfigSource source) {
    set(key, ConfigValue(values), description, source);
}

void Config::set(const std::string& key, const std::vector<double>& values,
                 const std::string& description, ConfigSource source) {
    set(key, ConfigValue(values), description, source);
}

void Config::set(const std::string& key, const std::vector<std::string>& values,
                 const std::string& description, ConfigSource source) {
    set(key, ConfigValue(values), description, source);
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

// Validation system implementation
bool Config::isComplete() const {
    for (const auto& entry : pImpl->entries) {
        if (entry.required) {
            if (!has(entry.key)) {
                NLM_LOG_ERROR("Missing required configuration key: " + entry.key);
                return false;
            }
        }
    }
    return true;
}

std::string Config::getStats() const {
    std::ostringstream oss;
    oss << "Configuration Statistics:" << std::endl;
    oss << "  Total entries: " << pImpl->entries.size() << std::endl;
    
    std::map<ConfigSource, int> sourceCount;
    for (const auto& entry : pImpl->entries) {
        sourceCount[entry.source]++;
    }
    
    oss << "  Entries by source:" << std::endl;
    for (const auto& pair : sourceCount) {
        oss << "    " << static_cast<int>(pair.first) << ": " << pair.second << std::endl;
    }
    
    if (isComplete()) {
        oss << "  Status: COMPLETE" << std::endl;
    } else {
        oss << "  Status: INCOMPLETE (missing required keys)" << std::endl;
    }
    
    return oss.str();
}

bool Config::validate(const ConfigValidationCallback& validator) const {
    // Use provided validator if not null, otherwise use internal validator
    const ConfigValidationCallback& customValidator = validator ? validator : pImpl->validator;
    
    for (const auto& entry : pImpl->entries) {
        if (customValidator) {
            if (!customValidator(entry.key, entry.value)) {
                NLM_LOG_ERROR("Configuration validation failed for key: " + entry.key);
                return false;
            }
        }
        
        // Basic type-based validation
        if (!validateType(entry.key, entry.value)) {
            NLM_LOG_ERROR("Invalid type for configuration key: " + entry.key);
            return false;
        }
    }
    
    return true;
}

void Config::setValidator(const ConfigValidationCallback& validator) {
    pImpl->validator = validator;
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

void Config::logError(const std::string& operation, const std::string& key, 
                     const std::string& message) const {
    NLM_LOG_ERROR("Config " + operation + " for key '" + key + "': " + message);
}

void Config::logWarning(const std::string& operation, const std::string& key,
                        const std::string& message) const {
    NLM_LOG_WARNING("Config " + operation + " for key '" + key + "': " + message);
}

// Type validation
bool Config::validateType(const std::string& key, const ConfigValue& value) const {
    std::string keyLower = toLower(key);
    
    // Special handling for known keys
    if (keyLower == "neuron_count" || keyLower == "region_count") {
        auto intVal = std::get_if<int>(&value);
        return intVal && *intVal > 0;
    } else if (keyLower == "connection_probability" || keyLower == "stdp_ltp_weight" || 
               keyLower == "stdp_ltd_weight" || keyLower == "plasticity_learning_rate" ||
               keyLower == "development_synaptogenesis_rate" || keyLower == "development_pruning_rate" ||
               keyLower == "dopamine_baseline" || keyLower == "reward_discount_factor" ||
               keyLower == "simulation_timestep" || keyLower == "max_simulation_steps") {
        auto doubleVal = std::get_if<double>(&value);
        return doubleVal && *doubleVal >= 0.0;
    } else if (keyLower == "log_level" || keyLower == "environment_name") {
        // These are string values
        return std::get_if<std::string>(&value) != nullptr;
    } else if (keyLower == "log_to_file") {
        auto boolVal = std::get_if<bool>(&value);
        return boolVal != nullptr;
    }
    
    // Default validation - just check that value exists
    return true;
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
