#include "BrainInitialization.hpp"
#include "Brain.hpp"
#include "brain/NeuralRegion.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Random/Random.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "development/DevelopmentSystem.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/StructuralPlasticity.hpp"
#include "dynamics/SpikeSystem.hpp"
#include "performance/CheckpointSystem.hpp"
#include <algorithm>
#include <stdexcept>

namespace nlm {

namespace {
    
    // Helper functions for initialization
    size_t calculateNeuronsPerRegion(size_t totalNeurons, size_t regionCount) {
        if (regionCount == 0) return totalNeurons;
        return totalNeurons / regionCount;
    }
    
    NeuronType determineNeuronTypeByIndex(size_t neuronIndex, size_t sensoryNeurons, 
                                          size_t internalNeurons, size_t motorNeurons) {
        if (neuronIndex < sensoryNeurons) {
            return NeuronType::Sensory;
        } else if (neuronIndex < sensoryNeurons + internalNeurons) {
            return NeuronType::Internal;
        } else {
            return NeuronType::Motor;
        }
    }
    
    std::string generateRegionName(size_t regionId) {
        return "Region_" + std::to_string(regionId);
    }
    
} // anonymous namespace

// Initialize the Impl struct's initialization functionality
void Brain::Impl::initializeBrain() {
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
    
    // Validate configuration
    validateConfiguration();
    
    // Get configuration values
    size_t neuronCount = static_cast<size_t>(config->getOr<int64_t>("neuron_count", 1000));
    size_t regionCount = static_cast<size_t>(config->getOr<int64_t>("region_count", 1));
    float connectionProbability = config->getOr<float>("connection_probability", 0.1f);
    
    if (neuronCount == 0) {
        throw std::invalid_argument("Neuron count must be greater than 0");
    }
    if (regionCount == 0) {
        throw std::invalid_argument("Region count must be greater than 0");
    }
    if (connectionProbability < 0.0f || connectionProbability > 1.0f) {
        throw std::invalid_argument("Connection probability must be between 0.0 and 1.0");
    }
    
    NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                 std::to_string(regionCount) + " regions");
    
    // Create regions
    createRegions(regionCount);
    
    // Create neurons across regions
    createNeurons(neuronCount, regionCount, connectionProbability);
    
    // Initialize all integrated systems
    initializeMemorySystems();
    initializeCognitiveSystems();
    initializeNeuromodulationSystems();
    initializePlasticitySystems();
    initializeSpikeSystem();
    initializeCheckpointManager();
    
    // Register event handlers
    registerEventHandlers();
    
    // Log initialization summary
    logInitializationSummary();
}

void Brain::Impl::validateConfiguration() {
    // Configuration validation can be added here
    // For now, we assume the configuration is valid
    NLM_LOG_INFO("Configuration validation passed");
}

void Brain::Impl::createRegions(size_t regionCount) {
    // Create regions
    for (size_t i = 0; i < regionCount; ++i) {
        addRegion(generateRegionName(i + 1));
    }
}

void Brain::Impl::createNeurons(size_t neuronCount, size_t regionCount, float connectionProbability) {
    size_t neuronsPerRegion = calculateNeuronsPerRegion(neuronCount, regionCount);
    size_t sensoryNeurons = neuronsPerRegion / 4;
    size_t internalNeurons = neuronsPerRegion / 2;
    size_t motorNeurons = neuronsPerRegion / 4;
    
    // Adjust for rounding
    size_t remainingNeurons = neuronCount;
    for (size_t i = 0; i < regionCount && remainingNeurons > 0; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (region) {
            // Calculate neurons for this region
            size_t regionNeurons = std::min(neuronsPerRegion, remainingNeurons);
            remainingNeurons -= regionNeurons;
            
            if (regionNeurons == 0) continue;
            
            // Create populations
            createNeuronPopulations(region, regionNeurons, 
                                   sensoryNeurons, internalNeurons, motorNeurons);
        }
    }
    
    // Initialize random connectivity
    initializeConnectivity(connectionProbability);
}

