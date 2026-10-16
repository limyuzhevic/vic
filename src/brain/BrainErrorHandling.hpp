// Enhanced NLM Brain error handling and validation
// Phase 6: Integrated Artificial Brain with robust error management

#ifndef BRAIN_ERROR_HANDLING_H
#define BRAIN_ERROR_HANDLING_H

#include <string>
#include <map>
#include <vector>
#include <chrono>
#include <stacktrace>
#include <sstream>
#include <iomanip>
#include <memory>
#include "core/ErrorHandling.hpp"

namespace nlm {

// Enhanced error handling specific to Brain.cpp
namespace BrainError {
    
    // Brain initialization errors
    class BrainInitError : public NLMError {
    public:
        BrainInitError(const std::string& message, ErrorCategory category = ErrorCategory::ERROR)
            : NLMError(ErrorCode::NEURON_INVALID_STATE, message, category) {}
    };
    
    // Region errors
    class RegionError : public NLMError {
    public:
        RegionError(const std::string& message, ErrorCategory category = ErrorCategory::ERROR)
            : NLMError(ErrorCode::REGION_NOT_FOUND, message, category) {}
    };
    
    // Synapse errors
    class SynapseError : public NLMError {
    public:
        SynapseError(const std::string& message, ErrorCategory category = ErrorCategory::ERROR)
            : NLMError(ErrorCode::SYNAPSE_INVALID_WEIGHT, message, category) {}
    };
    
    // Memory system errors
    class MemoryError : public NLMError {
    public:
        MemoryError(const std::string& message, ErrorCategory category = ErrorCategory::ERROR)
            : NLMError(ErrorCode::MEMORATION_FAILED, message, category) {}
    };
    
    // Plasticity errors
    class PlasticityError : public NLMError {
    public:
        PlasticityError(const std::string& message, ErrorCategory category = ErrorCategory::ERROR)
            : NLMError(ErrorCode::PLASTICITY_OVERFLOW, message, category) {}
    };
    
    // Simulation errors
    class SimulationError : public NLMError {
    public:
        SimulationError(const std::string& message, ErrorCategory category = ErrorCategory::ERROR)
            : NLMError(ErrorCode::SIMULATION_TIMEOUT, message, category) {}
    };
};

// Error recovery utilities for Brain operations
class BrainErrorRecovery {
public:
    // Rollback brain state changes
    static bool rollbackBrainState(std::vector<std::pair<SimulationStep, std::string>>& stateChanges,
                                   SimulationStep targetStep) {
        bool success = true;
        std::string errorMessage;
        
        for (auto it = stateChanges.rbegin(); it != stateChanges.rend(); ++it) {
            SimulationStep step = it->first;
            std::string operation = it->second;
            
            if (step <= targetStep) {
                // Rollback to target step
                try {
                    if (!rollbackOperation(operation)) {
                        errorMessage = "Failed to rollback operation: " + operation;
                        success = false;
                    }
                } catch (const std::exception& e) {
                    errorMessage = std::string("Exception during rollback: ") + e.what();
                    success = false;
                }
                
                if (step == targetStep) {
                    break;
                }
            }
        }
        
        if (!success) {
            NLM_LOG_ERROR("Brain state rollback failed: " + errorMessage);
        }
        
        return success;
    }
    
    // Validate brain configuration
    static bool validateBrainConfiguration(const std::shared_ptr<Config>& config) {
        if (!config) {
            return false;
        }
        
        // Check for essential configuration values
        auto requiredKeys = {"neuron_count", "region_count", "simulation_timestep"};
        for (const auto& key : requiredKeys) {
            if (!config->has(key)) {
                NLM_LOG_ERROR(std::string("Missing required configuration: ") + key);
                return false;
            }
        }
        
        // Validate neuron count
        auto neuronCount = config->get<int64_t>("neuron_count");
        if (!neuronCount || *neuronCount < 10 || *neuronCount > 1000000) {
            NLM_LOG_ERROR("Invalid neuron count in configuration");
            return false;
        }
        
        // Validate region count
        auto regionCount = config->get<int64_t>("region_count");
        if (!regionCount || *regionCount < 1 || *regionCount > 100) {
            NLM_LOG_ERROR("Invalid region count in configuration");
            return false;
        }
        
        // Validate timestep
        auto timestep = config->get<double>("simulation_timestep");
        if (!timestep || *timestep <= 0 || *timestep > 0.1) {
            NLM_LOG_ERROR("Invalid simulation timestep in configuration");
            return false;
        }
        
        return true;
    }
    
