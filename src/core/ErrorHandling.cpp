// Error handling and validation implementation for NLM
//
// This file implements the error handling and validation utilities
// defined in ErrorHandling.hpp.

#include "core/ErrorHandling.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include <cstring>
#include <stdexcept>
#include <system_error>
#include <limits>

namespace nlm {

// Initialize static validators
// (no implementation needed for static member initializers)

// ConfigValidator implementation
bool ConfigValidator::validateNeuronCount(int64_t neuronCount) {
    if (neuronCount <= 0) {
        throw ValidationError("Invalid neuron count", "neuron_count",
                            "positive integer", std::to_string(neuronCount));
    }
    if (neuronCount > 1000000) {
        throw ValidationError("Neuron count too large", "neuron_count",
                            "less than or equal to 1000000", std::to_string(neuronCount));
    }
    return true;
}

bool ConfigValidator::validateConnectionProbability(float probability) {
    if (probability < 0.0f || probability > 1.0f) {
        throw ValidationError("Invalid connection probability", "connection_probability",
                            "between 0.0 and 1.0", std::to_string(probability));
    }
    return true;
}

bool ConfigValidator::validateTimeStep(double timestep) {
    if (timestep <= 0.0) {
        throw ValidationError("Invalid time step", "simulation_timestep",
                            "positive value", std::to_string(timestep));
    }
    if (timestep > 0.1) {
        throw ValidationError("Time step too large", "simulation_timestep",
                            "less than or equal to 0.1", std::to_string(timestep));
    }
    return true;
}

bool ConfigValidator::validatePlasticityParameters(float ltp, float ltd, float tau) {
    if (ltp < 0.0f || ltd < 0.0f) {
        throw ValidationError("Invalid plasticity parameters", "stdp weights",
                            "positive values", "LTP: " + std::to_string(ltp) + ", LTD: " + std::to_string(ltd));
    }
    if (tau <= 0.0f) {
        throw ValidationError("Invalid STDP tau", "stdp_tau", "positive value", std::to_string(tau));
    }
    return true;
}

void ConfigValidator::validateConfigValue(const std::string& key, const ConfigValue& value) {
    // Type-specific validation
    std::visit([](const auto& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
            if (val != val) throw ValidationError("NaN found in configuration", key);
            if (std::isinf(val)) throw ValidationError("Infinity found in configuration", key);
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            for (size_t i = 0; i < val.size(); ++i) {
                if (val[i] != val[i]) throw ValidationError("NaN found in configuration array", key + ".[" + std::to_string(i) + "]");
                if (std::isinf(val[i])) throw ValidationError("Infinity found in configuration array", key + ".[" + std::to_string(i) + "]");
            }
        }
    }, value);
}

// ResourceManager implementation
ResourceManager::ResourceLimits ResourceManager::getSystemResourceLimits() {
    ResourceLimits limits;
    // Try to get actual system limits
    // For now, return reasonable defaults
    limits.maxNeurons = 1000000;
    limits.maxSynapses = 10000000;
    limits.maxMemoryMB = 2048;
    limits.maxFileHandles = 100;
    limits.maxThreads = 8;
    
    // Try to get memory limits from system
    #ifdef _WIN32
    // Windows implementation
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    limits.maxMemoryMB = std::min(limits.maxMemoryMB, sysInfo.dwTotalVirtual / (1024 * 1024));
    #elif defined(__linux__)
    // Linux implementation - read from /proc/meminfo
    std::ifstream meminfo("/proc/meminfo");
    if (meminfo.is_open()) {
        std::string line;
        while (std::getline(meminfo, line)) {
            if (line.find("MemTotal:") == 0) {
                size_t memKB = std::stoul(line.substr(9));
                limits.maxMemoryMB = std::min(limits.maxMemoryMB, memKB / 1024);
                break;
            }
        }
    }
    #endif
    
    return limits;
}

