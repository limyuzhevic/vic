// NLM (熙然) - Neural Learning Machine
// Validation Module
//
// This module provides comprehensive validation functions for
// configuration and inputs, ensuring data integrity and preventing
// common errors in NLM applications.

#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <algorithm>
#include <cmath>
#include <limits>

namespace nlm {

// Validation result structure
struct ValidationResult {
    bool valid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ValidationResult() : valid(true) {}
    ValidationResult(bool v) : valid(v) {}
    
    void addError(const std::string& error) {
        errors.push_back(error);
        valid = false;
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    bool hasErrors() const { return !errors.empty(); }
    bool hasWarnings() const { return !warnings.empty(); }
    
    std::string getSummary() const {
        std::string summary;
        if (!valid) {
            summary += "VALIDATION FAILED: ";
            summary += std::to_string(errors.size()) + " errors, ";
            summary += std::to_string(warnings.size()) + " warnings";
        } else {
            summary += "VALIDATED";
            if (!warnings.empty()) {
                summary += " with " + std::to_string(warnings.size()) + " warnings";
            }
        }
        return summary;
    }
};

// Configuration validation class
class ConfigValidator {
public:
    ConfigValidator();
    ~ConfigValidator() = default();
    
    // Validate configuration file
    ValidationResult validateConfigFile(const std::string& filepath) const;
    
    // Validate configuration values
    ValidationResult validateConfig(const std::string& key, const std::string& value) const;
    
    // Validate brain configuration
    ValidationResult validateBrainConfig(const std::map<std::string, std::string>& config) const;
    
    // Validate simulation parameters
    ValidationResult validateSimulationParams(double timestep, size_t neuronCount, 
                                             size_t regionCount, float connectionProb) const;
    
    // Validate STDP parameters
    ValidationResult validateStdpParams(float ltpWeight, float ltdWeight, float tau) const;
    
    // Validate plasticity parameters
    ValidationResult validatePlasticityParams(float synaptogenesisRate, float pruningRate) const;
    
    // Validate environment parameters
    ValidationResult validateEnvironmentParams(int width, int height, 
                                             int visionWidth, int visionHeight) const;
    
    // Validate all configuration
    ValidationResult validateFullConfig(const std::map<std::string, std::string>& config) const;
    
    // Set verbose mode
    void setVerbose(bool verbose) { this->verbose = verbose; }
    bool isVerbose() const { return verbose; }
    
private:
    bool verbose;
    
    // Helper validation methods
    ValidationResult validatePositive(const std::string& key, double value, 
                                     double min = 0.0, const std::string& description = "") const;
    
    ValidationResult validateRange(const std::string& key, double value,
                                   double min, double max, 
                                   const std::string& description = "") const;
    
    ValidationResult validateEnum(const std::string& key, const std::string& value,
                                  const std::set<std::string>& validValues,
                                  const std::string& description = "") const;
    
    ValidationResult validateString(const std::string& key, const std::string& value,
                                    size_t minLength = 0, size_t maxLength = std::numeric_limits<size_t>::max(),
                                    const std::string& pattern = "", const std::string& description = "") const;
    
    ValidationResult validateFile(const std::string& key, const std::string& filepath,
                                  bool required = false, const std::string& description = "") const;
    
    // Configuration-specific validators
    ValidationResult validateRandomSeed(const std::string& key, const std::string& value) const;
    ValidationResult validateSimulationTimestep(const std::string& key, const std::string& value) const;
    ValidationResult validateNeuronCount(const std::string& key, const std::string& value) const;
    ValidationResult validateRegionCount(const std::string& key, const std::string& value) const;
    ValidationResult validateConnectionProbability(const std::string& key, const std::string& value) const;
    ValidationResult validateStdpParameters(const std::map<std::string, std::string>& config) const;
    ValidationResult validatePlasticityParameters(const std::map<std::string, std::string>& config) const;
};

// Input validation class
class InputValidator {
public:
    InputValidator();
    ~InputValidator() = default();
    
    // Validate neural activity values
    ValidationResult validateNeuralActivity(double activity, const std::string& context = "") const;
    
    // Validate synaptic weights
    ValidationResult validateSynapticWeight(float weight, const std::string& context = "") const;
    
    // Validate membrane potentials
    ValidationResult validateMembranePotential(double voltage, const std::string& neuronType = "") const;
    
    // Validate current injection
    ValidationResult validateCurrentInjection(float current, const std::string& neuronType = "") const;
    
    // Validate spike times
    ValidationResult validateSpikeTime(double spikeTime, double previousSpikeTime,
                                      double minInterval = 0.001) const;
    
    // Validate neuron counts
    ValidationResult validateNeuronCount(size_t count, const std::string& context = "") const;
    
    // Validate synapse counts
    ValidationResult validateSynapseCount(size_t count, const std::string& context = "") const;
    
    // Validate firing rates
    ValidationResult validateFiringRate(double rate, const std::string& neuronType = "") const;
    
    // Validate energy levels
    ValidationResult validateEnergyLevel(double energy, const std::string& context = "") const;
    
    // Validate reward values
    ValidationResult validateReward(float reward, const std::string& context = "") const;
    
    // Validate action commands
    ValidationResult validateActionCommand(int action, const std::string& neuronType = "") const;
    
    // Validate all input parameters
    ValidationResult validateAllInputs(const std::map<std::string, double>& inputs) const;
    
    // Set verbose mode
    void setVerbose(bool verbose) { this->verbose = verbose; }
    bool isVerbose() const { return verbose; }
    
private:
    bool verbose;
    
    // Helper validation methods
    ValidationResult validatePositive(const std::string& key, double value,
                                     double min = 0.0, const std::string& description = "") const;
    
    ValidationResult validateRange(const std::string& key, double value,
                                   double min, double max,
                                   const std::string& description = "") const;
};

// Validation utilities
class ValidationUtils {
public:
    static std::string trim(const std::string& str);
    static bool isNumber(const std::string& str);
    static bool isInteger(const std::string& str);
    static bool isFloat(const std::string& str);
    static bool isPositive(const std::string& str);
    static bool isNonNegative(const std::string& str);
    static bool isValidFile(const std::string& filepath);
    static std::string toLower(const std::string& str);
    static bool stringContains(const std::string& str, const std::string& substr);
    static std::vector<std::string> split(const std::string& str, char delimiter);
    static std::string join(const std::vector<std::string>& parts, const std::string& delimiter);
    static double stringToDouble(const std::string& str, double defaultValue = 0.0);
    static int stringToInt(const std::string& str, int defaultValue = 0);
    static bool stringToBool(const std::string& str, bool defaultValue = false);
};

} // namespace nlm