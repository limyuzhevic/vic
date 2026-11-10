// AgentBrain.cpp - Refactored implementation

#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <cassert>
#include <unordered_map>

namespace nlm {

//===================================
// Helper Classes
//===================================

class NeuronGroupManager {
public:
    virtual ~NeuronGroupManager() = default;
    virtual void addNeuron(NeuronType type, Neuron* neuron) = 0;
    virtual size_t getTotalNeurons() const = 0;
};

class MotorGroupManager : public NeuronGroupManager {
public:
    MotorGroupManager()
        : motorForward_(), motorBackward_(), motorTurnLeft_(), 
          motorTurnRight_(), motorInteract_(), motorWait_() {}
    
    void addNeuron(NeuronType type, Neuron* neuron) override {
        if (!neuron) {
            throw std::invalid_argument("Neuron pointer is null");
        }
        assignMotorNeuron(neuron);
    }
    
    const std::vector<Neuron*>& getMotorForward() const { return motorForward_; }
    const std::vector<Neuron*>& getMotorBackward() const { return motorBackward_; }
    const std::vector<Neuron*>& getMotorTurnLeft() const { return motorTurnLeft_; }
    const std::vector<Neuron*>& getMotorTurnRight() const { return motorTurnRight_; }
    const std::vector<Neuron*>& getMotorInteract() const { return motorInteract_; }
    const std::vector<Neuron*>& getMotorWait() const { return motorWait_; }
    
    size_t getTotalNeurons() const override {
        return motorForward_.size() + motorBackward_.size() + 
               motorTurnLeft_.size() + motorTurnRight_.size() +
               motorInteract_.size() + motorWait_.size();
    }
    
private:
    void assignMotorNeuron(Neuron* neuron) {
        // Distribute motor neurons using round-robin strategy
        static size_t distributionCounter = 0;
        size_t idx = distributionCounter % 6;
        distributionCounter++;
        
        switch (idx) {
            case 0: motorForward_.push_back(neuron); break;
            case 1: motorBackward_.push_back(neuron); break;
            case 2: motorTurnLeft_.push_back(neuron); break;
            case 3: motorTurnRight_.push_back(neuron); break;
            case 4: motorInteract_.push_back(neuron); break;
            case 5: motorWait_.push_back(neuron); break;
        }
    }
    
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
};

class SensoryGroupManager : public NeuronGroupManager {
public:
    SensoryGroupManager()
        : sensoryVision_(), sensoryTouch_(), 
          sensoryInternal_(), sensoryProprioception_() {}
    
    void addNeuron(NeuronType type, Neuron* neuron) override {
        if (!neuron) {
            throw std::invalid_argument("Neuron pointer is null");
        }
        assignSensoryNeuron(neuron);
    }
    
    const std::vector<Neuron*>& getSensoryVision() const { return sensoryVision_; }
    const std::vector<Neuron*>& getSensoryTouch() const { return sensoryTouch_; }
    const std::vector<Neuron*>& getSensoryInternal() const { return sensoryInternal_; }
    const std::vector<Neuron*>& getSensoryProprioception() const { return sensoryProprioception_; }
    
