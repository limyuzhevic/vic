#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <vector>
#include "BrainUtils.cpp"

namespace nlm {

// BrainRegionManager implementation - Region and neuron management
// Extracted from Brain.cpp lines 189-256 and 274-338

class BrainRegionManager {
public:
    // Create brain regions and set up neural populations
    static void setupRegionsAndNeurons(Brain::Impl* impl, size_t neuronCount, size_t regionCount, 
                                      float connectionProbability) {
        // Re-initializing must rebuild, not append: a previous run left
        // reset()+initialize() reporting 1000 neurons for a 500-neuron brain.
        impl->regions.clear();
        impl->sensoryNeurons.clear();
        impl->motorNeurons.clear();
        impl->neuronById.clear();
        impl->nextRegionId = RegionId(1);
        impl->totalSpikesTotal = 0;
        impl->totalSpikesThisStep = 0;
        impl->currentStep = 0;
        impl->currentTime = 0.0;
        
        NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                     std::to_string(regionCount) + " regions");
        
        // Create regions
        for (size_t i = 0; i < regionCount; ++i) {
            addRegion(impl, "Region_" + std::to_string(i + 1));
        }
        
        // Create neurons across regions
        size_t neuronsPerRegion = neuronCount / regionCount;
        size_t nextNeuronIndex = 1;  // globally unique IDs across regions (see setNextNeuronIndex)
        for (size_t i = 0; i < regionCount; ++i) {
            auto* region = getRegion(impl, RegionId(i + 1));
            if (region) {
                region->setNextNeuronIndex(nextNeuronIndex);
                // Add populations. The old layout had no inhibitory neurons at
                // all (Sensory/Internal/Motor are all treated as excitatory by
                // initializeRandomConnectivity), so any input caused runaway
                // excitation and every weight saturated. Split the internal
                // population 80/20 into excitatory and inhibitory neurons.
                auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
                size_t internalCount = neuronsPerRegion / 2;
                size_t internalExcitatory = internalCount * 4 / 5;
                auto internalPopId = region->addPopulation(internalExcitatory, NeuronType::Internal);
                auto inhibitoryPopId = region->addPopulation(internalCount - internalExcitatory, NeuronType::Inhibitory);
                (void)internalPopId; (void)inhibitoryPopId;
                auto motorPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);
                
                // Collect sensory and motor neurons for I/O
                auto* sensoryPop = region->getPopulation(sensoryPopId);
                auto* motorPop = region->getPopulation(motorPopId);
                if (sensoryPop) {
                    for (auto* neuron : sensoryPop->getNeurons()) {
                        impl->sensoryNeurons.push_back(neuron);
                    }
                }
                if (motorPop) {
                    for (auto* neuron : motorPop->getNeurons()) {
                        impl->motorNeurons.push_back(neuron);
                    }
                }
                
                NLM_LOG_INFO("Created populations in region " + std::to_string(i + 1) +
                             ": " + std::to_string(region->getPopulationCount()) + " populations, " +
                             std::to_string(region->getTotalNeuronCount()) + " neurons");
                nextNeuronIndex += region->getTotalNeuronCount();
            }
        }
        
        // Build the neuron lookup used by synaptic delivery and injection.
        buildNeuronLookup(impl->regions, impl->neuronById, impl->sensoryNeurons, impl->motorNeurons);
        
