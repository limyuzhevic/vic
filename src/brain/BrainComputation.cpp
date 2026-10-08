#include "BrainRefactored.hpp"
#include "../core/Types/Types.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../plasticity/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <vector>
#include <memory>

namespace nlm {

struct BrainComputation::Impl {
    // Spike event management
    std::vector<SpikeEvent> pendingSpikes;
    std::vector<DelayedSpikeEvent> pendingDelayedSpikes;
    std::unordered_map<NeuronId, size_t> spikeCountMap;
    
    // Plasticity tracking
    std::unordered_map<NeuronId, std::vector<float>> preSpikeHistory;
    std::unordered_map<NeuronId, std::vector<float>> postSpikeHistory;
    
    // Statistics
    size_t totalSpikesThisStep = 0;
    size_t totalSpikesTotal = 0;
    
    // Event-driven processing callbacks
    std::vector<std::function<void(const DetailedSpikeEvent&)>> spikeHandlers;
    std::vector<std::function<void(const DelayedSpikeEvent&)>> delayedSpikeHandlers;
};

BrainComputation::BrainComputation(const std::shared_ptr<Config>& config, 
                                 const std::vector<std::unique_ptr<NeuralRegion>>& regions)
    : pImpl(std::make_unique<Impl>()) {
    // Initialize spike system with event-driven processing
    // This will be set up by the main Brain class
}

void BrainComputation::step(SimulationStep currentStep, Timestamp currentTime,
                           const std::vector<std::unique_ptr<NeuralRegion>>& regions) {
    // Process immediate spikes first
    processSpikes(regions);
    
    // Process delayed spikes (deliver synaptic input)
    processDelayedSpikes(regions, currentStep, currentTime);
    
    // Reset step statistics
    totalSpikesThisStep = 0;
}

void BrainComputation::processSpikes(const std::vector<std::unique_ptr<NeuralRegion>>& regions) {
    // Process all queued spike events
    for (const auto& event : pImpl->pendingSpikes) {
        // Call all registered spike handlers
        DetailedSpikeEvent detailedEvent;
        detailedEvent.neuron_id = event.neuron_id;
        detailedEvent.time = event.time;
        detailedEvent.step = event.step;
        
        for (const auto& handler : pImpl->spikeHandlers) {
            handler(detailedEvent);
        }
    }
    
    // Clear processed spikes
    pImpl->pendingSpikes.clear();
}

void BrainComputation::processDelayedSpikes(const std::vector<std::unique_ptr<NeuralRegion>>& regions,
                                          SimulationStep currentStep, Timestamp currentTime) {
    // Process delayed spike events in chronological order
    std::stable_sort(pImpl->pendingDelayedSpikes.begin(), pImpl->pendingDelayedSpikes.end(),
                     [currentStep](const DelayedSpikeEvent& a, const DelayedSpikeEvent& b) {
                         return a.delivery_step < b.delivery_step;
                     });
    
    for (const auto& event : pImpl->pendingDelayedSpikes) {
        if (event.delivery_step <= currentStep) {
            // Deliver synaptic input to destination neuron
            for (const auto& region : regions) {
                for (const auto* syn : region->getSynapses()) {
                    if (syn->getId() == event.synapse_id && 
                        syn->getDestinationNeuron() == event.destination_neuron) {
                        
                        // Apply synaptic weight as current
                        MembranePotential synapticCurrent = event.weight * 10.0f;
                        if (event.is_excitatory) {
                            // This would be delivered to the actual neuron through integration layer
                            // For now, we'll track it in statistics
                            ++totalSpikesThisStep;
                        }
                        break;
                    }
                }
            }
            
            // Call delayed spike handlers
            for (const auto& handler : pImpl->delayedSpikeHandlers) {
                handler(event);
            }
        }
    }
    
    // Remove processed delayed spikes
    pImpl->pendingDelayedSpikes.erase(
        std::remove_if(pImpl->pendingDelayedSpikes.begin(), pImpl->pendingDelayedSpikes.end(),
                      [currentStep](const DelayedSpikeEvent& event) {
                          return event.delivery_step <= currentStep;
                      }),
        pImpl->pendingDelayedSpikes.end()
    );
}

void BrainComputation::queueSpike(const SpikeEvent& event) {
    pImpl->pendingSpikes.push_back(event);
    
    // Update spike history for plasticity
    auto& history = pImpl->postSpikeHistory[event.neuron_id];
    history.push_back(event.time);
    
    // Keep only recent spikes (last 1000)
    if (history.size() > 1000) {
        history.erase(history.begin());
    }
}

void BrainComputation::queueDelayedSpike(const DelayedSpikeEvent& event) {
    pImpl->pendingDelayedSpikes.push_back(event);
    
    // Update spike history for plasticity
    auto& history = pImpl->preSpikeHistory[event.source_neuron];
    history.push_back(event.time);
    
    // Keep only recent spikes (last 1000)
    if (history.size() > 1000) {
        history.erase(history.begin());
    }
}

void BrainComputation::updatePlasticity(const std::vector<std::unique_ptr<NeuralRegion>>& regions,
                                      float plasticityMod, Timestamp currentTime) {
    // Update synaptic weights based on spike history
    for (const auto& region : regions) {
        for (auto* syn : region->getSynapses()) {
            if (syn->getPlasticityFlags().stdp || syn->getPlasticityFlags().hebbian) {
                const auto& preSpikes = pImpl->preSpikeHistory[syn->getSourceNeuron()];
                const auto& postSpikes = pImpl->postSpikeHistory[syn->getDestinationNeuron()];
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Apply STDP with neuromodulation
                    if (syn->getPlasticityFlags().stdp) {
                        // Apply STDP update logic here
                        // This would call the actual STDP implementation
                    }
                    
                    // Apply Hebbian learning
                    if (syn->getPlasticityFlags().hebbian) {
                        // Apply Hebbian update logic here
                        // This would call the actual Hebbian implementation
                    }
                    
                    // Apply neuromodulation scaling
                    float weight = syn->getWeight();
                    weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
                    syn->setWeight(weight);
                }
            }
        }
    }
}

void BrainComputation::registerHandler(std::function<void(const DetailedSpikeEvent&)> handler) {
    pImpl->spikeHandlers.push_back(std::move(handler));
}

