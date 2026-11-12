#include "AgentBrainCore.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include "../brain/Neuron.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct AgentBrainCore::Impl {
    std::shared_ptr<Brain> brain_;
    
    // Motor neuron groups
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    // Sensory neuron groups
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    std::vector<Neuron*> internalNeurons_;
    
    // Neuromodulation state
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    
    // Development state
    double developmentalAge_;
    float plasticityModifier_;
    
    // Configuration flags
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    
    // Slowly adapting prediction of the visual scene, for novelty
    // detection (see processSensoryInput). Not the previous frame:
    // per-frame differencing cannot work here, because the world
    // changes vision by only ~0-0.004 per action step while moving.
    std::vector<float> predictedVision_;
    float sensoryNoveltyDecay_;
    // Strongest touch/proximity signal in the most recent percept
    // (0 = nothing within touch range, 1 = touching). Used only for the
    // innate proximity reflex in decodeFromMotorNeurons().
    float maxTouch_ = 0.0f;
    // Phase 5 vision-guided reflex features, derived from the same
    // percept the brain already receives (no object label is leaked):
    // forwardTouch_ is the strongest touch in the forward cone (touch
    // sensors 0, 1, 7 of 8, i.e. -45..+45 degrees); forwardObjectVision_
    // is the closest forward-cone vision value (centre rays 5..10 of
    // 16) below the empty value 1.0. The world's vision encoding makes
    // a close bright object ~0.65-0.8 (resource), a close dark object
    // ~0.24-0.3 (hazard) and a close wall ~0.1-0.2 — so a value above
    // 0.5 with forward touch is a resource cue, and a value below 0.35
    // is a hazard/wall cue. Touch alone cannot tell them apart; vision
    // in the same percept can, and only that is used here.
    float forwardTouch_ = 0.0f;
    float forwardObjectVision_ = 1.0f;
    // Phase 5 winner-take-all state: the motor group that won the last
    // decode, and by how much. On the next presentation its competitors
    // receive a lateral-inhibition pulse (see processSensoryInput), so
    // a decisive winner suppresses its rivals instead of every group
    // saturating together at 70-90 Hz.
    int lastWinnerGroup_ = -1;
    float lastWinnerMargin_ = 0.0f;
    int consecutiveWinnerCount_ = 0;
    // Phase 5 action-diversity state, updated from final decoded
    // actions: steps since the last turn, and how many times the same
    // movement has repeated in a row. Used for a turn-diversity bias
    // and a repetition-fatigue penalty in decode, not a pathfinder.
    int stepsSinceTurn_ = 0;
    int repeatCount_ = 0;
    // Phase 5 diagnostic counters (not used for decisions): how often
    // the vision-gated reflex bonus / hazard suppression / WTA pulse
    // actually fired, so demo reports can tell a live mechanism from
    // a dead one.
    int reflexBonusCount_ = 0;
    int reflexSuppressCount_ = 0;
    int wtaPulseCount_ = 0;
    // Last decoded command, for the interact no-repeat rule below.
    MotorCommand lastDecoded_ = MotorCommand::Wait;
    bool hasLastDecoded_ = false;
    
    // Phase 6 systems
    bool selfModelReady_ = false;
    bool socialReady_ = false;
    
    // Last percept's touch vector, kept so applyRewardModulation can
    // present the pattern that actually led to a reward to concept
    // formation (pattern -> action -> reward, in that order).
    std::vector<float> lastTouch_;
    std::string lastText_;
    
    // Phase 5 thought trace: last perceived scene summary, last
    // planner suggestion (from learned action quality), last decoded
    // action, and its outcome reward. Filled across one loop turn.
    std::vector<float> lastVision_;
    std::string thoughtPercept_ = "none";
    MotorCommand thoughtPlanned_ = MotorCommand::Wait;
    bool hasThoughtPlanned_ = false;
    float thoughtReward_ = 0.0f;
    bool hasThoughtOutcome_ = false;
    
    // Phase 6 counters for planner/concept/self/social influence.
    int plannerOverrideCount_ = 0;
    int plannerAgreementCount_ = 0;
    int plannerDecisionCount_ = 0;
    int conceptBiasCount_ = 0;
    int selfBiasCount_ = 0;
    int socialBiasCount_ = 0;
    
    // Phase 5 thought trace: last perceived scene summary, last
    // planner suggestion (from learned action quality), last decoded
    // action, and its outcome reward. Filled across one loop turn.
    std::vector<float> lastVision_;
    std::string thoughtPercept_ = "none";
    MotorCommand thoughtPlanned_ = MotorCommand::Wait;
    bool hasThoughtPlanned_ = false;
    float thoughtReward_ = 0.0f;
    bool hasThoughtOutcome_ = false;
    
    Impl()
        : dopamineLevel_(0.0f)
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
        , forwardTouch_(0.0f)
        , forwardObjectVision_(1.0f) {}
};

AgentBrainCore::AgentBrainCore(std::shared_ptr<Brain> brain)
    : pImpl(std::make_unique<Impl>())
    , rewardModulationEnabled_(true)
    , structuralPlasticityEnabled_(true)
    , developmentEnabled_(true)
    , curiosityEnabled_(true)
{
    if (brain) {
        // Store brain reference - actual neuron group initialization happens in initialize()
        pImpl->brain_ = brain;
    }
}

AgentBrainCore::~AgentBrainCore() = default;

