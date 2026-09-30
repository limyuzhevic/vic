#pragma once

#include <string>
#include <vector>
#include <map>
#include <variant>
#include <memory>
#include <functional>
#include <optional>
#include <iostream>

namespace nlm {

// Configuration value types
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

// Schema value types
enum class SchemaValueType {
    Integer,
    Double,
    Boolean,
    String,
    IntArray,
    DoubleArray,
    StringArray,
    Object,
    Array
};

// Schema validation constraint
struct SchemaConstraint {
    std::string constraintType;
    std::string constraintValue;
    std::string errorMessage;
    
    SchemaConstraint(const std::string& type, const std::string& value, const std::string& error)
        : constraintType(type), constraintValue(value), errorMessage(error) {}
};

// Schema property
struct SchemaProperty {
    std::string name;
    SchemaValueType type;
    std::string description;
    std::string defaultValue;
    bool required;
    bool readOnly;
    
    std::vector<SchemaConstraint> constraints;
    
    SchemaProperty(const std::string& name, SchemaValueType type, const std::string& desc = "")
        : name(name), type(type), description(desc), required(false), readOnly(false) {}
};

// Schema
struct Schema {
    std::string name;
    std::string description;
    std::string version;
    bool strict;
    
    std::map<std::string, SchemaProperty> properties;
    std::vector<std::string> requiredProperties;
    
    Schema(const std::string& name, const std::string& desc = "", const std::string& ver = "1.0")
        : name(name), description(desc), version(ver), strict(true) {}
    
    void addProperty(const SchemaProperty& prop) {
        properties[prop.name] = prop;
        if (prop.required) {
            requiredProperties.push_back(prop.name);
        }
    }
    
    bool validate(const ConfigValue& value) const {
        return true; // Simplified implementation
    }
    
    std::string toJSON() const {
        std::string json = "{\n";
        json += "  \"$schema\": \"http://json-schema.org/draft-07/schema#\",\n";
        json += "  \"$id\": \"" + name + "\",\n";
        json += "  \"title\": \"" + name + "\",\n";
        json += "  \"description\": \"" + description + "\",\n";
        json += "  \"properties\": {\n";
        
        bool first = true;
        for (const auto& pair : properties) {
            if (!first) json += ",\n";
            first = false;
            
            json += "    \"" + pair.first + "\": {\n";
            json += "      \"type\": \"" + schemaTypeToString(pair.second.type) + "\",\n";
            if (!pair.second.description.empty()) {
                json += "      \"description\": \"" + pair.second.description + "\",\n";
            }
            if (!pair.second.defaultValue.empty()) {
                json += "      \"default\": \"" + pair.second.defaultValue + "\",\n";
            }
            if (pair.second.required) {
                json += "      \"required\": true,\n";
            }
            
            if (!pair.second.constraints.empty()) {
                json += "      \"constraints\": [\n";
                bool constraintFirst = true;
                for (const auto& constraint : pair.second.constraints) {
                    if (!constraintFirst) json += ",\n";
                    constraintFirst = false;
                    json += "        {\n";
                    json += "          \"type\": \"" + constraint.constraintType + "\",\n";
                    json += "          \"value\": \"" + constraint.constraintValue + "\",\n";
                    if (!constraint.errorMessage.empty()) {
                        json += "          \"errorMessage\": \"" + constraint.errorMessage + "\"\n";
                    }
                    json += "        }";
                }
                json += "\n      ],\n";
            }
            json += "    }";
        }
        
        json += "\n  },\n";
        if (!requiredProperties.empty()) {
            json += "  \"required\": [";
            for (size_t i = 0; i < requiredProperties.size(); ++i) {
                if (i > 0) json += ", ";
                json += "\"" + requiredProperties[i] + "\"";
            }
            json += "],\n";
        }
        json += "  \"type\": \"object\"\n";
        json += "}\n";
        
        return json;
    }
    
private:
    std::string schemaTypeToString(SchemaValueType type) const {
        switch (type) {
            case SchemaValueType::Integer: return "integer";
            case SchemaValueType::Double: return "number";
            case SchemaValueType::Boolean: return "boolean";
            case SchemaValueType::String: return "string";
            case SchemaValueType::IntArray: return "array of integers";
            case SchemaValueType::DoubleArray: return "array of numbers";
            case SchemaValueType::StringArray: return "array of strings";
            case SchemaValueType::Object: return "object";
            case SchemaValueType::Array: return "array";
            default: return "string";
        }
    }
};

// Schema-based configuration manager
class SchemaBasedConfig {
public:
    SchemaBasedConfig(const Schema& schema) : schema_(schema) {}
    
    bool validate(const ConfigValue& value) const {
        return schema_.validate(value);
    }
    
    std::string getErrorMessage(const ConfigValue& value) const {
        return "Validation failed for schema \"" + schema_.name + "\"";
    }
    
    std::string schemaToJSON() const {
        return schema_.toJSON();
    }
    
    std::string getSchemaName() const { return schema_.name; }
    std::string getSchemaDescription() const { return schema_.description; }
    std::string getSchemaVersion() const { return schema_.version; }
    
private:
    Schema schema_;
};

// Global schema registry
class SchemaRegistry {
public:
    static SchemaRegistry& getInstance() {
        static SchemaRegistry instance;
        return instance;
    }
    
