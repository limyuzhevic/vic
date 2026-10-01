#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(brain)
    , groupManager_(createDefaultNeuronGroupFactory())
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
{
    // Initialize with brain and neuron group manager
    if (brain_) {
        groupManager_->initialize(brain_);
    }
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    // Initialize development state
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Log group manager status
    if (groupManager_) {
        NLM_LOG_INFO("AgentBrain initialized with " +
                     std::to_string(groupManager_->getTotalNeuronCount()) + " total neurons, " +
                     std::to_string(groupManager_->getGroupCount()) + " neuron groups");
    }
}

size_t AgentBrain::getSensoryInputSize() const {
    // Vision (16x16) + touch (8) + internal (4) + proprioception (6)
    return 256 + 8 + 4 + 6;
}

size_t AgentBrain::getMotorOutputSize() const {
    // One motor neuron per action
    return 6;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_ || !groupManager_) return;
    
    // Process vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    
    NeuronGroup* visionGroup = groupManager_->getSensoryGroup(SensoryInputGroup::Vision);
    if (visionGroup) {
        const auto& visionNeurons = visionGroup->getNeurons();
        for (size_t i = 0; i < visionNeurons.size() && i < vision.size(); ++i) {
            if (visionNeurons[i]) {
                // Inject current proportional to vision intensity
                float current = vision[i] * 5.0f;  // Scale factor
                visionNeurons[i]->injectCurrent(current);
            }
        }
    }
    
    // Process touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    
    NeuronGroup* touchGroup = groupManager_->getSensoryGroup(SensoryInputGroup::Touch);
    if (touchGroup) {
        const auto& touchNeurons = touchGroup->getNeurons();
        for (size_t i = 0; i < touchNeurons.size() && i < touch.size(); ++i) {
            if (touchNeurons[i]) {
                float current = touch[i] * 8.0f;  // Collision signal
                touchNeurons[i]->injectCurrent(current);
            }
        }
    }
    
    // Process internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    
    NeuronGroup* internalGroup = groupManager_->getSensoryGroup(SensoryInputGroup::Internal);
    if (internalGroup) {
        const auto& internalNeurons = internalGroup->getNeurons();
        for (size_t i = 0; i < internalNeurons.size() && i < intern.size(); ++i) {
            if (internalNeurons[i]) {
                float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
                internalNeurons[i]->injectCurrent(current);
            }
        }
    }
    
    // Process proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    
    NeuronGroup* proprioceptionGroup = groupManager_->getSensoryGroup(SensoryInputGroup::Proprioception);
    if (proprioceptionGroup) {
        const auto& proprioceptionNeurons = proprioceptionGroup->getNeurons();
        for (size_t i = 0; i < proprioceptionNeurons.size() && i < proprio.size(); ++i) {
            if (proprioceptionNeurons[i]) {
                float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
                proprioceptionNeurons[i]->injectCurrent(current);
            }
        }
    }
    
    // Compute novelty (difference from previous vision)
    if (!vision.empty()) {
        float totalDiff = 0.0f;
        for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        // Normalize
        noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
        
        // Decay and update
        noveltyLevel_ *= sensoryNoveltyDecay_;
        
        // Store for next time
        previousVision_ = vision;
    }
    
    // Update curiosity based on novelty
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
    
    // Update all groups
    if (groupManager_) {
        groupManager_->update(0.01); // Assuming timestep for now
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_ || !groupManager_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    if (!groupManager_) return MotorCommand::Wait;
    
    // Calculate average activity in each motor group
    auto calcActivity = [](const NeuronGroup* group) -> float {
        if (!group || group->empty()) return 0.0f;
        float sum = 0.0f;
        const auto& neurons = group->getNeurons();
        for (Neuron* neuron : neurons) {
            // Use membrane potential deviation from rest as activity measure
            const auto& state = neuron->getState();
            sum += std::abs(state.membranePotential - state.restingPotential);
        }
        return sum / neurons.size();
    };
    
    float forwardAct = 0.0f, backwardAct = 0.0f, leftAct = 0.0f, rightAct = 0.0f, 
         interactAct = 0.0f, waitAct = 0.0f;
    
    // Get activity levels for each motor group
    NeuronGroup* forwardGroup = groupManager_->getMotorGroup(MotorActionGroup::Forward);
    if (forwardGroup) forwardAct = calcActivity(forwardGroup);
    
    NeuronGroup* backwardGroup = groupManager_->getMotorGroup(MotorActionGroup::Backward);
    if (backwardGroup) backwardAct = calcActivity(backwardGroup);
    
    NeuronGroup* leftGroup = groupManager_->getMotorGroup(MotorActionGroup::TurnLeft);
    if (leftGroup) leftAct = calcActivity(leftGroup);
    
    NeuronGroup* rightGroup = groupManager_->getMotorGroup(MotorActionGroup::TurnRight);
    if (rightGroup) rightAct = calcActivity(rightGroup);
    
    NeuronGroup* interactGroup = groupManager_->getMotorGroup(MotorActionGroup::Interact);
    if (interactGroup) interactAct = calcActivity(interactGroup);
    
    NeuronGroup* waitGroup = groupManager_->getMotorGroup(MotorActionGroup::Wait);
    if (waitGroup) waitAct = calcActivity(waitGroup);
    
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
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > 0.5f) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * 0.3f;  // Up to 30% random
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command
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

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
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
    
    // Update group manager
    if (groupManager_) {
        groupManager_->update(timestep);
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
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
    
    // Reset group manager
    if (groupManager_) {
        groupManager_->reset();
    }
}

} // namespace nlm
