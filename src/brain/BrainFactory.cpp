// Brain Factory Implementation
// Phase 6: Integrated Brain System Factory

#include "BrainFactory.hpp"
#include "core/config/Constants.hpp"
#include <iostream>
#include <stdexcept>
#include <algorithm>

namespace nlm {

// Helper function to get error message for system initialization failure
static std::string getSystemInitializationError(const std::string& systemName, const std::exception& e) {
    return "Failed to initialize " + systemName + ": " + std::string(e.what());
}

// Factory configuration validation
bool BrainFactory::validateConfig(const BrainFactoryConfig& config) {
    if (!config.config) {
        throw std::invalid_argument("Config cannot be null");
    }
    
    // Validate configuration values
    auto neuronCount = config.config->getOr<size_t>("neuron_count", config::DEFAULT_NEURON_COUNT);
    if (neuronCount == 0) {
        throw std::invalid_argument("Neuron count must be greater than 0");
    }
    
    auto regionCount = config.config->getOr<size_t>("region_count", config::DEFAULT_REGION_COUNT);
    if (regionCount == 0) {
        throw std::invalid_argument("Region count must be greater than 0");
    }
    
    auto connectionProb = config.config->getOr<float>("connection_probability", config::DEFAULT_CONNECTION_PROBABILITY);
    if (connectionProb < 0.0f || connectionProb > 1.0f) {
        throw std::invalid_argument("Connection probability must be between 0.0 and 1.0");
    }
    
    auto timestep = config.config->getOr<double>("simulation_timestep", config::SIMULATION_TIMESTEP);
    if (timestep <= 0.0) {
        throw std::invalid_argument("Simulation timestep must be positive");
    }
    
    return true;
}

// Helper function to safely initialize a system
template<typename SystemType, typename InitFunc>
void safeInitializeSystem(
    const std::string& systemName,
    std::unique_ptr<SystemType>& systemPtr,
    InitFunc initFunc
) {
    try {
        initFunc();
        if (!systemPtr) {
            throw std::runtime_error("System initialization failed: system pointer is null");
        }
        std::cout << "    ✓ " << systemName << " initialized successfully" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "    ✗ " << systemName << " initialization failed: " << e.what() << std::endl;
        throw;
    }
}

// Convenience function to create a basic integrated brain
std::shared_ptr<Brain> createBasicIntegratedBrain(std::shared_ptr<Config> config) {
    BrainFactoryConfig factoryConfig = BrainFactory::createConfigFromMainConfig(config);
    return BrainFactory::createIntegratedBrain(factoryConfig);
}

// Convenience function to create an integrated brain with custom options
std::shared_ptr<Brain> createIntegratedBrain(
    std::shared_ptr<Config> config,
    const BrainFactoryConfig::SystemOptions& options
) {
    BrainFactoryConfig factoryConfig(config);
    factoryConfig.options = options;
    return BrainFactory::createIntegratedBrain(factoryConfig);
}

// Helper function to extract configuration with fallbacks to constants
void applyConfigConstants(std::shared_ptr<Config> config) {
    // Apply configuration constants for consistency
    config->set("random_seed", static_cast<int64_t>(config::DEFAULT_RANDOM_SEED), ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(config::DEFAULT_NEURON_COUNT), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(config::DEFAULT_REGION_COUNT), ConfigSource::Default);
    config->set("connection_probability", config::DEFAULT_CONNECTION_PROBABILITY, ConfigSource::Default);
    config->set("stdp_ltp_weight", config::DEFAULT_STDP_LTP_WEIGHT, ConfigSource::Default);
    config->set("stdp_ltd_weight", config->getOr<float>("stdp_ltd_weight", config::STDP_LTD_WEIGHT), ConfigSource::Default);
    config->set("stdp_tau", config::DEFAULT_STDP_TAU, ConfigSource::Default);
    config->set("simulation_timestep", config::SIMULATION_TIMESTEP, ConfigSource::Default);
    config->set("synaptogenesis_rate", config::SYNAPTOGENESIS_RATE, ConfigSource::Default);
    config->set("pruning_rate", config::PRUNING_RATE, ConfigSource::Default);
}

// Function to create a development-stage specific brain
std::shared_ptr<Brain> createDevelopmentStageBrain(
    std::shared_ptr<Config> config,
    DevelopmentalStage stage
) {
    BrainFactoryConfig factoryConfig = BrainFactory::createConfigFromMainConfig(config);
    
    // Set developmental stage in config
    config->set("developmental_stage", static_cast<int>(stage), ConfigSource::Runtime);
    
    return BrainFactory::createIntegratedBrain(factoryConfig);
}

// Function to create a production-ready brain (optimized for performance)
std::shared_ptr<Brain::Brain> createProductionBrain(std::shared_ptr<Config> config) {
    // Configure for production performance
    BrainFactoryConfig::SystemOptions prodOptions;
    prodOptions.enableSTDP = true;
    prodOptions.enableHebbian = true;
    prodOptions.enableStructuralPlasticity = false;  // Disable for production stability
    prodOptions.enableWorkingMemory = true;
    prodOptions.enableEpisodicMemory = true;
    prodOptions.enableAssociativeMemory = true;
    prodOptions.enablePrediction = true;
    prodOptions.enableCognition = true;
    prodOptions.enableDevelopment = true;
    prodOptions.enableNeuromodulation = true;
    prodOptions.enableCheckpointing = true;
    
    prodOptions.workingMemoryCapacity = config->getOr<size_t>("working_memory_capacity", 10000);
    prodOptions.maxEpisodicEpisodes = config->getOr<size_t>("max_episodes", 100000);
    prodOptions.planningDepth = 3;  // Simplified planning for production
    prodOptions.attentionInhibitionStrength = 0.3f;
    prodOptions.attentionExcitationStrength = 1.2f;
    
    return createIntegratedBrain(config, prodOptions);
}

// Helper function to validate brain system compatibility
bool validateBrainSystemsCompatibility(const std::shared_ptr<Brain>& brain) {
    auto& impl = *brain->pImpl;
    
    // Check that required systems are present
    if (!impl.spikeSystem) {
        std::cerr << "[BrainFactory Warning] Spike system missing" << std::endl;
        return false;
    }
    
    if (!impl.stdp) {
        std::cerr << "[BrainFactory Warning] STDP system missing" << std::endl;
        return false;
    }
    
    // Check that systems have been properly initialized
    if (brain->getTotalNeuronCount() == 0) {
        std::cerr << "[BrainFactory Warning] No neurons initialized" << std::endl;
        return false;
    }
    
    if (brain->getTotalSynapseCount() == 0) {
        std::cerr << "[BrainFactory Warning] No synapses initialized" << std::endl;
        return false;
    }
    
    return true;
}

// Function to create a benchmark brain (optimized for performance testing)
std::shared_ptr<Brain> createBenchmarkBrain(std::shared_ptr<Config> config) {
    // Configure for benchmarking
    BrainFactoryConfig::SystemOptions benchmarkOptions;
    benchmarkOptions.enableSTDP = true;
    benchmarkOptions.enableHebbian = false;  // Disable for benchmark simplicity
    benchmarkOptions.enableStructuralPlasticity = false;
    benchmarkOptions.enableWorkingMemory = false;  // Disable for memory efficiency
    benchmarkOptions.enableEpisodicMemory = false;
    benchmarkOptions.enableAssociativeMemory = false;
    benchmarkOptions.enablePrediction = false;
    benchmarkOptions.enableCognition = false;
    benchmarkOptions.enableDevelopment = false;
    benchmarkOptions.enableNeuromodulation = false;
    benchmarkOptions.enableCheckpointing = false;
    
    // Set minimal configuration for benchmarking
    benchmarkOptions.workingMemoryCapacity = 0;
    benchmarkOptions.maxEpisodicEpisodes = 0;
    benchmarkOptions.planningDepth = 1;
    
    auto brain = createIntegratedBrain(config, benchmarkOptions);
    
    if (!validateBrainSystemsCompatibility(brain)) {
        std::cerr << "[BrainFactory Warning] Benchmark brain validation warnings" << std::endl;
    }
    
    return brain;
}

} // namespace nlm
