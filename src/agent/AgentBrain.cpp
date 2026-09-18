#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Constants.hpp"
#include "AgentBrainConstants.hpp"
#include <algorithm>
#include <cmath>
#include <cassert>
#include <memory>
#include <vector>

namespace nlm {

// AgentBrain constructor implementation
AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(std::move(brain))
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
    , sensoryNoveltyDecay_(DEFAULT_NOVELTY_DECAY)
{
    assert(brain_ && "Brain pointer cannot be null");
    
    // Initialize motor and sensory neuron groups
    initializeMotorSensoryGroups();
}

// AgentBrain destructor
AgentBrain::~AgentBrain() = default;

// AgentBrain initialization
void AgentBrain::initialize(const SimpleWorld& world) {
    assert(!previousVision_.empty() && "Previous vision should be initialized");
    
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
}

// AgentBrain main sensory processing
void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    assert(brain_ && "Brain pointer cannot be null");
    
    // Inject current into all sensory neuron groups
    injectVisionInput(perpct.getVision());
    injectTouchInput(perpct.getTouch());
    injectInternalInput(perpct.getInternal());
    injectProprioceptiveInput(perpct.getProprioception());
    
    // Compute novelty from vision changes
    computeNoveltyLevel(perpct.getVision());
    
    // Update curiosity based on novelty and prediction error
    updateCuriosity();
}

// AgentBrain motor command decoding
MotorCommand AgentBrain::decodeMotorCommand() {
    assert(brain_ && "Brain pointer cannot be null");
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > CURIOSITY_THRESHOLD_FOR_EXPLORATION) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

// AgentBrain reward modulation
void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = REWARD_PREDICTION_WEIGHT * expectedReward_ + REWARD_ERROR_WEIGHT * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, DEFAULT_DOPAMINE_CLAMP_MIN, DEFAULT_DOPAMINE_CLAMP_MAX);
    
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

// AgentBrain development update
void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    // Determine developmental stage
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

// AgentBrain curiosity getter
float AgentBrain::getCuriosityLevel() const {
    return curiosityLevel_;
}

// AgentBrain reset for new episode
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
}

// Helper function to get brain pointer
Brain* AgentBrain::getBrain() {
    return brain_.get();
}

// Configuration helper methods
void AgentBrain::enableRewardModulation(bool enable) {
    rewardModulationEnabled_ = enable;
}

void AgentBrain::enableStructuralPlasticity(bool enable) {
    structuralPlasticityEnabled_ = enable;
}

void AgentBrain::enableDevelopment(bool enable) {
    developmentEnabled_ = enable;
}

void AgentBrain::enableCuriosity(bool enable) {
    curiosityEnabled_ = enable;
}

bool AgentBrain::isRewardModulationEnabled() const {
    return rewardModulationEnabled_;
}

bool AgentBrain::isStructuralPlasticityEnabled() const {
    return structuralPlasticityEnabled_;
}

bool AgentBrain::isDevelopmentEnabled() const {
    return developmentEnabled_;
}

bool AgentBrain::isCuriosityEnabled() const {
    return curiosityEnabled_;
}

// Advanced API: Configure neuromodulation levels
void AgentBrain::setTargetNeuromodulation(float targetDopamine, float targetCuriosity) {
    // This provides a way to directly influence neuromodulation for advanced users
    dopamineLevel_ = std::clamp(targetDopamine, -1.0f, 1.0f);
    curiosityLevel_ = std::clamp(targetCuriosity, 0.0f, 1.0f);
}

// Advanced API: Get comprehensive system state
AgentBrainState AgentBrain::getBrainState() const {
    AgentBrainState state;
    state.dopamine = dopamineLevel_;
    state.novelty = noveltyLevel_;
    state.curiosity = curiosityLevel_;
    state.predictionError = predictionError_;
    state.expectedReward = expectedReward_;
    state.developmentalAge = developmentalAge_;
    state.plasticityModifier = plasticityModifier_;
    state.rewardModulationEnabled = rewardModulationEnabled_;
    state.structuralPlasticityEnabled = structuralPlasticityEnabled_;
    state.developmentEnabled = developmentEnabled_;
    state.curiosityEnabled = curiosityEnabled_;
    state.sensoryVisionCount = static_cast<size_t>(sensoryVision_.size());
    state.sensoryTouchCount = static_cast<size_t>(sensoryTouch_.size());
    state.sensoryInternalCount = static_cast<size_t>(sensoryInternal_.size());
    state.sensoryProprioceptionCount = static_cast<size_t>(sensoryProprioception_.size());
    state.motorForwardCount = static_cast<size_t>(motorForward_.size());
    state.motorBackwardCount = static_cast<size_t>(motorBackward_.size());
    state.motorTurnLeftCount = static_cast<size_t>(motorTurnLeft_.size());
    state.motorTurnRightCount = static_cast<size_t>(motorTurnRight_.size());
    state.motorInteractCount = static_cast<size_t>(motorInteract_.size());
    state.motorWaitCount = static_cast<size_t>(motorWait_.size());
    
    return state;
}

// Advanced API: Performance monitoring
float AgentBrain::getComputationalLoad() const {
    // Estimate computational load based on neuron counts
    size_t totalNeurons = sensoryVision_.size() + sensoryTouch_.size() + 
                          sensoryInternal_.size() + sensoryProprioception_.size() +
                          motorForward_.size() + motorBackward_.size() +
                          motorTurnLeft_.size() + motorTurnRight_.size() +
                          motorInteract_.size() + motorWait_.size();
    
    return static_cast<float>(totalNeurons) / 1000.0f;  // Normalized to 0-1 scale
}

} // namespace nlm