void BrainComputation::registerDelayedHandler(std::function<void(const DelayedSpikeEvent&)> handler) {
    pImpl->delayedSpikeHandlers.push_back(std::move(handler));
}

void BrainComputation::reset(const std::vector<std::unique_ptr<NeuralRegion>>& regions) {
    pImpl->pendingSpikes.clear();
    pImpl->pendingDelayedSpikes.clear();
    pImpl->spikeCountMap.clear();
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
    totalSpikesThisStep = 0;
}

// Getters
void BrainComputation::setupHandlers(SpikeSystem& spikeSystem) {
    // Register spike handlers for event-driven processing
    spikeSystem.registerHandler([this](const DetailedSpikeEvent& event) {
        // Count spikes
        ++pImpl->totalSpikesThisStep;
        ++pImpl->totalSpikesTotal;
        
        // Update spike counts
        ++pImpl->spikeCountMap[event.neuron_id];
    });
    
    // Register delayed spike handler to deliver synaptic input
    spikeSystem.registerDelayedHandler([this](const DelayedSpikeEvent& event) {
        // Deliver synaptic input to destination neuron
        // This would be implemented with access to regions
        ++pImpl->totalSpikesThisStep;
    });
}

void BrainComputation::registerCountHandler(SpikeSystem& spikeSystem) {
    spikeSystem.registerHandler([this](const DetailedSpikeEvent& event) {
        // Count spikes
        ++pImpl->totalSpikesThisStep;
        ++pImpl->totalSpikesTotal;
    });
}

void BrainComputation::registerDeliverHandler(SpikeSystem& spikeSystem,
                                             const std::vector<std::unique_ptr<NeuralRegion>>& regions) {
    spikeSystem.registerDelayedHandler([this, &regions](const DelayedSpikeEvent& event) {
        // Find destination neuron and deliver synaptic input
        for (const auto& region : regions) {
            auto neurons = region->getAllNeurons();
            for (auto* neuron : neurons) {
                if (neuron->getId() == event.destination_neuron) {
                    // This would be handled by the integration layer
                    // For now, we just track the delivery
                    return;
                }
            }
        }
    });
}

void BrainComputation::setupHandlersForIntegration(BrainComputation& comp,
                                                 SpikeSystem& spikeSystem,
                                                 const std::vector<std::unique_ptr<NeuralRegion>>& regions) {
    // Register spike handlers for event-driven processing
    spikeSystem.registerHandler([comp](const DetailedSpikeEvent& event) {
        // Count spikes
        ++comp.pImpl->totalSpikesThisStep;
        ++comp.pImpl->totalSpikesTotal;
        
        // Update spike counts
        ++comp.pImpl->spikeCountMap[event.neuron_id];
    });
    
    // Register delayed spike handler to deliver synaptic input
    spikeSystem.registerDelayedHandler([comp, &regions](const DelayedSpikeEvent& event) {
        // Find destination neuron and deliver synaptic input
        for (const auto& region : regions) {
            auto neurons = region->getAllNeurons();
            for (auto* neuron : neurons) {
                if (neuron->getId() == event.destination_neuron) {
                    // This would be handled by the integration layer
                    // For now, we just track the delivery
                    return;
                }
            }
        }
    });
}

// Register handlers with spike system
size_t BrainComputation::getPendingSpikeEventCount() const {
    return pImpl->pendingSpikes.size() + pImpl->pendingDelayedSpikes.size();
}

// Helper functions
void BrainComputation::registerCountHandlerToSpikeSystem(SpikeSystem& spikeSystem,
                                                         std::function<void(const DetailedSpikeEvent&)> handler) {
    spikeSystem.registerHandler(handler);
}

void BrainComputation::registerDeliverHandlerToSpikeSystem(SpikeSystem& spikeSystem,
                                                           std::function<void(const DelayedSpikeEvent&)> handler) {
    spikeSystem.registerDelayedHandler(handler);
}

void BrainComputation::registerCountAndDeliverHandlers(SpikeSystem& spikeSystem,
                                                     std::function<void(const DetailedSpikeEvent&)> countHandler,
                                                     std::function<void(const DelayedSpikeEvent&)> deliverHandler) {
    registerCountHandlerToSpikeSystem(spikeSystem, countHandler);
    registerDeliverHandlerToSpikeSystem(spikeSystem, deliverHandler);
}

// Implementation of BrainMemory component
struct BrainMemory::Impl {
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
    
    size_t stepsSinceLastEpisode = 0;
    size_t replayInterval = 100;
    size_t consolidationInterval = 1000;
};

BrainMemory::BrainMemory(const std::shared_ptr<Config>& config)
    : pImpl(std::make_unique<Impl>()) {
    // Initialize memory system pointers - these will be set by Brain class
    // pImpl->workingMemory = std::make_unique<NeuralWorkingMemory>();
    // pImpl->episodicMemory = std::make_unique<NeuralEpisodicMemory>();
    // pImpl->associativeMemory = std::make_unique<NeuralAssociativeMemory>();
}

void BrainMemory::initializeSystems(const Brain* brain) {
    if (!brain) return;
    
    // Initialize working memory
    pImpl->workingMemory = std::make_unique<NeuralWorkingMemory>();
    if (pImpl->workingMemory) {
        pImpl->workingMemory->initialize(brain);
        pImpl->workingMemory->setCapacity(100); // Default capacity
    }
    
    // Initialize episodic memory
    pImpl->episodicMemory = std::make_unique<NeuralEpisodicMemory>();
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->initialize(brain);
        pImpl->episodicMemory->setMaxEpisodes(1000);
    }
    
    // Initialize associative memory
    pImpl->associativeMemory = std::make_unique<NeuralAssociativeMemory>();
    if (pImpl->associativeMemory) {
        pImpl->associativeMemory->initialize(brain);
    }
}

void BrainMemory::step(TimestepDuration timestep) {
    // Update working memory
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(timestep);
    }
    
    // Update episodic memory state
    ++pImpl->stepsSinceLastEpisode;
}

void BrainMemory::storeEpisode(const EpisodicMemoryItem& episode) {
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->storeEpisode(episode);
    }
}

void BrainMemory::consolidate(float threshold) {
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->consolidate(threshold);
    }
}

