#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iomanip>

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
    // Advanced control parameters
    , maxExploration_(0.5f)
    , temperature_(0.5f)
    , epsilon_(0.1f)
    , developmentRate_(1.0f)
    , criticalPeriodEnd_(300.0)
    , curiositySignal_(0.0f)
    , noveltySignal_(0.0f)
    , predictionErrorSignal_(0.0f)
    , snapshotCounter_(0)
{
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

AgentBrain::AgentBrain(AgentBrain&& other) noexcept
    : brain_(std::move(other.brain_))
    , dopamineLevel_(other.dopamineLevel_)
    , noveltyLevel_(other.noveltyLevel_)
    , curiosityLevel_(other.curiosityLevel_)
    , predictionError_(other.predictionError_)
    , expectedReward_(other.expectedReward_)
    , developmentalAge_(other.developmentalAge_)
    , plasticityModifier_(other.plasticityModifier_)
    , rewardModulationEnabled_(other.rewardModulationEnabled_)
    , structuralPlasticityEnabled_(other.structuralPlasticityEnabled_)
    , developmentEnabled_(other.developmentEnabled_)
    , curiosityEnabled_(other.curiosityEnabled_)
    , sensoryNoveltyDecay_(other.sensoryNoveltyDecay_)
    , maxExploration_(other.maxExploration_)
    , temperature_(other.temperature_)
    , epsilon_(other.epsilon_)
    , developmentRate_(other.developmentRate_)
    , criticalPeriodEnd_(other.criticalPeriodEnd_)
    , curiositySignal_(other.curiositySignal_)
    , noveltySignal_(other.noveltySignal_)
    , predictionErrorSignal_(other.predictionErrorSignal_)
    , snapshotCounter_(other.snapshotCounter_)
    , motorForward_(std::move(other.motorForward_))
    , motorBackward_(std::move(other.motorBackward_))
    , motorTurnLeft_(std::move(other.motorTurnLeft_))
    , motorTurnRight_(std::move(other.motorTurnRight_))
    , motorInteract_(std::move(other.motorInteract_))
    , motorWait_(std::move(other.motorWait_))
    , sensoryVision_(std::move(other.sensoryVision_))
    , sensoryTouch_(std::move(other.sensoryTouch_))
    , sensoryInternal_(std::move(other.sensoryInternal_))
    , sensoryProprioception_(std::move(other.sensoryProprioception_))
    , previousVision_(std::move(other.previousVision_))
{
    // Clear other's state
    other.brain_.reset();
    other.dopamineLevel_ = 0.0f;
    other.noveltyLevel_ = 0.0f;
    other.curiosityLevel_ = 0.0f;
    other.predictionError_ = 0.0f;
    other.expectedReward_ = 0.0f;
    other.developmentalAge_ = 0.0;
    other.plasticityModifier_ = 1.0f;
    other.rewardModulationEnabled_ = true;
    other.structuralPlasticityEnabled_ = true;
    other.developmentEnabled_ = true;
    other.curiosityEnabled_ = true;
    other.sensoryNoveltyDecay_ = 0.99f;
    other.maxExploration_ = 0.5f;
    other.temperature_ = 0.5f;
    other.epsilon_ = 0.1f;
    other.developmentRate_ = 1.0f;
    other.criticalPeriodEnd_ = 300.0;
    other.curiositySignal_ = 0.0f;
    other.noveltySignal_ = 0.0f;
    other.predictionErrorSignal_ = 0.0f;
    other.snapshotCounter_ = 0;
}

AgentBrain& AgentBrain::operator=(AgentBrain&& other) noexcept {
    if (this != &other) {
        // Clean up current resources
        // (brain_ is shared_ptr, so no need to explicitly delete)
        // Clear vectors
        motorForward_.clear();
        motorBackward_.clear();
        motorTurnLeft_.clear();
        motorTurnRight_.clear();
        motorInteract_.clear();
        motorWait_.clear();
        sensoryVision_.clear();
        sensoryTouch_.clear();
        sensoryInternal_.clear();
        sensoryProprioception_.clear();
        previousVision_.clear();
        
        // Move resources from other
        brain_ = std::move(other.brain_);
        dopamineLevel_ = other.dopamineLevel_;
        noveltyLevel_ = other.noveltyLevel_;
        curiosityLevel_ = other.curiosityLevel_;
        predictionError_ = other.predictionError_;
        expectedReward_ = other.expectedReward_;
        developmentalAge_ = other.developmentalAge_;
        plasticityModifier_ = other.plasticityModifier_;
        rewardModulationEnabled_ = other.rewardModulationEnabled_;
        structuralPlasticityEnabled_ = other.structuralPlasticityEnabled_;
        developmentEnabled_ = other.developmentEnabled_;
        curiosityEnabled_ = other.curiosityEnabled_;
        sensoryNoveltyDecay_ = other.sensoryNoveltyDecay_;
        maxExploration_ = other.maxExploration_;
        temperature_ = other.temperature_;
        epsilon_ = other.epsilon_;
        developmentRate_ = other.developmentRate_;
        criticalPeriodEnd_ = other.criticalPeriodEnd_;
        curiositySignal_ = other.curiositySignal_;
        noveltySignal_ = other.noveltySignal_;
        predictionErrorSignal_ = other.predictionErrorSignal_;
        snapshotCounter_ = other.snapshotCounter_;
        
        // Set other's state to invalid
        other.brain_.reset();
        other.dopamineLevel_ = 0.0f;
        other.noveltyLevel_ = 0.0f;
        other.curiosityLevel_ = 0.0f;
        other.predictionError_ = 0.0f;
        other.expectedReward_ = 0.0f;
        other.developmentalAge_ = 0.0;
        other.plasticityModifier_ = 1.0f;
        other.rewardModulationEnabled_ = true;
        other.structuralPlasticityEnabled_ = true;
        other.developmentEnabled_ = true;
        other.curiosityEnabled_ = true;
        other.sensoryNoveltyDecay_ = 0.99f;
        other.maxExploration_ = 0.5f;
        other.temperature_ = 0.5f;
        other.epsilon_ = 0.1f;
        other.developmentRate_ = 1.0f;
        other.criticalPeriodEnd_ = 300.0;
        other.curiositySignal_ = 0.0f;
        other.noveltySignal_ = 0.0f;
        other.predictionErrorSignal_ = 0.0f;
        other.snapshotCounter_ = 0;
    }
    return *this;
}

AgentBrain::~AgentBrain() = default;