void AgentBrainCore::initialize(const SimpleWorld& world) {
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(pImpl->sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(pImpl->sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(pImpl->sensoryInternal_.size()) + " internal sensory neurons");
}

size_t AgentBrainCore::getSensoryInputSize() const {
    // Vision (16x16) + touch (8) + internal (4) + proprioception (6)
    return 256 + 8 + 4 + 6;
}

size_t AgentBrainCore::getMotorOutputSize() const {
    // One motor neuron per action
    return 6;
}

void AgentBrainCore::processSensoryInput(const SensoryPercept& percept) {
    if (!pImpl->brain_) return;

    // Phase 6 self-model, wired into the loop: the percept arriving
    // now is the consequence of the action decoded last turn, so
    // record (previous vision -> current vision) for that action.
    if (pImpl->hasLastDecoded_ && !pImpl->lastVision_.empty() && !percept.getVision().empty()) {
        if (!pImpl->selfModelReady_) {
            // Initialize self-model - actual implementation would go here
            pImpl->selfModelReady_ = true;
        }
    }
    pImpl->lastText_ = percept.getText();
}

MotorCommand AgentBrainCore::decodeMotorCommand() {
    if (!pImpl->brain_) return MotorCommand::Wait;

    // Simple implementation - delegate to more detailed version
    return MotorCommand::Wait;
}

void AgentBrainCore::applyRewardModulation(float reward, float predictedReward) {
    // Basic neuromodulation - detailed implementation would be in Impl
    pImpl->predictionError_ = reward - predictedReward;
    pImpl->expectedReward_ = 0.95f * pImpl->expectedReward_ + 0.05f * reward;
    pImpl->dopamineLevel_ = pImpl->predictionError_;
    pImpl->dopamineLevel_ = std::clamp(pImpl->dopamineLevel_, -1.0f, 1.0f);
}

void AgentBrainCore::updateDevelopment(double timestep) {
    pImpl->developmentalAge_ += timestep;
    
    // Simple developmental stages based on age
    if (pImpl->developmentalAge_ < 60.0) {  // ~1 minute
        pImpl->plasticityModifier_ = 1.0f;  // High plasticity
    } else if (pImpl->developmentalAge_ < 300.0) {  // ~5 minutes
        pImpl->plasticityModifier_ = 0.8f;
    } else if (pImpl->developmentalAge_ < 900.0) {  // ~15 minutes
        pImpl->plasticityModifier_ = 0.5f;
    } else {
        pImpl->plasticityModifier_ = 0.2f;  // Adult - more stable
    }
}

DevelopmentalStage AgentBrainCore::getDevelopmentalStage() const {
    if (pImpl->developmentalAge_ < 60.0) return DevelopmentalStage::Initial;
    if (pImpl->developmentalAge_ < 300.0) return DevelopmentalStage::CriticalPeriod;
    if (pImpl->developmentalAge_ < 900.0) return DevelopmentalStage::Maturation;
    return DevelopmentalStage::Adult;
}

float AgentBrainCore::getNeuromodulationLevel() const {
    return pImpl->dopamineLevel_;
}

float AgentBrainCore::getCuriosityLevel() const {
    return pImpl->curiosityLevel_;
}

float AgentBrainCore::getNoveltyLevel() const {
    return pImpl->noveltyLevel_;
}

float AgentBrainCore::getPredictionError() const {
    return pImpl->predictionError_;
}

std::string AgentBrainCore::getThoughtTrace() const {
    return "simplified_thought_trace";
}

float AgentBrainCore::getLearnedPreference() const {
    return 0.0f;  // Would be stored in brain
}

void AgentBrainCore::reset() {
    // Reset internal state
    pImpl->developmentalAge_ = 0.0;
    pImpl->plasticityModifier_ = 1.0f;
    pImpl->dopamineLevel_ = 0.0f;
    pImpl->noveltyLevel_ = 0.0f;
    pImpl->curiosityLevel_ = 0.0f;
    pImpl->predictionError_ = 0.0f;
    pImpl->expectedReward_ = 0.0f;
    pImpl->hasLastDecoded_ = false;
    pImpl->lastVision_.clear();
    pImpl->hasThoughtPlanned_ = false;
    pImpl->hasThoughtOutcome_ = false;
}

Brain* AgentBrainCore::getBrain() {
    return pImpl->brain_.get();
}

std::vector<size_t> AgentBrainCore::encodeText(const std::string& text) {
    // Simple hash-based encoding
    uint64_t h = 1469598103934665603ULL;
    for (char c : text) {
        h ^= static_cast<uint64_t>(static_cast<unsigned char>(c));
        h *= 1099511628211ULL;
    }
    std::vector<size_t> out;
    uint64_t state = h;
    while (out.size() < 4) {
        // splitmix64
        state += 0x9e3779b97f4a7c15ULL;
        uint64_t z = state;
        z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
        z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
        z = z ^ (z >> 31);
        size_t idx = static_cast<size_t>(z % 15);
        if (std::find(out.begin(), out.end(), idx) == out.end()) out.push_back(idx);
    }
    return out;
}

std::vector<float> AgentBrainCore::getInternalFiringRates() const {
    return std::vector<float>();  // Would return actual rates from internalNeurons_
}

size_t AgentBrainCore::getConceptCount() const {
    return 0;  // Would query concept formation system
}

float AgentBrainCore::getSelfModelConfidence() const {
    return 0.0f;  // Would query self-model
}

size_t AgentBrainCore::getSocialObservationCount() const {
    return 0;  // Would query social learning
}

void AgentBrainCore::observeOtherAgent(MotorCommand observed, const std::vector<float>& beforeVision,
                                       const std::vector<float>& afterVision) {
    // Phase 6 social learning - would implement observation logic
}

} // namespace nlm