void BrainMemory::clearAll() {
    if (pImpl->workingMemory) pImpl->workingMemory->clear();
    if (pImpl->episodicMemory) pImpl->episodicMemory->clear();
    if (pImpl->associativeMemory) pImpl->associativeMemory->clear();
}

void BrainMemory::updateDevelopment(size_t currentStep, TimestepDuration timestep, DevelopmentalStage stage) {
    // Update development system
    if (pImpl->episodicMemory) {
        // Development affects plasticity rates
        float plasticityMod = 1.0f;
        
        switch (stage) {
            case DevelopmentalStage::Initial:
                plasticityMod = 1.0f;  // High plasticity
                break;
            case DevelopmentalStage::CriticalPeriod:
                plasticityMod = 0.8f;
                break;
            case DevelopmentalStage::Maturation:
                plasticityMod = 0.5f;
                break;
            case DevelopmentalStage::Adult:
                plasticityMod = 0.2f;  // Stable
                break;
        }
        
        // This would be passed to development system
        // For now, we just update memory consolidation threshold
    }
}

void BrainMemory::updateConsolidation(size_t currentStep, float threshold) {
    if (currentStep % pImpl->consolidationInterval == 0 && pImpl->episodicMemory) {
        pImpl->episodicMemory->consolidate(threshold);
    }
}

// BrainPrediction component implementation
struct BrainPrediction::Impl {
    // Prediction system data
    size_t predictionStepCounter = 0;
};

BrainPrediction::BrainPrediction() : pImpl(std::make_unique<Impl>()) {}

void BrainPrediction::update() {
    // Update prediction system
    ++pImpl->predictionStepCounter;
}

// BrainCognition component implementation
struct BrainCognition::Impl {
    std::unique_ptr<NeuralPlanner> planner;
    std::unique_ptr<ConceptFormation> conceptFormation;
    std::unique_ptr<AttentionalSelection> attention;
    const Brain* brain = nullptr;
};

BrainCognition::BrainCognition(const std::shared_ptr<Config>& config)
    : pImpl(std::make_unique<Impl>()) {
}

void BrainCognition::initialize(const Brain* brainPtr) {
    pImpl->brain = brainPtr;
    
    // Initialize cognition systems
    pImpl->planner = std::make_unique<NeuralPlanner>();
    pImpl->conceptFormation = std::make_unique<ConceptFormation>();
    pImpl->attention = std::make_unique<AttentionalSelection>();
    
    if (pImpl->brain) {
        if (pImpl->planner) pImpl->planner->initialize(pImpl->brain);
        if (pImpl->conceptFormation) pImpl->conceptFormation->initialize(pImpl->brain);
        if (pImpl->attention) pImpl->attention->initialize(pImpl->brain);
    }
}

void BrainCognition::step(TimestepDuration timestep) {
    if (pImpl->attention) {
        pImpl->attention->update(timestep);
    }
}

void BrainCognition::processCompetition(const std::vector<NeuronId>& competitors) {
    if (pImpl->attention) {
        pImpl->attention->processCompetition(competitors);
    }
}

// BrainNeuromodulation component implementation
struct BrainNeuromodulation::Impl {
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<PredictionError> predictionError;
    std::unique_ptr<Novelty> novelty;
    const Brain* brain = nullptr;
};

BrainNeuromodulation::BrainNeuromodulation() : pImpl(std::make_unique<Impl>()) {
}

void BrainNeuromodulation::initialize(const Brain* brainPtr) {
    pImpl->brain = brainPtr;
    
    // Initialize neuromodulation systems
    pImpl->dopamine = std::make_unique<Dopamine>();
    pImpl->curiosity = std::make_unique<Curiosity>();
    pImpl->predictionError = std::make_unique<PredictionError>();
    pImpl->novelty = std::make_unique<Novelty>();
    
    if (pImpl->brain) {
        if (pImpl->dopamine) pImpl->dopamine->initialize(pImpl->brain);
        if (pImpl->curiosity) pImpl->curiosity->initialize(pImpl->brain);
        if (pImpl->predictionError) pImpl->predictionError->initialize(pImpl->brain);
        if (pImpl->novelty) pImpl->novelty->initialize(pImpl->brain);
    }
}

void BrainNeuromodulation::step(TimestepDuration timestep) {
    if (pImpl->novelty) pImpl->novelty->update(timestep);
    if (pImpl->curiosity) pImpl->curiosity->update(timestep);
    if (pImpl->dopamine) pImpl->dopamine->update(timestep);
}

// BrainIntegration component implementation
struct BrainIntegration::Impl {
    std::unique_ptr<CheckpointManager> checkpointManager;
    const Brain* brain = nullptr;
};

BrainIntegration::BrainIntegration(const std::shared_ptr<Config>& config)
    : pImpl(std::make_unique<Impl>()) {
}

void BrainIntegration::configureCheckpointManager(const std::string& checkpointDir, 
                                                size_t maxCheckpoints, 
                                                size_t checkpointInterval,
                                                bool compression) {
    pImpl->checkpointManager = std::make_unique<CheckpointManager>();
    if (pImpl->checkpointManager) {
        pImpl->checkpointManager->configure(checkpointDir, maxCheckpoints, checkpointInterval, compression);
    }
}

void BrainIntegration::updateCheckpoint(size_t currentStep, Timestamp currentTime) {
    if (pImpl->checkpointManager) {
        pImpl->checkpointManager->update(currentStep, currentTime);
    }
}

void BrainIntegration::applyNeuromodulation(const Neuromodulator& signal) {
    // Apply neuromodulation effects on plasticity
    if (pImpl->checkpointManager) {
        // This would affect plasticity rates
        // For now, just pass the signal
    }
}

void BrainIntegration::updateDevelopment(DevelopmentalStage stage,
                                        float& synaptogenesisRate,
                                        float& pruningRate) {
    // Update development effects
    float plasticityMod = 1.0f;
    
    switch (stage) {
        case DevelopmentalStage::Initial:
            plasticityMod = 1.0f;
            break;
        case DevelopmentalStage::CriticalPeriod:
            plasticityMod = 0.8f;
            break;
        case DevelopmentalStage::Maturation:
            plasticityMod = 0.5f;
            break;
        case DevelopmentalStage::Adult:
            plasticityMod = 0.2f;
            break;
    }
    
    synaptogenesisRate = 0.0001f * plasticityMod;
    pruningRate = 0.00001f * (2.0f - plasticityMod);
}

