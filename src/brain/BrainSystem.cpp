#include "BrainSystem.hpp"
#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <stdexcept>

namespace nlm {

BrainSystem::BrainSystem(std::shared_ptr<Config> config) 
    : config_(config)
    , rng_(nullptr)
    , spikeSystem_(nullptr)
    , stdp_(nullptr)
    , hebbian_(nullptr)
    , structuralPlasticity_(nullptr)
    , checkpointManager_(nullptr) {
    if (!checkAllocation(config_, "BrainSystem constructor")) {
        throw std::runtime_error("Failed to initialize BrainSystem");
    }
}

BrainSystem::~BrainSystem() = default;

bool BrainSystem::initialize(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("BrainSystem::initialize called with null brain pointer");
        return false;
    }
    
    NLM_LOG_INFO("Initializing BrainSystem components...");
    
    try {
        // Initialize random generator
        uint64_t seed = 42;  // Default seed
        if (auto seedOpt = config_->get<int64_t>("random_seed")) {
            seed = *seedOpt;
        }
        rng_ = std::make_unique<RandomGenerator>(seed);
        
        // Initialize plasticity systems
        spikeSystem_ = std::make_unique<SpikeSystem>();
        stdp_ = std::make_unique<STDP>();
        hebbian_ = std::make_unique<Hebbian>();
        structuralPlasticity_ = std::make_unique<StructuralPlasticity>();
        
        // Configure STDP parameters
        float ltpWeight = config_->getOr<float>("stdp_ltp_weight", 0.01f);
        float ltdWeight = config_->getOr<float>("stdp_ltd_weight", 0.012f);
        float tau = config_->getOr<float>("stdp_tau", 20.0f);
        if (!configureSTDP(ltpWeight, ltdWeight, tau)) {
            NLM_LOG_WARNING("STDP configuration failed");
        }
        
        // Configure structural plasticity
        float synaptogenesisRate = config_->getOr<float>("synaptogenesis_rate", 0.0001f);
        float pruningRate = config_->getOr<float>("pruning_rate", 0.00001f);
        if (!configureStructuralPlasticity(synaptogenesisRate, pruningRate)) {
            NLM_LOG_WARNING("Structural plasticity configuration failed");
        }
        
        // Initialize checkpoint manager
        setupCheckpointManager();
        
        NLM_LOG_INFO("BrainSystem initialization complete");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception during BrainSystem initialization: ") + e.what());
        return false;
    }
}

void BrainSystem::setupCheckpointManager() {
    if (!checkpointManager_) {
        checkpointManager_ = std::make_unique<CheckpointManager>();
    }
    
    std::string checkpointDir = config_->getOr<std::string>("checkpoint_dir", "./checkpoints");
    checkpointManager_->configure(checkpointDir, 10000, 5, true);
}

bool BrainSystem::configureSTDP(float ltpWeight, float ltdWeight, float tau) {
    if (!stdp_) {
        return false;
    }
    stdp_->configure(ltpWeight, ltdWeight, tau);
    return true;
}

bool BrainSystem::configureStructuralPlasticity(float synaptogenesisRate, float pruningRate) {
    if (!structuralPlasticity_) {
        return false;
    }
    structuralPlasticity_->setSynaptogenesisRate(synaptogenesisRate);
    structuralPlasticity_->setPruningRate(pruningRate);
    return true;
}

void BrainSystem::setupSpikeHandlers(Brain* brain) {
    if (!brain || spikeHandlersRegistered_) {
        return;
    }
    
    spikeHandlersRegistered_ = true;
    
    // Register spike handlers for event-driven processing
    spikeSystem_->registerHandler([brain](const DetailedSpikeEvent& event) {
        // Count spikes
        ++brain->getBrainSystem()->getSimulationState().totalSpikesThisStep;
        ++brain->getBrainSystem()->getSimulationState().totalSpikesTotal;
    });
    
    // Register delayed spike handler to deliver synaptic input
    spikeSystem_->registerDelayedHandler([brain](const DelayedSpikeEvent& event) {
        auto it = brain->getBrainSystem()->getNeuronById().find(event.destination_neuron.index());
        if (it != brain->getBrainSystem()->getNeuronById().end()) {
            Neuron* neuron = it->second;
            MembranePotential synapticCurrent = event.weight * 10.0f;  // Scale factor
            if (event.is_excitatory) {
                neuron->receiveExcitatoryInput(synapticCurrent);
            } else {
                neuron->receiveInhibitoryInput(-synapticCurrent);
            }
        }
    });
}

void BrainSystem::reset() {
    NLM_LOG_INFO("Resetting BrainSystem...");
    
    // Clear all neural components
    regions_.clear();
    interRegionConnections_.clear();
    neuronById_.clear();
    sensoryNeurons_.clear();
    motorNeurons_.clear();
    
    // Reset simulation state
    simulationState_ = SimulationState();
    integrationState_ = IntegrationState();
    
    // Reset spike handlers
    spikeHandlersRegistered_ = false;
    
    // Reset systems
    if (workingMemory_) workingMemory_->clear();
    if (episodicMemory_) episodicMemory_->clear();
    if (associativeMemory_) associativeMemory_->clear();
    if (attention_) attention_->reset();
    
    NLM_LOG_INFO("BrainSystem reset complete");
}

void BrainSystem::logStatus() const {
    NLM_LOG_INFO("BrainSystem Status:");
    NLM_LOG_INFO("  Regions: " + std::to_string(regions_.size()));
    NLM_LOG_INFO("  Sensory neurons: " + std::to_string(sensoryNeurons_.size());
    NLM_LOG_INFO("  Motor neurons: " + std::to_string(motorNeurons_.size());
    NLM_LOG_INFO("  Total neurons: " + std::to_string(neuronById_.size());
    NLM_LOG_INFO("  Spike handlers registered: " + std::to_string(spikeHandlersRegistered_));
    NLM_LOG_INFO("  Simulation step: " + std::to_string(simulationState_.currentStep));
    NLM_LOG_INFO("  Total spikes: " + std::to_string(simulationState_.totalSpikesTotal));
}

void BrainSystem::processStep(Brain* brain) {
    if (!brain || !brain->getBrainSystem()) {
        return;
    }
    
    setupSpikeHandlers(brain);
    
    // Process delayed spikes (deliver synaptic input)
    spikeSystem_->processDelayedSpikes(simulationState_.currentStep, simulationState_.currentTime);
    
    // Process immediate spikes
    spikeSystem_->processSpikes(simulationState_.currentStep);
    
    // Update integration state
    simulationState_.currentStep++;
    simulationState_.currentTime = simulationState_.currentStep * 0.001f;  // Assuming 1ms timestep
    simulationState_.totalSpikesThisStep = 0;
    
    // Update integration counters
    integrationState_.stepsSinceLastEpisode++;
    
    // Periodic memory consolidation
    if (integrationState_.stepsSinceLastEpisode >= 10) {
        integrationState_.stepsSinceLastEpisode = 0;
        if (episodicMemory_) {
            EpisodicMemoryItem episode;
            episode.timestamp = simulationState_.currentStep;
            
            // Store active neurons
            for (auto& region : regions_) {
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
            
            episodicMemory_->storeEpisode(episode);
        }
    }
}

bool BrainSystem::checkAllocation(bool condition, const std::string& context) {
    if (!condition) {
        NLM_LOG_ERROR("Memory allocation failed in " + context);
        return false;
    }
    return true;
}

} // namespace nlm