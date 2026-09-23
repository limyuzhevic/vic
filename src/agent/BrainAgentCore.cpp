#include "BrainAgentCore.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// ===========================================
// AGENT CORE FUNCTIONALITY
// ===========================================

void BrainAgentCore::initializeAgentBrain(AgentBrain* agentBrain, std::shared_ptr<Brain> brain) {
    if (!agentBrain || !brain) return;

    new (agentBrain) AgentBrain(brain);
}

void BrainAgentCore::destructAgentBrain(AgentBrain* agentBrain) {
    if (agentBrain) {
        agentBrain->~AgentBrain();
    }
}

void BrainAgentCore::initializeAgent(AgentBrain* agentBrain, const SimpleWorld& world) {
    if (!agentBrain) return;

    agentBrain->initialize(world);
}

void BrainAgentCore::resetAgent(AgentBrain* agentBrain) {
    if (!agentBrain) return;

    agentBrain->reset();
}

// ===========================================
// MOTOR DECODING AND ACTION SELECTION
// ===========================================

MotorCommand BrainAgentCore::decodeMotorCommand(AgentBrain* agentBrain) {
    if (!agentBrain) return MotorCommand::Wait;

    return agentBrain->decodeMotorCommand();
}

MotorCommand BrainAgentCore::decodeFromMotorNeurons(AgentBrain* agentBrain) {
    if (!agentBrain) return MotorCommand::Wait;

    return agentBrain->decodeFromMotorNeurons();
}

MotorCommand BrainAgentCore::selectWithCuriosity(AgentBrain* agentBrain, MotorCommand defaultCmd) {
    if (!agentBrain) return defaultCmd;

    return agentBrain->selectWithCuriosity(defaultCmd);
}

// ===========================================
// SENSORY PROCESSING
// ===========================================

void BrainAgentCore::processSensoryInput(AgentBrain* agentBrain, const SensoryPercept& percept) {
    if (!agentBrain) return;

    agentBrain->processSensoryInput(percept);
}

void BrainAgentCore::computeNovelty(AgentBrain* agentBrain, const std::vector<float>& vision,
                                   const std::vector<float>& previousVision, float& noveltyLevel) {
    if (!agentBrain) return;

    // Compute novelty (difference from previous vision)
    float totalDiff = 0.0f;
    for (size_t i = 0; i < vision.size() && i < previousVision.size(); ++i) {
        float diff = std::abs(vision[i] - previousVision[i]);
        totalDiff += diff;
    }

    // Normalize
    noveltyLevel = totalDiff / std::max<size_t>(vision.size(), 1);
    noveltyLevel *= 0.99f;  // Decay
}

// ===========================================
// NEUROMODULATION AND LEARNING
// ===========================================

void BrainAgentCore::applyRewardModulation(AgentBrain* agentBrain, float reward, float predictedReward) {
    if (!agentBrain) return;

    agentBrain->applyRewardModulation(reward, predictedReward);
}

void BrainAgentCore::updateDevelopment(AgentBrain* agentBrain, double timestep) {
    if (!agentBrain) return;

    agentBrain->updateDevelopment(timestep);
}

void BrainAgentCore::updateNeuromodulation(AgentBrain* agentBrain, const Neuromodulator& signal) {
    if (!agentBrain) return;

    // Apply neuromodulation based on prediction error
    float predictionError = signal.getLevel();
    float reward = signal.getLevel();  // Simplified - in real implementation would be separate

    agentBrain->applyRewardModulation(reward, predictionError);
}

// ===========================================
// CONFIGURATION AND STATE ACCESSORS
// ===========================================

Brain* BrainAgentCore::getBrain(AgentBrain* agentBrain) {
    if (!agentBrain) return nullptr;
    return agentBrain->getBrain();
}

std::vector<Neuron*> BrainAgentCore::getMotorNeurons(AgentBrain* agentBrain) {
    if (!agentBrain) return {};

    std::vector<Neuron*> allMotorNeurons;
    for (auto* neurons : {
        agentBrain->getMotorForward(),
        agentBrain->getMotorBackward(),
        agentBrain->getMotorTurnLeft(),
        agentBrain->getMotorTurnRight(),
        agentBrain->getMotorInteract(),
        agentBrain->getMotorWait()
    }) {
        allMotorNeurons.insert(allMotorNeurons.end(), neurons.begin(), neurons.end());
    }
    return allMotorNeurons;
}

