#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <typeinfo>

namespace nlm {

// Static validation registry
static std::map<std::string, ParameterSchema> g_parameterSchemas;

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Parameter validation helpers
namespace {
    bool validateType(const ConfigValue& value, const std::string& typeName) {
        const std::type_info& actualType = value.type();
        
        if (typeName == "int" && actualType == typeid(int)) return true;
        if (typeName == "int64_t" && actualType == typeid(int64_t)) return true;
        if (typeName == "double" && actualType == typeid(double)) return true;
        if (typeName == "bool" && actualType == typeid(bool)) return true;
        if (typeName == "string" && actualType == typeid(std::string)) return true;
        if (typeName == "vector<int>" && actualType == typeid(std::vector<int>)) return true;
        if (typeName == "vector<double>" && actualType == typeid(std::vector<double>)) return true;
        if (typeName == "vector<string>" && actualType == typeid(std::vector<std::string>)) return true;
        
        return false;
    }
    
    double getNumericValue(const ConfigValue& value) {
        if (std::holds_alternative<int>(value)) return std::get<int>(value);
        if (std::holds_alternative<int64_t>(value)) return static_cast<double>(std::get<int64_t>(value));
        if (std::holds_alternative<double>(value)) return std::get<double>(value);
        return 0.0;
    }
    
    size_t getLength(const ConfigValue& value) {
        if (std::holds_alternative<std::string>(value)) return std::get<std::string>(value).size();
        if (std::holds_alternative<std::vector<int>>(value)) return std::get<std::vector<int>>(value).size();
        if (std::holds_alternative<std::vector<double>>(value)) return std::get<std::vector<double>>(value).size();
        if (std::holds_alternative<std::vector<std::string>>(value)) return std::get<std::vector<std::string>>(value).size();
        return 0;
    }
    
    std::string getStringValue(const ConfigValue& value) {
        if (std::holds_alternative<std::string>(value)) return std::get<std::string>(value);
        if (std::holds_alternative<int>(value)) return std::to_string(std::get<int>(value));
        if (std::holds_alternative<int64_t>(value)) return std::to_string(std::get<int64_t>(value));
        if (std::holds_alternative<double>(value)) return std::to_string(std::get<double>(value));
        if (std::holds_alternative<bool>(value)) return std::get<bool>(value) ? "true" : "false";
        return "";
    }
    
