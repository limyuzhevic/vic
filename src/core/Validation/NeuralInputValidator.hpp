#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include <memory>
#include <chrono>
#include <algorithm>
#include <cmath>
#include "ErrorSystem.hpp"

namespace nlm {

// Validation result structure
struct ValidationResult {
    bool valid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    double confidence = 1.0;
    
    ValidationResult() : valid(true) {}
    ValidationResult(bool v, const std::vector<std::string>& errs, const std::vector<std::string>& warns = {})
        : valid(v), errors(errs), warnings(warns) {}
    
    static ValidationResult success() { return ValidationResult(); }
    static ValidationResult failure(const std::string& error) { 
        return ValidationResult(false, {error}); 
    }
    static ValidationResult failure(const std::vector<std::string>& errors) { 
        return ValidationResult(false, errors); 
    }
    
    void addError(const std::string& error) {
        errors.push_back(error);
        valid = false;
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    std::string getSummary() const {
        std::ostringstream oss;
        if (valid) {
            oss << "Validation PASSED";
        } else {
            oss << "Validation FAILED with " << errors.size() << " error(s) and " 
                << warnings.size() << " warning(s)";
            if (!errors.empty()) {
                oss << "\nErrors:";
                for (const auto& err : errors) {
                    oss << "\n  - " << err;
                }
            }
            if (!warnings.empty()) {
                oss << "\nWarnings:";
                for (const auto& warn : warnings) {
                    oss << "\n  - " << warn;
                }
            }
        }
        return oss.str();
    }
};

// Neural input validation
class NeuralInputValidator {
public:
    struct NeuronInput {
        float membranePotential;
        float inputCurrent;
        float threshold;
        float restingPotential;
        float leakConductance;
        std::chrono::system_clock::time_point timestamp;
        
        ValidationResult validate() const {
            ValidationResult result;
            
            // Validate membrane potential range
            if (std::isnan(membranePotential) || std::isinf(membranePotential)) {
                result.addError("Invalid membrane potential: " + std::to_string(membranePotential));
            } else if (membranePotential < -1000.0f || membranePotential > 1000.0f) {
                result.addWarning("Unusual membrane potential: " + std::to_string(membranePotential));
            }
            
            // Validate input current
            if (std::isnan(inputCurrent) || std::isinf(inputCurrent)) {
                result.addError("Invalid input current: " + std::to_string(inputCurrent));
            } else if (inputCurrent < -100.0f || inputCurrent > 100.0f) {
                result.addWarning("Unusual input current: " + std::to_string(inputCurrent));
            }
            
            // Validate threshold
            if (std::isnan(threshold) || std::isinf(threshold)) {
                result.addError("Invalid threshold: " + std::to_string(threshold));
            } else if (threshold < -100.0f || threshold > 100.0f) {
                result.addWarning("Unusual threshold: " + std::to_string(threshold));
            }
            
            // Validate resting potential
            if (std::isnan(restingPotential) || std::isinf(restingPotential)) {
                result.addError("Invalid resting potential: " + std::to_string(restingPotential));
            }
            
            // Validate leak conductance
            if (leakConductance < 0.0f) {
                result.addError("Negative leak conductance: " + std::to_string(leakConductance));
            }
            
            // Validate timestamp is not in the future
            auto now = std::chrono::system_clock::now();
            if (timestamp > now) {
                result.addWarning("Future timestamp in neural input");
            }
            
            return result;
        }
    };
    
    struct SynapseInput {
        NeuronId sourceNeuron;
        NeuronId destinationNeuron;
        float weight;
        Delay delay;
        float eligibilityTrace;
        
        ValidationResult validate() const {
            ValidationResult result;
            
            if (sourceNeuron == destinationNeuron) {
                result.addError("Synapse cannot connect a neuron to itself");
            }
            
            // Validate weight range
            if (std::isnan(weight) || std::isinf(weight)) {
                result.addError("Invalid weight: " + std::to_string(weight));
            } else if (weight < -10.0f || weight > 10.0f) {
                result.addWarning("Unusual weight magnitude: " + std::to_string(weight));
            }
            
            // Validate delay
            if (delay < 0) {
                result.addError("Negative delay: " + std::to_string(delay));
            }
            
            // Validate eligibility trace
            if (std::isnan(eligibilityTrace) || std::isinf(eligibilityTrace)) {
                result.addError("Invalid eligibility trace: " + std::to_string(eligibilityTrace));
            } else if (eligibilityTrace < 0.0f) {
                result.addError("Negative eligibility trace: " + std::to_string(eligibilityTrace));
            }
            
            return result;
        }
    };
    