void ResourceManager::allocateBrainResources(size_t neuronCount, size_t synapseCount) {
    ResourceLimits limits = getSystemResourceLimits();
    
    if (neuronCount > limits.maxNeurons) {
        throw NLMException("Not enough memory for neurons", NLMErrorCategory::ResourceAllocation,
                          "Requested: " + std::to_string(neuronCount) + ", Available: " + std::to_string(limits.maxNeurons));
    }
    
    // Estimate memory requirements (rough approximation)
    size_t estimatedMemoryMB = (neuronCount * 100 + synapseCount * 200) / (1024 * 1024);
    if (estimatedMemoryMB > limits.maxMemoryMB) {
        throw NLMException("Not enough memory", NLMErrorCategory::ResourceAllocation,
                          "Estimated: " + std::to_string(estimatedMemoryMB) + "MB, Available: " + std::to_string(limits.maxMemoryMB) + "MB");
    }
}

// SimulationValidator implementation
void SimulationValidator::validateSimulationStep(SimulationStep step, Timestamp time) {
    if (step < 0) {
        throw ValidationError("Invalid simulation step", "step", "non-negative integer", std::to_string(step));
    }
    if (time < 0.0) {
        throw ValidationError("Invalid simulation time", "time", "non-negative value", std::to_string(time));
    }
}

void SimulationValidator::validateBrainState(const Brain& brain) {
    size_t neuronCount = brain.getTotalNeuronCount();
    size_t synapseCount = brain.getTotalSynapseCount();
    
    if (neuronCount == 0 && synapseCount > 0) {
        throw ValidationError("Brain in inconsistent state", "neuron_count vs synapse_count",
                            "neuron_count should be 0 when synapse_count > 0");
    }
    
    float firingRate = brain.getAverageFiringRate();
    if (firingRate < 0.0f || firingRate > 10000.0f) {
        throw ValidationError("Invalid firing rate", "average_firing_rate",
                            "between 0.0 and 10000.0 Hz", std::to_string(firingRate));
    }
}

// ErrorRecovery implementation
void ErrorRecovery::safeBrainCleanup(std::shared_ptr<Brain> brain) {
    if (!brain) return;
    
    try {
        brain->reset();
    } catch (const std::exception& e) {
        // Log cleanup failure but don't throw
        // The brain's destructor should handle memory cleanup
    }
}

