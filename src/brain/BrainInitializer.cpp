// Brain Initializer implementation

#include "BrainInitializer.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "Brain.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../performance/CheckpointSystem.hpp"

namespace nlm {

bool BrainInitializer::initialize(std::shared_ptr<Brain> brain) {
    // Configure random generator
    configureRandomGenerator(brain->getRandomGenerator());
    
    // Configure plasticity systems
    configurePlasticitySystems(brain->getSTDP(), brain->getHebbian(), 
                               brain->getStructuralPlasticity());
    
    // Configure memory systems
    configureMemorySystems(brain->getWorkingMemory(), 
                           brain->getEpisodicMemory(),
                           brain->getAssociativeMemory());
    
    // Configure cognition systems
    configureCognitionSystems(brain->getPlanner(),
                               brain->getConceptFormation(),
                               brain->getAttention());
    
    // Configure neuromodulation systems
    configureNeuromodulationSystems(brain->getDopamine(),
                                     brain->getCuriosity(),
                                     brain->getPredictionErrorSignal(),
                                     brain->getNovelty());
    
    return true;
}

void BrainInitializer::configureRandomGenerator(std::shared_ptr<RandomGenerator> rng) {
    if (rng) {
        // Random generator is already configured by Brain constructor
        NLM_LOG_INFO("Random generator configured");
    }
}

void BrainInitializer::configurePlasticitySystems(std::shared_ptr<STDP> stdp,
                                                  std::shared_ptr<Hebbian> hebbian,
                                                  std::shared_ptr<StructuralPlasticity> structural) {
    if (stdp) {
        NLM_LOG_INFO("STDP system configured");
    }
    if (hebbian) {
        NLM_LOG_INFO("Hebbian system configured");
    }
    if (structural) {
        NLM_LOG_INFO("Structural plasticity system configured");
    }
}

void BrainInitializer::configureMemorySystems(std::shared_ptr<NeuralWorkingMemory> working,
                                              std::shared_ptr<NeuralEpisodicMemory> episodic,
                                              std::shared_ptr<NeuralAssociativeMemory> associative) {
    if (working) {
        NLM_LOG_INFO("Working memory system configured");
    }
    if (episodic) {
        NLM_LOG_INFO("Episodic memory system configured");
    }
    if (associative) {
        NLM_LOG_INFO("Associative memory system configured");
    }
}

void BrainInitializer::configureCognitionSystems(std::shared_ptr<NeuralPlanner> planner,
                                                 std::shared_ptr<ConceptFormation> concept,
                                                 std::shared_ptr<AttentionalSelection> attention) {
    if (planner) {
        NLM_LOG_INFO("Neural planner system configured");
    }
    if (concept) {
        NLM_LOG_INFO("Concept formation system configured");
    }
    if (attention) {
        NLM_LOG_INFO("Attention system configured");
    }
}

void BrainInitializer::configureNeuromodulationSystems(std::shared_ptr<Dopamine> dopamine,
                                                         std::shared_ptr<Curiosity> curiosity,
                                                         std::shared_ptr<PredictionError> predictionError,
                                                         std::shared_ptr<Novelty> novelty) {
    if (dopamine) {
        NLM_LOG_INFO("Dopamine system configured");
    }
    if (curiosity) {
        NLM_LOG_INFO("Curiosity system configured");
    }
    if (predictionError) {
        NLM_LOG_INFO("Prediction error system configured");
    }
    if (novelty) {
        NLM_LOG_INFO("Novelty system configured");
    }
}

void BrainInitializer::createRegions(std::shared_ptr<Brain> brain, size_t neuronCount,
                                     size_t regionCount, float connectionProbability,
                                     std::shared_ptr<RandomGenerator> rng) {
    NLM_LOG_INFO("Creating " + std::to_string(regionCount) + " regions with " + 
                std::to_string(neuronCount) + " neurons total");
    
    // Create regions
    for (size_t i = 0; i < regionCount; ++i) {
        brain->addRegion("Region_" + std::to_string(i + 1));
    }
    
    // Create neurons across regions
    size_t neuronsPerRegion = neuronCount / regionCount;
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = brain->getRegion(RegionId(i + 1));
        if (region) {
            // Add populations
            auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
            auto internalPopId = region->addPopulation(neuronsPerRegion / 2, NeuronType::Internal);
            auto motorPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);
            
            // Collect sensory and motor neurons for I/O
            // This is done in BrainRegionManager
            
            // Initialize random connectivity and synapse weights
            region->initializeRandomConnectivity(*rng, connectionProbability, 0.2f, 0.1f);
            
            NLM_LOG_INFO("Created populations in region " + std::to_string(i + 1) + 
                        ": " + std::to_string(region->getPopulationCount()) + " populations, " +
                        std::to_string(region->getTotalNeuronCount()) + " neurons");
        }
    }
}

void BrainInitializer::setupSpikeSystemHandlers(std::shared_ptr<SpikeSystem> spikeSystem,
                                                 std::shared_ptr<Brain> brain) {
    if (!spikeSystem || !brain) return;
    
    // Register spike handlers for event-driven processing
    spikeSystem->registerHandler([brain](const DetailedSpikeEvent& event) {
        // Count spikes
        ++brain->pImpl->totalSpikesThisStep;
        ++brain->pImpl->totalSpikesTotal;
    });
    
    // Register delayed spike handler to deliver synaptic input
    spikeSystem->registerDelayedHandler([brain](const DelayedSpikeEvent& event) {
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

void BrainInitializer::configureCheckpointSystem(std::shared_ptr<CheckpointManager> checkpointManager) {
    if (checkpointManager) {
        std::string checkpointDir = config_->getOr<std::string>("checkpoint_dir", "./checkpoints");
        checkpointManager->configure(checkpointDir, 10000, 5, true);
        NLM_LOG_INFO("Checkpoint system configured with directory: " + checkpointDir);
    }
}

} // namespace nlm