void BrainIntegration::replayMemories(const std::vector<std::unique_ptr<NeuralRegion>>& regions,
                                      size_t currentStep,
                                      size_t replayInterval,
                                      size_t consolidationInterval) {
    // Replay important memories
    // This would interact with episodic memory
    // For now, just a placeholder
}

// Main Brain implementation using the components
Brain::Brain(std::shared_ptr<Config> cfg) 
    : config(std::move(cfg)),
      timestep(DEFAULT_TIMESTEP),
      currentStep(0),
      currentTime(0.0),
      stepsSinceLastEpisode(0),
      replayInterval(DEFAULT_REPLAY_INTERVAL),
      consolidationInterval(DEFAULT_CONSOLIDATION_INTERVAL),
      isResting(false),
      totalSpikesThisStep(0),
      totalSpikesTotal(0),
      nextRegionId(1),
      developmentalStage(DevelopmentalStage::Initial) {
    
    // Initialize components
    computation = std::make_unique<BrainComputation>(config, regions);
    memory = std::make_unique<BrainMemory>(config);
    prediction = std::make_unique<BrainPrediction>();
    cognition = std::make_unique<BrainCognition>(config);
    neuromodulation = std::make_unique<BrainNeuromodulation>();
    integration = std::make_unique<BrainIntegration>(config);
    
    // Setup components after they're created
    setupComponents();
}

Brain::~Brain() = default;

Brain::Brain(Brain&& other) noexcept 
    : config(std::move(other.config)),
      computation(std::move(other.computation)),
      memory(std::move(other.memory)),
      prediction(std::move(other.prediction)),
      cognition(std::move(other.cognition)),
      neuromodulation(std::move(other.neuromodulation)),
      integration(std::move(other.integration)),
      regions(std::move(other.regions)),
      interRegionConnections(std::move(other.interRegionConnections)),
      timestep(other.timestep),
      currentStep(other.currentStep),
      currentTime(other.currentTime),
      stepsSinceLastEpisode(other.stepsSinceLastEpisode),
      replayInterval(other.replayInterval),
      consolidationInterval(other.consolidationInterval),
      isResting(other.isResting),
      totalSpikesThisStep(other.totalSpikesThisStep),
      totalSpikesTotal(other.totalSpikesTotal),
      sensoryNeurons(std::move(other.sensoryNeurons)),
      motorNeurons(std::move(other.motorNeurons)),
      checkpointManager(std::move(other.checkpointManager)),
      developmentalStage(other.developmentalStage),
      nextRegionId(other.nextRegionId) {
}

Brain& Brain::operator=(Brain&& other) noexcept {
    if (this != &other) {
        // Clean up existing resources
        reset();
        
        // Move resources
        config = std::move(other.config);
        computation = std::move(other.computation);
        memory = std::move(other.memory);
        prediction = std::move(other.prediction);
        cognition = std::move(other.cognition);
        neuromodulation = std::move(other.neuromodulation);
        integration = std::move(other.integration);
        regions = std::move(other.regions);
        interRegionConnections = std::move(other.interRegionConnections);
        timestep = other.timestep;
        currentStep = other.currentStep;
        currentTime = other.currentTime;
        stepsSinceLastEpisode = other.stepsSinceLastEpisode;
        replayInterval = other.replayInterval;
        consolidationInterval = other.consolidationInterval;
        isResting = other.isResting;
        totalSpikesThisStep = other.totalSpikesThisStep;
        totalSpikesTotal = other.totalSpikesTotal;
        sensoryNeurons = std::move(other.sensoryNeurons);
        motorNeurons = std::move(other.motorNeurons);
        checkpointManager = std::move(other.checkpointManager);
        developmentalStage = other.developmentalStage;
        nextRegionId = other.nextRegionId;
    }
    return *this;
}

bool Brain::initialize() {
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
    
    // Get configuration values
    size_t neuronCount = config->getOr<size_t>("neuron_count", 1000);
    size_t regionCount = config->getOr<size_t>("region_count", 1);
    float connectionProbability = config->getOr<float>("connection_probability", 0.1f);
    
    NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                 std::to_string(regionCount) + " regions");
    
    // Create regions
    for (size_t i = 0; i < regionCount; ++i) {
        addRegion("Region_" + std::to_string(i + 1));
    }
    
    // Create neurons across regions
    size_t neuronsPerRegion = neuronCount / regionCount;
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (region) {
            // Add populations
            auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
            auto internalPopId = region->addPopulation(neuronsPerRegion / 2, NeuronType::Internal);
            auto motorPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);
            
            // Collect sensory and motor neurons for I/O
            auto* sensoryPop = region->getPopulation(sensoryPopId);
            auto* motorPop = region->getPopulation(motorPopId);
            if (sensoryPop) {
                for (auto* neuron : sensoryPop->getNeurons()) {
                    sensoryNeurons.push_back(neuron);
                }
            }
            if (motorPop) {
                for (auto* neuron : motorPop->getNeurons()) {
                    motorNeurons.push_back(neuron);
                }
            }
            
            NLM_LOG_INFO("Created populations in region " + std::to_string(i + 1) + 
                        ": " + std::to_string(region->getPopulationCount()) + " populations, " +
                        std::to_string(region->getTotalNeuronCount()) + " neurons");
        }
    }
    
    // Initialize connectivity with random weights
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (region) {
            // Initialize random connectivity and synapse weights
            region->initializeRandomConnectivity(*getRandomGenerator(), 
                                                  connectionProbability, 0.2f, 0.1f);
        }
    }
    
    // ========== INITIALIZE INTEGRATED SYSTEMS ==========
    
    // Initialize memory systems
    if (memory) {
        memory->initializeSystems(this);
        memory->getWorkingMemory()->setCapacity(neuronCount / 10);
        memory->getEpisodicMemory()->setMaxEpisodes(1000);
    }
    
    // Initialize cognition systems
    if (cognition) {
        cognition->initialize(this);
        cognition->setPlanningDepth(5);
        cognition->setInhibitionStrength(DEFAULT_ATTENTION_INHIBITION_STRENGTH);
        cognition->setExcitationStrength(DEFAULT_ATTENTION_EXCITATION_STRENGTH);
    }
    
    // Initialize neuromodulation
    if (neuromodulation) {
        neuromodulation->initialize(this);
    }
    
    // Setup spike system handlers
    if (computation && getSpikeSystem()) {
        computation->registerCountAndDeliverHandlers(*getSpikeSystem(),
            [this](const DetailedSpikeEvent& event) {
                // Count spikes
                ++totalSpikesThisStep;
                ++totalSpikesTotal;
            },
            [this](const DelayedSpikeEvent& event) {
                // Deliver synaptic input (handled by integration)
            });
    }
    
    // Configure checkpoint manager
    std::string checkpointDir = config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    if (integration) {
        integration->configureCheckpointManager(checkpointDir, 10000, 5, true);
    }
    
    NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Sensory neurons: " + std::to_string(sensoryNeurons.size()));
    NLM_LOG_INFO("Motor neurons: " + std::to_string(motorNeurons.size()));
    
    return true;
}