bool ErrorRecovery::safeFileWrite(const std::string& filepath, const std::string& content) {
    std::error_code ec;
    std::filesystem::path path(filepath);
    
    try {
        // Create parent directories if they don't exist
        if (!path.parent_path().empty()) {
            std::filesystem::create_directories(path.parent_path(), ec);
            if (ec) {
                return false;
            }
        }
        
        // Write file atomically
        std::string tempPath = filepath + ".tmp";
        std::ofstream file(tempPath, std::ios::binary);
        if (!file) {
            return false;
        }
        
        file.write(content.c_str(), content.size());
        if (!file) {
            std::filesystem::remove(tempPath, ec);
            return false;
        }
        
        file.close();
        
        // Atomic replace
        std::error_code replaceEc;
        std::filesystem::rename(tempPath, filepath, replaceEc);
        if (replaceEc) {
            std::filesystem::remove(tempPath, replaceEc);
            return false;
        }
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

std::string ErrorRecovery::safeFileRead(const std::string& filepath) {
    std::error_code ec;
    std::filesystem::path path(filepath);
    
    try {
        if (!std::filesystem::exists(path, ec)) {
            throw NLMException("File not found", NLMErrorCategory::FileIO, filepath);
        }
        
        std::ifstream file(filepath, std::ios::binary);
        if (!file) {
            throw NLMException("Cannot open file for reading", NLMErrorCategory::FileIO, filepath);
        }
        
        file.seekg(0, std::ios::end);
        size_t fileSize = file.tellg();
        file.seekg(0, std::ios::beg);
        
        if (fileSize > 1024 * 1024 * 1024) {  // 1GB limit
            throw NLMException("File too large", NLMErrorCategory::FileIO, filepath);
        }
        
        std::string content(fileSize, '\0');
        if (!file.read(&content[0], fileSize)) {
            throw NLMException("Failed to read file", NLMErrorCategory::FileIO, filepath);
        }
        
        return content;
    } catch (const std::exception& e) {
        if (std::is_same<decltype(e), NLMException>::value) {
            throw;  // Re-throw NLM exceptions
        }
        throw NLMException("File read error", NLMErrorCategory::FileIO, filepath + ": " + e.what());
    }
}

// ErrorReporter implementation
void ErrorReporter::logError(const NLMException& error, const std::string& context) {
    std::cerr << "[NLM Error]";
    if (!context.empty()) {
        std::cerr << " Context: " << context;
    }
    std::cerr << " " << error.toString() << std::endl;
}

void ErrorReporter::logValidationError(const ValidationError& error, const std::string& context) {
    std::cerr << "[NLM Validation Error]";
    if (!context.empty()) {
        std::cerr << " Context: " << context;
    }
    std::cerr << " Field: " << error.getField();
    if (!error.getExpected().empty()) {
        std::cerr << " Expected: " << error.getExpected();
    }
    if (!error.getActual().empty()) {
        std::cerr << " Actual: " << error.getActual();
    }
    std::cerr << ". Message: " << error.what() << std::endl;
}

bool ErrorReporter::isRecoverable(const NLMException& error) {
    switch (error.getCategory()) {
        case NLMErrorCategory::InvalidArgument:
        case NLMErrorCategory::NotImplemented:
            return false;
        case NLMErrorCategory::ResourceAllocation:
            return true;
        case NLMErrorCategory::FileIO:
            return true;
        case NLMErrorCategory::Internal:
            return false;
        default:
            return true;
    }
}

// Configuration validation integration
void validateConfiguration(const Config& config) {
    try {
        // Validate neuron count
        if (config.has("neuron_count")) {
            int64_t neuronCount = config.get<int64_t>("neuron_count").value_or(0);
            ConfigValidator::validateNeuronCount(neuronCount);
        }
        
        // Validate connection probability
        if (config.has("connection_probability")) {
            float probability = config.get<float>("connection_probability").value_or(0.0f);
            ConfigValidator::validateConnectionProbability(probability);
        }
        
        // Validate time step
        if (config.has("simulation_timestep")) {
            double timestep = config.get<double>("simulation_timestep").value_or(0.0);
            ConfigValidator::validateTimeStep(timestep);
        }
        
        // Validate plasticity parameters
        if (config.has("stdp_ltp_weight") && config.has("stdp_ltd_weight") && config.has("stdp_tau")) {
            float ltp = config.get<float>("stdp_ltp_weight").value_or(0.0f);
            float ltd = config.get<float>("stdp_ltd_weight").value_or(0.0f);
            float tau = config.get<float>("stdp_tau").value_or(0.0f);
            ConfigValidator::validatePlasticityParameters(ltp, ltd, tau);
        }
        
        // Validate all configuration values for basic data types
        for (const auto& key : config.getKeys()) {
            ConfigValidator::validateConfigValue(key, config.get<ConfigValue>(key).value_or(ConfigValue()));
        }
        
    } catch (const ValidationError& e) {
        throw NLMException("Configuration validation failed", NLMErrorCategory::Configuration,
                          e.toString(), 100);
    } catch (const std::exception& e) {
        throw NLMException("Configuration validation error", NLMErrorCategory::Configuration,
                          e.what(), 101);
    }
}

void validateBrainResources(const Brain& brain) {
    try {
        size_t neuronCount = brain.getTotalNeuronCount();
        size_t synapseCount = brain.getTotalSynapseCount();
        
        ResourceManager::allocateBrainResources(neuronCount, synapseCount);
        
        SimulationValidator::validateBrainState(brain);
        
    } catch (const NLMException& e) {
        throw;  // Re-throw NLM exceptions
    } catch (const std::exception& e) {
        throw NLMException("Brain resource validation failed", NLMErrorCategory::ResourceAllocation,
                          e.what(), 200);
    }
}

} // namespace nlm
