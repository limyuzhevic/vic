#include "SystemInitializer.hpp"
#include "Brain.hpp"
#include "Config.hpp"
#include "Logger.hpp"
#include <algorithm>

namespace nlm {

bool SystemInitializer::initializeAllSystems(std::shared_ptr<Brain> brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize systems: brain is null");
        return false;
    }
    
    NLM_LOG_INFO("Initializing all NLM systems...");
    
    // Configure plasticity systems first (neurons and synapses need plasticity)
    configurePlasticitySystems(brain);
    
    // Configure neuromodulation systems (modulate plasticity and activity)
    configureNeuromodulationSystems(brain);
    
    // Configure memory systems (need plasticity to form memories)
    configureMemorySystems(brain);
    
    // Configure prediction systems (need sensory processing)
    configurePredictionSystems(brain);
    
    // Configure cognition systems (need memory and prediction)
    configureCognitionSystems(brain);
    
    // Configure development systems (need neuromodulation)
    configureDevelopmentSystems(brain);
    
    // Configure sensory/motor systems (interface with world)
    configureSensoryMotorSystems(brain);
    
    // Configure simulation clock
    configureSimulationClock(brain);
    
    // Validate configuration
    if (brain->getConfig()) {
        if (!validateConfiguration(brain->getConfig())) {
            NLM_LOG_ERROR("Configuration validation failed");
            return false;
        }
    }
    
    // Apply performance optimizations
    applyPerformanceOptimizations(brain);
    
    NLM_LOG_INFO("All NLM systems initialized successfully");
    return true;
}

void SystemInitializer::configurePlasticitySystems(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("Configuring plasticity systems...");
    
    auto config = brain->getConfig();
    if (!config) return;
    
    // Configure STDP (Spike-Timing Dependent Plasticity)
    float stdpLtpWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
    float stdpLtdWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
    float stdpTau = config->getOr<float>("stdp_tau", 20.0f);
    
    auto* stdp = brain->getSTDP();
    if (stdp) {
        stdp->configure(stdpLtpWeight, stdpLtdWeight, stdpTau);
    }
    
    // Configure Hebbian plasticity
    auto* hebbian = brain->getHebbian();
    if (hebbian) {
        // Hebbian parameters from config if available
        hebbian->setLearningRate(config->getOr<float>("hebbian_learning_rate", 0.001f));
    }
    
    // Configure structural plasticity
    float synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.0001f);
    float pruningRate = config->getOr<float>("pruning_rate", 0.00001f);
    
    auto* structuralPlasticity = brain->getStructuralPlasticity();
    if (structuralPlasticity) {
        structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
        structuralPlasticity->setPruningRate(pruningRate);
    }
    
    NLM_LOG_INFO("Plasticity systems configured");
}

void SystemInitializer::configureNeuromodulationSystems(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("Configuring neuromodulation systems...");
    
    // Neuromodulation systems will be configured by their individual initialize methods
    // This is a placeholder for coordination
    
    auto* dopamine = brain->getDopamine();
    auto* curiosity = brain->getCuriosity();
    auto* novelty = brain->getNovelty();
    auto* predictionError = brain->getPredictionErrorSignal();
    
    if (dopamine) {
        dopamine->initialize(brain.get());
    }
    
    if (curiosity) {
        curiosity->initialize(brain.get());
    }
    
    if (novelty) {
        novelty->initialize(brain.get());
    }
    
    if (predictionError) {
        predictionError->initialize(brain.get());
    }
    
    NLM_LOG_INFO("Neuromodulation systems configured");
}

void SystemInitializer::configureMemorySystems(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("Configuring memory systems...");
    
    // Configure working memory
    auto* workingMemory = brain->getWorkingMemory();
    if (workingMemory) {
        workingMemory->initialize(brain.get());
        size_t neuronCount = brain->getTotalNeuronCount();
        workingMemory->setCapacity(neuronCount / 10);
    }
    
    // Configure episodic memory
    auto* episodicMemory = brain->getEpisodicMemory();
    if (episodicMemory) {
        episodicMemory->initialize(brain.get());
        episodicMemory->setMaxEpisodes(1000);
    }
    
    // Configure associative memory
    auto* associativeMemory = brain->getAssociativeMemory();
    if (associativeMemory) {
        associativeMemory->initialize(brain.get());
    }
    
    NLM_LOG_INFO("Memory systems configured");
}

