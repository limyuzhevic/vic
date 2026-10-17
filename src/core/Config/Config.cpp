#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <any>

namespace nlm {

struct Config::Impl {
    std::unordered_map<std::string, std::any> values;
    std::unordered_map<std::string, std::pair<std::any, ConfigSource>> entries;
    std::unordered_map<std::string, ConfigType> types;
    std::unordered_map<std::string, std::string> descriptions;
    std::unordered_map<std::string, std::string> defaultValues;
    std::vector<std::string> validationErrors;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    initializeDefaults();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

void Config::initializeDefaults() {
    // Add standard NLM configuration parameters with enhanced descriptions
    addParameter("brain.neuron_count", ConfigType::Int64, "1000", 
                 "Total number of neurons in the brain", 
                 [](const std::string&, const std::string& val, const std::string&) {
                     try { std::stoll(val); return true; } catch (...) { return false; }
                 }, true);
    
    addParameter("brain.region_count", ConfigType::Int64, "1",
                 "Number of neural regions",
                 [](const std::string&, const std::string& val, const std::string&) {
                     try { std::stoll(val); return true; } catch (...) { return false; }
                 });
    
    addParameter("brain.connection_probability", ConfigType::Double, "0.1",
                 "Probability of synaptic connections (0-1)",
                 [](const std::string&, const std::string& val, const std::string&) {
                     try { double d = std::stod(val); return d >= 0.0 && d <= 1.0; } catch (...) { return false; }
                 });
    
    addParameter("brain.initial_weight_mean", ConfigType::Double, "0.5",
                 "Mean of initial synaptic weights");
    
    addParameter("brain.initial_weight_std", ConfigType::Double, "0.1",
                 "Standard deviation of initial weights");
    
    addParameter("brain.v_thresh", ConfigType::Double, "-50.0",
                 "Neuron threshold potential (mV)");
    
    addParameter("brain.v_rest", ConfigType::Double, "-70.0",
                 "Resting potential (mV)");
    
    addParameter("brain.v_reset", ConfigType::Double, "-75.0",
                 "Reset potential after spike (mV)");
    
    addParameter("brain.tau_mem", ConfigType::Double, "20.0",
                 "Membrane time constant (ms)");
    
    addParameter("brain.tau_ref", ConfigType::Double, "2.0",
                 "Refractory period (ms)");
    
    addParameter("plasticity.stdp.enable", ConfigType::Bool, "true",
                 "Enable STDP plasticity");
    
    addParameter("plasticity.stdp.learning_rate", ConfigType::Double, "0.001",
                 "STDP learning rate");
    
    addParameter("plasticity.stdp.tau_plus", ConfigType::Double, "20.0",
                 "STDP LTP time constant (ms)");
    
    addParameter("plasticity.stdp.tau_minus", ConfigType::Double, "20.0",
                 "STDP LTD time constant (ms)");
    
    addParameter("plasticity.hebbian.enable", ConfigType::Bool, "true",
                 "Enable Hebbian learning");
    
    addParameter("plasticity.structural.enable", ConfigType::Bool, "true",
                 "Enable structural plasticity");
    
    addParameter("neuromod.dopamine.scale", ConfigType::Double, "1.0",
                 "Dopamine modulation scale");
    
    addParameter("neuromod.curiosity.enable", ConfigType::Bool, "true",
                 "Enable curiosity-driven exploration");
    
    addParameter("neuromod.novelty.enable", ConfigType::Bool, "true",
                 "Enable novelty detection");
    
    addParameter("simulation.timestep", ConfigType::Double, "0.001",
                 "Simulation timestep (s)");
    
    addParameter("simulation.max_steps", ConfigType::Int64, "10000",
                 "Maximum simulation steps");
    
    addParameter("random.seed", ConfigType::Int64, "42",
                 "Random seed for reproducibility");
    
    addParameter("checkpoint.enable", ConfigType::Bool, "true",
                 "Enable checkpoint saving");
    
    addParameter("checkpoint.path", ConfigType::String, "./checkpoint.bin",
                 "Checkpoint file path");
    
    addParameter("development.enable", ConfigType::Bool, "true",
                 "Enable developmental stages");
}

bool Config::loadFromFile(const std::string& filepath) {
    try {
        // Try to parse as JSON first (recommended)
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json jsonData;
        file >> jsonData;
        return fromJson(jsonData);
    } catch (const std::exception&) {
        // Fall back to simple key=value format
        return loadFromSimpleFile(filepath);
    }
}

bool Config::loadFromSimpleFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    int lineNum = 0;
    while (std::getline(file, line)) {
        lineNum++;
        // Skip empty lines and comments
        size_t commentPos = line.find('#');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }
        
