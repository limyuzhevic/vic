#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <map>
#include <unordered_set>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, ConfigSchema> schema;
    ConfigCategory currentCategory;
    ConfigManager::Profile profile;
    
    Impl() : currentCategory(ConfigCategory::Basic), profile(ConfigManager::Profile::Release) {
        initializeSchema();
        applyProfileDefaults();
    }
    
    void initializeSchema() {
        // Basic category schemas
        schema["neuronCount"] = {"neuronCount", "int", "Number of neurons", ConfigCategory::Neural,
                               "^[0-9]+", 1, 1000000, 1000, true, {}};
        schema["learningRate"] = {"learningRate", "double", "Base learning rate", ConfigCategory::Advanced,
                               "^[0-9]*\.?[0-9]+", 0.0, 1.0, 0.01, true, {}};
        schema["timeSteps"] = {"timeSteps", "int", "Simulation time steps", ConfigCategory::Basic,
                               "^[0-9]+", 1, 1000000, 100, true, {}};
        
        // Advanced category schemas
        schema["stdpLTPWeight"] = {"stdpLTPWeight", "double", "STDP long-term potentiation weight",
                                 ConfigCategory::Advanced, "^[0-9]*\.?[0-9]+", 0.0, 1.0, 0.1, true, {}};
        schema["stdpLTDWeight"] = {"stdpLTDWeight", "double", "STDP long-term depression weight",
                                 ConfigCategory::Advanced, "^[0-9]*\.?[0-9]+", 0.0, 1.0, 0.12, true, {}};
        schema["stdpTimeConstant"] = {"stdpTimeConstant", "double", "STDP time constant (ms)",
                                    ConfigCategory::Advanced, "^[0-9]*\.?[0-9]+", 1.0, 1000.0, 20.0, true, {}};
        schema["plasticityLearningRate"] = {"plasticityLearningRate", "double", "Plasticity learning rate",
                                         ConfigCategory::Advanced, "^[0-9]*\.?[0-9]+", 0.0, 1.0, 0.05, true, {}};
        
        // Performance category schemas
        schema["threadCount"] = {"threadCount", "int", "Number of threads for parallel processing",
                                ConfigCategory::Performance, "^[0-9]+", 1, 32, 4, true, {}};
        schema["simdEnabled"] = {"simdEnabled", "bool", "Enable SIMD optimizations", ConfigCategory::Performance,
                                "^(true|false|1|0)$", 0.0, 1.0, true, true, {"true", "false"}};
        schema["memoryPoolSize"] = {"memoryPoolSize", "int", "Size of memory pool for neurons/synapses",
                                  ConfigCategory::Performance, "^[0-9]+", 1000, 1000000, 65536, true, {}};
        schema["optimizationFlags"] = {"optimizationFlags", "string", "Performance optimization flags",
                                     ConfigCategory::Performance, ".*", 0.0, 1.0, "default", false, {}};
        
        // Development category schemas
        schema["debugLevel"] = {"debugLevel", "int", "Debug logging level (0-3)", ConfigCategory::Development,
                               "^[0-3]", 0, 3, 1, true, {"0", "1", "2", "3"}};
        schema["profilingEnabled"] = {"profilingEnabled", "bool", "Enable profiling", ConfigCategory::Development,
                                    "^(true|false|1|0)$", 0.0, 1.0, false, true, {"true", "false"}};
        schema["validationEnabled"] = {"validationEnabled", "bool", "Enable validation", ConfigCategory::Development,
                                     "^(true|false|1|0)$", 0.0, 1.0, true, true, {"true", "false"}};
        
        // Neural category schemas
        schema["synapticDensity"] = {"synapticDensity", "double", "Synaptic density (0-1)", ConfigCategory::Neural,
                                   "^[0-9]*\.?[0-9]+", 0.0, 1.0, 0.1, true, {}};
        schema["populationSize"] = {"populationSize", "int", "Default population size", ConfigCategory::Neural,
                                   "^[0-9]+", 1, 10000, 100, true, {}};
        schema["regionSize"] = {"regionSize", "double", "Region size for neural regions", ConfigCategory::Neural,
                               "^[0-9]*\.?[0-9]+", 0.0, 100.0, 1.0, true, {}};
        schema["neuronTypes"] = {"neuronTypes", "string", "Comma-separated list of neuron types", ConfigCategory::Neural,
                                ".*", 0.0, 1.0, "excitatory,inhibitory", false, {}};
    }
    
    void applyProfileDefaults() {
        switch (profile) {
            case ConfigManager::Profile::Debug:
                set("debugLevel", true, ConfigSource::Default);
                set("profilingEnabled", true, ConfigSource::Default);
                set("validationEnabled", true, ConfigSource::Default);
                break;
            case ConfigManager::Profile::Benchmark:
                set("profilingEnabled", true, ConfigSource::Default);
                set("threadCount", std::thread::hardware_concurrency(), ConfigSource::Default);
                break;
            case ConfigManager::Profile::Release:
            default:
                set("debugLevel", false, ConfigSource::Default);
                set("profilingEnabled", false, ConfigSource::Default);
                set("validationEnabled", true, ConfigSource::Default);
                break;
        }
    }
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Check if file exists and is readable
    if (!validateFilePath(filepath) || !std::filesystem::exists(filepath)) {
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Check if it's a JSON file (by extension or content)
    bool isJSON = filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".json";
    
    if (isJSON) {
        return loadFromJSON(filepath);
    } else {
        // Legacy key=value format
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
                    ((value.front() == '\"' && value.back() == '\"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                set(key, value, ConfigSource::File);
            }
        }
    }
    
    return true;
}

