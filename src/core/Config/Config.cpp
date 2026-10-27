#include "Config.hpp"
#include "../Logger/Logger.hpp"
#include <algorithm>
#include <sstream>
#include <ctime>
#include <iomanip>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ConfigSchema> schema;
    std::string description;
    
    Impl() : description("NLM Configuration") {}
};

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
    std::string content;
    if (!readFile(filepath, content)) {
        NLM_LOG_ERROR("Failed to read config file: " + filepath);
        return false;
    }
    
    return loadFromString(content);
}

bool Config::loadFromString(const std::string& content) {
    ConfigValidationResult result;
    std::vector<ConfigEntry> entries;
    
    ConfigFormat format = ConfigFormatDetector::detectFormat(content);
    
    switch (format) {
        case ConfigFormat::JSON:
#ifdef JSON_CONFIG_AVAILABLE
            entries = fromJSON(content, result);
#else
            result.addError("JSON support not compiled in");
#endif
            break;
        case ConfigFormat::YAML:
#ifdef YAML_CONFIG_AVAILABLE
            entries = fromYAML(content, result);
#else
            result.addError("YAML support not compiled in");
#endif
            break;
        case ConfigFormat::Ini:
            entries = fromINI(content, result);
            break;
        default:
            result.addError("Unsupported or unrecognized config format");
            break;
    }
    
    if (!result.success) {
        for (const auto& error : result.errors) {
            NLM_LOG_ERROR("Config parse error: " + error);
        }
        return false;
    }
    
    // Merge with existing entries
    pImpl->entries.insert(pImpl->entries.end(), entries.begin(), entries.end());
    
    NLM_LOG_INFO("Loaded " + std::to_string(entries.size()) + " configuration entries");
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    bool success = true;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.find("--config") == 0 || arg.find("-c") == 0) {
            std::string configFile;
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
            if (!configFile.empty()) {
                if (!loadFromFile(configFile)) {
                    NLM_LOG_ERROR("Failed to load config file: " + configFile);
                    success = false;
                }
            }
        } else if (arg.find("--neurons") == 0 || arg.find("-n") == 0) {
            size_t value;
            if (arg.find('=') != std::string::npos) {
                value = std::stoull(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                value = std::stoull(argv[++i]);
            } else {
                NLM_LOG_ERROR("Missing value for --neurons option");
                success = false;
                continue;
            }
            set("neuron_count", static_cast<int64_t>(value));
        } else if (arg.find("--regions") == 0) {
            size_t value;
            if (arg.find('=') != std::string::npos) {
                value = std::stoull(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                value = std::stoull(argv[++i]);
            } else {
                NLM_LOG_ERROR("Missing value for --regions option");
                success = false;
                continue;
            }
            set("region_count", static_cast<int64_t>(value));
        } else if (arg.find("--steps") == 0) {
            size_t value;
            if (arg.find('=') != std::string::npos) {
                value = std::stoull(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                value = std::stoull(argv[++i]);
            } else {
                NLM_LOG_ERROR("Missing value for --steps option");
                success = false;
                continue;
            }
            set("simulation_steps", static_cast<int64_t>(value));
        } else if (arg.find("--timestep") == 0) {
            double value;
            if (arg.find('=') != std::string::npos) {
                value = std::stod(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                value = std::stod(argv[++i]);
            } else {
                NLM_LOG_ERROR("Missing value for --timestep option");
                success = false;
                continue;
            }
            set("simulation_timestep", value);
        } else if (arg.find("--seed") == 0) {
            uint64_t value;
            if (arg.find('=') != std::string::npos) {
                value = std::stoull(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                value = std::stoull(argv[++i]);
            } else {
                NLM_LOG_ERROR("Missing value for --seed option");
                success = false;
                continue;
            }
            set("random_seed", value);
        } else if (arg.find("--stdp-ltp") == 0) {
            float value;
            if (arg.find('=') != std::string::npos) {
                value = std::stof(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                value = std::stof(argv[++i]);
            } else {
                NLM_LOG_ERROR("Missing value for --stdp-ltp option");
                success = false;
                continue;
            }
            set("stdp_ltp_weight", value);
        } else if (arg.find("--stdp-ltd") == 0) {
            float value;
            if (arg.find('=') != std::string::npos) {
                value = std::stof(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                value = std::stof(argv[++i]);
            } else {
                NLM_LOG_ERROR("Missing value for --stdp-ltd option");
                success = false;
                continue;
            }
            set("stdp_ltd_weight", value);
        } else if (arg.find("--stdp-tau") == 0) {
            float value;
            if (arg.find('=') != std::string::npos) {
                value = std::stof(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                value = std::stof(argv[++i]);
            } else {
                NLM_LOG_ERROR("Missing value for --stdp-tau option");
                success = false;
                continue;
            }
            set("stdp_tau", value);
        } else if (arg.find("--synaptogenesis") == 0) {
            float value;
            if (arg.find('=') != std::string::npos) {
                value = std::stof(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                value = std::stof(argv[++i]);
            } else {
                NLM_LOG_ERROR("Missing value for --synaptogenesis option");
                success = false;
                continue;
            }
            set("synaptogenesis_rate", value);
        } else if (arg.find("--pruning") == 0) {
            float value;
            if (arg.find('=') != std::string::npos) {
                value = std::stof(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                value = std::stof(argv[++i]);
            } else {
                NLM_LOG_ERROR("Missing value for --pruning option");
                success = false;
                continue;
            }
            set("pruning_rate", value);
        } else if (arg.find("--plasticity") == 0) {
            std::string value;
            if (arg.find('=') != std::string::npos) {
                value = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                value = argv[++i];
            } else {
                NLM_LOG_ERROR("Missing value for --plasticity option");
                success = false;
                continue;
            }
            set("plasticity_mode", value);
        }
    }
    
    return success;
}

bool Config::saveToFile(const std::string& filepath, ConfigFormat format) const {
    std::string content;
    
    switch (format) {
        case ConfigFormat::JSON:
            content = toJSON(pImpl->entries);
            break;
        case ConfigFormat::YAML:
            content = toYAML(pImpl->entries);
            break;
        case ConfigFormat::Ini:
            content = toINI(pImpl->entries);
            break;
        default:
            NLM_LOG_ERROR("Unsupported config format for saving");
            return false;
    }
    
    return writeFile(filepath, content);
}

std::string Config::saveToString(ConfigFormat format) const {
    switch (format) {
        case ConfigFormat::JSON:
            return toJSON(pImpl->entries);
        case ConfigFormat::YAML:
            return toYAML(pImpl->entries);
        case ConfigFormat::Ini:
            return toINI(pImpl->entries);
        default:
            return "";
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    std::string lowerKey = toLower(key);
    
    for (const auto& entry : pImpl->entries) {
        if (toLower(entry.key) == lowerKey) {
            // Try to convert to type T
            return convertToType<T>(entry.value);
        }
    }
    
    return std::nullopt;
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto value = get<T>(key);
    return value.has_value() ? value.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    std::string lowerKey = toLower(key);
    
    // Check if key already exists
    for (auto& entry : pImpl->entries) {
        if (toLower(entry.key) == lowerKey) {
            entry.value = value;
            entry.source = source;
            NLM_LOG_INFO("Config key updated: " + key);
            return;
        }
    }
    
    // Add new entry
    pImpl->entries.emplace_back(key, value, source);
    NLM_LOG_INFO("Config key added: " + key);
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

ConfigValidationResult Config::validate(const ConfigSchema& schema) const {
    ConfigValidationResult result;
    
    for (const auto& entry : pImpl->entries) {
        // Validate key format
        if (entry.key.empty() || !std::isalpha(entry.key[0])) {
            result.addError("Invalid key name: " + entry.key);
        }
        
        // Validate type if schema is provided
        if (!schema.key.empty()) {
            std::string lowerKey = toLower(entry.key);
            std::string lowerSchemaKey = toLower(schema.key);
            
            if (lowerKey == lowerSchemaKey) {
                if (!validateValueType(entry.value, schema.type)) {
                    result.addError("Type mismatch for key '" + entry.key + "': expected " + schema.type);
                }
                
                if (!validateValueRange(entry.value, schema.type)) {
                    result.addWarning("Value out of recommended range for key '" + entry.key + "'");
                }
            }
        }
    }
    
    return result;
}

bool Config::has(const std::string& key) const {
    std::string lowerKey = toLower(key);
    
    for (const auto& entry : pImpl->entries) {
        if (toLower(entry.key) == lowerKey) {
            return true;
        }
    }
    
    return false;
}

void Config::remove(const std::string& key) {
    std::string lowerKey = toLower(key);
    
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [lowerKey](const ConfigEntry& entry) {
                return toLower(entry.key) == lowerKey;
            }),
        pImpl->entries.end()
    );
    
    NLM_LOG_INFO("Config key removed: " + key);
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    
    return keys;
}

std::vector<ConfigEntry> Config::getEntries() const {
    return pImpl->entries;
}

void Config::clear() {
    pImpl->entries.clear();
    NLM_LOG_INFO("Configuration cleared");
}

std::string Config::summary() const {
    std::stringstream ss;
    ss << "NLM Configuration Summary (" << pImpl->entries.size() << " entries)\n";
    ss << "=" << std::string(40, '=') << "\n";
    
    for (const auto& entry : pImpl->entries) {
        ss << "  " << entry.key << ": ";
        
        // Print value based on type
        if (std::holds_alternative<int>(entry.value)) {
            ss << std::get<int>(entry.value);
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            ss << std::get<int64_t>(entry.value);
        } else if (std::holds_alternative<double>(entry.value)) {
            ss << std::get<double>(entry.value);
        } else if (std::holds_alternative<bool>(entry.value)) {
            ss << (std::get<bool>(entry.value) ? "true" : "false");
        } else if (std::holds_alternative<std::string>(entry.value)) {
            ss << std::get<std::string>(entry.value);
        } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
            const auto& vec = std::get<std::vector<int>>(entry.value);
            ss << "[" << vec.size() << " integers]";
        } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
            const auto& vec = std::get<std::vector<double>>(entry.value);
            ss << "[" << vec.size() << " doubles]";
        } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
            const auto& vec = std::get<std::vector<std::string>>(entry.value);
            ss << "[" << vec.size() << " strings]";
        }
        
        ss << " (" << static_cast<int>(entry.source) << ")\n";
    }
    
    return ss.str();
}

void Config::merge(const Config& other, ConfigSource source) {
    for (const auto& otherEntry : other.pImpl->entries) {
        bool found = false;
        for (auto& thisEntry : pImpl->entries) {
            if (toLower(thisEntry.key) == toLower(otherEntry.key)) {
                thisEntry.value = otherEntry.value;
                thisEntry.source = source;
                found = true;
                break;
            }
        }
        
        if (!found) {
            pImpl->entries.push_back(otherEntry);
        }
    }
    
    NLM_LOG_INFO("Configuration merged");
}

size_t Config::getEntryCount() const {
    return pImpl->entries.size();
}

// Template specialization for type conversion
template<typename T>
std::optional<T> Config::convertToType(const ConfigValue& value) const {
    if constexpr (std::is_same_v<T, int>) {
        if (std::holds_alternative<int>(value)) {
            return std::get<int>(value);
        }
    } else if constexpr (std::is_same_v<T, int64_t>) {
        if (std::holds_alternative<int>(value)) {
            return static_cast<int64_t>(std::get<int>(value));
        } else if (std::holds_alternative<int64_t>(value)) {
            return std::get<int64_t>(value);
        }
    } else if constexpr (std::is_same_v<T, double>) {
        if (std::holds_alternative<int>(value)) {
            return static_cast<double>(std::get<int>(value));
        } else if (std::holds_alternative<double>(value)) {
            return std::get<double>(value);
        }
    } else if constexpr (std::is_same_v<T, bool>) {
        if (std::holds_alternative<bool>(value)) {
            return std::get<bool>(value);
        }
    } else if constexpr (std::is_same_v<T, std::string>) {
        if (std::holds_alternative<std::string>(value)) {
            return std::get<std::string>(value);
        }
    }
    
    return std::nullopt;
}

// Static helper methods
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

std::string Config::getFileExtension(const std::string& filename) {
    size_t dotPos = filename.find_last_of('.');
    if (dotPos == std::string::npos) return "";
    return filename.substr(dotPos + 1);
}

bool Config::validateValueType(const ConfigValue& value, const std::string& type) noexcept {
    if (type == "int" && std::holds_alternative<int>(value)) return true;
    if (type == "int64_t" && std::holds_alternative<int64_t>(value)) return true;
    if (type == "double" && (std::holds_alternative<int>(value) || std::holds_alternative<double>(value))) return true;
    if (type == "bool" && std::holds_alternative<bool>(value)) return true;
    if (type == "string" && std::holds_alternative<std::string>(value)) return true;
    if (type == "vector<int>" && std::holds_alternative<std::vector<int>>(value)) return true;
    if (type == "vector<double>" && std::holds_alternative<std::vector<double>>(value)) return true;
    if (type == "vector<string>" && std::holds_alternative<std::vector<std::string>>(value)) return true;
    
    return false;
}

bool Config::validateValueRange(const ConfigValue& value, const std::string& type) const noexcept {
    // Basic range validation
    if (type == "int" && std::holds_alternative<int>(value)) {
        int val = std::get<int>(value);
        return val >= -1000000 && val <= 1000000;
    } else if (type == "double" && std::holds_alternative<double>(value)) {
        double val = std::get<double>(value);
        return val >= -1000000.0 && val <= 1000000.0;
    } else if (type == "bool" && std::holds_alternative<bool>(value)) {
        return true;  // Booleans are always valid
    }
    
    return true; // For other types, accept any value
}

bool Config::readFile(const std::string& filepath, std::string& content) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    content.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return !content.empty();
}

bool Config::writeFile(const std::string& filepath, const std::string& content) noexcept {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    return file.good();
}

std::string Config::toJSON(const std::vector<ConfigEntry>& entries) noexcept {
    std::stringstream ss;
    ss << "{\n";
    
    for (size_t i = 0; i < entries.size(); ++i) {
        const auto& entry = entries[i];
        ss << "  \"" << entry.key << "\": ";
        
        // Convert value to JSON
        if (std::holds_alternative<int>(entry.value)) {
            ss << std::get<int>(entry.value);
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            ss << std::get<int64_t>(entry.value);
        } else if (std::holds_alternative<double>(entry.value)) {
            ss << std::get<double>(entry.value);
        } else if (std::holds_alternative<bool>(entry.value)) {
            ss << (std::get<bool>(entry.value) ? "true" : "false");
        } else if (std::holds_alternative<std::string>(entry.value)) {
            ss << "\"" << std::get<std::string>(entry.value) << "\"";
        } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
            const auto& vec = std::get<std::vector<int>>(entry.value);
            ss << "[";
            for (size_t j = 0; j < vec.size(); ++j) {
                ss << vec[j];
                if (j < vec.size() - 1) ss << ", ";
            }
            ss << "]";
        } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
            const auto& vec = std::get<std::vector<double>>(entry.value);
            ss << "[";
            for (size_t j = 0; j < vec.size(); ++j) {
                ss << vec[j];
                if (j < vec.size() - 1) ss << ", ";
            }
            ss << "]";
        } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
            const auto& vec = std::get<std::vector<std::string>>(entry.value);
            ss << "[";
            for (size_t j = 0; j < vec.size(); ++j) {
                ss << "\"" << vec[j] << "\"";
                if (j < vec.size() - 1) ss << ", ";
            }
            ss << "]";
        }
        
        if (i < entries.size() - 1) ss << ",\n";
    }
    
    ss << "\n}\n";
    return ss.str();
}

std::string Config::toYAML(const std::vector<ConfigEntry>& entries) noexcept {
    std::stringstream ss;
    ss << "# NLM Configuration File\n";
    ss << "# Generated on " << std::ctime(&std::time(nullptr));
    ss << "\n";
    
    for (const auto& entry : entries) {
        ss << entry.key << ": ";
        
        // Convert value to YAML
        if (std::holds_alternative<int>(entry.value)) {
            ss << std::get<int>(entry.value);
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            ss << std::get<int64_t>(entry.value);
        } else if (std::holds_alternative<double>(entry.value)) {
            ss << std::get<double>(entry.value);
        } else if (std::holds_alternative<bool>(entry.value)) {
            ss << (std::get<bool>(entry.value) ? "true" : "false");
        } else if (std::holds_alternative<std::string>(entry.value)) {
            ss << "\"" << std::get<std::string>(entry.value) << "\"";
        } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
            const auto& vec = std::get<std::vector<int>>(entry.value);
            ss << "[";
            for (size_t j = 0; j < vec.size(); ++j) {
                ss << vec[j];
                if (j < vec.size() - 1) ss << ", ";
            }
            ss << "]";
        } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
            const auto& vec = std::get<std::vector<double>>(entry.value);
            ss << "[";
            for (size_t j = 0; j < vec.size(); ++j) {
                ss << vec[j];
                if (j < vec.size() - 1) ss << ", ";
            }
            ss << "]";
        } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
            const auto& vec = std::get<std::vector<std::string>>(entry.value);
            ss << "[";
            for (size_t j = 0; j < vec.size(); ++j) {
                ss << "\"" << vec[j] << "\"";
                if (j < vec.size() - 1) ss << ", ";
            }
            ss << "]";
        }
        ss << "\n";
    }
    
    return ss.str();
}