    struct NeuralNetworkState {
        std::vector<NeuronInput> neurons;
        std::vector<SynapseInput> synapses;
        std::vector<std::pair<NeuronId, NeuronId>> connections;
        
        ValidationResult validate() const {
            ValidationResult result;
            
            // Check for duplicate neuron IDs
            std::unordered_map<NeuronId, size_t> neuronCount;
            for (const auto& neuron : neurons) {
                neuronCount[neuron.getId()]++;
            }
            
            for (const auto& pair : neuronCount) {
                if (pair.second > 1) {
                    result.addError("Duplicate neuron ID: " + std::to_string(pair.first));
                }
            }
            
            // Check for duplicate synapse IDs
            std::unordered_map<std::pair<NeuronId, NeuronId>, size_t> synapseCount;
            for (const auto& syn : synapses) {
                synapseCount[{syn.sourceNeuron, syn.destinationNeuron}]++;
            }
            
            for (const auto& pair : synapseCount) {
                if (pair.second > 1) {
                    result.addError("Duplicate synapse connection: " + 
                                  std::to_string(pair.first.first) + " -> " + 
                                  std::to_string(pair.first.second));
                }
            }
            
            // Check that all connections have corresponding neurons
            for (const auto& conn : connections) {
                bool hasSource = std::any_of(neurons.begin(), neurons.end(),
                                             [id = conn.first](const NeuronInput& n) { return n.getId() == id; });
                bool hasDest = std::any_of(neurons.begin(), neurons.end(),
                                           [id = conn.second](const NeuronInput& n) { return n.getId() == id; });
                
                if (!hasSource) {
                    result.addError("Connection references non-existent source neuron: " + std::to_string(conn.first));
                }
                if (!hasDest) {
                    result.addError("Connection references non-existent destination neuron: " + std::to_string(conn.second));
                }
            }
            
            // Validate population sizes
            for (const auto& neuron : neurons) {
                auto validation = neuron.validate();
                if (!validation.valid) {
                    result.errors.push_back(
                        "Neuron " + std::to_string(neuron.getId()) + ": " +
                        validation.errors[0]
                    );
                }
            }
            
            for (const auto& synapse : synapses) {
                auto validation = synapse.validate();
                if (!validation.valid) {
                    result.errors.push_back(
                        "Synapse " + std::to_string(synapse.sourceNeuron) + " -> " + 
                        std::to_string(synapse.destinationNeuron) + ": " +
                        validation.errors[0]
                    );
                }
            }
            
            return result;
        }
    };
    
    // Static validation methods
    static ValidationResult validateInputRange(float value, float min, float max, const std::string& paramName) {
        ValidationResult result;
        
        if (std::isnan(value) || std::isinf(value)) {
            result.addError("Invalid " + paramName + ": NaN or infinite value");
        } else if (value < min || value > max) {
            result.addError("Invalid " + paramName + ": must be between " + 
                          std::to_string(min) + " and " + std::to_string(max) +
                          ", got " + std::to_string(value));
        }
        
        return result;
    }
    
    static ValidationResult validateInputRange(double value, double min, double max, const std::string& paramName) {
        ValidationResult result;
        
        if (std::isnan(value) || std::isinf(value)) {
            result.addError("Invalid " + paramName + ": NaN or infinite value");
        } else if (value < min || value > max) {
            result.addError("Invalid " + paramName + ": must be between " + 
                          std::to_string(min) + " and " + std::to_string(max) +
                          ", got " + std::to_string(value));
        }
        
        return result;
    }
    
    static ValidationResult validateInputRange(int64_t value, int64_t min, int64_t max, const std::string& paramName) {
        ValidationResult result;
        
        if (value < min || value > max) {
            result.addError("Invalid " + paramName + ": must be between " + 
                          std::to_string(min) + " and " + std::to_string(max) +
                          ", got " + std::to_string(value));
        }
        
        return result;
    }
    
    static ValidationResult validatePositive(const std::string& paramName, double value, double epsilon = 1e-10) {
        ValidationResult result;
        
        if (value < -epsilon) {
            result.addError("Invalid " + paramName + ": must be non-negative, got " + std::to_string(value));
        }
        
        return result;
    }
    
