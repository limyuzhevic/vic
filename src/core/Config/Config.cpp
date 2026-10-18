#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <map>
#include <set>
#include <numeric>
#include <cmath>
#include <iomanip>
#include <chrono>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()),
                   nextCallbackId_(0) {
    initializeValidationSystem();
    setupParameterSchemas();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

void Config::initializeValidationSystem() {
    resetValidationStats();
    lastValidationResult_.valid = true;
}

void Config::setupParameterSchemas() {
    // Set up default parameter schemas
    addParameterDefinition({
        "neuron_count", "int", 
        "Number of neurons in the brain",
        ConfigSource::Default, true, 1, 10000000,
        "neurons", "must be positive integer"
    });
    
    addParameterDefinition({
        "synapse_density", "double",
        "Connection probability for random connectivity",
        ConfigSource::Default, false, 0.0, 1.0,
        "probability", "must be between 0.0 and 1.0"
    });
    
    addParameterDefinition({
        "simulation_timestep", "double",
        "Simulation timestep in seconds",
        ConfigSource::Default, false, 0.000001, 0.1,
        "seconds", "must be positive and reasonable"
    });
    
    addParameterDefinition({
        "random_seed", "uint64_t",
        "Random seed for reproducible experiments",
        ConfigSource::Default, false, 0, std::numeric_limits<uint64_t>::max(),
        "seed", "non-negative integer"
    });
    
    addParameterDefinition({
        "region_count", "int",
        "Number of brain regions",
        ConfigSource::Default, false, 1, 1000,
        "regions", "positive integer"
    });
    
    addParameterDefinition({
        "stdp_ltp_weight", "float",
        "STDP LTP weight",
        ConfigSource::Default, false, 0.0, 10.0,
        "weight", "positive weight for LTP"
    });
    
    addParameterDefinition({
        "stdp_ltd_weight", "float",
        "STDP LTD weight",
        ConfigSource::Default, false, 0.0, 10.0,
        "weight", "positive weight for LTD"
    });
    
    addParameterDefinition({
        "stdp_time_constant", "float",
        "STDP time constant",
        ConfigSource::Default, false, 1.0, 1000.0,
        "ms", "time constant in milliseconds"
    });
    
    addParameterDefinition({
        "development_synaptogenesis_rate", "float",
        "Development synaptogenesis rate",
        ConfigSource::Default, false, 0.0, 1.0,
        "rate", "rate between 0.0 and 1.0"
    });
    
    addParameterDefinition({
        "development_pruning_rate", "float",
        "Development pruning rate",
        ConfigSource::Default, false, 0.0, 1.0,
        "rate", "rate between 0.0 and 1.0"
    });
}

bool Config::addParameterDefinition(const ParameterDefinition& definition) {
    if (definition.name.empty()) {
        return false;
    }
    
    // Convert ParameterDefinition to lowercase for case-insensitive lookup
    std::string normalizedName = toLower(definition.name);
    
    if (parameterSchemas_.find(normalizedName) != parameterSchemas_.end()) {
        // Update existing definition
        parameterSchemas_[normalizedName] = definition;
    } else {
        // Add new definition
        parameterSchemas_[normalizedName] = definition;
    }
    
    return true;
}

std::optional<ParameterDefinition> Config::getParameterInfo(const std::string& key) const {
    auto it = parameterSchemas_.find(toLower(key));
    if (it != parameterSchemas_.end()) {
        return it->second;
    }
    return std::nullopt;
}

bool Config::validateParameterMetadata() const {
    ValidationResult result;
    
    for (const auto& pair : parameterSchemas_) {
        const auto& def = pair.second;
        
        if (def.name.empty()) {
            result.errors.emplace_back(ValidationErrorCategory::SchemaError, 
                                     "", "Parameter name cannot be empty",
                                     "Provide a valid parameter name");
            result.valid = false;
            continue;
        }
        
        if (def.type.empty()) {
            result.errors.emplace_back(ValidationErrorCategory::SchemaError,
                                     def.name, "Parameter type cannot be empty",
                                     "Specify a valid type (int, double, string, bool)");
            result.valid = false;
            continue;
        }
        
        // Validate type
        if (def.type != "int" && def.type != "double" && def.type != "string" &&
            def.type != "bool" && def.type != "uint64_t") {
            result.errors.emplace_back(ValidationErrorCategory::SchemaError,
                                     def.name, "Invalid parameter type: " + def.type,
                                     "Use one of: int, double, string, bool, uint64_t");
            result.valid = false;
        }
        
        if (def.required && def.defaultSource == ConfigSource::Default) {
            result.warnings.push_back("Required parameter '" + def.name + "' has no default value");
        }
        
        if (def.minValue > def.maxValue) {
            result.errors.emplace_back(ValidationErrorCategory::SchemaError,
                                     def.name, "minValue must be <= maxValue",
                                     "Swap minValue and maxValue");
            result.valid = false;
        }
    }
    
    lastValidationResult_ = result;
    return result.valid;
}

std::string Config::exportSchema() const {
    std::ostringstream oss;
    oss << "{\n\"parameters\": [\n";
    
    size_t count = 0;
    for (const auto& pair : parameterSchemas_) {
        const auto& def = pair.second;
        
        oss << "  {\n";
        oss << "    \"name\": \"" << def.name << "\",\n";
        oss << "    \"type\": \"" << def.type << "\",\n";
        oss << "    \"description\": \"" << def.description << "\",\n";
        oss << "    \"required\": " << (def.required ? "true" : "false") << ",\n";
        oss << "    \"minValue\": " << def.minValue << ",\n";
        oss << "    \"maxValue\": " << def.maxValue << ",\n";
        oss << "    \"unit\": \"" << def.unit << "\",\n";
        oss << "    \"validationRule\": \"" << def.validationRule << "\"\n";
        
        if (count < parameterSchemas_.size() - 1) {
            oss << "  },\n";
        } else {
            oss << "  }\n";
        }
        
        count++;
    }
    
    oss << "],\n\"metadata\": {\n";
    oss << "  \"totalParameters\": " << parameterSchemas_.size() << ",\n";
    oss << "  \"schemaVersion\": \"1.0\"\n";
    oss << "}\n}\n";
    
    return oss.str();
}

bool Config::loadFromFile(const std::string& filepath) {
    if (!validateFilePath(filepath)) {
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    setConfigPath(filepath);
    
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
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
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
std::optional<T> Config::getAs(const std::string& key) const {
    // First try direct get
    auto val = get<T>(key);
    if (val.has_value()) {
        return val;
    }
    
    // If not found or wrong type, check parameter schema
    auto paramInfo = getParameterInfo(key);
    if (!paramInfo.has_value() || paramInfo->type != typeid(T).name()) {
        return std::nullopt;
    }
    
    // For now, return nullopt if we can't convert
    // In a full implementation, we would attempt conversion
    return std::nullopt;
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

template<typename T>
void Config::setWithValidation(const std::string& key, const T& value, ConfigSource source) {
    ValidationResult result = validateParameterChange(key, ConfigValue(value), source);
    
    if (!result.valid) {
        // Create ValidationException with detailed error information
        ValidationException ex(result);
        throw ex;
    }
    
    // Record old value for history
    ConfigValue oldValue;
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        oldValue = it->value;
    }
    
    // Set the new value
    set(key, ConfigValue(value), source);
    
    // Add to history
    ParameterChangeEvent event;
    event.parameter = key;
    event.oldValue = oldValue;
    event.newValue = ConfigValue(value);
    event.source = source;
    event.validated = true;
    
    // Call callbacks
    for (auto& callback : changeCallbacks_) {
        callback(event);
    }
    
    // Add to history map
    parameterHistory_[key].push_back(event);
    
    // Update last validation result
    lastValidationResult_ = result;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate the change before applying
    ValidationResult result = validateParameterChange(key, value, source);
    
    // Create ConfigEntry
    ConfigEntry entry(key, value, source, getParameterInfo(key).value_or(ParameterDefinition()).description);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.push_back(entry);
    }
    
    // Record parameter change for history
    ParameterChangeEvent changeEvent;
    changeEvent.parameter = key;
    changeEvent.newValue = value;
    changeEvent.source = source;
    changeEvent.validated = result.valid;
    if (!result.valid) {
        changeEvent.errors = result.errors;
    }
    
    // Call callbacks
    for (auto& callback : changeCallbacks_) {
        callback(changeEvent);
    }
    
    // Add to history
    parameterHistory_[key].push_back(changeEvent);
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

template<typename T>
bool Config::ensureType(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return false;
    }
    
    try {
        std::get<T>(it->value);
        return true;
    } catch (const std::bad_variant_access&) {
        return false;
    }
}

template<typename T>
bool Config::convertType(const std::string& key, const ConfigValue& newValue) {
    // First validate the new value
    ValidationResult result = validateParameterChange(key, newValue, ConfigSource::Runtime);
    if (!result.valid) {
        return false;
    }
    
    // Try to convert
    try {
        T converted = std::get<T>(newValue);
        set(key, newValue, ConfigSource::Runtime);
        return true;
    } catch (const std::bad_variant_access&) {
        // Try string-based conversion for numeric types
        std::string newStr;
        try {
            newStr = std::get<std::string>(newValue);
            if constexpr (std::is_same_v<T, int>) {
                try {
                    size_t pos;
                    int val = std::stoi(newStr, &pos);
                    if (pos == newStr.size()) {
                        set(key, val, ConfigSource::Runtime);
                        return true;
                    }
                } catch (...) {}
            } else if constexpr (std::is_same_v<T, double>) {
                try {
                    size_t pos;
                    double val = std::stod(newStr, &pos);
                    if (pos == newStr.size()) {
                        set(key, val, ConfigSource::Runtime);
                        return true;
                    }
                } catch (...) {}
            }
        } catch (...) {}
    }
    
    return false;
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
    
    // Clear history for removed key
    parameterHistory_.erase(key);
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
    parameterHistory_.clear();
}

std::string Config::getConfigString() const {
    std::ostringstream oss;
    oss << "{\n";
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        oss << "  \"" << entry.key << "\": ";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, double>) {
                oss << std::fixed << std::setprecision(6) << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    if (j > 0) oss << ", ";
                    oss << arg[j];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    if (j > 0) oss << ", ";
                    oss << std::fixed << std::setprecision(6) << arg[j];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    if (j > 0) oss << ", ";
                    oss << "\"" << arg[j] << "\"";
                }
                oss << "]";
            } else {
                oss << "null";
            }
        }, entry.value);
        if (i < pImpl->entries.size() - 1) {
            oss << ",\n";
        } else {
            oss << "\n";
        }
    }
    oss << "}\n";
    return oss.str();
}