std::string Config::toINI(const std::vector<ConfigEntry>& entries) noexcept {
    std::stringstream ss;
    ss << "# NLM Configuration File\n";
    ss << "# Generated on " << std::ctime(&std::time(nullptr));
    ss << "\n[General]\n";
    
    for (const auto& entry : entries) {
        ss << entry.key << " = ";
        
        // Convert value to INI format
        if (std::holds_alternative<int>(entry.value)) {
            ss << std::get<int>(entry.value);
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            ss << std::get<int64_t>(entry.value);
        } else if (std::holds_alternative<double>(entry.value)) {
            ss << std::get<double>(entry.value);
        } else if (std::holds_alternative<bool>(entry.value)) {
            ss << (std::get<bool>(entry.value) ? "true" : "false");
        } else if (std::holds_alternative<std::string>(entry.value)) {
            ss << std::get<std::string>(entry.value);
        }
        ss << "\n";
    }
    
    return ss.str();
}

std::vector<ConfigEntry> Config::fromJSON(const std::string& jsonStr, ConfigValidationResult& result) {
    std::vector<ConfigEntry> entries;
    
#ifdef JSON_CONFIG_AVAILABLE
    try {
        auto j = nlohmann::json::parse(jsonStr);
        
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string key = it.key();
            
            std::string type;
            ConfigValue value;
            
            if (it->is_number_integer()) {
                type = "int";
                value = it->get<int>();
            } else if (it->is_number_unsigned()) {
                type = "uint64_t";
                value = it->get<uint64_t>();
            } else if (it->is_number_float()) {
                type = "double";
                value = it->get<double>();
            } else if (it->is_boolean()) {
                type = "bool";
                value = it->get<bool>();
            } else if (it->is_string()) {
                type = "string";
                value = it->get<std::string>();
            } else if (it->is_array()) {
                auto arr = it->get<std::vector<nlohmann::json>>();
                if (!arr.empty()) {
                    if (arr[0].is_number_integer()) {
                        type = "vector<int>";
                        value = arr.get<std::vector<int>>();
                    } else if (arr[0].is_number_float()) {
                        type = "vector<double>";
                        value = arr.get<std::vector<double>>();
                    } else if (arr[0].is_string()) {
                        type = "vector<string>";
                        value = arr.get<std::vector<std::string>>();
                    }
                }
            }
            
            if (!type.empty()) {
                entries.emplace_back(key, value, ConfigSource::File, "", type);
            } else {
                result.addError("Unsupported JSON type for key: " + key);
            }
        }
        
    } catch (const nlohmann::json::parse_error& e) {
        result.addError("JSON parse error: " + std::string(e.what()));
    } catch (const std::exception& e) {
        result.addError("Error parsing JSON: " + std::string(e.what()));
    }