    // Safe memory allocation for brain components
    template<typename T>
    static T* safeAllocate(size_t count, const std::string& context = "") {
        try {
            T* ptr = new T[count];
            if (!ptr) {
                throw NLMError(ErrorCode::ALLOCATION_FAILED,
                             "Failed to allocate " + std::to_string(count * sizeof(T)) + 
                             " bytes for " + context);
            }
            return ptr;
        } catch (const std::bad_alloc& e) {
            throw NLMError(ErrorCode::ALLOCATION_FAILED,
                         "Memory allocation failed for " + std::to_string(count * sizeof(T)) + 
                         " bytes: " + context + " - " + std::string(e.what()));
        }
    }
    
    // Validate neural connectivity
    static bool validateNeuralConnectivity(const std::vector<std::unique_ptr<NeuralRegion>>& regions) {
        size_t totalNeurons = 0;
        size_t totalSynapses = 0;
        
        for (const auto& region : regions) {
            if (!region) {
                NLM_LOG_ERROR("Null region in neural connectivity validation");
                return false;
            }
            
            totalNeurons += region->getTotalNeuronCount();
            totalSynapses += region->getSynapseCount();
        }
        
        if (totalNeurons == 0) {
            NLM_LOG_ERROR("No neurons in brain - connectivity validation failed");
            return false;
        }
        
        if (totalSynapses == 0) {
            NLM_LOG_WARNING("No synapses in brain - consider adjusting connectivity parameters");
        }
        
        // Check neuron density
        float neuronsPerRegion = static_cast<float>(totalNeurons) / regions.size();
        if (neuronsPerRegion < 10) {
            NLM_LOG_WARNING("Low neuron density: " + std::to_string(neuronsPerRegion) + " neurons per region");
        }
        
        return true;
    }
    
    // Rollback individual operations
    static bool rollbackOperation(const std::string& operation) {
        try {
            if (operation == "region_creation") {
                return rollbackRegionCreation();
            } else if (operation == "neuron_creation") {
                return rollbackNeuronCreation();
            } else if (operation == "connectivity_setup") {
                return rollbackConnectivitySetup();
            } else {
                NLM_LOG_WARNING(std::string("Unknown rollback operation: ") + operation);
                return false;
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception during rollback: ") + e.what());
            return false;
        }
    }
    
    // Individual rollback operations
    static bool rollbackRegionCreation() {
        // Placeholder for region creation rollback
        // In practice, this would undo region creation and clean up associated memory
        return true;
    }
    
    static bool rollbackNeuronCreation() {
        // Placeholder for neuron creation rollback
        // In practice, this would undo neuron creation and clean up synapses
        return true;
    }
    
    static bool rollbackConnectivitySetup() {
        // Placeholder for connectivity setup rollback
        // In practice, this would undo synapse creation and cleanup
        return true;
    }
};

// Brain operation timeout wrapper
class BrainOperationTimeout {
public:
    template<typename Func, typename... Args>
    static bool withTimeout(Func&& func, std::chrono::milliseconds timeout,
                           Args&&... args) {
        std::future<bool> future = std::async(std::launch::async, func, std::forward<Args>(args)...);
        
        std::future_status status = future.wait_for(timeout);
        if (status == std::future_status::timeout) {
            NLM_LOG_ERROR("Brain operation timed out after " + 
                         std::to_string(timeout.count()) + " ms");
            return false;
        }
        
        try {
            return future.get();
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Brain operation failed during timeout wait: ") + e.what());
            return false;
        }
    }
};

// Graceful degradation for optional brain features
class BrainGracefulDegradation {
public:
    template<typename Func, typename... Args>
    static auto optionalOperation(Func&& func, Args&&... args,
                                 const std::string& featureName = "Optional Feature") {
        try {
            return func(std::forward<Args>(args)...);
        } catch (const NLMError& e) {
            NLM_LOG_WARNING(featureName + " failed: " + std::string(e.what()) + 
                           ". System will continue with degraded functionality.");
            return decltype(func(std::forward<Args>(args)...)){}; // Return default
        } catch (const std::exception& e) {
            NLM_LOG_WARNING(featureName + " failed with standard exception: " + 
                           std::string(e.what()) + ". System will continue with degraded functionality.");
            return decltype(func(std::forward<Args>(args)...)){}; // Return default
        }
    }
};

} // namespace BrainError

#endif // BRAIN_ERROR_HANDLING_H