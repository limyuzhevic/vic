// NLM Brain Initialization Helper Implementation
// Phase 2: Real Neural Computation

#include "BrainInitializationHelper.hpp"
#include "core/Logger/Logger.hpp"
#include <stdexcept>
#include <algorithm>

using namespace nlm;

std::vector<RegionId> BrainInitializationHelper::setupRegions(Brain& brain, const Config& config) {
    size_t neuronCount = config.getOr<size_t>("neuron_count", 1000);
    size_t regionCount = config.getOr<size_t>("region_count", 1);
    
    NLM_LOG_INFO("Setting up " + std::to_string(regionCount) + " regions for " + 
                 std::to_string(neuronCount) + " neurons");
    
    std::vector<RegionId> createdRegionIds;
    
    // Create regions
    for (size_t i = 0; i < regionCount; ++i) {
        RegionId regionId = brain.addRegion("Region_" + std::to_string(i + 1));
        if (regionId.value == 0) {  // Invalid region ID
            throw InitializationError("Failed to create region " + std::to_string(i + 1));
        }
        createdRegionIds.push_back(regionId);
        NLM_LOG_INFO("Created region " + std::to_string(i + 1) + " (ID: " + std::to_string(regionId.value) + ")");
    }
    
    return createdRegionIds;
}

bool BrainInitializationHelper::createPopulations(Brain& brain, size_t neuronsPerRegion) {
    if (neuronsPerRegion == 0) {
        NLM_LOG_ERROR("Cannot create populations with 0 neurons per region");
        return false;
    }
    
    NLM_LOG_INFO("Creating populations in regions with " + std::to_string(neuronsPerRegion) + " neurons each");
    
    for (size_t i = 0; i < brain.getRegionCount(); ++i) {
        auto* region = brain.getRegion(RegionId(i + 1));
        if (!region) {
            NLM_LOG_ERROR("Region " + std::to_string(i + 1) + " not found");
            return false;
        }
        
        // Calculate population sizes
        size_t sensoryNeurons = neuronsPerRegion / 4;
        size_t internalNeurons = neuronsPerRegion / 2;
        size_t motorNeurons = neuronsPerRegion / 4;
        
        // Ensure we have at least one neuron per population if we have any neurons
        if (neuronsPerRegion > 0) {
            if (sensoryNeurons == 0) sensoryNeurons = 1;
            if (internalNeurons == 0) internalNeurons = neuronsPerRegion - sensoryNeurons - motorNeurons;
            if (motorNeurons == 0 && internalNeurons > 0) motorNeurons = 1;
        }
        
        // Add populations
        auto sensoryPopId = region->addPopulation(sensoryNeurons, NeuronType::Sensory);
        auto internalPopId = region->addPopulation(internalNeurons, NeuronType::Internal);
        auto motorPopId = region->addPopulation(motorNeurons, NeuronType::Motor);
        
        NLM_LOG_INFO("Created populations in region " + std::to_string(i + 1) + 
                     ": " + std::to_string(region->getPopulationCount()) + " populations, " +
                     std::to_string(region->getTotalNeuronCount()) + " neurons");
    }
    
    return true;
}

bool BrainInitializationHelper::establishConnectivity(Brain& brain, float connectionProbability) {
    NLM_LOG_INFO("Establishing connectivity with probability: " + std::to_string(connectionProbability));
    
    auto* randomGenerator = brain.getRandomGenerator();
    if (!randomGenerator) {
        NLM_LOG_ERROR("Cannot establish connectivity: random generator not available");
        return false;
    }
    
    for (size_t i = 0; i < brain.getRegionCount(); ++i) {
        auto* region = brain.getRegion(RegionId(i + 1));
        if (region) {
            region->initializeRandomConnectivity(*randomGenerator, connectionProbability, 0.2f, 0.1f);
        }
    }
    
    return true;
}

bool BrainInitializationHelper::initializeIntegrationSystems(Brain& brain) {
    NLM_LOG_INFO("Initializing integration systems...");
    
    // Note: The actual integration system initialization is handled within Brain::Impl
    // This helper class focuses on providing a clean interface for complex initialization
    
    return true;
}

void BrainInitializationHelper::configurePlasticitySystems(Brain& brain, const Config& config) {
    NLM_LOG_INFO("Configuring plasticity systems...");
    
    float ltpWeight = config.getOr<float>("stdp_ltp_weight", 0.02f);
    float ltdWeight = config.getOr<float>("stdp_ltd_weight", 0.015f);
    float tau = config.getOr<float>("stdp_tau", 20.0f);
    
    auto* stdp = brain.getSTDP();
    if (stdp) {
        stdp->configure(ltpWeight, ltdWeight, tau);
        NLM_LOG_INFO("STDP configured: LTP=" + std::to_string(ltpWeight) + 
                     ", LTD=" + std::to_string(ltdWeight) + ", tau=" + std::to_string(tau));
    }
    
    float synaptogenesisRate = config.getOr<float>("synaptogenesis_rate", 0.0001f);
    float pruningRate = config.getOr<float>("pruning_rate", 0.00001f);
    
    auto* structuralPlasticity = brain.getStructuralPlasticity();
    if (structuralPlasticity) {
        structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
        structuralPlasticity->setPruningRate(pruningRate);
        NLM_LOG_INFO("Structural plasticity configured: synaptogenesis=" + std::to_string(synaptogenesisRate) + 
                     ", pruning=" + std::to_string(pruningRate));
    }
}