#endif
    
    return entries;
}

std::vector<ConfigEntry> Config::fromYAML(const std::string& yamlStr, ConfigValidationResult& result) {
    std::vector<ConfigEntry> entries;
    
#ifdef YAML_CONFIG_AVAILABLE
    try {
        auto root = YAML::Load(yamlStr);
        
        for (auto it = root.begin(); it != root.end(); ++it) {
            std::string key = it->first.as<std::string>();
            
            std::string type;
            ConfigValue value;
            
            if (it->second.IsScalar()) {
                std::string strVal = it->second.as<std::string>();
                
                if (strVal == "true" || strVal == "false") {
                    type = "bool";
                    value = (strVal == "true");
                } else {
                    try {
                        if (strVal.find('.') != std::string::npos) {
                            type = "double";
                            value = std::stod(strVal);
                        } else {
                            type = "int";
                            value = std::stoi(strVal);
                        }
                    } catch (...) {
                        type = "string";
                        value = strVal;
                    }
                }
            } else if (it->second.IsSequence()) {
                auto seq = it->second.as<std::vector<YAML::Node>>();
                if (!seq.empty()) {
                    if (seq[0].IsScalar()) {
                        std::string firstVal = seq[0].as<std::string>();
                        if (firstVal.find('.') != std::string::npos) {
                            type = "vector<double>";
                            std::vector<double> doubleVec;
                            for (const auto& node : seq) {
                                doubleVec.push_back(node.as<double>());
                            }
                            value = doubleVec;
                        } else {
                            type = "vector<int>";
                            std::vector<int> intVec;
                            for (const auto& node : seq) {
                                intVec.push_back(node.as<int>());
                            }
                            value = intVec;
                        }
                    }
                }
            }
            
            if (!type.empty()) {
                entries.emplace_back(key, value, ConfigSource::File, "", type);
            } else {
                result.addError("Unsupported YAML type for key: " + key);
            }
        }
        
    } catch (const YAML::Exception& e) {
        result.addError("YAML parse error: " + std::string(e.what()));
    } catch (const std::exception& e) {
        result.addError("Error parsing YAML: " + std::string(e.what()));
    }
#endif
    
    return entries;
}

