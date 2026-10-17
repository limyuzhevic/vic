#pragma once

#include <string>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <set>

namespace nlm {

using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>
>;

enum class ConfigSource {
    Default = 0,
    File = 1,
    CommandLine = 2,
    Runtime = 3
};

class ConfigurationValidator {
public:
    struct ParameterConstraints {
        std::string name;
        std::string type;
        std::optional<std::string> defaultValue;
        std::optional<std::string> description;
        std::map<std::string, std::string> ranges;
        std::set<std::string> allowedValues;
        std::string category;
        bool required = false;
    };
    
    ConfigurationValidator() {
        initializeSchema();
    }
    
    bool validateParameter(const std::string& key, const ConfigValue& value, ConfigSource source) {
        auto it = schema.find(key);
        if (it == schema.end()) {
            return false;
        }
        
        const auto& constraints = it->second;
        return validateType(value, constraints.type) && 
               validateRange(value, constraints.ranges) &&
               validateAllowedValues(value, constraints.allowedValues);
    }
    
    std::map<std::string, ConfigValue> getExperimentPreset(const std::string& experimentType) {
        if (experimentPresets.find(experimentType) != experimentPresets.end()) {
            return experimentPresets[experimentType];
        }
        return {};
    }
    
    bool generateTemplate(const std::string& outputPath, const std::set<std::string>& categories = {}) {
        std::ofstream file(outputPath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "# NLM Configuration Template" << std::endl;
        file << "# All possible configuration parameters with constraints" << std::endl;
        file << std::endl;
        
        std::set<std::string> targetCategories;
        if (categories.empty()) {
            for (const auto& [key, constraints] : schema) {
                targetCategories.insert(constraints.category);
            }
        } else {
            targetCategories = categories;
        }
        
        for (const auto& category : targetCategories) {
            file << "# " << category << " parameters" << std::endl;
            for (const auto& [key, constraints] : schema) {
                if (constraints.category == category) {
                    file << "# " << constraints.description << std::endl;
                    if (constraints.defaultValue.has_value()) {
                        file << key << " = " << constraints.defaultValue.value() << std::endl;
                    } else {
                        file << "# " << key << " = <value>" << std::endl;
                    }
                    file << std::endl;
                }
            }
            file << std::endl;
        }
        
        return true;
    }
    
    std::vector<std::string> getAllParameterNames() const {
        std::vector<std::string> names;
        for (const auto& [key, _] : schema) {
            names.push_back(key);
        }
        return names;
    }
    
    std::set<std::string> getExperimentTypes() const {
        std::set<std::string> types;
        for (const auto& preset : experimentPresets) {
            types.insert(preset.first);
        }
        return types;
    }
    
    std::string getValidationError(const std::string& key, const ConfigValue& value, const std::string& reason) {
        std::ostringstream oss;
        oss << "Configuration validation error for parameter '" << key << "': " << reason;
        oss << " (value: ";
        
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, value);
        
        oss << ")";
        return oss.str();
    }
    
private:
    std::map<std::string, ParameterConstraints> schema;
    std::map<std::string, std::map<std::string, ConfigValue>> experimentPresets;
    
