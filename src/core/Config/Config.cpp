#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <cmath>

// JSON and YAML support - try to include external libraries
#ifdef HAS_NLOHMANN_JSON
    #include <nlohmann/json.hpp>
    using json = nlohmann::json;
#else
    #ifdef HAS_YAML_CPP
        #include <yaml-cpp/yaml.h>
        using YAML = YAML::Node;
    #endif
#endif

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::string formatVersion;
    std::string schemaVersion;
    std::string description;
    std::chrono::system_clock::time_point creationTime;
    std::chrono::system_clock::time_point lastModified;
    std::vector<std::string> tags;
    std::map<std::string, std::string> metadata;
    
    Impl() : formatVersion("2.0"), creationTime(std::chrono::system_clock::now()),
             lastModified(std::chrono::system_clock::now()) {
    }
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Set default configuration
    setDefaultConfiguration();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

void Config::setDefaultConfiguration() {
    // Set default configuration values
    set("neuron_count", 1000, ConfigSource::Default);
    set("region_count", 5, ConfigSource::Default);
    set("connection_probability", 0.15, ConfigSource::Default);
    set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    set("pruning_rate", 0.00001f, ConfigSource::Default);
    set("enable_structural_plasticity", true, ConfigSource::Default);
    set("enable_curiosity", true, ConfigSource::Default);
    set("enable_prediction_error", true, ConfigSource::Default);
    set("enable_developmental_stages", true, ConfigSource::Default);
    set("enable_checkpointing", true, ConfigSource::Default);
    set("enable_replay", true, ConfigSource::Default);
    set("learning_rate", 0.01f, ConfigSource::Default);
    set("batch_size", 32, ConfigSource::Default);
    set("max_steps", 1000, ConfigSource::Default);
    set("vision_width", 32, ConfigSource::Default);
    set("vision_height", 32, ConfigSource::Default);
    set("world_width", 64, ConfigSource::Default);
    set("world_height", 64, ConfigSource::Default);
    
    // Set descriptions
    auto entries = getKeys();
    for (const auto& key : entries) {
        auto it = pImpl->entries.begin();
        while (it != pImpl->entries.end()) {
            if (it->key == key) {
                setEntryDescription(*it);
                break;
            }
            ++it;
        }
    }
}

void Config::setEntryDescription(ConfigEntry& entry) const {
    if (entry.key == "neuron_count") {
        entry.description = "Number of neurons in the network";
    } else if (entry.key == "region_count") {
        entry.description = "Number of neural regions";
    } else if (entry.key == "connection_probability") {
        entry.description = "Probability of forming connections between neurons (0.0-1.0)";
    } else if (entry.key == "stdp_ltp_weight") {
        entry.description = "STDP Long-Term Potentiation weight";
    } else if (entry.key == "stdp_ltd_weight") {
        entry.description = "STDP Long-Term Depression weight";
    } else if (entry.key == "synaptogenesis_rate") {
        entry.description = "Rate of new synapse formation";
    } else if (entry.key == "pruning_rate") {
        entry.description = "Rate of synapse pruning";
    } else if (entry.key == "enable_structural_plasticity") {
        entry.description = "Enable structural plasticity (synaptogenesis and pruning)";
    } else if (entry.key == "enable_curiosity") {
        entry.description = "Enable curiosity-driven exploration";
    } else if (entry.key == "enable_prediction_error") {
        entry.description = "Enable prediction error-based learning";
    } else if (entry.key == "enable_developmental_stages") {
        entry.description = "Enable developmental stages and aging";
    } else if (entry.key == "enable_checkpointing") {
        entry.description = "Enable checkpoint saving for state recovery";
    } else if (entry.key == "enable_replay") {
        entry.description = "Enable memory replay and consolidation";
    } else if (entry.key == "learning_rate") {
        entry.description = "Learning rate for plasticity rules";
    } else if (entry.key == "batch_size") {
        entry.description = "Batch size for training";
    } else if (entry.key == "max_steps") {
        entry.description = "Maximum number of simulation steps";
    } else if (entry.key == "vision_width") {
        entry.description = "Vision input width (pixels)";
    } else if (entry.key == "vision_height") {
        entry.description = "Vision input height (pixels)";
    } else if (entry.key == "world_width") {
        entry.description = "World width in units";
    } else if (entry.key == "world_height") {
        entry.description = "World height in units";
    }
}

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Try JSON format first
    try {
        return loadFromJSON(filepath);
    } catch (const std::exception&) {
        // Reset file pointer
        file.clear();
        file.seekg(0, std::ios::beg);
        
        // Try YAML format
        try {
            return loadFromYAML(filepath);
        } catch (const std::exception&) {
            // Fall back to simple key=value format
            file.clear();
            file.seekg(0, std::ios::beg);
            return loadFromSimpleFormat(file);
        }
    }
}