    size_t getTotalNeurons() const override {
        return sensoryVision_.size() + sensoryTouch_.size() +
               sensoryInternal_.size() + sensoryProprioception_.size();
    }
    
private:
    void assignSensoryNeuron(Neuron* neuron) {
        // Distribute sensory neurons using specialized strategy
        static size_t distributionCounter = 0;
        size_t idx = distributionCounter % 4;
        distributionCounter++;
        
        switch (idx) {
            case 0: sensoryVision_.push_back(neuron); break;
            case 1: sensoryTouch_.push_back(neuron); break;
            case 2: sensoryInternal_.push_back(neuron); break;
            case 3: sensoryProprioception_.push_back(neuron); break;
        }
    }
    
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
};

class TextEncoder {
public:
    // Phase 6 text encoder: a teacher word becomes a sparse sensory
    // pattern, never a string the brain can match on. Lower-case, trim,
    // FNV-1a hash, then splitmix to pick 4 distinct neurons out of 16
    // dedicated text neurons. Different words get different (overlapping,
    // as in real sensory codes) patterns; the same word always gets the
    // same pattern, which is what makes paired learning possible at all.
    // The pattern is injected on sensoryVision_ neurons 16..30 — the
    // tail vision neurons that vision itself never uses (vision is 16
    // rays, the vision group has 31 neurons), so text has its own
    // channel and cannot masquerade as a visual scene.
    static std::vector<size_t> encodeText(const std::string& text) {
        if (text.empty()) return {};
        
        // Normalize text
        std::string norm;
        for (char c : text) {
            if (c == ' ' || c == '\t' || c == '\n') continue;
            norm += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        }
        
        if (norm.empty()) return {};
        
        // FNV-1a hash
        uint64_t h = 1469598103934665603ULL;
        for (char c : norm) {
            h ^= static_cast<uint64_t>(static_cast<unsigned char>(c));
            h *= 1099511628211ULL;
        }
        
        // Splitmix64 to get 4 distinct indices
        std::vector<size_t> out;
        out.reserve(4); // Pre-allocate for performance
        uint64_t state = h;
        
        while (out.size() < 4) {
            // splitmix64
            state += 0x9e3779b97f4a7c15ULL;
            uint64_t z = state;
            z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
            z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
            z = z ^ (z >> 31);
            size_t idx = static_cast<size_t>(z % 15);
            
            // Ensure uniqueness
            if (std::find(out.begin(), out.end(), idx) == out.end()) {
                out.push_back(idx);
            }
        }
        
        return out;
    }
};

//===================================
// AgentBrain Implementation
//===================================

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
    , sensoryNoveltyDecay_(0.99f)
    , motorGroupManager_(std::make_unique<MotorGroupManager>())
    , sensoryGroupManager_(std::make_unique<SensoryGroupManager>()) 
    , selfModelReady_(false)
    , socialReady_(false)
    , hasLastDecoded_(false)
    , lastWinnerGroup_(-1)
    , consecutiveWinnerCount_(0)
    , stepsSinceTurn_(0)
    , repeatCount_(0)
    , reflexBonusCount_(0)
    , reflexSuppressCount_(0)
    , wtaPulseCount_(0)
    , lastDecoded_(MotorCommand::Wait)
    , thoughtPlanned_(MotorCommand::Wait)
    , hasThoughtPlanned_(false)
    , hasThoughtOutcome_(false)
{
    if (!brain_) {
        throw std::invalid_argument("Brain pointer is null");
    }
    
    // Initialize neuron groups from brain regions
    initializeNeuronGroups();
}

AgentBrain::~AgentBrain() = default;

AgentBrain::AgentBrain(AgentBrain&& other) noexcept
    : brain_(std::move(other.brain_))
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
    , internalNeurons_(std::move(other.internalNeurons_))
    , dopamineLevel_(other.dopamineLevel_)
    , noveltyLevel_(other.noveltLevel_)
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
    , predictedVision_(std::move(other.predictedVision_))
    , maxTouch_(other.maxTouch_)
    , forwardTouch_(other.forwardTouch_)
    , forwardObjectVision_(other.forwardObjectVision_)
    , lastWinnerGroup_(other.lastWinnerGroup_)
    , lastWinnerMargin_(other.lastWinnerMargin_)
    , consecutiveWinnerCount_(other.consecutiveWinnerCount_)
    , stepsSinceTurn_(other.stepsSinceTurn_)
    , repeatCount_(other.repeatCount_)
    , reflexBonusCount_(other.reflexBonusCount_)
    , reflexSuppressCount_(other.reflexSuppressCount_)
    , wtaPulseCount_(other.wtaPulseCount_)
    , lastDecoded_(other.lastDecoded_)
    , hasLastDecoded_(other.hasLastDecoded_)
    , lastVision_(std::move(other.lastVision_))
    , lastText_(std::move(other.lastText_))
    , selfModel_(std::move(other.selfModel_))
    , socialLearning_(std::move(other.socialLearning_))
    , selfModelReady_(other.selfModelReady_)
    , socialReady_(other.socialReady_)
    , lastTouch_(std::move(other.lastTouch_))
    , thoughtPercept_(std::move(other.thoughtPercept_))
    , thoughtPlanned_(other.thoughtPlanned_)
    , hasThoughtPlanned_(other.hasThoughtPlanned_)
    , hasThoughtOutcome_(other.hasThoughtOutcome_)
{
    // Move assignment for member managers
    motorGroupManager_ = std::move(other.motorGroupManager_);
    sensoryGroupManager_ = std::move(other.sensoryGroupManager_);
}

AgentBrain& AgentBrain::operator=(AgentBrain&& other) noexcept {
    if (this != &other) {
        // Self-assignment check
        brain_ = std::move(other.brain_);
        
        // Move all members
        motorForward_ = std::move(other.motorForward_);
        motorBackward_ = std::move(other.motorBackward_);
        motorTurnLeft_ = std::move(other.motorTurnLeft_);
        motorTurnRight_ = std::move(other.motorTurnRight_);
        motorInteract_ = std::move(other.motorInteract_);
        motorWait_ = std::move(other.motorWait_);
        
        sensoryVision_ = std::move(other.sensoryVision_);
        sensoryTouch_ = std::move(other.sensoryTouch_);
        sensoryInternal_ = std::move(other.sensoryInternal_);
        sensoryProprioception_ = std::move(other.sensoryProprioception_);
        
        internalNeurons_ = std::move(other.internalNeurons_);
        
        // Copy scalar values
        dopamineLevel_ = other.dopamineLevel_;
        noveltyLevel_ = other.noveltLevel_;
        curiosityLevel_ = other.curiosityLevel_;
        predictionError_ = other.predictionError_;
        expectedReward_ = other.expectedReward_;
        developmentalAge_ = other.developmentalAge_;
        plasticityModifier_ = other.plasticityModifier_;
        
        // Copy flags
        rewardModulationEnabled_ = other.rewardModulationEnabled_;
        structuralPlasticityEnabled_ = other.structuralPlasticityEnabled_;
        developmentEnabled_ = other.developmentEnabled_;
        curiosityEnabled_ = other.curiosityEnabled_;
        sensoryNoveltyDecay_ = other.sensoryNoveltyDecay_;
        
        // Move complex objects
        predictedVision_ = std::move(other.predictedVision_);
        maxTouch_ = other.maxTouch_;
        forwardTouch_ = other.forwardTouch_;
        forwardObjectVision_ = other.forwardObjectVision_;
        
        // Copy simple state variables
        lastWinnerGroup_ = other.lastWinnerGroup_;
        lastWinnerMargin_ = other.lastWinnerMargin_;
        consecutiveWinnerCount_ = other.consecutiveWinnerCount_;
        stepsSinceTurn_ = other.stepsSinceTurn_;
        repeatCount_ = other.repeatCount_;
        reflexBonusCount_ = other.reflexBonusCount_;
        reflexSuppressCount_ = other.reflexSuppressCount_;
        wtaPulseCount_ = other.wtaPulseCount_;
        lastDecoded_ = other.lastDecoded_;
        hasLastDecoded_ = other.hasLastDecoded_;
        
        lastVision_ = std::move(other.lastVision_);
        lastText_ = std::move(other.lastText_);
        selfModel_ = std::move(other.selfModel_);
        socialLearning_ = std::move(other.socialLearning_);
        selfModelReady_ = other.selfModelReady_;
        socialReady_ = other.socialReady_;
        
        lastTouch_ = std::move(other.lastTouch_);
        thoughtPercept_ = std::move(other.thoughtPercept_);
        thoughtPlanned_ = other.thoughtPlanned_;
        hasThoughtPlanned_ = other.hasThoughtPlanned_;
        hasThoughtOutcome_ = other.hasThoughtOutcome_;
        
        // Move managers
        motorGroupManager_ = std::move(other.motorGroupManager_);
        sensoryGroupManager_ = std::move(other.sensoryGroupManager_);
    }
    return *this;
}

void AgentBrain::initialize(const SimpleWorld& world) {
    // Validate input
    if (!brain_) {
        NLM_LOG_ERROR("Cannot initialize: brain is null");
        return;
    }
    
    // Reset agent state
    reset();
    
    // Initialize with world
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(getTotalMotorNeurons()) + " motor neurons, " +
                 std::to_string(getTotalSensoryNeurons()) + " sensory neurons");
}