bool Config::validateNeuronCount() const {
    auto neuronCountOpt = get<size_t>("neuron_count");
    if (!neuronCountOpt) {
        return false;
    }
    
    size_t neuronCount = *neuronCountOpt;
    return neuronCount > 0 && neuronCount <= 10000000;
}

bool Config::validateSynapseDensity() const {
    auto densityOpt = get<double>("connection_probability");
    if (!densityOpt) {
        return false;
    }
    
    double density = *densityOpt;
    return density >= 0.0 && density <= 1.0;
}

bool Config::validateTimeParameters() const {
    auto timestepOpt = get<double>("simulation_timestep");
    if (!timestepOpt) {
        return false;
    }
    
    double timestep = *timestepOpt;
    return timestep > 0.0 && timestep <= 0.1;
}

bool Config::validatePlasticityParameters() const {
    auto ltpWeightOpt = get<double>("stdp_ltp_weight");
    auto ltdWeightOpt = get<double>("stdp_ltd_weight");
    auto timeConstantOpt = get<double>("stdp_time_constant");
    
    if (!ltpWeightOpt || !ltdWeightOpt || !timeConstantOpt) {
        return false;
    }
    
    return ltpWeightOpt >= 0.0 && ltdWeightOpt >= 0.0 && timeConstantOpt > 0.0;
}

