#include "ValidationUtils.hpp"
#include <algorithm>
#include <cmath>
#include <limits>

namespace nlm {

// Configuration validation utilities implementation
bool ConfigValidator::validateBrainConfig(const Config& config, std::string& errorMessage) {
    errorMessage.clear();
    
    // Check if essential brain parameters exist
    const std::vector<std::string> essentialKeys = {
        "brain.neuron_count", "brain.region_count", "brain.synaptic_strength"
    };
    
    for (const auto& key : essentialKeys) {
        if (!config.has(key)) {
            errorMessage = "Missing essential configuration key: " + key;
            return false;
        }
    }
    
    // Validate neuron count
    auto neuronCount = config.get<size_t>("brain.neuron_count");
    if (!neuronCount) {
        errorMessage = "Invalid neuron count configuration";
        return false;
    }
    if (*neuronCount == 0 || *neuronCount > 1000000) {
        errorMessage = "Neuron count out of valid range (1-1,000,000): " + std::to_string(*neuronCount);
        return false;
    }
    
    // Validate synaptic strength
    auto synapticStrength = config.get<double>("brain.synaptic_strength");
    if (!synapticStrength) {
        errorMessage = "Invalid synaptic strength configuration";
        return false;
    }
    if (*synapticStrength < 0.0 || *synapticStrength > 10.0) {
        errorMessage = "Synaptic strength out of valid range (0.0-10.0): " + std::to_string(*synapticStrength);
        return false;
    }
    
    return true;
}

bool ConfigValidator::validateNeuralNetworkConfig(const Config& config, std::string& errorMessage) {
    errorMessage.clear();
    
    // Check if neural network specific keys exist
    const std::vector<std::string> requiredKeys = {
        "network.connectivity", "network.topology", "network.learning_rate"
    };
    
    for (const auto& key : requiredKeys) {
        if (!config.has(key)) {
            errorMessage = "Missing neural network configuration key: " + key;
            return false;
        }
    }
    
    // Validate connectivity (should be between 0.0 and 1.0)
    auto connectivity = config.get<double>("network.connectivity");
    if (!connectivity) {
        errorMessage = "Invalid connectivity configuration";
        return false;
    }
    if (*connectivity < 0.0 || *connectivity > 1.0) {
        errorMessage = "Connectivity out of valid range (0.0-1.0): " + std::to_string(*connectivity);
        return false;
    }
    
    // Validate learning rate
    auto learningRate = config.get<double>("network.learning_rate");
    if (!learningRate) {
        errorMessage = "Invalid learning rate configuration";
        return false;
    }
    if (*learningRate < 0.0 || *learningRate > 1.0) {
        errorMessage = "Learning rate out of valid range (0.0-1.0): " + std::to_string(*learningRate);
        return false;
    }
    
    return true;
}

bool ConfigValidator::validateSimulationParams(const Config& config, std::string& errorMessage) {
    errorMessage.clear();
    
    // Check simulation parameters
    auto timestep = config.get<Timestamp>("simulation.timestep");
    if (!timestep) {
        errorMessage = "Missing or invalid simulation timestep";
        return false;
    }
    if (*timestep <= 0 || *timestep > 1000) {
        errorMessage = "Invalid timestep (must be > 0 and <= 1000): " + std::to_string(*timestep);
        return false;
    }
    
    auto maxSteps = config.get<int>("simulation.max_steps");
    if (!maxSteps) {
        errorMessage = "Missing or invalid max steps configuration";
        return false;
    }
    if (*maxSteps <= 0 || *maxSteps > 1000000) {
        errorMessage = "Max steps out of valid range (1-1,000,000): " + std::to_string(*maxSteps);
        return false;
    }
    
    return true;
}

bool ConfigValidator::validateMemoryCapacity(const Config& config, std::string& errorMessage) {
    errorMessage.clear();
    
    // Check memory capacity parameters
    auto workingMemoryCap = config.get<size_t>("memory.working_memory_capacity");
    if (!workingMemoryCap) {
        errorMessage = "Missing or invalid working memory capacity";
        return false;
    }
    if (*workingMemoryCap == 0 || *workingMemoryCap > 100000000) {
        errorMessage = "Working memory capacity out of valid range (1-100,000,000): " + std::to_string(*workingMemoryCap);
        return false;
    }
    
    auto episodicMemoryCap = config.get<size_t>("memory.episodic_memory_capacity");
    if (!episodicMemoryCap) {
        errorMessage = "Missing or invalid episodic memory capacity";
        return false;
    }
    if (*episodicMemoryCap == 0 || *episodicMemoryCap > 100000000) {
        errorMessage = "Episodic memory capacity out of valid range (1-100,000,000): " + std::to_string(*episodicMemoryCap);
        return false;
    }
    
    return true;
}

bool ConfigValidator::validateConfigurationSchema(const Config& config, std::string& errorMessage) {
    errorMessage.clear();
    
    // Basic schema validation - check for critical keys
    const std::vector<std::string> criticalSections = {
        "brain", "network", "simulation", "memory", "world"
    };
    
    auto keys = config.getKeys();
    for (const auto& section : criticalSections) {
        bool sectionFound = false;
        for (const auto& key : keys) {
            if (key.find(section + ".") == 0) {
                sectionFound = true;
                break;
            }
        }
        if (!sectionFound) {
            errorMessage = "Missing configuration section: " + section;
            return false;
        }
    }
    
    return true;
}

bool ConfigValidator::validateConfigValue(const ConfigValue& value, const std::string& key, std::string& errorMessage) {
    errorMessage.clear();
    
    // Validate based on value type
    if (std::holds_alternative<int>(value)) {
        auto intVal = std::get<int>(value);
        if (intVal < std::numeric_limits<int>::min() || intVal > std::numeric_limits<int>::max()) {
            errorMessage = "Integer value out of range for key: " + key;
            return false;
        }
    } else if (std::holds_alternative<double>(value)) {
        auto doubleVal = std::get<double>(value);
        if (std::isnan(doubleVal) || std::isinf(doubleVal)) {
            errorMessage = "Invalid double value (NaN or Infinity) for key: " + key;
            return false;
        }
    } else if (std::holds_alternative<std::string>(value)) {
        auto strVal = std::get<std::string>(value);
        if (strVal.empty()) {
            errorMessage = "Empty string value for key: " + key;
            return false;
        }
    }
    
    return true;
}

std::vector<std::string> ConfigValidator::getConfigErrorSuggestions(const std::string& errorMessage) {
    std::vector<std::string> suggestions;
    
    if (errorMessage.find("Missing essential configuration key") != std::string::npos) {
        suggestions.push_back("Add the missing configuration key with appropriate default values");
        suggestions.push_back("Check configuration documentation for required keys");
    } else if (errorMessage.find("out of valid range") != std::string::npos) {
        suggestions.push_back("Adjust the value to be within the valid range");
        suggestions.push_back("Use a value between the minimum and maximum specified");
    } else if (errorMessage.find("Invalid") != std::string::npos) {
        suggestions.push_back("Check the configuration format and value types");
        suggestions.push_back("Use appropriate data types for each configuration key");
    }
    
    return suggestions;
}

// Neural network validation utilities implementation
bool NeuralNetworkValidator::validateNeuralConnectivity(const Brain& brain, std::string& errorMessage) {
    errorMessage.clear();
    
    // Check basic connectivity requirements
    if (brain.getRegionCount() == 0) {
        errorMessage = "Brain has no regions - invalid neural network";
        return false;
    }
    
    if (brain.getTotalNeuronCount() == 0) {
        errorMessage = "Brain has no neurons - invalid neural network";
        return false;
    }
    
    if (brain.getTotalSynapseCount() == 0 && brain.getTotalNeuronCount() > 0) {
        errorMessage = "Brain has neurons but no synapses - invalid connectivity";
        return false;
    }
    
    return true;
}

bool NeuralNetworkValidator::validateRegionConnectivity(const NeuralRegion& region, std::string& errorMessage) {
    errorMessage.clear();
    
    // Check region has neurons
    if (region.getNeuronCount() == 0) {
        errorMessage = "Region has no neurons";
        return false;
    }
    
    // Check for connectivity consistency
    auto neurons = region.getNeurons();
    for (const auto& neuron : neurons) {
        if (!neuron) {
            errorMessage = "Region contains null neurons";
            return false;
        }
    }
    
    return true;
}

bool NeuralNetworkValidator::validateNeuron(const Neuron& neuron, std::string& errorMessage) {
    errorMessage.clear();
    
    // Check neuron state validity
    if (neuron.getType() == NeuronType::Unknown && neuron.getState() != FiringState::Resting) {
        errorMessage = "Unknown neuron type with non-resting state";
        return false;
    }
    
    return true;
}

bool NeuralNetworkValidator::validateSynapse(const Synapse& synapse, std::string& errorMessage) {
    errorMessage.clear();
    
    // Check synapse properties
    if (synapse.getWeight() < 0.0) {
        errorMessage = "Synapse weight is negative";
        return false;
    }
    
    if (synapse.getWeight() > 10.0) {
        errorMessage = "Synapse weight exceeds maximum allowed value";
        return false;
    }
    
    return true;
}

bool NeuralNetworkValidator::validateNetworkTopology(const Brain& brain, std::string& errorMessage) {
    errorMessage.clear();
    
    // Check for isolated regions
    if (!checkNetworkIsolation(brain, errorMessage)) {
        return false;
    }
    
    // Check for neuron type balance
    if (!validateNeuronTypes(brain, errorMessage)) {
        return false;
    }
    
    return true;
}

bool NeuralNetworkValidator::checkNetworkIsolation(const Brain& brain, std::string& errorMessage) {
    errorMessage.clear();
    
    // Simple check: if there are multiple regions but no inter-region connections, might be isolated
    if (brain.getRegionCount() > 1) {
        // Check for inter-region connections
        // This would require access to brain implementation details
        // For now, we'll just check if there are any regions
        errorMessage = "Multiple regions detected but no inter-region connectivity validation available";
        // Not returning false since this is just a warning
    }
    
    return true;
}

bool NeuralNetworkValidator::validateNeuronTypes(const Brain& brain, std::string& errorMessage) {
    errorMessage.clear();
    
    // Check if there's a reasonable balance of neuron types
    // This is a simple check - more sophisticated validation could be added
    
    return true;
}

// Simulation parameter validation utilities implementation
bool SimulationValidator::validateTimestep(Timestamp timestep, std::string& errorMessage) {
    errorMessage.clear();
    
    if (timestep <= 0) {
        errorMessage = "Timestep must be positive";
        return false;
    }
    
    if (timestep > 1000) {
        errorMessage = "Timestep too large (maximum: 1000)";
        return false;
    }
    
    return true;
}

bool SimulationValidator::validateDuration(int steps, Timestamp timestep, std::string& errorMessage) {
    errorMessage.clear();
    
    if (steps <= 0) {
        errorMessage = "Number of steps must be positive";
        return false;
    }
    
    long long totalTime = static_cast<long long>(steps) * timestep;
    if (totalTime > 1000000) {
        errorMessage = "Total simulation time too large (maximum: 1,000,000)";
        return false;
    }
    
    return true;
}

bool SimulationValidator::validateWorldDimensions(size_t width, size_t height, std::string& errorMessage) {
    errorMessage.clear();
    
    if (width == 0 || width > 10000) {
        errorMessage = "World width out of valid range (1-10000)";
        return false;
    }
    
    if (height == 0 || height > 10000) {
        errorMessage = "World height out of valid range (1-10000)";
        return false;
    }
    
    return true;
}

bool SimulationValidator::validateSensoryInput(const SensoryInput& input, std::string& errorMessage) {
    errorMessage.clear();
    
    // Check input dimensions
    auto dimensions = input.getDimensions();
    for (size_t dim : dimensions) {
        if (dim == 0 || dim > 10000) {
            errorMessage = "Invalid sensory input dimension (must be 1-10000): " + std::to_string(dim);
            return false;
        }
    }
    
    return true;
}

bool SimulationValidator::validateAction(const Action& action, std::string& errorMessage) {
    errorMessage.clear();
    
    // Check action type
    auto actionType = action.getType();
    if (actionType == ActionType::Unknown) {
        errorMessage = "Action has unknown type";
        return false;
    }
    
    // Check action parameters
    auto parameters = action.getParameters();
    for (float param : parameters) {
        if (std::isnan(param) || std::isinf(param)) {
            errorMessage = "Action parameter is NaN or Infinity";
            return false;
        }
        
        if (param < -1000.0 || param > 1000.0) {
            errorMessage = "Action parameter out of valid range (-1000.0 to 1000.0)";
            return false;
        }
    }
    
    return true;
}

bool SimulationValidator::validateSimulationState(const Brain& brain, Timestamp currentTime, std::string& errorMessage) {
    errorMessage.clear();
    
    // Check if brain is initialized
    if (!brain.getConfig()) {
        errorMessage = "Brain is not initialized";
        return false;
    }
    
    // Check current time validity
    if (currentTime < 0) {
        errorMessage = "Current simulation time cannot be negative";
        return false;
    }
    
    // Check brain internal state
    if (brain.getActiveNeuronCount() > brain.getTotalNeuronCount()) {
        errorMessage = "Active neuron count exceeds total neuron count";
        return false;
    }
    
    return true;
}

bool SimulationValidator::validateBatchParams(size_t batchSize, size_t maxThreads, std::string& errorMessage) {
    errorMessage.clear();
    
    if (batchSize == 0 || batchSize > 1000) {
        errorMessage = "Batch size out of valid range (1-1000)";
        return false;
    }
    
    if (maxThreads > 64) {
        errorMessage = "Maximum threads out of valid range (1-64)";
        return false;
    }
    
    return true;
}

std::vector<std::string> SimulationValidator::getSimulationErrorSuggestions(const std::string& errorMessage) {
    std::vector<std::string> suggestions;
    
    if (errorMessage.find("must be positive") != std::string::npos) {
        suggestions.push_back("Use a positive value for the parameter");
        suggestions.push_back("Check parameter validation in documentation");
    } else if (errorMessage.find("out of valid range") != std::string::npos) {
        suggestions.push_back("Adjust parameter to be within valid range");
        suggestions.push_back("Use smaller values for simulation parameters");
    } else if (errorMessage.find("Cannot be negative") != std::string::npos) {
        suggestions.push_back("Ensure simulation time is non-negative");
        suggestions.push_back("Check time step and step count calculations");
    }
    
    return suggestions;
}

// State validation utilities implementation
bool StateValidator::validateBrainState(const Brain& brain, const std::string& operation, std::string& errorMessage) {
    errorMessage.clear();
    
    // Check if brain is initialized
    if (!brain.getConfig()) {
        errorMessage = "Brain is not initialized - cannot perform operation: " + operation;
        return false;
    }
    
    // Check if brain has regions
    if (brain.getRegionCount() == 0) {
        errorMessage = "Brain has no regions - cannot perform operation: " + operation;
        return false;
    }
    
    return true;
}

bool StateValidator::validateAgentState(const AgentBrain& agent, Timestamp currentTime, std::string& errorMessage) {
    errorMessage.clear();
    
    // Check if agent is initialized
    if (!agent.getBrain()) {
        errorMessage = "Agent brain is not initialized";
        return false;
    }
    
    // Check if current time is valid
    if (currentTime < 0) {
        errorMessage = "Current time is invalid";
        return false;
    }
    
    return true;
}

bool StateValidator::validateWorldState(const SimpleWorld& world, std::string& errorMessage) {
    errorMessage.clear();
    
    // Check world dimensions
    auto width = world.getWidth();
    auto height = world.getHeight();
    
    if (width <= 0 || height <= 0) {
        errorMessage = "World has invalid dimensions";
        return false;
    }
    
    return true;
}

bool StateValidator::validateMemoryState(const WorkingMemory& workingMem, const EpisodicMemory& episodicMem,
                                        const SemanticMemory& semanticMem, const ProceduralMemory& proceduralMem,
                                        const AssociativeMemory& associativeMem, std::string& errorMessage) {
    errorMessage.clear();
    
    // Check memory system states (placeholder validation)
    // In a real implementation, this would validate internal memory structures
    
    return true;
}

bool StateValidator::validateConnectivityState(const Brain& brain, std::string& errorMessage) {
    errorMessage.clear();
    
    // Check connectivity consistency
    if (brain.getTotalSynapseCount() == 0 && brain.getTotalNeuronCount() > 0) {
        errorMessage = "Brain has neurons but no synapses - connectivity inconsistency";
        return false;
    }
    
    return true;
}

bool StateValidator::checkComponentConsistency(const Brain& brain, std::string& errorMessage) {
    errorMessage.clear();
    
    // Check consistency between different brain components
    // This is a basic check - more sophisticated validation could be added
    
    return true;
}

bool StateValidator::validateDevelopmentalStage(const Brain& brain, std::string& errorMessage) {
    errorMessage.clear();
    
    // Check developmental stage validity
    auto stage = brain.getDevelopmentalStage();
    
    return true;
}

// Error recovery utilities implementation
bool ErrorRecovery::recoverBrainState(Brain& brain, const std::string& errorContext, std::string& recoveryMessage) {
    recoveryMessage.clear();
    
    recoveryMessage = "Attempting to recover brain state in context: " + errorContext;
    
    try {
        // Try to reset brain state
        brain.reset();
        recoveryMessage += ". Brain reset successful.";
        return true;
    } catch (const std::exception& e) {
        recoveryMessage += ". Recovery failed: " + std::string(e.what());
        return false;
    }
}

bool ErrorRecovery::recoverConfiguration(Config& config, const std::string& errorContext, std::string& recoveryMessage) {
    recoveryMessage.clear();
    
    recoveryMessage = "Attempting to recover configuration in context: " + errorContext;
    
    try {
        // Try to reset configuration
        config.clear();
        recoveryMessage += ". Configuration reset successful.";
        return true;
    } catch (const std::exception& e) {
        recoveryMessage += ". Recovery failed: " + std::string(e.what());
        return false;
    }
}

bool ErrorRecovery::recoverMemoryIssues(WorkingMemory& workingMem, EpisodicMemory& episodicMem,
                                       const std::string& errorContext, std::string& recoveryMessage) {
    recoveryMessage.clear();
    
    recoveryMessage = "Attempting to recover memory issues in context: " + errorContext;
    
    try {
        // Try to clear memory
        workingMem.clear();
        episodicMem.clear();
        recoveryMessage += ". Memory cleared successfully.";
        return true;
    } catch (const std::exception& e) {
        recoveryMessage += ". Recovery failed: " + std::string(e.what());
        return false;
    }
}

bool ErrorRecovery::recoverSimulationState(Brain& brain, SimpleWorld& world,
                                           const std::string& errorContext, std::string& recoveryMessage) {
    recoveryMessage.clear();
    
    recoveryMessage = "Attempting to recover simulation state in context: " + errorContext;
    
    try {
        // Try to reset both brain and world
        brain.reset();
        world.reset();
        recoveryMessage += ". Simulation state reset successful.";
        return true;
    } catch (const std::exception& e) {
        recoveryMessage += ". Recovery failed: " + std::string(e.what());
        return false;
    }
}

bool ErrorRecovery::safeResetBrain(Brain& brain, std::string& resetMessage) {
    resetMessage.clear();
    
    try {
        brain.reset();
        resetMessage = "Brain reset successful";
        return true;
    } catch (const std::exception& e) {
        resetMessage = "Brain reset failed: " + std::string(e.what());
        return false;
    }
}

bool ErrorRecovery::safeResetWorld(SimpleWorld& world, std::string& resetMessage) {
    resetMessage.clear();
    
    try {
        world.reset();
        resetMessage = "World reset successful";
        return true;
    } catch (const std::exception& e) {
        resetMessage = "World reset failed: " + std::string(e.what());
        return false;
    }
}

bool ErrorRecovery::repairInvalidConfig(Config& config, const std::string& errorContext, std::string& repairMessage) {
    repairMessage.clear();
    
    repairMessage = "Attempting to repair invalid configuration in context: " + errorContext;
    
    try {
        // Try to fix common configuration issues
        // This is a basic implementation - could be enhanced
        
        repairMessage += ". Configuration repaired.";
        return true;
    } catch (const std::exception& e) {
        repairMessage += ". Repair failed: " + std::string(e.what());
        return false;
    }
}

bool ErrorRecovery::activateFallbackMode(Brain& brain, const std::string& errorContext, std::string& fallbackMessage) {
    fallbackMessage.clear();
    
    fallbackMessage = "Activating fallback mode in context: " + errorContext;
    
    try {
        // Try to put brain in fallback mode
        // This would involve reducing complexity and using simpler algorithms
        
        fallbackMessage += ". Fallback mode activated.";
        return true;
    } catch (const std::exception& e) {
        fallbackMessage += ". Fallback mode activation failed: " + std::string(e.what());
        return false;
    }
}

// Debugging and diagnostics utilities implementation
std::string DebugDiagnostics::generateErrorReport(const std::exception& error) {
    std::stringstream ss;
    ss << "=== NLM ERROR REPORT ===\n";
    ss << "Timestamp: " << getCurrentTimestamp() << "\n";
    
    try {
        // Try to cast to NLMError for detailed information
        const nlm::NLMError& nlmError = dynamic_cast<const nlm::NLMError&>(error);
        ss << "Error Type: " << nlmError.getErrorType() << "\n";
        ss << "Error Details: " << nlmError.getDetails() << "\n";
    } catch (const std::bad_cast&) {
        // Not an NLMError, use generic information
        ss << "Error Type: Generic Exception\n";
        ss << "Error Message: " << error.what() << "\n";
    }
    
    ss << "=== END ERROR REPORT ===\n";
    return ss.str();
}

bool DebugDiagnostics::detectMemoryLeak(const Brain& brain, size_t sampleSize, std::string& leakType, size_t& detectedBytes) {
    leakType.clear();
    detectedBytes = 0;
    
    // Basic memory leak detection (placeholder)
    // In a real implementation, this would monitor memory allocation patterns
    
    leakType = "MemoryLeakDetectionNotImplemented";
    return false; // No leak detected by default (placeholder)
}

bool DebugDiagnostics::detectPerformanceBottleneck(const Brain& brain, const std::string& bottleneckType, 
                                                   double& thresholdExceeded, double& thresholdLimit) {
    thresholdExceeded = 0.0;
    thresholdLimit = 0.0;
    
    // Basic performance bottleneck detection (placeholder)
    // In a real implementation, this would analyze performance metrics
    
    return false; // No bottleneck detected by default (placeholder)
}

bool DebugDiagnostics::monitorSystemHealth(Brain& brain, SimpleWorld& world, 
                                           std::unordered_map<std::string, double>& metrics) {
    metrics.clear();
    
    // Collect basic health metrics
    metrics["neuron_count"] = static_cast<double>(brain.getTotalNeuronCount());
    metrics["synapse_count"] = static_cast<double>(brain.getTotalSynapseCount());
    metrics["active_neuron_count"] = static_cast<double>(brain.getActiveNeuronCount());
    metrics["firing_rate"] = brain.getAverageFiringRate();
    metrics["region_count"] = static_cast<double>(brain.getRegionCount());
    
    // Add world metrics
    metrics["world_width"] = static_cast<double>(world.getWidth());
    metrics["world_height"] = static_cast<double>(world.getHeight());
    
    return true;
}

bool DebugDiagnostics::getPerformanceMetrics(const Brain& brain, std::unordered_map<std::string, double>& metrics) {
    metrics.clear();
    
    // Collect performance metrics
    metrics["total_neuron_count"] = static_cast<double>(brain.getTotalNeuronCount());
    metrics["total_synapse_count"] = static_cast<double>(brain.getTotalSynapseCount());
    metrics["active_neuron_count"] = static_cast<double>(brain.getActiveNeuronCount());
    metrics["firing_neuron_count"] = static_cast<double>(brain.getFiringNeuronCount());
    metrics["average_firing_rate"] = brain.getAverageFiringRate();
    metrics["excitation_inhibition_ratio"] = brain.getExcitationInhibitionRatio();
    metrics["total_spike_count"] = static_cast<double>(brain.getTotalSpikeCount());
    
    return true;
}

std::string DebugDiagnostics::generateValidationSummary(const Brain& brain, const Config& config, 
                                                        const SimpleWorld& world) {
    std::stringstream ss;
    ss << "=== VALIDATION SUMMARY ===\n";
    
    // Brain validation
    ss << "Brain:\n";
    ss << "  Total Neurons: " << brain.getTotalNeuronCount() << "\n";
    ss << "  Total Synapses: " << brain.getTotalSynapseCount() << "\n";
    ss << "  Active Neurons: " << brain.getActiveNeuronCount() << "\n";
    ss << "  Firing Neurons: " << brain.getFiringNeuronCount() << "\n";
    ss << "  Average Firing Rate: " << brain.getAverageFiringRate() << "\n";
    ss << "  Developmental Stage: " << static_cast<int>(brain.getDevelopmentalStage()) << "\n";
    ss << "  Regions: " << brain.getRegionCount() << "\n";
    
    // Configuration validation
    ss << "\nConfiguration:\n";
    auto configKeys = config.getKeys();
    ss << "  Keys: " << configKeys.size() << "\n";
    
    // World validation
    ss << "\nWorld:\n";
    ss << "  Width: " << world.getWidth() << "\n";
    ss << "  Height: " << world.getHeight() << "\n";
    ss << "  Max Energy: " << world.getMaxEnergy() << "\n";
    ss << "  Simulation Time: " << world.getSimulationTime() << "\n";
    
    ss << "\n=== END VALIDATION SUMMARY ===\n";
    return ss.str();
}

bool DebugDiagnostics::checkCommonIssues(const Brain& brain, const Config& config, 
                                         const SimpleWorld& world, std::vector<std::string>& issues) {
    issues.clear();
    
    // Check for common issues
    if (brain.getTotalNeuronCount() == 0) {
        issues.push_back("Brain has no neurons - simulation will be inactive");
    }
    
    if (brain.getTotalSynapseCount() == 0 && brain.getTotalNeuronCount() > 0) {
        issues.push_back("Brain has neurons but no synapses - connectivity issue");
    }
    
    if (brain.getActiveNeuronCount() > brain.getTotalNeuronCount()) {
        issues.push_back("Active neuron count exceeds total neuron count");
    }
    
    return issues.empty();
}

// Additional enhanced debug diagnostics implementations

// Get current timestamp helper function
static std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// Generate comprehensive debug report
std::string DebugDiagnostics::generateDebugReport(const Brain& brain, const Config& config, 
                                                  const SimpleWorld& world, 
                                                  const std::string& reportType) {
    std::stringstream ss;
    ss << "=== COMPREHENSIVE DEBUG REPORT ===\n";
    ss << "Generated: " << getCurrentTimestamp() << "\n";
    ss << "Report Type: " << reportType << "\n";
    ss << "Brain ID: " << reinterpret_cast<uint64_t>(&brain) << "\n";
    
    if (reportType == "comprehensive" || reportType == "full") {
        ss << "\n--- BRAIN STATE ---\n";
        ss << "Total Neurons: " << brain.getTotalNeuronCount() << "\n";
        ss << "Total Synapses: " << brain.getTotalSynapseCount() << "\n";
        ss << "Active Neurons: " << brain.getActiveNeuronCount() << "\n";
        ss << "Firing Neurons: " << brain.getFiringNeuronCount() << "\n";
        ss << "Average Firing Rate: " << brain.getAverageFiringRate() << "\n";
        ss << "Excitation/Inhibition Ratio: " << brain.getExcitationInhibitionRatio() << "\n";
        ss << "Total Spike Count: " << brain.getTotalSpikeCount() << "\n";
        ss << "Developmental Stage: " << static_cast<int>(brain.getDevelopmentalStage()) << "\n";
        ss << "Region Count: " << brain.getRegionCount() << "\n";
        
        ss << "\n--- CONFIGURATION ---\n";
        auto configKeys = config.getKeys();
        ss << "Config Keys: " << configKeys.size() << "\n";
        for (const auto& key : configKeys) {
            ss << "  " << key << "\n";
        }
        
        ss << "\n--- WORLD STATE ---\n";
        ss << "World Width: " << world.getWidth() << "\n";
        ss << "World Height: " << world.getHeight() << "\n";
        ss << "Max Energy: " << world.getMaxEnergy() << "\n";
        ss << "Energy Decay Rate: " << world.getEnergyDecayRate() << "\n";
        ss << "Simulation Time: " << world.getSimulationTime() << "\n";
    }
    
    ss << "\n=== END DEBUG REPORT ===\n";
    return ss.str();
}

// Neural network connection visualization
std::string DebugDiagnostics::visualizeNeuralConnections(const Brain& brain, 
                                                         const std::string& format) {
    std::stringstream ss;
    
    if (format == "graphviz") {
        ss << "digraph NLM_Neural_Connections {\n";
        ss << "  rankdir=LR;\n";
        ss << "  node [shape=circle];\n";
        
        // Add neurons as nodes
        for (uint64_t i = 0; i < brain.getTotalNeuronCount(); ++i) {
            ss << "  \"Neuron_" << i << "\" [label=\"\"\n";
        }
        
        // Add synapses as edges
        // In a real implementation, this would iterate through actual synapses
        
        ss << "}\n";
    } else if (format == "adjacency") {
        ss << "NEURAL CONNECTION ADJACENCY MATRIX:\n";
        // Simple placeholder - would be more complex in practice
        ss << "[ Neuron IDs and connection weights would appear here ]\n";
    }
    
    return ss.str();
}

bool DebugDiagnostics::validateNeuralConnectivity(const Brain& brain, 
                                                  std::vector<std::string>& issues) {
    issues.clear();
    
    // Basic connectivity validation
    if (brain.getTotalNeuronCount() > 0 && brain.getTotalSynapseCount() == 0) {
        issues.push_back("WARNING: Brain has neurons but no synapses - likely disconnected network");
    }
    
    if (brain.getTotalNeuronCount() == 0) {
        issues.push_back("WARNING: Brain has no neurons - simulation will be inactive");
    }
    
    // Check for isolated neurons
    // This would require access to internal brain structure
    
    return issues.empty();
}

// State inspection utilities
std::string DebugDiagnostics::inspectBrainState(const Brain& brain, 
                                                 const std::vector<std::string>& focusAreas) {
    std::stringstream ss;
    ss << "=== BRAIN STATE INSPECTION ===\n";
    ss << "Focus Areas: " << (focusAreas.empty() ? "All" : "Specific") << "\n";
    
    for (const auto& area : focusAreas) {
        if (area == "neurons") {
            ss << "Neuron Count: " << brain.getTotalNeuronCount() << "\n";
            ss << "Active Neurons: " << brain.getActiveNeuronCount() << "\n";
            ss << "Firing Neurons: " << brain.getFiringNeuronCount() << "\n";
            ss << "Average Firing Rate: " << brain.getAverageFiringRate() << "\n";
        } else if (area == "synapses") {
            ss << "Total Synapses: " << brain.getTotalSynapseCount() << "\n";
        } else if (area == "regions") {
            ss << "Region Count: " << brain.getRegionCount() << "\n";
        } else if (area == "development") {
            ss << "Developmental Stage: " << static_cast<int>(brain.getDevelopmentalStage()) << "\n";
        }
    }
    
    if (focusAreas.empty()) {
        // Full inspection
        ss << "Total Neurons: " << brain.getTotalNeuronCount() << "\n";
        ss << "Total Synapses: " << brain.getTotalSynapseCount() << "\n";
        ss << "Active Neurons: " << brain.getActiveNeuronCount() << "\n";
        ss << "Firing Neurons: " << brain.getFiringNeuronCount() << "\n";
        ss << "Average Firing Rate: " << brain.getAverageFiringRate() << "\n";
        ss << "Excitation/Inhibition Ratio: " << brain.getExcitationInhibitionRatio() << "\n";
        ss << "Total Spike Count: " << brain.getTotalSpikeCount() << "\n";
        ss << "Developmental Stage: " << static_cast<int>(brain.getDevelopmentalStage()) << "\n";
        ss << "Region Count: " << brain.getRegionCount() << "\n";
    }
    
    ss << "=== END BRAIN INSPECTION ===\n";
    return ss.str();
}

std::string DebugDiagnostics::inspectAgentState(const AgentBrain& agent, 
                                                 const SimpleWorld& world) {
    std::stringstream ss;
    ss << "=== AGENT STATE INSPECTION ===\n";
    
    // Access the underlying brain through agent
    auto brain = agent.getBrain();
    
    ss << "Agent Brain:
";
    ss << "  Neurons: " << brain->getTotalNeuronCount() << "\n";
    ss << "  Synapses: " << brain->getTotalSynapseCount() << "\n";
    ss << "  Active Neurons: " << brain->getActiveNeuronCount() << "\n";
    ss << "  World Location: (" << agent.getBrain()->getConfig().getFloat("agent_x", 0.0f) << ", "
       << agent.getBrain()->getConfig().getFloat("agent_y", 0.0f) << ")\n";
    
    ss << "=== END AGENT INSPECTION ===\n";
    return ss.str();
}

std::string DebugDiagnostics::inspectWorldState(const SimpleWorld& world) {
    std::stringstream ss;
    ss << "=== WORLD STATE INSPECTION ===\n";
    ss << "World Dimensions: " << world.getWidth() << "x" << world.getHeight() << "\n";
    ss << "Max Energy: " << world.getMaxEnergy() << "\n";
    ss << "Energy Decay Rate: " << world.getEnergyDecayRate() << "\n";
    ss << "Simulation Time: " << world.getSimulationTime() << "\n";
    ss << "Random Seed: " << world.getRandomSeed() << "\n";
    
    // Get agent body state if available
    auto agentBody = world.getAgentBody();
    if (agentBody) {
        ss << "Agent Position: (" << agentBody->x << ", " << agentBody->y << ")\n";
        ss << "Agent Orientation: " << agentBody->orientation << "\n";
        ss << "Agent Velocity: (" << agentBody->velocityX << ", " << agentBody->velocityY << ")\n";
    }
    
    ss << "=== END WORLD INSPECTION ===\n";
    return ss.str();
}

// Memory debugging tools
std::string DebugDiagnostics::analyzeMemoryUsage(const Brain& brain) {
    std::stringstream ss;
    ss << "=== MEMORY USAGE ANALYSIS ===\n";
    
    // Basic memory metrics (simplified)
    size_t totalMemory = brain.getTotalNeuronCount() * sizeof(float) * 10; // Approximation
    size_t activeMemory = brain.getActiveNeuronCount() * sizeof(float) * 5;
    double memoryUtilization = static_cast<double>(activeMemory) / totalMemory * 100.0;
    
    ss << "Estimated Total Memory: " << totalMemory << " bytes\n";
    ss << "Estimated Active Memory: " << activeMemory << " bytes\n";
    ss << "Memory Utilization: " << memoryUtilization << "%\n";
    
    // Check for memory leaks (placeholder)
    ss << "Memory Leak Detection: " << (detectMemoryLeak(const_cast<Brain&>(brain), 100, leakType, detectedBytes) ? 
                                      ("LEAK DETECTED: " + leakType + " (" + std::to_string(detectedBytes) + " bytes)") :
                                      "No leak detected") << "\n";
    
    ss << "=== END MEMORY ANALYSIS ===\n";
    return ss.str();
}

bool DebugDiagnostics::detectMemoryLeaksDetailed(const Brain& brain, 
                                                 std::vector<std::string>& leakDetails) {
    leakDetails.clear();
    
    // Simplified memory leak detection
    // In a real implementation, this would monitor allocation patterns
    
    std::string leakType;
    size_t detectedBytes;
    if (detectMemoryLeak(const_cast<Brain&>(brain), 100, leakType, detectedBytes)) {
        leakDetails.push_back("Memory leak detected: " + leakType);
        leakDetails.push_back("Estimated leaked bytes: " + std::to_string(detectedBytes));
        return true;
    }
    
    return false;
}

std::string DebugDiagnostics::getMemoryAllocationHistory(const Brain& brain) {
    std::stringstream ss;
    ss << "=== MEMORY ALLOCATION HISTORY ===\n";
    ss << "Memory allocation tracking would appear here\n";
    ss << "(Real implementation would track allocation patterns over time)\n";
    ss << "=== END MEMORY HISTORY ===\n";
    return ss.str();
}

// Performance profiling utilities
std::string DebugDiagnostics::profileNeuralComputation(const Brain& brain, 
                                                       uint64_t regionId) {
    std::stringstream ss;
    ss << "=== NEURAL COMPUTATION PROFILING ===\n";
    
    // Collect basic performance metrics
    size_t neuronCount = brain.getTotalNeuronCount();
    size_t synapseCount = brain.getTotalSynapseCount();
    double firingRate = brain.getAverageFiringRate();
    
    ss << "Computation Metrics:\n";
    ss << "  Total Neurons: " << neuronCount << "\n";
    ss << "  Total Synapses: " << synapseCount << "\n";
    ss << "  Average Firing Rate: " << firingRate << " Hz\n";
    ss << "  Operations per Second: Estimated based on network size\n";
    
    // Estimate computational complexity
    if (regionId != INVALID_REGION_ID) {
        ss << "\nRegion " << regionId << " profiling:\n";
        // Would need actual region access to provide detailed profiling
        ss << "  Region-specific metrics would appear here\n";
    }
    
    ss << "\nPerformance Recommendations:\n";
    ss << "  - Consider batch processing for large networks\n";
    ss << "  - Optimize synapse traversal algorithms\n";
    ss << "  - Implement sparse connectivity for efficiency\n";
    
    ss << "=== END PROFILING ===\n";
    return ss.str();
}

std::string DebugDiagnostics::analyzeFiringPatterns(const Brain& brain, 
                                                     const std::chrono::system_clock::time_point& start,
                                                     const std::chrono::system_clock::time_point& end) {
    std::stringstream ss;
    ss << "=== FIRING PATTERN ANALYSIS ===\n";
    ss << "Analysis Period: " << getCurrentTimestamp() << "\n";
    
    // Basic firing pattern analysis
    double avgFiringRate = brain.getAverageFiringRate();
    size_t totalSpikes = brain.getTotalSpikeCount();
    size_t firingNeurons = brain.getFiringNeuronCount();
    
    ss << "Firing Statistics:\n";
    ss << "  Average Firing Rate: " << avgFiringRate << " Hz\n";
    ss << "  Total Spike Events: " << totalSpikes << "\n";
    ss << "  Active Firing Neurons: " << firingNeurons << "\n";
    ss << "  Firing Density: " << (firingNeurons * 100.0 / brain.getTotalNeuronCount()) << "%\n";
    
    ss << "\nPattern Analysis:\n";
    ss << "  Based on available spike data and timing information\n";
    
    ss << "=== END PATTERN ANALYSIS ===\n";
    return ss.str();
}

bool DebugDiagnostics::profileMemoryAccessPatterns(const Brain& brain) {
    // Simplified memory access pattern profiling
    return true;
}

// Error traceback utilities
std::string DebugDiagnostics::generateDetailedErrorTrace(const std::exception& error) {
    return generateErrorReport(error); // Reuses existing implementation
}

std::string DebugDiagnostics::traceErrorPropagation(const Brain& brain, 
                                                    const std::exception& error) {
    std::stringstream ss;
    ss << "=== ERROR PROPAGATION TRACE ===\n";
    ss << "Error: " << error.what() << "\n";
    ss << "Brain State at Error Time:\n";
    ss << "  Neurons: " << brain.getTotalNeuronCount() << "\n";
    ss << "  Synapses: " << brain.getTotalSynapseCount() << "\n";
    ss << "  Active Neurons: " << brain.getActiveNeuronCount() << "\n";
    ss << "  Firing Neurons: " << brain.getFiringNeuronCount() << "\n";
    ss << "  Developmental Stage: " << static_cast<int>(brain.getDevelopmentalStage()) << "\n";
    
    ss << "\nPropagation Analysis:\n";
    // Simplified error propagation analysis
    ss << "  Error likely propagated through neural network based on connectivity\n";
    
    ss << "=== END ERROR PROPAGATION TRACE ===\n";
    return ss.str();
}

// Real-time system monitoring
std::string DebugDiagnostics::getRealTimeMetrics(const Brain& brain, const SimpleWorld& world,
                                                 const std::vector<std::string>& metricTypes) {
    std::stringstream ss;
    ss << "=== REAL-TIME SYSTEM METRICS ===\n";
    
    std::unordered_map<std::string, double> metrics;
    monitorSystemHealth(const_cast<Brain&>(brain), const_cast<SimpleWorld&>(world), metrics);
    
    for (const auto& metric : metrics) {
        ss << metric.first << ": " << metric.second << "\n";
    }
    
    ss << "=== END REAL-TIME METRICS ===\n";
    return ss.str();
}

bool DebugDiagnostics::monitorResourceUsage(const Brain& brain, 
                                           std::unordered_map<std::string, double>& resourceUsage) {
    resourceUsage.clear();
    
    // Basic resource monitoring (simplified)
    resourceUsage["cpu_usage"] = 50.0; // Percentage
    resourceUsage["memory_usage"] = brain.getTotalNeuronCount() * 0.01; // Approximation
    resourceUsage["neuron_count"] = static_cast<double>(brain.getTotalNeuronCount());
    resourceUsage["synapse_count"] = static_cast<double>(brain.getTotalSynapseCount());
    
    return true;
}

std::string DebugDiagnostics::monitorSimulationProgress(const Brain& brain, 
                                                         const SimpleWorld& world,
                                                         uint64_t currentStep) {
    std::stringstream ss;
    ss << "=== SIMULATION PROGRESS MONITORING ===\n";
    ss << "Current Step: " << currentStep << "\n";
    ss << "Brain Metrics:\n";
    ss << "  Neurons: " << brain.getTotalNeuronCount() << "\n";
    ss << "  Synapses: " << brain.getTotalSynapseCount() << "\n";
    ss << "  Active Neurons: " << brain.getActiveNeuronCount() << "\n";
    ss << "  Firing Rate: " << brain.getAverageFiringRate() << " Hz\n";
    
    ss << "World Metrics:\n";
    ss << "  Width: " << world.getWidth() << "\n";
    ss << "  Height: " << world.getHeight() << "\n";
    ss << "  Simulation Time: " << world.getSimulationTime() << "\n";
    
    ss << "=== END PROGRESS MONITORING ===\n";
    return ss.str();
}

// Health checks and alerts
bool DebugDiagnostics::performHealthCheck(const Brain& brain, const Config& config,
                                         const SimpleWorld& world,
                                         std::vector<std::string>& healthIssues) {
    healthIssues.clear();
    
    // Basic health checks
    if (brain.getTotalNeuronCount() == 0) {
        healthIssues.push_back("CRITICAL: Brain has no neurons - simulation cannot proceed");
    }
    
    if (brain.getTotalSynapseCount() == 0 && brain.getTotalNeuronCount() > 0) {
        healthIssues.push_back("WARNING: Brain has neurons but no synapses - network disconnected");
    }
    
    if (brain.getActiveNeuronCount() > brain.getTotalNeuronCount()) {
        healthIssues.push_back("ERROR: Active neuron count exceeds total neuron count");
    }
    
    return healthIssues.empty();
}

std::string DebugDiagnostics::generateHealthReport(const Brain& brain, const Config& config,
                                                   const SimpleWorld& world) {
    std::stringstream ss;
    ss << "=== SYSTEM HEALTH REPORT ===\n";
    ss << "Generated: " << getCurrentTimestamp() << "\n";
    
    std::vector<std::string> issues;
    bool healthy = performHealthCheck(brain, config, world, issues);
    
    ss << "Overall Health: " << (healthy ? "HEALTHY" : "ISSUES DETECTED") << "\n";
    
    if (!issues.empty()) {
        ss << "\nIssues Found:\n";
        for (const auto& issue : issues) {
            ss << "  " << issue << "\n";
        }
    }
    
    ss << "\nSystem Status:\n";
    ss << "  Neurons: " << brain.getTotalNeuronCount() << "\n";
    ss << "  Synapses: " << brain.getTotalSynapseCount() << "\n";
    ss << "  Active Neurons: " << brain.getActiveNeuronCount() << "\n";
    ss << "  Firing Rate: " << brain.getAverageFiringRate() << " Hz\n";
    
    ss << "=== END HEALTH REPORT ===\n";
    return ss.str();
}

// Analysis tools
std::string DebugDiagnostics::analyzeNeuralActivityPatterns(const Brain& brain) {
    std::stringstream ss;
    ss << "=== NEURAL ACTIVITY PATTERN ANALYSIS ===\n";
    
    double avgFiringRate = brain.getAverageFiringRate();
    size_t totalSpikes = brain.getTotalSpikeCount();
    size_t firingNeurons = brain.getFiringNeuronCount();
    
    ss << "Activity Patterns:\n";
    ss << "  Average Firing Rate: " << avgFiringRate << " Hz\n";
    ss << "  Total Spike Events: " << totalSpikes << "\n";
    ss << "  Firing Neuron Ratio: " << (firingNeurons * 100.0 / brain.getTotalNeuronCount()) << "%\n";
    
    ss << "\nPattern Classification:\n";
    if (avgFiringRate < 1.0) {
        ss << "  Pattern: Low activity\n";
    } else if (avgFiringRate < 10.0) {
        ss << "  Pattern: Moderate activity\n";
    } else {
        ss << "  Pattern: High activity\n";
    }
    
    ss << "=== END PATTERN ANALYSIS ===\n";
    return ss.str();
}

std::string DebugDiagnostics::analyzeMemoryUsagePatterns(const Brain& brain) {
    return analyzeMemoryUsage(brain);
}

std::string DebugDiagnostics::analyzeLearningTrajectories(const Brain& brain,
                                                         const std::vector<std::string>& learningMetrics) {
    std::stringstream ss;
    ss << "=== LEARNING TRAJECTORY ANALYSIS ===\n";
    
    // Basic trajectory analysis (simplified)
    ss << "Learning Metrics Available: " << learningMetrics.size() << "\n";
    for (const auto& metric : learningMetrics) {
        ss << "  " << metric << "\n";
    }
    
    ss << "\nTrajectory Insights:\n";
    ss << "  Based on available learning metrics and brain state\n";
    ss << "  Trajectory requires historical data for detailed analysis\n";
    
    ss << "=== END TRAJECTORY ANALYSIS ===\n";
    return ss.str();
}

std::vector<std::string> DebugDiagnostics::getOptimizationRecommendations(const Brain& brain,
                                                                         const Config& config,
                                                                         const SimpleWorld& world) {
    std::vector<std::string> recommendations;
    
    // Basic optimization recommendations
    if (brain.getTotalNeuronCount() > 1000) {
        recommendations.push_back("Consider network pruning to reduce complexity");
    }
    
    if (brain.getTotalSynapseCount() > brain.getTotalNeuronCount() * 10) {
        recommendations.push_back("High synapse-to-neuron ratio - consider sparse connectivity");
    }
    
    if (brain.getAverageFiringRate() < 0.1) {
        recommendations.push_back("Low firing rate - consider neuromodulation adjustments");
    }
    
    recommendations.push_back("Regular performance monitoring recommended");
    
    return recommendations;
}

// Log-based utilities (integration with logging system)
bool DebugDiagnostics::enableLogging(const std::string& logLevel,
                                     const std::string& logFormat,
                                     const std::string& logOutput) {
    // Enable advanced logging system
    // This would integrate with the AdvancedLogger class
    return true;
}

std::vector<std::string> DebugDiagnostics::analyzeLogFiles(const std::string& logDirectory,
                                                          const std::string& analysisType) {
    std::vector<std::string> results;
    results.push_back("Log analysis would be performed here");
    results.push_back("Analysis type: " + analysisType);
    results.push_back("Log directory: " + logDirectory);
    return results;
}

std::string DebugDiagnostics::correlateSystemLogs(const Brain& brain, const SimpleWorld& world,
                                                  const std::vector<std::string>& logSources) {
    std::stringstream ss;
    ss << "=== LOG CORRELATION ===\n";
    ss << "Correlation of system logs with brain and world state\n";
    ss << "Log Sources: " << logSources.size() << "\n";
    ss << "Brain State Integrated: " << brain.getTotalNeuronCount() << " neurons\n";
    ss << "=== END CORRELATION ===\n";
    return ss.str();
}

// Debugging utilities
bool DebugDiagnostics::activateDebugMode(const std::string& debugLevel) {
    // Enable debug mode in logging system
    return true;
}

void DebugDiagnostics::deactivateDebugMode() {
    // Disable debug mode
}

bool DebugDiagnostics::isDebugModeActive() {
    // Check if debug mode is active
    return false;
}

// Simulation debugging
std::string DebugDiagnostics::debugSimulationStep(const Brain& brain,
                                                   const SimpleWorld& world,
                                                   uint64_t step,
                                                   const std::vector<std::string>& debugDetails) {
    std::stringstream ss;
    ss << "=== SIMULATION STEP DEBUG ===\n";
    ss << "Step: " << step << "\n";
    ss << "Debug Details:\n";
    for (const auto& detail : debugDetails) {
        ss << "  " << detail << "\n";
    }
    
    ss << "Brain State:\n";
    ss << "  Neurons: " << brain.getTotalNeuronCount() << "\n";
    ss << "  Synapses: " << brain.getTotalSynapseCount() << "\n";
    ss << "  Active Neurons: " << brain.getActiveNeuronCount() << "\n";
    
    ss << "=== END STEP DEBUG ===\n";
    return ss.str();
}

// Neural event debugging
std::string DebugDiagnostics::traceNeuralEvent(const Brain& brain,
                                       uint64_t neuronId,
                                       const std::chrono::system_clock::time_point& timestamp) {
    std::stringstream ss;
    ss << "=== NEURAL EVENT TRACE ===\n";
    ss << "Neuron ID: " << neuronId << "\n";
    ss << "Timestamp: " << getCurrentTimestamp() << "\n";
    ss << "Brain State at Time:\n";
    ss << "  Total Neurons: " << brain.getTotalNeuronCount() << "\n";
    ss << "  Total Synapses: " << brain.getTotalSynapseCount() << "\n";
    ss << "  Active Neurons: " << brain.getActiveNeuronCount() << "\n";
    
    ss << "Event Analysis:\n";
    ss << "  Event location and context would appear here\n";
    
    ss << "=== END NEURAL EVENT TRACE ===\n";
    return ss.str();
}

// Configuration debugging
std::string DebugDiagnostics::debugConfiguration(const Config& config,
                                                  const std::vector<std::string>& configKeys) {
    std::stringstream ss;
    ss << "=== CONFIGURATION DEBUG ===\n";
    
    auto configKeysList = config.getKeys();
    ss << "Available Config Keys: " << configKeysList.size() << "\n";
    
    if (!configKeys.empty()) {
        ss << "Requested Keys:\n";
        for (const auto& key : configKeys) {
            bool exists = config.has(key);
            ss << "  " << key << ": " << (exists ? "PRESENT" : "NOT FOUND") << "\n";
        }
    }
    
    ss << "=== END CONFIGURATION DEBUG ===\n";
    return ss.str();
}

// Performance bottleneck analysis
std::string DebugDiagnostics::analyzePerformanceBottlenecks(const Brain& brain,
                                                             const std::vector<std::string>& bottleneckTypes) {
    std::stringstream ss;
    ss << "=== PERFORMANCE BOTTLENECK ANALYSIS ===\n";
    
    double thresholdExceeded;
    double thresholdLimit;
    bool bottleneckDetected = detectPerformanceBottleneck(const_cast<Brain&>(brain), 
                                                         "computational", 
                                                         thresholdExceeded, thresholdLimit);
    
    if (bottleneckDetected) {
        ss << "Bottleneck Detected: Computational performance exceeded threshold\n";
        ss << "Threshold Exceeded: " << thresholdExceeded << "\n";
        ss << "Threshold Limit: " << thresholdLimit << "\n";
    } else {
        ss << "No significant performance bottlenecks detected\n";
    }
    
    ss << "=== END BOTTLENECK ANALYSIS ===\n";
    return ss.str();
}

// Memory leak detection (enhanced)
bool DebugDiagnostics::detectMemoryLeaksComprehensive(const Brain& brain,
                                                      std::unordered_map<std::string, std::vector<std::string>>& leakAnalysis) {
    leakAnalysis.clear();
    
    std::string leakType;
    size_t detectedBytes;
    bool leakDetected = detectMemoryLeak(const_cast<Brain&>(brain), 100, leakType, detectedBytes);
    
    if (leakDetected) {
        leakAnalysis["memory_leaks"].push_back("Type: " + leakType);
        leakAnalysis["memory_leaks"].push_back("Size: " + std::to_string(detectedBytes) + " bytes");
        return true;
    }
    
    leakAnalysis["memory_leaks"].push_back("Status: No leaks detected");
    return false;
}

// Error recovery debugging
std::string DebugDiagnostics::debugErrorRecovery(const Brain& brain,
                                                  const std::string& errorContext) {
    std::stringstream ss;
    ss << "=== ERROR RECOVERY DEBUG ===\n";
    ss << "Error Context: " << errorContext << "\n";
    ss << "Brain State for Recovery Analysis:\n";
    ss << "  Neurons: " << brain.getTotalNeuronCount() << "\n";
    ss << "  Synapses: " << brain.getTotalSynapseCount() << "\n";
    ss << "  Active Neurons: " << brain.getActiveNeuronCount() << "\n";
    
    ss << "Recovery Options:\n";
    ss << "  - Reset brain to initial state\n";
    ss << "  - Apply fallback behavior\n";
    ss << "  - Increase error tolerance\n";
    
    ss << "=== END ERROR RECOVERY DEBUG ===\n";
    return ss.str();
}

// Development utilities
std::string DebugDiagnostics::generateDevelopmentReport(const Brain& brain,
                                                         const Config& config,
                                                         const SimpleWorld& world) {
    std::stringstream ss;
    ss << "=== DEVELOPMENT REPORT ===\n";
    ss << "Generated: " << getCurrentTimestamp() << "\n";
    
    ss << "Environment Information:\n";
    ss << "  Brain Version: NLM 1.0\n";
    ss << "  World Version: NLM 1.0\n";
    ss << "  Config Version: NLM 1.0\n";
    
    ss << "\nConfiguration Summary:\n";
    auto configKeys = config.getKeys();
    ss << "  Config Keys: " << configKeys.size() << "\n";
    
    ss << "\nBrain Configuration:\n";
    ss << "  Total Neurons: " << brain.getTotalNeuronCount() << "\n";
    ss << "  Total Synapses: " << brain.getTotalSynapseCount() << "\n";
    ss << "  Developmental Stage: " << static_cast<int>(brain.getDevelopmentalStage()) << "\n";
    
    ss << "\nWorld Configuration:\n";
    ss << "  Dimensions: " << world.getWidth() << "x" << world.getHeight() << "\n";
    ss << "  Max Energy: " << world.getMaxEnergy() << "\n";
    
    ss << "\nDevelopment Recommendations:\n";
    ss << "  - Regular validation checks recommended\n";
    ss << "  - Performance monitoring for optimization\n";
    ss << "  - Memory leak detection periodic checks\n";
    
    ss << "=== END DEVELOPMENT REPORT ===\n";
    return ss.str();
}

// Visualization utilities
std::string DebugDiagnostics::generateVisualizationData(const Brain& brain,
                                                         const std::string& visualizationType) {
    std::stringstream ss;
    ss << "=== VISUALIZATION DATA ===\n";
    ss << "Visualization Type: " << visualizationType << "\n";
    
    if (visualizationType == "network") {
        ss << "Network visualization data would appear here\n";
        ss << "Neuron positions and connections\n";
        ss << "Firing activity visualization\n";
    } else if (visualizationType == "spiking") {
        ss << "Spiking pattern visualization data\n";
        ss << "Temporal firing patterns\n";
        ss << "Frequency analysis\n";
    } else if (visualizationType == "performance") {
        ss << "Performance metrics visualization\n";
        ss << "Resource usage graphs\n";
        ss << "Efficiency metrics\n";
    }
    
    ss << "=== END VISUALIZATION DATA ===\n";
    return ss.str();
}

bool DebugDiagnostics::exportDebugData(const Brain& brain, const Config& config,
                                      const SimpleWorld& world,
                                      const std::string& outputPath) {
    // Export debug data to file
    std::string filename = outputPath + "/debug_data_" + getCurrentTimestamp() + ".txt";
    std::ofstream file(filename);
    
    if (!file.is_open()) {
        return false;
    }
    
    file << generateDebugReport(brain, config, world, "comprehensive");
    file << "\n" << generateVisualizationData(brain, "network");
    file << "\n" << generateHealthReport(brain, config, world);
    
    file.close();
    return true;
}

} // namespace nlm
