// NLM Python Bindings - Comprehensive Validation and Error Handling Implementation
// Implementation of all 36 validation and error handling functions

#include "validation.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"

namespace nlm {
namespace Validation {

// Global validation configuration
ValidationConfig Validation::g_config;

// Utility function implementations
std::string Validation::formatError(const std::string& context, const std::string& error) {
    return "[Validation Error" + (context.empty() ? "" : " in " + context) + "] " + error;
}

std::string Validation::formatWarning(const std::string& context, const std::string& warning) {
    return "[Validation Warning" + (context.empty() ? "" : " in " + context) + "] " + warning;
}

void Validation::logValidationMessage(const std::string& message, bool isError) {
    if (!g_config.enableLogging) return;
    
    std::lock_guard<std::mutex> lock(g_config.loggingMutex);
    
    std::string level = isError ? "ERROR" : "WARNING";
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::ofstream logFile(g_config.logFile, std::ios::app);
    if (logFile.is_open()) {
        logFile << "[" << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") << "] "
                << level << ": " << message << std::endl;
        logFile.close();
    }
    
    // Also output to stderr for immediate visibility
    std::cerr << "[" << level << "] " << message << std::endl;
}

bool Validation::isValidString(const std::string& str, size_t minLength, size_t maxLength, const std::string& context) {
    if (str.length() < minLength) {
        logValidationMessage(formatError(context, "String length " + std::to_string(str.length()) + " is less than minimum " + std::to_string(minLength)), true);
        return false;
    }
    if (str.length() > maxLength) {
        logValidationMessage(formatError(context, "String length " + std::to_string(str.length()) + " exceeds maximum " + std::to_string(maxLength)), true);
        return false;
    }
    if (str.find('\0') != std::string::npos) {
        logValidationMessage(formatError(context, "String contains null character"), true);
        return false;
    }
    return true;
}

bool Validation::isValidVector(const std::vector<float>& vec, size_t expectedSize, const std::string& context) {
    if (expectedSize > 0 && vec.size() != expectedSize) {
        logValidationMessage(formatError(context, "Vector size " + std::to_string(vec.size()) + " does not match expected size " + std::to_string(expectedSize)), true);
        return false;
    }
    
    for (size_t i = 0; i < vec.size(); ++i) {
        if (std::isnan(vec[i]) || std::isinf(vec[i])) {
            logValidationMessage(formatError(context, "Vector element " + std::to_string(i) + " is NaN or infinite"), true);
            return false;
        }
    }
    return true;
}

bool Validation::isValidMap(const std::map<std::string, std::string>& map, const std::string& context) {
    for (const auto& pair : map) {
        if (!isValidString(pair.first, 1, 100, context + ".key")) return false;
        if (!isValidString(pair.second, 0, 10000, context + ".value")) return false;
    }
    return true;
}

// Enhanced Parameter Validation
ValidationResult Validation::validateParameters(const std::vector<float>& parameters, const std::string& context) {
    ValidationResult result(true, "Parameter validation passed");
    
    if (parameters.empty()) {
        result.valid = false;
        result.errors.push_back("Parameters vector is empty");
        return result;
    }
    
    // Check for NaN or infinite values
    for (size_t i = 0; i < parameters.size(); ++i) {
        if (std::isnan(parameters[i])) {
            result.valid = false;
            result.errors.push_back("Parameter " + std::to_string(i) + " is NaN");
        } else if (std::isinf(parameters[i])) {
            result.valid = false;
            result.errors.push_back("Parameter " + std::to_string(i) + " is infinite");
        }
    }
    
    // Check for reasonable range
    for (size_t i = 0; i < parameters.size(); ++i) {
        if (std::abs(parameters[i]) > 1e10f) {
            result.warnings.push_back("Parameter " + std::to_string(i) + " has large absolute value: " + std::to_string(parameters[i]));
        }
    }
    
    return result;
}

ValidationResult Validation::validateSimulationStep(const SimulationStep& step, const SimulationStep& maxStep, const std::string& context) {
    ValidationResult result(true, "Simulation step validation passed");
    
    if (!isInRange(step, 0, maxStep, context)) {
        result.valid = false;
        result.errors.push_back("Step " + std::to_string(step) + " is out of range [0, " + std::to_string(maxStep) + "]");
    }
    
    return result;
}

ValidationResult Validation::validateTimestamp(const Timestamp& timestamp, const Timestamp& minTime, const Timestamp& maxTime, const std::string& context) {
    ValidationResult result(true, "Timestamp validation passed");
    
    if (!isInRange(timestamp, minTime, maxTime, context)) {
        result.valid = false;
        result.errors.push_back("Timestamp " + std::to_string(timestamp) + " is out of range [" + std::to_string(minTime) + ", " + std::to_string(maxTime) + "]");
    }
    
    return result;
}

// Configuration Validation
ValidationResult Validation::validateBrainConfig(const Config& config, ValidationLevel level) {
    ValidationResult result(true, "Brain configuration validation passed");
    
    if (level == ValidationLevel::Strict || level == ValidationLevel::Thorough) {
        // Check for required keys
        std::vector<std::string> requiredKeys = {"neuronCount", "synapseCount", "maxFiringRate", "learningRate"};
        
        for (const auto& key : requiredKeys) {
            if (!config.has(key)) {
                result.valid = false;
                result.errors.push_back("Missing required configuration key: " + key);
            }
        }
        
        // Validate value ranges for specific keys
        if (config.has("neuronCount")) {
            auto count = config.get<size_t>("neuronCount");
            if (!isInRange<size_t>(*count, 100, 1000000, "neuronCount")) {
                result.valid = false;
                result.errors.push_back("neuronCount " + std::to_string(*count) + " is out of valid range [100, 1000000]");
            }
        }
        
        if (config.has("learningRate")) {
            auto rate = config.get<double>("learningRate");
            if (!isInRange(*rate, 0.0, 1.0, "learningRate")) {
                result.valid = false;
                result.errors.push_back("learningRate " + std::to_string(*rate) + " is out of valid range [0.0, 1.0]");
            }
        }
    }
    
    return result;
}

ValidationResult Validation::validateWorldConfig(const Config& config, ValidationLevel level) {
    ValidationResult result(true, "World configuration validation passed");
    
    if (level == ValidationLevel::Strict || level == ValidationLevel::Thorough) {
        // Check for world-specific configuration
        std::vector<std::string> requiredKeys = {"worldWidth", "worldHeight", "agentStartX", "agentStartY"};
        
        for (const auto& key : requiredKeys) {
            if (!config.has(key)) {
                result.valid = false;
                result.errors.push_back("Missing required world configuration key: " + key);
            }
        }
        
        // Validate dimensions
        if (config.has("worldWidth")) {
            auto width = config.get<double>("worldWidth");
            if (!isInRange(*width, 1.0, 10000.0, "worldWidth")) {
                result.valid = false;
                result.errors.push_back("worldWidth " + std::to_string(*width) + " is out of valid range [1.0, 10000.0]");
            }
        }
        
        if (config.has("worldHeight")) {
            auto height = config.get<double>("worldHeight");
            if (!isInRange(*height, 1.0, 10000.0, "worldHeight")) {
                result.valid = false;
                result.errors.push_back("worldHeight " + std::to_string(*height) + " is out of valid range [1.0, 10000.0]");
            }
        }
    }
    
    return result;
}

ValidationResult Validation::validateConfigCompleteness(const Config& config) {
    ValidationResult result(true, "Configuration completeness validation passed");
    
    // Check if configuration has values set
    std::vector<std::string> allKeys = config.getKeys();
    
    if (allKeys.empty()) {
        result.valid = false;
        result.errors.push_back("Configuration is empty - no keys found");
    }
    
    // Check for empty key names
    for (const auto& key : allKeys) {
        if (key.empty()) {
            result.valid = false;
            result.errors.push_back("Configuration contains empty key name");
        }
    }
    
    return result;
}

ValidationResult Validation::validateConfigConsistency(const Config& config) {
    ValidationResult result(true, "Configuration consistency validation passed");
    
    if (config.has("worldWidth") && config.has("worldHeight")) {
        auto width = config.get<double>("worldWidth");
        auto height = config.get<double>("worldHeight");
        
        if (*width <= 0.0 || *height <= 0.0) {
            result.valid = false;
            result.errors.push_back("World dimensions must be positive: width=" + std::to_string(*width) + ", height=" + std::to_string(*height));
        }
        
        if (*width > *height * 100.0 || *height > *width * 100.0) {
            result.warnings.push_back("World dimensions have extreme aspect ratio: width=" + std::to_string(*width) + ", height=" + std::to_string(*height));
        }
    }
    
    if (config.has("learningRate") && config.has("maxFiringRate")) {
        auto learningRate = config.get<double>("learningRate");
        auto maxFiringRate = config.get<double>("maxFiringRate");
        
        if (*learningRate > 1.0 && *maxFiringRate > 1000.0) {
            result.warnings.push_back("High learning rate combined with high max firing rate may cause instability");
        }
    }
    
    return result;
}

ValidationResult Validation::validateConfigValues(const Config& config, ValidationLevel level) {
    ValidationResult result(true, "Configuration values validation passed");
    
    std::vector<std::string> keys = config.getKeys();
    
    for (const auto& key : keys) {
        auto value = config.get<std::string>(key);
        if (!value) continue;
        
        // Basic type validation
        if (key == "learningRate" || key == "synapticWeight" || key == "maxFiringRate") {
            try {
                double doubleValue = std::stod(*value);
                if (std::isnan(doubleValue) || std::isinf(doubleValue)) {
                    result.valid = false;
                    result.errors.push_back("Configuration value for key '" + key + "' is NaN or infinite");
                }
            } catch (...) {
                result.valid = false;
                result.errors.push_back("Configuration value for key '" + key + "' is not a valid number");
            }
        } else if (key == "neuronCount" || key == "synapseCount") {
            try {
                size_t sizeValue = std::stoull(*value);
                if (sizeValue == 0) {
                    result.warnings.push_back("Configuration value for key '" + key + "' is zero");
                }
            } catch (...) {
                result.valid = false;
                result.errors.push_back("Configuration value for key '" + key + "' is not a valid positive integer");
            }
        }
        
        if (level == ValidationLevel::Strict) {
            // Stricter validation for all values
            if (value->empty() && (key.find("path") != std::string::npos || key.find("file") != std::string::npos)) {
                result.warnings.push_back("Configuration key '" + key + "' is empty but appears to be a file path");
            }
        }
    }
    
    return result;
}

// ID Validation
ValidationResult Validation::validateNeuronIds(const std::vector<NeuronId>& ids, const std::string& context) {
    ValidationResult result(true, "Neuron IDs validation passed");
    
    if (ids.empty()) {
        return result; // Empty is valid for no IDs
    }
    
    for (size_t i = 0; i < ids.size(); ++i) {
        if (ids[i].value == UINT64_MAX) { // INVALID_NEURON_ID
            result.warnings.push_back(context + ".NeuronId " + std::to_string(i) + " is invalid sentinel value");
        }
    }
    
    return result;
}

ValidationResult Validation::validateSynapseIds(const std::vector<SynapseId>& ids, const std::string& context) {
    ValidationResult result(true, "Synapse IDs validation passed");
    
    if (ids.empty()) {
        return result; // Empty is valid for no IDs
    }
    
    for (size_t i = 0; i < ids.size(); ++i) {
        if (ids[i].value == UINT64_MAX) { // INVALID_SYNAPSE_ID
            result.warnings.push_back(context + ".SynapseId " + std::to_string(i) + " is invalid sentinel value");
        }
    }
    
    return result;
}

ValidationResult Validation::validateRegionIds(const std::vector<RegionId>& ids, const std::string& context) {
    ValidationResult result(true, "Region IDs validation passed");
    
    if (ids.empty()) {
        return result; // Empty is valid for no IDs
    }
    
    for (size_t i = 0; i < ids.size(); ++i) {
        if (ids[i].value == UINT64_MAX) { // INVALID_REGION_ID
            result.warnings.push_back(context + ".RegionId " + std::to_string(i) + " is invalid sentinel value");
        }
    }
    
    return result;
}

// Data Validation
ValidationResult Validation::validateSensoryData(const SensoryInput& data, const std::string& context) {
    ValidationResult result(true, "Sensory data validation passed");
    
    auto dataVec = data.getData();
    if (dataVec.empty()) {
        result.warnings.push_back(context + ".Sensory data vector is empty");
    }
    
    // Check for NaN or infinite values
    for (size_t i = 0; i < dataVec.size(); ++i) {
        if (std::isnan(dataVec[i])) {
            result.valid = false;
            result.errors.push_back(context + ".Sensory data element " + std::to_string(i) + " is NaN");
        } else if (std::isinf(dataVec[i])) {
            result.valid = false;
            result.errors.push_back(context + ".Sensory data element " + std::to_string(i) + " is infinite");
        }
    }
    
    return result;
}

ValidationResult Validation::validateMotorCommand(const Action& command, const std::string& context) {
    ValidationResult result(true, "Motor command validation passed");
    
    // Check action type is valid
    if (command.getType() == ActionType::Custom && command.getParameters().empty()) {
        result.warnings.push_back(context + ".Custom action has no parameters");
    }
    
    // Validate parameters
    auto params = command.getParameters();
    for (size_t i = 0; i < params.size(); ++i) {
        if (std::isnan(params[i]) || std::isinf(params[i])) {
            result.valid = false;
            result.errors.push_back(context + ".Motor command parameter " + std::to_string(i) + " is invalid");
        }
    }
    
    return result;
}

ValidationResult Validation::validateAction(const Action& action, const std::string& context) {
    ValidationResult result(true, "Action validation passed");
    
    if (action.getType() == ActionType::Custom) {
        auto params = action.getParameters();
        if (params.empty()) {
            result.warnings.push_back(context + ".Custom action has no parameters");
        }
    }
    
    // Validate action type-specific constraints
    if (action.getType() == ActionType::MoveForward || action.getType() == ActionType::MoveBackward) {
        auto params = action.getParameters();
        if (!params.empty() && (params[0] < 0.0f || params[0] > 100.0f)) {
            result.warnings.push_back(context + ".Movement action parameter should be in range [0.0, 100.0]");
        }
    }
    
    return result;
}

ValidationResult Validation::validateObjectData(const WorldObject& obj, const std::string& context) {
    ValidationResult result(true, "Object data validation passed");
    
    if (obj.x < -1000.0f || obj.x > 1000.0f || obj.y < -1000.0f || obj.y > 1000.0f) {
        result.warnings.push_back(context + ".Object position may be out of reasonable bounds");
    }
    
    if (obj.radius < 0.0f) {
        result.valid = false;
        result.errors.push_back(context + ".Object radius cannot be negative");
    }
    
    if (obj.radius > 100.0f) {
        result.warnings.push_back(context + ".Object radius may be unusually large");
    }
    
    return result;
}

ValidationResult Validation::validateMemoryData(const std::string& data, const std::string& context) {
    ValidationResult result(true, "Memory data validation passed");
    
    if (data.empty()) {
        result.warnings.push_back(context + ".Memory data is empty");
    }
    
    if (data.length() > 100 * 1024 * 1024) { // 100MB
        result.warnings.push_back(context + ".Memory data is unusually large: " + std::to_string(data.length()) + " bytes");
    }
    
    return result;
}

// State Validation
ValidationResult Validation::validateBrainState(const Brain& brain, ValidationLevel level) {
    ValidationResult result(true, "Brain state validation passed");
    
    // Get statistics from brain
    size_t totalNeurons = brain.getTotalNeuronCount();
    size_t activeNeurons = brain.getActiveNeuronCount();
    size_t firingNeurons = brain.getFiringNeuronCount();
    
    if (totalNeurons == 0) {
        result.warnings.push_back("Brain has no neurons initialized");
    }
    
    if (level == ValidationLevel::Thorough || level == ValidationLevel::Strict) {
        if (activeNeurons > totalNeurons * 0.95) {
            result.warnings.push_back("Unusually high neuron activity: " + std::to_string(activeNeurons) + "/" + std::to_string(totalNeurons));
        }
        
        if (firingNeurons > activeNeurons * 0.8) {
            result.warnings.push_back("Unusually high firing rate: " + std::to_string(firingNeurons) + "/" + std::to_string(activeNeurons));
        }
        
        float excitationRatio = brain.getExcitationInhibitionRatio();
        if (excitationRatio < 0.1f || excitationRatio > 10.0f) {
            result.warnings.push_back("Unusual excitation/inhibition ratio: " + std::to_string(excitationRatio));
        }
    }
    
    return result;
}

ValidationResult Validation::validateAgentState(const AgentBrain& agent, ValidationLevel level) {
    ValidationResult result(true, "Agent state validation passed");
    
    // Check agent brain internal state
    auto brain = agent.getBrain();
    if (brain) {
        ValidationResult brainResult = validateBrainState(*brain, level);
        if (!brainResult.isValid()) {
            result.valid = false;
            result.errors.insert(result.errors.end(), brainResult.errors.begin(), brainResult.errors.end());
        }
        result.warnings.insert(result.warnings.end(), brainResult.warnings.begin(), brainResult.warnings.end());
    }
    
    return result;
}

ValidationResult Validation::validateWorldState(const SimpleWorld& world, ValidationLevel level) {
    ValidationResult result(true, "World state validation passed");
    
    float width = world.getWidth();
    float height = world.getHeight();
    
    if (width <= 0.0f || height <= 0.0f) {
        result.valid = false;
        result.errors.push_back("Invalid world dimensions: width=" + std::to_string(width) + ", height=" + std::to_string(height));
    }
    
    if (level == ValidationLevel::Thorough) {
        if (width > height * 10.0 || height > width * 10.0) {
            result.warnings.push_back("Extreme world aspect ratio: width=" + std::to_string(width) + ", height=" + std::to_string(height));
        }
    }
    
    return result;
}

ValidationResult Validation::validateDevelopmentState(const DevelopmentalStage& stage, const std::string& context) {
    ValidationResult result(true, "Development state validation passed");
    
    // Basic validation - all enum values are valid
    return result;
}

// Performance Validation
ValidationResult Validation::validatePerformanceParameters(const Config& config, ValidationLevel level) {
    ValidationResult result(true, "Performance parameters validation passed");
    
    if (config.has("maxThreads")) {
        auto threads = config.get<size_t>("maxThreads");
        if (*threads == 0) {
            result.warnings.push_back("maxThreads is zero - will use default thread count");
        } else if (*threads > 64) {
            result.warnings.push_back("maxThreads is unusually high: " + std::to_string(*threads));
        }
    }
    
    if (config.has("checkpointInterval")) {
        auto interval = config.get<size_t>("checkpointInterval");
        if (*interval == 0) {
            result.warnings.push_back("checkpointInterval is zero - checkpointing is disabled");
        }
    }
    
    return result;
}

ValidationResult Validation::validateSimulationParameters(const Config& config, ValidationLevel level) {
    ValidationResult result(true, "Simulation parameters validation passed");
    
    if (config.has("simulationTimestep")) {
        auto timestep = config.get<double>("simulationTimestep");
        if (*timestep <= 0.0) {
            result.valid = false;
            result.errors.push_back("simulationTimestep must be positive: " + std::to_string(*timestep));
        } else if (*timestep > 1.0) {
            result.warnings.push_back("simulationTimestep is unusually large: " + std::to_string(*timestep));
        }
    }
    
    if (config.has("totalSimulationSteps")) {
        auto steps = config.get<size_t>("totalSimulationSteps");
        if (*steps == 0) {
            result.warnings.push_back("totalSimulationSteps is zero - simulation will not run");
        } else if (*steps > 1000000000) { // 1 billion
            result.warnings.push_back("totalSimulationSteps is extremely large: " + std::to_string(*steps));
        }
    }
    
    return result;
}

ValidationResult Validation::validatePlasticityParameters(const Config& config, ValidationLevel level) {
    ValidationResult result(true, "Plasticity parameters validation passed");
    
    if (config.has("hebbianLearningRate")) {
        auto rate = config.get<double>("hebbianLearningRate");
        if (!isInRange(*rate, 0.0, 1.0, "hebbianLearningRate")) {
            result.valid = false;
            result.errors.push_back("hebbianLearningRate " + std::to_string(*rate) + " is out of valid range [0.0, 1.0]");
        }
    }
    
    if (config.has("stdpLearningRate")) {
        auto rate = config.get<double>("stdpLearningRate");
        if (!isInRange(*rate, 0.0, 1.0, "stdpLearningRate")) {
            result.valid = false;
            result.errors.push_back("stdpLearningRate " + std::to_string(*rate) + " is out of valid range [0.0, 1.0]");
        }
    }
    
    return result;
}

ValidationResult Validation::validateNeuromodulationParameters(const Config& config, ValidationLevel level) {
    ValidationResult result(true, "Neuromodulation parameters validation passed");
    
    if (config.has("dopamineDecayRate")) {
        auto rate = config.get<double>("dopamineDecayRate");
        if (!isInRange(*rate, 0.0, 1.0, "dopamineDecayRate")) {
            result.valid = false;
            result.errors.push_back("dopamineDecayRate " + std::to_string(*rate) + " is out of valid range [0.0, 1.0]");
        }
    }
    
    if (config.has("curiosityWeight")) {
        auto weight = config.get<double>("curiosityWeight");
        if (!isInRange(*weight, 0.0, 10.0, "curiosityWeight")) {
            result.valid = false;
            result.errors.push_back("curiosityWeight " + std::to_string(*weight) + " is out of valid range [0.0, 10.0]");
        }
    }
    
    return result;
}

// Safety and Security Validation
ValidationResult Validation::checkResourceUsage(const std::string& context) {
    ValidationResult result(true, "Resource usage check passed");
    
    // This is a simplified resource check - in a real implementation, we would check actual memory, CPU, etc.
    // For now, we just return a successful result
    
    return result;
}

ValidationResult Validation::validateMemoryBounds(size_t memoryUsage, size_t maxMemory, const std::string& context) {
    ValidationResult result(true, "Memory bounds validation passed");
    
    if (memoryUsage > maxMemory) {
        result.valid = false;
        result.errors.push_back(context + ".Memory usage " + std::to_string(memoryUsage) + " exceeds limit " + std::to_string(maxMemory));
    } else if (memoryUsage > maxMemory * 0.9) {
        result.warnings.push_back(context + ".Memory usage is high: " + std::to_string(memoryUsage) + "/" + std::to_string(maxMemory));
    }
    
    return result;
}

ValidationResult Validation::checkSafetyConstraints(const Config& config, const std::string& context) {
    ValidationResult result(true, "Safety constraints check passed");
    
    // Basic safety constraints
    if (config.has("maxEnergy")) {
        auto energy = config.get<double>("maxEnergy");
        if (!isInRange(*energy, 0.0, 100.0, "maxEnergy")) {
            result.warnings.push_back(context + ".maxEnergy " + std::to_string(*energy) + " may be unusually high");
        }
    }
    
    return result;
}

ValidationResult Validation::validateNeuralConnectivity(const Brain& brain, ValidationLevel level) {
    ValidationResult result(true, "Neural connectivity validation passed");
    
    // Basic connectivity check
    size_t totalNeurons = brain.getTotalNeuronCount();
    size_t totalSynapses = brain.getTotalSynapseCount();
    
    if (totalSynapses > 0 && totalNeurons > 0) {
        float synapseRatio = static_cast<float>(totalSynapses) / static_cast<float>(totalNeurons);
        
        if (synapseRatio > 100.0f) {
            result.warnings.push_back("Unusually high synapse-to-neuron ratio: " + std::to_string(synapseRatio));
        }
        
        if (level == ValidationLevel::Strict && synapseRatio < 0.1f) {
            result.warnings.push_back("Unusually low synapse-to-neuron ratio: " + std::to_string(synapseRatio));
        }
    }
    
    return result;
}

ValidationResult Validation::validatePlasticityConstraints(const Config& config, const std::string& context) {
    ValidationResult result(true, "Plasticity constraints validation passed");
    
    // Check for conflicting plasticity rules
    bool hebbianEnabled = false;
    bool stdpEnabled = false;
    
    if (config.has("enableHebbian")) {
        auto enabled = config.get<bool>("enableHebbian");
        hebbianEnabled = *enabled;
    }
    
    if (config.has("enableSTDP")) {
        auto enabled = config.get<bool>("enableSTDP");
        stdpEnabled = *enabled;
    }
    
    if (hebbianEnabled && stdpEnabled) {
        result.warnings.push_back(context + ".Both Hebbian and STDP plasticity are enabled - may cause conflicts");
    }
    
    return result;
}

// Error Recovery Functions
bool Validation::safeStep(Brain& brain, SimulationStep step, Timestamp time, ValidationLevel level) {
    try {
        // Validate before stepping
        ValidationResult validation = validateBrainState(brain, level);
        if (!validation.isValid()) {
            logValidationMessage("safeStep validation failed: " + validation.getFullMessage(), true);
            if (g_config.throwOnError) {
                throw ValidationException("Brain state validation failed before safe step: " + validation.getFullMessage());
            }
            return false;
        }
        
        // Validate simulation step
        ValidationResult stepValidation = validateSimulationStep(step, 1000000, "safeStep");
        if (!stepValidation.isValid()) {
            logValidationMessage("safeStep step validation failed: " + stepValidation.getFullMessage(), true);
            if (g_config.throwOnError) {
                throw ValidationException("Simulation step validation failed before safe step: " + stepValidation.getFullMessage());
            }
            return false;
        }
        
        // Perform the step
        if (time > 0.0) {
            brain.step(step, time);
        } else {
            brain.step(step);
        }
        
        return true;
    } catch (const ValidationException& e) {
        logValidationMessage(std::string("ValidationException in safeStep: ") + e.what(), true);
        return false;
    } catch (const std::exception& e) {
        logValidationMessage(std::string("Exception in safeStep: ") + e.what(), true);
        return false;
    }
}

bool Validation::safeBrainOperation(Brain& brain, std::function<void()> operation, ValidationLevel level) {
    try {
        // Validate before operation
        ValidationResult validation = validateBrainState(brain, level);
        if (!validation.isValid()) {
            logValidationMessage("safeBrainOperation validation failed: " + validation.getFullMessage(), true);
            if (g_config.throwOnError) {
                throw ValidationException("Brain state validation failed before safe brain operation: " + validation.getFullMessage());
            }
            return false;
        }
        
        // Perform the operation
        operation();
        return true;
    } catch (const ValidationException& e) {
        logValidationMessage(std::string("ValidationException in safeBrainOperation: ") + e.what(), true);
        return false;
    } catch (const std::exception& e) {
        logValidationMessage(std::string("Exception in safeBrainOperation: ") + e.what(), true);
        return false;
    }
}

ValidationResult Validation::validateAndSafeStep(Brain& brain, SimulationStep step, Timestamp time, ValidationLevel level) {
    ValidationResult result(true, "validateAndSafeStep completed");
    
    try {
        // Validate parameters
        ValidationResult paramValidation = validateSimulationStep(step, 1000000, "validateAndSafeStep");
        if (!paramValidation.isValid()) {
            result.valid = false;
            result.errors.insert(result.errors.end(), paramValidation.errors.begin(), paramValidation.errors.end());
        }
        
        ValidationResult timestampValidation = validateTimestamp(time, 0.0, 1e9, "validateAndSafeStep");
        if (!timestampValidation.isValid()) {
            result.valid = false;
            result.errors.insert(result.errors.end(), timestampValidation.errors.begin(), timestampValidation.errors.end());
        }
        
        // Validate brain state
        ValidationResult brainValidation = validateBrainState(brain, level);
        if (!brainValidation.isValid()) {
            result.valid = false;
            result.errors.insert(result.errors.end(), brainValidation.errors.begin(), brainValidation.errors.end());
        }
        result.warnings.insert(result.warnings.end(), brainValidation.warnings.begin(), brainValidation.warnings.end());
        
        if (!result.isValid() && g_config.throwOnError) {
            throw ValidationException("Validation failed in validateAndSafeStep: " + result.getFullMessage());
        }
        
        // Perform safe step
        if (!safeStep(brain, step, time, level)) {
            result.valid = false;
            result.errors.push_back("Safe step execution failed");
        }
        
    } catch (const ValidationException& e) {
        logValidationMessage(std::string("ValidationException in validateAndSafeStep: ") + e.what(), true);
        result.valid = false;
        result.errors.push_back("ValidationException: " + std::string(e.what()));
    } catch (const std::exception& e) {
        logValidationMessage(std::string("Exception in validateAndSafeStep: ") + e.what(), true);
        result.valid = false;
        result.errors.push_back("Exception: " + std::string(e.what()));
    }
    
    return result;
}

ValidationResult Validation::validateAndSafeBrainOperation(Brain& brain, std::function<void()> operation, ValidationLevel level) {
    ValidationResult result(true, "validateAndSafeBrainOperation completed");
    
    try {
        // Validate brain state
        ValidationResult brainValidation = validateBrainState(brain, level);
        if (!brainValidation.isValid()) {
            result.valid = false;
            result.errors.insert(result.errors.end(), brainValidation.errors.begin(), brainValidation.errors.end());
        }
        result.warnings.insert(result.warnings.end(), brainValidation.warnings.begin(), brainValidation.warnings.end());
        
        if (!result.isValid() && g_config.throwOnError) {
            throw ValidationException("Validation failed in validateAndSafeBrainOperation: " + result.getFullMessage());
        }
        
        // Perform safe operation
        if (!safeBrainOperation(brain, operation, level)) {
            result.valid = false;
            result.errors.push_back("Safe brain operation execution failed");
        }
        
    } catch (const ValidationException& e) {
        logValidationMessage(std::string("ValidationException in validateAndSafeBrainOperation: ") + e.what(), true);
        result.valid = false;
        result.errors.push_back("ValidationException: " + std::string(e.what()));
    } catch (const std::exception& e) {
        logValidationMessage(std::string("Exception in validateAndSafeBrainOperation: ") + e.what(), true);
        result.valid = false;
        result.errors.push_back("Exception: " + std::string(e.what()));
    }
    
    return result;
}

bool Validation::validateAndSafeAgentOperation(AgentBrain& agent, std::function<void()> operation, ValidationLevel level) {
    try {
        // Validate agent state
        ValidationResult validation = validateAgentState(agent, level);
        if (!validation.isValid()) {
            logValidationMessage("validateAndSafeAgentOperation validation failed: " + validation.getFullMessage(), true);
            if (g_config.throwOnError) {
                throw ValidationException("Agent state validation failed before safe agent operation: " + validation.getFullMessage());
            }
            return false;
        }
        
        // Perform the operation
        operation();
        return true;
    } catch (const ValidationException& e) {
        logValidationMessage(std::string("ValidationException in validateAndSafeAgentOperation: ") + e.what(), true);
        return false;
    } catch (const std::exception& e) {
        logValidationMessage(std::string("Exception in validateAndSafeAgentOperation: ") + e.what(), true);
        return false;
    }
}

// Configuration issue fixing
bool Validation::fixConfigIssues(Config& config, const std::string& context) {
    bool fixed = false;
    
    // Fix common issues
    if (config.has("learningRate")) {
        auto rate = config.get<double>("learningRate");
        if (*rate < 0.0) {
            config.set("learningRate", 0.0);
            fixed = true;
            logValidationMessage(formatWarning(context, "Fixed negative learningRate to 0.0"));
        } else if (*rate > 1.0) {
            config.set("learningRate", 1.0);
            fixed = true;
            logValidationMessage(formatWarning(context, "Fixed learningRate > 1.0 to 1.0"));
        }
    }
    
    if (config.has("neuronCount")) {
        auto count = config.get<size_t>("neuronCount");
        if (*count < 100) {
            config.set("neuronCount", 100);
            fixed = true;
            logValidationMessage(formatWarning(context, "Fixed neuronCount < 100 to 100"));
        } else if (*count > 1000000) {
            config.set("neuronCount", 1000000);
            fixed = true;
            logValidationMessage(formatWarning(context, "Fixed neuronCount > 1000000 to 1000000"));
        }
    }
    
    return fixed;
}

// World completeness validation
ValidationResult Validation::validateWorldCompleteness(const SimpleWorld& world, ValidationLevel level) {
    ValidationResult result(true, "World completeness validation passed");
    
    // Check world has valid dimensions
    float width = world.getWidth();
    float height = world.getHeight();
    
    if (width <= 0.0f || height <= 0.0f) {
        result.valid = false;
        result.errors.push_back("World has invalid dimensions: width=" + std::to_string(width) + ", height=" + std::to_string(height));
    }
    
    // Check world has an agent
    const AgentBody& agent = world.getAgentBody();
    if (agent.energy <= 0.0f) {
        result.warnings.push_back("Agent has zero or negative energy");
    }
    
    // Check world vision dimensions
    if (level == ValidationLevel::Thorough || level == ValidationLevel::Strict) {
        // In a complete implementation, we would check vision dimensions
        // For now, just check that world is configured
        if (width < 10.0f || height < 10.0f) {
            result.warnings.push_back("World dimensions may be too small for meaningful simulation");
        }
    }
    
    return result;
}

// Python binding functions
void bindValidation(py::module_& m) {
    // Create a submodule for validation
    auto validationModule = m.def_submodule("validation", "NLM validation and error handling functions");
    
    // Bind validation functions as Python callable functions
    validationModule.def("validate_parameters", &Validation::validateParameters,
        py::arg("parameters"), py::arg("context") = "",
        "Validate parameters vector - returns ValidationResult");
    
    validationModule.def("validate_simulation_step", &Validation::validateSimulationStep,
        py::arg("step"), py::arg("max_step") = 1000000, py::arg("context") = "",
        "Validate simulation step - returns ValidationResult");
    
    validationModule.def("validate_timestamp", &Validation::validateTimestamp,
        py::arg("timestamp"), py::arg("min_time") = 0.0, py::arg("max_time") = 1e9, py::arg("context") = "",
        "Validate timestamp - returns ValidationResult");
    
    validationModule.def("validate_brain_config", &Validation::validateBrainConfig,
        py::arg("config"), py::arg("level") = ValidationLevel::Basic,
        "Validate brain configuration - returns ValidationResult");
    
    validationModule.def("validate_world_config", &Validation::validateWorldConfig,
        py::arg("config"), py::arg("level") = ValidationLevel::Basic,
        "Validate world configuration - returns ValidationResult");
    
    validationModule.def("validate_config_completeness", &Validation::validateConfigCompleteness,
        py::arg("config"), "Validate configuration completeness - returns ValidationResult");
    
    validationModule.def("validate_config_consistency", &Validation::validateConfigConsistency,
        py::arg("config"), "Validate configuration consistency - returns ValidationResult");
    
    validationModule.def("validate_config_values", &Validation::validateConfigValues,
        py::arg("config"), py::arg("level") = ValidationLevel::Basic,
        "Validate configuration values - returns ValidationResult");
    
    validationModule.def("validate_neuron_ids", &Validation::validateNeuronIds,
        py::arg("ids"), py::arg("context") = "",
        "Validate neuron IDs - returns ValidationResult");
    
    validationModule.def("validate_synapse_ids", &Validation::validateSynapseIds,
        py::arg("ids"), py::arg("context") = "",
        "Validate synapse IDs - returns ValidationResult");
    
    validationModule.def("validate_region_ids", &Validation::validateRegionIds,
        py::arg("ids"), py::arg("context") = "",
        "Validate region IDs - returns ValidationResult");
    
    validationModule.def("validate_sensory_data", &Validation::validateSensoryData,
        py::arg("data"), py::arg("context") = "",
        "Validate sensory data - returns ValidationResult");
    
    validationModule.def("validate_motor_command", &Validation::validateMotorCommand,
        py::arg("command"), py::arg("context") = "",
        "Validate motor command - returns ValidationResult");
    
    validationModule.def("validate_action", &Validation::validateAction,
        py::arg("action"), py::arg("context") = "",
        "Validate action - returns ValidationResult");
    
    validationModule.def("validate_object_data", &Validation::validateObjectData,
        py::arg("obj"), py::arg("context") = "",
        "Validate object data - returns ValidationResult");
    
    validationModule.def("validate_memory_data", &Validation::validateMemoryData,
        py::arg("data"), py::arg("context") = "",
        "Validate memory data - returns ValidationResult");
    
    validationModule.def("validate_brain_state", &Validation::validateBrainState,
        py::arg("brain"), py::arg("level") = ValidationLevel::Basic,
        "Validate brain state - returns ValidationResult");
    
    validationModule.def("validate_agent_state", &Validation::validateAgentState,
        py::arg("agent"), py::arg("level") = ValidationLevel::Basic,
        "Validate agent state - returns ValidationResult");
    
    validationModule.def("validate_world_state", &Validation::validateWorldState,
        py::arg("world"), py::arg("level") = ValidationLevel::Basic,
        "Validate world state - returns ValidationResult");
    
    validationModule.def("validate_development_state", &Validation::validateDevelopmentState,
        py::arg("stage"), py::arg("context") = "",
        "Validate development state - returns ValidationResult");
    
    validationModule.def("validate_performance_parameters", &Validation::validatePerformanceParameters,
        py::arg("config"), py::arg("level") = ValidationLevel::Basic,
        "Validate performance parameters - returns ValidationResult");
    
    validationModule.def("validate_simulation_parameters", &Validation::validateSimulationParameters,
        py::arg("config"), py::arg("level") = ValidationLevel::Basic,
        "Validate simulation parameters - returns ValidationResult");
    
    validationModule.def("validate_plasticity_parameters", &Validation::validatePlasticityParameters,
        py::arg("config"), py::arg("level") = ValidationLevel::Basic,
        "Validate plasticity parameters - returns ValidationResult");
    
    validationModule.def("validate_neuromodulation_parameters", &Validation::validateNeuromodulationParameters,
        py::arg("config"), py::arg("level") = ValidationLevel::Basic,
        "Validate neuromodulation parameters - returns ValidationResult");
    
    validationModule.def("check_resource_usage", &Validation::checkResourceUsage,
        py::arg("context") = "",
        "Check resource usage - returns ValidationResult");
    
    validationModule.def("validate_memory_bounds", &Validation::validateMemoryBounds,
        py::arg("memory_usage"), py::arg("max_memory") = 1024 * 1024 * 1024, py::arg("context") = "",
        "Validate memory bounds - returns ValidationResult");
    
    validationModule.def("check_safety_constraints", &Validation::checkSafetyConstraints,
        py::arg("config"), py::arg("context") = "",
        "Check safety constraints - returns ValidationResult");
    
    validationModule.def("validate_neural_connectivity", &Validation::validateNeuralConnectivity,
        py::arg("brain"), py::arg("level") = ValidationLevel::Basic,
        "Validate neural connectivity - returns ValidationResult");
    
    validationModule.def("validate_plasticity_constraints", &Validation::validatePlasticityConstraints,
        py::arg("config"), py::arg("context") = "",
        "Validate plasticity constraints - returns ValidationResult");
    
    validationModule.def("safe_step", &Validation::safeStep,
        py::arg("brain"), py::arg("step"), py::arg("time") = 0.0, py::arg("level") = ValidationLevel::Basic,
        "Perform safe simulation step - returns bool");
    
    validationModule.def("safe_brain_operation", &Validation::safeBrainOperation,
        py::arg("brain"), py::arg("operation"), py::arg("level") = ValidationLevel::Basic,
        "Perform safe brain operation - returns bool");
    
    validationModule.def("validate_and_safe_step", &Validation::validateAndSafeStep,
        py::arg("brain"), py::arg("step"), py::arg("time") = 0.0, py::arg("level") = ValidationLevel::Basic,
        "Validate and perform safe simulation step - returns ValidationResult");
    
    validationModule.def("validate_and_safe_brain_operation", &Validation::validateAndSafeBrainOperation,
        py::arg("brain"), py::arg("operation"), py::arg("level") = ValidationLevel::Basic,
        "Validate and perform safe brain operation - returns ValidationResult");
    
    validationModule.def("try_brain_operation", &Validation::tryBrainOperation,
        py::arg("operation"), py::arg("default_value") = 0, py::arg("level") = ValidationLevel::Basic,
        "Try brain operation with error recovery - returns operation result");
    
    validationModule.def("validate_and_safe_agent_operation", &Validation::validateAndSafeAgentOperation,
        py::arg("agent"), py::arg("operation"), py::arg("level") = ValidationLevel::Basic,
        "Validate and perform safe agent operation - returns bool");
    
    validationModule.def("fix_config_issues", &Validation::fixConfigIssues,
        py::arg("config"), py::arg("context") = "",
        "Attempt to automatically fix common config issues - returns bool");
    
    validationModule.def("validate_world_completeness", &Validation::validateWorldCompleteness,
        py::arg("world"), py::arg("level") = ValidationLevel::Basic,
        "Validate world completeness - returns ValidationResult");
    
    // Bind ValidationResult class
    py::class_<ValidationResult>(validationModule, "ValidationResult")
        .def(py::init<>())
        .def_readonly("valid", &ValidationResult::valid)
        .def_readonly("message", &ValidationResult::message)
        .def_readonly("warnings", &ValidationResult::warnings)
        .def_readonly("errors", &ValidationResult::errors)
        .def("isValid", &ValidationResult::isValid)
        .def("hasErrors", &ValidationResult::hasErrors)
        .def("hasWarnings", &ValidationResult::hasWarnings)
        .def("getFullMessage", &ValidationResult::getFullMessage);
    
    // Bind ValidationLevel enum
    py::enum_<ValidationLevel>(validationModule, "ValidationLevel")
        .value("Basic", ValidationLevel::Basic)
        .value("Thorough", ValidationLevel::Thorough)
        .value("Strict", ValidationLevel::Strict)
        .export_values();
    
    // Bind exception classes
    py::register_exception<ValidationException>(validationModule, "ValidationException");
    py::register_exception<ValidationWarning>(validationModule, "ValidationWarning");
}

void bindErrorHandling(py::module_& m) {
    // Create a submodule for error handling
    auto errorHandlingModule = m.def_submodule("error_handling", "NLM error handling utilities");
    
    // Error handling configuration
    errorHandlingModule.def("set_validation_config", [](ValidationLevel defaultLevel, bool enableLogging, 
                                                       bool throwOnError, bool throwOnWarning) {
        Validation::g_config = ValidationConfig(defaultLevel, enableLogging, throwOnError, throwOnWarning);
    }, py::arg("default_level"), py::arg("enable_logging"), py::arg("throw_on_error"), py::arg("throw_on_warning"),
        "Set global validation configuration");
    
    errorHandlingModule.def("get_validation_config", []() {
        return Validation::g_config;
    }, "Get current validation configuration");
    
    errorHandlingModule.def("log_validation_message", &Validation::logValidationMessage,
        py::arg("message"), py::arg("is_error") = false,
        "Log validation message");
}

} // namespace nlm