bool Config::loadFromJSON(const std::string& filepath) {
#ifdef HAS_NLOHMANN_JSON
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    json j;
    file >> j;
    
    // Update metadata
    pImpl->lastModified = std::chrono::system_clock::now();
    
    // Extract metadata
    if (j.contains("metadata")) {
        for (const auto& item : j["metadata"].items()) {
            pImpl->metadata[item.key()] = item.value().get<std::string>();
        }
    }
    
    // Extract tags
    if (j.contains("tags")) {
        pImpl->tags = j["tags"].get<std::vector<std::string>>();
    }
    
    // Extract format version
    if (j.contains("format_version")) {
        pImpl->formatVersion = j["format_version"].get<std::string>();
    }
    
    // Extract schema version
    if (j.contains("schema_version")) {
        pImpl->schemaVersion = j["schema_version"].get<std::string>();
    }
    
    // Extract description
    if (j.contains("description")) {
        pImpl->description = j["description"].get<std::string>();
    }
    
    // Load configuration entries
    if (j.contains("configuration")) {
        const auto& config_section = j["configuration"];
        
        for (const auto& item : config_section.items()) {
            std::string key = item.key();
            auto value = item.value();
            
            ConfigValue config_value;
            ConfigSource source = ConfigSource::File;
            
            // Convert JSON value to ConfigValue
            if (value.is_string()) {
                config_value = value.get<std::string>();
            } else if (value.is_number_integer()) {
                config_value = value.get<int>();
            } else if (value.is_number_unsigned()) {
                config_value = value.get<int64_t>();
            } else if (value.is_number_float()) {
                config_value = value.get<double>();
            } else if (value.is_boolean()) {
                config_value = value.get<bool>();
            } else if (value.is_array()) {
                // Handle array types
                std::vector<std::string> string_array = value.get<std::vector<std::string>>();
                // Try to determine array type
                bool all_int = true;
                for (const auto& item : string_array) {
                    try {
                        size_t pos;
                        int int_val = std::stoi(item, pos);
                        if (pos != item.size()) all_int = false;
                    } catch (...) {
                        all_int = false;
                    }
                }
                
                if (all_int) {
                    std::vector<int> int_array;
                    for (const auto& item : string_array) {
                        try {
                            int_array.push_back(std::stoi(item));
                        } catch (...) {
                            int_array.push_back(0);
                        }
                    }
                    config_value = int_array;
                } else {
                    config_value = string_array;
                }
            }
            
            // Set the configuration entry
            auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                [&key](const ConfigEntry& e) { return e.key == key; });n            
            if (it != pImpl->entries.end()) {
                it->value = config_value;
                it->source = source;
            } else {
                pImpl->entries.emplace_back(key, config_value, source);
            }
        }
    }
    
    return true;
#else
    // nlohmann/json not available
    throw std::runtime_error("JSON support not compiled in");
#endif
}

