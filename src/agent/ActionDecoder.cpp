#include "ActionDecoder.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

ActionDecoder::ActionDecoder(std::shared_ptr<Brain> brain) : brain_(std::move(brain)) {
    if (brain_) {
        // Initialize motor neuron groups (same logic as AgentBrain)
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
                }
            }
        }
    }
    
    // Initialize random number generator
    std::random_device rd;
    rng_ = std::mt19937(rd());
}

ActionDecoder::ActionDecoder(ActionDecoder&& other) noexcept
    : brain_(std::move(other.brain_))
    , motorForward_(std::move(other.motorForward_))
    , motorBackward_(std::move(other.motorBackward_))
    , motorTurnLeft_(std::move(other.motorTurnLeft_))
    , motorTurnRight_(std::move(other.motorTurnRight_))
    , motorInteract_(std::move(other.motorInteract_))
    , motorWait_(std::move(other.motorWait_))
    , motorActivity_(std::move(other.motorActivity_))
    , activityThreshold_(other.activityThreshold_)
    , randomExplorationEnabled_(other.randomExplorationEnabled_)
    , curiosityEnabled_(other.curiosityEnabled_)
    , rng_(std::move(other.rng_)) {
}

ActionDecoder& ActionDecoder::operator=(ActionDecoder&& other) noexcept {
    if (this != &other) {
        brain_ = std::move(other.brain_);
        motorForward_ = std::move(other.motorForward_);
        motorBackward_ = std::move(other.motorBackward_);
        motorTurnLeft_ = std::move(other.motorTurnLeft_);
        motorTurnRight_ = std::move(other.motorTurnRight_);
        motorInteract_ = std::move(other.motorInteract_);
        motorWait_ = std::move(other.motorWait_);
        motorActivity_ = std::move(other.motorActivity_);
        activityThreshold_ = other.activityThreshold_;
        randomExplorationEnabled_ = other.randomExplorationEnabled_;
        curiosityEnabled_ = other.curiosityEnabled_;
        rng_ = std::move(other.rng_);
    }
    return *this;
}

MotorCommand ActionDecoder::decodeMotorCommand() {
    // Calculate average activity in each motor group
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            // Use membrane potential deviation from rest as activity measure
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
    if (bestActivity < activityThreshold_) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand ActionDecoder::selectWithCuriosity(MotorCommand defaultCmd, float curiosityLevel) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityEnabled_ && curiosityLevel > 0.5f) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel * 0.3f;  // Up to 30% random
        
        float r = std::uniform_real_distribution<float>(0.0f, 1.0f)(rng_);
        if (r < exploreChance) {
            // Random motor command
            int choice = std::uniform_int_distribution<int>(0, 7)(rng_);
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

// NeuromodulationManager implementation
NeuromodulationManager::NeuromodulationManager(std::shared_ptr<Brain> brain) : brain_(std::move(brain)) {
}

NeuromodulationManager::NeuromodulationManager(NeuromodulationManager&& other) noexcept
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
    , noveltyDecayFactor_(other.noveltyDecayFactor_) {
}

NeuromodulationManager& NeuromodulationManager::operator=(NeuromodulationManager&& other) noexcept {
    if (this != &other) {
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
        noveltyDecayFactor_ = other.noveltyDecayFactor_;
    }
    return *this;
}

void NeuromodulationManager::applyRewardModulation(float reward, float predictedReward) {
    if (!rewardModulationEnabled_ || !brain_) return;
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Apply to all synapses with eligibility traces
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
    
    // Modulate plasticity based on dopamine
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = 0.5f + 0.5f * dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    // Apply to STDP
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(0.01f * plasticityFactor);
        stdp->setLTDWeight(0.012f * plasticityFactor);
    }
}

void NeuromodulationManager::updateDevelopment(double timestep) {
    if (!developmentEnabled_ || !brain_) return;
    
    developmentalAge_ += timestep;
    
    // Simple developmental stages based on age
    // This is a biologically inspired approximation
    if (developmentalAge_ < 60.0) {  // ~1 minute
        plasticityModifier_ = 1.0f;  // High plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < 300.0) {  // ~5 minutes
        plasticityModifier_ = 0.8f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < 900.0) {  // ~15 minutes
        plasticityModifier_ = 0.5f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = 0.2f;  // Adult - more stable
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    // Structural plasticity changes with development
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            // Higher synaptogenesis in early development
            float synRate = 0.0001f * plasticityModifier_;
            float pruneRate = 0.00001f * (2.0f - plasticityModifier_);
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
        }
    }
}

// AgentState implementation
AgentState::AgentState() {
}

AgentState::AgentState(AgentState&& other) noexcept
    : sensoryNoveltyDecay_(other.sensoryNoveltyDecay_)
    , curiosityEnabled_(other.curiosityEnabled_)
    , developmentEnabled_(other.developmentEnabled_)
    , structuralPlasticityEnabled_(other.structuralPlasticityEnabled_)
    , rewardModulationEnabled_(other.rewardModulationEnabled_)
    , developmentalAge_(other.developmentalAge_)
    , plasticityModifier_(other.plasticityModifier_) {
}

AgentState& AgentState::operator=(AgentState&& other) noexcept {
    if (this != &other) {
        sensoryNoveltyDecay_ = other.sensoryNoveltyDecay_;
        curiosityEnabled_ = other.curiosityEnabled_;
        developmentEnabled_ = other.developmentEnabled_;
        structuralPlasticityEnabled_ = other.structuralPlasticityEnabled_;
        rewardModulationEnabled_ = other.rewardModulationEnabled_;
        developmentalAge_ = other.developmentalAge_;
        plasticityModifier_ = other.plasticityModifier_;
    }
    return *this;
}

} // namespace nlm