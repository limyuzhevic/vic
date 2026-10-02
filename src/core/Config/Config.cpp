#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, std::string> parameterDescriptions;
    std::unordered_map<std::string, ParameterValidation> parameterValidations;
    std::vector<ParameterSpec> parameterSpecs;
    std::vector<std::string> validationErrors;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
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
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << toString(entry.value) << "\n";
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
    auto validationIt = pImpl->parameterValidations.find(key);
    if (validationIt != pImpl->parameterValidations.end()) {
        const auto& validation = validationIt->second;
        if (!validation.validate(value)) {
            pImpl->validationErrors.push_back("Validation failed for parameter '" + key + "'");
            return;
        }
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        // Check if we have a description for this key
        auto descIt = pImpl->parameterDescriptions.find(key);
        const std::string& description = (descIt != pImpl->parameterDescriptions.end()) ? descIt->second : "";
        
        ConfigEntry newEntry(key, value, source, description, 
                             (validationIt != pImpl->parameterValidations.end()) ? validationIt->second : ParameterValidation());
        pImpl->entries.emplace_back(std::move(newEntry));
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    auto parsedValue = parseValue(value);
    if (parsedValue.has_value()) {
        set(key, parsedValue.value(), source);
    } else {
        pImpl->validationErrors.push_back("Could not parse value for parameter '" + key + "': '" + value + "'");
    }
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
    pImpl->parameterDescriptions.erase(key);
    pImpl->parameterValidations.erase(key);
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
    pImpl->parameterDescriptions.clear();
    pImpl->parameterValidations.clear();
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
        if (!entry.description.empty()) {
            oss << "      # " << entry.description << "\n";
        }
    }
    return oss.str();
}

bool Config::validateConfiguration() const {
    pImpl->validationErrors.clear();
    
    for (const auto& entry : pImpl->entries) {
        auto validationIt = pImpl->parameterValidations.find(entry.key);
        if (validationIt != pImpl->parameterValidations.end()) {
            if (!validationIt->second.validate(entry.value)) {
                pImpl->validationErrors.push_back("Validation failed for parameter '" + entry.key + "'");
            }
        }
    }
    
    return pImpl->validationErrors.empty();
}

std::vector<std::string> Config::getValidationErrors() const {
    return pImpl->validationErrors;
}

void Config::setParameterDescription(const std::string& key, const std::string& description) {
    pImpl->parameterDescriptions[key] = description;
}

std::string Config::getParameterDescription(const std::string& key) const {
    auto it = pImpl->parameterDescriptions.find(key);
    return (it != pImpl->parameterDescriptions.end()) ? it->second : "";
}

std::vector<std::string> Config::getAllParameterNames() const {
    std::vector<std::string> names;
    for (const auto& spec : pImpl->parameterSpecs) {
        names.push_back(spec.key);
    }
    return names;
}

void Config::addParameterValidation(const std::string& key, const ParameterValidation& validation) {
    pImpl->parameterValidations[key] = validation;
}

