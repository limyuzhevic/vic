#include "agent/AgentCore.hpp"
#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>
#include <random>

namespace nlm {

bool AgentCore::validateAgentConfiguration(const std::shared_ptr<const Config>& config) {
    if (!config) {
        NLM_LOG_ERROR("Configuration is null in AgentCore validation");
        return false;
    }
    
    // Validate agent-specific configuration
    bool valid = true;
    
    auto motorCount = config->getOr<size_t>("motor_neuron_count", 50);
    if (motorCount == 0 || motorCount > 1000) {
        NLM_LOG_ERROR("Invalid motor_neuron_count: " + std::to_string(motorCount));
        valid = false;
    }
    
    auto sensoryCount = config->getOr<size_t>("sensory_neuron_count", 100);
    if (sensoryCount == 0 || sensoryCount > 2000) {
        NLM_LOG_ERROR("Invalid sensory_neuron_count: " + std::to_string(sensoryCount));
        valid = false;
    }
    
    auto curiosityThreshold = config->getOr<float>("curiosity_threshold", 0.5f);
    if (curiosityThreshold < 0.0f || curiosityThreshold > 1.0f) {
        NLM_LOG_ERROR("Invalid curiosity_threshold: " + std::to_string(curiosityThreshold));
        valid = false;
    }
    
    return valid;
}

void AgentCore::initializeAgentBody(AgentBody& agent, const SimpleWorld& world) {
    agent.initialize(world);
}

void AgentCore::distributeMotorNeurons(AgentBody& agent, const Brain& brain) {
    // Distribute motor neurons to different action groups
    // This is a simplified implementation
    auto& motorGroups = agent.getMotorGroups();
    
    // Clear existing motor groups
    motorGroups.clear();
    
    // Get all motor neurons from brain
    auto* brainPtr = brain.getBrain();  // Simplified - should get from agent
    // In reality, this would involve accessing the brain's motor neurons
    
    // For now, create placeholder motor groups
    motorGroups.push_back(std::vector<Neuron*>());  // Forward
    motorGroups.push_back(std::vector<Neuron*>());  // Backward
    motorGroups.push_back(std::vector<Neuron*>());  // Left
    motorGroups.push_back(std::vector<Neuron*>());  // Right
    motorGroups.push_back(std::vector<Neuron*>());  // Interact
    motorGroups.push_back(std::vector<Neuron*>());  // Wait
}

void AgentCore::distributeSensoryNeurons(AgentBody& agent, const Brain& brain) {
    // Distribute sensory neurons to different sensory groups
    auto& sensoryGroups = agent.getSensoryGroups();
    
    // Clear existing sensory groups
    sensoryGroups.clear();
    
    // Create sensory groups for vision, touch, internal, proprioception
    sensoryGroups.push_back(std::vector<Neuron*>());  // Vision
    sensoryGroups.push_back(std::vector<Neuron*>());  // Touch
    sensoryGroups.push_back(std::vector<Neuron*>());  // Internal
    sensoryGroups.push_back(std::vector<Neuron*>());  // Proprioception
}

void AgentCore::setupSensoryGroups(AgentBody& agent, const Brain& brain) {
    // Setup sensory groups with appropriate neurons
    // This would involve accessing the brain's sensory neuron populations
    
    auto& sensoryGroups = agent.getSensoryGroups();
    if (sensoryGroups.size() >= 4) {
        // Get brain reference (simplified)
        // auto* brainPtr = brain.getBrain();
        
        // For now, create placeholder sensory neurons
        // In reality, this would access the brain's sensory neuron populations
        
        // Create placeholder sensory neurons
        for (size_t i = 0; i < 10; ++i) {  // 10 neurons per sensory group
            // auto* neuron = /* get from brain sensory populations */
            // sensoryGroups[0].push_back(neuron);  // Vision
            // sensoryGroups[1].push_back(neuron);  // Touch
            // sensoryGroups[2].push_back(neuron);  // Internal
            // sensoryGroups[3].push_back(neuron);  // Proprioception
        }
    }
}

void AgentCore::setupMotorGroups(AgentBody& agent, const Brain& brain) {
    // Setup motor groups with appropriate neurons
    // This would involve accessing the brain's motor neuron populations
    
    auto& motorGroups = agent.getMotorGroups();
    if (motorGroups.size() >= 6) {
        // Get brain reference (simplified)
        // auto* brainPtr = brain.getBrain();
        
        // For now, create placeholder motor neurons
        // In reality, this would access the brain's motor neuron populations
        
        // Create placeholder motor neurons
        for (size_t i = 0; i < 10; ++i) {  // 10 neurons per motor group
            // auto* neuron = /* get from brain motor populations */
            // motorGroups[0].push_back(neuron);  // Forward
            // motorGroups[1].push_back(neuron);  // Backward
            // motorGroups[2].push_back(neuron);  // Left
            // motorGroups[3].push_back(neuron);  // Right
            // motorGroups[4].push_back(neuron);  // Interact
            // motorGroups[5].push_back(neuron);  // Wait
        }
    }
}

void AgentCore::processVisionInput(AgentBody& agent, const std::vector<float>& visionData) {
    if (visionData.empty()) return;
    
    auto& sensoryGroups = agent.getSensoryGroups();
    if (sensoryGroups.size() > 0) {
        // Process vision input and inject into vision sensory neurons
        // This would involve accessing the vision sensory group neurons
        
        // For now, store vision data in agent
        agent.setVisionData(visionData);
        
        // Calculate novelty if we have previous vision data
        auto previousVision = agent.getPreviousVision();
        if (!previousVision.empty()) {
            float novelty = calculateVisionNovelty(visionData, previousVision);
            agent.setNoveltyLevel(novelty);
        }
    }
}

void AgentCore::processTouchInput(AgentBody& agent, const std::vector<float>& touchData) {
    if (touchData.empty()) return;
    
    // Process touch input and inject into touch sensory neurons
    // Similar to vision input processing
    agent.setTouchData(touchData);
}

void AgentCore::processInternalInput(AgentBody& agent, const std::vector<float>& internalData) {
    if (internalData.empty()) return;
    
    // Process internal input and inject into internal sensory neurons
    agent.setInternalData(internalData);
}

void AgentCore::processProprioceptionInput(AgentBody& agent, const std::vector<float>& proprioceptionData) {
    if (proprioceptionData.empty()) return;
    
    // Process proprioception input and inject into proprioception sensory neurons
    agent.setProprioceptionData(proprioceptionData);
}

void AgentCore::computeNovelty(AgentBody& agent, const std::vector<float>& currentVision) {
    auto previousVision = agent.getPreviousVision();
    if (previousVision.empty()) {
        agent.setNoveltyLevel(1.0f);  // Maximum novelty for first input
        return;
    }
    
    float novelty = calculateVisionNovelty(currentVision, previousVision);
    agent.setNoveltyLevel(novelty);
    
    // Decay novelty over time
    agent.setNoveltyLevel(agent.getNoveltyLevel() * agent.getSensoryNoveltyDecay());
}

void AgentCore::updateCuriosity(AgentBody& agent, float noveltyLevel, float predictionError) {
    if (agent.isCuriosityEnabled()) {
        float curiosityLevel = noveltyLevel * 2.0f + std::abs(predictionError) * 0.5f;
        curiosityLevel = std::clamp(curiosityLevel, 0.0f, 1.0f);
        agent.setCuriosityLevel(curiosityLevel);
    }
}

MotorCommand AgentCore::decodeMotorCommand(AgentBody& agent) {
    MotorCommand decoded = decodeFromMotorNeurons(agent);
    
    // Apply curiosity-based exploration
    if (agent.isCuriosityEnabled() && agent.getCuriosityLevel() > 0.3f) {
        decoded = selectWithCuriosity(agent, decoded);
    }
    
    return decoded;
}

MotorCommand AgentCore::decodeFromMotorNeurons(AgentBody& agent) {
    // Calculate average activity in each motor group
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
        }
        return sum / neurons.size();
    };
    
    auto& motorGroups = agent.getMotorGroups();
    if (motorGroups.size() < 6) {
        return MotorCommand::Wait;
    }
    
    float forwardAct = calcActivity(motorGroups[0]);
    float backwardAct = calcActivity(motorGroups[1]);
    float leftAct = calcActivity(motorGroups[2]);
    float rightAct = calcActivity(motorGroups[3]);
    float interactAct = calcActivity(motorGroups[4]);
    float waitAct = calcActivity(motorGroups[5]);
    
    // Find maximum activity
    struct { MotorCommand cmd; float activity; } commands[] = {
        {MotorCommand::MoveForward, forwardAct},
        {MotorCommand::MoveBackward, backwardAct},
        {MotorCommand::TurnLeft, leftAct},
        {MotorCommand::TurnRight, rightAct},
        {MotorCommand::Interact, interactAct},
        {MotorCommand::Wait, waitAct}
    };
    
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = waitAct;  // Default to wait if nothing stronger
    
    for (const auto& c : commands) {
        if (c.activity > bestActivity) {
            bestActivity = c.activity;
            best = c.cmd;
        }
    }
    
    // Only act if there's meaningful activity
    if (bestActivity < 0.5f) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentCore::selectWithCuriosity(AgentBody& agent, MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (agent.getCuriosityLevel() > 0.5f) {
        // Higher curiosity = more exploration
        float exploreChance = agent.getCuriosityLevel() * 0.3f;  // Up to 30% random
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(0.0f, 1.0f);
        
        if (dis(gen) < exploreChance) {
            // Random motor command
            int choice = std::uniform_int_distribution<int>(0, 7)(gen);
            switch (choice) {
                case 0: return MotorCommand::MoveForward;
                case 1: return MotorCommand::MoveBackward;
                case 2: return MotorCommand::TurnLeft;
                case 3: return MotorCommand::TurnRight;
                case 4: return MotorCommand::LookLeft;
                case 5: return MotorCommand::LookRight;
                case 6: return MotorCommand::Interact;
                default: return MotorCommand::Wait;
            }
        }
    }
    
    return defaultCmd;
}