        line = trim(line);
        if (line.empty()) {
            continue;
        }
        
        // Parse key=value pairs
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
            
            try {
                set(key, value, ConfigSource::File);
            } catch (const ConfigError&) {
                pImpl->validationErrors.push_back("Line " + std::to_string(lineNum) + ": Invalid value for key: " + key);
                return false;
            }
        } else {
            pImpl->validationErrors.push_back("Line " + std::to_string(lineNum) + ": Invalid format (no '=' found)");
            return false;
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
                try {
                    set(key, value, ConfigSource::CommandLine);
                } catch (const ConfigError&) {
                    return false;
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            try {
                set(key, value, ConfigSource::CommandLine);
            } catch (const ConfigError&) {
                return false;
            }
        }
        // Handle --help option
        else if (arg == "--help" || arg == "-h") {
            std::cout << "=== NLM Configuration Options ===" << std::endl;
            const auto& params = getParameters();
            for (const auto& param : params) {
                std::cout << "  --" << param.key << "=" << param.defaultValue 
                         << "  " << param.description << std::endl;
            }
            std::cout << "  --help, -h  Show this help" << std::endl;
            std::cout << "  --config=file.json  Load configuration from JSON file" << std::endl;
            return true; // Special case for help
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        nlohmann::json jsonData = toJson();
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << jsonData.dump(2); // Pretty print with indentation
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = pImpl->values.find(key);
    if (it == pImpl->values.end()) {
        return std::nullopt;
    }
    
    try {
        return std::any_cast<T>(it->second);
    } catch (const std::bad_any_cast&) {
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    // Find parameter metadata
    auto paramIt = std::find_if(pImpl->parameters.begin(), pImpl->parameters.end(),
                                [&key](const ConfigParameter& p) { return p.key == key; });
    
    if (paramIt == pImpl->parameters.end()) {
        // No parameter validation, accept any value
        pImpl->values[key] = value;
        pImpl->entries[key] = {value, source};
        pImpl->types[key] = ConfigType::String;
        return;
    }
    
    const auto& param = *paramIt;
    
    // Validate against parameter metadata
    if (validateKeyValue(key, value)) {
        pImpl->values[key] = value;
        pImpl->entries[key] = {value, source};
        pImpl->types[key] = param.type;
        pImpl->descriptions[key] = param.description;
        pImpl->defaultValues[key] = param.defaultValue;
    } else {
        throw ConfigError("Invalid value for key: " + key + ", value: " + value);
    }
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, static_cast<int64_t>(value), source);
}

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
    if (validateKeyValue(key, std::to_string(value))) {
        pImpl->values[key] = value;
        pImpl->entries[key] = {value, source};
        pImpl->types[key] = ConfigType::Int64;
        
        auto paramIt = std::find_if(pImpl->parameters.begin(), pImpl->parameters.end(),
                                    [&key](const ConfigParameter& p) { return p.key == key; });
        if (paramIt != pImpl->parameters.end()) {
            pImpl->descriptions[key] = paramIt->description;
            pImpl->defaultValues[key] = paramIt->defaultValue;
        }
    } else {
        throw ConfigError("Invalid value for key: " + key);
    }
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    if (validateKeyValue(key, std::to_string(value))) {
        pImpl->values[key] = value;
        pImpl->entries[key] = {value, source};
        pImpl->types[key] = ConfigType::Double;
        
        auto paramIt = std::find_if(pImpl->parameters.begin(), pImpl->parameters.end(),
                                    [&key](const ConfigParameter& p) { return p.key == key; });
        if (paramIt != pImpl->parameters.end()) {
            pImpl->descriptions[key] = paramIt->description;
            pImpl->defaultValues[key] = paramIt->defaultValue;
        }
    } else {
        throw ConfigError("Invalid value for key: " + key);
    }
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    std::string boolStr = value ? "true" : "false";
    if (validateKeyValue(key, boolStr)) {
        pImpl->values[key] = value;
        pImpl->entries[key] = {value, source};
        pImpl->types[key] = ConfigType::Bool;
        
        auto paramIt = std::find_if(pImpl->parameters.begin(), pImpl->parameters.end(),
                                    [&key](const ConfigParameter& p) { return p.key == key; });
        if (paramIt != pImpl->parameters.end()) {
            pImpl->descriptions[key] = paramIt->description;
            pImpl->defaultValues[key] = paramIt->defaultValue;
        }
    } else {
        throw ConfigError("Invalid value for key: " + key);
    }
}

void Config::set(const std::string& key, const std::vector<int>& value, ConfigSource source) {
    std::string jsonStr = "[";
    for (size_t i = 0; i < value.size(); ++i) {
        if (i > 0) jsonStr += ",";
        jsonStr += std::to_string(value[i]);
    }
    jsonStr += "]";
    
    if (validateKeyValue(key, jsonStr)) {
        pImpl->values[key] = jsonStr;
        pImpl->entries[key] = {jsonStr, source};
        pImpl->types[key] = ConfigType::IntArray;
        
        auto paramIt = std::find_if(pImpl->parameters.begin(), pImpl->parameters.end(),
                                    [&key](const ConfigParameter& p) { return p.key == key; });
        if (paramIt != pImpl->parameters.end()) {
            pImpl->descriptions[key] = paramIt->description;
            pImpl->defaultValues[key] = paramIt->defaultValue;
        }
    } else {
        throw ConfigError("Invalid value for key: " + key);
    }
}

void Config::set(const std::string& key, const std::vector<double>& value, ConfigSource source) {
    std::string jsonStr = "[";
    for (size_t i = 0; i < value.size(); ++i) {
        if (i > 0) jsonStr += ",";
        jsonStr += std::to_string(value[i]);
    }
    jsonStr += "]";
    
    if (validateKeyValue(key, jsonStr)) {
        pImpl->values[key] = jsonStr;
        pImpl->entries[key] = {jsonStr, source};
        pImpl->types[key] = ConfigType::DoubleArray;
        
        auto paramIt = std::find_if(pImpl->parameters.begin(), pImpl->parameters.end(),
                                    [&key](const ConfigParameter& p) { return p.key == key; });
        if (paramIt != pImpl->parameters.end()) {
            pImpl->descriptions[key] = paramIt->description;
            pImpl->defaultValues[key] = paramIt->defaultValue;
        }
    } else {
        throw ConfigError("Invalid value for key: " + key);
    }
}

void Config::set(const std::string& key, const std::vector<std::string>& value, ConfigSource source) {
    std::string jsonStr = "[";
    for (size_t i = 0; i < value.size(); ++i) {
        if (i > 0) jsonStr += ",";
        jsonStr += "\"" + value[i] + "\"";
    }
    jsonStr += "]";
    
    if (validateKeyValue(key, jsonStr)) {
        pImpl->values[key] = jsonStr;
        pImpl->entries[key] = {jsonStr, source};
        pImpl->types[key] = ConfigType::StringArray;
        
        auto paramIt = std::find_if(pImpl->parameters.begin(), pImpl->parameters.end(),
                                    [&key](const ConfigParameter& p) { return p.key == key; });
        if (paramIt != pImpl->parameters.end()) {
            pImpl->descriptions[key] = paramIt->description;
            pImpl->defaultValues[key] = paramIt->defaultValue;
        }
    } else {
        throw ConfigError("Invalid value for key: " + key);
    }
}

bool Config::has(const std::string& key) const {
    return pImpl->values.find(key) != pImpl->values.end();
}

void Config::remove(const std::string& key) {
    pImpl->values.erase(key);
    pImpl->entries.erase(key);
    pImpl->types.erase(key);
    pImpl->descriptions.erase(key);
    pImpl->defaultValues.erase(key);
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->values.size());
    for (const auto& pair : pImpl->values) {
        keys.push_back(pair.first);
    }
    return keys;
}

