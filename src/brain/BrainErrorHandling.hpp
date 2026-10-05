// Brain Error Handling Utilities
#ifndef BRAIN_ERROR_HANDLING_HPP
#define BRAIN_ERROR_HANDLING_HPP

#include "Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <stdexcept>
#include <string>
#include <sstream>

namespace nlm {

// Exception types for NLM errors
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message)
        : std::runtime_error(message) {}
};

class BrainInitializationException : public NLMException {
public:
    explicit BrainInitializationException(const std::string& message)
        : NLMException("Brain initialization failed: " + message) {}
};

class SimulationStepException : public NLMException {
public:
    explicit SimulationStepException(const std::string& message)
        : NLMException("Simulation step failed: " + message) {}
};

class MemoryAllocationException : public NLMException {
public:
    explicit MemoryAllocationException(const std::string& message)
        : NLMException("Memory allocation failed: " + message) {}
};

class ConfigurationException : public NLMException {
public:
    explicit ConfigurationException(const std::string& message)
        : NLMException("Configuration error: " + message) {}
};

// Safe resource management for brain components
class BrainResourceManager {
public:
    BrainResourceManager(Brain* brain) : brain(brain), initialized(false) {}
    
    ~BrainResourceManager() {
        cleanup();
    }
    
    bool initialize() {
        try {
            // Validate configuration before initialization
            if (!validateConfiguration()) {
                throw BrainInitializationException("Invalid configuration");
            }
            
            // Initialize all components
            if (!initializeComponents()) {
                return false;
            }
            
            initialized = true;
            NLM_LOG_INFO("Brain resources initialized successfully");
            return true;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Resource initialization failed: ") + e.what());
            cleanup();
            return false;
        }
    }
    
    void cleanup() {
        if (!initialized) return;
        
        try {
            // Clean up in reverse order of initialization
            cleanupNeuralComponents();
            cleanupMemorySystems();
            cleanupPredictionSystem();
            cleanupCognitionSystems();
            cleanupNeuromodulationSystems();
            cleanupPlasticitySystems();
            cleanupCheckpointSystem();
            
            initialized = false;
            NLM_LOG_INFO("Brain resources cleaned up successfully");
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Resource cleanup failed: ") + e.what());
        }
    }
    
    bool isInitialized() const { return initialized; }
    
private:
    bool validateConfiguration() {
        if (!brain->getConfig()) {
            NLM_LOG_ERROR("Brain has no configuration");
            return false;
        }
        
        // Check required configuration parameters
        auto config = brain->getConfig();
        
        // Validate neuron count
        if (!config->has("neuron_count")) {
            NLM_LOG_WARNING("neuron_count not specified, using default");
        }
        
        // Validate region count
        if (!config->has("region_count")) {
            NLM_LOG_WARNING("region_count not specified, using default");
        }
        
        // Validate connection probability
        if (!config->has("connection_probability")) {
            NLM_LOG_WARNING("connection_probability not specified, using default");
        }
        
        return true;
    }
    
    bool initializeComponents() {
        try {
            initializeNeuralComponents();
            initializeMemorySystems();
            initializePredictionSystem();
            initializeCognitionSystems();
            initializeNeuromodulationSystems();
            initializePlasticitySystems();
            initializeCheckpointSystem();
            
            return true;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Component initialization failed: ") + e.what());
            return false;
        }
    }
    
    void initializeNeuralComponents() {
        // Create regions
        size_t regionCount = brain->getRegionCount();
        if (regionCount == 0) {
            throw BrainInitializationException("No regions to initialize");
        }
        
        for (size_t i = 0; i < regionCount; ++i) {
            try {
                brain->addRegion("Region_" + std::to_string(i + 1));
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Failed to create region ") + std::to_string(i + 1) + ": " + e.what());
                throw;
            }
        }
        
        NLM_LOG_INFO("Created " + std::to_string(regionCount) + " neural regions");
    }
    
    void initializeMemorySystems() {
        if (!brain->getWorkingMemory()) {
            NLM_LOG_WARNING("Working memory not available");
        }
        
        if (!brain->getEpisodicMemory()) {
            NLM_LOG_WARNING("Episodic memory not available");
        }
        
        if (!brain->getAssociativeMemory()) {
            NLM_LOG_WARNING("Associative memory not available");
        }
    }
    
    void initializePredictionSystem() {
        if (!brain->getPredictionSystem()) {
            NLM_LOG_WARNING("Prediction system not available");
        }
    }
    
    void initializeCognitionSystems() {
        if (!brain->getPlanner()) {
            NLM_LOG_WARNING("Neural planner not available");
        }
        
        if (!brain->getConceptFormation()) {
            NLM_LOG_WARNING("Concept formation not available");
        }
        
        if (!brain->getAttention()) {
            NLM_LOG_WARNING("Attention system not available");
        }
    }
    
    void initializeNeuromodulationSystems() {
        if (!brain->getDopamine()) {
            NLM_LOG_WARNING("Dopamine system not available");
        }
        
        if (!brain->getCuriosity()) {
            NLM_LOG_WARNING("Curiosity system not available");
        }
        
        if (!brain->getPredictionErrorSignal()) {
            NLM_LOG_WARNING("Prediction error system not available");
        }
        
        if (!brain->getNovelty()) {
            NLM_LOG_WARNING("Novelty detection not available");
        }
    }
    
