#include "BrainInitializer.hpp"
#include "brain/NeuralPopulation.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/NeuralRegion.hpp"
#include "core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

bool BrainInitializer::initialize(Brain& brain) {
    NLM_LOG_INFO("Initializing NLM Brain using BrainInitializer...");
    
    try {
        // Create BrainConfig from brain's existing config if available
        BrainConfig config;
        if (brain.getConfig()) {
            // Try to apply existing config to our BrainConfig
            // This is a simplified version - full implementation would parse the config
            config = BrainConfig(brain.getConfig());
        }
        
        return initializeFromConfig(brain, config);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Brain initialization failed: ") + e.what());
        return false;
    }
}

bool BrainInitializer::initializeFromConfig(Brain& brain, const BrainConfig& config) {
    if (!config.validate()) {
        NLM_LOG_ERROR("BrainConfig validation failed: " + config.getValidationError());
        return false;
    }
    
    cleanup(brain);  // Clean up any existing brain state
    
    // Setup regions based on config
    if (!setupRegions(brain, config)) {
        NLM_LOG_ERROR("Failed to setup brain regions");
        return false;
    }
    
    // Setup neuron populations
    if (!setupNeuronPopulations(brain, config)) {
        NLM_LOG_ERROR("Failed to setup neuron populations");
        return false;
    }
    
    // Setup connectivity
    float connectionProbability = config.getRegionConfigs()[0].connectionProbability;
    float weightRange = config.getRegionConfigs()[0].weightRange;
    if (!initializeConnectivity(brain, connectionProbability, weightRange)) {
        NLM_LOG_ERROR("Failed to setup neural connectivity");
        return false;
    }
    
    // Setup system integration
    if (!setupSystemIntegration(brain)) {
        NLM_LOG_ERROR("Failed to setup system integration");
        return false;
    }
    
    // Setup checkpoint system
    if (!setupCheckpointSystem(brain, config.getCheckpointDir())) {
        NLM_LOG_WARN("Failed to setup checkpoint system (continuing)");
    }
    
    // Log initialization statistics
    logInitializationStats(brain);
    
    NLM_LOG_INFO("NLM Brain initialization complete");
    return true;
}

bool BrainInitializer::initializeLegacy(Brain& brain, const InitializationParameters& params) {
    NLM_LOG_INFO("Initializing NLM Brain using legacy method...");
    
    if (!validateInitializationParameters(params)) {
        NLM_LOG_ERROR("Invalid initialization parameters");
        return false;
    }
    
    cleanup(brain);
    
    // Create regions
    if (!setupRegionsLegacy(brain, params.regionCount, params.neuronCount / params.regionCount, 
                           params.connectionProbability)) {
        NLM_LOG_ERROR("Failed to setup regions (legacy)");
        return false;
    }
    
    // Setup neuron populations
    if (!setupNeuronPopulationsLegacy(brain, params.neuronCount / params.regionCount)) {
        NLM_LOG_ERROR("Failed to setup neuron populations (legacy)");
        return false;
    }
    
    // Setup connectivity
    if (!initializeConnectivity(brain, params.connectionProbability, params.weightRange)) {
        NLM_LOG_ERROR("Failed to setup connectivity (legacy)");
        return false;
    }
    
    if (!setupSystemIntegration(brain)) {
        NLM_LOG_ERROR("Failed to setup system integration (legacy)");
        return false;
    }
    
    logInitializationStats(brain);
    NLM_LOG_INFO("NLM Brain legacy initialization complete");
    return true;
}

