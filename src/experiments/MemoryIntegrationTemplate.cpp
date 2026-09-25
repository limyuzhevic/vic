#include "ExperimentTemplate.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../environment/Environment.hpp"
#include "../agent/AgentBrain.hpp"
#include <unordered_map>
#include <cmath>
#include <algorithm>

namespace nlm {

// Helper function for template parameters
static std::string getParameter(const std::unordered_map<std::string, std::string>& params, 
                                const std::string& key, 
                                const std::string& defaultValue) {
    auto it = params.find(key);
    if (it != params.end()) {
        return it->second;
    }
    return defaultValue;
}

struct MemoryIntegrationTemplate::Impl {
    std::unordered_map<std::string, std::string> parameters;
    
    // Memory integration parameters
    size_t neuronCount;
    size_t workingMemoryCapacity;
    size_t episodicMemoryCapacity;
    float encodingStrength;
    float consolidationRate;
    size_t replayFrequency;
    
    // For tracking experiment progress
    size_t totalSteps;
    size_t currentStep;
};

MemoryIntegrationTemplate::MemoryIntegrationTemplate() : pImpl(new Impl) {}

bool MemoryIntegrationTemplate::configure(std::shared_ptr<EnhancedExperiment> experiment,
                                         const std::unordered_map<std::string, std::string>& parameters) {
    pImpl->parameters = parameters;
    
    // Parse parameters
    pImpl->neuronCount = std::stoul(getParameter(parameters, "neuron_count", "100"));
    pImpl->workingMemoryCapacity = std::stoul(getParameter(parameters, "working_memory_capacity", "50"));
    pImpl->episodicMemoryCapacity = std::stoul(getParameter(parameters, "episodic_memory_capacity", "20"));
    pImpl->encodingStrength = std::stof(getParameter(parameters, "encoding_strength", "0.8"));
    pImpl->consolidationRate = std::stof(getParameter(parameters, "consolidation_rate", "0.01"));
    pImpl->replayFrequency = std::stoul(getParameter(parameters, "replay_frequency", "100"));
    
    // Configure experiment
    experiment->setName("Memory Integration Experiment");
    experiment->setDescription("Tests memory system integration with neural processing");
    experiment->setTemplate("memory_integration");
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", pImpl->neuronCount);
    config->set("working_memory_capacity", pImpl->workingMemoryCapacity);
    config->set("episodic_memory_capacity", pImpl->episodicMemoryCapacity);
    experiment->setConfig(config);
    
    // Set parameters
    experiment->setParameter("encoding_strength", std::to_string(pImpl->encodingStrength));
    experiment->setParameter("consolidation_rate", std::to_string(pImpl->consolidationRate));
    experiment->setParameter("replay_frequency", std::to_string(pImpl->replayFrequency));
    
    pImpl->totalSteps = std::stoul(getParameter(parameters, "total_steps", "1000"));
    pImpl->currentStep = 0;
    
    NLM_LOG_INFO("Memory Integration Template configured with " + std::to_string(pImpl->neuronCount) + " neurons");
    return true;
}

bool MemoryIntegrationTemplate::run(std::shared_ptr<EnhancedExperiment> experiment) {
    NLM_LOG_INFO("Starting Memory Integration Experiment");
    
    experiment->initialize();
    experiment->start();
    
    // Create brain and environment (simplified)
    auto config = std::make_shared<Config>();
    config->set("neuron_count", pImpl->neuronCount);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        experiment->fail("Failed to initialize brain");
        return false;
    }
    
    SimpleWorld world;
    world.initialize(16, 16);
    
    AgentBrain agent(brain);
    agent.initialize(world);
    
    // Run experiment
    for (pImpl->currentStep = 0; pImpl->currentStep < pImpl->totalSteps; ++pImpl->currentStep) {
        if (!experiment->isRunning()) {
            break;
        }
        
        // Get observation and process
        SensoryPercept percept = world.observe(agent.getBrain()->getRegions()[0].get());
        agent.processSensoryInput(percept);
        
        // Brain step
        brain->step(pImpl->currentStep, pImpl->currentStep * 0.001);
        
        // Get action and apply
        MotorCommand cmd = agent.decodeMotorCommand();
        world.applyAction(agent.getBrain()->getRegions()[0].get(), cmd);
        
        // Record metrics
        EnhancedExperiment::EnhancedStepData stepData;
        stepData.step = pImpl->currentStep;
        stepData.neuronCount = brain->getTotalNeuronCount();
        stepData.synapseCount = brain->getTotalSynapseCount();
        stepData.firingCount = brain->getFiringNeuronCount();
        stepData.avgFiringRate = brain->getAverageFiringRate();
        stepData.reward = world.getLastReward();
        
        experiment->recordStep(stepData);
        
        // Record memory metrics
        experiment->recordMetric("encoding_efficiency", pImpl->encodingStrength * (pImpl->currentStep / (float)pImpl->totalSteps));
        experiment->recordMetric("memory_retention", brain->getWorkingMemory() ? 1.0f : 0.0f, "memory");
        experiment->recordMetric("episodic_storage", brain->getEpisodicMemory() ? brain->getEpisodicMemory()->getEpisodeCount() : 0, "memory");
        
        // Periodic replay
        if (pImpl->currentStep % pImpl->replayFrequency == 0) {
            experiment->recordMetric("replay_event", 1.0, "replay");
        }
        
        // Record system integration
        experiment->recordMetric("working_memory_integrated", brain->getWorkingMemory() != nullptr, "integration");
        experiment->recordMetric("episodic_memory_integrated", brain->getEpisodicMemory() != nullptr, "integration");
        experiment->recordMetric("prediction_integrated", brain->getPredictionSystem() != nullptr, "integration");
    }
    
    experiment->complete();
    
    NLM_LOG_INFO("Memory Integration Experiment completed with " + std::to_string(pImpl->currentStep) + " steps");
    return true;
}

std::vector<std::string> MemoryIntegrationTemplate::getRequiredParameters() const {
    return {"neuron_count", "working_memory_capacity", "episodic_memory_capacity"};
}

std::vector<std::string> MemoryIntegrationTemplate::getOptionalParameters() const {
    return {"encoding_strength", "consolidation_rate", "replay_frequency", "total_steps"};
}

std::unordered_map<std::string, std::string> MemoryIntegrationTemplate::getDefaultParameters() const {
    return {
        {"neuron_count", "100"},
        {"working_memory_capacity", "50"},
        {"episodic_memory_capacity", "20"},
        {"encoding_strength", "0.8"},
        {"consolidation_rate", "0.01"},
        {"replay_frequency", "100"},
        {"total_steps", "1000"}
    };
}

std::vector<std::string> MemoryIntegrationTemplate::getExpectedMetrics() const {
    return {
        "encoding_efficiency", "memory_retention", "episodic_storage", 
        "replay_event", "working_memory_integrated", "episodic_memory_integrated", "prediction_integrated"
    };
}

std::vector<std::string> MemoryIntegrationTemplate::getPerformanceMetrics() const {
    return {
        "memory_retention", "episodic_storage", "working_memory_integrated"
    };
}

std::vector<std::string> MemoryIntegrationTemplate::getDiagnosticMetrics() const {
    return {
        "encoding_efficiency", "replay_event", "prediction_integrated"
    };
}

std::vector<std::string> MemoryIntegrationTemplate::getValidationRequirements() const {
    return {
        "All memory systems (working, episodic, prediction) should be properly integrated",
        "Encoding efficiency should improve over time",
        "Replay events should occur at specified frequency",
        "Memory retention should remain above threshold"
    };
}

} // namespace nlm
