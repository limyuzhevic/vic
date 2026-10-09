// brain/BrainInitialization.cpp - Brain initialization implementation
// Contains the initialization logic for the Brain and its pImpl

#include "brain/BrainInitialization.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

bool Brain::Impl::initialize() {
    try {
        NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
        
        // Initialize random generator with seed from config
        uint64_t seed = 42;  // Default seed
        if (auto seedOpt = config->get<uint64_t>("random_seed")) {
            seed = *seedOpt;
        }
        rng = std::make_unique<RandomGenerator>(seed);
        
        // Initialize plasticity systems
        try {
            spikeSystem = std::make_unique<SpikeSystem>();
            stdp = std::make_unique<STDP>();
            hebbian = std::make_unique<Hebbian>();
            structuralPlasticity = std::make_unique<StructuralPlasticity>();
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Failed to initialize plasticity systems: ") + e.what());
            return false;
        }
        
        // Configure STDP parameters
        try {
            float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
            float ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
            float tau = config->getOr<float>("stdp_tau", 20.0f);
            stdp->configure(ltpWeight, ltdWeight, tau);
        } catch (const std::exception& e) {
            NLM_LOG_WARNING(std::string("Failed to configure STDP: ") + e.what());
            // Continue with default values
        }
        
        // Configure structural plasticity
        try {
            float synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.0001f);
            float pruningRate = config->getOr<float>("pruning_rate", 0.00001f);
            structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
            structuralPlasticity->setPruningRate(pruningRate);
        } catch (const std::exception& e) {
            NLM_LOG_WARNING(std::string("Failed to configure structural plasticity: ") + e.what());
            // Continue with default values
        }
        
        // Get timestep
        try {
            timestep = config->getOr<double>("simulation_timestep", 0.001);
        } catch (const std::exception& e) {
            NLM_LOG_WARNING(std::string("Failed to get timestep: ") + e.what());
            timestep = 0.001;  // Default value
        }
        
        // Get integration intervals from config
        try {
            replayInterval = config->getOr<size_t>("replay_interval", 100);
            consolidationInterval = config->getOr<size_t>("consolidation_interval", 1000);
        } catch (const std::exception& e) {
            NLM_LOG_WARNING(std::string("Failed to get integration intervals: ") + e.what());
            replayInterval = 100;  // Default value
            consolidationInterval = 1000;  // Default value
        }
        
        // Initialize working memory
        try {
            workingMemory = std::make_unique<NeuralWorkingMemory>();
            if (!workingMemory->initialize(this)) {
                NLM_LOG_WARNING("Failed to initialize working memory");
                // Continue without working memory or throw exception
            } else {
                workingMemory->setCapacity(std::max<size_t>(1, getTotalNeuronCount() / 10));
                NLM_LOG_INFO("Working memory initialized with capacity: " + 
                             std::to_string(workingMemory->getCapacity()));
            }
        } catch (const std::exception& e) {
            NLM_LOG_WARNING(std::string("Failed to initialize working memory: ") + e.what());
            // Continue without working memory
        }
        
        // Initialize episodic memory
        try {
            episodicMemory = std::make_unique<NeuralEpisodicMemory>();
            if (!episodicMemory->initialize(this)) {
                NLM_LOG_WARNING("Failed to initialize episodic memory");
            }
        } catch (const std::exception& e) {
            NLM_LOG_WARNING(std::string("Failed to initialize episodic memory: ") + e.what());
        }
        
        // Initialize associative memory
        try {
            associativeMemory = std::make_unique<NeuralAssociativeMemory>();
            if (!associativeMemory->initialize(this)) {
                NLM_LOG_WARNING("Failed to initialize associative memory");
            }
        } catch (const std::exception& e) {
            NLM_LOG_WARNING(std::string("Failed to initialize associative memory: ") + e.what());
        }
        
        // Initialize prediction system
        try {
            predictionSystem = std::make_unique<PredictionSystem>();
            if (!predictionSystem->initialize(this)) {
                NLM_LOG_WARNING("Failed to initialize prediction system");
            }
        } catch (const std::exception& e) {
            NLM_LOG_WARNING(std::string("Failed to initialize prediction system: ") + e.what());
        }
        
        // Initialize cognition systems
        try {
            planner = std::make_unique<NeuralPlanner>();
            conceptFormation = std::make_unique<ConceptFormation>();
            attention = std::make_unique<AttentionalSelection>();
        } catch (const std::exception& e) {
            NLM_LOG_WARNING(std::string("Failed to initialize cognition systems: ") + e.what());
        }
        
        // Initialize development system
        try {
            developmentSystem = std::make_unique<DevelopmentSystem>();
        } catch (const std::exception& e) {
            NLM_LOG_WARNING(std::string("Failed to initialize development system: ") + e.what());
        }
        
        // Initialize neuromodulation systems
        try {
            dopamine = std::make_unique<Dopamine>();
            curiosity = std::make_unique<Curiosity>();
            predictionError = std::make_unique<PredictionError>();
            novelty = std::make_unique<Novelty>();
        } catch (const std::exception& e) {
            NLM_LOG_WARNING(std::string("Failed to initialize neuromodulation systems: ") + e.what());
        }
        
        // Initialize checkpoint system
        try {
            checkpointManager = std::make_unique<CheckpointManager>();
        } catch (const std::exception& e) {
            NLM_LOG_WARNING(std::string("Failed to initialize checkpoint system: ") + e.what());
        }
        
        // Initialize developmental stage
        developmentalStage = DevelopmentalStage::Initial;
        nextRegionId = 1;
        
        // Initialize statistics
        totalSpikesThisStep = 0;
        totalSpikesTotal = 0;
        
        // Initialize sensory and motor neurons
        sensoryNeurons.clear();
        motorNeurons.clear();
        
        // Initialize integration state
        isResting = false;
        stepsSinceLastEpisode = 0;
        
        NLM_LOG_INFO("NLM Brain initialization completed successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception during Brain initialization: ") + e.what());
        return false;
    }
}

} // namespace nlm