[[nodiscard]] size_t AgentBrain::getSensoryInputSize() const noexcept {
    // Vision (16x16) + touch (8) + internal (4) + proprioception (6)
    return 256 + 8 + 4 + 6;
}

[[nodiscard]] size_t AgentBrain::getMotorOutputSize() const noexcept {
    // One motor neuron per action
    return 6;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) {
        NLM_LOG_WARN("Cannot process sensory input: brain is null");
        return;
    }
    
    // Validate percept
    if (percept.getVision().empty() && percept.getTouch().empty() && 
        percept.getText().empty()) {
        NLM_LOG_DEBUG("Empty percept received");
        return;
    }
    
    lastText_ = percept.getText();
    
    // Phase 6 self-model: record action-consequence pairs for self-model
    if (hasLastDecoded_ && !lastVision_.empty() && !percept.getVision().empty()) {
        if (!selfModelReady_) {
            selfModel_.initialize(brain_.get());
            selfModelReady_ = true;
        }
        ActionType act = ActionType::Wait;
        switch (lastDecoded_) {
            case MotorCommand::MoveForward: act = ActionType::MoveForward; break;
            case MotorCommand::MoveBackward: act = ActionType::MoveBackward; break;
            case MotorCommand::TurnLeft: act = ActionType::TurnLeft; break;
            case MotorCommand::TurnRight: act = ActionType::TurnRight; break;
            case MotorCommand::Interact: act = ActionType::Interact; break;
            default: break;
        }
        selfModel_.recordSelfAction(act, lastVision_, percept.getVision());
    }
    
    // Phase 5 winner-take-all lateral inhibition
    applyWinnerTakeAllInhibition();
    
    // Process vision input
    processVisionInput(percept.getVision());
    
    // Process text input (Phase 6)
    processTextInput(percept.getText());
    
    // Process touch input
    processTouchInput(percept.getTouch());
    
    // Update novelty detection
    updateNoveltyDetection();
    
    // Record last percept for next iteration
    lastVision_ = percept.getVision();
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) {
        NLM_LOG_ERROR("Cannot decode motor command: brain is null");
        return MotorCommand::Wait;
    }
    
    // Get brain action
    std::unique_ptr<Action> action = brain_->produceAction();
    if (!action) {
        return MotorCommand::Wait;
    }
    
    // Convert action to motor command
    MotorCommand cmd = action->getCommand();
    
    // Update state
    lastDecoded_ = cmd;
    hasLastDecoded_ = true;
    
    return cmd;
}