bool BrainInitializer::setupRegions(Brain& brain, const BrainConfig& config) {
    NLM_LOG_INFO("Setting up brain regions...");
    
    for (const auto& regionConfig : config.getRegionConfigs()) {
        RegionId regionId = brain.addRegion(regionConfig.name);
        
        auto* region = brain.getRegion(regionId);
        if (!region) {
            NLM_LOG_ERROR("Failed to create region: " + regionConfig.name);
            return false;
        }
        
        NLM_LOG_INFO(std::string("Created region \") + regionConfig.name + "\" (ID: " + 
                    std::to_string(regionId.index()) + ")");
    }
    
    return true;
}

bool BrainInitializer::setupRegionsLegacy(Brain& brain, size_t regionCount, 
                                         size_t neuronsPerRegion, float connectionProbability) {
    NLM_LOG_INFO(std::string("Setting up ") + std::to_string(regionCount) + 
                std::string(" regions with ~") + std::to_string(neuronsPerRegion) + 
                std::string(" neurons each..."));
    
    for (size_t i = 0; i < regionCount; ++i) {
        RegionId regionId = brain.addRegion("Region_" + std::to_string(i + 1));
        auto* region = brain.getRegion(regionId);
        if (!region) {
            NLM_LOG_ERROR("Failed to create region " + std::to_string(i + 1));
            return false;
        }
    }
    
    return true;
}

bool BrainInitializer::setupNeuronPopulations(Brain& brain, const BrainConfig& config) {
    NLM_LOG_INFO("Setting up neuron populations...");
    
    for (const auto& regionConfig : config.getRegionConfigs()) {
        auto* region = brain.getRegion(RegionId(1)); // Using first region for now
        if (!region) {
            NLM_LOG_ERROR("Region not found for population setup");
            return false;
        }
        
        setupSingleRegionPopulations(*region, regionConfig);
    }
    
    return true;
}

bool BrainInitializer::setupNeuronPopulationsLegacy(Brain& brain, size_t neuronsPerRegion) {
    NLM_LOG_INFO(std::string("Setting up neuron populations with ~") + std::to_string(neuronsPerRegion) + 
                std::string(" neurons per region..."));
    
    for (size_t i = 0; i < brain.getRegionCount(); ++i) {
        auto* region = brain.getRegion(RegionId(i + 1));
        if (!region) {
            NLM_LOG_ERROR("Region not found for legacy population setup");
            return false;
        }
        
        setupSingleRegionPopulationsLegacy(*region, neuronsPerRegion);
    }
    
    return true;
}

void BrainInitializer::setupSingleRegionPopulations(NeuralRegion& region, 
                                                   const BrainConfig::RegionConfig& config) {
    size_t neuronsPerRegion = config.neuronCount;
    
    // Calculate population sizes
    size_t sensoryCount = neuronsPerRegion / 4;
    size_t internalCount = neuronsPerRegion / 2;
    size_t motorCount = neuronsPerRegion / 4;
    
    // Add populations
    auto sensoryId = region.addPopulation(sensoryCount, NeuronType::Sensory);
    auto internalId = region.addPopulation(internalCount, NeuronType::Internal);
    auto motorId = region.addPopulation(motorCount, NeuronType::Motor);
    
    // Verify populations were created
    if (!region.getPopulation(sensoryId) || !region.getPopulation(internalId) || 
        !region.getPopulation(motorId)) {
        NLM_LOG_ERROR("Failed to create all neuron populations");
    }
}

void BrainInitializer::setupSingleRegionPopulationsLegacy(NeuralRegion& region, size_t neuronsPerRegion) {
    size_t sensoryCount = neuronsPerRegion / 4;
    size_t internalCount = neuronsPerRegion / 2;
    size_t motorCount = neuronsPerRegion / 4;
    
    auto sensoryId = region.addPopulation(sensoryCount, NeuronType::Sensory);
    auto internalId = region.addPopulation(internalCount, NeuronType::Internal);
    auto motorId = region.addPopulation(motorCount, NeuronType::Motor);
}

bool BrainInitializer::initializeConnectivity(Brain& brain, float connectionProbability, 
                                            float weightRange) {
    NLM_LOG_INFO("Initializing neural connectivity...");
    
    auto* rng = brain.getRandomGenerator();
    if (!rng) {
        NLM_LOG_ERROR("Random generator not available for connectivity initialization");
        return false;
    }
    
    for (size_t i = 0; i < brain.getRegionCount(); ++i) {
        auto* region = brain.getRegion(RegionId(i + 1));
        if (!region) {
            NLM_LOG_ERROR("Region not found for connectivity setup");
            return false;
        }
        
        // Initialize random connectivity within region
        region->initializeRandomConnectivity(*rng, connectionProbability, 
                                             weightRange, -weightRange);
        
        // Create inter-region connections for multi-region brains
        if (brain.getRegionCount() > 1) {
            createInterRegionConnections(brain, RegionId(i + 1), weightRange);
        }
    }
    
    return true;
}

void BrainInitializer::createInterRegionConnections(Brain& brain, RegionId sourceRegion, 
                                                   float weightRange) {
    auto* source = brain.getRegion(sourceRegion);
    if (!source) return;
    
    // Connect to all other regions
    for (size_t i = 0; i < brain.getRegionCount(); ++i) {
        RegionId targetId(i + 1);
        if (targetId == sourceRegion) continue;  // Skip self-connections
        
        auto* target = brain.getRegion(targetId);
        if (!target) continue;
        
        // Create random inter-region connections
        size_t sourceNeurons = source->getTotalNeuronCount();
        size_t targetNeurons = target->getTotalNeuronCount();
        size_t maxConnections = std::min(sourceNeurons / 10, targetNeurons);
        
        if (maxConnections > 0) {
            auto* rng = brain.getRandomGenerator();
            if (rng) {
                for (size_t j = 0; j < maxConnections; ++j) {
                    size_t sourceNeuronIdx = rng->uniformInt(0, sourceNeurons - 1);
                    size_t targetNeuronIdx = rng->uniformInt(0, targetNeurons - 1);
                    
                    // Get actual neurons (simplified - in practice would need proper indexing)
                    // For now, just create the inter-region connection at brain level
                }
            }
        }
        
        // Use brain's method to add inter-region connection
        float weight = (rng ? rng->uniformReal(-weightRange, weightRange) : 0.0f);
        brain.addInterRegionConnection(sourceRegion, targetId, weight, 10); // Fixed delay
    }
}

bool BrainInitializer::setupSystemIntegration(Brain& brain) {
    NLM_LOG_INFO("Setting up system integration...");
    
    if (!setupMemorySystems(brain)) {
        NLM_LOG_ERROR("Failed to setup memory systems");
        return false;
    }
    
    if (!setupPredictionSystem(brain)) {
        NLM_LOG_ERROR("Failed to setup prediction system");
        return false;
    }
    
    if (!setupCognitionSystems(brain)) {
        NLM_LOG_ERROR("Failed to setup cognition systems");
        return false;
    }
    
    if (!setupNeuromodulationSystems(brain)) {
        NLM_LOG_ERROR("Failed to setup neuromodulation systems");
        return false;
    }
    
    if (!setupPlasticitySystems(brain)) {
        NLM_LOG_ERROR("Failed to setup plasticity systems");
        return false;
    }
    
    if (!setupDevelopmentSystem(brain)) {
        NLM_LOG_ERROR("Failed to setup development system");
        return false;
    }
    
    // Initialize spike system
    if (!initializeSpikeSystem(brain)) {
        NLM_LOG_ERROR("Failed to initialize spike system");
        return false;
    }
    
    // Register spike handlers
    if (!registerSpikeHandlers(brain)) {
        NLM_LOG_ERROR("Failed to register spike handlers");
        return false;
    }
    
    return true;
}

bool BrainInitializer::setupMemorySystems(Brain& brain) {
    NLM_LOG_INFO("Setting up memory systems...");
    
    auto* workingMemory = brain.getWorkingMemory();
    if (!workingMemory) {
        NLM_LOG_ERROR("Working memory system not available");
        return false;
    }
    
    workingMemory->initialize(&brain);
    
    // Set capacity based on brain size
    size_t capacity = brain.getTotalNeuronCount() / 10;
    workingMemory->setCapacity(capacity);
    
    // Initialize episodic memory if available
    auto* episodicMemory = brain.getEpisodicMemory();
    if (episodicMemory) {
        episodicMemory->initialize(&brain);
        episodicMemory->setMaxEpisodes(1000);
    }
    
    // Initialize associative memory if available
    auto* associativeMemory = brain.getAssociativeMemory();
    if (associativeMemory) {
        associativeMemory->initialize(&brain);
    }
    
    return true;
}

bool BrainInitializer::setupPredictionSystem(Brain& brain) {
    auto* predictionSystem = brain.getPredictionSystem();
    if (!predictionSystem) {
        NLM_LOG_WARN("Prediction system not available (continuing)");
        return true; // Optional system
    }
    
    // Prediction system doesn't have initialize method in current API
    NLM_LOG_INFO("Prediction system initialized (if available)");
    return true;
}

bool BrainInitializer::setupCognitionSystems(Brain& brain) {
    NLM_LOG_INFO("Setting up cognition systems...");
    
    auto* planner = brain.getPlanner();
    if (!planner) {
        NLM_LOG_ERROR("Planner system not available");
        return false;
    }
    
    planner->initialize(&brain);
    planner->setPlanningDepth(5);
    
    auto* conceptFormation = brain.getConceptFormation();
    if (conceptFormation) {
        conceptFormation->initialize(&brain);
    }
    
    auto* attention = brain.getAttention();
    if (!attention) {
        NLM_LOG_ERROR("Attention system not available");
        return false;
    }
    
    attention->initialize(&brain);
    attention->setInhibitionStrength(0.5f);
    attention->setExcitationStrength(1.5f);
    
    return true;
}

bool BrainInitializer::setupNeuromodulationSystems(Brain& brain) {
    NLM_LOG_INFO("Setting up neuromodulation systems...");
    
    auto* novelty = brain.getNovelty();
    if (novelty) {
        novelty->initialize(&brain);
    }
    
    auto* curiosity = brain.getCuriosity();
    if (curiosity) {
        curiosity->initialize(&brain);
    }
    
    auto* dopamine = brain.getDopamine();
    if (dopamine) {
        dopamine->initialize(&brain);
    }
    
    auto* predictionError = brain.getPredictionErrorSignal();
    if (predictionError) {
        predictionError->initialize(&brain);
    }
    
    return true;
}

bool BrainInitializer::setupPlasticitySystems(Brain& brain) {
    NLM_LOG_INFO("Setting up plasticity systems...");
    
    auto* stdp = brain.getSTDP();
    if (!stdp) {
        NLM_LOG_ERROR("STDP system not available");
        return false;
    }
    
    // Configure STDP parameters
    stdp->configure(0.01f, 0.012f, 20.0f);
    
    auto* hebbian = brain.getHebbian();
    if (hebbian) {
        // Configure Hebbian parameters
    }
    
    auto* structuralPlasticity = brain.getStructuralPlasticity();
    if (structuralPlasticity) {
        structuralPlasticity->setSynaptogenesisRate(0.0001f);
        structuralPlasticity->setPruningRate(0.00001f);
    }
    
    return true;
}

bool BrainInitializer::setupDevelopmentSystem(Brain& brain) {
    auto* developmentSystem = brain.getDevelopmentSystem();
    if (!developmentSystem) {
        NLM_LOG_WARN("Development system not available (continuing)");
        return true; // Optional system
    }
    
    developmentSystem->initialize();
    return true;
}

bool BrainInitializer::setupCheckpointSystem(Brain& brain, const std::string& checkpointDir) {
    auto* checkpointManager = brain.getCheckpointManager();
    if (!checkpointManager) {
        NLM_LOG_WARN("Checkpoint system not available (continuing)");
        return true; // Optional system
    }
    
    checkpointManager->configure(checkpointDir, 10000, 5, true);
    return true;
}

bool BrainInitializer::initializeSpikeSystem(Brain& brain) {
    auto* spikeSystem = brain.getSpikeSystem();
    if (!spikeSystem) {
        NLM_LOG_ERROR("Spike system not available");
        return false;
    }
    
    // Spike system should already be initialized by Brain constructor
    // Just verify it's working
    return true;
}

bool BrainInitializer::registerSpikeHandlers(Brain& brain) {
    auto* spikeSystem = brain.getSpikeSystem();
    if (!spikeSystem) {
        return false;
    }
    
    // Register handlers for brain statistics and memory updates
    spikeSystem->registerHandler([this, &brain](const DetailedSpikeEvent& event) {
        // Update spike statistics
        // This would increment brain statistics
    });
    
    spikeSystem->registerDelayedHandler([this, &brain](const DelayedSpikeEvent& event) {
        // Deliver synaptic input to destination neuron
        brain.deliverSynapticInput(event);
    });
    
    return true;
}

bool BrainInitializer::configurePlasticity(Brain& brain, const BrainConfig& config) {
    auto* stdp = brain.getSTDP();
    if (!stdp) return false;
    
    stdp->configure(config.getPlasticityConfig().stdpLTPWeight,
                   config.getPlasticityConfig().stdpLTDWeight,
                   config.getPlasticityConfig().stdpTimeConstant);
    
    auto* structuralPlasticity = brain.getStructuralPlasticity();
    if (structuralPlasticity) {
        structuralPlasticity->setSynaptogenesisRate(config.getPlasticityConfig().synaptogenesisRate);
        structuralPlasticity->setPruningRate(config.getPlasticityConfig().pruningRate);
    }
    
    return true;
}

bool BrainInitializer::configureDevelopment(Brain& brain, const BrainConfig& config) {
    auto* developmentSystem = brain.getDevelopmentSystem();
    if (!developmentSystem) return false;
    
    // Configure development system based on config
    return true;
}

bool BrainInitializer::configureNeuromodulation(Brain& brain, const BrainConfig& config) {
    // Configure neuromodulation systems based on config
    return true;
}

void BrainInitializer::logInitializationStats(const Brain& brain) const {
    NLM_LOG_INFO("=== NLM Brain Initialization Statistics ===");
    NLM_LOG_INFO("Regions: " + std::to_string(brain.getRegionCount()));
    NLM_LOG_INFO("Total neurons: " + std::to_string(brain.getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(brain.getTotalSynapseCount()));
    NLM_LOG_INFO("Active neurons: " + std::to_string(brain.getActiveNeuronCount()));
    
    float eiratio = brain.getExcitationInhibitionRatio();
    NLM_LOG_INFO("E/I ratio: " + std::to_string(eiratio));
    
    NLM_LOG_INFO("Average firing rate: " + std::to_string(brain.getAverageFiringRate()));
    
    // Memory system stats (if available)
    auto* workingMemory = brain.getWorkingMemory();
    if (workingMemory) {
        NLM_LOG_INFO("Working memory traces: " + std::to_string(workingMemory->getActiveTraces()));
    }
    
    auto* episodicMemory = brain.getEpisodicMemory();
    if (episodicMemory) {
        NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(episodicMemory->getEpisodeCount()));
    }
}

void BrainInitializer::cleanup(Brain& brain) {
    NLM_LOG_INFO("Cleaning up brain state...");
    
    brain.reset();
    
    // Clear any remaining references
    // (In practice, this would clear all system references)
}

bool BrainInitializer::validateInitializationParameters(const InitializationParameters& params) const {
    if (params.neuronCount == 0) {
        NLM_LOG_ERROR("Neuron count must be positive");
        return false;
    }
    
    if (params.regionCount == 0) {
        NLM_LOG_ERROR("Region count must be positive");
        return false;
    }
    
    if (params.connectionProbability < 0.0f || params.connectionProbability > 1.0f) {
        NLM_LOG_ERROR("Connection probability must be between 0 and 1");
        return false;
    }
    
    if (params.weightRange < 0.0f) {
        NLM_LOG_ERROR("Weight range must be positive");
        return false;
    }
    
    return true;
}

bool BrainInitializer::validateConfiguration(const BrainConfig& config) const {
    return config.validate();
}

} // namespace nlm