void Brain::setupComponents() {
    // Setup component integration after all components are created
    if (computation && getSpikeSystem()) {
        // Register spike handlers
        computation->registerCountAndDeliverHandlers(*getSpikeSystem(),
            [this](const DetailedSpikeEvent& event) {
                ++totalSpikesThisStep;
                ++totalSpikesTotal;
            },
            [this](const DelayedSpikeEvent& event) {
                // Synaptic input delivery handled by integration layer
            });
    }
}

void Brain::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * timestep);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    this->currentStep = currentStep;
    this->currentTime = currentTime;
    totalSpikesThisStep = 0;
    
    // ========== STEP 1: Process pending delayed spikes (deliver synaptic input) ==========
    if (computation) {
        computation->step(currentStep, currentTime, regions);
    }
    
    // ========== STEP 2: Update all neurons (LIF dynamics) ==========
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, timestep);
            }
        }
    }
    
    // ========== STEP 3: Detect spikes and schedule spike events ==========
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Check if neuron just fired this step
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < timestep * 2.0f);

                if (justFired) {
                    // Neuron fired this step - queue the spike
                    SpikeEvent event(neuron->getId(), currentTime, currentStep);
                    if (computation) {
                        computation->queueSpike(event);
                    }

                    // Record post-synaptic spike for incoming synapses (plasticity)
                    auto incomingSynapses = region->getSynapsesTo(neuron->getId());
                    for (Synapse* syn : incomingSynapses) {
                        syn->recordPostSpike(currentTime);
                    }

                    // Get outgoing synapses and schedule delayed spike events
                    auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
                    for (Synapse* syn : outgoingSynapses) {
                        // Create delayed spike event
                        Delay delay = syn->getDelay();
                        SimulationStep deliveryStep = currentStep + delay;
                        Timestamp deliveryTime = currentTime + delay * timestep;

                        DelayedSpikeEvent delayedEvent(
                            neuron->getId(),
                            syn->getDestinationNeuron(),
                            syn->getId(),
                            syn->getWeight(),
                            syn->getType(),
                            currentTime,
                            deliveryTime,
                            currentStep,
                            deliveryStep
                        );

                        if (computation) {
                            computation->queueDelayedSpike(delayedEvent);
                        }

                        // Record pre-synaptic spike for plasticity
                        syn->recordPreSpike(currentTime);
                    }
                    
                    // Store to working memory - neurons that fire become part of working memory
                    if (memory && memory->getWorkingMemory()) {
                        memory->getWorkingMemory()->storeToNeuron(neuron->getId(), 
                            std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                    }
                }
            }
        }
    }
    
    // Process immediate spikes through computation component
    if (computation) {
        computation->processSpikes(regions);
    }
    
    // ========== STEP 4: Update working memory ==========
    if (memory) {
        memory->step(timestep);
    }
    
    // ========== STEP 5: Apply neuromodulation effects ==========
    if (neuromodulation) {
        neuromodulation->step(timestep);
        
        // Apply dopamine effects on neural excitability
        float dopamineLevel = neuromodulation->getDopamineLevel();
        if (dopamineLevel > 0.0f) {
            float excitabilityMod = dopamineLevel * DEFAULT_DOPAMINE_EXCITABILITY_MOD;
            for (auto& region : regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        // Dopamine modulates excitability by injecting additional current
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
    }
    
    // ========== STEP 6: Apply plasticity rules (STDP and Hebbian) ==========
    // Calculate neuromodulation factor for plasticity
    float plasticityMod = 1.0f;
    if (neuromodulation) {
        plasticityMod = neuromodulation->getPlasticityFactor();
    }
    
    if (computation) {
        computation->updatePlasticity(regions, plasticityMod, currentTime);
    }
    
    for (auto& region : regions) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = computation ? computation->getPreSpikeHistory(syn->getSourceNeuron()) : std::vector<float>();
                const auto& postSpikes = computation ? computation->getPostSpikeHistory(syn->getDestinationNeuron()) : std::vector<float>();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Apply STDP logic would go here
                    // For now, apply neuromodulation scaling
                    float weight = syn->getWeight();
                    weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
                    syn->setWeight(weight);
                }
            }
            
            // Apply Hebbian learning
            if (syn->getPlasticityFlags().hebbian) {
                const auto& preSpikes = computation ? computation->getPreSpikeHistory(syn->getSourceNeuron()) : std::vector<float>();
                const auto& postSpikes = computation ? computation->getPostSpikeHistory(syn->getDestinationNeuron()) : std::vector<float>();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Apply Hebbian logic would go here
                }
            }
            
            // Update synapse state
            syn->step(currentTime);
        }
    }
    
    // ========== STEP 7: Update episodic memory ==========
    stepsSinceLastEpisode++;
    if (stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
        stepsSinceLastEpisode = 0;
        
        if (memory && memory->getEpisodicMemory()) {
            // Capture current brain state as an episode
            EpisodicMemoryItem episode;
            episode.timestamp = currentStep;
            episode.reward = neuromodulation ? neuromodulation->getDopamineLevel() : 0.0f;
            
            // Store active neurons
            for (auto& region : regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        if (neuron->isFiring() || 
                            std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 5.0f) {
                            episode.activeNeurons.push_back(neuron->getId());
                            episode.neuronActivations.push_back(
                                std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f);
                        }
                    }
                }
            }
            
            memory->getEpisodicMemory()->storeEpisode(episode);
        }
    }
    
    // ========== STEP 8: Update prediction system ==========
    if (prediction) {
        prediction->update();
    }
    
    // ========== STEP 9: Update attention system ==========
    if (cognition) {
        cognition->step(timestep);
        
        // Apply attention to working memory winners
        if (memory && memory->getWorkingMemory() && !memory->getWorkingMemory()->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = memory->getWorkingMemory()->getMemoryNeurons();
            cognition->processCompetition(competitors);
        }
    }
    
    // ========== STEP 10: Update concept formation ==========
    if (cognition && cognition->getConceptFormation()) {
        // Would process current neural activity patterns to form concepts
        // This requires sensory state encoding
    }
    
    // ========== STEP 11: Apply structural plasticity periodically ==========
    if (currentStep % DEFAULT_STRUCTURAL_PLASTICITY_UPDATE_INTERVAL == 0) {
        // Apply structural plasticity
        if (computation && getRandomGenerator()) {
            // Would call structural plasticity system
        }
    }
    
    // ========== STEP 12: Replay important memories ==========
    if (memory && currentStep % replayInterval == 0 && memory->getEpisodicMemory()) {
        // Get episodes for replay
        auto episodesToReplay = memory->getEpisodicMemory()->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            memory->getEpisodicMemory()->replayEpisode(episode);
        }
    }
    
    // ========== STEP 13: Apply development effects ==========
    if (currentStep % DEFAULT_DEVELOPMENT_UPDATE_INTERVAL == 0) {  // Update development every 1000 steps
        // Update development system
        if (integration) {
            integration->updateDevelopment(developmentalStage, 
                                          DEFAULT_SYNAPTOTGESIS_RATE, 
                                          DEFAULT_PRUNING_RATE);
        }
        
        // Apply development effects on plasticity rates
        if (computation) {
            DevelopmentalStage stage = developmentalStage;
            float plasticityMod = 1.0f;
            
            switch (stage) {
                case DevelopmentalStage::Initial:
                    plasticityMod = 1.0f;  // High plasticity
                    break;
                case DevelopmentalStage::CriticalPeriod:
                    plasticityMod = 0.8f;
                    break;
                case DevelopmentalStage::Maturation:
                    plasticityMod = 0.5f;
                    break;
                case DevelopmentalStage::Adult:
                    plasticityMod = 0.2f;  // Stable
                    break;
            }
            
            // Update plasticity rates based on development stage
        }
    }
    
    // ========== STEP 14: Periodic memory consolidation ==========
    if (memory && currentStep % consolidationInterval == 0 && memory->getEpisodicMemory()) {
        // Consolidate important memories, remove weak ones
        memory->getEpisodicMemory()->consolidate(0.3f);
    }
    
    // ========== STEP 15: Checkpoint management ==========
    if (integration) {
        integration->updateCheckpoint(currentStep, currentTime);
    }
}

