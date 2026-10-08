#include <nlohmann/json.hpp>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <cmath>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        logError("Cannot open configuration file: " + filepath);
        return false;
    }
    
    // Try to parse as JSON first
    try {
        nlohmann::json j;
        file >> j;
        
        // Successfully parsed JSON
        for (auto& [key, value] : j.items()) {
            set(key, value, ConfigSource::File);
        }
        return true;
    } catch (const nlohmann::json::parse_error& e) {
        // JSON parsing failed, try simple key=value format
        file.clear();
        file.seekg(0);
        
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
    } catch (const std::exception& e) {
        logError("Error parsing configuration file " + filepath + ": " + std::string(e.what()));
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
        logError("Cannot open configuration file for writing: " + filepath);
        return false;
    }
    
    nlohmann::json j;
    
    for (const auto& entry : pImpl->entries) {
        // Add description as comment
        file << "# " << entry.description << "\n";
        
        // Add key-value pair to JSON
        j[entry.key] = convertConfigValueToJson(entry.value);
    }
    
    // Write JSON to file
    file << j.dump(4); // 4-space indentation
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

bool Config::validate() const {
    bool valid = true;
    
    // Validate required parameters
    std::vector<std::string> requiredParams = {
        "random_seed",
        "simulation_timestep",
        "neuron_count"
    };
    
    for (const auto& param : requiredParams) {
        if (!has(param)) {
            logError("Missing required configuration parameter: " + param);
            valid = false;
        }
    }
    
    // Validate value ranges for known parameters
    if (has("neuron_count")) {
        auto neuronCount = getOr<int64_t>("neuron_count", 0);
        if (neuronCount < 1 || neuronCount > 1000000) {
            logError("Invalid neuron_count: " + std::to_string(neuronCount) + ". Must be between 1 and 1000000");
            valid = false;
        }
    }
    
    if (has("simulation_timestep")) {
        auto timestep = getOr<double>("simulation_timestep", 0.0);
        if (timestep <= 0.0 || timestep > 1.0) {
            logError("Invalid simulation_timestep: " + std::to_string(timestep) + ". Must be between 0.0 and 1.0");
            valid = false;
        }
    }
    
    if (has("region_count")) {
        auto regionCount = getOr<int64_t>("region_count", 0);
        if (regionCount < 1) {
            logError("Invalid region_count: " + std::to_string(regionCount) + ". Must be at least 1");
            valid = false;
        }
    }
    
    if (has("connection_probability")) {
        auto connProb = getOr<double>("connection_probability", 0.0);
        if (connProb < 0.0 || connProb > 1.0) {
            logError("Invalid connection_probability: " + std::to_string(connProb) + ". Must be between 0.0 and 1.0");
            valid = false;
        }
    }
    
    return valid;
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

// Helper function to convert ConfigValue to JSON
nlohmann::json Config::convertConfigValueToJson(const ConfigValue& value) const {
    return std::visit(nlohmann::json overloaded {
        [](int v) { return v; },
        [](int64_t v) { return v; },
        [](double v) { return v; },
        [](bool v) { return v; },
        [](const std::string& v) { return v; },
        [](const std::vector<int>& v) { 
            nlohmann::json j = nlohmann::json::array();
            for (const auto& item : v) {
                j.push_back(item);
            }
            return j;
        },
        [](const std::vector<double>& v) {
            nlohmann::json j = nlohmann::json::array();
            for (const auto& item : v) {
                j.push_back(item);
            }
            return j;
        },
        [](const std::vector<std::string>& v) {
            nlohmann::json j = nlohmann::json::array();
            for (const auto& item : v) {
                j.push_back(item);
            }
            return j;
        }
    }, value);
}

// Helper function to convert JSON to ConfigValue
ConfigValue Config::convertJsonToConfigValue(const nlohmann::json& j) {
    if (j.is_string()) {
        return j.get<std::string>();
    } else if (j.is_number_integer()) {
        return j.get<int64_t>();
    } else if (j.is_number_float()) {
        return j.get<double>();
    } else if (j.is_boolean()) {
        return j.get<bool>();
    } else if (j.is_array()) {
        // Try to determine vector type based on array content
        if (j.empty()) {
            return std::vector<int>{}; // Default to empty int vector
        }
        
        // Check if all elements are integers
        bool allIntegers = true;
        for (const auto& item : j) {
            if (!item.is_number_integer()) {
                allIntegers = false;
                break;
            }
        }
        if (allIntegers) {
            std::vector<int> result;
            result.reserve(j.size());
            for (const auto& item : j) {
                result.push_back(item.get<int>());
            }
            return result;
        }
        
        // Check if all elements are floats
        bool allFloats = true;
        for (const auto& item : j) {
            if (!item.is_number_float()) {
                allFloats = false;
                break;
            }
        }
        if (allFloats) {
            std::vector<double> result;
            result.reserve(j.size());
            for (const auto& item : j) {
                result.push_back(item.get<double>());
            }
            return result;
        }
        
        // Check if all elements are strings
        bool allStrings = true;
        for (const auto& item : j) {
            if (!item.is_string()) {
                allStrings = false;
                break;
            }
        }
        if (allStrings) {
            std::vector<std::string> result;
            result.reserve(j.size());
            for (const auto& item : j) {
                result.push_back(item.get<std::string>());
            }
            return result;
        }
        
        // Default to int vector for mixed numeric types
        std::vector<int> result;
        result.reserve(j.size());
        for (const auto& item : j) {
            result.push_back(item.get<int>());
        }
        return result;
    }
    
    // Default to string for unknown types
    return j.get<std::string>();
}

// Add overloaded visitor for JSON parsing in set method
static nlohmann::json::to_json_func get_json_visitor() {
    return [](const ConfigValue& value, nlohmann::json& j) {
        j = std::visit(nlohmann::json overloaded {
            [](int v) { return v; },
            [](int64_t v) { return v; },
            [](double v) { return v; },
            [](bool v) { return v; },
            [](const std::string& v) { return v; },
            [](const std::vector<int>& v) { return v; },
            [](const std::vector<double>& v) { return v; },
            [](const std::vector<std::string>& v) { return v; }
        }, value);
    };
}

namespace {
    const nlohmann::json::to_json_func json_to_value = get_json_visitor();
}

// Validation helpers
bool Config::validateValue(const std::string& key, const ConfigValue& value) {
    if (key == "random_seed") {
        auto val = std::get<int64_t>(value);
        if (val < 0 || val > 2147483647) {
            logError("random_seed must be between 0 and 2147483647");
            return false;
        }
    }
    
    if (key == "neuron_count") {
        auto val = std::get<int64_t>(value);
        if (val < 1 || val > 1000000) {
            logError("neuron_count must be between 1 and 1000000");
            return false;
        }
    }
    
    if (key == "region_count") {
        auto val = std::get<int64_t>(value);
        if (val < 1) {
            logError("region_count must be at least 1");
            return false;
        }
    }
    
    if (key == "connection_probability") {
        auto val = std::get<double>(value);
        if (val < 0.0 || val > 1.0) {
            logError("connection_probability must be between 0.0 and 1.0");
            return false;
        }
    }
    
    return true;
}

bool Config::isValidType(const ConfigValue& value, const std::string& expectedType) {
    // Simple type checking
    std::string actualType;
    
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) actualType = "int";
        else if constexpr (std::is_same_v<T, int64_t>) actualType = "int64_t";
        else if constexpr (std::is_same_v<T, double>) actualType = "double";
        else if constexpr (std::is_same_v<T, bool>) actualType = "bool";
        else if constexpr (std::is_same_v<T, std::string>) actualType = "string";
        else if constexpr (std::is_same_v<T, std::vector<int>>) actualType = "vector<int>";
        else if constexpr (std::is_same_v<T, std::vector<double>>) actualType = "vector<double>";
        else if constexpr (std::is_same_v<T, std::vector<std::string>>) actualType = "vector<string>";
    }, value);
    
    return actualType == expectedType;
}