    void initializePlasticitySystems() {
        if (!brain->getSTDP()) {
            NLM_LOG_WARNING("STDP plasticity not available");
        }
        
        if (!brain->getHebbian()) {
            NLM_LOG_WARNING("Hebbian plasticity not available");
        }
        
        if (!brain->getStructuralPlasticity()) {
            NLM_LOG_WARNING("Structural plasticity not available");
        }
    }
    
    void initializeCheckpointSystem() {
        if (!brain->getConfig()->has("checkpoint_dir")) {
            NLM_LOG_INFO("Checkpoint directory not configured");
        }
    }
    
    void cleanupNeuralComponents() {
        // Reset all regions
        for (auto rid : brain->getRegionIds()) {
            auto* region = brain->getRegion(rid);
            if (region) {
                region->reset();
            }
        }
        
        // Clear spike system
        if (brain->getSpikeSystem()) {
            brain->getSpikeSystem()->reset();
        }
    }
    
    void cleanupMemorySystems() {
        if (brain->getWorkingMemory()) {
            brain->getWorkingMemory()->clear();
        }
        
        if (brain->getEpisodicMemory()) {
            brain->getEpisodicMemory()->clear();
        }
        
        if (brain->getAssociativeMemory()) {
            brain->getAssociativeMemory()->clear();
        }
    }
    
    void cleanupPredictionSystem() {
        // Prediction system cleanup
    }
    
    void cleanupCognitionSystems() {
        if (brain->getAttention()) {
            brain->getAttention()->reset();
        }
    }
    
    void cleanupNeuromodulationSystems() {
        // Neuromodulation systems cleanup
    }
    
    void cleanupPlasticitySystems() {
        // Plasticity systems cleanup
    }
    
    void cleanupCheckpointSystem() {
        // Checkpoint system cleanup
    }
};

// Safe simulation step execution
class SafeSimulationStep {
public:
    SafeSimulationStep(Brain* brain) : brain(brain) {}
    
    bool executeStep(SimulationStep currentStep, Timestamp currentTime) {
        try {
            // Validate inputs
            if (currentStep < 0) {
                throw SimulationStepException("Invalid simulation step: " + std::to_string(currentStep));
            }
            
            if (currentTime < 0.0) {
                throw SimulationStepException("Invalid simulation time: " + std::to_string(currentTime));
            }
            
            // Execute step with error recovery
            return executeStepWithRecovery(currentStep, currentTime);
            
        } catch (const SimulationStepException& e) {
            NLM_LOG_ERROR(std::string("Simulation step error: ") + e.what());
            return false;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Unexpected error during simulation step: ") + e.what());
            return false;
        }
    }
    
private:
    bool executeStepWithRecovery(SimulationStep currentStep, Timestamp currentTime) {
        // Ensure brain is initialized
        if (!brain->getTotalNeuronCount()) {
            NLM_LOG_ERROR("Brain not properly initialized");
            return false;
        }
        
        // Execute the step
        try {
            brain->step(currentStep, currentTime);
            
            // Validate step execution
            if (!validateStepExecution(currentStep, currentTime)) {
                NLM_LOG_ERROR("Step execution validation failed");
                return false;
            }
            
            NLM_LOG_DEBUG("Simulation step completed successfully");
            return true;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Step execution failed: ") + e.what());
            
            // Attempt recovery
            attemptRecovery(currentStep, currentTime);
            
            return false;
        }
    }
    
    bool validateStepExecution(SimulationStep currentStep, Timestamp currentTime) {
        // Check if all neurons are in valid states
        for (const auto& region : brain->getRegions()) {
            for (const auto& pop : region->getPopulations()) {
                for (const auto* neuron : pop->getNeurons()) {
                    const auto& state = neuron->getState();
                    
                    // Validate neuron state
                    if (std::isnan(state.membranePotential)) {
                        NLM_LOG_ERROR("Invalid neuron membrane potential");
                        return false;
                    }
                    
                    if (state.refractoryRemaining > state.refractoryPeriod) {
                        NLM_LOG_ERROR("Invalid refractory state");
                        return false;
                    }
                }
            }
        }
        
        return true;
    }
    
    void attemptRecovery(SimulationStep currentStep, Timestamp currentTime) {
        NLM_LOG_INFO("Attempting recovery after simulation step failure...");
        
        try {
            // Reset brain to a safe state
            brain->reset();
            brain->initialize();
            
            NLM_LOG_INFO("Recovery completed");
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Recovery failed: ") + e.what());
            throw;
        }
    }
    
    Brain* brain;
};

// Configuration error handling utilities
class SafeConfigLoader {
public:
    static bool safeLoadFromFile(Config& config, const std::string& filepath) {
        try {
            if (config.loadFromFile(filepath)) {
                NLM_LOG_INFO("Configuration loaded successfully from: " + filepath);
                return true;
            } else {
                NLM_LOG_WARNING(std::string("Failed to load configuration from ") + filepath);
                NLM_LOG_INFO("Using default configuration");
                return false;
            }
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Configuration file error: ") + e.what());
            return false;
        }
    }
    
    static bool safeLoadFromArgs(Config& config, int argc, char** argv) {
        try {
            if (config.loadFromArgs(argc, argv)) {
                NLM_LOG_INFO("Configuration loaded from command line arguments");
                return true;
            }
            
            return false;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Command line argument error: ") + e.what());
            return false;
        }
    }
};

} // namespace nlm

#endif // BRAIN_ERROR_HANDLING_HPP