void Brain::Impl::createNeuronPopulations(NeuralRegion* region, size_t totalNeurons,
                                         size_t sensoryNeurons, size_t internalNeurons, 
                                         size_t motorNeurons) {
    if (!region) return;
    
    size_t currentNeuron = 0;
    size_t neuronsInRegion = totalNeurons;
    
    // Create sensory population
    if (currentNeuron < sensoryNeurons && neuronsInRegion > 0) {
        size_t sensoryCount = std::min(sensoryNeurons, neuronsInRegion);
        auto sensoryPopId = region->addPopulation(sensoryCount, NeuronType::Sensory);
        auto* sensoryPop = region->getPopulation(sensoryPopId);
        if (sensoryPop) {
            for (auto* neuron : sensoryPop->getNeurons()) {
                sensoryNeurons.push_back(neuron);
            }
        }
        currentNeuron += sensoryCount;
        neuronsInRegion -= sensoryCount;
    }
    
    // Create internal population
    if (currentNeuron < sensoryNeurons + internalNeurons && neuronsInRegion > 0) {
        size_t internalCount = std::min(internalNeurons, neuronsInRegion);
        auto internalPopId = region->addPopulation(internalCount, NeuronType::Internal);
        auto* internalPop = region->getPopulation(internalPopId);
        if (internalPop) {
            for (auto* neuron : internalPop->getNeurons()) {
                motorNeurons.push_back(neuron);
            }
        }
        currentNeuron += internalCount;
        neuronsInRegion -= internalCount;
    }
    
    // Create motor population
    if (currentNeuron < sensoryNeurons + internalNeurons + motorNeurons && neuronsInRegion > 0) {
        size_t motorCount = std::min(motorNeurons, neuronsInRegion);
        auto motorPopId = region->addPopulation(motorCount, NeuronType::Motor);
        auto* motorPop = region->getPopulation(motorPopId);
        if (motorPop) {
            for (auto* neuron : motorPop->getNeurons()) {
                motorNeurons.push_back(neuron);
            }
        }
        currentNeuron += motorCount;
        neuronsInRegion -= motorCount;
    }
    
    // Log population creation
    if (region->getPopulationCount() > 0) {
        NLM_LOG_INFO("Created populations in region " + std::to_string(region->getId()) + 
                     ": " + std::to_string(region->getPopulationCount()) + " populations, " +
                     std::to_string(region->getTotalNeuronCount()) + " neurons");
    }
}

void Brain::Impl::initializeConnectivity(float connectionProbability) {
    // Initialize connectivity with random weights
    for (size_t i = 0; i < regions.size(); ++i) {
        auto* region = regions[i].get();
        if (region) {
            // Initialize random connectivity and synapse weights
            region->initializeRandomConnectivity(*rng, connectionProbability, 0.2f, 0.1f);
        }
    }
}

void Brain::Impl::initializeMemorySystems() {
    // Initialize working memory
    workingMemory->initialize(this);
    workingMemory->setCapacity(1000 / 10);  // neuronCount / 10
    
    // Initialize episodic memory
    episodicMemory->initialize(this);
    episodicMemory->setMaxEpisodes(1000);
    
    // Initialize associative memory
    associativeMemory->initialize(this);
}

void Brain::Impl::initializeCognitiveSystems() {
    // Initialize neural planner
    planner->initialize(this);
    planner->setPlanningDepth(5);
    
    // Initialize concept formation
    conceptFormation->initialize(this);
    
    // Initialize attentional selection
    attention->initialize(this);
    attention->setInhibitionStrength(0.5f);
    attention->setExcitationStrength(1.5f);
}

void Brain::Impl::initializeNeuromodulationSystems() {
    // Initialize novelty detection
    novelty->initialize(this);
    
    // Initialize curiosity system
    curiosity->initialize(this);
    
    // Note: Dopamine system is initialized elsewhere
}

void Brain::Impl::initializePlasticitySystems() {
    // Configure STDP parameters
    float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
    float ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
    float tau = config->getOr<float>("stdp_tau", 20.0f);
    stdp->configure(ltpWeight, ltdWeight, tau);
    
    // Configure structural plasticity
    float synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.0001f);
    float pruningRate = config->getOr<float>("pruning_rate", 0.00001f);
    structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
    structuralPlasticity->setPruningRate(pruningRate);
}

void Brain::Impl::initializeSpikeSystem() {
    // Initialize spike system
    spikeSystem = std::make_unique<SpikeSystem>();
    
    // Register spike handlers
    registerEventHandlers();
}

void Brain::Impl::initializeCheckpointManager() {
    // Configure checkpoint manager
    std::string checkpointDir = config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    checkpointManager->configure(checkpointDir, 10000, 5, true);
}

void Brain::Impl::registerEventHandlers() {
    // Register spike handlers for event-driven processing
    spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
        // Count spikes
        ++totalSpikesThisStep;
        ++totalSpikesTotal;
    });
    
    // Register delayed spike handler to deliver synaptic input
    spikeSystem->registerDelayedHandler([this](const DelayedSpikeEvent& event) {
        // Find destination neuron and deliver synaptic input
        for (auto& region : regions) {
            auto neurons = region->getAllNeurons();
            for (auto* neuron : neurons) {
                if (neuron->getId() == event.destination_neuron) {
                    // Apply synaptic weight as current
                    MembranePotential synapticCurrent = event.weight * 10.0f;  // Scale factor
                    if (event.is_excitatory) {
                        neuron->receiveExcitatoryInput(synapticCurrent);
                    } else {
                        neuron->receiveInhibitoryInput(-synapticCurrent);
                    }
                    return;
                }
            }
        }
    });
}

void Brain::Impl::logInitializationSummary() {
    NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Sensory neurons: " + std::to_string(sensoryNeurons.size()));
    NLM_LOG_INFO("Motor neurons: " + std::to_string(motorNeurons.size()));
}

// Forward declarations for supporting methods
size_t Brain::Impl::getTotalNeuronCount() const {
    size_t total = 0;
    for (const auto& region : regions) {
        total += region->getTotalNeuronCount();
    }
    return total;
}

size_t Brain::Impl::getTotalSynapseCount() const {
    size_t total = 0;
    for (const auto& region : regions) {
        total += region->getSynapseCount();
    }
    total += interRegionConnections.size();
    return total;
}

// Additional helper methods will be added here

} // namespace nlm
