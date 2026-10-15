#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <map>
#include <ctime>
#include <iomanip>
#include <numeric>

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, std::vector<std::string>> validationErrors;
};

Config::Config() : pImpl(std::make_unique<Impl>()), schema(std::make_shared<ConfigSchema>()) {
    // Create default schema
    setupDefaultSchema();
}

Config::Config(const std::shared_ptr<ConfigSchema>& newSchema) 
    : pImpl(std::make_unique<Impl>()), schema(newSchema) {
    if (!schema) {
        schema = std::make_shared<ConfigSchema>();
        setupDefaultSchema();
    }
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

void Config::setupDefaultSchema() {
    // Add common configuration parameters with validation
    
    // Random seed
    ConfigParameter seedParam;
    seedParam.name = "random_seed";
    seedParam.type = ConfigType::INT64;
    seedParam.defaultValue = int64_t(42);
    seedParam.description = "Random seed for reproducible results";
    seedParam.required = false;
    seedParam.numberRange.min = 0;
    seedParam.numberRange.max = std::numeric_limits<int64_t>::max();
    schema->addParameter(seedParam);
    
    // Simulation timestep
    ConfigParameter timestepParam;
    timestepParam.name = "simulation_timestep";
    timestepParam.type = ConfigType::DOUBLE;
    timestepParam.defaultValue = 0.001;
    timestepParam.description = "Time step for simulation in seconds";
    timestepParam.required = false;
    timestepParam.numberRange.min = 1e-6;
    timestepParam.numberRange.max = 1.0;
    timestepParam.numberRange.inclusive = true;
    schema->addParameter(timestepParam);
    
    // Neuron count
    ConfigParameter neuronCountParam;
    neuronCountParam.name = "neuron_count";
    neuronCountParam.type = ConfigType::INT64;
    neuronCountParam.defaultValue = int64_t(500);
    neuronCountParam.description = "Number of neurons in the brain";
    neuronCountParam.required = false;
    neuronCountParam.numberRange.min = 1;
    neuronCountParam.numberRange.max = 100000;
    schema->addParameter(neuronCountParam);
    
    // Region count
    ConfigParameter regionCountParam;
    regionCountParam.name = "region_count";
    regionCountParam.type = ConfigType::INT64;
    regionCountParam.defaultValue = int64_t(1);
    regionCountParam.description = "Number of brain regions";
    regionCountParam.required = false;
    regionCountParam.numberRange.min = 1;
    regionCountParam.numberRange.max = 10;
    schema->addParameter(regionCountParam);
    
    // Connection probability
    ConfigParameter connProbParam;
    connProbParam.name = "connection_probability";
    connProbParam.type = ConfigType::DOUBLE;
    connProbParam.defaultValue = 0.15;
    connProbParam.description = "Probability of forming connections between neurons";
    connProbParam.required = false;
    connProbParam.numberRange.min = 0.0;
    connProbParam.numberRange.max = 1.0;
    connProbParam.numberRange.inclusive = true;
    schema->addParameter(connProbParam);
    
    // STDP parameters
    ConfigParameter stdpLtpParam;
    stdpLtpParam.name = "stdp_ltp_weight";
    stdpLtpParam.type = ConfigType::DOUBLE;
    stdpLtpParam.defaultValue = 0.02;
    stdpLtpParam.description = "STDP LTP (Long-Term Potentiation) weight";
    stdpLtpParam.required = false;
    stdpLtpParam.numberRange.min = 0.0;
    stdpLtpParam.numberRange.max = 1.0;
    stdpLtpParam.numberRange.inclusive = true;
    schema->addParameter(stdpLtpParam);
    
    ConfigParameter stdpLtdParam;
    stdpLtdParam.name = "stdp_ltd_weight";
    stdpLtdParam.type = ConfigType::DOUBLE;
    stdpLtdParam.defaultValue = 0.015;
    stdpLtdParam.description = "STDP LTD (Long-Term Depression) weight";
    stdpLtdParam.required = false;
    stdpLtdParam.numberRange.min = 0.0;
    stdpLtdParam.numberRange.max = 1.0;
    stdpLtdParam.numberRange.inclusive = true;
    schema->addParameter(stdpLtdParam);
    
    ConfigParameter stdpTauParam;
    stdpTauParam.name = "stdp_tau";
    stdpTauParam.type = ConfigType::DOUBLE;
    stdpTauParam.defaultValue = 20.0;
    stdpTauParam.description = "STDP time constant in milliseconds";
    stdpTauParam.required = false;
    stdpTauParam.numberRange.min = 1.0;
    stdpTauParam.numberRange.max = 1000.0;
    stdpTauParam.numberRange.inclusive = true;
    schema->addParameter(stdpTauParam);
    
    // Structural plasticity
    ConfigParameter synaptogenesisParam;
    synaptogenesisParam.name = "synaptogenesis_rate";
    synaptogenesisParam.type = ConfigType::DOUBLE;
    synaptogenesisParam.defaultValue = 0.0001;
    synaptogenesisParam.description = "Rate of new synapse formation";
    synaptogenesisParam.required = false;
    synaptogenesisParam.numberRange.min = 0.0;
    synaptogenesisParam.numberRange.max = 1.0;
    synaptogenesisParam.numberRange.inclusive = true;
    schema->addParameter(synaptogenesisParam);
    
    ConfigParameter pruningParam;
    pruningParam.name = "pruning_rate";
    pruningParam.type = ConfigType::DOUBLE;
    pruningParam.defaultValue = 0.00001;
    pruningParam.description = "Rate of synapse elimination";
    pruningParam.required = false;
    pruningParam.numberRange.min = 0.0;
    pruningParam.numberRange.max = 1.0;
    pruningParam.numberRange.inclusive = true;
    schema->addParameter(pruningParam);
    
    // String parameters
    ConfigParameter brainNameParam;
    brainNameParam.name = "brain_name";
    brainNameParam.type = ConfigType::STRING;
    brainNameParam.defaultValue = std::string("NLM_Brain");
    brainNameParam.description = "Name identifier for the brain";
    brainNameParam.required = false;
    brainNameParam.stringConstraints.minLength = 1;
    brainNameParam.stringConstraints.maxLength = 100;
    brainNameParam.stringConstraints.pattern = "^[a-zA-Z0-9_]+$'";
    schema->addParameter(brainNameParam);
    
    ConfigParameter logLevelParam;
    logLevelParam.name = "log_level";
    logLevelParam.type = ConfigType::STRING;
    logLevelParam.defaultValue = std::string("Info");
    logLevelParam.description = "Logging level (Debug, Info, Warning, Error)";
    logLevelParam.required = false;
    logLevelParam.stringConstraints.allowedValues = {"Debug", "Info", "Warning", "Error"};
    schema->addParameter(logLevelParam);
}

bool Config::loadFromFile(const std::string& filepath) {
    // Try simple key=value format first
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    file.close();
    
    // Try to parse as key=value format
    return loadFromYAML(content);
}

bool Config::loadFromJSON(const std::string& jsonStr) {
    // Placeholder for JSON parsing - not implemented due to dependency issues
    // For now, fall back to key=value format
    return loadFromYAML(jsonStr);
}

bool Config::loadFromYAML(const std::string& yamlStr) {
    // Simple YAML-like parser for key: value format
    std::map<std::string, std::string> parsed;
    
    std::istringstream stream(yamlStr);
    std::string line;
    
    while (std::getline(stream, line)) {
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || line[0] == '%') {
            continue;
        }
        
        // Parse key: value format
        size_t pos = line.find(':');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            parsed[key] = value;
        } else {
            // Try key=value format as fallback
            std::string key, value;
            if (parseKeyValue(line, key, value)) {
                parsed[key] = value;
            }
        }
    }
    
    // Clear existing entries
    pImpl->entries.clear();
    pImpl->validationErrors.clear();
    
    bool success = true;
    for (const auto& pair : parsed) {
        if (!setFromString(pair.first, pair.second, ConfigSource::File)) {
            success = false;
        }
    }
    
    return success;
}

