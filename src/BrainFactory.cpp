// Brain Factory Implementation
//
// This file provides the implementation for the BrainFactory class,
// which creates and initializes the NLM brain with all its integrated systems.

#include "BrainFactory.hpp"
#include "Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

// Configure plasticity systems
inline void BrainFactory::configurePlasticity(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
    if (!brain) return;
    
    auto& impl = *brain->pImpl;
    
    // Configure STDP parameters
    float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
    float ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
    float tau = config->getOr<float>("stdp_tau", 20.0f);
    impl.stdp->configure(ltpWeight, ltdWeight, tau);
    
    // Configure structural plasticity
    float synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.0001f);
    float pruningRate = config->getOr<float>("pruning_rate", 0.00001f);
    impl.structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
    impl.structuralPlasticity->setPruningRate(pruningRate);
}

// Configure memory systems
inline void BrainFactory::configureMemory(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
    if (!brain) return;
    
    auto& impl = *brain->pImpl;
    
    // Initialize working memory
    impl.workingMemory->initialize(brain.get());
    impl.workingMemory->setCapacity(config->getOr<size_t>("working_memory_capacity", 100));
    
    // Initialize episodic memory
    impl.episodicMemory->initialize(brain.get());
    impl.episodicMemory->setMaxEpisodes(config->getOr<size_t>("max_episodes", 1000));
    
    // Initialize associative memory
    impl.associativeMemory->initialize(brain.get());
}

// Configure neuromodulation systems
inline void BrainFactory::configureNeuromodulation(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
    if (!brain) return;
    
    auto& impl = *brain->pImpl;
    
    // Initialize neuromodulators
    impl.dopamine->initialize(brain.get());
    impl.curiosity->initialize(brain.get());
    impl.novelty->initialize(brain.get());
    impl.predictionError->initialize(brain.get());
}

// Configure cognitive systems
inline void BrainFactory::configureCognition(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
    if (!brain) return;
    
    auto& impl = *brain->pImpl;
    
    // Initialize cognitive systems
    impl.planner->initialize(brain.get());
    impl.planner->setPlanningDepth(config->getOr<size_t>("planning_depth", 5));
    
    impl.conceptFormation->initialize(brain.get());
    
    impl.attention->initialize(brain.get());
    impl.attention->setInhibitionStrength(config->getOr<float>("attention_inhibition", 0.5f));
    impl.attention->setExcitationStrength(config->getOr<float>("attention_excitation", 1.5f));
}

// Configure development system
inline void BrainFactory::configureDevelopment(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
    if (!brain) return;
    
    auto& impl = *brain->pImpl;
    impl.developmentSystem->initialize(brain.get());
    
    // Configure development parameters
    float synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.0001f);
    float pruningRate = config->getOr<float>("pruning_rate", 0.00001f);
    impl.developmentSystem->setSynaptogenesisRate(synaptogenesisRate);
    impl.developmentSystem->setPruningRate(pruningRate);
    impl.developmentSystem->setMaturationRate(config->getOr<float>("maturation_rate", 0.01f));
}

// Configure checkpoint system
inline void BrainFactory::configureCheckpointing(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
    if (!brain) return;
    
    auto& impl = *brain->pImpl;
    
    // Configure checkpoint manager
    std::string checkpointDir = config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    int maxCheckpoints = config->getOr<int>("max_checkpoints", 10);
    int checkpointInterval = config->getOr<int>("checkpoint_interval", 1000);
    bool compressionEnabled = config->getOr<bool>("checkpoint_compression", true);
    
    impl.checkpointManager->configure(checkpointDir, maxCheckpoints, checkpointInterval, compressionEnabled);
}