void AgentCore::applyRewardModulation(AgentBody& agent, float reward, float predictedReward) {
    if (!agent.isRewardModulationEnabled()) return;
    
    // Apply reward and prediction error
    agent.applyRewardModulation(reward, predictedReward);
}

void AgentCore::updateDevelopment(AgentBody& agent, double timestep) {
    if (!agent.isDevelopmentEnabled()) return;
    
    agent.updateDevelopment(timestep);
}

void AgentCore::updatePlasticityRates(AgentBody& agent, double timestep) {
    if (agent.isStructuralPlasticityEnabled()) {
        // Update structural plasticity rates based on development
        // This would involve accessing the brain's development system
        
        // For now, just update agent's development system
        agent.updateDevelopment(timestep);
    }
}

std::string AgentCore::formatAgentStatistics(const AgentBody& agent, const Brain& brain) {
    std::ostringstream oss;
    oss << "=== Agent Statistics ===\n";
    oss << "Action: " << motorCommandToString(agent.getCurrentAction()) << "\n";
    oss << "Curiosity level: " << agent.getCuriosityLevel() << "\n";
    oss << "Novelty level: " << agent.getNoveltyLevel() << "\n";
    oss << "Prediction error: " << agent.getPredictionError() << "\n";
    oss << "Developmental age: " << agent.getDevelopmentalAge() << "\n";
    oss << "Plasticity modifier: " << agent.getPlasticityModifier() << "\n";
    oss << "Sensory groups: " << agent.getSensoryGroups().size() << "\n";
    oss << "Motor groups: " << agent.getMotorGroups().size() << "\n";
    oss << "Brain neurons: " << brain.getTotalNeuronCount() << "\n";
    oss << "Brain synapses: " << brain.getTotalSynapseCount() << "\n";
    
    return oss.str();
}