    void registerSchema(const Schema& schema) {
        schemas_[schema.name] = SchemaBasedConfig(schema);
    }
    
    SchemaBasedConfig* getSchema(const std::string& name) {
        auto it = schemas_.find(name);
        if (it != schemas_.end()) {
            return &it->second;
        }
        return nullptr;
    }
    
    bool hasSchema(const std::string& name) const {
        return schemas_.find(name) != schemas_.end();
    }
    
    std::vector<std::string> getSchemaNames() const {
        std::vector<std::string> names;
        for (const auto& pair : schemas_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    void unregisterSchema(const std::string& name) {
        schemas_.erase(name);
    }
    
private:
    SchemaRegistry() {}
    std::map<std::string, SchemaBasedConfig> schemas_;
};

// Convenience functions for common schemas
inline Schema createBrainSchema() {
    Schema schema("BrainConfiguration", "Configuration for NLM brain simulation", "1.0");
    
    // Neuron count
    SchemaProperty neuronCount("neuron_count", SchemaValueType::Integer, "Number of neurons in the brain");
    neuronCount.defaultValue = "500";
    neuronCount.constraints.push_back(SchemaConstraint("range", "1,10000", "Neuron count must be between 1 and 10000"));
    schema.addProperty(neuronCount);
    
    // Region count
    SchemaProperty regionCount("region_count", SchemaValueType::Integer, "Number of neural regions");
    regionCount.defaultValue = "1";
    regionCount.constraints.push_back(SchemaConstraint("range", "1,10", "Region count must be between 1 and 10"));
    schema.addProperty(regionCount);
    
    // Connection probability
    SchemaProperty connectionProb("connection_probability", SchemaValueType::Double, "Probability of synapse formation");
    connectionProb.defaultValue = "0.1";
    connectionProb.constraints.push_back(SchemaConstraint("range", "0.0,1.0", "Connection probability must be between 0 and 1"));
    schema.addProperty(connectionProb);
    
    // Simulation timestep
    SchemaProperty timestep("simulation_timestep", SchemaValueType::Double, "Simulation time step in seconds");
    timestep.defaultValue = "0.001";
    timestep.constraints.push_back(SchemaConstraint("range", "0.0001,1.0", "Time step must be between 0.0001s and 1.0s"));
    schema.addProperty(timestep);
    
    // STDP parameters
    SchemaProperty stdpLTP("stdp_ltp_weight", SchemaValueType::Double, "STDP long-term potentiation weight");
    stdpLTP.defaultValue = "0.01";
    schema.addProperty(stdpLTP);
    
    SchemaProperty stdpLTD("stdp_ltd_weight", SchemaValueType::Double, "STDP long-term depression weight");
    stdpLTD.defaultValue = "0.012";
    schema.addProperty(stdpLTD);
    
    SchemaProperty stdpTau("stdp_tau", SchemaValueType::Double, "STDP time constant");
    stdpTau.defaultValue = "20.0";
    schema.addProperty(stdpTau);
    
    // Plasticity rates
    SchemaProperty synaptogenesis("synaptogenesis_rate", SchemaValueType::Double, "Rate of new synapse formation");
    synaptogenesis.defaultValue = "0.0001";
    schema.addProperty(synaptogenesis);
    
    SchemaProperty pruning("pruning_rate", SchemaValueType::Double, "Rate of synapse pruning");
    pruning.defaultValue = "0.00001";
    schema.addProperty(pruning);
    
    return schema;
}

inline Schema createWorldSchema() {
    Schema schema("WorldConfiguration", "Configuration for NLM world simulation", "1.0");
    
    SchemaProperty width("environment_width", SchemaValueType::Integer, "Width of the simulation world");
    width.defaultValue = "20";
    width.constraints.push_back(SchemaConstraint("range", "10,100", "World width must be between 10 and 100"));
    schema.addProperty(width);
    
    SchemaProperty height("environment_height", SchemaValueType::Integer, "Height of the simulation world");
    height.defaultValue = "20";
    height.constraints.push_back(SchemaConstraint("range", "10,100", "World height must be between 10 and 100"));
    schema.addProperty(height);
    
    SchemaProperty visionWidth("vision_width", SchemaValueType::Integer, "Vision width in pixels");
    visionWidth.defaultValue = "16";
    schema.addProperty(visionWidth);
    
    SchemaProperty visionHeight("vision_height", SchemaValueType::Integer, "Vision height in pixels");
    visionHeight.defaultValue = "16";
    schema.addProperty(visionHeight);
    
    return schema;
}

inline Schema createAgentSchema() {
    Schema schema("AgentConfiguration", "Configuration for NLM agent", "1.0");
    
    SchemaProperty maxEnergy("max_energy", SchemaValueType::Double, "Maximum energy level");
    maxEnergy.defaultValue = "100.0";
    maxEnergy.constraints.push_back(SchemaConstraint("range", "10.0,1000.0", "Max energy must be between 10.0 and 1000.0"));
    schema.addProperty(maxEnergy);
    
    SchemaProperty energyDecay("energy_decay_rate", SchemaValueType::Double, "Energy decay rate per timestep");
    energyDecay.defaultValue = "0.01";
    energyDecay.constraints.push_back(SchemaConstraint("range", "0.0,0.1", "Energy decay rate must be between 0.0 and 0.1"));
    schema.addProperty(energyDecay);
    
    return schema;
}

} // namespace nlm