    bool matchesStringList(const ConfigValue& value, const std::vector<std::string>& allowedList) {
        std::string strVal = getStringValue(value);
        return std::find(allowedList.begin(), allowedList.end(), strVal) != allowedList.end();
    }
}

bool Config::validateValue(const std::string& key, const ConfigValue& value) {
    auto it = g_parameterSchemas.find(key);
    if (it == g_parameterSchemas.end()) {
        return true; // No schema defined, assume valid
    }
    
    const ParameterSchema& schema = it->second;
    
    // Type validation
    if (!validateType(value, schema.type)) {
        return false;
    }
    
    // Range validation for numeric types
    if (schema.minValue.has_value() || schema.maxValue.has_value()) {
        double val = getNumericValue(value);
        if (schema.minValue.has_value() && val < schema.minValue.value()) {
            return false;
        }
        if (schema.maxValue.has_value() && val > schema.maxValue.value()) {
            return false;
        }
    }
    
    // Length validation for strings and vectors
    if (schema.minLength.has_value() || schema.maxLength.has_value()) {
        size_t len = getLength(value);
        if (schema.minLength.has_value() && len < schema.minLength.value()) {
            return false;
        }
        if (schema.maxLength.has_value() && len > schema.maxLength.value()) {
            return false;
        }
    }
    
    // Allowed values validation for strings
    if (schema.allowedValues.has_value()) {
        if (!matchesStringList(value, schema.allowedValues.value())) {
            return false;
        }
    }
    
    // Custom validator
    if (schema.customValidator && !schema.customValidator(value)) {
        return false;
    }
    
    return true;
}

std::string Config::formatValidationError(const std::string& key, const ConfigValue& value) {
    auto it = g_parameterSchemas.find(key);
    if (it == g_parameterSchemas.end()) {
        return "Unknown parameter: " + key;
    }
    
    const ParameterSchema& schema = it->second;
    std::string error = "Invalid value for parameter '" + key + "': ";
    
    // Get value string
    std::string valueStr = "";
    if (std::holds_alternative<std::string>(value)) {
        valueStr = "\"" + std::get<std::string>(value) + "\"";
    } else if (std::holds_alternative<bool>(value)) {
        valueStr = std::get<bool>(value) ? "true" : "false";
    } else {
        valueStr = "value provided";
    }
    
    error += valueStr + ". ";
    
    // Type error
    if (!validateType(value, schema.type)) {
        error += "Expected type '" + schema.type + "', got different type. ";
    }
    
    // Range error
    if (schema.minValue.has_value() || schema.maxValue.has_value()) {
        double val = getNumericValue(value);
        if (schema.minValue.has_value() && val < schema.minValue.value()) {
            error += "Value must be >= " + std::to_string(schema.minValue.value()) + ". ";
        }
        if (schema.maxValue.has_value() && val > schema.maxValue.value()) {
            error += "Value must be <= " + std::to_string(schema.maxValue.value()) + ". ";
        }
    }
    
    // Length error
    if (schema.minLength.has_value() || schema.maxLength.has_value()) {
        size_t len = getLength(value);
        if (schema.minLength.has_value() && len < schema.minLength.value()) {
            error += "Length must be >= " + std::to_string(schema.minLength.value()) + ". ";
        }
        if (schema.maxLength.has_value() && len > schema.maxLength.value()) {
            error += "Length must be <= " + std::to_string(schema.maxLength.value()) + ". ";
        }
    }
    
    // Allowed values error
    if (schema.allowedValues.has_value() && !matchesStringList(value, schema.allowedValues.value())) {
        error += "Allowed values: ";
        for (size_t i = 0; i < schema.allowedValues.value().size(); ++i) {
            if (i > 0) error += ", ";
            error += "\"" + schema.allowedValues.value()[i] + "\"";
        }
        error += ". ";
    }
    
    if (schema.customValidator) {
        // Try to create a more specific error message
        if (schema.errorFormatter) {
            error = schema.errorFormatter(value);
        }
    }
    
    return error;
}

void Config::validate() const {
    for (const auto& entry : pImpl->entries) {
        if (!validateValue(entry.key, entry.value)) {
            throw ConfigValidationError(formatValidationError(entry.key, entry.value), entry.key, entry.value);
        }
    }
}

void Config::registerParameterSchema(const std::string& key, const ParameterSchema& schema) {
    g_parameterSchemas[key] = schema;
}

bool Config::isValidValue(const std::string& key, const ConfigValue& value) {
    return validateValue(key, value);
}

// Parameter registration function
void registerDefaultParameters() {
    // Register all standard parameters with validation rules
    
    // Neural parameters
    registerParameterSchema("neuron_count", ParameterSchema{
        "int64_t", "Number of neurons in the brain",
        10LL, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt
    });
    
    registerParameterSchema("region_count", ParameterSchema{
        "int64_t", "Number of brain regions",
        1LL, 100LL,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt
    });
    
    registerParameterSchema("connection_probability", ParameterSchema{
        "double", "Connection probability for random connectivity",
        0.0, 1.0,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt
    });
    
    // STDP parameters
    registerParameterSchema("stdp_ltp_weight", ParameterSchema{
        "double", "STDP Long-Term Potentiation weight",
        0.0, 0.1,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt
    });
    
    registerParameterSchema("stdp_ltd_weight", ParameterSchema{
        "double", "STDP Long-Term Depression weight",
        0.0, 0.2,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt
    });
    
    registerParameterSchema("stdp_tau", ParameterSchema{
        "double", "STDP time constant (ms)",
        1.0, 1000.0,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt
    });
    
    // Structural plasticity parameters
    registerParameterSchema("synaptogenesis_rate", ParameterSchema{
        "double", "Synaptogenesis rate",
        0.0, 0.1,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt
    });
    
    registerParameterSchema("pruning_rate", ParameterSchema{
        "double", "Pruning rate",
        0.0, 0.01,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt
    });
    
    // Neuromodulation parameters
    registerParameterSchema("dopamine_baseline", ParameterSchema{
        "double", "Dopamine baseline level",
        0.0, 10.0,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt
    });
    
    registerParameterSchema("reward_discount_factor", ParameterSchema{
        "double", "Reward discount factor",
        0.0, 1.0,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt
    });
    
    // Environment parameters
    registerParameterSchema("environment_name", ParameterSchema{
        "string", "Name of the environment",
        std::nullopt, std::nullopt,
        1, 50,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt
    });
    
    registerParameterSchema("environment_width", ParameterSchema{
        "int64_t", "Environment width",
        1, 1000,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt
    });
    
    registerParameterSchema("environment_height", ParameterSchema{
        "int64_t", "Environment height",
        1, 1000,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt
    });
    
    // Logging parameters
    registerParameterSchema("log_level", ParameterSchema{
        "string", "Logging level",
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::vector<std::string>{"DEBUG", "INFO", "WARNING", "ERROR"},
        std::nullopt, std::nullopt,
        std::nullopt
    });
    
    registerParameterSchema("log_to_file", ParameterSchema{
        "bool", "Log to file flag",
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt
    });
    
    // Simulation parameters
    registerParameterSchema("simulation_timestep", ParameterSchema{
        "double", "Simulation timestep in seconds",
        0.0001, 0.1,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt
    });
    
    registerParameterSchema("random_seed", ParameterSchema{
        "int64_t", "Random seed for reproducible experiments",
        0, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt
    });
    
    registerParameterSchema("max_simulation_steps", ParameterSchema{
        "int64_t", "Maximum number of simulation steps",
        1, 1000000,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt
    });
    
    // Visualization parameters
    registerParameterSchema("visualization_enabled", ParameterSchema{
        "bool", "Visualization enabled flag",
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt
    });
    
    registerParameterSchema("visualization_update_rate", ParameterSchema{
        "double", "Visualization update rate",
        1.0, 1000.0,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt
    });
    
    // Environment interface parameters
    registerParameterSchema("vision_width", ParameterSchema{
        "int64_t", "Vision width",
        1, 100,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt
    });
    
    registerParameterSchema("vision_height", ParameterSchema{
        "int64_t", "Vision height",
        1, 100,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt
    });
    
    // Custom validators for special parameters
    auto stringNonEmpty = [](const ConfigValue& val) -> bool {
        return std::holds_alternative<std::string>(val) && !std::get<std::string>(val).empty();
    };
    
    auto positiveInt = [](const ConfigValue& val) -> bool {
        if (std::holds_alternative<int>(val)) return std::get<int>(val) > 0;
        if (std::holds_alternative<int64_t>(val)) return std::get<int64_t>(val) > 0;
        if (std::holds_alternative<size_t>(val)) return std::get<size_t>(val) > 0;
        return false;
    };
    
    auto positiveDouble = [](const ConfigValue& val) -> bool {
        double val = getNumericValue(val);
        return val > 0.0;
    };
    
    auto betweenZeroAndOne = [](const ConfigValue& val) -> bool {
        double val = getNumericValue(val);
        return val >= 0.0 && val <= 1.0;
    };
    
    // Register validated versions
    registerParameterSchema("neuron_count", ParameterSchema{
        "int64_t", "Number of neurons in the brain",
        10LL, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, positiveInt,
        std::nullopt
    });
    
    registerParameterSchema("region_count", ParameterSchema{
        "int64_t", "Number of brain regions",
        1LL, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, positiveInt,
        std::nullopt
    });
    
    registerParameterSchema("simulation_timestep", ParameterSchema{
        "double", "Simulation timestep in seconds",
        0.0001, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, positiveDouble,
        std::nullopt
    });
    
    registerParameterSchema("random_seed", ParameterSchema{
        "int64_t", "Random seed for reproducible experiments",
        0LL, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt
    });
    
    registerParameterSchema("environment_name", ParameterSchema{
        "string", "Name of the environment",
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, stringNonEmpty,
        std::nullopt
    });
}

// Initialize default parameters on first use
static struct DefaultParametersInitializer {
    DefaultParametersInitializer() {
        registerDefaultParameters();
    }
} g_defaultParametersInitializer;

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <typeinfo>
#include <cerrno>

namespace nlm {

// Static validation registry
static std::map<std::string, ParameterSchema> g_parameterSchemas;

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Parameter validation helpers
namespace {
    bool validateType(const ConfigValue& value, const std::string& typeName) {
        const std::type_info& actualType = value.type();
        
        if (typeName == "int" && actualType == typeid(int)) return true;
        if (typeName == "int64_t" && actualType == typeid(int64_t)) return true;
        if (typeName == "double" && actualType == typeid(double)) return true;
        if (typeName == "bool" && actualType == typeid(bool)) return true;
        if (typeName == "string" && actualType == typeid(std::string)) return true;
        if (typeName == "vector<int>" && actualType == typeid(std::vector<int>)) return true;
        if (typeName == "vector<double>" && actualType == typeid(std::vector<double>)) return true;
        if (typeName == "vector<string>" && actualType == typeid(std::vector<std::string>)) return true;
        
        return false;
    }
    
