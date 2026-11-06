#include "AgentBrain.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

// Helper functions (private namespace)
namespace {
    // Motor command to string conversion helper
    const char* motorCommandToString(MotorCommand cmd) {
        switch (cmd) {
            case MotorCommand::MoveForward: return "MoveForward";
            case MotorCommand::MoveBackward: return "MoveBackward";
            case MotorCommand::TurnLeft: return "TurnLeft";
            case MotorCommand::TurnRight: return "TurnRight";
            case MotorCommand::LookLeft: return "LookLeft";
            case MotorCommand::LookRight: return "LookRight";
            case MotorCommand::Interact: return "Interact";
            default: return "Wait";
        }
    }
    
    // Calculate Euclidean distance between two vectors
    float vectorDistance(const std::vector<float>& a, const std::vector<float>& b) {
        if (a.size() != b.size()) return 0.0f;
        float sum = 0.0f;
        for (size_t i = 0; i < a.size(); ++i) {
            float diff = a[i] - b[i];
            sum += diff * diff;
        }
        return std::sqrt(sum);
    }
    
    // Clamp value between min and max
    float clamp(float value, float min, float max) {
        return std::max(min, std::min(max, value));
    }
}

// Initialize neuron groups from brain regions
void AgentBrain::initializeNeuronGroups() {
    if (!brain_) return;
    
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            
            if (type == NeuronType::Motor) {
                for (Neuron* n : pop->getNeurons()) {
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
            } else if (type == NeuronType::Internal) {
                for (Neuron* n : pop->getNeurons()) {
                    internalNeurons_.push_back(n);
                }
            } else if (type == NeuronType::Sensory) {
                for (Neuron* n : pop->getNeurons()) {
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

// Encode text input to sparse neuron pattern
std::vector<size_t> AgentBrain::encodeText(const std::string& text) {
    std::string norm;
    for (char c : text) {
        if (c == ' ' || c == '\t' || c == '\n') continue;
        norm += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    if (norm.empty()) return {};
    uint64_t h = 1469598103934665603ULL;
    for (char c : norm) {
        h ^= static_cast<uint64_t>(static_cast<unsigned char>(c));
        h *= 1099511628211ULL;
    }
    std::vector<size_t> out;
    uint64_t state = h;
    while (out.size() < 4) {
        state += 0x9e3779b97f4a7c15ULL;
        uint64_t z = state;
        z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
        z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
        z = z ^ (z >> 31);
        size_t idx = static_cast<size_t>(z % 15);
        if (std::find(out.begin(), out.end(), idx) == out.end()) {
            out.push_back(idx);
        }
    }
    return out;
}

// Get internal firing rates
std::vector<float> AgentBrain::getInternalFiringRates() const {
    std::vector<float> rates;
    rates.reserve(internalNeurons_.size());
    for (const Neuron* n : internalNeurons_) {
        rates.push_back(n ? n->getFiringRate() : 0.0f);
    }
    return rates;
}

// Generate concept pattern from vision and touch data
std::vector<float> AgentBrain::conceptPattern() const {
    std::vector<float> pat;
    for (float v : lastVision_) pat.push_back(1.0f - v);
    for (float t : lastTouch_) pat.push_back(t);
    return pat;
}

// Get concept count from brain
size_t AgentBrain::getConceptCount() const {
    return brain_ && brain_->getConceptFormation() ? brain_->getConceptFormation()->getConceptCount() : 0;
}

// Get self model confidence
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

// Get social observation count
size_t AgentBrain::getSocialObservationCount() const {
    return socialReady_ ? socialLearning_.getObservationCount() : 0;
}

// Update self model with new percept
void AgentBrain::updateSelfModel(const SensoryPercept& percept) {
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
    lastText_ = percept.getText();
}

// Calculate forward touch from touch sensors
void AgentBrain::calculateForwardTouch(const std::vector<float>& touch) {
    maxTouch_ = 0.0f;
    for (float t : touch) maxTouch_ = std::max(maxTouch_, t);
    
    forwardTouch_ = 0.0f;
    if (touch.size() >= 8) {
        forwardTouch_ = std::max({touch[0], touch[1], touch[7]});
    } else {
        forwardTouch_ = maxTouch_;
    }
}

// Calculate forward object vision from touch and vision
void AgentBrain::calculateForwardObjectVision(const std::vector<float>& touch, 
                                              const std::vector<float>& vision) {
    forwardObjectVision_ = 1.0f;
    if (touch.size() >= 8 && !vision.empty() && forwardTouch_ > 0.05f) {
        size_t touchDir = 0;
        float touchBest = touch[0];
        if (touch[1] > touchBest) { touchBest = touch[1]; touchDir = 1; }
        if (touch[7] > touchBest) { touchBest = touch[7]; touchDir = 7; }
        std::vector<size_t> rays;
        if (touchDir == 0) rays = {7, 8};
        else if (touchDir == 1) rays = {15};
        else rays = {0};
        for (size_t r : rays) {
            if (r < vision.size() && vision[r] < 0.99f) {
                forwardObjectVision_ = std::min(forwardObjectVision_, vision[r]);
            }
        }
    } else if (!vision.empty()) {
        for (size_t r = 5; r <= 10 && r < vision.size(); ++r) {
            if (vision[r] < 0.99f) {
                forwardObjectVision_ = std::min(forwardObjectVision_, vision[r]);
            }
        }
    }
}

// Process winner-take-all lateral inhibition
void AgentBrain::processWinnerTakeAll() {
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

// Process vision input
void AgentBrain::processVisionInput(const std::vector<float>& vision) {
    const auto& visionVec = vision;
    for (size_t i = 0; i < sensoryVision_.size() && i < visionVec.size(); ++i) {
        if (sensoryVision_[i]) {
            float current = visionVec[i] * 5.0f;
            sensoryVision_[i]->injectCurrent(current);
        }
    }
}

// Process text input
void AgentBrain::processTextInput(const std::string& text) {
    if (!text.empty()) {
        for (size_t idx : encodeText(text)) {
            size_t neuronIdx = 16 + idx;
            if (neuronIdx < sensoryVision_.size() && sensoryVision_[neuronIdx]) {
                sensoryVision_[neuronIdx]->injectCurrent(8.0f);
            }
        }
    }
}

// Process touch input
void AgentBrain::processTouchInput(const std::vector<float>& touch, float& visionMean) {
    const auto& touchVec = touch;
    maxTouch_ = 0.0f;
    for (float t : touchVec) maxTouch_ = std::max(maxTouch_, t);
    
    visionMean = 0.0f;
    for (float v : lastVision_) visionMean += v;
    if (!lastVision_.empty()) visionMean /= static_cast<float>(lastVision_.size());
    
    // Calculate forward touch and vision
    calculateForwardTouch(touchVec);
    calculateForwardObjectVision(touchVec, lastVision_);
    
    for (size_t i = 0; i < sensoryTouch_.size() && i < touchVec.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touchVec[i] * 8.0f;
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    lastTouch_ = touchVec;
}

// Process internal input
void AgentBrain::processInternalInput(const std::vector<float>& internal) {
    const auto& intern = internal;
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
}

// Process proprioception input
void AgentBrain::processProprioceptionInput(const std::vector<float>& proprio) {
    const auto& proprioVec = proprio;
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprioVec.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprioVec[i] * 2.0f - 1.0f) * 3.0f;
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
}

// Update novelty detection
void AgentBrain::updateNovelty(const std::vector<float>& vision) {
    if (!vision.empty()) {
        if (predictedVision_.size() != vision.size()) {
            predictedVision_.assign(vision.size(), 0.0f);
        }
        float totalDiff = 0.0f;
        for (size_t i = 0; i < vision.size(); ++i) {
            totalDiff += std::abs(vision[i] - predictedVision_[i]);
        }
        noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
        noveltyLevel_ *= sensoryNoveltyDecay_;
        
        constexpr float kPredictionAlpha = 0.05f;
        for (size_t i = 0; i < vision.size(); ++i) {
            predictedVision_[i] += kPredictionAlpha * (vision[i] - predictedVision_[i]);
        }
    }
}

// Update curiosity based on novelty and prediction error
void AgentBrain::updateCuriosity() {
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f
                         + 0.6f * curiosityLevel_;
        curiosityLevel_ = clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}

// Get motor group activities
std::vector<AgentBrain::MotorGroupActivity> AgentBrain::getMotorGroupActivities() const {
    auto calc = [](const std::vector<Neuron*>& neurons, const char* name) {
        MotorGroupActivity out{name, 0.0f, 0, neurons.size()};
        if (neurons.empty()) return out;
        float sum = 0.0f;
        float rateSum = 0.0f;
        for (const Neuron* n : neurons) {
            sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
            if (n->getState().firingState == FiringState::Refractory) ++out.refractoryCount;
            rateSum += n->getFiringRate();
        }
        out.activity = sum / static_cast<float>(neurons.size());
        out.firingRate = rateSum / static_cast<float>(neurons.size());
        return out;
    };
    
    return {
        calc(motorForward_, "forward"),
        calc(motorBackward_, "backward"),
        calc(motorTurnLeft_, "left"),
        calc(motorTurnRight_, "right"),
        calc(motorInteract_, "interact"),
        calc(motorWait_, "wait"),
    };
}

// Check for silent brain condition
bool AgentBrain::isSilentBrain(const std::vector<MotorGroupActivity>& activities) const {
    float maxRaw = 0.0f;
    for (size_t i = 0; i < 5 && i < activities.size(); ++i) {
        maxRaw = std::max(maxRaw, activities[i].firingRate);
    }
    return maxRaw <= 0.1f;
}

// Apply vision-guided interact reflex
void AgentBrain::applyVisionGuidedReflex(MotorGroupActivity& interactRate, 
                                        const MotorGroupActivity& forwardRate,
                                        const MotorGroupActivity& backwardRate,
                                        const MotorGroupActivity& turnLeftRate,
                                        const MotorGroupActivity& turnRightRate,
                                        float novelty, bool& resourceCue, bool& hazardCue) {
    resourceCue = forwardTouch_ > 0.05f &&
                  forwardObjectVision_ > 0.5f &&
                  forwardObjectVision_ < 0.99f;
    hazardCue = forwardTouch_ > 0.05f &&
               forwardObjectVision_ < 0.35f;
    
    if (maxTouch_ <= 0.05f && forwardObjectVision_ > 0.3f &&
        forwardObjectVision_ < 0.95f) {
        forwardRate.firingRate += 10.0f;
    }
    
    if (maxTouch_ <= 0.05f) {
        interactRate.firingRate = std::max(0.0f, interactRate.firingRate - 12.0f);
    }
    
    if (resourceCue && interactRate.firingRate > 0.1f && novelty > 0.02f &&
        !(hasLastDecoded_ && lastDecoded_ == MotorCommand::Interact)) {
        interactRate.firingRate += 20.0f;
        ++reflexBonusCount_;
    } else if (hazardCue) {
        interactRate.firingRate = std::max(0.0f, interactRate.firingRate - 12.0f);
        turnLeftRate.firingRate += 12.0f;
        turnRightRate.firingRate += 12.0f;
        ++reflexSuppressCount_;
    }
}

// Apply planner bias to motor rates
void AgentBrain::applyPlannerBias(MotorGroupActivity& forwardRate,
                                 MotorGroupActivity& backwardRate,
                                 MotorGroupActivity& turnLeftRate,
                                 MotorGroupActivity& turnRightRate,
                                 MotorGroupActivity& interactRate,
                                 const ActionType plannedAction,
                                 const ActionType preBiasBest) {
    if (!brain_ || !brain_->getPlanner() || !hasThoughtPlanned_) return;
    
    MotorCommand plannerCmd = MotorCommand::Wait;
    switch (plannedAction) {
        case ActionType::MoveForward: plannerCmd = MotorCommand::MoveForward; break;
        case ActionType::MoveBackward: plannerCmd = MotorCommand::MoveBackward; break;
        case ActionType::TurnLeft: plannerCmd = MotorCommand::TurnLeft; break;
        case ActionType::TurnRight: plannerCmd = MotorCommand::TurnRight; break;
        case ActionType::Interact: plannerCmd = MotorCommand::Interact; break;
        default: break;
    }
    
    if (brain_ && brain_->getPlanner() && hasThoughtPlanned_ &&
        thoughtPlanned_ != MotorCommand::Wait && thoughtPlanned_ != preBiasBest) {
        
        auto toAction = [](MotorCommand c) {
            switch (c) {
                case MotorCommand::MoveForward: return ActionType::MoveForward;
                case MotorCommand::MoveBackward: return ActionType::MoveBackward;
                case MotorCommand::TurnLeft: return ActionType::TurnLeft;
                case MotorCommand::TurnRight: return ActionType::TurnRight;
                case MotorCommand::Interact: return ActionType::Interact;
                default: return ActionType::Wait;
            }
        };
        
        auto* planner = brain_->getPlanner();
        bool learned = false;
        for (ActionType a : {ActionType::MoveForward, ActionType::MoveBackward,
                             ActionType::TurnLeft, ActionType::TurnRight,
                             ActionType::Interact, ActionType::Wait}) {
            if (std::abs(planner->getActionQuality(a)) > 0.02f) { learned = true; break; }
        }
        
        float qPlanned = planner->getActionQuality(toAction(thoughtPlanned_));
        float qBest = planner->getActionQuality(toAction(preBiasBest));
        
        bool interactWithoutTouch = thoughtPlanned_ == MotorCommand::Interact && maxTouch_ <= 0.05f;
        
        if (learned && !interactWithoutTouch && qPlanned - qBest > 0.05f) {
            switch (thoughtPlanned_) {
                case MotorCommand::MoveForward: forwardRate.firingRate += 15.0f; break;
                case MotorCommand::MoveBackward: backwardRate.firingRate += 15.0f; break;
                case MotorCommand::TurnLeft: turnLeftRate.firingRate += 15.0f; break;
                case MotorCommand::TurnRight: turnRightRate.firingRate += 15.0f; break;
                case MotorCommand::Interact: interactRate.firingRate += 15.0f; break;
                default: break;
            }
            ++plannerOverrideCount_;
        }
    }
}

// Apply concept bias to motor rates
void AgentBrain::applyConceptBias(MotorGroupActivity& interactRate,
                                  MotorGroupActivity& turnLeftRate,
                                  MotorGroupActivity& turnRightRate,
                                  float maxTouch) {
    if (!brain_ || !brain_->getConceptFormation()) return;
    
    auto pat = conceptPattern();
    float norm = 0.0f;
    for (float v : pat) norm += v * v;
    if (norm > 0.01f) {
        size_t cid = brain_->getConceptFormation()->getMatchingConcept(pat);
        const auto* foundConcept = cid ? brain_->getConceptFormation()->getConcept(cid) : nullptr;
        if (foundConcept && !foundConcept->instances.empty()) {
            float avg = 0.0f;
            for (const auto& inst : foundConcept->instances) avg += inst.avgReward;
            avg /= static_cast<float>(foundConcept->instances.size());
            
            if (avg > 0.1f && maxTouch > 0.05f) {
                interactRate.firingRate += 8.0f;
                ++conceptBiasCount_;
            } else if (avg < -0.1f) {
                interactRate.firingRate = std::max(0.0f, interactRate.firingRate - 8.0f);
                turnLeftRate.firingRate += 6.0f;
                turnRightRate.firingRate += 6.0f;
                ++conceptBiasCount_;
            }
        }
    }
}

// Apply self model bias to motor rates
void AgentBrain::applySelfModelBias(MotorGroupActivity& forwardRate,
                                    MotorGroupActivity& backwardRate,
                                    MotorGroupActivity& turnLeftRate,
                                    MotorGroupActivity& turnRightRate,
                                    MotorGroupActivity& interactRate) {
    if (!selfModelReady_) return;
    
    MotorCommand cmds[5] = {MotorCommand::MoveForward, MotorCommand::MoveBackward,
                            MotorCommand::TurnLeft, MotorCommand::TurnRight,
                            MotorCommand::Interact};
    ActionType acts[5] = {ActionType::MoveForward, ActionType::MoveBackward,
                          ActionType::TurnLeft, ActionType::TurnRight,
                          ActionType::Interact};
    float conf[5];
    for (int i = 0; i < 5; ++i) conf[i] = selfModel_.getSelfModelConfidence(acts[i]);
    
    int bestI = 0;
    float second = 0.0f;
    for (int i = 0; i < 5; ++i) {
        if (conf[i] > conf[bestI]) { second = conf[bestI]; bestI = i; }
        else if (conf[i] > second) { second = conf[i]; }
    }
    
    if (conf[bestI] > 0.8f && conf[bestI] - second > 0.05f) {
        switch (cmds[bestI]) {
            case MotorCommand::MoveForward: forwardRate.firingRate += 3.0f; break;
            case MotorCommand::MoveBackward: backwardRate.firingRate += 3.0f; break;
            case MotorCommand::TurnLeft: turnLeftRate.firingRate += 3.0f; break;
            case MotorCommand::TurnRight: turnRightRate.firingRate += 3.0f; break;
            case MotorCommand::Interact: interactRate.firingRate += 3.0f; break;
            default: break;
        }
        ++selfBiasCount_;
    }
}

// Apply social bias to motor rates
void AgentBrain::applySocialBias(MotorGroupActivity& forwardRate,
                                 MotorGroupActivity& backwardRate,
                                 MotorGroupActivity& turnLeftRate,
                                 MotorGroupActivity& turnRightRate,
                                 MotorGroupActivity& interactRate,
                                 const std::vector<float>& lastVision) {
    if (!socialReady_ || socialLearning_.getObservationCount() <= 0 || lastVision.empty()) return;
    
    auto [imitAct, sim] = socialLearning_.getImitationActionAndSimilarity(lastVision_);
    if (sim > 0.8f) {
        switch (imitAct) {
            case ActionType::MoveForward: forwardRate.firingRate += 5.0f; ++socialBiasCount_; break;
            case ActionType::MoveBackward: backwardRate.firingRate += 5.0f; ++socialBiasCount_; break;
            case ActionType::TurnLeft: turnLeftRate.firingRate += 5.0f; ++socialBiasCount_; break;
            case ActionType::TurnRight: turnRightRate.firingRate += 5.0f; ++socialBiasCount_; break;
            case ActionType::Interact: interactRate.firingRate += 5.0f; ++socialBiasCount_; break;
            default: break;
        }
    }
}

// Apply turn diversity and repetition fatigue
void AgentBrain::applyDiversityAndFatigue(MotorGroupActivity& forwardRate,
                                         MotorGroupActivity& backwardRate,
                                         int stepsSinceTurn, int repeatCount,
                                         MotorCommand lastDecoded) {
    if (stepsSinceTurn > 25 &&
        (forwardRate.firingRate > backwardRate.firingRate || backwardRate.firingRate > forwardRate.firingRate)) {
        turnLeftRate_.firingRate += 6.0f;
        turnRightRate_.firingRate += 6.0f;
    }
    
    if (repeatCount >= 3) {
        if (lastDecoded == MotorCommand::MoveForward) {
            forwardRate.firingRate = std::max(0.0f, forwardRate.firingRate - 8.0f);
        } else if (lastDecoded == MotorCommand::MoveBackward) {
            backwardRate.firingRate = std::max(0.0f, backwardRate.firingRate - 8.0f);
        }
    }
}

// Apply momentum bias
void AgentBrain::applyMomentumBias(MotorGroupActivity& forwardRate,
                                   MotorGroupActivity& backwardRate,
                                   MotorCommand lastDecoded) {
    if (hasLastDecoded_ && lastDecoded == MotorCommand::MoveForward) {
        backwardRate.firingRate = std::max(0.0f, backwardRate.firingRate - 8.0f);
    } else if (hasLastDecoded_ && lastDecoded == MotorCommand::MoveBackward) {
        forwardRate.firingRate = std::max(0.0f, forwardRate.firingRate - 8.0f);
    }
}

// Select motor command with curiosity-based exploration
MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    float exploreChance = curiosityLevel_ * 0.3f;
    
    float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
    if (r < exploreChance) {
        int choice = brain_->getRandomGenerator()->uniformInt(uint32_t(0), uint32_t(7));
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
    
    return defaultCmd;
}

// Update development state
void AgentBrain::updateDevelopmentState(double timestep) {
    developmentalAge_ += timestep;
    
    if (developmentalAge_ < 60.0) {
        plasticityModifier_ = 1.0f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < 300.0) {
        plasticityModifier_ = 0.8f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < 900.0) {
        plasticityModifier_ = 0.5f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = 0.2f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            float synRate = 0.0001f * plasticityModifier_;
            float pruneRate = 0.00001f * (2.0f - plasticityModifier_);
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
        }
    }
}

// Reset agent state
void AgentBrain::reset() {
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    predictedVision_.clear();
    maxTouch_ = 0.0f;
    forwardTouch_ = 0.0f;
    forwardObjectVision_ = 1.0f;
    lastVision_.clear();
    lastWinnerGroup_ = -1;
    lastWinnerMargin_ = 0.0f;
    consecutiveWinnerCount_ = 0;
    reflexBonusCount_ = 0;
    reflexSuppressCount_ = 0;
    wtaPulseCount_ = 0;
    plannerOverrideCount_ = 0;
    plannerAgreementCount_ = 0;
    plannerDecisionCount_ = 0;
    conceptBiasCount_ = 0;
    selfBiasCount_ = 0;
    socialBiasCount_ = 0;
    
    stepsSinceTurn_ = 0;
    repeatCount_ = 0;
    hasLastDecoded_ = false;
    lastDecoded_ = MotorCommand::Wait;
}

// Get thought trace as string
std::string AgentBrain::getThoughtTrace() const {
    std::stringstream ss;
    ss << "perceived=[" << thoughtPercept_ << "] -> planned=[";
    ss << (hasThoughtPlanned_ ? motorCommandToString(thoughtPlanned_) : "none");
    ss << "] -> acted=[" << std::string(motorCommandToString(lastDecoded_)) << "] -> outcome=[";
    ss << (hasThoughtOutcome_ ? ("reward=" + std::to_string(thoughtReward_)) : "none");
    ss << "] novelty=" << noveltyLevel_ 
       << " curiosity=" << curiosityLevel_;
    return ss.str();
}

// Apply neuromodulation based on reward
void AgentBrain::applyNeuromodulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    thoughtReward_ = reward;
    hasThoughtOutcome_ = true;
    
    predictionError_ = reward - predictedReward;
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    dopamineLevel_ = predictionError_;
    dopamineLevel_ = clamp(dopamineLevel_, -1.0f, 1.0f);
    
    for (const auto& region : brain_->getRegions()) {
        for (const auto& syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            if (std::abs(eligibility) > 0.001f) {
                float delta = eligibility * dopamineLevel_ * plasticityModifier_ * 0.05f;
                delta = clamp(delta, -0.1f, 0.1f);
                syn->addToWeight(delta);
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
    
    float plasticityFactor = 0.5f + 0.5f * dopamineLevel_;
    plasticityFactor = clamp(plasticityFactor, 0.1f, 2.0f);
    
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(0.01f * plasticityFactor);
        stdp->setLTDWeight(0.012f * plasticityFactor);
    }
}

// Process sensory input from percept
void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    updateSelfModel(percept);
    
    if (hasThoughtPlanned_) {
        ActionType planned = ActionType::Wait;
        switch (thoughtPlanned_) {
            case MotorCommand::MoveForward: planned = ActionType::MoveForward; break;
            case MotorCommand::MoveBackward: planned = ActionType::MoveBackward; break;
            case MotorCommand::TurnLeft: planned = ActionType::TurnLeft; break;
            case MotorCommand::TurnRight: planned = ActionType::TurnRight; break;
            case MotorCommand::Interact: planned = ActionType::Interact; break;
            default: break;
        }
        if (brain_->getPlanner() && !lastVision_.empty()) {
            ActionType plannedAction = brain_->getPlanner()->planAction(lastVision_);
            float actionQuality = brain_->getPlanner()->getActionQuality(plannedAction);
            brain_->getPlanner()->setActionQuality(plannedAction, 0.9f * actionQuality + 0.1f * (thoughtReward_ / 10.0f));
        }
    }
    
    processWinnerTakeAll();
    
    // Process vision input
    processVisionInput(percept.getVision());
    
    // Process text input
    processTextInput(percept.getText());
    
    // Process touch input
    float visionMean = 0.0f;
    processTouchInput(percept.getTouch(), visionMean);
    
    thoughtPercept_ = "visionMean=" + std::to_string(visionMean) +
                      " fwdObjVision=" + std::to_string(forwardObjectVision_) +
                      " fwdTouch=" + std::to_string(forwardTouch_) +
                      " maxTouch=" + std::to_string(maxTouch_);
    hasThoughtOutcome_ = false;
    
    // Process internal input
    processInternalInput(percept.getInternal());
    
    // Process proprioception input
    processProprioceptionInput(percept.getProprioception());
    
    // Update novelty detection
    updateNovelty(percept.getVision());
    
    // Update curiosity
    updateCuriosity();
}

// Get motor command from brain activity
MotorCommand AgentBrain::decodeMotorCommand() {
    auto activities = getMotorGroupActivities();
    
    if (isSilentBrain(activities)) {
        return MotorCommand::Wait;
    }
    
    // Apply reflexes and biases
    applyVisionGuidedReflex(activities[4], activities[0], activities[1], 
                            activities[2], activities[3], noveltyLevel_);
    
    // Apply planner bias
    ActionType plannedAction = ActionType::Wait;
    if (brain_->getPlanner() && !lastVision_.empty()) {
        plannedAction = brain_->getPlanner()->planAction(lastVision_);
    }
    
    // Determine pre-bias best command
    MotorCommand preBiasBest = MotorCommand::Wait;
    {
        float rates[5] = {activities[0].firingRate, activities[1].firingRate,
                         activities[2].firingRate, activities[3].firingRate,
                         activities[4].firingRate};
        MotorCommand cmds[5] = {MotorCommand::MoveForward, MotorCommand::MoveBackward,
                                MotorCommand::TurnLeft, MotorCommand::TurnRight,
                                MotorCommand::Interact};
        float bestR = -1.0f;
        for (int i = 0; i < 5; ++i) {
            if (rates[i] > bestR) { bestR = rates[i]; preBiasBest = cmds[i]; }
        }
    }
    
    applyPlannerBias(activities[0], activities[1], activities[2], activities[3], 
                    activities[4], plannedAction, preBiasBest);
    
    // Apply concept bias
    applyConceptBias(activities[4], activities[2], activities[3], maxTouch_);
    
    // Apply self model bias
    applySelfModelBias(activities[0], activities[1], activities[2], activities[3], activities[4]);
    
    // Apply social bias
    applySocialBias(activities[0], activities[1], activities[2], activities[3], activities[4], lastVision_);
    
    // Apply turn diversity and repetition fatigue
    applyDiversityAndFatigue(activities[0], activities[1], stepsSinceTurn_,
                             repeatCount_, lastDecoded_);
    
    // Apply momentum bias
    applyMomentumBias(activities[0], activities[1], lastDecoded_);
    
    // Select final command
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = 0.0f;
    for (int i = 0; i < 5 && i < static_cast<int>(activities.size()); ++i) {
        if (activities[i].firingRate > bestActivity) {
            bestActivity = activities[i].firingRate;
            best = (i == 0) ? MotorCommand::MoveForward :
                    (i == 1) ? MotorCommand::MoveBackward :
                    (i == 2) ? MotorCommand::TurnLeft :
                    (i == 3) ? MotorCommand::TurnRight : MotorCommand::Interact;
        }
    }
    
    if (bestActivity <= 0.1f) {
        return MotorCommand::Wait;
    }
    
    lastDecoded_ = best;
    hasLastDecoded_ = true;
    
    // Update winner-take-all state
    {
        float bestFiringRate = 0.0f, secondFiringRate = 0.0f;
        int bestIdx = -1;
        for (int i = 0; i < 5 && i < static_cast<int>(activities.size()); ++i) {
            float r = activities[i].firingRate;
            if (r > bestFiringRate) { secondFiringRate = bestFiringRate; bestFiringRate = r; bestIdx = i; }
            else if (r > secondFiringRate) { secondFiringRate = r; }
        }
        if (bestIdx == lastWinnerGroup_) {
            ++consecutiveWinnerCount_;
        } else {
            consecutiveWinnerCount_ = 1;
        }
        lastWinnerGroup_ = bestIdx;
        lastWinnerMargin_ = bestFiringRate - secondFiringRate;
    }
    
    return best;
}

// Select motor command with curiosity-based exploration
MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    float exploreChance = curiosityLevel_ * 0.3f;
    
    float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
    if (r < exploreChance) {
        int choice = brain_->getRandomGenerator()->uniformInt(uint32_t(0), uint32_t(7));
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
    
    return defaultCmd;
}

// Apply reward modulation
void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    thoughtReward_ = reward;
    hasThoughtOutcome_ = true;
    
    // Concept formation
    if (brain_->getConceptFormation() && !lastVision_.empty()) {
        auto pat = conceptPattern();
        float norm = 0.0f;
        for (float v : pat) norm += v * v;
        if (norm > 0.01f) {
            brain_->getConceptFormation()->presentExperience(pat, lastTouch_, reward, 0);
        }
    }
    
    // Update planner action quality
    if (hasLastDecoded_) {
        ActionType taken = ActionType::Wait;
        switch (lastDecoded_) {
            case MotorCommand::MoveForward: taken = ActionType::MoveForward; break;
            case MotorCommand::MoveBackward: taken = ActionType::MoveBackward; break;
            case MotorCommand::TurnLeft: taken = ActionType::TurnLeft; break;
            case MotorCommand::TurnRight: taken = ActionType::TurnRight; break;
            case MotorCommand::Interact: taken = ActionType::Interact; break;
            default: break;
        }
        if (brain_->getPlanner()) {
            float q = brain_->getPlanner()->getActionQuality(taken);
            float r = clamp(reward / 10.0f, -1.0f, 1.0f);
            brain_->getPlanner()->setActionQuality(taken, 0.9f * q + 0.1f * r);
        }
        if (lastDecoded_ == MotorCommand::Interact && std::abs(reward) > 0.01f) {
            float r = clamp(reward / 10.0f, -1.0f, 1.0f);
            setLearnedPreference(getLearnedPreference() + 0.1f * r);
        }
    }
    
    // Neuromodulation
    predictionError_ = reward - predictedReward;
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    dopamineLevel_ = predictionError_;
    dopamineLevel_ = clamp(dopamineLevel_, -1.0f, 1.0f);
    
    for (const auto& region : brain_->getRegions()) {
        for (const auto& syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            if (std::abs(eligibility) > 0.001f) {
                float delta = eligibility * dopamineLevel_ * plasticityModifier_ * 0.05f;
                delta = clamp(delta, -0.1f, 0.1f);
                syn->addToWeight(delta);
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
    
    float plasticityFactor = 0.5f + 0.5f * dopamineLevel_;
    plasticityFactor = clamp(plasticityFactor, 0.1f, 2.0f);
    
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(0.01f * plasticityFactor);
        stdp->setLTDWeight(0.012f * plasticityFactor);
    }
}

// Update development system
void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    updateDevelopmentState(timestep);
}

// Constructor
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
    , selfModelReady_(false)
    , socialReady_(false)
    , maxTouch_(0.0f)
    , forwardTouch_(0.0f)
    , forwardObjectVision_(1.0f)
    , lastWinnerGroup_(-1)
    , lastWinnerMargin_(0.0f)
    , consecutiveWinnerCount_(0)
    , stepsSinceTurn_(0)
    , repeatCount_(0)
    , hasLastDecoded_(false)
    , lastDecoded_(MotorCommand::Wait)
    , thoughtPercept_("none")
    , thoughtPlanned_(MotorCommand::Wait)
    , hasThoughtPlanned_(false)
    , thoughtReward_(0.0f)
    , hasThoughtOutcome_(false)
{
    initializeNeuronGroups();
}

// Destructor
AgentBrain::~AgentBrain() = default;

// Initialize with world
void AgentBrain::initialize(const SimpleWorld& world) {
    predictedVision_.clear();
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
}

// Decode motor command from brain activity
MotorCommand AgentBrain::decodeFromMotorNeurons() {
    auto activities = getMotorGroupActivities();
    
    if (isSilentBrain(activities)) {
        return MotorCommand::Wait;
    }
    
    // Apply reflexes and biases
    applyVisionGuidedReflex(activities[4], activities[0], activities[1], 
                            activities[2], activities[3], noveltyLevel_);
    
    // Apply planner bias
    ActionType plannedAction = ActionType::Wait;
    if (brain_->getPlanner() && !lastVision_.empty()) {
        plannedAction = brain_->getPlanner()->planAction(lastVision_);
    }
    
    // Determine pre-bias best command
    MotorCommand preBiasBest = MotorCommand::Wait;
    {
        float rates[5] = {activities[0].firingRate, activities[1].firingRate,
                         activities[2].firingRate, activities[3].firingRate,
                         activities[4].firingRate};
        MotorCommand cmds[5] = {MotorCommand::MoveForward, MotorCommand::MoveBackward,
                                MotorCommand::TurnLeft, MotorCommand::TurnRight,
                                MotorCommand::Interact};
        float bestR = -1.0f;
        for (int i = 0; i < 5; ++i) {
            if (rates[i] > bestR) { bestR = rates[i]; preBiasBest = cmds[i]; }
        }
    }
    
    applyPlannerBias(activities[0], activities[1], activities[2], activities[3], 
                    activities[4], plannedAction, preBiasBest);
    
    // Apply concept bias
    applyConceptBias(activities[4], activities[2], activities[3], maxTouch_);
    
    // Apply self model bias
    applySelfModelBias(activities[0], activities[1], activities[2], activities[3], activities[4]);
    
    // Apply social bias
    applySocialBias(activities[0], activities[1], activities[2], activities[3], activities[4], lastVision_);
    
    // Apply turn diversity and repetition fatigue
    applyDiversityAndFatigue(activities[0], activities[1], stepsSinceTurn_,
                             repeatCount_, lastDecoded_);
    
    // Apply momentum bias
    applyMomentumBias(activities[0], activities[1], lastDecoded_);
    
    // Select final command
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = 0.0f;
    for (int i = 0; i < 5 && i < static_cast<int>(activities.size()); ++i) {
        if (activities[i].firingRate > bestActivity) {
            bestActivity = activities[i].firingRate;
            best = (i == 0) ? MotorCommand::MoveForward :
                    (i == 1) ? MotorCommand::MoveBackward :
                    (i == 2) ? MotorCommand::TurnLeft :
                    (i == 3) ? MotorCommand::TurnRight : MotorCommand::Interact;
        }
    }
    
    if (bestActivity <= 0.1f) {
        return MotorCommand::Wait;
    }
    
    return best;
}

} // namespace nlm