bool Config::validateNeuromodulationParameters() const {
    // Check for neuromodulation parameters in configs
    bool hasDopamine = has("dopamine_baseline");
    bool hasCuriosity = has("curiosity_baseline");
    
    if (hasDopamine) {
        auto dopamineOpt = get<double>("dopamine_baseline");
        if (!dopamineOpt) return false;
        // Could add additional validation here
    }
    
    if (hasCuriosity) {
        auto curiosityOpt = get<double>("curiosity_baseline");
        if (!curiosityOpt) return false;
        // Could add additional validation here
    }
    
    return true;
}

bool Config::validateDevelopmentParameters() const {
    auto synaptogenesisOpt = get<double>("development_synaptogenesis_rate");
    auto pruningOpt = get<double>("development_pruning_rate");
    
    if (!synaptogenesisOpt || !pruningOpt) {
        return false;
    }
    
    return synaptogenesisOpt >= 0.0 && synaptogenesisOpt <= 1.0 &&
           pruningOpt >= 0.0 && pruningOpt <= 1.0;
}

bool Config::validateConsistency() const {
    // Check consistency between neuron_count and region_count
    auto neuronCountOpt = get<size_t>("neuron_count");
    auto regionCountOpt = get<size_t>("region_count");
    
    if (neuronCountOpt && regionCountOpt) {
        // Neurons per region should be reasonable
        size_t neuronsPerRegion = *neuronCountOpt / *regionCountOpt;
        if (neuronsPerRegion < 10) {
            return false;
        }
    }
    
    // Check that connection probability is consistent with expected connectivity
    auto densityOpt = get<double>("connection_probability");
    auto neuronCountOpt2 = get<size_t>("neuron_count");
    
    if (densityOpt && neuronCountOpt2) {
        double expectedConnections = *neuronCountOpt2 * (*neuronCountOpt2 - 1) * (*densityOpt) / 2.0;
        if (expectedConnections > 1000000) { // 1M connections is a reasonable upper bound for initial testing
            return false;
        }
    }
    
    return true;
}