// Create region and populate with neurons
inline void BrainFactory::createBrainRegions(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
    if (!brain) return;
    
    size_t neuronCount = config->getOr<size_t>("neuron_count", 1000);
    size_t regionCount = config->getOr<size_t>("region_count", 1);
    size_t neuronsPerRegion = neuronCount / regionCount;
    
    // Create regions
    for (size_t i = 0; i < regionCount; ++i) {
        brain->addRegion("Region_" + std::to_string(i + 1));
    }
    
    // Add neuron populations to each region
    for (size_t i = 0; i < regionCount; ++i++) {
        auto* region = brain->getRegion(RegionId(i + 1));
        if (region) {
            // Calculate population sizes
            size_t sensoryCount = neuronsPerRegion / 4;
            size_t internalCount = neuronsPerRegion / 2;
            size_t motorCount = neuronsPerRegion / 4;
            
            // Add populations
            auto sensoryPopId = region->addPopulation(sensoryCount, NeuronType::Sensory);
            auto internalPopId = region->addPopulation(internalCount, NeuronType::Internal);
            auto motorPopId = region->addPopulation(motorCount, NeuronType::Motor);
        }
    }
}

// Initialize connectivity and synapses
inline void BrainFactory::initializeConnectivity(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config,
                                       std::unique_ptr<RandomGenerator>& rng) {
    if (!brain || !rng) return;
    
    size_t regionCount = config->getOr<size_t>("region_count", 1);
    float connectionProbability = config->getOr<float>("connection_probability", 0.1f);
    float initialWeightMean = config->getOr<float>("initial_weight_mean", 0.2f);
    float initialWeightStd = config->getOr<float>("initial_weight_std", 0.05f);
    
    // Initialize connectivity for each region
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = brain->getRegion(RegionId(i + 1));
        if (region) {
            region->initializeRandomConnectivity(*rng, connectionProbability, initialWeightMean, initialWeightStd);
        }
    }
}

// Register all event handlers and systems
inline void BrainFactory::registerSystemHandlers(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
    if (!brain) return;
    
    auto& impl = *brain->pImpl;
    
    // Register spike system handlers
    setupSpikeSystem(brain, config);
    
    // Register neuromodulation signal handlers
    setupNeuromodulationSignals(brain);
    
    // Register memory system integration
    setupMemoryIntegration(brain);
    
    // Register prediction system integration
    setupPredictionIntegration(brain);
    
    // Register cognition system integration
    setupCognitionIntegration(brain);
    
    // Register development system integration
    setupDevelopmentIntegration(brain);
}

// Configure development stages
inline void BrainFactory::configureDevelopmentStages(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
    if (!brain) return;
    
    auto& impl = *brain->pImpl;
    
    // Set initial developmental stage
    impl.developmentSystem->setDevelopmentalStage(DevelopmentalStage::Initial);
    
    // Configure development parameters
    impl.developmentSystem->setInitialAge(config->getOr<double>("initial_age", 0.0));
    impl.developmentSystem->setCriticalPeriodEnd(config->getOr<double>("critical_period_end", 60.0));
    impl.developmentSystem->setMaturationEnd(config->getOr<double>("maturation_end", 300.0));
}

// Setup integration intervals
inline void BrainFactory::setupIntegrationIntervals(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
    if (!brain) return;
    
    auto& impl = *brain->pImpl;
    
    // Setup integration intervals from config
    impl.replayInterval = config->getOr<size_t>("replay_interval", 100);
    impl.consolidationInterval = config->getOr<size_t>("consolidation_interval", 1000);
    
    // Setup other integration parameters
    impl.stepsSinceLastEpisode = 0;
    impl.isResting = false;
}