        // Initialize connectivity with random weights
        for (size_t i = 0; i < regionCount; ++i) {
            auto* region = getRegion(impl, RegionId(i + 1));
            if (region) {
                // Initialize random connectivity and synapse weights
                initializeRegionConnectivity(region, *impl->rng, connectionProbability, 0.2f, 0.1f);
            }
        }
    }
    
    // Initialize all integrated systems
    static void initializeIntegratedSystems(Brain::Impl* impl) {
        // Initialize working memory
        if (impl->workingMemory) {
            impl->workingMemory->initialize(nullptr);
            impl->workingMemory->setCapacity(impl->totalSpikesTotal / 10);
        }
        
        // Initialize episodic memory
        if (impl->episodicMemory) {
            impl->episodicMemory->initialize(nullptr);
            impl->episodicMemory->setMaxEpisodes(1000);
        }
        
        // Initialize associative memory
        if (impl->associativeMemory) {
            impl->associativeMemory->initialize(nullptr);
        }
        
        // Initialize cognition systems
        if (impl->planner) {
            impl->planner->initialize(nullptr);
            impl->planner->setPlanningDepth(5);
        }
        
        if (impl->conceptFormation) {
            impl->conceptFormation->initialize(nullptr);
        }
        
        if (impl->attention) {
            impl->attention->initialize(nullptr);
            impl->attention->setInhibitionStrength(0.5f);
            impl->attention->setExcitationStrength(1.5f);
        }
        
        // Initialize neuromodulation
        if (impl->novelty) {
            impl->novelty->initialize(nullptr);
        }
        
        if (impl->curiosity) {
            impl->curiosity->initialize(nullptr);
        }
        
        // Configure STDP parameters
        float ltpWeight = impl->config->getOr<float>("stdp_ltp_weight", 0.01f);
        float ltdWeight = impl->config->getOr<float>("stdp_ltd_weight", 0.012f);
        float tau = impl->config->getOr<float>("stdp_tau", 20.0f);
        if (impl->stdp) {
            impl->stdp->configure(ltpWeight, ltdWeight, tau);
        }
        
        // Configure structural plasticity
        float synaptogenesisRate = impl->config->getOr<float>("synaptogenesis_rate", 0.0001f);
        float pruningRate = impl->config->getOr<float>("pruning_rate", 0.00001f);
        if (impl->structuralPlasticity) {
            impl->structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
            impl->structuralPlasticity->setPruningRate(pruningRate);
        }
        
        // Get timestep
        impl->timestep = impl->config->getOr<double>("simulation_timestep", 0.001);
        
        // Get integration intervals from config
        impl->replayInterval = static_cast<size_t>(impl->config->getOr<int64_t>("replay_interval", 100));
        impl->consolidationInterval = static_cast<size_t>(impl->config->getOr<int64_t>("consolidation_interval", 1000));
        
        // Configure checkpoint manager
        std::string checkpointDir = impl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
        if (impl->checkpointManager) {
            impl->checkpointManager->configure(checkpointDir, 10000, 5, true);
        }
    }
    
    // Register spike handlers for event-driven processing
    static void registerSpikeHandlers(Brain::Impl* impl) {
        if (!impl->spikeHandlersRegistered) {
            impl->spikeHandlersRegistered = true;
            // Register spike handlers for event-driven processing
            if (impl->spikeSystem) {
                impl->spikeSystem->registerHandler([impl](const DetailedSpikeEvent& event) {
                    // Count spikes
                    ++impl->totalSpikesThisStep;
                    ++impl->totalSpikesTotal;
                });
            }
            
            // Register delayed spike handler to deliver synaptic input
            if (impl->spikeSystem) {
                impl->spikeSystem->registerDelayedHandler([impl](const DelayedSpikeEvent& event) {
                    auto it = impl->neuronById.find(event.destination_neuron.index());
                    if (it != impl->neuronById.end()) {
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
        }
    }
    
private:
    static NeuralRegion* addRegion(Brain::Impl* impl, const std::string& name) {
        RegionId id(impl->nextRegionId++);
        auto region = std::make_unique<NeuralRegion>(id, name);
        auto* rawPtr = region.get();
        impl->regions.push_back(std::move(region));
        return rawPtr;
    }
    
    static NeuralRegion* getRegion(Brain::Impl* impl, RegionId id) {
        for (auto& region : impl->regions) {
            if (region->getId() == id) {
                return region.get();
            }
        }
        return nullptr;
    }
};

} // namespace nlm