    static ValidationResult validateNotZero(const std::string& paramName, double value, double epsilon = 1e-10) {
        ValidationResult result;
        
        if (std::abs(value) < epsilon) {
            result.addError("Invalid " + paramName + ": cannot be zero");
        }
        
        return result;
    }
};

// Runtime validation for neural computations
class RuntimeValidator {
public:
    struct RuntimeContext {
        std::chrono::system_clock::time_point timestamp;
        size_t currentStep;
        float simulationTimestep;
        float globalPlasiticityModulation;
        bool isRestingPhase;
        
        ValidationResult validate() const {
            ValidationResult result;
            
            if (simulationTimestep <= 0.0f) {
                result.addError("Invalid simulation timestep: must be positive");
            }
            
            if (globalPlasiticityModulation < 0.0f || globalPlasiticityModulation > 2.0f) {
                result.addWarning("Unusual plasticity modulation: " + std::to_string(globalPlasiticityModulation));
            }
            
            return result;
        }
    };
    
    // Runtime validation for neuron state updates
    static ValidationResult validateNeuronStep(const NeuralInputValidator::NeuronInput& neuron,
                                               const RuntimeContext& context) {
        ValidationResult result;
        
        // Check if neuron is in refractory period
        if (neuron.firingState == FiringState::Refractory && 
            neuron.refractoryRemaining > 0) {
            result.addError("Neuron " + std::to_string(neuron.getId()) + 
                          " is in refractory period with remaining time: " +
                          std::to_string(neuron.refractoryRemaining));
        }
        
        // Validate spike timing
        if (neuron.lastSpikeTime > 0.0f) {
            double timeSinceLastSpike = static_cast<double>(context.timestamp.time_since_epoch().count()) -
                                       static_cast<double>(neuron.lastSpikeTime);
            if (timeSinceLastSpike < 0) {
                result.addError("Future spike time detected for neuron " + std::to_string(neuron.getId()));
            }
        }
        
        return result;
    }
    
    // Runtime validation for synapse updates
    static ValidationResult validateSynapseUpdate(const NeuralInputValidator::SynapseInput& synapse,
                                                  const RuntimeContext& context) {
        ValidationResult result;
        
        // Validate eligibility trace decay
        if (synapse.eligibilityTrace < 0.0f) {
            result.addError("Negative eligibility trace: " + std::to_string(synapse.eligibilityTrace));
        }
        
        // Check for NaN or infinite weights
        if (std::isnan(synapse.weight) || std::isinf(synapse.weight)) {
            result.addError("Invalid synapse weight: NaN or infinite");
        }
        
        return result;
    }
    
    // Memory access validation
    static ValidationResult validateMemoryAccess(const void* ptr, size_t size, 
                                                  const std::string& accessType = "read") {
        ValidationResult result;
        
        if (ptr == nullptr) {
            result.addError("Null pointer access for " + accessType);
        }
        
        if (size == 0) {
            result.addWarning("Zero-size memory access for " + accessType);
        }
        
        // Check for obvious memory corruption (alignment)
        if (reinterpret_cast<uintptr_t>(ptr) % 8 != 0) {
            result.addWarning("Misaligned memory access for " + accessType);
        }
        
        return result;
    }
    
    // Neural connection validation
    static ValidationResult validateConnections(const std::vector<std::pair<NeuronId, NeuronId>>& connections,
                                                const std::unordered_map<NeuronId, bool>& neuronExists) {
        ValidationResult result;
        
        for (const auto& conn : connections) {
            auto [source, dest] = conn;
            
            if (!neuronExists.count(source)) {
                result.addError("Connection references non-existent source neuron: " + std::to_string(source));
            }
            
            if (!neuronExists.count(dest)) {
                result.addError("Connection references non-existent destination neuron: " + std::to_string(dest));
            }
            
            if (source == dest) {
                result.addError("Self-connection detected: neuron " + std::to_string(source));
            }
        }
        
        return result;
    }
};

// Configuration validation with enhanced schema support
class EnhancedConfigValidator {
public:
    struct SchemaConstraint {
        std::string key;
        std::string type; // "int", "double", "string", "bool", "array"
        std::string constraint; // additional constraint description
        bool required = false;
        double min = std::numeric_limits<double>::lowest();
        double max = std::numeric_limits<double>::max();
        std::string defaultValue;
        std::vector<std::string> allowedValues;
    };
    