bool Config::validateCompleteness() const {
    // Check that all required parameters are present
    std::set<std::string> requiredParams = {"neuron_count", "region_count"};
    
    for (const auto& param : requiredParams) {
        if (!has(param)) {
            return false;
        }
    }
    
    return true;
}

ValidationResult Config::validateConfig() const {
    ValidationResult result;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Validate individual parameters
    if (!validateNeuronCount()) {
        result.valid = false;
        ValidationError error(ValidationErrorCategory::RangeError,
                            "neuron_count",
                            "Neuron count must be between 1 and 10,000,000",
                            "Set neuron_count to a reasonable value (e.g., 1000)",
                            0.0, 1.0, 10000000.0);
        result.errors.push_back(error);
    }
    
    if (!validateSynapseDensity()) {
        result.valid = false;
        ValidationError error(ValidationErrorCategory::RangeError,
                            "connection_probability",
                            "Connection probability must be between 0.0 and 1.0",
                            "Set connection_probability to a value between 0.0 and 1.0",
                            0.0, 0.0, 1.0);
        result.errors.push_back(error);
    }
    
    if (!validateTimeParameters()) {
        result.valid = false;
        ValidationError error(ValidationErrorCategory::RangeError,
                            "simulation_timestep",
                            "Time step must be between 0.000001 and 0.1 seconds",
                            "Set simulation_timestep to a smaller positive value",
                            0.0, 0.000001, 0.1);
        result.errors.push_back(error);
    }
    
    // Check plasticity parameters
    auto ltpWeightOpt = get<double>("stdp_ltp_weight");
    auto ltdWeightOpt = get<double>("stdp_ltd_weight");
    auto timeConstantOpt = get<double>("stdp_time_constant");
    
    if (ltpWeightOpt && (ltpWeightOpt < 0.0 || ltpWeightOpt > 10.0)) {
        result.valid = false;
        ValidationError error(ValidationErrorCategory::RangeError,
                            "stdp_ltp_weight",
                            "STDP LTP weight must be between 0.0 and 10.0",
                            "Set stdp_ltp_weight to a value between 0.0 and 10.0",
                            ltpWeightOpt.value(), 0.0, 10.0);
        result.errors.push_back(error);
    }
    
    if (ltdWeightOpt && (ltdWeightOpt < 0.0 || ltdWeightOpt > 10.0)) {
        result.valid = false;
        ValidationError error(ValidationErrorCategory::RangeError,
                            "stdp_ltd_weight",
                            "STDP LTD weight must be between 0.0 and 10.0",
                            "Set stdp_ltd_weight to a value between 0.0 and 10.0",
                            ltdWeightOpt.value(), 0.0, 10.0);
        result.errors.push_back(error);
    }
    
    if (timeConstantOpt && timeConstantOpt <= 0.0) {
        result.valid = false;
        ValidationError error(ValidationErrorCategory::RangeError,
                            "stdp_time_constant",
                            "STDP time constant must be positive",
                            "Set stdp_time_constant to a positive value",
                            timeConstantOpt.value(), 0.0, 0.0);
        result.errors.push_back(error);
    }
    
    // Check consistency
    if (!validateConsistency()) {
        result.valid = false;
        result.warnings.push_back("Configuration values may be inconsistent (e.g., too many neurons for regions)");
    }
    
    // Check completeness
    if (!validateCompleteness()) {
        result.valid = false;
        result.warnings.push_back("Missing required configuration parameters");
    }
    
    // Update validation stats
    validationStats_.totalValidations++;
    if (result.valid) {
        validationStats_.successfulValidations++;
    } else {
        validationStats_.failedValidations++;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    result.validationTimeMs = static_cast<double>(duration);
    result.validatedParameters = pImpl->entries.size();
    
    lastValidationResult_ = result;
    return result;
}

ValidationResult Config::getValidationReport() const {
    ValidationResult result = validateConfig();
    
    // Add summary information
    std::string summary = "Validation " + std::string(result.valid ? "PASSED" : "FAILED") +
                         ": " + std::to_string(result.errors.size()) + " errors, " +
                         std::to_string(result.warnings.size()) + " warnings";
    
    // Add parameter count
    result.warnings.push_back("Validated " + std::to_string(result.validatedParameters) + 
                             " configuration parameters");
    
    return result;
}

bool Config::validateAgainstSchema() const {
    // First validate parameter metadata
    bool metadataValid = validateParameterMetadata();
    if (!metadataValid) {
        return false;
    }
    
    // Then validate consistency
    return validateConsistency() && validateCompleteness();
}

bool Config::validateFilePath(const std::string& filepath) const {
    if (filepath.empty()) {
        return false;
    }
    
    // Check if file exists and is accessible
    if (!validateFilePermissions(filepath)) {
        return false;
    }
    
    // Check file extension
    if (!validateFileFormat(filepath)) {
        return false;
    }
    
    return true;
}

bool Config::validateFileFormat(const std::string& filepath) const {
    std::string ext = getFileExtension(filepath);
    
    // Support common configuration file formats
    std::set<std::string> supportedFormats = {"cfg", "txt", "json", "yaml", "yml"};
    
    return supportedFormats.find(ext) != supportedFormats.end();
}

bool Config::validateFileContent(const std::string& content) const {
    // For now, just check if it's a valid key=value format
    // This is the same format used by the original Config class
    std::istringstream stream(content);
    std::string line;
    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            return false; // Invalid line format
        }
    }
    
    return true;
}

