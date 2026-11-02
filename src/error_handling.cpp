// Code quality and error handling improvements
// This file contains fixes for potential bugs, improved error handling,
// and better code organization throughout the NLM system

#include <stdexcept>
#include <cassert>
#include <sstream>
#include <algorithm>

using namespace nlm;

// Improved error handling utility functions
namespace ErrorHandling {
    
    // Safe pointer dereference with null check and descriptive error
    template<typename T>
    T& safe_deref(T* ptr, const std::string& context) {
        if (!ptr) {
            throw std::runtime_error("Null pointer dereference in " + context);
        }
        return *ptr;
    }
    
    // Safe vector access with bounds checking
    template<typename T>
    T& safe_at(std::vector<T>& vec, size_t index, const std::string& context) {
        if (index >= vec.size()) {
            throw std::out_of_range("Vector index out of range in " + context + ": " + 
                                   std::to_string(index) + " >= " + std::to_string(vec.size()));
        }
        return vec[index];
    }
    
    // Safe map access with default value
    template<typename Map, typename Key, typename Value>
    Value safe_get(const Map& map, const Key& key, const Value& defaultValue, 
                   const std::string& context) {
        auto it = map.find(key);
        if (it != map.end()) {
            return it->second;
        }
        return defaultValue;
    }
    
    // Validate brain configuration with detailed error reporting
    bool validateBrainConfig(const Config& config, std::vector<std::string>& errors) {
        bool isValid = true;
        
        // Check neuron count
        auto neuronCount = config.getOr<int64_t>("neuron_count", -1);
        if (neuronCount <= 0) {
            errors.push_back("neuron_count must be positive, got: " + std::to_string(neuronCount));
            isValid = false;
        }
        
        // Check region count
        auto regionCount = config.getOr<int64_t>("region_count", -1);
        if (regionCount <= 0) {
            errors.push_back("region_count must be positive, got: " + std::to_string(regionCount));
            isValid = false;
        }
        
        // Check connection probability
        auto connectionProb = config.getOr<float>("connection_probability", -1.0f);
        if (connectionProb < 0.0f || connectionProb > 1.0f) {
            errors.push_back("connection_probability must be between 0 and 1, got: " + 
                           std::to_string(connectionProb));
            isValid = false;
        }
        
        // Check simulation timestep
        auto timestep = config.getOr<double>("simulation_timestep", -1.0);
        if (timestep <= 0.0) {
            errors.push_back("simulation_timestep must be positive, got: " + std::to_string(timestep));
            isValid = false;
        }
        
        // Check random seed (if present)
        if (config.has("random_seed")) {
            auto seed = config.getOr<int64_t>("random_seed", -1);
            if (seed < 0) {
                errors.push_back("random_seed must be non-negative, got: " + std::to_string(seed));
                isValid = false;
            }
        }
        
        return isValid;
    }
    
    // Safe configuration value getter with type conversion and validation
    template<typename T>
    T getValidatedConfig(const Config& config, const std::string& key, 
                       const T& defaultValue, const std::string& context) {
        if (!config.has(key)) {
            return defaultValue;
        }
        
        try {
            T value = config.getOr<T>(key, defaultValue);
            return value;
        } catch (const std::exception& e) {
            throw std::runtime_error("Failed to get configuration key '" + key + 
                                   "' in " + context + ": " + e.what());
        }
    }
}