void Config::clear() {
    pImpl->values.clear();
    pImpl->entries.clear();
    pImpl->types.clear();
    pImpl->descriptions.clear();
    pImpl->defaultValues.clear();
    pImpl->validationErrors.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->values.size() << " entries):\n";
    
    for (const auto& pair : pImpl->values) {
        oss << "  " << pair.first << " = ";
        auto it = pImpl->types.find(pair.first);
        if (it != pImpl->types.end()) {
            switch (it->second) {
                case ConfigType::Int64:
                    oss << std::any_cast<int64_t>(pair.second);
                    break;
                case ConfigType::Double:
                    oss << std::any_cast<double>(pair.second);
                    break;
                case ConfigType::Bool:
                    oss << (std::any_cast<bool>(pair.second) ? "true" : "false");
                    break;
                case ConfigType::String:
                    oss << "\"" << std::any_cast<std::string>(pair.second) << "\"";
                    break;
                case ConfigType::IntArray:
                case ConfigType::DoubleArray:
                case ConfigType::StringArray:
                    oss << "[array]";
                    break;
                case ConfigType::Int:
                    oss << std::any_cast<int>(pair.second);
                    break;
            }
        } else {
            oss << "[unknown]";
        }
        
        auto entryIt = pImpl->entries.find(pair.first);
        if (entryIt != pImpl->entries.end()) {
            oss << " (" << static_cast<int>(entryIt->second.second) << ")";
        }
        oss << "\n";
    }
    
    if (!pImpl->validationErrors.empty()) {
        oss << "\nValidation Errors:" << std::endl;
        for (const auto& error : pImpl->validationErrors) {
            oss << "  " << error << std::endl;
        }
    }
    
    return oss.str();
}