float AgentCore::calculateActionSelectionConfidence(AgentBody& agent) {
    // Calculate confidence in action selection based on motor activity
    float totalActivity = 0.0f;
    size_t activeGroups = 0;
    
    auto& motorGroups = agent.getMotorGroups();
    for (const auto& group : motorGroups) {
        if (!group.empty()) {
            float activity = calculateMotorActivity(agent, group);
            if (activity > 0.5f) {
                totalActivity += activity;
                activeGroups++;
            }
        }
    }
    
    if (activeGroups == 0) return 0.0f;
    return totalActivity / activeGroups;
}

size_t AgentCore::getActiveMotorGroups(const AgentBody& agent) {
    size_t active = 0;
    for (const auto& group : agent.getMotorGroups()) {
        if (!group.empty()) {
            active++;
        }
    }
    return active;
}

float AgentCore::getAverageMotorActivity(AgentBody& agent) {
    float totalActivity = 0.0f;
    size_t activeGroups = 0;
    
    auto& motorGroups = agent.getMotorGroups();
    for (const auto& group : motorGroups) {
        if (!group.empty()) {
            float activity = calculateMotorActivity(agent, group);
            totalActivity += activity;
            activeGroups++;
        }
    }
    
    if (activeGroups == 0) return 0.0f;
    return totalActivity / activeGroups;
}