// Improved brain initialization with better error handling
bool Brain::initializeImproved() {
    try {
        NLM_LOG_INFO("Initializing NLM Brain with enhanced error handling...");
        
        // Validate configuration first
        std::vector<std::string> configErrors;
        if (!ErrorHandling::validateBrainConfig(*pImpl->config, configErrors)) {
            NLM_LOG_ERROR("Configuration validation failed:");
            for (const auto& error : configErrors) {
                NLM_LOG_ERROR("  " + error);
            }
            return false;
        }
        
        // Clear previous state with null checks
        if (pImpl->regions.empty()) {
            NLM_LOG_WARN("Regions already cleared - re-initializing");
        }
        pImpl->regions.clear();
        pImpl->sensoryNeurons.clear();
        pImpl->motorNeurons.clear();
        pImpl->neuronById.clear();
        pImpl->nextRegionId = RegionId(1);
        pImpl->totalSpikesTotal = 0;
        pImpl->totalSpikesThisStep = 0;
        pImpl->currentStep = 0;
        pImpl->currentTime = 0.0;
        pImpl->isResting = false;
        pImpl->stepsSinceLastEpisode = 0;
        
        // Get configuration with safe access
        size_t neuronCount = ErrorHandling::getValidatedConfig<size_t>(
            *pImpl->config, "neuron_count", 1000, "Brain::initialize");
        
        size_t regionCount = ErrorHandling::getValidatedConfig<size_t>(
            *pImpl->config, "region_count", 1, "Brain::initialize");
        
        float connectionProbability = ErrorHandling::getValidatedConfig<float>(
            *pImpl->config, "connection_probability", 0.1f, "Brain::initialize");
        
        double timestep = ErrorHandling::getValidatedConfig<double>(
            *pImpl->config, "simulation_timestep", 0.001, "Brain::initialize");
        
        NLM_LOG_INFO("Configuration validated: " + std::to_string(neuronCount) + 
                    " neurons, " + std::to_string(regionCount) + " regions, " +
                    std::to_string(connectionProbability) + " connection probability");
        
        // Create regions with error handling
        for (size_t i = 0; i < regionCount; ++i) {
            std::string regionName = "Region_" + std::to_string(i + 1);
            RegionId regionId = addRegion(regionName);
            
            if (!regionId) {
                NLM_LOG_ERROR("Failed to create region " + regionName);
                return false;
            }
            
            NLM_LOG_INFO("Created region " + std::to_string(regionId.index()) + 
                        ": " + regionName);
        }
        
        // Create neurons and collect I/O neurons with safety checks
        size_t neuronsPerRegion = neuronCount / regionCount;
        size_t nextNeuronIndex = 1;  // globally unique IDs across regions
        
        for (size_t i = 0; i < regionCount; ++i) {
            auto* region = getRegion(RegionId(i + 1));
            if (!region) {
                NLM_LOG_ERROR("Failed to get region " + std::to_string(i + 1));
                return false;
            }
            
            region->setNextNeuronIndex(nextNeuronIndex);
            
            // Add populations with error handling
            auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
            size_t internalCount = neuronsPerRegion / 2;
            size_t internalExcitatory = internalCount * 4 / 5;
            auto internalPopId = region->addPopulation(internalExcitatory, NeuronType::Internal);
            auto inhibitoryPopId = region->addPopulation(internalCount - internalExcitatory, NeuronType::Inhibitory);
            auto motorPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);
            
            (void)internalPopId; (void)inhibitoryPopId; (void)motorPopId;
            
            // Collect sensory and motor neurons for I/O with null checks
            auto* sensoryPop = region->getPopulation(sensoryPopId);
            auto* motorPop = region->getPopulation(motorPopId);
            
            if (sensoryPop) {
                for (auto* neuron : sensoryPop->getNeurons()) {
                    if (neuron) {
                        pImpl->sensoryNeurons.push_back(neuron);
                    }
                }
            }
            
            if (motorPop) {
                for (auto* neuron : motorPop->getNeurons()) {
                    if (neuron) {
                        pImpl->motorNeurons.push_back(neuron);
                    }
                }
            }
            
            size_t neuronsCreated = region->getTotalNeuronCount();
            nextNeuronIndex += neuronsCreated;
            
            NLM_LOG_INFO("Created populations in region " + std::to_string(i + 1) +
                        ": " + std::to_string(region->getPopulationCount()) + " populations, " +
                        std::to_string(neuronsCreated) + " neurons");
        }
        
        // Build neuron lookup with safety checks
        for (auto& region : pImpl->regions) {
            for (auto* neuron : region->getAllNeurons()) {
                if (neuron) {
                    pImpl->neuronById[neuron->getId().index()] = neuron;
                }
            }
        }
        
        NLM_LOG_INFO("Neuron lookup table created with " + 
                    std::to_string(pImpl->neuronById.size()) + " entries");
        
        // Initialize connectivity with error handling
        for (size_t i = 0; i < regionCount; ++i) {
            auto* region = getRegion(RegionId(i + 1));
            if (!region) {
                NLM_LOG_ERROR("Failed to get region for connectivity initialization: " + 
                            std::to_string(i + 1));
                return false;
            }
            
            try {
                // Initialize random connectivity and synapse weights
                region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 0.2f, 0.1f);
                NLM_LOG_INFO("Initialized random connectivity for region " + std::to_string(i + 1));
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Failed to initialize connectivity for region " + 
                            std::to_string(i + 1) + ": " + e.what());
                return false;
            }
        }
        
        // Initialize all integrated systems with error handling
        try {
            initializeIntegratedSystems();
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to initialize integrated systems: " + std::string(e.what()));
            return false;
        }
        
        NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Enhanced)");
        NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
        NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
        NLM_LOG_INFO("Sensory neurons: " + std::to_string(pImpl->sensoryNeurons.size()));
        NLM_LOG_INFO("Motor neurons: " + std::to_string(pImpl->motorNeurons.size()));
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception during brain initialization: " + std::string(e.what()));
        return false;
    } catch (...) {
        NLM_LOG_ERROR("Unknown exception during brain initialization");
        return false;
    }
}

