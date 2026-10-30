// NLM (熙然) - Neural Learning Machine
// Phase 6 Advanced Error Handling Implementation
//
// This file provides the implementation for the NLM error handling system

#include "ErrorHandling.hpp"

namespace nlm {
    // Initialize error handling globally
    void initializeGlobalErrorHandling() {
        ErrorManager::getInstance().registerHandler(new ConsoleErrorHandler());
    }
    
    // Error handling utility functions
    std::string formatError(const NLMError& error) {
        return error.toString();
    }
    
    bool isErrorRecoverable(const NLMError& error) {
        return error.isRecoverable();
    }
    
    bool shouldTerminate(const NLMError& error) {
        return error.isFatal();
    }
    
    // Configuration validation utilities
    class ConfigValidator {
    public:
        static bool validate(const Config& config) {
            bool valid = true;
            
            // Check required configuration keys
            std::vector<std::string> requiredKeys = {
                "random_seed",
                "simulation_timestep",
                "neuron_count",
                "region_count",
                "connection_probability"
            };
            
            for (const auto& key : requiredKeys) {
                if (!config.has(key)) {
                    ErrorManager::getInstance().report(
                        makeConfigError("Missing required configuration key: " + key, 
                                     ErrorSeverity::Error)
                    );
                    valid = false;
                }
            }
            
            // Validate numeric ranges
            if (config.has("random_seed")) {
                auto seed = config.get<int64_t>("random_seed");
                if (seed && *seed < 0) {
                    ErrorManager::getInstance().report(
                        makeConfigError("Random seed must be non-negative: " + std::to_string(*seed), 
                                     ErrorSeverity::Error)
                    );
                    valid = false;
                }
            }
            
            if (config.has("simulation_timestep")) {
                auto timestep = config.get<double>("simulation_timestep");
                if (timestep && *timestep <= 0.0) {
                    ErrorManager::getInstance().report(
                        makeConfigError("Simulation timestep must be positive: " + std::to_string(*timestep), 
                                     ErrorSeverity::Error)
                    );
                    valid = false;
                }
            }
            
            if (config.has("neuron_count")) {
                auto neuronCount = config.get<int64_t>("neuron_count");
                if (neuronCount && *neuronCount <= 0) {
                    ErrorManager::getInstance().report(
                        makeConfigError("Neuron count must be positive: " + std::to_string(*neuronCount), 
                                     ErrorSeverity::Error)
                    );
                    valid = false;
                }
            }
            
            return valid;
        }
    };
    
    // Simulation safety checker
    class SimulationSafetyChecker {
    public:
        static bool checkSafety(const Brain& brain, uint64_t step) {
            bool safe = true;
            
            // Check for neural membrane potential violations
            auto* spikeSystem = brain.getSpikeSystem();
            if (spikeSystem) {
                // Check neuron counts
                size_t totalNeurons = brain.getTotalNeuronCount();
                size_t activeNeurons = brain.getActiveNeuronCount();
                size_t firingNeurons = brain.getFiringNeuronCount();
                
                // Check for unrealistic activity levels
                if (totalNeurons > 0) {
                    float firingRatio = static_cast<float>(firingNeurons) / totalNeurons;
                    if (firingRatio > 0.95f) {
                        ErrorManager::getInstance().report(
                            makeSimulationError("Excessive firing ratio: " + std::to_string(firingRatio), step)
                        );
                        safe = false;
                    }
                }
                
                // Check spike event counts
                size_t pendingSpikes = spikeSystem->getPendingSpikeEventCount();
                if (pendingSpikes > 10000) { // Threshold for reasonable spike processing
                    ErrorManager::getInstance().report(
                        makeSimulationError("High pending spike count: " + std::to_string(pendingSpikes), step)
                    );
                    safe = false;
                }
            }
            
            // Check synaptic weight stability
            size_t totalSynapses = brain.getTotalSynapseCount();
            if (totalSynapses > 0) {
                // This would require accessing synapse weights - simplified check
                // In real implementation, would check for weight divergence
            }
            
            return safe;
        }
    };
    