bool Config::loadFromArgs(int argc, char** argv) {
    bool success = true;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                if (!setFromString(key, value, ConfigSource::CommandLine)) {
                    success = false;
                }
            } else {
                // Handle --key value format
                if (i + 1 < argc) {
                    std::string key = arg.substr(2);
                    std::string value = argv[++i];
                    if (!setFromString(key, value, ConfigSource::CommandLine)) {
                        success = false;
                    }
                }
            }
        }
        // Handle --help flag
        else if (arg == "--help" || arg == "-h") {
            printHelp(schema);
            exit(0);
        }
        // Handle --examples flag
        else if (arg == "--examples" || arg == "-e") {
            printExamples();
            exit(0);
        }
    }
    
    return success;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Write as simple key=value format with comments
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Try to serialize to string representation
        std::string valueStr;
        if (serializeToJSON(entry, valueStr)) {
            file << valueStr;
        } else {
            file << "UNKNOWN_VALUE";
        }
        file << "\n\n";
    }
    
    return true;
}

std::string Config::saveToJSON() const {
    std::ostringstream oss;
    oss << "{\n";
    
    bool first = true;
    for (const auto& entry : pImpl->entries) {
        if (!first) {
            oss << ",\n";
        }
        first = false;
        
        oss << "  \"" << entry.key << "\": ";
        
        // Simple JSON-like serialization
        std::string valueStr;
        if (serializeToJSON(entry, valueStr)) {
            oss << valueStr;
        } else {
            oss << "\"UNKNOWN_VALUE\"";
        }
    }
    
    oss << "\n}\n";
    return oss.str();
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

bool Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    if (!schema) {
        // Create basic schema if none exists
        schema = std::make_shared<ConfigSchema>();
        setupDefaultSchema();
    }
    
    // Validate against schema
    std::string error;
    if (!schema->validate(key, value, error)) {
        pImpl->validationErrors[key].push_back(error);
        return false;
    }
    
    // Clear any previous validation errors for this key
    pImpl->validationErrors.erase(key);
    
    // Find existing entry
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        it->timestamp = getCurrentTimestamp();
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
    
    return true;
}

