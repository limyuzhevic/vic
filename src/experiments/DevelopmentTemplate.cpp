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

struct DevelopmentTemplate::Impl {
    std::unordered_map<std::string, std::string> parameters;
    
    // Development parameters
    size_t neuronCount;
    size_t developmentalStages;
    float plasticityRate;
    float maturationRate;
    float synaptogenesisRate;
    float pruningRate;
    bool enableDevelopmentalStages;
    bool enableStructuralPlasticity;
    bool enableExperienceDependent;
    size_t totalSteps;
    size_t currentStep;
    size_t currentStage;
};

DevelopmentTemplate::DevelopmentTemplate() : pImpl(new Impl) {}

bool DevelopmentTemplate::configure(std::shared_ptr<EnhancedExperiment> experiment,
                                   const std::unordered_map<std::string, std::string>& parameters) {
    pImpl->parameters = parameters;
    
    // Parse parameters
    pImpl->neuronCount = std::stoul(getParameter(parameters, "neuron_count", "100"));
    pImpl->developmentalStages = std::stoul(getParameter(parameters, "developmental_stages", "5"));
    pImpl->plasticityRate = std::stof(getParameter(parameters, "plasticity_rate", "0.1"));
    pImpl->maturationRate = std::stof(getParameter(parameters, "maturation_rate", "0.05"));
    pImpl->synaptogenesisRate = std::stof(getParameter(parameters, "synaptogenesis_rate", "0.01"));
    pImpl->pruningRate = std::stof(getParameter(parameters, "pruning_rate", "0.005"));
    pImpl->enableDevelopmentalStages = getParameter(parameters, "enable_developmental_stages", "true") == "true";
    pImpl->enableStructuralPlasticity = getParameter(parameters, "enable_structural_plasticity", "true") == "true";
    pImpl->enableExperienceDependent = getParameter(parameters, "enable_experience_dependent", "true") == "true";
    
    // Configure experiment
    experiment->setName("Development Progression Experiment");
    experiment->setDescription("Studies developmental learning and maturation over time");
    experiment->setTemplate("development");
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", pImpl->neuronCount);
    experiment->setConfig(config);
    
    // Set parameters
    experiment->setParameter("plasticity_rate", std::to_string(pImpl->plasticityRate));
    experiment->setParameter("maturation_rate", std::to_string(pImpl->maturationRate));
    experiment->setParameter("synaptogenesis_rate", std::to_string(pImpl->synaptogenesisRate));
    experiment->setParameter("pruning_rate", std::to_string(pImpl->pruningRate));
    experiment->setParameter("developmental_stages", std::to_string(pImpl->developmentalStages));
    
    pImpl->totalSteps = std::stoul(getParameter(parameters, "total_steps", "600"));
    pImpl->currentStep = 0;
    pImpl->currentStage = 0;
    
    NLM_LOG_INFO("Development Template configured with " + std::to_string(pImpl->neuronCount) + " neurons");
    return true;
}

bool DevelopmentTemplate::run(std::shared_ptr<EnhancedExperiment> experiment) {
    NLM_LOG_INFO("Starting Development Progression Experiment");
    
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
    
    // Run development experiment
    for (pImpl->currentStep = 0; pImpl->currentStep < pImpl->totalSteps; ++pImpl->currentStep) {
        if (!experiment->isRunning()) {
            break;
        }
        
        // Update developmental stage if enabled
        if (pImpl->enableDevelopmentalStages) {
            pImpl->currentStage = (pImpl->currentStep * pImpl->developmentalStages) / pImpl->totalSteps;
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
        
        // Record developmental metrics
        float stageProgress = (float)pImpl->currentStage / (pImpl->developmentalStages - 1);
        experiment->recordMetric("developmental_stage", (float)pImpl->currentStage, "development");
        experiment->recordMetric("stage_progress", stageProgress, "development");
        
        // Record plasticity metrics
        experiment->recordMetric("plasticity_rate", pImpl->plasticityRate, "plasticity");
        experiment->recordMetric("maturation_rate", pImpl->maturationRate, "plasticity");
        
        // Record structural changes
        if (pImpl->enableStructuralPlasticity) {
            // Simulate structural plasticity
            float structuralChange = pImpl->synaptogenesisRate - pImpl->pruningRate;
            experiment->recordMetric("structural_change", structuralChange, "development");
        }
        
        // Record experience-dependent learning
        if (pImpl->enableExperienceDependent) {
            float experienceIntegration = stageProgress * (pImpl->currentStep / (float)pImpl->totalSteps);
            experiment->recordMetric("experience_integration", experienceIntegration, "learning");
        }
        
        // Record developmental milestones
        if (pImpl->currentStep % 50 == 0) {
            experiment->recordMetric("milestone_reached", 1.0, "milestones");
        }
        
        // Record system integration
        experiment->recordMetric("working_memory_integrated", brain->getWorkingMemory() != nullptr, "integration");
        experiment->recordMetric("episodic_memory_integrated", brain->getEpisodicMemory() != nullptr, "integration");
        experiment->recordMetric("prediction_integrated", brain->getPredictionSystem() != nullptr, "integration");
        experiment->recordMetric("development_integrated", brain->getDevelopmentSystem() != nullptr, "integration");
    }
    
    experiment->complete();
    
    NLM_LOG_INFO("Development Progression Experiment completed with " + std::to_string(pImpl->currentStep) + " steps");
    return true;
}

std::vector<std::string> DevelopmentTemplate::getRequiredParameters() const {
    return {"neuron_count", "developmental_stages"};
}

std::vector<std::string> DevelopmentTemplate::getOptionalParameters() const {
    return {"plasticity_rate", "maturation_rate", "synaptogenesis_rate", 
            "pruning_rate", "enable_developmental_stages", 
            "enable_structural_plasticity", "enable_experience_dependent", 
            "total_steps"};
}

std::unordered_map<std::string, std::string> DevelopmentTemplate::getDefaultParameters() const {
    return {
        {"neuron_count", "100"},
        {"developmental_stages", "5"},
        {"plasticity_rate", "0.1"},
        {"maturation_rate", "0.05"},
        {"synaptogenesis_rate", "0.01"},
        {"pruning_rate", "0.005"},
        {"enable_developmental_stages", "true"},
        {"enable_structural_plasticity", "true"},
        {"enable_experience_dependent", "true"},
        {"total_steps", "600"}
    };
}

std::vector<std::string> DevelopmentTemplate::getExpectedMetrics() const {
    return {
        "developmental_stage", "stage_progress", "plasticity_rate",
        "maturation_rate", "structural_change", "experience_integration",
        "milestone_reached", "working_memory_integrated",
        "episodic_memory_integrated", "prediction_integrated",
        "development_integrated"
    };
}

std::vector<std::string> DevelopmentTemplate::getPerformanceMetrics() const {
    return {
        "stage_progress", "experience_integration", "structural_change",
        "developmental_stage"
    };
}

std::vector<std::string> DevelopmentTemplate::getDiagnosticMetrics() const {
    return {
        "plasticity_rate", "maturation_rate", "structural_change",
        "milestone_reached", "development_integrated"
    };
}

std::vector<std::string> DevelopmentTemplate::getValidationRequirements() const {
    return {
        "Developmental stages should progress from 0 to max stages",
        "Stage progress should increase over time",
        "Plasticity rate should remain within bounds (0.0 - 1.0)",
        "Structural change should be positive (net growth)",
        "Experience integration should increase with time if enabled",
        "Milestones should be reached at appropriate intervals",
        "Development system should be properly integrated"
    };
}

} // namespace nlm