void Config::initializeStandardParameters() {
    // Register common parameters with validation and descriptions
    
    // Random seed
    setParameterDescription("random_seed", "Random seed for reproducible experiments");
    ParameterValidation seedValidation;
    seedValidation.rule = ValidationRule::Range;
    seedValidation.minValue = 0.0;
    seedValidation.maxValue = 2147483647.0;  // Max 32-bit int
    seedValidation.errorMessage = "Random seed must be a positive integer";
    addParameterValidation("random_seed", seedValidation);
    set("random_seed", 42, ConfigSource::Default);
    
    // Simulation timestep
    setParameterDescription("simulation_timestep", "Simulation timestep in seconds (smaller = more accurate but slower)");
    ParameterValidation timestepValidation;
    timestepValidation.rule = ValidationRule::Range;
    timestepValidation.minValue = 0.0001;
    timestepValidation.maxValue = 0.1;
    timestepValidation.errorMessage = "Timestep must be between 0.0001s and 0.1s";
    addParameterValidation("simulation_timestep", timestepValidation);
    set("simulation_timestep", 0.001, ConfigSource::Default);
    
    // Neuron count
    setParameterDescription("neuron_count", "Number of neurons in the brain");
    ParameterValidation neuronCountValidation;
    neuronCountValidation.rule = ValidationRule::Range;
    neuronCountValidation.minValue = 10;
    neuronCountValidation.maxValue = 100000;
    neuronCountValidation.errorMessage = "Neuron count must be between 10 and 100,000";
    addParameterValidation("neuron_count", neuronCountValidation);
    set("neuron_count", 1000, ConfigSource::Default);
    
    // Region count
    setParameterDescription("region_count", "Number of brain regions");
    ParameterValidation regionCountValidation;
    regionCountValidation.rule = ValidationRule::Range;
    regionCountValidation.minValue = 1;
    regionCountValidation.maxValue = 10;
    regionCountValidation.errorMessage = "Region count must be between 1 and 10";
    addParameterValidation("region_count", regionCountValidation);
    set("region_count", 2, ConfigSource::Default);
    
    // Connection probability
    setParameterDescription("connection_probability", "Probability of connection between neurons (0.0 to 1.0)");
    ParameterValidation connProbValidation;
    connProbValidation.rule = ValidationRule::Range;
    connProbValidation.minValue = 0.0;
    connProbValidation.maxValue = 1.0;
    connProbValidation.errorMessage = "Connection probability must be between 0.0 and 1.0";
    addParameterValidation("connection_probability", connProbValidation);
    set("connection_probability", 0.1, ConfigSource::Default);
    
    // STDP parameters
    setParameterDescription("stdp_ltp_weight", "STDP Long-Term Potentiation weight (positive)");
    ParameterValidation stdpLtpValidation;
    stdpLtpValidation.rule = ValidationRule::Positive;
    stdpLtpValidation.errorMessage = "STDP LTP weight must be positive";
    addParameterValidation("stdp_ltp_weight", stdpLtpValidation);
    set("stdp_ltp_weight", 0.01, ConfigSource::Default);
    
    setParameterDescription("stdp_ltd_weight", "STDP Long-Term Depression weight (positive)");
    ParameterValidation stdpLtdValidation;
    stdpLtdValidation.rule = ValidationRule::Positive;
    stdpLtdValidation.errorMessage = "STDP LTD weight must be positive";
    addParameterValidation("stdp_ltd_weight", stdpLtdValidation);
    set("stdp_ltd_weight", 0.012, ConfigSource::Default);
    
    // Structural plasticity
    setParameterDescription("synaptogenesis_rate", "Rate of new synapse formation");
    ParameterValidation synaptogenesisValidation;
    synaptogenesisValidation.rule = ValidationRule::Range;
    synaptogenesisValidation.minValue = 0.0;
    synaptogenesisValidation.maxValue = 0.1;
    synaptogenesisValidation.errorMessage = "Synaptogenesis rate must be between 0.0 and 0.1";
    addParameterValidation("synaptogenesis_rate", synaptogenesisValidation);
    set("synaptogenesis_rate", 0.001, ConfigSource::Default);
    
    setParameterDescription("pruning_rate", "Rate of synapse elimination");
    ParameterValidation pruningValidation;
    pruningValidation.rule = ValidationRule::Range;
    pruningValidation.minValue = 0.0;
    pruningValidation.maxValue = 0.01;
    pruningValidation.errorMessage = "Pruning rate must be between 0.0 and 0.01";
    addParameterValidation("pruning_rate", pruningValidation);
    set("pruning_rate", 0.0001, ConfigSource::Default);
    
    // Neuromodulation
    setParameterDescription("dopamine_baseline", "Baseline dopamine level (0.0 to 1.0)");
    ParameterValidation dopamineValidation;
    dopamineValidation.rule = ValidationRule::Range;
    dopamineValidation.minValue = 0.0;
    dopamineValidation.maxValue = 1.0;
    dopamineValidation.errorMessage = "Dopamine baseline must be between 0.0 and 1.0";
    addParameterValidation("dopamine_baseline", dopamineValidation);
    set("dopamine_baseline", 0.1, ConfigSource::Default);
    
    // Environment settings
    setParameterDescription("environment_name", "Name of the environment");
    ParameterValidation envNameValidation;
    envNameValidation.rule = ValidationRule::Enum;
    envNameValidation.allowedValues = {"GridWorld", "ContinuousWorld", "NestedWorld"};
    envNameValidation.errorMessage = "Environment name must be one of: GridWorld, ContinuousWorld, NestedWorld";
    addParameterValidation("environment_name", envNameValidation);
    set("environment_name", "GridWorld", ConfigSource::Default);
    
    setParameterDescription("environment_width", "Width of the environment");
    ParameterValidation widthValidation;
    widthValidation.rule = ValidationRule::Range;
    widthValidation.minValue = 5;
    widthValidation.maxValue = 100;
    widthValidation.errorMessage = "Environment width must be between 5 and 100";
    addParameterValidation("environment_width", widthValidation);
    set("environment_width", 10, ConfigSource::Default);
    
    setParameterDescription("environment_height", "Height of the environment");
    ParameterValidation heightValidation;
    heightValidation.rule = ValidationRule::Range;
    heightValidation.minValue = 5;
    heightValidation.maxValue = 100;
    heightValidation.errorMessage = "Environment height must be between 5 and 100";
    addParameterValidation("environment_height", heightValidation);
    set("environment_height", 10, ConfigSource::Default);
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

std::string Config::toString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        oss << arg;
    }, value);
    return oss.str();
}

std::optional<ConfigValue> Config::parseValue(const std::string& str) {
    // Try to parse as different types
    try {
        // Try integer
        size_t pos;
        int intVal = std::stoi(str, &pos);
        if (pos == str.size()) {
            return intVal;
        }
    } catch (...) {}
    
    try {
        // Try double
        size_t pos;
        double doubleVal = std::stod(str, &pos);
        if (pos == str.size()) {
            return doubleVal;
        }
    } catch (...) {}
    
    try {
        // Try boolean
        if (str == "true" || str == "1") {
            return true;
        } else if (str == "false" || str == "0") {
            return false;
        }
    } catch (...) {}
    
    // Return as string if all else fails
    return str;
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
