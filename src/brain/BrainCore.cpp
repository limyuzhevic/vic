#include "BrainCore.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <algorithm>

namespace nlm {

BrainCore::BrainCore(std::shared_ptr<Config> config)
    : config_(config)
    , rng_(nullptr)
    , developmentalStage_(DevelopmentalStage::Initial)
    , nextRegionId_(1)
    , timestep_(0.001)
    , currentStep_(0)
    , currentTime_(0.0)
    , totalSpikesThisStep_(0)
    , totalSpikesTotal_(0)
    , isResting_(false)
    , stepsSinceLastEpisode_(0)
    , replayInterval_(100)
    , consolidationInterval_(1000)
{
    // Initialize random generator with seed from config
    uint64_t seed = 42; // Default seed
    if (auto seedOpt = config_->get<uint64_t>("random_seed")) {
        seed = *seedOpt;
    }
    rng_ = std::make_unique<RandomGenerator>(seed);
    
    // Initialize plasticity systems
    spikeSystem_ = std::make_unique<SpikeSystem>();
    stdp_ = std::make_unique<STDP>();
    hebbian_ = std::make_unique<Hebbian>();
    structuralPlasticity_ = std::make_unique<StructuralPlasticity>();
}

void BrainCore::configurePlasticitySystems() {
    """Configure STDP and structural plasticity parameters from config"""
    // Configure STDP parameters
    float ltpWeight = config_->getOr<float>("stdp_ltp_weight", 0.01f);
    float ltdWeight = config_->getOr<float>("stdp_ltd_weight", 0.012f);
    float tau = config_->getOr<float>("stdp_tau", 20.0f);
    stdp_->configure(ltpWeight, ltdWeight, tau);
    
    // Configure structural plasticity
    float synaptogenesisRate = config_->getOr<float>("synaptogenesis_rate", 0.0001f);
    float pruningRate = config_->getOr<float>("pruning_rate", 0.00001f);
    structuralPlasticity_->setSynaptogenesisRate(synaptogenesisRate);
    structuralPlasticity_->setPruningRate(pruningRate);
}

void BrainCore::configureSimulationParameters() {
    """Configure simulation parameters from config"""
    // Get timestep
    timestep_ = config_->getOr<double>("simulation_timestep", 0.001);
    
    // Get integration intervals from config
    replayInterval_ = config_->getOr<size_t>("replay_interval", 100);
    consolidationInterval_ = config_->getOr<size_t>("consolidation_interval", 1000);
}

void BrainCore::initializeNeurons() {
    """Initialize neurons across all regions"""
    // Create regions based on configuration
    size_t regionCount = config_->getOr<size_t>("region_count", 1);
    size_t neuronCount = config_->getOr<size_t>("neuron_count", 1000);
    float connectionProbability = config_->getOr<float>("connection_probability", 0.1f);
    
    NLM_LOG_INFO("BrainCore: Initializing brain with " + std::to_string(neuronCount) + 
                 " neurons across " + std::to_string(regionCount) + " regions");
    
    // Create regions
    for (size_t i = 0; i < regionCount; ++i) {
        regions_.push_back(std::make_unique<NeuralRegion>(RegionId(i + 1), "Region_" + std::to_string(i + 1)));
    }
    
    // Distribute neurons across regions
    size_t neuronsPerRegion = neuronCount / regionCount;
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (region) {
            // Create sensory neurons (25% of population)
            size_t sensoryCount = neuronsPerRegion / 4;
            if (sensoryCount > 0) {
                region->addPopulation(sensoryCount, NeuronType::Sensory);
            }
            
            // Create excitatory neurons (50% of population)
            size_t excitatoryCount = neuronsPerRegion / 2;
            if (excitatoryCount > 0) {
                region->addPopulation(excitatoryCount, NeuronType::Excitatory);
            }
            
            // Create inhibitory neurons (15% of population)
            size_t inhibitoryCount = neuronsPerRegion * 15 / 100;
            if (inhibitoryCount > 0) {
                region->addPopulation(inhibitoryCount, NeuronType::Inhibitory);
            }
            
            // Create motor neurons (10% of population)
            size_t motorCount = neuronsPerRegion / 10;
            if (motorCount > 0) {
                region->addPopulation(motorCount, NeuronType::Motor);
            }
            
            // Create internal neurons (remaining)
            size_t internalCount = neuronsPerRegion - sensoryCount - excitatoryCount - inhibitoryCount - motorCount;
            if (internalCount > 0) {
                region->addPopulation(internalCount, NeuronType::Internal);
            }
        }
    }
    
    // Create synaptic connections between neurons
    createSynapticConnections(connectionProbability);
}