    double getNumericValue(const ConfigValue& value) {
        if (std::holds_alternative<int>(value)) return std::get<int>(value);
        if (std::holds_alternative<int64_t>(value)) return static_cast<double>(std::get<int64_t>(value));
        if (std::holds_alternative<double>(value)) return std::get<double>(value);
        return 0.0;
    }
    
    size_t getLength(const ConfigValue& value) {
        if (std::holds_alternative<std::string>(value)) return std::get<std::string>(value).size();
        if (std::holds_alternative<std::vector<int>>(value)) return std::get<std::vector<int>>(value).size();
        if (std::holds_alternative<std::vector<double>>(value)) return std::get<std::vector<double>>(value).size();
        if (std::holds_alternative<std::vector<std::string>>(value)) return std::get<std::vector<std::string>>(value).size();
        return 0;
    }
    
    std::string getStringValue(const ConfigValue& value) {
        if (std::holds_alternative<std::string>(value)) return std::get<std::string>(value);
        if (std::holds_alternative<int>(value)) return std::to_string(std::get<int>(value));
        if (std::holds_alternative<int64_t>(value)) return std::to_string(std::get<int64_t>(value));
        if (std::holds_alternative<double>(value)) return std::to_string(std::get<double>(value));
        if (std::holds_alternative<bool>(value)) return std::get<bool>(value) ? "true" : "false";
        return "";
    }
    
