#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::string format; // "INI", "JSON", "YAML"
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->format = "INI";
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Detect file format by extension and content
    std::string ext = std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json" || (ext.empty() && detectFormatFromContent(filepath) == "JSON")) {
        return loadFromJSON(filepath);
    } else if (ext == ".yaml" || ext == ".yml" || 
               (ext.empty() && detectFormatFromContent(filepath) == "YAML")) {
        return loadFromYAML(filepath);
    } else {
        // Default to INI format
        pImpl->format = "INI";
        return loadFromINI(filepath);
    }
}

bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    nlohmann::json jsonData;
    try {
        file >> jsonData;
    } catch (const nlohmann::json::parse_error& e) {
        NLM_LOG_ERROR("Failed to parse JSON file: " + std::string(e.what()) + " in " + filepath);
        return false;
    }
    
    pImpl->format = "JSON";
    pImpl->entries.clear();
    
    // Convert JSON to Config entries
    for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
        std::string key = it.key();
        ConfigValue value = jsonToConfigValue(it.value());
        pImpl->entries.emplace_back(key, value, ConfigSource::File);
    }
    
    return true;
}

bool Config::loadFromJSON(const std::string& jsonStr) {
    try {
        nlohmann::json jsonData = nlohmann::json::parse(jsonStr);
        return loadFromJSON(jsonData);
    } catch (const nlohmann::json::parse_error& e) {
        NLM_LOG_ERROR("Failed to parse JSON string: " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromJSON(const nlohmann::json& jsonNode) {
    pImpl->format = "JSON";
    pImpl->entries.clear();
    
    for (auto it = jsonNode.begin(); it != jsonNode.end(); ++it) {
        std::string key = it.key();
        ConfigValue value = jsonToConfigValue(it.value());
        pImpl->entries.emplace_back(key, value, ConfigSource::File);
    }
    
    return true;
}

bool Config::loadFromYAML(const std::string& filepath) {
    try {
        YAML::Node yamlData = YAML::LoadFile(filepath);
        return loadFromYAML(yamlData);
    } catch (const YAML::Exception& e) {
        NLM_LOG_ERROR("Failed to parse YAML file: " + std::string(e.what()) + " in " + filepath);
        return false;
    }
}

bool Config::loadFromYAML(const YAML::Node& yamlNode) {
    pImpl->format = "YAML";
    pImpl->entries.clear();
    
    for (const auto& it : yamlNode) {
        std::string key = it.first.as<std::string>();
        ConfigValue value = yamlToConfigValue(it.second);
        pImpl->entries.emplace_back(key, value, ConfigSource::File);
    }
    
    return true;
}

bool Config::loadFromINI(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    pImpl->format = "INI";
    pImpl->entries.clear();
    
    std::string line;
    while (std::getline(file, line)) {
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
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

std::string Config::detectFormatFromContent(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return "INI";
    }
    
    std::string firstLine;
    if (std::getline(file, firstLine)) {
        // Check for JSON array/object or YAML key-value pairs
        if ((firstLine.find('{') != std::string::npos && firstLine.find('}') != std::string::npos) ||
            (firstLine.find('[') != std::string::npos && firstLine.find(']') != std::string::npos)) {
            return "JSON";
        } else if (firstLine.find(':') != std::string::npos && firstLine.find('-') != std::string::npos) {
            return "YAML";
        }
    }
    
    return "INI";
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
    
    if (pImpl->format == "JSON") {
        nlohmann::json jsonData;
        for (const auto& entry : pImpl->entries) {
            jsonData[entry.key] = configValueToJSON(entry.value);
        }
        file << jsonData.dump(2);
    } else if (pImpl->format == "YAML") {
        YAML::Emitter emitter;
        emitter << YAML::BeginDoc;
        for (const auto& entry : pImpl->entries) {
            emitter << YAML::Key << entry.key;
            emitter << YAML::Value << configValueToYAML(entry.value);
        }
        emitter << YAML::EndDoc;
        file << emitter.c_str();
    } else {
        // Default to INI format
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
        }
    }
    
    return true;
}

std::string Config::saveToJSON() const {
    nlohmann::json jsonData;
    for (const auto& entry : pImpl->entries) {
        jsonData[entry.key] = configValueToJSON(entry.value);
    }
    return jsonData.dump(2);
}

std::string Config::saveToYAML() const {
    YAML::Emitter emitter;
    emitter << YAML::BeginDoc;
    for (const auto& entry : pImpl->entries) {
        emitter << YAML::Key << entry.key;
        emitter << YAML::Value << configValueToYAML(entry.value);
    }
    emitter << YAML::EndDoc;
    return emitter.c_str();
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
    if (!validateKey(key)) {
        NLM_LOG_WARNING("Invalid configuration key: " + key);
        return;
    }
    
    if (!validateValue(value, key)) {
        NLM_LOG_WARNING("Invalid configuration value for key: " + key);
        return;
    }
    
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

void Config::set(const std::string& key, const std::vector<int>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<double>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<std::string>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
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
    oss << "Configuration (" << pImpl->entries.size() << " entries, format: " << pImpl->format << "):" << std::endl;
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
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
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << "\"" << arg[i] << "\"";
                }
                oss << "]";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

std::vector<std::string> Config::validate() const {
    std::vector<std::string> errors;
    for (const auto& entry : pImpl->entries) {
        if (!validateKey(entry.key)) {
            errors.push_back("Invalid key: " + entry.key);
        }
        if (!validateValue(entry.value, entry.key)) {
            errors.push_back("Invalid value for key: " + entry.key);
        }
    }
    return errors;
}

bool Config::validateKey(const std::string& key) {
    // Basic validation: key should be alphanumeric with underscores
    if (key.empty()) return false;
    for (char c : key) {
        if (!std::isalnum(c) && c != '_' && c != '-') {
            return false;
        }
    }
    return true;
}

bool Config::validateValue(const ConfigValue& value, const std::string& key) {
    // Basic validation - ensure type matches expected key
    // For simplicity, we'll accept any valid ConfigValue
    return !std::holds_alternative<std::monostate>(value);
}

std::vector<std::string> Config::getDefaultKeys() {
    return {
        "random_seed",
        "simulation_timestep",
        "neuron_count",
        "region_count",
        "connection_probability",
        "stdp_ltp_weight",
        "stdp_ltd_weight",
        "stdp_tau",
        "synaptogenesis_rate",
        "pruning_rate",
        "learning_rate",
        "plasticity_mod",
        "temperature"
    };
}

void Config::merge(const Config& other) {
    for (const auto& entry : other.pImpl->entries) {
        set(entry.key, entry.value, entry.source);
    }
}

bool Config::applyPreset(const std::string& presetName) {
    auto presets = getPresets();
    auto it = presets.find(presetName);
    if (it == presets.end()) {
        NLM_LOG_WARNING("Unknown preset: " + presetName);
        return false;
    }
    
    // Apply preset
    for (const auto& pair : it->second) {
        set(pair.first, pair.second, ConfigSource::Runtime);
    }
    
    NLM_LOG_INFO("Applied preset: " + presetName);
    return true;
}

std::string Config::getFormat() const {
    return pImpl->format;
}

bool Config::validateSchema() const {
    // Basic schema validation - check if all required keys are present
    auto defaultKeys = getDefaultKeys();
    
    for (const auto& key : defaultKeys) {
        if (!has(key)) {
            NLM_LOG_WARNING("Missing required configuration key: " + key);
            return false;
        }
    }
    
    return true;
}

ConfigValue Config::jsonToConfigValue(const nlohmann::json& value) {
    if (value.is_string()) {
        return value.get<std::string>();
    } else if (value.is_number_integer()) {
        return value.get<int>();
    } else if (value.is_number_unsigned()) {
        return value.get<uint64_t>();
    } else if (value.is_number_float()) {
        return value.get<double>();
    } else if (value.is_boolean()) {
        return value.get<bool>();
    } else if (value.is_array()) {
        if (!value.empty()) {
            // Try to infer array type
            if (value[0].is_number_integer()) {
                std::vector<int> arr;
                for (const auto& item : value) {
                    arr.push_back(item.get<int>());
                }
                return arr;
            } else if (value[0].is_number_float()) {
                std::vector<double> arr;
                for (const auto& item : value) {
                    arr.push_back(item.get<double>());
                }
                return arr;
            } else if (value[0].is_string()) {
                std::vector<std::string> arr;
                for (const auto& item : value) {
                    arr.push_back(item.get<std::string>());
                }
                return arr;
            }
        }
    }
    
    // Default to empty string
    return std::string();
}

nlohmann::json Config::configValueToJSON(const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) {
        return std::get<int>(value);
    } else if (std::holds_alternative<int64_t>(value)) {
        return std::get<int64_t>(value);
    } else if (std::holds_alternative<double>(value)) {
        return std::get<double>(value);
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value);
    } else if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        std::vector<int> arr = std::get<std::vector<int>>(value);
        return arr;
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        std::vector<double> arr = std::get<std::vector<double>>(value);
        return arr;
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        std::vector<std::string> arr = std::get<std::vector<std::string>>(value);
        return arr;
    }
    
    // Default to null
    return nullptr;
}

ConfigValue Config::yamlToConfigValue(const YAML::Node& node) {
    if (node.IsScalar()) {
        // Try to infer type from scalar value
        if (node.IsBoolean()) {
            return node.as<bool>();
        } else if (node.IsInteger()) {
            return node.as<int64_t>();
        } else if (node.IsDouble()) {
            return node.as<double>();
        } else {
            return node.as<std::string>();
        }
    } else if (node.IsSequence()) {
        if (!node.empty()) {
            // Try to infer array type
            if (node[0].IsInteger()) {
                std::vector<int> arr;
                for (const auto& item : node) {
                    arr.push_back(item.as<int>());
                }
                return arr;
            } else if (node[0].IsDouble()) {
                std::vector<double> arr;
                for (const auto& item : node) {
                    arr.push_back(item.as<double>());
                }
                return arr;
            } else if (node[0].IsScalar()) {
                std::vector<std::string> arr;
                for (const auto& item : node) {
                    arr.push_back(item.as<std::string>());
                }
                return arr;
            }
        }
    }
    
    // Default to empty string
    return std::string();
}

YAML::Node Config::configValueToYAML(const ConfigValue& value) {
    YAML::Node node;
    if (std::holds_alternative<int>(value)) {
        node = std::get<int>(value);
    } else if (std::holds_alternative<int64_t>(value)) {
        node = std::get<int64_t>(value);
    } else if (std::holds_alternative<double>(value)) {
        node = std::get<double>(value);
    } else if (std::holds_alternative<bool>(value)) {
        node = std::get<bool>(value);
    } else if (std::holds_alternative<std::string>(value)) {
        node = std::get<std::string>(value);
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        std::vector<int> arr = std::get<std::vector<int>>(value);
        for (int item : arr) {
            node.push_back(item);
        }
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        std::vector<double> arr = std::get<std::vector<double>>(value);
        for (double item : arr) {
            node.push_back(item);
        }
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        std::vector<std::string> arr = std::get<std::vector<std::string>>(value);
        for (const auto& item : arr) {
            node.push_back(item);
        }
    }
    
    return node;
}

std::unordered_map<std::string, std::vector<std::pair<std::string, ConfigValue>>> Config::getPresets() {
    return {
        {"fast", {
            {"neuron_count", static_cast<int64_t>(100)},
            {"region_count", static_cast<int64_t>(1)},
            {"connection_probability", 0.05f},
            {"stdp_ltp_weight", 0.01f},
            {"stdp_ltd_weight", 0.01f}
        }},
        {"medium", {
            {"neuron_count", static_cast<int64_t>(500)},
            {"region_count", static_cast<int64_t>(2)},
            {"connection_probability", 0.15f}
        }},
        {"high", {
            {"neuron_count", static_cast<int64_t>(1000)},
            {"region_count", static_cast<int64_t>(4)},
            {"connection_probability", 0.2f},
            {"stdp_ltp_weight", 0.05f},
            {"stdp_ltd_weight", 0.04f}
        }},
        {"research", {
            {"neuron_count", static_cast<int64_t>(2000)},
            {"region_count", static_cast<int64_t>(8)},
            {"connection_probability", 0.3f},
            {"stdp_ltp_weight", 0.02f},
            {"stdp_ltd_weight", 0.015f},
            {"synaptogenesis_rate", 0.0005f},
            {"pruning_rate", 0.00005f}
        }}
    };
}

} // namespace nlm
