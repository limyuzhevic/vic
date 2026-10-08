#include "experiment_utils.hpp"
#include "../core/Logger/Logger.hpp"
#include "../world/SimpleWorld.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../agent/AgentBrain.hpp"
#include "../agent/SensoryPercept.hpp"
#include "../motor/MotorCommand.hpp"
#include "../world/ActionResult.hpp"

namespace nlm {

std::shared_ptr<Brain> createBrainFromConfig(const Config& config) {
    auto brain = std::make_shared<Brain>(std::make_shared<Config>(config));
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain from config");
        return nullptr;
    }
    return brain;
}

float runSimulationLoop(
    Brain* brain,
    float (*getReward)(Brain*),
    uint64_t steps,
    std::function<void(uint64_t, float)> progressCallback
) {
    if (!brain || !getReward) {
        NLM_LOG_ERROR("Invalid parameters for simulation loop");
        return 0.0f;
    }
    
    float totalReward = 0.0f;
    
    for (uint64_t step = 0; step < steps; ++step) {
        float reward = getReward(brain);
        totalReward += reward;
        
        if (progressCallback) {
            progressCallback(step, reward);
        }
        
        brain->step(step, step * 0.001);
    }
    
    return totalReward;
}

std::vector<float> getSensoryObservation(SimpleWorld& world, Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Invalid brain pointer for observation");
        return std::vector<float>();
    }
    
    SensoryPercept percept = world.getSensoryPercept();
    
    std::vector<float> observation;
    observation.reserve(percept.sensorySignals.size());
    
    for (const auto& signal : percept.sensorySignals) {
        observation.push_back(signal.magnitude);
    }
    
    return observation;
}

Config createDefaultConfigForExperiment(
    size_t neuronCount,
    size_t regionCount,
    float connectionProbability
) {
    Config config;
    config.set("neuron_count", static_cast<int>(neuronCount));
    config.set("region_count", static_cast<int>(regionCount));
    config.set("connection_probability", connectionProbability);
    return config;
}

bool checkWorkingMemoryIntegration(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Brain pointer is null for working memory check");
        return false;
    }
    
    auto* wm = brain->getWorkingMemory();
    if (!wm) {
        NLM_LOG_ERROR("Working memory not integrated");
        return false;
    }
    
    NLM_LOG_INFO("[PASS] Working memory is integrated");
    return true;
}

bool checkEpisodicMemoryIntegration(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Brain pointer is null for episodic memory check");
        return false;
    }
    
    auto* em = brain->getEpisodicMemory();
    if (!em) {
        NLM_LOG_ERROR("Episodic memory not integrated");
        return false;
    }
    
    NLM_LOG_INFO("[PASS] Episodic memory is integrated");
    return true;
}

bool checkNeuromodulationIntegration(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Brain pointer is null for neuromodulation check");
        return false;
    }
    
    auto* dopamine = brain->getDopamine();
    auto* curiosity = brain->getCuriosity();
    auto* novelty = brain->getNovelty();
    
    if (!dopamine || !curiosity || !novelty) {
        NLM_LOG_ERROR("Neuromodulation systems not integrated");
        return false;
    }
    
    NLM_LOG_INFO("[PASS] Neuromodulation systems are integrated");
    return true;
}

bool checkPredictionIntegration(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Brain pointer is null for prediction check");
        return false;
    }
    
    auto* predictor = brain->getPredictionSystem();
    if (!predictor) {
        NLM_LOG_ERROR("Prediction system not integrated");
        return false;
    }
    
    NLM_LOG_INFO("[PASS] Prediction system is integrated");
    return true;
}

bool checkDevelopmentIntegration(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Brain pointer is null for development check");
        return false;
    }
    
    auto* developer = brain->getDevelopmentSystem();
    if (!developer) {
        NLM_LOG_ERROR("Development system not integrated");
        return false;
    }
    
    NLM_LOG_INFO("[PASS] Development system is integrated");
    return true;
}

float computeStandardReward(Brain* brain, const std::vector<float>& observation) {
    if (!brain || observation.empty()) {
        return 0.0f;
    }
    
    float reward = 0.0f;
    
    for (float value : observation) {
        reward += value;
    }
    
    return reward / observation.size();
}

float computeEfficiencyReward(Brain* brain, float baseReward) {
    if (!brain) {
        return 0.0f;
    }
    
    float firingRate = brain->getAverageFiringRate();
    float efficiency = firingRate / 100.0f;  // Normalize to 0-1 scale
    
    return baseReward * efficiency;
}

std::vector<float> processSensoryInput(SimpleWorld& world, Brain* brain, uint64_t step) {
    if (!brain) {
        NLM_LOG_ERROR("Invalid brain pointer for sensory processing");
        return std::vector<float>();
    }
    
    AgentBrain agent(brain);
    agent.initialize(world);
    
    SensoryPercept percept = world.getSensoryPercept();
    agent.processSensoryInput(percept);
    
    return getSensoryObservation(world, brain);
}

void processBrainStep(Brain* brain, uint64_t step, float deltaTime) {
    if (!brain) {
        NLM_LOG_ERROR("Invalid brain pointer for brain step processing");
        return;
    }
    
    brain->step(step, deltaTime);
}

} // namespace nlm
