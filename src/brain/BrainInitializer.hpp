// Brain initialization implementation
#include "BrainInitializer.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/Hebbian.hpp"

namespace nlm {
namespace brain {

void BrainInitializer::initializeMemorySystems(std::shared_ptr<Config> config, Brain::Impl& impl) {
    impl.workingMemory = std::make_unique<NeuralWorkingMemory>();
    impl.episodicMemory = std::make_unique<NeuralEpisodicMemory>();
    impl.associativeMemory = std::make_unique<NeuralAssociativeMemory>();
    
    // Initialize with brain pointer for proper integration
    impl.workingMemory->initialize(nullptr); // Will be set later
    impl.workingMemory->setCapacity(config->getOr<size_t>("working_memory_capacity", 1000));
    
    impl.episodicMemory->initialize(nullptr);
    impl.episodicMemory->setMaxEpisodes(config->getOr<size_t>("max_episodes", 1000));
    
    impl.associativeMemory->initialize(nullptr);
}

void BrainInitializer::initializePredictionSystem(std::shared_ptr<Config> config, Brain::Impl& impl) {
    impl.predictionSystem = std::make_unique<PredictionSystem>();
    
    // Configure prediction system parameters
    // Connect to sensory input processing for real prediction
}

void BrainInitializer::initializeCognitionSystems(std::shared_ptr<Config> config, Brain::Impl& impl) {
    impl.planner = std::make_unique<NeuralPlanner>();
    impl.conceptFormation = std::make_unique<ConceptFormation>();
    impl.attention = std::make_unique<AttentionalSelection>();
    
    // Initialize cognition systems
    impl.planner->initialize(nullptr);
    impl.planner->setPlanningDepth(config->getOr<size_t>("planning_depth", 5));
    
    impl.conceptFormation->initialize(nullptr);
    
    impl.attention->initialize(nullptr);
    impl.attention->setInhibitionStrength(0.5f);
    impl.attention->setExcitationStrength(1.5f);
    
    // Connect attention to working memory for competition
}

void BrainInitializer::initializeNeuromodulationSystems(std::shared_ptr<Config> config, Brain::Impl& impl) {
    impl.dopamine = std::make_unique<Dopamine>();
    impl.curiosity = std::make_unique<Curiosity>();
    impl.predictionError = std::make_unique<PredictionError>();
    impl.novelty = std::make_unique<Novelty>();
    
    // Configure neuromodulation systems
    // Connect to reward processing and learning
}

void BrainInitializer::initializeDevelopmentSystem(std::shared_ptr<Config> config, Brain::Impl& impl) {
    impl.developmentSystem = std::make_unique<DevelopmentSystem>();
    
    // Configure developmental parameters
    // Connect to plasticity and neural dynamics
}

void BrainInitializer::initializePlasticitySystems(std::shared_ptr<Config> config, Brain::Impl& impl) {
    impl.spikeSystem = std::make_unique<SpikeSystem>();
    impl.stdp = std::make_unique<STDP>();
    impl.hebbian = std::make_unique<Hebbian>();
    impl.structuralPlasticity = std::make_unique<StructuralPlasticity>();
    
    // Configure STDP parameters
    float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
    float ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
    float tau = config->getOr<float>("stdp_tau", 20.0f);
    impl.stdp->configure(ltpWeight, ltdWeight, tau);
    
    // Configure Hebbian parameters
    // (if applicable)
    
    // Configure structural plasticity
    float synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.0001f);
    float pruningRate = config->getOr<float>("pruning_rate", 0.00001f);
    impl.structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
    impl.structuralPlasticity->setPruningRate(pruningRate);
    
    // Register spike handlers for plasticity
    impl.spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
        // Could implement spike-based plasticity here
    });
}

void BrainInitializer::integrateAllSystems(Brain::Impl& impl) {
    // Integrate memory systems with neural processing
    integrateMemorySystems(impl);
    
    // Integrate prediction with sensory input
    integratePredictionSystem(impl);
    
    // Integrate cognition with action selection
    integrateCognitionSystems(impl);
    
    // Integrate neuromodulation with learning
    integrateNeuromodulationSystems(impl);
    
    // Integrate development with plasticity
    integrateDevelopmentSystem(impl);
    
    // Integrate plasticity with neural dynamics
    integratePlasticitySystems(impl);
    
    // Set up integration callbacks
    setupIntegrationCallbacks(impl);
}