    void initializeSchema() {
        schema["neuron.count"] = ParameterConstraints{
            "neuron.count", "int64_t", "500",
            "Number of neurons in the brain", {{ "min", "1" }, { "max", "10000" }},
            {}, "neural", false
        };
        
        schema["neuron.regionCount"] = ParameterConstraints{
            "neuron.regionCount", "int64_t", "1",
            "Number of brain regions", {{ "min", "1" }, { "max", "10" }},
            {}, "neural", false
        };
        
        schema["neuron.connectionProbability"] = ParameterConstraints{
            "neuron.connectionProbability", "double", "0.15",
            "Connection probability for random connectivity", {{ "min", "0.0" }, { "max", "1.0" }},
            {}, "neural", false
        };
        
        schema["plasticity.learningRate"] = ParameterConstraints{
            "plasticity.learningRate", "double", "0.01",
            "General plasticity learning rate", {{ "min", "0.0" }, { "max", "1.0" }},
            {}, "plasticity", false
        };
        
        schema["plasticity.stdp.learningRate"] = ParameterConstraints{
            "plasticity.stdp.learningRate", "double", "0.01",
            "STDP learning rate", {{ "min", "0.0" }, { "max", "0.1" }},
            {}, "plasticity", false
        };
        
        schema["plasticity.stdp.ltpWeight"] = ParameterConstraints{
            "plasticity.stdp.ltpWeight", "double", "0.02",
            "STDP LTP weight", {{ "min", "0.0" }, { "max", "1.0" }},
            {}, "plasticity", false
        };
        
        schema["plasticity.stdp.ltdWeight"] = ParameterConstraints{
            "plasticity.stdp.ltdWeight", "double", "0.015",
            "STDP LTD weight", {{ "min", "0.0" }, { "max", "1.0" }},
            {}, "plasticity", false
        };
        
        schema["simulation.timestep"] = ParameterConstraints{
            "simulation.timestep", "double", "0.001",
            "Simulation time step (seconds)", {{ "min", "0.0001" }, { "max", "0.1" }},
            {}, "simulation", false
        };
        
        schema["simulation.maxSteps"] = ParameterConstraints{
            "simulation.maxSteps", "int64_t", "10000",
            "Maximum number of simulation steps", {{ "min", "1" }, { "max", "1000000" }},
            {}, "simulation", false
        };
        
        schema["logging.level"] = ParameterConstraints{
            "logging.level", "string", "INFO",
            "Logging level", {}, {"DEBUG", "INFO", "WARN", "ERROR"},
            "logging", false
        };
        
        schema["visualization.enabled"] = ParameterConstraints{
            "visualization.enabled", "bool", "false",
            "Enable visualization", {}, {}, "visualization", false
        };
        
        schema["checkpoint.interval"] = ParameterConstraints{
            "checkpoint.interval", "double", "0.0",
            "Checkpoint interval (seconds, 0 = disabled)", {{ "min", "0.0" }},
            {}, "checkpoint", false
        };
        
        experimentPresets["learning"] = {
            {"plasticity.learningRate", ConfigValue(0.05)},
            {"plasticity.stdp.learningRate", ConfigValue(0.03)},
            {"plasticity.stdp.ltpWeight", ConfigValue(0.05)},
            {"plasticity.stdp.ltdWeight", ConfigValue(0.04)}
        };
        
        experimentPresets["behavior"] = {
            {"plasticity.learningRate", ConfigValue(0.02)},
            {"plasticity.stdp.learningRate", ConfigValue(0.01)},
            {"plasticity.stdp.ltpWeight", ConfigValue(0.02)},
            {"plasticity.stdp.ltdWeight", ConfigValue(0.018)}
        };
        
        experimentPresets["plasticity"] = {
            {"plasticity.learningRate", ConfigValue(0.1)},
            {"plasticity.stdp.learningRate", ConfigValue(0.08)},
            {"plasticity.stdp.ltpWeight", ConfigValue(0.1)},
            {"plasticity.stdp.ltdWeight", ConfigValue(0.09)}
        };
    }
    
    bool validateType(const ConfigValue& value, const std::string& type) {
        if (type == "int64_t") return std::holds_alternative<int64_t>(value);
        if (type == "double") return std::holds_alternative<double>(value);
        if (type == "bool") return std::holds_alternative<bool>(value);
        if (type == "string") return std::holds_alternative<std::string>(value);
        return false;
    }
    
    bool validateRange(const ConfigValue& value, const std::map<std::string, std::string>& ranges) {
        for (const auto& [type, constraint] : ranges) {
            if (type == "min") {
                if (std::holds_alternative<int64_t>(value) && std::get<int64_t>(value) < std::stoll(constraint)) return false;
                if (std::holds_alternative<double>(value) && std::get<double>(value) < std::stod(constraint)) return false;
            } else if (type == "max") {
                if (std::holds_alternative<int64_t>(value) && std::get<int64_t>(value) > std::stoll(constraint)) return false;
                if (std::holds_alternative<double>(value) && std::get<double>(value) > std::stod(constraint)) return false;
            }
        }
        return true;
    }
    
    bool validateAllowedValues(const ConfigValue& value, const std::set<std::string>& allowedValues) {
        if (allowedValues.empty()) return true;
        if (std::holds_alternative<std::string>(value)) {
            const std::string& val = std::get<std::string>(value);
            return allowedValues.find(val) != allowedValues.end();
        }
        return true;
    }
};

} // namespace nlm