bool Config::loadFromYAML(const std::string& filepath) {
#ifdef HAS_YAML_CPP
    YAML::LoadFile(filepath);
    
    // Update metadata
    pImpl->lastModified = std::chrono::system_clock::now();
    
    // Extract metadata
    if (YAML::Node yaml = YAML::LoadFile(filepath); yaml["metadata"]) {
        for (const auto& item : yaml["metadata"]) {
            pImpl->metadata[item.first.as<std::string>()] = item.second.as<std::string>();
        }
    }
    
    // Extract tags
    if (YAML::Node yaml = YAML::LoadFile(filepath); yaml["tags"]) {
        pImpl->tags = yaml["tags"].as<std::vector<std::string>>();
    }
    
    // Extract format version
    if (YAML::Node yaml = YAML::LoadFile(filepath); yaml["format_version"]) {
        pImpl->formatVersion = yaml["format_version"].as<std::string>();
    }
    
    // Extract schema version
    if (YAML::Node yaml = YAML::LoadFile(filepath); yaml["schema_version"]) {
        pImpl->schemaVersion = yaml["schema_version"].as<std::string>();
    }
    
    // Extract description
    if (YAML::Node yaml = YAML::LoadFile(filepath); yaml["description"]) {
        pImpl->description = yaml["description"].as<std::string>();
    }
    
    // Load configuration entries
    if (YAML::Node yaml = YAML::LoadFile(filepath); yaml["configuration"]) {
        for (const auto& item : yaml["configuration"]) {
            std::string key = item.first.as<std::string>();
            YAML::Node value = item.second;
            
            ConfigValue config_value;
            ConfigSource source = ConfigSource::File;
            
            // Convert YAML value to ConfigValue
            if (value.IsScalar()) {
                std::string scalar_str = value.as<std::string>();
                
                // Try to determine type
                try {
                    size_t pos;
                    int int_val = std::stoi(scalar_str, pos);
                    if (pos == scalar_str.size()) {
                        config_value = int_val;
                    } else {
                        config_value = scalar_str;
                    }
                } catch (...) {
                    try {
                        size_t pos;
                        double double_val = std::stod(scalar_str, pos);
                        if (pos == scalar_str.size()) {
                            config_value = double_val;
                        } else {
                            config_value = scalar_str;
                        }
                    } catch (...) {
                        config_value = scalar_str;
                    }
                }
            } else if (value.IsSequence()) {
                std::vector<std::string> string_array;
                for (const auto& item : value) {
                    string_array.push_back(item.as<std::string>());
                }
                config_value = string_array;
            }
            
            // Set the configuration entry
            auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                [&key](const ConfigEntry& e) { return e.key == key; });n            
            if (it != pImpl->entries.end()) {
                it->value = config_value;
                it->source = source;
            } else {
                pImpl->entries.emplace_back(key, config_value, source);
            }
        }
    }
    
    return true;
#else
    // YAML-CPP not available
    throw std::runtime_error("YAML support not compiled in");
#endif
}

bool Config::loadFromSimpleFormat(std::ifstream& file) {
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
    
    // Try to save in JSON format with enhanced features
    try {
        saveToJSON(filepath);
        return true;
    } catch (const std::exception&) {
        // Fall back to simple format
        file.clear();
        file.seekp(0);
        return saveToSimpleFormat(file);
    }
}

void Config::saveToJSON(const std::string& filepath) const {
#ifdef HAS_NLOHMANN_JSON
    json j;
    
    // Add metadata
    j["format_version"] = pImpl->formatVersion;
    j["schema_version"] = "1.0";
    j["description"] = pImpl->description;
    j["creation_time"] = getTimeString(pImpl->creationTime);
    j["last_modified"] = getTimeString(pImpl->lastModified);
    
    // Add tags
    if (!pImpl->tags.empty()) {
        j["tags"] = pImpl->tags;
    }
    
    // Add metadata
    if (!pImpl->metadata.empty()) {
        j["metadata"] = pImpl->metadata;
    }
    
    // Add configuration section
    json config_section = json::object();
    for (const auto& entry : pImpl->entries) {
        config_section[entry.key] = configValueToJSON(entry.value);
    }
    j["configuration"] = config_section;
    
    std::ofstream file(filepath);
    if (file.is_open()) {
        file << j.dump(2); // Pretty print with 2-space indentation
    } else {
        throw std::runtime_error("Failed to open file for writing: " + filepath);
    }
#else
    throw std::runtime_error("JSON support not compiled in");
#endif
}

void Config::saveToSimpleFormat(std::ofstream& file) const {
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << configValueToString(entry.value) << "\n";
        file << "\n";
    }
}

// Helper function to convert ConfigValue to JSON
json Config::configValueToJSON(const ConfigValue& value) const {
#ifdef HAS_NLOHMANN_JSON
    return std::visit([](const auto& val) -> json {
        using T = std::decay_t<decltype(val)>;
        
        if constexpr (std::is_same_v<T, std::string>) {
            return val;
        } else if constexpr (std::is_same_v<T, int>) {
            return val;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return val;
        } else if constexpr (std::is_same_v<T, double>) {
            return val;
        } else if constexpr (std::is_same_v<T, bool>) {
            return val;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            return json(val);
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return json(val);
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return json(val);
        } else {
            return json("unknown");
        }
    }, value);
#else
    // Return empty JSON if nlohmann/json not available
    return json::object();
#endif
}

// Helper function to convert ConfigValue to string
std::string Config::configValueToString(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << '"' << arg << '"';
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

// Helper function to get time string
std::string Config::getTimeString(std::chrono::system_clock::time_point time_point) const {
    auto time = std::chrono::system_clock::to_time_t(time_point);
    std::tm tm_buf;
    gmtime_r(&time, &tm_buf);
    
    std::ostringstream oss;
    oss << std::put_time(&tm_buf, "%Y-%m-%dT%H:%M:%SZ");
    return oss.str();
}

// Template instantiations
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