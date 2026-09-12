#include "AgentBrain.hpp"
#include "SensoryProcessor.hpp"
#include "MotorDecoder.hpp"
#include "NeuromodulationController.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(brain)
    , previousVision_()
    , sensoryNoveltyDecay_(0.99f)
    , sensoryProcessor_(std::make_shared<SensoryProcessor>(brain))
    , motorDecoder_(std::make_shared<MotorDecoder>(brain))
    , rewardModulationController_(std::make_shared<NeuromodulationController>(brain))
{
    // Create motor and sensory neuron groups for backward compatibility
    if (brain_) {
        std::vector<Neuron*> allMotorNeurons;
        std::vector<Neuron*> allSensoryNeurons;
        
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
                        allMotorNeurons.push_back(n);
                    }
                } else if (type == NeuronType::Sensory) {
                    for (Neuron* n : pop->getNeurons()) {
                        allSensoryNeurons.push_back(n);
                    }
                }
            }
        }
        
        // Use centralized neuron distribution
        NeuronDistributor::distributeMotorNeurons(motorForward_, motorBackward_, motorTurnLeft_, 
                                                motorTurnRight_, motorInteract_, motorWait_, 
                                                allMotorNeurons);
        NeuronDistributor::distributeSensoryNeurons(sensoryVision_, sensoryTouch_, 
                                                  sensoryInternal_, sensoryProprioception_,
                                                  allSensoryNeurons);
        
        // Initialize sensor processor with neuron groups
        sensoryProcessor_->setMotorNeurons(motorForward_, motorBackward_, motorTurnLeft_, 
                                          motorTurnRight_, motorInteract_, motorWait_);
        sensoryProcessor_->setSensoryNeurons(sensoryVision_, sensoryTouch_, 
                                             sensoryInternal_, sensoryProprioception_);
    }
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    sensoryProcessor_->reset();
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
}

size_t AgentBrain::getSensoryInputSize() const {
    return sensoryProcessor_->getSensoryInputSize();
}

size_t AgentBrain::getMotorOutputSize() const {
    return motorDecoder_->getMotorOutputSize();
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    sensoryProcessor_->process(percept);
    
    // Backward compatibility: update novelty directly for external access
    noveltyLevel_ = sensoryProcessor_->getNoveltyLevel();
    curiosityLevel_ = sensoryProcessor_->getCuriosityLevel();
    
    // Also update neuromodulation controller with curiosity
    rewardModulationController_->updateCuriosityLevel(curiosityLevel_);
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Calculate average activity in each motor group using MotorDecoder
    float forwardAct = motorDecoder_->calculateActivity(motorForward_);
    float backwardAct = motorDecoder_->calculateActivity(motorBackward_);
    float leftAct = motorDecoder_->calculateActivity(motorTurnLeft_);
    float rightAct = motorDecoder_->calculateActivity(motorTurnRight_);
    float interactAct = motorDecoder_->calculateActivity(motorInteract_);
    float waitAct = motorDecoder_->calculateActivity(motorWait_);
    
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

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Delegate to MotorDecoder with backward compatibility state
    return motorDecoder_->selectWithCuriosity(defaultCmd, curiosityLevel_, curiosityEnabled_);
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    rewardModulationController_->applyRewardModulation(reward, predictedReward);
    
    // Backward compatibility: update direct state
    dopamineLevel_ = rewardModulationController_->getNeuromodulationLevel();
    predictionError_ = rewardModulationController_->getPredictionError();
    expectedReward_ = rewardModulationController_->getExpectedReward();
    plasticityModifier_ = rewardModulationController_->getPlasticityModifier();
}

void AgentBrain::updateDevelopment(double timestep) {
    rewardModulationController_->updateDevelopment(timestep);
    
    // Backward compatibility: update direct state
    developmentalAge_ = rewardModulationController_->getDevelopmentalAge();
    plasticityModifier_ = rewardModulationController_->getPlasticityModifier();
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    if (!brain_) return DevelopmentalStage::Initial;
    return brain_->getDevelopmentalStage();
}

float AgentBrain::getNeuromodulationLevel() const {
    return rewardModulationController_->getNeuromodulationLevel();
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
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
    
    // Reset components
    sensoryProcessor_->reset();
    rewardModulationController_->reset();
}

} // namespace nlm