bool Config::loadFromJSON(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Process JSON configuration
        if (jsonData.contains("configuration")) {
            auto config = jsonData["configuration"];
            
            if (config.contains("entries")) {
                for (const auto& entry : config["entries"]) {
                    std::string key = entry.value("key", "");
                    std::string type = entry.value("type", "string");
                    ConfigSource source = ConfigSource::File;
                    std::string desc = entry.value("description", "");
                    ConfigCategory category = ConfigCategory::Basic;
                    
                    // Parse value based on type
                    ConfigValue value;
                    if (type == "int") {
                        value = entry.value("intValue", 0);
                    } else if (type == "double") {
                        value = entry.value("doubleValue", 0.0);
                    } else if (type == "bool") {
                        value = entry.value("boolValue", false);
                    } else {
                        value = entry.value("stringValue", "");
                    }
                    
                    set(key, value, source);
                }
            }
        }
        
        return true;
    } catch (const nlohmann::json::exception& e) {
        // JSON parsing error
        return false;
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
    
    // Check if it's a JSON file (by extension)
    bool isJSON = filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".json";
    
    if (isJSON) {
        nlohmann::json jsonData;
        auto& config = jsonData["configuration"];
        config["format"] = "NLM v1.0";
        config["timestamp"] = getCurrentTimestamp();
        
        auto& entries = config["entries"];
        for (const auto& entry : pImpl->entries) {
            nlohmann::json jsonEntry;
            jsonEntry["key"] = entry.key;
            jsonEntry["description"] = entry.description;
            
            // Store value based on type
            if (std::holds_alternative<int>(entry.value)) {
                jsonEntry["type"] = "int";
                jsonEntry["intValue"] = std::get<int>(entry.value);
            } else if (std::holds_alternative<int64_t>(entry.value)) {
                jsonEntry["type"] = "int64_t";
                jsonEntry["int64Value"] = std::get<int64_t>(entry.value);
            } else if (std::holds_alternative<double>(entry.value)) {
                jsonEntry["type"] = "double";
                jsonEntry["doubleValue"] = std::get<double>(entry.value);
            } else if (std::holds_alternative<bool>(entry.value)) {
                jsonEntry["type"] = "bool";
                jsonEntry["boolValue"] = std::get<bool>(entry.value);
            } else if (std::holds_alternative<std::string>(entry.value)) {
                jsonEntry["type"] = "string";
                jsonEntry["stringValue"] = std::get<std::string>(entry.value);
            }
            
            entries.push_back(jsonEntry);
        }
        
        file << jsonData.dump(2);
    } else {
        // Legacy key=value format
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = ";
            
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else {
                    file << arg;
                }
            }, entry.value);
            
            file << "\n\n";
        }
    }
    
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
    // Validate key and value
    if (!validateKey(key) || !validateValue(value)) {
        return;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        // Determine category from key or schema
        ConfigCategory category = ConfigCategory::Basic;
        auto schemaIt = pImpl->schema.find(key);
        if (schemaIt != pImpl->schema.end()) {
            category = schemaIt->second.category;
        }
        
        pImpl->entries.emplace_back(key, value, source, "", category);
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
    
    // Group by category
    std::unordered_map<ConfigCategory, std::vector<const ConfigEntry*>> grouped;
    for (const auto& entry : pImpl->entries) {
        grouped[entry.category].push_back(&entry);
    }
    
    // Print header
    oss << "Profile: " << static_cast<int>(pImpl->profile) << "\n";
    
    // Print by category
    const char* categoryNames[] = {"Basic", "Neural", "Advanced", "Performance", "Development", "Experiment"};
    for (int cat = 0; cat < static_cast<int>(ConfigCategory::Experiment) + 1; ++cat) {
        auto it = grouped.find(static_cast<ConfigCategory>(cat));
        if (it != grouped.end() && !it->second.empty()) {
            oss << "\n[" << categoryNames[cat] << "]\n";
            for (const auto* entry : it->second) {
                oss << "  " << entry->key << " = [";
                std::visit([&oss](auto&& arg) {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, std::string>) {
                        oss << "\"" << arg << "\"";
                    } else {
                        oss << arg;
                    }
                }, entry->value);
                oss << "] (" << static_cast<int>(entry->source) << ")\n";
            }
        }
    }
    
    return oss.str();
}

bool Config::validateFilePath(const std::string& filepath) {
    if (filepath.empty() || filepath.size() > 4096) {
        return false;
    }
    
    // Check for invalid characters
    const std::string invalidChars = "<>?:\"|?*\\";
    for (char c : filepath) {
        if (invalidChars.find(c) != std::string::npos) {
            return false;
        }
    }
    
    return true;
}

bool Config::validateKey(const std::string& key) {
    if (key.empty() || key.size() > 128) {
        return false;
    }
    
    // Allow alphanumeric, underscore, hyphen, and dot
    std::regex keyPattern("^[a-zA-Z0-9_.-]+$");
    return std::regex_match(key, keyPattern);
}

bool Config::validateValue(const ConfigValue& value) {
    return true; // Variant handles basic type safety
}

std::string Config::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%dT%H:%M:%S");
    
    return ss.str();
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