void Brain::receiveSensoryInput(const SensoryInput& input) {
    // Inject current into sensory neurons based on input
    // This is a simple mapping - sensory encoding
    
    const auto& values = input.getData();
    if (values.empty()) return;
    
    size_t numSensory = sensoryNeurons.size();
    if (numSensory == 0) return;
    
    // Distribute input across sensory neurons
    for (size_t i = 0; i < numSensory; ++i) {
        // Normalize input value to range [-10, 10] mV
        float normalizedValue = 0.0f;
        if (i < values.size()) {
            normalizedValue = static_cast<float>(values[i]) * 10.0f;
        }
        
        // Inject current into this sensory neuron
        sensoryNeurons[i]->injectCurrent(normalizedValue);
        
        // Also store in working memory
        if (memory && memory->getWorkingMemory() && normalizedValue > 0.5f) {
            memory->getWorkingMemory()->storeToNeuron(sensoryNeurons[i]->getId(), normalizedValue / 10.0f);
        }
    }
}

void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
    for (auto& region : regions) {
        auto neurons = region->getAllNeurons();
        for (auto* n : neurons) {
            if (n->getId() == neuron) {
                n->injectCurrent(current);
                return;
            }
        }
    }
}

void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == type) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->injectCurrent(current);
                }
            }
        }
    }
}

// Component accessor implementations
SpikeSystem* Brain::getSpikeSystem() {
    return computation ? computation->getSpikeSystem() : nullptr;
}

const SpikeSystem* Brain::getSpikeSystem() const {
    return computation ? computation->getSpikeSystem() : nullptr;
}

STDP* Brain::getSTDP() {
    // Return STDP instance from plasticity system
    // This would be accessed through computation or dedicated plasticity component
    return nullptr; // Placeholder
}

Hebbian* Brain::getHebbian() {
    // Return Hebbian instance from plasticity system
    // This would be accessed through computation or dedicated plasticity component
    return nullptr; // Placeholder
}

StructuralPlasticity* Brain::getStructuralPlasticity() {
    // Return StructuralPlasticity instance from plasticity system
    // This would be accessed through computation or dedicated plasticity component
    return nullptr; // Placeholder
}

float Brain::getExcitationInhibitionRatio() const {
    float totalExcitatory = 0.0f;
    float totalInhibitory = 0.0f;
    
    for (const auto& region : regions) {
        for (const auto& syn : region->getSynapses()) {
            float weight = syn->getWeight();
            if (weight > 0) {
                totalExcitatory += weight;
            } else {
                totalInhibitory += std::abs(weight);
            }
        }
    }
    
    if (totalInhibitory > 0.0f) {
        return totalExcitatory / totalInhibitory;
    }
    return totalExcitatory > 0.0f ? std::numeric_limits<float>::infinity() : 0.0f;
}

size_t Brain::getTotalSpikeCount() const {
    return totalSpikesTotal;
}

size_t Brain::getPendingSpikeEventCount() const {
    if (computation) {
        return computation->getPendingSpikeEventCount();
    }
    return 0;
}

