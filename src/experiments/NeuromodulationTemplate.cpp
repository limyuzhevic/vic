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

struct NeuromodulationTemplate::Impl {
    std::unordered_map<std::string, std::string> parameters;
    
    // Neuromodulation parameters
    size_t neuronCount;
    float dopamineLevel;
    float curiosityLevel;
    float noveltyLevel;
    float serotoninLevel;
    float norepinephrineLevel;
    float acetylcholineLevel;
    bool enableRewardModulation;
    bool enableCuriosity;
    bool enableNovelty;
    size_t totalSteps;
    size_t currentStep;
};

NeuromodulationTemplate::NeuromodulationTemplate() : pImpl(new Impl) {}

bool NeuromodulationTemplate::configure(std::shared_ptr<EnhancedExperiment> experiment,
                                        const std::unordered_map<std::string, std::string>& parameters) {
    pImpl->parameters = parameters;
    
    // Parse parameters
    pImpl->neuronCount = std::stoul(getParameter(parameters, "neuron_count", "100"));
    pImpl->dopamineLevel = std::stof(getParameter(parameters, "dopamine_level", "1.0"));
    pImpl->curiosityLevel = std::stof(getParameter(parameters, "curiosity_level", "0.8"));
    pImpl->noveltyLevel = std::stof(getParameter(parameters, "novelty_level", "0.7"));
    pImpl->serotoninLevel = std::stof(getParameter(parameters, "serotonin_level", "0.5"));
    pImpl->norepinephrineLevel = std::stof(getParameter(parameters, "norepinephrine_level", "0.6"));
    pImpl->acetylcholineLevel = std::stof(getParameter(parameters, "acetylcholine_level", "0.4"));
    pImpl->enableRewardModulation = getParameter(parameters, "enable_reward_modulation", "true") == "true";
    pImpl->enableCuriosity = getParameter(parameters, "enable_curiosity", "true") == "true";
    pImpl->enableNovelty = getParameter(parameters, "enable_novelty", "true") == "true";
    
    // Configure experiment
    experiment->setName("Neuromodulation Effect Experiment");
    experiment->setDescription("Studies effects of neuromodulators on learning and behavior");
    experiment->setTemplate("neuromodulation");
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", pImpl->neuronCount);
    experiment->setConfig(config);
    
    // Set parameters
    experiment->setParameter("dopamine_level", std::to_string(pImpl->dopamineLevel));
    experiment->setParameter("curiosity_level", std::to_string(pImpl->curiosityLevel));
    experiment->setParameter("novelty_level", std::to_string(pImpl->noveltyLevel));
    experiment->setParameter("enable_reward_modulation", std::to_string(pImpl->enableRewardModulation));
    experiment->setParameter("enable_curiosity", std::to_string(pImpl->enableCuriosity));
    experiment->setParameter("enable_novelty", std::to_string(pImpl->enableNovelty));
    
    pImpl->totalSteps = std::stoul(getParameter(parameters, "total_steps", "400"));
    pImpl->currentStep = 0;
    
    NLM_LOG_INFO("Neuromodulation Template configured with " + std::to_string(pImpl->neuronCount) + " neurons");
    return true;
}

bool NeuromodulationTemplate::run(std::shared_ptr<EnhancedExperiment> experiment) {
    NLM_LOG_INFO("Starting Neuromodulation Effect Experiment");
    
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
    
    // Enable neuromodulation systems
    if (pImpl->enableRewardModulation) {
        agent.enableRewardModulation(true);
    }
    
    if (pImpl->enableCuriosity) {
        agent.enableCuriosity(true);
    }
    
    if (pImpl->enableNovelty) {
        // Novelty detection is enabled by default
    }
    
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
        
        // Record neuromodulation metrics
        experiment->recordMetric("dopamine_level", pImpl->dopamineLevel, "neuromodulation");
        experiment->recordMetric("curiosity_level", pImpl->curiosityLevel, "neuromodulation");
        experiment->recordMetric("novelty_level", pImpl->noveltyLevel, "neuromodulation");
        
        // Track neuromodulation effects
        experiment->recordMetric("reward_modulation_active", pImpl->enableRewardModulation ? 1.0 : 0.0, "behavior");
        experiment->recordMetric("curiosity_drive", pImpl->enableCuriosity ? pImpl->curiosityLevel : 0.0, "behavior");
        experiment->recordMetric("novelty_detection", pImpl->enableNovelty ? pImpl->noveltyLevel : 0.0, "behavior");
        
        // Track exploration-exploitation balance
        float explorationRatio = pImpl->curiosityLevel * pImpl->noveltyLevel;
        experiment->recordMetric("exploration_ratio", explorationRatio, "behavior");
        
        // Track learning acceleration
        float learningAcceleration = pImpl->dopamineLevel * (1.0 - pImpl->curiosityLevel);
        experiment->recordMetric("learning_acceleration", learningAcceleration, "performance");
        
        // Track behavioral flexibility
        float behavioralFlexibility = pImpl->curiosityLevel + pImpl->noveltyLevel;
        experiment->recordMetric("behavioral_flexibility", behavioralFlexibility, "behavior");
    }
    
    experiment->complete();
    
    NLM_LOG_INFO("Neuromodulation Effect Experiment completed with " + std::to_string(pImpl->currentStep) + " steps");
    return true;
}

std::vector<std::string> NeuromodulationTemplate::getRequiredParameters() const {
    return {"neuron_count"};
}

std::vector<std::string> NeuromodulationTemplate::getOptionalParameters() const {
    return {"dopamine_level", "curiosity_level", "novelty_level", 
            "serotonin_level", "norepinephrine_level", "acetylcholine_level",
            "enable_reward_modulation", "enable_curiosity", "enable_novelty", 
            "total_steps"};
}

std::unordered_map<std::string, std::string> NeuromodulationTemplate::getDefaultParameters() const {
    return {
        {"neuron_count", "100"},
        {"dopamine_level", "1.0"},
        {"curiosity_level", "0.8"},
        {"novelty_level", "0.7"},
        {"serotonin_level", "0.5"},
        {"norepinephrine_level", "0.6"},
        {"acetylcholine_level", "0.4"},
        {"enable_reward_modulation", "true"},
        {"enable_curiosity", "true"},
        {"enable_novelty", "true"},
        {"total_steps", "400"}
    };
}

std::vector<std::string> NeuromodulationTemplate::getExpectedMetrics() const {
    return {
        "dopamine_level", "curiosity_level", "novelty_level",
        "reward_modulation_active", "curiosity_drive", "novelty_detection",
        "exploration_ratio", "learning_acceleration", "behavioral_flexibility"
    };
}

std::vector<std::string> NeuromodulationTemplate::getPerformanceMetrics() const {
    return {
        "learning_acceleration", "behavioral_flexibility", "exploration_ratio"
    };
}

std::vector<std::string> NeuromodulationTemplate::getDiagnosticMetrics() const {
    return {
        "dopamine_level", "curiosity_level", "novelty_level",
        "reward_modulation_active", "curiosity_drive", "novelty_detection"
    };
}

std::vector<std::string> NeuromodulationTemplate::getValidationRequirements() const {
    return {
        "Neuromodulators should have appropriate levels (0.0 - 2.0)",
        "Reward modulation should be active if enabled",
        "Curiosity drive should be proportional to curiosity level",
        "Novelty detection should be proportional to novelty level",
        "Exploration ratio should balance curiosity and novelty",
        "Learning acceleration should be positive with appropriate neuromodulator combination"
    };
}

} // namespace nlm