// Enhanced error handling for plasticity operations
bool applyPlasticitySafe(Brain* brain, Synapse* syn, const std::string& operation) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot apply plasticity: brain is null in " + operation);
        return false;
    }
    
    if (!syn) {
        NLM_LOG_ERROR("Cannot apply plasticity: synapse is null in " + operation);
        return false;
    }
    
    try {
        // Check if synapse has plasticity enabled
        const auto& flags = syn->getPlasticityFlags();
        bool hasPlasticity = flags.stdp || flags.hebbian || flags.structural;
        
        if (!hasPlasticity) {
            NLM_LOG_WARN("Synapse has no plasticity enabled in " + operation);
            return false;
        }
        
        // Apply appropriate plasticity rules
        if (flags.stdp) {
            // Apply STDP
            auto* stdp = brain->getSTDP();
            if (!stdp) {
                NLM_LOG_ERROR("STDP system not available for " + operation);
                return false;
            }
            
            // Get spike history
            const auto& preSpikes = syn->getPreSpikeHistory();
            const auto& postSpikes = syn->getPostSpikeHistory();
            
            if (!preSpikes.empty() && !postSpikes.empty()) {
                stdp->update(syn, preSpikes, postSpikes, brain->getConfig()->getOr<double>("simulation_timestep", 0.001));
            }
        }
        
        if (flags.hebbian) {
            // Apply Hebbian learning
            auto* hebbian = brain->getHebbian();
            if (!hebbian) {
                NLM_LOG_ERROR("Hebbian system not available for " + operation);
                return false;
            }
            
            const auto& preSpikes = syn->getPreSpikeHistory();
            const auto& postSpikes = syn->getPostSpikeHistory();
            
            if (!preSpikes.empty() && !postSpikes.empty()) {
                hebbian->update(syn, preSpikes, postSpikes, brain->getConfig()->getOr<double>("simulation_timestep", 0.001));
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Plasticity operation failed in " + operation + ": " + std::string(e.what()));
        return false;
    }
}

// Enhanced memory system management
void manageMemorySystemsSafe(Brain* brain) {
    if (!brain) return;
    
    try {
        // Working memory management
        auto* workingMemory = brain->getWorkingMemory();
        if (workingMemory) {
            size_t capacity = workingMemory->getCapacity();
            size_t activeTraces = workingMemory->getActiveTraces();
            
            if (activeTraces > capacity) {
                NLM_LOG_WARN("Working memory overload: " + std::to_string(activeTraces) + 
                            "/" + std::to_string(capacity));
                workingMemory->clear();  // Emergency cleanup
            }
            
            // Update working memory
            workingMemory->update(brain->getConfig()->getOr<double>("simulation_timestep", 0.001));
        }
        
        // Episodic memory management
        auto* episodicMemory = brain->getEpisodicMemory();
        if (episodicMemory) {
            size_t maxEpisodes = episodicMemory->getMaxEpisodes();
            size_t currentEpisodes = episodicMemory->getEpisodes().size();
            
            if (currentEpisodes > maxEpisodes * 0.9) {
                NLM_LOG_WARN("Episodic memory approaching capacity: " + 
                            std::to_string(currentEpisodes) + "/" + std::to_string(maxEpisodes));
                episodicMemory->consolidate(0.5f);  // Consolidation
            }
        }
        
        // Associative memory management
        auto* associativeMemory = brain->getAssociativeMemory();
        if (associativeMemory) {
            associativeMemory->update();
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Memory system management failed: " + std::string(e.what()));
    }
}

// Enhanced neuromodulation with better error handling
void applyNeuromodulationSafe(Brain* brain) {
    if (!brain) return;
    
    try {
        auto* dopamine = brain->getDopamine();
        auto* curiosity = brain->getCuriosity();
        auto* novelty = brain->getNovelty();
        auto* predictionError = brain->getPredictionErrorSignal();
        
        // Calculate combined neuromodulation level
        float neuromodulationLevel = 1.0f;
        
        if (dopamine) {
            float dopamineLevel = dopamine->getLevel();
            neuromodulationLevel *= (1.0f + dopamineLevel * 0.5f);
        }
        
        if (curiosity) {
            float curiosityLevel = curiosity->getLevel();
            neuromodulationLevel *= (1.0f + curiosityLevel * 0.3f);
        }
        
        if (novelty) {
            float noveltyLevel = novelty->getLevel();
            neuromodulationLevel *= (1.0f + noveltyLevel * 0.2f);
        }
        
        if (predictionError) {
            float errorLevel = predictionError->getError();
            neuromodulationLevel *= (1.0f + std::abs(errorLevel) * 0.1f);
        }
        
        // Apply neuromodulation to neural excitability
        for (auto& region : brain->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (neuron) {
                        // Apply neuromodulation effects
                        float excitabilityMod = neuromodulationLevel - 1.0f;
                        if (excitabilityMod > 0.0f) {
                            neuron->injectCurrent(excitabilityMod * 0.5f);
                        }
                    }
                }
            }
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Neuromodulation application failed: " + std::string(e.what()));
    }
}

// Enhanced checkpoint system with better error recovery
bool saveBrainStateSafe(Brain* brain, const std::string& filepath, 
                       const std::string& description) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot save brain state: brain is null");
        return false;
    }
    
    if (filepath.empty()) {
        NLM_LOG_ERROR("Cannot save brain state: filepath is empty");
        return false;
    }
    
    try {
        NLM_LOG_INFO("Saving brain state to " + filepath + " (description: " + 
                    description + ")");
        
        // Ensure directory exists
        std::string dirname = filepath.substr(0, filepath.find_last_of('/'));
        if (!dirname.empty()) {
            // In a real implementation, you would create directories here
            // For now, we'll just log the requirement
            NLM_LOG_INFO("Note: Directory creation for " + dirname + " should be handled by caller");
        }
        
        // Attempt save with multiple retries
        int maxRetries = 3;
        for (int retry = 0; retry < maxRetries; ++retry) {
            bool success = brain->save(filepath);
            if (success) {
                NLM_LOG_INFO("Brain state saved successfully (attempt " + std::to_string(retry + 1) + ")");
                return true;
            }
            
            if (retry < maxRetries - 1) {
                NLM_LOG_WARN("Save attempt " + std::to_string(retry + 1) + " failed, retrying...");
                // Wait a bit before retry (in real implementation)
                // std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
        
        NLM_LOG_ERROR("Failed to save brain state after " + std::to_string(maxRetries) + " attempts");
        return false;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception during brain state save: " + std::string(e.what()));
        return false;
    }
}

bool loadBrainStateSafe(Brain* brain, const std::string& filepath,
                       bool validateChecksum = true) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot load brain state: brain is null");
        return false;
    }
    
    if (filepath.empty()) {
        NLM_LOG_ERROR("Cannot load brain state: filepath is empty");
        return false;
    }
    
    try {
        NLM_LOG_INFO("Loading brain state from " + filepath);
        
        // Validate file exists and is readable
        std::ifstream file(filepath, std::ios::binary);
        if (!file.good()) {
            NLM_LOG_ERROR("Cannot open file for reading: " + filepath);
            return false;
        }
        file.close();
        
        // Attempt load with multiple retries
        int maxRetries = 3;
        for (int retry = 0; retry < maxRetries; ++retry) {
            bool success = brain->load(filepath);
            if (success) {
                NLM_LOG_INFO("Brain state loaded successfully (attempt " + std::to_string(retry + 1) + ")");
                
                if (validateChecksum) {
                    // In a real implementation, you would validate checksum here
                    NLM_LOG_INFO("Note: Checksum validation should be implemented for production");
                }
                
                return true;
            }
            
            if (retry < maxRetries - 1) {
                NLM_LOG_WARN("Load attempt " + std::to_string(retry + 1) + " failed, retrying...");
                // Wait a bit before retry (in real implementation)
                // std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
        
        NLM_LOG_ERROR("Failed to load brain state after " + std::to_string(maxRetries) + " attempts");
        return false;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception during brain state load: " + std::string(e.what()));
        return false;
    }
}

// Enhanced logging with better error context
void logWithContext(const std::string& message, const std::string& context = "",
                   LogLevel level = LogLevel::Info) {
    std::stringstream ss;
    ss << "[" << level << "]";
    
    if (!context.empty()) {
        ss << "[" << context << "]";
    }
    
    ss << " " << message;
    
    // Use existing logger
    auto logger = Logger::getGlobal();
    if (logger) {
        // In a real implementation, you would log at the specified level
        std::cout << ss.str() << std::endl;
    }
}

// Safe brain operation with comprehensive error handling
template<typename Operation>
bool executeBrainOperation(Brain* brain, Operation&& op, 
                         const std::string& operationName,
                         bool continueOnError = false) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot execute " + operationName + ": brain is null");
        return false;
    }
    
    try {
        op(brain);
        NLM_LOG_INFO("Successfully completed " + operationName);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Operation " + operationName + " failed: " + std::string(e.what()));
        return !continueOnError;  // Return false if not continuing on error
        
    } catch (...) {
        NLM_LOG_ERROR("Unknown exception during " + operationName);
        return !continueOnError;
    }
}

