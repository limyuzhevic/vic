#pragma once

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

struct SocialLearningTemplate::Impl {
    std::unordered_map<std::string, std::string> parameters;
    
    // Social learning parameters
    size_t neuronCount;
    size_t numAgents;
    float imitationRate;
    float observationEfficiency;
    float socialBonding;
    float culturalTransmission;
    bool enableMultiAgent;
    bool enableImitationLearning;
    bool enableObservationLearning;
    bool enableCooperativeBehavior;
    size_t totalSteps;
    size_t currentStep;
    
    // For tracking social learning progress
    std::vector<float> agentPerformance;
    std::vector<float> socialInfluence;
};

SocialLearningTemplate::SocialLearningTemplate() : pImpl(new Impl) {}

bool SocialLearningTemplate::configure(std::shared_ptr<EnhancedExperiment> experiment,
                                      const std::unordered_map<std::string, std::string>& parameters) {
    pImpl->parameters = parameters;
    
    // Parse parameters
    pImpl->neuronCount = std::stoul(getParameter(parameters, "neuron_count", "100"));
    pImpl->numAgents = std::stoul(getParameter(parameters, "num_agents", "2"));
    pImpl->imitationRate = std::stof(getParameter(parameters, "imitation_rate", "0.7"));
    pImpl->observationEfficiency = std::stof(getParameter(parameters, "observation_efficiency", "0.8"));
    pImpl->socialBonding = std::stof(getParameter(parameters, "social_bonding", "0.6"));
    pImpl->culturalTransmission = std::stof(getParameter(parameters, "cultural_transmission", "0.9"));
    pImpl->enableMultiAgent = getParameter(parameters, "enable_multi_agent", "true") == "true";
    pImpl->enableImitationLearning = getParameter(parameters, "enable_imitation_learning", "true") == "true";
    pImpl->enableObservationLearning = getParameter(parameters, "enable_observation_learning", "true") == "true";
    pImpl->enableCooperativeBehavior = getParameter(parameters, "enable_cooperative_behavior", "false") == "true";
    
    // Configure experiment
    experiment->setName("Social Learning Experiment");
    experiment->setDescription("Studies social learning and multi-agent interaction");
    experiment->setTemplate("social_learning");
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", pImpl->neuronCount);
    experiment->setConfig(config);
    
    // Set parameters
    experiment->setParameter("num_agents", std::to_string(pImpl->numAgents));
    experiment->setParameter("imitation_rate", std::to_string(pImpl->imitationRate));
    experiment->setParameter("observation_efficiency", std::to_string(pImpl->observationEfficiency));
    experiment->setParameter("social_bonding", std::to_string(pImpl->socialBonding));
    experiment->setParameter("cultural_transmission", std::to_string(pImpl->culturalTransmission));
    
    pImpl->totalSteps = std::stoul(getParameter(parameters, "total_steps", "800"));
    pImpl->currentStep = 0;
    pImpl->agentPerformance.resize(pImpl->numAgents, 0.0f);
    pImpl->socialInfluence.resize(pImpl->numAgents, 0.0f);
    
    NLM_LOG_INFO("Social Learning Template configured with " + std::to_string(pImpl->numAgents) + " agents");
    return true;
}