// Apply preset configuration
void Config::applyPreset(const std::string& presetName) {
    std::string name = toLower(presetName);
    
    // Clear existing non-default entries to avoid conflicts
    std::vector<std::string> keys = getKeys();
    for (const auto& key : keys) {
        if (key != "random_seed" && key != "simulation_timestep" && 
            key != "neuron_count" && key != "region_count" &&
            key != "connection_probability") {
            remove(key);
        }
    }
    
    // Apply preset values
    if (name == "quick") {
        set("neuron_count", static_cast<int64_t>(100), ConfigSource::Preset);
        set("connection_probability", 0.1f, ConfigSource::Preset);
        set("simulation_timestep", 0.001, ConfigSource::Preset);
        set("region_count", static_cast<int64_t>(1), ConfigSource::Preset);
    } else if (name == "medium") {
        set("neuron_count", static_cast<int64_t>(500), ConfigSource::Preset);
        set("connection_probability", 0.15f, ConfigSource::Preset);
        set("simulation_timestep", 0.001, ConfigSource::Preset);
        set("region_count", static_cast<int64_t>(1), ConfigSource::Preset);
    } else if (name == "high") {
        set("neuron_count", static_cast<int64_t>(2000), ConfigSource::Preset);
        set("connection_probability", 0.2f, ConfigSource::Preset);
        set("simulation_timestep", 0.0005, ConfigSource::Preset);
        set("region_count", static_cast<int64_t>(2), ConfigSource::Preset);
    } else if (name == "custom") {
        // Custom preset leaves configuration to user command line
        logError("Custom preset does not set default values. Please specify configuration via command line.");
    }
}

