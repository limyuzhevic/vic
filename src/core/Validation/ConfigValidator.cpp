// Implementation of configuration validation framework

#include "ConfigValidator.hpp"
#include <numeric>
#include <sstream>

namespace nlm {

// ConfigValidator implementation
std::string ConfigValidator::formatAllowedValues(const std::vector<std::string>& values) {
    std::ostringstream oss;
    for (size_t i = 0; i < values.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << "\"" << values[i] << "\"";
    }
    return oss.str();
}

// SimulationValidator implementation
ValidationResult SimulationValidator::validateSimulationParameters(const SimulationParameters& params) {
    ValidationResult result;
    std::map<std::string, std::string> errors;
    
    // Basic parameter validation
    if (params.timestep <= 0.0) {
        errors["timestep"] = "Must be positive, got " + std::to_string(params.timestep);
        result.addSuggestion("Set timestep to a small positive value (e.g., 0.001)");
    }
    
    if (params.maxSteps < 0) {
        errors["max_steps"] = "Must be non-negative, got " + std::to_string(params.maxSteps);
        result.addSuggestion("Set max_steps to a non-negative value");
    }
    
    if (params.timeLimit < 0.0) {
        errors["time_limit"] = "Must be non-negative, got " + std::to_string(params.timeLimit);
        result.addSuggestion("Set time_limit to a non-negative value");
    }
    
    if (params.neuronCount <= 0) {
        errors["neuron_count"] = "Must be positive, got " + std::to_string(params.neuronCount);
        result.addSuggestion("Set neuron_count to a positive value");
    }
    
    if (params.regionCount <= 0) {
        errors["region_count"] = "Must be positive, got " + std::to_string(params.regionCount);
        result.addSuggestion("Set region_count to a positive value");
    }
    
    if (params.connectionProbability < 0.0 || params.connectionProbability > 1.0) {
        errors["connection_probability"] = 
            "Must be between 0 and 1, got " + std::to_string(params.connectionProbability);
        result.addSuggestion("Set connection_probability to a value between 0 and 1");
    }
    
    // Cross-parameter validation
    if (params.neuronCount < params.regionCount * 10) {
        errors["neuron_region_ratio"] = 
            "neuron_count (" + std::to_string(params.neuronCount) + ") "
            "should be at least 10 times region_count (" + std::to_string(params.regionCount) + ")";
        result.addSuggestion("Increase neuron_count or decrease region_count");
    }
    
    if (!errors.empty()) {
        result.valid = false;
        result.message = "Simulation parameter validation failed";
        for (const auto& pair : errors) {
            result.addDetail(pair.first, pair.second);
        }
    } else {
        result.message = "All simulation parameters are valid";
    }
    
    return result;
}

// NeuronValidator implementation
ValidationResult NeuronValidator::validateNeuronParameters(const NeuronParameters& params) {
    ValidationResult result;
    std::map<std::string, std::string> errors;
    
    // Physical parameter validation
    if (params.threshold <= params.restingPotential) {
        errors["threshold_resting"] = 
            "Threshold (" + std::to_string(params.threshold) + ") "
            "must be greater than resting potential (" + std::to_string(params.restingPotential) + ")";
        result.addSuggestion("Set threshold to a value higher than resting potential");
    }
    
    if (params.resetPotential <= params.restingPotential) {
        errors["reset_resting"] = 
            "Reset potential (" + std::to_string(params.resetPotential) + ") "
            "must be greater than resting potential (" + std::to_string(params.restingPotential) + ")";
        result.addSuggestion("Set reset potential to a value higher than resting potential");
    }
    
    if (params.refractoryPeriod == 0) {
        errors["refractory_period"] = 
            "Refractory period (0) must be greater than 0";
        result.addSuggestion("Set refractory period to a positive value (e.g., 2)");
    }
    
    if (params.leakConductance < 0.0 || params.leakConductance > 1.0) {
        errors["leak_conductance"] = 
            "Leak conductance (" + std::to_string(params.leakConductance) + ") "
            "must be between 0 and 1";
        result.addSuggestion("Set leak conductance to a value between 0 and 1");
    }
    
    if (params.adaptationVariable < 0.0) {
        errors["adaptation_variable"] = 
            "Adaptation variable (" + std::to_string(params.adaptationVariable) + ") "
            "must be non-negative";
        result.addSuggestion("Set adaptation variable to a non-negative value");
    }
    
    if (!errors.empty()) {
        result.valid = false;
        result.message = "Neuron parameter validation failed";
        for (const auto& pair : errors) {
            result.addDetail(pair.first, pair.second);
        }
    } else {
        result.message = "All neuron parameters are valid";
    }
    
    return result;
}

// SynapseValidator implementation
ValidationResult SynapseValidator::validateSynapseParameters(const SynapseParameters& params) {
    ValidationResult result;
    std::map<std::string, std::string> errors;
    
    // Connectivity validation
    if (params.sourceNeuron == params.destinationNeuron) {
        errors["self_connection"] = 
            "Synapse connects neuron to itself (ID " + 
            std::to_string(static_cast<long long>(params.sourceNeuron)) + ")";
        result.addSuggestion("Remove self-connection or create a different synapse");
    }
    
    // Weight validation
    if (params.weight < -1.0f || params.weight > 1.0f) {
        errors["weight_bounds"] = 
            "Weight (" + std::to_string(params.weight) + ") "
            "must be between -1.0 and 1.0";
        result.addSuggestion("Adjust weight to be within [-1.0, 1.0]");
    }
    
    // Delay validation
    if (params.delay <= 0) {
        errors["delay"] = 
            "Delay (" + std::to_string(params.delay) + ") "
            "must be positive";
        result.addSuggestion("Set delay to a positive value");
    } else if (params.delay > 100) {
        errors["delay_too_large"] = 
            "Delay (" + std::to_string(params.delay) + ") "
            "should be <= 100 steps (practical limit)";
        result.addSuggestion("Reduce delay to at most 100 steps");
    }
    
    // Efficacy validation
    if (params.efficacy < 0.0f || params.efficacy > 2.0f) {
        errors["efficacy_bounds"] = 
            "Efficacy (" + std::to_string(params.efficacy) + ") "
            "must be between 0.0 and 2.0";
        result.addSuggestion("Adjust efficacy to be within [0.0, 2.0]");
    }
    
    // Short-term plasticity state validation
    if (params.shortTermDepression < 0.0f || params.shortTermDepression > 1.0f) {
        errors["depression_bounds"] = 
            "Short-term depression (" + std::to_string(params.shortTermDepression) + ") "
            "must be between 0.0 and 1.0";
        result.addSuggestion("Adjust short-term depression to be within [0.0, 1.0]");
    }
    
    if (params.shortTermFacilitation < 0.0f || params.shortTermFacilitation > 1.0f) {
        errors["facilitation_bounds"] = 
            "Short-term facilitation (" + std::to_string(params.shortTermFacilitation) + ") "
            "must be between 0.0 and 1.0";
        result.addSuggestion("Adjust short-term facilitation to be within [0.0, 1.0]");
    }
    
    if (!errors.empty()) {
        result.valid = false;
        result.message = "Synapse parameter validation failed";
        for (const auto& pair : errors) {
            result.addDetail(pair.first, pair.second);
        }
    } else {
        result.message = "All synapse parameters are valid";
    }
    
    return result;
}

// DevelopmentValidator implementation
ValidationResult DevelopmentValidator::validateDevelopmentParameters(const DevelopmentParameters& params) {
    ValidationResult result;
    std::map<std::string, std::string> errors;
    
    // Maturity validation
    if (params.maturity < 0.0f || params.maturity > 1.0f) {
        errors["maturity_bounds"] = 
            "Maturity (" + std::to_string(params.maturity) + ") "
            "must be between 0.0 and 1.0";
        result.addSuggestion("Adjust maturity to be within [0.0, 1.0]");
    }
    
    // Complexity validation
    if (params.complexity < 0.0f || params.complexity > 1.0f) {
        errors["complexity_bounds"] = 
            "Complexity (" + std::to_string(params.complexity) + ") "
            "must be between 0.0 and 1.0";
        result.addSuggestion("Adjust complexity to be within [0.0, 1.0]");
    }
    
    // Growth validation
    if (params.maturity < params.complexity) {
        errors["maturity_complexity"] = 
            "Maturity (" + std::to_string(params.maturity) + ") "
            "should be >= complexity (" + std::to_string(params.complexity) + ")";
        result.addSuggestion("Increase maturity or decrease complexity");
    }
    
    // Resource validation
    if (params.neuronCount == 0) {
        errors["neuron_count"] = "Neuron count cannot be zero";
        result.addSuggestion("Add at least one neuron");
    }
    
    if (params.synapseCount < params.neuronCount) {
        errors["synapse_neuron_ratio"] = 
            "Synapse count (" + std::to_string(params.synapseCount) + ") "
            "should be at least neuron count (" + std::to_string(params.neuronCount) + ")";
        result.addSuggestion("Increase synapse count or decrease neuron count");
    }
    
    if (!errors.empty()) {
        result.valid = false;
        result.message = "Development parameter validation failed";
        for (const auto& pair : errors) {
            result.addDetail(pair.first, pair.second);
        }
    } else {
        result.message = "All development parameters are valid";
    }
    
    return result;
}

// MemoryValidator implementation
ValidationResult MemoryValidator::validateMemoryParameters(const MemoryParameters& params) {
    ValidationResult result;
    std::map<std::string, std::string> errors;
    
    // Capacity validation
    if (params.capacity == 0) {
        errors["capacity"] = "Capacity cannot be zero";
        result.addSuggestion("Set capacity to a positive value");
    }
    
    if (params.allocated > params.capacity) {
        errors["allocated_capacity"] = 
            "Allocated memory (" + std::to_string(params.allocated) + ") "
            "exceeds capacity (" + std::to_string(params.capacity) + ")";
        result.addSuggestion("Free up some memory or increase capacity");
    }
    
    // Consistency validation
    if (params.consistency < 0.0f || params.consistency > 1.0f) {
        errors["consistency_bounds"] = 
            "Consistency (" + std::to_string(params.consistency) + ") "
            "must be between 0.0 and 1.0";
        result.addSuggestion("Adjust consistency to be within [0.0, 1.0]");
    }
    
    // Utilization validation
    if (params.utilization < 0.0f || params.utilization > 1.0f) {
        errors["utilization_bounds"] = 
            "Utilization (" + std::to_string(params.utilization) + ") "
            "must be between 0.0 and 1.0";
        result.addSuggestion("Adjust utilization to be within [0.0, 1.0]");
    }
    
    // Free memory validation
    if (params.free > params.capacity) {
        errors["free_capacity"] = 
            "Free memory (" + std::to_string(params.free) + ") "
            "exceeds capacity (" + std::to_string(params.capacity) + ")";
        result.addSuggestion("Adjust free memory calculation");
    }
    
    // Utilization consistency check
    if (params.capacity > 0) {
        float calculatedUtilization = 
            static_cast<float>(params.allocated) / params.capacity;
        if (std::abs(calculatedUtilization - params.utilization) > 0.01f) {
            errors["utilization_inconsistency"] = 
                "Utilization mismatch: calculated " + std::to_string(calculatedUtilization) + 
                " but stored " + std::to_string(params.utilization);
            result.addSuggestion("Update utilization to match actual allocation");
        }
    }
    
    if (!errors.empty()) {
        result.valid = false;
        result.message = "Memory parameter validation failed";
        for (const auto& pair : errors) {
            result.addDetail(pair.first, pair.second);
        }
    } else {
        result.message = "All memory parameters are valid";
    }
    
    return result;
}

} // namespace nlm