bool Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    return set(key, ConfigValue(value), source);
}

bool Config::set(const std::string& key, int value, ConfigSource source) {
    return set(key, ConfigValue(value), source);
}

bool Config::set(const std::string& key, int64_t value, ConfigSource source) {
    return set(key, ConfigValue(value), source);
}

bool Config::set(const std::string& key, double value, ConfigSource source) {
    return set(key, ConfigValue(value), source);
}

bool Config::set(const std::string& key, float value, ConfigSource source) {
    return set(key, ConfigValue(value), source);
}

bool Config::set(const std::string& key, bool value, ConfigSource source) {
    return set(key, ConfigValue(value), source);
}

bool Config::setFromString(const std::string& key, const std::string& value, ConfigSource source) {
    if (!schema) {
        schema = std::make_shared<ConfigSchema>();
        setupDefaultSchema();
    }
    
    const ConfigParameter* param = schema->getParameter(key);
    if (!param) {
        // Unknown parameter, allow it but don't validate (backward compatibility)
        return set(key, value, source);
    }
    
    // Convert string to appropriate type
    ConfigValue configValue;
    std::string error;
    
    if (!schema->convertString(key, value, configValue, error)) {
        pImpl->validationErrors[key].push_back(error);
        return false;
    }
    
    // Clear any previous validation errors for this key
    pImpl->validationErrors.erase(key);
    
    return set(key, configValue, source);
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
    pImpl->validationErrors.erase(key);
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
    pImpl->validationErrors.clear();
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
    
    // Add validation errors
    if (!pImpl->validationErrors.empty()) {
        oss << "\nValidation Errors:\n";
        for (const auto& pair : pImpl->validationErrors) {
            for (const auto& error : pair.second) {
                oss << "  " << pair.first << ": " << error << "\n";
            }
        }
    }
    
    return oss.str();
}

void Config::setSchema(const std::shared_ptr<ConfigSchema>& newSchema) {
    schema = newSchema;
}

bool Config::validateAll(std::vector<std::string>& errors) const {
    bool valid = true;
    
    for (const auto& entry : pImpl->entries) {
        std::vector<std::string> keyErrors = getValidationErrors(entry.key);
        if (!keyErrors.empty()) {
            valid = false;
            for (const auto& error : keyErrors) {
                errors.push_back(entry.key + ": " + error);
            }
        }
    }
    
    return valid;
}