bool AgentCore::validateAgentConfig(const std::shared_ptr<const Config>& config) {
    if (!config) {
        return false;
    }
    
    // Validate agent-specific configuration
    auto motorCount = config->getOr<size_t>("motor_neuron_count", 50);
    if (motorCount < 10 || motorCount > 1000) {
        NLM_LOG_ERROR("Invalid motor_neuron_count: " + std::to_string(motorCount));
        return false;
    }
    
    auto sensoryCount = config->getOr<size_t>("sensory_neuron_count", 100);
    if (sensoryCount < 20 || sensoryCount > 2000) {
        NLM_LOG_ERROR("Invalid sensory_neuron_count: " + std::to_string(sensoryCount));
        return false;
    }
    
    return true;
}

void AgentCore::applyDefaultAgentSettings(std::shared_ptr<Config> config) {
    if (!config) {
        return;
    }
    
    // Apply default agent settings
    config->set("motor_neuron_count", 50);
    config->set("sensory_neuron_count", 100);
    config->set("curiosity_threshold", 0.5f);
    config->set("reward_modulation_enabled", true);
    config->set("structural_plasticity_enabled", true);
    config->set("development_enabled", true);
    config->set("curiosity_enabled", true);
}

void AgentCore::recordExperience(AgentBody& agent, const std::vector<float>& stimulus, 
                                MotorCommand action, float outcome) {
    // Record experience for learning
    // This would be used for action selection learning
    
    std::string experienceKey = generateExperienceKey(stimulus, action);
    float actionValue = calculateActionValue(agent, action, stimulus);
    
    // Update agent's experience record
    // This would be integrated with agent's learning system
}

MotorCommand AgentCore::selectActionWithLearning(AgentBody& agent, const std::vector<float>& stimulus) {
    // Select action based on learned values and current state
    // This is a simplified reinforcement learning approach
    
    // For now, fall back to standard action selection
    return decodeMotorCommand(agent);
}

float AgentCore::calculateActionValue(AgentBody& agent, MotorCommand action, const std::vector<float>& stimulus) {
    // Calculate value of taking a specific action in a given state
    // This would be based on learned experience
    
    // Simple calculation for demonstration
    float baseValue = 0.5f;
    
    // Adjust based on action type
    switch (action) {
        case MotorCommand::MoveForward:
            baseValue += 0.2f;
            break;
        case MotorCommand::MoveBackward:
            baseValue -= 0.1f;
            break;
        case MotorCommand::TurnLeft:
            baseValue += 0.1f;
            break;
        case MotorCommand::TurnRight:
            baseValue += 0.1f;
            break;
        case MotorCommand::Interact:
            baseValue += 0.3f;
            break;
        case MotorCommand::Wait:
            baseValue -= 0.2f;
            break;
        default:
            break;
    }
    
    return std::clamp(baseValue, 0.0f, 1.0f);
}

float AgentCore::calculateVisionNovelty(const std::vector<float>& current, const std::vector<float>& previous) {
    if (current.empty() || previous.empty()) return 0.0f;
    
    float totalDiff = 0.0f;
    for (size_t i = 0; i < std::min(current.size(), previous.size()); ++i) {
        totalDiff += std::abs(current[i] - previous[i]);
    }
    
    return totalDiff / std::min(current.size(), previous.size());
}

float AgentCore::calculateMotorActivity(AgentBody& agent, const std::vector<Neuron*>& neurons) {
    if (neurons.empty()) return 0.0f;
    float sum = 0.0f;
    for (Neuron* n : neurons) {
        sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
    }
    return sum / neurons.size();
}

