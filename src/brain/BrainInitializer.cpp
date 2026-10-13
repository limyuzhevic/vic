#include "BrainInitializer.hpp"
#include "../Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

BrainInitializer::BrainInitializer(std::shared_ptr<Config> config) 
    : config(config)
    , rng(nullptr)
    , timestep(0.001)
    , currentStep(0)
    , currentTime(0.0)
    , totalSpikesThisStep(0)
    , totalSpikesTotal(0)
    , isResting(false)
    , stepsSinceLastEpisode(0)
    , replayInterval(100)
    , consolidationInterval(1000)
{
    // Initialize random generator with seed from config
    uint64_t seed = 42;  // Default seed
    if (auto seedOpt = config->get<uint64_t>("random_seed")) {
        seed = *seedOpt;
    }
    rng = std::make_unique<RandomGenerator>(seed);
    
    // Initialize development stage and region counter
    developmentalStage = DevelopmentalStage::Initial;
    nextRegionId = 1;
}

void BrainInitializer::initializePlasticitySystems() {
    // Initialize plasticity systems
    spikeSystem = std::make_unique<SpikeSystem>();
    stdp = std::make_unique<STDP>();
    hebbian = std::make_unique<Hebbian>();
    structuralPlasticity = std::make_unique<StructuralPlasticity>();
    
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

void BrainInitializer::initializeMemorySystems() {
    // Initialize memory systems
    workingMemory = std::make_unique<NeuralWorkingMemory>();
    episodicMemory = std::make_unique<NeuralEpisodicMemory>();
    associativeMemory = std::make_unique<NeuralAssociativeMemory>();
    
    // Initialize prediction system
    predictionSystem = std::make_unique<PredictionSystem>();
}

void BrainInitializer::initializeCognitionSystems() {
    // Initialize cognition systems
    planner = std::make_unique<NeuralPlanner>();
    conceptFormation = std::make_unique<ConceptFormation>();
    attention = std::make_unique<AttentionalSelection>();
}

void BrainInitializer::initializeNeuromodulationSystems() {
    // Initialize neuromodulation systems
    dopamine = std::make_unique<Dopamine>();
    curiosity = std::make_unique<Curiosity>();
    predictionError = std::make_unique<PredictionError>();
    novelty = std::make_unique<Novelty>();
}

void BrainInitializer::initializeDevelopmentSystem() {
    // Initialize development system
    developmentSystem = std::make_unique<DevelopmentSystem>();
}

void BrainInitializer::initializeSpikeSystem() {
    // Initialize spike system components
    spikeSystem = std::make_unique<SpikeSystem>();
}

void BrainInitializer::initializeCheckpointManager() {
    // Initialize checkpoint manager
    checkpointManager = std::make_unique<CheckpointManager>();
}

void BrainInitializer::configureRegionsAndNeurons(size_t neuronCount, size_t regionCount, 
                                                 float connectionProbability) {
    this->neuronCount = neuronCount;
    this->regionCount = regionCount;
    this->connectionProbability = connectionProbability;
    
    // Get timestep from config
    timestep = config->getOr<double>("simulation_timestep", 0.001);
    
    // Get integration intervals from config
    replayInterval = config->getOr<size_t>("replay_interval", 100);
    consolidationInterval = config->getOr<size_t>("consolidation_interval", 1000);
}

void BrainInitializer::setupSpikeHandlers(class Brain* brain) {
    // Register spike handlers for event-driven processing
    spikeSystem->registerHandler([brain](const DetailedSpikeEvent& event) {
        // Count spikes
        ++brain->pImpl->brainInitializer->totalSpikesThisStep;
        ++brain->pImpl->brainInitializer->totalSpikesTotal;
    });
    
    // Register delayed spike handler to deliver synaptic input
    spikeSystem->registerDelayedHandler([brain](const DelayedSpikeEvent& event) {
        // Find destination neuron and deliver synaptic input
        for (auto& region : brain->pImpl->brainInitializer->regions) {
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

void BrainInitializer::initializePlasticityConfig() {
    // Configure STDP and other plasticity parameters
    initializePlasticitySystems();
}

void BrainInitializer::initializeMemorySystemConfigs() {
    // Configure memory system parameters
    initializeMemorySystems();
}

void BrainInitializer::initializeCognitionSystemConfigs() {
    // Configure cognition system parameters
    initializeCognitionSystems();
}

void BrainInitializer::initializeNeuromodulationSystemConfigs() {
    // Configure neuromodulation system parameters
    initializeNeuromodulationSystems();
}

void BrainInitializer::initializeDevelopmentSystemConfig() {
    // Configure development system parameters
    initializeDevelopmentSystem();
}

void BrainInitializer::createRegionConnections() {
    // Create inter-region connections
    for (size_t i = 0; i < regionCount - 1; ++i) {
        RegionId source(i + 1);
        RegionId target(i + 2);
        addInterRegionConnection(source, target, 0.5f, 10);
    }
}

void BrainInitializer::collectSensoryMotorNeurons() {
    // Collect sensory and motor neurons for I/O
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == NeuronType::Sensory) {
                for (auto* neuron : pop->getNeurons()) {
                    sensoryNeurons.push_back(neuron);
                }
            } else if (pop->getNeuronType() == NeuronType::Motor) {
                for (auto* neuron : pop->getNeurons()) {
                    motorNeurons.push_back(neuron);
                }
            }
        }
    }
}

void BrainInitializer::configureSpikeSystem() {
    // Configure spike system components
    initializeSpikeSystem();
}

void BrainInitializer::configureCheckpointManager() {
    // Configure checkpoint manager
    initializeCheckpointManager();
    std::string checkpointDir = config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    checkpointManager->configure(checkpointDir, 10000, 5, true);
}