std::vector<AgentBrain::MotorGroupActivity> AgentBrain::getMotorGroupActivities() const noexcept {
    std::vector<MotorGroupActivity> activities;
    
    // Create activity structure for each motor group
    MotorGroupActivity forwardAct = {"Forward", 0.0f, 0, 0, 0.0f};
    MotorGroupActivity backwardAct = {"Backward", 0.0f, 0, 0, 0.0f};
    MotorGroupActivity leftAct = {"TurnLeft", 0.0f, 0, 0, 0.0f};
    MotorGroupActivity rightAct = {"TurnRight", 0.0f, 0, 0, 0.0f};
    MotorGroupActivity interactAct = {"Interact", 0.0f, 0, 0, 0.0f};
    MotorGroupActivity waitAct = {"Wait", 0.0f, 0, 0, 0.0f};
    
    // Calculate activities for each group
    // (Implementation would depend on specific activity calculation)
    
    activities = {forwardAct, backwardAct, leftAct, rightAct, interactAct, waitAct};
    return activities;
}

void AgentBrain::applyWinnerTakeAllInhibition() {
    // Phase 5 winner-take-all lateral inhibition: if the last decode
    // had a decisive winner (margin > 5 Hz) and the scene is stable
    // (low novelty), suppress the losing motor groups with an
    // inhibitory pulse on this presentation.
    if (lastWinnerGroup_ >= 0 && lastWinnerGroup_ != 4 &&
        consecutiveWinnerCount_ < 3 &&
        lastWinnerMargin_ > 5.0f && noveltyLevel_ < 0.3f) {
        
        const std::vector<Neuron*>* groups[5] = {
            &motorForward_, &motorBackward_, &motorTurnLeft_,
            &motorTurnRight_, &motorInteract_};
        
        for (int g = 0; g < 5; ++g) {
            if (g == lastWinnerGroup_) continue;
            for (Neuron* n : *groups[g]) {
                if (n) n->injectCurrent(-4.0f);
            }
        }
        
        ++wtaPulseCount_;
    }
}