std::vector<ConfigEntry> Config::fromINI(const std::string& iniStr, ConfigValidationResult& result) {
    std::vector<ConfigEntry> entries;
    
    std::istringstream stream(iniStr);
    std::string line;
    std::string currentSection = "General";
    
    while (std::getline(stream, line)) {
        // Remove whitespace
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line.find('#') == 0 || line.find(';') == 0) {
            continue;
        }
        
        // Check for section headers
        if (line.find('[') == 0 && line.find(']') != std::string::npos) {
            currentSection = line.substr(1, line.find(']') - 1);
            continue;
        }
        
        // Parse key = value
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            result.addError("Invalid INI line: " + line);
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string valueStr = trim(line.substr(pos + 1));
        
        // Determine type and convert value
        std::string type = "string";
        ConfigValue value = valueStr;
        
        // Try to convert to numeric types
        try {
            if (valueStr.find('.') != std::string::npos) {
                double doubleVal = std::stod(valueStr);
                type = "double";
                value = doubleVal;
            } else {
                int intVal = std::stoi(valueStr);
                type = "int";
                value = intVal;
            }
        } catch (...) {
            // Try boolean
            if (valueStr == "true" || valueStr == "false") {
                type = "bool";
                value = (valueStr == "true");
            }
        }
        
        // Add entry with section prefix
        std::string fullKey = currentSection + "." + key;
        entries.emplace_back(fullKey, value, ConfigSource::File, "", type);
    }
    
    return entries;
}

} // namespace nlm
