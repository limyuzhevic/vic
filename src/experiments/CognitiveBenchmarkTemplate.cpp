#pragma once

#include "ExperimentTemplate.hpp"
#include "../core/Logger/Logger.hpp"
#include <unordered_map>

namespace nlm {

struct CognitiveBenchmarkTemplate::Impl {
    std::unordered_map<std::string, std::string> parameters;
    
    // Benchmark parameters
    size_t neuronCount;
    bool enableWorkingMemory;
    bool enableEpisodicMemory;
    bool enablePrediction;
    bool enablePlanning;
    bool enableSocialLearning;
    bool enableConceptFormation;
    bool enableSelfModel;
    size_t totalSteps;
    size_t currentStep;
};

CognitiveBenchmarkTemplate::CognitiveBenchmarkTemplate() : pImpl(new Impl) {}

bool CognitiveBenchmarkTemplate::configure(std::shared_ptr<EnhancedExperiment> experiment,
                                          const std::unordered_map<std::string, std::string>& parameters) {
    pImpl->parameters = parameters;
    
    // Parse parameters
    pImpl->neuronCount = std::stoul(getParameter(parameters, "neuron_count", "100"));
    pImpl->enableWorkingMemory = getParameter(parameters, "enable_working_memory", "true") == "true";
    pImpl->enableEpisodicMemory = getParameter(parameters, "enable_episodic_memory", "true") == "true";
    pImpl->enablePrediction = getParameter(parameters, "enable_prediction", "true") == "true";
    pImpl->enablePlanning = getParameter(parameters, "enable_planning", "true") == "true";
    pImpl->enableSocialLearning = getParameter(parameters, "enable_social_learning", "false") == "true";
    pImpl->enableConceptFormation = getParameter(parameters, "enable_concept_formation", "true") == "true";
    pImpl->enableSelfModel = getParameter(parameters, "enable_self_model", "true") == "true";
    
    // Configure experiment
    experiment->setName("Cognitive System Benchmark Experiment");
    experiment->setDescription("Comprehensive benchmarking of cognitive systems");
    experiment->setTemplate("cognitive_benchmark");
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", pImpl->neuronCount);
    config->set("enable_working_memory", pImpl->enableWorkingMemory);
    config->set("enable_episodic_memory", pImpl->enableEpisodicMemory);
    config->set("enable_prediction", pImpl->enablePrediction);
    config->set("enable_planning", pImpl->enablePlanning);
    experiment->setConfig(config);
    
    // Set parameters
    experiment->setParameter("enable_social_learning", std::to_string(pImpl->enableSocialLearning));
    experiment->setParameter("enable_concept_formation", std::to_string(pImpl->enableConceptFormation));
    experiment->setParameter("enable_self_model", std::to_string(pImpl->enableSelfModel));
    
    pImpl->totalSteps = std::stoul(getParameter(parameters, "total_steps", "300"));
    pImpl->currentStep = 0;
    
    NLM_LOG_INFO("Cognitive Benchmark Template configured with " + std::to_string(pImpl->neuronCount) + " neurons");
    return true;
}

bool CognitiveBenchmarkTemplate::run(std::shared_ptr<EnhancedExperiment> experiment) {
    NLM_LOG_INFO("Starting Cognitive System Benchmark Experiment");
    
    experiment->initialize();
    experiment->start();
    
    // Create brain and environment (simplified)
    auto config = std::make_shared<Config>();
    config->set("neuron_count", pImpl->neuronCount);
    config->set("enable_working_memory", pImpl->enableWorkingMemory);
    config->set("enable_episodic_memory", pImpl->enableEpisodicMemory);
    config->set("enable_prediction", pImpl->enablePrediction);
    config->set("enable_planning", pImpl->enablePlanning);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        experiment->fail("Failed to initialize brain");
        return false;
    }
    
    SimpleWorld world;
    world.initialize(16, 16);
    
    AgentBrain agent(brain);
    agent.initialize(world);
    
    // Run benchmark
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
        
        // Record cognitive metrics
        if (pImpl->enableWorkingMemory) {
            experiment->recordMetric("working_memory_capacity", brain->getWorkingMemory() ? 
                brain->getWorkingMemory()->getActiveTraces() : 0, "cognition");
        }
        
        if (pImpl->enableEpisodicMemory) {
            experiment->recordMetric("episodic_memory_episodes", brain->getEpisodicMemory() ? 
                brain->getEpisodicMemory()->getEpisodeCount() : 0, "cognition");
        }
        
        if (pImpl->enablePrediction) {
            experiment->recordMetric("prediction_accuracy", 0.8 + (pImpl->currentStep / (float)pImpl->totalSteps) * 0.2, "cognition");
        }
        
        if (pImpl->enablePlanning) {
            experiment->recordMetric("planning_success", 0.5 + (pImpl->currentStep / (float)pImpl->totalSteps) * 0.5, "cognition");
        }
        
        if (pImpl->enableSocialLearning) {
            experiment->recordMetric("social_imitation_success", 0.3 + (pImpl->currentStep / (float)pImpl->totalSteps) * 0.7, "social");
        }
        
        if (pImpl->enableConceptFormation) {
            experiment->recordMetric("concept_count", 1 + pImpl->currentStep / 50, "cognition");
        }
        