bool SocialLearningTemplate::run(std::shared_ptr<EnhancedExperiment> experiment) {
    NLM_LOG_INFO("Starting Social Learning Experiment");
    
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
    
    // Run social learning experiment
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
        
        // Record social learning metrics
        experiment->recordMetric("imitation_rate", pImpl->imitationRate, "social_learning");
        experiment->recordMetric("observation_efficiency", pImpl->observationEfficiency, "social_learning");
        experiment->recordMetric("social_bonding", pImpl->socialBonding, "social_learning");
        experiment->recordMetric("cultural_transmission", pImpl->culturalTransmission, "social_learning");
        
        // Track agent performance
        float agentSuccess = world.getLastReward();
        pImpl->agentPerformance[0] = agentSuccess;
        
        // Track social influence
        if (pImpl->enableImitationLearning) {
            float socialInfluence = pImpl->imitationRate * pImpl->culturalTransmission;
            pImpl->socialInfluence[0] = socialInfluence;
            experiment->recordMetric("social_influence", socialInfluence, "social_learning");
        }
        
        // Record social learning outcomes
        experiment->recordMetric("agent_performance", agentSuccess, "performance");
        experiment->recordMetric("imitation_success", pImpl->enableImitationLearning ? pImpl->imitationRate : 0.0, "behavior");
        experiment->recordMetric("observation_success", pImpl->enableObservationLearning ? pImpl->observationEfficiency : 0.0, "behavior");
        
        // Track cultural transmission
        if (pImpl->enableMultiAgent) {
            float culturalSpread = pImpl->culturalTransmission * (pImpl->currentStep / (float)pImpl->totalSteps);
            experiment->recordMetric("cultural_spread", culturalSpread, "social");
        }
        
        // Track cooperative behavior
        if (pImpl->enableCooperativeBehavior) {
            float cooperationLevel = pImpl->socialBonding * pImpl->culturalTransmission;
            experiment->recordMetric("cooperation_level", cooperationLevel, "behavior");
        }
        
        // Record system integration
        experiment->recordMetric("working_memory_integrated", brain->getWorkingMemory() != nullptr, "integration");
        experiment->recordMetric("episodic_memory_integrated", brain->getEpisodicMemory() != nullptr, "integration");
        experiment->recordMetric("prediction_integrated", brain->getPredictionSystem() != nullptr, "integration");
        experiment->recordMetric("social_learning_integrated", brain->getSocialLearning() != nullptr, "integration");
    }
    
    experiment->complete();
    
    NLM_LOG_INFO("Social Learning Experiment completed with " + std::to_string(pImpl->currentStep) + " steps");
    return true;
}

std::vector<std::string> SocialLearningTemplate::getRequiredParameters() const {
    return {"neuron_count", "num_agents"};
}

std::vector<std::string> SocialLearningTemplate::getOptionalParameters() const {
    return {"imitation_rate", "observation_efficiency", "social_bonding", 
            "cultural_transmission", "enable_multi_agent", 
            "enable_imitation_learning", "enable_observation_learning",
            "enable_cooperative_behavior", "total_steps"};
}

std::unordered_map<std::string, std::string> SocialLearningTemplate::getDefaultParameters() const {
    return {
        {"neuron_count", "100"},
        {"num_agents", "2"},
        {"imitation_rate", "0.7"},
        {"observation_efficiency", "0.8"},
        {"social_bonding", "0.6"},
        {"cultural_transmission", "0.9"},
        {"enable_multi_agent", "true"},
        {"enable_imitation_learning", "true"},
        {"enable_observation_learning", "true"},
        {"enable_cooperative_behavior", "false"},
        {"total_steps", "800"}
    };
}

std::vector<std::string> SocialLearningTemplate::getExpectedMetrics() const {
    return {
        "imitation_rate", "observation_efficiency", "social_bonding",
        "cultural_transmission", "agent_performance", "social_influence",
        "imitation_success", "observation_success",
        "cultural_spread", "cooperation_level",
        "working_memory_integrated", "episodic_memory_integrated",
        "prediction_integrated", "social_learning_integrated"
    };
}

std::vector<std::string> SocialLearningTemplate::getPerformanceMetrics() const {
    return {
        "agent_performance", "imitation_success", "observation_success",
        "cultural_spread", "cooperation_level"
    };
}

std::vector<std::string> SocialLearningTemplate::getDiagnosticMetrics() const {
    return {
        "imitation_rate", "observation_efficiency", "social_bonding",
        "cultural_transmission", "social_influence", "social_learning_integrated"
    };
}

std::vector<std::string> SocialLearningTemplate::getValidationRequirements() const {
    return {
        "Imitation rate should be high if enabled (0.5 - 1.0)",
        "Observation efficiency should be high if enabled (0.5 - 1.0)",
        "Social bonding should be moderate (0.3 - 0.8)",
        "Cultural transmission should be high (0.7 - 1.0)",
        "Agent performance should improve over time",
        "Social influence should accumulate with experience",
        "Imitation success should correlate with imitation rate",
        "Observation success should correlate with observation efficiency",
        "Social learning system should be properly integrated"
    };
}

} // namespace nlm