bool Config::validate(std::vector<std::string>& outErrors) const {
    pImpl->validationErrors.clear();
    outErrors.clear();
    
    for (const auto& param : pImpl->parameters) {
        if (param.required && !has(param.key)) {
            pImpl->validationErrors.push_back("Missing required parameter: " + param.key);
            outErrors.push_back("Missing required parameter: " + param.key);
            return false;
        }
        
        if (has(param.key)) {
            std::string error;
            if (!validateKeyValue(param.key, "", error)) {
                pImpl->validationErrors.push_back(error);
                outErrors.push_back(error);
                return false;
            }
        }
    }
    
    return pImpl->validationErrors.empty();
}

bool Config::fromJson(const nlohmann::json& jsonData) {
    try {
        clear();
        
        for (const auto& item : jsonData.items()) {
            const std::string& key = item.key();
            const nlohmann::json& value = item.value();
            
            if (value.is_number_integer()) {
                set(key, value.get<int64_t>(), ConfigSource::File);
            } else if (value.is_number_float()) {
                set(key, value.get<double>(), ConfigSource::File);
            } else if (value.is_boolean()) {
                set(key, value.get<bool>(), ConfigSource::File);
            } else if (value.is_string()) {
                set(key, value.get<std::string>(), ConfigSource::File);
            } else if (value.is_array()) {
                std::string jsonStr = value.dump();
                set(key, jsonStr, ConfigSource::File);
            }
        }
        
        return validate(pImpl->validationErrors);
    } catch (const std::exception& e) {
        pImpl->validationErrors.push_back(std::string("JSON parsing error: ") + e.what());
        return false;
    }
}

nlohmann::json Config::toJson() const {
    nlohmann::json jsonData;
    
    for (const auto& pair : pImpl->values) {
        auto typeIt = pImpl->types.find(pair.first);
        if (typeIt != pImpl->types.end()) {
            switch (typeIt->second) {
                case ConfigType::Int64:
                    jsonData[pair.first] = std::any_cast<int64_t>(pair.second);
                    break;
                case ConfigType::Double:
                    jsonData[pair.first] = std::any_cast<double>(pair.second);
                    break;
                case ConfigType::Bool:
                    jsonData[pair.first] = std::any_cast<bool>(pair.second);
                    break;
                case ConfigType::String:
                    jsonData[pair.first] = std::any_cast<std::string>(pair.second);
                    break;
                case ConfigType::IntArray:
                    jsonData[pair.first] = nlohmann::json::parse(std::any_cast<std::string>(pair.second));
                    break;
                case ConfigType::DoubleArray:
                    jsonData[pair.first] = nlohmann::json::parse(std::any_cast<std::string>(pair.second));
                    break;
                case ConfigType::StringArray:
                    jsonData[pair.first] = nlohmann::json::parse(std::any_cast<std::string>(pair.second));
                    break;
                case ConfigType::Int:
                    jsonData[pair.first] = std::any_cast<int>(pair.second);
                    break;
            }
        }
    }
    
    return jsonData;
}

void Config::addParameter(const std::string& key, ConfigType type, 
                         const std::string& defaultValue,
                         const std::string& description,
                         std::function<bool(const std::string&, const std::string&)> validator) {
    pImpl->parameters.push_back(ConfigParameter(key, description, defaultValue, type, 
                                              [validator](const std::string&, const std::string& val, const std::string&) {
                                                  return validator ? validator("", val) : true;
                                              }));
}