size_t Config::registerParameterChangeCallback(std::function<void(const ParameterChangeEvent&)> callback) {
    changeCallbacks_.push_back(callback);
    return nextCallbackId_++;
}

bool Config::unregisterParameterChangeCallback(size_t callbackId) {
    auto it = std::find_if(changeCallbacks_.begin(), changeCallbacks_.end(),
        [callbackId](const std::function<void(const ParameterChangeEvent&)>& callback) {
            return false; // TODO: track callback IDs
        });
    
    if (it != changeCallbacks_.end()) {
        changeCallbacks_.erase(it);
        return true;
    }
    
    return false;
}

std::vector<ParameterChangeEvent> Config::getParameterHistory(const std::string& key) const {
    auto it = parameterHistory_.find(key);
    if (it != parameterHistory_.end()) {
        return it->second;
    }
    return std::vector<ParameterChangeEvent>();
}

ValidationResult Config::validateParameterChange(const std::string& key, const ConfigValue& newValue,
                                                 ConfigSource source) {
    ValidationResult result;
    
    // Get parameter definition
    auto paramInfo = getParameterInfo(key);
    if (paramInfo) {
        // Type validation
        bool typeValid = false;
        if (paramInfo->type == "int") {
            typeValid = std::holds_alternative<int>(newValue);
        } else if (paramInfo->type == "double" || paramInfo->type == "float") {
            typeValid = std::holds_alternative<double>(newValue) || 
                       std::holds_alternative<float>(newValue);
        } else if (paramInfo->type == "string") {
            typeValid = std::holds_alternative<std::string>(newValue);
        } else if (paramInfo->type == "bool") {
            typeValid = std::holds_alternative<bool>(newValue);
        } else if (paramInfo->type == "uint64_t") {
            typeValid = std::holds_alternative<unsigned long long>(newValue);
        }
        
        if (!typeValid) {
            result.valid = false;
            result.errors.emplace_back(ValidationErrorCategory::TypeError,
                                     key,
                                     "Expected type '" + paramInfo->type + "' but got different type",
                                     "Ensure the value matches the expected type");
        }
        
        // Range validation
        double actualValue = 0.0;
        if (std::holds_alternative<double>(newValue)) {
            actualValue = std::get<double>(newValue);
        } else if (std::holds_alternative<float>(newValue)) {
            actualValue = static_cast<double>(std::get<float>(newValue));
        } else if (std::holds_alternative<int>(newValue)) {
            actualValue = static_cast<double>(std::get<int>(newValue));
        } else if (std::holds_alternative<unsigned long long>(newValue)) {
            actualValue = static_cast<double>(std::get<unsigned long long>(newValue));
        }
        
        if (actualValue < paramInfo->minValue || actualValue > paramInfo->maxValue) {
            result.valid = false;
            result.errors.emplace_back(ValidationErrorCategory::RangeError,
                                     key,
                                     "Value " + std::to_string(actualValue) + " outside range [" +
                                     std::to_string(paramInfo->minValue) + ", " +
                                     std::to_string(paramInfo->maxValue) + "]",
                                     "Adjust the value to be within the valid range",
                                     actualValue, paramInfo->minValue, paramInfo->maxValue);
        }
        
        // Special validation rules
        if (!paramInfo->validationRule.empty()) {
            if (paramInfo->validationRule == "positive") {
                if (actualValue <= 0.0) {
                    result.valid = false;
                    result.errors.emplace_back(ValidationErrorCategory::LogicError,
                                             key,
                                             "Value must be positive",
                                             "Set the value to a positive number");
                }
            }
            else if (paramInfo->validationRule == "non_negative") {
                if (actualValue < 0.0) {
                    result.valid = false;
                    result.errors.emplace_back(ValidationErrorCategory::LogicError,
                                             key,
                                             "Value must be non-negative",
                                             "Set the value to a non-negative number");
                }
            }
        }
    }
    
    // Additional parameter-specific validation
    if (key == "neuron_count") {
        if (std::holds_alternative<size_t>(newValue)) {
            size_t count = std::get<size_t>(newValue);
            if (count < 1 || count > 10000000) {
                result.valid = false;
                result.errors.emplace_back(ValidationErrorCategory::RangeError,
                                         key,
                                         "Neuron count must be between 1 and 10,000,000",
                                         "Set neuron_count to a reasonable value");
            }
        }
    } else if (key == "connection_probability") {
        if (std::holds_alternative<double>(newValue)) {
            double prob = std::get<double>(newValue);
            if (prob < 0.0 || prob > 1.0) {
                result.valid = false;
                result.errors.emplace_back(ValidationErrorCategory::RangeError,
                                         key,
                                         "Connection probability must be between 0.0 and 1.0",
                                         "Set connection_probability to a value between 0.0 and 1.0");
            }
        }
    } else if (key == "simulation_timestep") {
        if (std::holds_alternative<double>(newValue)) {
            double timestep = std::get<double>(newValue);
            if (timestep <= 0.0 || timestep > 0.1) {
                result.valid = false;
                result.errors.emplace_back(ValidationErrorCategory::RangeError,
                                         key,
                                         "Time step must be positive and <= 0.1 seconds",
                                         "Set simulation_timestep to a smaller positive value");
            }
        }
    }
    
    lastValidationResult_ = result;
    return result;
}