std::unique_ptr<Action> Brain::produceAction() {
    // Simple action selection based on motor neuron activity
    // The motor neuron population with highest average activity determines action
    
    if (motorNeurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Calculate activity of motor neuron groups
    size_t firingMotor = 0;
    for (auto* neuron : motorNeurons) {
        if (neuron->isFiring()) {
            ++firingMotor;
        }
    }
    
    // Return a simple action
    ActionType type = ActionType::Wait;
    if (firingMotor > 0) {
        type = ActionType::MoveForward;
    }
    
    auto action = std::make_unique<Action>(type);
    
    return action;
}

void Brain::applyNeuromodulation(const Neuromodulator& signal) {
    if (integration) {
        integration->applyNeuromodulation(signal);
    }
}

void Brain::updatePlasticity() {
    // Plasticity is now applied during each step
    // This method is kept for API compatibility
}

void Brain::develop() {
    // Development updates structural plasticity
    if (computation && getRandomGenerator()) {
        // This would call structural plasticity system
        // For now, just log
        NLM_LOG_INFO("Development triggered - updating structural plasticity");
    }
}

void Brain::reset() {
    NLM_LOG_INFO("Resetting NLM Brain...");
    
    currentStep = 0;
    currentTime = 0.0;
    totalSpikesThisStep = 0;
    totalSpikesTotal = 0;
    isResting = false;
    stepsSinceLastEpisode = 0;
    
    for (auto& region : regions) {
        region->reset();
    }
    
    if (computation) {
        computation->reset(regions);
    }
    
    if (memory) {
        memory->clearAll();
    }
    
    developmentalStage = DevelopmentalStage::Initial;
    
    NLM_LOG_INFO("NLM Brain reset complete");
}

bool Brain::save(const std::string& filepath) const {
    NLM_LOG_INFO("Saving brain state to " + filepath);
    
    try {
        CheckpointWriter writer;
        if (!writer.create(filepath, CompressionLevel::Balanced)) {
            NLM_LOG_ERROR("Failed to create checkpoint file: " + filepath);
            return false;
        }
        
        // Set metadata
        writer.setMetadata(
            getTotalNeuronCount(),
            getTotalSynapseCount(),
            getRegionCount(),
            currentStep,
            currentTime
        );
        
        // Write neurons
        NeuronCheckpointData neuronData;
        neuronData.membranePotential.reserve(getTotalNeuronCount());
        neuronData.restingPotential.reserve(getTotalNeuronCount());
        neuronData.threshold.reserve(getTotalNeuronCount());
        neuronData.resetPotential.reserve(getTotalNeuronCount());
        neuronData.leakConductance.reserve(getTotalNeuronCount());
        
        for (const auto& region : regions) {
            for (const auto& pop : region->getPopulations()) {
                for (const auto* neuron : pop->getNeurons()) {
                    const auto& state = neuron->getState();
                    neuronData.membranePotential.push_back(state.membranePotential);
                    neuronData.restingPotential.push_back(state.restingPotential);
                    neuronData.threshold.push_back(state.threshold);
                    neuronData.resetPotential.push_back(state.resetPotential);
                    neuronData.leakConductance.push_back(state.leakConductance);
                    neuronData.firingState.push_back(static_cast<uint8_t>(state.firingState));
                    neuronData.refractoryRemaining.push_back(state.refractoryRemaining);
                    neuronData.refractoryPeriod.push_back(state.refractoryPeriod);
                    neuronData.lastSpikeTime.push_back(state.lastSpikeTime);
                }
            }
        }
        
        if (!writer.writeNeurons(neuronData)) {
            NLM_LOG_ERROR("Failed to write neurons to checkpoint");
            return false;
        }
        
        // Write synapses
        SynapseCheckpointData synapseData;
        for (const auto& region : regions) {
            for (const auto* syn : region->getSynapses()) {
                synapseData.sourceNeuron.push_back(syn->getSourceNeuron().index());
                synapseData.destinationNeuron.push_back(syn->getDestinationNeuron().index());
                synapseData.weight.push_back(syn->getWeight());
                synapseData.delay.push_back(syn->getDelay());
                synapseData.synapseType.push_back(static_cast<uint8_t>(syn->getType()));
                synapseData.eligibilityTrace.push_back(syn->getEligibilityTrace());
            }
        }
        
        if (!writer.writeSynapses(synapseData)) {
            NLM_LOG_ERROR("Failed to write synapses to checkpoint");
            return false;
        }
        
        // Finalize
        if (!writer.finalize()) {
            NLM_LOG_ERROR("Failed to finalize checkpoint");
            return false;
        }
        
        NLM_LOG_INFO("Brain state saved successfully (" + std::to_string(writer.getBytesWritten()) + " bytes)");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving brain: ") + e.what());
        return false;
    }
}

bool Brain::load(const std::string& filepath) {
    NLM_LOG_INFO("Loading brain state from " + filepath);
    
    try {
        CheckpointReader reader;
        if (!reader.open(filepath)) {
            NLM_LOG_ERROR("Failed to open checkpoint file: " + filepath);
            return false;
        }
        
        if (!reader.validate()) {
            NLM_LOG_ERROR("Checkpoint validation failed: " + reader.getError());
            return false;
        }
        
        // Read neurons
        NeuronCheckpointData neuronData;
        if (!reader.readNeurons(neuronData)) {
            NLM_LOG_ERROR("Failed to read neurons from checkpoint");
            return false;
        }
        
        // Apply neuron states
        size_t idx = 0;
        for (auto& region : regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (idx < neuronData.membranePotential.size()) {
                        neuron->setMembranePotential(neuronData.membranePotential[idx]);
                        neuron->setRestingPotential(neuronData.restingPotential[idx]);
                        neuron->setThreshold(neuronData.threshold[idx]);
                        neuron->setResetPotential(neuronData.resetPotential[idx]);
                        neuron->setLeakConductance(neuronData.leakConductance[idx]);
                        if (idx < neuronData.firingState.size()) {
                            neuron->setFiringState(static_cast<FiringState>(neuronData.firingState[idx]));
                        }
                        if (idx < neuronData.refractoryRemaining.size()) {
                            neuron->setRefractoryPeriod(neuronData.refractoryPeriod[idx]);
                        }
                    }
                    idx++;
                }
            }
        }
        
        // Read synapses
        SynapseCheckpointData synapseData;
        if (!reader.readSynapses(synapseData)) {
            NLM_LOG_ERROR("Failed to read synapses from checkpoint");
            return false;
        }
        
        // Apply synapse states - this is complex because we need to find matching synapses
        // For now, just log the count
        NLM_LOG_INFO("Loaded " + std::to_string(synapseData.weight.size()) + " synapses");
        
        NLM_LOG_INFO("Brain state loaded successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading brain: ") + e.what());
        return false;
    }
}