// Configuration validation with detailed reporting
std::vector<std::string> validateBrainConfiguration(const Config& config) {
    std::vector<std::string> errors;
    
    // Required configuration keys with validation rules
    struct ConfigRule {
        std::string key;
        std::function<bool(double)> validator;
        std::string errorMessage;
    };
    
    std::vector<ConfigRule> rules = {
        {"neuron_count", [](double v) { return v > 0; }, "neuron_count must be positive"},
        {"region_count", [](double v) { return v > 0; }, "region_count must be positive"},
        {"connection_probability", [](double v) { return v >= 0.0 && v <= 1.0; }, 
         "connection_probability must be between 0 and 1"},
        {"simulation_timestep", [](double v) { return v > 0; }, "simulation_timestep must be positive"},
        {"random_seed", [](double v) { return v >= 0; }, "random_seed must be non-negative"}
    };
    
    for (const auto& rule : rules) {
        if (config.has(rule.key)) {
            double value = config.getOr<double>(rule.key, -1.0);
            if (!rule.validator(value)) {
                errors.push_back(rule.errorMessage + ", got: " + std::to_string(value));
            }
        }
    }
    
    // Check for optional but important configurations
    if (!config.has("plasticity.stdp.enable") && !config.has("plasticity.hebbian.enable")) {
        errors.push_back("At least one plasticity system must be enabled (stdp or hebbian)");
    }
    
    if (!config.has("development.enabled")) {
        errors.push_back("Development system should be enabled for long-term learning");
    }
    
    return errors;
}