bool Config::isParameterChangeSafe(const std::string& key, const ConfigValue& newValue) const {
    // Get current value
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        // Parameter doesn't exist, check if it's safe to add
        return true; // Allow adding new parameters
    }
    
    // Check if new value is different from current value
    if (it->value == newValue) {
        return true; // No change needed
    }
    
    // For critical parameters, additional checks
    if (key == "neuron_count" || key == "region_count") {
        // Changing these parameters would require recreating the brain
        return false; // Not safe to change during runtime
    }
    
    return true;
}

bool Config::validateForBrain() const {
    // Basic validation for brain initialization
    if (!validateNeuronCount() || !validateSynapseDensity() || !validateTimeParameters()) {
        return false;
    }
    
    if (!validatePlasticityParameters()) {
        return false;
    }
    
    return validateConsistency() && validateCompleteness();
}

std::string Config::getConfigPath() const {
    return configPath_;
}

void Config::setConfigPath(const std::string& path) {
    configPath_ = path;
}

std::optional<ValidationError> Config::getLastValidationError() const {
    if (lastValidationResult_.valid && lastValidationResult_.errors.empty()) {
        return std::nullopt;
    }
    
    if (!lastValidationResult_.errors.empty()) {
        return lastValidationResult_.errors[0];
    }
    
    return std::nullopt;
}