// Create event-driven spike system
inline void BrainFactory::setupSpikeSystem(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
    if (!brain) return;
    
    auto& impl = *brain->pImpl;
    
    // Register spike handlers for event-driven processing
    impl.spikeSystem->registerHandler([brain](const DetailedSpikeEvent& event) {
        // Count spikes
        ++brain->pImpl->totalSpikesThisStep;
        ++brain->pImpl->totalSpikesTotal;
        
        // Store in working memory
        if (brain->pImpl->workingMemory) {
            brain->pImpl->workingMemory->storeToNeuron(event.source_neuron, 1.0f);
        }
    });
    
    // Register delayed spike handler to deliver synaptic input
    impl.spikeSystem->registerDelayedHandler([brain](const DelayedSpikeEvent& event) {
        // Find destination neuron and deliver synaptic input
        for (auto& region : brain->pImpl->regions) {
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

// Create neuromodulation signal handlers
inline void BrainFactory::setupNeuromodulationSignals(std::shared_ptr<Brain> brain) {
    if (!brain) return;
    
    auto& impl = *brain->pImpl;
    
    // Register dopamine effects on neural excitability
    impl.dopamine->registerHandler([brain](float level) {
        // Apply dopamine effects on neural excitability
        // Dopamine modulates neural excitability by adjusting effective current injection
        // Higher dopamine increases excitability (lower effective threshold)
        float excitabilityMod = level * 0.5f;
        if (excitabilityMod > 0.0f) {
            for (auto& region : brain->pImpl->regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        // Dopamine modulates excitability by injecting additional current
                        // Positive dopamine adds excitatory bias
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
    });
}

// Create memory system integration
inline void BrainFactory::setupMemoryIntegration(std::shared_ptr<Brain> brain) {
    if (!brain) return;
    
    auto& impl = *brain->pImpl;
    
    // Register memory storage during step processing
    impl.workingMemory->registerHandler([brain](NeuronId neuronId, float activation) {
        // Store active neurons in working memory
        if (brain->pImpl->workingMemory) {
            brain->pImpl->workingMemory->storeToNeuron(neuronId, activation);
        }
    });
}

// Create prediction system integration
inline void BrainFactory::setupPredictionIntegration(std::shared_ptr<Brain> brain) {
    if (!brain) return;
    
    auto& impl = *brain->pImpl;
    
    // Register prediction system updates
    // This would integrate prediction system with sensory input
    // For now, just set up basic integration
    impl.predictionSystem->registerHandler([brain](float prediction, float error) {
        // Update prediction system with current input
        // Update prediction error signals
        // This would affect neuromodulation
    });
}

// Create cognition system integration
inline void BrainFactory::setupCognitionIntegration(std::shared_ptr<Brain> brain) {
    if (!brain) return;
    
    auto& impl = *brain->pImpl;
    
    // Register attention system integration
    impl.attention->registerHandler([brain](const std::vector<NeuronId>& competitors) {
        // Process competition for attention
        // Update working memory winners
        if (brain->pImpl->workingMemory && !competitors.empty()) {
            brain->pImpl->attention->processCompetition(competitors);
        }
    });
    
    // Register concept formation integration
    impl.conceptFormation->registerHandler([brain](const std::vector<float>& features) {
        // Process current neural patterns for concept formation
        // This would integrate with sensory input
    });
}

// Create development system integration
inline void BrainFactory::setupDevelopmentIntegration(std::shared_ptr<Brain> brain) {
    if (!brain) return;
    
    auto& impl = *brain->pImpl;
    
    // Register development system updates
    impl.developmentSystem->registerHandler([brain](double timestep) {
        // Update developmental stage
        // Update plasticity rates
        // Update structural plasticity parameters
        auto& devImpl = *brain->pImpl->developmentSystem;
        brain->pImpl->developmentalStage = devImpl.getCurrentStage();
        
        // Update structural plasticity based on developmental stage
        auto* sp = brain->pImpl->structuralPlasticity.get();
        if (sp) {
            float plasticityMod = 1.0f;
            switch (brain->pImpl->developmentalStage) {
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
            
            sp->setSynaptogenesisRate(0.0001f * plasticityMod);
            sp->setPruningRate(0.00001f * (2.0f - plasticityMod));
        }
    });
}

} // namespace nlm