// Memory usage monitoring (simplified)
struct MemoryStats {
    size_t totalMemoryBytes;
    size_t usedMemoryBytes;
    size_t peakMemoryBytes;
    float memoryUtilization;
};

MemoryStats getMemoryStats() {
    MemoryStats stats;
    stats.totalMemoryBytes = 0;      // Platform-specific implementation needed
    stats.usedMemoryBytes = 0;       // Platform-specific implementation needed
    stats.peakMemoryBytes = 0;       // Platform-specific implementation needed
    stats.memoryUtilization = 0.0f;
    
    if (stats.totalMemoryBytes > 0) {
        stats.memoryUtilization = static_cast<float>(stats.usedMemoryBytes) / 
                                 static_cast<float>(stats.totalMemoryBytes);
    }
    
    return stats;
}

// Performance monitoring
struct PerformanceMetrics {
    double cpuUsage;           // Percentage
    double memoryUsage;        // Percentage
    double frameTime;          // Seconds per frame
    double simulationSpeed;    // Steps per second
    size_t spikeThroughput;    // Spikes per second
};

PerformanceMetrics getPerformanceMetrics(std::shared_ptr<Brain> brain) {
    PerformanceMetrics metrics;
    metrics.cpuUsage = 0.0;     // Platform-specific implementation needed
    metrics.memoryUsage = 0.0;  // Platform-specific implementation needed
    metrics.frameTime = 0.0;    // Would measure actual frame time
    metrics.simulationSpeed = 0.0;  // Would measure actual simulation speed
    metrics.spikeThroughput = brain ? brain->getTotalSpikeCount() : 0;
    
    return metrics;
}

