// Brain Error Handling implementation
#include "BrainErrorHandling.hpp"

namespace nlm {

// NLMException implementation
NLMException::NLMException(const std::string& message) : std::runtime_error(message) {}

BrainInitializationException::BrainInitializationException(const std::string& message) : NLMException("Brain initialization failed: " + message) {}

SimulationStepException::SimulationStepException(const std::string& message) : NLMException("Simulation step failed: " + message) {}

MemoryAllocationException::MemoryAllocationException(const std::string& message) : NLMException("Memory allocation failed: " + message) {}

ConfigurationException::ConfigurationException(const std::string& message) : NLMException("Configuration error: " + message) {}

// SafeResourceManager implementation
BrainResourceManager::BrainResourceManager(Brain* brain) : brain(brain), initialized(false) {}

bool BrainResourceManager::initializeComponents() {
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

// SafeSimulationStep implementation
bool SafeSimulationStep::executeStepWithRecovery(SimulationStep currentStep, Timestamp currentTime) {
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

} // namespace nlm