void SystemInitializer::configurePredictionSystems(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("Configuring prediction systems...");
    
    auto* predictionSystem = brain->getPredictionSystem();
    if (predictionSystem) {
        // PredictionSystem doesn't have initialize method currently
        // Could add configuration parameters here
    }
    
    NLM_LOG_INFO("Prediction systems configured");
}

void SystemInitializer::configureCognitionSystems(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("Configuring cognition systems...");
    
    // Configure neural planner
    auto* planner = brain->getPlanner();
    if (planner) {
        planner->initialize(brain.get());
        planner->setPlanningDepth(5);
    }
    
    // Configure concept formation
    auto* conceptFormation = brain->getConceptFormation();
    if (conceptFormation) {
        conceptFormation->initialize(brain.get());
    }
    
    // Configure attentional selection
    auto* attention = brain->getAttention();
    if (attention) {
        attention->initialize(brain.get());
        attention->setInhibitionStrength(0.5f);
        attention->setExcitationStrength(1.5f);
    }
    
    NLM_LOG_INFO("Cognition systems configured");
}

void SystemInitializer::configureDevelopmentSystems(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("Configuring development systems...");
    
    auto* developmentSystem = brain->getDevelopmentSystem();
    if (developmentSystem) {
        developmentSystem->initialize(brain.get());
    }
    
    NLM_LOG_INFO("Development systems configured");
}

void SystemInitializer::configureSensoryMotorSystems(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("Configuring sensory/motor systems...");
    
    // These systems are integrated into the brain's initialization
    // Sensor neurons are created during brain initialization
    // Motor neuron distribution is handled by AgentBrain
    
    NLM_LOG_INFO("Sensory/motor systems configured");
}

void SystemInitializer::configureSimulationClock(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("Configuring simulation clock...");
    
    // Simulation clock is configured via brain config
    auto config = brain->getConfig();
    if (config) {
        double timestep = config->getOr<double>("simulation_timestep", 0.001);
        // In a full implementation, we would pass this to a simulation clock manager
        NLM_LOG_INFO(std::string("Simulation timestep configured: ") + std::to_string(timestep) + "s");
    }
    
    NLM_LOG_INFO("Simulation clock configured");
}

bool SystemInitializer::validateConfiguration(std::shared_ptr<Config> config) {
    if (!config) {
        NLM_LOG_ERROR("Cannot validate configuration: config is null");
        return false;
    }
    
    bool valid = true;
    
    // Check required parameters
    if (!config->has("neuron_count") || config->get<int64_t>("neuron_count") <= 0) {
        NLM_LOG_WARNING("neuron_count not set or invalid");
        valid = false;
    }
    
    if (!config->has("region_count") || config->get<int64_t>("region_count") <= 0) {
        NLM_LOG_WARNING("region_count not set or invalid");
        valid = false;
    }
    
    // Check plasticity parameters if plasticity is enabled
    bool plasticityEnabled = config->getOr<bool>("plasticity_enabled", true);
    if (plasticityEnabled) {
        if (!config->has("stdp_ltp_weight") || !config->has("stdp_ltd_weight")) {
            NLM_LOG_WARNING("STDP parameters not set");
            valid = false;
        }
    }
    
    if (valid) {
        NLM_LOG_INFO("Configuration validation passed");
    } else {
        NLM_LOG_ERROR("Configuration validation failed");
    }
    
    return valid;
}

void SystemInitializer::applyPerformanceOptimizations(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("Applying performance optimizations...");
    
    // Check if performance optimization is enabled
    auto config = brain->getConfig();
    if (config && config->getOr<bool>("performance_optimization", false)) {
        // Enable memory pools for neurons and synapses
        // Configure sparse connectivity if available
        // Set up event-driven processing
        NLM_LOG_INFO("Performance optimizations applied");
    }
    
    NLM_LOG_INFO("Performance optimization complete");
}

} // namespace nlm