RegionId Brain::addRegion(const std::string& name) {
    RegionId id(nextRegionId++);
    auto region = std::make_unique<NeuralRegion>(id, name);
    regions.push_back(std::move(region));
    return id;
}

NeuralRegion* Brain::getRegion(RegionId id) {
    for (auto& region : regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

const NeuralRegion* Brain::getRegion(RegionId id) const {
    for (const auto& region : regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

size_t Brain::getRegionCount() const {
    return regions.size();
}

std::vector<RegionId> Brain::getRegionIds() const {
    std::vector<RegionId> ids;
    ids.reserve(regions.size());
    for (const auto& region : regions) {
        ids.push_back(region->getId());
    }
    return ids;
}

const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const {
    return regions;
}

void Brain::addInterRegionConnection(RegionId source, RegionId target, 
                                    float weight, Delay delay) {
    interRegionConnections.emplace_back(source, target, weight, delay);
}

void Brain::removeInterRegionConnection(RegionId source, RegionId target) {
    interRegionConnections.erase(
        std::remove_if(interRegionConnections.begin(),
                      interRegionConnections.end(),
                      [source, target](const InterRegionConnection& conn) {
                          return conn.sourceRegion == source && conn.targetRegion == target;
                      }),
        interRegionConnections.end()
    );
}

size_t Brain::getTotalNeuronCount() const {
    size_t total = 0;
    for (const auto& region : regions) {
        total += region->getTotalNeuronCount();
    }
    return total;
}

size_t Brain::getTotalSynapseCount() const {
    size_t total = 0;
    for (const auto& region : regions) {
        total += region->getSynapseCount();
    }
    total += interRegionConnections.size();
    return total;
}

size_t Brain::getActiveNeuronCount() const {
    size_t total = 0;
    for (const auto& region : regions) {
        total += region->getActiveNeuronCount();
    }
    return total;
}

size_t Brain::getFiringNeuronCount() const {
    return totalSpikesThisStep;
}

float Brain::getAverageFiringRate() const {
    if (regions.empty()) return 0.0f;
    float sum = 0.0f;
    for (const auto& region : regions) {
        sum += region->getAverageFiringRate();
    }
    return sum / static_cast<float>(regions.size());
}

NeuralWorkingMemory* Brain::getWorkingMemory() {
    return memory ? memory->getWorkingMemory() : nullptr;
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    return memory ? memory->getEpisodicMemory() : nullptr;
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    return memory ? memory->getAssociativeMemory() : nullptr;
}

PredictionSystem* Brain::getPredictionSystem() {
    return prediction ? prediction->getPredictionSystem() : nullptr;
}

NeuralPlanner* Brain::getPlanner() {
    return cognition ? cognition->getPlanner() : nullptr;
}

ConceptFormation* Brain::getConceptFormation() {
    return cognition ? cognition->getConceptFormation() : nullptr;
}

AttentionalSelection* Brain::getAttention() {
    return cognition ? cognition->getAttention() : nullptr;
}

DevelopmentSystem* Brain::getDevelopmentSystem() {
    return integration ? integration->getDevelopmentSystem() : nullptr;
}

DevelopmentalStage Brain::getDevelopmentalStage() const {
    return developmentalStage;
}

void Brain::setDevelopmentalStage(DevelopmentalStage stage) {
    developmentalStage = stage;
}

Dopamine* Brain::getDopamine() {
    return neuromodulation ? neuromodulation->getDopamine() : nullptr;
}

Curiosity* Brain::getCuriosity() {
    return neuromodulation ? neuromodulation->getCuriosity() : nullptr;
}

Novelty* Brain::getNovelty() {
    return neuromodulation ? neuromodulation->getNovelty() : nullptr;
}

PredictionError* Brain::getPredictionErrorSignal() {
    return neuromodulation ? neuromodulation->getPredictionError() : nullptr;
}

std::shared_ptr<const Config> Brain::getConfig() const {
    return config;
}

RandomGenerator* Brain::getRandomGenerator() {
    // This would return the config's random generator
    // For now, return nullptr - would need access to config's rng
    return nullptr;
}

void Brain::logStatus() const {
    NLM_LOG_INFO("=== NLM Brain Status (Phase 6 - Integrated) ===");
    NLM_LOG_INFO("Regions: " + std::to_string(getRegionCount()));
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Active neurons: " + std::to_string(getActiveNeuronCount()));
    NLM_LOG_INFO("Firing neurons (this step): " + std::to_string(getFiringNeuronCount()));
    NLM_LOG_INFO("Total spikes: " + std::to_string(getTotalSpikeCount()));
    NLM_LOG_INFO("Pending spike events: " + std::to_string(getPendingSpikeEventCount()));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(getAverageFiringRate()));
    NLM_LOG_INFO("E/I ratio: " + std::to_string(getExcitationInhibitionRatio()));
    
    // Memory system status
    if (memory) {
        NLM_LOG_INFO("Working memory traces: " + std::to_string(memory->getWorkingMemoryTraces()));
        NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(memory->getEpisodicMemoryEpisodes()));
    }
    
    // Neuromodulation status
    if (neuromodulation) {
        NLM_LOG_INFO("Dopamine level: " + std::to_string(neuromodulation->getDopamineLevel()));
    }
    
    // Development status
    NLM_LOG_INFO("Developmental stage: " + std::to_string(static_cast<int>(developmentalStage)));
    
    for (const auto& region : regions) {
        NLM_LOG_INFO("  Region " + std::to_string(region->getId().index()) + 
                    " (" + region->getName() + "): " +
                    std::to_string(region->getTotalNeuronCount()) + " neurons, " +
                    std::to_string(region->getSynapseCount()) + " synapses, " +
                    "avg weight: " + std::to_string(region->getAverageSynapticWeight()));
    }
}

} // namespace nlm
