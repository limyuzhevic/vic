// Enhanced AgentBrain implementation with cognitive integration
#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(brain)
    , dopamineLevel_(0.0f)
    , noveltyLevel_(0.0f)
    , curiosityLevel_(0.0f)
    , predictionError_(0.0f)
    , expectedReward_(0.0f)
    , developmentalAge_(0.0)
    , plasticityModifier_(1.0f)
    , rewardModulationEnabled_(true)
    , structuralPlasticityEnabled_(true)
    , developmentEnabled_(true)
    , curiosityEnabled_(true)
    , conceptFormationEnabled_(true)
    , planningEnabled_(true)
    , selfModelEnabled_(true)
    , socialLearningEnabled_(true)
    , memoryConsolidationEnabled_(true)
    , sensoryNoveltyDecay_(0.99f)
{
    // Initialize motor neuron groups
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                if (pop->getNeuronType() == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
                        motorForward_.push_back(n);
                    }
                } else if (pop->getNeuronType() == NeuronType::Sensory) {
                    // Store sensory neurons for later access
                    // Note: In the enhanced brain, use brain_->getSensoryNeurons()
                }
            }
        }
    }
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    NLM_LOG_INFO("AgentBrain initialized");
}

size_t AgentBrain::getSensoryInputSize() const {
    return 256 + 8 + 4 + 6;  // Vision + touch + internal + proprioception
}

size_t AgentBrain::getMotorOutputSize() const {
    return 6;  // One motor neuron per action
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // Process each sensory modality
    const auto& vision = percept.getVision();
    const auto& touch = percept.getTouch();
    const auto& internal = percept.getInternal();
    const auto& proprioception = percept.getProprioception();
    
    // Update novelty detection
    float sensoryVariability = calculateSensoryVariability(percept);
    noveltyLevel_ = std::max(noveltyLevel_, sensoryVariability);
    
    // Inject sensory input into brain
    brain_->receiveSensoryInput(percept);
    
    // Update working memory if available
    if (workingMemory_) {
        workingMemory_->storeToNeuron(brain_->getSensoryNeurons()[0]->getId(), vision[0] * 0.1f);
    }
    
    // Update episodic memory if available
    if (episodicMemory_) {
        EpisodicMemoryItem episode;
        episode.timestamp = developmentalAge_;
        episode.reward = expectedReward_;
        episode.sensoryPattern = concatenateSensoryPatterns(percept);
        episodicMemory_->storeEpisode(episode);
    }
    
    // Update concept formation if enabled
    if (conceptFormationEnabled_ && conceptFormation_) {
        conceptFormation_->updateSensoryInput(percept);
    }
    
    // Update prediction system if available
    if (predictionSystem_) {
        predictionSystem_->updateSensory(percept);
    }
    
    // Update self-model if available
    if (selfModelEnabled_ && selfModel_) {
        selfModel_->update(percept);
    }
    
    // Decay novelty level
    decayNoveltyLevel();
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        decoded = selectWithCuriosity(decoded);
    }
    
    // Apply planning if available and enabled
    if (planningEnabled_ && planner_ && decoded != MotorCommand::Wait) {
        MotorCommand planned = planner_->planAction(*this);
        if (planned != MotorCommand::Wait) {
            decoded = planned;
        }
    }
    
    // Apply social learning if available and enabled
    if (socialLearningEnabled_ && socialLearning_ && decoded != MotorCommand::Wait) {
        MotorCommand socialAction = socialLearning_->imitateOtherAgent(*this);
        if (socialAction != MotorCommand::Wait) {
            decoded = socialAction;
        }
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
        }
        return sum / neurons.size();
    };
    
    float forwardAct = calcActivity(motorForward_);
    float backwardAct = calcActivity(motorBackward_);
    float leftAct = calcActivity(motorTurnLeft_);
    float rightAct = calcActivity(motorTurnRight_);
    float interactAct = calcActivity(motorInteract_);
    float waitAct = calcActivity(motorWait_);
    
    struct { MotorCommand cmd; float activity; } commands[] = {
        {MotorCommand::MoveForward, forwardAct},
        {MotorCommand::MoveBackward, backwardAct},
        {MotorCommand::TurnLeft, leftAct},
        {MotorCommand::TurnRight, rightAct},
        {MotorCommand::Interact, interactAct},
        {MotorCommand::Wait, waitAct}
    };
    
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = waitAct;
    
    for (const auto& c : commands) {
        if (c.activity > bestActivity) {
            bestActivity = c.activity;
            best = c.cmd;
        }
    }
    
    if (bestActivity < 0.5f) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    if (curiosityLevel_ > 0.5f) {
        float exploreChance = curiosityLevel_ * 0.3f;
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            int choice = brain_->getRandomGenerator()->uniformInt(0, 7);
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

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    predictionError_ = reward - predictedReward;
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    dopamineLevel_ = predictionError_;
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Update brain systems
    if (dopamine_) {
        dopamine_->setLevel(dopamineLevel_);
    }
    if (predictionErrorSignal_) {
        predictionErrorSignal_->setError(predictionError_);
    }
    
    // Apply neuromodulation effects to brain
    brain_->applyNeuromodulation(*dopamine_);
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    // Simple developmental stages
    if (developmentalAge_ < 60.0) {
        plasticityModifier_ = 1.0f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
        if (workingMemory_) {
            workingMemory_->setCapacity(200);
        }
    } else if (developmentalAge_ < 300.0) {
        plasticityModifier_ = 0.8f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
        if (workingMemory_) {
            workingMemory_->setCapacity(150);
        }
    } else if (developmentalAge_ < 900.0) {
        plasticityModifier_ = 0.5f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
        if (workingMemory_) {
            workingMemory_->setCapacity(100);
        }
    } else {
        plasticityModifier_ = 0.2f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
        if (workingMemory_) {
            workingMemory_->setCapacity(80);
        }
    }
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    if (!brain_) return DevelopmentalStage::Initial;
    return brain_->getDevelopmentalStage();
}