    // Memory management utilities
    class MemoryManager {
    public:
        static bool checkMemoryAllocation(size_t size) {
            if (size > 1024 * 1024 * 1024) { // 1GB limit
                ErrorManager::getInstance().report(
                    makeMemoryError("Allocation size exceeds system limit: " + std::to_string(size) + " bytes", size)
                );
                return false;
            }
            return true;
        }
        
        static bool checkMemoryUsage() {
            // This would require platform-specific memory checking
            // For now, provide a placeholder
            return true;
        }
    };
    
    // File I/O utilities
    class FileIOChecker {
    public:
        static bool checkFileAccess(const std::string& filepath, bool write = false) {
            // This would use platform-specific file access checks
            // For now, provide a placeholder that always passes
            return true;
        }
    };
    
    // Wrapper functions for error-prone operations
    bool safeSave(const Brain& brain, const std::string& filepath) {
        TRY_CATCH_ERROR(ErrorCategory::InputOutput, {
            CHECK_ERROR(FileIOChecker::checkFileAccess(filepath, true), 
                       makeIOError("File access denied: " + filepath));
            CHECK_ERROR(brain.save(filepath), 
                       makeIOError("Failed to save brain state to: " + filepath));
            return true;
        });
        return false;
    }
    
    bool safeLoad(Brain& brain, const std::string& filepath) {
        TRY_CATCH_ERROR(ErrorCategory::InputOutput, {
            CHECK_ERROR(FileIOChecker::checkFileAccess(filepath, false), 
                       makeIOError("File access denied: " + filepath));
            CHECK_ERROR(brain.load(filepath), 
                       makeIOError("Failed to load brain state from: " + filepath));
            return true;
        });
        return false;
    }
    
    bool safeInitialize(Brain& brain, const Config& config) {
        TRY_CATCH_ERROR(ErrorCategory::Configuration, {
            CHECK_ERROR(ConfigValidator::validate(config), 
                       makeConfigError("Configuration validation failed"));
            CHECK_ERROR(brain.initialize(), 
                       makeSimulationError("Failed to initialize brain"));
            return true;
        });
        return false;
    }
    
    bool safeStep(Brain& brain, uint64_t step, double time) {
        TRY_CATCH_ERROR(ErrorCategory::Simulation, {
            CHECK_ERROR(SimulationSafetyChecker::checkSafety(brain, step), 
                       makeSimulationError("Safety check failed", step));
            brain.step(step, time);
            return true;
        });
        return false;
    }
    
    // Logging utility functions
    std::string generateLogPrefix() {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = now.time_since_epoch();
        auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() % 1000;
        
        std::tm timeInfo;
        std::time_t timeT = static_cast<std::time_t>(seconds);
#ifdef _WIN32
        localtime_s(&timeInfo, &timeT);
#else
        localtime_r(&timeT, &timeInfo);
#endif
        
        std::ostringstream oss;
        oss << std::put_time(&timeInfo, "%H:%M:%S") << '.' 
            << std::setfill('0') << std::setw(3) << milliseconds;
        return oss.str();
    }
    
    std::string formatMemorySize(size_t bytes) {
        static const char* units[] = {"B", "KB", "MB", "GB", "TB"};
        int unitIndex = 0;
        double size = static_cast<double>(bytes);
        
        while (size >= 1024.0 && unitIndex < 4) {
            size /= 1024.0;
            unitIndex++;
        }
        
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << size << " " << units[unitIndex];
        return oss.str();
    }
}

// Initialize error handling when library is loaded
static bool initialize_error_handling() {
    nlm::initializeGlobalErrorHandling();
    return true;
}

// Use static initialization to ensure error handling is initialized
namespace {
    const bool error_handling_initialized = initialize_error_handling();
}