std::vector<std::string> Config::getValidationErrors(const std::string& key) const {
    auto it = pImpl->validationErrors.find(key);
    if (it != pImpl->validationErrors.end()) {
        return it->second;
    }
    return std::vector<std::string>();
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

bool Config::isNumber(const std::string& str) {
    std::istringstream iss(str);
    double val;
    return (iss >> val) && iss.eof();
}

bool Config::isBool(const std::string& str) {
    std::string lower = toLower(str);
    return lower == "true" || lower == "false" || 
           lower == "yes" || lower == "no" ||
           lower == "on" || lower == "off" ||
           lower == "1" || lower == "0";
}

std::string Config::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

bool Config::parseKeyValue(const std::string& line, std::string& key, std::string& value) const {
    // Parse key=value format
    size_t pos = line.find('=');
    if (pos != std::string::npos) {
        key = trim(line.substr(0, pos));
        value = trim(line.substr(pos + 1));
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        return !key.empty() && !value.empty();
    }
    return false;
}

bool Config::serializeToJSON(const ConfigEntry& entry, std::string& outValue) const {
    try {
        std::visit([&outValue](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                outValue = arg;
            } else if constexpr (std::is_same_v<T, int>) {
                outValue = std::to_string(arg);
            } else if constexpr (std::is_same_v<T, int64_t>) {
                outValue = std::to_string(arg);
            } else if constexpr (std::is_same_v<T, double>) {
                outValue = std::to_string(arg);
            } else if constexpr (std::is_same_v<T, float>) {
                outValue = std::to_string(arg);
            } else if constexpr (std::is_same_v<T, bool>) {
                outValue = arg ? "true" : "false";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                std::ostringstream oss;
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ",";
                    oss << arg[i];
                }
                outValue = oss.str();
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                std::ostringstream oss;
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ",";
                    oss << arg[i];
                }
                outValue = oss.str();
            } else if constexpr (std::is_same_v<T, std::vector<float>>) {
                std::ostringstream oss;
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ",";
                    oss << arg[i];
                }
                outValue = oss.str();
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                std::ostringstream oss;
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ",";
                    oss << "\"" << arg[i] << "\"";
                }
                outValue = oss.str();
            }
        }, entry.value);
        
        return true;
    } catch (...) {
        return false;
    }
}

ConfigValue Config::convertStringToType(const std::string& str, ConfigType type) const {
    std::istringstream iss(str);
    
    switch (type) {
        case ConfigType::INT: {
            int val;
            if (iss >> val) return val;
            break;
        }
        case ConfigType::INT64: {
            int64_t val;
            if (iss >> val) return val;
            break;
        }
        case ConfigType::DOUBLE: {
            double val;
            if (iss >> val) return val;
            break;
        }
        case ConfigType::FLOAT: {
            float val;
            if (iss >> val) return val;
            break;
        }
        case ConfigType::BOOL: {
            bool val;
            std::string lower = toLower(str);
            if (lower == "true" || lower == "1" || lower == "yes" || lower == "on") {
                return true;
            } else if (lower == "false" || lower == "0" || lower == "no" || lower == "off") {
                return false;
            }
            break;
        }
        case ConfigType::STRING: {
            return str;
        }
        case ConfigType::VECTOR_INT: {
            std::vector<int> result;
            std::string token;
            std::stringstream ss(str);
            while (std::getline(ss, token, ',')) {
                token = trim(token);
                int val;
                if (std::istringstream(token) >> val) {
                    result.push_back(val);
                }
            }
            return result;
        }
        case ConfigType::VECTOR_DOUBLE: {
            std::vector<double> result;
            std::string token;
            std::stringstream ss(str);
            while (std::getline(ss, token, ',')) {
                token = trim(token);
                double val;
                if (std::istringstream(token) >> val) {
                    result.push_back(val);
                }
            }
            return result;
        }
        case ConfigType::VECTOR_STRING: {
            std::vector<std::string> result;
            std::string token;
            std::stringstream ss(str);
            while (std::getline(ss, token, ',')) {
                result.push_back(trim(token));
            }
            return result;
        }
    }
    
    // Return empty string if conversion fails
    return std::string();
}

void Config::printHelp(const std::shared_ptr<ConfigSchema>& schema) {
    std::cout << "Config System - Help" << std::endl;
    std::cout << "====================" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "  --key=value         Set a configuration parameter" << std::endl;
    std::cout << "  --key value         Set a configuration parameter (alternative syntax)" << std::endl;
    std::cout << "  --config=file.cfg   Load configuration from file" << std::endl;
    std::cout << "  --help              Show this help message" << std::endl;
    std::cout << "  --examples          Show configuration examples" << std::endl;
    std::cout << std::endl;
    
    if (schema) {
        std::cout << "Available Parameters:" << std::endl;
        for (const auto& pair : schema->getParameters()) {
            const auto& param = pair.second;
            std::cout << "  " << pair.first << " (type: " << ConfigSchema::getTypeName(param.type);
            
            if (param.required) {
                std::cout << ", required";
            }
            
            if (param.defaultValue.has_value()) {
                std::cout << ", default: ";
                if (param.type == ConfigType::STRING) {
                    std::cout << "\"" << std::any_cast<std::string>(param.defaultValue) << "\"";
                } else {
                    std::cout << std::any_cast<double>(param.defaultValue);
                }
            }
            
            std::cout << ") - " << param.description << std::endl;
        }
    } else {
        std::cout << "No schema available - parameters are not validated." << std::endl;
    }
}

