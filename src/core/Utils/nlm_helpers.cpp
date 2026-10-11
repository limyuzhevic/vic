// NLM Helper Functions Implementation
// Provides common utilities for brain simulation and benchmarking

#include "core/Utils/nlm_helpers.hpp"
#include "core/Logger/Logger.hpp"
#include <chrono>
#include <iomanip>
#include <sstream>

namespace nlm {

// Benchmark result structure (defined here to avoid including benchmark headers)
struct BenchmarkResult {
    size_t neurons;
    size_t synapses;
    size_t steps;
    double totalTime;      // seconds
    double stepsPerSecond;
    double spikesPerStep;
    double synapsesPerNeuron;
};

// Logger initialization helpers

void initializeGlobalLogger(LogLevel level, bool addConsole) {
    auto logger = std::make_shared<Logger>();
    if (addConsole) {
        auto consoleLogger = std::make_shared<ConsoleLogger>(level);
        logger->addLogger(consoleLogger);
    }
    Logger::setGlobal(logger);
    NLM_LOG_INFO("Global logger initialized at level: " + std::to_string(static_cast<int>(level)));
}

// Configuration helper functions

std::shared_ptr<Config> createStandardBrainConfig(
    size_t neuronCount,
    size_t regionCount,
    float connectionProbability,
    float stdpLtpWeight,
    float stdpLtdWeight
) {
    auto config = std::make_shared<Config>();
    
    // Set standard configuration parameters
    config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(regionCount), ConfigSource::Default);
    config->set("connection_probability", connectionProbability, ConfigSource::Default);
    config->set("stdp_ltp_weight", stdpLtpWeight, ConfigSource::Default);
    config->set("stdp_ltd_weight", stdpLtdWeight, ConfigSource::Default);
    
    // Set default values for other commonly used parameters
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("reward_discount_factor", 0.99, ConfigSource::Default);
    config->set("log_level", "INFO", ConfigSource::Default);
    config->set("log_to_file", false, ConfigSource::Default);
    
    NLM_LOG_INFO("Created standard brain config: " + std::to_string(neuronCount) + 
                 " neurons, " + std::to_string(regionCount) + " regions, " +
                 "connection probability: " + std::to_string(connectionProbability));
    
    return config;
}

void setStandardConfigParameters(
    const std::shared_ptr<Config>& config,
    size_t neuronCount,
    size_t regionCount,
    float connectionProbability
) {
    if (!config) {
        throw std::invalid_argument("Config object cannot be null");
    }
    
    config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Runtime);
    config->set("region_count", static_cast<int64_t>(regionCount), ConfigSource::Runtime);
    config->set("connection_probability", connectionProbability, ConfigSource::Runtime);
    
    NLM_LOG_INFO("Set standard config parameters: " + std::to_string(neuronCount) + 
                 " neurons, " + std::to_string(regionCount) + " regions");
}

std::shared_ptr<Brain> createBrainWithConfig(
    size_t neuronCount,
    size_t regionCount,
    float connectionProbability
) {
    auto config = createStandardBrainConfig(neuronCount, regionCount, connectionProbability);
    auto brain = std::make_shared<Brain>(config);
    initializeBrain(brain, "Brain");
    return brain;
}