    struct ValidationContext {
        std::string profile = "development"; // development, testing, production
        bool strictMode = false;
        std::unordered_map<std::string, std::string> customConstraints;
    };
    
    static ValidationResult validateAgainstSchema(const Config& config, 
                                                   const std::vector<SchemaConstraint>& schema,
                                                   const ValidationContext& context = ValidationContext()) {
        ValidationResult result;
        
        // Check required keys
        for (const auto& constraint : schema) {
            if (constraint.required && !config.has(constraint.key)) {
                result.addError("Missing required configuration key: " + constraint.key);
                continue;
            }
            
            if (!config.has(constraint.key)) {
                if (context.profile == "production" && context.strictMode) {
                    result.addError("Configuration key missing in production strict mode: " + constraint.key);
                } else if (context.profile != "production") {
                    result.addWarning("Configuration key missing: " + constraint.key);
                }
                continue;
            }
            
            // Validate type and constraints
            auto valueOpt = getConfigValue(config, constraint.key, constraint.type);
            if (!valueOpt) {
                result.addError("Invalid type for configuration key: " + constraint.key);
                continue;
            }
            
            double numericValue = extractNumericValue(*valueOpt, constraint.type);
            
            if (constraint.type == "int" || constraint.type == "double") {
                ValidationResult rangeResult = validateRange(numericValue, constraint.min, constraint.max, constraint.key);
                result.errors.insert(result.errors.end(), rangeResult.errors.begin(), rangeResult.errors.end());
                result.warnings.insert(result.warnings.end(), rangeResult.warnings.begin(), rangeResult.warnings.end());
            }
            
            if (!constraint.allowedValues.empty()) {
                bool isAllowed = false;
                std::string stringValue;
                
                if (constraint.type == "string") {
                    auto strOpt = config.get<std::string>(constraint.key);
                    if (strOpt) stringValue = *strOpt;
                } else if (constraint.type == "int") {
                    auto intOpt = config.get<int64_t>(constraint.key);
                    if (intOpt) {
                        stringValue = std::to_string(*intOpt);
                    }
                } else if (constraint.type == "double") {
                    auto doubleOpt = config.get<double>(constraint.key);
                    if (doubleOpt) {
                        stringValue = std::to_string(*doubleOpt);
                    }
                } else if (constraint.type == "bool") {
                    auto boolOpt = config.get<bool>(constraint.key);
                    if (boolOpt) {
                        stringValue = *boolOpt ? "true" : "false";
                    }
                }
                
                for (const auto& allowed : constraint.allowedValues) {
                    if (stringValue == allowed) {
                        isAllowed = true;
                        break;
                    }
                }
                
                if (!isAllowed) {
                    result.addError("Invalid value for " + constraint.key + ": " + stringValue + ". " +
                                  "Allowed values: " + join(constraint.allowedValues, ", "));
                }
            }
        }
        
        return result;
    }
    
private:
    static std::optional<ConfigValue> getConfigValue(const Config& config, 
                                                      const std::string& key, const std::string& type) {
        if (type == "int") {
            return config.get<int64_t>(key);
        } else if (type == "double") {
            return config.get<double>(key);
        } else if (type == "string") {
            return config.get<std::string>(key);
        } else if (type == "bool") {
            return config.get<bool>(key);
        }
        return std::nullopt;
    }
    
    static double extractNumericValue(const ConfigValue& value, const std::string& type) {
        if (type == "int") {
            return std::get<int64_t>(value);
        } else if (type == "double") {
            return std::get<double>(value);
        } else if (type == "bool") {
            return std::get<bool>(value) ? 1.0 : 0.0;
        }
        return 0.0;
    }
    
    static ValidationResult validateRange(double value, double min, double max, const std::string& key) {
        ValidationResult result;
        
        if (value < min) {
            result.addError("Value for " + key + " must be >= " + std::to_string(min) + ", got " + std::to_string(value));
        }
        
        if (value > max) {
            result.addError("Value for " + key + " must be <= " + std::to_string(max) + ", got " + std::to_string(value));
        }
        
        return result;
    }
    
    static std::string join(const std::vector<std::string>& items, const std::string& separator) {
        std::ostringstream oss;
        for (size_t i = 0; i < items.size(); ++i) {
            if (i > 0) oss << separator;
            oss << items[i];
        }
        return oss.str();
    }
};

} // namespace nlm