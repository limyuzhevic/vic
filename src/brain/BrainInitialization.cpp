#include "BrainInitialization.hpp"

namespace nlm {

bool BrainInitialization::initialize(Brain& brain) {
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
    
    // Get configuration values
    size_t neuronCount = brain.getConfig()->getOr<size_t>("neuron_count", 1000);
    size_t regionCount = brain.getConfig()->getOr<size_t>("region_count", 1);
    float connectionProbability = brain.getConfig()->getOr<float>("connection_probability", 0.1f);
    
    NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                 std::to_string(regionCount) + " regions");
    
    // Create regions
    for (size_t i = 0; i < regionCount; ++i) {
        brain.addRegion("Region_" + std::to_string(i + 1));
    }
    
    // Create neurons across regions
    size_t neuronsPerRegion = neuronCount / regionCount;
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = brain.getRegion(RegionId(i + 1));
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
                    brain.pImpl->sensoryNeurons.push_back(neuron);
                }
            }
            if (motorPop) {
                for (auto* neuron : motorPop->getNeurons()) {
                    brain.pImpl->motorNeurons.push_back(neuron);
                }
            }
            
            NLM_LOG_INFO("Created populations in region " + std::to_string(i + 1) + 
                        ": " + std::to_string(region->getPopulationCount()) + " populations, " +
                        std::to_string(region->getTotalNeuronCount()) + " neurons");
        }
    }
    
    // Initialize connectivity with random weights
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = brain.getRegion(RegionId(i + 1));
        if (region) {
            // Initialize random connectivity and synapse weights
            region->initializeRandomConnectivity(*brain.getRandomGenerator(), connectionProbability, 0.2f, 0.1f);
        }
    }
    
    // Initialize working memory
    brain.getWorkingMemory()->initialize(&brain);
    brain.getWorkingMemory()->setCapacity(neuronCount / 10);
    
    // Initialize episodic memory
    brain.getEpisodicMemory()->initialize(&brain);
    brain.getEpisodicMemory()->setMaxEpisodes(1000);
    
    // Initialize associative memory
    brain.getAssociativeMemory()->initialize(&brain);
    
    // Initialize cognition systems
    brain.getPlanner()->initialize(&brain);
    brain.getPlanner()->setPlanningDepth(5);
    
    brain.getConceptFormation()->initialize(&brain);
    
    brain.getAttention()->initialize(&brain);
    brain.getAttention()->setInhibitionStrength(0.5f);
    brain.getAttention()->setExcitationStrength(1.5f);
    
    // Initialize neuromodulation
    brain.getNovelty()->initialize(&brain);
    brain.getCuriosity()->initialize(&brain);
    
    // Register spike handlers for event-driven processing
    brain.getSpikeSystem()->registerHandler([&brain](const DetailedSpikeEvent& event) {
        // Count spikes
        ++brain.pImpl->totalSpikesThisStep;
        ++brain.pImpl->totalSpikesTotal;
    });
    
    // Register delayed spike handler to deliver synaptic input
    brain.getSpikeSystem()->registerDelayedHandler([&brain](const DelayedSpikeEvent& event) {
        // Find destination neuron and deliver synaptic input
        for (auto& region : brain.pImpl->regions) {
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
    
    // Configure checkpoint manager
    std::string checkpointDir = brain.getConfig()->getOr<std::string>("checkpoint_dir", "./checkpoints");
    brain.getDevelopmentSystem()->getCheckpointManager()->configure(checkpointDir, 10000, 5, true);
    
    NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
    NLM_LOG_INFO("Total neurons: " + std::to_string(brain.getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(brain.getTotalSynapseCount()));
    NLM_LOG_INFO("Sensory neurons: " + std::to_string(brain.pImpl->sensoryNeurons.size()));
    NLM_LOG_INFO("Motor neurons: " + std::to_string(brain.pImpl->motorNeurons.size()));
    
    return true;
}

void BrainInitialization::setupSensoryMotorNeurons(Brain& brain) {
    // Collect sensory and motor neurons for I/O
    size_t neuronCount = brain.getConfig()->getOr<size_t>("neuron_count", 1000);
    size_t regionCount = brain.getConfig()->getOr<size_t>("region_count", 1);
    size_t neuronsPerRegion = neuronCount / regionCount;
    
    brain.pImpl->sensoryNeurons.clear();
    brain.pImpl->motorNeurons.clear();
    
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = brain.getRegion(RegionId(i + 1));
        if (region) {
            auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
            auto motorPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);
            
            auto* sensoryPop = region->getPopulation(sensoryPopId);
            auto* motorPop = region->getPopulation(motorPopId);
            if (sensoryPop) {
                for (auto* neuron : sensoryPop->getNeurons()) {
                    brain.pImpl->sensoryNeurons.push_back(neuron);
                }
            }
            if (motorPop) {
                for (auto* neuron : motorPop->getNeurons()) {
                    brain.pImpl->motorNeurons.push_back(neuron);
                }
            }
        }
    }
}

void BrainInitialization::initializeMemorySystems(Brain& brain) {
    size_t neuronCount = brain.getConfig()->getOr<size_t>("neuron_count", 1000);
    brain.getWorkingMemory()->initialize(&brain);
    brain.getWorkingMemory()->setCapacity(neuronCount / 10);
    
    brain.getEpisodicMemory()->initialize(&brain);
    brain.getEpisodicMemory()->setMaxEpisodes(1000);
    
    brain.getAssociativeMemory()->initialize(&brain);
}

void BrainInitialization::initializeCognitionSystems(Brain& brain) {
    brain.getPlanner()->initialize(&brain);
    brain.getPlanner()->setPlanningDepth(5);
    
    brain.getConceptFormation()->initialize(&brain);
    
    brain.getAttention()->initialize(&brain);
    brain.getAttention()->setInhibitionStrength(0.5f);
    brain.getAttention()->setExcitationStrength(1.5f);
}

void BrainInitialization::initializeNeuromodulationSystems(Brain& brain) {
    brain.getNovelty()->initialize(&brain);
    brain.getCuriosity()->initialize(&brain);
}

void BrainInitialization::configurePlasticitySystems(Brain& brain) {
    float ltpWeight = brain.getConfig()->getOr<float>("stdp_ltp_weight", 0.01f);
    float ltdWeight = brain.getConfig()->getOr<float>("stdp_ltd_weight", 0.012f);
    float tau = brain.getConfig()->getOr<float>("stdp_tau", 20.0f);
    brain.getSTDP()->configure(ltpWeight, ltdWeight, tau);
    
    float synaptogenesisRate = brain.getConfig()->getOr<float>("synaptogenesis_rate", 0.0001f);
    float pruningRate = brain.getConfig()->getOr<float>("pruning_rate", 0.00001f);
    brain.getStructuralPlasticity()->setSynaptogenesisRate(synaptogenesisRate);
    brain.getStructuralPlasticity()->setPruningRate(pruningRate);
}

void BrainInitialization::configureCheckpointManager(Brain& brain) {
    std::string checkpointDir = brain.getConfig()->getOr<std::string>("checkpoint_dir", "./checkpoints");
    brain.getDevelopmentSystem()->getCheckpointManager()->configure(checkpointDir, 10000, 5, true);
}

void BrainInitialization::registerSpikeHandlers(Brain& brain) {
    brain.getSpikeSystem()->registerHandler([&brain](const DetailedSpikeEvent& event) {
        // Count spikes
        ++brain.pImpl->totalSpikesThisStep;
        ++brain.pImpl->totalSpikesTotal;
    });
    
    brain.getSpikeSystem()->registerDelayedHandler([&brain](const DelayedSpikeEvent& event) {
        // Find destination neuron and deliver synaptic input
        for (auto& region : brain.pImpl->regions) {
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

} // namespace nlm