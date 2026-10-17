#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

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
    , sensoryNoveltyDecay_(0.99f)
    , currentConceptId_(0)
    , planningConfidence_(0.0f)
    , selfModelConfidence_(0.0f)
    , observationCount_(0)
    , socialLearningWeight_(0.0f)
    , attentionStrength_(1.0f)
    , goalReward_(0.5f)
{
    // Initialize integrated cognition systems
    if (brain_) {
        // Get integrated cognition systems from brain
        planner_ = brain_->getPlanner();
        conceptFormation_ = brain_->getConceptFormation();
        attention_ = brain_->getAttention();
        selfModel_ = brain_->getSelfModel();
        socialLearning_ = brain_->getSocialLearning();
        
        // Set up planning depth and other parameters
        if (planner_) {
            planner_->setPlanningDepth(5);
        }
        
        if (attention_) {
            attention_->setInhibitionStrength(0.5f);
            attention_->setExcitationStrength(1.5f);
        }
        
        if (selfModel_) {
            selfModel_->setCapabilityLevel(0.8f);
        }
        
        if (conceptFormation_) {
            conceptFormation_->setFormationThreshold(0.7f);
            conceptFormation_->setStabilityThreshold(0.8f);
        }
    }
    
    // Initialize motor and sensory neuron groups
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Distribute motor neurons to different action groups
                        size_t idx = motorForward_.size() + motorBackward_.size() + 
                                    motorTurnLeft_.size() + motorTurnRight_.size() +
                                    motorInteract_.size() + motorWait_.size();
                        
                        switch (idx % 6) {
                            case 0: motorForward_.push_back(n); break;
                            case 1: motorBackward_.push_back(n); break;
                            case 2: motorTurnLeft_.push_back(n); break;
                            case 3: motorTurnRight_.push_back(n); break;
                            case 4: motorInteract_.push_back(n); break;
                            case 5: motorWait_.push_back(n); break;
                        }
                    }
                } else if (type == NeuronType::Sensory) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Distribute sensory neurons
                        size_t idx = sensoryVision_.size() + sensoryTouch_.size() +
                                    sensoryInternal_.size() + sensoryProprioception_.size();
                        
                        switch (idx % 4) {
                            case 0: sensoryVision_.push_back(n); break;
                            case 1: sensoryTouch_.push_back(n); break;
                            case 2: sensoryInternal_.push_back(n); break;
                            case 3: sensoryProprioception_.push_back(n); break;
                        }
                    }
                }
            }
        }
    }
}

AgentBrain::~AgentBrain() = default;