void AgentBrain::processVisionInput(const std::vector<float>& vision) {
    // Vision input (16 rays in SimpleWorld -> sensoryVision_ neurons).
    const auto& visionNeurons = motorGroupManager_->getMotorForward();
    
    for (size_t i = 0; i < visionNeurons.size() && i < vision.size(); ++i) {
        if (visionNeurons[i]) {
            // Inject current proportional to vision intensity
            float current = vision[i] * 5.0f;  // Scale factor
            visionNeurons[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::processTextInput(const std::string& text) {
    if (!text.empty()) {
        for (size_t idx : TextEncoder::encodeText(text)) {
            size_t neuronIdx = 16 + idx;
            if (neuronIdx < sensoryVision_.size() && sensoryVision_[neuronIdx]) {
                sensoryVision_[neuronIdx]->injectCurrent(8.0f);
            }
        }
    }
}

void AgentBrain::processTouchInput(const std::vector<float>& touch) {
    maxTouch_ = 0.0f;
    for (float t : touch) maxTouch_ = std::max(maxTouch_, t);
    
    float visionMean = 0.0f;
    for (float v : lastVision_) visionMean += v;
    if (!lastVision_.empty()) visionMean /= static_cast<float>(lastVision_.size());
    
    // Forward touch: touch sensors 0, 1, 7 are the forward cone
    forwardTouch_ = 0.0f;
    if (touch.size() >= 8) {
        forwardTouch_ = std::max({touch[0], touch[1], touch[7]});
    } else {
        forwardTouch_ = maxTouch_;
    }
    
    // Closest object in the direction of the strongest forward touch
    forwardObjectVision_ = 1.0f;  // Default to empty
    if (!lastVision_.empty() && forwardTouch_ > 0.5f) {
        // Find closest forward-cone vision value below empty (1.0)
        for (size_t ray = 5; ray <= 10; ++ray) {  // Centre cone rays
            if (lastVision_[ray] < 0.5f) {
                forwardObjectVision_ = lastVision_[ray];
                break;
            }
        }
    }
}

void AgentBrain::updateNoveltyDetection() {
    if (!vision.empty() && !predictedVision_.empty()) {
        float novelty = 0.0f;
        for (size_t i = 0; i < vision.size() && i < predictedVision_.size(); ++i) {
            novelty += std::abs(vision[i] - predictedVision_[i]);
        }
        if (!vision.empty()) {
            novelty /= static_cast<float>(vision.size());
        }
        
        // Update prediction with current vision
        predictedVision_ = vision;
        
        // Apply novelty decay
        noveltyLevel_ = novelty * sensoryNoveltyDecay_;
    }
}

std::vector<size_t> AgentBrain::encodeText(const std::string& text) {
    return TextEncoder::encodeText(text);
}

std::vector<float> AgentBrain::getInternalFiringRates() const {
    std::vector<float> rates;
    rates.reserve(internalNeurons_.size());
    
    for (const Neuron* n : internalNeurons_) {
        rates.push_back(n ? n->getFiringRate() : 0.0f);
    }
    
    return rates;
}

std::vector<float> AgentBrain::conceptPattern() const {
    std::vector<float> pat;
    for (float v : lastVision_) pat.push_back(1.0f - v);
    for (float t : lastTouch_) pat.push_back(t);
    return pat;
}

size_t AgentBrain::getConceptCount() const {
    return brain_ && brain_->getConceptFormation() ? 
           brain_->getConceptFormation()->getConceptCount() : 0;
}

float AgentBrain::getSelfModelConfidence() const {
    if (!selfModelReady_) return 0.0f;
    float sum = 0.0f;
    int n = 0;
    for (int a = 0; a < 6; ++a) {
        float c = selfModel_.getSelfModelConfidence(static_cast<ActionType>(a));
        if (c > 0.0f) { sum += c; ++n; }
    }
    return n ? sum / n : 0.0f;
}

size_t AgentBrain::getSocialObservationCount() const {
    return socialReady_ ? socialLearning_.getObservationCount() : 0;
}

void AgentBrain::observeOtherAgent(MotorCommand observed, 
                                  const std::vector<float>& beforeVision,
                                  const std::vector<float>& afterVision) {
    if (!socialReady_ && brain_) {
        socialLearning_.initialize(brain_.get());
        socialReady_ = true;
    }
    
    ActionType act = ActionType::Wait;
    switch (observed) {
        case MotorCommand::MoveForward: act = ActionType::MoveForward; break;
        case MotorCommand::MoveBackward: act = ActionType::MoveBackward; break;
        case MotorCommand::TurnLeft: act = ActionType::TurnLeft; break;
        case MotorCommand::TurnRight: act = ActionType::TurnRight; break;
        case MotorCommand::Interact: act = ActionType::Interact; break;
        default: break;
    }
    
    socialLearning_.observeAgentAction(act, beforeVision, afterVision);
}

void AgentBrain::reset() {
    // Reset state variables
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Clear percept history
    lastVision_.clear();
    lastTouch_.clear();
    lastText_.clear();
    predictedVision_.clear();
    
    // Reset counters
    lastWinnerGroup_ = -1;
    lastWinnerMargin_ = 0.0f;
    consecutiveWinnerCount_ = 0;
    stepsSinceTurn_ = 0;
    repeatCount_ = 0;
    reflexBonusCount_ = 0;
    reflexSuppressCount_ = 0;
    wtaPulseCount_ = 0;
    lastDecoded_ = MotorCommand::Wait;
    hasLastDecoded_ = false;
    
    // Reset flags
    selfModelReady_ = false;
    socialReady_ = false;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!rewardModulationEnabled_) return;
    
    // Calculate reward prediction error
    predictionError_ = reward - predictedReward;
    expectedReward_ = reward;
    
    // Apply neuromodulation based on error
    if (predictionError_ > 0.0f) {
        // Positive prediction error -> dopamine increase
        dopamineLevel_ = std::max(dopamineLevel_, predictionError_ * 0.1f);
    } else {
        // Negative prediction error -> reduced excitability
        float reduction = std::min(-predictionError_, 0.5f);
        plasticityModifier_ = std::max(0.1f, 1.0f - reduction);
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    // Age-dependent plasticity modulation
    plasticityModifier_ = std::max(0.1f, 1.0f - developmentalAge_ * 0.001f);
    
    // Update development system if available
    if (brain_ && brain_->getDevelopmentSystem()) {
        brain_->getDevelopmentSystem()->update(developmentalAge_);
    }
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    if (!brain_) return DevelopmentalStage::Initial;
    return brain_->getDevelopmentalStage();
}

float AgentBrain::getNeuromodulationLevel() const {
    return (dopamineLevel_ + curiosityLevel_ + noveltyLevel_) / 3.0f;
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

std::string AgentBrain::getThoughtTrace() const {
    std::stringstream trace;
    trace << "Percept: " << (thoughtPercept_ == "none" ? "none" : thoughtPercept_);
    trace << ", Planned: " << static_cast<int>(thoughtPlanned_);
    trace << ", Outcome: " << thoughtReward_;
    return trace.str();
}

float AgentBrain::getLearnedPreference() const {
    return brain_ ? brain_->getLearnedPreference() : 0.0f;
}

int AgentBrain::getPlannerOverrideCount() const { return plannerOverrideCount_; }
int AgentBrain::getPlannerAgreementCount() const { return plannerAgreementCount_; }
int AgentBrain::getPlannerDecisionCount() const { return plannerDecisionCount_; }
int AgentBrain::getConceptBiasCount() const { return conceptBiasCount_; }
int AgentBrain::getSelfBiasCount() const { return selfBiasCount_; }
int AgentBrain::getSocialBiasCount() const { return socialBiasCount_; }

void AgentBrain::enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
void AgentBrain::enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
void AgentBrain::enableDevelopment(bool enable) { developmentEnabled_ = enable; }
void AgentBrain::enableCuriosity(bool enable) { curiosityEnabled_ = enable; }

bool AgentBrain::isRewardModulationEnabled() const { return rewardModulationEnabled_; }
bool AgentBrain::isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
bool AgentBrain::isDevelopmentEnabled() const { return developmentEnabled_; }
bool AgentBrain::isCuriosityEnabled() const { return curiosityEnabled_; }

[[nodiscard]] size_t AgentBrain::getTotalMotorNeurons() const {
    return motorGroupManager_ ? motorGroupManager_->getTotalNeurons() : 0;
}

[[nodiscard]] size_t AgentBrain::getTotalSensoryNeurons() const {
    return sensoryGroupManager_ ? sensoryGroupManager_->getTotalNeurons() : 0;
}

Brain* AgentBrain::getBrain() { return brain_.get(); }
