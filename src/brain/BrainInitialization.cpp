#include "BrainInitialization.hpp"

namespace nlm {

void BrainInitialization::Impl::initializeRandomGenerator() {
    uint64_t seed = 42;
    if (auto seedOpt = config->get<uint64_t>("random_seed")) {
        seed = *seedOpt;
    }
    rng = std::make_unique<RandomGenerator>(seed);
}

void BrainInitialization::Impl::initializePlasticitySystems() {
    spikeSystem = std::make_unique<SpikeSystem>();
    stdp = std::make_unique<STDP>();
    hebbian = std::make_unique<Hebbian>();
    structuralPlasticity = std::make_unique<StructuralPlasticity>();
}

void BrainInitialization::Impl::initializeMemorySystems() {
    workingMemory = std::make_unique<NeuralWorkingMemory>();
    episodicMemory = std::make_unique<NeuralEpisodicMemory>();
    associativeMemory = std::make_unique<NeuralAssociativeMemory>();
}

void BrainInitialization::Impl::initializePredictionSystem() {
    predictionSystem = std::make_unique<PredictionSystem>();
}

void BrainInitialization::Impl::initializeCognitionSystems() {
    planner = std::make_unique<NeuralPlanner>();
    conceptFormation = std::make_unique<ConceptFormation>();
    attention = std::make_unique<AttentionalSelection>();
}

void BrainInitialization::Impl::initializeDevelopmentSystem() {
    developmentSystem = std::make_unique<DevelopmentSystem>();
}

void BrainInitialization::Impl::initializeNeuromodulationSystems() {
    dopamine = std::make_unique<Dopamine>();
    curiosity = std::make_unique<Curiosity>();
    predictionError = std::make_unique<PredictionError>();
    novelty = std::make_unique<Novelty>();
}

void BrainInitialization::Impl::configurePlasticity() {
    float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
    float ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
    float tau = config->getOr<float>("stdp_tau", 20.0f);
    stdp->configure(ltpWeight, ltdWeight, tau);

    float synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.0001f);
    float pruningRate = config->getOr<float>("pruning_rate", 0.00001f);
    structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
    structuralPlasticity->setPruningRate(pruningRate);
}

void BrainInitialization::Impl::configureSimulationParameters() {
    timestep = config->getOr<double>("simulation_timestep", 0.001);
    replayInterval = config->getOr<size_t>("replay_interval", 100);
    consolidationInterval = config->getOr<size_t>("consolidation_interval", 1000);
}

void BrainInitialization::Impl::initializeCheckpointManager() {
    checkpointManager = std::make_unique<CheckpointManager>();
}

void BrainInitialization::Impl::configureCheckpointManager() {
    std::string checkpointDir = config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    checkpointManager->configure(checkpointDir, 10000, 5, true);
}

} // namespace nlm