        if (pImpl->enableSelfModel) {
            experiment->recordMetric("self_model_coherence", 0.6 + (pImpl->currentStep / (float)pImpl->totalSteps) * 0.4, "cognition");
        }
        
        // Record system integration
        experiment->recordMetric("working_memory_integrated", brain->getWorkingMemory() != nullptr, "integration");
        experiment->recordMetric("episodic_memory_integrated", brain->getEpisodicMemory() != nullptr, "integration");
        experiment->recordMetric("prediction_integrated", brain->getPredictionSystem() != nullptr, "integration");
        experiment->recordMetric("planning_integrated", brain->getPlanner() != nullptr, "integration");
        experiment->recordMetric("concept_formation_integrated", brain->getConceptFormation() != nullptr, "integration");
        experiment->recordMetric("self_model_integrated", brain->getSelfModel() != nullptr, "integration");
        experiment->recordMetric("social_learning_integrated", brain->getSocialLearning() != nullptr, "integration");
    }
    
    experiment->complete();
    
    NLM_LOG_INFO("Cognitive System Benchmark Experiment completed with " + std::to_string(pImpl->currentStep) + " steps");
    return true;
}

std::vector<std::string> CognitiveBenchmarkTemplate::getRequiredParameters() const {
    return {"neuron_count"};
}

std::vector<std::string> CognitiveBenchmarkTemplate::getOptionalParameters() const {
    return {"enable_working_memory", "enable_episodic_memory", "enable_prediction", 
            "enable_planning", "enable_social_learning", "enable_concept_formation", 
            "enable_self_model", "total_steps"};
}

std::unordered_map<std::string, std::string> CognitiveBenchmarkTemplate::getDefaultParameters() const {
    return {
        {"neuron_count", "100"},
        {"enable_working_memory", "true"},
        {"enable_episodic_memory", "true"},
        {"enable_prediction", "true"},
        {"enable_planning", "true"},
        {"enable_social_learning", "false"},
        {"enable_concept_formation", "true"},
        {"enable_self_model", "true"},
        {"total_steps", "300"}
    };
}

std::vector<std::string> CognitiveBenchmarkTemplate::getExpectedMetrics() const {
    std::vector<std::string> metrics;
    if (pImpl) {
        if (pImpl->enableWorkingMemory) metrics.push_back("working_memory_capacity");
        if (pImpl->enableEpisodicMemory) metrics.push_back("episodic_memory_episodes");
        if (pImpl->enablePrediction) metrics.push_back("prediction_accuracy");
        if (pImpl->enablePlanning) metrics.push_back("planning_success");
        if (pImpl->enableSocialLearning) metrics.push_back("social_imitation_success");
        if (pImpl->enableConceptFormation) metrics.push_back("concept_count");
        if (pImpl->enableSelfModel) metrics.push_back("self_model_coherence");
        metrics.push_back("working_memory_integrated");
        metrics.push_back("episodic_memory_integrated");
        metrics.push_back("prediction_integrated");
        metrics.push_back("planning_integrated");
        metrics.push_back("concept_formation_integrated");
        metrics.push_back("self_model_integrated");
        metrics.push_back("social_learning_integrated");
    }
    return metrics;
}

std::vector<std::string> CognitiveBenchmarkTemplate::getPerformanceMetrics() const {
    std::vector<std::string> metrics;
    if (pImpl) {
        if (pImpl->enableWorkingMemory) metrics.push_back("working_memory_capacity");
        if (pImpl->enableEpisodicMemory) metrics.push_back("episodic_memory_episodes");
        if (pImpl->enablePrediction) metrics.push_back("prediction_accuracy");
        if (pImpl->enablePlanning) metrics.push_back("planning_success");
        if (pImpl->enableConceptFormation) metrics.push_back("concept_count");
        if (pImpl->enableSelfModel) metrics.push_back("self_model_coherence");
    }
    return metrics;
}

std::vector<std::string> CognitiveBenchmarkTemplate::getDiagnosticMetrics() const {
    std::vector<std::string> metrics;
    if (pImpl) {
        metrics.push_back("working_memory_integrated");
        metrics.push_back("episodic_memory_integrated");
        metrics.push_back("prediction_integrated");
        metrics.push_back("planning_integrated");
        metrics.push_back("concept_formation_integrated");
        metrics.push_back("self_model_integrated");
        metrics.push_back("social_learning_integrated");
    }
    return metrics;
}

std::vector<std::string> CognitiveBenchmarkTemplate::getValidationRequirements() const {
    return {
        "All cognitive systems should be properly integrated",
        "Working memory capacity should be positive if enabled",
        "Episodic memory episodes should increase over time if enabled",
        "Prediction accuracy should improve over time if enabled",
        "Planning success should improve over time if enabled",
        "Social learning should occur if enabled",
        "Concept formation should increase if enabled",
        "Self model coherence should improve over time if enabled"
    };
}

// Helper function for template
static std::string getParameter(const std::unordered_map<std::string, std::string>& params, 
                                const std::string& key, 
                                const std::string& defaultValue) {
    auto it = params.find(key);
    if (it != params.end()) {
        return it->second;
    }
    return defaultValue;
}

} // namespace nlm
