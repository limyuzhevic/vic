// NLM Advanced CLI - Parameter Modifier Implementation
#pragma once

#include "tools/parameter_modifier.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <sstream>
#include <string>

namespace nlm {

parameter_modifier::parameter_modifier(std::shared_ptr<Brain> brain)
    : brain_(brain) {
}

parameter_modifier::~parameter_modifier() = default;

bool parameter_modifier::setParameter(const std::string& name, const std::string& value, const std::string& region) {
    if (!brain_) {
        std::cout << "Error: No brain provided for parameter modification." << std::endl;
        return false;
    }
    
    std::cout << "Setting parameter: " << name << " = " << value << std::endl;
    if (!region.empty()) {
        std::cout << "In region: " << region << std::endl;
    }
    
    // Parse parameter name and determine type
    std::string paramType = "general";
    double doubleValue = 0.0;
    int intValue = 0;
    bool boolValue = false;
    
    // Try to parse as double first (for most numeric parameters)
    try {
        size_t pos;
        doubleValue = std::stod(value, &pos);
        if (pos == value.size()) {
            paramType = "double";
        }
    } catch (...) {
        // Not a double
    }
    
    if (paramType != "double") {
        // Try as integer
        try {
            size_t pos;
            intValue = std::stoi(value, &pos);
            if (pos == value.size()) {
                paramType = "int";
            }
        } catch (...) {
            // Not an int
        }
    }
    
    if (paramType != "double" && paramType != "int") {
        // Try as boolean
        if (value == "true" || value == "1" || value == "yes") {
            boolValue = true;
            paramType = "bool";
        } else if (value == "false" || value == "0" || value == "no") {
            boolValue = false;
            paramType = "bool";
        } else {
            // Treat as string
            paramType = "string";
        }
    }
    
    // Apply parameter based on type and name
    if (!applyParameterByType(name, paramType, doubleValue, intValue, boolValue, region)) {
        std::cout << "Failed to set parameter: " << name << std::endl;
        return false;
    }
    
    return true;
}

bool parameter_modifier::applyParameterByType(const std::string& name, const std::string& type, double dValue, int iValue, bool bValue, const std::string& region) {
    // Common brain parameters that can be modified
    if (name == "neuron_count" || name == "neuron_count") {
        if (type == "int") {
            // Would require changing brain size - not typically done
            std::cout << "Note: Changing neuron count requires brain reset and reinitialization." << std::endl;
        }
        return false;
    } else if (name == "connection_probability" || name == "synapse_density") {
        if (type == "double" && brain_) {
            // This would require modifying the brain's connection probabilities
            std::cout << "Would modify connection probability to: " << dValue << std::endl;
        }
        return false;
    } else if (name == "plasticity_enabled" || name == "enable_plasticity") {
        if (type == "bool" && brain_) {
            brain_->setPlasticityEnabled(bValue);
            std::cout << "Plasticity " << (bValue ? "enabled" : "disabled") << std::endl;
        }
        return true;
    } else if (name == "development_stage" || name == "dev_stage") {
        if (type == "int" && brain_) {
            // This would require setting development stage
            std::cout << "Would set development stage to: " << iValue << std::endl;
        }
        return false;
    } else if (name == "synapse_weight" || name == "weight") {
        if (type == "double" && brain_) {
            // This would require modifying specific synapse weights
            std::cout << "Would modify synapse weight to: " << dValue << std::endl;
        }
        return false;
    } else if (name == "threshold" || name == "neuron_threshold") {
        if (type == "double" && brain_) {
            // This would require modifying neuron thresholds
            std::cout << "Would modify neuron threshold to: " << dValue << std::endl;
        }
        return false;
    } else if (name == "temperature" || name == "sim_temp") {
        if (type == "double") {
            std::cout << "Would set simulation temperature to: " << dValue << std::endl;
        }
        return true;
    } else if (name == "learning_rate" || name == "lr") {
        if (type == "double") {
            std::cout << "Would set learning rate to: " << dValue << std::endl;
        }
        return true;
    } else {
        std::cout << "Unknown parameter: " << name << std::endl;
        return false;
    }
}

bool parameter_modifier::getParameter(const std::string& name, std::string& value, const std::string& region) {
    if (!brain_) {
        std::cout << "Error: No brain provided for parameter retrieval." << std::endl;
        return false;
    }
    
    std::cout << "Getting parameter: " << name << std::endl;
    if (!region.empty()) {
        std::cout << "From region: " << region << std::endl;
    }
    
    // This would require implementing parameter retrieval from the brain
    // For now, just return a placeholder value
    value = "0.0 (placeholder - implementation needed)";
    
    return true;
}

parameter_modifier::parameter_modifier() : brain_(nullptr) {
}

} // namespace nlm