float AgentBrain::getNeuromodulationLevel() const {
    return dopamineLevel_;
}

float AgentBrain::getCuriosityLevel() const {
    return curiosityLevel_;
}

float AgentBrain::getNoveltyLevel() const {
    return noveltyLevel_;
}

float AgentBrain::getPredictionError() const {
    return predictionError_;
}

void AgentBrain::reset() {
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
}

// Helper methods
float AgentBrain::calculateSensoryVariability(const SensoryPercept& percept) const {
    float totalVariability = 0.0f;
    int count = 0;
    
    const auto& vision = percept.getVision();
    float visionMean = calculateMean(vision);
    for (float val : vision) {
        totalVariability += std::abs(val - visionMean);
        count++;
    }
    
    const auto& touch = percept.getTouch();
    float touchMean = calculateMean(touch);
    for (float val : touch) {
        totalVariability += std::abs(val - touchMean);
        count++;
    }
    
    const auto& internal = percept.getInternal();
    float internalMean = calculateMean(internal);
    for (float val : internal) {
        totalVariability += std::abs(val - internalMean);
        count++;
    }
    
    const auto& proprioception = percept.getProprioception();
    float proprioMean = calculateMean(proprioception);
    for (float val : proprioception) {
        totalVariability += std::abs(val - proprioMean);
        count++;
    }
    
    return count > 0 ? totalVariability / count : 0.0f;
}

float AgentBrain::calculateMean(const std::vector<float>& values) const {
    if (values.empty()) return 0.0f;
    float sum = 0.0f;
    for (float val : values) {
        sum += val;
    }
    return sum / values.size();
}

void AgentBrain::decayNoveltyLevel() {
    noveltyLevel_ *= sensoryNoveltyDecay_;
}

std::vector<float> AgentBrain::concatenateSensoryPatterns(const SensoryPercept& percept) const {
    std::vector<float> pattern;
    
    const auto& vision = percept.getVision();
    pattern.insert(pattern.end(), vision.begin(), vision.end());
    
    const auto& touch = percept.getTouch();
    pattern.insert(pattern.end(), touch.begin(), touch.end());
    
    const auto& internal = percept.getInternal();
    pattern.insert(pattern.end(), internal.begin(), internal.end());
    
    const auto& proprioception = percept.getProprioception();
    pattern.insert(pattern.end(), proprioception.begin(), proprioception.end());
    
    return pattern;
}

} // namespace nlm
