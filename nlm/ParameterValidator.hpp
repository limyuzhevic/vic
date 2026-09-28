/*
 * nlm/ParameterValidator.hpp - Neural parameter validation utilities for the NLM system
 * Specialized validation for neural, synaptic, and plasticity parameters
 */

#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include "nlm/Exceptions.hpp"

namespace nlm {

class ParameterValidator {
public:
    // Neuron parameter validation
    static void validateNeuronParameters(const std::unordered_map<std::string, double>& params);
    
    // Synaptic parameter validation
    static void validateSynapticParameters(const std::unordered_map<std::string, double>& params);
    
    // Plasticity parameter validation
    static void validatePlasticityParameters(const std::unordered_map<std::string, double>& params);
    
    // Validate individual neuron parameter
    static void validateNeuronParameter(const std::string& name, double value);
    
    // Validate individual synaptic parameter
    static void validateSynapticParameter(const std::string& name, double value);
    
    // Validate individual plasticity parameter
    static void validatePlasticityParameter(const std::string& name, double value);
    
    // Range checking utilities
    template<typename T>
    static void validateRange(const std::string& name, T value, T min, T max, 
                              const std::string& file = "", int line = 0,
                              const std::string& function = "");
    
    // Type checking utilities
    template<typename T>
    static void validateType(const std::string& name, const T& value,
                             const std::string& file = "", int line = 0,
                             const std::string& function = "");
};

// Neuron parameter validation implementation
inline void ParameterValidator::validateNeuronParameters(const std::unordered_map<std::string, double>& params) {
    for (const auto& pair : params) {
        validateNeuronParameter(pair.first, pair.second);
    }
}

inline void ParameterValidator::validateSynapticParameters(const std::unordered_map<std::string, double>& params) {
    for (const auto& pair : params) {
        validateSynapticParameter(pair.first, pair.second);
    }
}

inline void ParameterValidator::validatePlasticityParameters(const std::unordered_map<std::string, double>& params) {
    for (const auto& pair : params) {
        validatePlasticityParameter(pair.first, pair.second);
    }
}

inline void ParameterValidator::validateNeuronParameter(const std::string& name, double value) {
    if (name == "capacitance") {
        if (value < 0.1f || value > 1.0f) {
            throw NeuralParameterException(name, value, getCurrentFile(), getCurrentLine(), getCurrentFunction());
        }
    } else if (name == "timeConstant") {
        if (value < 10.0f || value > 30.0f) {
            throw NeuralParameterException(name, value, getCurrentFile(), getCurrentLine(), getCurrentFunction());
        }
    } else if (name == "leakConductance") {
        if (value < -70.0f || value > -50.0f) {
            throw NeuralParameterException(name, value, getCurrentFile(), getCurrentLine(), getCurrentFunction());
        }
    } else if (name == "threshold") {
        if (value < -50.0f || value > -40.0f) {
            throw NeuralParameterException(name, value, getCurrentFile(), getCurrentLine(), getCurrentFunction());
        }
    } else if (name == "refractoryPeriod") {
        if (value < 2 || value > 10) {
            throw NeuralParameterException(name, value, getCurrentFile(), getCurrentLine(), getCurrentFunction());
        }
    }
}

inline void ParameterValidator::validateSynapticParameter(const std::string& name, double value) {
    if (name == "excitatoryWeight") {
        if (value < 0.1f || value > 10.0f) {
            throw NeuralParameterException(name, value, getCurrentFile(), getCurrentLine(), getCurrentFunction());
        }
    } else if (name == "inhibitoryWeight") {
        if (value < -10.0f || value > -0.1f) {
            throw NeuralParameterException(name, value, getCurrentFile(), getCurrentLine(), getCurrentFunction());
        }
    } else if (name == "connectivityProbability") {
        if (value < 0.0f || value > 1.0f) {
            throw NeuralParameterException(name, value, getCurrentFile(), getCurrentLine(), getCurrentFunction());
        }
    }
}

inline void ParameterValidator::validatePlasticityParameter(const std::string& name, double value) {
    if (name == "stdpLearningRate") {
        if (value < 0.01f || value > 0.1f) {
            throw NeuralParameterException(name, value, getCurrentFile(), getCurrentLine(), getCurrentFunction());
        }
    } else if (name == "hebbianLearningRate") {
        if (value < 0.01f || value > 0.1f) {
            throw NeuralParameterException(name, value, getCurrentFile(), getCurrentLine(), getCurrentFunction());
        }
    } else if (name == "rewardLearningRate") {
        if (value < 0.0f || value > 1.0f) {
            throw NeuralParameterException(name, value, getCurrentFile(), getCurrentLine(), getCurrentFunction());
        }
    }
}

// Range checking implementation
inline void ParameterValidator::validateRange(const std::string& name, double value, double min, double max,
                                              const std::string& file, int line, const std::string& function) {
    if (value < min || value > max) {
        throw NeuralParameterException(name, value, file, line, function);
    }
}

inline void ParameterValidator::validateType(const std::string& name, const std::string& value,
                                             const std::string& file, int line, const std::string& function) {
    // String validation - check for empty or invalid characters
    if (value.empty()) {
        throw NeuralParameterException(name, 0.0f, file, line, function);
    }
    
    // Additional type-specific validation can be added here
}

} // namespace nlm