void BrainCore::createSynapticConnections(float connectionProbability) {
    """Create synaptic connections between neurons based on probability"""
    for (const auto& region : regions_) {
        const auto& neurons = region->getAllNeurons();
        size_t neuronCount = neurons.size();
        
        if (neuronCount < 2) continue;
        
        // Create connections between neurons in the same region
        for (size_t i = 0; i < neuronCount; ++i) {
            for (size_t j = i + 1; j < neuronCount; ++j) {
                if (rng_->nextFloat() < connectionProbability) {
                    // Create excitatory connection
                    region->addSynapse(neurons[i], neurons[j], 0.5f, 1, PlasticityFlags::Hebbian | PlasticityFlags::STDP);
                }
            }
        }
    }
}

void BrainCore::collectSensoryNeurons() {
    """Collect sensory neurons for input injection"""
    sensoryNeurons_.clear();
    motorNeurons_.clear();
    
    for (const auto& region : regions_) {
        for (const auto& population : region->getPopulations()) {
            NeuronType type = population->getNeuronType();
            if (type == NeuronType::Sensory) {
                for (Neuron* neuron : population->getNeurons()) {
                    sensoryNeurons_.push_back(neuron);
                }
            } else if (type == NeuronType::Motor) {
                for (Neuron* neuron : population->getNeurons()) {
                    motorNeurons_.push_back(neuron);
                }
            }
        }
    }
}

NeuralRegion* BrainCore::getRegion(RegionId id) {
    for (auto& region : regions_) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

const NeuralRegion* BrainCore::getRegion(RegionId id) const {
    for (const auto& region : regions_) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

void BrainCore::addInterRegionConnection(RegionId source, RegionId target, 
                                       float weight, Delay delay) {
    interRegionConnections_.emplace_back(source, target, weight, delay);
}

void BrainCore::processSimulationStep() {
    """Process a single simulation step"""
    totalSpikesThisStep_ = 0;
    
    // Process neural dynamics
    for (const auto& region : regions_) {
        region->processStep(timestep_);
    }
    
    // Process inter-region connections
    processInterRegionConnections();
    
    // Update plasticity systems
    updatePlasticitySystems();
    
    // Update development system
    develop();
    
    // Update memory systems
    updateMemorySystems();
    
    // Update prediction system
    updatePredictionSystem();
    
    // Increment simulation counters
    currentStep_++;
    currentTime_ += timestep_;
    stepsSinceLastEpisode_++;
    
    // Check for episode end and trigger replay/consolidation
    checkAndTriggerReplayConsolidation();
}

void BrainCore::processInterRegionConnections() {
    """Process inter-region connections"""
    for (const auto& connection : interRegionConnections_) {
        auto* sourceRegion = getRegion(connection.sourceRegion);
        auto* targetRegion = getRegion(connection.targetRegion);
        
        if (!sourceRegion || !targetRegion) continue;
        
        // Process connection with delay
        // Implementation would handle spike propagation with delays
    }
}

void BrainCore::updatePlasticitySystems() {
    """Update all plasticity systems"""
    // Update STDP
    stdp_->update(*spikeSystem_);
    
    // Update Hebbian plasticity
    hebbian_->update(*spikeSystem_);
    
    // Update structural plasticity
    structuralPlasticity_->update(*spikeSystem_);
}

void BrainCore::develop() {
    """Update development system"""
    // Development logic would go here
    // Could include age-dependent changes to neuron properties
    // or region-specific development patterns
}

void BrainCore::updateMemorySystems() {
    """Update all memory systems"""
    // Memory systems would be updated here
    // Based on current neural activity and spike patterns
}

void BrainCore::updatePredictionSystem() {
    """Update prediction system"""
    // Prediction system updates would go here
    // For sensory prediction and error computation
}

void BrainCore::checkAndTriggerReplayConsolidation() {
    """Check if episode should end and trigger replay/consolidation"""
    if (isResting_) {
        // During rest, replay and consolidate memories
        if (stepsSinceLastEpisode_ >= replayInterval_) {
            // Trigger replay
            stepsSinceLastEpisode_ = 0;
        }
        
        if (stepsSinceLastEpisode_ >= consolidationInterval_) {
            // Trigger consolidation
            stepsSinceLastEpisode_ = 0;
        }
    }
}

void BrainCore::logStatus() const {
    """Log current brain status"""
    NLM_LOG_INFO("=== BrainCore Status ===");
    NLM_LOG_INFO("Regions: " + std::to_string(regions_.size()));
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Active neurons: " + std::to_string(getActiveNeuronCount()));
    NLM_LOG_INFO("Firing neurons: " + std::to_string(getFiringNeuronCount()));
    NLM_LOG_INFO("Total spikes this step: " + std::to_string(totalSpikesThisStep_));
    NLM_LOG_INFO("Total spikes total: " + std::to_string(totalSpikesTotal_));
    NLM_LOG_INFO("Simulation time: " + std::to_string(currentTime_) + "s");
    NLM_LOG_INFO("Current step: " + std::to_string(currentStep_));
}

}} // namespace nlm