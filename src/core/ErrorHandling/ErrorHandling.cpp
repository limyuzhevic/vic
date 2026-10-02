// Enhanced error handling and validation framework for NLM
// Provides comprehensive error reporting, validation, and recovery mechanisms

#include "ErrorHandling.hpp"
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <cstring>
#include <limits>

namespace nlm {

ValidationResult ValidationUtils::validateConfig(const std::unordered_map<std::string, std::string>& values) {
    ValidationResult result;
    
    // Required keys validation
    const std::vector<std::string> requiredKeys = {
        "random_seed", "neuron_count", "region_count", "connection_probability"
    };
    
    for (const auto& key : requiredKeys) {
        if (values.find(key) == values.end()) {
            result.addError("Missing required configuration key: " + key);
        }
    }
    
    // Value validation
    for (const auto& pair : values) {
        const std::string& key = pair.first;
        const std::string& value = pair.second;
        
        if (key == "random_seed") {
            auto seed = toInt(value, 0, std::numeric_limits<int>::max());
            if (!seed) {
                result.addError("Invalid random_seed value: " + value);
            }
        } else if (key == "neuron_count") {
            auto count = toInt(value, 1, 1000000);
            if (!count) {
                result.addError("Invalid neuron_count value: " + value);
            } else if (*count < 10) {
                result.addWarning("neuron_count is very small (" + value + "), may affect simulation quality");
            }
        } else if (key == "region_count") {
            auto count = toInt(value, 1, 1000);
            if (!count) {
                result.addError("Invalid region_count value: " + value);
            }
        } else if (key == "connection_probability") {
            auto prob = toDouble(value, 0.0, 1.0);
            if (!prob) {
                result.addError("Invalid connection_probability value: " + value);
            } else if (*prob < 0.01) {
                result.addWarning("connection_probability is very low (" + value + "), may result in disconnected network");
            } else if (*prob > 0.5) {
                result.addWarning("connection_probability is high (" + value + "), may result in overly dense network");
            }
        } else if (key == "simulation_timestep") {
            auto timestep = toDouble(value, 1e-6, 1.0);
            if (!timestep) {
                result.addError("Invalid simulation_timestep value: " + value);
            }
        } else if (key == "stdp_ltp_weight") {
            auto weight = toDouble(value, 0.0, 10.0);
            if (!weight) {
                result.addError("Invalid stdp_ltp_weight value: " + value);
            }
        } else if (key == "stdp_ltd_weight") {
            auto weight = toDouble(value, 0.0, 10.0);
            if (!weight) {
                result.addError("Invalid stdp_ltd_weight value: " + value);
            }
        } else if (key == "stdp_time_constant") {
            auto tau = toDouble(value, 1.0, 1000.0);
            if (!tau) {
                result.addError("Invalid stdp_time_constant value: " + value);
            }
        } else if (key == "synaptogenesis_rate") {
            auto rate = toDouble(value, 0.0, 1.0);
            if (!rate) {
                result.addError("Invalid synaptogenesis_rate value: " + value);
            }
        } else if (key == "pruning_rate") {
            auto rate = toDouble(value, 0.0, 1.0);
            if (!rate) {
                result.addError("Invalid pruning_rate value: " + value);
            }
        } else if (key == "log_level") {
            if (!isValidLogLevel(value)) {
                result.addError("Invalid log_level value: " + value + ". Valid values: DEBUG, INFO, WARNING, ERROR, CRITICAL");
            }
        }
    }
    
    return result;
}

ValidationResult ValidationUtils::validateNeuronParameters(float vrest, float vreset, float threshold,
                                                         float tau, float cm, float refracPeriod) {
    ValidationResult result;
    
    // Biologically plausible ranges
    if (!isInRange(vrest, -100.0f, 0.0f)) {
        result.addError("Vrest out of range [-100, 0]: " + std::to_string(vrest));
    }
    
    if (!isInRange(vreset, -80.0f, -50.0f)) {
        result.addWarning("Vreset unusual: " + std::to_string(vreset) + ". Typical range: [-80, -50]");
    }
    
    if (!isInRange(threshold, vreset - 20.0f, vrest + 20.0f)) {
        result.addError("Threshold out of plausible range: " + std::to_string(threshold));
    }
    
    if (!isInRange(tau, 1.0f, 100.0f)) {
        result.addError("Time constant tau out of range [1, 100]: " + std::to_string(tau));
    }
    
    if (!isInRange(cm, 0.1f, 10.0f)) {
        result.addError("Membrane capacitance CM out of range [0.1, 10]: " + std::to_string(cm));
    }
    
    if (!isInRange(refracPeriod, 1, 100)) {
        result.addError("Refractory period out of range [1, 100]: " + std::to_string(refracPeriod));
    }
    
    return result;
}

ValidationResult ValidationUtils::validateSynapseParameters(float weight, float delay, bool isExcitatory) {
    ValidationResult result;
    
    if (!isInRange(weight, -1.0f, 10.0f)) {
        result.addError("Synaptic weight out of range [-1, 10]: " + std::to_string(weight));
    }
    
    if (!isInRange(delay, 0.0f, 100.0f)) {
        result.addError("Synaptic delay out of range [0, 100]: " + std::to_string(delay));
    }
    
    return result;
}

ValidationResult ValidationUtils::validateNetworkTopology(size_t neuronCount, size_t regionCount,
                                                         float connectionProbability) {
    ValidationResult result;
    
    if (neuronCount < 10) {
        result.addError("Neuron count too small: " + std::to_string(neuronCount) + ". Minimum: 10");
    }
    
    if (neuronCount > 1000000) {
        result.addError("Neuron count too large: " + std::to_string(neuronCount) + ". Maximum: 1,000,000");
    }
    
    if (regionCount < 1) {
        result.addError("Region count must be at least 1: " + std::to_string(regionCount));
    }
    
    if (regionCount > 1000) {
        result.addError("Region count too large: " + std::to_string(regionCount) + ". Maximum: 1,000");
    }
    
    if (connectionProbability < 0.0f || connectionProbability > 1.0f) {
        result.addError("Connection probability out of range [0, 1]: " + std::to_string(connectionProbability));
    }
    
    // Estimate expected synapses
    float expectedSynapses = static_cast<float>(neuronCount) * static_cast<float>(neuronCount) * connectionProbability / 2.0f;
    if (expectedSynapses > 1000000.0f) {
        result.addWarning("Estimated synapse count very high: ~" + std::to_string(static_cast<size_t>(expectedSynapses)));
    }
    
    return result;
}

ValidationResult ValidationUtils::validatePlasticityParameters(float learningRate, float stdpLTP, float stdpLTD) {
    ValidationResult result;
    
    if (!isInRange(learningRate, 0.0f, 1.0f)) {
        result.addError("Learning rate out of range [0, 1]: " + std::to_string(learningRate));
    }
    
    if (stdpLTP < 0.0f || stdpLTD < 0.0f) {
        result.addError("STDP weights must be non-negative: LTP=" + std::to_string(stdpLTP) + ", LTD=" + std::to_string(stdpLTD));
    }
    
    if (stdpLTP < stdpLTD) {
        result.addWarning("STDP LTP weight smaller than LTD weight: LTP=" + std::to_string(stdpLTP) + ", LTD=" + std::to_string(stdpLTD));
    }
    
    return result;
}

ValidationResult ValidationUtils::validateMemoryParameters(size_t capacity, size_t maxEpisodes) {
    ValidationResult result;
    
    if (capacity == 0) {
        result.addError("Memory capacity must be greater than 0: " + std::to_string(capacity));
    }
    
    if (maxEpisodes == 0) {
        result.addError("Max episodes must be greater than 0: " + std::to_string(maxEpisodes));
    }
    
    if (capacity > 1000000) {
        result.addWarning("Memory capacity very large: " + std::to_string(capacity));
    }
    
    if (maxEpisodes > 100000) {
        result.addWarning("Max episodes very large: " + std::to_string(maxEpisodes));
    }
    
    return result;
}

ValidationResult ValidationUtils::validateSimulationParameters(double timestep, size_t maxSteps) {
    ValidationResult result;
    
    if (timestep <= 0.0) {
        result.addError("Simulation timestep must be positive: " + std::to_string(timestep));
    }
    
    if (timestep > 0.1) {
        result.addWarning("Simulation timestep large: " + std::to_string(timestep) + ". Consider smaller for accuracy");
    }
    
    if (maxSteps == 0) {
        result.addError("Max simulation steps must be greater than 0: " + std::to_string(maxSteps));
    }
    
    if (maxSteps > 1000000000) {
        result.addWarning("Max simulation steps very large: " + std::to_string(maxSteps));
    }
    
    return result;
}

std::optional<int> ValidationUtils::toInt(const std::string& str, int min, int max) {
    if (str.empty()) return std::nullopt;
    
    try {
        size_t pos;
        int val = std::stoi(str, &pos);
        if (pos != str.size()) return std::nullopt;
        if (val < min || val > max) return std::nullopt;
        return val;
    } catch (...) {
        return std::nullopt;
    }
}

std::optional<double> ValidationUtils::toDouble(const std::string& str, double min, double max) {
    if (str.empty()) return std::nullopt;
    
    try {
        size_t pos;
        double val = std::stod(str, &pos);
        if (pos != str.size()) return std::nullopt;
        if (val < min || val > max) return std::nullopt;
        return val;
    } catch (...) {
        return std::nullopt;
    }
}

std::optional<float> ValidationUtils::toFloat(const std::string& str, float min, float max) {
    if (str.empty()) return std::nullopt;
    
    try {
        size_t pos;
        float val = std::stof(str, &pos);
        if (pos != str.size()) return std::nullopt;
        if (val < min || val > max) return std::nullopt;
        return val;
    } catch (...) {
        return std::nullopt;
    }
}

std::optional<bool> ValidationUtils::toBool(const std::string& str) {
    std::string lower = toLower(str);
    if (lower == "true" || lower == "1" || lower == "yes" || lower == "on") {
        return true;
    } else if (lower == "false" || lower == "0" || lower == "no" || lower == "off") {
        return false;
    }
    return std::nullopt;
}

bool ValidationUtils::isValidKey(const std::string& key) {
    if (key.empty()) return false;
    
    // Check each character
    for (char c : key) {
        if (!isValidKeyChar(c)) {
            return false;
        }
    }
    
    // Check for reserved names
    static const std::vector<std::string> reserved = {
        "help", "version", "verbose", "debug", "quiet", "config", "log-level"
    };
    
    if (std::find(reserved.begin(), reserved.end(), key) != reserved.end()) {
        return false;
    }
    
    return true;
}

bool ValidationUtils::isValidFilePath(const std::string& path) {
    if (path.empty()) return false;
    
    // Basic path validation
    for (char c : path) {
        if (c == '\0' || c == '/' || c == '\\') {
            // Allow forward and backward slashes, but check for invalid patterns
            if (c == '/' && path.back() == '/') {
                return false;
            }
            if (c == '\\' && path.back() == '\\') {
                return false;
            }
        }
    }
    
    return true;
}

bool ValidationUtils::isValidLogLevel(const std::string& level) {
    std::string lower = toLower(level);
    return lower == "debug" || lower == "info" || lower == "warning" || 
           lower == "error" || lower == "critical";
}

bool ValidationUtils::isNotEmpty(const std::string& str) {
    return !str.empty() && !std::all_of(str.begin(), str.end(), isWhitespace);
}

bool ValidationUtils::isNotEmpty(const std::vector<std::string>& vec) {
    return !vec.empty() && !std::all_of(vec.begin(), vec.end(), 
                                        [](const std::string& s) { return s.empty() || isWhitespace(s[0]); });
}

// Explicit template instantiations for SafeResourceManager
template std::shared_ptr<void> SafeResourceManager::createSafe<void>(void);

// Explicit template instantiations for RetryManager

template auto RetryManager::executeWithRetry<void()>(void() &&, const RetryPolicy&) -> decltype(std::declval<void>());

// Explicit template instantiations for ErrorHandlingWrapper

template auto ErrorHandlingWrapper::executeWithErrorHandling<void()>(void() &&, const std::string&, 
                                                                   ErrorCategory,
                                                                   const std::vector<std::shared_ptr<RecoveryAction>>&) -> decltype(std::declval<void>());

} // namespace nlm