// Merge configuration from another config object
void Config::mergeFrom(const Config& other, ConfigSource source) {
    std::vector<std::string> otherKeys = other.getKeys();
    for (const auto& key : otherKeys) {
        auto value = other.get<std::string>(key);
        if (value.has_value()) {
            // For string values, try to determine the actual type
            std::string strValue = value.value();
            
            // Try to convert to numeric types
            try {
                size_t pos;
                int intVal = std::stoi(strValue, &pos);
                if (pos == strValue.size()) {
                    set(key, intVal, source);
                    continue;
                }
            } catch (...) {}
            
            try {
                size_t pos;
                long long intVal = std::stoll(strValue, &pos);
                if (pos == strValue.size()) {
                    set(key, intVal, source);
                    continue;
                }
            } catch (...) {}
            
            try {
                size_t pos;
                double floatVal = std::stod(strValue, &pos);
                if (pos == strValue.size()) {
                    set(key, floatVal, source);
                    continue;
                }
            } catch (...) {}
            
            try {
                if (strValue == "true" || strValue == "false") {
                    bool boolVal = (strValue == "true");
                    set(key, boolVal, source);
                    continue;
                }
            } catch (...) {}
            
            // Use string value as fallback
            set(key, strValue, source);
        }
    }
}

// Get default configuration template
std::string Config::getTemplate(const std::string& format) {
    if (format == "yaml") {
        return "neuron_count: 500\nregion_count: 1\nconnection_probability: 0.15\nsimulation_timestep: 0.001\nstdp_ltp_weight: 0.02\nstdp_ltd_weight: 0.015\nrandom_seed: 42";
    } else if (format == "csv") {
        return "parameter,value,description\nneuron_count,500,Number of neurons in the network\nregion_count,1,Number of neural regions\nconnection_probability,0.15,Probability of connection between neurons\nsimulation_timestep,0.001,Time step for simulation\nstdp_ltp_weight,0.02,LTP weight for STDP\nstdp_ltd_weight,0.015,LTD weight for STDP\nrandom_seed,42,Random seed for reproducibility";
    } else { // Default to JSON
        nlohmann::json j;
        j["neuron_count"] = 500;
        j["region_count"] = 1;
        j["connection_probability"] = 0.15;
        j["simulation_timestep"] = 0.001;
        j["stdp_ltp_weight"] = 0.02;
        j["stdp_ltd_weight"] = 0.015;
        j["random_seed"] = 42;
        return j.dump(2);
    }
}