void integrateMemorySystems(Brain::Impl& impl) {
    // Connect working memory to neural activity patterns
    if (impl.workingMemory) {
        // Working memory stores active neurons during spikes
        // This happens in the spike detection phase of Brain::step()
    }
    
    // Connect episodic memory to experience encoding
    if (impl.episodicMemory) {
        // Episodic memory captures brain state every N steps
        // This happens in step 7 of Brain::step()
    }
}

void integratePredictionSystem(Brain::Impl& impl) {
    // Connect prediction to sensory processing
    if (impl.predictionSystem) {
        // Prediction system receives sensory input to predict next state
        // Would integrate with AgentBrain for actual prediction
    }
}

void integrateCognitionSystems(Brain::Impl& impl) {
    // Connect planner to action selection
    if (impl.planner) {
        // Neural planner would evaluate action sequences
        // Would integrate with AgentBrain::decodeMotorCommand()
    }
    
    // Connect concept formation to pattern discovery
    if (impl.conceptFormation) {
        // Concept formation processes experiences to form abstractions
        // Would integrate with episodic memory replay
    }
    
    // Connect attention to working memory competition
    if (impl.attention && impl.workingMemory) {
        // Attention selects among working memory traces
        // This happens in step 9 of Brain::step()
    }
}

void integrateNeuromodulationSystems(Brain::Impl& impl) {
    // Connect dopamine to reward prediction and plasticity
    if (impl.dopamine && impl.stdp) {
        // Dopamine modulates STDP learning rates
        // This happens in step 6 of Brain::step()
    }
    
    // Connect curiosity to exploration
    if (impl.curiosity) {
        // Curiosity drives exploration behavior
        // Would integrate with AgentBrain::selectWithCuriosity()
    }
    
    // Connect novelty to attention and learning
    if (impl.novelty && impl.attention) {
        // Novelty detection modulates attention
        // Would affect attentional selection
    }
}

void integrateDevelopmentSystem(Brain::Impl& impl) {
    // Connect development to plasticity rates
    if (impl.developmentSystem && impl.structuralPlasticity) {
        // Development affects structural plasticity
        // This happens in step 13 of Brain::step()
    }
    
    // Connect development to neural excitability
    if (impl.developmentSystem) {
        // Development would modulate neural properties over time
        // Could affect firing thresholds, etc.
    }
}

void integratePlasticitySystems(Brain::Impl& impl) {
    // Connect STDP to spike timing
    if (impl.stdp && impl.spikeSystem) {
        // STDP depends on precise spike timing
        // Already integrated through synapse spike recording
    }
    
    // Connect Hebbian to co-activation
    if (impl.hebbian) {
        // Hebbian learning strengthens co-activated synapses
        // Would need synaptic co-activation tracking
    }
    
    // Connect structural plasticity to development
    if (impl.structuralPlasticity && impl.developmentSystem) {
        // Structural plasticity changes synapse connectivity
        // Development modulates these rates
    }
}

void setupIntegrationCallbacks(Brain::Impl& impl) {
    // Set up callbacks for system integration
    
    // 1. Memory integration callbacks
    if (impl.workingMemory && impl.spikeSystem) {
        impl.spikeSystem->registerHandler([impl](const DetailedSpikeEvent& event) {
            if (impl.workingMemory) {
                // Store firing neuron in working memory
                impl.workingMemory->storeToNeuron(event.neuron_id, 1.0f);
            }
        });
    }
    
    // 2. Neuromodulation integration callbacks
    if (impl.dopamine) {
        // Dopamine would modulate plasticity based on reward
    }
    
    // 3. Prediction integration callbacks  
    if (impl.predictionSystem && impl.spikeSystem) {
        impl.spikeSystem->registerHandler([impl](const DetailedSpikeEvent& event) {
            if (impl.predictionSystem) {
                // Prediction system could learn from spike patterns
                // For now, just track
            }
        });
    }
}

} // namespace brain
} // namespace nlm