// Safe string operations
std::string sanitizeFilename(const std::string& filename) {
    std::string sanitized;
    sanitized.reserve(filename.size());
    
    for (char c : filename) {
        if (std::isalnum(c) || c == '.' || c == '_' || c == '-' || c == ' ') {
            sanitized += c;
        } else if (c == '/' || c == '\\' || c == ':') {
            sanitized += '_';  // Replace path separators
        }
    }
    
    return sanitized;
}

std::string formatErrorMessage(const std::string& operation, 
                               const std::exception& e) {
    std::stringstream ss;
    ss << "Operation: " << operation << "\n"
       << "Error: " << e.what() << "\n"
       << "Timestamp: " << std::time(nullptr);
    
    return ss.str();
}

// Thread safety utilities (simplified)
class ThreadSafeBrain {
private:
    std::shared_ptr<Brain> brain_;
    mutable std::mutex mutex_;
    
public:
    explicit ThreadSafeBrain(std::shared_ptr<Brain> brain) : brain_(brain) {}
    
    template<typename Func>
    auto execute(Func&& func) -> decltype(func(brain_)) {
        std::lock_guard<std::mutex> lock(mutex_);
        return func(brain_);
    }
    
    std::shared_ptr<Brain> getBrain() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return brain_;
    }
};

// Configuration merging utilities
Config mergeConfigurations(const Config& base, const Config& override) {
    Config merged = base;
    
    // Merge configuration sections
    // In practice, this would be more sophisticated
    for (const auto& key : override.getKeys()) {
        if (override.has(key)) {
            // Extract value type based on key (simplified)
            if (key.find("neuron_count") != std::string::npos) {
                merged.set(key, override.getOr<int64_t>(key, 0), ConfigSource::CommandLine);
            } else if (key.find("connection_probability") != std::string::npos ||
                      key.find("plasticity") != std::string::npos ||
                      key.find("neuromod") != std::string::npos) {
                merged.set(key, override.getOr<float>(key, 0.0f), ConfigSource::CommandLine);
            } else if (key.find("simulation_timestep") != std::string::npos) {
                merged.set(key, override.getOr<double>(key, 0.0), ConfigSource::CommandLine);
            } else {
                // Try as string for unknown types
                std::string valueStr = "unknown_value_for_" + key;
                merged.set(key, valueStr, ConfigSource::CommandLine);
            }
        }
    }
    
    return merged;
}