Config::ValidationStats Config::getValidationStats() const {
    return validationStats_;
}

void Config::resetValidationStats() {
    validationStats_.totalValidations = 0;
    validationStats_.successfulValidations = 0;
    validationStats_.failedValidations = 0;
    validationStats_.averageValidationTimeMs = 0.0;
    validationStats_.errorCounts.clear();
}

bool Config::validateFilePermissions(const std::string& filepath) const {
    std::filesystem::path path(filepath);
    
    // Check if file exists and is accessible
    if (std::filesystem::exists(path)) {
        // File exists, check if it's readable
        std::ifstream file(filepath);
        return file.good();
    }
    
    // File doesn't exist, check if parent directory is writable
    auto parentPath = path.parent_path();
    if (parentPath.empty()) {
        return true; // Root directory, assume writable
    }
    
    return std::filesystem::exists(parentPath) && 
           std::filesystem::is_directory(parentPath) &&
           (std::filesystem::status(parentPath).permissions() & 
            std::filesystem::perms::owner_write) != std::filesystem::perms::none;
}

std::string Config::getFileExtension(const std::string& filepath) const {
    size_t pos = filepath.find_last_of('.');
    if (pos != std::string::npos && pos < filepath.size() - 1) {
        return filepath.substr(pos + 1);
    }
    return "";
}

std::string Config::parseJsonContent(const std::string& content) const {
    // Since we're not using JsonCpp in this project, return an empty string
    return "";
}

std::string Config::configToJson(const Config& config) const {
    std::ostringstream oss;
    oss << "{\n\"parameters\": [\n";
    
    size_t count = 0;
    for (const auto& entry : config.pImpl->entries) {
        oss << "  {\n";
        oss << "    \"key\": \"" << entry.key << "\",\n";
        
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "    \"value\": \"" << arg << "\",\n";
                oss << "    \"type\": \"string\"\n";
            } else if constexpr (std::is_same_v<T, double>) {
                oss << "    \"value\": " << std::fixed << std::setprecision(6) << arg << ",\n";
                oss << "    \"type\": \"double\"\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << "    \"value\": " << (arg ? "true" : "false") << ",\n";
                oss << "    \"type\": \"bool\"\n";
            } else if constexpr (std::is_same_v<T, int>) {
                oss << "    \"value\": " << arg << ",\n";
                oss << "    \"type\": \"int\"\n";
            } else if constexpr (std::is_same_v<T, int64_t>) {
                oss << "    \"value\": " << arg << ",\n";
                oss << "    \"type\": \"int64_t\"\n";
            } else {
                oss << "    \"value\": null,\n";
                oss << "    \"type\": \"null\"\n";
            }
        }, entry.value);
        
        if (count < config.pImpl->entries.size() - 1) {
            oss << "  },\n";
        } else {
            oss << "  }\n";
        }
        
        count++;
    }
    
    oss << "],\n\"metadata\": {\n";
    oss << "  \"sourceFile\": \"" << config.getConfigPath() << "\"\n";
    oss << "}\n}\n";
    
    return oss.str();
}

// Explicit template instantiations

// get template instantiations
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

// getAs template instantiations
template std::optional<int> Config::getAs<int>(const std::string&) const;
template std::optional<double> Config::getAs<double>(const std::string&) const;
template std::optional<std::string> Config::getAs<std::string>(const std::string&) const;

// getOr template instantiations
template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

// setWithValidation template instantiations
template void Config::setWithValidation<int>(const std::string&, const int&, ConfigSource);
template void Config::setWithValidation<double>(const std::string&, const double&, ConfigSource);
template void Config::setWithValidation<std::string>(const std::string&, const std::string&, ConfigSource);
template void Config::setWithValidation<bool>(const std::string&, const bool&, ConfigSource);

// ensureType template instantiations
template bool Config::ensureType<int>(const std::string&) const;
template bool Config::ensureType<double>(const std::string&) const;
template bool Config::ensureType<std::string>(const std::string&) const;
template bool Config::ensureType<bool>(const std::string&) const;

// convertType template instantiations
template bool Config::convertType<int>(const std::string&, const ConfigValue&);
template bool Config::convertType<double>(const std::string&, const ConfigValue&);
template bool Config::convertType<std::string>(const std::string&, const ConfigValue&);
template bool Config::convertType<bool>(const std::string&, const ConfigValue&);

} // namespace nlm