std::vector<Neuron*> BrainAgentCore::getSensoryNeurons(AgentBrain* agentBrain) {
    if (!agentBrain) return {};

    std::vector<Neuron*> allSensoryNeurons;
    for (auto* neurons : {
        agentBrain->getSensoryVision(),
        agentBrain->getSensoryTouch(),
        agentBrain->getSensoryInternal(),
        agentBrain->getSensoryProprioception()
    }) {
        allSensoryNeurons.insert(allSensoryNeurons.end(), neurons.begin(), neurons.end());
    }
    return allSensoryNeurons;
}

// ===========================================
// STATISTICAL INFORMATION
// ===========================================

float BrainAgentCore::getNeuromodulationLevel(AgentBrain* agentBrain) {
    if (!agentBrain) return 0.0f;
    return agentBrain->getNeuromodulationLevel();
}

float BrainAgentCore::getCuriosityLevel(AgentBrain* agentBrain) {
    if (!agentBrain) return 0.0f;
    return agentBrain->getCuriosityLevel();
}

float BrainAgentCore::getNoveltyLevel(AgentBrain* agentBrain) {
    if (!agentBrain) return 0.0f;
    return agentBrain->getNoveltyLevel();
}

float BrainAgentCore::getPredictionError(AgentBrain* agentBrain) {
    if (!agentBrain) return 0.0f;
    return agentBrain->getPredictionError();
}

DevelopmentalStage BrainAgentCore::getDevelopmentalStage(AgentBrain* agentBrain) {
    if (!agentBrain) return DevelopmentalStage::Initial;
    return agentBrain->getDevelopmentalStage();
}

// ===========================================
// CONFIGURATION FLAGS
// ===========================================

void BrainAgentCore::enableRewardModulation(AgentBrain* agentBrain, bool enable) {
    if (!agentBrain) return;
    agentBrain->enableRewardModulation(enable);
}

void BrainAgentCore::enableStructuralPlasticity(AgentBrain* agentBrain, bool enable) {
    if (!agentBrain) return;
    agentBrain->enableStructuralPlasticity(enable);
}

void BrainAgentCore::enableDevelopment(AgentBrain* agentBrain, bool enable) {
    if (!agentBrain) return;
    agentBrain->enableDevelopment(enable);
}

void BrainAgentCore::enableCuriosity(AgentBrain* agentBrain, bool enable) {
    if (!agentBrain) return;
    agentBrain->enableCuriosity(enable);
}

bool BrainAgentCore::isRewardModulationEnabled(AgentBrain* agentBrain) {
    if (!agentBrain) return false;
    return agentBrain->isRewardModulationEnabled();
}

bool BrainAgentCore::isStructuralPlasticityEnabled(AgentBrain* agentBrain) {
    if (!agentBrain) return false;
    return agentBrain->isStructuralPlasticityEnabled();
}

bool BrainAgentCore::isDevelopmentEnabled(AgentBrain* agentBrain) {
    if (!agentBrain) return false;
    return agentBrain->isDevelopmentEnabled();
}

bool BrainAgentCore::isCuriosityEnabled(AgentBrain* agentBrain) {
    if (!agentBrain) return false;
    return agentBrain->isCuriosityEnabled();
}

// ===========================================
// UTILITY FUNCTIONS
// ===========================================

void BrainAgentCore::logAgentStatus(AgentBrain* agentBrain) {
    if (!agentBrain) return;

    NLM_LOG_INFO("=== AgentBrain Status ===");
    NLM_LOG_INFO("Neuromodulation level: " + std::to_string(agentBrain->getNeuromodulationLevel()));
    NLM_LOG_INFO("Curiosity level: " + std::to_string(agentBrain->getCuriosityLevel()));
    NLM_LOG_INFO("Novelty level: " + std::to_string(agentBrain->getNoveltyLevel()));
    NLM_LOG_INFO("Prediction error: " + std::to_string(agentBrain->getPredictionError()));
    NLM_LOG_INFO("Developmental stage: " + std::to_string(static_cast<int>(agentBrain->getDevelopmentalStage())));
}

} // namespace nlm