BrainInitializationHelper::InitializationStatus BrainInitializationHelper::initializeBrain(Brain& brain, const Config& config) {
    InitializationStatus status;
    
    try {
        // Validate initialization parameters
        status = validateInitializationParameters(config);
        if (!status.success) {
            return status;
        }
        
        NLM_LOG_INFO("Starting brain initialization (Phase 6: Integrated)...");
        
        // Setup regions
        std::vector<RegionId> createdRegions = setupRegions(brain, config);
        
        // Calculate neurons per region
        size_t neuronCount = config.getOr<size_t>("neuron_count", 1000);
        size_t regionCount = config.getOr<size_t>("region_count", 1);
        size_t neuronsPerRegion = neuronCount / regionCount;
        
        if (neuronsPerRegion == 0 && neuronCount > 0) {
            NLM_LOG_WARNING("Not enough neurons for each region, adjusting allocation");
            neuronsPerRegion = 1;
        } else if (neuronCount == 0) {
            status.success = false;
            status.errorMessage = "Cannot initialize brain with 0 neurons";
            return status;
        }
        
        // Create populations
        if (!createPopulations(brain, neuronsPerRegion)) {
            status.success = false;
            status.errorMessage = "Failed to create populations";
            return status;
        }
        
        // Establish connectivity
        float connectionProbability = config.getOr<float>("connection_probability", 0.1f);
        if (!establishConnectivity(brain, connectionProbability)) {
            status.success = false;
            status.errorMessage = "Failed to establish connectivity";
            return status;
        }
        
        // Configure plasticity systems
        configurePlasticitySystems(brain, config);
        
        // Initialize integration systems
        if (!initializeIntegrationSystems(brain)) {
            status.success = false;
            status.errorMessage = "Failed to initialize integration systems";
            return status;
        }
        
        NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
        NLM_LOG_INFO("Total neurons: " + std::to_string(brain.getTotalNeuronCount()));
        NLM_LOG_INFO("Total synapses: " + std::to_string(brain.getTotalSynapseCount()));
        
        status.success = true;
        
    } catch (const InitializationError& e) {
        status.success = false;
        status.errorMessage = e.what();
        NLM_LOG_ERROR(std::string("Brain initialization failed: ") + e.what());
    } catch (const std::exception& e) {
        status.success = false;
        status.errorMessage = std::string("Brain initialization exception: ") + e.what();
        NLM_LOG_ERROR(std::string("Brain initialization exception: ") + e.what());
    }
    
    return status;
}

void BrainInitializationHelper::resetBrainForTesting(Brain& brain) {
    NLM_LOG_INFO("Resetting brain for testing...");
    brain.reset();
    NLM_LOG_INFO("Brain reset complete");
}

void BrainInitializationHelper::logInitializationConfiguration(const Config& config) {
    NLM_LOG_INFO("=== Brain Initialization Configuration ===");
    NLM_LOG_INFO("neuron_count: " + std::to_string(config.getOr<size_t>("neuron_count", 1000)));
    NLM_LOG_INFO("region_count: " + std::to_string(config.getOr<size_t>("region_count", 1)));
    NLM_LOG_INFO("connection_probability: " + std::to_string(config.getOr<float>("connection_probability", 0.1f)));
    NLM_LOG_INFO("random_seed: " + std::to_string(config.getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("simulation_timestep: " + std::to_string(config.getOr<double>("simulation_timestep", 0.001)) + "s");
}

void BrainInitializationHelper::setupRegion(Brain& brain, RegionId regionId, size_t neuronsPerRegion, 
                                          const std::vector<NeuronType>& neuronTypes) {
    auto* region = brain.getRegion(regionId);
    if (!region) {
        throw InitializationError("Region " + std::to_string(regionId.value) + " not found");
    }
    
    // This method is called during population creation
    // Region setup is handled in the setupRegions method
}

void BrainInitializationHelper::createInputOutputNeurons(Brain& brain, RegionId regionId, size_t neuronsPerRegion) {
    auto* region = brain.getRegion(regionId);
    if (!region) return;
    
    // Collect sensory and motor neurons for I/O (implementation detail)
    auto* sensoryPop = region->getPopulation(NeuronType::Sensory);
    auto* motorPop = region->getPopulation(NeuronType::Motor);
    
    // This would be implemented if needed for specific I/O neuron collection
}

void BrainInitializationHelper::initializePlasticityComponents(Brain& brain, const Config& config) {
    // This is handled in configurePlasticitySystems method
}

BrainInitializationHelper::InitializationStatus BrainInitializationHelper::validateInitializationParameters(const Config& config) {
    InitializationStatus status;
    
    // Get required parameters with defaults
    size_t neuronCount = config.getOr<size_t>("neuron_count", 1000);
    size_t regionCount = config.getOr<size_t>("region_count", 1);
    float connectionProbability = config.getOr<float>("connection_probability", 0.1f);
    
    // Validate parameters
    if (neuronCount == 0) {
        status.success = false;
        status.errorMessage = "Cannot initialize brain with 0 neurons";
        return status;
    }
    
    if (regionCount == 0) {
        status.success = false;
        status.errorMessage = "Cannot initialize brain with 0 regions";
        return status;
    }
    
    if (neuronCount / regionCount == 0) {
        status.warnings.push_back("Not enough neurons for each region, adjusting allocation");
    }
    
    if (connectionProbability < 0.0f || connectionProbability > 1.0f) {
        status.success = false;
        status.errorMessage = "connection_probability must be in [0, 1], got: " + std::to_string(connectionProbability);
        return status;
    }
    
    status.success = true;
    return status;
}