std::string Config::getParameterDescription(const std::string& key) const {
    auto it = pImpl->descriptions.find(key);
    return it != pImpl->descriptions.end() ? it->second : "";
}

bool Config::isParameterRequired(const std::string& key) const {
    auto it = std::find_if(pImpl->parameters.begin(), pImpl->parameters.end(),
                          [&key](const ConfigParameter& p) { return p.key == key; });
    return it != pImpl->parameters.end() && it->required;
}

bool Config::validateKeyValue(const std::string& key, const std::string& value, std::string& error) const {
    auto paramIt = std::find_if(pImpl->parameters.begin(), pImpl->parameters.end(),
                                [&key](const ConfigParameter& p) { return p.key == key; });
    
    if (paramIt == pImpl->parameters.end()) {
        return true; // No parameter validation
    }
    
    const auto& param = *paramIt;
    
    try {
        switch (param.type) {
            case ConfigType::Int64:
                if (value.empty()) {
                    error = "Parameter '" + key + "' requires integer value";
                    return false;
                }
                try {
                    std::stoll(value);
                    return true;
                } catch (...) {
                    error = "Parameter '" + key + "' requires valid integer value, got: " + value;
                    return false;
                }
            case ConfigType::Double:
                if (value.empty()) {
                    error = "Parameter '" + key + "' requires floating-point value";
                    return false;
                }
                try {
                    double d = std::stod(value);
                    if (param.key == "brain.connection_probability" && (d < 0.0 || d > 1.0)) {
                        error = "Parameter '" + key + "' must be between 0.0 and 1.0";
                        return false;
                    }
                    return true;
                } catch (...) {
                    error = "Parameter '" + key + "' requires valid floating-point value, got: " + value;
                    return false;
                }
            case ConfigType::Bool:
                return (value == "true" || value == "false" || value == "1" || value == "0");
            case ConfigType::String:
                return true;
            case ConfigType::IntArray:
            case ConfigType::DoubleArray:
            case ConfigType::StringArray:
                return true; // Will be validated when parsed
            case ConfigType::Int:
                if (value.empty()) {
                    error = "Parameter '" + key + "' requires integer value";
                    return false;
                }
                try {
                    std::stoi(value);
                    return true;
                } catch (...) {
                    error = "Parameter '" + key + "' requires valid integer value, got: " + value;
                    return false;
                }
        }
    } catch (...) {
        error = "Error validating parameter: " + key;
        return false;
    }
}

void Config::addError(const std::string& key, const std::string& error) {
    pImpl->validationErrors.push_back("Key '" + key + "': " + error);
}

Config Config::createDefaultConfig() {
    Config config;
    return config;
}

Config Config::createExperimentConfig(uint64_t maxSteps, size_t neuronCount, size_t regionCount) {
    Config config;
    config.set("simulation.max_steps", static_cast<int64_t>(maxSteps));
    config.set("brain.neuron_count", static_cast<int64_t>(neuronCount));
    config.set("brain.region_count", static_cast<int64_t>(regionCount));
    config.set("plasticity.stdp.enable", true);
    config.set("plasticity.hebbian.enable", true);
    config.set("neuromod.curiosity.enable", true);
    config.set("development.enable", true);
    return config;
}

Config Config::createPerformanceConfig(size_t neuronCount, size_t regionCount, size_t connectionProbability) {
    Config config;
    config.set("brain.neuron_count", static_cast<int64_t>(neuronCount));
    config.set("brain.region_count", static_cast<int64_t>(regionCount));
    config.set("brain.connection_probability", static_cast<double>(connectionProbability) / 100.0);
    config.set("plasticity.stdp.enable", false); // Disable plasticity for performance
    config.set("neuromod.curiosity.enable", false); // Disable curiosity for performance
    return config;
}

Config Config::createResearchConfig(size_t neuronCount, bool enableDevelopment, bool enableCuriosity) {
    Config config;
    config.set("brain.neuron_count", static_cast<int64_t>(neuronCount));
    config.set("development.enable", enableDevelopment);
    config.set("neuromod.curiosity.enable", enableCuriosity);
    config.set("plasticity.structural.enable", true);
    config.set("neuromod.novelty.enable", true);
    return config;
}

std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

} // namespace nlm