void createRegionsWithPopulations(
    const std::shared_ptr<Brain>& brain,
    size_t neuronCount,
    size_t regionCount,
    float sensoryProportion,
    float motorProportion,
    float internalProportion
) {
    if (!brain) {
        throw std::invalid_argument("Brain object cannot be null");
    }
    
    if (regionCount == 0) {
        throw std::invalid_argument("Region count must be positive");
    }
    
    size_t neuronsPerRegion = neuronCount / regionCount;
    
    for (size_t i = 0; i < regionCount; ++i) {
        std::string regionName = "Region_" + std::to_string(i + 1);
        brain->addRegion(regionName);
        
        auto* region = brain->getRegion(RegionId(i + 1));
        if (!region) {
            NLM_LOG_ERROR("Failed to get region " + std::to_string(i + 1));
            continue;
        }
        
        // Calculate population sizes
        size_t sensoryNeurons = static_cast<size_t>(neuronsPerRegion * sensoryProportion);
        size_t motorNeurons = static_cast<size_t>(neuronsPerRegion * motorProportion);
        size_t internalNeurons = neuronsPerRegion - sensoryNeurons - motorNeurons;
        
        // Ensure minimum neurons for each population
        sensoryNeurons = std::max(sensoryNeurons, size_t(1));
        motorNeurons = std::max(motorNeurons, size_t(1));
        internalNeurons = std::max(internalNeurons, size_t(1));
        
        // Create populations
        auto sensoryPopId = region->addPopulation(sensoryNeurons, NeuronType::Sensory);
        auto internalPopId = region->addPopulation(internalNeurons, NeuronType::Internal);
        auto motorPopId = region->addPopulation(motorNeurons, NeuronType::Motor);
        
        auto* sensoryPop = region->getPopulation(sensoryPopId);
        auto* internalPop = region->getPopulation(internalPopId);
        auto* motorPop = region->getPopulation(motorPopId);
        
        // Collect sensory and motor neurons for I/O
        if (sensoryPop) {
            for (auto* neuron : sensoryPop->getNeurons()) {
                brain->addSensoryNeuron(neuron);
            }
        }
        
        if (motorPop) {
            for (auto* neuron : motorPop->getNeurons()) {
                brain->addMotorNeuron(neuron);
            }
        }
        
        NLM_LOG_INFO("Created region " + std::to_string(i + 1) + 
                     ": " + std::to_string(region->getPopulationCount()) + 
                     " populations, " + std::to_string(region->getTotalNeuronCount()) + 
                     " neurons");
    }
    
    NLM_LOG_INFO("Created " + std::to_string(regionCount) + " regions with " +
                 std::to_string(neuronCount) + " total neurons");
}

void initializeBrain(const std::shared_ptr<Brain>& brain, const std::string& logPrefix) {
    if (!brain) {
        throw std::invalid_argument("Brain object cannot be null");
    }
    
    NLM_LOG_INFO(logPrefix + " Initialization started...");
    
    if (!brain->initialize()) {
        throw std::runtime_error(logPrefix + " initialization failed");
    }
    
    NLM_LOG_INFO(logPrefix + " Initialization complete");
}

BenchmarkResult runSimpleSimulation(const std::shared_ptr<Brain>& brain, size_t steps) {
    if (!brain) {
        throw std::invalid_argument("Brain object cannot be null");
    }
    
    NLM_LOG_INFO("Starting simulation: " + std::to_string(steps) + " steps");
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    for (size_t step = 0; step < steps; ++step) {
        brain->step(step, step * 0.001);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);
    
    size_t finalSpikes = brain->getTotalSpikeCount();
    size_t totalSpikes = finalSpikes - initialSpikes;
    size_t synapses = brain->getTotalSynapseCount();
    
    BenchmarkResult result;
    result.neurons = brain->getTotalNeuronCount();
    result.synapses = synapses;
    result.steps = steps;
    result.totalTime = duration.count();
    result.stepsPerSecond = steps / duration.count();
    result.spikesPerStep = static_cast<double>(totalSpikes) / steps;
    result.synapsesPerNeuron = static_cast<double>(synapses) / result.neurons;
    
    NLM_LOG_INFO("Simulation completed: " + std::to_string(totalSpikes) + " spikes, " +
                 std::to_string(duration.count()) + " seconds");
    
    return result;
}

BenchmarkResult runBenchmark(size_t neuronCount, size_t steps, size_t regionCount) {
    auto brain = createBrainWithConfig(neuronCount, regionCount, 0.1f);
    return runSimpleSimulation(brain, steps);
}

std::shared_ptr<Brain> createCompleteBrain(
    const std::shared_ptr<Config>& config,
    bool addDevelopment,
    bool addPlasticity,
    bool addNeuromodulation
) {
    if (!config) {
        throw std::invalid_argument("Config object cannot be null");
    }
    
    auto brain = std::make_shared<Brain>(config);
    
    // Enable all optional features
    if (addDevelopment) {
        brain->enableDevelopment(true);
    }
    
    if (addPlasticity) {
        brain->enablePlasticity(true, true, false); // Enable STDP and Hebbian
    }
    
    if (addNeuromodulation) {
        brain->enableRewardModulation(true);
    }
    
    initializeBrain(brain, "Complete Brain");
    
    return brain;
}

BenchmarkParameters getBenchmarkParameters(size_t neuronCount) {
    BenchmarkParameters params;
    params.neuronCount = neuronCount;
    params.regionCount = std::max(size_t(1), neuronCount / 1000); // 1000 neurons per region
    params.steps = std::max(size_t(100), neuronCount / 10); // 10 neurons per step
    params.connectionProbability = 0.1f;
    params.stdpLtpWeight = 0.01f;
    params.stdpLtdWeight = 0.012f;
    
    return params;
}

} // namespace nlm