// Validate configuration with detailed error reporting
std::vector<std::string> Config::validateDetailed() const {
    std::vector<std::string> errors;
    
    // Validate required parameters
    std::vector<std::string> requiredParams = {
        "random_seed",
        "simulation_timestep",
        "neuron_count"
    };
    
    for (const auto& param : requiredParams) {
        if (!has(param)) {
            errors.push_back("Missing required configuration parameter: " + param);
        }
    }
    
    // Validate value ranges for known parameters
    if (has("neuron_count")) {
        auto neuronCount = getOr<int64_t>("neuron_count", 0);
        if (neuronCount < 1 || neuronCount > 1000000) {
            errors.push_back("Invalid neuron_count: " + std::to_string(neuronCount) + ". Must be between 1 and 1000000");
        }
    }
    
    if (has("simulation_timestep")) {
        auto timestep = getOr<double>("simulation_timestep", 0.0);
        if (timestep <= 0.0 || timestep > 1.0) {
            errors.push_back("Invalid simulation_timestep: " + std::to_string(timestep) + ". Must be between 0.0 and 1.0");
        }
    }
    
    if (has("region_count")) {
        auto regionCount = getOr<int64_t>("region_count", 0);
        if (regionCount < 1) {
            errors.push_back("Invalid region_count: " + std::to_string(regionCount) + ". Must be at least 1");
        }
    }
    
    if (has("connection_probability")) {
        auto connProb = getOr<double>("connection_probability", 0.0);
        if (connProb < 0.0 || connProb > 1.0) {
            errors.push_back("Invalid connection_probability: " + std::to_string(connProb) + ". Must be between 0.0 and 1.0");
        }
    }
    
    if (has("stdp_ltp_weight")) {
        auto ltp = getOr<double>("stdp_ltp_weight", 0.0);
        if (ltp < 0.0) {
            errors.push_back("Invalid stdp_ltp_weight: " + std::to_string(ltp) + ". Must be non-negative");
        }
    }
    
    if (has("stdp_ltd_weight")) {
        auto ltd = getOr<double>("stdp_ltd_weight", 0.0);
        if (ltd < 0.0) {
            errors.push_back("Invalid stdp_ltd_weight: " + std::to_string(ltd) + ". Must be non-negative");
        }
    }
    
    // Check for configuration quality issues
    if (has("neuron_count") && has("region_count")) {
        auto neuronCount = getOr<int64_t>("neuron_count", 0);
        auto regionCount = getOr<int64_t>("region_count", 0);
        if (regionCount > 10 && neuronCount / regionCount < 100) {
            errors.push_back("Warning: Very small neurons per region (" + std::to_string(neuronCount / regionCount) + "). Consider reducing region_count or increasing neuron_count.");
        }
    }
    
    return errors;
}

// Get configuration quality score (0-100)
int Config::getQualityScore() const {
    int score = 100;
    
    // Check required parameters
    std::vector<std::string> requiredParams = {"random_seed", "simulation_timestep", "neuron_count"};
    for (const auto& param : requiredParams) {
        if (!has(param)) {
            score -= 30;
        }
    }
    
    // Check value ranges
    if (has("neuron_count")) {
        auto neuronCount = getOr<int64_t>("neuron_count", 0);
        if (neuronCount < 1 || neuronCount > 10000) {
            score -= 10;
        }
    }
    
    if (has("simulation_timestep")) {
        auto timestep = getOr<double>("simulation_timestep", 0.0);
        if (timestep < 0.0001 || timestep > 0.1) {
            score -= 10;
        }
    }
    
    if (has("region_count")) {
        auto regionCount = getOr<int64_t>("region_count", 0);
        if (regionCount < 1 || regionCount > 10) {
            score -= 5;
        }
    }
    
    if (has("connection_probability")) {
        auto connProb = getOr<double>("connection_probability", 0.0);
        if (connProb < 0.01 || connProb > 0.5) {
            score -= 5;
        }
    }
    
    // Ensure score is between 0 and 100
    return std::max(0, std::min(100, score));
}

// Check if configuration is optimized
bool Config::isOptimized() const {
    // Check for common optimization criteria
    if (!has("neuron_count") || !has("region_count") || !has("connection_probability")) {
        return false;
    }
    
    auto neuronCount = getOr<int64_t>("neuron_count", 0);
    auto regionCount = getOr<int64_t>("region_count", 0);
    auto connProb = getOr<double>("connection_probability", 0.0);
    
    // Check neuron count is reasonable
    if (neuronCount < 10 || neuronCount > 100000) {
        return false;
    }
    
    // Check connection probability is reasonable
    if (connProb < 0.01 || connProb > 0.3) {
        return false;
    }
    
    // Check neurons per region is reasonable
    if (regionCount > 1 && neuronCount / regionCount < 50) {
        return false;
    }
    
    return true;
}

// Get memory usage of configuration
size_t Config::getMemoryUsage() const {
    size_t total = 0;
    for (const auto& entry : pImpl->entries) {
        // Estimate memory for each entry
        total += entry.key.size() + sizeof(entry.value) + entry.description.size();
    }
    return total;
}

// Error handling
void Config::logError(const std::string& message) {
    std::cerr << "[Config Error] " << message << std::endl;
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

