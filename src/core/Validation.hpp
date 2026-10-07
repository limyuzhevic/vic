#pragma once

#include <stdexcept>
#include <string>
#include <vector>

namespace nlm {

class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& message) : std::runtime_error(message) {}
};

class ConfigError : public NLMError {
public:
    explicit ConfigError(const std::string& message) : NLMError("Config error: " + message) {}
};

class VisionError : public NLMError {
public:
    explicit VisionError(const std::string& message) : NLMError("Vision error: " + message) {}
};

class ValidationResult {
public:
    bool isValid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ValidationResult() : isValid(true) {}
    
    bool hasErrors() const { return !errors.empty(); }
    bool hasWarnings() const { return !warnings.empty(); }
    
    std::string getErrorSummary() const {
        if (errors.empty()) return "Validation passed";
        std::string summary = "Validation failed with " + std::to_string(errors.size()) + " error";
        if (errors.size() != 1) summary += "s";
        summary += ": ";
        for (size_t i = 0; i < errors.size() && i < 3; ++i) {
            if (i > 0) summary += ", ";
            summary += errors[i];
        }
        if (errors.size() > 3) summary += " and " + std::to_string(errors.size() - 3) + " more";
        return summary;
    }
};

class ParameterValidator {
public:
    enum class ParameterType { INT, FLOAT, STRING, BOOL, VECTOR, DICT };
    
    static ParameterType inferType(const std::string& value) {
        if (value.empty()) return ParameterType::STRING;
        
        // Check for bool
        if (value == "true" || value == "false") return ParameterType::BOOL;
        
        // Check for int
        bool isInt = true;
        size_t pos = 0;
        if (value[0] == '-') pos = 1;
        for (; pos < value.size(); ++pos) {
            if (!std::isdigit(value[pos])) {
                isInt = false;
                break;
            }
        }
        if (isInt) return ParameterType::INT;
        
        // Check for float
        bool isFloat = true;
        size_t dotCount = 0;
        pos = 0;
        if (value[0] == '-') pos = 1;
        for (; pos < value.size(); ++pos) {
            if (std::isdigit(value[pos])) continue;
            if (value[pos] == '.' && dotCount++ == 0) continue;
            isFloat = false;
            break;
        }
        if (isFloat && dotCount > 0) return ParameterType::FLOAT;
        
        // Check for vector format [1,2,3] (simplified)
        if (value.size() >= 2 && value[0] == '[' && value[value.size() - 1] == ']') {
            return ParameterType::VECTOR;
        }
        
        return ParameterType::STRING;
    }
    
    static ValidationResult validateParameter(const std::string& value, 
                                              ParameterType expected_type,
                                              const py::dict& constraints = py::dict()) {
        ValidationResult result;
        
        // Check type
        ParameterType actual_type = inferType(value);
        if (actual_type != expected_type) {
            std::string type_name;
            switch (expected_type) {
                case ParameterType::INT: type_name = "int"; break;
                case ParameterType::FLOAT: type_name = "float"; break;
                case ParameterType::STRING: type_name = "string"; break;
                case ParameterType::BOOL: type_name = "bool"; break;
                case ParameterType::VECTOR: type_name = "vector"; break;
                case ParameterType::DICT: type_name = "dict"; break;
            }
            result.errors.push_back("Expected type '" + type_name + "' for value '" + value + "'");
        }
        
        // Validate constraints
        if (!constraints.empty()) {
            if (constraints.contains("min") && actual_type == ParameterType::INT) {
                int min_val = constraints["min"].cast<int>();
                int val = std::stoi(value);
                if (val < min_val) {
                    result.errors.push_back("Value " + value + " is less than minimum " + std::to_string(min_val));
                }
            }
            if (constraints.contains("max") && actual_type == ParameterType::INT) {
                int max_val = constraints["max"].cast<int>();
                int val = std::stoi(value);
                if (val > max_val) {
                    result.errors.push_back("Value " + value + " is greater than maximum " + std::to_string(max_val));
                }
            }
            if (constraints.contains("min") && actual_type == ParameterType::FLOAT) {
                float min_val = constraints["min"].cast<float>();
                float val = std::stof(value);
                if (val < min_val) {
                    result.errors.push_back("Value " + value + " is less than minimum " + std::to_string(min_val));
                }
            }
            if (constraints.contains("max") && actual_type == ParameterType::FLOAT) {
                float max_val = constraints["max"].cast<float>();
                float val = std::stof(value);
                if (val > max_val) {
                    result.errors.push_back("Value " + value + " is greater than maximum " + std::to_string(max_val));
                }
            }
        }
        
        return result;
    }
};

class MemoryMonitor {
public:
    struct MemoryInfo {
        size_t totalBytes;
        size_t usedBytes;
        size_t availableBytes;
        size_t allocatedObjects;
        double memoryUsagePercent;
    };
    