void Config::printExamples() {
    std::cout << "Configuration Examples" << std::endl;
    std::cout << "=====================" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Example 1: JSON Configuration" << std::endl;
    std::cout << "{\n";
    std::cout << "  \"random_seed\": 123,\n";
    std::cout << "  \"simulation_timestep\": 0.001,\n";
    std::cout << "  \"neuron_count\": 1000,\n";
    std::cout << "  \"connection_probability\": 0.2,\n";
    std::cout << "  \"stdp_ltp_weight\": 0.03,\n";
    std::cout << "  \"brain_name\": \"MyBrain\"\n";
    std::cout << "}" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Example 2: Simple Config File" << std::endl;
    std::cout << "# NLM Configuration File\n";
    std::cout << "random_seed = 456\n";
    std::cout << "simulation_timestep = 0.01\n";
    std::cout << "neuron_count = 2000\n";
    std::cout << "# connection_probability = 0.1  # commented out\n";
    std::cout << "stdp_ltp_weight = 0.025" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Example 3: Command Line Arguments" << std::endl;
    std::cout << "./app --random_seed=789 --simulation_timestep=0.005 --neuron_count=500" << std::endl;
    std::cout << "./app --brain_name=Production --connection_probability=0.3" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Example 4: Vector Values" << std::endl;
    std::cout << "./app --vector_param=1,2,3,4,5\n";
    std::cout << "./app --vector_param=0.1,0.2,0.3" << std::endl;
    std::cout << std::endl;
}

std::shared_ptr<Config> Config::createDefaultConfig() {
    auto config = std::make_shared<Config>();
    
    // Set default values
    config->set("random_seed", int64_t(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", int64_t(500), ConfigSource::Default);
    config->set("region_count", int64_t(1), ConfigSource::Default);
    config->set("connection_probability", 0.15, ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.02, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015, ConfigSource::Default);
    config->set("stdp_tau", 20.0, ConfigSource::Default);
    config->set("synaptogenesis_rate", 0.0001, ConfigSource::Default);
    config->set("pruning_rate", 0.00001, ConfigSource::Default);
    config->set("brain_name", std::string("NLM_Brain"), ConfigSource::Default);
    config->set("log_level", std::string("Info"), ConfigSource::Default);
    
    return config;
}

std::shared_ptr<Config> Config::createMinimalConfig() {
    auto config = std::make_shared<Config>();
    
    // Minimal set of parameters
    config->set("random_seed", int64_t(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", int64_t(100), ConfigSource::Default);
    
    return config;
}

std::shared_ptr<Config> Config::createExpertConfig() {
    auto config = std::make_shared<Config>();
    
    // Expert configuration with all parameters tuned
    config->set("random_seed", int64_t(12345), ConfigSource::Default);
    config->set("simulation_timestep", 0.0005, ConfigSource::Default);
    config->set("neuron_count", int64_t(2000), ConfigSource::Default);
    config->set("region_count", int64_t(3), ConfigSource::Default);
    config->set("connection_probability", 0.25, ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.03, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.025, ConfigSource::Default);
    config->set("stdp_tau", 15.0, ConfigSource::Default);
    config->set("synaptogenesis_rate", 0.0005, ConfigSource::Default);
    config->set("pruning_rate", 0.00005, ConfigSource::Default);
    config->set("brain_name", std::string("ExpertBrain"), ConfigSource::Default);
    config->set("log_level", std::string("Debug"), ConfigSource::Default);
    
    // Expert-specific parameters
    config->set("max_synapses_per_neuron", int64_t(1000), ConfigSource::Default);
    config->set("plasticity_threshold", 0.5, ConfigSource::Default);
    config->set("reward_decay", 0.95, ConfigSource::Default);
    
    return config;
}

} // namespace nlm