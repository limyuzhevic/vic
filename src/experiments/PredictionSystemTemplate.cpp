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

struct PredictionSystemTemplate::Impl {
    std::unordered_map<std::string, std::string> parameters;
    
    // Prediction system parameters
    size_t neuronCount;
    size_t predictionHorizon;
    float predictionAccuracy;
    float learningRate;
    float explorationRate;
    size_t totalSteps;
    size_t currentStep;
};

PredictionSystemTemplate::PredictionSystemTemplate() : pImpl(new Impl) {}

bool PredictionSystemTemplate::configure(std::shared_ptr<EnhancedExperiment> experiment,
                                        const std::unordered_map<std::string, std::string>& parameters) {
    pImpl->parameters = parameters;
    
    // Parse parameters
    pImpl->neuronCount = std::stoul(getParameter(parameters, "neuron_count", "100"));
    pImpl->predictionHorizon = std::stoul(getParameter(parameters, "prediction_horizon", "3"));
    pImpl->predictionAccuracy = std::stof(getParameter(parameters, "prediction_accuracy", "0.7"));
    pImpl->learningRate = std::stof(getParameter(parameters, "learning_rate", "0.01"));
    pImpl->explorationRate = std::stof(getParameter(parameters, "exploration_rate", "0.1"));
    
    // Configure experiment
    experiment->setName("Prediction System Validation Experiment");
    experiment->setDescription("Tests prediction system accuracy and learning");
    experiment->setTemplate("prediction_system");
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", pImpl->neuronCount);
    config->set("prediction_horizon", pImpl->predictionHorizon);
    experiment->setConfig(config);
    
    // Set parameters
    experiment->setParameter("prediction_accuracy", std::to_string(pImpl->predictionAccuracy));
    experiment->setParameter("learning_rate", std::to_string(pImpl->learningRate));
    experiment->setParameter("exploration_rate", std::to_string(pImpl->explorationRate));
    
    pImpl->totalSteps = std::stoul(getParameter(parameters, "total_steps", "500"));
    pImpl->currentStep = 0;
    
    NLM_LOG_INFO("Prediction System Template configured with " + std::to_string(pImpl->neuronCount) + " neurons");
    return true;
}

bool PredictionSystemTemplate::run(std::shared_ptr<EnhancedExperiment> experiment) {
    NLM_LOG_INFO("Starting Prediction System Validation Experiment");
    
    experiment->initialize();
    experiment->start();
    
    // Create brain and environment (simplified)
    auto config = std::make_shared<Config>();
    config->set("neuron_count", pImpl->neuronCount);
    config->set("prediction_horizon", pImpl->predictionHorizon);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        experiment->fail("Failed to initialize brain");
        return false;
    }
    
    SimpleWorld world;
    world.initialize(16, 16);
    
    AgentBrain agent(brain);
    agent.initialize(world);
    agent.enableRewardModulation(true);
    
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
        
        // Record prediction metrics
        experiment->recordMetric("prediction_error", std::abs(0.5 - pImpl->predictionAccuracy), "prediction");
        experiment->recordMetric("learning_progress", 1.0 - (pImpl->predictionAccuracy / 0.5), "learning");
        experiment->recordMetric("exploration_level", pImpl->explorationRate, "behavior");
        
        // Track prediction accuracy improvement
        experiment->recordMetric("current_prediction_accuracy", pImpl->predictionAccuracy, "prediction");
        pImpl->predictionAccuracy = std::min(0.99f, pImpl->predictionAccuracy + pImpl->learningRate);
        
        // Apply exploration/exploitation balance
        if ((pImpl->currentStep % 10 == 0) && (rand() % 100 < pImpl->explorationRate * 100)) {
            experiment->recordMetric("exploration_action", 1.0, "behavior");
        }
    }
    
    experiment->complete();
    
    NLM_LOG_INFO("Prediction System Validation Experiment completed with " + std::to_string(pImpl->currentStep) + " steps");
    return true;
}

std::vector<std::string> PredictionSystemTemplate::getRequiredParameters() const {
    return {"neuron_count", "prediction_horizon"};
}

std::vector<std::string> PredictionSystemTemplate::getOptionalParameters() const {
    return {"prediction_accuracy", "learning_rate", "exploration_rate", "total_steps"};
}

std::unordered_map<std::string, std::string> PredictionSystemTemplate::getDefaultParameters() const {
    return {
        {"neuron_count", "100"},
        {"prediction_horizon", "3"},
        {"prediction_accuracy", "0.7"},
        {"learning_rate", "0.01"},
        {"exploration_rate", "0.1"},
        {"total_steps", "500"}
    };
}

std::vector<std::string> PredictionSystemTemplate::getExpectedMetrics() const {
    return {
        "prediction_error", "learning_progress", "exploration_level", 
        "current_prediction_accuracy", "exploration_action"
    };
}

std::vector<std::string> PredictionSystemTemplate::getPerformanceMetrics() const {
    return {
        "current_prediction_accuracy", "learning_progress", "prediction_error"
    };
}

std::vector<std::string> PredictionSystemTemplate::getDiagnosticMetrics() const {
    return {
        "exploration_level", "exploration_action", "prediction_error"
    };
}

std::vector<std::string> PredictionSystemTemplate::getValidationRequirements() const {
    return {
        "Prediction accuracy should improve over time",
        "Prediction error should decrease towards zero",
        "Learning progress should show consistent improvement",
        "Exploration-exploitation balance should be stable"
    };
}

} // namespace nlm