    static MemoryInfo getCurrentUsage() {
        MemoryInfo info;
        
        #ifdef __linux__
        // Try to read from /proc/self/status
        std::ifstream status_file("/proc/self/status");
        if (status_file.is_open()) {
            std::string line;
            while (std::getline(status_file, line)) {
                if (line.find("VmRSS:") == 0) {
                    // Extract resident set size
                    size_t pos = line.find(':');
                    if (pos != std::string::npos) {
                        std::string value_str = line.substr(pos + 1);
                        size_t value = std::stoull(value_str) * 1024; // Convert KB to bytes
                        info.usedBytes = value;
                    }
                } else if (line.find("VmSize:") == 0) {
                    // Extract virtual memory size
                    size_t pos = line.find(':');
                    if (pos != std::string::npos) {
                        std::string value_str = line.substr(pos + 1);
                        size_t value = std::stoull(value_str) * 1024;
                        info.totalBytes = value;
                    }
                } else if (line.find("VmStk") == 0 && line.find("VmHWM") == std::string::npos) {
                    // Peak stack size (for thread count)
                    size_t pos = line.find(':');
                    if (pos != std::string::npos) {
                        size_t value = std::stoull(line.substr(pos + 1)) * 1024;
                        info.allocatedObjects = std::max(info.allocatedObjects, value / (4 * 1024)); // Estimate thread count
                    }
                }
            }
        }
        #endif
        
        if (info.totalBytes == 0) info.totalBytes = 1024 * 1024 * 1024; // Default 1GB estimate
        info.availableBytes = info.totalBytes - info.usedBytes;
        if (info.totalBytes > 0) {
            info.memoryUsagePercent = (info.usedBytes * 100.0) / info.totalBytes;
        }
        
        return info;
    }
};

class MemoryHealthChecker {
public:
    struct HealthReport {
        bool isHealthy;
        size_t memoryUsage;
        size_t recommendedCapacity;
        std::vector<std::string> warningMessages;
        std::vector<std::string> errorMessages;
    };
    
    static HealthReport checkBrainHealth(const std::shared_ptr<Brain>& brain) {
        HealthReport report;
        report.isHealthy = true;
        
        // Check memory usage
        auto memoryInfo = MemoryMonitor::getCurrentUsage();
        report.memoryUsage = memoryInfo.usedBytes;
        report.recommendedCapacity = memoryInfo.totalBytes * 0.8; // Warn at 80% usage
        
        if (memoryInfo.memoryUsagePercent > 80) {
            report.isHealthy = false;
            report.errorMessages.push_back("Memory usage at " + std::to_string(static_cast<int>(memoryInfo.memoryUsagePercent)) + "%");
        }
        
        if (brain) {
            // Check brain-specific memory
            size_t totalNeurons = brain->getTotalNeuronCount();
            size_t totalSynapses = brain->getTotalSynapseCount();
            
            // Estimate memory per neuron (rough approximation)
            size_t neuronMemory = totalNeurons * sizeof(Neuron);
            size_t synapseMemory = totalSynapses * sizeof(Synapse);
            
            report.memoryUsage += neuronMemory + synapseMemory;
            
            if (totalNeurons > 10000) {
                report.warningMessages.push_back("Large brain: " + std::to_string(totalNeurons) + " neurons");
            }
            
            if (totalSynapses > totalNeurons * 10) {
                report.warningMessages.push_back("Very high connectivity: " + std::to_string(totalSynapses) + " synapses");
            }
        }
        
        return report;
    }
};

class MemoryManager {
public:
    static void cleanupUnusedNeurons(const std::shared_ptr<Brain>& brain, double unusedThreshold = 0.8) {
        if (!brain) return;
        
        auto* spikeSystem = brain->getSpikeSystem();
        if (!spikeSystem) return;
        
        auto* stdp = brain->getSTDP();
        if (!stdp) return;
        
        for (const auto& region : brain->getRegions()) {
            for (auto& syn : region->getSynapses()) {
                float eligibility = syn->getEligibilityTrace();
                if (std::abs(eligibility) < unusedThreshold) {
                    // Reduce or zero out weight for unused connections
                    float currentWeight = syn->getWeight();
                    float reductionFactor = (1.0f - unusedThreshold);
                    syn->addToWeight(-currentWeight * reductionFactor);
                }
            }
        }
    }
    
    static void optimizeConnectivity(const std::shared_ptr<Brain>& brain, 
                                    float maxConnectivity = 0.3f) {
        if (!brain) return;
        
        auto* spikeSystem = brain->getSpikeSystem();
        if (!spikeSystem) return;
        
        // Analyze current connectivity
        size_t totalNeurons = brain->getTotalNeuronCount();
        size_t totalConnections = 0;
        
        for (const auto& region : brain->getRegions()) {
            for (const auto& syn : region->getSynapses()) {
                if (syn->getWeight() > 0.0f) {
                    totalConnections++;
                }
            }
        }
        
        float currentConnectivity = 0.0f;
        if (totalNeurons > 0) {
            currentConnectivity = static_cast<float>(totalConnections) / totalNeurons;
        }
        
        if (currentConnectivity > maxConnectivity) {
            // Remove excess connections by zeroing weights
            float targetConnections = static_cast<float>(totalNeurons) * maxConnectivity;
            float connectionsToRemove = static_cast<float>(totalConnections) - targetConnections;
            
            if (connectionsToRemove > 0.0f) {
                auto* stdp = brain->getSTDP();
                if (stdp) {
                    float removalRate = connectionsToRemove / static_cast<float>(totalConnections);
                    
                    for (const auto& region : brain->getRegions()) {
                        for (auto& syn : region->getSynapses()) {
                            if (syn->getWeight() > 0.0f && removalRate > 0.0f) {
                                syn->addToWeight(-syn->getWeight() * removalRate);
                            }
                        }
                    }
                }
            }
        }
    }
};

} // namespace nlm