    bool matchesStringList(const ConfigValue& value, const std::vector<std::string>& allowedList) {
        std::string strVal = getStringValue(value);
        return std::find(allowedList.begin(), allowedList.end(), strVal) != allowedList.end();
    }
}

bool Config::validateValue(const std::string& key, const ConfigValue& value) {
    auto it = g_parameterSchemas.find(key);
    if (it == g_parameterSchemas.end()) {
        return true; // No schema defined, assume valid
    }
    
    const ParameterSchema& schema = it->second;
    
    // Type validation
    if (!validateType(value, schema.type)) {
        return false;
    }
    
    // Range validation for numeric types
    if (schema.minValue.has_value() || schema.maxValue.has_value()) {
        double val = getNumericValue(value);
        if (schema.minValue.has_value() && val < schema.minValue.value()) {
            return false;
        }
        if (schema.maxValue.has_value() && val > schema.maxValue.value()) {
            return false;
        }
    }
    
    // Length validation for strings and vectors
    if (schema.minLength.has_value() || schema.maxLength.has_value()) {
        size_t len = getLength(value);
        if (schema.minLength.has_value() && len < schema.minLength.value()) {
            return false;
        }
        if (schema.maxLength.has_value() && len > schema.maxLength.value()) {
            return false;
        }
    }
    
    // Allowed values validation for strings
    if (schema.allowedValues.has_value()) {
        if (!matchesStringList(value, schema.allowedValues.value())) {
            return false;
        }
    }
    
    // Custom validator
    if (schema.customValidator && !schema.customValidator(value)) {
        return false;
    }
    
    return true;
}

std::string Config::formatValidationError(const std::string& key, const ConfigValue& value) {
    auto it = g_parameterSchemas.find(key);
    if (it == g_parameterSchemas.end()) {
        return "Unknown parameter: " + key;
    }
    
    const ParameterSchema& schema = it->second;
    std::string error = "Invalid value for parameter '" + key + "': ";
    
    // Get value string
    std::string valueStr = "";
    if (std::holds_alternative<std::string>(value)) {
        valueStr = "\"" + std::get<std::string>(value) + "\"";
    } else if (std::holds_alternative<bool>(value)) {
        valueStr = std::get<bool>(value) ? "true" : "false";
    } else {
        valueStr = "value provided";
    }
    
    error += valueStr + ". ";
    
    // Type error
    if (!validateType(value, schema.type)) {
        error += "Expected type '" + schema.type + "', got different type. ";
    }
    
    // Range error
    if (schema.minValue.has_value() || schema.maxValue.has_value()) {
        double val = getNumericValue(value);
        if (schema.minValue.has_value() && val < schema.minValue.value()) {
            error += "Value must be >= " + std::to_string(schema.minValue.value()) + ". ";
        }
        if (schema.maxValue.has_value() && val > schema.maxValue.value()) {
            error += "Value must be <= " + std::to_string(schema.maxValue.value()) + ". ";
        }
    }
    
    // Length error
    if (schema.minLength.has_value() || schema.maxLength.has_value()) {
        size_t len = getLength(value);
        if (schema.minLength.has_value() && len < schema.minLength.value()) {
            error += "Length must be >= " + std::to_string(schema.minLength.value()) + ". ";
        }
        if (schema.maxLength.has_value() && len > schema.maxLength.value()) {
            error += "Length must be <= " + std::to_string(schema.maxLength.value()) + ". ";
        }
    }
    
    // Allowed values error
    if (schema.allowedValues.has_value() && !matchesStringList(value, schema.allowedValues.value())) {
        error += "Allowed values: ";
        for (size_t i = 0; i < schema.allowedValues.value().size(); ++i) {
            if (i > 0) error += ", ";
            error += "\"" + schema.allowedValues.value()[i] + "\"";
        }
        error += ". ";
    }
    
    if (schema.customValidator) {
        // Try to create a more specific error message
        if (schema.errorFormatter) {
            error = schema.errorFormatter(value);
        }
    }
    
    return error;
}

void Config::validate() const {
    for (const auto& entry : pImpl->entries) {
        if (!validateValue(entry.key, entry.value)) {
            throw ConfigValidationError(formatValidationError(entry.key, entry.value), entry.key, entry.value);
        }
    }
}

void Config::registerParameterSchema(const std::string& key, const ParameterSchema& schema) {
    g_parameterSchemas[key] = schema;
}

bool Config::isValidValue(const std::string& key, const ConfigValue& value) {
    return validateValue(key, value);
}

// Parameter registration function
void registerDefaultParameters() {
    // Register all standard parameters with validation rules
    
    // Neural parameters
    registerParameterSchema("neuron_count", ParameterSchema{
        "int64_t", "Number of neurons in the brain",
        10LL, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt
    });
    
    registerParameterSchema("region_count", ParameterSchema{
        "int64_t", "Number of brain regions",
        1LL, 100LL,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt
    });
    
    registerParameterSchema("connection_probability", ParameterSchema{
        "double", "Connection probability for random connectivity",
        0.0, 1.0,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt
    });
    
    // STDP parameters
    registerParameterSchema("stdp_ltp_weight", ParameterSchema{
        "double", "STDP Long-Term Potentiation weight",
        0.0, 0.1,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt
    });
    
    registerParameterSchema("stdp_ltd_weight", ParameterSchema{
        "double", "STDP Long-Term Depression weight",
        0.0, 0.2,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt
    });
    
    registerParameterSchema("stdp_tau", ParameterSchema{
        "double", "STDP time constant (ms)",
        1.0, 1000.0,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt
    });
    
    // Structural plasticity parameters
    registerParameterSchema("synaptogenesis_rate", ParameterSchema{
        "double", "Synaptogenesis rate",
        0.0, 0.1,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt
    });
    
    registerParameterSchema("pruning_rate", ParameterSchema{
        "double", "Pruning rate",
        0.0, 0.01,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt
    });
    
    // Neuromodulation parameters
    registerParameterSchema("dopamine_baseline", ParameterSchema{
        "double", "Dopamine baseline level",
        0.0, 10.0,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt
    });
    
    registerParameterSchema("reward_discount_factor", ParameterSchema{
        "double", "Reward discount factor",
        0.0, 1.0,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt
    });
    
    // Environment parameters
    registerParameterSchema("environment_name", ParameterSchema{
        "string", "Name of the environment",
        std::nullopt, std::nullopt,
        1, 50,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt
    });
    
    registerParameterSchema("environment_width", ParameterSchema{
        "int64_t", "Environment width",
        1, 1000,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt
    });
    
    registerParameterSchema("environment_height", ParameterSchema{
        "int64_t", "Environment height",
        1, 1000,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt
    });
    
    // Logging parameters
    registerParameterSchema("log_level", ParameterSchema{
        "string", "Logging level",
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::vector<std::string>{"DEBUG", "INFO", "WARNING", "ERROR"},
        std::nullopt, std::nullopt,
        std::nullopt
    });
    
    registerParameterSchema("log_to_file", ParameterSchema{
        "bool", "Log to file flag",
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt
    });
    
    // Simulation parameters
    registerParameterSchema("simulation_timestep", ParameterSchema{
        "double", "Simulation timestep in seconds",
        0.0001, 0.1,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt
    });
    
    registerParameterSchema("random_seed", ParameterSchema{
        "int64_t", "Random seed for reproducible experiments",
        0, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt
    });
    
    registerParameterSchema("max_simulation_steps", ParameterSchema{
        "int64_t", "Maximum number of simulation steps",
        1, 1000000,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt
    });
    
    // Visualization parameters
    registerParameterSchema("visualization_enabled", ParameterSchema{
        "bool", "Visualization enabled flag",
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt
    });
    
    registerParameterSchema("visualization_update_rate", ParameterSchema{
        "double", "Visualization update rate",
        1.0, 1000.0,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt
    });
    
    // Environment interface parameters
    registerParameterSchema("vision_width", ParameterSchema{
        "int64_t", "Vision width",
        1, 100,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt
    });
    
    registerParameterSchema("vision_height", ParameterSchema{
        "int64_t", "Vision height",
        1, 100,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt
    });
    
    // Custom validators for special parameters
    auto stringNonEmpty = [](const ConfigValue& val) -> bool {
        return std::holds_alternative<std::string>(val) && !std::get<std::string>(val).empty();
    };
    
    auto positiveInt = [](const ConfigValue& val) -> bool {
        if (std::holds_alternative<int>(val)) return std::get<int>(val) > 0;
        if (std::holds_alternative<int64_t>(val)) return std::get<int64_t>(val) > 0;
        if (std::holds_alternative<size_t>(val)) return std::get<size_t>(val) > 0;
        return false;
    };
    
    auto positiveDouble = [](const ConfigValue& val) -> bool {
        double val = getNumericValue(val);
        return val > 0.0;
    };
    
    auto betweenZeroAndOne = [](const ConfigValue& val) -> bool {
        double val = getNumericValue(val);
        return val >= 0.0 && val <= 1.0;
    };
    
    // Register validated versions
    registerParameterSchema("neuron_count", ParameterSchema{
        "int64_t", "Number of neurons in the brain",
        10LL, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, positiveInt,
        std::nullopt
    });
    
    registerParameterSchema("region_count", ParameterSchema{
        "int64_t", "Number of brain regions",
        1LL, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, positiveInt,
        std::nullopt
    });
    
    registerParameterSchema("simulation_timestep", ParameterSchema{
        "double", "Simulation timestep in seconds",
        0.0001, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, positiveDouble,
        std::nullopt
    });
    
    registerParameterSchema("random_seed", ParameterSchema{
        "int64_t", "Random seed for reproducible experiments",
        0LL, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt
    });
    
    registerParameterSchema("environment_name", ParameterSchema{
        "string", "Name of the environment",
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, std::nullopt,
        std::nullopt, stringNonEmpty,
        std::nullopt
    });
}

// Initialize default parameters on first use
static struct DefaultParametersInitializer {
    DefaultParametersInitializer() {
        registerDefaultParameters();
    }
} g_defaultParametersInitializer;

void Config::update(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate the value before setting
    if (!validateValue(key, value)) {
        throw ConfigValidationError(formatValidationError(key, value), key, value);
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

// Original set() methods for backward compatibility (now use update() for validation)
void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    update(key, value, source);
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    update(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    update(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    update(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    update(key, ConfigValue(value), source);
}

// Backward compatibility get() and getOr() methods

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

// File I/O methods with improved exception handling

bool Config::loadFromFile(const std::string& filepath) {
    // Validate file path
    if (filepath.empty()) {
        throw ConfigValidationError("File path cannot be empty", "filepath", ConfigValue("") );
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        if (errno == ENOENT) {
            throw ConfigValidationError("Configuration file not found: " + filepath, "filepath", ConfigValue(filepath));
        } else if (errno == EACCES) {
            throw ConfigValidationError("Permission denied: " + filepath, "filepath", ConfigValue(filepath));
        } else {
            throw ConfigValidationError("Cannot open configuration file: " + filepath, "filepath", ConfigValue(filepath));
        }
    }
    
    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        ++lineNumber;
        
        try {
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
                
                // Validate key format
                if (key.empty()) {
                    throw ConfigValidationError("Empty key at line " + std::to_string(lineNumber), key, ConfigValue(value));
                }
                
                set(key, value, ConfigSource::File);
            } else {
                // Ignore malformed lines
                NLM_LOG_WARNING("Ignoring malformed line " + std::to_string(lineNumber) + ": " + line);
            }
        } catch (const ConfigValidationError& e) {
            // Add line number to validation error
            throw ConfigValidationError("Line " + std::to_string(lineNumber) + ": " + e.what(), e.getKey(), e.getValue());
        } catch (const std::exception& e) {
            throw ConfigValidationError("Error processing line " + std::to_string(lineNumber) + ": " + std::string(e.what()), key, ConfigValue(value));
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    try {
        if (!argv) {
            throw ConfigValidationError("Invalid command line arguments", "argc", ConfigValue(static_cast<int64_t>(argc)));
        }
        
        if (argc < 0) {
            throw ConfigValidationError("Invalid argument count: " + std::to_string(argc), "argc", ConfigValue(static_cast<int64_t>(argc)));
        }
        
        for (int i = 1; i < argc; ++i) {
            if (!argv[i]) {
                throw ConfigValidationError("Null argument at position " + std::to_string(i), "argv", ConfigValue(static_cast<int64_t>(i)));
            }
            
            std::string arg(argv[i]);
            
            // Handle --key=value format
            if (arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    
                    // Validate key and value
                    if (key.empty()) {
                        throw ConfigValidationError("Empty key in argument: " + arg, key, ConfigValue(value));
                    }
                    
                    set(key, value, ConfigSource::CommandLine);
                } else {
                    // Handle --key value format
                    std::string key = arg.substr(2);
                    if (i + 1 < argc && argv[i + 1][0] != '-') {
                        std::string value = argv[++i];
                        
                        if (key.empty()) {
                            throw ConfigValidationError("Empty key in argument: " + arg, key, ConfigValue(value));
                        }
                        
                        set(key, value, ConfigSource::CommandLine);
                    }
                }
            } 
            // Handle -key value format
            else if (arg[0] == '-' && i + 1 < argc && argv[i + 1][0] != '-') {
                std::string key = arg.substr(1);
                std::string value = argv[++i];
                
                if (key.empty()) {
                    throw ConfigValidationError("Empty key in argument: " + arg, key, ConfigValue(value));
                }
                
                set(key, value, ConfigSource::CommandLine);
            }
        }
    } catch (const ConfigValidationError& e) {
        throw; // Re-throw validation errors
    } catch (const std::exception& e) {
        throw ConfigValidationError("Error processing command line arguments: " + std::string(e.what()), "argv", ConfigValue(static_cast<int64_t>(argc)));
    }
    
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        if (filepath.empty()) {
            throw ConfigValidationError("File path cannot be empty", "filepath", ConfigValue("") );
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            if (errno == EACCES) {
                throw ConfigValidationError("Permission denied: " + filepath, "filepath", ConfigValue(filepath));
            } else if (errno == ENOENT) {
                // Create parent directories if they don't exist
                std::filesystem::path path(filepath);
                std::filesystem::create_directories(path.parent_path());
                file.open(filepath);
                if (!file.is_open()) {
                    throw ConfigValidationError("Cannot create configuration file: " + filepath, "filepath", ConfigValue(filepath));
                }
            } else {
                throw ConfigValidationError("Cannot open configuration file for writing: " + filepath, "filepath", ConfigValue(filepath));
            }
        }
        
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = ";
            
            // Format value appropriately
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else {
                    file << arg;
                }
            }, entry.value);
            
            file << "\n";
        }
        
        file << "\n"; // End with newline
        
    } catch (const ConfigValidationError& e) {
        throw; // Re-throw validation errors
    } catch (const std::exception& e) {
        throw ConfigValidationError("Error saving configuration to file: " + std::string(e.what()), "filepath", ConfigValue(filepath));
    }
    
    return true;
}

// Internal helpers

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

} // namespace nlm

// Original set() methods for backward compatibility (now use update() for validation)
void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    update(key, value, source);
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    update(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    update(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    update(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    update(key, ConfigValue(value), source);
}

// Backward compatibility get() and getOr() methods

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

// File I/O methods

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
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
    }
    
    return true;
}

// Internal helpers

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

} // namespace nlm