// AgentUtils namespace functions
namespace AgentUtils {
    std::string motorCommandToString(MotorCommand cmd) {
        switch (cmd) {
            case MotorCommand::MoveForward: return "MoveForward";
            case MotorCommand::MoveBackward: return "MoveBackward";
            case MotorCommand::TurnLeft: return "TurnLeft";
            case MotorCommand::TurnRight: return "TurnRight";
            case MotorCommand::Interact: return "Interact";
            case MotorCommand::Wait: return "Wait";
            case MotorCommand::LookLeft: return "LookLeft";
            case MotorCommand::LookRight: return "LookRight";
            default: return "Unknown";
        }
    }
    
    MotorCommand stringToMotorCommand(const std::string& str) {
        if (str == "MoveForward") return MotorCommand::MoveForward;
        if (str == "MoveBackward") return MotorCommand::MoveBackward;
        if (str == "TurnLeft") return MotorCommand::TurnLeft;
        if (str == "TurnRight") return MotorCommand::TurnRight;
        if (str == "Interact") return MotorCommand::Interact;
        if (str == "Wait") return MotorCommand::Wait;
        if (str == "LookLeft") return MotorCommand::LookLeft;
        if (str == "LookRight") return MotorCommand::LookRight;
        return MotorCommand::Wait;
    }
    
    bool isActionTypeValid(MotorCommand cmd) {
        return cmd >= MotorCommand::MoveForward && cmd <= MotorCommand::LookRight;
    }
    
    float getVisionDataQuality(const SensoryPercept& percept) {
        const auto& vision = percept.getVision();
        if (vision.empty()) return 0.0f;
        float maxVal = *std::max_element(vision.begin(), vision.end());
        float minVal = *std::min_element(vision.begin(), vision.end());
        return (maxVal - minVal) / std::max(1.0f, maxVal + 1.0f);
    }
    
    float getTouchDataQuality(const SensoryPercept& percept) {
        const auto& touch = percept.getTouch();
        if (touch.empty()) return 0.0f;
        float total = std::accumulate(touch.begin(), touch.end(), 0.0f);
        return total / touch.size();
    }
    
    float getInternalDataQuality(const SensoryPercept& percept) {
        const auto& internal = percept.getInternal();
        if (internal.empty()) return 0.0f;
        float variance = 0.0f;
        float mean = std::accumulate(internal.begin(), internal.end(), 0.0f) / internal.size();
        for (float val : internal) {
            variance += (val - mean) * (val - mean);
        }
        variance /= internal.size();
        return std::sqrt(variance);
    }
    
    std::string generateExperienceKey(const std::vector<float>& stimulus, MotorCommand action) {
        std::string key = "exp_" + std::to_string(stimulus.size()) + "_";
        for (float val : stimulus) {
            key += std::to_string(static_cast<int>(val)) + "_";
        }
        key += std::to_string(static_cast<int>(action));
        return key;
    }
    
    float calculateExperienceOutcome(const std::vector<float>& experience, MotorCommand action) {
        // Calculate outcome based on experience and action
        // This is a simplified calculation
        float baseOutcome = 0.5f;
        
        switch (action) {
            case MotorCommand::MoveForward:
                baseOutcome += 0.3f;
                break;
            case MotorCommand::Interact:
                baseOutcome += 0.5f;
                break;
            case MotorCommand::Wait:
                baseOutcome -= 0.2f;
                break;
            default:
                break;
        }
        
        return std::clamp(baseOutcome, 0.0f, 1.0f);
    }
    
    float calculateExplorationRate(const AgentBody& agent) {
        return agent.getCuriosityLevel();
    }
    
    float calculateExploitationRate(const AgentBody& agent) {
        float exploration = agent.getCuriosityLevel();
        return 1.0f - exploration;
    }
    
    float calculateLearningRate(const AgentBody& agent) {
        // Calculate learning rate based on neuromodulation and development
        float learningRate = 0.1f;
        
        if (agent.getCuriosityLevel() > 0.5f) {
            learningRate *= 1.5f;
        }
        
        if (agent.getDevelopmentalAge() > 100.0) {
            learningRate *= 0.8f;
        }
        
        return std::clamp(learningRate, 0.01f, 1.0f);
    }
}

} // namespace nlm
